/*****************************************************************//**
 * \file   WindowCallback.h
 * \brief  �R�[���o�b�N�N���X
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/
#pragma once

#include<Windows.h>

class WindowCallback{
public:

	virtual LRESULT OnKeyDown(HWND hHandle, UINT hMessage, WPARAM hWParam, LPARAM hLParam);
	virtual LRESULT OnClose(HWND hHandle, UINT hMessage, WPARAM hWParam, LPARAM hLParam);
	virtual LRESULT OnDestroy(HWND hHandle, UINT hMessage, WPARAM hWParam, LPARAM hLParam);


};

