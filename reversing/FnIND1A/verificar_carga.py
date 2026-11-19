r"""IND1A: comprobacion automatica de la carga (modo solo-cargar) frente al EXE original.

Lanza gta_reversed_exe.exe --ruta-original <gta_sa.exe> --volcar en una carpeta temporal (fuera de E:\JUEGOS),
lee ind1a_volcado.bin (secciones tal como quedaron en memoria, en orden y con tamano virtual) y compara con el PE original:
  - SHA-256 por seccion de memoria frente al archivo (excluyendo la IAT, que el cargador reescribe a proposito);
  - cola virtual (bss) a cero; sin relocaciones (directorio vacio y flag IMAGE_FILE_RELOCS_STRIPPED);
  - IAT: toda entrada de import (214 + terminadores) distinta de cero, y apuntando fuera del rango 0x400000-0xCB1000.
Uso: python -I verificar_carga.py --exe <gta_reversed_exe.exe> --original <gta_sa.exe> [--salida <json>]
Codigo de salida 0 si todo OK.
"""
import argparse, hashlib, json, os, struct, subprocess, sys, tempfile
from pathlib import Path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', required=True)
    ap.add_argument('--original', required=True)
    ap.add_argument('--salida', default=None)
    ap.add_argument('--timeout', type=int, default=120)
    a = ap.parse_args()
    orig = Path(a.original).read_bytes()
    pe = struct.unpack_from('<I', orig, 60)[0]
    nsec, = struct.unpack_from('<H', orig, pe + 6)
    chars, = struct.unpack_from('<H', orig, pe + 22)
    opt = struct.unpack_from('<H', orig, pe + 20)[0]
    o = pe + 24
    base, = struct.unpack_from('<I', orig, o + 28)
    reloc_sz, = struct.unpack_from('<I', orig, o + 96 + 5 * 8 + 4)
    iat_rva, iat_sz = struct.unpack_from('<II', orig, o + 96 + 12 * 8)
    secs = []
    for i in range(nsec):
        p = o + opt + 40 * i
        nm = orig[p:p + 8].split(b'\0')[0].decode()
        vs, rva, rs, rp = struct.unpack_from('<IIII', orig, p + 8)
        secs.append((nm, rva, vs, rp, rs))
    with tempfile.TemporaryDirectory(prefix='ind1a_') as td:
        exe = Path(td) / 'gta_reversed_exe.exe'
        exe.write_bytes(Path(a.exe).read_bytes())
        r = subprocess.run([str(exe), '--ruta-original', a.original, '--volcar'], cwd=td, timeout=a.timeout)
        dump = (Path(td) / 'ind1a_volcado.bin').read_bytes() if (Path(td) / 'ind1a_volcado.bin').exists() else b''
        informe = (Path(td) / 'ind1a_informe.txt').read_text(encoding='latin1') if (Path(td) / 'ind1a_informe.txt').exists() else ''
    res = {'exit_code': r.returncode, 'sin_relocaciones': bool(chars & 1) and reloc_sz == 0, 'secciones': {}}
    off = 0
    iat_lo, iat_hi = iat_rva, iat_rva + iat_sz
    ok = r.returncode == 0 and res['sin_relocaciones'] and len(dump) == sum(s[2] for s in secs)
    iat_vals = []
    for nm, rva, vs, rp, rs in secs:
        mem = bytearray(dump[off:off + vs])
        off += vs
        arch = orig[rp:rp + min(rs, vs)] + b'\0' * (vs - min(rs, vs))
        if rva <= iat_lo < rva + vs:
            lo = iat_lo - rva
            for k in range(lo, min(lo + iat_sz, vs), 4):
                iat_vals.append(struct.unpack_from('<I', mem, k)[0])
            mem[lo:lo + iat_sz] = arch[lo:lo + iat_sz]  # excluir la IAT
        igual = bytes(mem) == arch
        res['secciones'][nm] = {'va': hex(base + rva), 'virtual': vs, 'sha256_memoria': hashlib.sha256(mem).hexdigest(),
                                'sha256_archivo': hashlib.sha256(arch).hexdigest(), 'igual_salvo_IAT': igual}
        ok &= igual
    ceros = sum(1 for v in iat_vals if v == 0)
    en_rango = sum(1 for v in iat_vals if 0x400000 <= v < 0xCB1000)
    res['iat'] = {'palabras': len(iat_vals), 'ceros_terminadores': ceros, 'apuntan_dentro_de_la_imagen': en_rango,
                  'resueltas': len(iat_vals) - ceros}
    ok &= en_rango == 0 and len(iat_vals) - ceros == 214 and 'sin resolver: 0' in informe
    res['informe_dice_imports_ok'] = 'imports: 214, sin resolver: 0' in informe
    res['todo_ok'] = bool(ok)
    txt = json.dumps(res, indent=1)
    print(txt)
    if a.salida:
        Path(a.salida).write_text(txt, encoding='utf-8')
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
