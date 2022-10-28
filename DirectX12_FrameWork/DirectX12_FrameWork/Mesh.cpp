/*****************************************************************//**
 * \file   Mesh.cpp
 * \brief  ���b�V���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "Mesh.h"

//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
Mesh::Mesh()
	:m_materialID(UINT32_MAX)
	,m_indexCnt(0)
{ /* Do Nothing */ }

//-----------------------------------------------------------------------------
// �f�X�g���N�^
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{ Term(); }

//-----------------------------------------------------------------------------
// ����������
//-----------------------------------------------------------------------------
bool Mesh::Init(ID3D12Device* pDevice, const ResMesh& resouse) {
	
	//�����`�F�b�N
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
// �I������
//-----------------------------------------------------------------------------
void Mesh::Term() {
	m_VB.Term();
	m_IB.Term();
	m_materialID = UINT32_MAX;
	m_indexCnt = 0;
}

//-----------------------------------------------------------------------------
// �`�揈��
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
// �}�e���A��ID�擾
//-----------------------------------------------------------------------------
uint32_t Mesh::GetMaterialID()const
{ return m_materialID; }
