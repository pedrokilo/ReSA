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
    if d.get('tipo') in ('interior', 'exterior'): d['mem'] = volcado()
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
def arrancar():
    t8 = t9 = None; h = None; ult = 0; gs_ant = -1; hist = []
    while vivo() and desde_inicio() < 400:
        gs = m.u32(0xC8D4C0)
        if gs != gs_ant: hist.append((round(desde_inicio(), 1), gs)); gs_ant = gs
        if gs == 8 and t8 is None: t8 = desde_inicio()
        ped, mat = ped_mat()
        if gs == 9 and mat and t9 is None: t9 = desde_inicio(); break
        if MODO == 'orig':
            if gs in (2, 4): foco(); teclas.tecla('enter')
            elif gs == 7 and PM.setdefault('t7', time.time()) and time.time() - PM['t7'] > 3.0 and time.time() - ult > 1.8:
                ult = time.time(); foco(); time.sleep(0.2); menu_original(m)
        mantener(); time.sleep(0.1 if gs != 9 else 0.05)
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
ORI = {'on': False, 'a': 0.0, 'b': 0.0}
def _escritor():
    while True:
        time.sleep(0.02)
        if not ORI['on']: continue
        try:
            for k in range(3):
                c = 0xB6F028 + 0x1B4 + k * 0x238
                m.wr(c + 0xBC, struct.pack('<f', ORI['a'])); m.wr(c + 0xAC, struct.pack('<f', ORI['b'])); m.wr(c + 0x8C, struct.pack('<f', ORI['b'])); m.wr(c + 0xA4, struct.pack('<f', ORI['a'])); m.wr(c + 0xA0, struct.pack('<f', ORI['b']))
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

def main():
    P('modo', MODO, 'fase', FASE, 'pid', p, 'horas', HORAS, 'climas', CLIMAS, 'puntos', PUNTOS_SEL)
    if not arrancar(): P('no se llego a gGameState=9'); return
    h = foco(); time.sleep(1)
    rc = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(rc)); wr = wintypes.RECT(); u.GetWindowRect(h, ctypes.byref(wr))
    P('ventana: cliente %dx%d, rect ventana %dx%d' % (rc.right - rc.left, rc.bottom - rc.top, wr.right - wr.left, wr.bottom - wr.top))
    if FASE in ('interior', 'todo'): interior()
    if FASE in ('exterior', 'todo'): exterior()
    P('fin')
    h = ctypes.windll.kernel32.OpenProcess(1, False, p)   # el juego es el proceso propio de este lanzamiento
    if h: ctypes.windll.kernel32.TerminateProcess(h, 0)
if __name__ == '__main__': main()
