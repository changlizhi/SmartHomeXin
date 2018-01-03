///////////////////////////////////////////////////////////////////////
// FILE DESCRIPTION
//    Definitions for ssmtp.c
//
// VERSION
//    0.01 (2005/11/7)
//
// AUTHOR
//    Jerry Huang
///////////////////////////////////////////////////////////////////////

// debug message
#define DEBUGMSG 1
#ifdef DEBUGMSG
    #define dbgmsg(args...) printf(args)
#else
    #define dbgmsg(args...)
#endif

// host name length for message id
#ifndef MAXHOSTNAMELEN
    #define MAXHOSTNAMELEN 64
#endif

// max reference id
#define NUMREFERENCES 4

// max data size
#define MAXDATASIZE    1024

// timeout in seconds for waiting response
#define SMTPTIMEOUT 5

// time delay for packet receive, microseconds
#define READ_DELAY 10000

// SMTP port
#define SMTPPORT 25

// SASL authentication mechanism
#define SASL_LOGIN        0    // AUTH LOGIN SASL mechanism
#define SASL_PLAIN        1    // AUTH PLAIN SASL mechanism
#define SASL_CRAMMD5    2    // AUTH CRAM-MD5 SASL mechanism
#define SASL_DEFAULT    1    // Set the default auth mechanism


// system called error code
#define MEM_ALLOC_FAIL    1
#define SOCKET_FAIL        2
#define HOSTNAME_ERR    3
#define CONNECT_ERR        4
#define RECV_ERR        5
#define SEND_ERR        6
#define RECV_TIMEOUT    7
#define SMTP_FAILED        8
#define PIC_OPEN_ERR    9
#define TMP_OPEN_ERR    10
#define FILE_READ_ERR    11


// SMTP server response code
#define HELP_REPLY        "211"    // System status, or system help reply
#define HELP_MESSAGE    "214"    // Help message
#define SERVICE_READY    "220"    // <domain> Service ready
#define SERVICE_CLOSE1    "221"    // <domain> Service closing transmission channel
#define REPLY_OKAY        "250"    // Requested mail action okay, completed
#define USER_NOT_LOCAL1    "251"    // User not local; will forward to <forward-path>
#define VRFY_USER_ERR    "252"    // Cannot VRFY user, but will accept message and attempt delivery
#define MAIL_INPUT        "354"    // Start mail input; end with <CRLF>.<CRLF>
#define SERVICE_CLOSE2    "421"     // <domain> Service not available, closing transmission channel
#define MAILBOX_BUSY    "450"    // Requested mail action not taken: mailbox unavailable
#define MAIL_ABORTED    "451"    // Requested action aborted: local error in processing
#define STORE_ERR        "452"    // Requested action not taken: insufficient system storage
#define SYNTAX_ERR        "500"    // Syntax error, command unrecognized
#define PARAM_ERR        "501"    // Syntax error in parameters or arguments
#define NO_COMMAND        "502"    // Command not implemented
#define BAD_SEQUENCE    "503"    // Bad sequence of commands
#define NO_PARAM        "504"    // Command parameter not implemented
#define NO_MAILBOX        "550"    // Requested action not taken: mailbox unavailable
#define USER_NOT_LOCAL2    "551"    // User not local; please try <forward-path>
#define NO_STORAGE        "552"    // Requested mail action aborted: exceeded storage allocation
#define MAILBOC_ERR        "553"    // Requested action not taken: mailbox name not allowed
#define NO_SMTP            "554"     // Transaction failed  (Or, in the case of a connection-opening



// brief structure the server, and the connection socket
typedef struct SMTP_Client_Info
{
    char *host;            // Name of the relay host
    char *fromaddr;        // Address to use in the MAIL FROM command
    char *fromName;      //name of the sender  ,add by xudz
//    char *toaddr;
    char *domain;        // Domain to send in HELO/EHLO command
    char *auth_user;    // User to use in SASL authentication
    char *auth_pass;    // Password to use in SASL authentication
    int auth_mech;        // Mechanism to use in SASL authentication
    int port;            // Port to use to connect to the server
    int sockfd;            // Socket descriptor
    int num_rcpts;        // Number of recipients
#ifdef HAVE_SSL
    bool_t use_tls;        // Whether we should use SSL/TLS or not
    bool_t use_starttls;// Whether to use STARTTLS or not
    bool_t using_tls;    // Whether we started TLS or not
    SSL *ssl;            // SSL/TLS descriptor
#endif
}SMTP_Client_Info;

typedef struct SMTP_Mail_Info
{
    char * to;
    char * subject;
    char * body;
    char * attachment;
}SMTP_Mail_Info;

void initClientInfo( SMTP_Client_Info * client);

void initMailInfo( SMTP_Mail_Info * mail);

int sendMail( SMTP_Client_Info * smtpinfo, SMTP_Mail_Info * mail);   //if succeced ,return 0,else return -1
