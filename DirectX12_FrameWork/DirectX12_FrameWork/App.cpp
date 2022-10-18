#include "App.h"
#include "Window.h"
#include "GraphicsEngine.h"
#include "BaceScene.h"
#include "Actor.h"


#include <Windows.h>
#include <Crtdbg.h>
#include <chrono>
#include <thread>
#include <time.h>
#include "App.h"
#include "Window.h"
#include "GraphicsEngine.h"

#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"dxgi.lib")
#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"directxtex.lib")
#pragma comment (lib,"d3dcompiler.lib")
#pragma comment (lib,"dxguid.lib")

//-----------------------------------------------------------------------------
// �X�^�e�B�b�N�@�����o�[
//-----------------------------------------------------------------------------
const TCHAR* Application::WINDOW_TITLE = L"DirectX12";
const TCHAR* Application::WINDOW_CLASS_NAME = L"win32app";

const uint32_t Application::WINDOW_STYLE_WINDOWED = (WS_VISIBLE | WS_CAPTION | WS_SYSMENU);
const uint32_t Application::WINDOW_EX_STYLE_WINDOWED = (0);
const uint32_t Application::WINDOW_STYLE_FULL_SCREEN = (WS_VISIBLE | WS_POPUP);
const uint32_t Application::WINDOW_EX_STYLE_FULL_SCREEN = (0);

const uint32_t Application::CLIENT_WIDTH = 1280;
const uint32_t Application::CLIENT_HEIGHT = 720;

uint32_t	Application::SYSTEM_WIDTH = 0;
uint32_t	Application::SYSTEM_HEIGHT = 0;

const float	Application::FPS = 60;

/**************************************************//**
 * ��brief�@�R���X�g���N�^
 ******************************************************/
Application::Application() :mSystemCounter(0) {}


/**************************************************//**
 * ��brief�@�f�X�g���N�^
 ******************************************************/
Application::~Application() {
	Dispose();
}

/**************************************************//**
 * ��brief �C���X�^���X�擾(singleton)
 * \return �C���X�^���X�A�h���X
 ******************************************************/
Application* Application::Instance() {
	static Application instance;
	return &instance;
}


/**************************************************//**
 * ��brief �V�X�e���̈�̕��ƍ����̏�����
 ******************************************************/
void Application::InitSystemWH() {

}

/**************************************************//**
 * ��brief	����������
 * \param hInstance �C���X�^���X�n���h��
 * \return ����/���s
 ******************************************************/
bool Application::Init(HINSTANCE hInstance) {
	// �������[���[�N�����o
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//// �R���\�[�������蓖�Ă�
	//AllocConsole();
	//
	//// �W���o�͂̊��蓖��
	//freopen_s(&mfp, "CON", "w", stdout);
	//
	//// ���ƍ���������
	//InitSystemWH();

	//�E�B���h�E�N���X�̃C���X�^���X�擾
	mWndInstance = Window::Instance();

	//�E�B���h�E�N���X�o�^
	mWndInstance->RegisterClass(hInstance, WINDOW_CLASS_NAME, CS_OWNDC);

	//�E�B���h�E���Z�b�g
	mWndInstance->SetWindow(hInstance,
		WINDOW_STYLE_WINDOWED,
		WINDOW_EX_STYLE_WINDOWED,
		NULL,
		CLIENT_WIDTH + SYSTEM_WIDTH,
		CLIENT_HEIGHT + SYSTEM_HEIGHT,
		WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		false);

	//�E�B���h�E�n���h���擾
	mHWnd = mWndInstance->GetHandle();

	//�C���X�^���X�n���h���i�[
	mHInst = hInstance;

	//DirectX12������
	GraphicsEngine::GetInstance()->Init(mHWnd, CLIENT_WIDTH, CLIENT_HEIGHT);

	return true;
}

/**************************************************//**
 * ��brief �I������
 ******************************************************/
void Application::Dispose() {
	//// �W���o�̓N���[�Y
	//fclose(m_fp);
	//// �R���\�[���J��
	//::FreeConsole();
}

/**************************************************//**
 * ��brief�@���C�����[�v
 * \return ���b�Z�[�WID
 ******************************************************/
unsigned long Application::MainLoop(){
	//���b�Z�[�WID�i�[�p
	MSG		msg;
	ZeroMemory(&msg, sizeof(msg));

	clock_t current_time = 0;	//�A�v���P�[�V�����J�n����̌��݂̌o�ߎ���
	clock_t last_time = 0;			//�A�v���P�[�V�����J�n����̑O��t���[���̊J�n����



	while (mWndInstance->ExecMessage()) {

		//�����ŃQ�[���̍X�V����
		GraphicsEngine::GetInstance()->BeginRender();
		
		GraphicsEngine::GetInstance()->EndRender();

	}

	//�����ŃQ�[���̏I��������������^^

	return mWndInstance->GetMessage();
}

/**************************************************//**
 * ��brief	�E�B���h�E�n���h���擾
 * \return �E�B���h�E�n���h��
 ******************************************************/
HWND Application::GetHWnd() { return mHWnd; }

/**************************************************//**
 * ��brief �C���X�^���X�n���h���擾
 * \return �C���X�^���X�n���h��
 ******************************************************/
HINSTANCE Application::GetHInst() { return mHInst; }
