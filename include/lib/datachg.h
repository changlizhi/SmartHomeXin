#ifndef _DATACHG_H
#define _DATACHG_H
#include <string.h>
/*unsigned short htons(unsigned short us);
#define ntohs htons
unsigned int htonl(unsigned int ul);
#define ntohl htonl*/
int abs(int i);
int uldel(unsigned int ul1, unsigned int ul2, unsigned int *uld);
unsigned short make_short(unsigned char *str);
void depart_short(unsigned short us, unsigned char *str);

unsigned short makepkt_short(unsigned char *str);
void departpkt_short(unsigned short us, unsigned char *str);

unsigned int make_long(unsigned char *str);
void depart_long(unsigned int ul, unsigned char *str);

unsigned int bcdtol(const unsigned char *bcd, int len);
int ltobcd(unsigned int ul, unsigned char *bcd, int maxlen);
//int bcd_iszero(unsigned char *bcd, int len);
int bcd_isempty(unsigned char *bcd, int len, unsigned char flag);
//int bcdtoi(unsigned char *bcd, int len);
int itobcd(int l, unsigned char *bcd, int maxlen);

void hextobcd(unsigned char *str, int len);
void bcdtohex(unsigned char *str, int len);
unsigned int make_shortlong(unsigned char *str );

unsigned int ulxrate(unsigned int ul, unsigned short rate);

unsigned short cal_crc(unsigned char *pbuf, unsigned int count);

void depart_shortlong(unsigned int ul, unsigned char *str);
void flex4tobcd(unsigned char *src, unsigned char type, unsigned char *bcd, int len);

unsigned int cal_pwrf(int pwra, int pwri);
unsigned char cal_amppola(int pwra, int pwri, unsigned char flag);
void highttolowtime(unsigned char *str,int len);


//int strcmp(char * str1, char * str2);
int ustrcmp(unsigned char *str1, unsigned char *str2, int len, unsigned char reverse);
//char * strcat(char * str1, char * str2);
//int strlen(char * str);
//char * strcpy(char * dest, char * src);
void itostring(int i, char *str);
//void hex8toa(unsigned int ul, char *str);
void hex4toa(unsigned short us, char *str);
//void hex2toa(unsigned char uc, char *str);
unsigned char add_chksum(unsigned char *buf, int len);
void flex_itostring(int i, char *str, int span1, int span2);
void flex_hextoa(unsigned int ul, char *str, int span1, int span2);

//void sprintf(char *buf, char *fmt, ...);

int stringtoi(char *str);
//int atox(char *str);

int atoip(char *str, unsigned int *pip);
void itoa(int i, char *str);
void iptoa(unsigned long ip, char *str);
void bcd2str(char *str, unsigned char *bcd, int len, int dotlen);
void hex2toa(unsigned char uc, char *str);
void print_charbuf(unsigned char *buf, int len);
unsigned short GetCRC16(unsigned char *puchMsg, unsigned short usDataLen);

#endif /*_DATACHG_H*/
