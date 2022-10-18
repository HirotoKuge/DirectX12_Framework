/*****************************************************************//**
 * \file   ConstantBuffer.cpp
 * \brief  �萔�o�b�t�@�N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "ConstantBuffer.h"
#include "DescriptorPool.h"

//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer()
: m_pCB			(nullptr)
, m_pHandle		(nullptr)
, m_pPool		(nullptr)
, m_pMappedPtr	(nullptr) 
{ /* Do Nothing */ }

//-----------------------------------------------------------------------------
// �f�X�g���N�^
//-----------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{ Term(); };

//-----------------------------------------------------------------------------
// ����������
//-----------------------------------------------------------------------------
bool ConstantBuffer::Init(
ID3D12Device*	pDevice,
DescriptorPool* pPool,
size_t          size
) {
	if (pDevice == nullptr || pPool == nullptr || size == 0) 
	{ return false; }
	
	assert(m_pPool == nullptr);
	assert(m_pHandle == nullptr);

	m_pPool = pPool;
	m_pPool->AddRef();

	size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	UINT64 sizeAligned = (size + (align - 1)) & ~(align - 1); // align�ɐ؂�グ��.

	// �q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type					= D3D12_HEAP_TYPE_UPLOAD;			// �q�[�v�^�C�v
	prop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// GPU�y�[�W�v���p�e�B
	prop.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;		// �������v�[��
	prop.CreationNodeMask		= 1;								// ����GPU�̎��̘b�Ȃ̂�1�ł悢
	prop.VisibleNodeMask		= 1;								// ����GPU�̎��̘b�Ȃ̂�1�ł悢

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;	// ���\�[�X�̎���
	desc.Alignment				= 0;								// ���\�[�X�̃A���C�����g
	desc.Width					= sizeAligned;						// �o�b�t�@�T�C�Y
	desc.Height					= 1;								// �o�b�t�@�̏ꍇ�͂P
	desc.DepthOrArraySize		= 1;								// �o�b�t�@�̏ꍇ�͂P
	desc.MipLevels				= 1;								// �o�b�t�@�̏ꍇ�͂P
	desc.Format					= DXGI_FORMAT_UNKNOWN;				// �o�b�t�@�̏ꍇ��UNKNOWN
	desc.SampleDesc.Count		= 1;								// �o�b�t�@�̏ꍇ�͂P
	desc.SampleDesc.Quality		= 0;								// �o�b�t�@�̏ꍇ�͂O
	desc.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// �e�N�X�`�����C�A�E�g
	desc.Flags					= D3D12_RESOURCE_FLAG_NONE;			// �t���O

	// ���\�[�X�𐶐�.
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pCB.GetAddressOf()));
	if (FAILED(hr))
	{ return false; }

	// �������}�b�s���O
	hr = m_pCB->Map(0, nullptr, &m_pMappedPtr);

	m_desc.BufferLocation = m_pCB->GetGPUVirtualAddress();
	m_desc.SizeInBytes = UINT(sizeAligned);
	m_pHandle = pPool->AllocHandle();

	pDevice->CreateConstantBufferView(&m_desc, m_pHandle->HandleCPU);

	return true;
}

//-----------------------------------------------------------------------------
// �I������
//-----------------------------------------------------------------------------
void ConstantBuffer::Term() {
	// �������}�b�s���O����������,�萔�o�b�t�@���������
	if (m_pCB != nullptr) {
		m_pCB->Unmap(0, nullptr);
		m_pCB.Reset();
	}

	// �r���[��j��
	if (m_pPool != nullptr) {
		m_pPool->FreeHandle(m_pHandle);
		m_pHandle = nullptr;
	}

	// �f�B�X�N���v�^�v�[�������
	if (m_pPool != nullptr) {
		m_pPool->Release();
		m_pPool = nullptr;
	}

	m_pMappedPtr = nullptr;
}

//-----------------------------------------------------------------------------
// GPU���z�A�h���X���擾
//-----------------------------------------------------------------------------
D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress()const
{ return m_desc.BufferLocation; }

//-----------------------------------------------------------------------------
// CPU�f�B�X�N���v�^�n���h�����擾
//-----------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleCPU()const
{ return m_pHandle->HandleCPU; }

//-----------------------------------------------------------------------------
// GPU�f�B�X�N���v�^�n���h�����擾
//-----------------------------------------------------------------------------
D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::GetHandleGPU()const
{ return m_pHandle->HandleGPU; }

//-----------------------------------------------------------------------------
// �������}�b�s���O�ς݂̃|�C���^���擾
//-----------------------------------------------------------------------------
void* ConstantBuffer::GetPtr()const 
{ return m_pMappedPtr; }