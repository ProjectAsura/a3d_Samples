﻿//-----------------------------------------------------------------------------
// File : simpleRT.hlsl
// Desc : Simple Ray Tracing.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "a3dShader.hlsli"

#define VERTEX_STRIDE   (7 * 4)     // 1頂点あたりのサイズ.
#define INDEX_STRIDE    (3 * 4)     // 1ポリゴンあたりのインデックスサイズ.
#define COLOR_OFFSET    (3 * 4)     // 1頂点データの先頭からカラーデータまでのオフセット.

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------
typedef BuiltInTriangleIntersectionAttributes HitArgs;


///////////////////////////////////////////////////////////////////////////////
// SceneParam structure
///////////////////////////////////////////////////////////////////////////////
struct SceneParam
{
    float4x4    InvView;
    float4x4    InvViewProj;
};

///////////////////////////////////////////////////////////////////////////////
// RayPayload structure
///////////////////////////////////////////////////////////////////////////////
struct RayPayload
{
    float4 Color;
    float3 RayDir;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex structure
///////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    float3 Position;
    float4 Color;
};

//-----------------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------------
A3D_BINDING(0) RaytracingAccelerationStructure Scene         : register(t0);
A3D_BINDING(1) Texture2D<float4>               BackGround    : register(t1);
A3D_BINDING(2) ByteAddressBuffer               Indices       : register(t2);
A3D_BINDING(3) ByteAddressBuffer               Vertices      : register(t3);
A3D_BINDING(4) RWTexture2D<float4>             RenderTarget  : register(u0);
A3D_BINDING(5) ConstantBuffer<SceneParam>      SceneParam    : register(b0);
A3D_BINDING(6) SamplerState                    LinearSampler : register(s0);


//-----------------------------------------------------------------------------
//      プリミティブ番号を取得します.
//-----------------------------------------------------------------------------
uint3 GetIndices(uint triangleIndex)
{
    uint address = triangleIndex * INDEX_STRIDE;
    return Indices.Load3(address);
}

//-----------------------------------------------------------------------------
//      重心座標で補間した頂点データを取得します.
//-----------------------------------------------------------------------------
Vertex GetVertex(uint triangleIndex, float3 barycentrices)
{
    uint3 indices = GetIndices(triangleIndex);
    Vertex v = (Vertex)0;

    [unroll]
    for(uint i=0; i<3; ++i)
    {
        uint address = indices[i] * VERTEX_STRIDE;
        v.Position += asfloat(Vertices.Load3(address)) * barycentrices[i];

        address += COLOR_OFFSET;
        v.Color += asfloat(Vertices.Load4(address)) * barycentrices[i];
    }

    return v;
}

//-----------------------------------------------------------------------------
//      レイを求めます.
//-----------------------------------------------------------------------------
void CalcRay(float2 index, out float3 pos, out float3 dir)
{
    float4 orig   = float4(0.0f, 0.0f, 0.0f, 1.0f);           // カメラの位置.
    float4 screen = float4(-2.0f * index + 1.0f, 0.0f, 1.0f); // スクリーンの位置.

    orig   = mul(SceneParam.InvView,     orig);
    screen = mul(SceneParam.InvViewProj, screen);

    // w = 1 に射影.
    screen.xyz /= screen.w;

    // レイの位置と方向を設定.
    pos = orig.xyz;
    dir = normalize(screen.xyz - orig.xyz);
}


//-----------------------------------------------------------------------------
//      レイを生成します.
//-----------------------------------------------------------------------------
[shader("raygeneration")]
void OnGenerateRay()
{
    float2 index = (float2)DispatchRaysIndex() / (float2)DispatchRaysDimensions();

    // レイを生成.
    float3 rayOrig;
    float3 rayDir;
    CalcRay(index, rayOrig, rayDir);

    // ペイロード初期化.
    RayPayload payload;
    payload.RayDir = rayDir;
    payload.Color  = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // レイを設定.
    RayDesc ray;
    ray.Origin    = rayOrig;
    ray.Direction = rayDir;
    ray.TMin      = 1e-3f;
    ray.TMax      = 10000.0f;

    // レイを追跡.
    TraceRay(Scene, RAY_FLAG_NONE, ~0, 0, 1, 0, ray, payload);

    // レンダーターゲットに格納.
    RenderTarget[DispatchRaysIndex().xy] = payload.Color;
}

//-----------------------------------------------------------------------------
//      交差後の処理です.
//-----------------------------------------------------------------------------
[shader("closesthit")]
void OnClosestHit(inout RayPayload payload, in HitArgs args)
{
    // 重心座標を求める.
    float3 barycentrics = float3(
        1.0f - args.barycentrics.x - args.barycentrics.y,
        args.barycentrics.x,
        args.barycentrics.y);

    // プリミティブ番号取得.
    uint triangleIndex = PrimitiveIndex();

    // 頂点データ取得.
    Vertex v = GetVertex(triangleIndex, barycentrics);

    // カラーを格納.
    payload.Color = v.Color;
}

//-----------------------------------------------------------------------------
//      スフィアマップのテクスチャ座標を求めます.
//-----------------------------------------------------------------------------
float2 ToSphereMapCoord(float3 reflectDir)
{
    // https://www.clicktorelease.com/blog/creating-spherical-environment-mapping-shader/
    // Pierre Lepers氏のコメントを参照.
    // 計算式導出についてには，Jaume Sanchez Elias氏の画像を参照.
    const float kSqrt8 = 2.82842712474619;
    float s = 1.0f / (kSqrt8 * sqrt(abs(reflectDir.z) + 1.0f));

    return float2(
        reflectDir.x * s + 0.5f,
        1.0f - (reflectDir.y * s + 0.5f)); // DirectXなので上下反転.
}

//-----------------------------------------------------------------------------
//      非交差後の処理です.
//-----------------------------------------------------------------------------
[shader("miss")]
void OnMiss(inout RayPayload payload)
{
    // スフィアマップのテクスチャ座標を算出.
    float2 uv = ToSphereMapCoord(payload.RayDir);

    // スフィアマップをサンプル.
    float4 color = BackGround.SampleLevel(LinearSampler, uv, 0.0f);

    // 色を設定.
    payload.Color = color;
}
