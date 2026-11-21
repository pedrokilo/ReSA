"""Parte 2 de la auditoria: entradas binarias sin nombre catalogado (9.223), destinos explicitos sin catalogo (819) y huecos del .text.

Lo invoca codigo_vivo_informe.main(); escribe entradas-sin-catalogar.csv, destinos-sin-catalogo.csv, huecos.csv, huecos-resumen.csv,
motivos.csv y resumen.json en docs/CodigoVivoExe/.
"""
import csv
import json
import struct
from collections import Counter, defaultdict

from codigo_vivo import DOCS, ROOT, CODE_LO, CODE_HI, hx, leer, region


def motivo_entrada(v, f, est, grupo, sub, ctx):
    C = ctx['C']
    ex = C['ex']
    n = f['nombre']
    rs = ex['rasgos'].get(v, {})
    if grupo == 'Excepciones (Unwind/Catch)':
        return 'funclet de excepciones (Unwind/Catch)'
    if grupo == 'Importaciones y carga retardada' and 'DLL::' in n:
        return 'thunk de importación (jmp [IAT])'
    if f['thunk'] or (rs.get('ins') == 1 and f['bytes'] <= 6):
        return 'thunk o salto puro'
    if v in C['exc'] and C['exc'][v]['categoria'] == 'no-es-funcion':
        return 'fragmento interno (excluida «no es función»)'
    if v in C['exc']:
        return f"excluida del juego ({C['exc'][v]['categoria']})"
    ar = ex['aristas']
    entran = [o for (o, d), k in ar.items() if d == v and 'jcc' in k] if v in ctx['hacia'] else []
    if entran and not f['ghidra_llamadas']:
        return 'fragmento interno (salto condicional entrante, sin llamadas)'
    if grupo in ('D3DX9', 'libpng y zlib', 'QuickTime/QTML'):
        return f'biblioteca: {grupo}'
    if grupo == 'CRT y arranque' or region(v) == 'crt':
        return 'biblioteca: CRT y bibliotecas estáticas (0x820000+)'
    pe = C['pe']
    if 8 <= f['bytes'] <= 8192 and f['bytes'] % 4 == 0:
        off = pe.offset(v, f['bytes'])
        if off is not None:
            w = struct.unpack_from('<' + 'I' * (f['bytes'] // 4), pe.data, off)
            if all(CODE_LO <= x < CODE_HI for x in w):
                return 'datos en .text (tabla de punteros)'
    off = pe.offset(v, f['bytes'])
    if off is not None and all(b in (0xCC, 0x90, 0) for b in pe.data[off:off + f['bytes']]):
        return 'relleno de alineación'
    if est in ('reemplazada', 'enganchada'):
        return 'función real, ya reemplazada (falta nombre en el catálogo)'
    return 'función real viva (sin nombre ni reemplazo)'


def auditar(ctx, out):
    C, est, filas = ctx['C'], ctx['est'], ctx['filas']
    funcs, ex, hooks = C['funcs'], C['ex'], ctx['hooks']
    from codigo_vivo_informe import clasificar, decidir
    por_va = {r['va']: r for r in filas}

    # ---------------- 9.223 entradas sin nombre catalogado
    sin = [v for v, f in sorted(funcs.items()) if not f['catalogo']]
    cuenta = Counter()
    bts = Counter()
    ent_rows = []
    for v in sin:
        f = funcs[v]
        grupo, sub = clasificar(v, C, ctx['eh'])
        m = motivo_entrada(v, f, est[v], grupo, sub, ctx)
        e = est[v]
        cuenta[(m, e)] += 1
        bts[(m, e)] += f['bytes']
        ent_rows.append(dict(va=hx(v), bytes=f['bytes'], region=region(v), estado=e, motivo=m, grupo=grupo,
                             nombre_ghidra=f['nombre'], llamadas_directas=f['ghidra_llamadas'],
                             importa_para_quitar_el_exe='sí' if por_va.get(hx(v), {}).get('decision') in ('SUSTITUIR', 'ENLAZAR', 'FASE8') else 'no'))
    with open(out / 'entradas-sin-catalogar.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(ent_rows[0]))
        w.writeheader()
        w.writerows(ent_rows)
    with open(out / 'motivos.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['motivo', 'estado', 'funciones', 'bytes'])
        for (m, e), n in sorted(cuenta.items(), key=lambda x: (-x[1])):
            w.writerow([m, e, n, bts[(m, e)]])

    # ---------------- 819 destinos explicitos del C++ sin nombre en el catalogo
    dest = leer(ROOT / 'reversing/BinaryCoverageAudit/review-source-calls.csv')
    drows = []
    for r in dest:
        v = int(r['address'], 16)
        f = funcs.get(v)
        if f is None:
            continue
        e = est[v]
        grupo, sub = clasificar(v, C, ctx['eh'])
        fila = por_va.get(hx(v))
        vig = len(ctx['cpp_refs'].get(v, []))
        if e == 'viva':
            reach = [x for x, c in (('C++', v in ctx['alc_cpp']), ('entrada', v in ctx['alc_ent']), ('tabla', v in ctx['ptr_no_eh'])) if c]
            dec, des, nota = decidir(v, f, e, grupo, sub, C, '', bool(ctx['hacia'].get(v)), vig > 0, reach, f['virtuales'],
                                     ctx['raiz_grupo'].get(grupo, 0))
        elif vig:
            dec, des, nota = 'REEMPLAZAR_LLAMADA', 'C++ ya reconstruido', ('el C++ la llama por dirección y un enganche la redirige: '
                                                                         'al retirar el EXE hay que llamar a la función C++ directamente')
        else:
            dec, des, nota = 'RETIRADA', 'sin referencia vigente', 'el C++ actual ya no la llama por dirección (solo el enganche o el comentario)'
        refs = r['source_references'].split('; ')
        drows.append(dict(
            va=hx(v), bytes=r['body_bytes'], nombre_ghidra=r['ghidra_name'], region=region(v), grupo=grupo, subgrupo=sub or '',
            estado=e, enganche=hooks.get(v, ''), referencias_antiguas=len(refs), referencias_vigentes_cpp=vig, primera_referencia=refs[0], forma=r['wrapper_styles'],
            llamador_catalogado=r['sample_catalogued_caller'], aristas_desde_catalogo=r['incoming_edges_from_catalogued_functions'],
            clasificacion_previa=r['review_classification'], decision=dec, destino=des, nota=nota))
    with open(out / 'destinos-sin-catalogo.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(drows[0]))
        w.writeheader()
        w.writerows(drows)

    # ---------------- referencias del C++ por direccion al codigo del EXE (puentes plugin::Call, punteros, literales)
    import bisect as _b
    tz = sorted((s, e, fv) for fv, f in funcs.items() for s, e in f['rangos'])
    tz_s = [x[0] for x in tz]

    def contenedora(a):
        i = _b.bisect_right(tz_s, a) - 1
        return tz[i][2] if i >= 0 and tz[i][0] <= a <= tz[i][1] else None

    prows = []
    for va, rs in sorted(ctx['cpp_refs_all'].items()):
        if va in funcs:
            tipo, e, nombre = 'entrada de función', est[va], funcs[va]['nombre']
            grupo, _ = clasificar(va, C, ctx['eh'])
            if e == 'viva':
                accion = 'SUSTITUIR O ENLAZAR (ver funciones-vivas.csv)'
            else:
                accion = 'LLAMAR AL C++ DIRECTAMENTE (retirar el puente por dirección)'
        else:
            c = contenedora(va)
            tipo = 'mitad de función ' + hx(c) if c else 'dirección en hueco o dato del .text'
            e, nombre, grupo = (est[c] if c else ''), '', ''
            accion = 'REVISAR (no es entrada de función: ¿constante numérica o salto interior?)'
        formas = Counter(r[2] for r in rs)
        prows.append(dict(va=hx(va), tipo=tipo, estado=e, grupo=grupo, nombre_ghidra=nombre, referencias=len(rs),
                          formas=';'.join(f'{k}={n}' for k, n in sorted(formas.items())), primera=f'{rs[0][0]}:{rs[0][1]}',
                          en_catalogo_sin_nombre='sí' if va in {int(r['address'], 16) for r in dest} else 'no', accion=accion))
    with open(out / 'referencias-cpp-por-direccion.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(prows[0]))
        w.writeheader()
        w.writerows(prows)

    # ---------------- huecos del .text (bytes ejecutables sin cuerpo de funcion en Ghidra)
    pe = C['pe']
    init = ctx['init']
    ini_ls = sorted(x for x in init)
    ah = ex['a_hueco']
    ic = ex['imm_codigo']
    ah_s = sorted(ah)
    ic_s = sorted(ic)
    import bisect

    mc_s = sorted(ex['mem_codigo'])

    def es_tabla(b, s):
        w = [struct.unpack_from('<I', b, i)[0] for i in range(((-s) % 4), len(b) - 3, 4)]
        return len(w) >= 2 and sum(CODE_LO <= x < CODE_HI for x in w) >= 0.6 * len(w)

    def contiene(lista, s, e):
        i = bisect.bisect_left(lista, s)
        return i < len(lista) and lista[i] <= e

    hrows = []
    for r in leer(DOCS / 'MemoryMap/binary_unassigned_ranges.csv'):
        if r['section'] != '.text':
            continue
        s, e, n = int(r['va'], 16), int(r['end_va'], 16), int(r['bytes'])
        off = pe.offset(s, 1)
        b = pe.data[off:off + n] if off is not None else b''
        if b and all(x in (0xCC, 0x90, 0) for x in b):
            cat = 'relleno de alineación'
        elif contiene(ini_ls, s, e):
            cat = 'inicializador estático (_initterm)'
        elif b and es_tabla(b, s):
            cat = 'tabla de saltos o punteros'
        elif contiene(mc_s, s, e):
            cat = 'tabla de datos del .text (índices o saltos, referenciada por el código)'
        elif b and 5 <= n <= 64 and b[0] == 0xB8 and b[5:6] == b'\xe9':
            cat = 'envoltura de excepciones (mov eax,tabla; jmp __CxxFrameHandler)'
        elif b and b[0] in (0x59, 0x5E, 0x5F, 0x5B) and b'\xb8' in b[:8] and b'\xe9' in b[5:12] and n <= 64:
            cat = 'envoltura de excepciones (mov eax,tabla; jmp __CxxFrameHandler)'
        elif contiene(ah_s, s, e) or contiene(ic_s, s, e):
            cat = 'código con referencia (sin función de Ghidra)'
        else:
            cat = 'sin referencia directa (datos o código no alcanzado)'
        hrows.append(dict(va=hx(s), fin=hx(e), bytes=n, region=region(s), categoria=cat))
    with open(out / 'huecos.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(hrows[0]))
        w.writeheader()
        w.writerows(hrows)
    hc = defaultdict(Counter)
    for h in hrows:
        hc[(h['categoria'], h['region'])]['n'] += 1
        hc[(h['categoria'], h['region'])]['b'] += h['bytes']
    with open(out / 'huecos-resumen.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['categoria', 'region', 'huecos', 'bytes'])
        for (c, rg), v in sorted(hc.items()):
            w.writerow([c, rg, v['n'], v['b']])

    # ---------------- resumen
    por_estado = Counter(est.values())
    res = dict(
        funciones_ghidra=len(funcs), trozos=sum(f['trozos'] for f in funcs.values()),
        bytes_funciones=sum(f['bytes'] for f in funcs.values()),
        por_estado={k: dict(funciones=v, bytes=sum(funcs[x]['bytes'] for x in funcs if est[x] == k)) for k, v in por_estado.items()},
        vivas_trozos=sum(funcs[x]['trozos'] for x in funcs if est[x] == 'viva'),
        vivas_por_region={rg: dict(funciones=sum(1 for x in funcs if est[x] == 'viva' and region(x) == rg),
                                   bytes=sum(funcs[x]['bytes'] for x in funcs if est[x] == 'viva' and region(x) == rg))
                          for rg in ('juego', 'renderware', 'crt', 'rwcseg')},
        funcinfo_excepciones=ctx['n_fi'],
        inicializadores_tabla=len(init), inicializadores_en_hueco=len(ctx['init_hueco']),
        entradas_sin_catalogar=len(sin), destinos_sin_catalogo=len(drows),
        destinos_por_estado=dict(Counter(r['estado'] for r in drows)),
        destinos_por_decision=dict(Counter(r['decision'] for r in drows)),
        motivos={f'{m}|{e}': dict(funciones=n, bytes=bts[(m, e)]) for (m, e), n in cuenta.items()},
        referencias_cpp_por_direccion=dict(Counter((r['tipo'].split(' ')[0] + ' ' + r['estado']) for r in prows)),
        huecos=dict(huecos=len(hrows), bytes=sum(h['bytes'] for h in hrows)),
        alcanzables_desde_cpp=len(ctx['alc_cpp']), alcanzables_desde_entrada=len(ctx['alc_ent']),
        raices_cpp_vivas=sum(1 for v in ctx['raices_cpp'] if est[v] == 'viva'),
    )
    (out / 'resumen.json').write_text(json.dumps(res, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(res, ensure_ascii=False, indent=1)[:6000])
