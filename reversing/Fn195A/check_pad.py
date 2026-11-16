"""Comprueba contra el PE canonico (gta-sa.exe) los bytes de las 22 ranuras por defecto de CVehicle (0x6D6270..0x6D63C0) y de las dos constantes x87 (0x6D3D50, 0x6D56B0), y que tras cada una
queda relleno (`nop` 0x90 o `int3` 0xCC) hasta la siguiente funcion alineada a 16 bytes: el salto de 5 bytes del hook S (E9 rel32) no pisa a la funcion vecina. Sale con 1 si algo no coincide.   python -I check_pad.py [ruta-al-exe]"""
import hashlib, struct, sys
EXE = sys.argv[1] if len(sys.argv) > 1 else r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
data = open(EXE, 'rb').read()
print('SHA256', hashlib.sha256(data).hexdigest())
pe = struct.unpack_from('<I', data, 0x3C)[0]
nsec = struct.unpack_from('<H', data, pe + 6)[0]
opt = struct.unpack_from('<H', data, pe + 20)[0]
base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + opt + 40 * i
    vsize, va, rsize, raw = struct.unpack_from('<IIII', data, o + 8)
    secs.append((va, max(vsize, rsize), raw))


def at(addr, n):
    rva = addr - base
    for va, size, raw in secs:
        if va <= rva < va + size:
            return data[raw + rva - va: raw + rva - va + n]
    raise KeyError(hex(addr))


EXPECT = {0x6D6270: 'c20400', 0x6D6280: 'c20400', 0x6D6290: 'c20800', 0x6D62A0: '32c0c20400', 0x6D62B0: '32c0c20400', 0x6D62C0: '32c0c20400', 0x6D62D0: '32c0c20400', 0x6D62E0: '32c0c20400',
          0x6D62F0: '32c0c20400', 0x6D6300: '32c0c20400', 0x6D6310: '32c0c20400', 0x6D6320: '32c0c20400', 0x6D6330: 'c20400', 0x6D6340: 'c20800', 0x6D6350: '32c0c20400', 0x6D6360: '32c0c20800',
          0x6D6370: '32c0c20800', 0x6D6380: 'c3', 0x6D6390: 'c3', 0x6D63A0: 'c3', 0x6D63B0: 'c3', 0x6D63C0: 'c3',
          0x6D3D50: 'd905848c8500c20400', 0x6D56B0: 'd90550 8b8500c20400'.replace(' ', '')}
bad = 0
for addr, hexs in EXPECT.items():
    want = bytes.fromhex(hexs)
    got = at(addr, len(want))
    pad_len = (16 - ((addr + len(want)) % 16)) % 16
    pad = at(addr + len(want), pad_len)
    ok = got == want and all(b in (0x90, 0xCC) for b in pad)
    bad += not ok
    print('%08X %-20s %s relleno (nop/int3) %d bytes %s' % (addr, got.hex(), 'OK' if got == want else 'DISTINTO', pad_len, 'OK' if all(b in (0x90, 0xCC) for b in pad) else 'NO'))
print('float 0x858C84 =', struct.unpack('<f', at(0x858C84, 4))[0], ' float 0x858B50 =', struct.unpack('<f', at(0x858B50, 4))[0])
sys.exit(1 if bad else 0)
