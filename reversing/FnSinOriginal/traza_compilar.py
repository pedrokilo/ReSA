"""Compila con el cl.exe de 32 bits bajo el espia de archivos un .cpp del exe independiente (mismas opciones que enlazar.cmd, cl.rsp ya expandido en la salida del enlace)
y registra cada archivo abierto/consultado. Uso: python -I traza_compilar.py <salida del enlace> <fuente.cpp> <carpeta de resultados> [etiqueta]"""
import os, sys, glob, subprocess, ctypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import espiar
salida, fuente, res = sys.argv[1:4]
et = sys.argv[4] if len(sys.argv) > 4 else os.path.splitext(os.path.basename(fuente))[0]
os.makedirs(res, exist_ok=True)
bat = os.path.join(res, '_env.cmd')
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
open(bat, 'w', newline='').write('@echo off\r\ncall "' + VCVARS + '" >nul 2>nul\r\nset\r\n')
for l in subprocess.run(['cmd', '/c', bat], capture_output=True, text=True, errors='replace').stdout.splitlines():
    if '=' in l and not l.startswith('='): k, v = l.split('=', 1); os.environ[k] = v
cl = glob.glob(r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\*\bin\Hostx86\x86\cl.exe')[0]
obj = os.path.join(res, et + '.obj')
args = '@"%s" /DGTASA_STANDALONE_EXE /GS- /Fo"%s" "%s"' % (os.path.join(salida, 'cl.rsp'), obj, fuente)
log = os.path.join(res, 'espia_compilar_%s.log' % et)
pid, h, ht = espiar.lanzar(cl, args, os.path.dirname(fuente), os.path.join(aqui, 'espia_archivos.dll'), log)
ctypes.windll.kernel32.WaitForSingleObject(h, 900000)
c = ctypes.c_ulong(); ctypes.windll.kernel32.GetExitCodeProcess(h, ctypes.byref(c))
print('cl.exe (32 bits, espiado) %s exit=%d obj=%s' % (et, c.value, os.path.exists(obj)))
