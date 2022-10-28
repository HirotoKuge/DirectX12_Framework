/*****************************************************************//**
 * \file   Model.cpp
 * \brief  モデルクラス
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
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

//-----------------------------------------------------------------------------
// static members
//-----------------------------------------------------------------------------
MDLoader Model::m_loader;


//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Model::Model()
	:m_pPSO(nullptr)
	,m_pRootSig(nullptr)
{ /*Do Nothing */ }


//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
Model::~Model()
{ Term(); }

bool Model::Init(
	ID3D12Device* pDevice,
	DescriptorPool* pPool,
	const wchar_t* pPathMD,
	const wchar_t* pPathMT
){
	// 引数チェック
	if (pDevice == nullptr || pPool == nullptr || pPathMD == nullptr || pPathMT == nullptr) {
		ELOG("Erorr : Invalid Argument ");
		return false;
	}

	// リソースファイル読み込み
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

	// メッシュデータ初期化
	{
		// メモリを予約
		m_pMeshes.reserve(meshResouse.size());

		// メッシュを初期化
		for (size_t i = 0; i < meshResouse.size(); i++) {
			// メッシュを生成
			auto mesh = new(std::nothrow) Mesh();
			if (mesh == nullptr) {
				ELOG("Error : Out of memory Mesh");
				return false;
			}

			// 初期化処理
			if (mesh->Init(pDevice, meshResouse[i]) == false){
				ELOG("Error : Mesh Initialize Failed ");
				return false;
			}

			// 成功したら格納
			m_pMeshes.push_back(mesh);
		}
	}

	// マテリアル作成
	{
		//初期化
		if (m_Material.Init(pDevice, pPool, 0, materialResouse.size()) == false){
			ELOG("Error : Material Init Failed");
			return false;
		}

		// ディフューズマップを設定
		for (size_t i = 0; i < materialResouse.size(); i++) {
			std::wstring path = m_directoryPath + materialResouse[i].DiffuseMapPath;

			m_Material.SetTexture(i,Material::TEXTURE_USAGE::TEXTURE_USAGE_DIFFUSE,path);
		}
	}

	// ルートシグネチャを設定
	{
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		// ディスクリプタレンジを設定.
		D3D12_DESCRIPTOR_RANGE range = {};
		range.RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.NumDescriptors					= 1;
		range.BaseShaderRegister				= 0;
		range.RegisterSpace						= 0;
		range.OffsetInDescriptorsFromTableStart = 0;

		// ルートパラメータの設定.
		D3D12_ROOT_PARAMETER param[2] = {};
		param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param[0].Descriptor.ShaderRegister = 0;
		param[0].Descriptor.RegisterSpace = 0;
		param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		// ルートパラメータを設定.
		param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param[1].DescriptorTable.NumDescriptorRanges = 1;
		param[1].DescriptorTable.pDescriptorRanges = &range;
		param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// スタティックサンプラーの設定.
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

		// ルートシグニチャの設定.
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = 2;
		desc.NumStaticSamplers = 1;
		desc.pParameters = param;
		desc.pStaticSamplers = &sampler;
		desc.Flags = flag;

		ComPtr<ID3DBlob> pBlob;
		ComPtr<ID3DBlob> pErrorBlob;

		// シリアライズ
		auto hr = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			pBlob.GetAddressOf(),
			pErrorBlob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// ルートシグニチャを生成.
		hr = pDevice->CreateRootSignature(
			0,
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			IID_PPV_ARGS(m_pRootSig.GetAddressOf()));

		if (FAILED(hr)) {
			ELOG("Error : Root Signature Create Failed. retcode = 0x%x", hr);
			return false;
		}
	}

	// パイプラインステートオブジェクトを生成
	{
		std::wstring vsPath;
		std::wstring psPath;

		// 頂点シェーダを検索.
		if (!SearchFilePath(L"../x64/Debug/SimpleTexVS.cso", vsPath)){
			ELOG("Error : Vertex Shader Not Found.");
			return false;
		}

		// ピクセルシェーダを検索.
		if (!SearchFilePath(L"../x64/Debug/SimpleTexPS.cso", psPath)){
			ELOG("Error : Pixel Shader Node Found.");
			return false;
		}

		ComPtr<ID3DBlob> pVSBlob;
		ComPtr<ID3DBlob> pPSBlob;

		// 頂点シェーダを読み込む.
		auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
		if (FAILED(hr)){
			ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
			return false;
		}

		// ピクセルシェーダを読み込む.
		hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
		if (FAILED(hr)){
			ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
			return false;
		}

		// グラフィックスパイプラインステートを設定.
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout				= MeshVertex::InputLayout;
		desc.pRootSignature				= m_pRootSig.Get();
		desc.VS							= CD3DX12_SHADER_BYTECODE(pVSBlob.Get());
		desc.PS							= CD3DX12_SHADER_BYTECODE(pPSBlob.Get());
		desc.RasterizerState			= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		// ラスタライザーはデフォルト
		desc.RasterizerState.CullMode	= D3D12_CULL_MODE_NONE;							// カリングはなし
		desc.BlendState					= CD3DX12_BLEND_DESC(D3D12_DEFAULT);			// ブレンドステートもデフォルト
		desc.DepthStencilState			= CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// 深度ステンシルはデフォルトを使う
		desc.SampleMask					= UINT_MAX;
		desc.PrimitiveTopologyType		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;		// 三角形を描画
		desc.NumRenderTargets			= 1;											// 描画対象は1
		desc.RTVFormats[0]				= DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat					= DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count			= 1;											// サンプラーは1
		desc.SampleDesc.Quality			= 0;



		// パイプラインステートを生成.
		hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPSO.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
			return false;
		}
	}

	// 変換行列用の定数バッファの生成.
	{
		m_Transform.reserve(2);
		// TODO：カメラをクラス化してからアクセスする
		for (auto i = 0u; i < 2; ++i)
		{
			auto pCB = new (std::nothrow) ConstantBuffer();
			if (pCB == nullptr){
				ELOG("Error : Out of memory.");
				return false;
			}

			// 定数バッファ初期化.
			if (!pCB->Init(pDevice, pPool, sizeof(Transform) * 2)){
				ELOG("Error : ConstantBuffer::Init() Failed.");
				return false;
			}

			// カメラ設定.
			auto eyePos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
			auto targetPos = DirectX::XMVectorZero();
			auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


			// 垂直画角とアスペクト比の設定.
			auto fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(1280.f) / static_cast<float>(720.f);

			// 変換行列を設定.
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
// 描画処理
//-----------------------------------------------------------------------------
void Model::Draw(ID3D12GraphicsCommandList* pCmdList, DescriptorPool* pPoolRes){

	ID3D12DescriptorHeap* const pHeaps[] = {
		pPoolRes->GetHeap()
	};

	pCmdList->SetGraphicsRootSignature(m_pRootSig.Get());
	pCmdList->SetDescriptorHeaps(1, pHeaps);
	pCmdList->SetGraphicsRootConstantBufferView(0, m_Transform[GraphicsEngine::GetInstance()->GetCurrentFrameIndex()]->GetAddress());
	pCmdList->SetPipelineState(m_pPSO.Get());
	
	for (size_t i = 0; i < m_pMeshes.size(); i++) {

		// マテリアルIDを取得.
		auto id = m_pMeshes[i]->GetMaterialID();

		// テクスチャを設定.
		pCmdList->SetGraphicsRootDescriptorTable(1, m_Material.GetTextureHandle(id, Material::TEXTURE_USAGE::TEXTURE_USAGE_DIFFUSE));
		
		m_pMeshes[i]->Draw(pCmdList);
	}
	

}


void Model::Term(){
	m_pMeshes.clear();
	m_Material.Term();
	m_Transform.clear();
}
