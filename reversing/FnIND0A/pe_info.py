"""Lote IND0A: radiografia del PE del gta_sa.exe original (solo lectura).

Escribe en --out: pe_secciones.csv, pe_imports.csv, pe_recursos.csv, pe_resumen.json
Uso: python -I pe_info.py --exe <gta_sa.exe> --out <dir>
"""
import argparse, csv, hashlib, json, struct
from pathlib import Path

DIRS = ['export', 'import', 'resource', 'exception', 'security', 'basereloc', 'debug', 'architecture',
        'globalptr', 'tls', 'load_config', 'bound_import', 'iat', 'delay_import', 'clr', 'reserved']
RT = {1: 'CURSOR', 2: 'BITMAP', 3: 'ICON', 4: 'MENU', 5: 'DIALOG', 6: 'STRING', 9: 'ACCELERATOR', 10: 'RCDATA',
      12: 'GROUP_CURSOR', 14: 'GROUP_ICON', 16: 'VERSION', 24: 'MANIFEST'}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', required=True)
    ap.add_argument('--out', required=True)
    a = ap.parse_args()
    d = Path(a.exe).read_bytes()
    out = Path(a.out)
    out.mkdir(parents=True, exist_ok=True)
    pe = struct.unpack_from('<I', d, 60)[0]
    machine, nsec, ts, _, _, optsz, chars = struct.unpack_from('<HHIIIHH', d, pe + 4)
    o = pe + 24
    magic = struct.unpack_from('<H', d, o)[0]
    assert magic == 0x10b
    entry, base = struct.unpack_from('<I', d, o + 16)[0], struct.unpack_from('<I', d, o + 28)[0]
    sec_align, file_align = struct.unpack_from('<II', d, o + 32)
    size_image, size_hdr, checksum = struct.unpack_from('<III', d, o + 56)
    subsystem, dllchars = struct.unpack_from('<HH', d, o + 68)
    stack_res, stack_com, heap_res, heap_com = struct.unpack_from('<IIII', d, o + 72)
    nrva = struct.unpack_from('<I', d, o + 92)[0]
    dd = [struct.unpack_from('<II', d, o + 96 + 8 * i) for i in range(min(nrva, 16))]
    secs = []
    for i in range(nsec):
        p = o + optsz + 40 * i
        name = d[p:p + 8].split(b'\0')[0].decode('latin1')
        vs, rva, rs, rp = struct.unpack_from('<IIII', d, p + 8)
        ch = struct.unpack_from('<I', d, p + 36)[0]
        secs.append(dict(nombre=name, va=f'0x{base + rva:08X}', rva=f'0x{rva:08X}', tam_virtual=vs, tam_archivo=rs,
                         offset_archivo=rp, fin_va=f'0x{base + rva + max(vs, rs):08X}',
                         x=int(bool(ch & 0x20000000)), w=int(bool(ch & 0x80000000)), r=int(bool(ch & 0x40000000)),
                         codigo=int(bool(ch & 0x20)), datos_inic=int(bool(ch & 0x40)), bss=int(bool(ch & 0x80)),
                         _rva=rva, _vs=vs, _rs=rs, _rp=rp))

    def rva2off(rva):
        for s in secs:
            if s['_rva'] <= rva < s['_rva'] + max(s['_vs'], s['_rs']):
                return s['_rp'] + rva - s['_rva'] if rva - s['_rva'] < s['_rs'] else None
        return None

    def cstr(off):
        return d[off:d.index(b'\0', off)].decode('latin1')

    with (out / 'pe_secciones.csv').open('w', encoding='utf-8', newline='') as f:
        cols = [k for k in secs[0] if not k.startswith('_')]
        w = csv.DictWriter(f, cols, extrasaction='ignore'); w.writeheader(); w.writerows(secs)

    # imports
    imports = []
    ioff = rva2off(dd[1][0]) if dd[1][0] else None
    if ioff is not None:
        while True:
            oft, tds, fwd, name_rva, iat = struct.unpack_from('<IIIII', d, ioff)
            if not name_rva:
                break
            dll = cstr(rva2off(name_rva))
            t = rva2off(oft or iat)
            slot = 0
            while True:
                v = struct.unpack_from('<I', d, t)[0]
                if v == 0:
                    break
                if v & 0x80000000:
                    nm = f'#{v & 0xFFFF}'
                else:
                    nm = cstr(rva2off(v) + 2)
                imports.append(dict(dll=dll, funcion=nm, iat_va=f'0x{base + iat + 4 * slot:08X}'))
                t += 4; slot += 1
            ioff += 20
    # imports diferidos (delay-load)
    doff = rva2off(dd[13][0]) if dd[13][0] else None
    delay = []
    if doff is not None:
        while True:
            attrs, name_rva, hmod, iat_rva, int_rva = struct.unpack_from('<IIIII', d, doff)
            if not name_rva:
                break
            conv = (lambda v: v) if attrs & 1 else (lambda v: v - base)
            dll = cstr(rva2off(conv(name_rva)))
            t = rva2off(conv(int_rva)); slot = 0
            while True:
                v = struct.unpack_from('<I', d, t)[0]
                if v == 0:
                    break
                nm = f'#{v & 0xFFFF}' if v & 0x80000000 else cstr(rva2off(conv(v)) + 2)
                delay.append(dict(dll=dll + ' (diferida)', funcion=nm, iat_va=f'0x{base + conv(iat_rva) + 4 * slot:08X}'))
                t += 4; slot += 1
            doff += 32
    imports.extend(delay)
    with (out / 'pe_imports.csv').open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, ['dll', 'funcion', 'iat_va']); w.writeheader(); w.writerows(imports)

    # recursos
    res = []
    r0 = dd[2][0]
    def walk(off, level, path):
        base_r = rva2off(r0)
        nn, ni = struct.unpack_from('<HH', d, base_r + off + 12)
        for i in range(nn + ni):
            nid, tgt = struct.unpack_from('<II', d, base_r + off + 16 + 8 * i)
            label = f'{nid & 0x7FFFFFFF}' if not (nid & 0x80000000) else 'nombre'
            if level == 0 and not (nid & 0x80000000):
                label = RT.get(nid, str(nid))
            if tgt & 0x80000000:
                walk(tgt & 0x7FFFFFFF, level + 1, path + [label])
            else:
                drva, dsize, cp, _ = struct.unpack_from('<IIII', d, base_r + tgt)
                res.append(dict(tipo=path[0] if path else label, id=(path[1] if len(path) > 1 else label),
                                idioma=label, tamano=dsize, rva=f'0x{drva:08X}'))
    if r0:
        walk(0, 0, [])
    with (out / 'pe_recursos.csv').open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, ['tipo', 'id', 'idioma', 'tamano', 'rva']); w.writeheader(); w.writerows(res)

    # TLS
    tls = None
    if dd[9][0]:
        t = rva2off(dd[9][0] - 0 if dd[9][0] < base else dd[9][0] - base) if False else rva2off(dd[9][0])
        s_raw, e_raw, idx_va, cb_va, zero_fill, chars_ = struct.unpack_from('<IIIIII', d, t)
        tls = dict(inicio_datos=f'0x{s_raw:08X}', fin_datos=f'0x{e_raw:08X}', indice=f'0x{idx_va:08X}',
                   callbacks=f'0x{cb_va:08X}', relleno=zero_fill)
    # reubicaciones
    reloc_count = 0
    if dd[5][0]:
        t = rva2off(dd[5][0]); end = t + dd[5][1]
        while t < end:
            pg, bs = struct.unpack_from('<II', d, t)
            if bs < 8: break
            reloc_count += (bs - 8) // 2
            t += bs
    overlay = len(d) - max(s['_rp'] + s['_rs'] for s in secs)
    resumen = dict(
        sha256=hashlib.sha256(d).hexdigest(), tamano_archivo=len(d), maquina=hex(machine), secciones=nsec,
        fecha_compilacion=ts, base_imagen=f'0x{base:08X}', punto_entrada=f'0x{base + entry:08X}',
        tam_imagen=size_image, tam_cabeceras=size_hdr, checksum=hex(checksum), subsistema=subsystem,
        caracteristicas_archivo=hex(chars), reubicaciones_quitadas=bool(chars & 1),
        dll_characteristics=hex(dllchars), aslr=bool(dllchars & 0x40), dep=bool(dllchars & 0x100),
        pila_reservada=stack_res, pila_confirmada=stack_com, monton_reservado=heap_res,
        directorios={DIRS[i]: dict(rva=f'0x{r:08X}', tam=s) for i, (r, s) in enumerate(dd) if r or s},
        entradas_reubicacion=reloc_count, tls=tls, overlay_bytes=overlay,
        imports_dlls=sorted({i['dll'] for i in imports}), imports_total=len(imports), imports_diferidos=len(delay),
        recursos_total=len(res), recursos_por_tipo={k: sum(1 for r in res if r['tipo'] == k) for k in sorted({r['tipo'] for r in res})},
    )
    (out / 'pe_resumen.json').write_text(json.dumps(resumen, indent=1, ensure_ascii=False), encoding='utf-8')
    print(json.dumps(resumen, indent=1, ensure_ascii=False))


if __name__ == '__main__':
    main()
