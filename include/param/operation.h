/**
* operation.h -- 参数操作头文件
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-7
* 最后修改时间: 2009-5-7
*/

#ifndef _PARAM_OPERATION_H
#define _PARAM_OPERATION_H

#define POERR_OK            0
#define POERR_INVALID        1
#define POERR_FATAL        2

void ClearSaveParamFlag(void);
void SetSaveParamFlag(unsigned int flag);
void SaveParam(void);

#endif /*_PARAM_OPERATION_H*/
