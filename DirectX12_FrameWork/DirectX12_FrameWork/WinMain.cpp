#include "App.h"

INT APIENTRY WinMain(HINSTANCE  h_hInst,HINSTANCE  h_hPrevInst,LPSTR h_lpszArgs,int h_nWinMode){
	
	// アプリケーション初期処理
	Application* App = Application::Instance();		// インスタンス取得
	App->Init(h_hInst);

	// ウインドウを表示する
	ShowWindow(App->GetHWnd(), h_nWinMode);
	UpdateWindow(App->GetHWnd());

	// メインループ
	long ret = App->MainLoop();

	// アプリケーション終了処理
	App->Dispose();
	
	
	return 0;
}
