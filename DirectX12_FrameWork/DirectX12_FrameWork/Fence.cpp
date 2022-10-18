/*****************************************************************//**
 * \file   Fence.cpp
 * \brief  フェンスクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

 //-----------------------------------------------------------------------------
 // Includes
 //-----------------------------------------------------------------------------
#include "Fence.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Fence::Fence()
:m_pFence(nullptr)
, m_event(nullptr)
, m_counter(0) 
{ /* Do Nothing */ }

//-----------------------------------------------------------------------------
// デストラクタ
//----------------------------------------------------------------------------
Fence::~Fence()
{ Term(); };

bool Fence::Init(ID3D12Device* pDevice) {
	// 引数チェック
	if (pDevice == nullptr) 
	{ return false; }

	// イベントの生成
	m_event = CreateEventEx(nullptr,FALSE,FALSE,EVENT_ALL_ACCESS);
	if (m_event == nullptr)
	{ return false; }

	// フェンスを生成
	auto hr = pDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_pFence.GetAddressOf()));
	if(FAILED(hr))
	{ return false; }

	// カウンタ設定
	m_counter = 1;

	// 正常終了
	return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void Fence::Term() {
	// ハンドルを閉じる
	if (m_event != nullptr) {
		CloseHandle(m_event);
		m_event = nullptr;
	}

	// フェンスオブジェクトの破棄
	m_pFence.Reset();

	// カウンターリセット
	m_counter = 0;
}

//-----------------------------------------------------------------------------
// シグナル状態になるまで指定時間待機
//-----------------------------------------------------------------------------
void Fence::Wait(ID3D12CommandQueue* pQueue, UINT timeout_ms) {
	if (pQueue == nullptr) 
	{ return; }

	const auto fenceValue = m_counter;

	// シグナル処理
	auto hr = pQueue->Signal(m_pFence.Get(),fenceValue);
	if (FAILED(hr)) 
	{ return; }

	//カウンターを増やす
	m_counter++;

	//次のフレームの描画準備がまだならば待機
	if (m_pFence->GetCompletedValue() < fenceValue) {
		// 管理領事にイベントを設定
		auto hr = m_pFence->SetEventOnCompletion( fenceValue, m_event);
		if (FAILED(hr)) 
		{ return; }

		//待機処理
		if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_event, timeout_ms, FALSE))
		{ return;}
	}
}

//-----------------------------------------------------------------------------
//      シグナル状態になるまでずっと待機
//-----------------------------------------------------------------------------
void Fence::Sync(ID3D12CommandQueue* pQueue){
	if (pQueue == nullptr)
	{ return; }

	// シグナル処理.
	auto hr = pQueue->Signal(m_pFence.Get(), m_counter);
	if (FAILED(hr))
	{ return; }

	// 完了時にイベントを設定.
	hr = m_pFence->SetEventOnCompletion(m_counter, m_event);
	if (FAILED(hr))
	{ return; }

	// 待機処理.
	if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_event, INFINITE, FALSE))
	{ return; }

	// カウンターを増やす.
	m_counter++;
}

