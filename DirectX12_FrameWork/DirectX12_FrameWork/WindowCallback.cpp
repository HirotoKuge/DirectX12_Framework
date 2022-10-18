/*****************************************************************//**
 * \file   WindowCallback.h
 * \brief  �R�[���o�b�N�N���X
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/

#include "WindowCallback.h"

LRESULT WindowCallback::OnKeyDown(HWND hWnd,UINT hMessage,WPARAM hWParam,LPARAM hLParam){
	//�����ꂽ�L�[���m�F
	switch(hWParam) {
	//ESC�L�[�������ꂽ��I������
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



