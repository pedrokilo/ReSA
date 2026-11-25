# -*- coding: utf-8 -*-
# Vigilante de skinning de CJ: biblioteca comun a vigilante_skin.py (uso del usuario mientras juega) y sonda_skin.py (pruebas guiadas).
# Funciona con los TRES ejecutables (standalone, EXE original + ASI, EXE original puro) porque solo lee memoria por direcciones del EXE original
# (el standalone las respeta: /BASE 0x400000 /FIXED) y captura la ventana. Hilos:
#   MEM   : lee el bufer de matrices de hueso del skin (puntero en 0xC978AC, 0x4000 bytes: lo reescribe cada atomo skinneado que se dibuja) y la jerarquia
#           RpHAnim del jugador (clump -> atomo con skin -> pMatrixArray) a la mayor frecuencia posible; detecta NaN/inf/|valor| enorme/escala anomala.
#   CAP   : BitBlt de la ventana a la mayor cadencia, anillo circular de N segundos; detecta fotogramas "A-B-A" (un fotograma distinto de los vecinos).
#   LOG   : sigue logs\log.log (si existe) y reacciona a las lineas [SKINVIG] ANOMALIA que escribe el modo en el juego (GTASA_SKIN_VIGILANCIA=1).
# Cada evento guarda los fotogramas de los POST segundos anteriores y posteriores (PNG) + eventos.jsonl con matrices.
import ctypes, struct, sys, time, os, json, math, threading, collections, zlib, re
from ctypes import wintypes
k = ctypes.WinDLL('kernel32', use_last_error=True); u = ctypes.WinDLL('user32', use_last_error=True); g = ctypes.WinDLL('gdi32', use_last_error=True)
k.OpenProcess.restype = wintypes.HANDLE
k.VirtualAllocEx.restype = ctypes.c_void_p; k.VirtualAllocEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_size_t, wintypes.DWORD, wintypes.DWORD]
k.CreateRemoteThread.restype = wintypes.HANDLE
k.CreateRemoteThread.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p, ctypes.c_void_p, wintypes.DWORD, ctypes.c_void_p]
k.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
k.WriteProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
try: u.SetProcessDPIAware()
except Exception: pass

LIM_ABS = 2000.0
ESC_MIN, ESC_MAX = 0.05, 8.0
class Mem:
    def __init__(s, pid): s.pid = pid; s.h = k.OpenProcess(0x1F0FFF, False, pid)
    def rd(s, a, n):
        b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h, a, b, n, ctypes.byref(r)) and r.value == n else None
    def wr(s, a, d): r = ctypes.c_size_t(); return k.WriteProcessMemory(s.h, a, d, len(d), ctypes.byref(r))
    def u32(s, a): b = s.rd(a, 4); return struct.unpack('<I', b)[0] if b else 0
    def i32(s, a): b = s.rd(a, 4); return struct.unpack('<i', b)[0] if b else 0
    def u8(s, a): b = s.rd(a, 1); return b[0] if b else 0
    def f32(s, a): b = s.rd(a, 4); return struct.unpack('<f', b)[0] if b else float('nan')
    def llamar(s, dir_, *args, espera=30000):
        """llama a una funcion CDECL del proceso en un hilo remoto (trampolin: push args; call; add esp; ret 4). Devuelve eax o None"""
        code = b''.join(b'\x68' + struct.pack('<I', a & 0xFFFFFFFF) for a in reversed(args)) + b'\xB8' + struct.pack('<I', dir_) + b'\xFF\xD0'
        code += b'\x83\xC4' + bytes([4 * len(args)]) + b'\xC2\x04\x00'
        p = k.VirtualAllocEx(s.h, None, 64, 0x3000, 0x40)
        if not p: return None
        s.wr(p, code); h = k.CreateRemoteThread(s.h, None, 0, p, None, 0, None)
        if not h: return None
        k.WaitForSingleObject(h, espera); c = wintypes.DWORD(); k.GetExitCodeThread(h, ctypes.byref(c)); k.CloseHandle(h); return c.value

def anomalo_buf(vals, lim=LIM_ABS):
    """vals: tupla de floats. Devuelve (indice_primero_malo, motivo) o None. Rapido: usa sum/max/min y solo si hay indicio recorre."""
    s = sum(vals)
    if s != s or s in (float('inf'), float('-inf')):
        for i, v in enumerate(vals):
            if not math.isfinite(v): return i, 'no_finito'
    mx = max(vals); mn = min(vals)
    if mx > lim or mn < -lim:
        for i, v in enumerate(vals):
            if abs(v) > lim: return i, 'enorme'
    return None

def anomalo_mat16(vals):
    """vals: n*16 floats de RwMatrix de MUNDO (traslacion de miles de unidades es normal). (indice, motivo) o None"""
    vals = list(vals); vals[3::4] = [0.0] * (len(vals) // 4)   # los floats +0xC,+0x1C,+0x2C,+0x3C son relleno (flags/pad) y pueden tener basura 0xCCCCCCCC
    s_ = sum(vals)
    if s_ != s_ or s_ in (float('inf'), float('-inf')):
        for i, v in enumerate(vals):
            if not math.isfinite(v): return i, 'no_finito'
    if max(vals) > 8.0 or min(vals) < -8.0:       # solo puede haber valores >8 en la traslacion (indices 12..14 de cada matriz)
        for i, v in enumerate(vals):
            if abs(v) > 1e5: return i, 'enorme'
            if abs(v) > 8.0 and (i % 16) not in (12, 13, 14): return i, 'rotacion_enorme'
    return None

def escribir_png(ruta, w, h, bgra):
    rgb = bytearray(w * h * 3); rgb[0::3] = bgra[2::4]; rgb[1::3] = bgra[1::4]; rgb[2::3] = bgra[0::4]
    st = w * 3; raw = bytearray()
    for y in range(h): raw.append(0); raw += rgb[y * st:(y + 1) * st]
    def ch(t, d): return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
    with open(ruta, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n' + ch(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) + ch(b'IDAT', zlib.compress(bytes(raw), 1)) + ch(b'IEND', b''))

class BIH(ctypes.Structure):
    _fields_ = [('biSize', wintypes.DWORD), ('biWidth', wintypes.LONG), ('biHeight', wintypes.LONG), ('biPlanes', wintypes.WORD), ('biBitCount', wintypes.WORD), ('biCompression', wintypes.DWORD), ('biSizeImage', wintypes.DWORD), ('biX', wintypes.LONG), ('biY', wintypes.LONG), ('biClrUsed', wintypes.DWORD), ('biClrImportant', wintypes.DWORD)]
WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
def buscar_ventana(pid):
    r = []
    def cb(h, l):
        p = wintypes.DWORD(0); u.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and u.IsWindowVisible(h):
            rc = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(rc))
            if rc.right - rc.left >= 300: r.append(h)
        return True
    u.EnumWindows(WNDENUMPROC(cb), 0); return r[0] if r else None
class Grabber:
    def __init__(s, hwnd): s.h = hwnd; s.w = s.hh = 0; s.sdc = None
    def _crear(s, w, hh):
        s.liberar(); s.w, s.hh = w, hh
        s.sdc = u.GetDC(0); s.mdc = g.CreateCompatibleDC(s.sdc); s.bm = g.CreateCompatibleBitmap(s.sdc, w, hh); g.SelectObject(s.mdc, s.bm)
        s.bi = BIH(ctypes.sizeof(BIH), w, -hh, 1, 32, 0, 0, 0, 0, 0, 0); s.buf = ctypes.create_string_buffer(w * hh * 4)
    def liberar(s):
        if s.sdc: g.DeleteObject(s.bm); g.DeleteDC(s.mdc); u.ReleaseDC(0, s.sdc); s.sdc = None
    def grab(s):
        rc = wintypes.RECT(); u.GetClientRect(s.h, ctypes.byref(rc)); pt = wintypes.POINT(0, 0); u.ClientToScreen(s.h, ctypes.byref(pt))
        w, hh = rc.right - rc.left, rc.bottom - rc.top
        if w < 100 or hh < 100: return None
        if (w, hh) != (s.w, s.hh) or not s.sdc: s._crear(w, hh)
        g.BitBlt(s.mdc, 0, 0, w, hh, s.sdc, pt.x, pt.y, 0x00CC0020)
        g.GetDIBits(s.mdc, s.bm, 0, hh, s.buf, ctypes.byref(s.bi), 0)
        return w, hh, s.buf.raw

class Vigilante:
    def __init__(s, pid, modo, evid, ring_s=3.0, post_s=3.0, log=None, mapa=None, P=print, max_eventos=150, imagen=True):
        s.pid = pid; s.modo = modo; s.evid = evid; s.ring_s = ring_s; s.post_s = post_s; s.logruta = log; s.P = P; s.max_eventos = max_eventos; s.usar_imagen = imagen
        os.makedirs(evid, exist_ok=True)
        s.m = Mem(pid); s.parar = threading.Event(); s.lock = threading.Lock()
        s.ring = collections.deque(); s.eventos = []; s.pendientes = []; s.ultimo_ev = {}
        s.stats = {'lecturas_buf': 0, 'lecturas_jug': 0, 'max_abs_buf': 0.0, 'max_abs_jug': 0.0, 'max_esc_jug': 0.0, 'min_esc_jug': 9.0, 'frames': 0, 'frames_distintos': 0, 'aba': 0, 'anom_buf': 0, 'anom_jug': 0, 'anom_log': 0, 'anom_vigil': 0}
        s.vig_addr = None
        if mapa and os.path.exists(mapa):
            for l in open(mapa, encoding='latin-1'):
                if 'g_SkinVigilanciaEstado' in l:
                    t = l.split()
                    try: s.vig_addr = int(t[2], 16); break
                    except Exception: pass
        s.t_peds = 0.0; s.vig_prev = 0; s.hilos = []; s.t0 = time.time(); s.nev = 0; s.cola_png = collections.deque()
    # ------------------------------------------------------------ utilidades de jugador
    def jugador(s):
        foco = s.m.u8(0xB7CD74); return s.m.u32(0xB7CD98 + foco * 0x190)
    def jerarquia_de(s, ped):
        """(ptr jerarquia, numNodes, pMatrixArray) del primer atomo con skin del clump del ped"""
        clump = s.m.u32(ped + 0x18) if ped else 0
        if not clump: return None
        off = s.m.u32(0xC978A4)
        if off == 0 or off > 0x400: return None
        cab = clump + 8; p = s.m.u32(cab); n = 0
        while p and p != cab and n < 32:
            at = p - 0x40; h = s.m.u32(at + off)
            if h and 0 < s.m.i32(h + 4) <= 256: return h, s.m.i32(h + 4), s.m.u32(h + 8)
            p = s.m.u32(p); n += 1
        return None
    def vigilar_vehiculo(s, v):
        """la bici/moto/coche del jugador no tiene esqueleto: se vigila la matriz de la entidad y la LTM del frame de cada atomo de su clump
        (finitas, sin escala anomala). Si se estirase la bici seria por esto (matriz de frame/atomo), no por skin."""
        if not v: return
        mt = s.m.u32(v + 0x14); clump = s.m.u32(v + 0x18)
        mats = []
        if mt:
            d = s.m.rd(mt, 0x40)
            if d: mats.append(('entidad', d))
        if clump:
            cab = clump + 8; p = s.m.u32(cab); n = 0
            while p and p != cab and n < 40:
                fr = s.m.u32(p - 0x40 + 4)
                if fr:
                    d = s.m.rd(fr + 0x50, 0x40)
                    if d: mats.append(('ltm_atomo_%d' % n, d))
                p = s.m.u32(p); n += 1
        for nombre, d in mats:
            vals = struct.unpack('<16f', d); r = anomalo_mat16(vals)
            if r is not None:
                s.stats['anom_veh'] = s.stats.get('anom_veh', 0) + 1
                s.evento('vehiculo_matriz:' + nombre, {'indice': r[0], 'motivo': r[1], 'matriz': vals}, d); continue
            s.stats['lecturas_veh'] = s.stats.get('lecturas_veh', 0) + 1
            for c in (0, 4, 8):
                l = math.sqrt(vals[c] ** 2 + vals[c + 1] ** 2 + vals[c + 2] ** 2)
                s.stats['max_esc_veh'] = max(s.stats.get('max_esc_veh', 0.0), l)
                if l > 4.0 or l < 0.05:
                    s.evento('vehiculo_escala:' + nombre, {'columna': c // 4, 'longitud': l, 'matriz': vals})
    def animaciones(s, clump):
        """[(animId, blend, tiempo, velocidad, timestep, flags)] de la lista de asociaciones del clump (CAnimBlendClumpData en clump+gClumpOffset)"""
        off = s.m.u32(0xB5F878)
        if not clump or off == 0 or off > 0x400: return None
        cd = s.m.u32(clump + off)
        if not cd: return None
        r = []; p = s.m.u32(cd); n = 0
        while p and p != cd and n < 24:
            d = s.m.rd(p, 0x2C)
            if not d: break
            blend, delta, t, sp, ts = struct.unpack('<5f', d[0x14:0x28]); aid, fl = struct.unpack('<hH', d[0x28:0x2C])
            r.append((aid, round(blend, 3) if math.isfinite(blend) else str(blend), round(t, 3) if math.isfinite(t) else str(t), round(sp, 3) if math.isfinite(sp) else str(sp), str(ts) if not math.isfinite(ts) else round(ts, 4), fl))
            p = struct.unpack('<I', d[:4])[0]; n += 1
        return r
    def framedata(s, clump, idx):
        """AnimBlendFrameData (0x18 bytes) del frame idx: flags, BonePos, KeyFrame*, BoneTag, y los 0x24 bytes del keyframe apuntado"""
        off = s.m.u32(0xB5F878)
        if not clump or off == 0 or off > 0x400: return None
        cd = s.m.u32(clump + off)
        if not cd: return None
        n = s.m.i32(cd + 8); fds = s.m.u32(cd + 0x10)
        if not fds or idx >= n: return {'n': n}
        d = s.m.rd(fds + idx * 0x18, 0x18)
        if not d: return None
        fl = d[0]; bp = struct.unpack('<3f', d[4:16]); kf, tag = struct.unpack('<II', d[0x10:0x18])
        kb = s.m.rd(kf, 0x24) if kf else None
        return {'n': n, 'flags': hex(fl), 'ignoraOrient': bool(fl & 2), 'ignoraTrans': bool(fl & 4), 'comprimido': bool(fl & 0x40), 'BonePos': bp, 'KeyFrame': hex(kf), 'BoneTag': tag, 'kf_floats': struct.unpack('<9f', kb) if kb else None, 'kf_hex': kb.hex() if kb else None}
    def vigilar_peds(s):
        pp = s.m.u32(0xB74490)
        if not pp: return
        objs = s.m.u32(pp); fl = s.m.rd(s.m.u32(pp + 4), s.m.i32(pp + 8)) or b''
        jug = s.jugador()
        for i in range(len(fl)):
            if fl[i] & 0x80: continue
            e = objs + i * 0x7C4
            jh = s.jerarquia_de(e)
            if not jh: continue
            h, n, pm = jh; dm = s.m.rd(pm, n * 64) if pm else None
            if not dm: continue
            s.stats['lecturas_peds'] = s.stats.get('lecturas_peds', 0) + 1
            vals = struct.unpack('<%df' % (n * 16), dm); r = anomalo_mat16(vals)
            if r is None: continue
            s.stats['anom_peds'] = s.stats.get('anom_peds', 0) + 1
            clump = s.m.u32(e + 0x18)
            s.evento('ped_corrupto:%x' % e, {'ped': hex(e), 'es_jugador': e == jug, 'modelo': struct.unpack('<H', s.m.rd(e + 0x22, 2))[0], 'estado': s.m.u32(e + 0x530), 'hueso': r[0] // 16, 'motivo': r[1],
                'valor': vals[r[0]], 'anims': s.animaciones(clump), 'framedata': s.framedata(clump, r[0] // 16), 'hueso_matriz': vals[(r[0] // 16) * 16:(r[0] // 16) * 16 + 16]}, dm)
    # ------------------------------------------------------------ eventos
    def evento(s, motivo, extra=None, buf=None):
        t = time.time()
        with s.lock:
            clave = motivo.split(':')[0]
            if t - s.ultimo_ev.get(clave, 0) < 6.0: return
            s.ultimo_ev[clave] = t
            if s.nev >= s.max_eventos: return
            s.nev += 1; n = s.nev
            ev = {'n': n, 't': t, 't_rel': round(t - s.t0, 2), 'motivo': motivo, 'extra': extra}
            s.eventos.append(ev)
            carpeta = os.path.join(s.evid, 'evento_%03d_%s' % (n, time.strftime('%H%M%S')))
            os.makedirs(carpeta, exist_ok=True)
            ev['carpeta'] = carpeta
            previos = [f for f in s.ring if f[0] >= t - s.ring_s]; previos = previos[::max(1, len(previos) // 20)]
            s.pendientes.append({'t': t, 'hasta': t + s.post_s, 'carpeta': carpeta, 'previos': previos, 'guardados': set()})
            if buf is not None: open(os.path.join(carpeta, 'bufer_huesos.bin'), 'wb').write(buf)
            with open(os.path.join(s.evid, 'eventos.jsonl'), 'a', encoding='utf-8') as f: f.write(json.dumps(ev, default=str) + '\n')
        s.P('EVENTO %d: %s %s -> %s' % (n, motivo, json.dumps(extra, default=str)[:300], carpeta))
    # ------------------------------------------------------------ hilo MEM
    def hilo_mem(s):
        ant_buf = None
        while not s.parar.is_set():
            try:
                p = s.m.u32(0xC978AC)
                if p:
                    d = s.m.rd(p, 0x4000)
                    if d:
                        s.stats['lecturas_buf'] += 1
                        if d != ant_buf:
                            ant_buf = d; vals = struct.unpack('<4096f', d); r = anomalo_buf(vals)
                            if r is None:
                                s.stats['max_abs_buf'] = max(s.stats['max_abs_buf'], max(max(vals), -min(vals)))
                            else:
                                s.stats['anom_buf'] += 1
                                i, mot = r
                                s.evento('bufer_hueso:' + mot, {'indice': i, 'hueso_vs': i // 12, 'hueso_x87': i // 16, 'valores': [vals[j] for j in range(max(0, i - 6), min(4096, i + 6))]}, d)
                ped = s.jugador()
                if ped:
                    jh = s.jerarquia_de(ped)
                    if jh:
                        h, n, pm = jh; dm = s.m.rd(pm, n * 64) if pm else None
                        if dm:
                            s.stats['lecturas_jug'] += 1
                            vals = struct.unpack('<%df' % (n * 16), dm); r = anomalo_mat16(vals)
                            if r is not None:
                                s.stats['anom_jug'] += 1; i, mot = r
                                s.evento('jerarquia_jugador:' + mot, {'hueso': i // 16, 'campo': i % 16, 'valor': vals[i], 'n': n}, dm)
                            else:
                                s.stats['max_abs_jug'] = max(s.stats['max_abs_jug'], max(max(vals), -min(vals)))
                                for b in range(n):
                                    m_ = vals[b * 16:b * 16 + 16]
                                    for c in (0, 4, 8):   # right/up/at: 3 floats al principio de cada fila de 4
                                        l = math.sqrt(m_[c] ** 2 + m_[c + 1] ** 2 + m_[c + 2] ** 2)
                                        if l > s.stats['max_esc_jug']: s.stats['max_esc_jug'] = l
                                        if l < s.stats['min_esc_jug']: s.stats['min_esc_jug'] = l
                                        if l > ESC_MAX or l < ESC_MIN:
                                            s.evento('escala_jugador', {'hueso': b, 'longitud': l, 'matriz': m_})
                if ped:
                    s.vigilar_vehiculo(s.m.u32(ped + 0x58C))
                if time.time() - s.t_peds > 0.1:
                    s.t_peds = time.time(); s.vigilar_peds()
                if s.vig_addr:
                    v = s.m.rd(s.vig_addr, 24)
                    if v:
                        magic, an, uf, comp, mxa, mxe = struct.unpack('<IIIIff', v)
                        if magic == 0x47564B53:
                            s.stats['vigil'] = {'comprobaciones': comp, 'anomalias': an, 'maxAbs': mxa, 'maxEscala': mxe}
                            if an != s.vig_prev:
                                s.vig_prev = an; s.stats['anom_vigil'] += 1; s.evento('vigilancia_juego', {'anomalias': an, 'fotograma': uf})
            except Exception as ex:
                s.P('MEM excepcion', repr(ex)); time.sleep(0.5)
            time.sleep(0.0005)
    # ------------------------------------------------------------ hilo CAP
    def hilo_cap(s):
        gr = None; offs = None; prev = []; frames = 0; ult_f = None
        while not s.parar.is_set():
            try:
                if not gr or not u.IsWindow(gr.h):
                    hw = buscar_ventana(s.pid)
                    if not hw: time.sleep(0.5); continue
                    gr = Grabber(hw)
                r = gr.grab()
                if not r: time.sleep(0.05); continue
                w, hh, d = r; t = time.time(); frames += 1; s.stats['frames'] = frames
                if offs is None or offs[0] != (w, hh):
                    gx, gy = 16, 12; offs = [(w, hh), []]
                    for cy in range(gy):
                        for cx in range(gx):
                            offs[1].append([((cy * hh // gy + 3 + 9 * j) % hh * w + (cx * w // gx + 3 + 11 * j) % w) * 4 + 1 for j in range(25)])
                cell = [sum(d[o] for o in oo) // 25 for oo in offs[1]]
                if ult_f is not None and cell == ult_f:
                    s.escritura_pendiente(t); continue
                ult_f = cell; s.stats['frames_distintos'] += 1
                s.ring.append((t, w, hh, d, cell))
                while s.ring and s.ring[0][0] < t - s.ring_s - 0.5: s.ring.popleft()
                prev.append((t, cell)); prev = prev[-3:]
                if len(prev) == 3:
                    a, b, c = prev[0][1], prev[1][1], prev[2][1]
                    d01 = sum(1 for x, y in zip(a, b) if abs(x - y) > 28); d12 = sum(1 for x, y in zip(b, c) if abs(x - y) > 28); d02 = sum(1 for x, y in zip(a, c) if abs(x - y) > 28)
                    if d01 >= 3 and d12 >= 3 and d02 * 3 <= min(d01, d12):
                        s.stats['aba'] += 1; s.evento('imagen_ABA', {'celdas_01': d01, 'celdas_12': d12, 'celdas_02': d02, 'media': sum(b) / len(b)})
                s.escritura_pendiente(t)
            except Exception as ex:
                s.P('CAP excepcion', repr(ex)); time.sleep(0.5)
    def escritura_pendiente(s, t):
        with s.lock:
            for pe in list(s.pendientes):
                fin = t >= pe['hasta']
                nuevos = [f for f in s.ring if pe['t'] <= f[0] <= pe['hasta'] and int(f[0] * 1e6) % 2 == 0]
                for f in pe['previos'] + nuevos:
                    if f[0] not in pe['guardados']:
                        pe['guardados'].add(f[0])
                        s.cola_png.append((os.path.join(pe['carpeta'], 'f_%+.3f.png' % (f[0] - pe['t'])), f[1], f[2], f[3]))
                if fin: s.pendientes.remove(pe)
    def hilo_png(s):
        while not s.parar.is_set() or s.cola_png:
            if not s.cola_png: time.sleep(0.05); continue
            ruta, w, hh, d = s.cola_png.popleft()
            try: escribir_png(ruta, w, hh, d)
            except Exception as ex: s.P('PNG excepcion', repr(ex))
    # ------------------------------------------------------------ hilo LOG
    def hilo_log(s):
        if not s.logruta: return
        pos = 0
        while not s.parar.is_set():
            try:
                if os.path.exists(s.logruta):
                    with open(s.logruta, 'rb') as f:
                        f.seek(pos); dat = f.read(); pos += len(dat)
                    for l in dat.decode('utf-8', 'replace').splitlines():
                        if '[SKINVIG]' in l and ('ANOMALIA' in l or 'escala alta' in l):
                            s.stats['anom_log'] += 1; s.evento('log_juego', {'linea': l[:300]})
            except Exception: pass
            time.sleep(0.2)
    def iniciar(s):
        for f in (s.hilo_mem, s.hilo_png, s.hilo_log) + ((s.hilo_cap,) if s.usar_imagen else ()):
            t = threading.Thread(target=f, daemon=True); t.start(); s.hilos.append(t)
    def detener(s):
        t = time.time() + s.post_s + 0.5
        while s.pendientes and time.time() < t: time.sleep(0.2)
        s.parar.set()
        for t in s.hilos: t.join(timeout=30)
        json.dump({'stats': s.stats, 'eventos': s.eventos, 'duracion_s': time.time() - s.t0}, open(os.path.join(s.evid, 'vigilante_resumen.json'), 'w'), indent=1, default=str)
