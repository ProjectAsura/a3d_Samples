//-------------------------------------------------------------------------------------------------
// File : SampleUtil.h
// Desc : Sample Utility.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <a3d.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// FixedSizeString
///////////////////////////////////////////////////////////////////////////////////////////////////
class FixedSizeString
{
public:
    FixedSizeString();
    FixedSizeString(const char* value);

    const char* c_str() const;
    size_t size() const;

    FixedSizeString& operator =  (const FixedSizeString& value);
    FixedSizeString& operator =  (const char* value);
    FixedSizeString& operator += (const FixedSizeString& value);
    FixedSizeString& operator += (const char* value);
    FixedSizeString  operator +  (const FixedSizeString& value);
    FixedSizeString  operator +  (const char* value);

private:
    char m_Strings[256] = {};
};

//-------------------------------------------------------------------------------------------------
//! @brief      サンプルプログラムに使用するシェーダのディレクトリパスを取得します.
//-------------------------------------------------------------------------------------------------
void GetShaderPath(const char* name, FixedSizeString& result);

//-------------------------------------------------------------------------------------------------
//! @brief      サンプルプログラムに使用するテクスチャのディレクトリパスを取得します.
//-------------------------------------------------------------------------------------------------
void GetTexturePath(const char* name, FixedSizeString& result);

//-------------------------------------------------------------------------------------------------
//! @brief      シェーダバイナリを読み込みます.
//!
//! @param[in]      filename    ファイル名です.
//! @param[out]     result      シェーダバイナリの格納先です.
//! @retval true    読み込みに成功.
//! @retval false   読み込みに失敗.
//-------------------------------------------------------------------------------------------------
bool LoadShaderBinary(const char* filename, a3d::ShaderBinary& result);

//-------------------------------------------------------------------------------------------------
//! @brief      シェーダバイナリを破棄します.
//!
//! @param[in]      binary      破棄するシェーダバイナリです.
//-------------------------------------------------------------------------------------------------
void DisposeShaderBinary(a3d::ShaderBinary& binary);
