/*****************************************************************//**
 * \file   Model.cpp
 * \brief  ���f���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "Model.h"
#include "FileUtil.h"
#include "Logger.h"
#include "GraphicsEngine.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

//-----------------------------------------------------------------------------
// static members
//-----------------------------------------------------------------------------
MDLoader Model::m_loader;


//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
Model::Model()
{ /*Do Nothing */ }


//-----------------------------------------------------------------------------
// �f�X�g���N�^
//-----------------------------------------------------------------------------
Model::~Model()
{ Term(); }

bool Model::Init(
	ID3D12Device* pDevice,
	DescriptorPool* pPool,
	const wchar_t* pPathMD,
	const wchar_t* pPathMT
){
	// �����`�F�b�N
	if (pDevice == nullptr || pPool == nullptr || pPathMD == nullptr || pPathMT == nullptr) {
		ELOG("Erorr : Invalid Argument ");
		return false;
	}

	// ���\�[�X�t�@�C���ǂݍ���
	std::vector<ResMesh>	 meshResouse;
	std::vector<ResMaterial> materialResouse;

	if (m_loader.LoadMD(meshResouse, pPathMD) == false) {
		ELOG("Erorr : Failed load .md : FilePath = %ls ",&pPathMD);
		return false;
	}

	if (m_loader.LoadMT(materialResouse, pPathMT) == false) {
		ELOG("Erorr : Failed load .mt : FilePath = %ls ", &pPathMT);
		return false;
	}
	
	m_directoryPath = GetDirectoryPath(pPathMD);

	// ���b�V���f�[�^������
	{
		// ��������\��
		m_pMeshes.reserve(meshResouse.size());

		// ���b�V����������
		for (size_t i = 0; i < meshResouse.size(); i++) {
			// ���b�V���𐶐�
			auto mesh = new(std::nothrow) Mesh();
			if (mesh == nullptr) {
				ELOG("Error : Out of memory Mesh");
				return false;
			}

			// ����������
			if (mesh->Init(pDevice, meshResouse[i]) == false){
				ELOG("Error : Mesh Initialize Failed ");
				return false;
			}

			// ����������i�[
			m_pMeshes.push_back(mesh);
		}
	}

	// �}�e���A���쐬
	{
		//������
		if (m_Material.Init(pDevice, pPool, 0, materialResouse.size()) == false){
			ELOG("Error : Material Init Failed");
			return false;
		}

		// �f�B�t���[�Y�}�b�v��ݒ�
		for (size_t i = 0; i < materialResouse.size(); i++) {
			std::wstring path = m_directoryPath + materialResouse[i].DiffuseMapPath;

			m_Material.SetTexture(i,Material::TEXTURE_USAGE::TEXTURE_USAGE_DIFFUSE,path);
		}
	}

	// �x�[�V�b�N�ȃO���t�B�N�X�p�C�v���C���𐶐�
	{

		// ���[�g�V�O�l�`����ݒ�
		{
			auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
			flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

			// �f�B�X�N���v�^�����W��ݒ�.
			D3D12_DESCRIPTOR_RANGE range = {};
			range.RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.NumDescriptors					= 1;
			range.BaseShaderRegister				= 0;
			range.RegisterSpace						= 0;
			range.OffsetInDescriptorsFromTableStart = 0;

			// ���[�g�p�����[�^�̐ݒ�.
			D3D12_ROOT_PARAMETER param[2] = {};
			param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param[0].Descriptor.ShaderRegister = 0;
			param[0].Descriptor.RegisterSpace = 0;
			param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

			// ���[�g�p�����[�^��ݒ�.
			param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param[1].DescriptorTable.NumDescriptorRanges = 1;
			param[1].DescriptorTable.pDescriptorRanges = &range;
			param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			// �X�^�e�B�b�N�T���v���[�̐ݒ�.
			D3D12_STATIC_SAMPLER_DESC sampler = {};
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;
			sampler.MaxAnisotropy = 1;
			sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler.MinLOD = -D3D12_FLOAT32_MAX;
			sampler.MaxLOD = +D3D12_FLOAT32_MAX;
			sampler.ShaderRegister = 0;
			sampler.RegisterSpace = 0;
			sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			// ���[�g�V�O�j�`���̐ݒ�.
			D3D12_ROOT_SIGNATURE_DESC desc = {};
			desc.NumParameters = 2;
			desc.NumStaticSamplers = 1;
			desc.pParameters = param;
			desc.pStaticSamplers = &sampler;
			desc.Flags = flag;

			// ����
			m_pRootSignatures[PIPELINE_STATE_TYPE::BASIC] =  std::make_unique<RootSignature>();
			bool isSuccessed = m_pRootSignatures[PIPELINE_STATE_TYPE::BASIC]->Init(pDevice,desc);

			if (isSuccessed == false) {
				ELOG("Error : Failed Init RootSignature Class");
				return false;
			}
		}

		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�𐶐�
		{
			// �O���t�B�b�N�X�p�C�v���C���X�e�[�g��ݒ�.
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout				= MeshVertex::InputLayout;
			desc.RasterizerState			= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		// ���X�^���C�U�[�̓f�t�H���g
			desc.RasterizerState.CullMode	= D3D12_CULL_MODE_NONE;							// �J�����O�͂Ȃ�
			desc.BlendState					= CD3DX12_BLEND_DESC(D3D12_DEFAULT);			// �u�����h�X�e�[�g���f�t�H���g
			desc.DepthStencilState			= CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// �[�x�X�e���V���̓f�t�H���g���g��
			desc.SampleMask					= UINT_MAX;
			desc.PrimitiveTopologyType		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;		// �O�p�`��`��
			desc.NumRenderTargets			= 1;											// �`��Ώۂ�1
			desc.RTVFormats[0]				= DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.DSVFormat					= DXGI_FORMAT_D32_FLOAT;
			desc.SampleDesc.Count			= 1;											// �T���v���[��1
			desc.SampleDesc.Quality			= 0;

			// ������
			m_pPipelineStates[PIPELINE_STATE_TYPE::BASIC] = std::make_unique<PipelineState>();


#ifdef DEBUG
			bool isSuccessed = m_pPipelineStates[PIPELINE_STATE_TYPE::BASIC]->Init(	
				pDevice, 
				desc,
				L"../x64/Debug/SimpleTexVS.cso", 
				L"../x64/Debug/SimpleTexPS.cso", 
				m_pRootSignatures[PIPELINE_STATE_TYPE::BASIC]->Get());

			if (isSuccessed == false) {
				ELOG("Error : Failed Init PipelineState Class");
				return false;
			}
#else
			bool isSuccessed = m_pPipelineStates[PIPELINE_STATE_TYPE::BASIC]->Init(
				pDevice,
				desc,
				L"../x64/Release/SimpleTexVS.cso",
				L"../x64/Release/SimpleTexPS.cso",
				m_pRootSignatures[PIPELINE_STATE_TYPE::BASIC]->Get());

			if (isSuccessed == false) {
				ELOG("Error : Failed Init PipelineState Class");
				return false;
			}

#endif // DEBUG

		}

	}

	// �ϊ��s��p�̒萔�o�b�t�@�̐���.
	{
		m_Transform.reserve(2);
		// TODO�F�J�������N���X�����Ă���A�N�Z�X����
		for (auto i = 0u; i < 2; ++i)
		{
			auto pCB = new (std::nothrow) ConstantBuffer();
			if (pCB == nullptr){
				ELOG("Error : Out of memory.");
				return false;
			}

			// �萔�o�b�t�@������.
			if (!pCB->Init(pDevice, pPool, sizeof(Transform) * 2)){
				ELOG("Error : ConstantBuffer::Init() Failed.");
				return false;
			}

			// �J�����ݒ�.
			auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
			auto targetPos = DirectX::XMVectorZero();
			auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


			// ������p�ƃA�X�y�N�g��̐ݒ�.
			auto fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(1280.f) / static_cast<float>(720.f);

			// �ϊ��s���ݒ�.
			auto ptr = pCB->GetPtr<Transform>();
			ptr->World = DirectX::XMMatrixIdentity();
			ptr->View = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
			ptr->Proj = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, 1.0f, 1000.0f);

			m_Transform.push_back(pCB);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// �`�揈��
//-----------------------------------------------------------------------------
void Model::Draw(ID3D12GraphicsCommandList* pCmdList, DescriptorPool* pPoolRes, uint32_t nowFrameCnt){

	ID3D12DescriptorHeap* const pHeaps[] = {
		pPoolRes->GetHeap()
	};

	pCmdList->SetGraphicsRootSignature(m_pRootSignatures[PIPELINE_STATE_TYPE::BASIC]->Get());
	pCmdList->SetDescriptorHeaps(1, pHeaps);
	pCmdList->SetGraphicsRootConstantBufferView(0, m_Transform[nowFrameCnt]->GetAddress());
	pCmdList->SetPipelineState(m_pPipelineStates[PIPELINE_STATE_TYPE::BASIC]->Get());
	
	for (size_t i = 0; i < m_pMeshes.size(); i++) {

		// �}�e���A��ID���擾.
		auto id = m_pMeshes[i]->GetMaterialID();

		// �e�N�X�`����ݒ�.
		pCmdList->SetGraphicsRootDescriptorTable(1, m_Material.GetTextureHandle(id, Material::TEXTURE_USAGE::TEXTURE_USAGE_DIFFUSE));
		
		m_pMeshes[i]->Draw(pCmdList);
	}
	

}


void Model::Term(){
	m_pMeshes.clear();
	m_Material.Term();
	m_Transform.clear();
}
