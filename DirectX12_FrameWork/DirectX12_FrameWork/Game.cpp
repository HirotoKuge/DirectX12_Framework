#include "Game.h"
#include "GraphicsEngine.h"


bool Game::Init(){
	//シーンの登録

	

	return true;
}


void Game::RunLoop(){
}


void Game::Shutdown(){
}


/******************************************************************************//**
 * \brief	アクティブなシーンのキーをセット
 * \param activePath
 **********************************************************************************/
void Game::SetActiveSceneKey(std::string activeKey) {
	m_activeSceneKey = activeKey;
}
