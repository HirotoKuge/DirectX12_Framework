/*****************************************************************//**
 * \file   GraphicsEngine.cpp
 * \brief  描画エンジン
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#include "GraphicsEngine.h"
#include <d3d12.h>
#include <stdio.h>
#include <Windows.h>
#include <debugapi.h>
#include "SharedStruct.h"
#include "Logger.h"



//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GraphicsEngine::GraphicsEngine()
    :m_pDevice(nullptr)
    ,m_pQueue(nullptr)
    ,m_pSwapChain(nullptr)
    , m_pPool{ nullptr }
{ /* Do Nothing */ }


#pragma region DirectX12Create
//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool GraphicsEngine::Init(HWND hwnd, UINT windowWidth, UINT windowHeight){
	m_Width = windowWidth;
	m_Height = windowHeight;
	m_hWnd = hwnd;

#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debug;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

		// デバッグレイヤーを有効化.
		if (SUCCEEDED(hr))
		{
			debug->EnableDebugLayer();
		}
	}
#endif

#if defined(DEBUG) || defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> pDebug;
        auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            pDebug->EnableDebugLayer();
        }
    }
#endif

    HRESULT hr;

    // デバイスの生成
    {
        //デバイスの初期化
        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        //生成可能なデバイスレベルを調べる
        for (auto lv : levels) {
            hr = D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf()));
            if (hr == S_OK) {
                m_featureLevel = lv;
                break;
            }
        }
        if(FAILED(hr))
        { return false; }
    }
    
    

    // コマンドキューの生成.
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        auto hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
        if (FAILED(hr))
        { return false; }
    }

    // スワップチェインの生成.
    {
        // DXGIファクトリーの生成.
        ComPtr<IDXGIFactory4> pFactory;
        hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
        if (FAILED(hr))
        { return false; }

        // スワップチェインの設定.
        DXGI_SWAP_CHAIN_DESC desc = {};
        desc.BufferDesc.Width                    = m_Width;
        desc.BufferDesc.Height                   = m_Height;
        desc.BufferDesc.RefreshRate.Numerator    = 60;
        desc.BufferDesc.RefreshRate.Denominator  = 1;
        desc.BufferDesc.ScanlineOrdering         = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        desc.BufferDesc.Scaling                  = DXGI_MODE_SCALING_UNSPECIFIED;
        desc.BufferDesc.Format                   = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count                    = 1;
        desc.SampleDesc.Quality                  = 0;
        desc.BufferUsage                         = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount                         = FrameCount;
        desc.OutputWindow                        = m_hWnd;
        desc.Windowed                            = TRUE;
        desc.SwapEffect                          = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.Flags                               = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // スワップチェインの生成.
        ComPtr<IDXGISwapChain> pSwapChain;
        hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, pSwapChain.GetAddressOf());
        if (FAILED(hr))
        { return false; }

        // IDXGISwapChain3 を取得.
        hr = pSwapChain.As(&m_pSwapChain);
        if (FAILED(hr))
        { return false; }

        // バックバッファ番号を取得.
        m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

        // 不要になったので解放.
        pFactory.Reset();
        pSwapChain.Reset();
    }

    // ディスクリプタプールの生成 ( ディスクリプタを種類ごとにまとめとくやつみたいなイメージ )
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};

        desc.NodeMask        = 1;
        desc.Type            = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors  = 512;
        desc.Flags           = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (!DescriptorPool::Create(m_pDevice.Get(), &desc, &m_pPool[POOL_TYPE_RES]))
        { return false; }

        desc.Type            = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        desc.NumDescriptors  = 256;
        desc.Flags           = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (!DescriptorPool::Create(m_pDevice.Get(), &desc, &m_pPool[POOL_TYPE_SMP]))
        { return false; }

        desc.Type            = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors  = 512;
        desc.Flags           = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (!DescriptorPool::Create(m_pDevice.Get(), &desc, &m_pPool[POOL_TYPE_RTV]))
        { return false; }

        desc.Type            = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        desc.NumDescriptors  = 512;
        desc.Flags           = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (!DescriptorPool::Create(m_pDevice.Get(), &desc, &m_pPool[POOL_TYPE_DSV]))
        { return false; }
    }

    // コマンドリストの生成.
    {
        if (!m_CommandList.Init(
            m_pDevice.Get(),
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            FrameCount))
        { return false; }
    }

    // フレームバッファ用のレンダーターゲットビューの生成.
    {
        for (auto i = 0u; i < FrameCount; ++i){
            if (!m_ColorTarget[i].InitFromBackBuffer(
                m_pDevice.Get(),
                m_pPool[POOL_TYPE_RTV],
                i,
                m_pSwapChain.Get()))
            { return false; }
        }
    }

    // フレームバッファ用の深度ステンシルビューの生成
    {
        if (!m_DepthTarget.Init(
            m_pDevice.Get(),
            m_pPool[POOL_TYPE_DSV],
            m_Width,
            m_Height,
            DXGI_FORMAT_D32_FLOAT))
        { return false; }
    }

    // フェンスの生成.
    if (!m_Fence.Init(m_pDevice.Get()))
    { return false; }

    // ビューポートの設定.
    {
        m_Viewport.TopLeftX  = 0.0f;
        m_Viewport.TopLeftY  = 0.0f;
        m_Viewport.Width     = float(m_Width);
        m_Viewport.Height    = float(m_Height);
        m_Viewport.MinDepth  = 0.0f;
        m_Viewport.MaxDepth  = 1.0f;
    }

    // シザー矩形の設定.
    {
        m_Scissor.left   = 0;
        m_Scissor.right  = m_Width;
        m_Scissor.top    = 0;
        m_Scissor.bottom = m_Height;
    }

    // 正常終了.
    return true;
	

	DLOG("描画エンジンの初期化に成功");

	return true;
}
#pragma endregion

#pragma region DirectX12Render

void GraphicsEngine::BeginRender(){
    // コマンドリストの記録を開始.
    m_pNowCmdList = m_CommandList.GetListReset();

    // 書き込み用リソースバリア設定 描画モードから書き込みモードに切り替える
    D3D12_RESOURCE_BARRIER barriarDesc = {};
    barriarDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriarDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barriarDesc.Transition.pResource = m_ColorTarget[m_frameIndex].GetResource();
    barriarDesc.Transition.Subresource = 0;
    barriarDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barriarDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    m_pNowCmdList->ResourceBarrier(1, &barriarDesc);

    // ビューポートとシザー矩形を設定
    m_pNowCmdList->RSSetViewports(1,&m_Viewport);
    m_pNowCmdList->RSSetScissorRects(1,&m_Scissor);
   
    // フレームバッファ用のディスクリプタ取得
    auto handleRTV = m_ColorTarget[m_frameIndex].GetHandleRTV();
    auto handleDSV = m_DepthTarget.GetHandleDSV();

    // レンダーターゲットをフレームバッファに設定
    // TODO:MRTに対応させるためにレンダーターゲットクラスとレンダーコンテキストクラスを作る
    m_pNowCmdList->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

    // クリアカラー.
    float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

    // レンダーターゲットをクリア.
    m_pNowCmdList->ClearRenderTargetView(handleRTV->HandleCPU, clearColor, 0, nullptr);

    // 深度ステンシルビューをクリア.
    m_pNowCmdList->ClearDepthStencilView(handleDSV->HandleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
}

void GraphicsEngine::EndRender(){
    
    // 書き込み終了用リソースバリア設定
    D3D12_RESOURCE_BARRIER barriarDesc = {};
    barriarDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriarDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barriarDesc.Transition.pResource = m_ColorTarget[m_frameIndex].GetResource();
    barriarDesc.Transition.Subresource = 0;
    barriarDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barriarDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    m_pNowCmdList->ResourceBarrier(1, &barriarDesc);

    // コマンドリストの記録を終了.
    m_pNowCmdList->Close();

    // コマンドリストを実行.
    ID3D12CommandList* pLists[] = { m_pNowCmdList };
    m_pQueue->ExecuteCommandLists(1, pLists);

    // 画面に表示.
    m_pSwapChain->Present(1, 0);

    // 完了待ち.
    m_Fence.Wait(m_pQueue.Get(), INFINITE);

    // フレーム番号を更新.
    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

#pragma endregion


#pragma region DirextX12Getter
//-----------------------------------------------------------------------------
// デバイス取得
//-----------------------------------------------------------------------------
ID3D12Device* GraphicsEngine::GetDevice()const {
    return m_pDevice.Get();
}

//-----------------------------------------------------------------------------
// フレーム番号取得
//-----------------------------------------------------------------------------
uint32_t GraphicsEngine::GetCurrentFrameIndex()const {
    return m_frameIndex;
}

//-----------------------------------------------------------------------------
// コマンドリスト取得
//-----------------------------------------------------------------------------
ID3D12GraphicsCommandList* GraphicsEngine::GetCommandList()const {
    return m_pNowCmdList;
}


//-----------------------------------------------------------------------------
// ディスクリプタプールを取得
//-----------------------------------------------------------------------------
DescriptorPool* GraphicsEngine::GetDiscriptaPool(POOL_TYPE poolType)const {
    return m_pPool[poolType];
}


#pragma endregion

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void GraphicsEngine::Term() {
    // GPU処理の完了を待機.
    m_Fence.Sync(m_pQueue.Get());

    // フェンス破棄.
    m_Fence.Term();

    // レンダーターゲットビューの破棄.
    for (auto i = 0u; i < FrameCount; i++){
        m_ColorTarget[i].Term();
    }

    // 深度ステンシルビューの破棄.
    m_DepthTarget.Term();

    // コマンドリストの破棄.
    m_CommandList.Term();

    for (auto i = 0; i < POOL_COUNT; i++){
        if (m_pPool[i] != nullptr){
            m_pPool[i]->Release();
            m_pPool[i] = nullptr;
        }
    }

    // スワップチェインの破棄.
    m_pSwapChain.Reset();

    // コマンドキューの破棄.
    m_pQueue.Reset();

    // デバイスの破棄.
    m_pDevice.Reset();
}

void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
}