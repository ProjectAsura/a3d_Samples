setlocal
set SHADER_COMPILER="%VK_SDK_PATH%/Bin/dxc.exe"

mkdir spirv

%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -spirv -E main -T vs_4_0 -Fo "spirv/simpleQuadVS.spv" simpleQuadVS.hlsl
%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -spirv -E main -T ps_4_0 -Fo "spirv/simpleQuadPS.spv" simpleQuadPS.hlsl

%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -spirv -E main -T vs_4_0 -Fo "spirv/simpleVS.spv" simpleVS.hlsl
%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -spirv -E main -T ps_4_0 -Fo "spirv/simplePS.spv" simplePS.hlsl

%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -spirv -E main -T vs_4_0 -Fo "spirv/simpleTexVS.spv" simpleTexVS.hlsl
%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -spirv -E main -T ps_4_0 -Fo "spirv/simpleTexPS.spv" simpleTexPS.hlsl

%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -spirv -E main -T vs_4_0 -Fo "spirv/imguiVS.spv" imguiVS.hlsl
%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -spirv -E main -T ps_4_0 -Fo "spirv/imguiPS.spv" imguiPS.hlsl

endlocal
