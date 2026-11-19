# PL1: sonda de solo lectura de un gta_sa.exe en marcha. Lee los pools (+4 mapa de estados con bit 7 = libre, +8 capacidad)
# y la posicion del jugador. Salida en una linea: nombre=usados/capacidad ... pos=x,y,z
import ctypes, sys, struct, os
from ctypes import wintypes
k = ctypes.WinDLL('kernel32', use_last_error=True)
k.OpenProcess.restype = wintypes.HANDLE
h = k.OpenProcess(0x0010 | 0x0400 | 0x0020 | 0x0008, False, int(sys.argv[1]))
if not h: print('sin_acceso'); sys.exit(0)
def rd(a, n):
    b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
    if not k.ReadProcessMemory(h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) or r.value != n: return None
    return b.raw
def u32(a):
    d = rd(a, 4); return struct.unpack('<I', d)[0] if d else 0
POOLS = [('enex', 0x96A7D8), ('ped', 0xB74490), ('veh', 0xB74494), ('bld', 0xB74498), ('obj', 0xB7449C), ('dum', 0xB744A0),
         ('col', 0xB744A4), ('ptr1', 0xB74484), ('ptr2', 0xB74488), ('einf', 0xB7448C)]
out = []
for nm, a in POOLS:
    p = u32(a)
    if not p: out.append(f'{nm}=nulo'); continue
    hd = rd(p, 0x10)
    if not hd: out.append(f'{nm}=?'); continue
    st, cap = struct.unpack('<Ii', hd[4:12])
    m = rd(st, cap) if 0 < cap < 200000 else None
    out.append(f'{nm}={sum(1 for x in m if not x & 0x80) if m else -1}/{cap}')
ped = u32(0xB7CD98)
pos = 'sinped'
if ped:
    mat = u32(ped + 0x14)
    d = rd(mat + 0x30, 12) if mat else rd(ped + 4, 12)
    pos = '%.1f,%.1f,%.1f' % struct.unpack('<fff', d) if d else '?'
out.append('pos=' + pos)
print(' '.join(out))
# --tp: (PL1, solo en pruebas automaticas) saca al jugador de la casa de CJ colocandolo en la calle, como hacen los entrenadores:
# posicion en la matriz (+0x14 -> +0x30), codigo de area del peaton (+0x2F) y area actual (0xB72914) a 0 (exterior)
if len(sys.argv) > 2 and sys.argv[2] == '--tp' and ped:
    marca = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'tmp', 'tp_hecho_%s' % sys.argv[1])
    if not os.path.exists(marca):
        mat = u32(ped + 0x14)
        if mat:
            w = ctypes.c_size_t()
            def wr(a, b): return k.WriteProcessMemory(h, ctypes.c_void_p(a), b, len(b), ctypes.byref(w))
            ok = wr(0xB72914, struct.pack('<I', 0)) and wr(ped + 0x2F, bytes([0])) and wr(mat + 0x30, struct.pack('<fff', 2495.0, -1685.0, 14.5))
            open(marca, 'w').write('1'); print('TP', ok)
# --tpexit: coloca al jugador sobre el marcador de SALIDA del interior (la entrada de salida de la casa de CJ, busqueda en el pool por
# rectangulo/Z), de modo que el juego haga la salida por su propio camino (CEntryExitManager::Update). Una sola vez.
if len(sys.argv) > 2 and sys.argv[2] == '--tpexit' and ped:
    marca = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'tmp', 'tp_hecho_%s' % sys.argv[1])
    if not os.path.exists(marca):
        pool = u32(0x96A7D8); stor = u32(pool); st = u32(pool + 4)
        cap = struct.unpack('<i', rd(pool + 8, 4))[0]
        m = rd(st, cap); cand = []
        for i in range(cap):
            if m[i] & 0x80: continue
            d = rd(stor + i * 0x3C, 0x3C)
            l, b, r, t, z = struct.unpack('<fffff', d[8:28])
            cx, cy = (l + r) / 2, (b + t) / 2
            if abs(z - 1014.7) < 4 and abs(cx - 2496) < 25 and abs(cy + 1695) < 25: cand.append((i, d[:8].split(b'\0')[0].decode('ascii', 'replace'), cx, cy, z))
        print('ENEX', cand)
        mat = u32(ped + 0x14)
        if cand and mat:
            i, nm, cx, cy, z = cand[0]
            w = ctypes.c_size_t()
            ok = k.WriteProcessMemory(h, ctypes.c_void_p(mat + 0x30), struct.pack('<fff', cx, cy, z + 1.0), 12, ctypes.byref(w))
            open(marca, 'w').write('1'); print('TPEXIT', ok)
