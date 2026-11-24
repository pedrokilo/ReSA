# Captura del cliente por BitBlt desde la pantalla (la ventana debe estar visible y al frente); devuelve (w,h,rgb) o None
import ctypes
from ctypes import wintypes
u = ctypes.WinDLL('user32', use_last_error=True); g = ctypes.WinDLL('gdi32', use_last_error=True)
class BIH(ctypes.Structure):
    _fields_ = [('biSize', wintypes.DWORD), ('biWidth', wintypes.LONG), ('biHeight', wintypes.LONG), ('biPlanes', wintypes.WORD), ('biBitCount', wintypes.WORD), ('biCompression', wintypes.DWORD), ('biSizeImage', wintypes.DWORD), ('biX', wintypes.LONG), ('biY', wintypes.LONG), ('biClrUsed', wintypes.DWORD), ('biClrImportant', wintypes.DWORD)]
def bitblt(h):
    rc = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(rc)); pt = wintypes.POINT(0, 0); u.ClientToScreen(h, ctypes.byref(pt))
    w, hh = rc.right - rc.left, rc.bottom - rc.top
    sdc = u.GetDC(0); mdc = g.CreateCompatibleDC(sdc); bm = g.CreateCompatibleBitmap(sdc, w, hh); g.SelectObject(mdc, bm)
    g.BitBlt(mdc, 0, 0, w, hh, sdc, pt.x, pt.y, 0x00CC0020)
    bi = BIH(ctypes.sizeof(BIH), w, -hh, 1, 32, 0, 0, 0, 0, 0, 0); buf = ctypes.create_string_buffer(w * hh * 4)
    g.GetDIBits(mdc, bm, 0, hh, buf, ctypes.byref(bi), 0); g.DeleteObject(bm); g.DeleteDC(mdc); u.ReleaseDC(0, sdc)
    d = buf.raw; rgb = bytearray(w * hh * 3); rgb[0::3] = d[2::4]; rgb[1::3] = d[1::4]; rgb[2::3] = d[0::4]
    return w, hh, bytes(rgb)
