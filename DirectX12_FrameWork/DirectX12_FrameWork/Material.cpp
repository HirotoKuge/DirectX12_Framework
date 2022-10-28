/*****************************************************************//**
 * \file   Material.cpp
 * \brief  マテリアルクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "Material.h"
#include "FileUtil.h"
#include "Logger.h"

//-----------------------------------------------------------------------------
// static member.
//-----------------------------------------------------------------------------
const wchar_t* Material::DummyTag = L""; // ダミーテクスチャ用

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Material::Material()
:m_pDevice(nullptr),
m_pPool(nullptr)
{ /* Do Nothing*/ }

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
Material::~Material()
{ Term(); }

//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool Material::Init(
	ID3D12Device* pDevice,
	DescriptorPool* pPool,
	size_t bufferSize,
	size_t count
) {
	// 引数チェック
	if (pDevice == nullptr || pPool == nullptr || count == 0) {
		ELOG("Error : Invalid Argument.");
		return false;
	}

	Term();

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pPool = pPool;
	m_pPool->AddRef();

	m_subsets.resize(count);

	//ダミーテクスチャ生成
	{
		auto pTexture = new(std::nothrow)Texture();
		if (pTexture == nullptr) {
			ELOG("Error : Out of memory");
			return false;
		}

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = 1;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		if (!pTexture->Init(pDevice, pPool, &desc, false)){
			ELOG("Error : Texture::Init() Failed.");
			pTexture->Term();
			delete pTexture;
			return false;
		}

		m_pTextures[DummyTag] = pTexture;
	}

	auto size = bufferSize * count;

	if (size > 0) {
		for (size_t i = 0; i < m_subsets.size(); i++) {
			auto pBuffer = new (std::nothrow) ConstantBuffer();
			if (pBuffer == nullptr){
				ELOG("Error : Out of memory.");
				return false;
			}

			if (!pBuffer->Init(pDevice, pPool, bufferSize)){
				ELOG("Error : ConstantBuffer::Init() Failed.");
				return false;
			}

			m_subsets[i].pCostantBuffer = pBuffer;
			for (auto j = 0; j < TEXTURE_USAGE_COUNT; ++j){
				m_subsets[i].TextureHandle[j].ptr = 0;
			}
		}
	}
	else {
		for (size_t i = 0; i < m_subsets.size(); ++i){
			m_subsets[i].pCostantBuffer = nullptr;
			for (auto j = 0; j < TEXTURE_USAGE_COUNT; ++j){
				m_subsets[i].TextureHandle[j].ptr = 0;
			}
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void Material::Term() {
	for (auto& itr : m_pTextures){
		if (itr.second != nullptr){
			itr.second->Term();
			delete itr.second;
			itr.second = nullptr;
		}
	}

	for (size_t i = 0; i < m_subsets.size(); ++i){
		if (m_subsets[i].pCostantBuffer != nullptr){
			m_subsets[i].pCostantBuffer->Term();
			delete m_subsets[i].pCostantBuffer;
			m_subsets[i].pCostantBuffer = nullptr;
		}
	}

	m_pTextures.clear();
	m_subsets.clear();

	if (m_pDevice != nullptr){
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	if (m_pPool != nullptr){
		m_pPool->Release();
		m_pPool = nullptr;
	}
}


//-----------------------------------------------------------------------------
// テクスチャ設定
//-----------------------------------------------------------------------------
bool Material::SetTexture(
	size_t index,
	TEXTURE_USAGE usage,
	const std::wstring& path
) {
	// 範囲内か確認
	if (index >= GetCount()) 
	{ return false; }

	// すでに登録されていないか確認
	if (m_pTextures.find(path) != m_pTextures.end()){
		m_subsets[index].TextureHandle[usage] = m_pTextures[path]->GetHandleGPU();
		return true;
	}

	// ファイルパスが存在するか検索
	std::wstring findPath;
	if (!SearchFilePathW(path.c_str(), findPath)) {
		// 存在しない場合はダミーテクスチャを設定
		m_subsets[index].TextureHandle[usage] = m_pTextures[DummyTag]->GetHandleGPU();
		DLOG("Alert : FilePath does not exist : FilePath = %ls", path);
		return true;
	}
	
	// ファイル名であることをチェック
	{
		if (PathIsDirectoryW(findPath.c_str()) != FALSE){
			// ファイル名でない場合はダミーテクスチャを設定
			m_subsets[index].TextureHandle[usage] = m_pTextures[DummyTag]->GetHandleGPU();
			DLOG("Alert : Not a FileName : FilePath = %ls", path);
			return true;
		}
	}

	// インスタンス生成
	auto pTexture = new (std::nothrow) Texture();
	if (pTexture == nullptr){
		ELOG("Error : Out of memory.");
		return false;
	}

	// 初期化
	if (!pTexture->Init(m_pDevice, m_pPool, findPath.c_str())){
		ELOG("Error : Texture::Init() Failed.");
		pTexture->Term();
		delete pTexture;
		return false;
	}

	// 登録
	m_pTextures[path] = pTexture;
	m_subsets[index].TextureHandle[usage] = pTexture->GetHandleGPU();

	// 正常終了
	return true;
}

//-----------------------------------------------------------------------------
// 定数バッファのポインタを取得
//-----------------------------------------------------------------------------
void* Material::GetBufferPtr(size_t index) const{
	if (index >= GetCount()){
		return nullptr;
	}

	return m_subsets[index].pCostantBuffer->GetPtr();
}

//-----------------------------------------------------------------------------
// 定数バッファのアドレスを取得
//-----------------------------------------------------------------------------
D3D12_GPU_VIRTUAL_ADDRESS Material::GetBufferAddress(size_t index) const{
	if (index >= GetCount())
	{
		return D3D12_GPU_VIRTUAL_ADDRESS();
	}

	return m_subsets[index].pCostantBuffer->GetAddress();
}

//-----------------------------------------------------------------------------
// テクスチャハンドルを取得
//-----------------------------------------------------------------------------
D3D12_GPU_DESCRIPTOR_HANDLE Material::GetTextureHandle(size_t index, TEXTURE_USAGE usage) const{
	if (index >= GetCount()){
		return D3D12_GPU_DESCRIPTOR_HANDLE();
	}

	return m_subsets[index].TextureHandle[usage];
}

//-----------------------------------------------------------------------------
// マテリアル数を取得
//-----------------------------------------------------------------------------
size_t Material::GetCount() const
{ return m_subsets.size(); }