/*****************************************************************//**
 * \file   Logger.h
 * \brief  ���O
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

/*****************************************************************//**
 * \brief�@���O���o��
 * 
 * \param format �t�H�[�}�b�g
 *********************************************************************/
void OutputLog(const char* format, ...);

#ifndef DLOG
#if defined(DEBUG) || defined(_DEBUG)
#define DLOG( x, ... ) OutputLog( x "\n", ##__VA_ARGS__ );
#else
#define DLOG( x, ... ) 
#endif
#endif//DLOG

#ifndef ELOG
#define ELOG( x, ... ) OutputLog( "[File : %s, Line : %d] " x "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG