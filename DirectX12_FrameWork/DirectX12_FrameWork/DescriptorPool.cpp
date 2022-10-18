/*****************************************************************//**
 * \file   DescriptorPool.cpp
 * \brief  �f�B�X�N���v�^�v�[���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#include "DescriptorPool.h"

 //-----------------------------------------------------------------------------
 //		�R���X�g���N�^
 //-----------------------------------------------------------------------------
DescriptorPool::DescriptorPool()
: m_refCount(1)
, m_pool()
, m_pHeap()
, m_descriptorSize(0)
{/* Do Nothing*/ }

//-----------------------------------------------------------------------------
//		�f�X�g���N�^
//-----------------------------------------------------------------------------
DescriptorPool::~DescriptorPool() {
	m_pool.Term();
	m_pHeap.Reset();
	m_descriptorSize = 0;
}

//-----------------------------------------------------------------------------
//		�Q�ƃJ�E���g�𑝂₷
//-----------------------------------------------------------------------------
void DescriptorPool::AddRef() 
{ m_refCount++; }

//-----------------------------------------------------------------------------
//		�J������
//-----------------------------------------------------------------------------
void DescriptorPool::Release(){
	m_refCount--;

	if (m_refCount == 0) {
		delete this;
	}
}

//-----------------------------------------------------------------------------
//		�Q�ƃJ�E���g�̎擾
//-----------------------------------------------------------------------------
uint32_t DescriptorPool::GetRefCount()
{ return m_refCount; }

//-----------------------------------------------------------------------------
//		�f�B�X�N���v�^�n���h���̊��蓖��
//-----------------------------------------------------------------------------
DescriptorHandle* DescriptorPool::AllocHandle() {
	//�������֐��̒�`
	auto func = [&](uint32_t index, DescriptorHandle* pHandle){
		auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += m_descriptorSize * index;

		auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += m_descriptorSize * index;

		pHandle->HandleCPU = handleCPU;
		pHandle->HandleGPU = handleGPU;
	};

	// �������֐������s���Ă���n���h����ԋp
	return m_pool.AllocItem(func);	
}

//-----------------------------------------------------------------------------
//		�f�B�X�N���v�^�n���h���̊J��
//-----------------------------------------------------------------------------
void DescriptorPool::FreeHandle(DescriptorHandle*& pHandle){
	if (pHandle != nullptr) {
		//�n���h�����v�[���ɖ߂�
		m_pool.FreeItem(pHandle);

		//nullptr�ŃN���A
		pHandle = nullptr;
	}
}

//-----------------------------------------------------------------------------
//      ���p�\�ȃn���h�������擾
//-----------------------------------------------------------------------------
uint32_t DescriptorPool::GetAvailableHandleCount() const
{ return m_pool.GetAvailableCount(); }

//-----------------------------------------------------------------------------
//      ���蓖�čς݂̃n���h�������擾
//-----------------------------------------------------------------------------
uint32_t DescriptorPool::GetAllocatedHandleCount() const
{ return m_pool.GetUsedCount(); }

//-----------------------------------------------------------------------------
//      �n���h���������擾
//-----------------------------------------------------------------------------
uint32_t DescriptorPool::GetTotalHandleCount() const
{ return m_pool.GetSize(); }

//-----------------------------------------------------------------------------
//		�f�B�X�N���v�^�q�[�v���擾
//-----------------------------------------------------------------------------
ID3D12DescriptorHeap* const DescriptorPool::GetHeap() const
{ return m_pHeap.Get(); }

//-----------------------------------------------------------------------------
//		��������
//-----------------------------------------------------------------------------
bool DescriptorPool::Create(
    ID3D12Device* pDevice,
    const D3D12_DESCRIPTOR_HEAP_DESC* pDesc,
    DescriptorPool** ppPool
) {
    // �����`�F�b�N
    if (pDevice == nullptr || pDesc == nullptr || ppPool == nullptr){
        return false;
    }

    // �C���X�^���X����
    auto instance = new (std::nothrow) DescriptorPool();
    if (instance == nullptr)
    {
        return false;
    }

    // �f�B�X�N���v�^�q�[�v�𐶐�.
    auto hr = pDevice->CreateDescriptorHeap(
        pDesc,
        IID_PPV_ARGS(instance->m_pHeap.GetAddressOf()));

    // ���s���������������s���ďI��
    if (FAILED(hr)){
        instance->Release();
        return false;
    }

    // �v�[����������
    if (!instance->m_pool.Init(pDesc->NumDescriptors)){
        instance->Release();
        return false;
    }

    // �f�B�X�N���v�^�̃C���N�������g�T�C�Y���擾
    instance->m_descriptorSize =
        pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

    // �C���X�^���X��ݒ�
    *ppPool = instance;

    // ����I��
    return true;

}


