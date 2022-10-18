#include "Component.h"
#include "Actor.h"

Component::Component(Actor* owner, int updateOrder)
	:mOwner(owner)
	, mUpdateOrder(updateOrder){
	//保有アクター(Owner)に自身を追加する
	//mOwner->AddComponent(this);
}

Component::~Component(){
	//mOwner->RemoveComponent(this);
}

void Component::Update(float deltaTime){
}