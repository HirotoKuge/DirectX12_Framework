/*****************************************************************//**
 * \file   FileUtil.h
 * \brief  ÉtÉ@ÉCÉãì«Ç›çûÇ›ï÷óòä÷êî
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <string>
#include <Shlwapi.h>


//-----------------------------------------------------------------------------
// Linker
//-----------------------------------------------------------------------------
#pragma comment( lib, "shlwapi.lib ")

/*****************************************************************//**
 * \brief 
 * \param filename
 * \param result
 * \return 
 *********************************************************************/
bool SearchFilePathA(const char* filename, std::string& result);

/*****************************************************************//**
 * \brief 
 * \param filename
 * \param result
 * \return 
 *********************************************************************/
bool SearchFilePathW(const wchar_t* filename, std::wstring& result);

/*****************************************************************//**
 * \brief 
 * \param path
 * \return 
 *********************************************************************/
std::string RemoveDirectoryPathA(const std::string& path);

/*****************************************************************//**
 * \brief 
 * \param path
 * \return 
 *********************************************************************/
std::wstring RemoveDirectoryPathW(const std::wstring& path);

/*****************************************************************//**
 * \brief 
 * \param path
 * \return 
 *********************************************************************/
std::wstring GetDirectoryPathW(const wchar_t* path);

/*****************************************************************//**
 * \brief 
 *  
 * \param filename
 * \param result
 * \return 
 *********************************************************************/
std::wstring GetFileExtension(const std::wstring& filePath);

std::wstring ReplaceExtension(const std::wstring& origin, const char* ext);

#if defined(UNICODE) || defined(_UNICODE)
inline bool SearchFilePath(const wchar_t* filename, std::wstring& result)
{
    return SearchFilePathW(filename, result);
}

inline std::wstring RemoveDirectoryPath(const std::wstring& path)
{
    return RemoveDirectoryPathW(path);
}

inline std::wstring GetDirectoryPath(const wchar_t* path)
{
    return GetDirectoryPathW(path);
}


#else
inline bool SearchFilePath(const char* filename, std::string& result)
{
    return SearchFilePathA(filename, result);
}

inline std::string RemoveDirectoryPath(const std::string& path)
{
    return RemoveDirectoryPathA(path);
}

inline std::string GetDirectoryPath(const char* path)
{
    return GetDirectoryPathA(path);
}
#endif//defined(UNICODE) || defined(_UNICODE)

