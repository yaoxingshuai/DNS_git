/*************************************************************************
    > File Name: functions.h
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Fri 17 Feb 2017 07:18:20 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <string>

//当asr传来aid时，处理
void onRcvfromAsr(int sock, short ievent, void *arg);

void onRcvfromLms(int sock, short ievent, void *arg);
void onRcvfromDms(int sock, short ievent, void *arg);
void onRcvfromRms(int sock, short ievent, void *arg);

//如果rms,dms,lms向lms回复 <aid,"None"> 表示该aid对应的映射关系 没有服务器管理时
//lms也应该向发起该aid查询的asr返回 <aid,"None">
void recurNone(const std::string &aidRequest);
#endif
