@echo off
rem Asistente de biseccion BIS1. Uso: biseccion.bat            (interactivo, juegas tu)
rem                               biseccion.bat -Auto       (la DLL roba coches sola)
rem                               biseccion.bat -Restaurar  (solo restaurar la copia de pruebas)
rem                               biseccion.bat -Simular    (prueba el circuito sin jugar)
pwsh -NoProfile -ExecutionPolicy Bypass -File "%~dp0biseccion.ps1" %*
pause
