/*****************************************************************//**
 * \file   BaceScene.h
 * \brief  シーンの基底クラス 
 * \brief　ここでシーンとは,ゲーム空間
 * \author Hiroto Kuge
 * \date   September 2022
 *********************************************************************/
#pragma once
class SceneManager;


class Scene{
public:
	Scene(SceneManager*);
	virtual bool Init() = 0; // 初期化
	virtual void Update() = 0; // 更新処理
	virtual void Draw() = 0; // 描画処理

protected:
	SceneManager* m_managerDI;	//依存性の注入のやつ


};

