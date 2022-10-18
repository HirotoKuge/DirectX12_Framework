/*****************************************************************//**
 * \file   Game.h
 * \brief  �Q�[���S�̂̊Ǘ��N���X
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/
#pragma once
#include<iostream>
#include<string>
#include<memory>
#include"SceneManager.h"


//�Q�[���Ǘ��N���X
class Game{
public:
	Game(){};
	//����������
	bool Init();
	//�Q�[�����[�v�����s����
	void RunLoop();
	//�Q�[���̏I������
	void Shutdown();


public:
	void SetActiveSceneKey(std::string);

private:
	std::unique_ptr<SceneManager> m_pSceneManager;	//�V�[���}�l�[�W���[
	std::string m_activeSceneKey;	//���݃A�N�e�B�u�ȃV�[���̃p�X(�L�[)


};

