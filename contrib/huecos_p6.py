"""Parte 6 del codigo vivo: clasifica los 270 huecos del .text con codigo referenciado pero sin funcion de Ghidra.

Uso: python -X utf8 contrib/huecos_p6.py [--exe RUTA_gta_sa.exe] [--salida docs/CodigoVivoExe]
Escribe: huecos-p6.csv (una fila por pieza: trozo de codigo entre rellenos), cierre-p6.csv (una fila por hueco) y huecos-p6-resumen.csv.

Metodo (estatico, capstone sobre el EXE canonico):
 1. Cada hueco de huecos.csv con categoria `codigo con referencia` se parte en piezas: tramos de instrucciones entre rellenos (nop/int3);
    dos tramos se funden si el primero no termina en ret/jmp y nadie apunta al segundo.
 2. Entradas: toda llamada, salto o `push/mov` inmediato del .text (funciones y huecos) y todo dword del EXE (cualquier seccion) que apunte a la pieza.
 3. Origen de cada entrada: funcion reemplazada o enlazada (muerta), funcion viva descartada (excepciones, D3DX9, protector, QuickTime: muerta),
    funcion viva del juego/CRT, otra pieza (se propaga), inicializador estatico (atexit), tabla de datos o puntero en .data/.rdata.
 4. Cruce con el C++: la pieza esta enganchada (literal en RH_Scoped*, docs/hooks.csv) o citada (literal hexadecimal en source/).
Destinos: reconstruida, viva-sin-hook, viva-enlazada, viva-protector, cerrada-comentario, muerta-por-reemplazo, destructor-atexit, sin-referencia-real,
crt-enlazar, d3dx9-enlazar, pendiente-dato.
Regla de raices por valor (contrib/raices_valor_p6.py, docs/CodigoVivoExe/reclasificacion-raices-valor.md): una direccion que el C++ escribe como
VALOR en codigo (tabla, campo de callback, inicializador, llamada por direccion) o que el codigo compilado lleva como inmediato (fase8-experimento/
dependencias_codigo.csv, confianza A/B) es una raiz viva sin hook; la antigua clase `citada-en-cpp` ya no existe: o es viva (valor), o enlazada (libjpeg),
o del protector, o solo un comentario (cerrada-comentario).
"""
import argparse
import bisect
import csv
import pickle
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

import capstone

sys.path.insert(0, str(Path(__file__).resolve().parent))
import raices_valor_p6 as rv  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / 'docs'


# Revision de las piezas "citada-en-cpp" que podian ser funciones vivas sin enganche (parte CRT de la fase 8, rama vivo-crtf8): destino que sustituye al de «citada-en-cpp» y motivo.
# Una pieza enganchada (hooks.csv o RH_Scoped*) sigue saliendo como `reconstruida` (0x61A8A0, 0x68B7D0 y 0x8194D0 se enganchan en esa rama).
REVISION = {
    0x453FB0: ('muerta-por-reemplazo', 'accion=documentar; destructor `ret` de los elementos de CPhoneInfo (eh vector); solo lo cita un comentario de PhoneInfo.cpp y el constructor 0x4541E0 ya es C++ (no llama al iterador): muerta'),
    0x5C91E0: ('citada-en-cpp', 'accion=documentar; ycck_cmyk_convert de libjpeg (jdcolor.c), instalada por JPegCompress.cpp con LIBJPEG_CB: con GTASA_WITH_LIBJPEG es la de libs/libjpeg-6b (enlazada, como libpng/zlib); sin la opcion sigue la del EXE'),
    0x61A8A0: ('reconstruida', 'accion=enganchar; CTaskSimpleAnim::FinishRunAnimCB ya era C++ (TaskSimpleAnim.cpp) pero sin RH_ScopedInstall; viva (la registran tareas del EXE): enganchada'),
    0x68B7D0: ('reconstruida', 'accion=enganchar; CTaskSimpleArrestPed::FinishAnimArrestPedCB ya era C++ sin enganche; viva: enganchada'),
    0x7F40F0: ('pendiente-dato', 'accion=reconstruir; callback de lectura por defecto del modulo de texturas de RenderWare (+0x14 de 0xC97B4C, lo guarda _rwTextureModuleOpen): viva sin enganche; 966 bytes; lote V7/rwcore'),
    0x7F5140: ('pendiente-dato', 'accion=reconstruir; generacion de mipmaps por defecto del modulo de texturas (+0x2C): viva sin enganche; 652 bytes; lote V7/rwcore'),
    0x7F53D0: ('pendiente-dato', 'accion=reconstruir; busqueda de texturas por nombre por defecto del modulo de texturas (+0x18): viva sin enganche; 292 bytes; lote V7/rwcore'),
    0x7FFF00: ('pendiente-dato', 'accion=reconstruir; funcion estandar 0x09 (RwImageFindRasterFormat) del controlador D3D9, guardada en _rwD3D9DeviceSystemStandards: viva sin enganche; 208 bytes; controlador D3D9 (otro agente)'),
    0x8194D0: ('reconstruida', 'accion=enganchar; `mov ax, 0F7D1h; ret` (OSErr -2095), valor de los tres punteros de QuickTime (0x8E2A0C/10/14): QtmlUnavailableStub la reconstruye y la engancha'),
}


def h(x):
    return int(x, 16)


def leer(p):
    with open(p, encoding='utf-8-sig', newline='') as s:
        return list(csv.DictReader(s))


class Exe:
    def __init__(self, path):
        self.data = open(path, 'rb').read()
        d = self.data
        e = struct.unpack_from('<I', d, 0x3C)[0]
        n = struct.unpack_from('<H', d, e + 6)[0]
        osz = struct.unpack_from('<H', d, e + 20)[0]
        self.base = struct.unpack_from('<I', d, e + 24 + 28)[0]
        self.secs = []
        for i in range(n):
            o = e + 24 + osz + i * 40
            nm = d[o:o + 8].rstrip(b'\0').decode()
            vsz, va, rsz, rp = struct.unpack_from('<IIII', d, o + 8)
            self.secs.append((nm, self.base + va, vsz, rp, rsz))
        t = [s for s in self.secs if s[0] == '.text'][0]
        self.t0, self.t1 = t[1], t[1] + t[2]

    def rd(self, va, k):
        for nm, sva, vsz, rp, rsz in self.secs:
            if sva <= va < sva + max(vsz, rsz):
                off = va - sva
                return self.data[rp + off:rp + min(off + k, rsz)]
        return b''


# Cierres de la rama vivo-p6b (cierre de los 11 huecos pendientes). El analisis automatico solo ve como entradas los call/jmp/push/mov inmediato y los dwords de datos; no ve los
# `mov [reg+x], imm32` con los que los nodos de RenderWare guardan sus callbacks ni las funciones pegadas a otra, y por eso estas piezas salian como pendiente-dato. Cada entrada es
# va de la pieza -> (destino, nota).
MANUAL = {
    # Hueco 0x452F90 / 0x453F91 (CPhoneInfo y busqueda de ruta sobre rejilla)
    0x00453E40: ('reconstruida', 'CPhoneInfo::PhoneAnimAnswerCallback (lote P6B), enganchada en PhoneInfo.cpp'),
    0x00453000: ('sin-referencia-real', 'ayudante de la busqueda de ruta 0x453690: nadie llama a la rutina (solo dwords espurios)'),
    0x00453060: ('sin-referencia-real', 'ayudante de la busqueda de ruta 0x453690: nadie llama a la rutina (solo dwords espurios)'),
    0x00453280: ('sin-referencia-real', 'cuerpo de 0x453060 (mismo codigo): sin llamadores'),
    0x00453690: ('sin-referencia-real', 'busqueda de ruta sobre rejilla 40x40 de sectores repetidos: sin llamadores, codigo muerto'),
    0x00453A00: ('sin-referencia-real', 'bucle interno de 0x453690'),
    0x00453AE0: ('sin-referencia-real', 'bucle interno de 0x453690'),
    0x00454010: ('sin-referencia-real', 'metodo de CPhoneInfo (+4 cabinas de 0x34 bytes) sin llamadores'),
    # Hueco 0x47C0F5: despachadores originales de las ranuras 3 y 4 de 0x8A6168
    0x0047C100: ('reconstruida', 'CRunningScript::ProcessCommands300To399 (lote P6B) y ProcessCommands400To499 (0x47D210, dentro de la misma pieza)'),
    0x0047D520: ('reconstruida', 'cola de ProcessCommands400To499 (0x47D210), enganchada en RunningScriptCommands300To499.cpp'),
    # Huecos de RenderWare: callbacks de los nodos AllInOne de D3D9
    0x00756DF0: ('reconstruida', 'D3D9DefaultRenderCallBack (lote P6B)'),
    0x00757400: ('reconstruida', 'D3D9AtomicDefaultLightingCallBack (lote P6B)'),
    0x007575F0: ('reconstruida', 'D3D9AtomicAllInOneNode (lote P6B)'),
    0x00757890: ('reconstruida', 'D3D9AtomicAllInOneNodeInit (lote P6B)'),
    0x00758270: ('reconstruida', 'D3D9AtomicDefaultReinstanceCallback (lote P6B); antes citada-en-cpp solo en un comentario'),
    0x0075E3F0: ('reconstruida', 'D3D9WorldSectorAllInOneNode (lote P6B)'),
    0x0075E540: ('reconstruida', 'D3D9WorldSectorAllInOneNodeInit (lote P6B)'),
    0x0075E570: ('reconstruida', 'D3D9WorldSectorDefaultLightingCallBack (lote P6B)'),
    0x0075E5E0: ('reconstruida', 'D3D9WorldSectorDefaultInstanceCallback (lote P6B)'),
    0x007CAC60: ('reconstruida', '_rpSkinSSEMatrixBlend4 (lote P6B)'),
    0x007CAF50: ('reconstruida', '_rpSkinSSEMatrixBlend2 (lote P6B)'),
    0x007CB190: ('reconstruida', '_rxD3D9SkinVertexShaderBeginCallBack (lote P6B)'),
    0x007CB240: ('reconstruida', '_rpSkinD3D9AtomicAllInOneNodeInit (lote P6B)'),
    0x00770FD3: ('d3dx9-enlazar', 'comparador de qsort del compilador de shaders de D3DX9 (se enlaza D3DX9)'),
    0x007C073F: ('d3dx9-enlazar', 'cuerpo del compilador de shaders de D3DX9 (se enlaza D3DX9)'),
    0x0080FFB0: ('muerta-por-reemplazo', 'cola de la funcion reemplazada 0x80FC30 (la apunta un `mov eax, imm` de ella)'),
}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=r'D:\Original\_crt1a_work\gta_sa.exe')
    ap.add_argument('--salida', default=str(DOCS / 'CodigoVivoExe'))
    a = ap.parse_args()
    out = Path(a.salida)
    ex = Exe(a.exe)
    cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    cs.detail = True
    X86_IMM = capstone.x86.X86_OP_IMM

    funcs = [(h(r['start_va']), h(r['end_va'])) for r in leer(DOCS / 'MemoryMap' / 'binary_function_ranges.csv')]
    holes = [(h(r['va']), h(r['fin']), r['categoria'], r['region']) for r in leer(out / 'huecos.csv')]
    estado = {}
    for f in (DOCS / 'JuegoExe' / 'funciones.csv', DOCS / 'RenderWareExe' / 'funciones.csv'):
        for r in leer(f):
            estado[h(r['va'])] = r['estado']
    vivas = {h(r['va']): r for r in leer(out / 'funciones-vivas.csv')}
    fr = sorted(funcs)
    fa = [x for x, _ in fr]

    def func_of(x):
        i = bisect.bisect_right(fa, x) - 1
        if i >= 0 and fr[i][0] <= x <= fr[i][1]:
            return fr[i][0]
        return None

    hrng = sorted((x, y, c) for x, y, c, _ in holes)
    hst = [x[0] for x in hrng]

    def hole_at(x):
        i = bisect.bisect_right(hst, x) - 1
        if i >= 0 and hrng[i][0] <= x <= hrng[i][1]:
            return hrng[i]
        return None

    def region_de(x):
        f = func_of(x)
        if f is not None:
            return ('f', f)
        hh = hole_at(x)
        return ('h', hh[0]) if hh else None

    # --- referencias (llamadas, saltos, inmediatos del .text y dwords de todo el EXE)
    refs = defaultdict(list)
    seh_tablas = []   # (tabla de ambitos SEH, direccion del `push` en el prologo): `push tabla; call __SEH_prolog (0x8286E0)`
    for x, y in sorted(funcs + [(p, q) for p, q, _, _ in holes]):
        prev = None
        for ins in cs.disasm(ex.rd(x, y - x + 1), x):
            m = ins.mnemonic
            if m == 'call' and prev is not None and prev.mnemonic == 'push' and ins.operands and ins.operands[0].type == X86_IMM and ins.operands[0].imm == 0x8286E0                     and prev.operands[0].type == X86_IMM and prev.operands[0].imm >= ex.t1:
                seh_tablas.append((prev.operands[0].imm, prev.address))
            prev = ins
            if m == 'call' or m.startswith('j'):
                if ins.operands and ins.operands[0].type == X86_IMM:
                    refs[ins.operands[0].imm].append((ins.address, m))
            elif m in ('push', 'mov'):
                for op in ins.operands:
                    if op.type == X86_IMM and ex.t0 <= op.imm < ex.t1:
                        refs[op.imm].append((ins.address, 'imm'))
    seh_idx = {tb: pa for tb, pa in seh_tablas}
    raw = defaultdict(list)
    for nm, sva, vsz, rp, rsz in ex.secs:
        blob = ex.data[rp:rp + rsz]
        for off in range(0, len(blob) - 3):
            v = struct.unpack_from('<I', blob, off)[0]
            if ex.t0 <= v < ex.t1:
                # un puntero de la tabla de ambitos SEH de una funcion (manejador/funclet de desenrollado) solo vive si vive esa funcion: se atribuye al `push` de su prologo
                # (antes contaba como puntero de datos vivo y mantenia abiertas funclets de funciones muertas, p. ej. 0x82E924 `ctrlevent` de `signal`)
                dueno = next((seh_idx[a_] for a_ in range(sva + off, sva + off - 0x30, -1) if a_ in seh_idx and region_de(seh_idx[a_]) is not None and region_de(seh_idx[a_]) == region_de(v)), None)
                if dueno is not None:
                    refs[v].append((dueno, 'imm'))
                else:
                    raw[v].append(sva + off)

    # --- citas del C++ clasificadas (codigo / comentario / cadena) y evidencia del codigo compilado (inmediatos de los .obj)
    citas = rv.escanear_citas()
    obj_ev = {}
    dep_csv = out / 'fase8-experimento' / 'dependencias_codigo.csv'
    if dep_csv.exists():
        for r in leer(dep_csv):
            obj_ev[h(r['direccion'])] = (r['confianza'], r['formas'], r['ejemplo_archivo'], r['ejemplo_funcion'], r['ejemplo_instruccion'])
    # --- literales del C++
    hook_lits, all_lits = set(), set()
    pat = re.compile(r'0x(?:00)?([4-8][0-9A-Fa-f]{5})\b')
    for p in (ROOT / 'source').rglob('*'):
        if p.suffix not in ('.cpp', '.h', '.hpp', '.inl'):
            continue
        try:
            txt = p.read_text(encoding='utf-8', errors='replace')
        except OSError:
            continue
        for line in txt.splitlines():
            ms = pat.findall(line)
            if not ms:
                continue
            for m in ms:
                all_lits.add(int(m, 16))
                if 'RH_Scoped' in line:
                    hook_lits.add(int(m, 16))
    for r in leer(DOCS / 'hooks.csv'):
        m = re.fullmatch(r'0x[0-9A-Fa-f]{6,8}', r.get('address', ''))
        if m:
            hook_lits.add(int(r['address'], 16))

    # --- piezas
    pieces = []
    for a0, b0, cat, reg in holes:
        if not cat.startswith('código con ref'):
            continue
        cur, ps = None, []
        for i in cs.disasm(ex.rd(a0, b0 - a0 + 1), a0):
            pad = i.mnemonic in ('nop', 'int3')
            if pad:
                if cur:
                    cur[1] = i.address - 1
                    ps.append(cur)
                    cur = None
            else:
                if cur is None:
                    cur = [i.address, None, []]
                cur[2].append(i)
        if cur:
            last = cur[2][-1]
            cur[1] = last.address + last.size - 1
            ps.append(cur)
        merged = []
        for p in ps:
            if merged and merged[-1][2][-1].mnemonic not in ('ret', 'jmp') and p[0] not in refs and p[0] not in raw:
                merged[-1][1] = p[1]
                merged[-1][2] += p[2]
            else:
                merged.append(p)
        for s, e, il in merged:
            ents = []
            for t in range(s, e + 1):
                for src, k in refs.get(t, ()):
                    if not (s <= src <= e):
                        ents.append((t, src, k))
                for src in raw.get(t, ()):
                    if not (s <= src <= e):
                        ents.append((t, src, 'raw'))
            pieces.append(dict(hole=a0, s=s, e=e, size=e - s + 1, last=il[-1].mnemonic, n=len(il), ents=ents,
                               mn=[i.mnemonic for i in il]))
    pieces.sort(key=lambda o: o['s'])
    pst = [o['s'] for o in pieces]

    def piece_at(x):
        i = bisect.bisect_right(pst, x) - 1
        if i >= 0 and pieces[i]['s'] <= x <= pieces[i]['e']:
            return pieces[i]
        return None

    def srcclass(src, k):
        if k == 'raw' and not (ex.t0 <= src < ex.t1):
            return ('datos', None) if src % 4 == 0 else ('espurio', None)
        f = func_of(src)
        if f is not None:
            st = estado.get(f, 'viva')
            if st in ('reemplazada', 'enlazada'):
                return ('muerto', None)
            v = vivas.get(f)
            if v:
                g = v['grupo']
                if g.startswith(('Excep', 'D3DX9', 'Protec', 'QuickTime')) or v['decision'] == 'DESCARTAR':
                    return ('muerto', None)
                return ('vivo', None)
            return ('muerto', None)
        hh = hole_at(src)
        if hh:
            c = hh[2]
            if c.startswith('código con ref'):
                p = piece_at(src)
                return ('pieza', p) if p else ('espurio', None)
            if c.startswith('envoltura'):
                return ('muerto', None)
            if c.startswith('inicializador'):
                return ('init', None)
            if c.startswith('relleno') or (k == 'raw' and src % 4):
                return ('espurio', None)
            if c.startswith(('tabla de datos', 'tabla de saltos')):
                return ('espurio', None)   # tabla de saltos/indices: la usa la funcion que la contiene, no da vida por si sola
            return ('chunk', src)           # hueco sin referencia directa: codigo vivo solo si su tramo tiene entrada desde fuera
        return ('espurio', None)

    chunk_cache = {}

    def srcclass0(src, k):
        c_, x_ = srcclass(src, k)
        return ('espurio', None) if c_ in ('chunk', 'pieza') else (c_, x_)

    def chunk_alive(src):
        """Un tramo de un hueco sin referencia directa esta vivo si algo vivo apunta a su entrada (un nivel)."""
        hh = hole_at(src)
        if hh not in chunk_cache:
            cc, cur = [], None
            for i in cs.disasm(ex.rd(hh[0], hh[1] - hh[0] + 1), hh[0]):
                if i.mnemonic in ('nop', 'int3'):
                    if cur:
                        cc.append((cur[0], i.address - 1))
                        cur = None
                elif cur is None:
                    cur = [i.address]
            if cur:
                cc.append((cur[0], hh[1]))
            chunk_cache[hh] = cc
        for cs_, ce in chunk_cache[hh]:
            if cs_ <= src <= ce:
                for t in range(cs_, ce + 1):
                    for r_, k_ in list(refs.get(t, ())) + [(q, 'raw') for q in raw.get(t, ())]:
                        if cs_ <= r_ <= ce:
                            continue
                        c_, x_ = srcclass0(r_, k_)
                        if c_ in ('vivo', 'datos', 'init'):
                            return True
                return False
        return False

    for o in pieces:
        o['src'] = []
        for t, s_, k in o['ents']:
            c_, x_ = srcclass(s_, k)
            if c_ == 'chunk':
                c_, x_ = ('tabla', None) if chunk_alive(s_) else ('muerto', None)
            o['src'].append((t, s_, k, (c_, x_)))
    live = {}
    for _ in range(30):
        ch = False
        for o in pieces:
            if o['s'] in live:
                continue
            v = None
            for t, s, k, (c, x) in o['src']:
                if c == 'vivo':
                    v = 'vivo'
                elif c in ('init', 'datos', 'tabla') and v is None:
                    v = c
                elif c == 'pieza' and x['s'] != o['s'] and live.get(x['s']) and v is None:
                    v = live[x['s']]
            if v:
                live[o['s']] = v
                ch = True
        if not ch:
            break

    for o in pieces:
        ents = set(t for t, _, _ in o['ents']) | {o['s']}
        o['hooked'] = any(e in hook_lits for e in ents)
        o['cited'] = any(e in all_lits for e in ents)
        # citas en cualquier direccion de la pieza (no solo en las entradas) y evidencia compilada
        o['cs'] = rv.citas_en(citas, o['s'], o['e'])
        o['ccl'] = rv.resumen_clase(o['cs'])
        o['obj'] = sorted((a_, obj_ev[a_]) for a_ in obj_ev if o['s'] <= a_ <= o['e'])
        o['obj_ab'] = [a_ for a_, ev in o['obj'] if ev[0] in ('A', 'B')]
        o['obj_c_real'] = [a_ for a_, ev in o['obj'] if ev[0] == 'C' and any(a2 == a_ and k_ in (rv.K_VALOR, rv.K_LLAMADA, rv.K_LIBJPEG) for a2, _, _, k_, _ in o['cs'])]
        o['live'] = live.get(o['s'], 'muerto' if o['ents'] else 'sin-ref')
        s = o['s']
        o['region'] = 'crt' if s >= 0x820000 else 'renderware' if s >= 0x750000 else 'juego'
        if o['hooked']:
            d = 'reconstruida'
        elif o['live'] == 'vivo' and o['region'] == 'crt':
            d = 'crt-enlazar'
        elif o['live'] == 'init':
            d = 'destructor-atexit'
        elif o['ccl'] == 'libjpeg':
            d = 'viva-enlazada'       # LIBJPEG_CB / JPegCompressPlugin: con GTASA_WITH_LIBJPEG es la de libs/libjpeg-6b (se enlaza, como libpng y zlib)
        elif o['ccl'] == 'isla':
            d = 'd3dx9-enlazar'       # cita de la isla del compilador de shaders de D3DX9 (se enlaza)
        elif o['ccl'] == 'protector':
            d = 'viva-protector'      # tabla de ExeProtectionStub.cpp: se retira con el archivo (V1)
        elif (o['ccl'] == 'valor' or o['obj_ab'] or o['obj_c_real']) and rv.modulo_enlazado(s) == 'QuickTime':
            d = 'viva-quicktime'      # puente de QuickTime citado por QTMLRuntime.cpp: QuickTime se retira
        elif (o['ccl'] == 'valor' or o['obj_ab'] or o['obj_c_real']) and rv.modulo_enlazado(s):
            d = 'viva-enlazada'       # D3DX9 / libpng+zlib / CRT de MSVC: se enlazan
        elif o['ccl'] == 'valor' or o['obj_ab'] or o['obj_c_real']:
            d = 'viva-sin-hook'       # el C++ la escribe como valor / el codigo compilado la lleva como inmediato: raiz viva
        elif o['cited'] and o['live'] in ('muerto', 'sin-ref'):
            d = 'cerrada-comentario'  # solo un comentario/constante: no es raiz
        elif o['live'] in ('datos', 'tabla', 'vivo'):
            d = 'pendiente-dato'
        elif o['live'] == 'muerto':
            d = 'muerta-por-reemplazo'
        else:
            d = 'sin-referencia-real'
        if s in REVISION and d != 'reconstruida' and not d.startswith('viva-'):
            d = REVISION[s][0]
        # destino con el criterio anterior (antes de la regla de raices por valor): lo que decidia «citada-en-cpp» y «muerto» por falta de llamadores directos
        if o['hooked']:
            da = 'reconstruida'
        elif o['live'] == 'vivo' and o['region'] == 'crt':
            da = 'crt-enlazar'
        elif o['live'] == 'init':
            da = 'destructor-atexit'
        elif o['cited']:
            da = 'citada-en-cpp'
        elif o['live'] in ('datos', 'tabla', 'vivo'):
            da = 'pendiente-dato'
        elif o['live'] == 'muerto':
            da = 'muerta-por-reemplazo'
        else:
            da = 'sin-referencia-real'
        if s in REVISION and da != 'reconstruida':
            da = REVISION[s][0]
        o['destino_antes'] = da
        o['destino'] = d
        o['nota'] = ''
        if o['s'] in MANUAL:
            o['destino'], o['nota'] = MANUAL[o['s']]
            o['destino_antes'] = o['destino']
        if o['size'] <= 2 and o['last'] == 'ret':
            o['forma'] = 'ret-vacio'
        elif o['size'] <= 8 and o['last'] == 'jmp' and o['n'] <= 2:
            o['forma'] = 'thunk'
        elif o['size'] < 16:
            o['forma'] = 'trivial (<16 B)'
        else:
            o['forma'] = 'funcion'

    # --- salidas
    def srcdesc(o):
        c = Counter()
        for t, s, k, (cl, x) in o['src']:
            c[cl] += 1
        return ' '.join(f'{k}={v}' for k, v in sorted(c.items()))

    with open(out / 'huecos-p6.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'fin', 'bytes', 'hueco', 'region', 'forma', 'destino', 'vida', 'enganchada', 'citada', 'entradas', 'origenes', 'nota', 'citas_codigo', 'citas_comentario', 'evidencia_obj', 'tabla_destino', 'destino_antes'])
        for o in pieces:
            w.writerow(['0x%08X' % o['s'], '0x%08X' % o['e'], o['size'], '0x%08X' % o['hole'], o['region'], o['forma'], o['destino'], o['live'],
                        int(o['hooked']), int(o['cited']), len(set(t for t, _, _ in o['ents'])), srcdesc(o), o['nota'],
                        len({(f_, n_) for _, f_, n_, k_, _ in o['cs'] if k_ != rv.K_COMENTARIO and k_ != rv.K_ENGANCHE}),
                        len({(f_, n_) for _, f_, n_, k_, _ in o['cs'] if k_ == rv.K_COMENTARIO}),
                        ';'.join(f"0x{a_:08X}:{ev[0]}:{ev[1]}" for a_, ev in o['obj']), rv.tabla_destino(o['cs']), o['destino_antes']])
    por_hueco = defaultdict(list)
    for o in pieces:
        por_hueco[o['hole']].append(o)
    with open(out / 'revision-citadas-p6.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'fin', 'bytes', 'destino', 'accion_y_motivo'])
        for o in pieces:
            if o['s'] in REVISION:
                w.writerow(['0x%08X' % o['s'], '0x%08X' % o['e'], o['size'], o['destino'], REVISION[o['s']][1]])
    orden = ['viva-sin-hook', 'pendiente-dato', 'crt-enlazar', 'd3dx9-enlazar', 'viva-enlazada', 'viva-quicktime', 'viva-protector', 'reconstruida', 'cerrada-comentario', 'destructor-atexit', 'muerta-por-reemplazo', 'sin-referencia-real']
    hrows = []
    for a0, b0, cat, reg in sorted(holes):
        if not cat.startswith('código con ref'):
            continue
        lote = len(hrows) // 45 + 1
        ps = por_hueco.get(a0, [])
        pesos = Counter()
        for o in ps:
            pesos[o['destino']] += o['size']
        pend = [o for o in ps if o['destino'] in ('pendiente-dato', 'viva-sin-hook')]
        if pend:
            clase = 'viva-sin-hook' if any(o['destino'] == 'viva-sin-hook' for o in pend) else 'pendiente-dato'
        elif pesos:
            clase = max(pesos, key=lambda k: (pesos[k], -orden.index(k)))
        else:
            clase = 'sin-referencia-real'
        hrows.append(dict(lote=lote, va=a0, fin=b0, bytes=b0 - a0 + 1, region=reg, clase=clase, piezas=len(ps), pendientes=len(pend),
                          cerrado=int(not pend),
                          detalle=' '.join(f'{k}:{pesos[k]}' for k in orden if pesos[k])))
    with open(out / 'cierre-p6.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['lote', 'va', 'fin', 'bytes', 'region', 'clase', 'piezas', 'pendientes', 'cerrado', 'bytes_por_destino'])
        for r in hrows:
            w.writerow(['H%d' % (r['lote']), '0x%08X' % r['va'], '0x%08X' % r['fin'], r['bytes'], r['region'], r['clase'], r['piezas'], r['pendientes'], r['cerrado'], r['detalle']])
    res = defaultdict(lambda: [0, 0, 0, 0])
    for r in hrows:
        k = (r['clase'], r['region'])
        res[k][0] += 1
        res[k][1] += r['bytes']
        res[k][2] += r['cerrado']
    with open(out / 'huecos-p6-resumen.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['clase_hueco', 'region', 'huecos', 'bytes', 'cerrados'])
        for (c, rg), v in sorted(res.items()):
            w.writerow([c, rg, v[0], v[1], v[2]])
    # --- plan.csv y plan-lotes.md: estado de los lotes H1..H6
    por_lote = defaultdict(list)
    for r in hrows:
        por_lote[r['lote']].append(r)
    est = {}
    for k, rs in por_lote.items():
        cer = sum(r['cerrado'] for r in rs)
        cl = Counter(r['clase'] for r in rs)
        est[f'H{k}'] = (f'clasificado: {cer} de {len(rs)} huecos cerrados',
                        'P6 estatico: ' + ', '.join(f'{c} {n}' for c, n in sorted(cl.items())) + f'; {len(rs) - cer} con piezas pendientes (cierre-p6.csv, huecos-p6.csv)')
    plan = out / 'plan.csv'
    with open(plan, encoding='utf-8-sig', newline='') as f:
        rd_ = csv.DictReader(f)
        campos = rd_.fieldnames
        filas = list(rd_)
    for r in filas:
        if r['lote'] in est and r['parte'] == 'P6':
            r['estado'] = est[r['lote']][0]
            base = r['nota'].split(' | P6 estatico')[0]
            r['nota'] = base + ' | ' + est[r['lote']][1]
    with open(plan, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, campos)
        w.writeheader()
        w.writerows(filas)
    pl = out / 'plan-lotes.md'
    lineas = pl.read_text(encoding='utf-8').split(chr(10))
    for n_, ln in enumerate(lineas):
        m = re.match(r'\| (H\d) \| P6 \|', ln)
        if m and m.group(1) in est:
            celdas = ln.split(' | ')
            celdas[-1] = est[m.group(1)][0] + ' |'
            lineas[n_] = ' | '.join(celdas)
    pl.write_text(chr(10).join(lineas), encoding='utf-8')

    tot = Counter()
    for o in pieces:
        tot[o['destino']] += 1
    print('piezas', len(pieces), dict(tot))
    print('huecos', len(hrows), 'cerrados', sum(r['cerrado'] for r in hrows))
    for (c, rg), v in sorted(res.items()):
        print(c, rg, v)


if __name__ == '__main__':
    main()
