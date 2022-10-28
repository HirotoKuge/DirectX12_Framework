/*****************************************************************//**
 * \file   Logger.cpp
 * \brief  ���O
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
//      ���O�o��
//-----------------------------------------------------------------------------
void OutputLog(const char* format, ...){
    char msg[2048];
    memset(msg, '\0', sizeof(msg));
    va_list arg;

    va_start(arg, format);
    vsprintf_s(msg, format, arg);
    va_end(arg);

    // �R���\�[���ɏo��.
    printf_s("%s", msg);

    // Visual Studio�̏o�̓E�B���h�E�ɂ��\��.
    OutputDebugStringA(msg);
}