/*****************************************************************//**
 * \file   WindowCallback.h
 * \brief  コールバッククラス
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/

#include "WindowCallback.h"

LRESULT WindowCallback::OnKeyDown(HWND hWnd,UINT hMessage,WPARAM hWParam,LPARAM hLParam){
	//押されたキーを確認
	switch(hWParam) {
	//ESCキーが押されたら終了処理
	case VK_ESCAPE:
		DestroyWindow(hWnd);
	default:
		return DefWindowProc(hWnd, hMessage, hWParam, hLParam);
	}
	return 0;
}


LRESULT WindowCallback::OnClose(HWND hWnd, UINT hMessage, WPARAM hWParam, LPARAM hLParam) {
	PostQuitMessage(0);
	return 0;
}

LRESULT WindowCallback::OnDestroy(HWND hWnd, UINT hMessage, WPARAM hWParam, LPARAM hLParam) {
	PostQuitMessage(0);
	return 0;
}



