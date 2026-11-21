"""Auditoria del CODIGO VIVO del EXE: funciones que ni el C++ ni un enganche reemplazan, y de donde cuelgan.

Estatica (no ejecuta el juego). Entradas, todas dentro del repositorio salvo el EXE canonico:
  docs/MemoryMap/binary_function_ranges.csv  (cuerpos; una funcion puede tener varios trozos)
  docs/MemoryMap/binary_functions.csv        (entradas de Ghidra, nombre, thunk, nombre del catalogo)
  docs/JuegoExe/funciones.csv, docs/RenderWareExe/funciones.csv, docs/JuegoExe/excluidas.csv
  docs/hooks.csv + hooks_notsa.csv + los RH_Scoped*Install del codigo (direcciones enganchadas)
  source/ (referencias por direccion desde el C++: plugin::Call, punteros a funcion, enganches)
  gta-sa.exe canonico (capstone: llamadas, saltos, inmediatos, punteros en datos, importaciones, inicializadores)

Este modulo solo reune los hechos (cache opcional con --cache); el informe lo escribe codigo_vivo_informe.py.
Salida en docs/CodigoVivoExe/: funciones-vivas.csv, grupos.csv, resumen.json (y destinos-sin-catalogo.csv, entradas-sin-catalogar.csv
con --destinos). Uso:
  python -X utf8 contrib/codigo_vivo.py [--exe RUTA] [--salida docs/CodigoVivoExe]
Cotas: no se siguen llamadas indirectas por registro ni punteros construidos en tiempo de ejecucion; la alcanzabilidad es
una cota (los punteros en tablas de datos se tratan como raices conservadoras).
"""
import argparse
import bisect
import csv
import json
import os
import pickle
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
from pe_inventory import PE  # noqa: E402
from datos_exe import quitar_comentarios  # noqa: E402

DOCS = ROOT / 'docs'
EXE_DEFECTO = r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
CODE_LO, CODE_HI = 0x401000, 0x858000
ENTRADA = 0x824570
HOOK_KINDS = ('RH_ScopedInstall', 'RH_ScopedGlobalInstall', 'RH_ScopedNamedGlobalInstall', 'RH_ScopedOverloadedInstall',
              'RH_ScopedNamedInstall', 'RH_ScopedGlobalOverloadedInstall')
HEX6 = re.compile(r'(?<![0-9A-Za-z_])0[xX]([0-9A-Fa-f]{6,8})(?![0-9A-Za-z_])')


def leer(path):
    with open(path, encoding='utf-8-sig', newline='') as s:
        return list(csv.DictReader(s))


def hx(n):
    return f'0x{n:08X}'


def region(va):
    if va < 0x750000:
        return 'juego'
    if va < 0x820000:
        return 'renderware'
    if va < 0x857000:
        return 'crt'
    return 'rwcseg'


# ------------------------------------------------------------------ funciones del mapa

def cargar_funciones():
    rangos = defaultdict(list)
    for r in leer(DOCS / 'MemoryMap/binary_function_ranges.csv'):
        rangos[int(r['function_va'], 16)].append((int(r['start_va'], 16), int(r['end_va'], 16)))
    ghidra = {int(r['va'], 16): r for r in leer(DOCS / 'MemoryMap/binary_functions.csv')}
    funcs = {}
    for va, rs in rangos.items():
        rs.sort()
        g = ghidra[va]
        funcs[va] = dict(va=va, rangos=rs, bytes=sum(e - s + 1 for s, e in rs), trozos=len(rs), nombre=g['name'],
                         thunk=g['thunk'] == 'true', catalogo=g['catalogue_names'], virtuales=g['virtual_owners'],
                         ghidra_llamadas=int(g['direct_call_sites_to_entry'] or 0))
    return funcs


def cargar_estado(funcs):
    """Devuelve (rep, rw, jg, exc): conjuntos/dicts de los CSV de estado."""
    rep, rw, jg = set(), {}, {}
    for r in leer(DOCS / 'JuegoExe/funciones.csv'):
        va = int(r['va'], 16)
        jg[va] = r
        if r['estado'] == 'reemplazada':
            rep.add(va)
    for r in leer(DOCS / 'RenderWareExe/funciones.csv'):
        va = int(r['va'], 16)
        rw[va] = r
        if r['estado'] == 'reemplazada':
            rep.add(va)
    exc = {int(r['va'], 16): r for r in leer(DOCS / 'JuegoExe/excluidas.csv')}
    return rep, rw, jg, exc


# ------------------------------------------------------------------ C++ (la DLL)

def escanear_fuente(starts, contenido):
    """-> (hooks:{va:[archivo]}, refs:{va:[(archivo,linea,forma)]}); direcciones que caen en el codigo del EXE."""
    hooks, refs, desact = defaultdict(list), defaultdict(list), defaultdict(list)
    for dp, _, fs in os.walk(ROOT / 'source'):
        for f in sorted(fs):
            if not f.endswith(('.h', '.cpp', '.hpp', '.inc', '.c')):
                continue
            p = os.path.join(dp, f)
            rel = os.path.relpath(p, ROOT).replace('\\', '/')
            try:
                txt = quitar_comentarios(open(p, encoding='utf-8', errors='replace').read())
            except OSError:
                continue
            if '0x' not in txt and '0X' not in txt:
                continue
            # una instruccion de enganche puede ocupar varias lineas: se analiza por sentencia
            for m in re.finditer(r'(RH_Scoped\w*)\s*\(([^;]*)\)\s*;', txt):
                if m.group(1) in HOOK_KINDS:
                    a = HEX6.findall(m.group(2))
                    if a:
                        va = int(a[-1], 16)
                        if CODE_LO <= va < CODE_HI:
                            if re.search(r'enabled\s*=\s*false', m.group(2)):
                                desact[va].append(rel)
                            else:
                                hooks[va].append(rel)
            for ln, linea in enumerate(txt.split('\n'), 1):
                if '0x' not in linea and '0X' not in linea:
                    continue
                for m in HEX6.finditer(linea):
                    va = int(m.group(1), 16)
                    if CODE_LO <= va < CODE_HI:
                        pre = linea[:m.start()]
                        if re.search(r'RH_Scoped\w*\s*\(', linea):
                            forma = 'enganche'
                        elif 'plugin::Call' in linea or 'CallAndReturn' in linea or 'CallMethod' in linea:
                            forma = 'plugin_call'
                        elif re.search(r'\*\s*>?\s*\)\s*\(?\s*$|reinterpret_cast\s*<[^>]*\(\s*\*', pre) or '(*)' in linea:
                            forma = 'puntero_funcion'
                        else:
                            forma = 'literal'
                        refs[va].append((rel, ln, forma))
    return hooks, refs, desact


# ------------------------------------------------------------------ EXE

def importaciones(pe):
    """slot IAT (va) -> 'DLL::nombre'."""
    d = pe.data
    pos = struct.unpack_from('<I', d, 60)[0]
    rva = struct.unpack_from('<I', d, pos + 24 + 96 + 8)[0]
    out = {}
    o = pe.offset(pe.base + rva, 20)
    while o is not None:
        oft, _, _, name, ft = struct.unpack_from('<IIIII', d, o)
        if not name:
            break
        no = pe.offset(pe.base + name, 1)
        dll = d[no:d.index(b'\0', no)].decode('ascii', 'replace')
        t = oft or ft
        k = 0
        while True:
            to = pe.offset(pe.base + t + 4 * k, 4)
            if to is None:
                break
            v = struct.unpack_from('<I', d, to)[0]
            if not v:
                break
            if v & 0x80000000:
                nm = f'#{v & 0xFFFF}'
            else:
                ho = pe.offset(pe.base + v, 3)
                nm = d[ho + 2:d.index(b'\0', ho + 2)].decode('ascii', 'replace')
            out[pe.base + ft + 4 * k] = f'{dll}::{nm}'
            k += 1
        o += 20
    return out


def escanear_exe(pe, funcs):
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_MEM, CS_OP_IMM, CS_GRP_JUMP, CS_GRP_CALL
    from capstone.x86 import X86_REG_INVALID
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    starts = sorted(funcs)
    ini_trozos = sorted((s, e, fv) for fv, f in funcs.items() for s, e in f['rangos'])
    ini_s = [t[0] for t in ini_trozos]
    iat = importaciones(pe)

    def contenedora(a):
        i = bisect.bisect_right(ini_s, a) - 1
        if i >= 0 and ini_trozos[i][0] <= a <= ini_trozos[i][1]:
            return ini_trozos[i][2]
        return None

    aristas = defaultdict(set)         # (origen, destino) -> {'call','jmp','imm','jcc'}
    iat_uso = defaultdict(set)         # funcion -> {nombre importacion}
    rasgos = defaultdict(Counter)
    fragmento = defaultdict(set)       # funcion contenedora -> {origen} (saltos a mitad de otra funcion)
    entradas_mitad = defaultdict(set)  # direccion destino mitad de funcion -> {origen}
    mem_codigo = defaultdict(set)      # acceso a memoria con direccion absoluta dentro de .text (tablas de saltos/indices) -> {funciones}
    gap_calls = defaultdict(set)       # hueco (inicio) -> funciones a las que llama o salta
    gap_ini = []                       # (inicio, fin) de los huecos de .text
    a_hueco = defaultdict(set)         # destino de call/jmp que cae en un hueco sin funcion -> {origenes}
    imm_codigo = defaultdict(set)      # inmediato que apunta a codigo que no es inicio de funcion -> {origenes}
    gap_rdata = defaultdict(set)       # huecos de .text (no son funcion en Ghidra): inmediato a .rdata -> {direccion de la instruccion}
    imm_rdata = defaultdict(set)       # inmediato que apunta a .rdata -> {funciones que lo cargan} (estructuras de excepciones)
    for fva, f in funcs.items():
        for s, e in f['rangos']:
            off = pe.offset(s, 1)
            if off is None:
                continue
            code = pe.data[off:off + (e - s + 1)]
            for ins in md.disasm(code, s):
                mn = ins.mnemonic
                rt = rasgos[fva]
                rt['ins'] += 1
                if mn.startswith('f') and mn not in ('fs',):
                    rt['x87'] += 1
                elif mn.startswith(('movss', 'movsd', 'addss', 'addsd', 'mulss', 'mulsd', 'cvt', 'sqrtss', 'sqrtsd', 'pxor', 'movaps',
                                    'movups', 'movq', 'movd', 'andps', 'xorps', 'ucomis', 'comis', 'subss', 'subsd', 'divss', 'divsd')):
                    rt['sse'] += 1
                elif mn in ('pushad', 'popad'):
                    rt['pushad'] += 1
                elif mn.startswith('prefetch') or mn in ('femms', 'pfadd', 'pfmul', 'pfsub', 'pfmax', 'pfmin', 'pfrcp', 'pfrsqrt',
                                                         'pf2id', 'pi2fd', 'pfcmpgt', 'pfacc', 'pfrcpit1', 'pfrcpit2', 'pfrsqit1'):
                    rt['now3d'] += 1
                if mn == 'call':
                    rt['calls'] += 1
                op0 = ins.operands[0] if ins.operands else None
                if op0 is not None and (ins.group(CS_GRP_JUMP) or ins.group(CS_GRP_CALL)):
                    if op0.type == CS_OP_IMM:
                        t = op0.imm & 0xFFFFFFFF
                        kind = 'call' if mn == 'call' else ('jmp' if mn == 'jmp' else 'jcc')
                        if t in funcs:
                            if t != fva:
                                aristas[(fva, t)].add(kind)
                        elif CODE_LO <= t < CODE_HI:
                            c = contenedora(t)
                            if c is None:
                                a_hueco[t].add(fva)
                            elif c != fva:
                                entradas_mitad[t].add(fva)
                                fragmento[c].add(fva)
                                aristas[(fva, c)].add('mitad')
                    elif op0.type == CS_OP_MEM and op0.mem.base == X86_REG_INVALID and op0.mem.index == X86_REG_INVALID:
                        d = op0.mem.disp & 0xFFFFFFFF
                        if d in iat:
                            iat_uso[fva].add(iat[d])
                        rt['indirectas_abs'] += 1
                    else:
                        rt['indirectas_reg'] += 1
                for op in ins.operands:
                    if op.type == CS_OP_IMM and not (ins.group(CS_GRP_JUMP) or ins.group(CS_GRP_CALL)):
                        t = op.imm & 0xFFFFFFFF
                        if t in funcs and t != fva:
                            aristas[(fva, t)].add('imm')
                        elif 0x858000 <= t < 0x8A4000:
                            imm_rdata[t].add(fva)
                        elif CODE_LO <= t < CODE_HI and ins.mnemonic in ('push', 'mov', 'lea'):
                            imm_codigo[t].add(fva)
                    elif op.type == CS_OP_MEM and op.mem.base == X86_REG_INVALID and op.mem.segment == 0:
                        d = op.mem.disp & 0xFFFFFFFF
                        if d in iat:
                            iat_uso[fva].add(iat[d])
                        elif CODE_LO <= d < CODE_HI:
                            mem_codigo[d].add(fva)
    # huecos del .text sin funcion de Ghidra (envolturas de excepciones, saltos, relleno, datos)
    for r in leer(DOCS / 'MemoryMap/binary_unassigned_ranges.csv'):
        if r['section'] != '.text':
            continue
        s, e = int(r['va'], 16), int(r['end_va'], 16)
        off = pe.offset(s, 1)
        if off is None or e <= s:
            continue
        gap_ini.append((s, e))
        for ins in md.disasm(pe.data[off:off + (e - s + 1)], s):
            if (ins.group(CS_GRP_JUMP) or ins.group(CS_GRP_CALL)) and ins.operands and ins.operands[0].type == CS_OP_IMM:
                tg = ins.operands[0].imm & 0xFFFFFFFF
                if tg in funcs:
                    gap_calls[s].add(tg)
            for op in ins.operands:
                if op.type == CS_OP_IMM and not (ins.group(CS_GRP_JUMP) or ins.group(CS_GRP_CALL)):
                    t = op.imm & 0xFFFFFFFF
                    if 0x858000 <= t < 0x8A4000:
                        gap_rdata[t].add(ins.address)
                    elif CODE_LO <= t < CODE_HI and ins.mnemonic in ('push', 'mov', 'lea'):
                        imm_codigo[t].add(('hueco', ins.address))
    # punteros en datos y en huecos de .text
    punt = defaultdict(list)           # funcion -> [(va_dato, seccion)]
    for s in pe.sections:
        if s['raw_size'] == 0:
            continue
        data = pe.data[s['raw_offset']:s['raw_offset'] + s['raw_size'] // 4 * 4]
        ejecutable = s['executable']
        for i, (v,) in enumerate(struct.iter_unpack('<I', data)):
            if CODE_LO <= v < CODE_HI and v in funcs:
                a = s['va'] + 4 * i
                if ejecutable and contenedora(a) is not None:
                    continue
                punt[v].append((a, s['name']))
    return dict(aristas=dict(aristas), iat_uso=dict(iat_uso), rasgos={k: dict(v) for k, v in rasgos.items()},
                imm_rdata=dict(imm_rdata), a_hueco=dict(a_hueco), mem_codigo=dict(mem_codigo), gap_calls=dict(gap_calls), gap_ini=gap_ini, imm_codigo=dict(imm_codigo), gap_rdata=dict(gap_rdata), fragmento=dict(fragmento), entradas_mitad=dict(entradas_mitad), punteros=dict(punt), iat=iat)


def tablas_inicio(pe, funcs):
    """Inicializadores estaticos de _initterm: C 0x8A5A14-0x8A5A30 y C++ 0x8A4000-0x8A5A10 (medidos por IND0A)."""
    out = {}
    for ini, fin, tipo in ((0x8A5A14, 0x8A5A30, 'C'), (0x8A4000, 0x8A5A10, 'C++')):
        w = pe.words(ini, (fin - ini) // 4)
        for v in w or ():
            if CODE_LO <= v < CODE_HI:
                out[v] = tipo
    return out


def cargar_ejecutado(args):
    cache = Path(args.cache) if args.cache else None
    if cache and cache.exists():
        return pickle.load(open(cache, 'rb'))
    pe = PE(Path(args.exe))
    funcs = cargar_funciones()
    ex = escanear_exe(pe, funcs)
    ex['tablas'] = tablas_inicio(pe, funcs)
    ex['pe_codigo'] = None
    hooks_src, refs_src, desact = escanear_fuente(set(funcs), None)
    res = dict(funcs=funcs, ex=ex, hooks_src=dict(hooks_src), refs_src=dict(refs_src), hooks_off=dict(desact))
    if cache:
        pickle.dump(res, open(cache, 'wb'))
    return res


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=EXE_DEFECTO)
    ap.add_argument('--cache', default=None)
    r = cargar_ejecutado(ap.parse_args())
    print(len(r['funcs']), len(r['ex']['aristas']), len(r['ex']['punteros']), len(r['hooks_src']), len(r['refs_src']))
