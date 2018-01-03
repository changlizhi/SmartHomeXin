/**
* rfmet.h -- rf抄表接口头文件
* 
* 作者: csg
* 创建时间: 2012-9-2
* 最后修改时间: 2012-9-12
*/

#ifndef _RFMET_H
#define _RFMET_H

#define RS485BUS_BUF_LEN        272
int RFMetRead(const plc_dest_t *dest, unsigned long itemid, unsigned char *buf, int len);
int RFMetCtrl(const plc_dest_t *dest, unsigned long itemid,unsigned char *buf,int len);
#endif/*_RS485MET_H*/
