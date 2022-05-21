//-------------------------------------------------------------------------------------------------
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
#include <string>
#include <SampleApp.h>
#include <SampleAllocator.h>
#include <SampleUtil.h>
#include <SampleMath.h>
#include <SampleTarga.h>
#include "gui.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    Vec3 Position;  //!< �ʒu���W�ł�.
    Vec2 TexCoord;  //!< �e�N�X�`�����W�ł�.
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Transform structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Transform
{
    Mat4 World;     //!< ���[���h�s��ł�.
    Mat4 View;      //!< �r���[�s��ł�.
    Mat4 Proj;      //!< �ˉe�s��ł�.
};


//-------------------------------------------------------------------------------------------------
// Forward declarations.
//-------------------------------------------------------------------------------------------------
bool InitA3D();
void TermA3D();
void DrawA3D();
void Resize(uint32_t w, uint32_t h, void* ptr);

//-------------------------------------------------------------------------------------------------
// Global Varaibles.
//-------------------------------------------------------------------------------------------------
IApp*                       g_pApp                  = nullptr;  //!< �E�B���h�E�̐������s���w���p�[�N���X�ł�.
a3d::IDevice*               g_pDevice               = nullptr;  //!< �f�o�C�X�ł�.
a3d::ISwapChain*            g_pSwapChain            = nullptr;  //!< �X���b�v�`�F�C���ł�.
a3d::IQueue*                g_pGraphicsQueue        = nullptr;  //!< �R�}���h�L���[�ł�.
a3d::IFence*                g_pFence                = nullptr;  //!< �t�F���X�ł�.
a3d::IDescriptorSetLayout*  g_pDescriptorSetLayout  = nullptr;  //!< �f�B�X�N���v�^�Z�b�g���C�A�E�g�ł�.
a3d::IPipelineState*        g_pPipelineState        = nullptr;  //!< �p�C�v���C���X�e�[�g�ł�.
a3d::IBuffer*               g_pVertexBuffer         = nullptr;  //!< ���_�o�b�t�@�ł�.
a3d::IBuffer*               g_pIndexBuffer          = nullptr;  //!< �C���f�b�N�X�o�b�t�@�ł�.
a3d::ITexture*              g_pDepthBuffer          = nullptr;  //!< �[�x�o�b�t�@�ł�.
a3d::IDepthStencilView*     g_pDepthView            = nullptr;  //!< �[�x�X�e���V���^�[�Q�b�g�r���[�ł�.
a3d::ITexture*              g_pTexture              = nullptr;  //!< �e�N�X�`���ł�.
a3d::IShaderResourceView*   g_pTextureView          = nullptr;  //!< �e�N�X�`���r���[�ł�.
a3d::ISampler*              g_pSampler              = nullptr;  //!< �T���v���[�ł�.
a3d::ITexture*              g_pColorBuffer[2]       = {};       //!< �J���[�o�b�t�@�ł�.
a3d::IRenderTargetView*     g_pColorView[2]         = {};       //!< �J���[�r���[�ł�.
a3d::ICommandList*          g_pCommandList[2]       = {};       //!< �R�}���h���X�g�ł�.
a3d::IBuffer*               g_pConstantBuffer[2]    = {};       //!< �萔�o�b�t�@�ł�.
a3d::IConstantBufferView*   g_pConstantView[2]      = {};       //!< �萔�o�b�t�@�r���[�ł�.
a3d::Viewport               g_Viewport              = {};       //!< �r���[�|�[�g�ł�.
a3d::Rect                   g_Scissor               = {};       //!< �V�U�[��`�ł�.
Transform                   g_Transform             = {};       //!< �ϊ��s��ł�.
float                       g_RotateAngle           = 0.0f;     //!< ��]�p�ł�.
void*                       g_pCbHead[2]            = {};       //!< �萔�o�b�t�@�̐擪�|�C���^�ł�.
float                       g_ClearColor[4]         = {};       //!< �N���A�J���[�ł�.
float                       g_RotateSpeed           = 1.0f;     //!< ��]���x�ł�.
bool                        g_Prepare               = false;    //!< �������o������true.
SampleAllocator             g_Allocator;                        //!< �A���P�[�^.

//-------------------------------------------------------------------------------------------------
//      ���C���G���g���[�|�C���g�ł�.
//-------------------------------------------------------------------------------------------------
void Main()
{
    // �E�B���h�E����.
    if (!CreateApp(960, 540, &g_pApp))
    { return; }

    // ���T�C�Y���̃R�[���o�b�N�֐���ݒ�.
    g_pApp->SetResizeCallback(Resize, nullptr);

    // A3D������.
    if (!InitA3D())
    {
        g_pApp->Release();
        return;
    }

    // ���C�����[�v.
    while( g_pApp->IsLoop() )
    {
        // �`�揈��.
        DrawA3D();
    }

    // ��n��.
    TermA3D();
    g_pApp->Release();
}

//-------------------------------------------------------------------------------------------------
//      A3D�̏��������s���܂�.
//-------------------------------------------------------------------------------------------------
bool InitA3D()
{
    g_Prepare = false;

    // �O���t�B�b�N�X�V�X�e���̏�����.
    {
        a3d::SystemDesc desc = {};
        desc.pSystemAllocator = &g_Allocator;

        if (!a3d::InitSystem(&desc))
        { return false; }
    }

    // �f�o�C�X�̐���.
    {
        a3d::DeviceDesc desc = {};

        desc.EnableDebug = true;

        // �ő�f�B�X�N���v�^����ݒ�.
        desc.MaxColorTargetCount            = 256;
        desc.MaxDepthTargetCount            = 256;
        desc.MaxShaderResourceCount         = 256;
        desc.MaxSamplerCount                = 256;

        // �ő�T�u�~�b�g����ݒ�.
        desc.MaxGraphicsQueueSubmitCount    = 256;
        desc.MaxCopyQueueSubmitCount        = 256;
        desc.MaxComputeQueueSubmitCount     = 256;

        // �f�o�C�X�𐶐�.
        if (!a3d::CreateDevice(&desc, &g_pDevice))
        { return false; }

        // �R�}���h�L���[���擾.
        g_pDevice->GetGraphicsQueue(&g_pGraphicsQueue);
    }

    auto info = g_pDevice->GetInfo();

    #if SAMPLE_IS_VULKAN && A3D_IS_WIN
        auto format = a3d::RESOURCE_FORMAT_B8G8R8A8_UNORM;
    #else
        auto format = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
    #endif

    // �X���b�v�`�F�C���̐���.
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

        // �X���b�v�`�F�C������o�b�t�@���擾.
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

    // �[�x�o�b�t�@�̐���.
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

    // �R�}���h���X�g�𐶐�.
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

    // �t�F���X�𐶐�.
    {
        if (!g_pDevice->CreateFence(&g_pFence))
        { return false; }
    }

    // ���_�o�b�t�@�𐶐�.
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

    // �C���f�b�N�X�o�b�t�@�𐶐�.
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

    // �萔�o�b�t�@�𐶐�.
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

        Vec3 pos = Vec3(0.0f, 0.0f, 5.0f);
        Vec3 at  = Vec3(0.0f, 0.0f, 0.0f);
        Vec3 up  = Vec3(0.0f, 1.0f, 0.0f);

        auto aspect = static_cast<float>(g_pApp->GetWidth()) / static_cast<float>(g_pApp->GetHeight());

        g_Transform.World = Mat4::Identity();
        g_Transform.View  = Mat4::LookAt(pos, at, up);
        g_Transform.Proj  = Mat4::PersFov(ToRadian(45.0f), aspect, 0.1f, 1000.0f);
    }

    // �V�F�[�_�o�C�i����ǂݍ��݂܂�.
    a3d::ShaderBinary vs = {};
    a3d::ShaderBinary ps = {};
    {
        FixedSizeString vsPath;
        FixedSizeString psPath;
        GetShaderPath("SimpleTexVS", vsPath);
        GetShaderPath("SimpleTexPS", psPath);

        if (!LoadShaderBinary(vsPath.c_str(), vs))
        { return false; }

        if (!LoadShaderBinary(psPath.c_str(), ps))
        {
            DisposeShaderBinary(vs);
            return false;
        }
    }

    // �f�B�X�N���v�^�Z�b�g���C�A�E�g�𐶐����܂�.
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
        desc.EntryCount                = 2;

        desc.Entries[0].ShaderStasge   = a3d::SHADER_STAGE_VERTEX;
        desc.Entries[0].ShaderRegister = 0;
        desc.Entries[0].BindLocation   = 0;
        desc.Entries[0].Type           = a3d::DESCRIPTOR_TYPE_CBV;

        desc.Entries[1].ShaderStage    = a3d::SHADER_STAGE_PIXEL;
        desc.Entries[1].ShaderRegister = 0;
        desc.Entries[1].BindLocation   = 0;
        desc.Entries[1].Type           = a3d::DESCRIPTOR_TYPE_SRV_T;
    #endif

        if (!g_pDevice->CreateDescriptorSetLayout(&desc, &g_pDescriptorSetLayout))
        { return false; }
    }

    // �O���t�B�b�N�X�p�C�v���C���X�e�[�g�𐶐����܂�.
    {
        // ���͗v�f�ł�.
        a3d::InputElementDesc inputElements[] = {
            { "POSITION", 0, 0, a3d::RESOURCE_FORMAT_R32G32B32_FLOAT , 0,  0, a3d::INPUT_CLASSIFICATION_PER_VERTEX },
            { "TEXCOORD", 0, 5, a3d::RESOURCE_FORMAT_R32G32_FLOAT    , 0, 12, a3d::INPUT_CLASSIFICATION_PER_VERTEX },
        };

        // ���̓��C�A�E�g�ł�.
        a3d::InputLayoutDesc inputLayout = {};
        inputLayout.ElementCount = 2;
        inputLayout.pElements    = inputElements;

        // �O���t�B�b�N�X�p�C�v���C���X�e�[�g��ݒ肵�܂�.
        a3d::GraphicsPipelineStateDesc desc = a3d::GraphicsPipelineStateDesc::Default();

        // �V�F�[�_�̐ݒ�.
        desc.VS = vs;
        desc.PS = ps;

        // ���̓A�E�g�̐ݒ�.
        desc.InputLayout = inputLayout;

        // �f�B�X�N���v�^�Z�b�g���C�A�E�g�̐ݒ�.
        desc.pLayout = g_pDescriptorSetLayout;

        // �t�H�[�}�b�g�̐ݒ�.
        desc.RenderTargetCount  = 1;
        desc.RenderTarget[0]    = format;
        desc.DepthTarget        = a3d::RESOURCE_FORMAT_D32_FLOAT;

        // �L���b�V���ς݃p�C�v���C���X�e�[�g�̐ݒ�.
        desc.pCachedPSO = nullptr;

        // �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̐���.
        if (!g_pDevice->CreateGraphicsPipeline(&desc, &g_pPipelineState))
        {
            DisposeShaderBinary(vs);
            DisposeShaderBinary(ps);
            return false;
        }
    }
     // �s�v�ɂȂ����̂Ŕj�����܂�.
    DisposeShaderBinary(vs);
    DisposeShaderBinary(ps);

    // �e�N�X�`���̐���.
    {
        FixedSizeString path;
        GetTexturePath("sample32bitRLE.tga", path);

        Targa targa;
        if (!targa.Load(path.c_str()))
        { return false; }

        auto rowPitch = targa.GetWidth() * targa.GetBytePerPixel();
        auto size     = rowPitch * targa.GetHeight() ;

        // �e���|�����o�b�t�@�̗p��.
        a3d::BufferDesc bufDesc = {};
        bufDesc.Size        = size;
        bufDesc.InitState   = a3d::RESOURCE_STATE_GENERAL;
        bufDesc.Usage       = a3d::RESOURCE_USAGE_COPY_SRC;
        bufDesc.HeapType    = a3d::HEAP_TYPE_UPLOAD;

        // �e���|�����o�b�t�@�𐶐�.
        a3d::IBuffer* pImmediate = nullptr;
        if (!g_pDevice->CreateBuffer(&bufDesc, &pImmediate))
        { return false; }

        // �e�N�X�`���̐ݒ�.
        a3d::TextureDesc desc = {};
        desc.Dimension          = a3d::RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width              = targa.GetWidth();
        desc.Height             = targa.GetHeight();
        desc.DepthOrArraySize   = 1;
        desc.MipLevels          = 1;
        desc.Format             = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        desc.SampleCount        = 1;
        desc.Layout             = a3d::RESOURCE_LAYOUT_OPTIMAL;
        desc.InitState          = a3d::RESOURCE_STATE_GENERAL;
        desc.Usage              = a3d::RESOURCE_USAGE_SHADER_RESOURCE | a3d::RESOURCE_USAGE_COPY_DST;
        desc.HeapType           = a3d::HEAP_TYPE_DEFAULT;

        // �e�N�X�`���𐶐�.
        if (!g_pDevice->CreateTexture(&desc, &g_pTexture))
        {
            a3d::SafeRelease(pImmediate);
            return false;
        }

        // �e�N�X�`���r���[�̐ݒ�.
        a3d::ShaderResourceViewDesc viewDesc = {};
        viewDesc.Dimension          = a3d::VIEW_DIMENSION_TEXTURE2D;
        viewDesc.Format             = a3d::RESOURCE_FORMAT_R8G8B8A8_UNORM;
        viewDesc.MipSlice           = 0;
        viewDesc.MipLevels          = desc.MipLevels;
        viewDesc.FirstElement       = 0;
        viewDesc.ElementCount       = desc.DepthOrArraySize;

        // �e�N�X�`���r���[�̐���.
        if (!g_pDevice->CreateShaderResourceView(g_pTexture, &viewDesc, &g_pTextureView))
        {
            a3d::SafeRelease(pImmediate);
            return false;
        }

        // �T�u���\�[�X���C�A�E�g���擾.
        auto layout = g_pTexture->GetSubresourceLayout(0);

        auto dstPtr = static_cast<uint8_t*>(pImmediate->Map());
        assert( dstPtr != nullptr );

        auto srcPtr = targa.GetPixels();
        assert( srcPtr != nullptr );

        // �e���|�����o�b�t�@�Ƀe�N�Z���f�[�^����������.
        for(auto i=0; i<layout.RowCount; ++i)
        {
            memcpy(dstPtr, srcPtr, rowPitch);
            srcPtr += rowPitch;
            dstPtr += layout.RowPitch;
        }
        pImmediate->Unmap();

        a3d::Offset3D offset = {0, 0, 0};

        // �e�N�X�`���ɃR�s�[����.
        g_pCommandList[0]->Begin();
        g_pCommandList[0]->TextureBarrier(
            g_pTexture,
            a3d::RESOURCE_STATE_GENERAL,
            a3d::RESOURCE_STATE_COPY_DST);
        g_pCommandList[0]->CopyBufferToTexture(
            g_pTexture,
            0,
            offset,
            pImmediate,
            0);
        g_pCommandList[0]->TextureBarrier(
            g_pTexture,
            a3d::RESOURCE_STATE_COPY_DST,
            a3d::RESOURCE_STATE_SHADER_READ);
        g_pCommandList[0]->End();
        g_pGraphicsQueue->Submit(g_pCommandList[0]);
        g_pGraphicsQueue->Execute(nullptr);
        g_pGraphicsQueue->WaitIdle();

        // �s�v�ɂȂ����̂Ŕj������.
        a3d::SafeRelease(pImmediate);
    }

    // �T���v���[�̐���.
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

    // �r���[�|�[�g�̐ݒ�.
    g_Viewport.X        = 0.0f;
    g_Viewport.Y        = 0.0f;
    g_Viewport.Width    = static_cast<float>(g_pApp->GetWidth());
    g_Viewport.Height   = static_cast<float>(g_pApp->GetHeight());
    g_Viewport.MinDepth = 0.0f;
    g_Viewport.MaxDepth = 1.0f;

    // �V�U�[��`�̐ݒ�.
    g_Scissor.Offset.X      = 0;
    g_Scissor.Offset.Y      = 0;
    g_Scissor.Extent.Width  = g_pApp->GetWidth();
    g_Scissor.Extent.Height = g_pApp->GetHeight();

    // �f�B�X�N���v�^�Z�b�g�̍X�V.
    for(auto i=0; i<2; ++i)
    {

    }

    GuiMgr::TargetViewInfo targetInfo = {};
    targetInfo.ColorCount       = 1;
    targetInfo.ColorTargets[0]  = format;
    targetInfo.DepthTarget      = a3d::RESOURCE_FORMAT_D32_FLOAT;

    // GUI�}�l�[�W���̏�����.
    if (!GuiMgr::GetInstance().Init(g_pDevice, targetInfo, g_pApp))
    { return false; }

    // �N���A�J���[�̐ݒ�.
    g_ClearColor[0] = 0.25f;
    g_ClearColor[1] = 0.25f;
    g_ClearColor[2] = 0.25f;
    g_ClearColor[3] = 1.0f;

    g_Prepare = true;

    return true;
}

//-------------------------------------------------------------------------------------------------
//      A3D�̏I���������s���܂�.
//-------------------------------------------------------------------------------------------------
void TermA3D()
{
    g_Prepare = false;

    // �A�C�h����ԂɂȂ�܂ő҂�.
    g_pGraphicsQueue->WaitIdle();
    g_pDevice->WaitIdle();

    // GUI�}�l�[�W���̏I������.
    GuiMgr::GetInstance().Term();

    // �_�u���o�b�t�@���\�[�X�̔j��.
    for(auto i=0; i<2; ++i)
    {
        // �R�}���h���X�g�̔j��.
        a3d::SafeRelease(g_pCommandList[i]);

        // �J���[�^�[�Q�b�g�r���[�̔j��.
        a3d::SafeRelease(g_pColorView[i]);

        // �J���[�o�b�t�@�̔j��.
        a3d::SafeRelease(g_pColorBuffer[i]);

        // �萔�o�b�t�@�r���[�̔j��.
        a3d::SafeRelease(g_pConstantView[i]);

        // �j���O�Ƀ������}�b�s���O������.
        if (g_pConstantBuffer[i] != nullptr)
        { g_pConstantBuffer[i]->Unmap(); }

        // �萔�o�b�t�@�̔j��.
        a3d::SafeRelease(g_pConstantBuffer[i]);
    }

    // �T���v���[�̔j��.
    a3d::SafeRelease(g_pSampler);

    // �e�N�X�`���r���[�̔j��.
    a3d::SafeRelease(g_pTextureView);

    // �e�N�X�`���̔j��.
    a3d::SafeRelease(g_pTexture);

    // �p�C�v���C���X�e�[�g�̔j��.
    a3d::SafeRelease(g_pPipelineState);

    // ���_�o�b�t�@�̔j��.
    a3d::SafeRelease(g_pVertexBuffer);

    // �C���f�b�N�X�o�b�t�@�̔j��.
    a3d::SafeRelease(g_pIndexBuffer);

    // �f�B�X�N���v�^�Z�b�g���C�A�E�g�̔j��.
    a3d::SafeRelease(g_pDescriptorSetLayout);

    // �t�F���X�̔j��.
    a3d::SafeRelease(g_pFence);

    // �[�x�X�e���V���^�[�Q�b�g�r���[�̔j��.
    a3d::SafeRelease(g_pDepthView);

    // �[�x�o�b�t�@�̔j��.
    a3d::SafeRelease(g_pDepthBuffer);

    // �X���b�v�`�F�C���̔j��.
    a3d::SafeRelease(g_pSwapChain);

    // �O���t�B�b�N�X�L���[�̔j��.
    a3d::SafeRelease(g_pGraphicsQueue);

    // �f�o�C�X�̔j��.
    a3d::SafeRelease(g_pDevice);

    // �O���t�B�b�N�X�V�X�e���̏I������.
    a3d::TermSystem();
}

//-------------------------------------------------------------------------------------------------
//      A3D�ɂ��`�揈�����s���܂�.
//-------------------------------------------------------------------------------------------------
void DrawA3D()
{
    if (!g_Prepare)
    { return; }

    // �o�b�t�@�ԍ����擾���܂�.
    auto idx = g_pSwapChain->GetCurrentBufferIndex();

    // �萔�o�b�t�@���X�V.
    {
        g_RotateAngle += 0.025f * g_RotateSpeed;
        g_Transform.World = Mat4::RotateY(g_RotateAngle);

        auto ptr = static_cast<uint8_t*>(g_pCbHead[idx]);
        memcpy(ptr, &g_Transform, sizeof(g_Transform));
    }

    // �R�}���h�̋L�^���J�n���܂�.
    auto pCmd = g_pCommandList[idx];
    pCmd->Begin();

    // �������ݗp�̃o���A��ݒ肵�܂�.
    pCmd->TextureBarrier(
        g_pColorBuffer[idx],
        a3d::RESOURCE_STATE_PRESENT,
        a3d::RESOURCE_STATE_COLOR_WRITE);

    a3d::ClearColorValue clearColor = {};
    clearColor.R            = g_ClearColor[0];
    clearColor.G            = g_ClearColor[1];
    clearColor.B            = g_ClearColor[2];
    clearColor.A            = g_ClearColor[3];
    clearColor.SlotIndex    = 0;

    a3d::ClearDepthStencilValue clearDepth = {};
    clearDepth.Depth                = 1.0f;
    clearDepth.Stencil              = 0;
    clearDepth.EnableClearDepth     = true;
    clearDepth.EnableClearStencil   = false;

    a3d::IRenderTargetView* pRTVs[] = {
        g_pColorView[idx]
    };

    // �t���[���o�b�t�@��ݒ肵�܂�.
    pCmd->BeginFrameBuffer(1, pRTVs, g_pDepthView, 1, &clearColor, &clearDepth);

    // 3D�`��.
    {
        // �p�C�v���C���X�e�[�g��ݒ肵�܂�.
        pCmd->SetPipelineState(g_pPipelineState);

        // �r���[�|�[�g�ƃV�U�[��`��ݒ肵�܂�.
        // NOTE : �r���[�|�[�g�ƃV�U�[��`�̐ݒ�́C�K��SetPipelineState() �̌�ł���K�v������܂�.
        pCmd->SetViewports(1, &g_Viewport);
        pCmd->SetScissors (1, &g_Scissor);

        // ���_�o�b�t�@��ݒ肵�܂�.
        pCmd->SetVertexBuffers(0, 1, &g_pVertexBuffer, nullptr);
        pCmd->SetIndexBuffer(g_pIndexBuffer, 0);

    #if SAMPLE_IS_VULKAN || SAMPLE_IS_D3D12 || SAMPLE_IS_D3D11
        pCmd->SetView   (0, g_pConstantView[idx]);
        pCmd->SetSampler(1, g_pSampler);
        pCmd->SetView   (2, g_pTextureView);
    #else
        pCmd->SetView   (0, g_pConstantView[i]);
        pCmd->SetSampler(1, g_pSampler);
        pCmd->SetView   (1, g_pTextureView);
    #endif

        // ��`��`��.
        pCmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
    }

    // 2D�`��.
    {
        // �`��J�n.
        GuiMgr::GetInstance().SwapBuffers();

        static bool show_another_window = false;

        // 1. Show a simple window
        {
            ImGui::SetNextWindowSize(ImVec2(350, 150));
            ImGui::Begin("Test");
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("rotate speed", &g_RotateSpeed, 0.0f, 10.0f);
            ImGui::ColorEdit4("clear color", g_ClearColor);
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 2. Show another simple window
        if (show_another_window)
        {
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello");
            ImGui::End();
        }

        // �`��I��.
        GuiMgr::GetInstance().Issue(pCmd);
    }

    // �\���p�̃o���A��ݒ肷��O�ɁC�t���[���o�b�t�@�̐ݒ����������K�v������܂�.
    pCmd->EndFrameBuffer();

    // �\���p�Ƀo���A��ݒ肵�܂�.
    pCmd->TextureBarrier(
        g_pColorBuffer[idx],
        a3d::RESOURCE_STATE_COLOR_WRITE,
        a3d::RESOURCE_STATE_PRESENT);

    // �R�}���h���X�g�ւ̋L�^���I�����܂�.
    pCmd->End();

    // �R�}���h�L���[�ɓo�^���܂�.
    g_pGraphicsQueue->Submit(pCmd);

    // �R�}���h�����s���܂�.
    g_pGraphicsQueue->Execute(g_pFence);

    // �R�}���h�����s������ҋ@���܂�.
    if (!g_pFence->IsSignaled())
    { g_pFence->Wait(UINT32_MAX); }

    // ��ʂɕ\�����܂�.
    g_pGraphicsQueue->Present( g_pSwapChain );
}

//-------------------------------------------------------------------------------------------------
//      ���T�C�Y�����ł�.
//-------------------------------------------------------------------------------------------------
void Resize( uint32_t w, uint32_t h, void* pUser )
{
    A3D_UNUSED( pUser );

    // �������������ĂȂ���Ԃ�������C�����ł��Ȃ��̂ő��I��.
    if (!g_Prepare || g_pSwapChain == nullptr)
    { return; }

    // �T���v�����ȉ��̏ꍇ�̓N���b�V�����錴���ƂȂ�̂ŏ��������Ȃ�.
    {
        auto desc = g_pSwapChain->GetDesc();
        if ( w < desc.SampleCount || h < desc.SampleCount )
        { return; }
    }

    g_Prepare = false;

    // �A�C�h����ԂɂȂ�܂ő҂�.
    g_pGraphicsQueue->WaitIdle();
    g_pDevice->WaitIdle();

    for(auto i=0; i<2; ++i)
    {
        // �J���[�r���[�̔j��.
        a3d::SafeRelease(g_pColorView[i]);

        // �J���[�o�b�t�@�̔j��.
        a3d::SafeRelease(g_pColorBuffer[i]);
    }

    // �[�x�X�e���V���^�[�Q�b�g�r���[�̔j��.
    a3d::SafeRelease(g_pDepthView);

    // �[�x�o�b�t�@�̔j��.
    a3d::SafeRelease(g_pDepthBuffer);

    // �X���b�v�`�F�C���̃��T�C�Y�����ł�.
    g_pSwapChain->ResizeBuffers( w, h );

    // �e�N�X�`���r���[�𐶐�.
    {
        auto desc = g_pSwapChain->GetDesc();

        // �X���b�v�`�F�C������o�b�t�@���擾.
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

    // �[�x�o�b�t�@�̐���.
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

    // �r���[�|�[�g�̐ݒ�.
    g_Viewport.X        = 0;
    g_Viewport.Y        = 0;
    g_Viewport.Width    = float(w);
    g_Viewport.Height   = float(h);

    // �V�U�[��`�̐ݒ�.
    g_Scissor.Offset.X      = 0;
    g_Scissor.Offset.Y      = 0;
    g_Scissor.Extent.Width  = w;
    g_Scissor.Extent.Height = h;

    g_Prepare = true;
}