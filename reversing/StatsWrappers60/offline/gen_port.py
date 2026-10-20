"""Extrae de source/game_sa (Stats.cpp, X87Math.cpp/.h) los trozos de las cuatro funciones del banco StatsWrappers60 para compilarlos en el arnes offline (harness.cpp).

Uso: python gen_port.py <raiz del repo> <carpeta de salida>
Escribe: port_find.inc, port_build.inc, port_cons.inc, port_resp.inc, port_top.inc (StatsFtol e IntStat), x87_block.inc y x87_decl.inc, tal cual estan en las fuentes.
"""
import re
import sys
from pathlib import Path

root = Path(sys.argv[1])
out = Path(sys.argv[2])
out.mkdir(parents=True, exist_ok=True)
stats = (root / 'source/game_sa/Stats.cpp').read_bytes().decode('utf-8').replace('\r\n', '\n')


def between(text, start, end):
    i = text.index(start)
    j = text.index(end, i + len(start))
    return text[i:j]


def put(name, text):
    (out / name).write_bytes(text.encode('utf-8'))


put('port_top.inc', between(stats, 'int32 StatsFtol(float v)', '\n\n') + '\n' + between(stats, 'int32& IntStat(eStats stat)', '\n\n') + '\n')
put('port_find.inc', between(stats, '// 0x559010\nint32 CStats::FindLeastFavoriteRadioStation()', '// 0x559080'))
put('port_build.inc', between(stats, '// 0x559230\nnamespace {', '// 0x559540'))
put('port_cons.inc', between(stats, '// 0x55A780\nnamespace {', '// 0x55B900'))
put('port_resp.inc', between(stats, '// 0x55BC50\nvoid CStats::UpdateRespectStat', '// 0x55BF20'))
x87cpp = (root / 'source/game_sa/X87Math.cpp').read_bytes().decode('utf-8').replace('\r\n', '\n')
x87h = (root / 'source/game_sa/X87Math.h').read_bytes().decode('utf-8').replace('\r\n', '\n')
put('x87_block.inc', x87cpp[x87cpp.index('// --- Lote 60:'):])
put('x87_decl.inc', x87h[x87h.index('// --- Lote 60:'):])
print('ok')
