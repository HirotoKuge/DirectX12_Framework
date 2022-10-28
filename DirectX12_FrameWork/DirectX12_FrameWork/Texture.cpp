/*****************************************************************//**
 * \file   Texture.cpp
 * \brief  テクスチャクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "Texture.h"
#include "DescriptorPool.h"
#include "Logger.h"
#include "FileUtil.h"
#include <DirectXTex.h>

#pragma comment(lib, "DirectXTex.lib")

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Texture::Texture()
:m_pTex(nullptr)
,m_pHandle(nullptr)
,m_pPool(nullptr)
{ /* Do Nothing*/ }


//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
Texture::~Texture()
{ Term(); }

//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool Texture::Init(
	ID3D12Device* pDevice, 
	DescriptorPool* pPool, 
	const wchar_t* filepath
) {
	// 引数チェック
	if (pDevice == nullptr || pPool == nullptr || filepath == nullptr) {
		ELOG("Erorr : Invalid Argument ");
		return false;
	}

	assert(m_pPool	 == nullptr);
	assert(m_pHandle == nullptr);

	// WICテクスチャロード
	DirectX::TexMetadata meta = {};
	DirectX::ScratchImage scratch = {};

	// 拡張子を取得
	std::wstring ext = GetFileExtension(filepath);

	// .pngならWICFileを使う
	if (ext == L"png") {
		auto hr = DirectX::LoadFromWICFile(filepath, DirectX::WIC_FLAGS_NONE, &meta, scratch);
		
		if (FAILED(hr)) {
			ELOG("Erorr : Failed Load TextureFile WIC : filepath = %ls",filepath);
			return false;
		}
	}

	// psdなら同梱されてるtgaを使う
	// TODO:Aliciaちゃん専用処理すぎるので何とかする
	if (ext == L"psd") {
		std::wstring texPath = ReplaceExtension(filepath, "tga");
		auto hr = DirectX::LoadFromTGAFile(texPath.c_str(), &meta, scratch);
		if (FAILED(hr)) {
			ELOG("Erorr : Failed Load TextureFile TGA : filepath = %ls", texPath);
			return false;
		}
	}

	// .tgaならTGAFileを使う
	if (ext == L"tga") {
		auto hr = DirectX::LoadFromTGAFile(filepath, &meta, scratch);
		if (FAILED(hr)) {
			ELOG("Erorr : Failed Load TextureFile TGA : filepath = %ls", filepath);
			return false;
		}
	}

	// .ddsならDDSFileを使う
	if (ext == L"dds") {
		auto hr = DirectX::LoadFromDDSFile(filepath, DirectX::DDS_FLAGS_NONE, &meta, scratch); 
		if (FAILED(hr)) {
			ELOG("Erorr : Failed Load TextureFile DDS : filepath = %ls", filepath);
			return false;
		}
	}


	// ディスクリプタプールを設定
	m_pPool = pPool;
	m_pPool->AddRef();

	// ディスクリプタハンドルを取得
	m_pHandle = pPool->AllocHandle();
	if (m_pHandle == nullptr) {
		ELOG("Erorr : Failed Alloc Descriptor Handle");
		return false;
	}

	auto img = scratch.GetImage(0, 0, 0);
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
		meta.format,
		meta.width,
		meta.height,
		static_cast<UINT16>(meta.arraySize),
		static_cast<UINT16>(meta.mipLevels));

	// リソースの生成
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc, 
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr, 
		IID_PPV_ARGS(m_pTex.GetAddressOf()));

	if (FAILED(hr)) {
		ELOG("Erorr : ID3D12Device::CreateCommittedResource() Failed.retcode = 0x%x", hr);
		return false;
	}

	hr = m_pTex->WriteToSubresource(0,
		nullptr,								//全領域へコピー
		img->pixels,							//元データアドレス
		static_cast<UINT>(img->rowPitch),		//1ラインサイズ
		static_cast<UINT>(img->slicePitch));	//全サイズ

	if (FAILED(hr)) {
		ELOG("Erorr : ID3D12Resource::WriteToSubresource() Failed");
		return false;
	}

	// シェーダーリソースビューの設定を取得
	auto viewDesc = GetViewDesc(false);

	// シェーダーリソースビューを生成
	pDevice->CreateShaderResourceView(m_pTex.Get(),&viewDesc,m_pHandle->HandleCPU);

	return true;
}


bool Texture::Init(
	ID3D12Device* pDevice,
	DescriptorPool* pPool,
	const D3D12_RESOURCE_DESC* pDesc,
	bool isCube
){
	// 引数チェック
	if (pDevice == nullptr || pPool == nullptr || pDesc == nullptr) {
		ELOG("Erorr : Invalid Argument ");
		return false;
	}

	assert(m_pPool == nullptr);
	assert(m_pHandle == nullptr);

	// ディスクリプタプールを設定
	m_pPool = pPool;
	m_pPool->AddRef();

	// ディスクリプタハンドルを取得
	m_pHandle = pPool->AllocHandle();
	if (m_pHandle == nullptr) {
		ELOG("Erorr : Failed Alloc Descriptor Handle");
		return false;
	}

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.CreationNodeMask = 0;
	prop.VisibleNodeMask = 0;

	// リソース生成
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		pDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(m_pTex.GetAddressOf()));

	if (FAILED(hr)) {
		ELOG("Erorr : ID3D12Device::CreateCommittedResource() Failed.retcode = 0x%x", hr);
		return false;
	}

	// シェーダリソースビューの設定を求める.
	auto viewDesc = GetViewDesc(isCube);

	// シェーダリソースビューを生成します.
	pDevice->CreateShaderResourceView(m_pTex.Get(), &viewDesc, m_pHandle->HandleCPU);

	return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void Texture::Term() {
	m_pTex.Reset();

	// ディスクリプタハンドルを開放
	if (m_pHandle != nullptr && m_pPool != nullptr) {
		m_pPool->FreeHandle(m_pHandle);
		m_pHandle = nullptr;
	}

	// ディスクリプタプールを開放
	if (m_pPool != nullptr) {
		m_pPool->Release();
		m_pPool = nullptr;
	}
}

//-----------------------------------------------------------------------------
// CPUディスクリプタハンドルを取得
//-----------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetHandleCPU()const{
	if (m_pHandle != nullptr) 
	{ return m_pHandle->HandleCPU; }

	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

//-----------------------------------------------------------------------------
// GPUディスクリプタハンドルを取得
//-----------------------------------------------------------------------------
D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetHandleGPU()const {
	if (m_pHandle != nullptr) 
	{ return m_pHandle->HandleGPU; }

	return D3D12_GPU_DESCRIPTOR_HANDLE();
}

//-----------------------------------------------------------------------------
// シェーダーリソースビューの設定を取得
//-----------------------------------------------------------------------------
D3D12_SHADER_RESOURCE_VIEW_DESC Texture::GetViewDesc(bool isCube) {
	auto desc = m_pTex->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	
	viewDesc.Format					 = desc.Format;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_BUFFER:
	{
		// バッファは対象外
		abort(); // アプリを止める.
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
	{
		if (desc.DepthOrArraySize > 1)
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

			viewDesc.Texture1DArray.MostDetailedMip = 0;
			viewDesc.Texture1DArray.MipLevels = desc.MipLevels;
			viewDesc.Texture1DArray.FirstArraySlice = 0;
			viewDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
			viewDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
		}
		else
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

			viewDesc.Texture1D.MostDetailedMip = 0;
			viewDesc.Texture1D.MipLevels = desc.MipLevels;
			viewDesc.Texture1D.ResourceMinLODClamp = 0.0f;
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	{
		if (isCube)
		{
			if (desc.DepthOrArraySize > 6)
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;

				viewDesc.TextureCubeArray.MostDetailedMip = 0;
				viewDesc.TextureCubeArray.MipLevels = desc.MipLevels;
				viewDesc.TextureCubeArray.First2DArrayFace = 0;
				viewDesc.TextureCubeArray.NumCubes = (desc.DepthOrArraySize / 6);
				viewDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

				viewDesc.TextureCube.MostDetailedMip = 0;
				viewDesc.TextureCube.MipLevels = desc.MipLevels;
				viewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			}
		}
		else
		{
			if (desc.DepthOrArraySize > 1)
			{
				if (desc.MipLevels > 1)
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;

					viewDesc.Texture2DMSArray.FirstArraySlice = 0;
					viewDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

					viewDesc.Texture2DArray.MostDetailedMip = 0;
					viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
					viewDesc.Texture2DArray.FirstArraySlice = 0;
					viewDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
					viewDesc.Texture2DArray.PlaneSlice = 0;
					viewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
				}
			}
			else
			{
				if (desc.MipLevels > 1)
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

					viewDesc.Texture2D.MostDetailedMip = 0;
					viewDesc.Texture2D.MipLevels = desc.MipLevels;
					viewDesc.Texture2D.PlaneSlice = 0;
					viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				}
			}
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
	{
		viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;

		viewDesc.Texture3D.MostDetailedMip = 0;
		viewDesc.Texture3D.MipLevels = desc.MipLevels;
		viewDesc.Texture3D.ResourceMinLODClamp = 0.0f;
	}
	break;

	default:
	{
		// 想定外
		abort(); // アプリを止める.
	}
	break;
	}

	return viewDesc;

}



