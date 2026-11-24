"""Repite el ENLACE del independiente (mismo link.rsp que enlazar.cmd) con el link.exe de 32 bits bajo el espia de archivos, para registrar cada archivo
que el enlazador abre o consulta. El resultado va a salida\trazado\ (no pisa el exe normal). Necesita haber corrido antes enlazar.cmd (usa su link.rsp).
Uso: python -I traza_enlace.py <raiz del repo> <carpeta de salida del enlace> <carpeta de resultados>"""
import os, sys, glob, subprocess, ctypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import espiar
repo, salida, res = sys.argv[1:4]
os.makedirs(res, exist_ok=True)
bat = os.path.join(res, '_env.cmd')
VCVARS = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
open(bat, 'w', newline='').write('@echo off\r\ncall "' + VCVARS + '" >nul 2>nul\r\nset\r\n')
env = subprocess.run(['cmd', '/c', bat], capture_output=True, text=True, errors='replace').stdout
for l in env.splitlines():
    if '=' in l and not l.startswith('='): k, v = l.split('=', 1); os.environ[k] = v
cands = glob.glob(r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\*\bin\Hostx86\x86\link.exe')
assert cands, 'no hay link.exe de 32 bits'
link = cands[0]
os.makedirs(os.path.join(salida, 'trazado'), exist_ok=True)
out = os.path.join(salida, 'trazado', 'gta_sa_standalone.exe')
args = ('/nologo /DEBUG /OUT:"%s" /PDB:"%s" /MACHINE:X86 /SUBSYSTEM:WINDOWS /ENTRY:FaseEntrada /BASE:0x400000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE /INCREMENTAL:NO /SAFESEH:NO /NXCOMPAT:NO /MAP:"%s" @"%s"'
        % (out, os.path.join(salida, 'trazado', 'gta_sa_standalone.pdb'), os.path.join(salida, 'trazado', 'gta_sa_standalone.map'), os.path.join(salida, 'link.rsp')))
log = os.path.join(res, 'espia_enlace.log')
# el link.exe usa la variable de entorno del arbol para dirigir DLLs; no hay rutas del original en ella (se comprueba abajo)
pid, h, ht = espiar.lanzar(link, args, salida, os.path.join(aqui, 'espia_archivos.dll'), log)
ctypes.windll.kernel32.WaitForSingleObject(h, 900000)
c = ctypes.c_ulong(); ctypes.windll.kernel32.GetExitCodeProcess(h, ctypes.byref(c))
print('link.exe (32 bits, espiado) exit=%d  exe=%s existe=%s tam=%s' % (c.value, out, os.path.exists(out), os.path.getsize(out) if os.path.exists(out) else None))
print('variables de entorno del proceso con "Grand Theft" o gta-sa:', [k for k, v in os.environ.items() if 'grand theft' in v.lower() or 'gta-sa' in v.lower() or 'gta_sa.exe' in v.lower()])
