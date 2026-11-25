# -*- coding: utf-8 -*-
# Sonda GUIADA del fallo "textura de CJ se expande": arranca/espera el juego (standalone, original+ASI u original puro), pone a CJ a pie / en bici / moto / coche,
# lo hace pedalear, girar (volante a tope, zigzag), frenar, saltar, bajarse y subirse, volcar (caida) y cambiar de cuerpo (gordo/delgado/musculoso), durante --minutos,
# mientras el Vigilante (skinwatch.py) lee matrices de hueso y captura la ventana. Solo toca el proceso con --pid.
# Uso: python -I sonda_skin.py <salida> --pid N --modo orig|std|asi --evid <carpeta evidencias> [--escenarios pie,bici481,bici510,moto463,coche411] [--minutos 10]
#                              [--log ruta\log.log] [--mapa ruta.map] [--sin-imagen]
import sys, os, time, json, math, random, struct, ctypes, hashlib
from ctypes import wintypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
sys.path.insert(0, os.path.join(aqui, '..', 'FnJuego')); sys.path.insert(0, os.path.join(aqui, '..', 'FnTraf'))
import skinwatch as W
u = W.u
def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
sal = sys.argv[1]; os.makedirs(sal, exist_ok=True)
PID = int(arg('--pid')); MODO = arg('--modo', 'std'); EVID = arg('--evid', os.path.join(sal, 'evidencias'))
LOG = open(os.path.join(sal, 'sonda_skin.log'), 'w', encoding='utf-8'); T0 = time.time()
def P(*a):
    t = '[%7.1f] ' % (time.time() - T0) + ' '.join(str(x) for x in a); print(t, flush=True); LOG.write(t + '\n'); LOG.flush()
# ---------------------------------------------------------------- entrada (SendInput por codigos de barrido)
class KI(ctypes.Structure):
    _fields_ = [('wVk', wintypes.WORD), ('wScan', wintypes.WORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class MI(ctypes.Structure):
    _fields_ = [('dx', wintypes.LONG), ('dy', wintypes.LONG), ('mouseData', wintypes.DWORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class INP(ctypes.Structure):
    class _U(ctypes.Union): _fields_ = [('ki', KI), ('mi', MI)]
    _anonymous_ = ('u',); _fields_ = [('type', wintypes.DWORD), ('u', _U)]
SC = {c: s for c, s in zip('qwertyuiop', range(0x10, 0x1A))}
SC.update({c: s for c, s in zip('asdfghjkl', range(0x1E, 0x27))}); SC.update({c: s for c, s in zip('zxcvbnm', range(0x2C, 0x33))})
SC.update({'enter': 0x1C, 'space': 0x39, 'lshift': 0x2A, 'ctrl': 0x1D, 'esc': 0x01, 'down': 0x50, 'up': 0x48})
EXT = {'up', 'down', 'left', 'right'}
def tecla(n, abajo):
    i = INP(); i.type = 1; i.ki = KI(0, SC[n], 0x8 | (0 if abajo else 0x2) | (0x1 if n in EXT else 0), 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP))
def pulsar(n, t=0.12): tecla(n, True); time.sleep(t); tecla(n, False); time.sleep(0.06)
def soltar_todo():
    for n in ('w', 'a', 's', 'd', 'space', 'lshift', 'f', 'enter'): tecla(n, False)

m = W.Mem(PID)
def vivo(): return W.k.WaitForSingleObject(m.h, 0) == 0x102
def mantener(hora=12):
    m.wr(0x8D621C, struct.pack('<I', 1)); m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0]))
def ped(): foco = m.u8(0xB7CD74); return m.u32(0xB7CD98 + foco * 0x190)
def mat_ent(e): return m.u32(e + 0x14) if e else 0
def pos_ent(e):
    a = mat_ent(e); return (m.f32(a + 0x30), m.f32(a + 0x34), m.f32(a + 0x38)) if a else None
def tp(q):
    a = mat_ent(ped())
    if a:
        for i, v in enumerate(q): m.wr(a + 0x30 + 4 * i, struct.pack('<f', v))
def veh(): p = ped(); return m.u32(p + 0x58C) if p else 0
def estado_ped(): p = ped(); return m.u32(p + 0x530) if p else 0
def en_veh(): return bool(veh()) and estado_ped() == 50
def modelo(v): return struct.unpack('<H', m.rd(v + 0x22, 2))[0] if v else 0
def vel(e): return math.sqrt(sum(m.f32(e + 0x44 + 4 * j) ** 2 for j in range(3))) if e else 0.0
def vehiculos():
    pv = m.u32(0xB74494); objs = m.u32(pv); fl = m.rd(m.u32(pv + 4), m.i32(pv + 8)) or b''; return [objs + i * 0xA18 for i in range(len(fl)) if not fl[i] & 0x80]
def area(): return m.i32(0xB72914)
def frente_ventana():
    h = W.buscar_ventana(PID)
    if h: u.ShowWindow(h, 9); u.SetWindowPos(wintypes.HWND(h), wintypes.HWND(-1), 0, 0, 0, 0, 0x0001 | 0x0002 | 0x0040); u.SetForegroundWindow(h)
    return h
def dormir(seg, paso=0.05):
    t = time.time()
    while time.time() - t < seg:
        if not vivo(): raise SystemExit('el juego termino')
        mantener(); time.sleep(paso)

# ---------------------------------------------------------------- arranque
PM = {'i': 0, 'h': None, 'reint': 0, 't': 0.0}
SECUENCIA_MENU = 'enter,down,enter,down,enter,enter'.split(',')
def menu_original():
    ah = hashlib.md5(m.rd(0xBA6748, 0x400) or b'').hexdigest()
    if PM['i'] > 0 and PM['h'] == ah and PM['reint'] < 3 and PM['i'] <= len(SECUENCIA_MENU):
        PM['reint'] += 1; k = SECUENCIA_MENU[PM['i'] - 1]
    elif PM['i'] < len(SECUENCIA_MENU):
        k = SECUENCIA_MENU[PM['i']]; PM['i'] += 1; PM['reint'] = 0; PM['h'] = ah
    elif time.time() - PM['t'] > 12:
        for _ in range(3): pulsar('esc')
        PM.update({'i': 0, 'h': None, 'reint': 0}); PM['t'] = time.time(); return
    else: return
    PM['t'] = time.time(); pulsar(k, 0.15)
def arrancar():
    t0 = time.time(); ult = 0
    while vivo() and time.time() - t0 < 500:
        gs = m.u32(0xC8D4C0); p = ped(); a = mat_ent(p)
        if gs == 9 and a: break
        if MODO == 'orig':
            if gs in (2, 4): frente_ventana(); pulsar('enter')
            elif gs == 7 and time.time() - ult > 1.8:
                ult = time.time(); frente_ventana(); time.sleep(0.2); menu_original()
        mantener(); time.sleep(0.1)
    P('juego en marcha tras %.0f s; gGameState=%d' % (time.time() - t0, m.u32(0xC8D4C0)))
    ts = time.time()
    while time.time() - ts < 90 and vivo():
        mantener(); a = mat_ent(ped())
        if area() == 0 and a: break
        if a: m.wr(a + 0x30, struct.pack('<3f', 2496.05, -1692.6, 1014.2))
        time.sleep(0.4)
    P('salida de la casa: area=%d tras %.0f s' % (area(), time.time() - ts))
    h = frente_ventana(); time.sleep(1.0); return h

# ---------------------------------------------------------------- vehiculos
VEHICLE_CHEAT = 0x43A0B0
LUGARES = [(2652.5, -1719.9, 13.8), (2672.1, -1652.5, 11.5), (2658.1, -1433.7, 31.5), (2255.0, -1750.0, 13.5)]
def spawn(modelo_id):
    antes = set(vehiculos()); frente_ventana()
    r = m.llamar(VEHICLE_CHEAT, modelo_id); time.sleep(0.8)
    nuevos = [v for v in vehiculos() if v not in antes and modelo(v) == modelo_id]
    P('spawn modelo', modelo_id, 'retorno', hex(r) if r is not None else None, 'nuevos', [hex(v) for v in nuevos])
    return nuevos[0] if nuevos else 0
def junto(v):
    a = mat_ent(v); rx, ry = m.f32(a), m.f32(a + 4); vp = pos_ent(v)
    if vp: tp((vp[0] - rx * 1.2, vp[1] - ry * 1.2, vp[2] + 0.6))
def montar(v, intentos=5):
    for n in range(intentos):
        junto(v); dormir(0.8); frente_ventana(); pulsar('f', 0.15)
        t = time.time()
        while time.time() - t < 4 and not en_veh() and vivo(): dormir(0.2)
        if en_veh(): return True
    return False
def bajar():
    if not en_veh(): return True
    pulsar('f', 0.15); t = time.time()
    while time.time() - t < 4 and en_veh() and vivo(): dormir(0.2)
    return not en_veh()

# ---------------------------------------------------------------- conduccion
TELE = []
def tele(etq):
    v = veh(); p = pos_ent(ped())
    TELE.append((round(time.time() - T0, 1), etq, hex(v), modelo(v) if v else 0, estado_ped(), round(vel(v), 3) if v else round(vel(ped()), 3), p))
IMPULSO = [0.0]
def fase(etq, teclas, seg, zigzag=None, pedalear=False, extra=None):
    """mantiene `teclas` durante `seg` s; zigzag=(periodo_min, periodo_max) alterna a/d; pedalear toca espacio (BMX) a ritmo"""
    frente_ventana()
    for k in teclas: tecla(k, True)
    t = time.time(); lado = 'a'; prox = 0; ultp = 0; vmax = 0; nxt = 0
    try:
        while time.time() - t < seg and vivo():
            mantener(); now = time.time()
            if zigzag and now >= prox:
                tecla('a', False); tecla('d', False); lado = 'd' if lado == 'a' else 'a'; tecla(lado, True); prox = now + random.uniform(*zigzag)
            if pedalear and now - ultp > 0.18: ultp = now; tecla('space', True); time.sleep(0.05); tecla('space', False)
            if extra: extra(now - t)
            if IMPULSO[0] and 'w' in teclas and en_veh():
                vv = veh(); aa = mat_ent(vv)
                if vv and aa and vel(vv) < IMPULSO[0]: m.wr(vv + 0x44, struct.pack('<2f', m.f32(aa + 0x10) * IMPULSO[0], m.f32(aa + 0x14) * IMPULSO[0]))
            v = veh(); vmax = max(vmax, vel(v) if v else vel(ped()))
            if now > nxt: nxt = now + 1.0; tele(etq)
            time.sleep(0.02)
    finally:
        for k in teclas + ['a', 'd', 'space']: tecla(k, False)
    P('FASE', etq, 'seg=%.1f' % seg, 'vmax=%.3f' % vmax, 'en_veh=%s' % en_veh(), 'estado=%d' % estado_ped(), 'pos', pos_ent(ped()))
def volcar(v):
    a = mat_ent(v)
    if not a: return
    for off in (0, 4, 8, 0x20, 0x24, 0x28): m.wr(a + off, struct.pack('<f', -m.f32(a + off)))
    m.wr(v + 0x44, struct.pack('<3f', 0.0, 0.0, 0.25)); P('volcar: matriz invertida (right y at) del vehiculo', hex(v))
CUERPOS = ['BTCDBCB', 'KVGYZQK', 'JYSDSOD']   # gordo, delgado, musculoso (cheats; reconstruyen el modelo de CJ)
def cuerpo(n):
    c = CUERPOS[n % 3]; frente_ventana(); P('cuerpo (cheat)', c)
    for ch in c.lower(): pulsar(ch, 0.09)
    dormir(1.5)

def ciclo_vehiculo(modelo_id, seg_total, bici, cambiar_cuerpo=True):
    t_fin = time.time() + seg_total; n = 0; lugar = 0
    while time.time() < t_fin and vivo():
        lugar = (lugar + 1) % len(LUGARES); tp(LUGARES[lugar]); dormir(4)
        if cambiar_cuerpo and n % 2 == 1: cuerpo(n // 2)
        v = spawn(modelo_id)
        if not v: P('sin vehiculo, reintento'); dormir(3); n += 1; continue
        if not montar(v): P('no se pudo montar', hex(v)); n += 1; continue
        P('MONTADO modelo', modelo(veh()), 'ciclo', n, 'clump_ped', hex(m.u32(ped() + 0x18)), 'ped', hex(ped()))
        for sub in range(6):
            if time.time() >= t_fin or not en_veh(): break
            IMPULSO[0] = (0.30 if bici else 0.0) if sub % 2 == 1 else 0.0
            fase('recto', ['w'], 7, pedalear=bici)
            fase('zigzag_rapido', ['w'], 10, zigzag=(0.25, 0.7), pedalear=bici)
            fase('giro_izq_cerrado', ['w', 'a'], 5, pedalear=bici)
            fase('giro_der_cerrado', ['w', 'd'], 5, pedalear=bici)
            fase('zigzag_lento', ['w'], 10, zigzag=(0.8, 1.6), pedalear=bici)
            fase('freno', ['s'], 3)
            fase('salto', ['w'], 4, pedalear=bici, extra=lambda t: pulsar('lshift', 0.1) if 1.0 < t < 1.1 else None)
            fase('parado_girando', ['a'], 2); fase('parado_girando', ['d'], 2)
            if sub % 2 == 1 and en_veh():
                bajar(); dormir(1.0); P('bajado; vuelvo a subir')
                if not montar(v, 3): break
        if en_veh():
            fase('recto_antes_caida', ['w'], 4, pedalear=bici); volcar(veh()); dormir(5)
        P('fin ciclo', n, 'en_veh', en_veh()); bajar(); n += 1
def ciclo_pie(seg_total):
    t_fin = time.time() + seg_total; n = 0
    while time.time() < t_fin and vivo():
        tp(LUGARES[n % len(LUGARES)]); dormir(3)
        if n % 2 == 1: cuerpo(n // 2)
        for sub in range(4):
            fase('andar_recto', ['w'], 6); fase('correr_zigzag', ['w'], 12, zigzag=(0.2, 0.6), pedalear=True)
            fase('girar_parado_izq', ['a'], 3); fase('girar_parado_der', ['d'], 3)
            fase('andar_giro_izq', ['w', 'a'], 6); fase('andar_giro_der', ['w', 'd'], 6)
            fase('correr_giro_amplio', ['w', 'd'], 8, pedalear=True); fase('saltar', ['w'], 5, extra=lambda t: pulsar('lshift', 0.1) if int(t * 10) % 12 == 0 else None)
        n += 1


RUTA_LOG = 'D:/Original/_fase8b_run_imagen/salida_jugar/fase8_arranque.log'
def ruta_usuario(seg_total):
    """recorre en bici (481) las posiciones 'mundo' del log de la partida del usuario (una cada 4 s) con giros alternos; teletransporta bici+CJ a cada punto y pedalea 4 s"""
    import re
    pts = []
    for l in open(arg('--ruta-log', RUTA_LOG), encoding='utf-8', errors='replace'):
        m_ = re.search(r'mundo: .*pos=\((-?[\d.]+), (-?[\d.]+), (-?[\d.]+)\) area=0 ', l)
        if m_ and float(m_.group(1)) != 0 and float(m_.group(1)) < 3000: pts.append(tuple(float(m_.group(i)) for i in (1, 2, 3)))
    P('ruta del usuario:', len(pts), 'puntos', pts[:3], '...')
    if not pts: return
    t_fin = time.time() + seg_total
    tp((pts[0][0], pts[0][1], pts[0][2] + 1)); dormir(5)
    v = spawn(481)
    if not v or not montar(v): P('ruta: no se pudo montar'); return
    IMPULSO[0] = 0.30; i = 0
    while time.time() < t_fin and vivo():
        x, y, z = pts[i % len(pts)]; i += 1
        vv = veh()
        if vv:
            a = mat_ent(vv)
            for k, val in enumerate((x, y, z + 0.8)): m.wr(a + 0x30 + 4 * k, struct.pack('<f', val))
            m.wr(vv + 0x44, struct.pack('<3f', 0, 0, 0))
        fase('ruta_giros_alternos', ['w'], 4, zigzag=(0.3, 0.9), pedalear=True)
        if not en_veh():
            P('ruta: CJ ya no esta en la bici; vuelvo a montar'); montar(v, 2)
    IMPULSO[0] = 0.0

ESCENARIOS = {'ruta': lambda s: ruta_usuario(s), 'pie': lambda s: ciclo_pie(s), 'bici481': lambda s: ciclo_vehiculo(481, s, True), 'bici509': lambda s: ciclo_vehiculo(509, s, True), 'bici510': lambda s: ciclo_vehiculo(510, s, True),
              'moto463': lambda s: ciclo_vehiculo(463, s, False), 'moto521': lambda s: ciclo_vehiculo(521, s, False), 'coche411': lambda s: ciclo_vehiculo(411, s, False), 'coche475': lambda s: ciclo_vehiculo(475, s, False)}

def main():
    P('modo', MODO, 'pid', PID)
    if not arrancar(): P('no arranco'); return
    vig = W.Vigilante(PID, MODO, EVID, ring_s=3.0, post_s=3.0, log=arg('--log'), mapa=arg('--mapa'), P=P, imagen='--sin-imagen' not in sys.argv)
    vig.iniciar(); T_INI = time.time()
    esc = (arg('--escenarios', 'pie,bici481,bici510,moto463,coche411')).split(','); minutos = float(arg('--minutos', '10')); por = minutos * 60 / len(esc)
    try:
        for e in esc:
            if not vivo(): break
            P('=== ESCENARIO', e, 'durante %.0f s' % por)
            try: ESCENARIOS[e](por)
            except SystemExit: raise
            except Exception as ex:
                import traceback; P('EXCEPCION', e, repr(ex), traceback.format_exc()[-500:])
            soltar_todo(); bajar()
    except SystemExit as ex: P('salida:', ex)
    finally:
        soltar_todo(); dur = time.time() - T_INI
        P('vivo al final:', vivo(), 'duracion de vigilancia %.0f s (%.2f h)' % (dur, dur / 3600)); vig.detener()
        json.dump({'telemetria': TELE[-400:], 'stats': vig.stats, 'n_eventos': len(vig.eventos), 'duracion_s': dur, 'modo': MODO}, open(os.path.join(sal, 'resumen_skin.json'), 'w'), indent=1, default=str)
        P('RESUMEN stats', json.dumps(vig.stats, default=str)); P('eventos', len(vig.eventos))
main()
