"""Ronda 5 de DatosExe: objetos de .rdata, .data y .bss que el codigo propio cita por direccion fija, con CUALQUIER forma de uso
(StaticRef, reinterpret_cast, cast de C, ayudante f(0x...), literal de direccion), como definiciones propias con el valor inicial leido
del EXE canonico (pefile/capstone-free: bytes del archivo) y todas las referencias reconducidas a la definicion.

Diferencia con DE04/DE05: aquellos solo convertian objetos cuyas referencias eran todas `StaticRef<T>(0x...)` con el mismo tipo; los
objetos con formas mezcladas (StaticRef + literal, ayudantes, reinterpret_cast) quedaban con dos copias o sin convertir. Aqui cada
aparicion del literal se reescribe segun su contexto:

  StaticRef<T>(0xA)               -> DatosExe::R5::vA            (o reinterpret_cast<T&>(vA) si T no es el tipo de la definicion)
  reinterpret_cast<T*>(0xA)       -> reinterpret_cast<T*>(&DatosExe::R5::vA)
  (T*)0xA / *(T*)0xA              -> (T*)(&DatosExe::R5::vA)
  otro uso de valor (ayudante...) -> reinterpret_cast<uintptr_t>(&DatosExe::R5::vA)

Un objeto SOLO se convierte si: el EXE no lo toca desde codigo que pueda ejecutarse (misma medicion que DE04/DE05: objetos.csv de
datos_exe.py con DATOSEXE_HUECOS=alcanzables: funcs_exe_vivas=0, sin direccion tomada, sin tabla indexada), su tamano es fiable
(tipo declarado o ancho de acceso), todas sus referencias estan en .cpp propios (no cabeceras, no RenderWare/CRT/libs), ninguna esta
en un contexto que exija constante (constexpr, case, #, static_assert), ninguna es aritmetica de direccion (L + i, L[i]), ningun
vecino referenciado cae dentro de su extension, no es una direccion de las pruebas de humo ni la escribe DatosExe/StaticInit.cpp.

Uso: python -X utf8 contrib/datos_exe_r5.py <A|B|C> --med <carpeta de datos_exe.py> [--aplicar] [--solo lista.txt] [--no <lista.txt>]
  A = .rdata (constantes), B = .data (inicializado), C = .bss (estado).
"""
import bisect
import csv
import os
import re
import struct
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
import datos_exe_de05 as d5  # noqa: E402
from datos_exe_lote import RAIZ  # noqa: E402
from pe_inventory import PE  # noqa: E402

NL = chr(10)
SEC = {'A': ('.rdata', 0x858000, 0x8A4000), 'B': ('.data', 0x8A4000, 0x8E4000), 'C': ('.bss', 0x8E4000, 0xC9E000)}
EXCL = ('/RenderWare/', '/Crt', '/crt', 'ExeProtectionStub', 'DatosExe/', 'StandaloneExe', 'SDLWrapper', 'app/platform', 'ImportThunks',
        'zlib', 'JPeg', 'Jpeg', 'jpeg', 'D3D', 'QuickTime', 'rwcore', 'rpworld', 'rwplcore', 'rpmatfx', 'rpskin')
PRIMS = {k: v for k, v in d5.SC.items() if k != 'DWORD'}
# nombre en las cabeceras generadas: tipos estandar (la cabecera se incluye desde otras cabeceras, antes de que existan los alias de RenderWare)
CT = {'bool': 'bool', 'char': 'char', 'GxtChar': 'uint8_t', 'uint8': 'uint8_t', 'int8': 'int8_t', 'RwUInt8': 'uint8_t', 'int16': 'int16_t',
      'uint16': 'uint16_t', 'RwInt16': 'int16_t', 'RwUInt16': 'uint16_t', 'int32': 'int32_t', 'uint32': 'uint32_t', 'RwInt32': 'int32_t',
      'RwUInt32': 'uint32_t', 'RwBool': 'int32_t', 'size_t': 'size_t', 'int': 'int', 'float': 'float', 'RwReal': 'float', 'double': 'double',
      'uint64': 'uint64_t', 'int64': 'int64_t'}
FLOATS = {'float', 'RwReal'}
DOUBLES = {'double'}
SIGNED_FMT = {1: 'B', 2: 'H', 4: 'I', 8: 'Q'}
HARNESS = list(d5.HARNESS)
HARNESS_RANGOS = list(d5.HARNESS_RANGOS)
MARGEN = d5.MARGEN
VENTANA_TABLA = 0x1000   # ventana tras la base de una tabla indexada (la extension real no se conoce)
VENTANA = 0x40   # bytes tras un objeto de extension desconocida (tabla indexada, tipo no primitivo, tamano estimado) donde no se separan objetos


def norm(t):
    return re.sub(r'\s+', '', t)


def tipo_def(t):
    """(elemento, n_o_None, bytes) si t es primitivo, prim[N] o std::array<prim,N]; si no None."""
    t = norm(t)
    if t in PRIMS:
        return t, None, PRIMS[t]
    m = re.fullmatch(r'std::array<(\w+),(\d+)>', t) or re.fullmatch(r'(\w+)\[(\d+)\]', t)
    if m and m.group(1) in PRIMS:
        return m.group(1), int(m.group(2)), PRIMS[m.group(1)] * int(m.group(2))
    return None


def direcciones_staticinit():
    """Direcciones citadas con codigo (no en comentarios) por los .cpp de DatosExe que no son definiciones DataXxx: StaticInit.cpp..."""
    out = set()
    for p in (RAIZ / 'source/game_sa/DatosExe').glob('*'):
        if p.name.startswith(('DataBss', 'DataInicializada', 'DataR5', 'Rdata')):
            continue
        txt = de.quitar_comentarios(p.read_text(encoding='utf-8', errors='replace'))
        for m in de.HEX.finditer(txt):
            out.add(int(m.group(1), 16))
    return out


def formatear(el, valor_bytes):
    n = PRIMS[el]
    v = struct.unpack('<' + SIGNED_FMT[n], valor_bytes)[0]
    if el in FLOATS:
        return f'std::bit_cast<float>(0x{v:08X}u)'
    if el in DOUBLES:
        return f'std::bit_cast<double>(0x{v:016X}ull)'
    if el == 'bool':
        return 'true' if v else 'false'
    if el in ('char', 'int8', 'GxtChar'):
        return f'static_cast<{el}>({v})' if v else '0'
    suf = 'ull' if n == 8 else 'u'
    if el in ('uint8', 'RwUInt8', 'uint16', 'RwUInt16'):
        return f'{v}'
    return f'static_cast<{el}>(0x{v:0{n * 2}X}{suf})' if v else '0'


def valor_inicial(pe, va, el, n, seccion):
    tam_el = PRIMS[el]
    cnt = n if n is not None else 1
    if seccion == '.bss':
        b = bytes(tam_el * cnt)
    else:
        off = pe.offset(va, tam_el * cnt)
        if off is None:
            return None
        b = pe.data[off:off + tam_el * cnt]
    elems = [formatear(el, b[i * tam_el:(i + 1) * tam_el]) for i in range(cnt)]
    if n is None:
        return elems[0], all(c == 0 for c in b)
    return elems, all(c == 0 for c in b)


# contexto de cada aparicion --------------------------------------------------------------------------------------------------------
RE_PRE_STATIC = re.compile(r'StaticRef\s*<\s*([^<>()]*(?:<[^<>()]*>)?[^<>()]*)>\s*\(\s*$')
RE_PRE_RCAST = re.compile(r'reinterpret_cast\s*<\s*((?:const\s+|volatile\s+)*)([^<>()]*(?:<[^<>()]*>)?[^<>()]*?)\s*\*\s*>\s*\(\s*$')
RE_PRE_CCAST = re.compile(r'\(\s*(?:const\s+|volatile\s+)*([A-Za-z_][\w:<>, ]*?)\s*(\*+)\s*\)\s*\(?\s*$')
RE_POST_CLOSE = re.compile(r'^\s*\)')
RE_IDX_POST = re.compile(r'^[\s\)]*[\+\-\*\[/%]')
RE_IDX_PRE = re.compile(r'[\+\-\*/%]\s*[\w:]*\(*\s*$')


# ayudantes que leen/escriben un ESCALAR en la direccion (el objeto no es una tabla ni una cadena)
ESCALARES = re.compile(r'^(put|Raw(<[\w:]+>)?|f|ExeByte|ExeFloat|ExeWord|StoreWord|LoadWord|LoadFloat|LoadDword|CAM_LF|ScopedStaticRef<[\w:]+>|InstantHitConstant|readFloat)$')


def extension_segura(clase, tipo, pre_cast, despues, deref, pre):
    """True si el uso accede a un objeto de extension conocida (escalar o clase de tamano conocido), False si podria ser una tabla o cadena."""
    if clase == 'static':
        return True
    if clase in ('rcast', 'ccast'):
        base = re.sub(r'(const|volatile)', '', tipo).strip()
        if deref or re.match(r'^\s*->', despues):
            return True
        return base not in PRIMS and base not in ('void', 'char', 'unsigned char')
    m = re.search(r'([A-Za-z_][\w:]*(?:<[^<>()]*>)?)\s*\(\s*$', pre)
    return bool(m and ESCALARES.match(m.group(1)))


def contexto(txt, ini, fin):
    """Clasifica la aparicion del literal txt[ini:fin]. Devuelve (clase, ini_reemplazo, fin_reemplazo, datos) o (None, motivo)."""
    ls = txt.rfind(NL, 0, ini) + 1
    le = txt.find(NL, fin)
    le = len(txt) if le < 0 else le
    linea = txt[ls:le]
    stripped = linea.lstrip()
    if stripped.startswith('#'):
        return None, 'preprocesador'
    pre_l = txt[ls:ini]
    if re.search(r'\bconstexpr\b|\bstatic_assert\b|\bconsteval\b|\bconstinit\b', linea) or re.search(r'\bcase\b', pre_l):
        return None, 'constante'
    pre = txt[max(0, ini - 200):ini]
    post = txt[fin:fin + 80]
    m = RE_PRE_STATIC.search(pre)
    if m and RE_POST_CLOSE.match(post):
        cierre = fin + RE_POST_CLOSE.match(post).end()
        if re.match(r'^\s*\[', txt[cierre:cierre + 20]):
            return None, 'indexado'
        return 'static', ini - (len(pre) - m.start()), cierre, m.group(1).strip(), True
    m = RE_PRE_RCAST.search(pre)
    if m and RE_POST_CLOSE.match(post):
        cierre = fin + RE_POST_CLOSE.match(post).end()
        ini_c = ini - (len(pre) - m.start())
        deref = txt[max(0, ini_c - 12):ini_c].rstrip().endswith('*')
        despues = txt[cierre:cierre + 20]
        if re.match(r'^\s*\[', despues) or (not deref and re.match(r'^\s*[\+\-]', despues)):
            return None, 'indexado'
        tt = (m.group(1) + m.group(2)).strip()
        return 'rcast', ini_c, cierre, tt, extension_segura('rcast', tt, pre, despues, deref, pre)
    m = RE_PRE_CCAST.search(pre)
    if m:
        ini_c = ini - (len(pre) - m.start())
        deref = txt[max(0, ini_c - 12):ini_c].rstrip().endswith('*')
        if re.match(r'^[\s\)]*\[', post) or (not deref and RE_IDX_POST.match(post)):
            return None, 'indexado'
        tt = m.group(1).strip()
        return 'ccast', ini, fin, tt, extension_segura('ccast', tt, pre, post, deref, pre)
    if RE_IDX_POST.match(post) or RE_IDX_PRE.search(pre):
        return None, 'indexado'
    return 'valor', ini, fin, '', extension_segura('valor', '', pre, post, False, pre)


ESCRIBE_POST = re.compile(r'^\s*(=(?!=)|[-+*/|&^%]=|<<=|>>=|\+\+|--)')
LECTORES = re.compile(r'^(f|ExeFloat|ExeByte|ExeWord|LoadWord|LoadFloat|LoadDword|CAM_LF|InstantHitConstant|readFloat|constant)$')


def escribe(txt, ini, fin, c):
    """True si la aparicion puede ESCRIBIR el objeto (asignacion, ++, &direccion, ayudante que no solo lee)."""
    clase, a, b = c[0], c[1], c[2]
    pre = txt[max(0, a - 40):a]
    if re.search(r'(\+\+|--)\s*\(?\s*\*?\s*$', pre):
        return True
    if clase in ('static', 'rcast', 'ccast'):
        despues = txt[b:b + 12]
        if ESCRIBE_POST.match(despues) or re.match(r'^\s*\)\s*(=(?!=)|[-+*/|&^%]=)', despues):
            return True
        if clase == 'static' and re.search(r'&\s*$', pre):
            return True
        if clase != 'static' and not re.search(r'\*\s*\(?\s*$', pre):
            return True       # puntero que se guarda: podria escribirse mas tarde
        return False
    m = re.search(r'([A-Za-z_][\w:]*(?:<[^<>()]*>)?)\s*\(\s*$', pre)
    return not (m and LECTORES.match(m.group(1)))


def main():
    args = sys.argv[1:]
    lote = args[0]
    sec, LO, HI = SEC[lote[0]]
    aplicar = '--aplicar' in args
    med = Path(args[args.index('--med') + 1])
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    objs = {}
    with (med / 'objetos.csv').open(encoding='utf-8', newline='') as s:
        for r in csv.DictReader(s):
            objs[int(r['va'], 16)] = r
    refs = de.escanear_fuente('source')
    cubre = de.cubierto_por_vecino(refs)
    por = defaultdict(list)
    for r in refs:
        por[r['va']].append(r)
    todas = sorted(set(por) | {va for va, o in objs.items() if int(o['funcs_exe_vivas']) or o['escribe_exe'] == '1'})
    si_init = direcciones_staticinit()
    motivos = defaultdict(list)
    cand = {}
    # 1. filtros por objeto -------------------------------------------------------------------------------------------------------------
    for va in sorted(por):
        if not (LO <= va < HI):
            continue
        o = objs.get(va)
        rs = por[va]

        def no(m):
            motivos[m].append(va)
        if not o:
            no('sin_objeto'); continue
        if int(o['refs_dll']) == 0:
            continue
        if va in si_init:
            no('staticinit'); continue
        const = (sec == '.rdata')   # constantes: dos copias iguales de un valor inmutable son seguras aunque el EXE vivo o una tabla las use
        if not const and (int(o['funcs_exe_vivas']) or o['compartido'] == '1'):
            no('vivo'); continue
        if not const and o['direccion_tomada'] == '1':
            no('direccion_tomada'); continue
        if o['tabla_indexada'] == '1':
            no('indexada'); continue
        if o['tam_origen'] not in ('tipo', 'acceso'):
            no('tam_dudoso'); continue
        if any('hook_vtable' == r['forma'] or r['forma'] == 'parche' for r in rs):
            no('hook'); continue
        if any(any(x in r['archivo'] for x in EXCL) for r in rs):
            no('archivo_excluido'); continue
        if any(not r['archivo'].endswith(('.cpp', '.h')) for r in rs):
            no('archivo_no_admitido'); continue
        if cubre(va) is not None:
            no('campo_de_estructura'); continue
        if va in d5.CULPABLES:
            no('culpable'); continue
        # tipo de la definicion: primitivo/array de primitivos tipado, o almacen de bytes (clases y estructuras de tamano conocido)
        tipados = [(norm(r['tipo']), r['forma'], r['tam']) for r in rs if r['tipo']]
        tam = int(o['tam'])
        tdef = None
        raw = False
        if any(sz is None for t_, f_, sz in tipados):
            no('tipo_sin_tam'); continue
        if any(sz > tam for t_, f_, sz in tipados):
            no('tipos_mayores'); continue
        cnt_t = defaultdict(int)
        for t_, f_, sz in tipados:
            if tipo_def(t_) and tipo_def(t_)[2] == tam:
                cnt_t[t_] += (2 if f_ == 'StaticRef' else 1)
        if cnt_t and all(tipo_def(t_) for t_, f_, sz in tipados):
            tdef = max(cnt_t, key=cnt_t.get)
        elif not tipados:
            if tam in (1, 2, 4, 8) and o['tam_origen'] == 'acceso':
                tdef = {1: 'uint8', 2: 'uint16', 4: 'uint32', 8: 'uint64'}[tam]
            else:
                no('tam_sin_tipo'); continue
        elif o['tam_origen'] == 'tipo' and tam > 0 and tam <= 0x4000 and '--sin-raw' not in args:
            tdef, raw = f'raw{tam}', True
        else:
            no('tipo_no_admitido'); continue
        if any(va - MARGEN <= h < va + tam + MARGEN for h in HARNESS) or any(a <= va < b or va <= a < va + tam for a, b in HARNESS_RANGOS):
            no('humo'); continue
        i = bisect.bisect_right(todas, va)
        if i < len(todas) and todas[i] < va + tam:
            no('vecino_dentro'); continue
        cand[va] = (tdef, tam, raw)
    # un vecino anterior cuya extension llegue a este objeto tampoco vale
    for va in sorted(cand):
        i = bisect.bisect_left(todas, va)
        prev = todas[i - 1] if i > 0 else None
        if prev in cand and prev + cand[prev][1] > va:
            del cand[va]
            motivos['vecino_anterior'].append(va)
    # 2. analisis de contexto de cada aparicion ----------------------------------------------------------------------------------------
    archivos = defaultdict(set)
    for va in cand:
        for r in por[va]:
            archivos[r['archivo']].add(va)
    ocur = defaultdict(list)     # va -> [(archivo, clase, ini, fin, tipo)]
    malos = {}
    textos = {}
    for arch, vas in archivos.items():
        raw = (RAIZ / arch).read_bytes()
        txt = raw.decode('utf-8', errors='surrogateescape')
        bl = de.blanquear_comentarios(txt)
        textos[arch] = (raw, txt, bl)
        for m in de.HEX.finditer(bl):
            va = int(m.group(1), 16)
            if va not in vas:
                continue
            c = contexto(bl, m.start(), m.end())
            if c[0] is None:
                malos[va] = c[1]
            elif not c[4]:
                malos[va] = 'extension_dudosa'
            elif sec == '.rdata' and escribe(bl, m.start(), m.end(), c):
                malos[va] = 'escritura'
            else:
                ocur[va].append((arch,) + c[:4])
    for va, mot in malos.items():
        if va in cand:
            del cand[va]
            motivos['contexto_' + mot].append(va)
    # ventana de tablas de extension desconocida: el interior de una tabla indexada (o de tamano solo estimado) no se puede separar
    # (0) uso indexado visto en TODO el codigo (tambien archivos excluidos y objetos descartados por otro motivo): L + i, L[i]
    indexados_global = set()
    for dp, _, fs in os.walk('source'):
        for f in fs:
            if not f.endswith(('.h', '.cpp', '.hpp', '.inc', '.c')):
                continue
            pth = os.path.join(dp, f)
            if '/game_sa/DatosExe/' in pth.replace(chr(92), '/'):
                continue
            try:
                tx = de.blanquear_comentarios(open(pth, encoding='utf-8', errors='replace').read())
            except OSError:
                continue
            if '0x' not in tx and '0X' not in tx:
                continue
            for m in de.HEX.finditer(tx):
                v = int(m.group(1), 16)
                if SEC['A'][1] <= v < SEC['C'][2]:
                    c = contexto(tx, m.start(), m.end())
                    if c[0] is None and c[1] == 'indexado':
                        indexados_global.add(v)
    # (1) bases de tablas indexadas (por el EXE o por el contexto del C++): la extension real es desconocida, ventana grande
    tablas = {va for va, o in objs.items() if o['tabla_indexada'] == '1'}
    tablas |= {va for va, mot in malos.items() if mot == 'indexado'} | indexados_global
    tablas |= set(motivos['indexada'])
    # (2) objetos de extension solo estimada o de tipo sin tamano: ventana corta
    estimados = {va for va, o in objs.items() if o['tam_origen'] == 'hueco' and int(o['refs_dll']) > 0}
    estimados |= set(motivos['tam_dudoso']) | set(motivos['tipo_sin_tam']) | set(motivos['tipos_mayores'])
    estimados |= {va for va, mot in malos.items() if mot == 'extension_dudosa'}
    # extension de una tabla indexada: hasta la siguiente direccion que el EXE (no el C++) cita, entre VENTANA y VENTANA_TABLA bytes
    conocidas = sorted(va for va, o in objs.items() if int(o['funcs_exe_vivas']) or int(o['funcs_exe_reemplazadas']))

    def ext_tabla(base):
        i = bisect.bisect_right(conocidas, base)
        sig = conocidas[i] if i < len(conocidas) else base + VENTANA_TABLA
        return max(VENTANA, min(sig - base, VENTANA_TABLA))
    # C++ que indexa el literal (L + i): ventana fija grande; EXE que indexa: hasta la siguiente direccion que cita el EXE
    tablas_cpp = (indexados_global | {va for va, mot in malos.items() if mot == 'indexado'})
    ventanas = sorted([(va, VENTANA_TABLA if va in tablas_cpp else ext_tabla(va)) for va in tablas] + [(va, VENTANA) for va in estimados - tablas])
    claves = [a for a, _ in ventanas]
    for va in list(cand):
        if sec == '.rdata':
            break      # constantes: una copia igual de un valor inmutable es segura aunque otra ruta lea la tabla original
        i = bisect.bisect_left(claves, va)
        # cualquier base anterior cuya ventana alcance a va
        j = i - 1
        while j >= 0 and va - claves[j] <= VENTANA_TABLA:
            a0, w = ventanas[j]
            if a0 < va <= a0 + w:
                del cand[va]
                motivos['ventana_de_tabla'].append(va)
                break
            j -= 1
    # coherencia: nº de apariciones analizadas = nº de referencias del escaner (por si el escaner vio una que aqui no)
    for va in list(cand):
        n_ref = len(por[va])
        if len(ocur[va]) != n_ref:
            del cand[va]
            motivos['apariciones_distintas'].append(va)
    # los StaticRef con tipo distinto en un contexto de variable de espacio de nombres exigirian constante: solo se admite igual
    for va in list(cand):
        pass
    if '--solo' in args:
        solo = {int(x.strip(), 16) for x in Path(args[args.index('--solo') + 1]).read_text().split() if x.strip()}
        cand = {k: v for k, v in cand.items() if k in solo}
    if '--no' in args:
        no_ = {int(x.strip(), 16) for x in Path(args[args.index('--no') + 1]).read_text().split() if x.strip()}
        cand = {k: v for k, v in cand.items() if k not in no_}
    sel = sorted(cand)
    nref = sum(len(ocur[v]) for v in sel)
    print(f'R5-{lote} ({sec}): {len(sel)} objetos, {nref} referencias, {sum(cand[v][1] for v in sel)} bytes')
    for m, vs in sorted(motivos.items(), key=lambda kv: -len(kv[1])):
        print(f'   descartados por {m}: {len(vs)}')
    out = RAIZ / 'docs/DatosExe/lotes'
    out.mkdir(parents=True, exist_ok=True)
    with (out / f'R5{lote}.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'seccion', 'tipo', 'bytes', 'refs', 'archivos'])
        for va in sel:
            w.writerow([f'0x{va:08X}', sec, cand[va][0], cand[va][1], len(ocur[va]), ';'.join(sorted({Path(x[0]).name for x in ocur[va]}))[:100]])
    if not aplicar:
        return
    # 3. definiciones ---------------------------------------------------------------------------------------------------------------------
    nom = 'DataR5' + lote
    cab = ['#pragma once', '', '#include <cstdint>', '#include <cstddef>', '#include <array>', '#include "Vista.h"', '', '/*', f' * Ronda 5 de DatosExe, lote {lote} ({sec}): objetos que el codigo propio citaba por direccion fija, como definiciones',
           f' * propias con el valor inicial del EXE canonico. Generado por contrib/datos_exe_r5.py; definidas en {nom}.cpp.',
           ' * Se incluye despues de StdInc.h del consumidor. No se leen del EXE en ejecucion.', ' */', '', 'namespace DatosExe::R5 {']
    cpp = ['#include "StdInc.h"', '', f'#include "{nom}.h"', '', '#include <bit>', '', 'namespace DatosExe::R5 {']
    for va in sel:
        tdef, tam, raw = cand[va]
        if raw:
            off = pe.offset(va, tam) if sec != '.bss' else None
            bts = bytes(tam) if sec == '.bss' else (pe.data[off:off + tam] if off is not None else None)
            if bts is None:
                raise SystemExit(f'sin bytes para 0x{va:X}')
            decl = f'alignas(16) unsigned char r{va:X}[{tam}]'
            cab.append(f'extern {decl}; // 0x{va:08X}')
            if not any(bts):
                cpp.append(f'{decl}{{}}; // 0x{va:08X}')
            else:
                cpp.append(f'{decl}{{{", ".join(str(x) for x in bts)}}}; // 0x{va:08X}')
            continue
        el, n, _ = tipo_def(tdef)
        val = valor_inicial(pe, va, el, n, sec)
        if val is None:
            raise SystemExit(f'sin bytes para 0x{va:X}')
        v, cero = val
        es_std = norm(tdef).startswith('std::array')
        if n is None:
            decl = f'{CT[el]} v{va:X}'
        elif es_std:
            decl = f'std::array<{CT[el]}, {n}> v{va:X}'
        else:
            decl = f'{CT[el]} v{va:X}[{n}]'
        cab.append(f'extern {decl}; // 0x{va:08X}')
        if n is None:
            cpp.append(f'{decl} = {v}; // 0x{va:08X}' if not cero else f'{decl}{{}}; // 0x{va:08X}')
        elif es_std:
            cpp.append(f'{decl}{{}}; // 0x{va:08X}' if cero else f'{decl}{{{{{", ".join(v)}}}}}; // 0x{va:08X}')
        else:
            cpp.append(f'{decl}{{}}; // 0x{va:08X}' if cero else f'{decl}{{{", ".join(v)}}}; // 0x{va:08X}')
    cab += ['', '} // namespace DatosExe::R5', '']
    cpp += ['', '} // namespace DatosExe::R5', '']
    d = RAIZ / 'source/game_sa/DatosExe'
    (d / (nom + '.h')).write_text(NL.join(cab), encoding='utf-8', newline=NL)
    (d / (nom + '.cpp')).write_text(NL.join(cpp), encoding='utf-8', newline=NL)
    # 4. reescritura de los consumidores --------------------------------------------------------------------------------------------------
    inc = f'#include "DatosExe/{nom}.h"'
    por_f = defaultdict(list)
    for va in sel:
        for arch, clase, a, b, t in ocur[va]:
            por_f[arch].append((a, b, va, clase, t))
    for arch in sorted(por_f):
        raw, txt, bl = textos[arch]
        if arch.endswith('.cpp') and not re.search(r'#include\s+"StdInc.h"', txt) and not re.search(r'(?m)^#include\s', txt):
            raise SystemExit('sin #include (excluir el archivo): ' + arch)
        if arch.endswith('.h') and not re.search(r'#pragma once', txt):
            raise SystemExit('cabecera sin #pragma once (excluir el archivo): ' + arch)
    for arch, lst in sorted(por_f.items()):
        raw, txt, bl = textos[arch]
        piezas, pos, cnt = [], 0, 0
        for a, b, va, clase, t in sorted(lst):
            tdef, _tam, es_raw = cand[va]
            nombre = f"DatosExe::R5::{'r' if es_raw else 'v'}{va:X}"
            if clase == 'static':
                rep = nombre if (not es_raw and norm(t) == tdef) else f'DATOSEXE_VISTA({nombre}, {t})'   # vista tipada con sizeof comprobado en compilacion (auditoria de tamanos)
            elif clase == 'rcast':
                rep = f'(&DATOSEXE_VISTA({nombre}, {t}))'
            elif clase == 'ccast':
                rep = f'(&{nombre})'
            else:
                rep = f'reinterpret_cast<uintptr_t>(&{nombre})'
            piezas += [txt[pos:a], rep]
            pos = b
            cnt += 1
        piezas.append(txt[pos:])
        nuevo = ''.join(piezas)
        nl = '\r\n' if b'\r\n' in raw else NL
        if inc not in nuevo:
            if arch.endswith('.cpp'):
                mm = re.search(r'#include\s+"StdInc.h"[^' + NL + r']*' + NL, nuevo)
                if not mm:      # archivo compilado sin PCH (X87Math.cpp): tras el ultimo #include del primer bloque
                    todos = list(re.finditer(r'(?m)^#include\s[^' + NL + r']*' + NL, nuevo))
                    mm = todos[-1] if todos else None
            else:
                mm = re.search(r'#pragma once[^' + NL + r']*' + NL, nuevo)
            if not mm:
                raise SystemExit('sin punto de insercion en ' + arch)
            nuevo = nuevo[:mm.end()] + inc + nl + nuevo[mm.end():]
        (RAIZ / arch).write_bytes(nuevo.encode('utf-8', errors='surrogateescape'))
        print(arch, cnt)


if __name__ == '__main__':
    main()
