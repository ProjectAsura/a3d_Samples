//-------------------------------------------------------------------------------------------------
// File : colorFilterCS.hlsl
// Desc : Color Filter.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#include "a3dShader.hlsli"

A3D_BINDING(0) Texture2D           ColorMap  : register(t0);
A3D_BINDING(1) SamplerState        ColorSmp  : register(s0);
A3D_BINDING(2) RWTexture2D<float4> ResultMap : register(u0);

A3D_BINDING(3)
cbuffer CbColorFilter : register(b0)
{
    float2   TargetSize;
    uint2    Reserved;
    float4x4 ColorMatrix;
};

[numthreads(8, 8, 1)]
void main(uint3 dispatchId : SV_DispatchThreadID)
{
     if (any(dispatchId.xy >= (uint2)TargetSize) )
     { return; }
     
     float2 uv = (float2)dispatchId.xy / TargetSize;
     
     float4 color = ColorMap.SampleLevel(ColorSmp, uv, 0.0f);
     color = mul(color, ColorMatrix);
     color = max(color, float(0.0).xxxx);

     ResultMap[dispatchId.xy] = color;
}
