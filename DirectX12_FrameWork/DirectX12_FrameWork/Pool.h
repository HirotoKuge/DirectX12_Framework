/*****************************************************************//**
 * \file   Pool.h
 * \brief  プールクラス
 * \brief　この辺から注釈は英語で書き始めます
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
     * \brief コンストラクタ
     *********************************************************************/
    Pool()
    : m_pBuffer(nullptr)
    , m_pActive(nullptr)
    , m_pFree(nullptr)
    , m_Capacity(0)
    , m_Count(0)
    { /* Do Nothing. */ }

    /*****************************************************************//**
     * \brief デストラクタ
     *********************************************************************/
    ~Pool()
    { Term(); }
    

    /*****************************************************************//**
     * \brief 初期化処理
     * 
     * \param count 確保するアイテム数
     * \return ture:初期化に成功 
     * \return false:初期化に失敗
     *********************************************************************/
    bool Init(uint32_t count) {
        std::lock_guard<std::mutex> guard(m_Mutex);

        m_pBuffer = static_cast<uint8_t*>(malloc(sizeof(Item) * (count + 2)));
        if (m_pBuffer == nullptr)
        {
            return false;
        }

        m_Capacity = count;

        // インデックスを振る.
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
     * \brief 終了処理
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
     * \brief アイテム確保
     * 
     * \param func：ユーザーによる初期化処理　処理がない場合はnullptr
     * \return 確保に成功した場合：確保したアイテムへのポインタ 
     * \return 確保に失敗した場合：nullptr
     *********************************************************************/
    T* AllocItem(std::function<void(uint32_t, T*)> func = nullptr) {
        std::lock_guard<std::mutex> guard(m_Mutex);

        //  これ以上確保できない場合の例外処理
        if (m_pFree->m_pNext == m_pFree || m_Count + 1 > m_Capacity){
            return nullptr;
        }

        auto item = m_pFree->m_pNext;
        m_pFree->m_pNext = item->m_pNext;

        item->m_pPrev = m_pActive->m_pPrev;
        item->m_pNext = m_pActive;
        item->m_pPrev->m_pNext = item->m_pNext->m_pPrev = item;

        m_Count++;

        // メモリ割り当て.
        auto val = new ((void*)item) T();

        // 初期化の必要があれば呼び出す.
        if (func != nullptr){
            func(item->m_Index, val);
        }

        return val;
    }

    /*****************************************************************//**
     * \brief アイテムの開放
     * 
     * \param pValue：開放するアイテムへのポインタ
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
     * \brief 総アイテム数の取得
     * 
     * \return 総アイテム数
     *********************************************************************/
    uint32_t GetSize() const
    { return m_Capacity; }

    /*****************************************************************//**
     * \brief　使用中のアイテム数の取得
     * 
     * \return 使用中のアイテム数
     *********************************************************************/
    uint32_t GetUsedCount() const
    { return m_Count; }

    /*****************************************************************//**
     * 利用可能なアイテム数の取得
     * 
     * \return 利用可能なアイテム数
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
        T           m_Value;    // 値
        uint32_t    m_Index;    // インデックス
        Item*       m_pNext;    // 次のアイテムへのポインタ
        Item*       m_pPrev;    // 前のアイテムへのポインタ

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
    
    uint8_t*    m_pBuffer;  //!< バッファ
    Item*       m_pActive;  //!< アクティブアイテムの先頭ポインタ
    Item*       m_pFree;    //!< フリーアイテムの先頭ポインタ
    uint32_t    m_Capacity; //!< 総アイテム数
    uint32_t    m_Count;    //!< 確保したアイテム数
    std::mutex  m_Mutex;    //!< ミューテックス
    
    //!}
    //------------------------------------------
    // private methods.
    //------------------------------------------
    //!{
   
    /*****************************************************************//**
     * \brief アイテムの取得
     * 
     * \param index：取得するアイテムのインデックス
     * \return アイテムへのポインタ
     *********************************************************************/
    Item* GetItem(uint32_t index){
        assert(0 <= index && index <= m_Capacity + 2);
        return reinterpret_cast<Item*>(m_pBuffer + sizeof(Item) * index);
    }

    /*****************************************************************//**
     * \brief アイテムにメモリを割り当てる
     *  
     * \param index：取得するアイテムへのインデックス
     * \return アイテムへのポインタ
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

