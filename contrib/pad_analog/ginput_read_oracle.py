# -*- coding: utf-8 -*-
"""Oraculo: evalua la parte de sticks y botones de la rutina 0x10007550 de GInputSA.asi (instruccion a instruccion, sin ejecutar el .asi)
con valores de entrada dados y devuelve los int16 que se escribirian en CPad+0xA8.. (PCTempJoyState)."""
import struct, sys
sys.path.insert(0, __file__.rsplit('\\', 1)[0])
from x87_interp import Interp, OBJ, CPAD, Stop

# desplazamientos (en el objeto de SCP, base del objeto +0x1c): ver docs/CodigoVivoExe/lectura-mando-ginput.md
IN = dict(up=0x1c, right=0x20, down=0x24, left=0x28, lx=0x2c, ly=0x30, l1=0x34, l2=0x38, l3=0x3c, rx=0x40, ry=0x44, r1=0x48, r2=0x4c, r3=0x50,
          tri=0x54, cir=0x58, cross=0x5c, sq=0x60, sel=0x64, start=0x68, ps=0x6c)
OUT = dict(lx=0xa8, ly=0xaa, rx=0xac, ry=0xae, l1=0xb0, l2=0xb2, r1=0xb4, r2=0xb6, up=0xb8, down=0xba, left=0xbc, right=0xbe, start=0xc0, select=0xc2,
           square=0xc4, triangle=0xc6, cross=0xc8, circle=0xca, l3=0xcc, r3=0xce, ps=0xd0)

DEFAULT = dict(ldz=0.24, rdz=0.27, lsens=1.0, rsens=1.0, face=510.0, invLX=0, invLY=0, swapL=0, invRX=0, invRY=0, swapR=0)


def run(pad=0, inputs=None, cfg=None, trace=False):
    c = dict(DEFAULT)
    c.update(cfg or {})
    mem = {}
    for k, v in (inputs or {}).items():
        mem[OBJ + IN[k]] = struct.pack('<f', v)
    f = lambda a, v: mem.__setitem__(a + 4 * pad, struct.pack('<f', v))
    b = lambda a, v: mem.__setitem__(a + pad, bytes([v]))
    f(0x1003abd0, c['ldz']); f(0x1003ab84, c['rdz']); f(0x1003ac8c, c['lsens']); f(0x1003abc4, c['rsens']); f(0x1003ae58, c['face'])
    b(0x1003ab80, c['invLX']); b(0x1003abd8, c['invLY']); b(0x1003abc0, c['swapL'])
    b(0x1003ab64, c['invRX']); b(0x1003abbc, c['invRY']); b(0x1003abdc, c['swapR'])
    mem[0x1003ad18] = struct.pack('<I', CPAD - 0x134 * pad)  # CPad::Pads (base + pad*0x134 = CPAD)
    mem[0x1003acec + 4 * pad] = struct.pack('<I', OBJ)
    it = Interp(mem)
    it.regs['edi'] = pad
    it.regs['esi'] = OBJ
    it.run(0x1000758a, 0x10007ac1, trace=trace)
    out = {}
    for k, off in OUT.items():
        v = it.writes.get(off)
        if v is not None:
            out[k] = struct.unpack('<h', v[:2])[0] if k in ('lx', 'ly', 'rx', 'ry') else struct.unpack('<H', v[:2])[0]
    return out


if __name__ == '__main__':
    print(run(0, dict(lx=0.5, ly=0.25, rx=-0.7, ry=0.9, l2=0.5, r2=1.0, cross=1.0), trace=False))


def run_xinput(pad=0, raw=None, cfg=None, trace=False):
    """Ruta de XInput normal (0x10006EA0 -> 0x10006ED7..0x100074A5): sticks int16 crudos, botones en wButtons, gatillos 0..255."""
    c = dict(DEFAULT)
    c.update(cfg or {})
    raw = raw or {}
    mem = {}
    obj = OBJ
    for off, k in ((0x10, 'buttons'), (0x12, 'lt'), (0x13, 'rt'), (0x14, 'lx'), (0x16, 'ly'), (0x18, 'rx'), (0x1a, 'ry')):
        v = raw.get(k, 0)
        if k == 'buttons':
            mem[obj + off] = struct.pack('<H', v)
        elif k in ('lt', 'rt'):
            mem[obj + off] = bytes([v])
        else:
            mem[obj + off] = struct.pack('<h', v)
    f = lambda a, v: mem.__setitem__(a + 4 * pad, struct.pack('<f', v))
    b = lambda a, v: mem.__setitem__(a + pad, bytes([v]))
    f(0x1003abd0, c['ldz']); f(0x1003ab84, c['rdz']); f(0x1003ac8c, c['lsens']); f(0x1003abc4, c['rsens'])
    b(0x1003ab80, c['invLX']); b(0x1003abd8, c['invLY']); b(0x1003abc0, c['swapL'])
    b(0x1003ab64, c['invRX']); b(0x1003abbc, c['invRY']); b(0x1003abdc, c['swapR'])
    mem[0x1003ad18] = struct.pack('<I', CPAD - 0x134 * pad)
    mem[0x1003acec + 4 * pad] = struct.pack('<I', obj)
    it = Interp(mem)
    it.regs['esi'] = pad
    it.regs['edi'] = CPAD
    it.regs['esp'] = 0x70001000
    it.mem[0x70001000 + 0xf] = b'\x01'
    it.run(0x10006eed, 0x100074a5, trace=trace)
    out = {}
    for k, off in OUT.items():
        v = it.writes.get(off)
        if v is not None:
            out[k] = struct.unpack('<h', v[:2])[0] if k in ('lx', 'ly', 'rx', 'ry') else struct.unpack('<H', v[:2])[0]
    return out
