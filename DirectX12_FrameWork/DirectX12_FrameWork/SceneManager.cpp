#include "Game.h"
#include "SceneManager.h"


SceneManager::SceneManager(Game* di) {
	m_pGameDI = di;
}

bool SceneManager::ChangeScene(std::string scenePath){
	return true;
}

/******************************************************************************//**
 * ��brief �V�[���̓o�^
 * \param scenePath:�R���e�i�ɓo�^����L�[�ƂȂ镶����
 * \param createScene:�o�^����V�[���N���X(make_unique�֐�)
 * \return �����ɐ���:true/���s:false
 **********************************************************************************/
bool SceneManager::RegistScene(std::string scenePath, std::unique_ptr<Scene> createScene) {
	m_sceneList.insert_or_assign(scenePath, std::move(createScene));



	return true;
}

/**************************************************//**
 * ��brief �����Ŏw�肵���L�[����V�[�����擾����
 * \param scenePath
 * \return �V�[���̃C���X�^���X�A�h���X/���݂��Ȃ����nullptr
 ******************************************************/
[[nodiscard]] Scene* SceneManager::GetScene(std::string scenePath){
	
	//�w�肵���L�[�̗v�f�����邩�m�F
	if (m_sceneList.contains(scenePath)) {
		auto p_scene = m_sceneList[scenePath].get();
		return p_scene;
	}
	//�������nullptr��Ԃ�
	else {
		return nullptr;
	}
	
}
