# Teclado por SendInput con codigos de barrido (DirectInput los lee) y ventana del juego en primer plano.
import ctypes, time
from ctypes import wintypes
u = ctypes.WinDLL('user32', use_last_error=True)
class KBD(ctypes.Structure):
    _fields_ = [('wVk', wintypes.WORD), ('wScan', wintypes.WORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class MOU(ctypes.Structure):
    _fields_ = [('dx', wintypes.LONG), ('dy', wintypes.LONG), ('mouseData', wintypes.DWORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class INP(ctypes.Structure):
    class _U(ctypes.Union): _fields_ = [('ki', KBD), ('mi', MOU)]
    _anonymous_ = ('u',); _fields_ = [('type', wintypes.DWORD), ('u', _U)]
CODIGOS = {'enter': (0x1C, False), 'esc': (0x01, False), 'space': (0x39, False), 'up': (0x48, True), 'down': (0x50, True), 'left': (0x4B, True), 'right': (0x4D, True), 'tab': (0x0F, False), 'pgdn': (0x51, True)}
def tecla(nombre, mant=0.12):
    sc, ext = CODIGOS[nombre]
    for up in (0, 1):
        i = INP(); i.type = 1; i.ki = KBD(0, sc, 0x8 | (0x1 if ext else 0) | (0x2 if up else 0), 0, None)
        u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP)); time.sleep(mant)
