/*****************************************************************//**
 * \file   SceneManager.h
 * \brief  シーン管理クラス
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
	SceneManager(class Game*);	//コンストラクタ
	bool ChangeScene(std::string); //シーンの切り替え
	bool RegistScene(std::string, std::unique_ptr<Scene>); //シーンの生成＆登録

	[[nodiscard]] Scene* GetScene(std::string);	//シーンの取得

private:
	std::unordered_map<std::string, std::unique_ptr<Scene>> m_sceneList;	//シーンを格納するコンテナ
	class Game* m_pGameDI;	//自身の依存先
};

