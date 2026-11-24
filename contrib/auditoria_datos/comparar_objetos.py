"""Comparacion dinamica 'dato en dos sitios' en el ejecutable independiente (rama datos-g6g8).

Para cada objeto propio de source/game_sa/DatosExe (que NO vive en la direccion del EXE original) lee en un
proceso vivo los bytes del objeto (direccion resuelta con el .map de enlace) y los de la direccion original
(0x858000-0xCB0FFF, que en el independiente es la imagen incrustada + lo que escribe el codigo por literal).
Si difieren, el valor se guarda en dos sitios: alguien escribe por un lado y lee por el otro (o nadie lee uno).

Uso: python -I comparar_objetos.py --map <exe.map> --repo <raiz> --pid <pid> --salida <dir> [--t 40,80,110]
(solo toca el pid dado; solo lee memoria).  Salida: instantaneas JSON y diferencias.csv.
"""
import argparse, csv, ctypes, json, os, re, struct, sys, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ctypes import wintypes

PRIM = {'bool': 1, 'char': 1, 'int8': 1, 'uint8': 1, 'int8_t': 1, 'uint8_t': 1, 'unsigned char': 1, 'signed char': 1,
        'int16': 2, 'uint16': 2, 'int16_t': 2, 'uint16_t': 2, 'short': 2, 'unsigned short': 2, 'wchar_t': 2,
        'int32': 4, 'uint32': 4, 'int32_t': 4, 'uint32_t': 4, 'int': 4, 'unsigned int': 4, 'unsigned': 4, 'long': 4,
        'unsigned long': 4, 'float': 4, 'uintptr_t': 4, 'size_t': 4, 'DWORD': 4, 'HANDLE': 4, 'RwInt32': 4, 'RwBool': 4,
        'RwUInt32': 4, 'RwReal': 4, 'int64': 8, 'uint64': 8, 'double': 8, 'int64_t': 8, 'uint64_t': 8}

DECL = re.compile(r'^\s*(?:extern\s+)?(?:const\s+)?(?:inline\s+)?(.+?)\s+(\w+)((?:\[[^\]]*\])*)\s*(?:\{[^;]*\}|=[^;]*)?;\s*//\s*0x([0-9A-Fa-f]{6,8})')
NS = re.compile(r'namespace\s+DatosExe::(\w+)')


def tam_tipo(t, arr):
    t = t.replace('const ', '').replace('volatile ', '').strip()
    n = None
    if t.endswith('*'):
        n = 4
    elif t in PRIM:
        n = PRIM[t]
    if n is None:
        return None
    for d in re.findall(r'\[([^\]]*)\]', arr):
        if d.strip().isdigit():
            n *= int(d)
        else:
            return None
    return n


def leer_decls(repo):
    base = os.path.join(repo, 'source', 'game_sa', 'DatosExe')
    out = {}
    for f in sorted(os.listdir(base)):
        if not f.endswith(('.h', '.cpp')):
            continue
        ns = None
        for line in open(os.path.join(base, f), encoding='utf-8', errors='replace'):
            m = NS.match(line)
            if m:
                ns = m.group(1)
            m = DECL.match(line)
            if m and ns and '(' not in m.group(1):
                t, n, arr, a = m.groups()
                out.setdefault((ns, n), dict(ns=ns, name=n, tipo=t, arr=arr, va=int(a, 16), tam=tam_tipo(t, arr), file=f))
    return out


def leer_mapa(path):
    sym = {}
    alld = []
    pat = re.compile(r'^\s*([0-9a-f]{4}):([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})\s+(f\s+)?(?:i\s+)?(\S+)')
    for line in open(path, encoding='utf-8', errors='replace'):
        m = pat.match(line)
        if not m:
            continue
        seg, off, name, addr, isf, obj = m.groups()
        if isf:
            continue
        a = int(addr, 16)
        if a >= 0x1000000:   # solo datos propios del exe (fuera de la reserva 0x401000-0xCB1000)
            alld.append(a)
        mm = re.match(r'\?(\w+)@(\w+)@DatosExe@@3', name)
        if mm:
            sym[(mm.group(2), mm.group(1))] = a
    alld = sorted(set(alld))
    return sym, alld


class Mem:
    def __init__(s, pid):
        s.k = ctypes.WinDLL('kernel32', use_last_error=True)
        s.k.OpenProcess.restype = wintypes.HANDLE
        s.k.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
        s.h = s.k.OpenProcess(0x10 | 0x400, False, pid)   # solo lectura
        if not s.h:
            raise SystemExit('no se pudo abrir el pid %d' % pid)

    def base_modulo(s, nombre):
        ps = ctypes.WinDLL('psapi.dll', use_last_error=True)
        ps.EnumProcessModulesEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.DWORD, ctypes.POINTER(wintypes.DWORD), wintypes.DWORD]
        ps.GetModuleBaseNameW.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_wchar_p, wintypes.DWORD]
        mods = (ctypes.c_void_p * 1024)()
        need = wintypes.DWORD()
        ps.EnumProcessModulesEx(s.h, mods, ctypes.sizeof(mods), ctypes.byref(need), 3)
        buf = ctypes.create_unicode_buffer(260)
        for i in range(need.value // ctypes.sizeof(ctypes.c_void_p)):
            ps.GetModuleBaseNameW(s.h, mods[i], buf, 260)
            if buf.value.lower() == nombre.lower():
                return mods[i]
        return None

    def rd(s, a, n):
        b = ctypes.create_string_buffer(n)
        r = ctypes.c_size_t()
        ok = s.k.ReadProcessMemory(s.h, ctypes.c_void_p(a), b, n, ctypes.byref(r))
        return b.raw[:r.value] if ok or r.value else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--map', default=None)
    ap.add_argument('--pdb', default=None, help='PDB (con --bin y --modulo): tamanos exactos; direccion = base del modulo en el proceso + RVA')
    ap.add_argument('--bin', default=None)
    ap.add_argument('--modulo', default=None, help='nombre del modulo (gta_reversed.asi o gta_sa_standalone.exe)')
    ap.add_argument('--repo', required=True)
    ap.add_argument('--pid', type=int, required=True)
    ap.add_argument('--salida', required=True)
    ap.add_argument('--t', default='40,80,110')
    ap.add_argument('--plan', default=None, help='plan-objetos.csv: ademas vuelca la memoria de las direcciones de G6-G8 (va_dump.json)')
    a = ap.parse_args()
    os.makedirs(a.salida, exist_ok=True)
    dec = leer_decls(a.repo)
    import bisect
    m = Mem(a.pid)
    objs = []
    if a.pdb:
        import simbolos_pdb
        base_pdb = 0x10000000
        pd = simbolos_pdb.enumerar(a.pdb, a.bin, base_pdb)
        mb = m.base_modulo(a.modulo)
        if not mb:
            raise SystemExit('modulo %s no encontrado en el pid' % a.modulo)
        print('base de %s en el proceso: 0x%X' % (a.modulo, mb), flush=True)
        for k, d in dec.items():
            r = pd.get('DatosExe::%s::%s' % (k[0], k[1]))
            if r is None:
                continue
            objs.append(dict(d, dir=mb + r[0], tam=min(r[1] or 4, 0x10000), exacto=True))
    else:
        sym, alld = leer_mapa(a.map)
        for k, d in dec.items():
            ad = sym.get(k)
            if ad is None:
                continue
            i = bisect.bisect_right(alld, ad)
            gap = (alld[i] - ad) if i < len(alld) else 64
            tam = d['tam'] if d['tam'] else min(gap, 256)
            tam = min(tam, 0x10000)
            objs.append(dict(d, dir=ad, tam=tam, exacto=bool(d['tam'])))
    print('objetos con direccion: %d de %d declarados' % (len(objs), len(dec)), flush=True)
    t0 = time.time()
    snaps = []
    for ts in [float(x) for x in a.t.split(',')]:
        while time.time() - t0 < ts:
            time.sleep(0.5)
        filas = {}
        for o in objs:
            ob = m.rd(o['dir'], o['tam'])
            mem = m.rd(o['va'], o['tam'])
            if ob is None or mem is None or len(ob) != len(mem):
                filas[o['name']] = None
            else:
                filas[o['name']] = (ob.hex(), mem.hex())
        snaps.append((ts, filas))
        nd = sum(1 for v in filas.values() if v and v[0] != v[1])
        print('t=%.0f: %d objetos leidos, %d difieren' % (ts, sum(1 for v in filas.values() if v), nd), flush=True)
    with open(os.path.join(a.salida, 'diferencias.csv'), 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f)
        w.writerow(['va', 'ns', 'nombre', 'tipo', 'tam', 'exacto', 'archivo', 'dir_obj'] + ['dif_t%d' % s[0] for s in snaps] + ['obj_final', 'mem_final'])
        for o in sorted(objs, key=lambda x: x['va']):
            fl = [s[1].get(o['name']) for s in snaps]
            difs = [(1 if v and v[0] != v[1] else 0) if v else -1 for v in fl]
            last = next((v for v in reversed(fl) if v), ('', ''))
            w.writerow(['0x%08X' % o['va'], o['ns'], o['name'], o['tipo'], o['tam'], int(o['exacto']), o['file'], '0x%08X' % o['dir']] + difs + [last[0][:128], last[1][:128]])
    if a.plan:
        dump = {}
        for x in csv.DictReader(open(a.plan, encoding='utf-8')):
            if x['grupo'] not in ('G6', 'G7', 'G8'):
                continue
            va = int(x['va'], 16)
            n = min(int(x['tam']), 64)
            b = m.rd(va, n)
            dump['0x%08X' % va] = dict(grupo=x['grupo'], sec=x['seccion'], tam=n, hex=b.hex() if b else None)
        json.dump(dump, open(os.path.join(a.salida, 'va_dump.json'), 'w'))
        print('va_dump: %d direcciones' % len(dump), flush=True)
    print('listo', flush=True)


if __name__ == '__main__':
    main()
