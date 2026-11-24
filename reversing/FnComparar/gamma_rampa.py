# Lee la rampa de gamma actual de la pantalla principal (GDI GetDeviceGammaRamp) y la resume. Uso: python -I gamma_rampa.py [etiqueta]
# Rampa identidad: ramp[i] = i*257 (65535 al final). El juego (CGamma) pone una rampa potencia 1-nivel+0.2 con SetGammaRamp(D3DSGR_CALIBRATE).
import ctypes, sys, time, json
from ctypes import wintypes
g, u = ctypes.windll.gdi32, ctypes.windll.user32
u.GetDC.restype = wintypes.HDC; u.GetDC.argtypes = [wintypes.HWND]; u.ReleaseDC.argtypes = [wintypes.HWND, wintypes.HDC]; g.GetDeviceGammaRamp.argtypes = [wintypes.HDC, ctypes.c_void_p]
def leer():
    hdc = u.GetDC(None); buf = (ctypes.c_ushort * 768)()
    ok = g.GetDeviceGammaRamp(hdc, buf); u.ReleaseDC(None, hdc)
    return ok, list(buf)
def identidad(b): return all(b[c * 256 + i] == i * 257 for c in range(3) for i in range(256))
if __name__ == '__main__':
    ok, b = leer()
    r = {'t': time.strftime('%H:%M:%S'), 'etiqueta': sys.argv[1] if len(sys.argv) > 1 else '', 'ok': bool(ok), 'identidad': bool(ok) and identidad(b)}
    if ok: r['rampa_R'] = [b[i] for i in (0, 32, 64, 128, 192, 224, 255)]; r['rampa_G'] = [b[256 + i] for i in (0, 128, 255)]
    print(json.dumps(r))
