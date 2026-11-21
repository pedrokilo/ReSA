"""Plan por partes y lotes para sustituir los datos del EXE por definiciones C++ propias (docs/DatosExe/plan-lotes.{md,csv}).

Entrada: docs/DatosExe/objetos.csv y referencias-dll.csv (contrib/datos_exe.py). Cada objeto con referencia de la DLL se asigna
a un grupo por (seccion, origen, forma de acceso, comparticion con funciones vivas del EXE); los grupos se parten en lotes de
hasta 40 objetos o 8 KB por propietario. Los lotes ya aplicados (docs/DatosExe/lotes/*.csv) se marcan como hechos.
Los objetos sin referencia de la DLL se resumen aparte (no son lotes: dependen de codigo vivo o ya no los lee nadie).
"""
import csv
import sys
from collections import Counter, defaultdict
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent
D = RAIZ / 'docs/DatosExe'
MAX_OBJ, MAX_BYTES = 40, 8192
LIMITE_G1 = 1000  # DE01 se aplico entero en un solo lote

GRUPOS = [
    ('G1', 'Constantes escalares de .rdata (StaticRef tipado)', 'DE01', 'bajo'),
    ('G2', 'Constantes de .rdata por otras vias (ayudante, reinterpret_cast, literal, mezcla)', 'DE02', 'bajo'),
    ('G3', 'Tablas y cadenas de .rdata del juego (mas de 8 bytes)', 'DE03', 'bajo'),
    ('G4', 'Datos inicializados (.data) del juego sin lector vivo', 'DE04', 'medio'),
    ('G5', 'Estado en ejecucion (.bss) del juego sin lector vivo', 'DE05', 'medio'),
    ('G6', 'Datos del juego que aun lee codigo vivo del EXE (.data/.bss compartidos)', 'DE06', 'alto'),
    ('G7', 'Datos de RenderWare y D3D (cualquier seccion)', 'DE07', 'alto'),
    ('G8', 'CRT, x87 y bibliotecas (cualquier seccion)', 'DE08', 'alto'),
    ('G9', 'vtables de las clases enganchadas (hook_vtable en .rdata/.data)', 'DE09', 'alto'),
    ('G10', 'Proteccion del EXE (_TEXT_HA, _rwdseg)', 'DE10', 'descartado'),
]
SIMPLES = ('float', 'double', 'int32', 'uint32')


def grupo(o):
    sec, org, formas = o['seccion'], o['origen'], set(o['formas'].split(';'))
    if sec in ('_TEXT_HA', '_rwdseg', '.rsrc'):
        return 'G10'
    if formas == {'hook_vtable'} or formas == {'hook_vtable', 'literal'}:
        return 'G9'
    if org == 'renderware':
        return 'G7'
    if org == 'crt':
        return 'G8'
    if sec == '.rdata':
        if o['formas'] == 'StaticRef' and o['tipos'] in SIMPLES and o['escribe_exe'] != '1':
            return 'G1'
        if int(o['tam']) <= 8:
            return 'G2'
        return 'G3'
    if o['compartido'] == '1':
        return 'G6'
    return 'G4' if sec == '.data' else 'G5'


def main():
    objs = list(csv.DictReader((D / 'objetos.csv').open(encoding='utf-8')))
    refs = list(csv.DictReader((D / 'referencias-dll.csv').open(encoding='utf-8')))
    prop = defaultdict(Counter)
    for r in refs:
        prop[r['va']][r['propietario']] += 1
    hechos = {}
    for f in sorted((D / 'lotes').glob('DE*.csv')) if (D / 'lotes').exists() else []:
        for r in csv.DictReader(f.open(encoding='utf-8')):
            hechos[r['va']] = f.stem
    dll = [o for o in objs if int(o['refs_dll'])]
    solo = [o for o in objs if not int(o['refs_dll'])]
    for o in dll:
        o['grupo'] = grupo(o)
        o['dueno'] = prop[o['va']].most_common(1)[0][0] if prop[o['va']] else ''
        o['t'] = int(o['tam'])
    por_g = defaultdict(list)
    for o in dll:
        por_g[o['grupo']].append(o)

    lotes = []   # (grupo, id, propietario, objetos)
    resumen_g = []
    num = 0
    for gid, titulo, pref, riesgo in GRUPOS:
        os_ = sorted(por_g[gid], key=lambda o: (o['dueno'], int(o['va'], 16)))
        num_g = 0
        cur, cb, cd = [], 0, None

        def cerrar():
            nonlocal cur, cb, cd, num_g
            if cur:
                num_g += 1
                own = Counter(o['dueno'] for o in cur)
                nombres = [k for k, _ in own.most_common(3)]
                lotes.append((gid, f'{pref}{"" if num_g == 1 and gid == "G1" else "-" + str(num_g)}', ', '.join(nombres) + (' ...' if len(own) > 3 else ''), cur))
            cur, cb, cd = [], 0, None
        for o in os_:
            if cur and gid != 'G1' and (len(cur) >= MAX_OBJ or cb + o['t'] > MAX_BYTES):
                cerrar()
            cur.append(o); cb += o['t']
        cerrar()
        fiable = sum(o['t'] for o in os_ if o['tam_origen'] != 'hueco')
        resumen_g.append((gid, titulo, riesgo, len(os_), sum(o['t'] for o in os_), fiable, sum(1 for o in os_ if o['va'] in hechos),
                          len({o['dueno'] for o in os_}), num_g))

    with (D / 'plan-lotes.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['lote', 'grupo', 'propietario', 'objetos', 'bytes', 'seccion', 'va_min', 'va_max', 'estado'])
        for gid, lid, own, os_ in lotes:
            vas = sorted(int(o['va'], 16) for o in os_)
            secs = '/'.join(sorted({o['seccion'] for o in os_}))
            est = 'hecho' if all(o['va'] in hechos for o in os_) else ('parcial' if any(o['va'] in hechos for o in os_) else 'pendiente')
            w.writerow([lid, gid, own, len(os_), sum(o['t'] for o in os_), secs, f'0x{vas[0]:08X}', f'0x{vas[-1]:08X}', est])
    with (D / 'plan-objetos.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'seccion', 'grupo', 'lote', 'propietario', 'tam', 'tam_origen', 'tipos', 'formas', 'refs_dll', 'compartido', 'estado'])
        for gid, lid, own, os_ in lotes:
            for o in os_:
                w.writerow([o['va'], o['seccion'], gid, lid, own, o['t'], o['tam_origen'], o['tipos'], o['formas'], o['refs_dll'],
                            o['compartido'], 'hecho' if o['va'] in hechos else 'pendiente'])

    # resumen de objetos sin referencia de la DLL
    sb = defaultdict(lambda: [0, 0])
    for o in solo:
        if o['funcs_exe_vivas'] != '0':
            k = f"{o['seccion']} · leido por codigo vivo del EXE ({o['origen']})"
        elif o['funcs_exe_reemplazadas'] != '0':
            k = f"{o['seccion']} · solo lo leen funciones ya reemplazadas (candidato a huerfano)"
        else:
            k = f"{o['seccion']} · sin lector directo"
        sb[k][0] += 1; sb[k][1] += int(o['tam'])

    L = ['# Plan de sustitucion de los datos del EXE por partes y lotes', '',
         '[Medicion y resumen](README.md) · [Lotes (CSV)](plan-lotes.csv) · [Objetos por lote (CSV)](plan-objetos.csv)', '',
         f'Generado por `contrib/datos_exe_plan.py`. {len(dll)} objetos con referencia de la DLL repartidos en {len(lotes)} lotes y '
         f'{len(GRUPOS)} grupos; lotes de hasta {MAX_OBJ} objetos o {MAX_BYTES // 1024} KB del mismo propietario. '
         'Los tamanos son cota (tam_origen: tipo declarado, ancho de acceso del EXE o hueco hasta el siguiente objeto, tope 64).', '',
         '## Grupos', '',
         '| Grupo | Descripcion | Riesgo | Lotes | Objetos | Bytes (cota) | Bytes de tamano fiable | Hechos | Propietarios |',
         '| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |']
    for g in resumen_g:
        gid, t, r, n, b, f, h, p, nl = g
        L.append(f'| {gid} ({dict((x[0], x[2]) for x in GRUPOS)[gid]}) | {t} | {r} | {nl} | {n} | {b:,} | {f:,} | {h} | {p} |')
    L += ['', '## Objetos sin referencia de la DLL (no son lotes)', '',
          '| Categoria | Objetos | Bytes (cota) |', '| --- | ---: | ---: |']
    for k, v in sorted(sb.items()):
        L.append(f'| {k} | {v[0]} | {v[1]:,} |')
    for gid, titulo, pref, riesgo in GRUPOS:
        ls = [l for l in lotes if l[0] == gid]
        if not ls:
            continue
        L += ['', f'## {gid} · {titulo}', '', '| Lote | Propietario | Objetos | Bytes | Seccion | Rango | Estado |', '| --- | --- | ---: | ---: | --- | --- | --- |']
        for _, lid, own, os_ in ls:
            vas = sorted(int(o['va'], 16) for o in os_)
            est = 'hecho' if all(o['va'] in hechos for o in os_) else 'pendiente'
            L.append(f"| {lid} | {own} | {len(os_)} | {sum(o['t'] for o in os_):,} | {'/'.join(sorted({o['seccion'] for o in os_}))} | "
                     f"0x{vas[0]:08X}–0x{vas[-1]:08X} | {est} |")
    (D / 'plan-lotes.md').write_text('\n'.join(L) + '\n', encoding='utf-8')
    for g in resumen_g:
        print(g)


if __name__ == '__main__':
    main()
