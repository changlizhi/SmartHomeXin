/**
* file.h -- 文件操作接口头文件
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-3-31
* 最后修改时间: 2009-3-31
*/

#ifndef _SYS_FILE_H
#define _SYS_FILE_H

#include <stdio.h>

int SysGetFileSize(FILE *pf);
void SysRemoveOneFile(const char *file);
int SysRemoveFiles(const char *files);

#endif /*_SYS_FILE_H*/
