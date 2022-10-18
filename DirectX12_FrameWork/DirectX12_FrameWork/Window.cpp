/*****************************************************************//**
 * \file   Window.cpp
 * \brief  ウィンドウ関連クラス
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/

#include "Window.h"
#include "WindowCallback.h"


 /**************************************************//**
  * ￥brief コールバック関数
  * \param hWnd:ウィンドウハンドル
  * \param msg:メッセージ
  * \param wp:Wパラメータ
  * \param lp:Lパラメータ
  * \return 終了状況
  ******************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	// コールバックが設定されていなければ終了
	WindowCallback* callback = Window::Instance()->GetCallback();
	if (callback == nullptr) {
		return DefWindowProc(hWnd, msg, wp, lp);
	}

    switch (msg){
	// キー押下
	case WM_KEYDOWN:
		return callback->OnKeyDown(hWnd, msg, wp, lp);
		break;

    case WM_DESTROY:
		return callback->OnDestroy(hWnd, msg, wp, lp);
        break;

	// ×
	case WM_CLOSE:
		return callback->OnClose(hWnd, msg, wp, lp);
		break;

	default:
        break;
    }

    return  DefWindowProc(hWnd, msg, wp, lp);
}


/**************************************************//**
 * ￥brief　インスタンス取得
 * \return インスタンス
 ******************************************************/
Window* Window::Instance() {
	static Window instance;
	return &instance;
}


/**************************************************//**
 * ￥brief　メッセージ処理
 * \return 正常/異常
 ******************************************************/
bool Window::ExecMessage() {
	// メッセージ確認
	while (PeekMessage(&mMessage, NULL, 0, 0, PM_NOREMOVE))
	{
		// メッセージなし
		if (!(::GetMessage(&mMessage, NULL, 0, 0))) {
			return false;
		}

		TranslateMessage(&mMessage);	// キーメッセージの変換
		DispatchMessage(&mMessage);	// メッセージの送出
	}

	return true;

}

/**************************************************//**
 * ￥brief ウィンドウクラス登録
 * \param hInstance	インスタンスハンドル
 * \param h_ClassName	クラスネーム
 * \param h_Style	ウィンドウスタイル
 * \return 成功/失敗
 ******************************************************/
bool Window::RegisterClass(HINSTANCE	  hInstance, const TCHAR* h_ClassName, unsigned long h_Style) {
    // ウィンドウクラス登録
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
 * ￥brief ウィンドウ情報設定
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
			h_ClassName,					// ウィンドウクラスの名前
			h_Title,						// タイトル
			h_Style,						// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横
			h_Width, h_Height,				// ウィンドウサイズ
			NULL,							// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			h_Instance,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし
	}
	else {
		RECT	rWindow, rClient;

		mHWnd = CreateWindowEx(
			h_ExStyle,
			h_ClassName,					// ウィンドウクラスの名前
			h_Title,						// タイトル
			WS_CAPTION | WS_SYSMENU,		// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横(あとで中央に移動させます)
			h_Width, h_Height,				// ウィンドウサイズ
			HWND_DESKTOP,					// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			h_Instance,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし

		// ウインドウサイズを再計算（Metricsだけでは、フレームデザインでクライアント領域サイズが変わってしまうので）
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

	// コールバック関数設定
	mpCallback = new WindowCallback;
}


/**************************************************//**
 * ￥brief　ウィンドウメッセージを取得
 * \return ウィンドウメッセージ
 ******************************************************/
long Window::GetMessage()const {
	return mMessage.message;
}

/**************************************************//**
 * ￥brief ウィンドウハンドル取得
 * \return ウィンドウハンドル
 ******************************************************/
HWND Window::GetHandle()const{ return mHWnd; };

/**************************************************//**
 * ￥brief　コールバック取得
 * \return コールバック
 ******************************************************/
WindowCallback* Window::GetCallback()const{
	return mpCallback;
}

