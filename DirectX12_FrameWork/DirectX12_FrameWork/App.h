#pragma once
#include <Windows.h>
#include <stdio.h>
#include <cinttypes>
#include <wrl/client.h>
#include <memory>

#include "Window.h"

class GraphicsEngine;

class Application {
public:
	static const TCHAR* WINDOW_TITLE;			// �E�B���h�E�̃^�C�g��
	static const TCHAR* WINDOW_CLASS_NAME;		// = "win32app";

	static const uint32_t		WINDOW_STYLE_WINDOWED;			// = (WS_VISIBLE | WS_CAPTION | WS_SYSMENU);
	static const uint32_t		WINDOW_EX_STYLE_WINDOWED;		// = (0);
	static const uint32_t		WINDOW_STYLE_FULL_SCREEN;		// = (WS_VISIBLE | WS_POPUP);
	static const uint32_t		WINDOW_EX_STYLE_FULL_SCREEN;	// = (0);

	static const uint32_t		CLIENT_WIDTH;	// �N���C�A���g�̈� ��
	static const uint32_t		CLIENT_HEIGHT;	// �N���C�A���g�̈� ����

	static uint32_t				SYSTEM_WIDTH;	// = 0;
	static uint32_t				SYSTEM_HEIGHT;	// = 0;

	static const float			FPS;// = 60;

private:
	HWND		mHWnd;				// Window�n���h��
	HINSTANCE	mHInst;				// �C���X�^���X�n���h��
	uint32_t	mSystemCounter;		// �V�X�e���J�E���^

	FILE* mfp;		// �f�o�b�O�p�R���\�[��

	Window* mWndInstance;

private:
	Application();									// �R���X�g���N�^
	Application(const Application&);				// �R�s�[
	Application& operator = (const Application&) {}	// =
	
	std::unique_ptr<GraphicsEngine> m_pGraphicsEngine;
	
public:
	virtual ~Application();		// �f�X�g���N�^
	static void InitSystemWH();	// �V�X�e��������������

	bool Init(HINSTANCE hInstance);		// �V�X�e���L����
	void Dispose();						// �V�X�e��������
	unsigned long MainLoop();			// ���C�����[�v

	// �����o�擾�֐�
	static Application* Instance();		// �C���X�^���X�擾
	HWND				GetHWnd();		// �E�B���h�E�n���h��
	HINSTANCE			GetHInst();		// �C���X�^���X�n���h��
};
