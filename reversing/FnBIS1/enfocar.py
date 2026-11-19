# BIS1: trae a primer plano la ventana del juego LANZADO POR EL ARNES, solo si el usuario lleva >= 600 s sin tocar teclado/raton.
# El bucle original del EXE (control --unhook-all) no avanza en el menu sin el foco del sistema.
# Uso: python enfocar.py <pid> [segundos_de_inactividad_minimos=600]
import ctypes, sys, time
from ctypes import wintypes
u = ctypes.WinDLL('user32', use_last_error=True); k = ctypes.WinDLL('kernel32')
pid = int(sys.argv[1]); minimo = float(sys.argv[2]) if len(sys.argv) > 2 else 600.0
class LII(ctypes.Structure): _fields_ = [('cbSize', wintypes.UINT), ('dwTime', wintypes.DWORD)]
li = LII(ctypes.sizeof(LII), 0); u.GetLastInputInfo(ctypes.byref(li))
inactivo = ((k.GetTickCount() - li.dwTime) & 0xFFFFFFFF) / 1000.0
found = []
@ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
def cb(h, l):
    p = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(p))
    if p.value == pid and u.IsWindowVisible(h) and u.GetWindow(h, 4) == 0: found.append(h)   # ventanas propias sin dueno
    return True
u.EnumWindows(cb, 0)
if not found: print('sin ventana del juego todavia'); sys.exit(0)
h = found[0]
if u.GetForegroundWindow() == h: print('ya esta en primer plano'); sys.exit(0)
if inactivo < minimo:
    print('AVISO: el juego espera estar en primer plano (el control original no avanza sin foco): haz clic en la ventana del juego. '
          'No se roba el foco porque el usuario esta activo (inactivo %.0f s < %.0f s).' % (inactivo, minimo)); sys.exit(0)
fg = u.GetForegroundWindow(); tf = u.GetWindowThreadProcessId(fg, None); tg = k.GetCurrentThreadId()
u.AllowSetForegroundWindow(pid)
u.AttachThreadInput(tg, tf, True)
u.ShowWindow(h, 9)                                              # SW_RESTORE
u.keybd_event(0x12, 0, 0, 0); u.keybd_event(0x12, 0, 2, 0)       # Alt: permite SetForegroundWindow
u.BringWindowToTop(h)
ok = u.SetForegroundWindow(h)
u.AttachThreadInput(tg, tf, False)
print('traida al frente (usuario inactivo %.0f s): %s; ahora el primer plano %s la ventana del juego' % (inactivo, ok, 'ES' if u.GetForegroundWindow() == h else 'NO es'))
