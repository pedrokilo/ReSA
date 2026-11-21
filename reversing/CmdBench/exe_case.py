"""Desensambla el caso de un comando de script en el despachador de su centena del EXE (capstone, PE a mano).

Uso: python exe_case.py <id decimal> [--max N]
Busca en el despachador de la ranura id/100 (tabla 0x8A6168) la tabla de saltos del switch (patron `jmp dword ptr [eax*4+tabla]`,
con indice id-base, y opcionalmente una segunda tabla de bytes) y desensambla desde el destino del caso hasta el primer `ret`.
"""
import struct
import sys
from pathlib import Path
import capstone

EXE = Path(r'E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe')


class Pe:
    def __init__(self, path):
        self.d = Path(path).read_bytes()
        d = self.d
        pe = struct.unpack('<I', d[0x3c:0x40])[0]
        ns = struct.unpack('<H', d[pe + 6:pe + 8])[0]
        opt = struct.unpack('<H', d[pe + 20:pe + 22])[0]
        self.base = struct.unpack('<I', d[pe + 52:pe + 56])[0]
        so = pe + 24 + opt
        self.secs = []
        for i in range(ns):
            s = d[so + 40 * i:so + 40 * i + 40]
            vs, va, rs, ro = struct.unpack('<IIII', s[8:24])
            self.secs.append((va, vs, rs, ro))

    def off(self, va):
        r = va - self.base
        for sva, vs, rs, ro in self.secs:
            if sva <= r < sva + max(vs, rs):
                return ro + r - sva
        raise ValueError(hex(va))

    def u32(self, va):
        return struct.unpack('<I', self.d[self.off(va):self.off(va) + 4])[0]

    def u8(self, va):
        return self.d[self.off(va)]

    def bytes(self, va, n):
        o = self.off(va)
        return self.d[o:o + n]


def disasm(pe, start, n=400, until_ret=True):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False
    out = []
    code = pe.bytes(start, n * 8)
    for ins in md.disasm(code, start):
        out.append(f'{ins.address:08X}: {ins.mnemonic} {ins.op_str}')
        if len(out) >= n:
            break
    return out


def dispatcher_of(pe, cmd):
    return pe.u32(0x8A6168 + 4 * (cmd // 100))


def find_case(pe, cmd):
    disp = dispatcher_of(pe, cmd)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    code = pe.bytes(disp, 600)
    ins = list(md.disasm(code, disp))
    # patron: [sub/add eax, K] ... cmp eax, N ... ja default ... (movzx ecx, byte [eax+T2]) jmp [ecx*4+T1] | jmp [eax*4+T1]
    base = None
    t1 = t2 = None
    for i in ins:
        if i.mnemonic == 'add' and i.op_str.startswith('eax, -') or (i.mnemonic == 'sub' and i.op_str.startswith('eax, ')):
            try:
                v = int(i.op_str.split(',')[1].strip(), 0)
                base = v if i.mnemonic == 'sub' else -v
            except ValueError:
                pass
        if i.mnemonic == 'lea' and i.op_str.startswith('eax, [') and ' - ' in i.op_str:
            base = int(i.op_str.split(' - ')[1].strip(' ]'), 0)
        if i.mnemonic == 'jmp' and '*4 +' in i.op_str:
            t1 = int(i.op_str.split('+')[-1].strip(' ]'), 0)
        if i.mnemonic == 'movzx' and 'byte ptr [eax +' in i.op_str:
            t2 = int(i.op_str.split('+')[-1].strip(' ]'), 0)
    return disp, base, t1, t2, ins


def main():
    cmd = int(sys.argv[1])
    mx = int(sys.argv[sys.argv.index('--max') + 1]) if '--max' in sys.argv else 120
    pe = Pe(EXE)
    disp, base, t1, t2, ins = find_case(pe, cmd)
    print(f'despachador {disp:#x}  base={base}  tabla_saltos={t1 and hex(t1)}  tabla_indices={t2 and hex(t2)}')
    if t1 is None:
        print('no se encontro el switch; cabecera del despachador:')
        print('\n'.join(f'{i.address:08X}: {i.mnemonic} {i.op_str}' for i in ins[:40]))
        return
    idx = cmd - base
    if t2 is not None:
        idx = pe.u8(t2 + idx)
    target = pe.u32(t1 + 4 * idx)
    print(f'comando {cmd}: indice {idx} -> caso {target:#x}')
    print('\n'.join(disasm(pe, target, mx)))


if __name__ == '__main__':
    main()
