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
// スタティック　メンバー
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
 * ￥brief　コンストラクタ
 ******************************************************/
Application::Application() :mSystemCounter(0) {}


/**************************************************//**
 * ￥brief　デストラクタ
 ******************************************************/
Application::~Application() {
	Dispose();
}

/**************************************************//**
 * ￥brief インスタンス取得(singleton)
 * \return インスタンスアドレス
 ******************************************************/
Application* Application::Instance() {
	static Application instance;
	return &instance;
}


/**************************************************//**
 * ￥brief システム領域の幅と高さの初期化
 ******************************************************/
void Application::InitSystemWH() {

}

/**************************************************//**
 * ￥brief	初期化処理
 * \param hInstance インスタンスハンドル
 * \return 成功/失敗
 ******************************************************/
bool Application::Init(HINSTANCE hInstance) {
	// メモリーリークを検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//// コンソールを割り当てる
	//AllocConsole();
	//
	//// 標準出力の割り当て
	//freopen_s(&mfp, "CON", "w", stdout);
	//
	//// 幅と高さ初期化
	//InitSystemWH();

	//ウィンドウクラスのインスタンス取得
	mWndInstance = Window::Instance();

	//ウィンドウクラス登録
	mWndInstance->RegisterClass(hInstance, WINDOW_CLASS_NAME, CS_OWNDC);

	//ウィンドウ情報セット
	mWndInstance->SetWindow(hInstance,
		WINDOW_STYLE_WINDOWED,
		WINDOW_EX_STYLE_WINDOWED,
		NULL,
		CLIENT_WIDTH + SYSTEM_WIDTH,
		CLIENT_HEIGHT + SYSTEM_HEIGHT,
		WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		false);

	//ウィンドウハンドル取得
	mHWnd = mWndInstance->GetHandle();

	//インスタンスハンドル格納
	mHInst = hInstance;

	//DirectX12初期化
	GraphicsEngine::GetInstance()->Init(mHWnd, CLIENT_WIDTH, CLIENT_HEIGHT);

	return true;
}

/**************************************************//**
 * ￥brief 終了処理
 ******************************************************/
void Application::Dispose() {
	//// 標準出力クローズ
	//fclose(m_fp);
	//// コンソール開放
	//::FreeConsole();
}

/**************************************************//**
 * ￥brief　メインループ
 * \return メッセージID
 ******************************************************/
unsigned long Application::MainLoop(){
	//メッセージID格納用
	MSG		msg;
	ZeroMemory(&msg, sizeof(msg));

	clock_t current_time = 0;	//アプリケーション開始からの現在の経過時間
	clock_t last_time = 0;			//アプリケーション開始からの前回フレームの開始時間



	while (mWndInstance->ExecMessage()) {

		//ここでゲームの更新処理
		GraphicsEngine::GetInstance()->BeginRender();
		
		GraphicsEngine::GetInstance()->EndRender();

	}

	//ここでゲームの終了処理をするんよ^^

	return mWndInstance->GetMessage();
}

/**************************************************//**
 * ￥brief	ウィンドウハンドル取得
 * \return ウィンドウハンドル
 ******************************************************/
HWND Application::GetHWnd() { return mHWnd; }

/**************************************************//**
 * ￥brief インスタンスハンドル取得
 * \return インスタンスハンドル
 ******************************************************/
HINSTANCE Application::GetHInst() { return mHInst; }
