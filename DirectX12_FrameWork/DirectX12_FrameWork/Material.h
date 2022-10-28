/*****************************************************************//**
 * \file   Material.h
 * \brief  �}�e���A���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

 //-----------------------------------------------------------------------------
 // Includes
 //-----------------------------------------------------------------------------
 #include <map>

 #include "ConstantBuffer.h"
 #include "DescriptorPool.h"
 #include "Texture.h"


//-----------------------------------------------------------------------------
// Material Class
//-----------------------------------------------------------------------------
class Material{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
public:
	//=============================================================================
	// enum
	//=============================================================================
	// �e�N�X�`���̗p�r
	enum TEXTURE_USAGE{
		TEXTURE_USAGE_DIFFUSE = 0,	// �f�B�t���[�Y�}�b�v
		TEXTURE_USAGE_SPECULAR,		// �X�y�L�����}�b�v
		TEXTURE_USAGE_SHININESS,	// �V���C�l�X�}�b�v
		TEXTURE_USAGE_NORMAL,		// �@���}�b�v

		TEXTURE_USAGE_COUNT			// ��`��
	};


	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief �R���X�g���N�^
	 *********************************************************************/
	Material();

	/*****************************************************************//**
	 * \brief �f�X�g���N�^
	 *********************************************************************/
	~Material();
		
	/*****************************************************************//**
	 * \brief ����������
	 *  
	 * \param	pDevice		�f�o�C�X
	 * \param	pPool		�f�B�X�N���v�^�v�[��
	 * \param	bufferSize	�o�b�t�@�T�C�Y
	 * \param	count		�}�e���A����
	 * \return	true		�������ɐ���
	 * \return	false		�������Ɏ��s
	 *********************************************************************/
	bool Init(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		size_t bufferSize,
		size_t count);

	/*****************************************************************//**
	 * \brief �I������
	 *********************************************************************/
	void Term();

	/*****************************************************************//**
	 * \brief �e�N�X�`����ݒ�
	 *  
	 * \param	index	�}�e���A���ԍ�
	 * \param	usage	�e�N�X�`���̗p�r
	 * \param	path	�t�@�C���p�X
	 * \return	true	�������ɐ���
	 * \return	false	�������Ɏ��s
	 *********************************************************************/
	bool SetTexture(
		size_t index,
		TEXTURE_USAGE usage,
		const std::wstring& path);
	
	/*****************************************************************//**
	 * \brief �萔�o�b�t�@�̃|�C���^���擾
	 *  
	 * \param index	�}�e���A���ԍ�
	 * \return �w�肳�ꂽ�}�e���A���ԍ��ɑΉ�����萔�o�b�t�@�̃|�C���^
	 *		   ��v������̂��Ȃ��ꍇ�� nullptr
	 *********************************************************************/
	void* GetBufferPtr(size_t index)const;

	/*****************************************************************//**
	 * \brief �萔�o�b�t�@�̃|�C���^���擾
	 *
	 * \param index	�}�e���A���ԍ�
	 * \return �w�肳�ꂽ�}�e���A���ԍ��ɑΉ�����萔�o�b�t�@�̃|�C���^
	 *		   ��v������̂��Ȃ��ꍇ�� nullptr
	 *********************************************************************/
	template<typename T>
	T* GetBufferPtr(size_t index)const 
	{ return reinterpret_cast<T*>(GetBufferPtr(index)); }

	/*****************************************************************//**
	 * \brief �萔�o�b�t�@�̉��zGPU�A�h���X���擾
	 *  
	 * \param index	�}�e���A���ԍ�
	 * \return �w�肳�ꂽ�}�e���A���ԍ��ɑΉ�����萔�o�b�t�@��GPU���z�A�h���X
	 *********************************************************************/
	D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(size_t index)const;

	/*****************************************************************//**
	 * \brief �e�N�X�`���n���h�����擾
	 *  
	 * \param index �}�e���A���ԍ�
	 * \param usage �e�N�X�`���̗p�r�@
	 * \return ��v����e�N�X�`���̃f�B�X�N���v�^�n���h��
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(size_t index,TEXTURE_USAGE usage)const;

	/*****************************************************************//**
	 * \brief �}�e���A�������擾
	 *  
	 * \return �}�e���A����
	 *********************************************************************/
	size_t GetCount()const;

	//!}
private:
	//=============================================================================
	// structures
	//=============================================================================
	// �T�u�Z�b�g
	struct Subset {
		ConstantBuffer* pCostantBuffer;									// �萔�o�b�t�@
		D3D12_GPU_DESCRIPTOR_HANDLE TextureHandle[TEXTURE_USAGE_COUNT];	// �e�N�X�`���n���h��
	};

	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	std::map<std::wstring,Texture*> m_pTextures;	// �e�N�X�`��
	std::vector<Subset>				m_subsets;		// �T�u�Z�b�g
	ID3D12Device*					m_pDevice;		// �f�o�C�X
	DescriptorPool*					m_pPool;		// �f�B�X�N���v�^�v�[��
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	Material(const Material&) = delete;
	void operator = (const Material&) = delete;
	
	static const wchar_t* DummyTag;

	//!} 
};

constexpr auto TU_DIFFUSE = Material::TEXTURE_USAGE_DIFFUSE;
constexpr auto TU_SPECULAR = Material::TEXTURE_USAGE_SPECULAR;
constexpr auto TU_SHININESS = Material::TEXTURE_USAGE_SHININESS;
constexpr auto TU_NORMAL = Material::TEXTURE_USAGE_NORMAL;
