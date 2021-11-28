//-------------------------------------------------------------------------------------------------
// File : SampleUtil.cpp
// Desc : Sample Utility.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include "SampleUtil.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>


#if A3D_IS_WIN

//-------------------------------------------------------------------------------------------------
//      サンプルプログラム向けのシェーダディレクトリ名を取得します.
//-------------------------------------------------------------------------------------------------
std::string GetShaderPathForSampleProgram(const char* name)
{
    #if SAMPLE_IS_VULKAN
        return std::string("../shaders/vulkan_glsl/") + name + ".spv";
    #elif SAMPLE_IS_D3D12 || SAMPLE_IS_D3D11
        return std::string("../shaders/hlsl/") + name + ".cso";
    #endif
}

//-------------------------------------------------------------------------------------------------
//      サンプルプログラム向けのテクスチャディレクトリ名を取得します.
//-------------------------------------------------------------------------------------------------
std::string GetTexturePathForSampleProgram(const char* path)
{ return std::string("../textures/") + path; }

#endif//A3D_IS_WIN


//-------------------------------------------------------------------------------------------------
//      シェーダバイナリを読み込みます.
//-------------------------------------------------------------------------------------------------
bool LoadShaderBinary(const char* filename, a3d::ShaderBinary& result )
{
    FILE* pFile = nullptr;

    // ファイルを開きます.
    #if A3D_IS_WIN
        auto err = fopen_s( &pFile, filename, "rb" );
        if ( err != 0 )
        { return false; }
    #else
        pFile = fopen( filename, "r" );
        if ( pFile == nullptr )
        { return false; }
    #endif

    // ファイルサイズ産出.
    auto curpos = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    auto endpos = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    auto size = endpos - curpos;

    // メモリ確保.
    auto buf = malloc(size);
    if ( buf == nullptr )
    {
        fclose( pFile );
        return false;
    }

    // ファイル読み込み.
    fread( buf, 1, size, pFile );

    // データを設定.
    result.pByteCode    = buf;
    result.ByteCodeSize = size;

    // ファイルを閉じます.
    fclose( pFile );

    // 正常終了.
    return true;
}


//-------------------------------------------------------------------------------------------------
//      シェーダバイナリを破棄します.
//-------------------------------------------------------------------------------------------------
void DisposeShaderBinary(a3d::ShaderBinary& binary)
{
    if (binary.pByteCode != nullptr)
    { free(const_cast<void*>(binary.pByteCode)); }

    memset(&binary, 0, sizeof(binary));
}
