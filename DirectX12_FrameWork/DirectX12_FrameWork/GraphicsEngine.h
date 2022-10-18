/*****************************************************************//**
 * \file   GraphicsEngine.h
 * \brief  DirectX12�̕`��G���W��
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXTex.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include "ComPtr.h"
#include "DescriptorPool.h"
#include "ColorTarget.h"
#include "DepthTarget.h"
#include "CommandList.h"
#include "Fence.h"


//-----------------------------------------------------------------------------
// Linker.
//-----------------------------------------------------------------------------
#pragma comment( lib, "d3d12.lib") 
#pragma comment( lib, "dxgi.lib") 
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )



//DirectX12�̕`���ՂƂȂ�@�\���܂Ƃ߂��N���X
//�V���O���g���p�^�[���̎���
//���e��o�b�t�@�Ȃǂ���f�o�C�X�̃C���^�[�t�F�[�X�ɃA�N�Z�X�������ꍇ����������
//�����Ń|�C���^��n�������ł͉���GraphicsEngine�N���X���j�����ꂽ�Ƃ��ɃI�u�W�F�N�g�����m�ł��Ȃ�����
//���I�u�U�[�o�[�p�^�[����p����Ή\�ł͂��邪�Q�Ƃ������ӏ��������̂ŃV���O���g���̂ق��������I���Ɣ��f
class GraphicsEngine {
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
public:

	//-----------------------------------------------------------------------------
	// PoolType enum
	//-----------------------------------------------------------------------------
	enum POOL_TYPE {
		POOL_TYPE_RES	= 0,     // CBV / SRV / UAV
		POOL_TYPE_SMP	= 1,     // Sampler
		POOL_TYPE_RTV	= 2,     // RTV
		POOL_TYPE_DSV	= 3,     // DSV
		POOL_COUNT		= 4,
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
	 * \brief	�C���X�^���X�擾
	 * \return	�C���X�^���X�A�h���X
	 *********************************************************************/
	static GraphicsEngine* GetInstance() {
		static GraphicsEngine pInstance;
		return &pInstance;
	}

	/*****************************************************************//**
	 * \brief ����������
	 * \param hwnd			�E�B���h�E�n���h��
	 * \param windowWidth	�E�B���h�E��
	 * \param windowHeight	�E�B���h�E����
	 * \return true			���������s
	 * \return false		����������
	 *********************************************************************/
	bool Init(HWND hwnd, UINT windowWidth, UINT windowHeight);
	
	/*****************************************************************//**
	 * \brief �I������
	 *********************************************************************/
	void Term();

	/*****************************************************************//**
	 * \brief �`��O�̏���
	 *********************************************************************/
	void BeginRender();

	/*****************************************************************//**
	 * \brief �`��Ō�̏���
	 *********************************************************************/
	void EndRender();

	/*****************************************************************//**
	 * \brief	�f�o�C�X���擾
	 * \return	�f�o�C�X
	 *********************************************************************/
	ID3D12Device* GetDevice()const;

	/*****************************************************************//**
	 * \brief	���݂̃t���[���ԍ����擾
	 * \return	���݂̃t���[���ԍ�
	 *********************************************************************/
	uint32_t GetCurrentFrameIndex()const;

	/*****************************************************************//**
	 * \brief	�R�}���h���X�g���擾
	 * \return	�R�}���h���X�g
	 *********************************************************************/
	ID3D12GraphicsCommandList* GetCommandList()const;

	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�v�[�����擾
	 * \param poolType	�擾����f�B�X�N���v�^�v�[���̃^�C�v
	 * \return �f�B�X�N���v�^�v�[��
	 *********************************************************************/
	DescriptorPool* GetDiscriptaPool(POOL_TYPE poolType)const;

	//!}
private:

	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	static const uint32_t FrameCount = 2;   // �t���[���o�b�t�@��

	HINSTANCE   m_hInst;        // �C���X�^���X�n���h��
	HWND        m_hWnd;         // �E�B���h�E�n���h��
	uint32_t    m_Width;        // �E�B���h�E�̉���
	uint32_t    m_Height;       // �E�B���h�E�̏c��

	ComPtr<ID3D12Device>        m_pDevice;                  // �f�o�C�X
	ComPtr<ID3D12CommandQueue>  m_pQueue;                   // �R�}���h�L���[
	ComPtr<IDXGISwapChain3>     m_pSwapChain;               // �X���b�v�`�F�C��
	ColorTarget                 m_ColorTarget[FrameCount];  // �J���[�^�[�Q�b�g
	DepthTarget                 m_DepthTarget;              // �[�x�^�[�Q�b�g
	DescriptorPool*				m_pPool[POOL_COUNT];		// �f�B�X�N���v�^�v�[��
	CommandList                 m_CommandList;              // �R�}���h���X�g
	Fence                       m_Fence;                    // �t�F���X
	D3D12_VIEWPORT              m_Viewport;                 // �r���[�|�[�g
	D3D12_RECT                  m_Scissor;                  // �V�U�[��`


	uint32_t                    m_frameIndex;               // �t���[���ԍ�
	D3D_FEATURE_LEVEL			m_featureLevel;				// ���s���̃f�o�C�X�̑Ή����x��
	ID3D12GraphicsCommandList*	m_pNowCmdList;				// ���ݎg�p���̃R�}���h���X�g�ւ̃|�C���^

	//!} 
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	GraphicsEngine();
	
	//!} 
};