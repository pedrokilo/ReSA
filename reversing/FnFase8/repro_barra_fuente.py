# Repro del fallo de fin de mision: escribe en CHud::m_BigMessage[0] (0xBAACC0, 128 bytes) un texto con todos los caracteres 0x20..0xFF (salvo '~')
# para que CHud::DrawSuccessFailedMessage -> CFont::PrintString indexe m_propValues con todo rango. Uso: python FnCrash_repro.py <exe> <etiqueta> [segundos]
import ctypes, struct, subprocess, sys, time, os
from ctypes import wintypes
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE
exe, et = sys.argv[1], sys.argv[2]; seg = int(sys.argv[3]) if len(sys.argv) > 3 else 150
D = r'D:\Original\_crash_run'; GS = int(os.environ.get('GS_ADDR', '0xC8D4C0'), 16)
while True:
    try: os.mkdir(r'D:\Original\gta_lock'); break
    except OSError: time.sleep(5)
open(r'D:\Original\gta_lock\owner_crash', 'w').write('crash')
try:
    sal = D + r'\salida'; os.makedirs(sal, exist_ok=True)
    for f in os.listdir(os.path.join(D, 'logs')): os.remove(os.path.join(D, 'logs', f))
    p = subprocess.Popen([exe, '--ruta-original', r'D:\Grand Theft Auto San Andreas\gta-sa.exe', '--directorio-juego', D, '--salida', sal, '--limite', '0'], cwd=D)
    h = k.OpenProcess(0x38 | 0x400, False, p.pid)
    def rd(a, n):
        b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) else None
    msg = bytes([c for c in range(0x20, 0x100) if c != 0x7E])[:127] + b'\0'
    t0 = time.time(); e9 = None
    while time.time() - t0 < seg and p.poll() is None:
        gs = struct.unpack('<I', rd(GS, 4) or b'\0' * 4)[0]
        if gs == 9 and e9 is None: e9 = time.time(); print('gGameState=9 a los %.0f s' % (e9 - t0), flush=True)
        if e9 and time.time() - e9 > 25:
            w = ctypes.c_size_t(); k.WriteProcessMemory(h, ctypes.c_void_p(0xBAACC0), msg, 128, ctypes.byref(w))
        time.sleep(0.2)
    rc = p.poll()
    if rc is None: p.kill(); print('vivo al terminar (%d s), lo cierro' % seg)
    else: print('SALIO exit=0x%08X' % (rc & 0xffffffff))
finally:
    try: os.rmdir(r'D:\Original\gta_lock\owner_crash') 
    except OSError: pass
    try: os.remove(r'D:\Original\gta_lock\owner_crash'); 
    except OSError: pass
    try: os.rmdir(r'D:\Original\gta_lock')
    except OSError: pass
