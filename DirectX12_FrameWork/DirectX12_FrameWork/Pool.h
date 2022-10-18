/*****************************************************************//**
 * \file   Pool.h
 * \brief  �v�[���N���X
 * \brief�@���̕ӂ��璍�߂͉p��ŏ����n�߂܂�
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

//-------------------------------
// inclides
//-------------------------------
#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>


//--------------------------
// pool class.
//--------------------------
template<typename T>
class Pool{
	//------------------------------------------
	// 	list of friend class and method.
	//------------------------------------------
	//!{
			/* 	nothing */
	//!}

public:
	//------------------------------------------
	// public variables.	
	//------------------------------------------	
	//!{
			/* nothing */
	//!} 

    //------------------------------------------
    // public methods.	
    //------------------------------------------	
    //!{
            
    /*****************************************************************//**
     * \brief �R���X�g���N�^
     *********************************************************************/
    Pool()
    : m_pBuffer(nullptr)
    , m_pActive(nullptr)
    , m_pFree(nullptr)
    , m_Capacity(0)
    , m_Count(0)
    { /* Do Nothing. */ }

    /*****************************************************************//**
     * \brief �f�X�g���N�^
     *********************************************************************/
    ~Pool()
    { Term(); }
    

    /*****************************************************************//**
     * \brief ����������
     * 
     * \param count �m�ۂ���A�C�e����
     * \return ture:�������ɐ��� 
     * \return false:�������Ɏ��s
     *********************************************************************/
    bool Init(uint32_t count) {
        std::lock_guard<std::mutex> guard(m_Mutex);

        m_pBuffer = static_cast<uint8_t*>(malloc(sizeof(Item) * (count + 2)));
        if (m_pBuffer == nullptr)
        {
            return false;
        }

        m_Capacity = count;

        // �C���f�b�N�X��U��.
        for (auto i = 2u, j = 0u; i < m_Capacity + 2; ++i, ++j)
        {
            auto item = GetItem(i);
            item->m_Index = j;
        }

        m_pActive = GetItem(0);
        m_pActive->m_pPrev = m_pActive->m_pNext = m_pActive;
        m_pActive->m_Index = uint32_t(-1);

        m_pFree = GetItem(1);
        m_pFree->m_Index = uint32_t(-2);

        for (auto i = 1u; i < m_Capacity + 2; ++i)
        {
            GetItem(i)->m_pPrev = nullptr;
            GetItem(i)->m_pNext = GetItem(i + 1);
        }

        GetItem(m_Capacity + 1)->m_pPrev = m_pFree;

        m_Count = 0;

        return true;
    }

    /*****************************************************************//**
     * \brief �I������
     *********************************************************************/
    void Term() {
        std::lock_guard<std::mutex> guard(m_Mutex);

        if (m_pBuffer)
        {
            free(m_pBuffer);
            m_pBuffer = nullptr;
        }

        m_pActive = nullptr;
        m_pFree = nullptr;
        m_Capacity = 0;
        m_Count = 0;
    }

    /*****************************************************************//**
     * \brief �A�C�e���m��
     * 
     * \param func�F���[�U�[�ɂ�鏉���������@�������Ȃ��ꍇ��nullptr
     * \return �m�ۂɐ��������ꍇ�F�m�ۂ����A�C�e���ւ̃|�C���^ 
     * \return �m�ۂɎ��s�����ꍇ�Fnullptr
     *********************************************************************/
    T* AllocItem(std::function<void(uint32_t, T*)> func = nullptr) {
        std::lock_guard<std::mutex> guard(m_Mutex);

        //  ����ȏ�m�ۂł��Ȃ��ꍇ�̗�O����
        if (m_pFree->m_pNext == m_pFree || m_Count + 1 > m_Capacity){
            return nullptr;
        }

        auto item = m_pFree->m_pNext;
        m_pFree->m_pNext = item->m_pNext;

        item->m_pPrev = m_pActive->m_pPrev;
        item->m_pNext = m_pActive;
        item->m_pPrev->m_pNext = item->m_pNext->m_pPrev = item;

        m_Count++;

        // ���������蓖��.
        auto val = new ((void*)item) T();

        // �������̕K�v������ΌĂяo��.
        if (func != nullptr){
            func(item->m_Index, val);
        }

        return val;
    }

    /*****************************************************************//**
     * \brief �A�C�e���̊J��
     * 
     * \param pValue�F�J������A�C�e���ւ̃|�C���^
     *********************************************************************/
    void FreeItem(T* pValue){
        if (pValue == nullptr){
            return;
        }

        std::lock_guard<std::mutex> guard(m_Mutex);

        auto item = reinterpret_cast<Item*>(pValue);

        item->m_pPrev->m_pNext = item->m_pNext;
        item->m_pNext->m_pPrev = item->m_pPrev;

        item->m_pPrev = nullptr;
        item->m_pNext = m_pFree->m_pNext;

        m_pFree->m_pNext = item;
        m_Count--;
    }

    /*****************************************************************//**
     * \brief ���A�C�e�����̎擾
     * 
     * \return ���A�C�e����
     *********************************************************************/
    uint32_t GetSize() const
    { return m_Capacity; }

    /*****************************************************************//**
     * \brief�@�g�p���̃A�C�e�����̎擾
     * 
     * \return �g�p���̃A�C�e����
     *********************************************************************/
    uint32_t GetUsedCount() const
    { return m_Count; }

    /*****************************************************************//**
     * ���p�\�ȃA�C�e�����̎擾
     * 
     * \return ���p�\�ȃA�C�e����
     *********************************************************************/
    uint32_t GetAvailableCount() const
    {  return m_Capacity - m_Count; }

	//!}
private:
    

    //------------------------------------------
    // Item struct
    //------------------------------------------
    struct Item
    {
        T           m_Value;    // �l
        uint32_t    m_Index;    // �C���f�b�N�X
        Item*       m_pNext;    // ���̃A�C�e���ւ̃|�C���^
        Item*       m_pPrev;    // �O�̃A�C�e���ւ̃|�C���^

        Item()
            : m_Value()
            , m_Index(0)
            , m_pNext(nullptr)
            , m_pPrev(nullptr)
        { /* Do Nothing */ }

        ~Item()
        { /* Do Nothing */ }
    };

    //------------------------------------------
    // private variables.
    //------------------------------------------
    //!{
    
    uint8_t*    m_pBuffer;  //!< �o�b�t�@
    Item*       m_pActive;  //!< �A�N�e�B�u�A�C�e���̐擪�|�C���^
    Item*       m_pFree;    //!< �t���[�A�C�e���̐擪�|�C���^
    uint32_t    m_Capacity; //!< ���A�C�e����
    uint32_t    m_Count;    //!< �m�ۂ����A�C�e����
    std::mutex  m_Mutex;    //!< �~���[�e�b�N�X
    
    //!}
    //------------------------------------------
    // private methods.
    //------------------------------------------
    //!{
   
    /*****************************************************************//**
     * \brief �A�C�e���̎擾
     * 
     * \param index�F�擾����A�C�e���̃C���f�b�N�X
     * \return �A�C�e���ւ̃|�C���^
     *********************************************************************/
    Item* GetItem(uint32_t index){
        assert(0 <= index && index <= m_Capacity + 2);
        return reinterpret_cast<Item*>(m_pBuffer + sizeof(Item) * index);
    }

    /*****************************************************************//**
     * \brief �A�C�e���Ƀ����������蓖�Ă�
     *  
     * \param index�F�擾����A�C�e���ւ̃C���f�b�N�X
     * \return �A�C�e���ւ̃|�C���^
     *********************************************************************/
    Item* AssignItem(uint32_t index){
        assert(0 <= index && index <= m_Capacity + 2);
        auto buf = (m_pBuffer + sizeof(Item) * index);
        return new (buf) Item;
    }

    Pool(const Pool&) = delete;
    void operator = (const Pool&) = delete;

    //!}
};

