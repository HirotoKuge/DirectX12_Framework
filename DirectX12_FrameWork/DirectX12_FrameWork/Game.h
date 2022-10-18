/*****************************************************************//**
 * \file   Game.h
 * \brief  ゲーム全体の管理クラス
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/
#pragma once
#include<iostream>
#include<string>
#include<memory>
#include"SceneManager.h"


//ゲーム管理クラス
class Game{
public:
	Game(){};
	//初期化処理
	bool Init();
	//ゲームループを実行する
	void RunLoop();
	//ゲームの終了処理
	void Shutdown();


public:
	void SetActiveSceneKey(std::string);

private:
	std::unique_ptr<SceneManager> m_pSceneManager;	//シーンマネージャー
	std::string m_activeSceneKey;	//現在アクティブなシーンのパス(キー)


};

