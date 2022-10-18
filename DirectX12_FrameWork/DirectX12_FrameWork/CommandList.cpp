/*****************************************************************//**
 * \file   CommandList.cpp
 * \brief  �R�}���h���X�g�N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

 //-----------------------------------------------------------------------------
 // Includes.
 //-----------------------------------------------------------------------------
#include "CommandList.h"

//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
CommandList::CommandList()
:m_pCmdList(nullptr)
,m_pAllocators()
,m_index(0)
{ /*Do Nothing */ }

//-----------------------------------------------------------------------------
// �f�X�g���N�^
//-----------------------------------------------------------------------------
CommandList::~CommandList()
{ Term();}

//-----------------------------------------------------------------------------
// ����������
//-----------------------------------------------------------------------------
bool CommandList::Init(	
ID3D12Device* pDevice, 
D3D12_COMMAND_LIST_TYPE type, 
uint32_t count
){
    if (pDevice == nullptr || count == 0)
    { return false;}

    m_pAllocators.resize(count);

    //�A���P�[�^�̐���
    for (auto i = 0u; i < count; ++i){
        auto hr = pDevice->CreateCommandAllocator(
            type, IID_PPV_ARGS(m_pAllocators[i].GetAddressOf()));
        if (FAILED(hr)){
            return false;
        }
    }

   // �R�}���h���X�g�̐���
   auto hr = pDevice->CreateCommandList(
       1,
       type,
       m_pAllocators[0].Get(),
       nullptr,
       IID_PPV_ARGS(m_pCmdList.GetAddressOf()));
   if (FAILED(hr)){
       return false;
   }

   //�J���ꂽ��ԂŐ��������̂ŕ���
   m_pCmdList->Close();
   

    m_index = 0;
    return true;
}

//-----------------------------------------------------------------------------
//  �I������
//-----------------------------------------------------------------------------
void CommandList::Term(){
    m_pCmdList.Reset();

    for (size_t i = 0; i < m_pAllocators.size(); ++i)
    {
        m_pAllocators[i].Reset();
    }

    m_pAllocators.clear();
    m_pAllocators.shrink_to_fit();
}

//-----------------------------------------------------------------------------
//  ���Z�b�g�������s�����R�}���h���X�g���擾����
//-----------------------------------------------------------------------------
ID3D12GraphicsCommandList* CommandList::GetListReset()
{
    auto hr = m_pAllocators[m_index]->Reset();
    if (FAILED(hr)){
        return nullptr;
    }

    hr = m_pCmdList->Reset(m_pAllocators[m_index].Get(), nullptr);
    if (FAILED(hr)){
        return nullptr;
    }

    m_index = (m_index + 1) % uint32_t(m_pAllocators.size());
    return m_pCmdList.Get();
}
