/**
* operation_inner.h -- 参数操作头文件(内部使用)
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-7
* 最后修改时间: 2009-5-7
*/

#ifndef _PARAM_OPERATION_INNER_H
#define _PARAM_OPERATION_INNER_H

int SaveParaTerm(void);
int LoadParaTerm(void);
int LoadParaUni(void);
int LoadParaDataUse(void);
int LoadParaTimerTask(void);


void SetSaveParamFlag(unsigned int flag);
void SaveParam(void);


#endif /*_PARAM_OPERATION_INNER_H*/

