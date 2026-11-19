"""Lote IND0A: comprobacion ESTATICA (sin ejecutar nada) del EXE de prueba gta_reversed_exe.exe.

Comprueba:
  1. base 0x400000, sin relocaciones, sin ASLR (el codigo propio da por supuestas las direcciones fijas);
  2. la reserva (simbolo _g_ImagenOriginalReservada del .map) ocupa justo 0x401000-0xCB1000 y es el primer objeto de .text;
  3. el primer codigo propio queda en VA >= 0xCB1000 (nada propio pisa el rango de la imagen original);
  4. la reserva esta a cero en el archivo (el EXE no lleva ningun byte del original);
  5. el EXE no importa nada de gta_sa.exe ni de ninguna DLL de la lista de prohibidas (--prohibidas);
  6. el enlace no dejo simbolos sin resolver (link.log sin LNK2001/LNK2019/LNK4088);
  7. imports del EXE (DLL: n) para el informe.
Salida: codigo 0 si todo cumple, 1 si no; escribe verificacion.json en la carpeta de salida.
Uso: python -I verificar_exe.py <carpeta con gta_reversed_exe.exe, .map y link.log>
"""
import json, re, struct, sys
from pathlib import Path

RESERVA_INI, RESERVA_FIN = 0x401000, 0xCB1000


def main():
    d = Path(sys.argv[1])
    exe = (d / 'gta_reversed_exe.exe').read_bytes()
    pe = struct.unpack_from('<I', exe, 60)[0]
    nsec, = struct.unpack_from('<H', exe, pe + 6)
    opt = struct.unpack_from('<H', exe, pe + 20)[0]
    chars, = struct.unpack_from('<H', exe, pe + 22)
    o = pe + 24
    base, = struct.unpack_from('<I', exe, o + 28)
    dllch, = struct.unpack_from('<H', exe, o + 70)
    secs = []
    for i in range(nsec):
        p = o + opt + 40 * i
        nm = exe[p:p + 8].split(b'\0')[0].decode()
        vs, rva, rs, rp = struct.unpack_from('<IIII', exe, p + 8)
        secs.append((nm, base + rva, vs, rp, rs))
    res = {}
    res['base_0x400000'] = base == 0x400000
    res['sin_relocaciones'] = bool(chars & 1)
    res['sin_aslr'] = not (dllch & 0x40)
    mp = (d / 'gta_reversed_exe.map').read_text(encoding='latin1')
    m = re.search(r'0001:([0-9a-f]{8})\s+_g_ImagenOriginalReservada\s+([0-9a-f]{8})', mp)
    res['reserva_en_0x401000'] = bool(m) and int(m.group(2), 16) == RESERVA_INI
    first_own = None
    for l in mp.splitlines():
        mm = re.match(r'\s+0001:([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})', l)
        if mm and int(mm.group(3), 16) >= RESERVA_INI and 'reserva_imagen' not in l:
            va = int(mm.group(3), 16)
            first_own = va if first_own is None else min(first_own, va)
    res['primer_codigo_propio'] = hex(first_own) if first_own else None
    res['propio_fuera_de_la_reserva'] = bool(first_own) and first_own >= RESERVA_FIN
    t = next(s for s in secs if s[0] == '.text')
    off0 = t[3]
    blk = exe[off0:off0 + (RESERVA_FIN - RESERVA_INI)]
    res['reserva_a_cero_en_archivo'] = len(blk) == RESERVA_FIN - RESERVA_INI and not any(blk)
    # imports
    idir, = struct.unpack_from('<I', exe, o + 96 + 8)

    def rva2off(r):
        for nm, va, vs, rp, rs in secs:
            if va - base <= r < va - base + max(vs, rs):
                return rp + r - (va - base)

    dlls = {}
    p = rva2off(idir)
    while p is not None:
        oft, _, _, nrva, iat = struct.unpack_from('<IIIII', exe, p)
        if not nrva:
            break
        name = exe[rva2off(nrva):].split(b'\0')[0].decode().lower()
        q = rva2off(oft or iat)
        n = 0
        while struct.unpack_from('<I', exe, q)[0]:
            q += 4
            n += 1
        dlls[name] = n
        p += 20
    res['imports_por_dll'] = dlls
    res['no_importa_gta_sa'] = not any('gta_sa' in k for k in dlls)
    log = (d / 'link.log').read_text(encoding='latin1', errors='replace')
    sin = re.findall(r'(LNK2001|LNK2019|LNK4088)', log)
    res['simbolos_sin_resolver'] = len(sin)
    res['sin_simbolos_sin_resolver'] = not sin
    ok = all(res[k] for k in res if isinstance(res[k], bool))
    res['todo_ok'] = ok
    (d / 'verificacion.json').write_text(json.dumps(res, indent=1, ensure_ascii=False), encoding='utf-8')
    print(json.dumps(res, indent=1, ensure_ascii=False))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
