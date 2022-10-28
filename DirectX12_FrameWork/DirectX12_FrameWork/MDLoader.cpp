/*****************************************************************//**
 * \file   MDLoader.cpp
 * \brief  バイナリモデルデータ読み込みクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#include "MDLoader.h"
#include "Logger.h"
#include <time.h>
#include <iostream>

//-----------------------------------------------------------------------------
// .md読み込み処理
//-----------------------------------------------------------------------------
bool MDLoader::LoadMD(std::vector<ResMesh>& dstMeshes, std::string filepath) {
	clock_t start = clock();
	//ファイルの奴
	FILE* fp = nullptr;

	//読みたいmdファイルを読み取り専用でオープン
	fp = fopen(filepath.c_str(), "rb");

	//メッシュ数を読み取り
	size_t meshNum = 0;
	fread(&meshNum, sizeof(size_t), 1, fp);

	//メッシュ数だけ回す
	for (size_t i = 0; i < meshNum; i++) {
		ResMesh toHouseMesh;//格納用のメッシュ

		//頂点数読み取り
		size_t vertexNum = 0;
		fread(&vertexNum, sizeof(size_t), 1, fp);

		//頂点データ読みだして格納
		for (size_t j = 0; j < vertexNum; j++) {
			MeshVertex tmp;
			fread(&tmp, sizeof(MeshVertex), 1, fp);
			toHouseMesh.Vertices.emplace_back(tmp);
		}

		//インデックス数読み取り
		size_t indexNum = 0;
		fread(&indexNum, sizeof(size_t), 1, fp);

		//インデックス読みだして格納
		for (size_t j = 0; j < indexNum; j++) {
			uint32_t tmp;
			fread(&tmp, sizeof(uint32_t), 1, fp);
			toHouseMesh.Indices.emplace_back(tmp);
		}

		//マテリアルインデックス読み取り
		uint32_t materialID = 0;
		fread(&materialID,sizeof(uint32_t),1,fp);
		toHouseMesh.matetialID=materialID;

		//配列に格納
		dstMeshes.emplace_back(toHouseMesh);
	}

	clock_t end = clock();
	
	double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.f;

	DLOG("required time [.md] : %f ms", time_ms);

	return true;
}

//-----------------------------------------------------------------------------
// .md読み込み処理 ワイド文字
//-----------------------------------------------------------------------------
bool MDLoader::LoadMD(std::vector<ResMesh>& dstMeshes, std::wstring filepath) {
	clock_t start = clock();
	//ファイルの奴
	FILE* fp = nullptr;

	//読みたいmdファイルを読み取り専用でオープン
	fp = _wfopen(filepath.c_str(), L"rb");

	//メッシュ数を読み取り
	size_t meshNum = 0;
	fread(&meshNum, sizeof(size_t), 1, fp);

	//メッシュ数だけ回す
	for (size_t i = 0; i < meshNum; i++) {
		ResMesh toHouseMesh;//格納用のメッシュ

		//頂点数読み取り
		size_t vertexNum = 0;
		fread(&vertexNum, sizeof(size_t), 1, fp);

		//頂点データ読みだして格納
		for (size_t j = 0; j < vertexNum; j++) {
			MeshVertex tmp;
			fread(&tmp, sizeof(MeshVertex), 1, fp);
			toHouseMesh.Vertices.emplace_back(tmp);
		}

		//インデックス数読み取り
		size_t indexNum = 0;
		fread(&indexNum, sizeof(size_t), 1, fp);

		//インデックス読みだして格納
		for (size_t j = 0; j < indexNum; j++) {
			uint32_t tmp;
			fread(&tmp, sizeof(uint32_t), 1, fp);
			toHouseMesh.Indices.emplace_back(tmp);
		}

		//マテリアルインデックス読み取り
		uint32_t materialID = 0;
		fread(&materialID, sizeof(uint32_t), 1, fp);
		toHouseMesh.matetialID = materialID;

		//配列に格納
		dstMeshes.emplace_back(toHouseMesh);
	}

	clock_t end = clock();

	double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.f;

	DLOG("required time [.md] : %f ms", time_ms);

	return true;
}

//-----------------------------------------------------------------------------
// .mt読み込み処理
//-----------------------------------------------------------------------------
bool MDLoader::LoadMT(std::vector<ResMaterial>& dstMaterials, std::string filepath) {
	clock_t start = clock();
	//ファイルの奴
	FILE* fp = nullptr;

	//読みたいmdファイルを読み取り専用でオープン
	fp = fopen(filepath.c_str(), "rb");

	//マテリアル数を読み取り
	size_t materialNum = 0;
	fread(&materialNum, sizeof(size_t), 1, fp);

	//マテリアル数分読み出し
	for (size_t i = 0; i < materialNum; i++) {
		ResMaterial toHouse;

		//拡散反射成分読み出し
		{
			DirectX::XMFLOAT3 diffuse;
			fread(&diffuse,sizeof(DirectX::XMFLOAT3),1,fp);
			toHouse.Diffuse = diffuse;
		}
		
		//鏡面反射成分読み出し
		{
			DirectX::XMFLOAT3 specular;
			fread(&specular, sizeof(DirectX::XMFLOAT3), 1, fp);
			toHouse.Specular = specular;
		}

		//透過成分読み出し
		{
			float alpha;
			fread(&alpha, sizeof(float), 1, fp);
			toHouse.Alpha = alpha;
		}

		//鏡面反射強度読み出し
		{
			float shininess;
			fread(&shininess, sizeof(float), 1, fp);
			toHouse.Shininess = shininess;
		}

		//ディフューズマップパス読み出し
		{
			//パスの長さ取得
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);
			
			//一文字ずつ読みだす
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.DiffuseMapPath += tmp;
			}
		}

		//スペキュラマップパス読み出し
		{
			//パスの長さ取得
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//一文字ずつ読みだす
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.SpecularMapPath += tmp;
			}
		}

		//シャイネスマップパス読み出し
		{
			//パスの長さ取得
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//一文字ずつ読みだす
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.SpecularMapPath += tmp;
			}
		}

		//法線マップパス読み出し
		{
			//パスの長さ取得
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//一文字ずつ読みだす
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.NormalMapPath += tmp;
			}
		}

		//配列に格納
		dstMaterials.emplace_back(toHouse);

	}

	clock_t end = clock();

	double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.f;

	DLOG("required time [.mt] : %f ms", time_ms);

	return true;
}

//-----------------------------------------------------------------------------
// .mt読み込み処理 ワイド文字
//-----------------------------------------------------------------------------
bool MDLoader::LoadMT(std::vector<ResMaterial>& dstMaterials, std::wstring filepath) {
	clock_t start = clock();
	//ファイルの奴
	FILE* fp = nullptr;

	//読みたいmdファイルを読み取り専用でオープン
	fp = _wfopen(filepath.c_str(), L"rb");

	//マテリアル数を読み取り
	size_t materialNum = 0;
	fread(&materialNum, sizeof(size_t), 1, fp);

	//マテリアル数分読み出し
	for (size_t i = 0; i < materialNum; i++) {
		ResMaterial toHouse;

		//拡散反射成分読み出し
		{
			DirectX::XMFLOAT3 diffuse;
			fread(&diffuse, sizeof(DirectX::XMFLOAT3), 1, fp);
			toHouse.Diffuse = diffuse;
		}

		//鏡面反射成分読み出し
		{
			DirectX::XMFLOAT3 specular;
			fread(&specular, sizeof(DirectX::XMFLOAT3), 1, fp);
			toHouse.Specular = specular;
		}

		//透過成分読み出し
		{
			float alpha;
			fread(&alpha, sizeof(float), 1, fp);
			toHouse.Alpha = alpha;
		}

		//鏡面反射強度読み出し
		{
			float shininess;
			fread(&shininess, sizeof(float), 1, fp);
			toHouse.Shininess = shininess;
		}

		//ディフューズマップパス読み出し
		{
			//パスの長さ取得
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//一文字ずつ読みだす
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.DiffuseMapPath += tmp;
			}
		}

		//スペキュラマップパス読み出し
		{
			//パスの長さ取得
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//一文字ずつ読みだす
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.SpecularMapPath += tmp;
			}
		}

		//シャイネスマップパス読み出し
		{
			//パスの長さ取得
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//一文字ずつ読みだす
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.SpecularMapPath += tmp;
			}
		}

		//法線マップパス読み出し
		{
			//パスの長さ取得
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//一文字ずつ読みだす
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.NormalMapPath += tmp;
			}
		}

		//配列に格納
		dstMaterials.emplace_back(toHouse);

	}

	clock_t end = clock();

	double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.f;

	DLOG("required time [.mt] : %f ms", time_ms);

	return true;
}
