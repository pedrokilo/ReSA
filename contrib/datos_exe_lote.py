"""Aplica un lote de DATOS del EXE a C++ propio: genera el par .h/.cpp con las definiciones (valores iniciales extraidos del EXE)
y reescribe en la fuente cada `StaticRef<T>(0xDIR)` al objeto propio.

Requiere haber ejecutado contrib/datos_exe.py (docs/DatosExe/objetos.csv y referencias-dll.csv).
Lotes soportados: DE01 (constantes escalares .rdata: float, double, int32, uint32 con una sola forma StaticRef).

Uso: python contrib/datos_exe_lote.py DE01 [--aplicar]
Sin --aplicar solo informa. Idempotente: tras aplicar, las referencias ya no existen y el lote queda vacio en la siguiente medicion.
Las constantes se definen en un .cpp aparte y se declaran `extern const` para que el compilador las lea de memoria igual que
antes (sin plegado de constantes ni cambios de x87 en los consumidores).
"""
import csv
import struct
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
import re  # noqa: E402

RAIZ = Path(__file__).resolve().parent.parent
EXCLUIR_ARCHIVOS = ('/RenderWare/', '/Crt', '/crt')


def cargar():
    objs = list(csv.DictReader((RAIZ / 'docs/DatosExe/objetos.csv').open(encoding='utf-8')))
    refs = list(csv.DictReader((RAIZ / 'docs/DatosExe/referencias-dll.csv').open(encoding='utf-8')))
    return objs, refs


def seleccionar_de01(objs, refs):
    por_va = defaultdict(list)
    for r in refs:
        por_va[r['va']].append(r)
    sel = []
    for o in objs:
        if o['seccion'] != '.rdata' or not int(o['refs_dll']):
            continue
        if o['formas'] != 'StaticRef' or o['escribe_exe'] == '1':
            continue
        if o['tipos'] not in ('float', 'double', 'int32', 'uint32'):  # bool y char se excluyen: DbgFlag se escribe y el char se usa como char*
            continue
        if any(any(x in r['archivo'] for x in EXCLUIR_ARCHIVOS) for r in por_va[o['va']]):
            continue
        if any(r['tipo'] != o['tipos'] for r in por_va[o['va']]):
            continue
        sel.append(o)
    return sel


def literal(tipo, valor):
    b = bytes.fromhex(valor)
    if tipo == 'float':
        (u,) = struct.unpack('<I', b[:4])
        (f,) = struct.unpack('<f', b[:4])
        return f'std::bit_cast<float>(0x{u:08X}u)', f'{f!r}'
    if tipo == 'double':
        (u,) = struct.unpack('<Q', b[:8])
        (f,) = struct.unpack('<d', b[:8])
        return f'std::bit_cast<double>(0x{u:016X}ull)', f'{f!r}'
    if tipo == 'int32':
        (v,) = struct.unpack('<i', b[:4])
        return f'{v}', f'{v}'
    if tipo == 'uint32':
        (v,) = struct.unpack('<I', b[:4])
        return f'0x{v:08X}u', f'{v}'
    if tipo == 'bool':
        return ('true' if b[0] else 'false'), str(bool(b[0]))
    return f'static_cast<char>({b[0]})', str(b[0])


TIPO_CPP = {'float': 'float', 'double': 'double', 'int32': 'int32', 'uint32': 'uint32', 'bool': 'bool', 'char': 'char'}
TAM = {'float': 4, 'double': 8, 'int32': 4, 'uint32': 4, 'bool': 1, 'char': 1}


def nombre(o):
    return 'k' + o['va'][2:].upper().lstrip('0')


def generar(sel, refs):
    por_va = defaultdict(list)
    for r in refs:
        por_va[r['va']].append(r)
    d = RAIZ / 'source/game_sa/DatosExe'
    d.mkdir(parents=True, exist_ok=True)
    cab = ['#pragma once', '',
           '/*',
           ' * Lote DE01 (docs/DatosExe): constantes escalares de .rdata del EXE (gta_sa.exe 1.0 US) como definiciones propias.',
           ' * Los valores iniciales se extrajeron del EXE canonico (SHA256 72ae59e4...ecc9b) con contrib/datos_exe.py; el',
           ' * nombre lleva la direccion original. Generado por contrib/datos_exe_lote.py; las constantes se definen en',
           ' * RdataConstantes.cpp y se declaran extern para que el compilador las lea de memoria como antes.',
           ' */',
           '', 'namespace DatosExe::Rdata {']
    cpp = ['#include "StdInc.h"', '', '#include "RdataConstantes.h"', '', '#include <bit>', '', 'namespace DatosExe::Rdata {']
    for o in sorted(sel, key=lambda o: int(o['va'], 16)):
        t = TIPO_CPP[o['tipos']]
        lit, human = literal(o['tipos'], o['valor_inicial'])
        users = sorted({Path(r['archivo']).stem for r in por_va[o['va']]})
        cab.append(f'extern const {t} {nombre(o)}; // {o["va"]} = {human} ({", ".join(users)})')
        cpp.append(f'const {t} {nombre(o)} = {lit}; // {o["va"]}')
    cab += ['', '} // namespace DatosExe::Rdata', '']
    cpp += ['', '} // namespace DatosExe::Rdata', '']
    (d / 'RdataConstantes.h').write_text('\n'.join(cab), encoding='utf-8', newline='\n')
    (d / 'RdataConstantes.cpp').write_text('\n'.join(cpp), encoding='utf-8', newline='\n')


def reescribir(sel, refs):
    por_arch = defaultdict(list)
    tipos = {o['va']: o['tipos'] for o in sel}
    nombres = {o['va']: nombre(o) for o in sel}
    for r in refs:
        if r['va'] in tipos:
            por_arch[r['archivo']].append(r)
    total = 0
    for arch in sorted(por_arch):
        p = RAIZ / arch
        raw = p.read_bytes()
        crlf = b'\r\n' in raw
        txt = raw.decode('utf-8', errors='surrogateescape')
        blank = de.blanquear_comentarios(txt)
        patron = re.compile(r'StaticRef\s*<\s*(float|double|int32|uint32)\s*>\s*\(\s*0[xX]([0-9A-Fa-f]{6,8})\s*\)')
        piezas, ult, n = [], 0, 0
        for m in patron.finditer(blank):
            va = f'0x{int(m.group(2), 16):08X}'
            if va in tipos and tipos[va] == m.group(1):
                piezas.append(txt[ult:m.start()])
                piezas.append(f'DatosExe::Rdata::{nombres[va]}')
                ult = m.end(); n += 1
        piezas.append(txt[ult:])
        nuevo = ''.join(piezas)
        if n == 0:
            continue
        inc = '#include "DatosExe/RdataConstantes.h"'
        nl = '\r\n' if crlf else '\n'
        if inc not in nuevo:
            if arch.endswith('.cpp'):
                mm = re.search(r'#include\s+"StdInc.h"[^\n]*\n', nuevo)
            else:
                mm = re.search(r'#pragma once[^\n]*\n', nuevo)
            if not mm:
                raise SystemExit(f'sin punto de insercion de include en {arch}')
            nuevo = nuevo[:mm.end()] + inc + nl + nuevo[mm.end():]
        p.write_bytes(nuevo.encode('utf-8', errors='surrogateescape'))
        total += n
        print(f'{arch}: {n}')
    return total


def main():
    lote = sys.argv[1]
    if lote != 'DE01':
        raise SystemExit('lote desconocido')
    objs, refs = cargar()
    sel = seleccionar_de01(objs, refs)
    print(f'{lote}: {len(sel)} objetos, {sum(int(o["refs_dll"]) for o in sel)} referencias')
    out = RAIZ / 'docs/DatosExe/lotes'
    out.mkdir(parents=True, exist_ok=True)
    with (out / f'{lote}.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'tipo', 'valor_inicial', 'refs_dll', 'archivos', 'nombre'])
        for o in sel:
            w.writerow([o['va'], o['tipos'], o['valor_inicial'], o['refs_dll'], o['archivos'], nombre(o)])
    if '--aplicar' in sys.argv:
        generar(sel, refs)
        print('reescritas', reescribir(sel, refs))


if __name__ == '__main__':
    main()
