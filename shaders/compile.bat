setlocal
set SHADER_COMPILER="C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x64\fxc.exe"

mkdir dxbc

%SHADER_COMPILER% /I "../asura-SDK/a3d/res/shader" /E main /T vs_4_0 /Fo "dxbc/simpleQuadVS.cso" simpleQuadVS.hlsl
%SHADER_COMPILER% /I "../asura-SDK/a3d/res/shader" /E main /T ps_4_0 /Fo "dxbc/simpleQuadPS.cso" simpleQuadPS.hlsl

%SHADER_COMPILER% /I "../asura-SDK/a3d/res/shader" /E main /T vs_4_0 /Fo "dxbc/simpleVS.cso" simpleVS.hlsl
%SHADER_COMPILER% /I "../asura-SDK/a3d/res/shader" /E main /T ps_4_0 /Fo "dxbc/simplePS.cso" simplePS.hlsl

%SHADER_COMPILER% /I "../asura-SDK/a3d/res/shader" /E main /T vs_4_0 /Fo "dxbc/simpleTexVS.cso" simpleTexVS.hlsl
%SHADER_COMPILER% /I "../asura-SDK/a3d/res/shader" /E main /T ps_4_0 /Fo "dxbc/simpleTexPS.cso" simpleTexPS.hlsl

%SHADER_COMPILER% /I "../asura-SDK/a3d/res/shader" /E main /T vs_4_0 /Fo "dxbc/imguiVS.cso" imguiVS.hlsl
%SHADER_COMPILER% /I "../asura-SDK/a3d/res/shader" /E main /T ps_4_0 /Fo "dxbc/imguiPS.cso" imguiPS.hlsl

endlocal
