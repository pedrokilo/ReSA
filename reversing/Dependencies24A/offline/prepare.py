"""Prepara offline/obj/WndProc_h.cpp: copia de source/app/platform/win/WndProc.cpp SIN InjectHooksWndProcStuff (que arrastraria el registro de hooks al enlazar). El resto, incluido
__MainWndProc, es byte a byte el del repositorio. Uso: python prepare.py <raiz del repositorio>"""
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
root = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else here.parents[2]
s = (root / 'source/app/platform/win/WndProc.cpp').read_bytes().decode('utf-8')
a = s.index('void InjectHooksWndProcStuff() {')
(here / 'obj').mkdir(exist_ok=True)
(here / 'obj/WndProc_h.cpp').write_bytes(s[:a].encode('utf-8'))
print('WndProc_h.cpp listo (%d bytes)' % a)
