# Prueba de humo del modo ventana con la DLL propia (variante X de jugar.ps1 de FnBIS1): lanza el juego, escribe ForegroundApp (0x8D621C) = 1 desde fuera, espera al estado de juego 9 (0xC8D4C0)
# y lee de la memoria del proceso lo que deja _rwD3D9System (peticion START) con el modo ventana: D3DPRESENT_PARAMETERS (0xC9C040), dispositivo (0xC97C28), "modos listos" (0xC97C38), ventana (0xC97C1C),
# estilo de la ventana, y a que modulo pertenecen las entradas Reset/SetViewport de la tabla virtual del dispositivo (el proxy de WindowedMode.cpp debe estar en la DLL propia).
# Uso: set AGUA_ASI=<ruta de gta_reversed.asi> && python humo_ventana.py <archivo.args> [etiqueta]
# Espera (hasta 900 s) a que no haya otro gta_sa.exe ni candado gta_lock/respaldo_pendiente; mata solo su PID.
import ctypes, struct, subprocess, sys, time, os
from ctypes import wintypes
aqui = r'D:\Original\wv-8\reversing\FnBIS1'          # jugar.ps1 y los .args
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE
u = ctypes.WinDLL('user32', use_last_error=True)
ps = ctypes.WinDLL('psapi', use_last_error=True)

def pid():
    o = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa.exe', '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout
    for l in o.splitlines():
        if 'gta_sa' in l: return int(l.split('","')[1])

class M:
    def __init__(s, p): s.h = k.OpenProcess(0x38 | 0x400 | 0x10, False, p)
    def rd(s, a, n):
        b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) else None
    def wr(s, a, d): r = ctypes.c_size_t(); return k.WriteProcessMemory(s.h, ctypes.c_void_p(a), d, len(d), ctypes.byref(r))
    def u32(s, a): b = s.rd(a, 4); return struct.unpack('<I', b)[0] if b else 0

def modulos(h):
    arr = (ctypes.c_void_p * 1024)(); need = wintypes.DWORD()
    ps.GetModuleFileNameExW.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.LPWSTR, wintypes.DWORD]
    ps.GetModuleInformation.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, wintypes.DWORD]
    ps.EnumProcessModulesEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.DWORD, ctypes.POINTER(wintypes.DWORD), wintypes.DWORD]
    if not ps.EnumProcessModulesEx(h, arr, ctypes.sizeof(arr), ctypes.byref(need), 3): return []
    out = []
    for i in range(need.value // ctypes.sizeof(ctypes.c_void_p)):
        name = ctypes.create_unicode_buffer(260); ps.GetModuleFileNameExW(h, arr[i], name, 260)
        info = (ctypes.c_size_t * 3)(); ps.GetModuleInformation(h, arr[i], info, ctypes.sizeof(info))
        out.append((os.path.basename(name.value), info[0], info[0] + info[1]))
    return out
def duenio(mods, a):
    for n, lo, hi in mods:
        if lo <= a < hi: return n
    return '?'

PID_PROPIO = [None]
def main():
    try: return _main()
    finally:
        if PID_PROPIO[0]: subprocess.run(['taskkill', '/F', '/PID', str(PID_PROPIO[0])], capture_output=True)
def _main():
    args = sys.argv[1]; et = sys.argv[2] if len(sys.argv) > 2 else 'ventana'
    t = time.time()
    while (pid() or os.path.exists('D:/Original/gta_lock') or os.path.exists(os.path.join(aqui, 'respaldo_pendiente'))) and time.time() - t < 900: time.sleep(3)
    asi = os.environ.get('AGUA_ASI'); var = ['-Variante', 'X', '-AsiPath', asi] if asi else ['-Variante', 'A']
    lan = subprocess.Popen(['pwsh', '-NoProfile', '-File', os.path.join(aqui, 'jugar.ps1')] + var + ['-Etiqueta', et, '-ArgsFile', args, '-Auto', '120', '-Silencioso', '-NoEnfocar'], cwd=aqui,
                           stdout=open('D:/Original/sim_out/jugar_%s.log' % et, 'w'), stderr=subprocess.STDOUT)
    p = None; t0 = time.time()
    while time.time() - t0 < 90 and not p:
        p = pid(); time.sleep(1)
    if not p: print('ERROR sin juego'); return 3
    PID_PROPIO[0] = p; m = M(p); estado9 = False
    while time.time() - t0 < 150:
        if lan.poll() is not None: print('ERROR el juego se cerro'); return 3
        m.wr(0x8D621C, struct.pack('<I', 1))      # ForegroundApp = 1
        if m.u32(0xC8D4C0) == 9: estado9 = True; break
        time.sleep(0.5)
    print('estado de juego 9 alcanzado:', estado9, flush=True)
    time.sleep(5)
    pp = m.rd(0xC9C040, 0x38); (w, h, fmt, cnt, ms, mq, swap, hwnd, windowed, auto, dsf, flags, hz, pint) = struct.unpack('<14I', pp)
    dev = m.u32(0xC97C28); ready = m.u32(0xC97C38); win = m.u32(0xC97C1C)
    style = u.GetWindowLongW(wintypes.HWND(win), -16) & 0xFFFFFFFF if win else 0
    print('Present: ancho %d alto %d formato %d backbuffers %d multimuestreo %d/%d swap %d hDeviceWindow %08x windowed %d autoDS %d dsFormat %d flags %d hz %d intervalo %08x' % (w, h, fmt, cnt, ms, mq, swap, hwnd, windowed, auto, dsf, flags, hz, pint))
    print('dispositivo %08x modos_listos %d ventana %08x estilo %08x (WS_CAPTION=%s)' % (dev, ready, win, style, bool(style & 0xC00000)))
    mods = modulos(m.h); vt = m.u32(dev) if dev else 0
    reset = m.u32(vt + 16 * 4) if vt else 0; setvp = m.u32(vt + 47 * 4) if vt else 0
    print('Reset -> %08x (%s)  SetViewport -> %08x (%s)  CreateDevice via _rwD3D9System: hook en 0x7F5F70 = %s' % (reset, duenio(mods, reset), setvp, duenio(mods, setvp), (m.rd(0x7F5F70, 1) or b'?').hex()))
    bien = estado9 and dev != 0 and ready == 1 and windowed == 1 and hwnd == win and duenio(mods, reset).lower().endswith('.asi') and duenio(mods, setvp).lower().endswith('.asi')
    print('VEREDICTO', 'BIEN' if bien else 'MAL')
    subprocess.run(['taskkill', '/F', '/PID', str(p)], capture_output=True)
    try: lan.wait(timeout=60)
    except Exception: pass
    return 0 if bien else 1
sys.exit(main())
