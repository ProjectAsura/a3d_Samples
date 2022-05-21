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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Transform buffer
///////////////////////////////////////////////////////////////////////////////////////////////////
A3D_BINDING(0)
cbuffer Transform : register(b0)
{
    float4x4 World : packoffset(c0);
    float4x4 View  : packoffset(c4);
    float4x4 Proj  : packoffset(c8);
};

//-------------------------------------------------------------------------------------------------
//      頂点シェーダメインエントリーポイントです.
//-------------------------------------------------------------------------------------------------
VSOutput main(const VSInput input)
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.Position, 1.0f);
    
    float4 worldPos = mul(World, localPos);
    float4 viewPos  = mul(View,  worldPos);
    float4 projPos  = mul(Proj,  viewPos);

    output.Position = projPos;
    output.Color    = input.Color;

    A3D_FLIP_Y(output.Position);
    return output;
}