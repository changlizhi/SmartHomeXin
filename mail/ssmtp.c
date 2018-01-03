///////////////////////////////////////////////////////////////////////
// FILE DESCRIPTION
//    This file plays as simple SMTP (ssmtp) client communication
//
// NOTE
//    For each response we wait 5 seconds since,
//    you know, wireless LAN is unstable, sometimes.
//
//    For attached file sending, we encoded it as base 64 MIME
//    format by using free sourcing encode package.
//
//    Each time sending mail, the number of attachment is only one
//    Adjust it if you need to send out more than one attachments
//    at function encode
//
// VERSION
//    0.01 (2005/11/7)
//
// AUTHOR
//    Jerry Huang
///////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include "ssmtp.h"


///////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////
extern char *md5digest(FILE *infile, long int *len);
extern int to64(FILE *infile, FILE *outfile, long int limit);

int ssmtp_usage(void);
int ssmtp_init(SMTP_Client_Info *smtpinfo, int argc, char *argv[]);
int ssmtp_connect(SMTP_Client_Info *smtpinfo);
int ssmtp_read(SMTP_Client_Info *smtpinfo, char *buf);
int ssmtp_send(SMTP_Client_Info *smtpinfo, char *buf);
int ssmtp_ready(SMTP_Client_Info *smtpinfo, char *buf);
int ssmtp_helo(SMTP_Client_Info *smtpinfo, char *status_code);
int ssmtp_ehlo(SMTP_Client_Info *smtpinfo, char *status_code);
int ssmtp_mail(SMTP_Client_Info *smtpinfo, char *status_code);
int ssmtp_rcpt(SMTP_Client_Info *smtpinfo, SMTP_Mail_Info * mail , char *status_code);
int ssmtp_data(SMTP_Client_Info *smtpinfo, char *status_code);
int ssmtp_content(SMTP_Client_Info *smtpinfo, SMTP_Mail_Info * mail ,char *status_code);
int ssmtp_auth(SMTP_Client_Info *smtpinfo);
int ssmtp_quit(SMTP_Client_Info *smtpinfo);

void ssmtp_failed(int error_code);


///////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////
static char mail_date[100];
//static char *mail_subject="Alarm from IP-camera";
static char *tmp_host="";
static char *tmp_fromaddr="IP-camera";
static char *tmp_toaddr="";
static char *tmp_domain="";
static char *tmp_auth_user="";
static char *tmp_auth_pass="";
static char *tmp_port="";
static char *tmp_auth="";

//static char attachment[80];
//static char *mail_body="Motion detected as attachment!";
//static char mimetmpfile[80];

// Description of the various file formats and their magic numbers
struct magic
{
    char *name;    // Name of the file format
    char *num;    // The magic number
    int len;    // Length of same (0 means strlen(magicnum))
};

// The magic numbers of the file formats we know about
static struct magic magic[] =
{
    { "image/gif", "GIF", 0 },
    { "image/jpeg", "\377\330\377", 0 },
    { "video/mpeg", "\0\0\001\263", 4 },
    { "application/postscript", "%!", 0 },
};

static int num_magic = (sizeof(magic)/sizeof(magic[0]));
static int max_magiclen = 0; // The longest magic number
static char *default_type = "application/octet-stream";

static const char base64digits[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char ssmtp_stored_message[MAXDATASIZE];


///////////////////////////////////////////////////////////////////////
// Function sendall
//
//    be sure that system call send() sent out all data
//
///////////////////////////////////////////////////////////////////////
int sendall(int s, char *buf, int len)
{
    int total = 0;            // how many bytes we sent
    int bytesleft = len;    // how many we have left to send
    int n;

    while(total < len)
    {
        n = send(s, buf+total, bytesleft, 0);
        if (n < 0)
        {
            perror("send");
            return -1;
        }

        total += n;
        bytesleft -= n;
    }
    len = total;    // number of bytes actually sent
    return 0;        // return -1 on failure, 0 on success
}


///////////////////////////////////////////////////////////////////////
// Function magic_look
//
//    Determins the format of the file "inputf".  The name
//    of the file format (or NULL on error) is returned.
//
///////////////////////////////////////////////////////////////////////
char *magic_look(FILE *infile)
{
    int i, j;
    char buf[80];
    int numread = 0;

    if (max_magiclen == 0)
    {
        for (i=0; i<num_magic; i++)
        {
            if (magic[i].len == 0) magic[i].len = strlen(magic[i].num);
            if (magic[i].len > max_magiclen) max_magiclen = magic[i].len;
        }
    }

    numread = fread(buf, 1, max_magiclen, infile);
    rewind(infile);

    for (i=0; i<num_magic; i++)
    {
        if (numread >= magic[i].len)
        {
            for (j=0; j<magic[i].len; j++)
            {
                if (buf[j] != magic[i].num[j]) break;
            }

            if (j == magic[i].len) return magic[i].name;
        }
    }

    return default_type;
}


///////////////////////////////////////////////////////////////////////
// Function os_genid
//
//    Generate a message-id
//
///////////////////////////////////////////////////////////////////////
char *os_genid(void)
{
    static int pid = 0;
    static time_t curtime;
    static char hostname[MAXHOSTNAMELEN+1];
    char *result;
    struct hostent *hp;
    

    if (pid == 0)
    {
        pid = getpid();
        time(&curtime);
        gethostname(hostname, sizeof(hostname));

        // If we don't have a FQDN, try canonicalizing with gethostbyname
        if (!strchr(hostname, '.'))
        {
            hp = gethostbyname(hostname);
            if (hp)
            {
                strcpy(hostname, hp->h_name);
            }
        }
    }

    result = malloc(25+strlen(hostname));
    sprintf(result, "%d.%d@%s", pid, curtime++, hostname);
    return result;
}


///////////////////////////////////////////////////////////////////////
// Function os_createnewfile
//
//    create file for encoded MIME output
//
///////////////////////////////////////////////////////////////////////
FILE *os_createnewfile(char *fname)
{
    int fd;
    FILE *ret;
     
#ifdef O_EXCL
    fd=open(fname, O_RDWR|O_CREAT|O_EXCL, 0644);
#else
    fd=open(fname, O_RDWR|O_CREAT|O_TRUNC, 0644);
#endif

    if (fd == -1)
        return NULL;
     
    ret=fdopen(fd, "w");
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Function os_perror
//
//    print out create file error
//
///////////////////////////////////////////////////////////////////////
void os_perror(char *file)
{
    perror(file);
}


///////////////////////////////////////////////////////////////////////
// Function encode
//
//    Encode a file into one or more MIME messages, each
//    no larger than 'maxsize'. A 'maxsize' of zero means no size limit.
//    If 'applefile' is non-null, it is the first part of a
//    multipart/appledouble pair.
//
// PARAMETERS:
//    infile            = file description for the attached file
//    applefile        = not used
//    fname            = attached file name
//    desc            = MAILMSG, mail message
//    subject            = MAILSUBJECT, mail subject
//    headers            = 0, not used
//    maxsize            = 0, not used
//    typeoverride    = 0, not used
//    outfname        = file to be create for whole MIME encoded message
///////////////////////////////////////////////////////////////////////
int encode(FILE *infile, FILE *applefile, char *fname, char *desc, char *subject,
           char *headers, long int maxsize, char *typeoverride, char *outfname)
{
    char *type;
    FILE *outfile;
    char *cleanfname, *p;
    char *digest, *appledigest;
    long filesize, l, written;
    int thispart, numparts = 1;
    int wrotefiletype = 0;
    char *multipartid, *msgid, *referenceid[NUMREFERENCES];
    char buf[1024];
    int i;

    cleanfname = fname;
  if(fname){
    // Clean up fname for printing
#ifdef __riscos
    // This filename-cleaning knowledge will probably
    // be moved to the os layer in a future version.
    //
    if (p = strrchr(cleanfname, '.')) cleanfname = p+1;
#else
    if (p = strrchr(cleanfname, '/')) cleanfname = p+1;
    if (p = strrchr(cleanfname, '\\')) cleanfname = p+1;
#endif
    if (p = strrchr(cleanfname, ':')) cleanfname = p+1;
  }

    // Find file type
    if(infile)
    {
    type = magic_look(infile);

    if (typeoverride)
    {
        type = typeoverride;
    }
    else
    {
        type = magic_look(infile);
    }


    // Compute MD5 digests
    digest = md5digest(infile, &filesize);
    }

    if (applefile)
    {
        appledigest = md5digest(applefile, &l);
        filesize += l;
    }

#if 0
    // See if we have to do multipart
    if (maxsize)
    {
        filesize = (filesize / 54) * 73; // Allow for base64 expansion

        // Add in size of desc file
        if (descfile)
        {
            free(md5digest(descfile, &l));     // XXX
            filesize += l;
        }

        numparts = (filesize-1000)/maxsize + 1;
        if(numparts < 1) numparts = 1;
    }
#endif

    multipartid = os_genid(); // generate output ID (message ID)

    for (i=0; i<NUMREFERENCES; i++)
    {
        referenceid[i] = 0;
    }


    // main encode loop
    for (thispart=1; thispart <= numparts; thispart++)
    {
    written = 0;

    /* Open output file */
    if (numparts == 1)
    {
        outfile = os_createnewfile(outfname);
    }
    else
    {
#ifdef __riscos
        /* Arrgh, riscos uses '.' as directory separator */
        sprintf(buf, "%s/%02d", outfname, thispart);
#else
        sprintf(buf, "%s.%02d", outfname, thispart);
#endif
        outfile = os_createnewfile(buf);
    }
    if (!outfile) // file open failed
    {
        os_perror(buf);
            return 1;
    }

    msgid = os_genid();
    fprintf(outfile, "Message-ID: <%s>\r\n", msgid);
    fprintf(outfile, "Mime-Version: 1.0\r\n");
    if (headers) fputs(headers, outfile);

    if (numparts > 1)
    {
        fprintf(outfile, "Subject: %s (%02d/%02d)\r\n", subject, thispart, numparts);
        if (thispart == 1)
           {
            referenceid[0] = msgid;
        }
        else
        {
            // Put out References: header pointing to previous parts
            fprintf(outfile, "References: <%s>\r\n", referenceid[0]);
            for(i=1; i<NUMREFERENCES; i++)
            {
                if (referenceid[i])
                    fprintf(outfile, "\t <%s>\r\n", referenceid[i]);
            }

            for(i=2; i<NUMREFERENCES; i++)
            {
                referenceid[i-1] = referenceid[i];
            }

            referenceid[NUMREFERENCES-1] = msgid;
        }

        fprintf(outfile, "Content-Type: message/partial; number=%d; total=%d;\r\n", thispart, numparts);
        fprintf(outfile, "\t id=\"%s\"\r\n", multipartid);
        fprintf(outfile, "\r\n");
    }

    if (thispart == 1)
    {
        // if multi-part, new Message-ID has to be appended
        if (numparts > 1)
        {
            fprintf(outfile, "Message-ID: <%s>\r\n", multipartid);
            fprintf(outfile, "MIME-Version: 1.0\r\n");
        }

        fprintf(outfile, "Subject: %s\r\n", subject);
        fprintf(outfile, "Content-Type: multipart/mixed; boundary=\"-\"\r\n");
        fprintf(outfile, "\r\nThis is a MIME encoded message.  Decode it with \"munpack\"\r\n");
        fprintf(outfile, "or any other MIME reading software.  Mpack/munpack is available\r\n");
        fprintf(outfile, "via anonymous FTP in ftp.andrew.cmu.edu:pub/mpack/\r\n");
        written = 300;

        // SMTP body message here!!
        // Original from a file, now modified from a string
        if (desc)
        {
            fprintf(outfile, "---\r\n\r\n");

            #if 0
            while(gets(buf, sizeof(buf), desc))
            {
                // Strip multiple leading dashes as they may become MIME boundaries
                p = buf;

                if (*p == '-')
                {
                    while (p[1] == '-') p++;
                }

                fputs(p, outfile);
                written += strlen(p);
            }
            #endif

            p = desc;
            if(*p=='-')
            {
                while(p[1] == '-')
                    p++;
            }

            fputs(p, outfile);

            fprintf(outfile, "\r\n\r\n");
        }
    
        fprintf(outfile, "---\r\n"); // SMTP body message boundary

        // jry, we are not applefile
        if (applefile)
        {
            fprintf(outfile, "Content-Type: multipart/appledouble; boundary=\"=\"; name=\"%s\"\r\n", cleanfname);
            fprintf(outfile, "Content-Disposition: inline; filename=\"%s\"\r\n", cleanfname);
            fprintf(outfile, "\r\n\r\n--=\r\n");
            fprintf(outfile, "Content-Type: application/applefile\r\n");
            fprintf(outfile, "Content-Transfer-Encoding: base64\r\n");
            fprintf(outfile, "Content-MD5: %s\r\n\r\n", appledigest);
            free(appledigest);
            written += 100;
        }
    }

    // jry, we are not applefile
    if(applefile && !feof(applefile))
    {
        if (written == maxsize)
            written--; // avoid a nasty fencepost error

        written += to64(applefile, outfile, (thispart == numparts) ? 0 : (maxsize-written));

        if (!feof(applefile))
        {
            fclose(outfile);
            continue;
        }

        fprintf(outfile, "\r\n--=\r\n");
    }

    if(!wrotefiletype++ && cleanfname)
    {
        fprintf(outfile, "Content-Type: %s; name=\"%s\"\r\n", type, cleanfname);
        fprintf(outfile, "Content-Transfer-Encoding: base64\r\n");
        fprintf(outfile, "Content-Disposition: inline; filename=\"%s\"\r\n", cleanfname);
        fprintf(outfile, "Content-MD5: %s\r\n\r\n", digest);
        free(digest);
        written += 80;
    }

    if (written == maxsize) written--; // avoid a nasty fencepost error

    // to base 64
    if(infile)
      written += to64(infile, outfile, (thispart == numparts) ? 0 : (maxsize-written));

    if (thispart == numparts)
    {
        if (applefile)
            fprintf(outfile, "\r\n--=--\r\n");

        fprintf(outfile, "\r\n-----\r\n"); // end boundary
    }

    fclose(outfile);

    } // end of main for loop

    return 0;
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_failed
//
//    Fatal handle function
//
///////////////////////////////////////////////////////////////////////
void ssmtp_failed(int error_code)
{
    switch(error_code)
    {
        case MEM_ALLOC_FAIL:
            dbgmsg("ssmtp: memory allocate failed\n");
            break;
        case SOCKET_FAIL:
            dbgmsg("ssmtp: socket error\n");
            break;
        case HOSTNAME_ERR:
            dbgmsg("ssmtp: unresolved server name\n");
            break;
        case CONNECT_ERR:
            dbgmsg("ssmtp: unable to connect to server, check DNS and network settings\n");
            break;
        case RECV_ERR:
            dbgmsg("ssmtp: receive from server error\n");
            break;
        case SEND_ERR:
            dbgmsg("ssmtp: send to server error\n");
            break;
        case RECV_TIMEOUT:
            dbgmsg("ssmtp: receive from server timed out\n");
            break;
        case SMTP_FAILED:
            dbgmsg("ssmtp: SMTP server error\n");
            dbgmsg("%s", ssmtp_stored_message);
            break;
        case PIC_OPEN_ERR:
            dbgmsg("ssmtp: unable to open picture file\n");
            break;
        case TMP_OPEN_ERR:
            dbgmsg("ssmtp: unable to create tmep file\n");
            break;
        case FILE_READ_ERR:
            dbgmsg("ssmtp: file read error\n");
            break;
    }
}




///////////////////////////////////////////////////////////////////////
// Function ssmtp_connect
//
//    brief opens a socket and getting connection to server
//
///////////////////////////////////////////////////////////////////////
int ssmtp_connect(SMTP_Client_Info *smtpinfo)
{
    struct hostent *servp;
    struct sockaddr_in addr;

    // get host name if domain name assigned
    servp = NULL;
    servp = gethostbyname(smtpinfo->host);
    if( servp == NULL )
    {
        ssmtp_failed(HOSTNAME_ERR);
        return -1;
    }

    // open socket
    if( (smtpinfo->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        perror("ssmtp: socket");
        ssmtp_failed(SOCKET_FAIL);
        return -1;
    }

    // set non_blocking
    // fcntl(smtpinfo->sockfd, F_SETFL, O_NONBLOCK);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(smtpinfo->port);
    addr.sin_addr = *((struct in_addr *)servp->h_addr);
    memset(&(addr.sin_zero), '\0', 8);

    // connect
    if( connect(smtpinfo->sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1 )
    {
        perror("ssmtp: connect");
        ssmtp_failed(CONNECT_ERR);
        return -1;
    }

    return smtpinfo->sockfd;
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_okay
//
//    simplely read response from server
//
///////////////////////////////////////////////////////////////////////
int ssmtp_read(SMTP_Client_Info *smtpinfo, char *buf)
{
    int i,s,n;
    struct timeval timeout_tv;
    fd_set sockfds;

    s = smtpinfo->sockfd;

    timeout_tv.tv_sec = SMTPTIMEOUT;
    timeout_tv.tv_usec = 0;

    FD_ZERO(&sockfds);
    FD_SET(s, &sockfds);

    select(s+1, &sockfds, NULL, NULL, &timeout_tv);

    if(FD_ISSET(s, &sockfds))
    {
        n = recv(smtpinfo->sockfd, buf, MAXDATASIZE, 0);

        if(n<0)
        {
            perror("ssmtp: recv");
            ssmtp_failed(RECV_ERR);
            return -1;
        }

        buf[n]='\0';

        // store response message
        memset(ssmtp_stored_message, '\0', MAXDATASIZE);
        for( i=0; i<n; i++ )
            ssmtp_stored_message[i] = buf[i];

        return n;
    }
    else
    {
        // timeout, retry ?
        ssmtp_failed(RECV_TIMEOUT);
        return -1;
    }
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_send
//
//    simplely read response from server
//
///////////////////////////////////////////////////////////////////////
int ssmtp_send(SMTP_Client_Info *smtpinfo, char *buf)
{
    if( sendall(smtpinfo->sockfd, buf, strlen(buf)) < 0 )
    {
        ssmtp_failed(SEND_ERR);
        printf("ssmtp send FAIL !!!!\n");
        return -1;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_ready
//
//    simply read status code from server
//    to check whetehr server is ready
//
///////////////////////////////////////////////////////////////////////
int ssmtp_ready(SMTP_Client_Info *smtpinfo, char *status_code)
{
    int i;
    char buf[MAXDATASIZE];


    usleep(READ_DELAY); // wait for response, avoid packet split
    if( ssmtp_read(smtpinfo, buf) < 0 )
        return -1;

    i = strlen(status_code);
    if( !strncmp(status_code, buf, i) )
    {
        // status code match, return 0
        dbgmsg("%s", ssmtp_stored_message);
        return 0;
    }
    else
    {
        // status code not match, ssmtp state failed
        ssmtp_failed(SMTP_FAILED);
        return -1;
    }

}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_ehlo
//
//    sends EHLO SMTP command
//
///////////////////////////////////////////////////////////////////////
int ssmtp_ehlo(SMTP_Client_Info *smtpinfo, char *status_code)
{
    int i;
    char buf[MAXDATASIZE];


    strcpy(buf, "EHLO aaa.bbb");
    strcat(buf, smtpinfo->domain);
    strcat(buf, "\r\n");

    if( ssmtp_send(smtpinfo, buf) < 0 )
        return -1;

    usleep(READ_DELAY);
    memset(buf, 0, MAXDATASIZE);
    if( ssmtp_read(smtpinfo, buf) < 0 )
        return -1;

    i = strlen(status_code);
    if( !strncmp(status_code, buf, i) )
    {
        // status code match, return 0
        dbgmsg("%s", ssmtp_stored_message);
        return 0;
    }
    else
    {
        // status code not match, ssmtp state failed
        ssmtp_failed(SMTP_FAILED);
        return -1;
    }
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_helo
//
//    sends HELO SMTP command
//
///////////////////////////////////////////////////////////////////////
int ssmtp_helo(SMTP_Client_Info *smtpinfo, char *status_code)
{
    int i;
    char buf[MAXDATASIZE];


    strcpy(buf, "HELO aaa.bbb\r\n");
    if( ssmtp_send(smtpinfo, buf) < 0 )
        return -1;

    usleep(READ_DELAY);
    memset(buf, 0, MAXDATASIZE);
    if( ssmtp_read(smtpinfo, buf) < 0 )
        return -1;
    i = strlen(status_code);
    if( !strncmp(status_code, buf, i) )
    {
        // status code match, return 0
        dbgmsg("%s", ssmtp_stored_message);
        return 0;
    }
    else
    {
        // status code not match, ssmtp state failed
        ssmtp_failed(SMTP_FAILED);
        return -1;
    }
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_mail
//
//    sends MAIL SMTP command
//
///////////////////////////////////////////////////////////////////////
int ssmtp_mail(SMTP_Client_Info *smtpinfo, char *status_code)
{
    int i;
    char buf[MAXDATASIZE];


    strcpy(buf, "MAIL FROM: <");
    strcat(buf, smtpinfo->fromaddr);
    strcat(buf, ">\r\n");
    printf(buf);
    if( ssmtp_send(smtpinfo, buf) < 0 )
        return -1;

    usleep(READ_DELAY);
    memset(buf, 0, MAXDATASIZE);
    if( ssmtp_read(smtpinfo, buf) < 0 )
        return -1;

    i = strlen(status_code);
    if( !strncmp(status_code, buf, i) )
    {
        // status code match, return 0
        dbgmsg("%s", ssmtp_stored_message);
        return 0;
    }
    else
    {
        // status code not match, ssmtp state failed
        ssmtp_failed(SMTP_FAILED);
        return -1;
    }
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_rcpt
//
//    sends RCPT SMTP command
//
///////////////////////////////////////////////////////////////////////
int ssmtp_rcpt(SMTP_Client_Info *smtpinfo,SMTP_Mail_Info *mail , char *status_code)
{
    int i;
    char buf[MAXDATASIZE];


    strcpy(buf, "RCPT TO:<");
    strcat(buf, mail->to);
    strcat(buf, ">\r\n");
    if( ssmtp_send(smtpinfo, buf) < 0 )
        return -1;

    usleep(READ_DELAY);
    memset(buf, 0, MAXDATASIZE);
    if( ssmtp_read(smtpinfo, buf) < 0 )
        return -1;

    i = strlen(status_code);
    if( !strncmp(status_code, buf, i) )
    {
        // status code match, return 0
        dbgmsg("%s", ssmtp_stored_message);
        return 0;
    }
    else
    {
        // status code not match, ssmtp state failed
        ssmtp_failed(SMTP_FAILED);
        return -1;
    }
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_data
//
//    sends DATA SMTP command
//
///////////////////////////////////////////////////////////////////////
int ssmtp_data(SMTP_Client_Info *smtpinfo, char *status_code)
{
    int i;
    char buf[MAXDATASIZE];


    strcpy(buf, "DATA\r\n");
    if( ssmtp_send(smtpinfo, buf) < 0 )
        return -1;

    usleep(READ_DELAY);
    memset(buf, 0, MAXDATASIZE);
    if( ssmtp_read(smtpinfo, buf) < 0 )
        return -1;
    i = strlen(status_code);
    if( !strncmp(status_code, buf, i) )
    {
        // status code match, return 0
        dbgmsg("%s", ssmtp_stored_message);
        return 0;
    }
    else
    {
        // status code not match, ssmtp state failed
        ssmtp_failed(SMTP_FAILED);
        return -1;
    }
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_data
//
//    send whole content including attachment
//
///////////////////////////////////////////////////////////////////////
int ssmtp_content(SMTP_Client_Info *smtpinfo, SMTP_Mail_Info * mailinfo,char *status_code)
{
    FILE *infile = NULL, *fp;
    int i;
    char buf[MAXDATASIZE];
    unsigned char raw;
    char *headers=0;
    char *ctype=0;
    long maxsize=0;

    char mimetmpfile[80];
    strcpy(mimetmpfile ,"ltkjwarnMail");
    strcat(mimetmpfile,"mime.tmp");

#if 1
    strcpy(buf, "dummy");
#else
    strcpy(buf, "Date: ");
    strncat(buf, mail_date, strlen(mail_date)-1); // must strip \n and then plus \r\n
    strcat(buf, "\r\n");
    strcat(buf, "From: ");
    strcat(buf, smtpinfo->fromaddr);
    strcat(buf, "\r\n");
    strcat(buf, "To: ");
    strcat(buf, mailinfo->to);
    strcat(buf, "\r\n");
#endif

    if( sendall(smtpinfo->sockfd, buf, strlen(buf)) < 0 )
        return -1;
    // open file and send
    if(mailinfo->attachment){
       if( (infile = fopen(mailinfo->attachment, "r")) == NULL )
      {
        perror("ssmtp: open");
        ssmtp_failed(PIC_OPEN_ERR);
        return -1;
      }
      fseek(infile, 0, SEEK_SET); // seek to beginning
    }
    // MIME encode and output to temporary file TMPFILE
    encode(infile, (FILE *)0, mailinfo->attachment, mailinfo->body, mailinfo->subject, headers,
           maxsize, ctype, mimetmpfile);
    usleep(500); // wait file creation

    // open MIME file
    if( (fp = fopen(mimetmpfile, "r")) == NULL )
    {
        perror("ssmtp: open");
        ssmtp_failed(TMP_OPEN_ERR);
        return -1;
    }
    rewind(fp);
    // send out the MIME file
    while( feof(fp)==0 )
    {
        if( fread(&raw, 1, 1, fp) < 0 )
        {
            ssmtp_failed(FILE_READ_ERR);
            // delete tmporary file
            fclose(fp);
            fclose(infile);
            unlink(mimetmpfile);
            return -1;
        }

        if( sendall(smtpinfo->sockfd, &raw, 1) < 0)
        {
            fclose(fp);
            fclose(infile);
            unlink(mimetmpfile);
            return -1;
        }
    }

    // end of mail
    strcpy(buf, "\r\n.\r\n");
    if( sendall(smtpinfo->sockfd, buf, strlen(buf)) < 0 )
    {
        fclose(fp);
        fclose(infile);
        unlink(mimetmpfile);
        return -1;
    }

    usleep(READ_DELAY);
    memset(buf, 0, MAXDATASIZE);
    if( ssmtp_read(smtpinfo, buf) < 0 )
    {
        fclose(fp);
        if(infile)
           fclose(infile);
        unlink(mimetmpfile);
        return -1;
    }
    i = strlen(status_code);
    if( !strncmp(status_code, buf, i) )
    {
        // status code match, return 0
        fclose(fp);
        if(infile)
          fclose(infile);
        unlink(mimetmpfile);
        dbgmsg("%s", ssmtp_stored_message);
        return 0;
    }
    else
    {
        // status code not match, ssmtp state failed
        ssmtp_failed(SMTP_FAILED);
        fclose(fp);
        if(infile)
          fclose(infile);
        unlink(mimetmpfile);
        return -1;
    }
}


///////////////////////////////////////////////////////////////////////
// Function to64frombits
//
//    brief Raw bytes in quasi-big-endian order to
//    base 64 string (NUL-terminated)
//
//    param[out]    out        A pointer to a char to hold the converted string
//    param[in]    in        String to convert
//    param[in]    inlen    Length of the string to be converted
///////////////////////////////////////////////////////////////////////
void to64frombits(unsigned char *out, const unsigned char *in, int inlen)
{
    for (; inlen >= 3; inlen -= 3)
    {
        *out++ = base64digits[in[0] >> 2];
        *out++ = base64digits[((in[0] << 4) & 0x30) | (in[1] >> 4)];
        *out++ = base64digits[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
        *out++ = base64digits[in[2] & 0x3f];
        in += 3;
    }

    if (inlen > 0)
    {
        unsigned char fragment;

        *out++ = base64digits[in[0] >> 2];
        fragment = (in[0] << 4) & 0x30;

        if (inlen > 1)
            fragment |= in[1] >> 4;

        *out++ = base64digits[fragment];
        *out++ = (inlen < 2) ? '=' : base64digits[(in[1] << 2) & 0x3c];
        *out++ = '=';
    }

    *out = '\0';
}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_auth
//
//    sends AUTH command to server
//
///////////////////////////////////////////////////////////////////////
int ssmtp_auth(SMTP_Client_Info *smtpinfo)
{
    char local_in_buf[128];
    char local_tmp_buf[128];
    char local_out_buf[MAXDATASIZE];
    int len;
    int i;


    memset(local_tmp_buf,0,sizeof(local_tmp_buf));

    // login authentication
    if( smtpinfo->auth_mech==SASL_LOGIN )
    {
        to64frombits((unsigned char*)local_tmp_buf,
                     (const unsigned char *)smtpinfo->auth_user,
                     strlen(smtpinfo->auth_user));

        //asprintf(&local_out_buf,"AUTH LOGIN %s\r\n",local_tmp_buf);
        strcpy(local_out_buf, "AUTH LOGIN ");
        strcat(local_out_buf, local_tmp_buf);
        strcat(local_out_buf, "\r\n");

        if( ssmtp_send(smtpinfo, local_out_buf) < 0 )
            return -1;

        // commened by jzks
        //free(local_out_buf);


        usleep(READ_DELAY);
        if( ssmtp_read(smtpinfo, local_in_buf) < 0 )
            return -1;

        dbgmsg("%s", ssmtp_stored_message);

        if( strncmp(local_in_buf, "3", 1) != 0 )
        {
            ssmtp_failed(SMTP_FAILED);
            return -1;
        }

        memset(local_tmp_buf,0,sizeof(local_tmp_buf));

        to64frombits((unsigned char *)local_tmp_buf,
                     (const unsigned char *)smtpinfo->auth_pass,
                     strlen(smtpinfo->auth_pass));

        //asprintf(&local_out_buf,"%s\r\n",local_tmp_buf);
        strcpy(local_out_buf, local_tmp_buf);
        strcat(local_out_buf, "\r\n");

        if( ssmtp_send(smtpinfo, local_out_buf) < 0 )
            return -1;

        //free(local_out_buf);

        usleep(READ_DELAY);
        if ( ssmtp_read(smtpinfo, local_in_buf) < 0 )
            return -1;

        dbgmsg("%s", ssmtp_stored_message);

        if( strncmp(local_in_buf, "2", 1) != 0 )
        {
            ssmtp_failed(SMTP_FAILED);
            return -1;
        }
    }

    // plain authentication
    if( smtpinfo->auth_mech==SASL_PLAIN )
    {
        //asprintf(&local_out_buf,"^%s^%s",smtpinfo->auth_user,smtpinfo->auth_pass);
        strcpy(local_out_buf, "^");
        strcat(local_out_buf, smtpinfo->auth_user);
        strcat(local_out_buf, "^");
        strcat(local_out_buf, smtpinfo->auth_pass);

        len = strlen(local_out_buf);

        for ( i = len-1 ; i >= 0 ; i-- )
        {
            if (local_out_buf[i]=='^')
            {
                local_out_buf[i]='\0';
            }
        }

        to64frombits((unsigned char *)local_tmp_buf,(const unsigned char *)local_out_buf,len);

        //free(local_out_buf);

        //asprintf(&local_out_buf,"AUTH PLAIN %s\r\n",local_tmp_buf);
        strcpy(local_out_buf, "AUTH PLAIN ");
        strcat(local_out_buf, local_tmp_buf);
        strcat(local_out_buf, "\r\n");

        if(ssmtp_send(smtpinfo, local_out_buf) < 0 )
            return -1;

        //free(local_out_buf);

        usleep(READ_DELAY);
        if( ssmtp_read(smtpinfo, local_in_buf) < 0 )
            return -1;

        dbgmsg("%s", ssmtp_stored_message);

        if( strncmp(local_in_buf, "2", 1) != 0 )
        {
            ssmtp_failed(SMTP_FAILED);
            return -1;
        }
    }


    return 0;

}


///////////////////////////////////////////////////////////////////////
// Function ssmtp_quit
//
//    sends QUIT command to server
//    never mind what server responses
//
///////////////////////////////////////////////////////////////////////
int ssmtp_quit(SMTP_Client_Info *smtpinfo)
{
    int i;
    char buf[MAXDATASIZE];

    strcpy(buf, "QUIT\r\n");
    if( ssmtp_send(smtpinfo, buf) < 0 )
        return -1;

    usleep(READ_DELAY);
    memset(buf, 0, MAXDATASIZE);
    if( ssmtp_read(smtpinfo, buf) < 0 )
        return -1;

    dbgmsg("%s", ssmtp_stored_message);
        return 0;
}


void initClientInfo(SMTP_Client_Info * client){
 
  memset(client, 0 ,sizeof(struct SMTP_Client_Info));
  client->port = SMTPPORT;
  client->auth_mech = SASL_DEFAULT;
  client->domain = "";

}

void initMailInfo( SMTP_Mail_Info * mail){

  memset(mail, 0 ,sizeof(struct SMTP_Mail_Info));
  mail->subject = "";
  mail->body = "";
}

int sendMail(SMTP_Client_Info * smtpinfo, SMTP_Mail_Info * mail){  //if succeced ,return 0,else return -1

 if(smtpinfo ==0 || mail ==0)
    {
      dbgmsg("SMTP_Client_Info and SMTP_Mail_Infor can not be NULL !");
      return -1;
    }
  if(ssmtp_connect(smtpinfo) < 0)
       return -1;
  if( ssmtp_ready(smtpinfo ,SERVICE_READY) < 0)
       return -1;

    // say helo to server
    if( ssmtp_ehlo(smtpinfo, REPLY_OKAY) < 0 )
    {
        // EHLO failed, try HELO instead
        if( ssmtp_helo(smtpinfo, REPLY_OKAY) < 0 )
            ssmtp_quit(smtpinfo);
    }
    else
    {
        // authentication
        if( strlen(smtpinfo->auth_user) >0 && strlen(smtpinfo->auth_pass)>0 )
        {
            if( ssmtp_auth(smtpinfo) < 0 )
            {
                // auth failed, try HELO instead
                if( ssmtp_helo(smtpinfo, REPLY_OKAY) < 0 )
                    ssmtp_quit(smtpinfo);
            }
        }
    }

        // mail from
    if ( ssmtp_mail(smtpinfo, REPLY_OKAY) < 0)
          return -1;

    // mail to
    if ( ssmtp_rcpt(smtpinfo,mail , "2") < 0)
          return -1;

    // data
    if (ssmtp_data(smtpinfo, "3") < 0)
         return -1;

        // send content
    if( ssmtp_content(smtpinfo, mail ,"2") < 0)
         return -1;

        // send quit
    ssmtp_quit(smtpinfo);

    close(smtpinfo->sockfd);

    return 0;
}

