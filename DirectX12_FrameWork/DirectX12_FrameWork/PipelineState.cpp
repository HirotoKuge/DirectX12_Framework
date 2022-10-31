/*****************************************************************//**
 * \file   PipelineState.cpp
 * \brief  �O���t�B�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "PipelineState.h"
#include "Logger.h"
#include "FileUtil.h"
#include <d3dcompiler.h>
#include <d3dx12.h>
#include "SharedStruct.h"

#pragma comment(lib, "d3dcompiler.lib")

//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
PipelineState::PipelineState()
	:m_pPipelineState(nullptr)
	,m_pVsBlob(nullptr)
	,m_pPsBlob(nullptr)
{ /* Do Nothing */ }


//-----------------------------------------------------------------------------
// �f�X�g���N�^
//-----------------------------------------------------------------------------
PipelineState::~PipelineState()
{ Term(); };

//-----------------------------------------------------------------------------
// ����������
//-----------------------------------------------------------------------------
bool PipelineState::Init(
	ID3D12Device* pDevice,
	D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc,
	const wchar_t* pVsPath,
	const wchar_t* pPsPath,
	ID3D12RootSignature* pRootSignature
){
	//�����`�F�b�N
	if (pDevice == nullptr || pVsPath == nullptr || pPsPath == nullptr||pRootSignature == nullptr) {
		ELOG("Erorr : Invalid Argument ");
		return false;
	}

	std::wstring vsPath;
	std::wstring psPath;

	// ���_�V�F�[�_������.
	if (!SearchFilePath(pVsPath, vsPath)) {
		ELOG("Error : Vertex Shader Not Found.");
		return false;
	}

	// �s�N�Z���V�F�[�_������.
	if (!SearchFilePath(pPsPath, psPath)) {
		ELOG("Error : Pixel Shader Node Found.");
		return false;
	}

	ComPtr<ID3DBlob> pVSBlob;
	ComPtr<ID3DBlob> pPSBlob;

	// ���_�V�F�[�_��ǂݍ���.
	auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
	if (FAILED(hr)) {
		ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
		return false;
	}

	// �s�N�Z���V�F�[�_��ǂݍ���.
	hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
	if (FAILED(hr)) {
		ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
		return false;
	}

	// �e��ݒ�
	desc.pRootSignature = pRootSignature;
	desc.VS = CD3DX12_SHADER_BYTECODE(pVSBlob.Get());
	desc.PS = CD3DX12_SHADER_BYTECODE(pPSBlob.Get());
	
	// �p�C�v���C���X�e�[�g�𐶐�.
	hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPipelineState.GetAddressOf()));
	if (FAILED(hr)){
		ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
		return false;
	}

	// ����I��
	return true;
}

//-----------------------------------------------------------------------------
// �I������
//-----------------------------------------------------------------------------
void PipelineState::Term(){
	m_pPipelineState.Reset();
	m_pVsBlob.Reset();
	m_pPsBlob.Reset();
}

//-----------------------------------------------------------------------------
// �p�C�v���C���X�e�[�g�I�u�W�F�N�g���擾	
//-----------------------------------------------------------------------------
ID3D12PipelineState* PipelineState::Get() 
{ return m_pPipelineState.Get();}
