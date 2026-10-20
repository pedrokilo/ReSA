"""Lanza harness.exe reservando antes la zona 0x400000..0xCB1000 en el proceso (suspendido) para que quepa el gta-sa.exe mapeado en su base.

  python -B reversing/AutoPreRender60/offgame/run_offgame.py [--exe-harness RUTA] [casos] [primera_semilla] [ruta_gta-sa.exe]

Windows mapea al arrancar archivos de localizacion y segmentos de monton en la zona baja (0x400000...), asi que no basta con VirtualAlloc dentro del propio proceso.
Este lanzador crea harness.exe suspendido, hace VirtualAllocEx(0x400000, 0x8B1000, MEM_RESERVE) y lo reanuda; la salida va a la consola del lanzador.
"""
import ctypes, sys
from ctypes import wintypes
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXE_BASE, EXE_END = 0x400000, 0xCB1000
CREATE_SUSPENDED = 0x4
MEM_RESERVE, PAGE_EXECUTE_READWRITE = 0x2000, 0x40
INFINITE = 0xFFFFFFFF

k32 = ctypes.WinDLL('kernel32', use_last_error=True)
k32.VirtualAllocEx.restype = ctypes.c_void_p
k32.VirtualAllocEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_size_t, wintypes.DWORD, wintypes.DWORD]


class STARTUPINFO(ctypes.Structure):
    _fields_ = [('cb', wintypes.DWORD), ('lpReserved', wintypes.LPWSTR), ('lpDesktop', wintypes.LPWSTR), ('lpTitle', wintypes.LPWSTR), ('dwX', wintypes.DWORD),
                ('dwY', wintypes.DWORD), ('dwXSize', wintypes.DWORD), ('dwYSize', wintypes.DWORD), ('dwXCountChars', wintypes.DWORD), ('dwYCountChars', wintypes.DWORD),
                ('dwFillAttribute', wintypes.DWORD), ('dwFlags', wintypes.DWORD), ('wShowWindow', wintypes.WORD), ('cbReserved2', wintypes.WORD),
                ('lpReserved2', ctypes.c_void_p), ('hStdInput', wintypes.HANDLE), ('hStdOutput', wintypes.HANDLE), ('hStdError', wintypes.HANDLE)]


class PROCESS_INFORMATION(ctypes.Structure):
    _fields_ = [('hProcess', wintypes.HANDLE), ('hThread', wintypes.HANDLE), ('dwProcessId', wintypes.DWORD), ('dwThreadId', wintypes.DWORD)]


def main():
    args = sys.argv[1:]
    harness = HERE / 'build' / 'harness.exe'
    if args and args[0] == '--exe-harness':
        harness = Path(args[1])
        args = args[2:]
    cmd = '"%s" %s' % (harness, ' '.join('"%s"' % a for a in args))
    si = STARTUPINFO()
    si.cb = ctypes.sizeof(si)
    pi = PROCESS_INFORMATION()
    if not k32.CreateProcessW(None, ctypes.create_unicode_buffer(cmd), None, None, True, CREATE_SUSPENDED, None, str(harness.parent), ctypes.byref(si), ctypes.byref(pi)):
        raise SystemExit('CreateProcess fallo: %d' % ctypes.get_last_error())
    got = k32.VirtualAllocEx(pi.hProcess, EXE_BASE, EXE_END - EXE_BASE, MEM_RESERVE, PAGE_EXECUTE_READWRITE)
    if got != EXE_BASE:
        k32.TerminateProcess(pi.hProcess, 3)
        raise SystemExit('VirtualAllocEx no pudo reservar 0x400000 (%s, error %d)' % (hex(got or 0), ctypes.get_last_error()))
    k32.ResumeThread(pi.hThread)
    k32.WaitForSingleObject(pi.hProcess, INFINITE)
    code = wintypes.DWORD()
    k32.GetExitCodeProcess(pi.hProcess, ctypes.byref(code))
    sys.exit(code.value)


if __name__ == '__main__':
    main()
