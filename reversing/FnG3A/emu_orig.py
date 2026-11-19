"""Tercera referencia: ejecuta el codigo REAL de 0x00746480 (bytes sin parchear del EXE) con Unicorn y compara con oracle.py.

malloc (0x824257) se sustituye por una reserva con relleno 0xCD (para ver las ranuras sin inicializar); el resto es el
codigo del EXE. Por caso compara: argc, bloque completo devuelto (tabla + copia de la cadena), tamano reservado,
registros preservados (ebx, esi, edi, ebp) y el desplazamiento de ESP al volver (4 = ret simple).
"""
import hashlib
import struct
import sys
from pathlib import Path
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP,
                               UC_X86_REG_ESP, UC_X86_REG_EIP)

sys.path.insert(0, str(Path(__file__).resolve().parent))
import oracle

EXE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
SHA = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
FN, MALLOC, SENT = 0x746480, 0x824257, 0x900000
HEAP, STACK = 0x10000000, 0x20000000
CODE_SHA = '82d2bb6e491f95133e0024727fe2fc2ea1ab30aa0cdac2d7492cb548bc0a7ee6'


def load_fn(exe=EXE):
    d = open(exe, 'rb').read()
    assert hashlib.sha256(d).hexdigest() == SHA, 'EXE distinto'
    pe = struct.unpack_from('<I', d, 0x3c)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    opt = struct.unpack_from('<H', d, pe + 20)[0]
    base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
    for i in range(nsec):
        nm, vs, va, rs, ro = struct.unpack_from('<8sIIII', d, pe + 24 + opt + i * 40)
        r = FN - base
        if va <= r < va + max(vs, rs):
            code = d[ro + r - va: ro + r - va + 300]
            assert hashlib.sha256(code).hexdigest() == CODE_SHA
            return code
    raise SystemExit('seccion no encontrada')


class Emu:
    def __init__(self, exe=EXE):
        self.code = load_fn(exe)
        u = self.u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(0x746000, 0x1000)
        u.mem_write(FN, self.code)
        u.mem_map(0x824000, 0x1000)
        u.mem_write(MALLOC, b'\xc3')
        u.mem_map(SENT, 0x1000)
        u.mem_map(HEAP, 0x4000000)
        u.mem_map(STACK, 0x10000)
        self.allocs = []
        self.next = HEAP
        u.hook_add(UC_HOOK_CODE, self.hook, begin=MALLOC, end=MALLOC)

    def hook(self, u, addr, size, _):
        esp = u.reg_read(UC_X86_REG_ESP)
        n = struct.unpack('<I', u.mem_read(esp + 4, 4))[0]
        p = self.next
        self.next = (p + n + 15) & ~15
        u.mem_write(p, b'\xcd' * n)
        self.allocs.append((p, n))
        u.reg_write(UC_X86_REG_EAX, p)

    def run(self, cmd: bytes):
        u = self.u
        cmdp = HEAP
        argcp = HEAP + len(cmd) + 16
        self.next = (argcp + 16 + 15) & ~15
        self.allocs = []
        u.mem_write(cmdp, cmd + b'\0')
        u.mem_write(argcp, b'\xef\xbe\xad\xde')
        esp = STACK + 0x8000 - 8
        u.mem_write(esp, struct.pack('<II', SENT, argcp))   # [esp]=retorno, [esp+4]=&argc
        u.reg_write(UC_X86_REG_ESP, esp)
        u.reg_write(UC_X86_REG_EAX, cmdp)
        for r, v in ((UC_X86_REG_EBX, 0x11111111), (UC_X86_REG_ESI, 0x22222222), (UC_X86_REG_EDI, 0x33333333), (UC_X86_REG_EBP, 0x44444444)):
            u.reg_write(r, v)
        u.emu_start(FN, SENT, count=100_000_000)
        assert u.reg_read(UC_X86_REG_EIP) == SENT
        eax = u.reg_read(UC_X86_REG_EAX)
        argc = struct.unpack('<i', u.mem_read(argcp, 4))[0]
        regs = tuple(u.reg_read(r) for r in (UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP))
        (p, n), = self.allocs
        return argc, eax, p, n, bytes(u.mem_read(p, n)), regs, u.reg_read(UC_X86_REG_ESP) - esp


def main(path):
    em = Emu()
    bad = tot = 0
    for line in open(path):
        cmd = bytes.fromhex(line.strip())
        argc, eax, p, n, blk, regs, dsp = em.run(cmd)
        oc, ob = oracle.orig_split(cmd, base=p)
        ok = (argc == oc and eax == p and blk == ob and n == len(ob)
              and regs == (0x11111111, 0x22222222, 0x33333333, 0x44444444) and dsp == 4)
        tot += 1
        if not ok:
            bad += 1
            if bad <= 10:
                print('DIF', cmd[:80], argc, oc, blk[:64], ob[:64])
    print(f'emulacion del EXE vs oraculo: {tot} casos, {bad} diferencias')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
