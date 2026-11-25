# Sonda de la comparacion original vs standalone. Se lanza junto al juego (ejecutar_comparar.ps1) con el PID del proceso propio.
# Mismo guion en los dos modos (orig = gta_sa.exe original sin ASI del proyecto, std = gta_sa_standalone.exe, asi = EXE original + gta_reversed.asi):
#   - arranque: (orig) pulsa Enter en los logos y navega el menu con teclado hasta cargar la ranura 2; (std/asi) FastLoader carga la ranura 2.
#   - fase interior : la casa de CJ (partida cargada, sin tocar) y el ropero (se pisa el marcador CHANGER hasta area 14).
#   - fase exterior : se sale de la casa y, en cada punto, para cada hora y clima forzados por memoria (CClock 0xB70153/52, CWeather::ForcedWeatherType 0xC81318)
#     se fija la orientacion de la camara (yaw/pitch objetivo con el raton leyendo el vector frente de TheCamera 0xB6F9AC) y se captura el cliente completo.
# Uso: python -I comparar.py <salida> --pid P --modo orig|std|asi [--fase interior|exterior|todo] [--horas 6,12,18,0] [--climas 0,1,4,8] [--puntos 0,1,2]
import sys, os, time, json, struct, math, ctypes
aqui = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(aqui, '..', 'FnTraf')); sys.path.insert(0, aqui)
ctypes.windll.user32.SetProcessDPIAware()
import sonda_traf as S, teclas, capt2, png_util
captura, u, wintypes = S.captura, S.u, S.wintypes
u.SetWindowPos.argtypes = [wintypes.HWND, wintypes.HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, wintypes.UINT]
def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
sal = sys.argv[1]; os.makedirs(sal, exist_ok=True)
MODO = arg('--modo', 'std'); FASE = arg('--fase', 'todo')
HORAS = [int(x) for x in arg('--horas', '6,12,18,0').split(',')]
CLIMAS = [int(x) for x in arg('--climas', '0,1,4,8').split(',')]
PUNTOS_SEL = [int(x) for x in arg('--puntos', '0,1,2').split(',')]
YAW_OBJ = float(arg('--yaw', '90')); PITCH_OBJ = float(arg('--pitch', '0.10'))
ASENTAR = float(arg('--asentar', '4')); ESPERA_PUNTO = float(arg('--espera-punto', '14'))
PUNTOS = [(2652.5, -1719.9, 13.8), (2672.1, -1652.5, 11.5), (2658.1, -1433.7, 31.5)]   # (2), (3), (4) del encargo
LOG = open(os.path.join(sal, 'comparar.log'), 'w', encoding='utf-8')
def P(*a):
    s = ' '.join(str(x) for x in a); print(s, flush=True); LOG.write(s + '\n'); LOG.flush()
RES = open(os.path.join(sal, 'resultados.jsonl'), 'a', encoding='utf-8')
def volcado():
    # CTimeCycle::m_CurrentColours (0xB7C4A0), gamma (0xC92134), CMenuManager (0xBA6748): bytes crudos para comparar original y standalone
    return {'timecycle': (m.rd(0xB7C4A0, 0x100) or b'').hex(), 'gamma': (m.rd(0xC92134, 0x20) or b'').hex(), 'menu': (m.rd(0xBA6748, 0x180) or b'').hex()}
def R(d):
    if d.get('tipo') in ('interior', 'exterior', 'cielo'): d['mem'] = volcado()
    RES.write(json.dumps(d) + chr(10)); RES.flush()

p = S.FORZAR_PID
m = S.M(p)
# --- tiempos de proceso
class FT(ctypes.Structure): _fields_ = [('lo', wintypes.DWORD), ('hi', wintypes.DWORD)]
def creacion(h):
    a, b, c, d = FT(), FT(), FT(), FT()
    ctypes.windll.kernel32.GetProcessTimes(h, ctypes.byref(a), ctypes.byref(b), ctypes.byref(c), ctypes.byref(d))
    return ((a.hi << 32) | a.lo) / 1e7 - 11644473600
T_PROC = creacion(m.h)
def desde_inicio(): return time.time() - T_PROC

def vivo(): return S.pid('gta_sa') is not None
def mantener(): m.wr(0x8D621C, struct.pack('<I', 1))      # ForegroundApp = 1
def ped_mat():
    ped, _ = S.jugador(m); return ped, (m.u32(ped + 0x14) if ped else 0)
def pos():
    return S.jugador(m)[1]
def tp(q):
    ped, mat = ped_mat()
    if mat:
        for i, v in enumerate(q): m.wr(mat + 0x30 + 4 * i, struct.pack('<f', v))
def foco():
    h = captura.buscar()
    if h: u.ShowWindow(h, 9); u.SetWindowPos(wintypes.HWND(h), wintypes.HWND(-1), 0, 0, 0, 0, 0x0001 | 0x0002 | 0x0040); u.SetForegroundWindow(h)
    return h

# --- arranque
CAPS8 = []; GUARD8 = []
def _ls_dir(a):
    cargar_mapa(); return dir_(a)
DIR_LS = {}
def arrancar():
    DIR_LS['activo'] = _ls_dir(0xBAB318); DIR_LS['pct'] = _ls_dir(0xBAB330)
    t8 = t9 = None; h = None; ult = 0; gs_ant = -1; hist = []
    while vivo() and desde_inicio() < 400:
        gs = m.u32(0xC8D4C0)
        if gs != gs_ant: hist.append((round(desde_inicio(), 1), gs)); gs_ant = gs
        if gs == 8 and t8 is None: t8 = desde_inicio()
        if gs == 8 and arg('--cap-gs8', '0') == '1':   # pantalla de carga: muestrea rapido el brillo de la ventana mientras gGameState = 8 y guarda el fotograma mas claro y unos pocos mas
            hh = captura.buscar()
            if hh:
                w_, h_, rgb_ = capt2.bitblt(hh); med_ = png_util.media(w_, h_, rgb_)
                CAPS8.append((round(desde_inicio(), 2), [round(x, 1) for x in med_], m.i32(_ls_dir(0x8D0940)), round(m.f32(_ls_dir(0xBAB330)), 1), m.u8(_ls_dir(0xBAB318))))
                if len(GUARD8) < 4 and (not GUARD8 or sum(med_) > 1.4 * GUARD8[-1] + 5): GUARD8.append(sum(med_)); png_util.escribir(os.path.join(sal, 'gs8_%03d.png' % len(CAPS8)), w_, h_, rgb_, 3)
            continue
        ped, mat = ped_mat()
        if gs == 9 and mat and t9 is None: t9 = desde_inicio(); break
        if MODO == 'orig':
            if gs in (2, 4): foco(); teclas.tecla('enter')
            elif gs == 7 and PM.setdefault('t7', time.time()) and time.time() - PM['t7'] > 3.0 and time.time() - ult > 1.8:
                ult = time.time(); foco(); time.sleep(0.2); menu_original(m)
        mantener(); time.sleep(0.1 if gs != 9 else 0.05)
    if CAPS8: P('GS8 muestras (t desde inicio, media RGB):', json.dumps(CAPS8)); R({'tipo': 'gs8', 'muestras': CAPS8})
    P('ARRANQUE gs historial', hist, 't_gs8=%s t_gs9=%s (s desde el inicio del proceso)' % (t8, t9))
    R({'tipo': 'arranque', 'hist': hist, 't_gs8': t8, 't_gs9': t9, 'carga_8_a_9': (t9 - t8) if (t8 and t9) else None})
    return t9 is not None

# --- menu del original (teclado): cada tecla se confirma mirando que el estado del CMenuManager (0xBA6748) cambie; si no cambia se repite (hasta 3 veces);
# al terminar la secuencia sin pasar a gGameState 8 se pulsa Esc y se reinicia.
import hashlib
PM = {'i': 0, 'h': None, 'reint': 0, 't': 0.0}
SECUENCIA_MENU = arg('--menu', 'enter,down,enter,down,enter,enter').split(',')
def menu_original(m):
    ah = hashlib.md5(m.rd(0xBA6748, 0x400) or b'').hexdigest()
    if PM['i'] > 0 and PM['h'] == ah and PM['reint'] < 3 and PM['i'] <= len(SECUENCIA_MENU):
        PM['reint'] += 1; k = SECUENCIA_MENU[PM['i'] - 1]; P('menu: sin reaccion, repito', k, PM['reint'])
    elif PM['i'] < len(SECUENCIA_MENU):
        k = SECUENCIA_MENU[PM['i']]; PM['i'] += 1; PM['reint'] = 0; PM['h'] = ah; P('menu: tecla', k)
    elif time.time() - PM['t'] > 12:
        for _ in range(3): teclas.tecla('esc')
        PM.update({'i': 0, 'h': None, 'reint': 0}); P('menu: reinicio de la secuencia'); PM['t'] = time.time(); return
    else: return
    PM['t'] = time.time(); teclas.tecla(k, 0.15)

# --- medidas por memoria
def pool(ptr):
    pl = m.u32(ptr); flags = m.u32(pl + 4); n = m.i32(pl + 8)
    fl = m.rd(flags, n) or b''
    return [i for i in range(n) if not fl[i] & 0x80], pl, n
def conteos(pj):
    d = {}
    for nom, ptr, tam in (('peds', 0xB74490, 0x7C4), ('veh', 0xB74494, 0x988), ('edif', 0xB74498, 0x38), ('obj', 0xB7449C, 0x19C), ('dummy', 0xB744A0, 0x38)):
        idx, pl, n = pool(ptr); objs = m.u32(pl); cerca = 0; rw = 0
        for i in idx:
            e = objs + i * tam
            if m.u32(e + 0x18): rw += 1
            if nom in ('peds', 'veh') and pj:
                mt = m.u32(e + 0x14)
                if mt and ((m.f32(mt + 0x30) - pj[0]) ** 2 + (m.f32(mt + 0x34) - pj[1]) ** 2) ** 0.5 < 120: cerca += 1
        d[nom] = {'usados': len(idx), 'cap': n, 'con_rw': rw}
        if nom in ('peds', 'veh'): d[nom]['a_menos_de_120m'] = cerca
    d['NumRandomCars'] = m.i32(0x969094); d['MaxCarsInUse'] = m.i32(0x8A5B24)
    return d
def marcadores():
    out = []
    for i in range(32):
        r = m.rd(0xC7DD58 + i * 0xA0, 0xA0)
        if not r: continue
        x, y, z = struct.unpack('<3f', r[0x30:0x3C]); atom = struct.unpack('<I', r[0x48:0x4C])[0]; t = struct.unpack('<H', r[0x50:0x52])[0]
        if t != 257 and atom: out.append({'i': i, 'tipo': t, 'pos': [round(x, 1), round(y, 1), round(z, 1)], 'color': list(r[0x58:0x5C])})
    return out
def texnames(rw):
    out = []; t = m.rd(rw, 1)
    if not t: return out
    atomics = []
    if t[0] == 1: atomics = [rw]
    elif t[0] == 2:
        first = m.u32(rw + 8); l = first; n = 0
        while l and l != rw + 8 and n < 32: atomics.append(l - 0x40); l = m.u32(l); n += 1
    for a in atomics:
        g = m.u32(a + 0x18)
        if not g: out.append('<sin geometria>'); continue
        mats = m.u32(g + 0x20); nm = m.u32(g + 0x24)
        for i in range(min(nm, 64)):
            mt = m.u32(mats + 4 * i); tx = m.u32(mt) if mt else 0
            if tx:
                nmb = m.rd(tx + 0x10, 32); out.append(nmb.split(bytes([0]))[0].decode('latin1') if nmb else '?')
            else: out.append('<sin textura>')
    return out
def entidades_cerca(pj, radio):
    """edificios/objetos/dummies a menos de `radio` m del jugador: modelo, si tiene objeto RenderWare cargado y nombres de textura"""
    res = []
    for nom, ptr, tam in (('edificio', 0xB74498, 0x38), ('objeto', 0xB7449C, 0x19C), ('dummy', 0xB744A0, 0x38)):
        idx, pl, n = pool(ptr); objs = m.u32(pl)
        for i in idx:
            e = objs + i * tam; mt = m.u32(e + 0x14)
            x, y, z = (m.f32(mt + 0x30), m.f32(mt + 0x34), m.f32(mt + 0x38)) if mt else (m.f32(e + 4), m.f32(e + 8), m.f32(e + 12))
            if not (abs(x - pj[0]) < radio and abs(y - pj[1]) < radio and abs(z - pj[2]) < radio): continue
            rw = m.u32(e + 0x18); mod = struct.unpack('<H', m.rd(e + 0x22, 2))[0]
            res.append({'t': nom, 'modelo': mod, 'rw': bool(rw), 'tex': sorted(set(texnames(rw))) if rw else None})
    return res
def fps(dur=3.0):
    f0 = m.u32(0xB7CB4C); g0 = m.u32(0xB7CB84); t0 = time.time()
    while time.time() - t0 < dur and vivo(): mantener(); time.sleep(0.1)
    f1 = m.u32(0xB7CB4C); g1 = m.u32(0xB7CB84); dt = time.time() - t0
    return {'fps': round((f1 - f0) / dt, 1), 'juego_ms_por_s_real': round((g1 - g0) / dt / 1.0, 1)}

# --- camara
def frente(): return [m.f32(0xB6F9AC + 4 * j) for j in range(3)]
def yaw_actual():
    f = frente(); return math.degrees(math.atan2(f[1], f[0]))
def cam_activa():
    return 0xB6F028 + 0x1B4 + (m.u8(0xB6F028 + 0x59) % 3) * 0x238
def frente_cam():
    c = cam_activa(); return [m.f32(c + 0x190 + 4 * j) for j in range(3)]
ORI = {'on': False, 'a': 0.0, 'b': 0.0, 'solo_h': False}
def _escritor():
    while True:
        time.sleep(0.02)
        if not ORI['on']: continue
        try:
            for k in range(3):
                c = 0xB6F028 + 0x1B4 + k * 0x238
                m.wr(c + 0xBC, struct.pack('<f', ORI['a'])); m.wr(c + 0xA4, struct.pack('<f', ORI['a']))
                if not ORI['solo_h']: m.wr(c + 0xAC, struct.pack('<f', ORI['b'])); m.wr(c + 0x8C, struct.pack('<f', ORI['b'])); m.wr(c + 0xA0, struct.pack('<f', ORI['b']))
        except Exception: pass
import threading
threading.Thread(target=_escritor, daemon=True).start()
def orientar(yaw, pitch, dur=2.0, maximo=7.0):
    """fija la orientacion de la camara siguiendo al jugador escribiendo los angulos en las tres CCam de TheCamera (m_fHorizontalAngle +0xBC, m_fVerticalAngle +0xAC,
    m_fTargetBeta +0x8C, m_fTrueAlpha +0xA4, m_fTrueBeta +0xA0; la activa cambia): frente = (-cos(b)cos(a), -cos(b)sin(a), -sin(b)); azimut `yaw` -> a = yaw + pi; frente.z = `pitch` -> b = -asin(pitch).
    Escribe cada 20 ms durante al menos `dur` s y hasta `maximo` s hasta que el azimut leido (vector 0xB6F9AC) quede a menos de 3 grados. Devuelve True si se logro."""
    ORI['a'] = math.radians(yaw) + math.pi; ORI['b'] = -math.asin(max(-0.99, min(0.99, pitch))); ORI['on'] = True
    t0 = time.time(); ok = False
    while vivo():
        mantener(); time.sleep(0.05)
        f = frente(); err = abs((math.degrees(math.atan2(f[1], f[0])) - yaw + 180) % 360 - 180)
        ok = err < 3.0 and -0.4 < f[2] < 0.4
        if time.time() - t0 >= dur and ok: break
        if time.time() - t0 >= maximo: break
    return ok

# --- captura validada: BitBlt del cliente desde la pantalla (proceso DPI-aware, ventana al frente y en primer plano absoluto); PrintWindow devuelve negro con el D3D9
# ventaneado del EXE original, asi que se usa el mismo metodo en los dos modos. Se valida el tamano contra GetClientRect y se anota la media de PrintWindow como contraste.
def traer(h):
    u.ShowWindow(h, 9); u.SetWindowPos(wintypes.HWND(h), wintypes.HWND(-1), 0, 0, 0, 0, 0x0001 | 0x0002 | 0x0040); u.SetForegroundWindow(h)
def capturar(nombre):
    h = captura.buscar()
    if not h: P('sin ventana'); return None
    traer(h); time.sleep(0.15)
    rc = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(rc)); wr = wintypes.RECT(); u.GetWindowRect(h, ctypes.byref(wr))
    cw, ch = rc.right - rc.left, rc.bottom - rc.top
    r = None
    for _ in range(3):
        w, hh, rgb = capt2.bitblt(h); med = png_util.media(w, hh, rgb)
        if w == cw and hh == ch and max(rgb) > 20: break
        time.sleep(0.5)
    ok = (w == cw and hh == ch and max(rgb) > 20)
    png_util.escribir(os.path.join(sal, nombre + '.png'), w, hh, rgb, 6)
    pw = captura.capturar(None, traer=False)
    return {'ancho': w, 'alto': hh, 'media_rgb': [round(x, 1) for x in med], 'cliente': [cw, ch], 'ventana': [wr.right - wr.left, wr.bottom - wr.top], 'valida': bool(ok),
            'metodo': 'BitBlt', 'printwindow_media': pw and pw.get('media_rgb'), 'pos_cliente_pantalla': None}

def sin_dpi(): pass

def sujetar_juego(seg):
    t = time.time()
    while time.time() - t < seg and vivo(): mantener(); time.sleep(0.2)

# --- escenas
def interior():
    P('INTERIOR: casa de CJ; esperando 12 s tras gGameState=9')
    foco(); sujetar_juego(12)
    pj = pos(); P('pos', pj, 'area', m.i32(0xB72914))
    for hora in (None, 12):
        if hora is not None:
            t = time.time()
            while time.time() - t < ASENTAR: mantener(); m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0])); time.sleep(0.2)
            sujetar_juego(1.5)
        nom = 'casa_h%s' % ('save' if hora is None else '%02d' % hora)
        fr = frente(); r = capturar(nom); f = fps(3.0)
        d = {'tipo': 'interior', 'escena': nom, 'cap': r, 'fps': f, 'frente': [round(x, 2) for x in fr], 'pos': [round(x, 2) for x in pos()], 'area': m.i32(0xB72914),
             'hora_reloj': m.u8(0xB70153), 'conteos': conteos(pos()), 'marcadores': marcadores()}
        ents = entidades_cerca(pos(), 12.0)
        d['entidades_12m'] = {'n': len(ents), 'con_rw': sum(1 for e in ents if e['rw']), 'sin_rw': sum(1 for e in ents if not e['rw'])}
        json.dump(ents, open(os.path.join(sal, nom + '_entidades.json'), 'w'))
        P(nom, json.dumps({k: d[k] for k in ('cap', 'fps', 'frente', 'pos', 'area', 'conteos', 'entidades_12m')}))
        R(d)
    # ropero: se pisa el marcador CHANGER hasta que el juego pase al interior 14
    P('ROPERO: pisando CHANGER (2492.4,-1708.5,1019.3)')
    t_ent = 0; ok = False; ts = time.time()
    while time.time() - ts < 90 and vivo():
        mantener(); ped, mat = ped_mat()
        area = m.i32(0xB72914)
        if area == 14 and mat: ok = True; break
        if area == 3 and mat and time.time() - t_ent > 0.4:
            t_ent = time.time(); tp((2492.4, -1708.5, 1019.3)); m.wr(ped + 0x558, struct.pack('<2f', 3.1416, 3.1416))
        time.sleep(0.25)
    P('ropero: area', m.i32(0xB72914), 'llego', ok, 'tras %.0f s' % (time.time() - ts))
    sujetar_juego(11)
    fr = frente(); r = capturar('ropero'); f = fps(3.0)
    d = {'tipo': 'interior', 'escena': 'ropero', 'cap': r, 'fps': f, 'frente': [round(x, 2) for x in fr], 'pos': [round(x, 2) for x in pos()], 'area': m.i32(0xB72914),
         'conteos': conteos(pos()), 'marcadores': marcadores(), 'llego_area14': ok}
    ents = entidades_cerca(pos(), 12.0)
    d['entidades_12m'] = {'n': len(ents), 'con_rw': sum(1 for e in ents if e['rw']), 'sin_rw': sum(1 for e in ents if not e['rw']),
                          'puerta_2877_con_rw': any(e['modelo'] == 2877 and e['rw'] for e in ents)}
    json.dump(ents, open(os.path.join(sal, 'ropero_entidades.json'), 'w'))
    P('ropero', json.dumps({k: d[k] for k in ('cap', 'fps', 'area', 'conteos', 'entidades_12m', 'marcadores')}))
    R(d)

def salir_de_casa():
    ts = time.time()
    while time.time() - ts < 90 and vivo():
        mantener(); ped, mat = ped_mat()
        if m.i32(0xB72914) == 0 and mat: break
        if mat: tp((2496.05, -1692.6, 1014.2))
        time.sleep(0.4)
    P('salida de la casa: area=%d tras %.0f s' % (m.i32(0xB72914), time.time() - ts))

def exterior():
    salir_de_casa(); sujetar_juego(6); foco()
    for k in PUNTOS_SEL:
        q = PUNTOS[k]; P('PUNTO %d %s: teletransporte y espera de %.0f s' % (k + 2, q, ESPERA_PUNTO))
        t1 = time.time()
        while time.time() - t1 < ESPERA_PUNTO and vivo():
            mantener(); m.wr(0xB70153, bytes([HORAS[0]])); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', CLIMAS[0]))
            if time.time() - t1 < 4: tp(q)
            time.sleep(0.4)
        for clima in CLIMAS:
            for hora in HORAS:
                t0 = time.time()
                while time.time() - t0 < ASENTAR and vivo():
                    mantener(); m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', clima)); time.sleep(0.2)
                ok_or = orientar(YAW_OBJ, PITCH_OBJ, 2.0)
                fr = frente(); nom = 'p%d_h%02d_c%02d' % (k + 2, hora, clima)
                r = capturar(nom); f = fps(2.0)
                pj = pos(); cc = {'cov': round(m.f32(0xC81304), 3), 'fog': round(m.f32(0xC81300), 3), 'extra': round(m.f32(0xC812F8), 3), 'rain': round(m.f32(0xC81324), 3), 'old': m.i32(0xC81320), 'new': m.i32(0xC8131C)}
                d = {'tipo': 'exterior', 'escena': nom, 'punto': k + 2, 'hora': hora, 'clima': clima, 'cap': r, 'fps': f, 'frente': [round(x, 3) for x in fr], 'orient_ok': ok_or, 'pos': [round(x, 2) for x in pj] if pj else None,
                     'area': m.i32(0xB72914), 'hora_reloj': m.u8(0xB70153), 'min_reloj': m.u8(0xB70152), 'clima_mem': cc, 'conteos': conteos(pj)}
                ORI['on'] = False
                R(d); P(nom, 'media', r and r.get('media_rgb'), 'valida', r and r.get('valida'), 'frente', d['frente'], 'orient_ok', ok_or, 'veh', d['conteos']['veh']['usados'], 'peds', d['conteos']['peds']['usados'], 'rnd', d['conteos']['NumRandomCars'], 'fps', f['fps'])
        # entidades alrededor al final del punto (modelos y texturas)
        ents = entidades_cerca(pos(), 60.0)
        json.dump(ents, open(os.path.join(sal, 'p%d_entidades.json' % (k + 2)), 'w'))
        R({'tipo': 'entidades', 'punto': k + 2, 'n': len(ents), 'con_rw': sum(1 for e in ents if e['rw']), 'sin_rw': sum(1 for e in ents if not e['rw'])})
    m.wr(0xC81318, struct.pack('<i', -1))


# --- vigilante: si la ventana del juego no responde 40 s seguidos se anota, se cierra el juego propio y termina la sonda
import threading
def _vigilar():
    colgado = None
    while True:
        time.sleep(2)
        h = captura.buscar()
        if not h: continue
        if u.IsHungAppWindow(wintypes.HWND(h)):
            colgado = colgado or time.time()
            if time.time() - colgado > 40:
                P('JUEGO COLGADO (la ventana no responde desde hace 40 s): se cierra el proceso propio'); R({'tipo': 'colgado', 'hora_s': desde_inicio()})
                hh = ctypes.windll.kernel32.OpenProcess(1, False, p)
                if hh: ctypes.windll.kernel32.TerminateProcess(hh, 0)
                os._exit(3)
        else: colgado = None
threading.Thread(target=_vigilar, daemon=True).start()

# --- fase lluvia: repite varias veces la misma escena (punto, hora, clima fijos) para medir el ruido del propio juego y leer las variables de CWeather.
# En el standalone las variables de CWeather son objetos propios (DatosExe) en otra direccion: se resuelven con el .map del exe (--mapa); en el original/ASI van por direccion.
import re
MAPA = arg('--mapa', 'D:/Original/wv-int/reversing/FnFase8/salida/gta_sa_standalone.map')
_SIMB = {}
def cargar_mapa():
    if _SIMB or MODO != 'std': return
    for ln in open(MAPA, encoding='latin1'):
        mm = re.search(r'\?([bv])([0-9A-F]{6})@(?:Bss|Data)@DatosExe@@\S+\s+([0-9a-f]{8})\s', ln)
        if mm: _SIMB[int(mm.group(2), 16)] = int(mm.group(3), 16)
def dir_(a):
    cargar_mapa(); return _SIMB.get(a, a) if MODO == 'std' else a
WVARS = {'LightningFlash': (0xC812CC, 'u8'), 'LightningBurst': (0xC812CD, 'u8'), 'LightningStart': (0xC812C8, 'i'), 'LightningDuration': (0xC812C4, 'i'), 'LightningFlashLastChange': (0xC812BC, 'i'),
         'WhenToPlayLightningSound': (0xC812C0, 'i'), 'TrafficLightsBrightness': (0xC812A8, 'f'), 'bScriptsForceRain': (0xC812AC, 'u8'), 'CurrentRainParticleStrength': (0xC812B0, 'f'),
         'HeadLightsSpectrum': (0xC812D0, 'f'), 'HeatHazeFXControl': (0xC812D8, 'f'), 'HeatHaze': (0xC812DC, 'f'), 'SunGlare': (0xC812E0, 'f'), 'Rainbow': (0xC812E4, 'f'), 'Wavyness': (0xC812E8, 'f'),
         'WindClipped': (0xC812EC, 'f'), 'Wind': (0xC812F0, 'f'), 'Sandstorm': (0xC812F4, 'f'), 'ExtraSunnyness': (0xC812F8, 'f'), 'Foggyness_SF': (0xC812FC, 'f'), 'Foggyness': (0xC81300, 'f'),
         'CloudCoverage': (0xC81304, 'f'), 'WetRoads': (0xC81308, 'f'), 'InterpolationValue': (0xC8130C, 'f'), 'WeatherTypeInList': (0xC81310, 'i'), 'WeatherRegion': (0xC81314, 'i16'),
         'Forced': (0xC81318, 'i16'), 'New': (0xC8131C, 'i16'), 'Old': (0xC81320, 'i16'), 'Rain': (0xC81324, 'f'), 'StreamAfterRainFlag': (0xC81328, 'i'), 'UnderWaterness': (0xC8132C, 'f'),
         'WaterDepth': (0xC81330, 'f'), 'InTunnelness': (0xC81334, 'f'), 'WaterFogFXControl': (0xC81338, 'f'), 'Earthquake': (0xC81340, 'f'), 'RainFXAlpha': (0xC81410, 'f')}
def leer_clima():
    d = {}
    for k, (a, t) in WVARS.items():
        q = dir_(a)
        try:
            d[k] = round(m.f32(q), 4) if t == 'f' else (m.u8(q) if t == 'u8' else (m.i32(q) if t == 'i' else struct.unpack('<h', m.rd(q, 2))[0]))
        except Exception as e: d[k] = None
    return d
def lluvia():
    salir_de_casa(); sujetar_juego(6); foco()
    clima = CLIMAS[0]; N = int(arg('--n', '10')); GAP = float(arg('--gap', '3'))
    escenas = [tuple(int(y) for y in x.split(':')) for x in arg('--escenas', '%d:%d' % (PUNTOS_SEL[0], HORAS[0])).split(',')]   # punto:hora
    ult = None
    for (k, hora) in escenas:
        q = PUNTOS[k]
        P('LLUVIA punto %d hora %d clima %d N=%d' % (k + 2, hora, clima, N))
        t1 = time.time(); esp = (ESPERA_PUNTO if ult != k else 4.0) + float(arg('--extra', '20'))
        while time.time() - t1 < esp and vivo():
            mantener(); m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', clima))
            if ult != k and time.time() - t1 < 4: tp(q)
            time.sleep(0.2)
        ult = k
        ok_or = orientar(YAW_OBJ, PITCH_OBJ, 2.0)
        ORI['on'] = True
        for i in range(N):
            t0 = time.time()
            while time.time() - t0 < GAP and vivo():
                mantener(); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', clima)); time.sleep(0.1)
            nom = 'l_p%d_h%02d_c%02d_%02d' % (k + 2, hora, clima, i)
            r = capturar(nom)
            d = {'tipo': 'lluvia', 'escena': nom, 'punto': k + 2, 'hora': hora, 'i': i, 'cap': r, 'hora_reloj': m.u8(0xB70153), 'min_reloj': m.u8(0xB70152), 'clima_mem': leer_clima(), 'orient_ok': ok_or, 't': round(desde_inicio(), 1)}
            R(d); P(nom, 'media', r and r.get('media_rgb'), 'valida', r and r.get('valida'), json.dumps({x: d['clima_mem'][x] for x in ('LightningFlash', 'LightningBurst', 'Rain')}))
        ORI['on'] = False
    m.wr(0xC81318, struct.pack('<i', -1))

# --- fase carga: tras gGameState=9 (sin tocar al jugador) muestrea cada 5 s durante --dur s el estado de streaming (CStreamingInfo en 0x8E4CC0, 26316 x 20 bytes; loadState en +7)
# y los pools, y guarda los estados crudos (carga_<t>s.bin) para comparar que modelos/txd/col/ipl/ifp quedan cargados en cada modo.
def carga():
    dur = float(arg('--dur', '60')); k = 0
    if arg('--captura-casa', '1') == '1':   # imagen de la casa a los 12 s (brillo de referencia: casa_hsave del original 61.9/55.9/42.4)
        foco(); sujetar_juego(12); r = capturar('carga_casa'); R({'tipo': 'carga_casa', 'cap': r}); P('carga_casa', json.dumps(r and r.get('media_rgb')), 'valida', r and r.get('valida'))
    t0 = time.time()
    while time.time() - t0 <= dur and vivo():
        mantener()
        raw = m.rd(0x8E4CC0, 26316 * 20) or b''
        st = bytes(raw[i * 20 + 7] for i in range(len(raw) // 20))
        hist = {}
        for b in st: hist[b] = hist.get(b, 0) + 1
        rangos = {}
        for nom, a, b in (('dff', 0, 20000), ('txd', 20000, 25000), ('col', 25000, 25255), ('ipl', 25255, 25511), ('dat', 25511, 25575), ('ifp', 25575, 25755), ('rrr', 25755, 26230), ('scm', 26230, 26312)):
            rangos[nom] = sum(1 for x in st[a:b] if x == 1)
        t = round(time.time() - t0, 1)
        open(os.path.join(sal, 'carga_%03.0fs.bin' % t), 'wb').write(st)
        pj = pos()
        d = {'tipo': 'carga', 't': t, 'desde_inicio': round(desde_inicio(), 1), 'loadstate_hist': hist, 'cargados_por_rango': rangos, 'memUsada': m.u32(0x8E4CB4), 'numRequested': m.i32(0x8E4CB8), 'prioridad': m.u32(0x8E4BA0), 'conteos': conteos(pj), 'pos': pj}
        R(d); P('carga', json.dumps({x: d[x] for x in ('t', 'loadstate_hist', 'cargados_por_rango', 'memUsada', 'numRequested')}))
        time.sleep(max(0, 5.0 - (time.time() - t0 - t))) 


# --- fase cielo: camara mirando al cielo (pitch = frente.z pedido, positivo = hacia arriba), varias horas/climas/puntos, repeticiones; lee variables de CClouds y CWeather
# (en el standalone por el mapa de enlace). Mismas condiciones en original y standalone. Uso: --fase cielo --puntos 0,1 --horas 12 --climas 0,4 --pitchs 0.8,-0.05 --reps 3
CVARS = {'VolMaxDist': (0xC6AA58, 'f'), 'VolUsedNum': (0xC6AA5C, 'i'), 'CamRoll': (0xC6AA64, 'f'), 'IndivRot': (0xC6AA6C, 'i'), 'CloudRot': (0xC6AA70, 'f'),
         'VolFader': (0xC6E970, 'f'), 'VolDensity': (0x8D5388, 'f'), 'VolHeightSwitch': (0x8D538C, 'i'), 'VolWindMove': (0x8D5390, 'f'), 'CurFogIntensity': (0x8D5798, 'f')}
def leer_nubes():
    d = {}
    for k, (a, t) in CVARS.items():
        q = dir_(a)
        try: d[k] = round(m.f32(q), 4) if t == 'f' else m.i32(q)
        except Exception: d[k] = None
    try:
        raw = m.rd(0xC6AAB0, 64) or b''
        d['vc_usados'] = sum(1 for b in raw[:64] if b)   # bUsed[0..63] (si MAX_VOLUMETRIC_CLOUDS < 64 incluye bJustCreated)
    except Exception: pass
    return d
def _raton(dy):
    class MI(ctypes.Structure): _fields_ = [('dx', wintypes.LONG), ('dy', wintypes.LONG), ('d', wintypes.DWORD), ('fl', wintypes.DWORD), ('t', wintypes.DWORD), ('x', ctypes.c_void_p)]
    class IN(ctypes.Structure):
        class U(ctypes.Union): _fields_ = [('mi', MI)]
        _anonymous_ = ('u',); _fields_ = [('type', wintypes.DWORD), ('u', U)]
    i = IN(); i.type = 0; i.mi = MI(0, dy, 0, 1, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(IN))
SIGNO = {'v': None, 'x': None}
MOUSEY = {'v': 0.0, 'x': 0.0}
def _escritor_raton():
    # escribe CMouseControllerState::m_AmountMoved (x 0xB73424, y 0xB73428) cada ~1 ms: la camara de CJ lo lee despues de que CPad lo ponga a cero al inicio del fotograma.
    # Mismo mecanismo en los dos modos (no depende de SendInput/DirectInput, que se comportan distinto con la calza de ventana del original).
    while True:
        time.sleep(0.001)
        try:
            if MOUSEY['v'] != 0.0: m.wr(0xB73428, struct.pack('<f', MOUSEY['v']))
            if MOUSEY['x'] != 0.0: m.wr(0xB73424, struct.pack('<f', MOUSEY['x']))
        except Exception: pass
threading.Thread(target=_escritor_raton, daemon=True).start()
TOCAR = {'on': False}
FIJAR = {'q': None}
def _fijar():
    # mantiene al jugador en el aire (--alt): teletransporte y velocidad nula a 20 Hz
    while True:
        time.sleep(0.05)
        q = FIJAR['q']
        if not q: continue
        try:
            ped, mat = ped_mat()
            if mat:
                for i, v in enumerate(q): m.wr(mat + 0x30 + 4 * i, struct.pack('<f', v))
                m.wr(ped + 0x44, struct.pack('<3f', 0, 0, 0))
        except Exception: pass
threading.Thread(target=_fijar, daemon=True).start()
def _tocar_pad():
    # CPad::LastTimeTouched (CPad[0] 0xB73458 + 0x120) = tiempo de juego (0xB7CB84): evita la camara ociosa (CIdleCam), que gira la camara tras un rato sin tocar el mando
    while True:
        time.sleep(0.05)
        if TOCAR['on']:
            try: m.wr(0xB73458 + 0x120, struct.pack('<I', m.u32(0xB7CB84)))
            except Exception: pass
threading.Thread(target=_tocar_pad, daemon=True).start()
def _yaw(): f = frente(); return math.degrees(math.atan2(f[1], f[0]))
def _dyaw(a, b): return (a - b + 180) % 360 - 180
def orientar_libre(yaw, pitch, dur=2.0, maximo=25.0):
    """Orienta con CMouseControllerState (x e y) en bucle cerrado sobre el frente leido (0xB6F9AC); no escribe los angulos de la CCam. Signos calibrados en cada ejecucion
    (SIGNO) y anotados. Devuelve (frente, ok): ok = yaw a menos de 3 grados y z a menos de 0.04 del pedido, sostenido 25 lecturas."""
    ORI['on'] = False; foco(); t0 = time.time()
    if SIGNO['v'] is None or SIGNO['x'] is None:
        z0 = frente()[2]; y0 = _yaw(); MOUSEY['v'] = 3.0; MOUSEY['x'] = 3.0; t1 = time.time()
        while time.time() - t1 < 0.4: mantener(); time.sleep(0.03)
        MOUSEY['v'] = 0.0; MOUSEY['x'] = 0.0; time.sleep(0.4); z1 = frente()[2]; y1 = _yaw()
        SIGNO['v'] = 1 if z1 > z0 + 0.02 else (-1 if z1 < z0 - 0.02 else None); dd = _dyaw(y1, y0); SIGNO['x'] = 1 if dd > 3 else (-1 if dd < -3 else None)
        P('calibracion: mouse +3 durante 0.4 s: z %.3f -> %.3f (signo y = %s), yaw %.1f -> %.1f (signo x = %s)' % (z0, z1, SIGNO['v'], y0, y1, SIGNO['x']))
    sy = SIGNO['v'] or 1; sx = SIGNO['x'] or 1; ult = 0
    while vivo() and time.time() - t0 < maximo:
        mantener(); f = frente(); ez = pitch - f[2]; ey = _dyaw(yaw, _yaw())
        okz = abs(ez) < 0.04; oky = abs(ey) < 3.0
        MOUSEY['v'] = 0.0 if okz else sy * max(-3.0, min(3.0, ez * 4.0 if abs(ez * 4.0) >= 0.3 else (0.3 if ez > 0 else -0.3)))
        MOUSEY['x'] = 0.0 if oky else sx * max(-3.0, min(3.0, ey / 20.0 if abs(ey / 20.0) >= 0.3 else (0.3 if ey > 0 else -0.3)))
        if okz and oky:
            ult += 1
            if ult > 25 and time.time() - t0 > dur: break
        else: ult = 0
        time.sleep(0.03)
    MOUSEY['v'] = 0.0; MOUSEY['x'] = 0.0; time.sleep(0.3)
    f = frente(); return f, (abs(pitch - f[2]) < 0.06 and abs(_dyaw(yaw, _yaw())) < 4.0)
def cielo():
    TOCAR['on'] = True
    salir_de_casa(); sujetar_juego(6); foco()
    PITCHS = [float(x) for x in arg('--pitchs', '0.8,-0.05').split(',')]; REPS = int(arg('--reps', '1')); GAP = float(arg('--gap', '0.3'))
    YAWS = [float(x) for x in arg('--yaws', str(YAW_OBJ)).split(',')]
    for k in PUNTOS_SEL:
        q = PUNTOS[k]; ALT = arg('--alt')
        if ALT: q = (q[0], q[1], float(ALT)); FIJAR['q'] = q
        P('PUNTO %d %s' % (k + 2, q)); t1 = time.time()
        while time.time() - t1 < ESPERA_PUNTO and vivo():
            mantener(); m.wr(0xB70153, bytes([HORAS[0]])); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', CLIMAS[0]))
            if time.time() - t1 < 4: tp(q)
            time.sleep(0.4)
        for clima in CLIMAS:
            for hora in HORAS:
                t0 = time.time()
                while time.time() - t0 < ASENTAR and vivo():
                    mantener(); m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', clima)); time.sleep(0.2)
                for yaw in YAWS:
                  for pitch in PITCHS:
                    f, ok_or = orientar_libre(yaw, pitch)
                    for rep in range(REPS):
                        if rep: f, ok_or = orientar_libre(yaw, pitch, 1.0)
                        t0 = time.time()
                        while time.time() - t0 < GAP and vivo(): mantener(); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', clima)); time.sleep(0.1)
                        nom = 'c_p%d_h%02d_c%02d_y%03d_pi%+.2f_r%d' % (k + 2, hora, clima, yaw, pitch, rep)
                        fr = frente(); r = capturar(nom); pj = pos()
                        d = {'tipo': 'cielo', 'escena': nom, 'punto': k + 2, 'hora': hora, 'clima': clima, 'pitch_pedido': pitch, 'yaw_pedido': yaw, 'rep': rep, 'orient_ok': ok_or, 'cap': r, 'frente': [round(x, 3) for x in fr],
                             'pos': [round(x, 2) for x in pj] if pj else None, 'cam_z': round(m.f32(0xB6F028 + 0x30 * 0 + 0x0), 3) if False else None, 'hora_reloj': m.u8(0xB70153),
                             'clima_mem': leer_clima(), 'nubes': leer_nubes(), 'signo_raton': SIGNO['v'], 'area': m.i32(0xB72914), 't': round(desde_inicio(), 1)}
                        R(d); P(nom, 'media', r and r.get('media_rgb'), 'valida', r and r.get('valida'), 'ok', ok_or, 'frente', d['frente'], 'cov', d['clima_mem'].get('CloudCoverage'), json.dumps(d['nubes']))
                ORI['on'] = False
    m.wr(0xC81318, struct.pack('<i', -1))

def mtest():
    salir_de_casa(); sujetar_juego(6); foco(); tp(PUNTOS[0]); sujetar_juego(8)
    ORI['a'] = math.radians(YAW_OBJ) + math.pi; ORI['solo_h'] = True; ORI['on'] = True
    for dy in (-2, -2, -2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 5, 5, 5, 0, 0, -5, -5, -5, 0, 0):
        _raton(dy); time.sleep(0.12); mantener()
        P('dy', dy, 'z', round(frente()[2], 3), 'fade', m.u8(dir_(0xC3EFAB)), 'ctl', m.u8(0xB6F079), 'st', m.u16(0xB6FC58) if hasattr(m, 'u16') else struct.unpack('<H', m.rd(0xB6FC58, 2))[0],
          'mouse', round(m.f32(0xB73424), 3), round(m.f32(0xB73428), 3), 'D4', round(m.f32(dir_(0x8CC7D4)), 3), 'D8', round(m.f32(dir_(0x8CC7D8)), 3), 'mode', m.u8(0xB6F028 + 0x59) if False else struct.unpack('<H', m.rd(cam_activa() + 0xC, 2))[0])

def ctest():
    salir_de_casa(); sujetar_juego(6); foco(); tp(PUNTOS[0]); sujetar_juego(8)
    base = 0xB6F028
    P('TheCamera+0x59..', [m.u8(base + o) for o in (0x58, 0x59, 0x5A)])
    for k in range(3):
        c = base + 0x1B4 + k * 0x238
        r = struct.unpack('<%df' % (0x238 // 4), m.rd(c, 0x238)); P('cam', k, hex(c), 'AC', round(r[0xAC // 4], 3), 'BC', round(r[0xBC // 4], 3), 'front', [round(x, 3) for x in r[0x190 // 4:0x190 // 4 + 3]], 'src', [round(x, 1) for x in r[0x19C // 4:0x19C // 4 + 3]])
    time.sleep(1.0)
    P('TheCamera front 0xB6F9AC', [round(m.f32(0xB6F9AC + 4 * j), 3) for j in range(3)])
    # buscar en toda la region TheCamera (0xB6F028, 0xA00 bytes) un float igual al angulo vertical esperado: front z = -sin(alpha)
    fz = m.f32(0xB6F9AC + 8); al = -math.asin(max(-1, min(1, fz)))
    r = m.rd(base, 0xA00); fl = struct.unpack('<%df' % (0xA00 // 4), r)
    P('candidatos alpha=%.3f' % al, [(hex(base + 4 * i), round(v, 3)) for i, v in enumerate(fl) if abs(v - al) < 0.002 and v != 0][:20])
    P('candidatos beta 4.712/-1.571', [(hex(base + 4 * i), round(v, 3)) for i, v in enumerate(fl) if abs(abs(v) - 4.712) < 0.01 or abs(abs(v) - 1.5708) < 0.01][:20])
    P('candidatos beta (BC~)', [(hex(base + 4 * i), round(v, 3)) for i, v in enumerate(fl) if abs(v - m.f32(base + 0x1B4 + 0xBC)) < 0.002][:10])

def nrot():
    """CloudRotation en el tiempo: muestrea cada 0.5 s (t, CloudRot, contador de fotogramas, TheCamera.m_fOrientation 0xB6F178, Wind, IndivRot) con la camara quieta, clima --climas[0]"""
    TOCAR['on'] = True; salir_de_casa(); sujetar_juego(6); foco(); tp(PUNTOS[0]); clima = CLIMAS[0]; hora = HORAS[0]
    t0 = time.time()
    while time.time() - t0 < 8 and vivo(): mantener(); m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', clima)); time.sleep(0.2)
    f, ok = orientar_libre(YAW_OBJ, -0.05); P('orient', f, ok)
    t0 = time.time(); fr0 = m.u32(0xB7CB4C); r0 = m.f32(dir_(0xC6AA70)); filas = []
    while time.time() - t0 < float(arg('--dur', '20')) and vivo():
        mantener(); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', clima))
        filas.append((round(time.time() - t0, 2), round(m.f32(dir_(0xC6AA70)), 5), m.u32(0xB7CB4C) - fr0, round(m.f32(0xB6F178), 4), round(m.f32(dir_(0xC812F0)), 3), m.i32(dir_(0xC6AA6C)), round(m.f32(0xB7CB5C), 3)))
        time.sleep(0.5)
    for r in filas: P('rot', *r)
    P('tasa CloudRot/s', (filas[-1][1] - filas[0][1]) / (filas[-1][0] - filas[0][0]), 'fotogramas/s', (filas[-1][2] - filas[0][2]) / (filas[-1][0] - filas[0][0]))

# --- fase medir (comparativa standalone vs original): a los --t-medir s del proceso (60 por defecto) mide modulos cargados, memoria, fotogramas por segundo, hilos/handles, ventana
# y vuelca por memoria el codigo original (0x401000-0x858000) y los datos (0x858000-0xCB1000) para el analisis estatico (contrib/comparativa_standalone.py).
def _modulos(h):
    ps = ctypes.WinDLL('psapi', use_last_error=True)
    ps.EnumProcessModulesEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.DWORD, ctypes.POINTER(wintypes.DWORD), wintypes.DWORD]
    ps.GetModuleFileNameExW.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.LPWSTR, wintypes.DWORD]
    arr = (ctypes.c_void_p * 2048)(); need = wintypes.DWORD()
    ps.EnumProcessModulesEx(h, arr, ctypes.sizeof(arr), ctypes.byref(need), 3)
    out = []
    buf = ctypes.create_unicode_buffer(520)
    class MI(ctypes.Structure): _fields_ = [('base', ctypes.c_void_p), ('size', wintypes.DWORD), ('entry', ctypes.c_void_p)]
    for i in range(need.value // ctypes.sizeof(ctypes.c_void_p)):
        ps.GetModuleFileNameExW(h, ctypes.c_void_p(arr[i]), buf, 520)
        mi = MI(); ps.GetModuleInformation.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, wintypes.DWORD]
        ps.GetModuleInformation(h, ctypes.c_void_p(arr[i]), ctypes.byref(mi), ctypes.sizeof(mi))
        out.append({'ruta': buf.value, 'base': arr[i], 'tam': mi.size})
    return out
def _memoria(h):
    class PMC(ctypes.Structure):
        _fields_ = [('cb', wintypes.DWORD), ('PageFaultCount', wintypes.DWORD), ('PeakWorkingSetSize', ctypes.c_size_t), ('WorkingSetSize', ctypes.c_size_t), ('QuotaPeakPagedPoolUsage', ctypes.c_size_t),
                    ('QuotaPagedPoolUsage', ctypes.c_size_t), ('QuotaPeakNonPagedPoolUsage', ctypes.c_size_t), ('QuotaNonPagedPoolUsage', ctypes.c_size_t), ('PagefileUsage', ctypes.c_size_t),
                    ('PeakPagefileUsage', ctypes.c_size_t), ('PrivateUsage', ctypes.c_size_t)]
    ps = ctypes.WinDLL('psapi', use_last_error=True)
    ps.GetProcessMemoryInfo.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.DWORD]
    pm = PMC(); pm.cb = ctypes.sizeof(pm); ps.GetProcessMemoryInfo(h, ctypes.byref(pm), pm.cb)
    return {k_: getattr(pm, k_) for k_, _ in PMC._fields_ if k_ != 'cb'}
def _volcar(a, b, ruta):
    with open(ruta, 'wb') as f:
        va = a
        while va < b:
            n = min(0x10000, b - va); d = m.rd(va, n)
            if d is None:
                d = b''.join((m.rd(va + o, 0x1000) or b'\x00' * 0x1000) for o in range(0, n, 0x1000))
            f.write(d); va += n
def medir():
    t_med = float(arg('--t-medir', '60')); vdir = arg('--volcado-dir', sal); os.makedirs(vdir, exist_ok=True)
    while vivo() and desde_inicio() < t_med: mantener(); time.sleep(0.2)
    if not vivo(): P('MEDIR: el proceso murio antes'); return
    h = ctypes.windll.kernel32.OpenProcess(0x410, False, p)
    mods = _modulos(h); mem = _memoria(h)
    hc = wintypes.DWORD(); ctypes.windll.kernel32.GetProcessHandleCount(h, ctypes.byref(hc))
    buf = ctypes.create_unicode_buffer(520); sz = wintypes.DWORD(520); ctypes.windll.kernel32.QueryFullProcessImageNameW(h, 0, buf, ctypes.byref(sz))
    fps_ = [fps(10.0) for _ in range(3)]
    mem2 = _memoria(h)
    rc = wintypes.RECT(); hw = captura.buscar()
    if hw: u.GetClientRect(hw, ctypes.byref(rc))
    _volcar(0x401000, 0x858000, os.path.join(vdir, 'volcado_codigo_%s.bin' % MODO)); _volcar(0x858000, 0xCB1000, os.path.join(vdir, 'volcado_datos_%s.bin' % MODO))
    gs = m.u32(0xC8D4C0)
    R({'tipo': 'medir', 'modo': MODO, 't_desde_inicio': round(desde_inicio(), 1), 'exe': buf.value, 'gGameState': gs, 'modulos': mods, 'memoria_a_los_t': mem, 'memoria_tras_fps': mem2, 'handles': hc.value,
       'fps': fps_, 'cliente': [rc.right - rc.left, rc.bottom - rc.top]})
    P('MEDIR: %d modulos, WorkingSet %.1f MiB, Private %.1f MiB, fps %s' % (len(mods), mem['WorkingSetSize'] / 2**20, mem['PrivateUsage'] / 2**20, [f['fps'] for f in fps_]))

def main():
    P('modo', MODO, 'fase', FASE, 'pid', p, 'horas', HORAS, 'climas', CLIMAS, 'puntos', PUNTOS_SEL)
    if not arrancar(): P('no se llego a gGameState=9'); return
    h = foco(); time.sleep(1)
    rc = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(rc)); wr = wintypes.RECT(); u.GetWindowRect(h, ctypes.byref(wr))
    P('ventana: cliente %dx%d, rect ventana %dx%d' % (rc.right - rc.left, rc.bottom - rc.top, wr.right - wr.left, wr.bottom - wr.top))
    if FASE in ('interior', 'todo'): interior()
    if FASE in ('exterior', 'todo'): exterior()
    if FASE == 'lluvia': lluvia()
    if FASE == 'cielo': cielo()
    if FASE == 'mtest': mtest()
    if FASE == 'nrot': nrot()
    if FASE == 'ctest': ctest()
    if FASE == 'carga': carga()
    if FASE == 'medir': medir()
    P('fin')
    h = ctypes.windll.kernel32.OpenProcess(1, False, p)   # el juego es el proceso propio de este lanzamiento
    if h: ctypes.windll.kernel32.TerminateProcess(h, 0)
if __name__ == '__main__': main()
