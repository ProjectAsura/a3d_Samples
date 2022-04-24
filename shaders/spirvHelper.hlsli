//-------------------------------------------------------------------------------------------------
// File : spirvHelper.hlsli
// Desc : SPIR-V Helper.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#ifdef __spirv__
#define LOCATION(x)              [[vk::location(x)]]
#define RESOURCE(var, reg, x)    [[vk::binding(x)]] var
#define FLIP_Y(var)              var.y = -var.y
#else
#define LOCATION(x)
#define RESOURCE(var, reg, x)    var : register(reg)
#define FLIP_Y(var)
#endif

