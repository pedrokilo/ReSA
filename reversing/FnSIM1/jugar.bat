@echo off
rem Uso: jugar.bat B   (A, B, C, D o E)   |   jugar.bat restaurar
if /I "%~1"=="restaurar" ( pwsh -NoProfile -ExecutionPolicy Bypass -File "%~dp0jugar.ps1" -Restaurar & goto :eof )
pwsh -NoProfile -ExecutionPolicy Bypass -File "%~dp0jugar.ps1" -Variante %1
pause
