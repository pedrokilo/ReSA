"""Lote IND0A: barrido de los .obj del proyecto en busca de direcciones fijas del EXE original.

Para cada .obj de build/source/gta_reversed.dir/Debug se desensamblan las secciones de codigo
(capstone, x86 de 32 bits) y se anota toda constante dentro de 0x400000-0xCFFFFF que NO este
cubierta por una relocalizacion (es decir, una direccion absoluta del EXE escrita a mano, como
las de plugin::Call, StaticRef, reinterpret_cast<...>(0x...) o las de RH_Scoped*Install).

Salida (CSV, una fila por referencia): obj, fuente, funcion (simbolo COFF, sin demanglear),
direccion, forma (mov_call = mov reg,imm seguido de call/jmp reg; mem = operando de memoria;
push; imm = otra inmediata), instruccion.
No ejecuta nada del juego. Uso:
  python -I scan_obj.py --obj-dir <dir con los .obj> --out refs_codigo.csv [--jobs N]
"""
import argparse, csv, re, struct, sys
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

LO, HI = 0x400000, 0xD00000
HEXRE = re.compile(r'0x[0-9a-fA-F]+')
REGS = {'eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp'}


def parse_coff(data):
    # soporta /bigobj (Sig1=0, Sig2=0xFFFF)
    if struct.unpack_from('<HH', data, 0) == (0, 0xFFFF):
        machine = struct.unpack_from('<H', data, 6)[0]
        nsec, sym_ptr, nsyms = struct.unpack_from('<III', data, 44)
        sym_size = 20
        sec_off = 56
        big = True
    else:
        machine, nsec, ts, sym_ptr, nsyms, opt, chars = struct.unpack_from('<HHIIIHH', data, 0)
        sec_off = 20 + opt
        sym_size = 18
        big = False
    secs = []
    for i in range(nsec):
        name, vsize, va, rsize, rptr, relptr, lnptr, nrel, nln, ch = struct.unpack_from('<8sIIIIIIHHI', data, sec_off + 40 * i)
        secs.append(dict(name=name.split(b'\0')[0].decode('latin1'), size=rsize, ptr=rptr, relptr=relptr, nrel=nrel, ch=ch))
    return machine, secs, sym_ptr, nsyms, big


def read_symbols(data, sym_ptr, nsyms, big):
    ssz = 20 if big else 18
    strtab = sym_ptr + nsyms * ssz
    def nm(raw):
        if raw[:4] == b'\0\0\0\0':
            off = struct.unpack_from('<I', raw, 4)[0]
            end = data.index(b'\0', strtab + off)
            return data[strtab + off:end].decode('latin1')
        return raw.split(b'\0')[0].decode('latin1')
    out, fname, i = [], '', 0
    while i < nsyms:
        o = sym_ptr + i * ssz
        raw = data[o:o + 8]
        if big:
            val, sec, typ, cls, aux = struct.unpack_from('<IIHBB', data, o + 8)
        else:
            val, sec, typ, cls, aux = struct.unpack_from('<IhHBB', data, o + 8)
        if cls == 103 and aux:  # .file
            fname = data[o + ssz:o + ssz + aux * ssz].split(b'\0')[0].decode('latin1')
        else:
            out.append((nm(raw), val, sec, typ, cls))
        i += 1 + aux
    return out, fname


def scan_obj(path):
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    data = Path(path).read_bytes()
    machine, secs, sym_ptr, nsyms, big = parse_coff(data)
    if machine != 0x14c:
        return Path(path).name, '', [], 'no x86'
    syms, srcfile = read_symbols(data, sym_ptr, nsyms, big)
    secsym = {}
    for name, val, sec, typ, cls in syms:
        if sec > 0 and val == 0 and (typ & 0xF0) == 0x20 and (sec not in secsym):
            secsym[sec] = name
    rows = []
    for idx, s in enumerate(secs, 1):
        if not (s['ch'] & 0x20) or s['size'] == 0:
            continue
        code = data[s['ptr']:s['ptr'] + s['size']]
        relocs = {}
        for r in range(s['nrel']):
            o = s['relptr'] + 10 * r
            va, sym, typ = struct.unpack_from('<IIH', data, o)
            relocs[va] = typ
        reloc_vals = set()
        for va in relocs:
            if va + 4 <= len(code):
                reloc_vals.add(struct.unpack_from('<I', code, va)[0])
        func = secsym.get(idx, s['name'])
        ins = list(md.disasm_lite(code, 0))
        # si el barrido se detiene antes del final (bytes de datos), seguir saltando un byte
        pos = ins[-1][0] + ins[-1][1] if ins else 0
        while pos < len(code):
            more = list(md.disasm_lite(code[pos:], pos))
            if more:
                ins.extend(more)
                pos = more[-1][0] + more[-1][1]
            else:
                pos += 1
        for n, (addr, size, mn, ops) in enumerate(ins):
            if '0x' not in ops:
                continue
            for m in HEXRE.findall(ops):
                v = int(m, 16)
                if not (LO <= v < HI) or v in reloc_vals:
                    continue
                form = 'imm'
                inner = ops.split('[', 1)[1].split(']', 1)[0] if '[' in ops else ''
                if m in inner:
                    form = 'mem'
                elif mn == 'push':
                    form = 'push'
                elif mn == 'mov' and ',' in ops:
                    dst = ops.split(',')[0].strip()
                    if dst in REGS and not ops.split(',', 1)[1].strip().startswith('dword ptr') and '[' not in ops:
                        for _, _, mn2, ops2 in ins[n + 1:n + 14]:
                            if mn2 in ('call', 'jmp') and ops2.strip() == dst:
                                form = 'mov_call'
                                break
                rows.append((Path(path).name, srcfile, func, f'0x{v:08X}', form, f'{mn} {ops}'))
    return Path(path).name, srcfile, rows, ''


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--obj-dir', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--jobs', type=int, default=4)
    a = ap.parse_args()
    objs = sorted(Path(a.obj_dir).glob('*.obj'))
    total = 0
    with open(a.out, 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f)
        w.writerow(['obj', 'fuente', 'funcion', 'direccion', 'forma', 'instruccion'])
        with ProcessPoolExecutor(a.jobs) as ex:
            for name, src, rows, err in ex.map(scan_obj, objs, chunksize=4):
                if err:
                    print('aviso', name, err, file=sys.stderr)
                w.writerows(rows)
                total += len(rows)
    print(f'{len(objs)} objs, {total} referencias', file=sys.stderr)


if __name__ == '__main__':
    main()
