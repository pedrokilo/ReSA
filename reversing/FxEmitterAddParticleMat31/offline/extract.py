"""Extrae de source/game_sa/Fx/FxEmitter.cpp (el codigo REAL de produccion) el texto de AddParticle(matriz) (0x4A4050) y lo deja en add_body.inc, sustituyendo solo las direcciones del EXE
(rand 0x821B1E y tabla de senos 0xBB3E00) por los equivalentes del arnes (aqui no aparecen; se conserva por si acaso). Uso: python -I extract.py <ruta de FxEmitter.cpp>"""
import sys
src = open(sys.argv[1], encoding='utf-8').read().replace(chr(13) + chr(10), chr(10))
def sub(t):
    t = t.replace('reinterpret_cast<int(__cdecl*)()>(0x821B1E)()', 'HarnessExeRand()')
    t = t.replace('reinterpret_cast<const float*>(0xBB3E00)', 'gSinTable')
    return t
c = src.index('// 0x4A4050 (vtable +0x10)')
e = src.index('// 0x4A41E0' + chr(10))
open('add_body.inc', 'w', encoding='utf-8').write(sub(src[c:e]))
print('ok')
