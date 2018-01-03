#ifndef _FAAL_DL_H
#define _FAAL_DL_H

/*上行通道编号*/
#define FAALITF_SERIAL    0
#define FAALITF_ETHMTN    1
#define FAALITF_GPRS    2
#define FAALITF_ETHER    3
#define FAALITF_SMS    4
#define FAALITF_IR    5
#define FAALITF_CASCADE    6
#define FAALITF_MAXNUM    7

#define FAALFRAM_MAX 7/*帧内最大数量*/

int faal_sendpkt(unsigned char itf, faalpkt_t *pkt);
int faal_mulfram_sendpkt(unsigned char itf, faalpkt_t *pkt, unsigned char frameid);
int faal_makepkt(unsigned char itf, faalpkt_t *pkt);

void faal_printpkt(faalpkt_t *pkt, char *prompt);

#endif
