/*****************************************************************//**
 * \file   ConstantBuffer.cpp
 * \brief  定数バッファクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "ConstantBuffer.h"
#include "DescriptorPool.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer()
: m_pCB			(nullptr)
, m_pHandle		(nullptr)
, m_pPool		(nullptr)
, m_pMappedPtr	(nullptr) 
{ /* Do Nothing */ }

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{ Term(); };

//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool ConstantBuffer::Init(
ID3D12Device*	pDevice,
DescriptorPool* pPool,
size_t          size
) {
	if (pDevice == nullptr || pPool == nullptr || size == 0) 
	{ return false; }
	
	assert(m_pPool == nullptr);
	assert(m_pHandle == nullptr);

	m_pPool = pPool;
	m_pPool->AddRef();

	size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	UINT64 sizeAligned = (size + (align - 1)) & ~(align - 1); // alignに切り上げる.

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type					= D3D12_HEAP_TYPE_UPLOAD;			// ヒープタイプ
	prop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// GPUページプロパティ
	prop.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;		// メモリプール
	prop.CreationNodeMask		= 1;								// 複数GPUの時の話なので1でよい
	prop.VisibleNodeMask		= 1;								// 複数GPUの時の話なので1でよい

	// リソースの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;	// リソースの次元
	desc.Alignment				= 0;								// リソースのアライメント
	desc.Width					= sizeAligned;						// バッファサイズ
	desc.Height					= 1;								// バッファの場合は１
	desc.DepthOrArraySize		= 1;								// バッファの場合は１
	desc.MipLevels				= 1;								// バッファの場合は１
	desc.Format					= DXGI_FORMAT_UNKNOWN;				// バッファの場合はUNKNOWN
	desc.SampleDesc.Count		= 1;								// バッファの場合は１
	desc.SampleDesc.Quality		= 0;								// バッファの場合は０
	desc.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// テクスチャレイアウト
	desc.Flags					= D3D12_RESOURCE_FLAG_NONE;			// フラグ

	// リソースを生成.
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pCB.GetAddressOf()));
	if (FAILED(hr))
	{ return false; }

	// メモリマッピング
	hr = m_pCB->Map(0, nullptr, &m_pMappedPtr);

	m_desc.BufferLocation = m_pCB->GetGPUVirtualAddress();
	m_desc.SizeInBytes = UINT(sizeAligned);
	m_pHandle = pPool->AllocHandle();

	pDevice->CreateConstantBufferView(&m_desc, m_pHandle->HandleCPU);

	return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void ConstantBuffer::Term() {
	// メモリマッピングを解除して,定数バッファを解放する
	if (m_pCB != nullptr) {
		m_pCB->Unmap(0, nullptr);
		m_pCB.Reset();
	}

	// ビューを破棄
	if (m_pPool != nullptr) {
		m_pPool->FreeHandle(m_pHandle);
		m_pHandle = nullptr;
	}

	// ディスクリプタプールを解放
	if (m_pPool != nullptr) {
		m_pPool->Release();
		m_pPool = nullptr;
	}

	m_pMappedPtr = nullptr;
}

//-----------------------------------------------------------------------------
// GPU仮想アドレスを取得
//-----------------------------------------------------------------------------
D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress()const
{ return m_desc.BufferLocation; }

//-----------------------------------------------------------------------------
// CPUディスクリプタハンドルを取得
//-----------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleCPU()const
{ return m_pHandle->HandleCPU; }

//-----------------------------------------------------------------------------
// GPUディスクリプタハンドルを取得
//-----------------------------------------------------------------------------
D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleGPU()const
{ return m_pHandle->HandleGPU; }

//-----------------------------------------------------------------------------
// メモリマッピング済みのポインタを取得
//-----------------------------------------------------------------------------
void* ConstantBuffer::GetPtr()const 
{ return m_pMappedPtr; }