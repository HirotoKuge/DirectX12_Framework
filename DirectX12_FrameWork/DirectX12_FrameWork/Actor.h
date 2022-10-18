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


//ゲーム世界に存在するオブジェクトのベースのクラス
//今回はモノリシックな構造になりがちなオブジェクト指向のクラスをベースに
//基本的にオブジェクトに最低限必要なmethodは継承で引き継ぎ
//その他共通する追加機能はコンポーネントによって実装する
class Actor{
public:
	~Actor(){};
	bool Init();
	void Draw();

protected:

};




