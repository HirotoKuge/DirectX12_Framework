#include "App.h"

INT APIENTRY WinMain(HINSTANCE  h_hInst,HINSTANCE  h_hPrevInst,LPSTR h_lpszArgs,int h_nWinMode){
	
	// �A�v���P�[�V������������
	Application* App = Application::Instance();		// �C���X�^���X�擾
	App->Init(h_hInst);

	// �E�C���h�E��\������
	ShowWindow(App->GetHWnd(), h_nWinMode);
	UpdateWindow(App->GetHWnd());

	// ���C�����[�v
	long ret = App->MainLoop();

	// �A�v���P�[�V�����I������
	App->Dispose();
	
	
	return 0;
}
