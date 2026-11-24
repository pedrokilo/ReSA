"""Lanza un exe x86 bajo un depurador minimo que, en el punto de ruptura inicial (antes de los callbacks TLS y del punto de entrada), inyecta
espia_archivos.dll (hook de NtCreateFile/NtOpenFile/NtQuery*Attributes de ntdll) y se desengancha. No necesita administrador.
Uso como modulo: lanzar(exe, args_linea, cwd, dll, log) -> (pid, hProcess, hThread)   (el log lo escribe la DLL con ESPIA_LOG)."""
import ctypes, os, struct
from ctypes import wintypes as W
k = ctypes.WinDLL('kernel32', use_last_error=True)
ps = ctypes.WinDLL('psapi', use_last_error=True)
k.VirtualAllocEx.restype = ctypes.c_void_p
k.VirtualAllocEx.argtypes = [W.HANDLE, ctypes.c_void_p, ctypes.c_size_t, W.DWORD, W.DWORD]
k.WriteProcessMemory.argtypes = [W.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
ps.GetModuleBaseNameW.argtypes = [W.HANDLE, ctypes.c_void_p, W.LPWSTR, W.DWORD]
ps.EnumProcessModulesEx.argtypes = [W.HANDLE, ctypes.c_void_p, W.DWORD, ctypes.POINTER(W.DWORD), W.DWORD]


class STARTUPINFO(ctypes.Structure):
    _fields_ = [('cb', W.DWORD), ('lpReserved', W.LPWSTR), ('lpDesktop', W.LPWSTR), ('lpTitle', W.LPWSTR), ('dwX', W.DWORD), ('dwY', W.DWORD), ('dwXSize', W.DWORD), ('dwYSize', W.DWORD),
                ('dwXCountChars', W.DWORD), ('dwYCountChars', W.DWORD), ('dwFillAttribute', W.DWORD), ('dwFlags', W.DWORD), ('wShowWindow', W.WORD), ('cbReserved2', W.WORD),
                ('lpReserved2', ctypes.c_void_p), ('hStdInput', W.HANDLE), ('hStdOutput', W.HANDLE), ('hStdError', W.HANDLE)]


class PI(ctypes.Structure):
    _fields_ = [('hProcess', W.HANDLE), ('hThread', W.HANDLE), ('dwProcessId', W.DWORD), ('dwThreadId', W.DWORD)]


class EXC(ctypes.Structure):
    _fields_ = [('code', W.DWORD), ('flags', W.DWORD), ('record', ctypes.c_void_p), ('address', ctypes.c_void_p), ('nparams', W.DWORD), ('info', ctypes.c_void_p * 15)]


class DBGEV(ctypes.Structure):
    class U(ctypes.Union):
        _fields_ = [('exc', EXC), ('raw', ctypes.c_byte * 160)]
    _fields_ = [('code', W.DWORD), ('pid', W.DWORD), ('tid', W.DWORD), ('u', U)]


class FLT(ctypes.Structure):
    _fields_ = [('a', W.DWORD * 7), ('reg', ctypes.c_byte * 80), ('b', W.DWORD)]


class CTX(ctypes.Structure):   # WOW64_CONTEXT (x86)
    _fields_ = [('ContextFlags', W.DWORD), ('Dr0', W.DWORD), ('Dr1', W.DWORD), ('Dr2', W.DWORD), ('Dr3', W.DWORD), ('Dr6', W.DWORD), ('Dr7', W.DWORD), ('Float', FLT),
                ('SegGs', W.DWORD), ('SegFs', W.DWORD), ('SegEs', W.DWORD), ('SegDs', W.DWORD), ('Edi', W.DWORD), ('Esi', W.DWORD), ('Ebx', W.DWORD), ('Edx', W.DWORD), ('Ecx', W.DWORD),
                ('Eax', W.DWORD), ('Ebp', W.DWORD), ('Eip', W.DWORD), ('SegCs', W.DWORD), ('EFlags', W.DWORD), ('Esp', W.DWORD), ('SegSs', W.DWORD), ('Ext', ctypes.c_byte * 512)]


assert ctypes.sizeof(CTX) == 716, ctypes.sizeof(CTX)


def _export(dll, nombre):
    d = open(dll, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3C)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    osz = struct.unpack_from('<H', d, pe + 20)[0]
    secs = [struct.unpack_from('<IIII', d, pe + 24 + osz + 40 * i + 8) for i in range(nsec)]   # vsize, va, rawsize, rawptr

    def off(rva):
        for vs, va, rs, rp in secs:
            if va <= rva < va + max(vs, rs):
                return rp + rva - va
    ed = struct.unpack_from('<I', d, pe + 24 + 96)[0]
    e = off(ed)
    nn = struct.unpack_from('<I', d, e + 24)[0]
    fa = struct.unpack_from('<I', d, e + 28)[0]
    na = struct.unpack_from('<I', d, e + 32)[0]
    no = struct.unpack_from('<I', d, e + 36)[0]
    for i in range(nn):
        nm = struct.unpack_from('<I', d, off(na) + 4 * i)[0]
        s = off(nm)
        t = d[s:d.index(b'\0', s)]
        if t == nombre.encode():
            ordi = struct.unpack_from('<H', d, off(no) + 2 * i)[0]
            return struct.unpack_from('<I', d, off(fa) + 4 * ordi)[0]
    raise KeyError(nombre)


def _modulo32(h, nombre):
    arr = (ctypes.c_void_p * 1024)()
    need = W.DWORD()
    ps.EnumProcessModulesEx(h, arr, ctypes.sizeof(arr), ctypes.byref(need), 1)
    buf = ctypes.create_unicode_buffer(260)
    for i in range(need.value // ctypes.sizeof(ctypes.c_void_p)):
        ps.GetModuleBaseNameW(h, ctypes.c_void_p(arr[i]), buf, 260)
        if buf.value.lower() == nombre.lower():
            return arr[i]
    return None


def lanzar(exe, args_linea, cwd, dll, log):
    os.environ['ESPIA_LOG'] = log
    os.environ['_NO_DEBUG_HEAP'] = '1'   # sin el monton de depuracion del sistema (no cambiar el comportamiento del proceso)
    open(log, 'w').close()
    si = STARTUPINFO()
    si.cb = ctypes.sizeof(si)
    pi = PI()
    cmd = ctypes.create_unicode_buffer('"%s" %s' % (exe, args_linea))
    if not k.CreateProcessW(None, cmd, None, None, False, 0x2, None, cwd, ctypes.byref(si), ctypes.byref(pi)):
        raise OSError(ctypes.get_last_error())
    ev = DBGEV()
    inyectado = False
    guardado = None
    while True:
        if not k.WaitForDebugEvent(ctypes.byref(ev), 30000):
            raise TimeoutError('sin eventos de depuracion')
        cont = 0x00010002   # DBG_CONTINUE
        if ev.code == 5:
            raise RuntimeError('el proceso termino durante la inyeccion')
        if ev.code == 1:
            c = ev.u.exc.code
            if c == 0x80000003 and not inyectado:
                pass   # punto de ruptura nativo (64 bits) previo a la carga del mundo de 32 bits: se ignora
            elif c in (0x4000001F, 0x80000003):
                if not inyectado:
                    inyectado = True
                    ctx = CTX()
                    ctx.ContextFlags = 0x10003
                    if not k.Wow64GetThreadContext(pi.hThread, ctypes.byref(ctx)):
                        raise OSError(ctypes.get_last_error())
                    guardado = CTX.from_buffer_copy(ctx)
                    base32 = _modulo32(pi.hProcess, 'kernel32.dll')
                    if not base32:
                        raise RuntimeError('kernel32 de 32 bits no cargado en el punto de ruptura inicial')
                    ll = base32 + _export(os.path.join(os.environ['SystemRoot'], 'SysWOW64', 'kernel32.dll'), 'LoadLibraryA')
                    ruta = dll.encode('ascii') + b'\0'
                    mem = k.VirtualAllocEx(pi.hProcess, None, 4096, 0x3000, 0x40)
                    cod = b'\x68' + struct.pack('<I', mem + 64) + b'\xB8' + struct.pack('<I', ll) + b'\xFF\xD0' + b'\xCC'
                    n = ctypes.c_size_t()
                    k.WriteProcessMemory(pi.hProcess, ctypes.c_void_p(mem), cod, len(cod), ctypes.byref(n))
                    k.WriteProcessMemory(pi.hProcess, ctypes.c_void_p(mem + 64), ruta, len(ruta), ctypes.byref(n))
                    ctx.Eip = mem
                    if not k.Wow64SetThreadContext(pi.hThread, ctypes.byref(ctx)):
                        raise OSError(ctypes.get_last_error())
                else:
                    # segundo punto de ruptura: el int3 tras LoadLibraryA; restaurar el contexto y soltar el proceso
                    if not k.Wow64SetThreadContext(pi.hThread, ctypes.byref(guardado)):
                        raise OSError(ctypes.get_last_error())
                    k.ContinueDebugEvent(ev.pid, ev.tid, cont)
                    k.DebugSetProcessKillOnExit(False)
                    if not k.DebugActiveProcessStop(ev.pid):
                        raise OSError(ctypes.get_last_error())
                    return pi.dwProcessId, pi.hProcess, pi.hThread
            else:
                cont = 0x80010001   # DBG_EXCEPTION_NOT_HANDLED
        k.ContinueDebugEvent(ev.pid, ev.tid, cont)
