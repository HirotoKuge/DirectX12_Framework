/*****************************************************************//**
 * \file   ConstantBuffer.h
 * \brief  定数バッファクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <d3d12.h>
#include <vector>
#include "ComPtr.h"

//-----------------------------------------------------------------------------
// Forward Declarations.
//-----------------------------------------------------------------------------
class DescriptorHandle;
class DescriptorPool;



class ConstantBuffer{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
public:
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
	ConstantBuffer();

	/*****************************************************************//**
	 * \brief デストラクタ
	 *********************************************************************/
	~ConstantBuffer();
	
	/*****************************************************************//**
	 * \brief 初期化処理
	 * 
	 * \param pDevice	デバイス
	 * \param pPool		ディスクリプタプール
	 * \param size		サイズ
	 * \return true		初期化に成功
	 * \return false	初期化に失敗
	 *********************************************************************/
	bool Init(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		size_t          size);

	/*****************************************************************//**
	 * \brief 終了処理
	 *********************************************************************/
	void Term();

	/*****************************************************************//**
	 * \brief	GPUの仮想アドレスを取得
	 * \return	GPUの仮想アドレス 
	 *********************************************************************/
	D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

	/*****************************************************************//**
	 * \brief	CPUディスクリプタハンドルを取得
	 * \return	CPUディスクリプタハンドル
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU()const;

	/*****************************************************************//**
	 * \brief	GPUディスクリプタハンドルを取得
	 * \return	GPUディスクリプタハンドル
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU()const;

	/*****************************************************************//**
	 * \brief	メモリマッピング済のポインタを取得
	 * \return	メモリマッピング済のポインタ
	 *********************************************************************/
	void* GetPtr()const;

	/*****************************************************************//**
	 * \brief	メモリマッピング済のポインタを取得
	 * \return	メモリマッピング済のポインタ
	 *********************************************************************/
	template<typename T>
	T* GetPtr()
	{ return reinterpret_cast<T*>(GetPtr()); };

	//!}
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	ComPtr<ID3D12Resource>			m_pCB;			// 定数バッファ
	DescriptorHandle*				m_pHandle;		// ディスクリプタハンドル
	DescriptorPool*					m_pPool;		// ディスクリプタプール
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_desc;			// 定数バッファビューの構成設定
	void*							m_pMappedPtr;	// マッピング済みポインタ

	//!} 
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	ConstantBuffer(const ConstantBuffer&) = delete;		// アクセス禁止.
	void operator = (const ConstantBuffer&) = delete;	// アクセス禁止.

	//!} 
};

