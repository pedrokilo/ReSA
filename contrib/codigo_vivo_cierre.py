"""Verificacion estatica de los descartes de la parte 1 del plan de codigo vivo (V1, V2, V3...) y registro de lo cerrado.

Para cada funcion de los lotes pedidos se comprueba, de forma independiente del informe original (que usaba capstone sobre
funciones de Ghidra), que ningun codigo que pueda ejecutarse la alcanza:

  1. aristas directas (call/jmp/jcc/imm) desde funciones vivas que no se descartan en la misma pasada (hechos de codigo_vivo.py);
  2. busqueda BRUTA del valor de la direccion como dword little-endian en TODA la imagen del EXE, a cualquier alineacion
     (punteros, push/mov imm, tablas, inmediatos dentro de instrucciones mal decodificadas); cada acierto se clasifica por la
     funcion o el hueco que lo contiene y por el estado de esa funcion (reemplazada, viva, descartada, dato);
  3. referencias del C++ por literal, excluyendo enganches;
  4. direcciones calculadas: instrucciones de las funciones vivas que se quedan que construyen constantes cercanas a la base de
     imagen (0x400000..0x400FFF) o que llaman por registro; se listan como cota residual (no hay analisis de flujo de datos).

Salidas (docs/CodigoVivoExe/): cierre-p1.csv (una fila por funcion), cierre.csv (estado por lote; lo lee codigo_vivo_plan.py)
y la columna `cierre` de funciones-vivas.csv. Uso:
  python -X utf8 contrib/codigo_vivo_cierre.py --lotes V1,V2-1,V2-2,V3-1,... [--exe RUTA] [--cache hechos.pkl] [--aplicar]
Sin --aplicar solo imprime el resumen. Es determinista.
"""
import argparse
import bisect
import csv
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
import codigo_vivo as cv  # noqa: E402
from pe_inventory import PE  # noqa: E402

OUT = ROOT / 'docs' / 'CodigoVivoExe'


def leer(p):
    with open(p, encoding='utf-8-sig', newline='') as s:
        return list(csv.DictReader(s))


def escribir(p, filas, campos):
    with open(p, 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos)
        w.writeheader()
        w.writerows(filas)


def busqueda_bruta(data, valor):
    """posiciones de archivo donde aparece el dword little-endian `valor` (a cualquier alineacion)."""
    pat = valor.to_bytes(4, 'little')
    out, i = [], data.find(pat)
    while i != -1:
        out.append(i)
        i = data.find(pat, i + 1)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--lotes', required=True)
    ap.add_argument('--exe', default=cv.EXE_DEFECTO)
    ap.add_argument('--cache', default=None)
    ap.add_argument('--aplicar', action='store_true')
    args = ap.parse_args()
    lotes = args.lotes.split(',')

    hechos = cv.cargar_ejecutado(argparse.Namespace(exe=args.exe, cache=args.cache))
    funcs, ex = hechos['funcs'], hechos['ex']
    pe = PE(Path(args.exe))
    vivas = leer(OUT / 'funciones-vivas.csv')
    por_va = {int(x['va'], 16): x for x in vivas}
    D = {int(x['va'], 16) for x in vivas if x['lote'] in lotes}
    # trozos ordenados para localizar el contenedor de una direccion
    trozos = sorted((s, e, fv) for fv, f in funcs.items() for s, e in f['rangos'])
    ini = [t[0] for t in trozos]

    def contenedor(a):
        i = bisect.bisect_right(ini, a) - 1
        if i >= 0 and trozos[i][0] <= a <= trozos[i][1]:
            return trozos[i][2]
        return None

    hueco_ini = sorted(ex['gap_ini'])
    hi = [h[0] for h in hueco_ini]

    def hueco(a):
        i = bisect.bisect_right(hi, a) - 1
        if i >= 0 and hueco_ini[i][0] <= a <= hueco_ini[i][1]:
            return hueco_ini[i][0]
        return None

    def estado_origen(o):
        if o in D:
            return 'descartada'
        r = por_va.get(o)
        if r is None:
            return 'reemplazada'          # no figura en funciones-vivas.csv: reemplazada (CSV) o enganchada
        return 'viva' if r['estado'] == 'viva' else r['estado']

    # aristas entrantes desde funciones
    entrantes = defaultdict(list)
    for (o, d), kinds in ex['aristas'].items():
        if d in D and o != d:
            entrantes[d].append((o, ','.join(sorted(kinds))))
    # llamadas desde huecos
    desde_hueco = defaultdict(list)
    for g, ts in ex['gap_calls'].items():
        for t in ts:
            if t in D:
                desde_hueco[t].append(g)
    refs_cpp = hechos['refs_src']

    filas = []
    rangos_D = sorted((s, e, fv) for fv in D for s, e in funcs[fv]['rangos'])
    d_ini = [t[0] for t in rangos_D]

    def en_D(a):
        i = bisect.bisect_right(d_ini, a) - 1
        return i >= 0 and rangos_D[i][0] <= a <= rangos_D[i][1]

    for va in sorted(D):
        f = funcs[va]
        bloqueos, neutros, blandos = [], [], []
        funclet = por_va[va]['subgrupo'] == 'funclet'   # sus referencias de hueco/tabla son envolturas y mapas de excepciones
        # 1. aristas directas
        for o, k in entrantes.get(va, ()):
            st = estado_origen(o)
            (bloqueos if st in ('viva',) else neutros).append(f'arista {k} desde 0x{o:08X} ({st})')
        for g in desde_hueco.get(va, ()):
            (neutros if funclet else blandos).append(f'llamada desde hueco 0x{g:08X}')
        # 2. busqueda bruta
        brutos = []
        for pos in busqueda_bruta(pe.data, va):
            a = pe.file_va(pos)
            if a is None:
                continue
            if en_D(a):
                continue
            c = contenedor(a)
            if c is not None:
                brutos.append(('func', c, a))
            else:
                h = hueco(a)
                brutos.append(('hueco' if h is not None else 'dato', h, a))
        for tipo, c, a in brutos:
            if tipo == 'func':
                st = estado_origen(c)
                (bloqueos if st == 'viva' else neutros).append(f'dword en 0x{a:08X} dentro de 0x{c:08X} ({st})')
            elif tipo == 'hueco':
                (neutros if funclet else blandos).append(f'dword en hueco 0x{a:08X}')
            else:
                sec = pe.section(a)
                (neutros if funclet else blandos).append(f'dword en dato 0x{a:08X} ({sec["name"] if sec else "?"})')
        # 3. C++
        for rel, ln, forma in refs_cpp.get(va, ()):
            if forma != 'enganche':
                bloqueos.append(f'C++ {forma} {rel}:{ln}')
        r = por_va[va]
        filas.append(dict(va=f'0x{va:08X}', bytes=f['bytes'], lote=r['lote'], grupo=r['grupo'], subgrupo=r['subgrupo'],
                          bloqueos=len(bloqueos), detalle_bloqueos=' | '.join(bloqueos[:6]),
                          referencias_muertas=len(neutros), detalle_muertas=' | '.join(neutros[:4]),
                          referencias_a_clasificar=len(blandos), detalle_a_clasificar=' | '.join(blandos[:4]),
                          dueno=r['dueno_excepciones'], estado_dueno=r['estado_dueno']))

    # 4. direcciones calculadas / llamadas por registro en las funciones vivas que se quedan
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    resid = Counter()
    base_const = []
    for x in vivas:
        va = int(x['va'], 16)
        if va in D or x['estado'] != 'viva':
            continue
        for s, e in funcs[va]['rangos']:
            off = pe.offset(s, 1)
            if off is None:
                continue
            for ins in md.disasm(pe.data[off:off + (e - s + 1)], s):
                if ins.mnemonic in ('call', 'jmp') and ins.operands and ins.operands[0].type not in (CS_OP_IMM,):
                    op = ins.operands[0]
                    if not (op.type == CS_OP_MEM and op.mem.base == 0 and op.mem.index == 0):
                        resid['llamada_indirecta_por_registro'] += 1
                for op in ins.operands:
                    if op.type == CS_OP_IMM and 0x400000 <= (op.imm & 0xFFFFFFFF) < 0x401000:
                        base_const.append(ins.address)
    resid['constantes_cerca_de_la_base_de_imagen'] = len(base_const)

    # decision por funcion y por lote
    for r in filas:
        if r['bloqueos']:
            r['cierre'] = 'abierta'
        elif r['dueno'] and r['estado_dueno'] == 'viva':
            r['cierre'] = 'condicionada (dueno vivo: se cierra con el lote de su biblioteca)'
        elif r['referencias_a_clasificar']:
            r['cierre'] = 'condicionada (hueco o tabla por clasificar: P6)'
        else:
            r['cierre'] = 'cerrada'
    por_lote = defaultdict(list)
    for r in filas:
        por_lote[r['lote']].append(r)
    print('residuo en funciones vivas que se quedan:', dict(resid), 'direcciones base:', [hex(a) for a in base_const[:10]])
    resumen = []
    for l in lotes:
        c = Counter(r['cierre'] for r in por_lote[l])
        print(l, len(por_lote[l]), dict(c))
        resumen.append((l, len(por_lote[l]), c))
    abiertas = [r for r in filas if r['cierre'] != 'cerrada']
    for r in abiertas[:30]:
        print('  ', r['va'], r['lote'], r['cierre'][:30], r['detalle_bloqueos'][:200])

    if not args.aplicar:
        return
    escribir(OUT / 'cierre-p1.csv', filas, list(filas[0]))
    # estado por lote
    previo = {x['lote']: x for x in leer(OUT / 'cierre.csv')} if (OUT / 'cierre.csv').exists() else {}
    for l, n, c in resumen:
        if c.get('cerrada', 0) == n:
            est = 'cerrado'
        elif c.get('cerrada', 0) == 0:
            est = 'verificado sin cerrar'
        else:
            est = 'parcial'
        previo[l] = dict(lote=l, estado=est,
                         nota=f'{c.get("cerrada", 0)} de {n} funciones verificadas sin alcance por aristas, busqueda bruta de la direccion, referencias del C++ ni llamadas por registro conocidas; '
                              f'{c.get("abierta", 0)} abiertas, {n - c.get("cerrada", 0) - c.get("abierta", 0)} condicionadas (detalle en cierre-p1.csv)')
    escribir(OUT / 'cierre.csv', list(previo.values()), ['lote', 'estado', 'nota'])
    cierre_va = {r['va']: r['cierre'] for r in filas}
    if 'cierre' not in vivas[0]:
        campos = list(vivas[0]) + ['cierre']
    else:
        campos = list(vivas[0])
    for x in vivas:
        if x['va'] in cierre_va:
            x['cierre'] = cierre_va[x['va']]
        else:
            x.setdefault('cierre', '')
    escribir(OUT / 'funciones-vivas.csv', vivas, campos)


if __name__ == '__main__':
    main()
