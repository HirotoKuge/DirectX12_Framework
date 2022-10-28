/*****************************************************************//**
 * \file   Material.h
 * \brief  マテリアルクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

 //-----------------------------------------------------------------------------
 // Includes
 //-----------------------------------------------------------------------------
 #include <map>

 #include "ConstantBuffer.h"
 #include "DescriptorPool.h"
 #include "Texture.h"


//-----------------------------------------------------------------------------
// Material Class
//-----------------------------------------------------------------------------
class Material{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
public:
	//=============================================================================
	// enum
	//=============================================================================
	// テクスチャの用途
	enum TEXTURE_USAGE{
		TEXTURE_USAGE_DIFFUSE = 0,	// ディフューズマップ
		TEXTURE_USAGE_SPECULAR,		// スペキュラマップ
		TEXTURE_USAGE_SHININESS,	// シャイネスマップ
		TEXTURE_USAGE_NORMAL,		// 法線マップ

		TEXTURE_USAGE_COUNT			// 定義数
	};


	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief コンストラクタ
	 *********************************************************************/
	Material();

	/*****************************************************************//**
	 * \brief デストラクタ
	 *********************************************************************/
	~Material();
		
	/*****************************************************************//**
	 * \brief 初期化処理
	 *  
	 * \param	pDevice		デバイス
	 * \param	pPool		ディスクリプタプール
	 * \param	bufferSize	バッファサイズ
	 * \param	count		マテリアル数
	 * \return	true		初期化に成功
	 * \return	false		初期化に失敗
	 *********************************************************************/
	bool Init(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		size_t bufferSize,
		size_t count);

	/*****************************************************************//**
	 * \brief 終了処理
	 *********************************************************************/
	void Term();

	/*****************************************************************//**
	 * \brief テクスチャを設定
	 *  
	 * \param	index	マテリアル番号
	 * \param	usage	テクスチャの用途
	 * \param	path	ファイルパス
	 * \return	true	初期化に成功
	 * \return	false	初期化に失敗
	 *********************************************************************/
	bool SetTexture(
		size_t index,
		TEXTURE_USAGE usage,
		const std::wstring& path);
	
	/*****************************************************************//**
	 * \brief 定数バッファのポインタを取得
	 *  
	 * \param index	マテリアル番号
	 * \return 指定されたマテリアル番号に対応する定数バッファのポインタ
	 *		   一致するものがない場合は nullptr
	 *********************************************************************/
	void* GetBufferPtr(size_t index)const;

	/*****************************************************************//**
	 * \brief 定数バッファのポインタを取得
	 *
	 * \param index	マテリアル番号
	 * \return 指定されたマテリアル番号に対応する定数バッファのポインタ
	 *		   一致するものがない場合は nullptr
	 *********************************************************************/
	template<typename T>
	T* GetBufferPtr(size_t index)const 
	{ return reinterpret_cast<T*>(GetBufferPtr(index)); }

	/*****************************************************************//**
	 * \brief 定数バッファの仮想GPUアドレスを取得
	 *  
	 * \param index	マテリアル番号
	 * \return 指定されたマテリアル番号に対応する定数バッファのGPU仮想アドレス
	 *********************************************************************/
	D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(size_t index)const;

	/*****************************************************************//**
	 * \brief テクスチャハンドルを取得
	 *  
	 * \param index マテリアル番号
	 * \param usage テクスチャの用途　
	 * \return 一致するテクスチャのディスクリプタハンドル
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(size_t index,TEXTURE_USAGE usage)const;

	/*****************************************************************//**
	 * \brief マテリアル数を取得
	 *  
	 * \return マテリアル数
	 *********************************************************************/
	size_t GetCount()const;

	//!}
private:
	//=============================================================================
	// structures
	//=============================================================================
	// サブセット
	struct Subset {
		ConstantBuffer* pCostantBuffer;									// 定数バッファ
		D3D12_GPU_DESCRIPTOR_HANDLE TextureHandle[TEXTURE_USAGE_COUNT];	// テクスチャハンドル
	};

	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	std::map<std::wstring,Texture*> m_pTextures;	// テクスチャ
	std::vector<Subset>				m_subsets;		// サブセット
	ID3D12Device*					m_pDevice;		// デバイス
	DescriptorPool*					m_pPool;		// ディスクリプタプール
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	Material(const Material&) = delete;
	void operator = (const Material&) = delete;
	
	static const wchar_t* DummyTag;

	//!} 
};

constexpr auto TU_DIFFUSE = Material::TEXTURE_USAGE_DIFFUSE;
constexpr auto TU_SPECULAR = Material::TEXTURE_USAGE_SPECULAR;
constexpr auto TU_SHININESS = Material::TEXTURE_USAGE_SHININESS;
constexpr auto TU_NORMAL = Material::TEXTURE_USAGE_NORMAL;
