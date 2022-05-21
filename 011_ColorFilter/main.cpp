﻿//-------------------------------------------------------------------------------------------------
// File : main.cpp
// Desc : Main Entry Point.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <a3d.h>
#include <cassert>
#include <cfloat>
#include <cstring>
#include <cstdlib>
#include <string>
#include <SampleApp.h>
#include <SampleAllocator.h>
#include <SampleUtil.h>
#include <SampleMath.h>
#include <SampleTarga.h>
#include <SampleGui.h>
#include <imgui/imgui.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    Vec3 Position;  //!< 位置座標です.
    Vec2 TexCoord;  //!< テクスチャ座標.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ColorVertex structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ColorVertex
{
    Vec3 Position;  //!< 位置座標です.
    Vec4 Color;     //!< 頂点カラーです.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Transform structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Transform
{
    Mat4 World;     //!< ワールド行列です.
    Mat4 View;      //!< ビュー行列です.
    Mat4 Proj;      //!< 射影行列です.
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// ColorFilter structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ColorFilter
{
    float       TargetWidth;        //!< ターゲットの横幅.
    float       TargetHeight;       //!< ターゲットの縦幅.
    uint32_t    Reserved0;          //!< 予約領域.
    uint32_t    Reserved1;          //!< 予約領域.
    Mat4        ColorMatrix;        //!< カラー行列.
};


//-------------------------------------------------------------------------------------------------
// Forward declarations.
//-------------------------------------------------------------------------------------------------
bool InitA3D();
void TermA3D();
void DrawA3D();
void Resize(uint32_t w, uint32_t h, void* ptr);
Mat4 CreateSepiaMatrix(float tone);
Mat4 CreateGrayScaleMatrix(float value);
Mat4 CreateHueMatrix(float hueRad);
Mat4 CreateSaturationMatrix(float r, float g, float b);
Mat4 CreateBrightnessMatrix(float value);
Mat4 CreateContrastMatrix(float value);
Mat4 CreateColorInvertMatrix();

//-------------------------------------------------------------------------------------------------
// Global Varaibles.
//-------------------------------------------------------------------------------------------------
IApp*                       g_pApp                  = nullptr;  //!< ウィンドウの生成を行うヘルパークラスです.
a3d::IDevice*               g_pDevice               = nullptr;  //!< デバイスです.
a3d::ISwapChain*            g_pSwapChain            = nullptr;  //!< スワップチェインです.
a3d::IQueue*                g_pGraphicsQueue        = nullptr;  //!< コマンドキューです.
a3d::IFence*                g_pFence                = nullptr;  //!< フェンスです.
a3d::IDescriptorSetLayout*  g_pDescriptorSetLayout  = nullptr;  //!< ディスクリプタセットレイアウトです.
a3d::IPipelineState*        g_pPipelineState        = nullptr;  //!< パイプラインステートです.
a3d::IBuffer*               g_pVertexBuffer         = nullptr;  //!< 頂点バッファです.
a3d::IBuffer*               g_pIndexBuffer          = nullptr;  //!< インデックスバッファです.
a3d::ITexture*              g_pDepthBuffer          = nullptr;  //!< 深度バッファです.
a3d::IDepthStencilView*     g_pDepthView            = nullptr;  //!< 深度ステンシルターゲットビューです.
a3d::ISampler*              g_pSampler              = nullptr;  //!< サンプラーです.
a3d::ITexture*              g_pOffScreenBuffer      = nullptr;  //!< オフスクリーンバッファです.
a3d::IRenderTargetView*     g_pOffScreenTargetView  = nullptr;  //!< オフスクリーンビューです.
a3d::IShaderResourceView*   g_pOffScreenTextureView = nullptr;  //!< オフスクリーンシェーダリソースビューです.
a3d::IBuffer*               g_pTriangleVertexBuffer = nullptr;  //!< 三角形の頂点バッファ.
a3d::IPipelineState*        g_pOffScreenPipeline    = nullptr;  //!< オフスクリーン描画用パイプラインステートです.
a3d::IDescriptorSetLayout*  g_pOffScreenDSetLayout  = nullptr;  //!< オフスクリーン描画用ディスクリプタセットレイアウトです.
a3d::ITexture*              g_pColorBuffer[2]       = {};       //!< カラーバッファです.
a3d::IRenderTargetView*     g_pColorView[2]         = {};       //!< カラービューです.
a3d::ICommandList*          g_pCommandList[2]       = {};       //!< コマンドリストです.
a3d::IBuffer*               g_pConstantBuffer[2]    = {};       //!< 定数バッファです.
a3d::IConstantBufferView*   g_pConstantView[2]      = {};       //!< 定数バッファビューです.
a3d::Viewport               g_Viewport              = {};       //!< ビューポートです.
a3d::Rect                   g_Scissor               = {};       //!< シザー矩形です.
a3d::IPipelineState*        g_pColorFilterPipeline  = nullptr;
a3d::IDescriptorSetLayout*  g_pColorFilterLayout    = nullptr;
a3d::ITexture*              g_pFilteredTexture      = nullptr;
a3d::IUnorderedAccessView*  g_pFilteredUAV          = nullptr;
a3d::IShaderResourceView*   g_pFilteredSRV          = nullptr;
a3d::IBuffer*               g_pColorFilterBuffer[2] = {};
a3d::IConstantBufferView*   g_pColorFilterCBV[2]    = {};
Transform                   g_Transform             = {};       //!< 変換行列です.
float                       g_RotateAngle           = 0.0f;     //!< 回転角です.
void*                       g_pCbHead[2]            = {};       //!< 定数バッファの先頭ポインタです.
float                       g_ClearColor[4]         = {};       //!< クリアカラーです.
float                       g_RotateSpeed           = 1.0f;     //!< 回転速度です.
bool                        g_Prepare               = false;    //!< 準備が出来たらtrue.
float                       g_SepiaTone             = 0.0f;
float                       g_GrayScale             = 0.0f;
float                       g_Hue                   = 0.0f;
float                       g_Saturation            = 1.0f;
float                       g_Brightness            = 1.0f;
float                       g_Contrast              = 0.0f;
bool                        g_ColorInvert           = false;
void*                       g_pCbFilter[2]          = {};
SampleAllocator             g_Allocator;                        //!< アロケータ.

//-------------------------------------------------------------------------------------------------
//      メインエントリーポイントです.
//-------------------------------------------------------------------------------------------------
void Main()
{
    // ウィンドウ生成.
    if (!CreateApp(960, 540, &g_pApp))
    { return; }

    // リサイズ時のコールバック関数を設定.
    g_pApp->SetResizeCallback(Resize, nullptr);

    // A3D初期化.
    if (!InitA3D())
    {
        g_pApp->Release();
        return;
    }

    // メインループ.
    while( g_pApp->IsLoop() )
    {
        // 描画処理.
        DrawA3D();
    }

    // 後始末.
    TermA3D();
    g_pApp->Release();
}

//-------------------------------------------------------------------------------------------------
//      A3Dの初期化を行います.
//-------------------------------------------------------------------------------------------------
bool InitA3D()
{
    g_Prepare = false;

    // グラフィックスシステムの初期化.
    {
        a3d::SystemDesc desc = {};
        desc.pSystemAllocator = &g_Allocator;

        if (!a3d::InitSystem(&desc))
        { return false; }
    }

    // デバイスの生成.
    {
        a3d::DeviceDesc desc = {};

        desc.EnableDebug = true;

        // 最大ディスクリプタ数を設定.
        desc.MaxColorTargetCount            = 256;
        desc.MaxDepthTargetCount            = 256;
        desc.MaxShaderResourceCount         = 256;
        desc.MaxSamplerCount                = 256;

        // 最大サブミット数を設定.
        desc.MaxGraphicsQueueSubmitCount    = 256;
        desc.MaxCopyQueueSubmitCount        = 256;
        desc.MaxComputeQueueSubmitCount     = 256;

        // デバイスを生成.
        if (!a3d::CreateDevice(&desc, &g_pDevice))
        { return false; }

        // コマンドキューを取得.
        g_pDevice->GetGraphicsQueue(&g_pGraphicsQueue);
    }

    auto info = g_pDevice->GetInfo();

    #if SAMPLE_IS_VULKAN && A3D_IS_WIN
        auto format = a3d::RESOURCE_FORMAT_B8G8R8A8_UNORM;
    #else
        auto format = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
    #endif

    // スワップチェインの生成.
    {
        a3d::SwapChainDesc desc = {};
        desc.Extent.Width   = g_pApp->GetWidth();
        desc.Extent.Height  = g_pApp->GetHeight();
        desc.Format         = format;
        desc.MipLevels      = 1;
        desc.SampleCount    = 1;
        desc.BufferCount    = 2;
        desc.SyncInterval   = 1;
        desc.InstanceHandle = g_pApp->GetInstanceHandle();
        desc.WindowHandle   = g_pApp->GetWindowHandle();

        if (!g_pDevice->CreateSwapChain(&desc, &g_pSwapChain))
        { return false; }

        // スワップチェインからバッファを取得.
        g_pSwapChain->GetBuffer(0, &g_pColorBuffer[0]);
        g_pSwapChain->GetBuffer(1, &g_pColorBuffer[1]);

        a3d::TargetViewDesc viewDesc = {};
        viewDesc.Dimension          = a3d::VIEW_DIMENSION_TEXTURE2D;
        viewDesc.Format             = format;
        viewDesc.MipSlice           = 0;
        viewDesc.MipLevels          = desc.MipLevels;
        viewDesc.FirstArraySlice    = 0;
        viewDesc.ArraySize          = 1;

        for(auto i=0; i<2; ++i)
        {
            if (!g_pDevice->CreateRenderTargetView(g_pColorBuffer[i], &viewDesc, &g_pColorView[i]))
            { return false; }
        }
    }

    // 深度バッファの生成.
    {
        a3d::TextureDesc desc = {};
        desc.Dimension          = a3d::RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width              = g_pApp->GetWidth();
        desc.Height             = g_pApp->GetHeight();
        desc.DepthOrArraySize   = 1;
        desc.Format             = a3d::RESOURCE_FORMAT_D32_FLOAT;
        desc.MipLevels          = 1;
        desc.SampleCount        = 1;
        desc.Layout             = a3d::RESOURCE_LAYOUT_OPTIMAL;
        desc.Usage              = a3d::RESOURCE_USAGE_DEPTH_STENCIL;
        desc.InitState          = a3d::RESOURCE_STATE_DEPTH_WRITE;
        desc.HeapType           = a3d::HEAP_TYPE_DEFAULT;

        if (!g_pDevice->CreateTexture(&desc, &g_pDepthBuffer))
        { return false; }

        a3d::TargetViewDesc viewDesc = {};
        viewDesc.Dimension          = a3d::VIEW_DIMENSION_TEXTURE2D;
        viewDesc.Format             = desc.Format;
        viewDesc.MipSlice           = 0;
        viewDesc.MipLevels          = desc.MipLevels;
        viewDesc.FirstArraySlice    = 0;
        viewDesc.ArraySize          = desc.DepthOrArraySize;

        if (!g_pDevice->CreateDepthStencilView(g_pDepthBuffer, &viewDesc, &g_pDepthView))
        { return false; }
    }

    // コマンドリストを生成.
    {
        a3d::CommandListDesc desc = {};
        desc.Type       = a3d::COMMANDLIST_TYPE_DIRECT;
        desc.BufferSize = 4096;

        for(auto i=0; i<2; ++i)
        {
            if (!g_pDevice->CreateCommandList(&desc, &g_pCommandList[i]))
            { return false; }
        }
    }

    // フェンスを生成.
    {
        if (!g_pDevice->CreateFence(&g_pFence))
        { return false; }
    }

    // 頂点バッファを生成.
    {
        Vertex vertices[] = {
            { Vec3( 1.0f, -1.0f, 0.0f), Vec2(1.0f, 0.0f) },
            { Vec3(-1.0f, -1.0f, 0.0f), Vec2(0.0f, 0.0f) },
            { Vec3(-1.0f,  1.0f, 0.0f), Vec2(0.0f, 1.0f) },
            { Vec3( 1.0f,  1.0f, 0.0f), Vec2(1.0f, 1.0f) },
        };

        a3d::BufferDesc desc = {};
        desc.Size       = sizeof(vertices);
        desc.Stride     = sizeof(Vertex);
        desc.InitState  = a3d::RESOURCE_STATE_GENERAL;
        desc.Usage      = a3d::RESOURCE_USAGE_VERTEX_BUFFER;
        desc.HeapType   = a3d::HEAP_TYPE_UPLOAD;

        if ( !g_pDevice->CreateBuffer(&desc, &g_pVertexBuffer) )
        { return false; }

        auto ptr = g_pVertexBuffer->Map();
        if ( ptr == nullptr )
        { return false; }

        memcpy( ptr, vertices, sizeof(vertices) );
        
        g_pVertexBuffer->Unmap();
    }

    // インデックスバッファを生成.
    {
        uint32_t indices[] = {
            0, 1, 2,
            2, 0, 3,
        };

        a3d::BufferDesc desc = {};
        desc.Size       = sizeof(indices);
        desc.Stride     = sizeof(uint32_t);
        desc.InitState  = a3d::RESOURCE_STATE_GENERAL;
        desc.Usage      = a3d::RESOURCE_USAGE_INDEX_BUFFER;
        desc.HeapType   = a3d::HEAP_TYPE_UPLOAD;

        if ( !g_pDevice->CreateBuffer(&desc, &g_pIndexBuffer) )
        { return false; }

        auto ptr = g_pIndexBuffer->Map();
        if ( ptr == nullptr )
        { return false; }

        memcpy( ptr, indices, sizeof(indices) );

        g_pIndexBuffer->Unmap();
    }

    // 定数バッファを生成.
    {
        auto stride = a3d::RoundUp<uint32_t>( sizeof(Transform), info.ConstantBufferAlignment );

        a3d::BufferDesc desc = {};
        desc.Size       = stride;
        desc.Stride     = stride;
        desc.InitState  = a3d::RESOURCE_STATE_GENERAL;
        desc.Usage      = a3d::RESOURCE_USAGE_CONSTANT_BUFFER;
        desc.HeapType   = a3d::HEAP_TYPE_UPLOAD;

        a3d::ConstantBufferViewDesc viewDesc = {};
        viewDesc.Offset = 0;
        viewDesc.Range  = stride;

        for(auto i=0; i<2; ++i)
        {
            if (!g_pDevice->CreateBuffer(&desc, &g_pConstantBuffer[i]))
            { return false; }

            if (!g_pDevice->CreateConstantBufferView(g_pConstantBuffer[i], &viewDesc, &g_pConstantView[i]))
            { return false; }

            g_pCbHead[i] = g_pConstantBuffer[i]->Map();
        }

        Vec3 pos = Vec3(0.0f, 0.0f, 3.0f);
        Vec3 at  = Vec3(0.0f, 0.0f, 0.0f);
        Vec3 up  = Vec3(0.0f, 1.0f, 0.0f);

        auto aspect = static_cast<float>(g_pApp->GetWidth()) / static_cast<float>(g_pApp->GetHeight());

        g_Transform.World = Mat4::Identity();
        g_Transform.View  = Mat4::LookAt(pos, at, up);
        g_Transform.Proj  = Mat4::PersFov(ToRadian(45.0f), aspect, 0.1f, 1000.0f);
    }

    // シェーダバイナリを読み込みます.
    a3d::ShaderBinary vs = {};
    a3d::ShaderBinary ps = {};
    {
        FixedSizeString vsPath;
        FixedSizeString psPath;
        GetShaderPath("simpleTexVS", vsPath);
        GetShaderPath("simpleTexPS", psPath);

        if (!LoadShaderBinary(vsPath.c_str(), vs))
        { return false; }

        if (!LoadShaderBinary(psPath.c_str(), ps))
        {
            DisposeShaderBinary(vs);
            return false;
        }
    }

    // ディスクリプタセットレイアウトを生成します.
    {
    #if SAMPLE_IS_VULKAN || SAMPLE_IS_D3D12 || SAMPLE_IS_D3D11
        a3d::DescriptorSetLayoutDesc desc = {};
        desc.EntryCount                = 3;

        desc.Entries[0].ShaderStage    = a3d::SHADER_STAGE_VS;
        desc.Entries[0].ShaderRegister = 0;
        desc.Entries[0].BindLocation   = 0;
        desc.Entries[0].Type           = a3d::DESCRIPTOR_TYPE_CBV;

        desc.Entries[1].ShaderStage    = a3d::SHADER_STAGE_PS;
        desc.Entries[1].ShaderRegister = 0;
        desc.Entries[1].BindLocation   = 1;
        desc.Entries[1].Type           = a3d::DESCRIPTOR_TYPE_SMP;

        desc.Entries[2].ShaderStage    = a3d::SHADER_STAGE_PS;
        desc.Entries[2].ShaderRegister = 0;
        desc.Entries[2].BindLocation   = 2;
        desc.Entries[2].Type           = a3d::DESCRIPTOR_TYPE_SRV_T;
    #else
        a3d::DescriptorSetLayoutDesc desc = {};
        desc.MaxSetCount               = 2;
        desc.EntryCount                = 2;

        desc.Entries[0].ShaderStage    = a3d::SHADER_STAGE_VS;
        desc.Entries[0].ShaderRegister = 0;
        desc.Entries[0].BindLocation   = 0;
        desc.Entries[0].Type           = a3d::DESCRIPTOR_TYPE_CBV;

        desc.Entries[1].ShaderStage    = a3d::SHADER_STAGE_PS;
        desc.Entries[1].ShaderRegister = 0;
        desc.Entries[1].BindLocation   = 0;
        desc.Entries[1].Type           = a3d::DESCRIPTOR_TYPE_SRV_T;
    #endif

        if (!g_pDevice->CreateDescriptorSetLayout(&desc, &g_pDescriptorSetLayout))
        { return false; }
    }

    // グラフィックスパイプラインステートを生成します.
    {
        // 入力要素です.
        a3d::InputElementDesc inputElements[] = {
            { "POSITION", 0, 0, a3d::RESOURCE_FORMAT_R32G32B32_FLOAT , 0,  0, a3d::INPUT_CLASSIFICATION_PER_VERTEX },
            { "TEXCOORD", 0, 5, a3d::RESOURCE_FORMAT_R32G32_FLOAT    , 0, 12, a3d::INPUT_CLASSIFICATION_PER_VERTEX },
        };

        // 入力レイアウトです.
        a3d::InputLayoutDesc inputLayout = {};
        inputLayout.ElementCount = 2;
        inputLayout.pElements    = inputElements;

        // ステンシルテスト設定です.
        a3d::StencilTestDesc stencilTest = {};
        stencilTest.StencilFailOp      = a3d::STENCIL_OP_KEEP;
        stencilTest.StencilDepthFailOp = a3d::STENCIL_OP_KEEP;
        stencilTest.StencilFailOp      = a3d::STENCIL_OP_KEEP;
        stencilTest.StencilCompareOp   = a3d::COMPARE_OP_NEVER;

        // グラフィックスパイプラインステートを設定します.
        a3d::GraphicsPipelineStateDesc desc = {};

        // シェーダの設定.
        desc.VS = vs;
        desc.PS = ps;

        // ブレンドステートの設定.
        desc.BlendState.IndependentBlendEnable          = false;
        desc.BlendState.LogicOpEnable                   = false;
        desc.BlendState.LogicOp                         = a3d::LOGIC_OP_NOOP;
        for(auto i=0; i<8; ++i)
        { desc.BlendState.RenderTarget[i] = a3d::ColorBlendState::Opaque(); }

        // ラスタライザ―ステートの設定.
        desc.RasterizerState.PolygonMode                = a3d::POLYGON_MODE_SOLID;
        desc.RasterizerState.CullMode                   = a3d::CULL_MODE_NONE;
        desc.RasterizerState.FrontCounterClockWise      = false;
        desc.RasterizerState.DepthBias                  = 0;
        desc.RasterizerState.DepthBiasClamp             = 0.0f;
        desc.RasterizerState.SlopeScaledDepthBias       = 0;
        desc.RasterizerState.DepthClipEnable            = false;
        desc.RasterizerState.EnableConservativeRaster   = false;
        
        // マルチサンプルステートの設定.
        desc.MultiSampleState.EnableAlphaToCoverage = false;
        desc.MultiSampleState.EnableMultiSample     = false;
        desc.MultiSampleState.SampleCount           = 1;

        // 深度ステートの設定.
        desc.DepthState.DepthTestEnable      = true;
        desc.DepthState.DepthWriteEnable     = true;
        desc.DepthState.DepthCompareOp       = a3d::COMPARE_OP_LEQUAL;

        // ステンシルステートの設定.
        desc.StencilState.StencilTestEnable    = false;
        desc.StencilState.StencllReadMask      = 0;
        desc.StencilState.StencilWriteMask     = 0;
        desc.StencilState.FrontFace            = stencilTest;
        desc.StencilState.BackFace             = stencilTest;

        // テッセレーションステートの設定.
        desc.TessellationState.PatchControlCount = 0;

        // 入力アウトの設定.
        desc.InputLayout = inputLayout;

        // ディスクリプタセットレイアウトの設定.
        desc.pLayout = g_pDescriptorSetLayout;
        
        // プリミティブトポロジーの設定.
        desc.PrimitiveTopology = a3d::PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        // フォーマットの設定.
        desc.RenderTargetCount  = 1;
        desc.RenderTarget[0]    = format;
        desc.DepthTarget        = a3d::RESOURCE_FORMAT_D32_FLOAT;

        // キャッシュ済みパイプラインステートの設定.
        desc.pCachedPSO = nullptr;

        // グラフィックスパイプラインステートの生成.
        if (!g_pDevice->CreateGraphicsPipeline(&desc, &g_pPipelineState))
        {
            DisposeShaderBinary(vs);
            DisposeShaderBinary(ps);
            return false;
        }
    }
     // 不要になったので破棄します.
    DisposeShaderBinary(vs);
    DisposeShaderBinary(ps);

    // サンプラーの生成.
    {
        a3d::SamplerDesc desc = {};
        desc.MinFilter          = a3d::FILTER_MODE_LINEAR;
        desc.MagFilter          = a3d::FILTER_MODE_LINEAR;
        desc.MipMapMode         = a3d::MIPMAP_MODE_LINEAR;
        desc.AddressU           = a3d::TEXTURE_ADDRESS_MODE_CLAMP;
        desc.AddressV           = a3d::TEXTURE_ADDRESS_MODE_CLAMP;
        desc.AddressW           = a3d::TEXTURE_ADDRESS_MODE_CLAMP;
        desc.MinLod             = 0.0f;
        desc.AnisotropyEnable   = false;
        desc.MaxAnisotropy      = 1;
        desc.CompareEnable      = false;
        desc.CompareOp          = a3d::COMPARE_OP_NEVER;
        desc.MinLod             = 0.0f;
        desc.MaxLod             = FLT_MAX;
        desc.BorderColor        = a3d::BORDER_COLOR_OPAQUE_WHITE;

        if (!g_pDevice->CreateSampler(&desc, &g_pSampler))
        { return false; }
    }

    // 頂点バッファを生成.
    {
        ColorVertex vertices[] = {
            { Vec3( 0.5f, -0.5f, 0.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f) },
            { Vec3(-0.5f, -0.5f, 0.0f), Vec4(0.0f, 1.0f, 0.0f, 1.0f) },
            { Vec3( 0.0f,  0.5f, 0.0f), Vec4(0.0f, 0.0f, 1.0f, 1.0f) },
        };

        a3d::BufferDesc desc = {};
        desc.Size       = sizeof(vertices);
        desc.Stride     = sizeof(ColorVertex);
        desc.InitState  = a3d::RESOURCE_STATE_GENERAL;
        desc.Usage      = a3d::RESOURCE_USAGE_VERTEX_BUFFER;
        desc.HeapType   = a3d::HEAP_TYPE_UPLOAD;

        if ( !g_pDevice->CreateBuffer(&desc, &g_pTriangleVertexBuffer) )
        { return false; }

        auto ptr = g_pTriangleVertexBuffer->Map();
        if ( ptr == nullptr )
        { return false; }

        memcpy( ptr, vertices, sizeof(vertices) );

        g_pTriangleVertexBuffer->Unmap();
    }

    // オフスクリーンターゲットの生成.
    {
        a3d::TextureDesc desc = {};
        desc.Dimension          = a3d::RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width              = g_pApp->GetWidth();
        desc.Height             = g_pApp->GetHeight();
        desc.DepthOrArraySize   = 1;
        desc.Format             = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        desc.MipLevels          = 1;
        desc.SampleCount        = 1;
        desc.Layout             = a3d::RESOURCE_LAYOUT_OPTIMAL;
        desc.Usage              = a3d::RESOURCE_USAGE_RENDER_TARGET | a3d::RESOURCE_USAGE_SHADER_RESOURCE;
        desc.InitState          = a3d::RESOURCE_STATE_GENERAL;
        desc.HeapType           = a3d::HEAP_TYPE_DEFAULT;

        if (!g_pDevice->CreateTexture(&desc, &g_pOffScreenBuffer))
        { return false; }

        a3d::TargetViewDesc rtvDesc = {};
        rtvDesc.Dimension          = a3d::VIEW_DIMENSION_TEXTURE2D;
        rtvDesc.Format             = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.MipSlice           = 0;
        rtvDesc.MipLevels          = desc.MipLevels;
        rtvDesc.FirstArraySlice    = 0;
        rtvDesc.ArraySize          = desc.DepthOrArraySize;

        if (!g_pDevice->CreateRenderTargetView(g_pOffScreenBuffer, &rtvDesc, &g_pOffScreenTargetView))
        { return false; }

        a3d::ShaderResourceViewDesc srvDesc = {};
        srvDesc.Dimension       = a3d::VIEW_DIMENSION_TEXTURE2D;
        srvDesc.Format          = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        srvDesc.MipSlice        = 0;
        srvDesc.MipLevels       = desc.MipLevels;
        srvDesc.FirstElement    = 0;
        srvDesc.ElementCount    = desc.DepthOrArraySize;
        if (!g_pDevice->CreateShaderResourceView(g_pOffScreenBuffer, &srvDesc, &g_pOffScreenTextureView))
        { return false; }

        a3d::CommandListDesc cmdDesc = {};
        cmdDesc.Type        = a3d::COMMANDLIST_TYPE_DIRECT;
        cmdDesc.BufferSize  = 128;
        a3d::ICommandList* pCommandList;
        if (!g_pDevice->CreateCommandList(&cmdDesc, &pCommandList))
        { return false; }
        pCommandList->Begin();
        pCommandList->TextureBarrier(g_pOffScreenBuffer, a3d::RESOURCE_STATE_GENERAL, a3d::RESOURCE_STATE_SHADER_READ);
        pCommandList->End();

        a3d::IQueue* pQueue;
        g_pDevice->GetGraphicsQueue(&pQueue);
        pQueue->Submit(pCommandList);
        pQueue->Execute(nullptr);
        pQueue->WaitIdle();

        a3d::SafeRelease(pCommandList);
        a3d::SafeRelease(pQueue);
    }

    // オフスクリーン用ディスクリプタセットレイアウトを生成.
    {
        a3d::DescriptorSetLayoutDesc desc = {};
        desc.EntryCount  = 0;

        if (!g_pDevice->CreateDescriptorSetLayout(&desc, &g_pOffScreenDSetLayout))
        { return false; }
    }

    // オフスクリーン描画用シェーダを読み込みます.
    {
        FixedSizeString vsPath;
        FixedSizeString psPath;
        GetShaderPath("simpleQuadVS", vsPath);
        GetShaderPath("simpleQuadPS", psPath);

        if (!LoadShaderBinary(vsPath.c_str(), vs))
        { return false; }

        if (!LoadShaderBinary(psPath.c_str(), ps))
        {
            DisposeShaderBinary(vs);
            return false;
        }
    }

    // グラフィックスパイプラインステートを生成します.
    {
        // 入力要素です.
        a3d::InputElementDesc inputElements[] = {
            { "POSITION", 0, 0, a3d::RESOURCE_FORMAT_R32G32B32_FLOAT   , 0,  0, a3d::INPUT_CLASSIFICATION_PER_VERTEX },
            { "COLOR"   , 0, 1, a3d::RESOURCE_FORMAT_R32G32B32A32_FLOAT, 0, 12, a3d::INPUT_CLASSIFICATION_PER_VERTEX },
        };

        // 入力レイアウトです.
        a3d::InputLayoutDesc inputLayout = {};
        inputLayout.ElementCount = 2;
        inputLayout.pElements    = inputElements;

        // グラフィックスパイプラインステートを設定します.
        a3d::GraphicsPipelineStateDesc desc = a3d::GraphicsPipelineStateDesc::Default();

        // シェーダの設定.
        desc.VS = vs;
        desc.PS = ps;

        // 入力アウトの設定.
        desc.InputLayout = inputLayout;

        // ディスクリプタセットレイアウトの設定.
        desc.pLayout = g_pOffScreenDSetLayout;

        // フォーマットの設定.
        desc.RenderTargetCount  = 1;
        desc.RenderTarget[0]    = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;

        // キャッシュ済みパイプラインステートの設定.
        desc.pCachedPSO = nullptr;

        // グラフィックスパイプラインステートの生成.
        if (!g_pDevice->CreateGraphicsPipeline(&desc, &g_pOffScreenPipeline))
        {
            DisposeShaderBinary(vs);
            DisposeShaderBinary(ps);
            return false;
        }
    }

    // 不要になったので破棄します.
    DisposeShaderBinary(vs);
    DisposeShaderBinary(ps);

    // カラーフィルタ用テクスチャ.
    {
        a3d::TextureDesc desc = {};
        desc.Dimension          = a3d::RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width              = g_pApp->GetWidth();
        desc.Height             = g_pApp->GetHeight();
        desc.DepthOrArraySize   = 1;
        desc.Format             = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        desc.MipLevels          = 1;
        desc.SampleCount        = 1;
        desc.Layout             = a3d::RESOURCE_LAYOUT_OPTIMAL;
        desc.Usage              = a3d::RESOURCE_USAGE_UNORDERED_ACCESS | a3d::RESOURCE_USAGE_SHADER_RESOURCE;
        desc.InitState          = a3d::RESOURCE_STATE_UNORDERED_ACCESS;
        desc.HeapType           = a3d::HEAP_TYPE_DEFAULT;

        if (!g_pDevice->CreateTexture(&desc, &g_pFilteredTexture))
        { return false; }

        a3d::UnorderedAccessViewDesc uavDesc = {};
        uavDesc.Dimension       = a3d::VIEW_DIMENSION_TEXTURE2D;
        uavDesc.FirstElement    = 0;
        uavDesc.ElementCount    = 1;
        uavDesc.Format          = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        uavDesc.MipLevels       = 1;
        uavDesc.MipSlice        = 0;

        if (!g_pDevice->CreateUnorderedAccessView(g_pFilteredTexture, &uavDesc, &g_pFilteredUAV))
        { return false; }

        a3d::ShaderResourceViewDesc srvDesc = {};
        srvDesc.Dimension       = a3d::VIEW_DIMENSION_TEXTURE2D;
        srvDesc.FirstElement    = 0;
        srvDesc.ElementCount    = 1;
        srvDesc.Format          = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        srvDesc.MipLevels       = 1;
        srvDesc.MipSlice        = 0;

        if (!g_pDevice->CreateShaderResourceView(g_pFilteredTexture, &srvDesc, &g_pFilteredSRV))
        { return false; }
    }

    {
        auto size = a3d::RoundUp<uint32_t>( sizeof(ColorFilter), info.ConstantBufferAlignment );

        a3d::BufferDesc desc = {};
        desc.Size       = size;
        desc.Stride     = size;
        desc.InitState  = a3d::RESOURCE_STATE_GENERAL;
        desc.Usage      = a3d::RESOURCE_USAGE_CONSTANT_BUFFER;
        desc.HeapType   = a3d::HEAP_TYPE_UPLOAD;

        a3d::ConstantBufferViewDesc viewDesc = {};
        viewDesc.Offset = 0;
        viewDesc.Range  = size;

        for(auto i=0; i<2; ++i)
        {
            if (!g_pDevice->CreateBuffer(&desc, &g_pColorFilterBuffer[i]))
            { return false; }

            if (!g_pDevice->CreateConstantBufferView(g_pColorFilterBuffer[i], &viewDesc, &g_pColorFilterCBV[i]))
            { return false; }

            g_pCbFilter[i] = g_pColorFilterBuffer[i]->Map();
        }
    }

    // カラーフィルタ用ディスクリプタセットレイアウト.
    {
        a3d::DescriptorSetLayoutDesc desc = {};
        desc.EntryCount = 4;
        desc.Entries[0].ShaderStage     = a3d::SHADER_STAGE_CS;
        desc.Entries[0].Type            = a3d::DESCRIPTOR_TYPE_SRV_T;
        desc.Entries[0].ShaderRegister  = 0;
        desc.Entries[0].BindLocation    = 0;

        desc.Entries[1].ShaderStage     = a3d::SHADER_STAGE_CS;
        desc.Entries[1].Type            = a3d::DESCRIPTOR_TYPE_SMP;
        desc.Entries[1].ShaderRegister  = 0;
        desc.Entries[1].BindLocation    = 1;

        desc.Entries[2].ShaderStage     = a3d::SHADER_STAGE_CS;
        desc.Entries[2].Type            = a3d::DESCRIPTOR_TYPE_UAV_T;
        desc.Entries[2].ShaderRegister  = 0;
        desc.Entries[2].BindLocation    = 2;

        desc.Entries[3].ShaderStage     = a3d::SHADER_STAGE_CS;
        desc.Entries[3].Type            = a3d::DESCRIPTOR_TYPE_CBV;
        desc.Entries[3].ShaderRegister  = 0;
        desc.Entries[3].BindLocation    = 3;

        if (!g_pDevice->CreateDescriptorSetLayout(&desc, &g_pColorFilterLayout))
        { return false; }
    }

    // カラーフィルタ用パイプラインレイアウト.
    {
        a3d::ShaderBinary cs = {};
        FixedSizeString csPath;
        GetShaderPath("colorFilterCS", csPath);

        if (!LoadShaderBinary(csPath.c_str(), cs))
        { return false; }

        a3d::ComputePipelineStateDesc desc = {};
        desc.pLayout = g_pColorFilterLayout;
        desc.CS      = cs;

        if (!g_pDevice->CreateComputePipeline(&desc, &g_pColorFilterPipeline))
        {
            DisposeShaderBinary(cs);
            return false;
        }

        DisposeShaderBinary(cs);
    }

    // ビューポートの設定.
    g_Viewport.X        = 0.0f;
    g_Viewport.Y        = 0.0f;
    g_Viewport.Width    = static_cast<float>(g_pApp->GetWidth());
    g_Viewport.Height   = static_cast<float>(g_pApp->GetHeight());
    g_Viewport.MinDepth = 0.0f;
    g_Viewport.MaxDepth = 1.0f;

    // シザー矩形の設定.
    g_Scissor.Offset.X      = 0;
    g_Scissor.Offset.Y      = 0;
    g_Scissor.Extent.Width  = g_pApp->GetWidth();
    g_Scissor.Extent.Height = g_pApp->GetHeight();

    GuiMgr::TargetViewInfo targetInfo = {};
    targetInfo.ColorCount       = 1;
    targetInfo.ColorTargets[0]  = format;
    targetInfo.DepthTarget      = a3d::RESOURCE_FORMAT_D32_FLOAT;

    // GUIマネージャの初期化.
    if (!GuiMgr::GetInstance().Init(g_pDevice, targetInfo, g_pApp))
    { return false; }

    // クリアカラーの設定.
    g_ClearColor[0] = 0.3f;
    g_ClearColor[1] = 0.3f;
    g_ClearColor[2] = 1.0f;
    g_ClearColor[3] = 1.0f;

    g_Prepare = true;

    return true;
}

//-------------------------------------------------------------------------------------------------
//      A3Dの終了処理を行います.
//-------------------------------------------------------------------------------------------------
void TermA3D()
{
    g_Prepare = false;

    // アイドル状態になるまで待つ.
    g_pGraphicsQueue->WaitIdle();
    g_pDevice->WaitIdle();

    // GUIマネージャの終了処理.
    GuiMgr::GetInstance().Term();

    // ダブルバッファリソースの破棄.
    for(auto i=0; i<2; ++i)
    {
        // コマンドリストの破棄.
        a3d::SafeRelease(g_pCommandList[i]);

        // カラーターゲットビューの破棄.
        a3d::SafeRelease(g_pColorView[i]);

        // カラーバッファの破棄.
        a3d::SafeRelease(g_pColorBuffer[i]);

        // 定数バッファビューの破棄.
        a3d::SafeRelease(g_pConstantView[i]);

        // 破棄前にメモリマッピングを解除.
        if (g_pConstantBuffer[i] != nullptr)
        { g_pConstantBuffer[i]->Unmap(); }

        if (g_pColorFilterBuffer[i] != nullptr)
        { g_pColorFilterBuffer[i]->Unmap(); }

        // 定数バッファの破棄.
        a3d::SafeRelease(g_pConstantBuffer[i]);

        a3d::SafeRelease(g_pColorFilterCBV[i]);
        a3d::SafeRelease(g_pColorFilterBuffer[i]);
    }

    a3d::SafeRelease(g_pColorFilterLayout);
    a3d::SafeRelease(g_pColorFilterPipeline);

    a3d::SafeRelease(g_pFilteredSRV);
    a3d::SafeRelease(g_pFilteredUAV);
    a3d::SafeRelease(g_pFilteredTexture);

    // オフスクリーンビューを破棄.
    a3d::SafeRelease(g_pOffScreenTargetView);

    a3d::SafeRelease(g_pOffScreenTextureView);

    // オフスクリーンバッファを破棄.
    a3d::SafeRelease(g_pOffScreenBuffer);

    // サンプラーの破棄.
    a3d::SafeRelease(g_pSampler);

    // オフスクリーン用パイプラインステートを破棄.
    a3d::SafeRelease(g_pOffScreenPipeline);

    // オフスクリーン用ディスクリプタセットを破棄.
    a3d::SafeRelease(g_pOffScreenDSetLayout);

    // 頂点バッファを破棄.
    a3d::SafeRelease(g_pTriangleVertexBuffer);

    // パイプラインステートの破棄.
    a3d::SafeRelease(g_pPipelineState);

    // 頂点バッファの破棄.
    a3d::SafeRelease(g_pVertexBuffer);

    // インデックスバッファの破棄.
    a3d::SafeRelease(g_pIndexBuffer);

    // ディスクリプタセットレイアウトの破棄.
    a3d::SafeRelease(g_pDescriptorSetLayout);

    // フェンスの破棄.
    a3d::SafeRelease(g_pFence);

    // 深度ステンシルターゲットビューの破棄.
    a3d::SafeRelease(g_pDepthView);

    // 深度バッファの破棄.
    a3d::SafeRelease(g_pDepthBuffer);

    // スワップチェインの破棄.
    a3d::SafeRelease(g_pSwapChain);

    // グラフィックスキューの破棄.
    a3d::SafeRelease(g_pGraphicsQueue);

    // デバイスの破棄.
    a3d::SafeRelease(g_pDevice);

    // グラフィックスシステムの終了処理.
    a3d::TermSystem();
}

//-------------------------------------------------------------------------------------------------
//      A3Dによる描画処理を行います.
//-------------------------------------------------------------------------------------------------
void DrawA3D()
{
    if (!g_Prepare)
    { return; }

    // バッファ番号を取得します.
    auto idx = g_pSwapChain->GetCurrentBufferIndex();

    // 定数バッファを更新.
    {
        g_RotateAngle += 0.025f * g_RotateSpeed;
        g_Transform.World = Mat4::RotateY(g_RotateAngle);

        auto ptr = static_cast<uint8_t*>(g_pCbHead[idx]);
        memcpy(ptr, &g_Transform, sizeof(g_Transform));
    }

    // 
    {
        auto desc =  g_pFilteredTexture->GetDesc();
        ColorFilter res = {};
        res.TargetWidth    = float(desc.Width);
        res.TargetHeight   = float(desc.Height);
        res.ColorMatrix    = CreateSepiaMatrix(g_SepiaTone)
            * CreateGrayScaleMatrix(g_GrayScale)
            * CreateHueMatrix(g_Hue)
            * CreateSaturationMatrix(g_Saturation, g_Saturation, g_Saturation)
            * CreateBrightnessMatrix(g_Brightness)
            * CreateContrastMatrix(g_Contrast);
        if (g_ColorInvert)
        { res.ColorMatrix = res.ColorMatrix * CreateColorInvertMatrix(); }

        auto ptr = static_cast<uint8_t*>(g_pCbFilter[idx]);
        memcpy(ptr, &res, sizeof(res));
    }

    // コマンドの記録を開始します.
    auto pCmd = g_pCommandList[idx];
    pCmd->Begin();

    // 書き込み用のバリアを設定します.
    pCmd->TextureBarrier(
        g_pOffScreenBuffer,
        a3d::RESOURCE_STATE_SHADER_READ,
        a3d::RESOURCE_STATE_COLOR_WRITE);

    // フレームバッファをクリアします.
    a3d::ClearColorValue clearColor = {};
    clearColor.R            = g_ClearColor[0];
    clearColor.G            = g_ClearColor[1];
    clearColor.B            = g_ClearColor[2];
    clearColor.A            = g_ClearColor[3];
    clearColor.SlotIndex    = 0;

    // オフスクリーンフレームバッファを設定.
    pCmd->BeginFrameBuffer(1, &g_pOffScreenTargetView, nullptr, 1, &clearColor, nullptr);

    // オフスクリーンへの描画.
    {
        // パイプラインステートを設定します.
        pCmd->SetPipelineState(g_pOffScreenPipeline);

        // ビューポートとシザー矩形を設定します.
        // NOTE : ビューポートとシザー矩形の設定は，必ずSetPipelineState() の後である必要があります.
        pCmd->SetViewports(1, &g_Viewport);
        pCmd->SetScissors (1, &g_Scissor);

        // 頂点バッファを設定します.
        pCmd->SetVertexBuffers(0, 1, &g_pTriangleVertexBuffer, nullptr);

        // 三角形を描画します.
        pCmd->DrawInstanced(3, 1, 0, 0);
    }

    // オフスクリーンフレームバッファを解除します(バリア設定前に解除しておく必要があります).
    pCmd->EndFrameBuffer();

    // 読み込み用にバリアを設定します.
    pCmd->TextureBarrier(
        g_pOffScreenBuffer,
        a3d::RESOURCE_STATE_COLOR_WRITE,
        a3d::RESOURCE_STATE_SHADER_READ);

    // コンピュートシェーダでカラーフィルタ.
    {
        pCmd->SetPipelineState(g_pColorFilterPipeline);

        pCmd->SetView   (0, g_pOffScreenTextureView);
        pCmd->SetSampler(1, g_pSampler);
        pCmd->SetView   (2, g_pFilteredUAV);
        pCmd->SetView   (3, g_pColorFilterCBV[idx]);

        auto desc = g_pFilteredTexture->GetDesc();
        uint32_t threadX = (desc.Width  + 7) / 8;
        uint32_t threadY = (desc.Height + 7) / 8;

        pCmd->DispatchCompute(threadX, threadY, 1);
    }

    pCmd->TextureBarrier(
        g_pFilteredTexture,
        a3d::RESOURCE_STATE_UNORDERED_ACCESS,
        a3d::RESOURCE_STATE_SHADER_READ);

    // 書き込み用のバリアを設定します.
    pCmd->TextureBarrier(
        g_pColorBuffer[idx],
        a3d::RESOURCE_STATE_PRESENT,
        a3d::RESOURCE_STATE_COLOR_WRITE);

    clearColor.R            = 0.25f;
    clearColor.G            = 0.25f;
    clearColor.B            = 0.25f;
    clearColor.A            = 1.0f;
    clearColor.SlotIndex    = 0;

    a3d::ClearDepthStencilValue clearDepth = {};
    clearDepth.Depth                = 1.0f;
    clearDepth.Stencil              = 0;
    clearDepth.EnableClearDepth     = true;
    clearDepth.EnableClearStencil   = false;

    // フレームバッファを設定します.
    pCmd->BeginFrameBuffer(1, &g_pColorView[idx], g_pDepthView, 1, &clearColor, &clearDepth);

    // 3D描画.
    {
        // パイプラインステートを設定します.
        pCmd->SetPipelineState(g_pPipelineState);

        // ビューポートとシザー矩形を設定します.
        // NOTE : ビューポートとシザー矩形の設定は，必ずSetPipelineState() の後である必要があります.
        pCmd->SetViewports(1, &g_Viewport);
        pCmd->SetScissors (1, &g_Scissor);

        // 頂点バッファを設定します.
        pCmd->SetVertexBuffers(0, 1, &g_pVertexBuffer, nullptr);
        pCmd->SetIndexBuffer(g_pIndexBuffer, 0);

        // 矩形を描画.

    #if SAMPLE_IS_VULKAN || SAMPLE_IS_D3D12 || SAMPLE_IS_D3D11
        pCmd->SetView   (0, g_pConstantView[idx]);
        pCmd->SetSampler(1, g_pSampler);
        pCmd->SetView   (2, g_pFilteredSRV);
    #else
        pCmd->SetView   (0, g_pConstantView[i]);
        pCmd->SetSampler(1, g_pSampler);
        pCmd->SetView   (1, g_pFilteredSRV);
    #endif
        pCmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
    }

    // 2D描画.
    {
        // 描画開始.
        GuiMgr::GetInstance().SwapBuffers();

        {
            ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(400, 260), ImGuiCond_Once);

            ImGui::Begin("Debug");
            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            ImGui::SliderFloat("rotate speed", &g_RotateSpeed, 0.0f, 10.0f);
            ImGui::ColorEdit3("clear color", g_ClearColor);
            ImGui::SliderFloat("sepia tone", &g_SepiaTone, 0.0f, 1.0f);
            ImGui::SliderFloat("gray scale", &g_GrayScale, 0.0f, 1.0f);
            ImGui::SliderFloat("hue", &g_Hue, 0.0f, F_PI);
            ImGui::SliderFloat("saturation", &g_Saturation, 0.0f, 5.0f);
            ImGui::SliderFloat("brightness", &g_Brightness, 0.0f, 5.0f);
            ImGui::SliderFloat("contrast", &g_Contrast, -1.0f, 1.0f);
            ImGui::Checkbox("color invert", &g_ColorInvert);
            ImGui::End();
        }

        // 描画終了.
        GuiMgr::GetInstance().Issue(pCmd);
    }

    // 表示用のバリアを設定する前に，フレームバッファの設定を解除する必要があります.
    pCmd->EndFrameBuffer();

    pCmd->TextureBarrier(
        g_pFilteredTexture,
        a3d::RESOURCE_STATE_SHADER_READ,
        a3d::RESOURCE_STATE_UNORDERED_ACCESS);

    // 表示用にバリアを設定します.
    pCmd->TextureBarrier(
        g_pColorBuffer[idx],
        a3d::RESOURCE_STATE_COLOR_WRITE,
        a3d::RESOURCE_STATE_PRESENT);

    // コマンドリストへの記録を終了します.
    pCmd->End();

    // コマンドキューに登録します.
    g_pGraphicsQueue->Submit(pCmd);

    // コマンドを実行します.
    g_pGraphicsQueue->Execute(g_pFence);

    // コマンドを実行完了を待機します.
    if (!g_pFence->IsSignaled())
    { g_pFence->Wait(UINT32_MAX); }

    // 画面に表示します.
    g_pGraphicsQueue->Present( g_pSwapChain );
}

//-------------------------------------------------------------------------------------------------
//      リサイズ処理です.
//-------------------------------------------------------------------------------------------------
void Resize( uint32_t w, uint32_t h, void* pUser )
{
    A3D_UNUSED( pUser );

    // 準備が完了してない状態だったら，処理できないので即終了.
    if (!g_Prepare || g_pSwapChain == nullptr)
    { return; }

    // サンプル数以下の場合はクラッシュする原因となるので処理させない.
    {
        auto desc = g_pSwapChain->GetDesc();
        if ( w < desc.SampleCount || h < desc.SampleCount )
        { return; }
    }

    g_Prepare = false;

    // アイドル状態になるまで待つ.
    g_pGraphicsQueue->WaitIdle();
    g_pDevice->WaitIdle();

    for(auto i=0; i<2; ++i)
    {
        // カラービューの破棄.
        a3d::SafeRelease(g_pColorView[i]);

        // カラーバッファの破棄.
        a3d::SafeRelease(g_pColorBuffer[i]);
    }

    // 深度ステンシルターゲットビューの破棄.
    a3d::SafeRelease(g_pDepthView);

    // 深度バッファの破棄.
    a3d::SafeRelease(g_pDepthBuffer);

    // オフスクリーンビューを破棄.
    a3d::SafeRelease(g_pOffScreenTargetView);

    a3d::SafeRelease(g_pOffScreenTextureView);

    // オフスクリーンバッファを破棄.
    a3d::SafeRelease(g_pOffScreenBuffer);

    // スワップチェインのリサイズ処理です.
    g_pSwapChain->ResizeBuffers( w, h );

    // テクスチャビューを生成.
    {
        auto desc = g_pSwapChain->GetDesc();

        // スワップチェインからバッファを取得.
        g_pSwapChain->GetBuffer(0, &g_pColorBuffer[0]);
        g_pSwapChain->GetBuffer(1, &g_pColorBuffer[1]);

        a3d::TargetViewDesc viewDesc = {};
        viewDesc.Dimension          = a3d::VIEW_DIMENSION_TEXTURE2D;
        viewDesc.Format             = desc.Format;
        viewDesc.MipSlice           = 0;
        viewDesc.MipLevels          = desc.MipLevels;
        viewDesc.FirstArraySlice    = 0;
        viewDesc.ArraySize          = 1;

        for(auto i=0; i<2; ++i)
        {
            auto ret = g_pDevice->CreateRenderTargetView(g_pColorBuffer[i], &viewDesc, &g_pColorView[i]);
            assert(ret == true);
            A3D_UNUSED(ret);
        }
    }

    // 深度バッファの生成.
    {
        a3d::TextureDesc desc = {};
        desc.Dimension          = a3d::RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width              = g_pApp->GetWidth();
        desc.Height             = g_pApp->GetHeight();
        desc.DepthOrArraySize   = 1;
        desc.Format             = a3d::RESOURCE_FORMAT_D32_FLOAT;
        desc.MipLevels          = 1;
        desc.SampleCount        = 1;
        desc.Layout             = a3d::RESOURCE_LAYOUT_OPTIMAL;
        desc.Usage              = a3d::RESOURCE_USAGE_DEPTH_STENCIL;
        desc.InitState          = a3d::RESOURCE_STATE_DEPTH_WRITE;
        desc.HeapType           = a3d::HEAP_TYPE_DEFAULT;

        auto ret = g_pDevice->CreateTexture(&desc, &g_pDepthBuffer);
        assert(ret == true);
        A3D_UNUSED(ret);

        a3d::TargetViewDesc viewDesc = {};
        viewDesc.Dimension          = a3d::VIEW_DIMENSION_TEXTURE2D;
        viewDesc.Format             = desc.Format;
        viewDesc.MipSlice           = 0;
        viewDesc.MipLevels          = desc.MipLevels;
        viewDesc.FirstArraySlice    = 0;
        viewDesc.ArraySize          = desc.DepthOrArraySize;

        ret = g_pDevice->CreateDepthStencilView(g_pDepthBuffer, &viewDesc, &g_pDepthView);
        assert(ret == true);
    }

    // オフスクリーンターゲットの生成.
    {
        a3d::TextureDesc desc = {};
        desc.Dimension          = a3d::RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width              = g_pApp->GetWidth();
        desc.Height             = g_pApp->GetHeight();
        desc.DepthOrArraySize   = 1;
        desc.Format             = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        desc.MipLevels          = 1;
        desc.SampleCount        = 1;
        desc.Layout             = a3d::RESOURCE_LAYOUT_OPTIMAL;
        desc.Usage              = a3d::RESOURCE_USAGE_RENDER_TARGET | a3d::RESOURCE_USAGE_SHADER_RESOURCE;
        desc.InitState          = a3d::RESOURCE_STATE_GENERAL;
        desc.HeapType           = a3d::HEAP_TYPE_DEFAULT;

        auto ret = g_pDevice->CreateTexture(&desc, &g_pOffScreenBuffer);
        assert(ret == true);
        A3D_UNUSED(ret);

        a3d::TargetViewDesc rtvDesc = {};
        rtvDesc.Dimension          = a3d::VIEW_DIMENSION_TEXTURE2D;
        rtvDesc.Format             = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.MipSlice           = 0;
        rtvDesc.MipLevels          = desc.MipLevels;
        rtvDesc.FirstArraySlice    = 0;
        rtvDesc.ArraySize          = desc.DepthOrArraySize;

        ret = g_pDevice->CreateRenderTargetView(g_pOffScreenBuffer, &rtvDesc, &g_pOffScreenTargetView);
        assert(ret == true);

        a3d::ShaderResourceViewDesc srvDesc = {};
        srvDesc.Dimension       = a3d::VIEW_DIMENSION_TEXTURE2D;
        srvDesc.Format          = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        srvDesc.MipSlice        = 0;
        srvDesc.MipLevels       = desc.MipLevels;
        srvDesc.FirstElement    = 0;
        srvDesc.ElementCount    = desc.DepthOrArraySize;

        ret = g_pDevice->CreateShaderResourceView(g_pOffScreenBuffer, &srvDesc, &g_pOffScreenTextureView);
        assert(ret == true);
    }

    // ビューポートの設定.
    g_Viewport.X        = 0;
    g_Viewport.Y        = 0;
    g_Viewport.Width    = float(w);
    g_Viewport.Height   = float(h);

    // シザー矩形の設定.
    g_Scissor.Offset.X      = 0;
    g_Scissor.Offset.Y      = 0;
    g_Scissor.Extent.Width  = w;
    g_Scissor.Extent.Height = h;

    g_Prepare = true;
}

//-------------------------------------------------------------------------------------------------
//      セピアカラー行列を生成します.
//-------------------------------------------------------------------------------------------------
Mat4 CreateSepiaMatrix(float tone)
{
    const Vec3 kWhiteSRGB(0.298912f, 0.586611f, 0.114478f);
    const Vec3 kSepia(0.941f, 0.784f, 0.569f);

    return Mat4(
        tone * kWhiteSRGB.x * kSepia.x + (1.0f - tone),
        tone * kWhiteSRGB.y * kSepia.x,
        tone * kWhiteSRGB.z * kSepia.x,
        0.0f,

        tone * kWhiteSRGB.x * kSepia.y,
        tone * kWhiteSRGB.y * kSepia.y + (1.0f - tone),
        tone * kWhiteSRGB.z * kSepia.y,
        0.0f,

        tone * kWhiteSRGB.x * kSepia.z,
        tone * kWhiteSRGB.y * kSepia.z,
        tone * kWhiteSRGB.z * kSepia.z + (1.0f - tone),
        0.0f,

        0.0f,
        0.0f,
        0.0f,
        1.0f);
}

//-------------------------------------------------------------------------------------------------
//      グレースケール行列を生成します.
//-------------------------------------------------------------------------------------------------
Mat4 CreateGrayScaleMatrix(float value)
{
    const Vec3 kGrayScale(0.22015f, 0.706655f, 0.071330f);

    return Mat4(
        value * kGrayScale.x + (1.0f - value),
        value * kGrayScale.y,
        value * kGrayScale.z,
        0.0f,

        value * kGrayScale.x,
        value * kGrayScale.y + (1.0f - value),
        value * kGrayScale.z,
        0.0f,

        value * kGrayScale.x,
        value * kGrayScale.y,
        value * kGrayScale.z + (1.0f - value),
        0.0f,

        0.0f,
        0.0f,
        0.0f,
        1.0f);
}

//-------------------------------------------------------------------------------------------------
//      色相変換行列を生成します.
//-------------------------------------------------------------------------------------------------
Mat4 CreateHueMatrix(float hue)
{
    auto c = cosf(hue);
    auto s = sinf(hue);

    return Mat4(
        0.213f + 0.787f * c - 0.213f * s,
        0.715f - 0.715f * c - 0.715f * s,
        0.072f - 0.072f * c + 0.928f * s,
        0.0f,

        0.213f - 0.213f * c + 0.143f * s,
        0.715f + 0.285f * c + 0.140f * s,
        0.072f - 0.072f * c - 0.283f * s,
        0.0f,
        
        0.213f - 0.213f * c - 0.787f * s,
        0.715f - 0.715f * c - 0.283f * s,
        0.072f + 0.928f * c + 0.072f * s,
        0.0f,
        
        0.0f,
        0.0f,
        0.0f,
        1.0f);
}

//-------------------------------------------------------------------------------------------------
//      彩度変換行列を生成します.
//-------------------------------------------------------------------------------------------------
Mat4 CreateSaturationMatrix(float r, float g, float b)
{
    return Mat4(
        0.213f + 0.787f * r,
        0.715f - 0.715f * g,
        0.072f - 0.072f * b,
        0.0f,

        0.213f - 0.213f * r,
        0.715f + 0.285f * g,
        0.072f - 0.072f * b,
        0.0f,

        0.213f - 0.213f * r,
        0.715f - 0.715f * g,
        0.072f + 0.928f * b,
        0.0f,

        0.0f,
        0.0f,
        0.0f,
        1.0f);
}

//-------------------------------------------------------------------------------------------------
//      輝度変換行列を生成します.
//-------------------------------------------------------------------------------------------------
Mat4 CreateBrightnessMatrix(float value)
{
    return Mat4(
        value, 0.0f, 0.0f, 0.0f,
        0.0f, value, 0.0f, 0.0f,
        0.0f, 0.0f, value, 0.0f,
        0.0f, 0.0f, 0.0f,  1.0f);
}

//-------------------------------------------------------------------------------------------------
//      コントラスト変換行列を生成します.
//-------------------------------------------------------------------------------------------------
Mat4 CreateContrastMatrix(float value)
{
    return Mat4(
        1.0f + value, 0.0f, 0.0f, -0.5f * value,
        0.0f, 1.0f + value, 0.0f, -0.5f * value,
        0.0f, 0.0f, 1.0f + value, -0.5f * value,
        0.0f, 0.0f, 0.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------
//      色反転行列を生成します.
//-------------------------------------------------------------------------------------------------
Mat4 CreateColorInvertMatrix()
{
    return Mat4(
        -1.0f,  0.0f,  0.0f,  1.0f,
         0.0f, -1.0f,  0.0f,  1.0f,
         0.0f,  0.0f, -1.0f,  1.0f,
         0.0f,  0.0f,  0.0f,  1.0f);
}