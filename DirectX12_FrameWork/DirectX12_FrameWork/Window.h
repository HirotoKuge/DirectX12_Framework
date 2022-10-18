#pragma once
/*****************************************************************//**
 * \file   Window.h
 * \brief  �E�B���h�E�֘A�N���X
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/

#include<Windows.h>

class WindowCallback;

class Window{
public:
	static Window* Instance();


	bool ExecMessage();

	bool RegisterClass(HINSTANCE	  hInstance,
		const TCHAR* h_ClassName,
		unsigned long h_Style);


	void SetWindow(HINSTANCE		h_Instance,
		unsigned long	h_Style,
		unsigned long	h_ExStyle,
		HWND			h_Parent,
		long			h_Width,
		long			h_Height,
		const TCHAR* h_ClassName,
		const TCHAR* h_Title,
		bool			FULLSCREEN);


	long GetMessage() const;
	HWND GetHandle() const;
	WindowCallback* GetCallback()const;

private:
	//�E�B���h�E�T�C�Y
	
	MSG	 mMessage;
	HWND mHWnd = NULL;	//�E�B���h�E�n���h��
	WindowCallback* mpCallback;


	//�E�B���h�E��
	TCHAR* appName;

private:
	Window(){};
	virtual ~Window(){};
	Window(const Window&);
	Window& operator = (const Window&) {}
};

