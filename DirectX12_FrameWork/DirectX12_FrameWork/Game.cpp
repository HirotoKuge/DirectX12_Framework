#include "Game.h"
#include "GraphicsEngine.h"


bool Game::Init(){
	//�V�[���̓o�^

	

	return true;
}


void Game::RunLoop(){
}


void Game::Shutdown(){
}


/******************************************************************************//**
 * \brief	�A�N�e�B�u�ȃV�[���̃L�[���Z�b�g
 * \param activePath
 **********************************************************************************/
void Game::SetActiveSceneKey(std::string activeKey) {
	m_activeSceneKey = activeKey;
}
