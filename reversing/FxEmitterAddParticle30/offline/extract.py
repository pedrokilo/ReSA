"""Extrae de source/game_sa/Fx/FxEmitter.cpp (el codigo REAL de produccion) el texto de CreateParticle y de AddParticle(vector) y lo deja en cp_rand.inc / cp_body.inc / add_body.inc,
sustituyendo solo las dos direcciones del EXE (rand 0x821B1E y tabla de senos 0xBB3E00) por los equivalentes del arnes. Uso: python -I extract.py <ruta de FxEmitter.cpp>"""
import sys
src = open(sys.argv[1], encoding='utf-8').read()
def sub(t):
    t = t.replace('reinterpret_cast<int(__cdecl*)()>(0x821B1E)()', 'HarnessExeRand()')
    t = t.replace('reinterpret_cast<const float*>(0xBB3E00)', 'gSinTable')
    return t
a = src.index('// rand() del CRT del EXE (0x821B1E)')
b = src.index('// 0x4A2580 (thiscall, ret 1Ch)')
open('cp_rand.inc', 'w', encoding='utf-8').write(sub(src[a:b]))
open('cp_body.inc', 'w', encoding='utf-8').write(sub(src[b:]))
c = src.index('// 0x4A3EA0 (vtable +0x14)')
e = src.index('// 0x4A4050' + chr(10))
open('add_body.inc', 'w', encoding='utf-8').write(sub(src[c:e]))
print('ok')
