/*****************************************************************//**
 * \file   DescriptorPool.cpp
 * \brief  ディスクリプタプールクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#include "DescriptorPool.h"

 //-----------------------------------------------------------------------------
 //		コンストラクタ
 //-----------------------------------------------------------------------------
DescriptorPool::DescriptorPool()
: m_refCount(1)
, m_pool()
, m_pHeap()
, m_descriptorSize(0)
{/* Do Nothing*/ }

//-----------------------------------------------------------------------------
//		デストラクタ
//-----------------------------------------------------------------------------
DescriptorPool::~DescriptorPool() {
	m_pool.Term();
	m_pHeap.Reset();
	m_descriptorSize = 0;
}

//-----------------------------------------------------------------------------
//		参照カウントを増やす
//-----------------------------------------------------------------------------
void DescriptorPool::AddRef() 
{ m_refCount++; }

//-----------------------------------------------------------------------------
//		開放する
//-----------------------------------------------------------------------------
void DescriptorPool::Release(){
	m_refCount--;

	if (m_refCount == 0) {
		delete this;
	}
}

//-----------------------------------------------------------------------------
//		参照カウントの取得
//-----------------------------------------------------------------------------
uint32_t DescriptorPool::GetRefCount()
{ return m_refCount; }

//-----------------------------------------------------------------------------
//		ディスクリプタハンドルの割り当て
//-----------------------------------------------------------------------------
DescriptorHandle* DescriptorPool::AllocHandle() {
	//初期化関数の定義
	auto func = [&](uint32_t index, DescriptorHandle* pHandle){
		auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += m_descriptorSize * index;

		auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += m_descriptorSize * index;

		pHandle->HandleCPU = handleCPU;
		pHandle->HandleGPU = handleGPU;
	};

	// 初期化関数を実行してからハンドルを返却
	return m_pool.AllocItem(func);	
}

//-----------------------------------------------------------------------------
//		ディスクリプタハンドルの開放
//-----------------------------------------------------------------------------
void DescriptorPool::FreeHandle(DescriptorHandle*& pHandle){
	if (pHandle != nullptr) {
		//ハンドルをプールに戻す
		m_pool.FreeItem(pHandle);

		//nullptrでクリア
		pHandle = nullptr;
	}
}

//-----------------------------------------------------------------------------
//      利用可能なハンドル数を取得
//-----------------------------------------------------------------------------
uint32_t DescriptorPool::GetAvailableHandleCount() const
{ return m_pool.GetAvailableCount(); }

//-----------------------------------------------------------------------------
//      割り当て済みのハンドル数を取得
//-----------------------------------------------------------------------------
uint32_t DescriptorPool::GetAllocatedHandleCount() const
{ return m_pool.GetUsedCount(); }

//-----------------------------------------------------------------------------
//      ハンドル総数を取得
//-----------------------------------------------------------------------------
uint32_t DescriptorPool::GetTotalHandleCount() const
{ return m_pool.GetSize(); }

//-----------------------------------------------------------------------------
//		ディスクリプタヒープを取得
//-----------------------------------------------------------------------------
ID3D12DescriptorHeap* const DescriptorPool::GetHeap() const
{ return m_pHeap.Get(); }

//-----------------------------------------------------------------------------
//		生成処理
//-----------------------------------------------------------------------------
bool DescriptorPool::Create(
    ID3D12Device* pDevice,
    const D3D12_DESCRIPTOR_HEAP_DESC* pDesc,
    DescriptorPool** ppPool
) {
    // 引数チェック
    if (pDevice == nullptr || pDesc == nullptr || ppPool == nullptr){
        return false;
    }

    // インスタンス生成
    auto instance = new (std::nothrow) DescriptorPool();
    if (instance == nullptr)
    {
        return false;
    }

    // ディスクリプタヒープを生成.
    auto hr = pDevice->CreateDescriptorHeap(
        pDesc,
        IID_PPV_ARGS(instance->m_pHeap.GetAddressOf()));

    // 失敗したら解放処理を行って終了
    if (FAILED(hr)){
        instance->Release();
        return false;
    }

    // プールを初期化
    if (!instance->m_pool.Init(pDesc->NumDescriptors)){
        instance->Release();
        return false;
    }

    // ディスクリプタのインクリメントサイズを取得
    instance->m_descriptorSize =
        pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

    // インスタンスを設定
    *ppPool = instance;

    // 正常終了
    return true;

}


