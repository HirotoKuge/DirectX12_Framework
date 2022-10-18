/*****************************************************************//**
 * \file   BaceScene.h
 * \brief  �V�[���̊��N���X 
 * \brief�@�����ŃV�[���Ƃ�,�Q�[�����
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/
#pragma once
class SceneManager;


class Scene{
public:
	Scene(SceneManager*);
	virtual bool Init() = 0; // ������
	virtual void Update() = 0; // �X�V����
	virtual void Draw() = 0; // �`�揈��

protected:
	SceneManager* m_managerDI;	//�ˑ����̒����̂��


};

