//-------------------------------------------------------------------------------------------------
// File : simpleVS.hlsl
// Desc : Simple Vertex Shader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#include "a3dShader.hlsli"

///////////////////////////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VSInput 
{
    A3D_LOCATION(0) float3 Position : POSITION;     // 位置座標です.
    A3D_LOCATION(1) float4 Color    : COLOR0;       // 頂点カラーです.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    A3D_LOCATION(0) float4 Position : SV_POSITION;  // 位置座標です.
    A3D_LOCATION(1) float4 Color    : COLOR;        // 頂点カラーです.
};

//-------------------------------------------------------------------------------------------------
//      頂点シェーダメインエントリーポイントです.
//-------------------------------------------------------------------------------------------------
VSOutput main(const VSInput input)
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.Position, 1.0f);

    output.Position = localPos;
    output.Color    = input.Color;

    A3D_FLIP_Y(output.Position);
    return output;
}