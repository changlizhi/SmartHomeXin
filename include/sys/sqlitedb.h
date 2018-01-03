/**
* sqlitedb.c -- 类ini文本配置文件储存头文件
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-4-22
* 最后修改时间: 2009-4-22
*/

#ifndef _SQLITEDB_H
#define _SQLITEDB_H

//创建表库文件
// param table_name : 表名
// param pdata: 输入缓存区
// param pdatalen     : 缓存区长度
int ReadFromTable(char * table_name, unsigned char * pdata, unsigned int pdatalen);

//保存表库文件
// param table_name : 表名
// param qdata: 输出缓存区
// param qdatalen     : 缓存区长度
int SaveToTable(char * table_name, unsigned char * qdata, unsigned int qdatalen);
#endif /*_XIN_H*/

