"""Extrae de source/game_sa/RwHelper.cpp (y de rwtexdict.cpp) el codigo real que prueba bench_rwhelper.cpp: TransformAtomicSphereCenter, los dos callbacks de
RpClumpGetBoundingSphere (0x734970, 0x734ED0), RpClumpGetBoundingSphere (0x734FC0) y TexDictDestroyTextureCallback (0x730E50). Uso: python gen_slice_rwhelper.py <raiz del repo> <salida.inc>"""
import sys
from pathlib import Path
root = Path(sys.argv[1]); out = Path(sys.argv[2])
rw = (root / 'source/game_sa/RwHelper.cpp').read_text(encoding='utf-8').replace('\r\n', '\n')
a = rw.index('namespace {\n// Transforma el centro de la esfera del atomo')
b = rw.index('    return clump;\n}\n', rw.index('RpClump* RpClumpGetBoundingSphere(')) + len('    return clump;\n}\n')
td = (root / 'source/game_sa/RenderWare/rw/rwtexdict.cpp').read_text(encoding='utf-8').replace('\r\n', '\n')
c = td.index('RwTexture* TexDictDestroyTextureCallback(')
d = td.index('\n}\n', c) + 3
out.write_text(rw[a:b] + '\n' + td[c:d], encoding='utf-8', newline='\n')
print('slice:', len(rw[a:b]) + len(td[c:d]), 'bytes')
