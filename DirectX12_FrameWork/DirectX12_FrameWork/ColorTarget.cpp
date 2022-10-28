/*****************************************************************//**
 * \file   ColorTarget.cpp
 * \brief  カラーターゲットクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "ColorTarget.h"
#include "DescriptorPool.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ColorTarget::ColorTarget()
:m_pTarget(nullptr)
,m_pHandleRTV(nullptr)
,m_pPoolRTV(nullptr)
{ /* Do Nothing */ }

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
ColorTarget::~ColorTarget()
{ Term();}

//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool ColorTarget::Init(
    ID3D12Device*   pDevice,
    DescriptorPool* pPoolRTV,
    uint32_t        width,
    uint32_t        height,
    DXGI_FORMAT     format
){
    //引数チェック
    if (pDevice == nullptr || pPoolRTV == nullptr || width == 0 || height == 0) {
        return false;
    }

    assert(m_pHandleRTV == nullptr);
    assert(m_pPoolRTV == nullptr);
   
    // ディスクリプタプールのアドレスを格納して参照カウントを増やす
    m_pPoolRTV = pPoolRTV;
    m_pPoolRTV->AddRef();
    
    // ハンドルを割り当てる
    m_pHandleRTV = m_pPoolRTV->AllocHandle();
    if (m_pHandleRTV == nullptr){
        return false;
    }

    // ディスクリプタヒープの設定
    D3D12_HEAP_PROPERTIES prop = {};
    prop.Type = D3D12_HEAP_TYPE_DEFAULT;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;

    // リソースの設定
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = UINT64(width);
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    
    // クリアカラーの設定
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format;
    clearValue.Color[0] = 1.0f;
    clearValue.Color[1] = 1.0f;
    clearValue.Color[2] = 1.0f;
    clearValue.Color[3] = 1.0f;

    // リソースを生成
    auto hr = pDevice->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(m_pTarget.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    // リソースビューの設定
    m_ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    m_ViewDesc.Format = format;
    m_ViewDesc.Texture2D.MipSlice = 0;
    m_ViewDesc.Texture2D.PlaneSlice = 0;

    // リソースビューを生成
    pDevice->CreateRenderTargetView(
        m_pTarget.Get(),
        &m_ViewDesc,
        m_pHandleRTV->HandleCPU);

    return true;
}

//-----------------------------------------------------------------------------
// バックバッファから初期化
//-----------------------------------------------------------------------------
bool ColorTarget::InitFromBackBuffer(
    ID3D12Device* pDevice,
    DescriptorPool* pPoolRTV,
    uint32_t        index,
    IDXGISwapChain* pSwapChain
){
    //引数チェック
    if (pDevice == nullptr || pPoolRTV == nullptr || pSwapChain == nullptr) {
        return false;
    }

    assert(m_pHandleRTV == nullptr);
    assert(m_pPoolRTV == nullptr);

    m_pPoolRTV = pPoolRTV;
    m_pPoolRTV->AddRef();

    m_pHandleRTV = m_pPoolRTV->AllocHandle();
    if (m_pHandleRTV == nullptr){
        return false;
    }

    auto hr = pSwapChain->GetBuffer(index, IID_PPV_ARGS(m_pTarget.GetAddressOf()));
    if (FAILED(hr)){
        return false;
    }

    DXGI_SWAP_CHAIN_DESC desc;
    pSwapChain->GetDesc(&desc);

    m_ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    m_ViewDesc.Format = desc.BufferDesc.Format;
    m_ViewDesc.Texture2D.MipSlice = 0;
    m_ViewDesc.Texture2D.PlaneSlice = 0;

    pDevice->CreateRenderTargetView(
        m_pTarget.Get(),
        &m_ViewDesc,
        m_pHandleRTV->HandleCPU);

    return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void ColorTarget::Term() {
    m_pTarget.Reset();

    if (m_pPoolRTV != nullptr && m_pHandleRTV == nullptr) {
        m_pPoolRTV->FreeHandle(m_pHandleRTV);
 
    }

    if (m_pPoolRTV != nullptr) {
        m_pPoolRTV->Release();
        m_pPoolRTV = nullptr;
    }
}

//-----------------------------------------------------------------------------
// ディスクリプタハンドルの取得
//-----------------------------------------------------------------------------
DescriptorHandle* ColorTarget::GetHandleRTV()const 
{ return m_pHandleRTV;}

//-----------------------------------------------------------------------------
// リソースの取得
//-----------------------------------------------------------------------------
ID3D12Resource* ColorTarget::GetResource() const
{ return m_pTarget.Get();}

//-----------------------------------------------------------------------------
// リソース設定の取得
//-----------------------------------------------------------------------------
D3D12_RESOURCE_DESC ColorTarget::GetDesc() const{
    if (m_pTarget == nullptr)
    {
        return D3D12_RESOURCE_DESC();
    }

    return m_pTarget->GetDesc();
}

//-----------------------------------------------------------------------------
// レンダーターゲットビューの設定を取得
//-----------------------------------------------------------------------------
D3D12_RENDER_TARGET_VIEW_DESC ColorTarget::GetViewDesc() const
{return m_ViewDesc;}