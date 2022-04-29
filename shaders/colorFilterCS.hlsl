//-------------------------------------------------------------------------------------------------
// File : colorFilterCS.hlsl
// Desc : Color Filter.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#include "a3dShader.hlsli"

A3D_RESOURCE(Texture2D     ColorMap,  t0, 0);
A3D_RESOURCE(SamplerState  ColorSmp,  s0, 1);
A3D_RESOURCE(RWTexture2D<float4>    ResultMap, u0, 2);

A3D_RESOURCE(cbuffer CbColorFilter, b0, 3)
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
