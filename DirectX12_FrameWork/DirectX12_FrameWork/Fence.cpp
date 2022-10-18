/*****************************************************************//**
 * \file   Fence.cpp
 * \brief  �t�F���X�N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

 //-----------------------------------------------------------------------------
 // Includes
 //-----------------------------------------------------------------------------
#include "Fence.h"

//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
Fence::Fence()
:m_pFence(nullptr)
, m_event(nullptr)
, m_counter(0) 
{ /* Do Nothing */ }

//-----------------------------------------------------------------------------
// �f�X�g���N�^
//----------------------------------------------------------------------------
Fence::~Fence()
{ Term(); };

bool Fence::Init(ID3D12Device* pDevice) {
	// �����`�F�b�N
	if (pDevice == nullptr) 
	{ return false; }

	// �C�x���g�̐���
	m_event = CreateEventEx(nullptr,FALSE,FALSE,EVENT_ALL_ACCESS);
	if (m_event == nullptr)
	{ return false; }

	// �t�F���X�𐶐�
	auto hr = pDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_pFence.GetAddressOf()));
	if(FAILED(hr))
	{ return false; }

	// �J�E���^�ݒ�
	m_counter = 1;

	// ����I��
	return true;
}

//-----------------------------------------------------------------------------
// �I������
//-----------------------------------------------------------------------------
void Fence::Term() {
	// �n���h�������
	if (m_event != nullptr) {
		CloseHandle(m_event);
		m_event = nullptr;
	}

	// �t�F���X�I�u�W�F�N�g�̔j��
	m_pFence.Reset();

	// �J�E���^�[���Z�b�g
	m_counter = 0;
}

//-----------------------------------------------------------------------------
// �V�O�i����ԂɂȂ�܂Ŏw�莞�ԑҋ@
//-----------------------------------------------------------------------------
void Fence::Wait(ID3D12CommandQueue* pQueue, UINT timeout_ms) {
	if (pQueue == nullptr) 
	{ return; }

	const auto fenceValue = m_counter;

	// �V�O�i������
	auto hr = pQueue->Signal(m_pFence.Get(),fenceValue);
	if (FAILED(hr)) 
	{ return; }

	//�J�E���^�[�𑝂₷
	m_counter++;

	//���̃t���[���̕`�揀�����܂��Ȃ�Αҋ@
	if (m_pFence->GetCompletedValue() < fenceValue) {
		// �Ǘ��̎��ɃC�x���g��ݒ�
		auto hr = m_pFence->SetEventOnCompletion( fenceValue, m_event);
		if (FAILED(hr)) 
		{ return; }

		//�ҋ@����
		if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_event, timeout_ms, FALSE))
		{ return;}
	}
}

//-----------------------------------------------------------------------------
//      �V�O�i����ԂɂȂ�܂ł����Ƒҋ@
//-----------------------------------------------------------------------------
void Fence::Sync(ID3D12CommandQueue* pQueue){
	if (pQueue == nullptr)
	{ return; }

	// �V�O�i������.
	auto hr = pQueue->Signal(m_pFence.Get(), m_counter);
	if (FAILED(hr))
	{ return; }

	// �������ɃC�x���g��ݒ�.
	hr = m_pFence->SetEventOnCompletion(m_counter, m_event);
	if (FAILED(hr))
	{ return; }

	// �ҋ@����.
	if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_event, INFINITE, FALSE))
	{ return; }

	// �J�E���^�[�𑝂₷.
	m_counter++;
}

