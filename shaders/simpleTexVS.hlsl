//-------------------------------------------------------------------------------------------------
// File : simpleVS.hlsl
// Desc : Simple Vertex Shader.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#include "spirvHelper.hlsli"

///////////////////////////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VSInput 
{
    LOCATION(0) float3 Position : POSITION;     // 位置座標です.
    LOCATION(5) float2 TexCoord : TEXCOORD;     // テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    LOCATION(0) float4 Position : SV_POSITION;  // 位置座標です.
    LOCATION(1) float2 TexCoord : TEXCOORD;     // テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Transform buffer
///////////////////////////////////////////////////////////////////////////////////////////////////
RESOURCE(cbuffer Transform, b0, 0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
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
    output.TexCoord = input.TexCoord;

    FLIP_Y(output.Position);
    return output;
}