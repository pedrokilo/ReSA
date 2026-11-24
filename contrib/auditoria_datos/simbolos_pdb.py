"""Enumera los objetos propios DatosExe de un PDB (gta_reversed.pdb o gta_sa_standalone.pdb) con dbghelp (ctypes):
nombre, RVA y tamano exactos.  Uso desde comparar_objetos_asi.py; solo lectura del PDB.
"""
import ctypes, os
from ctypes import wintypes

MAX_SYM_NAME = 2000


class SYMBOL_INFO(ctypes.Structure):
    _fields_ = [('SizeOfStruct', wintypes.ULONG), ('TypeIndex', wintypes.ULONG), ('Reserved', ctypes.c_uint64 * 2),
                ('Index', wintypes.ULONG), ('Size', wintypes.ULONG), ('ModBase', ctypes.c_uint64), ('Flags', wintypes.ULONG),
                ('Value', ctypes.c_uint64), ('Address', ctypes.c_uint64), ('Register', wintypes.ULONG), ('Scope', wintypes.ULONG),
                ('Tag', wintypes.ULONG), ('NameLen', wintypes.ULONG), ('MaxNameLen', wintypes.ULONG), ('Name', ctypes.c_char * MAX_SYM_NAME)]


ENUMCB = ctypes.WINFUNCTYPE(wintypes.BOOL, ctypes.POINTER(SYMBOL_INFO), wintypes.ULONG, ctypes.c_void_p)


def enumerar(pdb, exe, base=0x10000000, mascara='DatosExe::*'):
    """devuelve {nombre_completo: (rva, tam)} segun el PDB; exe es el binario al que pertenece (para tamano de imagen)."""
    dh = ctypes.WinDLL('dbghelp.dll')
    dh.SymLoadModuleExW.restype = ctypes.c_uint64
    dh.SymLoadModuleExW.argtypes = [wintypes.HANDLE, wintypes.HANDLE, wintypes.LPCWSTR, wintypes.LPCWSTR, ctypes.c_uint64, wintypes.DWORD, ctypes.c_void_p, wintypes.DWORD]
    dh.SymEnumSymbols.argtypes = [wintypes.HANDLE, ctypes.c_uint64, ctypes.c_char_p, ENUMCB, ctypes.c_void_p]
    h = ctypes.c_void_p(0x1234)   # identificador propio
    dh.SymSetOptions(0x2 | 0x4 | 0x10000 | 0x100000)   # UNDNAME | DEFERRED_LOADS... + NO_PROMPTS
    if not dh.SymInitializeW(h, None, False):
        raise OSError('SymInitialize')
    size = os.path.getsize(exe)
    m = dh.SymLoadModuleExW(h, None, exe, None, base, 0x4000000, None, 0)   # tamano generoso
    if not m:
        raise OSError('SymLoadModuleEx %d' % ctypes.get_last_error())
    res = {}

    def cb(si, tam, ctx):
        s = si.contents
        res[s.Name[:s.NameLen].decode('latin1')] = (s.Address - base, tam or s.Size)
        return True
    f = ENUMCB(cb)
    dh.SymEnumSymbols(h, m, mascara.encode(), f, None)
    dh.SymCleanup(h)
    return res
