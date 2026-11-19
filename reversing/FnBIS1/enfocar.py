# BIS1: trae a primer plano la ventana de un proceso (para que el bucle original del EXE, que se PAUSA en segundo plano, avance)
import ctypes, sys
from ctypes import wintypes
u = ctypes.WinDLL('user32', use_last_error=True)
pid = int(sys.argv[1]); found = []
@ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
def cb(h, l):
    p = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(p))
    if p.value == pid and u.IsWindowVisible(h): found.append(h)
    return True
u.EnumWindows(cb, 0)
for h in found:
    u.ShowWindow(h, 9)                      # SW_RESTORE
    u.keybd_event(0x12, 0, 0, 0); u.keybd_event(0x12, 0, 2, 0)   # truco Alt: permite SetForegroundWindow
    print('enfocar', hex(h), u.SetForegroundWindow(h))
