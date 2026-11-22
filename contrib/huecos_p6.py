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
Destinos: reconstruida, citada-en-cpp, muerta-por-reemplazo, destructor-atexit, sin-referencia-real, crt-enlazar, pendiente-dato.
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

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / 'docs'


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

    # --- referencias (llamadas, saltos, inmediatos del .text y dwords de todo el EXE)
    refs = defaultdict(list)
    for x, y in sorted(funcs + [(p, q) for p, q, _, _ in holes]):
        for ins in cs.disasm(ex.rd(x, y - x + 1), x):
            m = ins.mnemonic
            if m == 'call' or m.startswith('j'):
                if ins.operands and ins.operands[0].type == X86_IMM:
                    refs[ins.operands[0].imm].append((ins.address, m))
            elif m in ('push', 'mov'):
                for op in ins.operands:
                    if op.type == X86_IMM and ex.t0 <= op.imm < ex.t1:
                        refs[op.imm].append((ins.address, 'imm'))
    raw = defaultdict(list)
    for nm, sva, vsz, rp, rsz in ex.secs:
        blob = ex.data[rp:rp + rsz]
        for off in range(0, len(blob) - 3):
            v = struct.unpack_from('<I', blob, off)[0]
            if ex.t0 <= v < ex.t1:
                raw[v].append(sva + off)

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
        o['live'] = live.get(o['s'], 'muerto' if o['ents'] else 'sin-ref')
        s = o['s']
        o['region'] = 'crt' if s >= 0x820000 else 'renderware' if s >= 0x750000 else 'juego'
        if o['hooked']:
            d = 'reconstruida'
        elif o['live'] == 'vivo' and o['region'] == 'crt':
            d = 'crt-enlazar'
        elif o['live'] == 'init':
            d = 'destructor-atexit'
        elif o['cited']:
            d = 'citada-en-cpp'
        elif o['live'] in ('datos', 'tabla', 'vivo'):
            d = 'pendiente-dato'
        elif o['live'] == 'muerto':
            d = 'muerta-por-reemplazo'
        else:
            d = 'sin-referencia-real'
        o['destino'] = d
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
        w.writerow(['va', 'fin', 'bytes', 'hueco', 'region', 'forma', 'destino', 'vida', 'enganchada', 'citada', 'entradas', 'origenes'])
        for o in pieces:
            w.writerow(['0x%08X' % o['s'], '0x%08X' % o['e'], o['size'], '0x%08X' % o['hole'], o['region'], o['forma'], o['destino'], o['live'],
                        int(o['hooked']), int(o['cited']), len(set(t for t, _, _ in o['ents'])), srcdesc(o)])
    por_hueco = defaultdict(list)
    for o in pieces:
        por_hueco[o['hole']].append(o)
    orden = ['pendiente-dato', 'crt-enlazar', 'reconstruida', 'citada-en-cpp', 'destructor-atexit', 'muerta-por-reemplazo', 'sin-referencia-real']
    hrows = []
    for a0, b0, cat, reg in sorted(holes):
        if not cat.startswith('código con ref'):
            continue
        lote = len(hrows) // 45 + 1
        ps = por_hueco.get(a0, [])
        pesos = Counter()
        for o in ps:
            pesos[o['destino']] += o['size']
        pend = [o for o in ps if o['destino'] == 'pendiente-dato']
        if pend:
            clase = 'pendiente-dato'
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
