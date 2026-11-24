# Prueba: ¿la captura de pantalla por BitBlt (la usada en la comparacion) refleja una rampa de gamma aplicada con SetDeviceGammaRamp?
# Aplica 3 s una rampa potencia 1.2 (como CGamma con nivel 0) mientras mide la media de una region del escritorio, y restaura SIEMPRE la rampa original.
import ctypes, sys, time
from ctypes import wintypes
sys.path.insert(0, '.')
import gamma_rampa as G
g, u = ctypes.windll.gdi32, ctypes.windll.user32
V = ctypes.c_void_p
u.GetDC.restype = V; u.GetDC.argtypes = [V]; u.ReleaseDC.argtypes = [V, V]
g.SetDeviceGammaRamp.argtypes = [V, V]
g.CreateCompatibleDC.restype = V; g.CreateCompatibleDC.argtypes = [V]
g.CreateCompatibleBitmap.restype = V; g.CreateCompatibleBitmap.argtypes = [V, ctypes.c_int, ctypes.c_int]
g.SelectObject.argtypes = [V, V]; g.SelectObject.restype = V
g.BitBlt.argtypes = [V, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, V, ctypes.c_int, ctypes.c_int, wintypes.DWORD]
g.GetDIBits.argtypes = [V, V, wintypes.UINT, wintypes.UINT, V, V, wintypes.UINT]
g.DeleteObject.argtypes = [V]; g.DeleteDC.argtypes = [V]
def media_pantalla(w=400, h=300):
    hdc = u.GetDC(None); mem = g.CreateCompatibleDC(hdc); bmp = g.CreateCompatibleBitmap(hdc, w, h); g.SelectObject(mem, bmp)
    g.BitBlt(mem, 0, 0, w, h, hdc, 0, 0, 0x00CC0020 | 0x40000000)
    class BI(ctypes.Structure): _fields_ = [('s', wintypes.DWORD), ('w', wintypes.LONG), ('h', wintypes.LONG), ('p', wintypes.WORD), ('b', wintypes.WORD), ('c', wintypes.DWORD), ('si', wintypes.DWORD), ('x', wintypes.LONG), ('y', wintypes.LONG), ('u', wintypes.DWORD), ('i', wintypes.DWORD)]
    bi = BI(40, w, -h, 1, 32, 0, 0, 0, 0, 0, 0); buf = (ctypes.c_ubyte * (w * h * 4))()
    g.GetDIBits(mem, bmp, 0, h, ctypes.cast(buf, V), ctypes.cast(ctypes.byref(bi), V), 0)
    s = [0, 0, 0]
    for c in range(3): s[c] = sum(buf[c::4]) / (w * h)
    g.DeleteObject(bmp); g.DeleteDC(mem); u.ReleaseDC(None, hdc)
    return [round(x, 1) for x in s]
ok, orig = G.leer(); assert ok and G.identidad(orig), 'la rampa actual no es identidad; no se prueba'
hdc = u.GetDC(None)
try:
    a = media_pantalla()
    rampa = (ctypes.c_ushort * 768)()
    for c in range(3):
        for i in range(256): rampa[c * 256 + i] = int(65535 * (i / 255.0) ** 1.2)
    r = g.SetDeviceGammaRamp(hdc, ctypes.cast(rampa, V)); time.sleep(1.0)
    ok2, leida = G.leer(); b = media_pantalla()
    print('SetDeviceGammaRamp ->', r, '| rampa leida[128]=', leida[128] if ok2 else None, '(identidad 32896)', '| media escritorio sin rampa', a, 'con rampa 1.2', b)
finally:
    g.SetDeviceGammaRamp(hdc, ctypes.cast((ctypes.c_ushort * 768)(*orig), V)); u.ReleaseDC(None, hdc)
    ok3, f = G.leer(); print('restaurada: identidad =', ok3 and G.identidad(f))
