/*****************************************************************//**
 * \file   SceneManager.h
 * \brief  �V�[���Ǘ��N���X
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/
#pragma once
#include<iostream>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <string>
#include <memory>
#include "BaceScene.h"

class SceneManager{
public:
	SceneManager(class Game*);	//�R���X�g���N�^
	bool ChangeScene(std::string); //�V�[���̐؂�ւ�
	bool RegistScene(std::string, std::unique_ptr<Scene>); //�V�[���̐������o�^

	[[nodiscard]] Scene* GetScene(std::string);	//�V�[���̎擾

private:
	std::unordered_map<std::string, std::unique_ptr<Scene>> m_sceneList;	//�V�[�����i�[����R���e�i
	class Game* m_pGameDI;	//���g�̈ˑ���
};

