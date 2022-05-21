setlocal
set SHADER_COMPILER="C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\x64\dxc.exe"

mkdir dxil


%SHADER_COMPILER% -I "../asura-SDK/a3d/res/shader" -T lib_6_3 -Fo "dxil/simpleRT.cso" simpleRT.hlsl

endlocal
