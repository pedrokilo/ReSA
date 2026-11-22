"""Genera source/game_sa/DatosExe/StaticInit.h/.cpp: los inicializadores estaticos de _initterm que dependen de DatosExe, en C++.

Entrada: docs/DatosExe/inicializadores.csv y inicializadores-escrituras.csv (contrib/datos_exe_inicializadores.py), docs/hooks.csv
(constructores y destructores que ya son C++), cabeceras de source/ (clases declaradas) y los lotes DE05-F (objetos que ya construye
DataBssF.cpp). Salida:
  * StaticInit.cpp: DatosExe::RunStaticInitializers(), que en el ejecutable independiente (cuando el _initterm del EXE ya no corre)
    construye cada objeto estatico con el constructor C++ de su clase (std::construct_at sobre StaticRef<T>(dir)), registra su
    destructor con el atexit del CRT propio (ExeCrt::Onexit) y escribe los valores iniciales (los 21 no nulos de las 35 asignaciones y
    las dos constantes x87 raras). Con la DLL sobre el EXE original NO se llama: el EXE ya ejecuto sus inicializadores.
  * docs/DatosExe/inicializadores-estado.csv: por fila, que se genero y que queda pendiente con su motivo.

Uso: python -X utf8 contrib/datos_exe_staticinit.py [--excluir lista.txt]   (lista de indices C++ a omitir, p. ej. por error de compilacion)
"""
import csv
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from datos_exe_de05f import nombres_declarados  # noqa: E402

RAIZ = Path(__file__).resolve().parent.parent
NL = chr(10)


def clase_de(nombre, decl, sufijo):
    m = re.fullmatch(r'(\w+)::' + sufijo, nombre or '')
    return m.group(1) if m and m.group(1) in decl else None


def cabeceras_de_clases():
    """nombre de clase -> ruta del .h que la declara (se prefiere el que se llama igual)."""
    pat = re.compile(r'(?m)^\s*(?:class|struct)\s+(?:\[\[\w+\]\]\s+)?(\w+)\s*(?:final\s*)?[{:]')
    out = {}
    for p in sorted((RAIZ / 'source').rglob('*.h')):
        txt = p.read_bytes().decode('utf-8', errors='replace')
        for n in pat.findall(txt):
            if n not in out or p.stem == n:
                out[n] = p
    return out


def main():
    hdr = cabeceras_de_clases()
    decl = set(hdr)      # solo clases DEFINIDAS (no basta una declaracion adelantada)
    usadas = set()
    excl = set()
    if '--excluir' in sys.argv:
        excl = {x.strip() for x in Path(sys.argv[sys.argv.index('--excluir') + 1]).read_text().split() if x.strip()}
    filas = list(csv.DictReader((RAIZ / 'docs/DatosExe/inicializadores.csv').open(encoding='utf-8')))
    esc = {}
    for r in csv.DictReader((RAIZ / 'docs/DatosExe/inicializadores-escrituras.csv').open(encoding='utf-8')):
        esc.setdefault((r['tabla'], r['indice']), []).append(r)
    dueno = set()
    f_csv = RAIZ / 'docs/DatosExe/lotes/DE05F.csv'
    if f_csv.exists():
        for r in csv.DictReader(f_csv.open(encoding='utf-8')):
            dueno.add(int(r['va'], 16))
    cuerpo, estado = [], []
    for r in filas:
        clave = (r['tabla'], r['indice'])
        etq = f"{r['tabla']} {r['indice']} ({r['va']})"
        if r['tabla'] == 'C' or r['clase_fase8'].startswith('llamada'):
            estado.append((etq, r['tipo'], 'CRT (StandaloneStartup / CrtStartup.cpp)', ''))
            continue
        if r['indice'] in excl and r['tabla'] == 'C++':
            estado.append((etq, r['tipo'], 'omitida', 'excluida por la lista (error de compilacion)'))
            continue
        obj = int(r['objeto'], 16) if r['objeto'] else None
        if obj in dueno:
            estado.append((etq, r['tipo'], 'construida por DataBssF.cpp', ''))
            continue
        if r['tipo'] == 'ctor':
            cl = clase_de(r['ctor_cpp'], decl, 'Constructor')
            dcl = clase_de(r['dtor_cpp'], decl, 'Destructor')
            if not cl:
                estado.append((etq, 'ctor', 'pendiente', f"constructor {r['ctor']} aun del EXE"
                               + (f" ({r['ctor_cpp']})" if r['ctor_cpp'] else '')))
                continue
            if r['dtor'] and dcl == cl:
                cuerpo.append(f"    // {etq}: {cl} en {r['objeto']}")
                cuerpo.append(f"    ConstructAt<{cl}, {r['objeto']}>();")
                estado.append((etq, 'ctor', 'generada', f"{cl}; destructor {r['dtor']} registrado"))
            else:
                nota = f"destructor {r['dtor']} {r['dtor_cpp']} no es el de la clase" if r['dtor'] else 'sin destructor (el funclet no hace nada)'
                cuerpo.append(f"    // {etq}: {cl} en {r['objeto']} ({nota})")
                cuerpo.append(f"    ConstructAtNoDtor<{cl}, {r['objeto']}>();")
                estado.append((etq, 'ctor', 'generada', f"{cl}; {nota}" + ('; destructor pendiente' if r['dtor'] else '')))
        elif r['tipo'] == 'ctor-vec':
            cl = clase_de(r['ctor_cpp'], decl, 'Constructor')
            if not cl:
                estado.append((etq, 'ctor-vec', 'pendiente', f"constructor de elemento {r['elem_ctor']} aun del EXE"))
                continue
            dcl = clase_de(r['dtor_cpp'], decl, 'Destructor')
            cuerpo.append(f"    // {etq}: {r['cantidad']} x {cl} ({r['tam_elem']} bytes) en {r['objeto']}")
            cuerpo.append(f"    ConstructArrayAt<{cl}, {r['objeto']}, {r['cantidad']}, {r['tam_elem']}>({'true' if dcl == cl else 'false'});")
            estado.append((etq, 'ctor-vec', 'generada', f"{cl} x {r['cantidad']}" + ('' if dcl == cl else '; destructor pendiente')))
        elif r['tipo'] == 'dtor-solo':
            dcl = clase_de(r['dtor_cpp'], decl, 'Destructor')
            if r['dtor'] and dcl and r['objeto']:
                cuerpo.append(f"    // {etq}: destructor de {dcl} en {r['objeto']}")
                cuerpo.append(f"    RegisterDtor<{dcl}, {r['objeto']}>();")
                estado.append((etq, 'dtor-solo', 'generada', dcl))
            elif not r['dtor']:
                estado.append((etq, 'dtor-solo', 'nada que hacer', 'el destructor registrado es un `ret` (objeto trivial)'))
            else:
                estado.append((etq, 'dtor-solo', 'pendiente', f"destructor {r['dtor']} {r['dtor_cpp']} no es de una clase C++"))
        elif r['tipo'] in ('escrituras', 'escrituras+dtor'):
            lineas = []
            omit = 0
            for w in esc.get(clave, []):
                n, st, an, v = int(w['elementos']), int(w['paso']), int(w['ancho']), w['valor']
                a = int(w['direccion'], 16)
                if v == '0x0' and a >= 0x8E4000:
                    omit += 1
                    continue
                tipo = {1: 'uint8', 2: 'uint16', 4: 'uint32'}[an]
                if v.startswith('=['):
                    src = int(v[2:v.index(']')], 16)
                    lineas.append(f"    CopyValue<{tipo}>(0x{a:X}, 0x{src:X});")
                elif v == '?':
                    lineas = None
                    break
                else:
                    lineas.append(f"    StoreRun<{tipo}>(0x{a:X}, {n}, {st if st else an}, {v});")
            if lineas is None:
                estado.append((etq, r['tipo'], 'pendiente', 'valor no determinado por el emulador'))
                continue
            if lineas:
                cuerpo.append(f"    // {etq}: valores iniciales")
                cuerpo += lineas
            estado.append((etq, r['tipo'], 'generada' if lineas else 'nada que hacer',
                           f"{len(lineas)} escrituras" + (f"; {omit} de cero omitidas (el .bss arranca en cero)" if omit else '')
                           + ('; destructor registrado por el EXE es un `ret`' if r['tipo'] == 'escrituras+dtor' else '')))
        else:
            if r['clase_fase8'].startswith('constante x87'):
                estado.append((etq, 'x87', 'generada', 'constante escrita a mano (CrtStaticInit.cpp no la cubre)'))
            else:
                estado.append((etq, r['tipo'], 'pendiente', f"el emulador no la entiende: {r['emulacion']} {r['llamadas_otras']}"))
    # las dos constantes x87 que CrtStaticInit.cpp no cubre
    cuerpo += [
        '    // C++ 1223 (0x852840): cos(pi/8) como float en 0xC18D48 (TaskSimpleGoTo::ms_fLookAtThresholdDotProduct); fld qword [0x870190]; fcos; fstp dword',
        '    StaticRef<float>(0xC18D48) = static_cast<float>(std::cos(0.39269909262657166));',
        '    // C++ 1329 (0x853540): tres restas float en 0xC1C208..0xC1C210 (posicion del arma del Tiger: punto2 - punto1 con las constantes 0x8D318C..0x8D31A0)',
        '    StaticRef<float>(0xC1C208) = StaticRef<float>(0x8D3198) - StaticRef<float>(0x8D318C);',
        '    StaticRef<float>(0xC1C20C) = StaticRef<float>(0x8D319C) - StaticRef<float>(0x8D3190);',
        '    StaticRef<float>(0xC1C210) = StaticRef<float>(0x8D31A0) - StaticRef<float>(0x8D3194);',
    ]
    import os
    for linea in cuerpo:
        m = re.search(r'(?:ConstructAt|ConstructAtNoDtor|ConstructArrayAt|RegisterDtor)<(\w+),', linea)
        if m:
            usadas.add(m.group(1))
    incs = sorted({os.path.relpath(hdr[c], RAIZ / 'source/game_sa/DatosExe').replace(chr(92), '/') for c in usadas if c in hdr})
    cab = ['#pragma once', '', '// Inicializadores estaticos de _initterm que dependen de DatosExe (generado por contrib/datos_exe_staticinit.py).',
           '// RunStaticInitializers() construye los objetos con constructor (std::construct_at sobre su direccion), registra sus destructores con',
           '// el atexit del CRT propio y escribe los valores iniciales, en el orden de la tabla del EXE. Solo para el ejecutable independiente:',
           '// con la DLL sobre el EXE original el _initterm del EXE ya los ejecuto y esta funcion no se llama.', '',
           'namespace DatosExe {', 'void RunStaticInitializers();', '} // namespace DatosExe', '']
    cpp = ['#include "StdInc.h"'] + [f'#include "{i}"' for i in incs] + ['', '#include "StaticInit.h"', '#include "Crt/CrtOnexit.h"', '',
           '// Generado por contrib/datos_exe_staticinit.py; no editar a mano (ver docs/DatosExe/inicializadores-estado.csv).', '',
           'namespace DatosExe {', 'namespace {',
           'template <class T, uintptr_t VA> void __cdecl DestroyAt() { std::destroy_at(&StaticRef<T>(VA)); }',
           'template <class T, uintptr_t VA, size_t N> void __cdecl DestroyArrayAt() {',
           '    for (size_t i = N; i-- > 0;) {', '        std::destroy_at(&StaticRef<T>(VA) + i);', '    }', '}',
           'template <class T, uintptr_t VA> void ConstructAtNoDtor() { std::construct_at(&StaticRef<T>(VA)); }',
           'template <class T, uintptr_t VA> void RegisterDtor() {',
           '    if constexpr (!std::is_trivially_destructible_v<T>) {', '        ExeCrt::Onexit(reinterpret_cast<void*>(&DestroyAt<T, VA>));', '    }', '}',
           'template <class T, uintptr_t VA> void ConstructAt() {', '    ConstructAtNoDtor<T, VA>();', '    RegisterDtor<T, VA>();', '}',
           'template <class T, uintptr_t VA, size_t N, size_t SZ> void ConstructArrayAt(bool withDtor) {',
           '    static_assert(sizeof(T) == SZ, "tamano de elemento distinto del EXE");',
           '    for (size_t i = 0; i < N; ++i) {', '        std::construct_at(&StaticRef<T>(VA) + i);', '    }',
           '    if (withDtor && !std::is_trivially_destructible_v<T>) {', '        ExeCrt::Onexit(reinterpret_cast<void*>(&DestroyArrayAt<T, VA, N>));', '    }', '}',
           'template <class V> void StoreRun(uintptr_t addr, size_t count, size_t stride, V value) {',
           '    for (size_t i = 0; i < count; ++i) {', '        StaticRef<V>(addr + i * stride) = value;', '    }', '}',
           'template <class V> void CopyValue(uintptr_t dst, uintptr_t src) { StaticRef<V>(dst) = StaticRef<V>(src); }',
           '}  // namespace', '', 'void RunStaticInitializers() {'] + cuerpo + ['}', '', '} // namespace DatosExe', '']
    d = RAIZ / 'source/game_sa/DatosExe'
    (d / 'StaticInit.h').write_text(NL.join(cab), encoding='utf-8', newline=NL)
    (d / 'StaticInit.cpp').write_text(NL.join(cpp), encoding='utf-8', newline=NL)
    with (RAIZ / 'docs/DatosExe/inicializadores-estado.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['inicializador', 'tipo', 'estado', 'nota'])
        w.writerows(estado)
    from collections import Counter
    c = Counter((e[2].split(' (')[0], e[1]) for e in estado)
    for k, v in sorted(c.items()):
        print(v, k)


if __name__ == '__main__':
    main()
