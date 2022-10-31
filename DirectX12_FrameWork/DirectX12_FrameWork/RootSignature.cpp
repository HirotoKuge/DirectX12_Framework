/*****************************************************************//**
 * \file   RootSignature.cpp
 * \brief  ルートシグネチャクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "RootSignature.h"
#include "Logger.h"
#include "FileUtil.h"
#include <d3dcompiler.h>
#include <d3dx12.h>
#include "SharedStruct.h"

#pragma comment(lib, "d3dcompiler.lib")

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
RootSignature::RootSignature()
	:m_pRootSignature(nullptr)
{ /* Do Nothing */ }

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
RootSignature::~RootSignature()
{ Term(); }

//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool RootSignature::Init(ID3D12Device* pDevice, D3D12_ROOT_SIGNATURE_DESC& desc) {
	
	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	// シリアライズ
	auto hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		pBlob.GetAddressOf(),
		pErrorBlob.GetAddressOf());

	if (FAILED(hr)){
		ELOG("Error : Failed D3D12SerializeRootSignature");
		return false; 
	}

	// ルートシグニチャを生成.
	hr = pDevice->CreateRootSignature(
		0,
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf()));

	if (FAILED(hr)) {
		ELOG("Error : Root Signature Create Failed. retcode = 0x%x", hr);
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void RootSignature::Term() {
	m_pRootSignature.Reset();
}

//-----------------------------------------------------------------------------
// ルートシグネチャ取得
//-----------------------------------------------------------------------------
ID3D12RootSignature* RootSignature::Get()
{ return m_pRootSignature.Get(); }
