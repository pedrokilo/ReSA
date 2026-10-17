"""Extrae de TaskSimpleGangDriveBy.cpp (ya parcheado) el constructor y los tres ayudantes para compilarlos con los tipos falsos de h/mock.h.
Uso: python extract_glue.py <ruta a source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp> [carpeta de salida]"""
import sys
from pathlib import Path
src = Path(sys.argv[1]).read_bytes().decode('utf-8').replace('\r\n', '\n')
out = Path(sys.argv[2]) if len(sys.argv) > 2 else Path(__file__).parent
a = src.index('CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(CEntity* target')
b = src.index('CTaskSimpleGangDriveBy::~CTaskSimpleGangDriveBy()')
(out / 'ctor_glue.txt').write_text(src[a:b], encoding='utf-8', newline='\n')
c = src.index('\nnamespace {\n// Constantes del EXE')
(out / 'glue_cpp.txt').write_text(src[c:], encoding='utf-8', newline='\n')
print('ctor_glue.txt y glue_cpp.txt escritos en', out)
