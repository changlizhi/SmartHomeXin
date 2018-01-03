#include <string.h>
#include <math.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "include/lib/datachg.h"

static const char g_strlib_digits[] = {"0123456789ABCDEF"};

unsigned short htons(unsigned short us)
{
    VARSHORT var1, var2;

    var1.us = us;
    var2.uc[0] = var1.uc[1];
    var2.uc[1] = var1.uc[0];

    return(var2.us);
}

unsigned int htonl(unsigned int ul)
{
    VARLONG var1, var2;

    var1.ul = ul;
    var2.uc[0] = var1.uc[3];
    var2.uc[1] = var1.uc[2];
    var2.uc[2] = var1.uc[1];
    var2.uc[3] = var1.uc[0];

    return(var2.ul);
}
#if 0
/*
Func Description: 缓存区清零
Func Param:
Func Return:
*/
void memset(unsigned char *addr, unsigned char value, int len)
{
    int i;

    for(i=0; i<len; i++)
        *addr++ = value;
}

/*
Func Description: 内存拷贝
Func Param:
Func Return:
*/
void memcpy(unsigned char *dst, unsigned char *src, int len)
{
    int i;

    for(i=0; i<len; i++)
        *dst++ = *src++;
}
#endif
/*
Func Description: 计算绝对值
Func Param:
Func Return:
*/
int abs(int i)
{
    if(i < 0) i*=-1;

    return(i);
}

/*
Func Description: 绝对减
Func Param:
Func Return:
*/
int uldel(unsigned int ul1, unsigned int ul2, unsigned int *uld)
{
    int rtn;
    unsigned int ulrtn;

    if(ul1 < ul2)
    {
        ulrtn = ul2-ul1;
        rtn = -1;
    }
    else if(ul1 > ul2)
    {
        ulrtn = ul1-ul2;
        rtn = 1;
    }
    else
    {
        ulrtn = 0;
        rtn = 0;
    }

    *uld = ulrtn;
    return rtn;
}

unsigned short make_short(unsigned char *str)
{
    VARSHORT us;

    us.uc[0] = str[0];
    us.uc[1] = str[1];

    return(us.us);
}

void depart_short(unsigned short us, unsigned char *str)
{
    VARSHORT var;

    var.us = us;


    str[0] = var.uc[0];
    str[1] = var.uc[1];
}

unsigned short makepkt_short(unsigned char *str)
{
    VARSHORT us;

    us.uc[0] = str[1];
    us.uc[1] = str[0];

    return(us.us);
}

void departpkt_short(unsigned short us, unsigned char *str)
{
    VARSHORT var;

    var.us = us;

    str[0] = var.uc[1];
    str[1] = var.uc[0];
}


unsigned int make_long(unsigned char *str)
{
    VARLONG ul;

    ul.uc[0] = str[0];
    ul.uc[1] = str[1];
    ul.uc[2] = str[2];
    ul.uc[3] = str[3];

    return(ul.ul);
}
unsigned int make_shortlong(unsigned char *str)
{
    VARLONG ul;

    ul.uc[0] = str[0];
    ul.uc[1] = str[1];
    ul.uc[2] = str[2];
    ul.uc[3] = 0;

    return(ul.ul);
}

void depart_long(unsigned int ul, unsigned char *str)
{
    VARLONG var;

    var.ul = ul;

    str[0] = var.uc[0];
    str[1] = var.uc[1];
    str[2] = var.uc[2];
    str[3] = var.uc[3];
}
void depart_shortlong(unsigned int ul, unsigned char *str)
{
    VARLONG var;
    var.ul=ul;

    str[0] = var.uc[0];
    str[1] = var.uc[1];
    str[2] = var.uc[2];

}

/*
Func Description: BCD转为unsigned int
Func Param:
    bcd: bcd数指针
    len: bcd数长度
Func Return: unsigned int
*/
unsigned int bcdtol(const unsigned char *bcd, int len)
{
    unsigned int rtn;
    int i;
    unsigned char uc;

    rtn=0;

    for(i=(len-1); i>=0; i--)
    {
        rtn *= 100;

        uc = bcd[i]&0xf0;
        uc >>= 4;
        uc *= 10;

        uc += bcd[i]&0x0f;

        rtn += (unsigned int)uc&0xff;
    }

    return(rtn);
}

/*
Func Description: unsigned int 转 BCD
Func Param:
    ul: 源unsigned int
    bcd: bcd数指针
    maxlen: bcd数最大长度
Func Return: 0-正常, 1-溢出
*/
int ltobcd(unsigned int ul, unsigned char *bcd, int maxlen)
{
    int i;
    unsigned char uc;

    for(i=0; i<maxlen; i++)
    {
        bcd[i] = 0;
    }

    i = 0;
    while(0 != ul)
    {
        if(i >= maxlen)
            return 1;

        uc = (unsigned char)(ul%10);
        ul /= 10;
        uc += (unsigned char)((ul%10)<<4);
        ul /= 10;

        bcd[i] = uc;
        i++;
    }

    return 0;
}

/*
Func Description: BCD 转 int
Func Param:
    bcd: bcd数指针
    len: bcd数长度
Func Return: int
*/
/*int bcdtoi(unsigned char *bcd, int len)
{
    int rtn;
    int i;
    unsigned char uc, flag;

    rtn=0;
    flag = 0;

    for(i=(len-1); i>=0; i--)
    {
        rtn *= 100;

        if(i == (len-1))
        {
            uc = bcd[i]&0xf0;
            if(uc) flag = 1;
            uc = 0;
        }
        else
        {
            uc = bcd[i]&0xf0;
            uc >>= 4;
            uc *= 10;
        }

        uc += bcd[i]&0x0f;

        rtn += (int)uc&0xff;
    }

    if(flag) rtn *= -1;
    return(rtn);
}*/

/*
Func Description: int 转 BCD
Func Param:
    i: 源int
    bcd: bcd数指针
    len: bcd数最大长度
Func Return:0-正常, 1-溢出
*/
int itobcd(int l, unsigned char *bcd, int maxlen)
{
    int i, rtn;
    unsigned char uc, flag;

    if(maxlen <= 0) return 0;

    if(l < 0)
    {
        l *= -1;
        flag = 1;
    }
    else flag = 0;

    for(i=0; i<maxlen; i++)
    {
        bcd[i] = 0;
    }

    i = 0;
    while(0 != l)
    {
        if(i >= maxlen)
            break;

        uc = (unsigned char)(l%10);
        l /= 10;
        uc += (unsigned char)((l%10)<<4);
        l /= 10;

        bcd[i] = uc;
        i++;
    }

    maxlen -= 1;
    if((bcd[maxlen]&0xf0) || (0 != l)) rtn = 1;
    else rtn = 0;

    bcd[maxlen] &= 0x0f;
    if(flag) bcd[maxlen] |= 0x10;

    return(rtn);
}

/*
Func Description: 检查BCD数是否为零
Func Param:
Func Return:1-全为零, 0-非零
*/
/*int bcd_iszero(unsigned char *bcd, int len)
{
    int i;

    for(i=0; i<len; i++)
    {
        if(0 != bcd[i]) break;
    }

    if(i >= len) return 1;
    else return 0;
}*/

/*
Func Description: 检查BCD数是否全为flag
Func Param:
    bcd: bcd数指针
    len: bcd数长度
    flag: 检测字节
Func Return:1-全为flag, 否则为0
*/
int bcd_isempty(unsigned char *bcd, int len, unsigned char flag)
{
    int i;

    for(i=0; i<len; i++)
    {
        if(flag != bcd[i]) break;
    }

    if(i >= len) return 1;
    else return 0;
}

/*
Func Description: unsigned char数组转为BCD数组
Func Param:
    str: 源和目的缓存
    len: 缓存长度
Func Return:
*/
void hextobcd(unsigned char *str, int len)
{
    int i;
    unsigned char uc1, uc2;

    for(i=0; i<len; i++)
    {
        uc1 = str[i]%10;
        uc2 = str[i]/10;
        if(uc2 > 9) uc2 = 9;
        uc1 |= (uc2<<4)&0xf0;

        str[i] = uc1;
    }
}

/*
Func Description: BCD数组转为unsigned char数组
Func Param:
    str: 源和目的缓存
    len: 缓存长度
Func Return:
*/
void bcdtohex(unsigned char *str, int len)
{
    int i;
    unsigned char uc1, uc2;

    for(i=0; i<len; i++)
    {
        uc1 = str[i]&0x0f;
        uc2 = str[i]&0xf0;
        uc2 /= 16;
        uc2 *= 10;
        uc1 += uc2;

        str[i] = uc1;
    }
}

//rate = XX.XX%*10000
/*
Func Description: 计算比例 return=ul*rate%
Func Param:
Func Return:
*/
unsigned int ulxrate(unsigned int ul, unsigned short rate)
{
    INT64 li;
    li = (INT64)ul;

    li *= rate;
    li /= 10000;

    ul = (unsigned int)li;
    return(ul);
}

#define CRC16_POLY    0x1021
const unsigned short g_crc_tab[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
};

/*
Func Description: 计算CRC校验和
Func Param:
Func Return:
*/
unsigned short cal_crc(unsigned char *pbuf, unsigned int count)
{
    unsigned short temp = 0;

    if(0 == count) return 0;

    while ( count-- != 0 )
    {
        temp = ( temp<<8 ) ^ g_crc_tab[ ( temp>>8 ) ^ *pbuf++ ];
    }
    
    return temp;
}

/*float make_float(unsigned char *str)
{
    VARFLOAT var;

    var.uc[0] = str[0];
    var.uc[1] = str[1];
    var.uc[2] = str[2];
    var.uc[3] = str[3];

    return(var.ft);
}*/

/*double make_double(unsigned char *str)
{
    int i;
    VARDOUBLE var;

    for(i=0; i<8; i++) var.uc[i] = str[i];

    return(var.db);
}*/

/*void depart_float(float ft, unsigned char *str)
{
    VARFLOAT var;

    var.ft = ft;
    str[0] = var.uc[0];
    str[1] = var.uc[1];
    str[2] = var.uc[2];
    str[3] = var.uc[3];
}*/

/*void depart_double(double db, unsigned char *str)
{
    VARDOUBLE var;
    int i;

    var.db = db;
    for(i=0; i<8; i++) str[i] = var.uc[i];
}*/

void flex4tobcd(unsigned char *src, unsigned char type, unsigned char *bcd, int len)
{
    VARFLEX_4 flex;

    switch(type)
    {
    case VARFLEX_ULONG:
        flex.uc[0] = src[0];
        flex.uc[1] = src[1];
        flex.uc[2] = src[2];
        flex.uc[3] = src[3];
        ltobcd(flex.ul, bcd, len);
        break;
    case VARFLEX_INT:
        flex.uc[0] = src[0];
        flex.uc[1] = src[1];
        flex.uc[2] = src[2];
        flex.uc[3] = src[3];
        itobcd(flex.si, bcd, len);
        break;
    case VARFLEX_UCHAR:
        memcpy(bcd, src, len);
        hextobcd(bcd, len);
        break;
    case VARFLEX_BCD:
        memcpy(bcd, src, len);
        break;
    case VARFLEX_USHORT:
        flex.uc[0] = src[0];
        flex.uc[1] = src[1];
        ltobcd(flex.us, bcd, len);
        break;
    case VARFLEX_SSHORT:
        flex.uc[0] = src[0];
        flex.uc[1] = src[1];
        itobcd(flex.ss, bcd, len);
        break;
    /*
    case VARFLEX_USHORTFLAG
        for(i=0;i<len;i++)
        {
        flex.uc[0] = src[i];
        flex.uc[1] = src[i+1];
        ltobcd(flex.us, bcd, 2);
        }
    */
    default:
        memset(bcd, 0, len);
        break;
    }

}


/*
Func Description:  计算功率因数
Func Param:
    pwra: 有功功率
    pwri: 无功功率
Func Return: 功率因数
*/
unsigned int cal_pwrf(int pwra, int pwri)
{
    double db1, db2;
    int i;

    db1 = (double)pwra;
    db1 *= pwra;
    db2 = (double)pwri;
    db2 *= pwri;
    db1 += db2;
    db1 = sqrt(db1);
    i = (int)db1;
    if(0 == i) i = 1000;
    else
    {
        pwra *= 1000;
        i = pwra / i;
        if(i < 0) i *= -1;
    }

    return((unsigned int)i);
}

//return NN.NN%
/*
Func Description: 计算谐波含有率
Func Param:
    vol: 电压
    cwvol: 谐波电压
Func Return: 含有率
*/
/*unsigned short cal_cwpercent(unsigned int vol, unsigned int cwvol)
{
    double db;
    unsigned int i;

    if((0 == cwvol) || (0 == vol)) return 0;

    vol *= vol;
    i = cwvol*cwvol;

    vol += i;

    db = (double)vol;
    db = sqrt(db);
    vol = (unsigned int)db;

    if(0 == vol) return 0;

    cwvol *= 10000;
    cwvol /= vol;

    return((unsigned short)cwvol);
}*/

/*
计算三相三线电流的极性
return : 1-负, 0-正
flag : 0-A相, 1-C相
*/
unsigned char cal_amppola(int pwra, int pwri, unsigned char flag)
{
    double db;
    int meg;

    if(0 == pwra) return 0;

    db = (double)pwri;
    db /= pwra;
    db = atan(db);
    db *= 57.29578;   //*180/Pi
    meg = (int)db;

    if(0 == flag) meg -= 30;
    else meg += 30;
    if(pwra < 0) meg += 180;

    while((meg < -180) || (meg > 180))
    {
        if(meg > 0) meg -= 360;
        else meg += 360;
    }

    if((meg < 90) && (meg > -90)) return 0;
    else return 1;
}

/*void bcdtonum(unsigned char*dst,unsigned char *scr,int len)
{
    int i;
    unsigned char flag=0;
    for(i=0; i<len; i++)
    {
        dst[i] = 0;
    }
    if(len%2)flag=1;
    for(i=len/2-1;i>=0;i--)
    {
        dst[len-(i+1)*2]=(scr[i]>>4)&0x0f;
        dst[len+1-(i+1)*2]=scr[i]&0x0f;
    }
    return ;
}

void strtobcd( char *str, unsigned char *bcd, int maxlen)
{
    int i, j, addup, len;
    unsigned char uc;

    for(i=0; i<maxlen; i++) bcd[i] = 0;

    len=maxlen*2;
    if(len%2) j = len/2+1;
    else j= len/2;

    if(j > maxlen)
    {
        len = maxlen*2;
        j = maxlen;
    }
    else if(j <= 0) return;

    if(len%2) addup = 1;
    else addup = 0;

    j--;
    for(i=0; i<len; i++)
    {
        uc=str[i];
        if(addup)
        {
            bcd[j] |= uc&0x0f;
            j--;
        }
        else
        {
            bcd[j] |= (uc<<4)&0xf0;
        }

        addup = (addup)?0:1;
    }
}
void bcdltohnum(unsigned char*dst,unsigned char *scr,int len,int flag)
{
    int i;
//    unsigned char flag=0;
    for(i=0; i<len; i++)
    {
        dst[i] = 0;
    }
    if(0==flag)
    {
        for(i=0;i<=len/2-1;i++)
        {
            dst[i*2]=(scr[i]>>4)&0x0f;
            dst[i*2+1]=scr[i]&0x0f;
        }
    }
    else
    {
        unsigned char uc;
        for(i=0;i<len;i++)
        {
            if((scr[i] >= '0') && (scr[i] <= '9'))
                uc = scr[i] - '0'+1;
            else if((scr[i] >= 'a') && (scr[i] <= 'z'))
                uc = scr[i] - 'a' + 11;
            else if((scr[i] >= 'A') && (scr[i] <= 'Z'))
                uc = scr[i] - 'A' + 37;
               else uc=0;
            dst[i]=uc;

        }

    }
        return ;
}
void strtonum(unsigned char *dst,unsigned char *scr,int len)
{
    int i;
    unsigned char uc;
    for(i=0;i<len;i++)
    {
        dst[i]=' ';
    }
    for(i=0;i<len;i++)
    {
        if((scr[i]>=1) && (scr[i]<=10))
            uc=scr[i]+'0'-1;
        else if((scr[i]>=11)&&(scr[i]<=36))
            uc=scr[i]+'a'-11;
        else if((scr[i]>=37)&&(scr[i]<=63))
            uc=scr[i]+'A'-37;
        else uc=' ';
        dst[i]=uc;
    }
}*/
/*
int compile_Apn(int flag, unsigned char *buf, int len, unsigned char *str)
{
    int i, j;

    if(0 == flag)   //read
    {
        for(i=0; i<len; i++)
        {
            if(0xaa == buf[i]) break;
        }
        if(0 == i) return 0;
        i -= 1;
        for(;i>=0;i--)
        {
            str += (char)buf[i];
        }
    }
    else  //set
    {
        i = strlen(str);
        if(i > len) i = len;

        for(j=0; j<i; j++)
        //    buf[i-1-j] = str.GetAt(j);
        for(j=i; j<16; j++)
            buf[j] = 0xaa;
    }

    return 0;
}*/
void highttolowtime(unsigned char *str,int len)
{
    int i;
    unsigned char buf;
    for(i=0;i<len/2;i++)
    {
            buf=str[i];
            str[i]=str[len-i-1];
            str[len-i-1]=buf;
    }
}

void hex2toa(unsigned char uc, char *str)
{
    *str++ = g_strlib_digits[(uc>>4)&0x0f];
    *str++ = g_strlib_digits[uc&0x0f];

    *str = 0;
}

void print_charbuf(unsigned char *buf, int len)
{
    static char str[128];

    int i;
    char *pc;

    pc = str;
    *pc = 0;
    for(i=1; i<=len; i++)
    {
        hex2toa(*buf++, pc);
        pc += 2;

        *pc++ = ' ';
        *pc = 0;

        if(0 == (i&0x0f))
        {
            PrintLog(0, "%s\r\n", str);
            pc = str;
            *pc = 0;
        }
        else if(0 == (i&0x07))
        {
            *pc++ = ':';
            *pc++ = ' ';
            *pc = 0;
        }
    }

    if(0 != str[0])
    {
        PrintLog(0, "%s\r\n", str);
    }
}

static unsigned long g_itoa_tens;
void itoa(int i, char *str)
{
    int digit;

    if(0 == i)
    {
        *str++ = '0';
        *str = 0;
        return;
    }
    else if(i < 0)
    {
        *str++ = '-';
        i *= -1;
    }

    g_itoa_tens = 1;
    while((i/10) >= g_itoa_tens)
        g_itoa_tens *= 10;

    while(g_itoa_tens)
    {
        digit = i/g_itoa_tens;
        *str++ = g_strlib_digits[digit];

        i -= digit * g_itoa_tens;
        g_itoa_tens /= 10;
    }

    *str = 0;
}


void iptoa(unsigned long ip, char *str)
{
    int i;
    unsigned char *pip = (unsigned char *)&ip;

    for(i=3; i>=0; i--)
    {
        itoa((int)(pip[i])&0xff, str);

        str += strlen(str);
        if(i > 0)
            *str++ = '.';
    }
}

/*********************************************************************************/  
/*函数名称: GetCRC16()                            
*输入参数：  共  个参数；  
*输出参数：  共  个参数；  
*返回值：    
*需储存的参数： 共  个参数；      
*功能介绍：    
        (1)CRC16校验； 返回校验码；                
*修改日志：  
*[2005-11-28 16:40]    Ver. 1.00  
        开始编写；  
        完成；                                      
/*                                      */  
/*********************************************************************************/  
  
unsigned short GetCRC16(unsigned char *puchMsg, unsigned short usDataLen)
{    
    /* CRC 高位字节值表 */    
    unsigned char auchCRCHi[256] = {  
    
    0xAD, 0x50, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x23, 0x50
      
    };    
      
    unsigned char auchCRCLo[256] = {  
    
    0xAD, 0x50, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x23, 0x50
      
    };  
      
    unsigned char uchCRCHi = 0xFF ; /* 高CRC字节初始化 */    
    unsigned char uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */    
    unsigned uIndex = 0; /* CRC循环中的索引 */    
      
    while (usDataLen--) /* 传输消息缓冲区 */    
    {    
        uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */    
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;    
        uchCRCLo = auchCRCLo[uIndex] ;    
    }    
    return (unsigned short)((unsigned short)uchCRCHi << 8 | uchCRCLo) ;    
}

