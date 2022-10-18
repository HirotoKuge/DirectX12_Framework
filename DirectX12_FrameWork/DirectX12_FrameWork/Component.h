#pragma once
class Component{
public:
	//コンストラクタ
	//updateOrderが小さいコンポーネント程早く更新される(シングルスレッド)
	Component(class Actor* owner, int updateOrder);
	//デストラクタ
	virtual ~Component();
	//デルタタイムで更新
	virtual void Update(float deltaTime);

	int GetUpdateOrder() const { return mUpdateOrder; }
protected:
	//自身を保有しているアクターのポインタ
	class Actor* mOwner;
	// コンポーネントの更新順序
	int mUpdateOrder;
};

