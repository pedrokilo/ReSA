@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul 2>&1
cd /d %~dp0out
cl /nologo /O1 ..\volcar_dll.cpp /Fe:volcar_dll.exe
.\volcar_dll.exe D3DXMatrixMultiply D3DXVec3TransformCoord D3DXVec3TransformNormal D3DXFloat16To32Array D3DXMatrixTranspose
