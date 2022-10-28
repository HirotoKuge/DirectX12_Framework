/*****************************************************************//**
 * \file   MDLoader.h
 * \brief  �o�C�i�����f���f�[�^�ǂݍ��݃N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once
#include <string>
#include "SharedStruct.h"

//-----------------------------------------------------------------------------
// MDLoader Class
//-----------------------------------------------------------------------------
class MDLoader{
public:
	/*****************************************************************//**
	 * \brief .md�t�@�C���ǂݍ��ݗp�֐�
	 *  
	 * \param �ǂݍ��񂾃f�[�^�̊i�[��
	 * \param �ǂݍ��ރt�@�C���p�X
	 * \return true:����I��
	 * \return false:�G���[
	 *
	 * \note .md�̓o�C�i���`���̃��f���f�[�^�ł� 
	 *********************************************************************/
	bool LoadMD(std::vector<ResMesh>&, std::string);
	bool LoadMD(std::vector<ResMesh>&, std::wstring);
	
	/*****************************************************************//**
	 * \brief .mt�t�@�C���ǂ݂��݊֐�
	 *  
	 * \param �ǂ݂��񂾃f�[�^�̊i�[��
	 * \param �ǂ݂��ރt�@�C���p�X
	 * \return true:����I��
	 * \return false:�G���[
	 * 
	 * \note .mt�̓o�C�i���`���̃}�e���A���f�[�^�ł�
	 *********************************************************************/
	bool LoadMT(std::vector<ResMaterial>&,std::string);
	bool LoadMT(std::vector<ResMaterial>&,std::wstring);

};

