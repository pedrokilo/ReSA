# Capturas de ventana completa (BitBlt/PrintWindow, DPI-aware, tamano validado) del juego durante el banco en juego de los subtitulos del mundo libre.
# Uso: python -I frs_captura.py <carpeta de salida> <log del banco> --pid N --limite S
# Saca una captura cuando el log del banco (free_roam_subs_juego.log) escribe una linea nueva MOSTRADO (a +0.3 s y +1.0 s) y otra cuando escribe FIN DE VOZ (a +0.1 s, a la mitad del
# tiempo de lectura y a +ReadingTime+0.6 s). Solo lee el log y captura la ventana del PID indicado.
import ctypes, os, struct, sys, time, zlib, re
from ctypes import wintypes
ctypes.windll.user32.SetProcessDPIAware()
u = ctypes.WinDLL('user32', use_last_error=True); g = ctypes.WinDLL('gdi32', use_last_error=True)
WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
pid = int(arg('--pid', '0')); limite = float(arg('--limite', '150')); sal = sys.argv[1]; log = sys.argv[2]
def ventana():
    r = []
    def cb(h, l):
        p = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and u.IsWindowVisible(h):
            c = ctypes.create_unicode_buffer(128); u.GetClassNameW(h, c, 128)
            if c.value == 'Grand theft auto San Andreas': r.append(h)
        return True
    u.EnumWindows(WNDENUMPROC(cb), 0); return r[0] if r else None
class BIH(ctypes.Structure):
    _fields_ = [('biSize', wintypes.DWORD), ('biWidth', wintypes.LONG), ('biHeight', wintypes.LONG), ('biPlanes', wintypes.WORD), ('biBitCount', wintypes.WORD), ('biCompression', wintypes.DWORD),
                ('biSizeImage', wintypes.DWORD), ('biX', wintypes.LONG), ('biY', wintypes.LONG), ('biClrUsed', wintypes.DWORD), ('biClrImportant', wintypes.DWORD)]
def grab():
    h = ventana()
    if not h: return None
    rc = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(rc)); w, hh = rc.right - rc.left, rc.bottom - rc.top
    if w < 320 or hh < 240: return {'error': 'tamano invalido %dx%d' % (w, hh)}
    pt = wintypes.POINT(0, 0); u.ClientToScreen(h, ctypes.byref(pt))
    sdc = u.GetDC(0); mdc = g.CreateCompatibleDC(sdc); bm = g.CreateCompatibleBitmap(sdc, w, hh); g.SelectObject(mdc, bm)
    ok = u.PrintWindow(h, mdc, 3)
    if not ok: g.BitBlt(mdc, 0, 0, w, hh, sdc, pt.x, pt.y, 0x00CC0020)
    bi = BIH(ctypes.sizeof(BIH), w, -hh, 1, 32, 0, 0, 0, 0, 0, 0); buf = ctypes.create_string_buffer(w * hh * 4)
    g.GetDIBits(mdc, bm, 0, hh, buf, ctypes.byref(bi), 0)
    g.DeleteObject(bm); g.DeleteDC(mdc); u.ReleaseDC(0, sdc)
    data = buf.raw
    if len(data) != w * hh * 4: return {'error': 'buffer de tamano inesperado'}
    return {'w': w, 'h': hh, 'data': data, 'pw': bool(ok), 't': time.time()}
def guardar(s, ruta):
    w, hh, data = s['w'], s['h'], s['data']
    raw = bytearray()
    for y in range(hh):
        raw.append(0); row = data[y * w * 4:(y + 1) * w * 4]
        raw += bytes(sum(([row[i + 2], row[i + 1], row[i]] for i in range(0, w * 4, 4)), []))
    def ch(t, d): return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
    open(ruta, 'wb').write(b'\x89PNG\r\n\x1a\n' + ch(b'IHDR', struct.pack('>IIBBBBB', w, hh, 8, 2, 0, 0, 0)) + ch(b'IDAT', zlib.compress(bytes(raw), 3)) + ch(b'IEND', b''))
snaps = []
t0 = time.time(); leidas = 0; pend = []; n = 0
def P(*a): print('[%6.1f]' % (time.time() - t0), *a, flush=True)
P('esperando ventana de pid', pid)
while time.time() - t0 < limite + 45:
    time.sleep(0.05)
    if os.path.exists(log):
        try: lineas = open(log, encoding='latin1').read().split('\n')
        except Exception: lineas = []
        for l in lineas[leidas:]:
            if l.startswith('MOSTRADO'): pend += [(time.time() + 0.3, 'mostrado_a'), (time.time() + 1.0, 'mostrado_b')]
            elif l.startswith('FIN DE VOZ'):
                m = re.search(r'se retira en (\d+) ms', l); rt = int(m.group(1)) / 1000.0 if m else 0.2
                pend += [(time.time() + 0.05, 'finvoz'), (time.time() + max(0.05, rt / 2), 'lectura_mitad'), (time.time() + rt + 0.6, 'despues')]
            elif '--paso' in sys.argv and l.startswith('PASO') and '(suena)' in l: pend += [(time.time() + 0.4, 'suena_a'), (time.time() + 1.0, 'suena_b')]
            elif l.startswith('RETIRADO') or l.startswith('CEDIDO'): pend += [(time.time() + 0.1, 'retirado')]
        leidas = len(lineas) - 1 if lineas and lineas[-1] == '' else len(lineas)
        if any(x.strip() == 'FIN' for x in lineas):
            if not pend: break
    for e in sorted(pend):
        if time.time() >= e[0]:
            pend.remove(e); n += 1
            s = grab(); r = None
            if s and 'data' in s: snaps.append((os.path.join(sal, 'cap_%02d_%s.png' % (n, e[1])), s)); r = {'ancho': s['w'], 'alto': s['h'], 'printwindow': s['pw']}
            else: r = s
            P('captura', n, e[1], r, 'retraso sobre lo programado %.2f s' % (time.time() - e[0]))
    if pid and not ventana() and time.time() - t0 > 30 and not pend and os.path.exists(log): pass
for ruta, s in snaps: guardar(s, ruta)
P('fin; capturas:', n)
