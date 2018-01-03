/**
* xin.c -- 类ini文本配置文件储存
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-4-21
* 最后修改时间: 2009-5-5
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sqlite3.h>
#include "include/sys/mutex.h"
#include "include/debug.h"
#include "include/environment.h"
#include "include/debug/shellcmd.h"
unsigned char table_value = 0;
int table_callback(void* table_name,int nCount,char** pValue,char** pName)
{
/*    int i, flag = 0;
    char *p = (char *)table_name;
    //char *p = "Unique_1";
    char *q = "name";
         PrintLog(0, "nCount = %d\n", nCount);

    for(i=0; i<nCount; i++){
        PrintLog(0, "字段名: %s <-> 字段值: %s\n", pName[i], pValue[i]);
        if(!memcmp(pValue[i], p, 8) && !memcmp(pName[i], q, 4)) flag = 1;
    }
    if(flag) table_value = 1;//表存在
    else  table_value = 0;//表不存在*/
         return 0;
}

//创建表库文件
// param table_name : 表名
// param pdata: 输入缓存区
// param pdatalen     : 缓存区长度
int ReadFromTable(char * table_name, unsigned char * pdata, unsigned int pdatalen)
{
#if 0
    sqlite3 * db = NULL;
    int result;
    char *errmsg = NULL;
    sqlite3_stmt *stat =  NULL;
    char sql[128]={0};
    table_value = 0;
    result = sqlite3_open(PARAM_DB, &db);
    if(result != SQLITE_OK){
        PrintLog(0, "fail to open param.db!\n");
        goto MARK_FAIL1;
    }

    const char*s = "select * from sqlite_master";
    strcpy(sql, s);
    result = sqlite3_exec(db, sql, table_callback, (void *)table_name, &errmsg);
    if(result != SQLITE_OK){
        PrintLog(0, "查询表名称错误!错误码: %d, 错误原因: %s!\n", result, errmsg);
        //sqlite3_free(errmsg);
        goto MARK_FAIL1;
    }
    #if 1
    if(!table_value){
        PrintLog(0, "表(%s)不存在,等待创建...\n", table_name);
        sprintf(sql, "create table %s(ID integer primary key,parauni blob)", table_name);
        result = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
        if(result != SQLITE_OK){
            PrintLog(0, "fail to create %s!错误码: %d, 错误原因: %s!\n", table_name, result, errmsg);
            goto MARK_FAIL1;
        }
        sprintf(sql, "insert into %s(ID,parauni) values(0,?)", table_name);
        result = sqlite3_prepare(db, sql, -1, &stat, 0);
        if(result != SQLITE_OK || stat == NULL){
            PrintLog(0, "fail to insert %s!\n", table_name);
            goto MARK_FAIL1;
        }
        result = sqlite3_bind_blob(stat, 1, (const void *)pdata, (int)pdatalen, NULL);
        if(result != SQLITE_OK){
            PrintLog(0, "fail to bind %s!\n", table_name);
            goto MARK_FAIL1;
        }

        result  = sqlite3_step(stat);
        if(result != SQLITE_DONE){
            PrintLog(0, "fail to step insert!\n");
            goto MARK_FAIL1;
        }
    }
    #endif
    ///
    ///读出二进制数据
    sprintf(sql, "select * from %s", table_name);
    result = sqlite3_prepare(db, sql, -1, &stat, 0);
    if(result != SQLITE_OK){
            PrintLog(0, "fail to select %s!\n", table_name);
            goto MARK_FAIL1;
    }

    while(sqlite3_step(stat) == SQLITE_ROW){
        const void* data = sqlite3_column_blob(stat, 1);
        int              len = sqlite3_column_bytes(stat, 1);
        PrintLog(0, "读表文件中的二进制数据长度: %d(字节)\n", len);
        PrintLog(0, "结构体中的二进制数据长度: %d(字节)\n", pdatalen);
        memcpy((unsigned char *)pdata, (unsigned char *)data, len);

    }
    sqlite3_finalize(stat);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    return 0;
MARK_FAIL1:
    sqlite3_finalize(stat);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    #endif
    return -1;
}
//保存表库文件
// param table_name : 表名
// param qdata: 输出缓存区
// param qdatalen     : 缓存区长度
int SaveToTable(char * table_name, unsigned char * qdata, unsigned int qdatalen)
{
#if 0
    table_value = 0;
    sqlite3 * db = NULL;
    int result;
    char *errmsg = NULL;
    sqlite3_stmt *stat =  NULL;
    char sql[128] = {0};

    result = sqlite3_open(PARAM_DB, &db);
    if(result != SQLITE_OK){
        PrintLog(0, "fail to open param.db!\n");
        goto MARK_FAIL;
    }
    //const char * sql = "select count(*) as c from sqlite_master where type ='table' and name ='Unique_1'";
    const char * s = "select * from sqlite_master";
    strcpy(sql,s);
    result = sqlite3_exec(db, sql, table_callback, (void *)table_name, &errmsg);
    if(result != SQLITE_OK){
        PrintLog(0, "查询表名称错误!错误码: %d, 错误原因: %s!\n", result, errmsg);
        //sqlite3_free(errmsg);
        goto MARK_FAIL;
    }
    ///
    #if 1
    if(!table_value){
        PrintLog(0, "表(%s)不存在,等待创建...\n", table_name);
        sprintf(sql, "create table %s(ID integer primary key,parauni blob)", table_name);
        result = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
        if(result != SQLITE_OK){
            PrintLog(0, "fail to create %s!错误码: %d, 错误原因: %s!\n", table_name, result, errmsg);
            goto MARK_FAIL;
        }
        sprintf(sql, "insert into %s(ID,parauni) values(0,?)", table_name);
        result = sqlite3_prepare(db, sql, -1, &stat, 0);
        if(result != SQLITE_OK || stat == NULL){
            PrintLog(0, "fail to insert %s!\n", table_name);
            goto MARK_FAIL;
        }
        result = sqlite3_bind_blob(stat, 1, (const void *)qdata, (int)qdatalen, NULL);
        if(result != SQLITE_OK){
            PrintLog(0, "fail to bind %s!\n", table_name);
            goto MARK_FAIL;
        }

        result  = sqlite3_step(stat);
        if(result != SQLITE_DONE){
            PrintLog(0, "fail to step insert!\n");
            goto MARK_FAIL;
        }
        PrintLog(0, "success to insert data!\n");
        sqlite3_finalize(stat);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return 0;
    }
    #endif
    sprintf(sql, "update %s set parauni = ? where ID = 0", table_name);
    result = sqlite3_prepare(db, sql, -1, &stat, 0);
    if(result != SQLITE_OK || stat == NULL){
        PrintLog(0, "fail to update %s!\n", table_name);
        goto MARK_FAIL;
    }
    unsigned char arr[100*1024];
    smallcpy(arr, qdata, qdatalen);
    result = sqlite3_bind_blob(stat, 1, (const void *)arr, qdatalen,  0);
    if(result != SQLITE_OK){
        PrintLog(0, "fail to bind %s!\n", table_name);
        goto MARK_FAIL;
    }

    result  = sqlite3_step(stat);
    if(result != SQLITE_DONE){
        PrintLog(0, "fail to step update! result = %d\n", result);
        goto MARK_FAIL;
    }
    PrintLog(0, "success to update data!\n");
    sqlite3_finalize(stat);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    return 0;
MARK_FAIL:
    sqlite3_finalize(stat);
    sqlite3_free(errmsg);
    sqlite3_close(db);
#endif
    return -1;
}



