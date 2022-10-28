/*****************************************************************//**
 * \file   Logger.cpp
 * \brief  ログ
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdio>
#include <cstdarg>
#include <Windows.h>


//-----------------------------------------------------------------------------
//      ログ出力
//-----------------------------------------------------------------------------
void OutputLog(const char* format, ...){
    char msg[2048];
    memset(msg, '\0', sizeof(msg));
    va_list arg;

    va_start(arg, format);
    vsprintf_s(msg, format, arg);
    va_end(arg);

    // コンソールに出力.
    printf_s("%s", msg);

    // Visual Studioの出力ウィンドウにも表示.
    OutputDebugStringA(msg);
}