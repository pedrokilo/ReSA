"""Itera DE05-F: genera DataBssF (modo sonda, sin tocar consumidores), compila ese unico archivo con MSBuild y excluye las direcciones
cuyo static_assert (tamano) o definicion falla, hasta que compila limpio. Deja la lista en docs/DatosExe/lotes/DE05F.excluir.txt,
que se pasa a `datos_exe_de05f.py aplicar --excluir`.

Requisitos: el proyecto ya configurado (build/source/gta_reversed.vcxproj) y `cmake build` ejecutado tras crear DataBssF.cpp por primera
vez (para que el archivo conste en el proyecto: el script lo hace). MSBuild: Visual Studio 2026 Community (ruta en MSBUILD).

Uso: python -X utf8 contrib/datos_exe_de05f_iter.py
"""
import os
import re
import subprocess
import sys
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent
MSBUILD = r'C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe'
CMAKE = next((RAIZ / 'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
F_CPP = RAIZ / 'source/game_sa/DatosExe/DataBssF.cpp'
F_H = RAIZ / 'source/game_sa/DatosExe/DataBssF.h'
EXCL = RAIZ / 'docs/DatosExe/lotes/DE05F.excluir.txt'


def main():
    env = dict(os.environ, DATOSEXE_HUECOS='alcanzables')
    excl = set(EXCL.read_text().split()) if EXCL.exists() else set()
    for _ in range(10):
        for f in (F_CPP, F_H):
            f.unlink(missing_ok=True)       # sus propios literales no deben contar como referencias
        EXCL.write_text('\n'.join(sorted(excl)))
        r = subprocess.run([sys.executable, '-X', 'utf8', str(RAIZ / 'contrib/datos_exe_de05f.py'), 'sonda', '--excluir', str(EXCL)],
                           env=env, capture_output=True, text=True, cwd=RAIZ)
        print(r.stdout.splitlines()[0])
        lineas = {}
        for ln in (RAIZ / 'docs/DatosExe/lotes/DE05F.lineas.csv').read_text().split():
            a, b = ln.split(',')
            lineas[int(a)] = b
        subprocess.run([str(CMAKE), 'build'], cwd=RAIZ, capture_output=True)
        out = subprocess.run([MSBUILD, str(RAIZ / 'build/source/gta_reversed.vcxproj'), '-nologo', '-v:m', '-t:ClCompile', '-p:Configuration=Debug',
                              '-p:Platform=Win32', f'-p:SelectedFiles={F_CPP}', '-p:BuildProjectReferences=false', '-m:1'],
                             capture_output=True, text=True, cwd=RAIZ).stdout
        errores = [int(e) for e in re.findall(r'DataBssF\.cpp\((\d+),\d+\): error', out)]
        if not errores:
            print('compila limpio; excluidos:', len(excl))
            return
        claves = sorted(lineas)
        nuevos = set()
        for e in errores:
            k = max((x for x in claves if x <= e), default=None)
            if k is not None and e - k < 8:
                nuevos.add(lineas[k])
        if not nuevos:
            print(out[-3000:])
            sys.exit(1)
        print('excluyo', sorted(nuevos))
        excl |= nuevos


if __name__ == '__main__':
    main()
