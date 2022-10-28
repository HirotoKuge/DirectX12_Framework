/*****************************************************************//**
 * \file   MDLoader.cpp
 * \brief  �o�C�i�����f���f�[�^�ǂݍ��݃N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#include "MDLoader.h"
#include "Logger.h"
#include <time.h>
#include <iostream>

//-----------------------------------------------------------------------------
// .md�ǂݍ��ݏ���
//-----------------------------------------------------------------------------
bool MDLoader::LoadMD(std::vector<ResMesh>& dstMeshes, std::string filepath) {
	clock_t start = clock();
	//�t�@�C���̓z
	FILE* fp = nullptr;

	//�ǂ݂���md�t�@�C����ǂݎ���p�ŃI�[�v��
	fp = fopen(filepath.c_str(), "rb");

	//���b�V������ǂݎ��
	size_t meshNum = 0;
	fread(&meshNum, sizeof(size_t), 1, fp);

	//���b�V����������
	for (size_t i = 0; i < meshNum; i++) {
		ResMesh toHouseMesh;//�i�[�p�̃��b�V��

		//���_���ǂݎ��
		size_t vertexNum = 0;
		fread(&vertexNum, sizeof(size_t), 1, fp);

		//���_�f�[�^�ǂ݂����Ċi�[
		for (size_t j = 0; j < vertexNum; j++) {
			MeshVertex tmp;
			fread(&tmp, sizeof(MeshVertex), 1, fp);
			toHouseMesh.Vertices.emplace_back(tmp);
		}

		//�C���f�b�N�X���ǂݎ��
		size_t indexNum = 0;
		fread(&indexNum, sizeof(size_t), 1, fp);

		//�C���f�b�N�X�ǂ݂����Ċi�[
		for (size_t j = 0; j < indexNum; j++) {
			uint32_t tmp;
			fread(&tmp, sizeof(uint32_t), 1, fp);
			toHouseMesh.Indices.emplace_back(tmp);
		}

		//�}�e���A���C���f�b�N�X�ǂݎ��
		uint32_t materialID = 0;
		fread(&materialID,sizeof(uint32_t),1,fp);
		toHouseMesh.matetialID=materialID;

		//�z��Ɋi�[
		dstMeshes.emplace_back(toHouseMesh);
	}

	clock_t end = clock();
	
	double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.f;

	DLOG("required time [.md] : %f ms", time_ms);

	return true;
}

//-----------------------------------------------------------------------------
// .md�ǂݍ��ݏ��� ���C�h����
//-----------------------------------------------------------------------------
bool MDLoader::LoadMD(std::vector<ResMesh>& dstMeshes, std::wstring filepath) {
	clock_t start = clock();
	//�t�@�C���̓z
	FILE* fp = nullptr;

	//�ǂ݂���md�t�@�C����ǂݎ���p�ŃI�[�v��
	fp = _wfopen(filepath.c_str(), L"rb");

	//���b�V������ǂݎ��
	size_t meshNum = 0;
	fread(&meshNum, sizeof(size_t), 1, fp);

	//���b�V����������
	for (size_t i = 0; i < meshNum; i++) {
		ResMesh toHouseMesh;//�i�[�p�̃��b�V��

		//���_���ǂݎ��
		size_t vertexNum = 0;
		fread(&vertexNum, sizeof(size_t), 1, fp);

		//���_�f�[�^�ǂ݂����Ċi�[
		for (size_t j = 0; j < vertexNum; j++) {
			MeshVertex tmp;
			fread(&tmp, sizeof(MeshVertex), 1, fp);
			toHouseMesh.Vertices.emplace_back(tmp);
		}

		//�C���f�b�N�X���ǂݎ��
		size_t indexNum = 0;
		fread(&indexNum, sizeof(size_t), 1, fp);

		//�C���f�b�N�X�ǂ݂����Ċi�[
		for (size_t j = 0; j < indexNum; j++) {
			uint32_t tmp;
			fread(&tmp, sizeof(uint32_t), 1, fp);
			toHouseMesh.Indices.emplace_back(tmp);
		}

		//�}�e���A���C���f�b�N�X�ǂݎ��
		uint32_t materialID = 0;
		fread(&materialID, sizeof(uint32_t), 1, fp);
		toHouseMesh.matetialID = materialID;

		//�z��Ɋi�[
		dstMeshes.emplace_back(toHouseMesh);
	}

	clock_t end = clock();

	double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.f;

	DLOG("required time [.md] : %f ms", time_ms);

	return true;
}

//-----------------------------------------------------------------------------
// .mt�ǂݍ��ݏ���
//-----------------------------------------------------------------------------
bool MDLoader::LoadMT(std::vector<ResMaterial>& dstMaterials, std::string filepath) {
	clock_t start = clock();
	//�t�@�C���̓z
	FILE* fp = nullptr;

	//�ǂ݂���md�t�@�C����ǂݎ���p�ŃI�[�v��
	fp = fopen(filepath.c_str(), "rb");

	//�}�e���A������ǂݎ��
	size_t materialNum = 0;
	fread(&materialNum, sizeof(size_t), 1, fp);

	//�}�e���A�������ǂݏo��
	for (size_t i = 0; i < materialNum; i++) {
		ResMaterial toHouse;

		//�g�U���ː����ǂݏo��
		{
			DirectX::XMFLOAT3 diffuse;
			fread(&diffuse,sizeof(DirectX::XMFLOAT3),1,fp);
			toHouse.Diffuse = diffuse;
		}
		
		//���ʔ��ː����ǂݏo��
		{
			DirectX::XMFLOAT3 specular;
			fread(&specular, sizeof(DirectX::XMFLOAT3), 1, fp);
			toHouse.Specular = specular;
		}

		//���ߐ����ǂݏo��
		{
			float alpha;
			fread(&alpha, sizeof(float), 1, fp);
			toHouse.Alpha = alpha;
		}

		//���ʔ��ˋ��x�ǂݏo��
		{
			float shininess;
			fread(&shininess, sizeof(float), 1, fp);
			toHouse.Shininess = shininess;
		}

		//�f�B�t���[�Y�}�b�v�p�X�ǂݏo��
		{
			//�p�X�̒����擾
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);
			
			//�ꕶ�����ǂ݂���
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.DiffuseMapPath += tmp;
			}
		}

		//�X�y�L�����}�b�v�p�X�ǂݏo��
		{
			//�p�X�̒����擾
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//�ꕶ�����ǂ݂���
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.SpecularMapPath += tmp;
			}
		}

		//�V���C�l�X�}�b�v�p�X�ǂݏo��
		{
			//�p�X�̒����擾
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//�ꕶ�����ǂ݂���
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.SpecularMapPath += tmp;
			}
		}

		//�@���}�b�v�p�X�ǂݏo��
		{
			//�p�X�̒����擾
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//�ꕶ�����ǂ݂���
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.NormalMapPath += tmp;
			}
		}

		//�z��Ɋi�[
		dstMaterials.emplace_back(toHouse);

	}

	clock_t end = clock();

	double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.f;

	DLOG("required time [.mt] : %f ms", time_ms);

	return true;
}

//-----------------------------------------------------------------------------
// .mt�ǂݍ��ݏ��� ���C�h����
//-----------------------------------------------------------------------------
bool MDLoader::LoadMT(std::vector<ResMaterial>& dstMaterials, std::wstring filepath) {
	clock_t start = clock();
	//�t�@�C���̓z
	FILE* fp = nullptr;

	//�ǂ݂���md�t�@�C����ǂݎ���p�ŃI�[�v��
	fp = _wfopen(filepath.c_str(), L"rb");

	//�}�e���A������ǂݎ��
	size_t materialNum = 0;
	fread(&materialNum, sizeof(size_t), 1, fp);

	//�}�e���A�������ǂݏo��
	for (size_t i = 0; i < materialNum; i++) {
		ResMaterial toHouse;

		//�g�U���ː����ǂݏo��
		{
			DirectX::XMFLOAT3 diffuse;
			fread(&diffuse, sizeof(DirectX::XMFLOAT3), 1, fp);
			toHouse.Diffuse = diffuse;
		}

		//���ʔ��ː����ǂݏo��
		{
			DirectX::XMFLOAT3 specular;
			fread(&specular, sizeof(DirectX::XMFLOAT3), 1, fp);
			toHouse.Specular = specular;
		}

		//���ߐ����ǂݏo��
		{
			float alpha;
			fread(&alpha, sizeof(float), 1, fp);
			toHouse.Alpha = alpha;
		}

		//���ʔ��ˋ��x�ǂݏo��
		{
			float shininess;
			fread(&shininess, sizeof(float), 1, fp);
			toHouse.Shininess = shininess;
		}

		//�f�B�t���[�Y�}�b�v�p�X�ǂݏo��
		{
			//�p�X�̒����擾
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//�ꕶ�����ǂ݂���
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.DiffuseMapPath += tmp;
			}
		}

		//�X�y�L�����}�b�v�p�X�ǂݏo��
		{
			//�p�X�̒����擾
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//�ꕶ�����ǂ݂���
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.SpecularMapPath += tmp;
			}
		}

		//�V���C�l�X�}�b�v�p�X�ǂݏo��
		{
			//�p�X�̒����擾
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//�ꕶ�����ǂ݂���
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.SpecularMapPath += tmp;
			}
		}

		//�@���}�b�v�p�X�ǂݏo��
		{
			//�p�X�̒����擾
			size_t pathLength;
			fread(&pathLength, sizeof(size_t), 1, fp);

			//�ꕶ�����ǂ݂���
			for (size_t i = 0; i < pathLength; i++) {
				wchar_t tmp;
				fread(&tmp, sizeof(wchar_t), 1, fp);
				toHouse.NormalMapPath += tmp;
			}
		}

		//�z��Ɋi�[
		dstMaterials.emplace_back(toHouse);

	}

	clock_t end = clock();

	double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.f;

	DLOG("required time [.mt] : %f ms", time_ms);

	return true;
}
