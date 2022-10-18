#pragma once
#include<iostream>
#include<vector>
#include<DirectXMath.h>
#include <memory>
#include "GraphicsEngine.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"



enum class STATE {
	Active,
	Paused,
	Dead
};


//�Q�[�����E�ɑ��݂���I�u�W�F�N�g�̃x�[�X�̃N���X
//����̓��m���V�b�N�ȍ\���ɂȂ肪���ȃI�u�W�F�N�g�w���̃N���X���x�[�X��
//��{�I�ɃI�u�W�F�N�g�ɍŒ���K�v��method�͌p���ň����p��
//���̑����ʂ���ǉ��@�\�̓R���|�[�l���g�ɂ���Ď�������
class Actor{
public:
	~Actor(){};
	bool Init();
	void Draw();

protected:

};




