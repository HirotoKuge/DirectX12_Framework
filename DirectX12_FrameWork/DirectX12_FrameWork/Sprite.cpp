/*****************************************************************//**
 * \file   Sprite.cpp
 * \brief  �X�v���C�g�N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "Sprite.h"

//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
Sprite::Sprite()
:m_pPSO(nullptr)
,m_pRootSig(nullptr)
{/*Do Nothing*/ }

//-----------------------------------------------------------------------------
// �f�X�g���N�^
//-----------------------------------------------------------------------------
Sprite::~Sprite()
{ Term(); }

bool Sprite::Init(ID3D12Device* pDevice) {
	MeshVertex vertices[]{
		/*					���_								�@��								UV						�ڋ��*/
		{DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT3(0.f,0.f,0.f)},
		{DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT3(0.f,0.f,0.f)},
		{DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT3(0.f,0.f,0.f)},
		{DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT3(0.f,0.f,0.f)}
	};

	uint32_t index[]={0,1,2 ,0,2,3};

	if (!m_VB.Init(pDevice, sizeof(MeshVertex) * 4, vertices)){
		return false;
	}

	if (!m_IB.Init(pDevice, sizeof(uint32_t) * 6, index)){
		return false;
	}

	return true;
}




//-----------------------------------------------------------------------------
// �`�揈��
//-----------------------------------------------------------------------------
void Sprite::Draw(ID3D12GraphicsCommandList* pCmdList) {
	auto VBV = m_VB.GetView();
	auto IBV = m_IB.GetView();
	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdList->IASetVertexBuffers(0, 1, &VBV);
	pCmdList->IASetIndexBuffer(&IBV);
	pCmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}


void Sprite::Term() {
	m_VB.Term();
	m_IB.Term();
}
