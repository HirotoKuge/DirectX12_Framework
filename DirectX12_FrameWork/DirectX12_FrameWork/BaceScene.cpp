#include "BaceScene.h"
#include "GraphicsEngine.h"
#include "App.h"
#include <d3dx12.h>
#include<memory>
#include <filesystem>
#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "AssimpLoader.h"




// �g���q��u�������鏈��
namespace fs = std::filesystem;
std::wstring ReplaceExtension(const std::wstring& origin, const char* ext){
	fs::path p = origin.c_str();
	return p.replace_extension(ext).c_str();
}


Scene* g_Scene;

using namespace DirectX;


float rotateY = 0.0f;


bool Scene::Init(){

	printf("�V�[���̏������ɐ���\n");

	return true;
}

void Scene::Update(){

}

void Scene::Draw(){

}
