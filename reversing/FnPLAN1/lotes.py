"""Lote PLAN1: agrupa las raices (docs/ExeIndependiente/raices.csv) en lotes de sustitucion S1A, S2A... y calcula la curva de cierre.

Reglas:
  * Entran en lotes numerados las raices seguras, probables y las 9 promovidas que NO son de biblioteca ni QuickTime:
    juego (incluye las 2 excluidas) y RenderWare interno. Maximo 32 por lote, objetivo ~25.
  * Biblioteca (CRT, D3DX9, libpng+zlib, QuickTime-gancho) = 'LIB (lote aparte)'; las 31 no reconstruibles (puentes QTML) = 'SQA'.
  * Orden: juego primero (S1A-S4A: sencillas, con riesgo, libjpeg compresion, libjpeg descompresion);
    despues RenderWare por oleada de plan-validacion.md (1 puras, 2 flujos y memoria, 3 dispositivo D3D) y, dentro de la oleada,
    por modulo, riesgo (sin x87/SEH/indirectas primero), nivel de dependencia (las hojas primero) y direccion.
  * --ejecutadas <csv>: lista de direcciones de funciones originales ejecutadas hasta el menu (la que produzca IND1A);
    cuenta cuantas raices de cada lote se ejecutan y reordena por ese numero (lotes calientes primero).
Salidas: docs/ExeIndependiente/lotes-sustitucion.csv, columnas lote/oleada/tipo_trabajo/arranque en raices.csv,
         reversing/FnPLAN1/datos/curva_cierre.csv, resumen_lotes.json
Uso: python -I lotes.py --repo <raiz> [--ejecutadas <csv>]"""
import argparse, bisect, collections, csv, json, math, re
from pathlib import Path

MAXLOTE = 32
OBJETIVO = 25
ORDEN_W1 = ['RtQuat', 'RtBMP', 'RtSlerp', 'RtDict', 'RtAnim', 'RpAnisot', 'RpUVAnim', 'RpHAnim', 'RpSkin', 'RpMatFX', 'RwCore']
ORDEN_W2 = ['RwPlCore', 'RwTexDict', 'RpWorld']
INFRA = {1: 'memoria falsa (cabecera comun de R1A) y listas libres falsas',
         2: 'memoria y flujos falsos (RwStream en memoria con fallos programables; ya existen en 227A-229A)',
         3: 'dispositivo D3D falso (IDirect3DDevice9 con tabla virtual propia; pieza nueva)'}


def rc(p):
    with open(p, encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def wcsv(path, rows, fields):
    with open(path, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fields, extrasaction='ignore')
        w.writeheader()
        w.writerows(rows)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--repo', default=str(Path(__file__).resolve().parents[2]))
    ap.add_argument('--ejecutadas')
    a = ap.parse_args()
    repo = Path(a.repo)
    D = repo / 'docs/ExeIndependiente'
    OUT = repo / 'reversing/FnPLAN1/datos'
    rows = rc(D / 'raices.csv')
    grafo = rc(OUT / 'grafo_raices.csv')
    cierre = rc(OUT / 'cierre_por_raiz.csv')
    W = {int(r['va'], 16): r for r in rc(repo / 'docs/RenderWareExe/funciones.csv')}
    ws = sorted(W)

    def vecino_d3d(v):
        i = bisect.bisect_left(ws, v)
        mods = {W[ws[j]]['clase_modulo'] for j in (i - 1, i) if 0 <= j < len(ws)}
        return any('D3D9' in m for m in mods) and all(('D3D9' in m or m == '') for m in mods)

    ejec = None
    if a.ejecutadas:
        ejec = set()
        with open(a.ejecutadas, encoding='utf-8-sig') as f:
            for ln in f:
                for tok in re.findall(r'0x[0-9A-Fa-f]{6,8}', ln):
                    ejec.add(int(tok, 16))

    R = {int(r['direccion'], 16): r for r in rows}
    activo = {v: r for v, r in R.items() if r['confianza'] != 'ruido'}
    # --- tipo de trabajo
    for v, r in activo.items():
        formas = r['forma_uso']
        es_llamada = 'mov_call' in formas
        n_exe = int(r['llamadoras_en_exe'])
        if r['categoria'] == 'excluida':
            t = 'excluida cerrada por inspeccion: sacar la llamada propia'
        elif r['funcion_propia_sin_hook'].startswith('hook propio'):
            t = 'hook propio ya instalado fuera de hooks.csv: catalogar y validar (banco estandar)'
        elif r['categoria'] == 'RenderWare no reconstruible':
            t = 'puente QTML: inspeccion (sin cuerpo en C++)'
        elif r['categoria'].startswith('biblioteca'):
            t = 'biblioteca: lote aparte'
        elif 'libjpeg' in r['modulo']:
            t = 'callback de libjpeg: cuerpo propio e instalar el puntero propio (el banco compara con el original que sigue cargado)'
        elif es_llamada and r['tipo_uso'] == 'envoltura plugin::Call':
            t = 'convertir la envoltura plugin::Call en cuerpo propio' + (' + hook (la llama el EXE)' if n_exe else '')
        elif es_llamada:
            t = 'convertir la llamada por puntero fijo en cuerpo propio' + (' + hook (la llama el EXE)' if n_exe else '')
        else:
            t = 'callback registrado por direccion (plugin/qsort/memoria): cuerpo propio y registrar el puntero propio'
            if n_exe:
                t += ' + hook (la llama el EXE)'
        r['tipo_trabajo'] = t
        txt = r['texto_fuente']
        r['arranque'] = 'si' if re.search(r'RegisterPlugin|PluginAdd|ctor\(|dtor\(|copy\(|memoryFuncs|memoryAlloc|memoryFree|fileFuncs|FreeList|RwEngine|Initialise|InitialiseRenderWare|diDevice|PluginAttach', txt) or 'PluginAttach' in r['llamadas_desde'] else 'no'

    # --- niveles de dependencia (hojas = 0)
    deps = collections.defaultdict(set)
    for g in grafo:
        a_, b_ = int(g['raiz'], 16), int(g['depende_de'], 16)
        if a_ in activo and b_ in activo:
            deps[a_].add(b_)
    nivel = {}

    def lv(v, pila=()):
        if v in nivel:
            return nivel[v]
        if v in pila:
            return 0
        n = 0
        for d in deps.get(v, ()):
            n = max(n, 1 + lv(d, pila + (v,)))
        nivel[v] = n
        return n

    for v in activo:
        lv(v)

    def riesgo(r):
        fl = set(filter(None, (r['flags_cuerpo'] + ',' + r['flags_cierre']).split(',')))
        pts = 0
        pts += 1 if 'x87' in fl else 0
        pts += 2 if 'seh' in fl else 0
        pts += 1 if 'sse' in fl else 0
        pts += 1 if int(r['llamadas_indirectas']) else 0
        return pts

    # --- clasificacion en grupos y orden de grupos
    juego = {v: r for v, r in activo.items() if r['categoria'] in ('juego', 'excluida')}
    rw = {v: r for v, r in activo.items() if r['categoria'] == 'RenderWare interno'}
    qt = {v: r for v, r in activo.items() if r['categoria'] == 'RenderWare no reconstruible'}
    lib = {v: r for v, r in activo.items() if r['categoria'].startswith('biblioteca')}

    def trocear(items, nombre_base, orden_clave):
        """items: lista de direcciones ya ordenadas. Devuelve trozos casi iguales de <=MAXLOTE (objetivo OBJETIVO)."""
        n = len(items)
        k = max(1, math.ceil(n / OBJETIVO))
        while math.ceil(n / k) > MAXLOTE:
            k += 1
        size = math.ceil(n / k)
        return [items[i:i + size] for i in range(0, n, size)]

    lotes = []  # (grupo, oleada, direcciones, nota)
    # (a) juego
    j_lib = sorted(v for v, r in juego.items() if 'libjpeg' in r['modulo'])
    j_otr = [v for v in juego if v not in set(j_lib)]
    j_otr.sort(key=lambda v: (riesgo(juego[v]) > 0 or juego[v]['categoria'] == 'excluida' or 'WinInput' in juego[v]['llamadas_desde'] or 'CPad' in juego[v]['modulo'] or 'Win' == juego[v]['modulo'], nivel[v], v))
    corte = len([v for v in j_otr if not (riesgo(juego[v]) > 0 or juego[v]['categoria'] == 'excluida' or 'WinInput' in juego[v]['llamadas_desde'] or 'CPad' in juego[v]['modulo'] or 'Win' == juego[v]['modulo'])])
    lotes.append(('juego: envolturas y callbacks sencillos (sin x87 ni SEH)', 'juego', j_otr[:corte], 'banco estandar'))
    lotes.append(('juego: x87, SEH, COM de DirectInput, excluidas', 'juego', j_otr[corte:], 'x87 a 24 bits; SEH; COM falso'))
    mitad = len(j_lib) // 2
    lotes.append(('juego: libjpeg, compresion (0x5C2FF0-0x5C8xxx)', 'juego', j_lib[:mitad], 'estructuras jpeg falsas'))
    lotes.append(('juego: libjpeg, descompresion, memoria y errores', 'juego', j_lib[mitad:], 'estructuras jpeg falsas'))
    # (b) RenderWare por oleada
    def oleada(v, r):
        m = r['modulo']
        if m in ORDEN_W2:
            return 2
        if m == 'RwCore' and vecino_d3d(v):
            return 3
        if 'D3D9' in m:
            return 3
        return 1

    por_ol = {1: collections.defaultdict(list), 2: collections.defaultdict(list), 3: collections.defaultdict(list)}
    for v, r in rw.items():
        m = r['modulo']
        por_ol[oleada(v, r)][m].append(v)
    for ol, orden in ((1, ORDEN_W1), (2, ORDEN_W2), (3, ['RwCore', 'Controlador D3D9 (RwD3D9)'])):
        # modulos pequenos contiguos se juntan
        grupos = [(m, sorted(por_ol[ol][m], key=lambda v: (nivel[v], riesgo(rw[v]) > 0, v))) for m in orden if por_ol[ol].get(m)]
        fus = []
        for m, its in grupos:
            if fus and len(fus[-1][1]) < 12 and len(fus[-1][1]) + len(its) <= MAXLOTE:
                fus[-1] = (fus[-1][0] + '+' + m, fus[-1][1] + its)
            else:
                fus.append((m, its))
        trozos = []
        for m, its in fus:
            for ch in trocear(its, m, None):
                if trozos and len(ch) < 8 and len(trozos[-1][1]) + len(ch) <= MAXLOTE:
                    trozos[-1] = (trozos[-1][0] + '+' + m if m not in trozos[-1][0] else trozos[-1][0], trozos[-1][1] + ch)
                else:
                    trozos.append((m, ch))
        for m, ch in trozos:
            mods_ch = []
            for v in ch:
                if rw[v]['modulo'] not in mods_ch:
                    mods_ch.append(rw[v]['modulo'])
            lotes.append(('RenderWare %s' % '+'.join(mods_ch), ol, ch, INFRA[ol]))

    # --- numeracion
    nombres = []
    for i, (g, ol, its, nota) in enumerate(lotes, 1):
        nombres.append('S%dA' % i)
    asign = {}
    for (g, ol, its, nota), nm in zip(lotes, nombres):
        for v in its:
            asign[v] = nm
    for v in qt:
        asign[v] = 'SQA'
    for v in lib:
        asign[v] = 'LIB (lote aparte)'

    # --- reordenar por ejecucion hasta el menu si hay datos
    # (solo cambia el orden de la tabla de lotes, no los miembros)
    cierre_por = collections.defaultdict(dict)
    for c in cierre:
        cierre_por[int(c['raiz'], 16)][int(c['funcion'], 16)] = int(c['bytes'])
    BF = {}
    for v, r in R.items():
        pass

    def clos(v):
        # el cierre se guarda por funcion contenedora; la mayoria coincide con la raiz
        return cierre_por.get(v, {})

    # curva de cierre: funciones y bytes del EXE que siguen necesarios tras cerrar los lotes 1..k (solo lo que cuentan las raices de IND0A)
    todas = list(activo)
    resto_lotes = [nm for nm in nombres]
    filas_lotes = []
    cerrado = set()
    union_total = set()
    for v in todas:
        union_total |= set(clos(v))
    bytes_de = {}
    for v in todas:
        for f, b in clos(v).items():
            bytes_de[f] = b
    por_lote = {nm: [v for v, x in asign.items() if x == nm] for nm in nombres}

    def resto_tras(cerrados_lotes):
        vivas = set()
        for v in todas:
            if asign[v] in cerrados_lotes:
                continue
            vivas |= set(clos(v))
        return len(vivas), sum(bytes_de[f] for f in vivas)

    curva = [dict(paso='antes', lote='', raices_abiertas=len(todas), funciones_necesarias=resto_tras(set())[0], bytes_necesarios=resto_tras(set())[1])]
    cerr = set()
    abiertas = len(todas)
    for (g, ol, its, nota), nm in zip(lotes, nombres):
        cerr.add(nm)
        abiertas -= len(its)
        nf, nb = resto_tras(cerr)
        curva.append(dict(paso='tras %s' % nm, lote=nm, raices_abiertas=abiertas, funciones_necesarias=nf, bytes_necesarios=nb))
    cerr.add('SQA')
    abiertas -= len(qt)
    nf, nb = resto_tras(cerr)
    curva.append(dict(paso='tras SQA', lote='SQA', raices_abiertas=abiertas, funciones_necesarias=nf, bytes_necesarios=nb))
    wcsv(OUT / 'curva_cierre.csv', curva, ['paso', 'lote', 'raices_abiertas', 'funciones_necesarias', 'bytes_necesarios'])

    # --- filas por lote
    resumen_lotes = []
    for (g, ol, its, nota), nm in zip(lotes, nombres):
        rs = [activo[v] for v in its]
        fl = collections.Counter()
        for r in rs:
            fs = set(filter(None, (r['flags_cuerpo'] + ',' + r['flags_cierre']).split(',')))
            for f in fs:
                fl[f] += 1
        previos = set()
        for v in its:
            for d in deps.get(v, ()):
                if asign[d] != nm and asign[d].startswith('S') and asign[d] != 'SQA':
                    previos.add(asign[d])
        post = sorted(p for p in previos if int(p[1:-1]) > int(nm[1:-1]))
        previos = sorted(p for p in previos if int(p[1:-1]) < int(nm[1:-1]))
        n_arr = sum(1 for r in rs if r['arranque'] == 'si')
        n_hook = sum(1 for r in rs if int(r['llamadoras_en_exe']) > 0)
        n_ej = sum(1 for v in its if ejec is not None and v in ejec)
        mods = collections.Counter(r['modulo'][:22] for r in rs)
        banco = 'si (metodo estandar)' if (ol == 'juego' and 'libjpeg' not in g) else \
            ('si, con estructuras jpeg falsas' if 'libjpeg' in g else
             ('si, con memoria falsa' if ol == 1 else ('si, con memoria y flujos falsos' if ol == 2 else 'no hasta tener el dispositivo D3D falso')))
        bytes_ = sum(int(r['bytes']) for r in rs)
        fila = dict(lote=nm, grupo=g, oleada=ol, funciones=len(its), bytes=bytes_, modulos='; '.join('%s %d' % kv for kv in mods.most_common(4)),
                    con_x87=fl.get('x87', 0), con_seh=fl.get('seh', 0), con_sse=fl.get('sse', 0),
                    con_hook_necesario=n_hook, sin_hook=len(its) - n_hook, de_arranque=n_arr, ejecutadas_hasta_menu=(n_ej if ejec is not None else ''),
                    depende_de_lotes=','.join(previos), depende_de_lotes_posteriores=','.join(post),
                    infraestructura=nota if ol != 'juego' else nota, banco_metodo_estandar=banco,
                    cierre_exclusivo_funciones=sum(int(r['cierre_exclusivo']) for r in rs), cierre_total_funciones=sum(int(r['cierre_total']) for r in rs),
                    direccion_min='0x%08X' % min(its), direccion_max='0x%08X' % max(its))
        resumen_lotes.append(fila)
    campos = list(resumen_lotes[0])
    # lote SQA y LIB
    resumen_lotes.append(dict(lote='SQA', grupo='puentes QTML (no reconstruibles)', oleada='4 (especiales)', funciones=len(qt), bytes=sum(int(r['bytes']) for r in qt.values()),
                              modulos='QuickTime/QTML (puentes) %d' % len(qt), infraestructura='inspeccion de bytes en EXE y DLL', banco_metodo_estandar='no: validacion por inspeccion'))
    cnt = collections.Counter(r['categoria'] for r in lib.values())
    resumen_lotes.append(dict(lote='LIB', grupo='biblioteca (lote aparte): ' + '; '.join('%s %d' % kv for kv in sorted(cnt.items())), oleada='aparte', funciones=len(lib),
                              bytes=sum(int(r['bytes']) for r in lib.values()), modulos='LIB1A (libpng+zlib), CRT, D3DX9, QuickTime', infraestructura='lotes aparte', banco_metodo_estandar='depende'))
    wcsv(D / 'lotes-sustitucion.csv', resumen_lotes, campos)

    md = ['| Lote | Grupo | Fn | Bytes | x87 | SEH | EXE la llama | Arranque | Depende de | Banco estandar |', '|---|---|---:|---:|---:|---:|---:|---:|---|---|']
    for f in resumen_lotes:
        md.append('| %s | %s | %s | %s | %s | %s | %s | %s | %s | %s |' % (f['lote'], f['grupo'][:70], f['funciones'], f['bytes'], f.get('con_x87', ''), f.get('con_seh', ''),
                                                                          f.get('con_hook_necesario', ''), f.get('de_arranque', ''), (f.get('depende_de_lotes', '') or '-') + (' (y posterior: %s)' % f['depende_de_lotes_posteriores'] if f.get('depende_de_lotes_posteriores') else ''), f['banco_metodo_estandar']))
    (OUT / 'tabla_lotes.md').write_text(chr(10).join(md) + chr(10), encoding='utf-8')

    # --- escribir columnas en raices.csv
    for v, r in R.items():
        r['lote'] = asign.get(v, '')
        r['oleada_validacion'] = ''
        r['tipo_trabajo'] = r.get('tipo_trabajo', 'ruido: no es raiz' if r['confianza'] == 'ruido' else '')
        r['arranque'] = r.get('arranque', '')
        if v in rw:
            r['oleada_validacion'] = str(oleada(v, r))
    campos_r = [c for c in rows[0].keys()]
    for extra in ('lote', 'oleada_validacion', 'tipo_trabajo', 'arranque'):
        if extra not in campos_r:
            campos_r.insert(campos_r.index('confianza') + 1 + ('lote', 'oleada_validacion', 'tipo_trabajo', 'arranque').index(extra), extra)
    wcsv(D / 'raices.csv', rows, campos_r)

    res = dict(lotes=len(lotes), por_grupo=collections.Counter(g.split(':')[0].split(' ')[0] for g, _, _, _ in lotes),
               juego=len(juego), rw=len(rw), qt=len(qt), lib=len(lib), activo=len(activo),
               hooks_necesarios=sum(1 for r in activo.values() if int(r['llamadoras_en_exe']) > 0),
               arranque=sum(1 for r in activo.values() if r['arranque'] == 'si'),
               por_oleada={k: sum(1 for v, r in rw.items() if oleada(v, r) == k) for k in (1, 2, 3)})
    (OUT / 'resumen_lotes.json').write_text(json.dumps(res, indent=1, ensure_ascii=False, default=dict), encoding='utf-8')
    print(json.dumps(res, indent=1, ensure_ascii=False, default=dict))
    for f in resumen_lotes:
        print(f['lote'], f['funciones'], f['bytes'], f.get('modulos', '')[:60], '| x87', f.get('con_x87'), 'seh', f.get('con_seh'), 'hook', f.get('con_hook_necesario'), 'arr', f.get('de_arranque'), 'dep', f.get('depende_de_lotes'), 'post', f.get('depende_de_lotes_posteriores'))
    for c in curva:
        print(c)


if __name__ == '__main__':
    main()
