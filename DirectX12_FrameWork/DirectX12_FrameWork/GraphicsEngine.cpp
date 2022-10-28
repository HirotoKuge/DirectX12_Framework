/*****************************************************************//**
 * \file   GraphicsEngine.cpp
 * \brief  �`��G���W��
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
// �R���X�g���N�^
//-----------------------------------------------------------------------------
GraphicsEngine::GraphicsEngine()
    :m_pDevice(nullptr)
    ,m_pQueue(nullptr)
    ,m_pSwapChain(nullptr)
    , m_pPool{ nullptr }
{ /* Do Nothing */ }


#pragma region DirectX12Create
//-----------------------------------------------------------------------------
// ����������
//-----------------------------------------------------------------------------
bool GraphicsEngine::Init(HWND hwnd, UINT windowWidth, UINT windowHeight){
	m_Width = windowWidth;
	m_Height = windowHeight;
	m_hWnd = hwnd;

#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debug;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

		// �f�o�b�O���C���[��L����.
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

    // �f�o�C�X�̐���
    {
        //�f�o�C�X�̏�����
        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        //�����\�ȃf�o�C�X���x���𒲂ׂ�
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
    
    

    // �R�}���h�L���[�̐���.
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

    // �X���b�v�`�F�C���̐���.
    {
        // DXGI�t�@�N�g���[�̐���.
        ComPtr<IDXGIFactory4> pFactory;
        hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
        if (FAILED(hr))
        { return false; }

        // �X���b�v�`�F�C���̐ݒ�.
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

        // �X���b�v�`�F�C���̐���.
        ComPtr<IDXGISwapChain> pSwapChain;
        hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, pSwapChain.GetAddressOf());
        if (FAILED(hr))
        { return false; }

        // IDXGISwapChain3 ���擾.
        hr = pSwapChain.As(&m_pSwapChain);
        if (FAILED(hr))
        { return false; }

        // �o�b�N�o�b�t�@�ԍ����擾.
        m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

        // �s�v�ɂȂ����̂ŉ��.
        pFactory.Reset();
        pSwapChain.Reset();
    }

    // �f�B�X�N���v�^�v�[���̐��� ( �f�B�X�N���v�^����ނ��Ƃɂ܂Ƃ߂Ƃ���݂����ȃC���[�W )
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

    // �R�}���h���X�g�̐���.
    {
        if (!m_CommandList.Init(
            m_pDevice.Get(),
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            FrameCount))
        { return false; }
    }

    // �t���[���o�b�t�@�p�̃����_�[�^�[�Q�b�g�r���[�̐���.
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

    // �t���[���o�b�t�@�p�̐[�x�X�e���V���r���[�̐���
    {
        if (!m_DepthTarget.Init(
            m_pDevice.Get(),
            m_pPool[POOL_TYPE_DSV],
            m_Width,
            m_Height,
            DXGI_FORMAT_D32_FLOAT))
        { return false; }
    }

    // �t�F���X�̐���.
    if (!m_Fence.Init(m_pDevice.Get()))
    { return false; }

    // �r���[�|�[�g�̐ݒ�.
    {
        m_Viewport.TopLeftX  = 0.0f;
        m_Viewport.TopLeftY  = 0.0f;
        m_Viewport.Width     = float(m_Width);
        m_Viewport.Height    = float(m_Height);
        m_Viewport.MinDepth  = 0.0f;
        m_Viewport.MaxDepth  = 1.0f;
    }

    // �V�U�[��`�̐ݒ�.
    {
        m_Scissor.left   = 0;
        m_Scissor.right  = m_Width;
        m_Scissor.top    = 0;
        m_Scissor.bottom = m_Height;
    }

    // ����I��.
    return true;
	

	DLOG("�`��G���W���̏������ɐ���");

	return true;
}
#pragma endregion

#pragma region DirectX12Render

void GraphicsEngine::BeginRender(){
    // �R�}���h���X�g�̋L�^���J�n.
    m_pNowCmdList = m_CommandList.GetListReset();

    // �������ݗp���\�[�X�o���A�ݒ� �`�惂�[�h���珑�����݃��[�h�ɐ؂�ւ���
    D3D12_RESOURCE_BARRIER barriarDesc = {};
    barriarDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriarDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barriarDesc.Transition.pResource = m_ColorTarget[m_frameIndex].GetResource();
    barriarDesc.Transition.Subresource = 0;
    barriarDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barriarDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    m_pNowCmdList->ResourceBarrier(1, &barriarDesc);

    // �r���[�|�[�g�ƃV�U�[��`��ݒ�
    m_pNowCmdList->RSSetViewports(1,&m_Viewport);
    m_pNowCmdList->RSSetScissorRects(1,&m_Scissor);
   
    // �t���[���o�b�t�@�p�̃f�B�X�N���v�^�擾
    auto handleRTV = m_ColorTarget[m_frameIndex].GetHandleRTV();
    auto handleDSV = m_DepthTarget.GetHandleDSV();

    // �����_�[�^�[�Q�b�g���t���[���o�b�t�@�ɐݒ�
    // TODO:MRT�ɑΉ������邽�߂Ƀ����_�[�^�[�Q�b�g�N���X�ƃ����_�[�R���e�L�X�g�N���X�����
    m_pNowCmdList->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

    // �N���A�J���[.
    float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

    // �����_�[�^�[�Q�b�g���N���A.
    m_pNowCmdList->ClearRenderTargetView(handleRTV->HandleCPU, clearColor, 0, nullptr);

    // �[�x�X�e���V���r���[���N���A.
    m_pNowCmdList->ClearDepthStencilView(handleDSV->HandleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
}

void GraphicsEngine::EndRender(){
    
    // �������ݏI���p���\�[�X�o���A�ݒ�
    D3D12_RESOURCE_BARRIER barriarDesc = {};
    barriarDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriarDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barriarDesc.Transition.pResource = m_ColorTarget[m_frameIndex].GetResource();
    barriarDesc.Transition.Subresource = 0;
    barriarDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barriarDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    m_pNowCmdList->ResourceBarrier(1, &barriarDesc);

    // �R�}���h���X�g�̋L�^���I��.
    m_pNowCmdList->Close();

    // �R�}���h���X�g�����s.
    ID3D12CommandList* pLists[] = { m_pNowCmdList };
    m_pQueue->ExecuteCommandLists(1, pLists);

    // ��ʂɕ\��.
    m_pSwapChain->Present(1, 0);

    // �����҂�.
    m_Fence.Wait(m_pQueue.Get(), INFINITE);

    // �t���[���ԍ����X�V.
    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

#pragma endregion


#pragma region DirextX12Getter
//-----------------------------------------------------------------------------
// �f�o�C�X�擾
//-----------------------------------------------------------------------------
ID3D12Device* GraphicsEngine::GetDevice()const {
    return m_pDevice.Get();
}

//-----------------------------------------------------------------------------
// �t���[���ԍ��擾
//-----------------------------------------------------------------------------
uint32_t GraphicsEngine::GetCurrentFrameIndex()const {
    return m_frameIndex;
}

//-----------------------------------------------------------------------------
// �R�}���h���X�g�擾
//-----------------------------------------------------------------------------
ID3D12GraphicsCommandList* GraphicsEngine::GetCommandList()const {
    return m_pNowCmdList;
}


//-----------------------------------------------------------------------------
// �f�B�X�N���v�^�v�[�����擾
//-----------------------------------------------------------------------------
DescriptorPool* GraphicsEngine::GetDiscriptaPool(POOL_TYPE poolType)const {
    return m_pPool[poolType];
}


#pragma endregion

//-----------------------------------------------------------------------------
// �I������
//-----------------------------------------------------------------------------
void GraphicsEngine::Term() {
    // GPU�����̊�����ҋ@.
    m_Fence.Sync(m_pQueue.Get());

    // �t�F���X�j��.
    m_Fence.Term();

    // �����_�[�^�[�Q�b�g�r���[�̔j��.
    for (auto i = 0u; i < FrameCount; i++){
        m_ColorTarget[i].Term();
    }

    // �[�x�X�e���V���r���[�̔j��.
    m_DepthTarget.Term();

    // �R�}���h���X�g�̔j��.
    m_CommandList.Term();

    for (auto i = 0; i < POOL_COUNT; i++){
        if (m_pPool[i] != nullptr){
            m_pPool[i]->Release();
            m_pPool[i] = nullptr;
        }
    }

    // �X���b�v�`�F�C���̔j��.
    m_pSwapChain.Reset();

    // �R�}���h�L���[�̔j��.
    m_pQueue.Reset();

    // �f�o�C�X�̔j��.
    m_pDevice.Reset();
}

void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
}