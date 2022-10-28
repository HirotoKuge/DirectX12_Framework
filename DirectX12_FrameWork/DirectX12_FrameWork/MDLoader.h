/*****************************************************************//**
 * \file   MDLoader.h
 * \brief  バイナリモデルデータ読み込みクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once
#include <string>
#include "SharedStruct.h"

//-----------------------------------------------------------------------------
// MDLoader Class
//-----------------------------------------------------------------------------
class MDLoader{
public:
	/*****************************************************************//**
	 * \brief .mdファイル読み込み用関数
	 *  
	 * \param 読み込んだデータの格納先
	 * \param 読み込むファイルパス
	 * \return true:正常終了
	 * \return false:エラー
	 *
	 * \note .mdはバイナリ形式のモデルデータです 
	 *********************************************************************/
	bool LoadMD(std::vector<ResMesh>&, std::string);
	bool LoadMD(std::vector<ResMesh>&, std::wstring);
	
	/*****************************************************************//**
	 * \brief .mtファイル読みこみ関数
	 *  
	 * \param 読みこんだデータの格納先
	 * \param 読みこむファイルパス
	 * \return true:正常終了
	 * \return false:エラー
	 * 
	 * \note .mtはバイナリ形式のマテリアルデータです
	 *********************************************************************/
	bool LoadMT(std::vector<ResMaterial>&,std::string);
	bool LoadMT(std::vector<ResMaterial>&,std::wstring);

};

