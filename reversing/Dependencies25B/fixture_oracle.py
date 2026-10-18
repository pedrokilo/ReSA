"""Oraculo independiente del banco Dependencies25B.

No repite las formulas de las fixtures: cada fila del registro lleva su entrada completa (argumentos, valor impuesto al desplazamiento global, retorno del colaborador y el arena inicial
en hexadecimal, con sus 16 bytes de guarda a cada lado). Aqui se SIMULA cada funcion sobre esa memoria a partir del desensamblado del EXE (ver g25b-classification.md) y se calcula
el retorno, la traza de colaboradores y el hash FNV-1a de 64 bits del arena final, que `run_local.py` exige iguales a los de los dos lados (EXE y DLL).
El unico dato externo es la tabla de tamanos de D3DDECLTYPE de `.rdata` (0x874EF8), que se lee del EXE original (D:/Grand Theft Auto San Andreas/gta-sa.exe).
"""
import struct
from pathlib import Path

M32 = 0xFFFFFFFF
GUARD = 16
EXE_PATH = Path('D:/Grand Theft Auto San Andreas/gta-sa.exe')
NAMES = ['RwCore::RwD3D9IndexBufferCreate', 'RpAnisot::RpAnisotGetMaxSupportedMaxAnisotropy', 'RpAnisot::RpAnisotTextureSetMaxAnisotropy', 'RpAnisot::RpAnisotTextureGetMaxAnisotropy',
         'RpWorld::RpClumpGetNumAtomics', 'RpWorld::RpAtomicSetStreamAlwaysCallBack', 'RpWorld::RpAtomicSetStreamRightsCallBack', 'RpWorld::RpAtomicGetPluginOffset',
         'RpWorld::RpGeometryTriangleGetMaterial', 'RpWorld::_rpMaterialListInitialize', 'RpWorld::_rpMaterialListGetMaterial', 'RpWorld::_rpMaterialListFindMaterialIndex',
         'RpWorld::RpWorldRegisterPlugin', 'RpWorld::RpWorldRegisterPluginStream', 'RpWorld::RpAtomicGetWorld', 'RpWorld::RpLightRegisterPlugin',
         'RpWorld::_rpD3D9VertexDeclarationGetSize', 'RpWorld::RpD3D9GeometryGetUsageFlags', 'RpWorld::RpD3D9WorldSectorSetUsageFlags', 'RpWorld::RpD3D9WorldSectorGetUsageFlags']
(IDX_BUF, MAXSUP, ANSET, ANGET, CLUMP, ALWAYS, RIGHTS, PLUGOFF, TRIMAT, MATINIT, MATGET, MATFIND, WREG, WSTREAM, ATOMWORLD, LREG, VDECL, GEOUSE, SECSET, SECGET) = range(20)
REG_ATOMIC, REG_WORLD, REG_LIGHT = 0x8D624C, 0x8D62D0, 0x8D62F8
VDECL_TABLE = 0x874EF8
CASES = 1024


def fnv64(data):
    h = 0xCBF29CE484222325
    for b in data:
        h ^= b
        h = (h * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return '%016x' % h


_exe = None


def exe_image():
    global _exe
    if _exe is None:
        raw = EXE_PATH.read_bytes()
        pe = struct.unpack_from('<I', raw, 0x3C)[0]
        n = struct.unpack_from('<H', raw, pe + 6)[0]
        opt = struct.unpack_from('<H', raw, pe + 20)[0]
        base = struct.unpack_from('<I', raw, pe + 24 + 28)[0]
        secs = []
        for i in range(n):
            vs, va, rs, rp = struct.unpack_from('<IIII', raw, pe + 24 + opt + i * 40 + 8)
            secs.append((va, vs, rp, rs))
        _exe = (raw, base, secs)
    return _exe


def exe_u32(addr):
    """DWORD de la imagen del EXE (secciones .text/.rdata: no cambian en ejecucion). Los bytes de la seccion mas alla de su tamano en disco son ceros."""
    raw, base, secs = exe_image()
    rva = addr - base
    for va, vs, rp, rs in secs:
        if va <= rva and rva + 4 <= va + max(vs, rs):
            chunk = bytearray(4)
            off = rva - va
            avail = max(0, min(4, min(vs, rs) - off))
            chunk[:avail] = raw[rp + off:rp + off + avail]
            return struct.unpack('<I', bytes(chunk))[0]
    raise ValueError('direccion fuera de las secciones: %#x' % addr)


class Mem:
    """Arena de kArena bytes con 16 de guarda a cada lado, direccionado por la direccion absoluta que registro el banco."""

    def __init__(self, base, init_hex):
        self.base = base
        self.data = bytearray(bytes.fromhex(init_hex))

    def at(self, addr, size):
        i = (addr - self.base) % (1 << 32) + GUARD
        if not (GUARD <= i and i + size <= len(self.data) - GUARD):
            raise ValueError('acceso del oraculo fuera del arena: %#x (+%d)' % (addr, size))
        return i

    def r32(self, addr):
        return struct.unpack_from('<I', self.data, self.at(addr & M32, 4))[0]

    def r16(self, addr):
        return struct.unpack_from('<H', self.data, self.at(addr & M32, 2))[0]

    def r8(self, addr):
        return self.data[self.at(addr & M32, 1)]

    def w32(self, addr, v):
        struct.pack_into('<I', self.data, self.at(addr & M32, 4), v & M32)

    def w8(self, addr, v):
        self.data[self.at(addr & M32, 1)] = v & 0xFF

    def digest(self):
        return fnv64(self.data)


def simulate(fid, inp):
    """Devuelve (ret|None, traza, hash del arena final) para la entrada registrada."""
    m = Mem(inp['base'], inp['init'])
    a = inp['args']
    glob = inp['glob']
    sret = inp['stub_ret'] & M32
    trace = []
    ret = None
    if fid == IDX_BUF:
        trace = [7, a[0], 0x65, a[1]]
        ret = 1 if (sret & 0x80000000) == 0 else 0
    elif fid == MAXSUP:
        trace = [1]
        ret = m.r8(inp['caps_at'] + 0x6C)
    elif fid == ANSET:
        tex, val = a
        if tex == 0:
            ret = 0
        else:
            m.w8(tex + glob, val)
            ret = tex
    elif fid == ANGET:
        tex = a[0]
        ret = 0 if tex == 0 else m.r8(tex + glob)
    elif fid == CLUMP:
        end = (a[0] + 8) & M32
        cur = m.r32(end)
        n = 0
        while cur != end:
            n += 1
            if cur == 0x40:
                break
            cur = m.r32(cur)
            if n > 64:
                raise ValueError('lista sin cierre en el oraculo')
        ret = n
    elif fid in (ALWAYS, RIGHTS):
        trace = [2 if fid == ALWAYS else 3, REG_ATOMIC, a[0], a[1]]
        ret = sret
    elif fid == PLUGOFF:
        trace = [4, REG_ATOMIC, a[0]]
        ret = sret
    elif fid == TRIMAT:
        geo, tri = a
        ident = m.r16(tri + 6)
        ret = 0 if ident == 0xFFFF else m.r32((m.r32(geo + 0x20) + 4 * ident) & M32)
    elif fid == MATINIT:
        lst = a[0]
        for k in range(3):
            m.w32(lst + 4 * k, 0)
        ret = lst
    elif fid == MATGET:
        lst, idx = a
        ret = m.r32((m.r32(lst) + 4 * idx) & M32)
    elif fid == MATFIND:
        lst, mat = a
        n = m.r32(lst + 4)
        sn = n - (1 << 32) if n & 0x80000000 else n
        if sn > 0:
            table = m.r32(lst)
            ret = 0xFFFFFFFF   # sin coincidencia el bucle termina con el indice en 0 y `dec eax` lo deja en -1
            for i in range(sn - 1, -1, -1):
                if m.r32((table + 4 * i) & M32) == mat:
                    ret = i
                    break
        else:
            ret = (sn - 1) & M32   # jle -> dec eax: no se lee la tabla
    elif fid in (WREG, LREG):
        trace = [5, REG_WORLD if fid == WREG else REG_LIGHT] + a
        ret = sret
    elif fid == WSTREAM:
        trace = [6, REG_WORLD] + a
        ret = sret
    elif fid in (ATOMWORLD, GEOUSE, SECGET):
        ret = m.r32((a[0] + glob) & M32)
    elif fid == SECSET:
        m.w32((a[0] + glob) & M32, a[1])
    elif fid == VDECL:
        addr = (VDECL_TABLE + ((a[0] << 2) & M32)) & M32
        # Solo la tabla de 17 DWORD (0x874EF8..0x874F3B) tiene valor propio; fuera de ella el EXE lee memoria real del proceso (.rdata/.text contiguos, que en ejecucion
        # puede diferir del fichero) y el oraculo no la predice: ret = ANY y la compuerta es la igualdad EXE == DLL del mismo par.
        ret = exe_u32(addr) if VDECL_TABLE <= addr < VDECL_TABLE + 17 * 4 else ANY
    else:
        raise ValueError(fid)
    return ret, trace, m.digest()


ANY = 'any'   # sin valor predicho por el oraculo (lectura fuera de la tabla); se exige solo original == candidato


def expected_ret_mask(fid):
    return 0xFF if fid in (MAXSUP, ANGET) else M32
