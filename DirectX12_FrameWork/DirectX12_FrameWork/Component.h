#pragma once
class Component{
public:
	//�R���X�g���N�^
	//updateOrder���������R���|�[�l���g�������X�V�����(�V���O���X���b�h)
	Component(class Actor* owner, int updateOrder);
	//�f�X�g���N�^
	virtual ~Component();
	//�f���^�^�C���ōX�V
	virtual void Update(float deltaTime);

	int GetUpdateOrder() const { return mUpdateOrder; }
protected:
	//���g��ۗL���Ă���A�N�^�[�̃|�C���^
	class Actor* mOwner;
	// �R���|�[�l���g�̍X�V����
	int mUpdateOrder;
};

