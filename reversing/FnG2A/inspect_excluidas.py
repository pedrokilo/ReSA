#!/usr/bin/env python
"""Evidencia por inspeccion estatica del EXE para funciones excluidas (o thunks).

Uso (desde la raiz del repo; requiere `pip install capstone`):

  python -I reversing/FnG2A/inspect_excluidas.py <gta_sa.exe>                 genera el JSON
  python -I reversing/FnG2A/inspect_excluidas.py <gta_sa.exe> --check         revalida el JSON contra el EXE
  python -I reversing/FnG2A/inspect_excluidas.py <gta_sa.exe> --write-csv     ademas anade categoria/tratamiento/evidencia al CSV

Entrada: un CSV con columnas va,bytes,modulo,motivo (por defecto docs/JuegoExe/excluidas.csv).
Salida: reversing/FnG2A/excluidas-evidencia.json, una entrada por funcion.

La herramienta es generica: cada categoria es un plugin (clase Plugin) con
  match(ctx, fila)      -> True si la fila encaja en la categoria
  inspect(ctx, e)       -> rellena la entrada `e` con la evidencia (destinos, notas...)
  check(ctx, e)         -> lista de discrepancias al revalidar contra el EXE
Para los thunks de RenderWare (ImportThunks, DelayLoadThunks, QTML) basta anadir un
plugin nuevo (o reutilizar SaltoPuro, que ya resuelve `jmp [IAT]` con el nombre de la
importacion) y las filas en el CSV de entrada; el JSON, --check y el CSV no cambian.
Consulta README.md de esta carpeta.
"""
import argparse
import bisect
import csv
import datetime
import glob
import hashlib
import json
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT_JSON = ROOT / 'reversing/FnG2A/excluidas-evidencia.json'
JSON_REL = 'reversing/FnG2A/excluidas-evidencia.json'
IN_CSV = ROOT / 'docs/JuegoExe/excluidas.csv'
EXE_SHA256 = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
RW_LO, RW_HI = 0x750000, 0x81FFFF
HEX_MAX = 64

CATEGORIAS = [
    'no-es-funcion', 'proteccion-descartada', 'salto-puro',
    'usercall-cubierta', 'usercall-parcial', 'usercall-sin-llamadores', 'usercall-renderware', 'otra',
]


def hx(v):
    return f'0x{v:08X}'


def _int(s):
    return int(s, 16) if isinstance(s, str) else s


# --------------------------------------------------------------------------- contexto (EXE + tablas)

class Ctx:
    def __init__(self, exe_path, root=ROOT):
        self.root = Path(root)
        self.exe = Path(exe_path).read_bytes()
        self.sha = hashlib.sha256(self.exe).hexdigest()
        e = self.exe
        pe = struct.unpack_from('<I', e, 0x3C)[0]
        n, = struct.unpack_from('<H', e, pe + 6)
        opt, = struct.unpack_from('<H', e, pe + 20)
        self.base, = struct.unpack_from('<I', e, pe + 52)
        self.secs = []   # (nombre, va, vsize, raw_ptr, raw_size)
        o = pe + 24 + opt
        for _ in range(n):
            nm = e[o:o + 8].rstrip(b'\0').decode('ascii', 'replace')
            vs, va, rs, rp = struct.unpack_from('<IIII', e, o + 8)
            self.secs.append((nm, self.base + va, vs, rp, rs))
            o += 40
        self.text = self.secs[0]
        self._imports = None
        self._load_tables()
        self._disc = {}
        self._ref_index = None
        self._registry = None

    # -- memoria
    def sec_of(self, va):
        for s in self.secs:
            if s[1] <= va < s[1] + s[2]:
                return s
        return None

    def rd(self, va, n):
        s = self.sec_of(va)
        if not s:
            return b''
        off = s[3] + va - s[1]
        return self.exe[off:off + min(n, s[1] + s[2] - va)]

    def u32(self, va):
        b = self.rd(va, 4)
        return struct.unpack('<I', b)[0] if len(b) == 4 else None

    # -- imports (IAT va -> "dll!nombre")
    def iat_name(self, va):
        if self._imports is None:
            self._imports = {}
            try:
                e = self.exe
                pe = struct.unpack_from('<I', e, 0x3C)[0]
                rva, = struct.unpack_from('<I', e, pe + 24 + 96 + 8)   # directorio 1 (imports)
                s = self.sec_of(self.base + rva)
                p = self.base + rva
                while s:
                    ocf, _, _, name_rva, ft = struct.unpack('<IIIII', self.rd(p, 20))
                    if not name_rva:
                        break
                    dll = self.rd(self.base + name_rva, 64).split(b'\0')[0].decode('ascii', 'replace')
                    t, a = self.base + (ocf or ft), self.base + ft
                    while True:
                        ent = self.u32(t)
                        if not ent:
                            break
                        if ent & 0x80000000:
                            nm = f'#{ent & 0xFFFF}'
                        else:
                            nm = self.rd(self.base + ent + 2, 80).split(b'\0')[0].decode('ascii', 'replace')
                        self._imports[a] = f'{dll}!{nm}'
                        t += 4
                        a += 4
                    p += 20
            except Exception:
                pass
        return self._imports.get(va)

    # -- tablas del repo
    def _csv(self, rel):
        p = self.root / rel
        if not p.exists():
            return []
        with open(p, encoding='utf-8', newline='') as f:
            return list(csv.DictReader(f))

    def _load_tables(self):
        self.funcs = {}        # va -> dict(size, nombre, clase, origen)
        for rel, org in (('docs/JuegoExe/funciones.csv', 'juego'), ('docs/RenderWareExe/funciones.csv', 'renderware')):
            for r in self._csv(rel):
                va = _int(r['va'])
                self.funcs[va] = dict(size=int(r['bytes'] or 0), nombre=r.get('nombre') or r.get('nombre_catalogo') or '',
                                      clase=r.get('clase_modulo') or r.get('modulo') or '', origen=org,
                                      estado=r.get('estado', ''))
        self.hooks = {}        # va -> filas de hooks.csv
        for r in self._csv('docs/hooks.csv'):
            try:
                va = _int(r['address'])
            except Exception:
                continue
            self.hooks.setdefault(va, []).append(r)
        self.hooks_nombre = {}   # 'Clase::Fn' -> filas (incluye los virtuales, que tienen address 0x00000000)
        for rows in self.hooks.values():
            for r in rows:
                self.hooks_nombre.setdefault(f"{r['class']}::{r['fn_name'].split('-')[0]}", []).append(r)
        self.sin_catalogo = {_int(r['va']): r for r in self._csv('docs/JuegoExe/cubiertas-sin-catalogo.csv')}
        self.excluidas = {}
        for r in self._csv('docs/JuegoExe/excluidas.csv'):
            self.excluidas[_int(r['va'])] = r
        starts = set(self.funcs) | set(self.hooks) | set(self.excluidas)
        self.starts = sorted(starts)

    def registry(self):
        """address -> lista de rutas de evidencia del registro de validaciones."""
        if self._registry is None:
            self._registry = {}
            for p in sorted(glob.glob(str(self.root / 'reversing/validation-registry/parte-*.json'))):
                d = json.loads(Path(p).read_bytes())
                for f in d.get('functions', []):
                    h = f.get('hook', {})
                    try:
                        self._registry.setdefault(_int(h['address']), []).append(f.get('evidence', {}).get('path', ''))
                    except Exception:
                        pass
        return self._registry

    # -- funciones conocidas
    def func_containing(self, va):
        """Funcion que contiene `va` (incluidas las excluidas, que se resuelven despues): (inicio, dentro_del_cuerpo)."""
        i = bisect.bisect_right(self.starts, va) - 1
        if i < 0:
            return None, False
        s = self.starts[i]
        size = self.funcs.get(s, {}).get('size') or (int(self.excluidas[s]['bytes']) if s in self.excluidas else 0)
        return s, (va < s + size) if size else False

    def name_of(self, va):
        rows = self.hooks.get(va)
        if rows:
            r = rows[0]
            return f"{r['class']}::{r['fn_name']}"
        f = self.funcs.get(va)
        if f and f['nombre']:
            return f['nombre']
        if f:
            return f"{f['clase']} (sin nombre)"
        x = self.excluidas.get(va)
        if x:
            return f"{x['modulo']} (excluida)"
        return ''

    def hook_rows(self, va):
        rows = self.hooks.get(va, [])
        if not rows:
            f = self.funcs.get(va)
            if f and f['nombre']:
                rows = self.hooks_nombre.get(f['nombre'], [])
        return rows

    def hook_state(self, va):
        rows = [r for r in self.hook_rows(va) if r.get('reversed') == '1']
        if not rows:
            return 'sin-hook'
        return 'validada' if all(r.get('validated') == '1' for r in rows) else 'hook-sin-validar'

    # -- desensamblado
    def disasm(self, va, n):
        from capstone import Cs, CS_ARCH_X86, CS_MODE_32
        md = Cs(CS_ARCH_X86, CS_MODE_32)
        md.detail = False
        out = []
        for i in md.disasm(self.rd(va, n), va):
            out.append((i.address, i.size, i.mnemonic, i.op_str))
        return out

    def disasm_text(self, va, n):
        lines = []
        last = va
        for a, sz, m, o in self.disasm(va, n):
            lines.append(f'{hx(a)}: {(m + " " + o).strip()}')
            last = a + sz
        if last < va + n:
            lines.append(f'; sin decodificar {va + n - last} bytes desde {hx(last)} (datos o instruccion truncada)')
        return lines

    def insn_set(self, start, end):
        k = (start, end)
        if k not in self._disc:
            self._disc[k] = {a for a, _, _, _ in self.disasm(start, end - start)}
        return self._disc[k]

    # -- referencias a una direccion
    def _build_ref_index(self):
        e = self.exe
        nm, tva, tvs, trp, trs = self.text
        self._rel = {}
        seg = e[trp:trp + trs]
        for m in re.finditer(rb'[\xe8\xe9]', seg):
            p = m.start()
            if p + 5 > len(seg):
                continue
            rel, = struct.unpack_from('<i', seg, p + 1)
            site = tva + p
            tgt = (site + 5 + rel) & 0xFFFFFFFF
            self._rel.setdefault(tgt, []).append((site, 'call' if seg[p] == 0xE8 else 'jmp'))
        self._ref_index = True

    def refs_to(self, va, own=None):
        """Referencias directas (call/jmp rel32) y de direccion absoluta (datos o inmediatos) a `va`."""
        if self._ref_index is None:
            self._build_ref_index()
        calls = []
        for site, kind in self._rel.get(va, []):
            f, dentro = self.func_containing(site)
            cont = dentro or (f is not None and self.contiguo(f, site))
            anon = False
            if f is not None and not cont:
                a = self.anon_start(f, site)
                if a is not None:
                    f, anon, cont = a, True, True
            calls.append(dict(sitio=hx(site), tipo=kind, funcion=hx(f) if f is not None else None,
                              dentro_del_cuerpo=dentro, contiguo_desde_la_entrada=cont,
                              **({'funcion_no_listada_en_el_mapa': True} if anon else {})))
        data = []
        pat = struct.pack('<I', va)
        for nm, sva, vs, rp, rs in self.secs:
            blob = self.exe[rp:rp + min(vs, rs)]
            i = blob.find(pat)
            while i >= 0:
                site = sva + i
                item = dict(sitio=hx(site), seccion=nm)
                if nm == self.text[0]:
                    f, dentro = self.func_containing(site)
                    item['funcion'] = hx(f) if f is not None else None
                    item['tipo'] = 'inmediato-en-codigo'
                else:
                    item['tipo'] = 'dword-en-datos'
                    if site % 4 == 0:
                        item['vtable'] = self.vtable_guess(site)
                data.append(item)
                i = blob.find(pat, i + 1)
        return calls, data

    def contiguo(self, start, site):
        """True si desde `start` el desensamblado lineal llega a `site` como limite de instruccion, sin relleno
        (int3/nop de alineacion) ni otro inicio de funcion conocido por medio: el sitio es parte de la misma funcion
        aunque el mapa solo cuente hasta el primer ret."""
        key = ('c', start, site)
        if key in self._disc:
            return self._disc[key]
        ok = False
        for a, sz, m, o in self.disasm(start, site + 1 - start):
            if a != start and a in self.excluidas_o_funcs():
                break
            if m == 'int3':
                break
            if a == site:
                ok = True
                break
        self._disc[key] = ok
        return ok

    def anon_start(self, start, site):
        """Inicio de una funcion NO listada en las tablas que contiene `site`: el mapa de Ghidra omite funciones pequenas
        entre dos conocidas. Se detecta por relleno (nop/int3) tras un ret/jmp; devuelve None si no se encuentra."""
        key = ('a', start, site)
        if key in self._disc:
            return self._disc[key]
        res = None
        cur = start
        pad = False
        prev_term = False
        for a, sz, m, o in self.disasm(start, site + 1 - start):
            is_pad = m in ('int3', 'nop') or (m == 'xchg' and o in ('ax, ax',)) or (m == 'lea' and o.startswith('esi, [esi') or o.startswith('edi, [edi'))
            if is_pad:
                pad = True
            else:
                if pad and prev_term:
                    cur = a
                pad = False
                prev_term = m.startswith('ret') or m == 'jmp'
            if a >= site:
                break
        if cur != start:
            res = cur
        self._disc[key] = res
        return res

    def excluidas_o_funcs(self):
        if not hasattr(self, '_sset'):
            self._sset = set(self.starts)
        return self._sset

    def vtable_guess(self, slot_va):
        """Estimacion: retrocede mientras los dwords previos sean punteros a codigo (.text)."""
        nm, tva, tvs, _, _ = self.text
        a = slot_va
        while True:
            v = self.u32(a - 4)
            if v is None or not (tva <= v < tva + tvs):
                break
            a -= 4
        return dict(inicio_estimado=hx(a), ranura=(slot_va - a) // 4)


# --------------------------------------------------------------------------- plugins

class Plugin:
    nombre = ''
    tratamiento = ''

    def match(self, ctx, r):
        return False

    def inspect(self, ctx, e):
        pass

    def check(self, ctx, e):
        return []


class ProteccionDescartada(Plugin):
    nombre = 'proteccion-descartada'
    tratamiento = ('Se descarta: es codigo del protector/anti-manipulacion del EXE original; un ejecutable propio no lo necesita '
                   '(no se reconstruye ni se engancha)')

    def match(self, ctx, r):
        return r['modulo'] == 'ExeProtectionStub' or _int(r['va']) == 0x746800

    def inspect(self, ctx, e):
        va, n = _int(e['va']), e['bytes']
        ins = ctx.disasm(va, n)
        marcas = []
        txt = ' '.join(f'{m} {o}' for _, _, m, o in ins)
        for k in ('pushal', 'popal', 'pushfd', 'popfd', 'rdtsc', 'cpuid', 'stc', 'clc', 'xlatb', 'int ', 'lock', 'bound', 'salc'):
            if k in txt:
                marcas.append(k.strip())
        # referencias a la seccion del protector (_TEXT_HA / _rwcseg ...) y a direcciones fuera del .text
        sec_prot = set()
        for x in re.findall(r'0x([0-9a-f]{6,8})', txt):
            sc = ctx.sec_of(int(x, 16))
            if sc and sc[0] not in ('.text', '.data', '.rdata'):
                sec_prot.add(sc[0])
        sec_prot = sorted(sec_prot)
        if sec_prot:
            marcas.append('referencias a secciones del protector: ' + ', '.join(sec_prot))
        for _, _, m, o in ins:
            mm = re.match(r'(?:\w+), 0x([0-9a-f]+)$', o) if m == 'mov' else None
            if mm and va <= int(mm.group(1), 16) < va + n:
                marcas.append('autodireccion como constante (comprobacion de integridad del propio codigo)')
                break
        if any(m == 'call' and re.match(r'dword ptr \[0x[0-9a-f]+\]$', o) for _, _, m, o in ins) and any(
                m == 'mov' and re.search(r', 0x[89a-f][0-9a-f]{7}$', o) for _, _, m, o in ins):
            marcas.append('llamada indirecta con constantes magicas de 32 bits (rutina de verificacion)')
        sin_ret = not any(m.startswith('ret') for _, _, m, _ in ins)
        calls, data = ctx.refs_to(va)
        fuera = [x['sitio'] for x in calls + data if not (0x402000 <= _int(x['sitio']) < 0x404000)]
        e['referencias_fuera_del_bloque_protector_0x402000_0x403FFF'] = fuera
        e['marcadores'] = marcas
        e['contiene_ret'] = not sin_ret
        e['referencias_directas'] = calls
        e['referencias_datos'] = data
        e['notas'] = ['pertenece al bloque de proteccion/arranque del EXE (rango 0x402000-0x403FFF o 0x746800 en VideoPlayer); '
                      'usa pushal/popal y registros como convencion de paso; no tiene contraparte en el juego en C++']

    def check(self, ctx, e):
        return []


class SaltoPuro(Plugin):
    nombre = 'salto-puro'
    tratamiento = ('Cerrada por inspeccion: un solo salto a una funcion ya reconstruida/conocida; la logica vive en el destino '
                   '(no se engancha: el hook del destino ya la cubre)')

    def _decode(self, ctx, va, n):
        b = ctx.rd(va, n)
        if n == 5 and b[:1] == b'\xe9':
            return 'E9', (va + 5 + struct.unpack('<i', b[1:5])[0]) & 0xFFFFFFFF, None
        if n == 2 and b[:1] == b'\xeb':
            return 'EB', (va + 2 + struct.unpack('<b', b[1:2])[0]) & 0xFFFFFFFF, None
        if n == 6 and b[:2] == b'\xff\x25':
            slot = struct.unpack('<I', b[2:6])[0]
            return 'FF25', ctx.u32(slot), slot
        return None, None, None

    def match(self, ctx, r):
        return self._decode(ctx, _int(r['va']), int(r['bytes']))[0] is not None

    def inspect(self, ctx, e):
        va, n = _int(e['va']), e['bytes']
        op, dest, slot = self._decode(ctx, va, n)
        d = dict(opcode=op, destino=hx(dest) if dest else None)
        if slot is not None:
            d['ranura_iat'] = hx(slot)
            d['importacion'] = ctx.iat_name(slot)
        if dest is not None:
            d['destino_nombre'] = ctx.name_of(dest)
            rows = ctx.hook_rows(dest)
            d['destino_hooks'] = [dict(clase=r['class'], fn=r['fn_name'], reversed=r['reversed'], validated=r['validated']) for r in rows]
            f = ctx.funcs.get(dest)
            d['destino_en_mapa'] = bool(f)
            d['destino_es_inicio_de_funcion'] = bool(f or rows or dest in ctx.excluidas)
            d['destino_estado'] = ctx.hook_state(dest) if rows else ('mapa:' + f['estado'] if f else ('importacion' if slot is not None and d.get('importacion') else 'desconocido'))
            reg = ctx.registry().get(dest)
            if reg:
                d['destino_registro'] = sorted(set(reg))[:3]
        e['salto'] = d
        e['destinos'] = [hx(dest)] if dest else []
        calls, data = ctx.refs_to(va)
        e['referencias_directas'] = calls
        e['referencias_datos'] = data
        vt = [x['vtable'] for x in data if x.get('vtable')]
        if vt:
            e['vtable'] = vt
        ok = d.get('destino_es_inicio_de_funcion') or d.get('importacion')
        e['notas'] = [('el destino es un inicio de funcion conocido' if ok else 'ATENCION: el destino no se reconoce como inicio de funcion')]
        e['comprobacion_ok'] = bool(ok)

    def check(self, ctx, e):
        errs = []
        va, n = _int(e['va']), e['bytes']
        op, dest, slot = self._decode(ctx, va, n)
        if op is None:
            return ['ya no decodifica como salto puro']
        if (hx(dest) if dest else None) != e['salto']['destino']:
            errs.append(f"destino distinto: {hx(dest) if dest else None} != {e['salto']['destino']}")
        return errs


class NoEsFuncion(Plugin):
    nombre = 'no-es-funcion'
    tratamiento = ('Cerrada por inspeccion: no es una funcion independiente (sale del recuento de funciones a reconstruir); '
                   'su codigo ya esta en el padre indicado')

    def match(self, ctx, r):
        return 'no es una funcion' in r['motivo'].lower() and 'jmp' not in r['motivo'].lower()

    def inspect(self, ctx, e):
        va, n = _int(e['va']), e['bytes']
        i = bisect.bisect_left(ctx.starts, va) - 1
        padre = None
        while i >= 0:
            s = ctx.starts[i]
            if s in ctx.excluidas and s not in ctx.funcs and s not in ctx.hooks:
                i -= 1
                continue
            padre = s
            break
        ins = ctx.disasm(va, n)
        rets = [hx(a) for a, _, m, _ in ins if m.startswith('ret')]
        ebp_padre = any(re.search(r'ebp [-+] 0x', o) for _, _, _, o in ins)
        calls, data = ctx.refs_to(va)
        ext = [c for c in calls if c['funcion'] != hx(padre)] if padre is not None else calls
        pinfo = ctx.funcs.get(padre, {})
        e['padre'] = dict(va=hx(padre) if padre is not None else None, nombre=ctx.name_of(padre) if padre is not None else '',
                          bytes_en_mapa=pinfo.get('size'),
                          distancia_al_padre=(va - padre) if padre is not None else None,
                          fin_padre_en_mapa=hx(padre + pinfo['size']) if pinfo.get('size') else None,
                          estado_padre=ctx.hook_state(padre) if padre is not None else None)
        pr = ctx.registry().get(padre) if padre is not None else None
        if pr:
            e['padre']['registro'] = sorted(set(pr))[:3]
        e['sin_prologue_estandar'] = not (ctx.rd(va, 3)[:3] in (b'\x55\x8b\xec', b'\x55\x89\xe5'))
        e['rets_en_el_bloque'] = rets
        e['accede_a_marco_del_padre_por_ebp'] = ebp_padre
        e['referencias_directas'] = calls
        e['referencias_directas_externas_al_padre'] = ext
        e['referencias_datos'] = data
        e['destinos'] = [hx(padre)] if padre is not None else []
        # continuidad: el siguiente bloque conocido empieza donde acaba este (el bloque esta en el hueco del padre)
        sig = ctx.starts[bisect.bisect_right(ctx.starts, va)] if bisect.bisect_right(ctx.starts, va) < len(ctx.starts) else None
        e['siguiente_funcion'] = hx(sig) if sig else None
        pins = ctx.disasm(padre, sig - padre) if padre is not None and sig else []
        objetivo = f'0x{va:x}'
        saltos = [f'{hx(a)}: {(m + " " + o).strip()}' for a, _, m, o in pins if m.startswith('j') and o == objetivo]
        e['va_es_limite_de_instruccion_del_padre'] = any(a == va for a, _, _, _ in pins)
        e['saltos_internos_del_padre_al_bloque'] = saltos
        prev = [x for x in pins if x[0] < va]
        cont = bool(prev) and not (prev[-1][2].startswith('ret') or prev[-1][2] == 'jmp') and prev[-1][0] + prev[-1][1] == va
        e['llega_por_continuacion_desde_el_padre'] = cont
        notas = []
        if cont:
            notas.append('el desensamblado lineal del padre llega a este bloque sin ret/jmp previo: es la continuacion del cuerpo del padre')
        if saltos:
            notas.append('el propio padre salta al bloque (rama interna): ' + '; '.join(saltos))
        if not ext:
            notas.append('ninguna llamada/salto directo desde fuera del padre: no es entrada independiente')
        if ebp_padre and not rets:
            notas.append('usa [ebp+-N] del marco del padre y no tiene ret propio: funclet/bloque interior (catch o rama del padre)')
        elif ebp_padre:
            notas.append('usa [ebp+-N] del marco del padre: funclet de catch(...) que devuelve la direccion de continuacion en el padre')
        if data:
            notas.append('referenciada desde datos (tabla de excepciones de MSVC o tabla de saltos del padre)')
        e['notas'] = notas
        e['comprobacion_ok'] = padre is not None and not ext and bool(saltos or data or ebp_padre or cont)

    def check(self, ctx, e):
        errs = []
        va = _int(e['va'])
        calls, data = ctx.refs_to(va)
        if sorted(c['sitio'] for c in calls) != sorted(c['sitio'] for c in e['referencias_directas']):
            errs.append('referencias directas distintas')
        if sorted(c['sitio'] for c in data) != sorted(c['sitio'] for c in e['referencias_datos']):
            errs.append('referencias de datos distintas')
        padre = _int(e['padre']['va'])
        sig = _int(e['siguiente_funcion'])
        objetivo = f'0x{va:x}'
        saltos = [f'{hx(a)}: {(m + " " + o).strip()}' for a, _, m, o in ctx.disasm(padre, sig - padre) if m.startswith('j') and o == objetivo]
        if saltos != e['saltos_internos_del_padre_al_bloque']:
            errs.append('saltos internos del padre distintos')
        return errs


def es_renderware(r):
    va = _int(r['va'])
    m = r['modulo'].lower()
    return RW_LO <= va <= RW_HI or m.startswith('renderware') or 'rwtexdict' in m or 'd3d9' in m


class Usercall(Plugin):
    """Cubre __usercall: la categoria final (cubierta/parcial/sin-llamadores/renderware) se decide en `resolver`."""
    nombre = 'usercall'
    tratamiento = ''

    def match(self, ctx, r):
        t = r['motivo'].lower()
        return '__usercall' in t or 'registros' in t or es_renderware(r)

    def inspect(self, ctx, e):
        calls, data = ctx.refs_to(_int(e['va']))
        e['referencias_directas'] = calls
        e['referencias_datos'] = data


# --------------------------------------------------------------------------- orquestacion

PLUGINS = [ProteccionDescartada(), SaltoPuro(), NoEsFuncion(), Usercall()]


def leer_filas(path):
    with open(path, encoding='utf-8', newline='') as f:
        return list(csv.DictReader(f))


def inspeccionar(ctx, filas):
    entradas = []
    for r in filas:
        va, n = _int(r['va']), int(r['bytes'])
        body = ctx.rd(va, n)
        e = dict(va=hx(va), bytes=n, modulo=r['modulo'], motivo_original=r['motivo'])
        e['hex'] = body[:HEX_MAX].hex()
        e['hex_completo'] = n <= HEX_MAX
        e['sha256_cuerpo'] = hashlib.sha256(body).hexdigest()
        e['desensamblado'] = ctx.disasm_text(va, n)
        e['plugin'] = None
        for p in PLUGINS:
            if p.match(ctx, r):
                e['plugin'] = p.nombre
                p.inspect(ctx, e)
                break
        entradas.append(e)
    resolver(ctx, entradas)
    return entradas


def resolver(ctx, entradas):
    """Asigna categoria y tratamiento. Los __usercall se resuelven por punto fijo sobre sus llamadoras."""
    by = {_int(e['va']): e for e in entradas}
    for e in entradas:
        pl = e['plugin']
        if pl in ('proteccion-descartada', 'salto-puro', 'no-es-funcion'):
            e['categoria'] = pl
            e['tratamiento'] = next(p.tratamiento for p in PLUGINS if p.nombre == pl)
            if pl == 'salto-puro' and not e.get('comprobacion_ok'):
                e['categoria'] = 'otra'
                e['tratamiento'] = 'Revisar: el salto no apunta a un inicio de funcion conocido'
            if pl == 'no-es-funcion' and not e.get('comprobacion_ok'):
                e['categoria'] = 'otra'
                e['tratamiento'] = 'Revisar: hay referencias externas o no se encontro el padre'
        elif pl is None:
            e['categoria'] = 'otra'
            e['tratamiento'] = 'Sin plugin que la reconozca: revisar a mano'
        else:
            e['categoria'] = None

    def resuelta_a_padre(f):
        """Si la llamadora es un fragmento/funclet excluido, vale el estado de su padre."""
        x = by.get(f)
        if x and x['categoria'] == 'no-es-funcion' and x['padre']['va']:
            return _int(x['padre']['va'])
        return f

    usercalls = [e for e in entradas if e['plugin'] == 'usercall']
    nodos = {}   # va -> dict(llamadoras, datos, anonima) para excluidas-usercall y funciones no listadas

    def estado_de(f, c):
        if f is None:
            return dict(estado='sin-funcion', validated=None)
        if c.get('funcion_no_listada_en_el_mapa'):
            expandir(f)
            return dict(estado='no-listada-en-el-mapa', validated=None, nota='funcion pequena que el mapa omite; se resuelve por sus propias llamadoras')
        f2 = resuelta_a_padre(f)
        x = by.get(f2)
        if x is not None:
            if x['plugin'] == 'usercall':
                return dict(estado='excluida-usercall', validated=None, nota=f'llamadora excluida {hx(f2)}')
            return dict(estado='excluida-' + str(x['categoria']), validated=None)
        st = ctx.hook_state(f2)
        d = dict(estado=st, validated=1 if st == 'validada' else 0)
        if f2 != f:
            d['padre'] = hx(f2)
        if st == 'sin-hook' and f2 in ctx.sin_catalogo:
            d['estado'] = 'cubierta-sin-catalogo'
        if st != 'sin-hook' or f2 in ctx.sin_catalogo:
            reg = ctx.registry().get(f2)
            if reg:
                d['registro'] = sorted(set(reg))[:3]
        return d

    def marcar(calls, va):
        out = []
        for c in calls:
            c = dict(c)
            if c.get('funcion') == hx(va) and c.get('contiguo_desde_la_entrada') and not c.get('funcion_no_listada_en_el_mapa'):
                c['estado'] = 'recursiva'
            else:
                f = _int(c['funcion']) if c.get('funcion') else None
                c['nombre'] = ('sub_%08X (no listada en el mapa)' % f) if c.get('funcion_no_listada_en_el_mapa') else (ctx.name_of(f) if f is not None else '')
                c.update(estado_de(f, c))
            out.append(c)
        return out

    def marcar_datos(data):
        out = []
        for d in data:
            d = dict(d)
            if d.get('tipo') == 'inmediato-en-codigo' and d.get('funcion'):
                f = _int(d['funcion'])
                d['nombre'] = ctx.name_of(f)
                d.update(estado_de(f, dict(d)))
            else:
                d['estado'] = 'dato-sin-resolver'
            out.append(d)
        return out

    def expandir(va):
        if va in nodos or va in by:
            return
        nodos[va] = dict(llamadoras=[], datos=[], anonima=True)
        calls, data = ctx.refs_to(va)
        nodos[va]['llamadoras'] = marcar(calls, va)
        nodos[va]['datos'] = marcar_datos(data)

    for e in usercalls:
        calls, data = e['referencias_directas'], e['referencias_datos']
        e['llamadoras'] = marcar(calls, _int(e['va']))
        e['referencias_datos'] = marcar_datos(data)
        nodos[_int(e['va'])] = dict(llamadoras=e['llamadoras'], datos=e['referencias_datos'], anonima=False)
    cubierta = {v: False for v in nodos}
    changed = True
    while changed:
        changed = False
        for v, nd in nodos.items():
            if cubierta[v]:
                continue
            calls = [c for c in nd['llamadoras'] if c.get('estado') != 'recursiva']
            if not calls and not nd['datos']:
                ok = nd['anonima']   # funcion no listada y sin ninguna referencia: nunca se ejecuta
            else:
                ok = True
                for c in calls + nd['datos']:
                    st = c.get('estado')
                    if st == 'validada':
                        continue
                    if st in ('excluida-usercall', 'no-listada-en-el-mapa') and cubierta.get(_int(c['funcion'])):
                        continue
                    ok = False
            if ok:
                cubierta[v] = True
                changed = True
    e_nodos = {hx(v): dict(anonima=nd['anonima'], cubierta=cubierta[v], llamadoras=nd['llamadoras'], referencias_datos=nd['datos'])
               for v, nd in nodos.items() if nd['anonima']}
    resolver.funciones_no_listadas = e_nodos
    for e in usercalls:
        v = _int(e['va'])
        ext = [c for c in e['llamadoras'] if c.get('estado') != 'recursiva']
        v = _int(e['va'])
        rw = es_renderware(dict(va=e['va'], modulo=e['modulo']))
        if rw:
            cat = 'usercall-renderware'
            tr = 'Pasa al plan de validacion de RenderWare (docs/RenderWareExe/plan-validacion.md); no se trabaja aqui'
        elif cubierta[v]:
            cat = 'usercall-cubierta'
            tr = ('Cerrada por inspeccion: todas las llamadoras directas estan reconstruidas en C++ y validadas; '
                  'la logica ya la ejecuta C++ a traves de ellas (sin hook propio)')
        elif not ext and not e['referencias_datos']:
            cat = 'usercall-sin-llamadores'
            tr = 'Sin llamadores directos ni referencias: posible codigo muerto; requiere confirmar (ver informe del lote G2A)'
        else:
            cat = 'usercall-parcial'
            tr = 'Parcial: hay llamadoras sin validar, sin resolver o referencias de direccion; requiere trabajo (ver informe del lote G2A)'
        e['categoria'] = cat
        e['tratamiento'] = tr
        e['cubierta_por_llamadoras_validadas'] = bool(cubierta[v])
    for e in entradas:
        # filas relacionadas de hooks.csv
        rel = {_int(e['va'])}
        rel.update(_int(x) for x in e.get('destinos', []))
        rel.update(_int(c['funcion']) for c in e.get('llamadoras', []) if c.get('funcion'))
        rows = []
        for a in sorted(rel):
            for r in ctx.hook_rows(a):
                rows.append(dict(address=r['address'], clase=r['class'], fn=r['fn_name'], reversed=r['reversed'], validated=r['validated']))
        e['hooks_csv_relacionadas'] = rows
        e['validada_por_inspeccion'] = e['categoria'] in ('no-es-funcion', 'proteccion-descartada', 'salto-puro', 'usercall-cubierta')
        e['criterio_validacion'] = CRITERIO.get(e['categoria'], '')


CRITERIO = {
    'no-es-funcion': 'sin referencias directas externas al padre + padre conocido que contiene el bloque',
    'proteccion-descartada': 'bytes/desensamblado muestran codigo del protector; no se reconstruye',
    'salto-puro': 'bytes exactos del salto + destino calculado = inicio de funcion conocida',
    'usercall-cubierta': 'todas las llamadoras directas son funciones reconstruidas con validated=1 (o excluidas ya cubiertas)',
}


def documento(ctx, entradas, entrada_csv):
    cuenta = {}
    for e in entradas:
        cuenta[e['categoria']] = cuenta.get(e['categoria'], 0) + 1
    return dict(
        generado=datetime.date.today().isoformat(),
        exe_sha256=ctx.sha,
        entrada=entrada_csv,
        total=len(entradas),
        por_categoria=dict(sorted(cuenta.items())),
        funciones_no_listadas_en_el_mapa=getattr(resolver, 'funciones_no_listadas', {}),
        funciones=entradas,
    )


def cmd_check(ctx, doc_path):
    doc = json.loads(Path(doc_path).read_text(encoding='utf-8'))
    errs = []
    if ctx.sha != doc['exe_sha256']:
        errs.append(f"SHA256 del EXE distinto: {ctx.sha} != {doc['exe_sha256']}")
    csv_rows = {r['va']: r for r in leer_filas(IN_CSV)} if IN_CSV.exists() else {}
    pl = {p.nombre: p for p in PLUGINS}
    for e in doc['funciones']:
        va, n = _int(e['va']), e['bytes']
        body = ctx.rd(va, n)
        tag = e['va']
        if len(body) != n:
            errs.append(f'{tag}: fuera de la imagen')
            continue
        if body[:HEX_MAX].hex() != e['hex']:
            errs.append(f'{tag}: bytes iniciales distintos')
        if hashlib.sha256(body).hexdigest() != e['sha256_cuerpo']:
            errs.append(f'{tag}: sha256 del cuerpo distinto')
        if ctx.disasm_text(va, n) != e['desensamblado']:
            errs.append(f'{tag}: desensamblado distinto')
        p = pl.get(e['plugin'])
        if p:
            for m in p.check(ctx, e):
                errs.append(f'{tag}: {m}')
        if e['plugin'] == 'usercall':
            calls, data = ctx.refs_to(va)
            a = sorted(c['sitio'] for c in calls)
            b = sorted(c['sitio'] for c in e['referencias_directas'])
            if a != b:
                errs.append(f'{tag}: llamadoras directas distintas')
            if sorted(c['sitio'] for c in data) != sorted(c['sitio'] for c in e['referencias_datos']):
                errs.append(f'{tag}: referencias de datos distintas')
        if csv_rows and tag in csv_rows and 'categoria' in csv_rows[tag] and csv_rows[tag]['categoria'] != e['categoria']:
            errs.append(f"{tag}: categoria del CSV ({csv_rows[tag]['categoria']}) != JSON ({e['categoria']})")
    if csv_rows and set(csv_rows) != {e['va'] for e in doc['funciones']}:
        errs.append('el CSV y el JSON no contienen las mismas direcciones')
    # cuadre de cifras
    cuenta = {}
    for e in doc['funciones']:
        cuenta[e['categoria']] = cuenta.get(e['categoria'], 0) + 1
    if dict(sorted(cuenta.items())) != doc['por_categoria']:
        errs.append('el resumen por_categoria no cuadra con las entradas')
    for m in errs:
        print('FALLO', m)
    print(f"{'OK' if not errs else 'ERRORES'}: {len(doc['funciones'])} entradas revisadas, EXE {ctx.sha[:16]}..., "
          f"{len(errs)} discrepancias; por categoria: {doc['por_categoria']}")
    return 0 if not errs else 1


def escribir_csv(path, filas, entradas):
    by = {e['va']: e for e in entradas}
    cols = ['va', 'bytes', 'modulo', 'motivo', 'categoria', 'tratamiento', 'evidencia']
    with open(path, 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(cols)
        for r in filas:
            e = by[r['va']]
            w.writerow([r['va'], r['bytes'], r['modulo'], r['motivo'], e['categoria'], e['tratamiento'], f"{JSON_REL}#{e['va']}"])


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('exe')
    ap.add_argument('--input', default=str(IN_CSV))
    ap.add_argument('--out', default=str(OUT_JSON))
    ap.add_argument('--check', action='store_true')
    ap.add_argument('--write-csv', action='store_true')
    a = ap.parse_args(argv)
    ctx = Ctx(a.exe)
    if a.check:
        return cmd_check(ctx, a.out)
    if ctx.sha != EXE_SHA256:
        print(f'AVISO: el SHA256 del EXE ({ctx.sha}) no es el de referencia ({EXE_SHA256})', file=sys.stderr)
    filas = leer_filas(a.input)
    entradas = inspeccionar(ctx, filas)
    doc = documento(ctx, entradas, str(Path(a.input).resolve().relative_to(ROOT)).replace('\\', '/'))
    Path(a.out).write_text(json.dumps(doc, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    if a.write_csv:
        escribir_csv(a.input, filas, entradas)
    print(json.dumps(doc['por_categoria'], ensure_ascii=False))
    return 0


if __name__ == '__main__':
    sys.exit(main())
