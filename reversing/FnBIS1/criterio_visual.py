# Criterio visual para las pruebas de humo (prueba_marcadores.py, prueba_ropero.py, prueba_partida_nueva.py).
# Motivo (22/11/2026): las pruebas daban BIEN solo por memoria (marcadores que cambian cada segundo, objeto 2877, ...) mientras el
# dibujo estaba roto: la tuberia de atomos por defecto de D3D9 no tenia callback de render y los marcadores 3D no se veian.
# Reglas: (1) captura de la ventana COMPLETA (area cliente entera, proceso con DPI consciente: sin ello GetClientRect devuelve el tamano
# virtualizado y la imagen sale mas pequena), validando que el tamano de la imagen coincide con el rectangulo cliente; (2) el objeto
# esperado se mide en pixeles de su color en la captura (no por memoria); (3) siempre contra un control (variante B = EXE original,
# misma posicion/angulo/hora): el criterio compara pixeles del objeto con y sin la funcion; (4) mirar la imagen entera antes de afirmar.
# Uso (desde una prueba):  import criterio_visual as cv ; cap = cv.capturar_completa(ruta) ; n = cv.pixeles_color(cap['rgb'], (20,25,202), tol=60)
import ctypes, struct, zlib
from ctypes import wintypes

try:
    ctypes.windll.user32.SetProcessDPIAware()
except Exception:
    pass
u = ctypes.WinDLL('user32', use_last_error=True)
g = ctypes.WinDLL('gdi32', use_last_error=True)
WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)


class _BIH(ctypes.Structure):
    _fields_ = [('biSize', wintypes.DWORD), ('biWidth', wintypes.LONG), ('biHeight', wintypes.LONG), ('biPlanes', wintypes.WORD), ('biBitCount', wintypes.WORD),
                ('biCompression', wintypes.DWORD), ('biSizeImage', wintypes.DWORD), ('biX', wintypes.LONG), ('biY', wintypes.LONG), ('biClrUsed', wintypes.DWORD),
                ('biClrImportant', wintypes.DWORD)]


def buscar_ventana(clase='Grand theft auto San Andreas'):
    r = []

    def cb(h, _):
        c = ctypes.create_unicode_buffer(128)
        u.GetClassNameW(h, c, 128)
        if c.value == clase and u.IsWindowVisible(h):
            r.append(h)
        return True
    u.EnumWindows(WNDENUMPROC(cb), 0)
    return r[0] if r else None


def capturar_completa(ruta=None, traer=True):
    """Captura el area cliente entera. Devuelve dict con ancho, alto, cliente (del rectangulo), completa (tamano coincide) y rgb (bytes BGRA)."""
    h = buscar_ventana()
    if not h:
        return None
    if traer:
        u.ShowWindow(h, 9)
        u.SetForegroundWindow(h)
    rc = wintypes.RECT()
    u.GetClientRect(h, ctypes.byref(rc))
    pt = wintypes.POINT(0, 0)
    u.ClientToScreen(h, ctypes.byref(pt))
    w, hh = rc.right - rc.left, rc.bottom - rc.top
    sdc = u.GetDC(0)
    mdc = g.CreateCompatibleDC(sdc)
    bm = g.CreateCompatibleBitmap(sdc, w, hh)
    g.SelectObject(mdc, bm)
    if not u.PrintWindow(h, mdc, 3):
        g.BitBlt(mdc, 0, 0, w, hh, sdc, pt.x, pt.y, 0x00CC0020)
    bi = _BIH(ctypes.sizeof(_BIH), w, -hh, 1, 32, 0, 0, 0, 0, 0, 0)
    buf = ctypes.create_string_buffer(w * hh * 4)
    g.GetDIBits(mdc, bm, 0, hh, buf, ctypes.byref(bi), 0)
    g.DeleteObject(bm)
    g.DeleteDC(mdc)
    u.ReleaseDC(0, sdc)
    data = buf.raw
    if ruta:
        raw = b''.join(b'\x00' + bytes(sum(([data[i + 2], data[i + 1], data[i]] for i in range(y * w * 4, (y + 1) * w * 4, 4)), [])) for y in range(hh))

        def ch(t, d):
            return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
        open(ruta, 'wb').write(b'\x89PNG\r\n\x1a\n' + ch(b'IHDR', struct.pack('>IIBBBBB', w, hh, 8, 2, 0, 0, 0)) + ch(b'IDAT', zlib.compress(raw, 6)) + ch(b'IEND', b''))
    return {'ancho': w, 'alto': hh, 'cliente': (rc.right, rc.bottom), 'completa': (w, hh) == (rc.right, rc.bottom) and w >= 640 and hh >= 480, 'rgb': data}


def pixeles_color(bgra, rgb, tol=60, ancho=None):
    """Cuenta pixeles de la captura (bytes BGRA) cuyo color esta a menos de `tol` (distancia maxima por canal) de rgb=(r,g,b)."""
    r0, g0, b0 = rgb
    n = 0
    for i in range(0, len(bgra), 4):
        if abs(bgra[i + 2] - r0) <= tol and abs(bgra[i + 1] - g0) <= tol and abs(bgra[i] - b0) <= tol:
            n += 1
    return n


def criterio_objeto(captura, color_rgb, minimo=150, tol=60):
    """BIEN si la captura es completa y hay al menos `minimo` pixeles del color del objeto esperado (marcador 3D, coche, ...)."""
    if not captura or not captura['completa']:
        return False, 'captura incompleta o ausente'
    n = pixeles_color(captura['rgb'], color_rgb, tol)
    return n >= minimo, '%d pixeles de color %s (minimo %d) en %dx%d' % (n, color_rgb, minimo, captura['ancho'], captura['alto'])
