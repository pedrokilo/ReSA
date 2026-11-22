# Prueba del raton del EXE independiente: lanza gta_sa_standalone.exe (toma D:\Original\gta_lock), trae la ventana al frente,
# simula movimiento y botones con SendInput y lee por memoria el estado del ratón del juego.
# Uso: python probar_raton.py <etiqueta> [exe] [--espera S] [--sin-frente]
# Lee: RsGlobal.ps (0xC17054) -> diMouse en +0x1C, ForegroundApp (0x8D621C), CMouseControllerState Temp/New/Old (0xB73404/0xB73418/0xB7342C).
# Solo toca el proceso que lanza (lo cierra al terminar).
import ctypes, struct, subprocess, sys, time, os
from ctypes import wintypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import captura
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE
u = captura.u
DEST = r'D:\Original\_fase8b_run'
OUT = 'D:/Original/sim_out'
LOCK = r'D:\Original\gta_lock'

class INPUT_MOUSE(ctypes.Structure):
    _fields_ = [('dx', wintypes.LONG), ('dy', wintypes.LONG), ('mouseData', wintypes.DWORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class INPUT(ctypes.Structure):
    class _U(ctypes.Union): _fields_ = [('mi', INPUT_MOUSE)]
    _anonymous_ = ('u',); _fields_ = [('type', wintypes.DWORD), ('u', _U)]
def mouse(dx=0, dy=0, flags=0x0001, data=0):
    i = INPUT(); i.type = 0; i.mi = INPUT_MOUSE(dx, dy, data, flags, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INPUT))

class M:
    def __init__(s, p): s.h = k.OpenProcess(0x38 | 0x400, False, p)
    def rd(s, a, n):
        b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) else None
    def u32(s, a): b = s.rd(a, 4); return struct.unpack('<I', b)[0] if b else 0

def estado(m):
    def e(a):
        b = m.rd(a, 0x14)
        return {'btn': (b[0], b[1], b[2]), 'rueda': (b[3], b[4]), 'mov': struct.unpack('<ff', b[0xC:0x14]), 'rueda_f': struct.unpack('<f', b[8:12])[0]}
    ps = m.u32(0xC17054)
    return {'ps': hex(ps), 'diInterface': hex(m.u32(ps + 0x18)), 'diMouse': hex(m.u32(ps + 0x1C)), 'fg': m.u32(0x8D621C), 'temp': e(0xB73404), 'new': e(0xB73418)}

def main():
    et = sys.argv[1]
    exe = sys.argv[2] if len(sys.argv) > 2 and not sys.argv[2].startswith('--') else os.path.join(DEST, 'gta_sa_standalone.exe')
    espera = int(sys.argv[sys.argv.index('--espera') + 1]) if '--espera' in sys.argv else 45
    frente = '--sin-frente' not in sys.argv
    os.makedirs(OUT, exist_ok=True)
    f = open('%s/raton_%s.log' % (OUT, et), 'w', encoding='utf-8')
    def P(*a):
        s = ' '.join(str(x) for x in a); print(s, flush=True); f.write(s + '\n'); f.flush()
    while True:
        try: os.mkdir(LOCK); break
        except FileExistsError: time.sleep(5)
    open(LOCK + r'\owner_raton', 'w').write('raton')
    p = None
    try:
        while subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa_standalone.exe'], capture_output=True, text=True).stdout.count('gta_sa_standalone') or \
              subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa.exe'], capture_output=True, text=True).stdout.count('gta_sa.exe'):
            time.sleep(10)
        sal = os.path.join(DEST, 'salida_raton'); os.makedirs(sal, exist_ok=True)
        for x in os.listdir(os.path.join(DEST, 'logs')):
            try: os.remove(os.path.join(DEST, 'logs', x))
            except OSError: pass
        args = f'--ruta-original "D:\\Grand Theft Auto San Andreas\\gta-sa.exe" --directorio-juego "{DEST}" --salida "{sal}" --limite 0'
        p = subprocess.Popen(f'"{exe}" {args}', cwd=DEST)
        P('lanzado pid', p.pid)
        m = M(p.pid); t0 = time.time()
        while time.time() - t0 < espera: time.sleep(1)
        h = captura.buscar(); P('ventana', h)
        if frente and h:
            u.ShowWindow(h, 9)
            # truco para poder tomar el primer plano: ALT
            u.keybd_event(0x12, 0, 0, 0); u.keybd_event(0x12, 0, 2, 0)
            P('SetForegroundWindow ->', u.SetForegroundWindow(h), 'fg real == ventana:', u.GetForegroundWindow() == h)
            rc = wintypes.RECT(); u.GetWindowRect(h, ctypes.byref(rc)); u.SetCursorPos((rc.left + rc.right) // 2, (rc.top + rc.bottom) // 2)
            time.sleep(1)
        P('ANTES', estado(m))
        for fase in range(3):
            for i in range(30):
                mouse(8, 3); time.sleep(0.02)
            P('tras movimiento %d' % fase, estado(m))
            mouse(flags=0x0002); time.sleep(0.15); P('  boton izq abajo', estado(m)['new']['btn']); mouse(flags=0x0004); time.sleep(0.1)
            mouse(flags=0x0800, data=120 & 0xFFFFFFFF); time.sleep(0.15); P('  rueda', estado(m)['new']['rueda'], estado(m)['temp']['rueda_f'])
            time.sleep(0.3)
        if h: P(captura.capturar('%s/raton_%s.png' % (OUT, et), traer=False))
    finally:
        if p and p.poll() is None: p.kill()
        try: os.rmdir(LOCK) if not os.listdir(LOCK) else (os.remove(LOCK + r'\owner_raton'), os.rmdir(LOCK))
        except OSError: pass
        for x in os.listdir(os.path.join(DEST, 'logs')) if os.path.isdir(os.path.join(DEST, 'logs')) else []:
            if x.endswith('.log'):
                try: open('%s/raton_%s_%s' % (OUT, et, x), 'wb').write(open(os.path.join(DEST, 'logs', x), 'rb').read())
                except OSError: pass

if __name__ == '__main__':
    main()
