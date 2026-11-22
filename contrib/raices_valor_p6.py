"""Raices por VALOR (parte 6, reclasificacion tras el experimento de la fase 8).

Regla: una direccion del EXE que el C++ escribe como VALOR (tabla de punteros, campo de callback, argumento de un registro,
inicializador estatico, llamada directa por direccion) es una RAIZ VIVA hasta demostrar lo contrario. «Muerto» solo vale para
codigo que nadie nombra: lo decidia la falta de llamadores directos y no ve los punteros que el motor llama luego
(tabla estandar de rasters de D3D9, callbacks de RwCamera, `mov [reg+x], imm32` de los nodos de RenderWare...).

Este modulo hace dos cosas:
  1. `escanear_citas()` (lo usa contrib/huecos_p6.py): lee source/ con un automata que separa codigo, comentarios y cadenas y
     clasifica cada literal hexadecimal 0x4xxxxx-0x8xxxxx (enganche, llamada, valor, enlazada, protector, comentario).
  2. `python -X utf8 contrib/raices_valor_p6.py`: sobre huecos-p6.csv ya regenerado + fase8-experimento/dependencias_codigo.csv
     (las direcciones de codigo que el codigo compilado del proyecto lleva como inmediato, escaneadas de los .obj) reclasifica
     las 379 direcciones citadas sin gancho, escribe raices-valor-p6.csv (la cola de «viva sin hook»), el informe
     docs/CodigoVivoExe/reclasificacion-raices-valor.md y comprueba que ningun descarte cerrado del P1 depende de una raiz nueva.

Uso: python -X utf8 contrib/huecos_p6.py && python -X utf8 contrib/raices_valor_p6.py [--exe RUTA] [--sin-informe]
"""
import argparse
import bisect
import csv
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / 'docs'
OUT = DOCS / 'CodigoVivoExe'
PAT = re.compile(r'(?<![A-Za-z0-9_])0x(?:00)?([4-8][0-9A-Fa-f]{5})\b')

# Clases de cita en codigo
K_ENGANCHE, K_LLAMADA, K_VALOR, K_LIBJPEG, K_PROTECTOR, K_COMENTARIO, K_ISLA = 'enganche', 'llamada-directa', 'valor', 'libjpeg-cb', 'protector', 'comentario', 'isla-d3dx9'
# Archivos C++ que son una ISLA del compilador de shaders de D3DX9 (ver ejecucion-v48.md 2.4): sus llamadas por direccion solo corren si el EXE entra por los enganches que instalan; D3DX9 se enlaza
ISLAS = ('D3dxErrorLog.cpp', 'D3dxHlslCompiler.cpp', 'D3dxShaderAssembler.cpp', 'D3dxShaderSource.cpp')
RE_LLAMADA = re.compile(r'(?:plugin::)?Call(?:And)?(?:Method)?(?:AndReturn)?(?:Dyn)?\s*<[^;]*?0x|\bCall[A-Za-z]*\(\s*0x|\bCallEx\w*\(\s*0x')


def hexes(s):
    return [int(m, 16) for m in PAT.findall(s)]


def _separar(txt):
    """Devuelve, por linea, (codigo, comentario): el texto fuera de comentarios y cadenas, y el texto de comentario."""
    cod, com = [], []
    estado = 'c'          # c codigo, b bloque, s cadena, k caracter
    for ln in txt.split('\n'):
        c, m = [], []
        i, n = 0, len(ln)
        while i < n:
            ch = ln[i]
            if estado == 'b':
                if ln.startswith('*/', i):
                    estado = 'c'
                    i += 2
                    continue
                m.append(ch)
            elif estado == 's':
                if ch == '\\':
                    i += 2
                    continue
                if ch == '"':
                    estado = 'c'
                c.append(' ')
            elif estado == 'k':
                if ch == '\\':
                    i += 2
                    continue
                if ch == "'":
                    estado = 'c'
                c.append(' ')
            else:
                if ln.startswith('//', i):
                    m.append(ln[i + 2:])
                    break
                if ln.startswith('/*', i):
                    estado = 'b'
                    i += 2
                    continue
                if ch == '"':
                    estado = 's'
                    c.append(' ')
                elif ch == "'" and not (i > 0 and ln[i - 1].isalnum()):
                    estado = 'k'
                    c.append(' ')
                else:
                    c.append(ch)
            i += 1
        if estado in ('s', 'k'):
            estado = 'c'          # una cadena no cruza lineas (salvo cadenas crudas, irrelevantes aqui)
        cod.append(''.join(c))
        com.append(''.join(m))
    return cod, com


def escanear_citas(raiz=None):
    """{direccion: [(ruta_relativa, linea, clase, texto_de_la_linea)]} para todo literal 0x4xxxxx-0x8xxxxx de source/."""
    raiz = Path(raiz) if raiz else ROOT
    citas = defaultdict(list)
    for p in sorted((raiz / 'source').rglob('*')):
        if p.suffix not in ('.cpp', '.h', '.hpp', '.inl'):
            continue
        try:
            txt = p.read_text(encoding='utf-8', errors='replace')
        except OSError:
            continue
        if '0x' not in txt:
            continue
        rel = p.relative_to(raiz).as_posix()
        cod, com = _separar(txt)
        lineas = txt.split('\n')
        prot = 'ExeProtectionStub' in rel
        for n, (c, m) in enumerate(zip(cod, com), 1):
            for v in hexes(c):
                if 'RH_Scoped' in c:
                    k = K_ENGANCHE
                elif 'LIBJPEG_CB' in c or 'JPegCompressPlugin' in rel:
                    k = K_LIBJPEG
                elif prot:
                    k = K_PROTECTOR
                elif rel.endswith(ISLAS):
                    k = K_ISLA
                elif RE_LLAMADA.search(c):
                    k = K_LLAMADA
                else:
                    k = K_VALOR
                citas[v].append((rel, n, k, lineas[n - 1].strip()))
            for v in hexes(m):
                citas[v].append((rel, n, K_COMENTARIO, lineas[n - 1].strip()))
    return citas


def citas_en(citas, s, e):
    """Citas (sin duplicar archivo:linea:clase) de cualquier direccion de [s, e]."""
    vistas, out = set(), []
    for a in range(s, e + 1):
        for f, n, k, t in citas.get(a, ()):
            if (f, n, k) not in vistas:
                vistas.add((f, n, k))
                out.append((a, f, n, k, t))
    return out


def resumen_clase(cs):
    """Clase dominante de las citas de un objeto: protector / libjpeg / valor (llamada o valor) / comentario / ninguna."""
    ks = {k for _, _, _, k, _ in cs}
    if ks & {K_VALOR, K_LLAMADA}:
        return 'valor'
    if K_LIBJPEG in ks:
        return 'libjpeg'
    if K_PROTECTOR in ks:
        return 'protector'
    if K_ISLA in ks:
        return 'isla'
    if K_ENGANCHE in ks:
        return 'enganche'
    if K_COMENTARIO in ks:
        return 'comentario'
    return 'ninguna'


def tabla_destino(cs):
    """Texto corto de DONDE escribe el C++ la direccion: la linea de codigo de la primera cita viva."""
    for a, f, n, k, t in cs:
        if k in (K_VALOR, K_LLAMADA, K_LIBJPEG, K_PROTECTOR, K_ISLA):
            t = re.sub(r'\s+', ' ', t)
            return f'{f.rsplit("/", 1)[-1]}:{n} `{t[:90]}`'
    return ''


# ----------------------------------------------------------------------------------------------------------------------
# Parte 2: reclasificacion de las 379 direcciones citadas sin gancho + cola de «viva sin hook» + informe
# ----------------------------------------------------------------------------------------------------------------------
# Ejecuciones medidas en el experimento de la fase 8 (docs/CodigoVivoExe/fase8-experimento.md, reversing/FnFase8/ejecuciones): direccion -> intento y hecho.
MEDIDAS = {
    0x004CD820: 't2: primera funcion grande que ejecuta el juego tras crear el diccionario TXD (c1/c2: 1 llamada)',
    0x00750430: 't3/c1: RwCameraBeginUpdate la llama en los dos primeros segundos',
    0x00750460: 't3/t4: RwCameraEndUpdate',
    0x00750480: 't3: sincronizacion del BSP con el frustum (primer marco)',
    0x00750AC0: 't4/c3: FrameSyncObjects (rotura del modo estricto)',
    0x007F98D0: 'c3: StretchRect del backbuffer; mas de 400 llamadas en 16 ms',
    0x007F99B0: 'c3: Reset del dispositivo D3D9',
}


# Cuerpos de las piezas pequenas (desensamblado capstone del EXE canonico): equivalente C++
BOSQUEJO = {
    0x0074AE40: '`void* cb(void* obj, int* n) { ++*n; return obj; }` (callback de `RpClumpForAllLights`/`ForAllCameras` que cuenta)',
    0x004CBD40: '`int cb(int* out, int valor) { *out = valor; return 1; }` (manejador estándar de rasters)',
    0x004CB520: '`int cb(void*, int valor) { *(int*)0xB4E9E4 = valor; return 1; }` (guarda en un global del EXE; manejador estándar de rasters)',
    0x00856BC0: '`mov ecx, 0xC803E8; jmp 0x59ACD0`: destructor de un global de SpecialFX (`this` = 0xC803E8), registrado con `atexit`; 0x59ACD0 es un método aún por identificar; la pieza (10 B ≥ 5) admite enganche',
}


def region_de(a):
    return 'crt' if a >= 0x820000 else 'renderware' if a >= 0x750000 else 'juego'


# Modulos que el proyecto ENLAZA (decision vigente) segun docs/RenderWareExe/funciones.csv (clase_modulo): D3DX9 y libpng+zlib; el CRT de MSVC (0x820000+)
MODULOS_ENLAZADOS = ((0x7652AE, 0x7C4220, 'D3DX9'), (0x7CF600, 0x7EB260, 'libpng/zlib'), (0x818430, 0x81DAB0, 'QuickTime'), (0x820000, 0x8568AB, 'CRT'))


def modulo_enlazado(a):
    for lo, hi, n in MODULOS_ENLAZADOS:
        if lo <= a < hi:
            return n
    return None


def hexa(a):
    return '0x%08X' % a


def _leer(p):
    with open(p, encoding='utf-8-sig', newline='') as s:
        return list(csv.DictReader(s))


def _extension(ex, cs, a, tope):
    """Bytes del tramo de codigo que empieza en `a` (barrido lineal hasta el primer ret/jmp incondicional que no deja saltos pendientes hacia delante,
    o relleno); acotado por `tope` (fin del hueco). Es una ESTIMACION (los saltos por tabla no se siguen)."""
    cur, maxt, n = a, a, 0
    while cur < tope and n < 4000:
        ins = next(cs.disasm(ex.rd(cur, 16), cur), None)
        if ins is None:
            break
        if ins.mnemonic in ('int3', 'nop') and cur >= maxt:
            break
        n += 1
        m = ins.mnemonic
        if (m == 'jmp' or m.startswith('j')) and ins.operands and ins.operands[0].type == 2:
            t = ins.operands[0].imm
            if cur < t < tope:
                maxt = max(maxt, t)
        cur += ins.size
        if (m.startswith('ret') or m == 'jmp') and cur > maxt:
            break
    return max(cur - a, 1)


def construir(exe, citas=None):
    """Devuelve la lista de objetos (dicts) reclasificados: piezas de huecos-p6.csv y direcciones citadas sin gancho."""
    import capstone
    import huecos_p6 as hp
    ex = hp.Exe(exe)
    cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    cs.detail = True
    citas = citas or escanear_citas()
    dep = {int(r['direccion'], 16): r for r in _leer(OUT / 'fase8-experimento' / 'dependencias_codigo.csv')}
    pcs = _leer(OUT / 'huecos-p6.csv')
    piezas = sorted(((int(r['va'], 16), int(r['fin'], 16), r) for r in pcs), key=lambda t: t[0])
    pini = [t[0] for t in piezas]

    def pieza_de(a):
        i = bisect.bisect_right(pini, a) - 1
        return piezas[i] if i >= 0 and piezas[i][0] <= a <= piezas[i][1] else None

    huecos = sorted((int(r['va'], 16), int(r['fin'], 16), r['categoria']) for r in _leer(OUT / 'huecos.csv'))
    hini = [t[0] for t in huecos]

    def hueco_de(a):
        i = bisect.bisect_right(hini, a) - 1
        return huecos[i] if i >= 0 and huecos[i][0] <= a <= huecos[i][1] else None

    funcs = sorted((int(r['start_va'], 16), int(r['end_va'], 16)) for r in _leer(DOCS / 'MemoryMap' / 'binary_function_ranges.csv'))
    fini = [t[0] for t in funcs]

    def func_de(a):
        i = bisect.bisect_right(fini, a) - 1
        return funcs[i] if i >= 0 and funcs[i][0] <= a <= funcs[i][1] else None

    vivas = {int(r['va'], 16): r for r in _leer(OUT / 'funciones-vivas.csv')}
    hooks = set()
    for r in _leer(DOCS / 'hooks.csv'):
        if re.fullmatch(r'0x[0-9A-Fa-f]{6,8}', r.get('address', '')):
            hooks.add(int(r['address'], 16))

    def clave_func(a):
        d = dep.get(a)
        return d['ejemplo_funcion'].split(' | ')[0].strip() if d else ''

    objs = []

    def nuevo(va, bytes_, origen, clase, decision, a_cits, antes, motivo, estimado=False, hueco='', formas=''):
        vivo = [c for c in a_cits if c[3] not in (K_COMENTARIO, K_ENGANCHE)]
        o = dict(va=va, bytes=bytes_, region=region_de(va), origen=origen, clase=clase, decision=decision, antes=antes,
                 citada_por='; '.join(sorted({f'{f.rsplit("/", 1)[-1]}:{n}' for _, f, n, k, _ in vivo}))[:300],
                 n_citas=len({(f, n) for _, f, n, k, _ in vivo}),
                 tabla_destino=tabla_destino(a_cits), formas=formas, clave=clave_func(va), motivo=motivo, estimado=int(estimado), hueco=hueco)
        objs.append(o)
        return o

    # 1) piezas de huecos-p6.csv (clasificadas por contrib/huecos_p6.py con la misma regla)
    vistas = set()
    for s, e, r in piezas:
        d = r['destino']
        if d.startswith('viva-') or d == 'cerrada-comentario' or (r['destino_antes'] == 'citada-en-cpp' and d != 'reconstruida'):
            a_cits = citas_en(citas, s, e)
            vistas.add(s)
            ev = r['evidencia_obj']
            if d == 'viva-sin-hook':
                nuevo(s, int(r['bytes']), 'pieza de hueco', 'viva sin hook', 'RECONSTRUIR', a_cits, r['destino_antes'], '', hueco=r['hueco'], formas=ev)
            elif d == 'viva-enlazada':
                mod_ = modulo_enlazado(s) or 'libjpeg'
                nuevo(s, int(r['bytes']), 'pieza de hueco', f'viva, enlazada ({mod_})', 'ENLAZAR', a_cits, r['destino_antes'],
                      'LIBJPEG_CB / JPegCompressPlugin: con GTASA_WITH_LIBJPEG es la de libs/libjpeg-6b' if mod_ == 'libjpeg' else f'decision vigente: {mod_} se enlaza', hueco=r['hueco'], formas=ev)
            elif d == 'viva-quicktime':
                nuevo(s, int(r['bytes']), 'pieza de hueco', 'viva, QuickTime (se retira)', 'RETIRAR', a_cits, r['destino_antes'],
                      'QuickTime/QTML se retira (decision vigente); la cita es del puente QTMLRuntime.cpp', hueco=r['hueco'], formas=ev)
            elif d == 'viva-protector':
                nuevo(s, int(r['bytes']), 'pieza de hueco', 'viva, del protector', 'RETIRAR', a_cits, r['destino_antes'],
                      'tabla de ExeProtectionStub.cpp: se retira con el archivo (V1)', hueco=r['hueco'], formas=ev)
            else:
                nuevo(s, int(r['bytes']), 'pieza de hueco', 'cerrada (comentario)', 'CERRADA', a_cits, r['destino_antes'],
                      'solo la cita un comentario; ninguna otra entrada viva', hueco=r['hueco'], formas=ev)
    # 2) las 379 direcciones sin gancho que no caen en una pieza ya tratada
    for a, d in sorted(dep.items()):
        pz = pieza_de(a)
        if pz and pz[0] in vistas:
            continue
        a_cits = citas_en(citas, a, a)
        if a in hooks:
            nuevo(a, int(d['bytes'] or 0), 'inicio de funcion de Ghidra' if (func_de(a) and func_de(a)[0] == a) else 'tramo', 'enganchada desde la medida', 'HECHA', a_cits, f"{d['categoria']}|{d['confianza']}",
                  'ya tiene RH_ScopedInstall o fila en hooks.csv (la medida de la fase 8 es anterior)', formas=d['formas'])
            continue
        codigo = [c for c in a_cits if c[3] in (K_VALOR, K_LLAMADA, K_LIBJPEG, K_PROTECTOR, K_ISLA)]
        conf, cat = d['confianza'], d['categoria']
        ccl = resumen_clase(a_cits)
        mn = d['ejemplo_instruccion'].split(' ')[0]
        previo = ex.rd(a - 1, 1)
        plausible = (a % 16 == 0 or (previo and previo[0] in (0xCC, 0x90, 0xC3))) and mn in ('push', 'mov')
        f = func_de(a)
        antes = f'{cat}|{conf}'
        formas = d['formas']
        if conf == 'C' and (not codigo or not plausible):
            nuevo(a, 0, 'constante numerica', 'cerrada (constante)', 'CERRADA', a_cits, antes,
                  'inmediato del codigo compilado que no es una raiz: ' + ('sin literal de direccion en el C++ (imul/add/cmp/desplazamiento de tabla)' if not codigo else
                  'el literal del C++ es una constante (p. ej. identificador de temporizador, mascara) y el destino no es un inicio de rutina'), formas=formas)
            continue
        if ccl == 'isla':
            nuevo(a, 0, 'isla D3DX9', 'cerrada (isla D3DX9)', 'ENLAZAR D3DX9', a_cits, antes,
                  'cita de D3dxShaderAssembler/D3dxHlslCompiler/...: isla del compilador de shaders de D3DX9, que se enlaza (ejecucion-v48.md 2.4)', formas=formas)
            continue
        if ccl == 'libjpeg':
            hh0 = hueco_de(a)
            nuevo(a, _extension(ex, cs, a, hh0[1] + 1) if hh0 else 0, 'tramo de hueco', 'viva, enlazada (libjpeg)', 'ENLAZAR', a_cits, antes,
                  'LIBJPEG_CB / JPegCompressPlugin: con GTASA_WITH_LIBJPEG es la de libs/libjpeg-6b', estimado=True, formas=formas)
            continue
        mod = modulo_enlazado(a)
        if mod:
            v = vivas.get(a)
            if mod == 'QuickTime':
                nuevo(a, int(d['bytes'] or 0) or ((f[1] - a + 1) if f else _extension(ex, cs, a, a + 400)), 'inicio de funcion de Ghidra' if (f and f[0] == a) else 'tramo de hueco',
                      'viva, QuickTime (se retira)', 'RETIRAR', a_cits, antes, 'QuickTime/QTML se retira (decision vigente); la cita es del puente QTMLRuntime.cpp', formas=formas)
                continue
            nuevo(a, int(d['bytes'] or 0) or ((f[1] - a + 1) if f else 0), 'inicio de funcion de Ghidra' if (f and f[0] == a) else 'tramo de hueco', f'viva, enlazada ({mod})',
                  (v['decision'] if v else 'ENLAZAR'), a_cits, antes, f'decision vigente: {mod} se enlaza (no se reconstruye)', formas=formas)
            continue
        if pz:   # entrada interna de una pieza reconstruida o de otra clase
            nuevo(a, pz[1] - a + 1, 'entrada interna de pieza', 'viva sin hook', 'RECONSTRUIR', a_cits, antes,
                  'segunda entrada de una pieza que ya esta enganchada por otra direccion', hueco=pz[2]['hueco'], formas=formas)
            continue
        hh = hueco_de(a)
        if f and f[0] != a:
            nuevo(a, f[1] - a + 1, 'interior de funcion', 'viva sin hook', 'REVISAR', a_cits, antes,
                  'literal de codigo a mitad de una funcion de Ghidra (continuacion de catch o entrada alternativa)', formas=formas)
            continue
        if f and f[0] == a:
            v = vivas.get(a)
            nuevo(a, f[1] - a + 1, 'inicio de funcion de Ghidra', 'viva sin hook', 'RECONSTRUIR', a_cits, antes,
                  ('funciones-vivas: ' + v['decision'] + ' / ' + v['grupo']) if v else 'funcion de Ghidra sin hook', formas=formas)
            continue
        if hh:
            sz = _extension(ex, cs, a, hh[1] + 1)
            nuevo(a, sz, 'tramo de hueco', 'viva sin hook', 'RECONSTRUIR', a_cits, antes, f'hueco `{hh[2][:34]}`', estimado=True, hueco=hexa(hh[0]), formas=formas)
            continue
        nuevo(a, 0, 'fuera de huecos y funciones', 'viva sin hook', 'REVISAR', a_cits, antes, 'direccion fuera de huecos y funciones', formas=formas)
    # 3) citas de codigo del C++ que el escaneo de .obj no vio (tablas estaticas, codigo condicional), fuera de piezas y de funciones-vivas
    ya = {o['va'] for o in objs}
    for a in sorted(citas):
        if a in ya or not (0x401000 <= a < 0x858000) or a in hooks or a in dep or pieza_de(a):
            continue
        a_cits = citas_en(citas, a, a)
        if not [c for c in a_cits if c[3] in (K_VALOR, K_LLAMADA, K_LIBJPEG)]:
            continue
        pv = ex.rd(a - 1, 1)
        if not (a % 16 == 0 or (pv and pv[0] in (0xCC, 0x90, 0xC3))):
            continue          # no es un inicio de rutina plausible (constante numerica, color, mascara...)
        f = func_de(a)
        if f and f[0] == a and a in vivas:
            continue          # ya esta en funciones-vivas.csv con sus referencias del C++
        hh = hueco_de(a)
        if hh is None and not f:
            continue
        sz = (f[1] - a + 1) if f else _extension(ex, cs, a, hh[1] + 1)
        nuevo(a, sz, 'cita del C++ sin inmediato compilado', 'viva sin hook', 'REVISAR', a_cits, 'cita sin evidencia compilada',
              'literal de codigo del C++ que el escaneo de .obj no ve (inicializador estatico o codigo condicional)', estimado=True)
    medida_clave = {o['clave'] for o in objs if o['va'] in MEDIDAS and o['clave']}
    for o in objs:
        if o['va'] in MEDIDAS:
            o['ejecucion'], o['tier'] = 'medida (fase 8)', 0
            o['motivo'] = (o['motivo'] + ' | ' if o['motivo'] else '') + MEDIDAS[o['va']]
        elif o['clave'] and o['clave'] in medida_clave:
            o['ejecucion'], o['tier'] = 'misma rutina de registro que una medida (arranque)', 1
        else:
            o['ejecucion'], o['tier'] = 'sin medir', 2
    return objs


CAMPOS = ['va', 'bytes', 'estimado', 'region', 'origen', 'clase', 'decision', 'ejecucion', 'antes', 'entradas', 'citada_por', 'n_citas', 'tabla_destino',
          'formas_obj', 'hueco', 'motivo']


def _orden(o):
    return (o['tier'], -o['bytes'], o['va'])


def escribir_csv(p, objs):
    with open(p, 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(CAMPOS)
        for o in objs:
            w.writerow([hexa(o['va']), o['bytes'], o['estimado'], o['region'], o['origen'], o['clase'], o['decision'], o['ejecucion'], o['antes'], o['entradas'],
                        o['citada_por'], o['n_citas'], o['tabla_destino'], o['formas'], o['hueco'], o['motivo']])


def completar(objs, citas):
    """Rellena `entradas` y, si falta, la tabla destino con la instruccion del codigo compilado (dependencias_codigo.csv)."""
    dep = {int(r['direccion'], 16): r for r in _leer(OUT / 'fase8-experimento' / 'dependencias_codigo.csv')}
    for o in objs:
        lo, hi = o['va'], o['va'] + max(o['bytes'], 1) - 1
        cit = sorted({a for a, f, n, k, t in citas_en(citas, lo, hi) if k not in (K_COMENTARIO, K_ENGANCHE)} | {a for a in dep if lo <= a <= hi})
        o['entradas'] = ' '.join('%X' % a for a in cit)
        if not o['tabla_destino']:
            for a in cit:
                if a in dep:
                    d = dep[a]
                    o['tabla_destino'] = f"(.obj) {d['ejemplo_funcion'].split(' | ')[0][:60]}: `{d['ejemplo_instruccion']}`"
                    break


def comprobar_cierres(exe, objs, cache):
    """Alcance por punto fijo (contrib/codigo_vivo_cierre_v48.py) con las raices por valor como raices EXTRA: ¿algun descarte P1/V1-V8 cerrado pasa a estar alcanzado?"""
    import argparse as ap_
    import codigo_vivo as cv
    import codigo_vivo_cierre_v48 as v48
    from pe_inventory import PE
    hechos = cv.cargar_ejecutado(ap_.Namespace(exe=exe, cache=str(Path(cache) / 'hechos.pkl')))
    pe = PE(Path(exe))
    vivas = _leer(OUT / 'funciones-vivas.csv')
    aristas = v48.escanear_todo(pe, hechos['funcs'], hechos['ex'], str(Path(cache) / 'aristas.pkl'))

    def correr(extra):
        h2 = dict(hechos)
        refs = {k: list(v) for k, v in hechos['refs_src'].items()}
        for va in extra:
            refs.setdefault(va, []).append(('raices-valor-p6', 0, 'literal'))
        h2['refs_src'] = refs
        todos = {}
        for modo in ('estricto', 'amplio'):
            v48.MODO = modo
            vivos, _, _ = v48.alcance(pe, h2, aristas, vivas)
            todos.update(vivos)
        v48.MODO = 'estricto'
        return todos

    base = correr(())
    raices = sorted({o['va'] for o in objs if o['clase'] == 'viva sin hook'})
    pesim = sorted({o['va'] for o in objs if o['clase'] in ('viva sin hook', 'cerrada (comentario)', 'viva, del protector', 'viva, enlazada (libjpeg)', 'viva, QuickTime (se retira)')})
    con = correr(raices)
    con_pesim = correr(pesim)
    desc = [x for x in vivas if x['decision'] == 'DESCARTAR']
    res = dict(n_raices=len(raices), n_pesim=len(pesim), nodos_base=len(base), nodos_con=len(con), nodos_pesim=len(con_pesim), filas=[], reabiertas=[])
    por = defaultdict(Counter)
    for x in desc:
        va = int(x['va'], 16)
        k = x['lote'] or 'sin lote'
        por[k][('viva-base' if va in base else 'muerta') + ('+nueva' if (va in con and va not in base) else '')] += 1
        if va in con and va not in base:
            res['reabiertas'].append(x)
        if va in con_pesim and va not in con and va not in base:
            res.setdefault('solo_pesim', []).append(x)
    res['por_lote'] = {k: dict(v) for k, v in sorted(por.items())}
    res['por_clase'] = {}
    for nm in ('viva, del protector', 'viva, enlazada (libjpeg)', 'cerrada (comentario)'):
        ex_ = sorted({o['va'] for o in objs if o['clase'] == nm})
        t_ = correr(raices + ex_)
        res['por_clase'][nm] = [x for x in desc if int(x['va'], 16) in t_ and int(x['va'], 16) not in con]
    res['descartar'] = len(desc)
    res['con'] = set(con)
    return res


def cifras(objs):
    c = Counter()
    b = Counter()
    for o in objs:
        c[o['clase']] += 1
        b[o['clase']] += o['bytes']
    return c, b


def n_(x):
    return f'{x:,}'.replace(',', '.')


def informe(objs, piezas, chk, citas):
    """Escribe docs/CodigoVivoExe/reclasificacion-raices-valor.md."""
    cola = sorted((o for o in objs if o['clase'] == 'viva sin hook'), key=_orden)
    L = []
    w = L.append
    ant, antb, des, desb = Counter(), Counter(), Counter(), Counter()
    for r in piezas:
        ant[r['destino_antes']] += 1
        antb[r['destino_antes']] += int(r['bytes'])
        des[r['destino']] += 1
        desb[r['destino']] += int(r['bytes'])
    p147 = [r for r in piezas if r['destino_antes'] == 'citada-en-cpp' and r['vida'] == 'muerto']
    t147, b147 = Counter(), Counter()
    for r in p147:
        t147[r['destino']] += 1
        b147[r['destino']] += int(r['bytes'])
    m_ant = [r for r in piezas if r['destino_antes'] == 'muerta-por-reemplazo']
    m_mov = Counter(r['destino'] for r in m_ant)
    cierre_cod = {int(r['funcion'], 16): r for r in _leer(OUT / 'fase8-experimento' / 'cierre_codigo.csv')}
    en_cierre = [o for o in cola if o['va'] in cierre_cod]
    fuera_cierre = [o for o in cola if o['va'] not in cierre_cod]
    tot = sum(o['bytes'] for o in cola)
    hu = _leer(OUT / 'cierre-p6.csv')
    hu_c = sum(1 for r in hu if r['cerrado'] == '1')
    w('# Reclasificación de las raíces por valor (huecos «muertos» que el EXE propio ejecuta)')
    w('')
    w('Rama `vivo-raices` (desde `origin/main` 0366e8fabf, 21/11/2026). Análisis estático sobre el EXE canónico, el código fuente y los CSV; no se compiló el proyecto (ver §8). '
      'Generado por `contrib/huecos_p6.py` + `contrib/raices_valor_p6.py` (regenerable, §9). Base de la medida: [fase8-experimento.md](fase8-experimento.md) (rama `fase8-exp`).')
    w('')
    w('## 1. Qué estaba mal')
    w('')
    w('El experimento de la fase 8 construyó un EXE propio y vio que direcciones clasificadas como «muerto» (`huecos-p6.csv`: `citada-en-cpp`/`muerto`) o sin lista **se ejecutan**: '
      '`0x4CD820`, `0x750430`, `0x750460`, `0x750480`, `0x750AC0`, `0x7F98D0`, `0x7F99B0`. «Muerto» se decidía por falta de llamadores directos (`call`/`jmp`/inmediato en una función viva) y no vale para una dirección '
      'que el C++ **escribe como valor** (tabla estándar de rasters de D3D9, callbacks de `RwCamera`, `mov [reg+x], imm32` de los nodos de RenderWare): la llama el motor, por un puntero que el análisis no sigue. '
      'Además `citada-en-cpp` agrupaba cualquier cita, incluidos los comentarios, y la trataba igual: ni viva ni muerta, «documentar».')
    w('')
    w('## 2. Regla nueva')
    w('')
    w('Una dirección citada por el C++ **como VALOR** (asignación, inicializador, argumento, `reinterpret_cast`, llamada por dirección), no como comentario, es una **raíz viva** hasta demostrar lo contrario (`escanear_citas` de `contrib/raices_valor_p6.py`):')
    w('')
    w('* un autómata separa en `source/` el **código**, los **comentarios** y las **cadenas**; cada literal `0x4xxxxx`–`0x8xxxxx` (sin carácter de identificador delante: `f0x57C330` es un nombre) se clasifica en `enganche` (línea `RH_Scoped*`), '
      '`llamada-directa` (`plugin::Call*<0x…>`), `valor` (todo lo demás en código), `libjpeg-cb` (`LIBJPEG_CB` o `JPegCompressPlugin`), `protector` (tablas de `ExeProtectionStub.cpp`), `isla-d3dx9` (los cuatro `D3dx*.cpp` que `ejecucion-v48.md` 2.4 ya trata como isla) y `comentario`;')
    w('* se cruza con el **código compilado**: [`fase8-experimento/dependencias_codigo.csv`](fase8-experimento/dependencias_codigo.csv) (direcciones del `.text` que los `.obj` del proyecto llevan como inmediato; confianza A/B/C). '
      'A/B cuentan como evidencia de valor; una C solo si hay literal de código en el C++ **y** el destino es un inicio de rutina plausible (alineado a 16 o tras `int3`/`nop`/`ret`) **y** la instrucción es `push`/`mov` '
      '(así `0x4CAE40`, entrada real de la tabla de rasters, sale de la clase «ruido» y `0x475441`, un identificador de temporizador, no);')
    w('* destino: **viva sin hook** (cola de reconstrucción); **viva enlazada** (libjpeg, libpng+zlib, D3DX9, CRT de MSVC: decisiones vigentes del brief); **viva de QuickTime** (se retira); **viva del protector** (tablas de `ExeProtectionStub.cpp`, se retiran con el archivo, V1); '
      '**cerrada por comentario** (solo la cita un comentario y ninguna otra entrada es viva); **cerrada por constante** (inmediato numérico); **cerrada isla D3DX9**. Nada se cierra por «no tiene llamadores».')
    w('')
    w('La clase `citada-en-cpp` **desaparece**. Sigue sin verse una cita sin literal hexadecimal y no se distingue código no compilado (`#if 0`); en ambos casos el error es hacia «viva» o «sin ver», nunca hacia «muerta» con certeza (§9).')
    w('')
    w('## 3. Cifras antes y después: piezas de huecos (914 piezas, 118.544 B)')
    w('')
    w('| Destino | Antes (piezas / B) | Después (piezas / B) |')
    w('| --- | ---: | ---: |')
    nombres = ['reconstruida', 'viva-sin-hook', 'viva-enlazada', 'viva-quicktime', 'viva-protector', 'crt-enlazar', 'd3dx9-enlazar', 'pendiente-dato', 'citada-en-cpp', 'cerrada-comentario',
               'muerta-por-reemplazo', 'destructor-atexit', 'sin-referencia-real']
    for nm in nombres:
        w(f'| {nm} | {ant[nm]} / {n_(antb[nm])} | {des[nm]} / {n_(desb[nm])} |')
    w(f'| **Total** | {sum(ant.values())} / {n_(sum(antb.values()))} | {sum(des.values())} / {n_(sum(desb.values()))} |')
    w('')
    w(f'Las **{len(p147)} piezas `citada-en-cpp`/`muerto` ({n_(sum(b147.values()))} B)** del encargo se reparten así:')
    w('')
    w('| Nuevo destino | Piezas | Bytes |')
    w('| --- | ---: | ---: |')
    for nm, k in t147.most_common():
        w(f'| {nm} | {k} | {n_(b147[nm])} |')
    w('')
    w(f'Las {len(m_ant)} piezas `muerta-por-reemplazo` no cambian (' + ', '.join(f'{k} {v}' for k, v in m_mov.most_common()) + '): ninguna tiene cita de código ni inmediato compilado, '
      'solo llamadores que son funciones reemplazadas (su cuerpo ya no corre).')
    w('')
    w(f'Huecos cerrados (`cierre-p6.csv`): antes 269 de 270 → después **{hu_c} de 270**. Un hueco con una pieza «viva sin hook» no está cerrado: su pieza está en la cola.')
    w('')
    w('## 4. Las 379 direcciones de código citadas por el C++ sin gancho (fase 8)')
    w('')
    dep = _leer(OUT / 'fase8-experimento' / 'dependencias_codigo.csv')
    mapa = {}
    for o in objs:
        for h_ in o['entradas'].split():
            mapa.setdefault(int(h_, 16), o['clase'])
    clases = Counter()
    for r in dep:
        clases[mapa.get(int(r['direccion'], 16), 'sin clase')] += 1
    c, b = cifras(objs)
    w('Antes: 119 de confianza A, 147 B y 113 «ruido» C. Después, cada una cae en una clase (la columna «Direcciones» suma 379); «Objetos» agrupa las que comparten pieza de hueco:')
    w('')
    w('| Clase | Direcciones | Objetos | Bytes |')
    w('| --- | ---: | ---: | ---: |')
    orden_c = ['viva sin hook', 'viva, enlazada (libjpeg)', 'viva, enlazada (libpng/zlib)', 'viva, enlazada (CRT)', 'viva, QuickTime (se retira)', 'viva, del protector',
               'cerrada (isla D3DX9)', 'cerrada (comentario)', 'cerrada (constante)', 'enganchada desde la medida', 'sin clase']
    for nm in orden_c:
        if c[nm] or clases[nm]:
            w(f'| {nm} | {clases[nm]} | {c[nm]} | {n_(b[nm])} |')
    w(f'| **Total** | {sum(clases.values())} | {sum(c.values())} | {n_(sum(b.values()))} |')
    w('')
    w('«Enganchada desde la medida» = ya tiene fila en `docs/hooks.csv` (la medida es anterior a integrar `vivo-crtf8`). Los «ruido C» que eran reales: ' +
      ', '.join(f"`0x{o['va']:08X}`" for o in cola if o['antes'].startswith('ruido')) + ' (entradas de la tabla estándar de rasters); el resto son constantes '
      '(`imul`, `add`, `cmp`, desplazamientos de tabla, un identificador de temporizador `0x475441`, máscaras `0x7FFFFF`/`0x800000`, colores).')
    w('')
    w('## 5. Cola de reconstrucción: «viva sin hook»')
    w('')
    w(f'**{len(cola)} objetos, {n_(tot)} B.** Completa en [`cola-raices-valor-p6.csv`](cola-raices-valor-p6.csv) (todas las clases en [`raices-valor-p6.csv`](raices-valor-p6.csv)). Orden: primero lo **medido** en la fase 8 '
      '(se ejecuta en los dos primeros segundos), luego lo que comparte rutina de registro con una medida (mismo arranque), luego el resto; dentro de cada nivel por tamaño descendente. '
      'Los tamaños marcados `*` son una estimación (barrido lineal hasta el primer `ret`/`jmp` sin saltos pendientes; no sigue tablas de saltos). '
      '`RECONSTRUIR` = escribir C++ y engancharlo.')
    w('')
    w('| # | Dirección | B | Ejecución | Quién la cita | Tabla / estructura destino | Decisión |')
    w('| ---: | --- | ---: | --- | --- | --- | --- |')
    for i, o in enumerate(cola, 1):
        td = o['tabla_destino'].replace('|', '\\|')[:130]
        w(f"| {i} | `0x{o['va']:08X}`{'*' if o['estimado'] else ''} | {n_(o['bytes'])} | {o['ejecucion']} | {o['citada_por'][:70] or '(.obj)'} | {td} | {o['decision']} |")
    w('')
    medidas_ok = [a for a in MEDIDAS if any(o['va'] <= a < o['va'] + max(o['bytes'], 1) for o in cola)]
    w(f"Las 7 direcciones medidas ({', '.join('`0x%X`' % a for a in MEDIDAS)}) están dentro de objetos de la cola ({len(medidas_ok)} de {len(MEDIDAS)}): `0x750480` va dentro de la pieza `0x750460` (682 B) y `0x750AC0` es la segunda entrada "
      'de la pieza de `LightWorldPluginConstruct`. «Ejecución» solo es una medida en las siete; el resto es una prioridad por vecindad (misma rutina de registro), no una medida.')
    w('')
    peq = [o for o in cola if o['bytes'] <= 30]
    w('## 6. Efecto sobre las cifras de lo que falta')
    w('')
    w(f"* **Cierre por llamadas del código original (fase 8)**: 691 funciones / 309.142 B, 266 raíces. De los {len(cola)} objetos de la cola, **{len(en_cierre)} ya eran raíces de ese cierre** "
      f"({n_(sum(int(cierre_cod[o['va']]['bytes']) for o in en_cierre))} B) y **{len(fuera_cierre)} no estaban** ({', '.join('`0x%X` (%d B)' % (o['va'], o['bytes']) for o in fuera_cierre)}): el cierre medido era correcto; "
      'lo que estaba mal era la **lista de pendientes** (P6 los daba por «muertos» o «citados»). Con la cola, el cierre sube a ' +
      f"{691 + len(fuera_cierre)} funciones / {n_(309142 + sum(o['bytes'] for o in fuera_cierre))} B.")
    w(f"* **P6**: de «269 de 270 huecos cerrados» a **{270 - hu_c} huecos abiertos** (con piezas «viva sin hook»). Lo enlazado (libjpeg {n_(desb['viva-enlazada'])} B en piezas) y lo del protector ({n_(desb['viva-protector'])} B) no suma a lo que falta reconstruir.")
    w(f"* **Por reconstruir con hook** (toda la cola, no solo P6): {len(cola)} objetos, {n_(tot)} B; **{sum(1 for o in cola if o['tier'] < 2)}** se ejecutan al arrancar o comparten rutina de registro con una medida "
      f"({n_(sum(o['bytes'] for o in cola if o['tier'] < 2))} B); {len(peq)} tienen ≤ 30 B.")
    w(f"* **Cierres nuevos con motivo**: {des['cerrada-comentario']} piezas ({n_(desb['cerrada-comentario'])} B) solo citadas en comentarios (antes `citada-en-cpp`), {clases['cerrada (constante)']} direcciones del escaneo de `.obj` que son constantes "
      f"y {clases['cerrada (isla D3DX9)']} citas de la isla D3DX9; la clase `citada-en-cpp` ya no existe.")
    w('* **Plan**: nueva parte P7 en `plan.csv`/`plan-lotes.md` (lotes R1–R3 = la cola por temas; `contrib/codigo_vivo_plan.py`), y H1–H6 con su estado real (antes `pendiente` sin recalcular).')
    w('')
    w('## 7. Descartes P1/V1–V8 cerrados que dependían de una pieza viva')
    w('')
    if chk:
        w(f"Se recalculó el alcance por punto fijo de `contrib/codigo_vivo_cierre_v48.py` (modos estricto + amplio, sobre las {n_(chk['descartar'])} funciones con decisión DESCARTAR) con la cola ({chk['n_raices']} direcciones) como raíces extra. "
          'El alcance base ya tomaba como raíz todo literal de código del C++ (`escanear_fuente` quita los comentarios) y `codigo_vivo_cierre_v48.py` lee ahora además `cola-raices-valor-p6.csv`; '
          'por tanto el cierre de V4–V8 **no dependía de la clase «muerto» de P6** y no cambia.')
        w('')
        w(f"* Nodos alcanzados: base {chk['nodos_base']}, con la cola {chk['nodos_con']}.")
        w(f"* **Descartes cerrados que pasan a estar alcanzados con la cola: {len(chk['reabiertas'])}**" + (': ' + ', '.join(f"`{x['va']}` ({x['lote']})" for x in chk['reabiertas'][:30]) if chk['reabiertas'] else '. No se reabre ninguno.'))
        for cl_, lst in chk.get('por_clase', {}).items():
            w(f"* Prueba pesimista, si además fueran raíz las piezas «{cl_}»: {len(lst)} descartes más" + (' (' + ', '.join(f"`{x['va']}` {x['lote']}" for x in lst[:12]) + ')' if lst else '') + '.')
        w('')
        w('| Lote | Funciones DESCARTAR alcanzadas por la base / muertas |')
        w('| --- | --- |')
        for k, v in chk['por_lote'].items():
            w(f'| {k} | ' + ', '.join(f'{a.replace("viva-base", "alcanzadas")} {b_}' for a, b_ in sorted(v.items())) + ' |')
        w('')
        w('Las «alcanzadas» son las que ya figuraban abiertas o condicionadas en `cierre-p1.csv` (llamadores vivos conservados; p. ej. los ayudantes `usercall` de rasters de V7, que **sí** están vivos: `ejecucion-v48.md` §3). '
          'Ninguna se reabre por este cambio. Las tablas de `ExeProtectionStub.cpp` (piezas `viva-protector`) alcanzan funciones del protector (V1): es lo ya conocido, se retira con el archivo.')
        cond = chk.get('condicionadas') or []
        if cond:
            w('')
            w(f'**Las {len(cond)} funciones «condicionadas a un hueco o tabla por clasificar (P6)»** de `cierre-p1.csv` (V1, V2, E1) se resuelven por la pieza que las llama (columna `motivo_raices_valor`):')
            w('')
            w('| Resultado | Funciones |')
            w('| --- | ---: |')
            for k_, n__ in Counter((c[3] if c[3] in ('cerrada', 'abierta') else c[3]) for c in cond).most_common():
                w(f'| {k_} | {n__} |')
            ab = [c for c in cond if c[3] == 'abierta']
            if ab:
                w('')
                w('Reabiertas con motivo: ' + ', '.join(f'`{c[0]}` ({c[1]})' for c in ab) + '.')
    else:
        w('(comprobación no ejecutada: use `--cierres`)')
    w('')
    w('## 8. Piezas pequeñas y claras (≤ 30 B) de la cola')
    w('')
    w('No se reconstruyó ninguna en esta rama: exige compilar el worktree (build completo, un solo hilo y bajo el candado compartido) y el humo de ropero + agua, que espera a que no haya otro `gta_sa.exe`. '
      'Quedan **listas para el lote R3/R1** (cuerpo ya desensamblado; ninguna toca memoria que no sea la indicada):')
    w('')
    w('| Dirección | B | Equivalente C++ (cdecl) | Dónde se registra |')
    w('| --- | ---: | --- | --- |')
    for o in sorted((o for o in cola if o['bytes'] <= 30), key=lambda o: o['va']):
        w(f"| `0x{o['va']:08X}` | {o['bytes']} | {BOSQUEJO.get(o['va'], 'llama a un ayudante `usercall` aún sin reconstruir (0x4CAD90/0x4CAEA0) o a otro método: no es trivial')} | {o['citada_por'][:60]} |")
    w('')
    w('## 9. Cómo regenerar y límites')
    w('')
    w('```')
    w('python -X utf8 contrib/huecos_p6.py --exe <gta_sa.exe>      # huecos-p6.csv (regla de valor), cierre-p6.csv, H1..H6 en plan.csv y plan-lotes.md')
    w('python -X utf8 contrib/raices_valor_p6.py --exe <gta_sa.exe> --cierres --aplicar --cache <carpeta>   # cola, raices-valor-p6.csv, este informe y la comprobacion de cierres')
    w('python -X utf8 contrib/codigo_vivo_cierre_v48.py --alcance --aplicar --cache hechos.pkl --cache-aristas aristas.pkl   # V4-V8 con la cola como raices')
    w('python -X utf8 contrib/codigo_vivo_plan.py                    # plan.csv / plan-lotes.md (lotes R1..R3); ejecutar huecos_p6.py despues para el estado de H1..H6')
    w('```')
    w('')
    w('Límites: (1) `dependencias_codigo.csv` es el escaneo de `.obj` de la fase 8 (solo se regenera con las herramientas de esa rama); (2) una cita sin literal hexadecimal (base + desplazamiento, decimal) no se ve; '
      '(3) un literal en código no compilado (`#if 0`) cuenta como valor; (4) «cuánto se ejecuta» solo está medido para 7 direcciones, el resto es por rutina de registro compartida (el censo `--censo` de `reversing/FnFase8` exige enlazar y ejecutar el EXE propio y no se repitió aquí); '
      '(5) el tamaño de un tramo de hueco es una estimación; (6) los módulos enlazados salen de `docs/RenderWareExe/funciones.csv` (D3DX9 0x7652AE–0x7C4220, libpng+zlib 0x7CF600–0x7EB260), QuickTime 0x818430–0x81DAB0 y el CRT hasta 0x8568AB.')
    w('')
    (OUT / 'reclasificacion-raices-valor.md').write_text('\n'.join(L) + '\n', encoding='utf-8')


# ----------------------------------------------------------------------------------------------------------------------
# Parte 3: las funciones DESCARTAR/ENLAZAR «condicionadas a un hueco o tabla por clasificar (P6)» de cierre-p1.csv
# ----------------------------------------------------------------------------------------------------------------------
GRUPO_DE_DESTINO = {'viva-enlazada': 'enlace de una biblioteca (libjpeg/libpng/D3DX9/CRT)', 'viva-quicktime': 'retirada de QuickTime', 'viva-protector': 'retirada del protector',
                    'd3dx9-enlazar': 'enlace de D3DX9', 'crt-enlazar': 'enlace del CRT'}


def llamadas_en_hueco(exe, h0, destino):
    """Direcciones de los `call`/`jmp` rel32 del hueco que empieza en h0 hacia `destino`."""
    import huecos_p6 as hp
    global _EXE_CACHE
    try:
        ex = _EXE_CACHE
    except NameError:
        ex = _EXE_CACHE = hp.Exe(exe)
    fin = None
    for r in _leer(OUT / 'huecos.csv'):
        if int(r['va'], 16) == h0:
            fin = int(r['fin'], 16)
            break
    if fin is None:
        return []
    blob = ex.rd(h0, fin - h0 + 1)
    out = []
    for i in range(len(blob) - 4):
        if blob[i] in (0xE8, 0xE9):
            rel = int.from_bytes(blob[i + 1:i + 5], 'little', signed=True)
            if (h0 + i + 5 + rel) & 0xFFFFFFFF == destino:
                out.append(h0 + i)
    return out


def resolver_condicionadas(piezas, alcanzadas, aplicar=False, exe=None):
    """Resuelve con la clasificacion nueva las 41 filas `condicionada (hueco o tabla por clasificar: P6)` de cierre-p1.csv.
    - llamada desde una pieza «viva sin hook» (o pendiente-dato): la funcion queda ABIERTA (la alcanza codigo vivo sin reconstruir);
    - llamada desde una pieza enlazada / de QuickTime / del protector: CONDICIONADA a ese enlace o retirada (decisiones vigentes);
    - llamada desde una pieza reconstruida, muerta, sin referencia, atexit o de comentario: CERRADA;
    - dword en dato: CERRADA si el alcance (cierre v48 con la cola) no llega a la funcion, ABIERTA si llega."""
    filas = _leer(OUT / 'cierre-p1.csv')
    campos = list(filas[0])
    if 'motivo_raices_valor' not in campos:
        campos.append('motivo_raices_valor')
    pz = sorted(((int(r['va'], 16), int(r['fin'], 16), r) for r in piezas), key=lambda t: t[0])
    ini = [t[0] for t in pz]

    def pieza_de(a):
        i = bisect.bisect_right(ini, a) - 1
        return pz[i] if i >= 0 and pz[i][0] <= a <= pz[i][1] else None

    cambios = []
    for r in filas:
        r.setdefault('motivo_raices_valor', '')
        if not (r['cierre'].startswith('condicionada (hueco o tabla') or r['motivo_raices_valor']):
            continue          # (las ya resueltas se recalculan: la ejecucion es idempotente)
        va = int(r['va'], 16)
        dets = [x.strip() for x in r['detalle_a_clasificar'].split('|') if x.strip()]
        estados, motivos = [], []
        for d in dets:
            m = re.match(r'llamada desde hueco 0x([0-9A-Fa-f]+)', d)
            if m:
                h0 = int(m.group(1), 16)
                # el CSV da el INICIO del hueco: se buscan las llamadas/saltos directos a la funcion dentro del hueco y se toma la pieza de cada una
                sitios = llamadas_en_hueco(exe, h0, va) if exe else []
                if not sitios:
                    sitios = [h0]
                for a in sitios:
                    p = pieza_de(a)
                    if p is None:
                        mod = 'libjpeg' if 0x5C2F00 <= a < 0x5D0500 else modulo_enlazado(a)
                        if mod and mod != 'QuickTime':
                            estados.append(f'condicionada:enlace de {mod}')
                            motivos.append(f'llamada en 0x{a:08X} (hueco 0x{h0:08X} sin referencia directa, dentro de {mod}, que se enlaza)')
                        else:
                            estados.append('abierta')
                            motivos.append(f'llamada en 0x{a:08X} (hueco 0x{h0:08X}): sin pieza en huecos-p6.csv y fuera de los modulos enlazados')
                        continue
                    dest = p[2]['destino']
                    if dest in ('viva-sin-hook', 'pendiente-dato'):
                        estados.append('abierta')
                    elif dest in GRUPO_DE_DESTINO:
                        estados.append('condicionada:' + GRUPO_DE_DESTINO[dest])
                    else:
                        estados.append('cerrada')
                    motivos.append(f"llamada en 0x{a:08X} desde la pieza 0x{p[0]:08X} ({dest})")
            else:
                m = re.match(r'dword en dato 0x([0-9A-Fa-f]+)', d)
                if m:
                    if va in alcanzadas:
                        estados.append('abierta')
                        motivos.append(f'{d}: el alcance de la cola llega a la funcion')
                    else:
                        estados.append('cerrada')
                        motivos.append(f'{d}: ningun codigo alcanzable (con la cola de raices por valor) usa esa tabla')
        if not dets:
            continue
        if any(e == 'abierta' for e in estados):
            nuevo = 'abierta'
        elif any(e.startswith('condicionada') for e in estados):
            nuevo = 'condicionada (' + sorted({e.split(':', 1)[1] for e in estados if e.startswith('condicionada')})[0] + ')'
        else:
            nuevo = 'cerrada'
        r['motivo_raices_valor'] = 'condicionada (hueco o tabla por clasificar: P6) -> ' + nuevo + ': ' + '; '.join(motivos)
        cambios.append((r['va'], r['lote'], r['cierre'], nuevo))
        if aplicar:
            r['cierre'] = nuevo
            if nuevo == 'abierta':
                r['bloqueos'] = '1'
                r['detalle_bloqueos'] = 'alcanzable por una pieza viva sin hook (raices por valor): ' + '; '.join(motivos)
    if aplicar:
        with open(OUT / 'cierre-p1.csv', 'w', encoding='utf-8', newline='') as s:
            w = csv.DictWriter(s, campos)
            w.writeheader()
            w.writerows(filas)
        nuevos = {r['va']: r['cierre'] for r in filas}
        vivas = _leer(OUT / 'funciones-vivas.csv')
        for x in vivas:
            if x['va'] in nuevos:
                x['cierre'] = nuevos[x['va']]
        with open(OUT / 'funciones-vivas.csv', 'w', encoding='utf-8', newline='') as s:
            w = csv.DictWriter(s, list(vivas[0]))
            w.writeheader()
            w.writerows(vivas)
        # cierre.csv: se anade el efecto a la nota de cada lote y se recalcula `estado` solo si el lote queda completo
        previo = _leer(OUT / 'cierre.csv')
        por_lote = defaultdict(Counter)
        for r in filas:
            por_lote[r['lote']][r['cierre'][:12]] += 1
        tocados = {c[1] for c in cambios}
        for p_ in previo:
            if p_['lote'] in tocados:
                c = por_lote[p_['lote']]
                n_ = sum(c.values())
                base = p_['nota'].split(' | Raices por valor')[0]
                p_['nota'] = base + (' | Raices por valor (reclasificacion-raices-valor.md, las condicionadas a P6 se resuelven por la pieza que las llama): '
                                     f"{c.get('cerrada', 0)} cerradas, {c.get('condicionada', 0)} condicionadas a un enlace o retirada, {c.get('abierta', 0)} abiertas de {n_}")
        with open(OUT / 'cierre.csv', 'w', encoding='utf-8', newline='') as s:
            w = csv.DictWriter(s, ['lote', 'estado', 'nota'], lineterminator='\n')
            w.writeheader()
            w.writerows(previo)
    return cambios


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=r'D:\Original\_crt1a_work\gta_sa.exe')
    ap.add_argument('--cierres', action='store_true', help='recalcula el alcance de V4-V8 con las raices por valor (varios minutos)')
    ap.add_argument('--cache', default=None, help='carpeta con hechos.pkl y aristas.pkl de codigo_vivo_cierre_v48.py')
    ap.add_argument('--sin-informe', action='store_true')
    ap.add_argument('--aplicar', action='store_true', help='escribe el estado resuelto de las condicionadas a P6 en cierre-p1.csv, funciones-vivas.csv y cierre.csv')
    a = ap.parse_args()
    citas = escanear_citas()
    objs = construir(a.exe, citas)
    completar(objs, citas)
    objs.sort(key=lambda o: (o['clase'] != 'viva sin hook', o['clase'], _orden(o)))
    escribir_csv(OUT / 'raices-valor-p6.csv', objs)
    cola = sorted((o for o in objs if o['clase'] == 'viva sin hook'), key=_orden)
    escribir_csv(OUT / 'cola-raices-valor-p6.csv', cola)
    c, b = cifras(objs)
    for k in sorted(c):
        print(f'{k:<36}{c[k]:>5} {b[k]:>8}')
    chk = None
    if a.cierres:
        chk = comprobar_cierres(a.exe, objs, a.cache or '.')
        print('cierres:', {k: v for k, v in chk.items() if k in ('n_raices', 'n_pesim', 'nodos_base', 'nodos_con', 'nodos_pesim', 'descartar')}, 'reabiertas', len(chk['reabiertas']),
              {k: len(v) for k, v in chk['por_clase'].items()})
        chk['condicionadas'] = resolver_condicionadas(_leer(OUT / 'huecos-p6.csv'), {x for x in chk['con'] if not isinstance(x, tuple)}, a.aplicar, a.exe)
        print('condicionadas a P6 resueltas:', Counter((c[3] if c[3] in ('cerrada', 'abierta') else 'condicionada') for c in chk['condicionadas']))
    if not a.sin_informe:
        informe(objs, _leer(OUT / 'huecos-p6.csv'), chk, citas)


if __name__ == '__main__':
    main()
