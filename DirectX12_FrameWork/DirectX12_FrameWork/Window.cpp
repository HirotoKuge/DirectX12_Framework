/*****************************************************************//**
 * \file   Window.cpp
 * \brief  �E�B���h�E�֘A�N���X
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/

#include "Window.h"
#include "WindowCallback.h"


 /**************************************************//**
  * ��brief �R�[���o�b�N�֐�
  * \param hWnd:�E�B���h�E�n���h��
  * \param msg:���b�Z�[�W
  * \param wp:W�p�����[�^
  * \param lp:L�p�����[�^
  * \return �I����
  ******************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	// �R�[���o�b�N���ݒ肳��Ă��Ȃ���ΏI��
	WindowCallback* callback = Window::Instance()->GetCallback();
	if (callback == nullptr) {
		return DefWindowProc(hWnd, msg, wp, lp);
	}

    switch (msg){
	// �L�[����
	case WM_KEYDOWN:
		return callback->OnKeyDown(hWnd, msg, wp, lp);
		break;

    case WM_DESTROY:
		return callback->OnDestroy(hWnd, msg, wp, lp);
        break;

	// �~
	case WM_CLOSE:
		return callback->OnClose(hWnd, msg, wp, lp);
		break;

	default:
        break;
    }

    return  DefWindowProc(hWnd, msg, wp, lp);
}


/**************************************************//**
 * ��brief�@�C���X�^���X�擾
 * \return �C���X�^���X
 ******************************************************/
Window* Window::Instance() {
	static Window instance;
	return &instance;
}


/**************************************************//**
 * ��brief�@���b�Z�[�W����
 * \return ����/�ُ�
 ******************************************************/
bool Window::ExecMessage() {
	// ���b�Z�[�W�m�F
	while (PeekMessage(&mMessage, NULL, 0, 0, PM_NOREMOVE))
	{
		// ���b�Z�[�W�Ȃ�
		if (!(::GetMessage(&mMessage, NULL, 0, 0))) {
			return false;
		}

		TranslateMessage(&mMessage);	// �L�[���b�Z�[�W�̕ϊ�
		DispatchMessage(&mMessage);	// ���b�Z�[�W�̑��o
	}

	return true;

}

/**************************************************//**
 * ��brief �E�B���h�E�N���X�o�^
 * \param hInstance	�C���X�^���X�n���h��
 * \param h_ClassName	�N���X�l�[��
 * \param h_Style	�E�B���h�E�X�^�C��
 * \return ����/���s
 ******************************************************/
bool Window::RegisterClass(HINSTANCE	  hInstance, const TCHAR* h_ClassName, unsigned long h_Style) {
    // �E�B���h�E�N���X�o�^
    WNDCLASSEX WndClassEx;
    WndClassEx.cbSize = sizeof(WNDCLASSEX);
    WndClassEx.style = h_Style;
    WndClassEx.lpfnWndProc = WndProc;
    WndClassEx.cbClsExtra = 0L;
    WndClassEx.cbWndExtra = 0L;
    WndClassEx.hInstance = hInstance;
    WndClassEx.hIcon = nullptr;
    WndClassEx.hCursor = nullptr;
    WndClassEx.hbrBackground = nullptr;
    WndClassEx.lpszMenuName = nullptr;
    WndClassEx.lpszClassName = h_ClassName;
    WndClassEx.hIconSm = NULL;

    if (!RegisterClassEx(&WndClassEx)){
        MessageBox(NULL, L"RegisterClassEx", L"Error!", MB_OK);
        return false;
    }

    return true;
}

/**************************************************//**
 * ��brief �E�B���h�E���ݒ�
 * \param h_Instance
 * \param h_Style
 * \param h_ExStyle
 * \param h_Parent
 * \param h_Width
 * \param h_Height
 * \param h_ClassName
 * \param h_Title
 * \param FULLSCREEN
 ******************************************************/
void Window::SetWindow(HINSTANCE		h_Instance,
    unsigned long	h_Style,
    unsigned long	h_ExStyle,
    HWND			h_Parent,
    long			h_Width,
    long			h_Height,
    const TCHAR* h_ClassName,
    const TCHAR* h_Title,
    bool			FULLSCREEN) {


	if (FULLSCREEN) {
		mHWnd = CreateWindowEx(
			h_ExStyle,
			h_ClassName,					// �E�B���h�E�N���X�̖��O
			h_Title,						// �^�C�g��
			h_Style,						// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��
			h_Width, h_Height,				// �E�B���h�E�T�C�Y
			NULL,							// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			h_Instance,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�
	}
	else {
		RECT	rWindow, rClient;

		mHWnd = CreateWindowEx(
			h_ExStyle,
			h_ClassName,					// �E�B���h�E�N���X�̖��O
			h_Title,						// �^�C�g��
			WS_CAPTION | WS_SYSMENU,		// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��(���ƂŒ����Ɉړ������܂�)
			h_Width, h_Height,				// �E�B���h�E�T�C�Y
			HWND_DESKTOP,					// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			h_Instance,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�

		// �E�C���h�E�T�C�Y���Čv�Z�iMetrics�����ł́A�t���[���f�U�C���ŃN���C�A���g�̈�T�C�Y���ς���Ă��܂��̂Łj
		GetWindowRect(mHWnd, &rWindow);
		GetClientRect(mHWnd, &rClient);
		int width = (rWindow.right - rWindow.left) - (rClient.right - rClient.left) + h_Width;
		int height = (rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top) + h_Height;
		SetWindowPos(
			mHWnd,
			NULL,
			GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2,
			width - 1,
			height - 1,
			SWP_NOZORDER);
	}

	if (!mHWnd) {
		MessageBox(nullptr, L"CreateWindow Error", L"error", MB_OK);
	}

	// �R�[���o�b�N�֐��ݒ�
	mpCallback = new WindowCallback;
}


/**************************************************//**
 * ��brief�@�E�B���h�E���b�Z�[�W���擾
 * \return �E�B���h�E���b�Z�[�W
 ******************************************************/
long Window::GetMessage()const {
	return mMessage.message;
}

/**************************************************//**
 * ��brief �E�B���h�E�n���h���擾
 * \return �E�B���h�E�n���h��
 ******************************************************/
HWND Window::GetHandle()const{ return mHWnd; };

/**************************************************//**
 * ��brief�@�R�[���o�b�N�擾
 * \return �R�[���o�b�N
 ******************************************************/
WindowCallback* Window::GetCallback()const{
	return mpCallback;
}

