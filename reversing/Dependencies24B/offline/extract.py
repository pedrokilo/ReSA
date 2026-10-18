"""Extrae de WinInput.cpp el texto de diPadInit (desde su comentario de cabecera hasta `#pragma runtime_checks("", restore)`) a dipad_cpp.inc, sin cambios.
Uso: python extract.py [ruta de WinInput.cpp]   (por omision, source/app/platform/win/WinInput.cpp del repositorio)"""
import sys
from pathlib import Path
here = Path(__file__).resolve().parent
src = Path(sys.argv[1]) if len(sys.argv) > 1 else here.parents[2] / 'source/app/platform/win/WinInput.cpp'
t = src.read_text(encoding='utf-8')
a = t.index('// 0x7485C0 - _InputInitialiseJoys')
end = '#pragma runtime_checks("", restore)\n'
b = t.index(end, a) + len(end)
(here / 'dipad_cpp.inc').write_text(t[a:b], encoding='utf-8', newline='\n')
print('extraidos', b - a, 'caracteres de', src)
