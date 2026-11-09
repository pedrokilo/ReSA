"""Tablas (columna, fila) de las dos funciones del lote 136A, decodificadas del gta-sa.exe canonico (sin usar ningun dato del C++ de produccion).

  0x6FD7C0 (plantilla de matricula): AL - 0x20 > 0x5A -> caso por defecto; si no byte-tabla en 0x6FDB98 (indice de caso) y tabla de saltos en 0x6FDB00.
  0x6FE260 (cartel de carretera):    AL - 0x21 > 0x5D -> caso por defecto 0x6FE9DF; si no tabla de saltos de 94 entradas en 0x6FE9F4.
Cada caso es un trozo de codigo de la forma `mov edx,[esp+4]; <eax=k>; mov [edx],eax; mov eax,[esp+8]; <ecx=k>; mov [eax],ecx; ret`; se interpreta con un
interprete minimo de esas instrucciones (error si aparece otra) y devuelve (valor escrito en el primer puntero, valor escrito en el segundo).

  python -I exe_tables.py            imprime las dos tablas de 256 entradas (None = no escribe nada: no ocurre) como literales de fixture_oracle.py
  python -I exe_tables.py --check    comprueba que las tablas de fixture_oracle.py son las del EXE
"""
import struct
import sys
from pathlib import Path

EXE = Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe')


class Image:
    def __init__(self):
        self.d = EXE.read_bytes()
        pe = struct.unpack_from('<I', self.d, 0x3C)[0]
        n = struct.unpack_from('<H', self.d, pe + 6)[0]
        opt = struct.unpack_from('<H', self.d, pe + 20)[0]
        self.secs = [struct.unpack_from('<8sIIII', self.d, pe + 24 + opt + i * 40) for i in range(n)]

    def off(self, va):
        va -= 0x400000
        for _, vsize, vaddr, rsize, roff in self.secs:
            if vaddr <= va < vaddr + max(rsize, vsize):
                return roff + va - vaddr
        raise ValueError(hex(va))

    def u8(self, va):
        return self.d[self.off(va)]

    def u32(self, va):
        return struct.unpack_from('<I', self.d, self.off(va))[0]


def run_case(img, va):
    """interpreta el caso que empieza en `va`; devuelve (primer valor, segundo valor)"""
    regs = {'eax': 0xDEAD, 'ecx': 0xBEEF, 'edx': 0}
    out = {}
    p1 = p2 = None
    for _ in range(40):
        b = img.u8(va)
        if b == 0xC3:
            return out['p1'], out['p2']
        if b == 0x8B and img.u8(va + 1) == 0x54 and img.u8(va + 2) == 0x24 and img.u8(va + 3) == 4:    # mov edx,[esp+4]
            regs['edx'] = 'p1'; va += 4
        elif b == 0x8B and img.u8(va + 1) == 0x44 and img.u8(va + 2) == 0x24 and img.u8(va + 3) == 8:  # mov eax,[esp+8]
            regs['eax'] = 'p2'; va += 4
        elif b == 0x33 and img.u8(va + 1) == 0xC0:   # xor eax,eax
            regs['eax'] = 0; va += 2
        elif b == 0x33 and img.u8(va + 1) == 0xC9:   # xor ecx,ecx
            regs['ecx'] = 0; va += 2
        elif b == 0xB8:                              # mov eax,imm32
            regs['eax'] = img.u32(va + 1); va += 5
        elif b == 0xB9:                              # mov ecx,imm32
            regs['ecx'] = img.u32(va + 1); va += 5
        elif b == 0x8B and img.u8(va + 1) == 0xC8:   # mov ecx,eax
            regs['ecx'] = regs['eax']; va += 2
        elif b == 0x89 and img.u8(va + 1) == 0x02:   # mov [edx],eax
            assert regs['edx'] == 'p1' and isinstance(regs['eax'], int)
            out['p1'] = regs['eax']; va += 2
        elif b == 0x89 and img.u8(va + 1) == 0x08:   # mov [eax],ecx
            assert regs['eax'] == 'p2' and isinstance(regs['ecx'], int)
            out['p2'] = regs['ecx']; va += 2
        else:
            raise ValueError('instruccion no prevista en %x: %02x' % (va, b))
    raise ValueError('caso sin ret')


def tables():
    img = Image()
    plate, sign = [], []
    for ch in range(256):
        i = (ch - 0x20) & 0xFFFFFFFF
        if i > 0x5A:
            target = 0x6FDAE9
        else:
            target = img.u32(0x6FDB00 + 4 * img.u8(0x6FDB98 + i))
        plate.append(run_case(img, target))
        j = (ch - 0x21) & 0xFFFFFFFF
        target = 0x6FE9DF if j > 0x5D else img.u32(0x6FE9F4 + 4 * j)
        sign.append(run_case(img, target))
    return plate, sign


if __name__ == '__main__':
    plate, sign = tables()
    if len(sys.argv) > 1 and sys.argv[1] == '--check':
        sys.path.insert(0, str(Path(__file__).resolve().parent))
        import fixture_oracle as o
        assert [tuple(x) for x in o.PLATE] == plate and [tuple(x) for x in o.SIGN] == sign
        print('ok: las tablas del oraculo son las del EXE')
    else:
        print('PLATE =', plate)
        print('SIGN =', sign)
