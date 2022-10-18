#include "Game.h"
#include "SceneManager.h"


SceneManager::SceneManager(Game* di) {
	m_pGameDI = di;
}

bool SceneManager::ChangeScene(std::string scenePath){
	return true;
}

/******************************************************************************//**
 * ￥brief シーンの登録
 * \param scenePath:コンテナに登録するキーとなる文字列
 * \param createScene:登録するシーンクラス(make_unique関数)
 * \return 生成に成功:true/失敗:false
 **********************************************************************************/
bool SceneManager::RegistScene(std::string scenePath, std::unique_ptr<Scene> createScene) {
	m_sceneList.insert_or_assign(scenePath, std::move(createScene));



	return true;
}

/**************************************************//**
 * ￥brief 引数で指定したキーからシーンを取得する
 * \param scenePath
 * \return シーンのインスタンスアドレス/存在しなければnullptr
 ******************************************************/
[[nodiscard]] Scene* SceneManager::GetScene(std::string scenePath){
	
	//指定したキーの要素があるか確認
	if (m_sceneList.contains(scenePath)) {
		auto p_scene = m_sceneList[scenePath].get();
		return p_scene;
	}
	//無ければnullptrを返す
	else {
		return nullptr;
	}
	
}
