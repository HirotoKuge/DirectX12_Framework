/*****************************************************************//**
 * \file   Mesh.cpp
 * \brief  メッシュクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "Mesh.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Mesh::Mesh()
	:m_materialID(UINT32_MAX)
	,m_indexCnt(0)
{ /* Do Nothing */ }

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{ Term(); }

//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool Mesh::Init(ID3D12Device* pDevice, const ResMesh& resouse) {
	
	//引数チェック
	if (pDevice == nullptr) 
	{ return false; }

	if (!m_VB.Init(
		pDevice, sizeof(MeshVertex) * resouse.Vertices.size(), resouse.Vertices.data())) 
	{ return false; }

	if (!m_IB.Init(
		pDevice, sizeof(uint32_t) * resouse.Indices.size(), resouse.Indices.data())) 
	{ return false; }

	m_materialID = resouse.matetialID;
	m_indexCnt = uint32_t(resouse.Indices.size());

	return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void Mesh::Term() {
	m_VB.Term();
	m_IB.Term();
	m_materialID = UINT32_MAX;
	m_indexCnt = 0;
}

//-----------------------------------------------------------------------------
// 描画処理
//-----------------------------------------------------------------------------
void Mesh::Draw(ID3D12GraphicsCommandList* pCmdList) {
	auto VBV = m_VB.GetView();
	auto IBV = m_IB.GetView();
	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdList->IASetVertexBuffers(0, 1, &VBV);
	pCmdList->IASetIndexBuffer(&IBV);
	pCmdList->DrawIndexedInstanced(m_indexCnt, 1, 0, 0, 0);
}


//-----------------------------------------------------------------------------
// マテリアルID取得
//-----------------------------------------------------------------------------
uint32_t Mesh::GetMaterialID()const
{ return m_materialID; }
