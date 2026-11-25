# Sonda de PRUEBAS DE JUEGO del standalone (mando XInput falso, conduccion, armas, interiores, trucos, camara cinematografica, audio).
# Se lanza desde ejecutar_juego.ps1 (candado gta_lock, carpeta D:\Original\_juego_run, copia de la partida 2). Solo toca el proceso con --pid.
# Uso: python -I sonda_juego.py <salida> <escenario> --pid N --dir <carpeta_ejecucion> --log <log.log>
# Escenarios: base, xinput, conduccion, armas, interiores, trucos, camara, audio  (se pueden encadenar con comas: conduccion,camara)
# Entrada: SendInput real (teclado por codigos de barrido, raton) y, para el mando, el xinput1_4.dll FALSO de esta carpeta (xinput_falso.bin).
import ctypes, struct, sys, time, os, json, math, re, zlib
from ctypes import wintypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(aqui, '..', 'FnTraf'))
import sonda_traf as S
import captura
u = S.u

def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d

# ---------------------------------------------------------------- entrada
class KI(ctypes.Structure):
    _fields_ = [('wVk', wintypes.WORD), ('wScan', wintypes.WORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class MI(ctypes.Structure):
    _fields_ = [('dx', wintypes.LONG), ('dy', wintypes.LONG), ('mouseData', wintypes.DWORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class INP(ctypes.Structure):
    class _U(ctypes.Union): _fields_ = [('ki', KI), ('mi', MI)]
    _anonymous_ = ('u',); _fields_ = [('type', wintypes.DWORD), ('u', _U)]
SC = {c: s for c, s in zip('qwertyuiop', range(0x10, 0x1A))}
SC.update({c: s for c, s in zip('asdfghjkl', range(0x1E, 0x27))}); SC.update({c: s for c, s in zip('zxcvbnm', range(0x2C, 0x33))})
SC.update({'[': 0x1A, ']': 0x1B}); SC.update({'enter': 0x1C, 'space': 0x39, 'lshift': 0x2A, 'ctrl': 0x1D, 'esc': 0x01, 'tab': 0x0F, 'ins': 0x52, 'up': 0x48, 'down': 0x50, 'left': 0x4B, 'right': 0x4D,
           'f1': 0x3B, 'f2': 0x3C, 'num0': 0x52, 'cap': 0x3A, '1': 0x02, '2': 0x03, '3': 0x04, '4': 0x05})
EXT = {'up', 'down', 'left', 'right', 'ins'}
def tecla(n, abajo):
    i = INP(); i.type = 1; i.ki = KI(0, SC[n], 0x8 | (0 if abajo else 0x2) | (0x1 if n in EXT else 0), 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP))
def pulsar(n, t=0.12): tecla(n, True); time.sleep(t); tecla(n, False); time.sleep(0.06)
def raton_boton(b, abajo):   # b: 'l' o 'r'
    i = INP(); i.type = 0; f = {('l', 1): 0x2, ('l', 0): 0x4, ('r', 1): 0x8, ('r', 0): 0x10}[(b, abajo)]; i.mi = MI(0, 0, 0, f, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP))
def rueda(d):
    i = INP(); i.type = 0; i.mi = MI(0, 0, ctypes.c_ulong(d * 120 & 0xFFFFFFFF).value, 0x800, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP))

# ---------------------------------------------------------------- sonda
class J:
    def __init__(s, sal, dirjuego):
        s.sal = sal; s.dir = dirjuego; os.makedirs(sal, exist_ok=True)
        s.f = open(os.path.join(sal, 'sonda_juego.log'), 'w', encoding='utf-8'); s.T0 = time.time(); s.res = {}
        s.exe = 'gta_sa_standalone'; s.m = None; s.h = None; s.vivo = True; s.hora = 12; s.nfoto = 0; s.lpos = 0
    def P(s, *a):
        t = ' '.join(str(x) for x in a); print(t, flush=True); s.f.write('[%7.1f] ' % (time.time() - s.T0) + t + '\n'); s.f.flush()
    def alive(s):
        s.vivo = bool(S.pid(s.exe)); return s.vivo
    def tick(s):
        s.m.wr(0x8D621C, struct.pack('<I', 1)); s.m.wr(0xB70153, bytes([s.hora])); s.m.wr(0xB70152, bytes([0]))
    def dormir(s, seg, paso=0.1):
        t = time.time()
        while time.time() - t < seg:
            if not s.alive(): return False
            s.tick(); time.sleep(paso)
        return True
    # lecturas
    def ped(s): return S.jugador(s.m)
    def pos(s): return s.ped()[1]
    def mat(s):
        p, _ = s.ped(); a = s.m.u32(p + 0x14) if p else 0
        return a
    def yaw(s, mat=None):
        a = mat or s.mat()
        if not a: return float('nan')
        return math.degrees(math.atan2(s.m.f32(a + 0x10), s.m.f32(a + 0x14)))
    def veh(s):
        p, _ = s.ped(); return s.m.u32(p + 0x58C) if p else 0
    def estado_ped(s):
        p, _ = s.ped(); return s.m.u32(p + 0x530) if p else 0
    def en_coche(s): return bool(s.veh()) and s.estado_ped() == 50
    def vel(s, ent):
        return math.sqrt(sum(s.m.f32(ent + 0x44 + 4 * j) ** 2 for j in range(3))) if ent else 0.0
    def vpos(s, v):
        mt = s.m.u32(v + 0x14); return (s.m.f32(mt + 0x30), s.m.f32(mt + 0x34), s.m.f32(mt + 0x38)) if mt else None
    def modelo(s, v): return struct.unpack('<H', s.m.rd(v + 0x22, 2))[0] if v else 0
    def pad(s, n=0):
        d = s.m.rd(0xB73458 + n * 0x134, 0x30)
        nombres = ['LSX', 'LSY', 'RSX', 'RSY', 'L1', 'L2', 'R1', 'R2', 'up', 'down', 'left', 'right', 'Start', 'Select', 'Square', 'Triangle', 'Cross', 'Circle', 'ShockL', 'ShockR', 'chat', 'walk', 'vmouse', 'radiotrk']
        return dict(zip(nombres, struct.unpack('<24h', d[:48]))) if d else {}
    def tiempo(s): return s.m.u32(0xB7CB84)
    def area(s): return s.m.i32(0xB72914)
    def camara(s):
        c = 0xB6F028; act = s.m.u8(c + 0x59)
        return {'activa': act, 'modo': s.m.u32(c + 0x174 + (act % 3) * 0x238 + 0x0C), 'cinema': s.m.u8(c + 0xCF0)}
    def cam_yaw(s):
        a = s.m.u32(0xB6F028 + 0x14)
        if not a: return float('nan')
        return math.degrees(math.atan2(s.m.f32(a + 0x10), s.m.f32(a + 0x14)))
    def ped_rot(s):
        p, _ = s.ped(); return math.degrees(s.m.f32(p + 0x55C)) if p else float('nan')
    def arma(s, slot=None):
        p, _ = s.ped(); act = s.m.u8(p + 0x718)
        ws = []
        for i in range(13):
            b = s.m.rd(p + 0x5A0 + i * 0x1C, 0x1C); tipo, est, clip, tot = struct.unpack('<IIII', b[:16]); ws.append((i, tipo, est, clip, tot))
        return act, ws
    def pools(s):
        r = {}
        for n, a in (('peds', 0xB74490), ('veh', 0xB74494), ('edif', 0xB74498), ('obj', 0xB7449C), ('dummy', 0xB744A0)):
            try: r[n] = S.pool(s.m, a)[0]
            except Exception: r[n] = -1
        return r
    def foto(s, nombre, desc=''):
        r = captura.capturar(os.path.join(s.sal, nombre + '.png'), traer=False); s.P('captura', nombre, json.dumps(r), desc); return r
    def tp(s, x, y, z):
        a = s.mat()
        if a: [s.m.wr(a + 0x30 + 4 * i, struct.pack('<f', v)) for i, v in enumerate((x, y, z))]
    def nan(s, v): return v is None or not all(math.isfinite(x) for x in v)
    def log_nuevas(s, log):
        if not log or not os.path.exists(log): return []
        with open(log, 'rb') as g:
            g.seek(s.lpos); d = g.read(); s.lpos += len(d)
        return d.decode('utf-8', 'replace').splitlines()
    # mando falso
    def xi(s, botones=0, lt=0, rt=0, lx=0, ly=0, rx=0, ry=0, conectado=1):
        s.xi_pkt = getattr(s, 'xi_pkt', 0) + 1
        d = struct.pack('<IIHBBhhhh', s.xi_pkt, conectado, botones, lt, rt, lx, ly, rx, ry) + b'\0\0\0\0'
        ruta = os.path.join(s.dir, 'xinput_falso.bin'); tmp = ruta + '.tmp'
        for _ in range(50):
            try:
                with open(tmp, 'wb') as g: g.write(d)
                os.replace(tmp, ruta); return
            except OSError: time.sleep(0.01)
    def xi_llamadas(s):
        try: return [int(x) for x in open(os.path.join(s.dir, 'xinput_falso_llamadas.txt')).read().split()]
        except Exception: return None
    # trucos
    def tabla_trucos(s): return [struct.unpack('<I', s.m.rd(0x8A5CC8 + 4 * i, 4))[0] for i in range(98)]
    def truco_activo(s, i): return s.m.u8(0x969130 + i)
    def escribir_truco(s, codigo, t=0.09):
        for c in codigo.lower(): pulsar(c, t)
    def inicio(s, salir=True):
        pid = None
        for _ in range(300):
            pid = S.pid(s.exe)
            if pid: break
            time.sleep(1)
        if not pid: s.P('no hay proceso'); return False
        s.m = S.M(pid); s.P('pid', pid)
        if '--nav-menu' in sys.argv:   # juego ORIGINAL sin FastLoader: Enter hasta el menu y la partida (Enter, Enter, y 4 Enter: Start Game / Load / ranura / confirmar), medido a mano
            s.h = captura.buscar(); u.ShowWindow(s.h, 9); u.SetForegroundWindow(s.h); t0 = time.time()
            while time.time() - t0 < 120 and S.pid(s.exe) and s.m.u32(0xC8D4C0) != 9:
                pulsar('enter', 0.2); time.sleep(3.0)
            s.P('navegacion de menu: gGameState=%d tras %.0f s' % (s.m.u32(0xC8D4C0), time.time() - t0))
        if '--pausa' in sys.argv: s.h = captura.buscar(); return True   # navegacion manual del menu (juego original sin FastLoader)
        t0 = time.time()
        while time.time() - t0 < 400 and S.pid(s.exe):
            s.m.wr(0x8D621C, struct.pack('<I', 1)); ped, pos = s.ped()
            if s.m.u32(0xC8D4C0) == 9 and pos and pos[2] == pos[2]: break
            time.sleep(1)
        s.P('juego en marcha tras %.0f s; gGameState=%d' % (time.time() - t0, s.m.u32(0xC8D4C0)))
        if salir:
            ts = time.time()
            while time.time() - ts < 90 and S.pid(s.exe):
                s.m.wr(0x8D621C, struct.pack('<I', 1)); a = s.mat()
                if s.area() == 0 and a: break
                if a: s.m.wr(a + 0x30, struct.pack('<3f', 2496.05, -1692.6, 1014.2))
                time.sleep(0.4)
            s.P('salida de la casa: area=%d tras %.0f s' % (s.area(), time.time() - ts))
        s.h = captura.buscar()
        if s.h: u.ShowWindow(s.h, 9); u.SetForegroundWindow(s.h); time.sleep(1.0)
        rc = wintypes.RECT(); u.GetClientRect(s.h, ctypes.byref(rc)); s.P('ventana: cliente %dx%d' % (rc.right - rc.left, rc.bottom - rc.top))
        return True
    def al_frente(s):
        if s.h: u.SetForegroundWindow(s.h)

def tele(j, veh):
    """(velocidad con signo sobre el frente del coche, pedal de gas, freno, angulo de volante) leidos de la memoria del vehiculo"""
    mt = j.m.u32(veh + 0x14); fx, fy = j.m.f32(mt + 0x10), j.m.f32(mt + 0x14)
    vx, vy = j.m.f32(veh + 0x44), j.m.f32(veh + 0x48)
    return (round(vx * fx + vy * fy, 3), round(j.m.f32(veh + 0x49C), 2), round(j.m.f32(veh + 0x4A0), 2), round(j.m.f32(veh + 0x494), 3))
# ---------------------------------------------------------------- escenarios
SALIDA = (2652.5, -1719.9, 13.8)   # calle junto a la casa (posicion 1 de --robar)
def suelo_libre(j):
    j.tp(*SALIDA); j.dormir(6)

def esc_base(j):
    suelo_libre(j)
    j.P('pools', j.pools(), 'pos', j.pos(), 'cam', j.camara(), 'pad', j.pad())
    j.foto('base0', 'CJ en la calle junto a su casa, vista de seguimiento')
    j.P('tabla de trucos (primeros 10):', [hex(x) for x in j.tabla_trucos()[:10]])

def esc_xinput(j):
    suelo_libre(j)
    # P0: sin archivo de estado el falso devuelve DEVICE_NOT_CONNECTED -> camino DirectInput/teclado
    ll = j.xi_llamadas(); j.P('P0 llamadas a XInputGetState (indices 0..3):', ll)
    j.al_frente(); p0 = j.pos(); tecla('w', True); j.dormir(2.0); pk = j.pad(); tecla('w', False); p1 = j.pos()
    j.P('P0 teclado W 2 s: avance %.2f m, pad.LSY=%d (DirectInput/teclado sigue vivo con el mando falso desconectado)' % (math.dist(p0[:2], p1[:2]), pk.get('LSY', 0)))
    j.dormir(1.0); ll2 = j.xi_llamadas(); j.P('P0 llamadas despues:', ll2, '(sube => CPad::ProcessPad llama a la capa XInput)')
    res = {}
    def fase(nombre, dur=2.5, **kw):
        j.xi(**kw); j.al_frente(); j.dormir(0.3)
        a0 = j.pos(); y0 = j.ped_rot(); c0 = j.cam_yaw(); t0 = j.tiempo(); z0 = a0[2]; zmax = z0; vmax = 0.0; pk = {}
        t = time.time()
        while time.time() - t < dur and j.alive():
            j.tick(); pe, _ = j.ped(); vmax = max(vmax, j.vel(pe)); q = j.pos()
            if q: zmax = max(zmax, q[2])
            k = j.pad()
            for kk, vv in k.items():
                if abs(vv) > abs(pk.get(kk, 0)): pk[kk] = vv
            time.sleep(0.05)
        a1 = j.pos(); y1 = j.ped_rot(); c1 = j.cam_yaw(); t1 = j.tiempo()
        dy = (y1 - y0 + 180) % 360 - 180; dc = (c1 - c0 + 180) % 360 - 180
        r = {'avance_m': round(math.dist(a0[:2], a1[:2]), 2), 'dz_max': round(zmax - z0, 2), 'dyaw_ped': round(dy, 1), 'dyaw_cam': round(dc, 1), 'vmax': round(vmax, 3),
             'dt_juego_ms': t1 - t0, 'pad_max': {k: v for k, v in pk.items() if v}, 'ped_estado': j.estado_ped(), 'cam': j.camara()['modo'], 'veh': hex(j.veh())}
        res[nombre] = r; j.P('FASE', nombre, json.dumps(r)); return r
    fase('reposo_conectado', 2.0)
    fase('stick_izq_arriba', 2.5, ly=32767)
    fase('stick_izq_abajo', 1.5, ly=-32767)
    fase('stick_izq_derecha+arriba', 2.0, ly=32767, lx=32767)
    fase('stick_izq_izquierda+arriba', 2.0, ly=32767, lx=-32767)
    fase('stick_izq_medio(camina)', 2.5, ly=14000)
    j.foto('xi_mover', 'CJ moviendose con el stick izquierdo')
    j.m.wr(0xB6EC2E, b'\x00'); j.P('m_bUseMouse3rdPerson puesto a 0 (la camara de tercera persona deja de ignorar el stick derecho, como al elegir mando en el menu)')
    fase('stick_der_derecha', 2.0, rx=32767)
    fase('stick_der_izquierda', 2.0, rx=-32767)
    fase('stick_der_arriba', 1.5, ry=32767)
    fase('stick_der_abajo', 1.5, ry=-32767)
    j.foto('xi_camara', 'camara tras mover el stick derecho')
    fase('dpad_arriba', 1.0, botones=0x0001); fase('dpad_derecha', 1.0, botones=0x0008)
    fase('A_mantenido+stick', 2.5, botones=0x1000, ly=32767)
    # A pulsado a ritmo (esprint de a pie)
    t = time.time(); j.xi(ly=32767); j.al_frente(); a0 = j.pos(); vmax = 0; n = 0
    while time.time() - t < 3.5 and j.alive():
        j.xi(botones=0x1000 if n % 2 == 0 else 0, ly=32767); n += 1; j.tick(); vmax = max(vmax, j.vel(j.ped()[0])); time.sleep(0.1)
    j.P('FASE A_pulsada_a_ritmo avance=%.2f m vmax=%.3f estado=%d' % (math.dist(a0[:2], j.pos()[:2]), vmax, j.estado_ped()))
    fase('X', 1.5, botones=0x4000)
    fase('B', 1.0, botones=0x2000)
    fase('Y_sin_coche', 1.0, botones=0x8000)
    fase('LB', 1.0, botones=0x0100); fase('RB', 1.0, botones=0x0200); fase('LT', 1.0, lt=255); fase('RT', 1.0, rt=255)
    fase('L3', 0.8, botones=0x0040); fase('R3', 0.8, botones=0x0080); fase('Back', 0.8, botones=0x0020)
    j.xi()
    j.res['xinput'] = res
    # Y: entrar al coche. Se crea un coche con un truco (CADDY) delante de CJ y se pulsa Y
    return res

def esc_xinput_coche(j):
    j.xi(); veh = entrar_coche(j, 'FOURWHEELFUN', via='mando', modelo=471)
    if not veh:
        j.P('Y (Triangulo) NO hizo entrar; se prueba F de teclado como comparacion')
        veh = entrar_coche(j, 'FOURWHEELFUN', via='teclado', modelo=471)
        if not veh: return
    j.foto('xi_en_coche', 'CJ dentro del coche')
    def pasada(n, dur=3.0, **kw):
        j.xi(**kw); j.dormir(0.2); p0 = j.vpos(veh); y0 = j.yaw(j.m.u32(veh + 0x14)); mx = 0; pk = {}
        t = time.time(); tl = []; ult = 0
        while time.time() - t < dur and j.alive():
            j.tick(); mx = max(mx, j.vel(veh))
            for kk, vv in j.pad().items():
                if abs(vv) > abs(pk.get(kk, 0)): pk[kk] = vv
            if time.time() - ult > 0.5: ult = time.time(); tl.append(tele(j, veh))
            time.sleep(0.05)
        p1 = j.vpos(veh); y1 = j.yaw(j.m.u32(veh + 0x14)); dy = (y1 - y0 + 180) % 360 - 180
        r = {'avance': round(math.dist(p0[:2], p1[:2]), 2), 'dyaw': round(dy, 1), 'vmax': round(mx, 3), 'vfin': round(j.vel(veh), 3), 'pad': {k: v for k, v in pk.items() if v}, 'tele(v_frente,gas,freno,volante)': tl}
        j.P('COCHE_FASE', n, json.dumps(r)); return r
    pasada('A_acelera', 5.0, botones=0x1000, lx=0)
    pasada('reposo', 2.0)
    tecla('w', True); pasada('teclado_W_comparacion', 4.0); tecla('w', False)
    pasada('reposo', 2.0)
    pasada('A_acelera_2', 5.0, botones=0x1000, lx=0)
    pasada('A+stick_der', 3.0, botones=0x1000, lx=32767)
    pasada('A+stick_izq', 3.0, botones=0x1000, lx=-32767)
    j.foto('xi_conduciendo', 'conduciendo con A y el stick')
    pasada('X_freno', 3.0, botones=0x4000)
    pasada('RB_freno_de_mano', 2.0, botones=0x0200)
    pasada('RT', 1.5, rt=255); pasada('LT', 1.5, lt=255)
    j.xi(botones=0x8000); j.dormir(0.3); j.xi()
    t = time.time()
    while time.time() - t < 8 and j.en_coche() and j.alive(): j.tick(); time.sleep(0.2)
    j.P('Y para salir: en_coche=%s estado=%d' % (j.en_coche(), j.estado_ped()))
    # Start: pausa. El reloj del juego (CTimer) debe pararse con el menu de pausa
    j.xi(); j.dormir(0.5); t0 = j.tiempo(); j.dormir(1.0); t1 = j.tiempo()
    j.xi(botones=0x0010); j.dormir(0.25); j.xi(); j.dormir(1.0); a = j.tiempo(); j.dormir(1.5); b = j.tiempo()
    j.foto('xi_start', 'pantalla tras pulsar Start (esperado: menu de pausa)')
    j.P('Start: reloj antes (1 s) avanza %d ms; tras Start avanza %d ms en 1.5 s => %s' % (t1 - t0, b - a, 'PAUSA' if b - a < 100 else 'sin pausa'))
    j.xi(botones=0x0010); j.dormir(0.25); j.xi(); j.dormir(1.5); c = j.tiempo(); j.dormir(1.0); d = j.tiempo()
    j.P('Start otra vez: reloj avanza %d ms en 1 s => %s' % (d - c, 'reanuda' if d - c > 500 else 'sigue pausado'))
    j.xi()

CODIGOS_CANDIDATOS = '''HESOYAM LXGIWYL KJKSZPJ UZUMYMW AIWPRTON JUMPJET PANZER OHDUDE RZHSUEW UBHYZHQ NATURALTALENT FOURWHEELFUN FLYINGFISH GHOSTTOWN IOWDLAC LLQPFBN BTCDBCB KVGYZQK YLTEICZ
 BAGOWPG JHJOECW OUIQDMW CIKGCGX ASNAEB OSRBLHH ICIKPYH CVWKXAM LJSPQK PRIEBJ OGXSDAG IAVENJQ CPKTNWT BGLUAWML MROEMZH GUSNHDE PPGWJHT LIYOAAY SZCMAWO BRINGITON AMOMHRER SJMAHPE
 YSOHNUL ROCKETMAN AFPHULTL BMTPWHR ZSOXFSQ XICWMD ZEIIVG AFSNMSMW EHIBXQS MUNASEF AIYPWZQP BIFBUZZ JYSDSOD OFVIAC AJLOJYQY AGBDLCID JQNTDMH THGLOJ URKQSRK EEGCYXT AKJJYGLC WHERESTHEFUNERAL
 JCNRUAP BSXSGGC FVTMNBZ KGGGDKP LFGMHAL KRGTCNH UZUMYMW FOOOXFT CRAZYTOWN YOUWONTTAKEMEALIVE'''.split()
def clave(s):
    tab = []
    for i in range(256):
        r = i
        for _ in range(8): r = (r >> 1) ^ 0xEDB88320 if r & 1 else r >> 1
        tab.append(r)
    c = 0xFFFFFFFF
    for ch in s.upper().encode(): c = tab[(c ^ ch) & 0xFF] ^ (c >> 8)
    return c
def mapa_trucos(j):
    """codigo tecleado -> indice de eCheats segun la tabla de hashes que hay en memoria (0x8A5CC8)"""
    tabla = j.tabla_trucos(); r = {}
    for c in dict.fromkeys(CODIGOS_CANDIDATOS):
        h = clave(c[::-1])
        if h in tabla: r[c] = tabla.index(h)
    return r
NOM = {}
def nombres_cheats():
    if NOM: return NOM
    ruta = os.path.join(aqui, '..', '..', 'source', 'game_sa', 'Enums', 'eCheats.h'); i = 0
    for l in open(ruta, encoding='utf-8'):
        m = re.match(r'\s*(CHEAT_\w+|TOTAL_CHEATS)(\s*=\s*(\d+))?,?', l)
        if m:
            if m.group(3): i = int(m.group(3))
            NOM[m.group(1)] = i; i += 1
    return NOM

def spawn_coche(j, codigo='RZHSUEW', modelo=None):
    """crea un coche con un truco real de spawn (por teclado): devuelve el puntero al coche nuevo mas cercano"""
    antes = set(vehiculos(j))
    j.al_frente(); j.escribir_truco(codigo); j.dormir(2.0)
    nuevos = [v for v in vehiculos(j) if v not in antes and 400 <= j.modelo(v) <= 611 and not j.nan(j.vpos(v))]
    j.P('truco spawn', codigo, 'vehiculos nuevos:', [(hex(v), j.modelo(v)) for v in nuevos])
    if modelo:
        nm = [v for v in nuevos if j.modelo(v) == modelo]
        if nm: return nm[0]
    return nuevos[0] if nuevos else None
def vehiculos(j):
    pv = j.m.u32(0xB74494); objs = j.m.u32(pv); fl = j.m.rd(j.m.u32(pv + 4), j.m.i32(pv + 8)) or b''; r = []
    for i in range(len(fl)):
        if fl[i] & 0x80: continue
        r.append(objs + i * 0xA18)   # el pool de vehiculos tiene elementos de 0xA18 bytes (medido: con 0x988 las lecturas son basura)
    return r

def junto_puerta(j, v):
    mt = j.m.u32(v + 0x14); rx, ry = j.m.f32(mt), j.m.f32(mt + 4); vp = j.vpos(v)
    j.tp(vp[0] - rx * 1.6, vp[1] - ry * 1.6, vp[2] + 0.6)
def entrar_coche(j, codigo='RZHSUEW', via='teclado', modelo=None):
    suelo_libre(j)
    v = spawn_coche(j, codigo, modelo)
    if not v: return 0
    j.dormir(1.0)
    for n in range(4):
        junto_puerta(j, v); j.dormir(1.0); j.al_frente()
        if via == 'mando': j.xi(botones=0x8000); j.dormir(0.3); j.xi()
        else: pulsar('f', 0.15)
        t = time.time()
        while time.time() - t < 6 and not j.en_coche() and j.alive(): j.tick(); time.sleep(0.2)
        j.P('  intento %d (%s): en_coche=%s estado=%d' % (n + 1, via, j.en_coche(), j.estado_ped()))
        if j.en_coche(): break
    j.P('entrar con %s: en_coche=%s modelo=%d' % (via, j.en_coche(), j.modelo(j.veh())))
    return j.veh() if j.en_coche() else 0

def esc_conduccion(j):
    veh = entrar_coche(j, 'FOURWHEELFUN', modelo=471)
    if not veh: j.P('NO se pudo entrar al coche'); return
    j.foto('cond_dentro', 'CJ dentro del coche (tecla F)')
    def prueba(n, teclas, dur=3.0):
        al = [k for k in teclas]
        j.al_frente(); p0 = j.vpos(veh); y0 = j.yaw(j.m.u32(veh + 0x14)); v0 = j.vel(veh); mx = 0
        for k in al: tecla(k, True)
        t = time.time(); tl = []; ult = 0
        while time.time() - t < dur and j.alive():
            j.tick(); mx = max(mx, j.vel(veh))
            if time.time() - ult > 0.5: ult = time.time(); tl.append(tele(j, veh))
            time.sleep(0.05)
        vf = j.vel(veh)
        for k in al: tecla(k, False)
        p1 = j.vpos(veh); y1 = j.yaw(j.m.u32(veh + 0x14)); dy = (y1 - y0 + 180) % 360 - 180
        mt = j.m.u32(veh + 0x14); fx, fy = j.m.f32(mt + 0x10), j.m.f32(mt + 0x14); sentido = (p1[0] - p0[0]) * fx + (p1[1] - p0[1]) * fy
        r = {'v_ini': round(v0, 3), 'v_max': round(mx, 3), 'v_fin': round(vf, 3), 'avance': round(math.dist(p0[:2], p1[:2]), 2), 'dyaw_grados': round(dy, 1), 'avance_sobre_frente': round(sentido, 2), 'nan': j.nan(p1), 'tele(v_frente,gas,freno,volante)': tl}
        j.P('PRUEBA', n, json.dumps(r)); return r
    res = {}
    res['reposo'] = prueba('reposo', [], 2.0)
    res['W_4s'] = prueba('W acelerar 4 s', ['w'], 4.0)
    res['S_frenar'] = prueba('S frenar desde velocidad hacia delante', ['s'], 2.5)
    res['soltar'] = prueba('soltar todo', [], 2.5)
    res['S_atras'] = prueba('S desde parado (marcha atras)', ['s'], 3.5)
    res['soltar2'] = prueba('soltar todo', [], 3.0)
    res['W+A'] = prueba('W+A (giro izquierda)', ['w', 'a'], 3.5)
    j.foto('cond_giro_izq', 'coche girando con W+A')
    res['W+D'] = prueba('W+D (giro derecha)', ['w', 'd'], 3.5)
    j.foto('cond_giro_der', 'coche girando con W+D')
    res['W_otra'] = prueba('W (rumbo recto)', ['w'], 3.0)
    res['freno_mano'] = prueba('Espacio (freno de mano) a velocidad', ['space'], 3.0)
    res['W+Space'] = prueba('W+Espacio', ['w', 'space'], 2.0)
    j.res['conduccion'] = res
    return veh

def esc_camara(j):
    veh = entrar_coche(j)
    if not veh: j.P('NO se pudo entrar al coche'); return
    # modos de camara: V cicla; mantener V (o Insert) activa la camara cinematografica
    j.P('camara inicial', j.camara())
    seq = []
    t0 = time.time()
    def lee(): c = j.camara(); return (c['modo'], c['cinema'], j.m.u8(0xB6EC2D))
    ult = None
    j.al_frente()
    for n in range(6):
        pulsar('v', 0.12); j.dormir(1.5); c = lee(); seq.append(c); j.P('tras V #%d: modo=%d cinema=%d procesada=%d' % (n + 1, c[0], c[1], c[2]))
        if n == 3: j.foto('cam_v4', 'camara tras 4 pulsaciones de V')
    # mantener V (camara cinematografica en PC = mantener la tecla de camara)
    j.P('mantener V 4 s')
    tecla('v', True); t = time.time()
    while time.time() - t < 4 and j.alive():
        j.tick(); c = lee()
        if c != ult: j.P('  t=%.1f modo=%d cinema=%d procesada=%d' % (time.time() - t, *c)); ult = c
        time.sleep(0.1)
    tecla('v', False)
    j.foto('cam_cine', 'camara tras mantener V')
    # tecla Insert (cinematografica en algunas configuraciones)
    j.P('Insert')
    pulsar('ins', 0.2)
    t = time.time(); vistos = set()
    while time.time() - t < 12 and j.alive():
        j.tick(); c = lee()
        if c != ult: j.P('  t=%.1f modo=%d cinema=%d procesada=%d' % (time.time() - t, *c)); ult = c
        vistos.add(c[0]); time.sleep(0.2)
    j.foto('cam_ins', 'camara 12 s tras Insert')
    j.P('modos vistos tras Insert:', sorted(vistos))
    # forzar la camara cinematografica por memoria (m_bCinemaCamera = TheCamera+0xCF0) y vigilar los modos; luego soltarla
    j.P('forzando m_bCinemaCamera=1 durante 40 s dentro del coche (parado y en marcha)')
    j.m.wr(0xB6F028 + 0xCF0, bytes([1])); t = time.time(); modos = {}; ult = None; tecla('w', True)
    while time.time() - t < 40 and j.alive():
        j.tick(); c = lee(); modos[c[0]] = modos.get(c[0], 0) + 1
        if c != ult: j.P('  t=%.1f modo=%d cinema=%d procesada=%d vel=%.3f' % (time.time() - t, *c, j.vel(veh))); ult = c
        if int(time.time() - t) in (8, 20, 32) and not getattr(j, '_f%d' % int(time.time() - t), 0):
            setattr(j, '_f%d' % int(time.time() - t), 1); j.foto('cam_forzada_%d' % int(time.time() - t), 'camara cinematografica forzada, modo %d' % c[0])
        time.sleep(0.2)
    tecla('w', False); j.P('modos vistos con la camara cinematografica forzada (modo: muestras):', modos)
    j.m.wr(0xB6F028 + 0xCF0, bytes([0])); t = time.time(); fin = {}
    while time.time() - t < 10 and j.alive():
        j.tick(); c = lee(); fin[c[0]] = fin.get(c[0], 0) + 1; time.sleep(0.2)
    j.P('modos 10 s despues de soltarla:', fin, 'estado final', lee())
    j.foto('cam_tras_soltar', 'camara tras soltar el modo cinematografico')
    j.res['camara'] = {'seq_V': seq, 'forzada': modos, 'tras_soltar': fin}

def esc_armas(j):
    suelo_libre(j)
    a0 = j.arma(); j.P('armas antes', a0[0], [w for w in a0[1] if w[1]])
    j.al_frente(); j.escribir_truco('LXGIWYL'); j.dormir(2.0)
    act, ws = j.arma(); j.P('armas tras LXGIWYL (set 1): slot activo', act, [w for w in ws if w[1]])
    j.escribir_truco('HESOYAM'); j.dormir(1.0)
    j.foto('armas_ini', 'CJ con armas del truco 1')
    # cambiar de arma con la rueda y comprobar que cambia el slot
    def ciclar(sentido, via):
        if via == 'rueda': rueda(sentido)
        elif via == 'teclas': pulsar(']' if sentido > 0 else '[', 0.15)
        else:
            j.xi(rt=255) if sentido > 0 else j.xi(lt=255); j.dormir(0.2); j.xi()
        j.dormir(0.6)
    for via in ('rueda', 'teclas', 'mando'):
        vistos = [j.arma()[0]]
        for n in range(8): ciclar(1, via); vistos.append(j.arma()[0])
        j.P('cambio de arma por', via, ': slots activos tras 8 pasos:', vistos)
    j.res['cambio_arma'] = True
    # elegir un arma de fuego (el slot con mas municion util: 'AK-47' tipo 30, uzi 28, pistola 22, escopeta 25)
    def elegir(tipos):
        for _ in range(14):
            act, ws = j.arma(); tipo = ws[act][1] if act < len(ws) else 0
            if tipo in tipos: return tipo
            ciclar(1, 'mando'); j.dormir(0.3)
        return 0
    fuego = {}
    for nombre, tipos in (('pistola', {22, 23}), ('escopeta', {25}), ('uzi', {28}), ('ak47', {30})):
        tp_ = elegir(tipos)
        if not tp_: j.P('arma', nombre, 'no encontrada'); continue
        j.P('arma activa', nombre, 'tipo', tp_)
        res_ = {'tipo': tp_}
        for modo in ('raton', 'teclado', 'mando'):
            act, ws = j.arma(); tot0 = ws[act][4]; clip0 = ws[act][3]
            j.al_frente()
            for _ in range(60): S.mouse(3, 0); time.sleep(0.01)   # el raton debe moverse para que RecoverMouseFromRawInputSteal devuelva el raton a DirectInput
            if modo == 'raton': raton_boton('r', 1); j.dormir(0.8)
            elif modo == 'teclado': tecla('ctrl', True); j.dormir(0.2)
            else: j.xi(botones=0x0200); j.dormir(0.8)
            antes_modo = j.camara()['modo']
            if modo == 'raton': raton_boton('l', 1)
            elif modo == 'mando': j.xi(botones=0x0200 | 0x2000)
            t = time.time(); nanc = 0
            while time.time() - t < 2.5 and j.alive():
                j.tick(); p = j.pos()
                if j.nan(p): nanc += 1
                time.sleep(0.05)
            j.foto('armas_disparo_%s_%s' % (nombre, modo), 'CJ disparando con %s (%s)' % (nombre, modo))
            if modo == 'raton': raton_boton('l', 0); raton_boton('r', 0)
            elif modo == 'teclado': tecla('ctrl', False)
            else: j.xi()
            j.dormir(0.6)
            act, ws = j.arma(); tot1 = ws[act][4] if act < len(ws) else -1; clip1 = ws[act][3] if act < len(ws) else -1
            res_[modo] = {'municion_antes': tot0, 'despues': tot1, 'cargador_antes': clip0, 'cargador_despues': clip1, 'camara_modo': antes_modo, 'nan': nanc, 'vivo': j.alive(), 'pools': j.pools()}
            j.P('DISPARO', nombre, modo, json.dumps(res_[modo]))
            if not j.alive(): break
        fuego[nombre] = res_
        if not j.alive(): break
    j.res['armas'] = fuego

def esc_interiores(j):
    suelo_libre(j)
    lista = [('CARLS (casa de CJ)', (2495.33, -1690.75, 13.78), (2496.05, -1692.73, 1014.0), 3),
             ('GYM1 (gimnasio Ganton)', (2229.63, -1721.63, 12.65), (772.112, -4.998, 1000.2), 5),
             ('CLOTHGP (tienda de ropa)', (1456.77, -1138.02, 23.29), (161.391, -96.685, 1001.2), 18),
             ('AMMUN1 (Ammu-Nation)', (1368.35, -1279.06, 12.55), (286.149, -41.544, 1001.0), 1),
             ('BARBERS (peluqueria Idlewood)', (2070.86, -1793.84, 12.66), (411.626, -23.333, 1001.1), 2)]
    res = []
    for nombre, ext, interior, areaesp in lista:
        if not j.alive(): break
        r = {'nombre': nombre}
        t = time.time(); entro = False; p_antes = j.pools()
        for intento, tec in enumerate((None, 'w', 's', 'a', 'd')):
            if j.area() == areaesp: break
            j.tp(ext[0], ext[1], ext[2] + 1.0); j.al_frente()
            if tec: tecla(tec, True)
            t1 = time.time()
            while time.time() - t1 < 6 and j.alive():
                j.tick(); time.sleep(0.25)
                if j.area() == areaesp: break
            if tec: tecla(tec, False)
            r['intento_entrada'] = (intento, tec)
        entro = j.area() == areaesp
        j.dormir(4.0)
        r.update({'entro': entro, 'area': j.area(), 'pos': j.pos(), 'vivo': j.alive(), 'tiempo_entrar_s': round(time.time() - t, 1), 'pools_antes': p_antes, 'pools_dentro': j.pools()})
        j.foto('int_%s' % nombre.split()[0], 'interior %s: CJ en %s, area %d' % (nombre, j.pos(), j.area()))
        if not j.alive(): res.append(r); break
        # marcadores de salida: teleport al marcador de salida (rect de entrada de la enex del interior) y esperar a volver al exterior
        if entro:
            j.dormir(3.0); r['z_dentro'] = (j.pos() or [0, 0, None])[2]; j.foto('int_%s_dentro2' % nombre.split()[0], 'interior %s 3 s despues' % nombre)
            pp = j.pos(); dx, dy = pp[0] - interior[0], pp[1] - interior[1]; nn = math.hypot(dx, dy) or 1.0
            j.tp(interior[0] + dx / nn * 5.0, interior[1] + dy / nn * 5.0, interior[2]); j.dormir(2.0)   # salir del rectangulo del marcador antes de volver a pisarlo
            j.tp(interior[0], interior[1], interior[2]); j.al_frente()
            t = time.time(); salio = False
            while time.time() - t < 20 and j.alive():
                j.tick(); time.sleep(0.25)
                if j.area() == 0: salio = True; break
            j.dormir(4.0)
            r.update({'salio': salio, 'area_final': j.area(), 'pos_final': j.pos(), 'nan': j.nan(j.pos())})
            j.foto('int_%s_fuera' % nombre.split()[0], 'tras salir de %s: area %d pos %s' % (nombre, j.area(), j.pos()))
        res.append(r); j.P('INTERIOR', json.dumps(r, default=str))
        if not entro or not r.get('salio'):   # recuperar
            j.tp(*SALIDA); j.dormir(6)
    j.res['interiores'] = res

def esc_caida_interior(j):
    """Coches/peds del exterior que caen del mapa al entrar a un interior (informe del usuario: Grove Street). Mide por memoria la z de TODOS los vehiculos
    y peds del exterior antes de entrar, durante 25 s dentro y tras salir; cuenta las lineas 'han caido' del log. Entradas: casa de CJ y gimnasio."""
    import collections
    def peds(j):
        pv = j.m.u32(0xB74490); objs = j.m.u32(pv); fl = j.m.rd(j.m.u32(pv + 4), j.m.i32(pv + 8)) or b''; r = []
        for i in range(len(fl)):
            if not fl[i] & 0x80: r.append(objs + i * 0x7C4)
        return r
    def snap():
        d = {}
        for v in vehiculos(j):
            p = j.vpos(v)
            d[('v', v)] = (j.modelo(v), p, j.m.u8(v + 0x2F), j.m.u8(v + 0x4A4) & 0x1F, j.m.u8(v + 0x36) >> 3)
        for q in peds(j):
            mt = j.m.u32(q + 0x14); p = (j.m.f32(mt + 0x30), j.m.f32(mt + 0x34), j.m.f32(mt + 0x38)) if mt else None
            d[('p', q)] = (j.modelo(q), p, j.m.u8(q + 0x2F), 0, 0)
        return d
    def cerca(d, cx, cy, r=250.0):
        return {k: v for k, v in d.items() if v[1] and all(math.isfinite(x) for x in v[1]) and math.hypot(v[1][0] - cx, v[1][1] - cy) < r}
    ext0 = (2495.0, -1685.0, 13.5)
    casa = ('CJ casa', (2495.33, -1690.75, 13.78), (2496.05, -1692.73, 1014.0), 3)
    sitios = [casa] * int(arg('--rondas', 4)) if '--solo-casa' in sys.argv else [casa, ('GYM Ganton', (2229.63, -1721.63, 12.65), (772.112, -4.998, 1000.2), 5)]
    res = []
    j.log_nuevas(arg('--log'))
    for nombre, ext, interior, areaesp in sitios:
        if not j.alive(): break
        j.tp(ext[0] + 8, ext[1] - 8, ext[2] + 1.0); j.dormir(10.0)   # esperar a que el trafico/aparcados se generen alrededor
        if '--wanted' in sys.argv:   # policias a la vista: nivel de busqueda +2 (truco OSRBLHH) dos veces, esperar a que lleguen
            j.al_frente()
            for _ in range(2): j.escribir_truco('OSRBLHH'); j.dormir(1.5)
            j.dormir(14.0)
            pp = j.pos()
            j.P('busqueda: nivel', j.m.u32(j.m.u32(0xB7CD98 + j.m.u8(0xB7CD74) * 0x190) + 0x598 + 0) if False else '?', 'peds', j.pools())
        if '--sin-spawn' not in sys.argv:
            for codigo in ('RZHSUEW', 'FOURWHEELFUN'):
                spawn_coche(j, codigo)
        j.dormir(2.0)
        antes = cerca(snap(), ext[0], ext[1])
        nv = sum(1 for k in antes if k[0] == 'v'); npd = sum(1 for k in antes if k[0] == 'p')
        j.P('SITIO', nombre, 'antes: vehiculos cerca', nv, 'peds cerca', npd, 'minz', min([v[1][2] for v in antes.values()] or [0]), 'modelos', sorted(v[0] for k, v in antes.items() if k[0] == 'v'), 'CJ en', j.pos(),
            'velocidades>0.05 antes de entrar', sorted((v[0], round(j.vel(k[1]), 2)) for k, v in antes.items() if k[0] == 'v' and j.vel(k[1]) > 0.05))
        movers = [k for k in antes if k[0] == 'v' and j.vel(k[1]) > 0.05]
        def traza(et):
            if '--traza' not in sys.argv: return
            o = []
            for k in movers:
                p_ = j.vpos(k[1]); o.append((antes[k][0], tuple(round(x, 1) for x in p_) if p_ else None, round(j.vel(k[1]), 2), j.m.u8(k[1] + 0x36) >> 3, j.m.u8(k[1] + 0x2F)))
            j.P('TRAZA', et, o)
        traza('antes de entrar')
        # entrar
        entro = False
        for intento, tec in enumerate((None, 'w', 's', 'a', 'd')):
            if j.area() == areaesp: break
            j.tp(ext[0], ext[1], ext[2] + 1.0); j.al_frente()
            if tec: tecla(tec, True)
            t1 = time.time()
            while time.time() - t1 < 6 and j.alive():
                j.tick(); time.sleep(0.25)
                if j.area() == areaesp: break
            if tec: tecla(tec, False)
        entro = j.area() == areaesp
        j.P('entro', entro, 'area', j.area())
        minz = collections.defaultdict(lambda: 1e9); serie = []
        t0 = time.time(); caidos = 0
        while time.time() - t0 < float(arg('--dentro', 25)) and j.alive():
            j.tick(); time.sleep(0.5)
            s_ = snap()
            for k in antes:
                if k in s_ and s_[k][1] and math.isfinite(s_[k][1][2]):
                    minz[k] = min(minz[k], s_[k][1][2])
            serie.append(round(time.time() - t0, 1))
            for l in j.log_nuevas(arg('--log')):
                if 'ha caído' in l or 'ha caido' in l or 'han ca' in l.lower():
                    caidos += 1; j.P('LOG', l[:200])
        bajos = [(k[0], hex(k[1]), antes[k][0], 'area%d' % antes[k][2], 'cre%d' % antes[k][3], 'st%d' % antes[k][4], tuple(round(x, 1) for x in antes[k][1]), round(minz[k], 1)) for k in antes if minz[k] < antes[k][1][2] - 3.0]
        r = {'sitio': nombre, 'entro': entro, 'vehiculos_cerca': nv, 'peds_cerca': npd, 'caidas_log': caidos, 'bajaron_mas_de_3m': bajos}
        j.P('RESULTADO', json.dumps(r, default=str)); res.append(r)
        # salir
        j.tp(interior[0], interior[1], interior[2]); j.al_frente()
        t = time.time()
        while time.time() - t < 20 and j.alive():
            j.tick(); time.sleep(0.25)
            if j.area() == 0: break
        traza('dentro (fin)')
        for _t in (1, 2, 4, 8):
            j.dormir(1.0 if _t == 1 else (_t - {2: 1, 4: 2, 8: 4}[_t])); traza('tras salir +%ds' % _t)
        s1 = snap(); j.dormir(5.0); s2 = snap()
        # tras salir: los coches de antes siguen en el mundo, a su altura, y se mueven/colisionan otra vez (z y desplazamiento xy en 5 s)
        vivos = [k for k in antes if k[0] == 'v' and k in s2 and s2[k][1] and k in s1 and s1[k][1]]
        mov = sum(1 for k in vivos if math.hypot(s2[k][1][0] - s1[k][1][0], s2[k][1][1] - s1[k][1][1]) > 0.5)
        caidos2 = sum(1 for k in vivos if s2[k][1][2] < antes[k][1][2] - 3.0)
        r2 = {'tras_salir_vehiculos_antes': sum(1 for k in antes if k[0] == 'v'), 'siguen': len(vivos), 'se_mueven_5s': mov, 'con_z_baja': caidos2,
              'velocidad_max': round(max([j.vel(k[1]) for k in vivos] or [0]), 2), 'velocidades_de_los_que_siguen': sorted((s2[k][0], round(j.vel(k[1]), 2)) for k in vivos)}
        res[-1].update(r2); j.P('TRAS SALIR', json.dumps(r2))
        for l in j.log_nuevas(arg('--log')):
            if 'ha caído' in l or 'ha caido' in l: j.P('LOG(tras salir)', l[:200])
        if j.area() != 0: j.tp(*SALIDA); j.dormir(6)
    j.res['caida_interior'] = res

def esc_pausa(j):
    t = time.time()
    while time.time() - t < float(arg('--pausa-seg', 240)) and j.alive(): time.sleep(1)

def esc_trucos(j):
    suelo_libre(j)
    nm = nombres_cheats(); mp = mapa_trucos(j); j.P('mapa codigo -> indice (tabla en memoria):', mp)
    inv = {v: k for k, v in nm.items()}
    def lee_trafico():
        r = {'NumRandomCars': j.m.i32(0x969094), 'MaxCarsInUse': j.m.i32(0x8A5B24)}; r.update(j.pools()); return r
    j.dormir(5); base = lee_trafico(); j.P('base', base)
    res = {}
    def teclear(codigo, esperar=2.0):
        if codigo not in mp: j.P('codigo', codigo, 'no esta en la tabla'); return None
        i = mp[codigo]; antes = j.truco_activo(i); j.al_frente(); j.escribir_truco(codigo); j.dormir(esperar)
        d = j.truco_activo(i); j.P('TRUCO', codigo, 'indice', i, inv.get(i), 'antes', antes, 'despues', d); return i, antes, d
    # trucos con efecto medible
    for codigo in ('GHOSTTOWN', 'FOURWHEELFUN', 'LLQPFBN', 'IOWDLAC', 'YLTEICZ', 'OUIQDMW', 'BGLUAWML', 'BAGOWPG'):
        pass
    r = teclear('GHOSTTOWN', 1.0)
    if r:
        i = r[0]; j.P('GHOSTTOWN activo; esperando 25 s a que baje el trafico'); j.dormir(25); res['reducido'] = {'indice': i, 'nombre': inv.get(i), 'trafico': lee_trafico(), 'REDUCED_TRAFFIC_flag': j.truco_activo(nm['CHEAT_REDUCED_TRAFFIC'])}
        j.P('trafico con GHOSTTOWN', res['reducido']['trafico']); j.foto('truc_reducido', 'trafico reducido')
        teclear('GHOSTTOWN', 1.0)
    for codigo, esp in (('LLQPFBN', 25), ('IOWDLAC', 25), ('YLTEICZ', 25), ('OUIQDMW', 20), ('BGLUAWML', 15), ('MROEMZH', 15), ('BAGOWPG', 12), ('PRIEBJ', 3), ('CPKTNWT', 3), ('BTCDBCB', 3), ('KVGYZQK', 3), ('JYSDSOD', 3), ('ASNAEB', 2), ('OSRBLHH', 2), ('AIYPWZQP', 2), ('LXGIWYL', 2)):
        if not j.alive(): break
        r = teclear(codigo, 1.0)
        if not r: continue
        j.dormir(esp); d = {'indice': r[0], 'nombre': inv.get(r[0]), 'activo': j.truco_activo(r[0]), 'trafico': lee_trafico(), 'pos': j.pos(), 'vivo': j.alive()}
        res[codigo] = d; j.P('EFECTO', codigo, json.dumps(d, default=str)); j.foto('truc_%s' % codigo, 'truco %s (%s)' % (codigo, inv.get(r[0])))
        # coche de trafico: lee el estado de conduccion agresiva/color de los coches cercanos
        if codigo in ('LLQPFBN', 'IOWDLAC'):
            cols = []
            for v in vehiculos(j)[:30]:
                cols.append((j.modelo(v), j.m.u8(v + 0x434), j.m.u8(v + 0x435)))  # m_nPrimaryColor/Secondary en CVehicle (0x434/0x435)
            d['colores_vehiculos'] = cols; j.P('  colores (modelo, prim, sec):', cols)
        teclear(codigo, 0.5)
    j.res['trucos'] = res

def esc_audio(j):
    suelo_libre(j)
    log = arg('--log'); l = j.log_nuevas(log)
    j.P('lineas de log hasta ahora:', len(l), 'con audio:', len([x for x in l if re.search('audio|sfx|radio|bank|sound', x, re.I)]))
    # contadores de audio de memoria del propio juego: AudioEngine (0xB6BC90), canales activos, radio
    j.P('(solo trazas; sin forma de oir)')

def esc_spawn(j):
    suelo_libre(j)
    j.al_frente()
    for codigo in ('RZHSUEW', 'UBHYZHQ'):
        antes = set(vehiculos(j)); j.escribir_truco(codigo); j.dormir(3.0)
        j.foto('spawn_%s' % codigo, 'tras teclear %s' % codigo)
        for v in vehiculos(j):
            mt = j.m.u32(v + 0x14); j.P(' veh', hex(v), 'NUEVO' if v not in antes else '', 'modelo', j.modelo(v), 'matriz', hex(mt), 'pos', j.vpos(v), 'conductor', hex(j.m.u32(v + 0x460)), 'creado_por', j.m.u8(v + 0x394))
        j.P('pos CJ', j.pos(), 'ult cheat str', j.m.rd(0x969110, 30))
def esc_camara_aire(j):
    for codigo, nombre, mod in (('OHDUDE', 'Hunter', 425), ('JHJOECW', 'Hydra', 520)):
        if not j.alive(): break
        j.P('--- aeronave', nombre, codigo)
        veh = entrar_coche(j, codigo, modelo=mod)
        if not veh: j.P('no se pudo entrar a', nombre); continue
        mod = j.modelo(veh); j.P('dentro de modelo', mod)
        modos = {}; ult = None; t = time.time(); j.m.wr(0xB6F028 + 0xCF0, bytes([1]))
        while time.time() - t < 45 and j.alive():
            j.tick(); c = j.camara(); k = (c['modo'], c['cinema'], j.m.u8(0xB6EC2D)); modos[k[0]] = modos.get(k[0], 0) + 1
            if k != ult: j.P('  t=%.1f modo=%d cinema=%d procesada=%d' % (time.time() - t, *k)); ult = k
            if int(time.time() - t) in (10, 25, 40) and not getattr(j, '_g%d' % int(time.time() - t), 0):
                setattr(j, '_g%d' % int(time.time() - t), 1); j.foto('camaire_%s_%d' % (nombre, int(time.time() - t)), '%s camara cinematografica forzada modo %d' % (nombre, k[0]))
            time.sleep(0.2)
        j.P('modos vistos', nombre, modos)
        j.m.wr(0xB6F028 + 0xCF0, bytes([0])); fin = {}; t = time.time()
        while time.time() - t < 10 and j.alive(): j.tick(); c = j.camara(); fin[c['modo']] = fin.get(c['modo'], 0) + 1; time.sleep(0.2)
        j.P('tras soltar', nombre, fin, 'vivo', j.alive(), 'pos', j.pos())
        j.res['camara_aire_' + nombre] = {'forzada': modos, 'tras': fin}
        # salir y quitar la aeronave del camino: teleport de CJ lejos
        pulsar('f', 0.15); j.dormir(3)
def esc_gimnasio(j):
    suelo_libre(j)
    j.tp(2229.63, -1721.63, 13.65); j.al_frente(); t = time.time()
    while time.time() - t < 25 and j.alive() and j.area() != 5: j.tick(); time.sleep(0.25)
    j.P('entrada al gimnasio: area', j.area(), 'tras %.1f s' % (time.time() - t))
    ult = 0; t0 = time.time()
    while time.time() - t0 < 40 and j.alive():
        j.tick()
        if time.time() - ult > 3: ult = time.time(); j.P('  t=%.0f area=%d pos=%s pools=%s' % (time.time() - t0, j.area(), j.pos(), j.pools()))
        if int(time.time() - t0) in (5, 15, 30) and not getattr(j, '_gy%d' % int(time.time() - t0), 0):
            setattr(j, '_gy%d' % int(time.time() - t0), 1); j.foto('gym_%d' % int(time.time() - t0), 'dentro del gimnasio a los %d s' % int(time.time() - t0))
        time.sleep(0.2)
    j.P('vivo al final del gimnasio:', j.alive())
def modulos_proceso(j):
    """nombres de los modulos cargados en el proceso del juego (EnumProcessModulesEx + GetModuleBaseNameW); para saber si Media Foundation llego a cargarse"""
    ps = ctypes.WinDLL('psapi', use_last_error=True); arr = (ctypes.c_void_p * 1024)(); n = wintypes.DWORD()
    ps.EnumProcessModulesEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.DWORD, ctypes.POINTER(wintypes.DWORD), wintypes.DWORD]
    ps.GetModuleBaseNameW.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_wchar_p, wintypes.DWORD]
    if not ps.EnumProcessModulesEx(j.m.h, arr, ctypes.sizeof(arr), ctypes.byref(n), 3): return []
    r = []
    for i in range(n.value // ctypes.sizeof(ctypes.c_void_p)):
        b = ctypes.create_unicode_buffer(260)
        if ps.GetModuleBaseNameW(j.m.h, arr[i], b, 260): r.append(b.value.lower())
    return r
def esc_usertrack(j):
    # Radio de pistas de usuario (CAEMFDecoder / Media Foundation): CJ en un coche y se pide la emisora RADIO_USER_TRACKS (12) por la peticion de script
    # de CAERadioTrackManager (0x8CB6F8 + 0x7C = m_iRadioStationScriptRequest, la misma variable que escribe RetuneRadio). Registra emisora (+0x74) y modo (+0x68) cada 2 s.
    veh = entrar_coche(j, 'RZHSUEW')   # coche con radio (el quad de FOURWHEELFUN no tiene)
    if not veh: j.P('NO se pudo entrar al coche'); return
    j.dormir(2.0)
    base = 0x8CB6F8
    j.P('radio antes: emisora', j.m.u32(base + 0x74), 'modo', j.m.u32(base + 0x68), 'peticion script', j.m.u32(base + 0x7C))
    j.P('volcado radio antes:', [hex(j.m.u32(base + 4 * k)) for k in range(0, 64)]); j.P('gestor de pistas de usuario 0xB6B970:', [hex(j.m.u32(0xB6B970 + 4 * k)) for k in range(0, 10)])
    j.P('modulos antes de pedir la emisora:', sorted(m for m in modulos_proceso(j) if re.search(r'^mf|mp3|msmpeg|msaud|wmvcore|quicktime|qtml|rtworkq', m)))
    j.m.wr(base + 0x7C, struct.pack('<i', 12))
    t0 = time.time(); ult = 0; hist = []
    while time.time() - t0 < 60 and j.alive():
        j.tick()
        if time.time() - ult > 2:
            ult = time.time(); e = (j.m.u32(base + 0x74), j.m.u32(base + 0x68), j.m.u32(base + 0x7C)); hist.append(e)
            j.P('  t=%.0f radio emisora=%d modo=%d peticion=%d' % (time.time() - t0, *[x - (1 << 32) if x >> 31 else x for x in e]))
        time.sleep(0.2)
    mods = [m for m in modulos_proceso(j) if re.search(r'^mf|mp3|msmpeg|msaud|wmvcore|quicktime|qtml|rtworkq|mfperf', m)]; j.P('modulos de Media Foundation / decodificadores cargados:', sorted(mods)); j.res['modulos_mf'] = sorted(mods)
    j.P('volcado radio despues:', [hex(j.m.u32(base + 4 * k)) for k in range(0, 64)]); j.P('gestor de pistas de usuario despues:', [hex(j.m.u32(0xB6B970 + 4 * k)) for k in range(0, 10)])
    j.res['usertrack'] = hist
    j.foto('usertrack', 'CJ en coche con la emisora de pistas de usuario pedida')
def esc_altf4(j):
    # SilentPatch AltF4Fix: se PUBLICA WM_SYSKEYDOWN/WM_SYSKEYUP de F4 con el bit de contexto Alt (como Alt+F4) en la ventana del juego (PostMessage: no toca el teclado real ni la ventana en primer plano).
    # Con la correccion el DefWindowProc convierte el mensaje en SC_CLOSE -> WM_CLOSE y el proceso termina; sin ella el mensaje se traga y el juego sigue vivo.
    suelo_libre(j)
    if not j.h: j.P('SIN ventana: no se puede probar'); return
    j.dormir(2.0)
    pm = ctypes.WinDLL('user32').PostMessageW; pm.argtypes = [wintypes.HWND, wintypes.UINT, wintypes.WPARAM, wintypes.LPARAM]
    WM_SYSKEYDOWN, WM_SYSKEYUP, VK_F4 = 0x104, 0x105, 0x73
    sc = 0x3E
    pm(j.h, WM_SYSKEYDOWN, VK_F4, (1 << 29) | (sc << 16) | 1)
    pm(j.h, WM_SYSKEYUP, VK_F4, ctypes.c_long((1 << 31) | (1 << 30) | (1 << 29) | (sc << 16) | 1).value)
    t0 = time.time(); salio = False
    while time.time() - t0 < 8:
        if not j.alive(): salio = True; break
        time.sleep(0.2)
    j.P('ALTF4 proceso terminado tras Alt+F4 publicado: %s (%.1f s)' % (salio, time.time() - t0)); j.res['altf4_salio'] = salio
def leer_vehiculos(j):
    """(modelo, vida, estado-ruedas[4], estado-paneles) de cada vehiculo vivo del pool; DamageManager en +0x5A0: ruedas en +5..+8, paneles en +0x14"""
    r = []
    for v in vehiculos(j):
        m = j.modelo(v)
        if not (400 <= m <= 611): continue
        dm = j.m.rd(v + 0x5A0, 0x18) or b'\0' * 0x18
        r.append({'v': hex(v), 'modelo': m, 'vida': round(j.m.f32(v + 0x4C0), 1), 'ruedas': list(dm[5:9]), 'paneles': struct.unpack('<I', dm[0x14:0x18])[0]})
    return r

def esc_sp2veh(j):
    """SilentPatch 2 (vehiculos): voladura masiva (BIGBANG) midiendo las ruedas que pierden los coches por memoria, Hydra en vuelo y Rhino; capturas de cada fase"""
    suelo_libre(j)
    res = {'rondas': []}
    perdidas = [0, 0, 0, 0]; coches = 0; paneles_danados = 0
    for ronda in range(6):
        if not j.alive(): break
        for codigo in ('RZHSUEW', 'UBHYZHQ', 'FOURWHEELFUN'):
            spawn_coche(j, codigo)
        j.dormir(1.0)
        antes = {d['v']: d for d in leer_vehiculos(j)}
        j.al_frente(); j.escribir_truco('CPKTNWT'); j.dormir(4.0)
        despues = leer_vehiculos(j)
        for d in despues:
            a = antes.get(d['v'])
            if a and a['modelo'] == d['modelo'] and d['vida'] <= 0.5:
                coches += 1
                for i in range(4):
                    if d['ruedas'][i] == 2: perdidas[i] += 1
                if d['paneles']: paneles_danados += 1
        j.P('RONDA', ronda, 'vehiculos tras BIGBANG', json.dumps(despues)[:900])
        if ronda in (0, 3): j.foto('bigbang_%d' % ronda, 'tras BIGBANG')
        res['rondas'].append(len(despues))
        suelo_libre(j)
    res['coches_destruidos'] = coches; res['ruedas_perdidas_por_indice(DI,TI,DD,TD)'] = perdidas; res['con_paneles_danados'] = paneles_danados
    j.P('RESUMEN voladura: coches', coches, 'ruedas perdidas [delantera-izq, trasera-izq, delantera-der, trasera-der]', perdidas, 'con paneles danados', paneles_danados)
    for codigo, nombre, mod in (('JHJOECW', 'Hydra', 520), ('AIWPRTON', 'Rhino', 432)):
        if not j.alive(): break
        veh = entrar_coche(j, codigo, modelo=mod)
        if not veh: j.P('no se pudo entrar a', nombre); continue
        j.dormir(4.0); j.foto('veh_%s_a' % nombre, '%s dentro (motor en marcha)' % nombre)
        tecla('w', True); j.dormir(3.0); j.foto('veh_%s_b' % nombre, '%s acelerando' % nombre); tecla('w', False)
        res[nombre] = {'modelo': j.modelo(j.veh()), 'vel': round(j.vel(j.veh()), 2)}
    j.res['sp2veh'] = res

def esc_entrada(j):
    """SilentPatch (segunda tanda, entrada/camara/replay): latencia del teclado (tecla -> estado del mando, N pruebas), replay con F1 (inicio, fin y pulsaciones rapidas) y vida del proceso."""
    import statistics, random
    suelo_libre(j); j.al_frente(); j.dormir(4)
    # --- raton: movimiento relativo real (SendInput) con el cursor confinado (ClipCursorToGameWindow): lectura de CMouseControllerState New (0xB73418, mov float x2 en +0xC) y del rectangulo de ClipCursor
    def mov_raton(dx, dy):
        i = INP(); i.type = 0; i.mi = MI(dx, dy, 0, 0x0001, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP))
    rc = wintypes.RECT(); u.GetClipCursor(ctypes.byref(rc)); cr = wintypes.RECT(); u.GetClientRect(j.h, ctypes.byref(cr))
    pt = wintypes.POINT(0, 0); u.ClientToScreen(j.h, ctypes.byref(pt))
    j.P('CLIP rect=(%d,%d,%d,%d) cliente en pantalla=(%d,%d,%d,%d)' % (rc.left, rc.top, rc.right, rc.bottom, pt.x, pt.y, pt.x + cr.right - 1, pt.y + cr.bottom - 1))
    j.res['clip'] = {'clip': [rc.left, rc.top, rc.right, rc.bottom], 'cliente': [pt.x, pt.y, pt.x + cr.right - 1, pt.y + cr.bottom - 1]}
    # calentamiento: RecoverMouseFromRawInputSteal (Pad.cpp) necesita >= 30 eventos de movimiento de SDL antes de devolver el raton a DirectInput (docs/CodigoVivoExe/ejecucion-raton.md)
    for n in range(60): j.tick(); mov_raton(3, 1); time.sleep(0.012)
    j.dormir(3.0); mov_raton(-180, -60); j.dormir(1.0)
    movs = []; yaw0 = j.cam_yaw()
    for n in range(8):
        j.tick(); j.al_frente(); mov_raton(30, 12); t = time.perf_counter(); visto = (0.0, 0.0)
        while time.perf_counter() - t < 0.15:   # el delta del raton dura un solo fotograma (~33 ms): se muestrea cada ~1 ms
            b = j.m.rd(0xB73418, 0x14); v = struct.unpack('<ff', b[0xC:0x14]) if b else (0.0, 0.0)
            if abs(v[0]) + abs(v[1]) > abs(visto[0]) + abs(visto[1]): visto = v
        movs.append(visto)
    j.P('camara: giro tras 8 movimientos de raton (grados)', round(j.cam_yaw() - yaw0, 2))
    j.res['cam_giro_raton'] = j.cam_yaw() - yaw0
    # empuja el cursor contra el borde derecho/inferior (ClipCursor debe retenerlo dentro de la ventana)
    for n in range(40): mov_raton(400, 300); time.sleep(0.01)
    cur = wintypes.POINT(); u.GetCursorPos(ctypes.byref(cur))
    j.P('RATON New.mov por fotograma', movs, 'cursor tras empujar al borde', (cur.x, cur.y))
    j.res['raton'] = {'mov': movs, 'cursor': [cur.x, cur.y]}
    mov_raton(-2000, -1500); time.sleep(0.3)
    # --- latencia de teclado: W abajo -> LSY != 0 en el mando 0 (se lee la memoria cada ~0,5 ms)
    lat = []
    for n in range(60):
        if not j.alive(): break
        j.tick(); j.al_frente()
        t0 = time.perf_counter(); tecla('w', True); t1 = None
        while time.perf_counter() - t0 < 0.5:
            if j.pad().get('LSY', 0) != 0: t1 = time.perf_counter(); break
        tecla('w', False)
        if t1 is not None: lat.append((t1 - t0) * 1000)
        t = time.time()
        while time.time() - t < 1.0 and j.alive() and j.pad().get('LSY', 0) != 0: time.sleep(0.002)
        time.sleep(random.uniform(0.0, 0.06))
    if lat:
        lat.sort(); j.P('LATENCIA teclado (ms) n=%d media=%.1f mediana=%.1f p10=%.1f p90=%.1f min=%.1f max=%.1f' % (len(lat), statistics.mean(lat), statistics.median(lat), lat[len(lat) // 10], lat[(9 * len(lat)) // 10], lat[0], lat[-1]))
        j.res['latencia_teclado_ms'] = {'n': len(lat), 'media': statistics.mean(lat), 'mediana': statistics.median(lat), 'p10': lat[len(lat) // 10], 'p90': lat[(9 * len(lat)) // 10]}
    # --- replay: F1 inicia la reproduccion (CReplay::Mode 0xA43088: 0 = grabar, 1 = reproducir), F1 la termina (FinishPlayback)
    j.dormir(15)
    for ronda in range(3):
        if not j.alive(): break
        m0 = j.m.u8(0xA43088); j.al_frente(); pulsar('f1', 0.15); t = time.time()
        while time.time() - t < 6 and j.alive() and j.m.u8(0xA43088) != 1: j.tick(); time.sleep(0.05)
        m1 = j.m.u8(0xA43088); j.P('replay ronda %d: modo %d -> %d tras %.1f s; pos' % (ronda, m0, m1, time.time() - t), j.pos())
        if m1 == 1:
            j.dormir(5); j.foto('replay_%d' % ronda, 'durante la reproduccion del replay')
            j.al_frente(); pulsar('f1', 0.15); t = time.time()
            while time.time() - t < 6 and j.alive() and j.m.u8(0xA43088) != 0: j.tick(); time.sleep(0.05)
            j.P('replay ronda %d: fin -> modo %d tras %.1f s' % (ronda, j.m.u8(0xA43088), time.time() - t))
        j.dormir(8)
    # pulsaciones rapidas del boton de replay (mash)
    for n in range(12):
        if not j.alive(): break
        j.al_frente(); pulsar('f1', 0.05); time.sleep(0.25); j.tick()
    j.dormir(10)
    j.P('tras el mash: modo replay', j.m.u8(0xA43088), 'vivo', j.alive(), 'pos', j.pos(), 'pools', j.pools())
    j.res['replay_final'] = {'modo': j.m.u8(0xA43088), 'vivo': j.alive()}
    u.ClipCursor(None)   # por seguridad: libera el rectangulo de ClipCursor que haya dejado el juego

def foto_gr(j, nombre, desc=''):
    # captura de ventana completa DPI-aware (SetProcessDPIAware antes de medir) y validacion del tamano contra RsGlobal (0xC17044 ancho, 0xC17048 alto) y el cliente de la ventana
    u.SetProcessDPIAware()
    j.h = captura.buscar()
    rc = wintypes.RECT(); u.GetClientRect(j.h, ctypes.byref(rc))
    r = captura.capturar(os.path.join(j.sal, nombre + '.png'), traer=False)
    rw, rh = j.m.u32(0xC17044), j.m.u32(0xC17048)
    ok = bool(r) and r['ancho'] == rc.right - rc.left and r['alto'] == rc.bottom - rc.top and (r['ancho'], r['alto']) == (rw, rh)
    j.P('captura', nombre, json.dumps(r), desc, '| cliente %dx%d, RsGlobal %dx%d, tamano %s' % (rc.right - rc.left, rc.bottom - rc.top, rw, rh, 'VALIDADO' if ok else 'NO COINCIDE'))
    return r

def esc_fps(j):
    # SilentPatch segunda tanda (graficos): FPS real = fotogramas del juego (CTimer::m_FrameCounter 0xB7CB4C) entre segundos de reloj de pared, y ritmo del tiempo de juego (ms / s de pared)
    suelo_libre(j)
    j.dormir(3.0)
    for k in range(4):
        f0 = j.m.u32(0xB7CB4C); g0 = j.tiempo(); t0 = time.time()
        j.dormir(15.0)
        f1 = j.m.u32(0xB7CB4C); g1 = j.tiempo(); t1 = time.time()
        j.P('FPS tramo %d: %.2f fotogramas/s; tiempo de juego %.1f ms por s de pared (fotogramas %d en %.2f s)' % (k, (f1 - f0) / (t1 - t0), (g1 - g0) / (t1 - t0), f1 - f0, t1 - t0))

def esc_graficos(j):
    # SilentPatch segunda tanda (graficos): capturas de ventana completa de dia, de noche (luna, coronas) y con el HUD; la ventana del juego mide lo que diga el log
    suelo_libre(j)
    j.al_frente()
    for hora, nom in ((12, 'dia'), (23, 'noche'), (3, 'madrugada')):
        j.hora = hora
        j.dormir(3.0)
        foto_gr(j, 'gr_' + nom, 'hora %d' % hora)
    j.hora = 12
    j.P('pools', j.pools(), 'pos', j.pos(), 'cam', j.camara())

def girar_camara(j, objetivo, max_iter=60):
    # gira la camara con movimientos relativos del raton (SendInput) hasta que su rumbo (0 = norte +Y, 180 = sur) este cerca de 'objetivo' grados; aprende el signo con la primera medida
    signo = 1
    previo = j.cam_yaw()
    for n in range(max_iter):
        yaw = j.cam_yaw()
        dif = ((objetivo - yaw + 540.0) % 360.0) - 180.0
        if abs(dif) < 12.0: return yaw
        if n and abs(dif) > abs(((objetivo - previo + 540.0) % 360.0) - 180.0) + 1.0: signo = -signo
        previo = yaw
        i = INP(); i.type = 0; i.mi = MI(int(max(-60, min(60, signo * dif * 2))), 0, 0, 0x0001, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP))
        j.tick(); time.sleep(0.12)
    return j.cam_yaw()

def esc_luna(j):
    # SilentPatch segunda tanda (graficos): la luna (fases) y las estrellas se ven mirando al sur (-Y) de madrugada (03:00; la luna se dibuja en 00:00-07:20): CJ de cara al sur, camara tras el
    suelo_libre(j)
    j.al_frente()
    j.hora = 3
    j.dormir(2.0)
    j.P('formato del buffer trasero D3DFORMAT=%d (21 = A8R8G8B8, 22 = X8R8G8B8); tipo de multimuestreo %d' % (j.m.u32(0xC9C048), j.m.u32(0xC9C050)))
    j.P('rumbo de la camara antes %.1f' % j.cam_yaw())
    yaw = girar_camara(j, 180.0)
    j.P('rumbo de la camara despues %.1f (180 = sur, donde sale la luna)' % yaw)
    for dia in (4, 10, 16, 24):   # CClock::ms_nGameClockDays (0xB70154): el desplazamiento de la mascara de la luna depende del dia (fase)
        j.m.wr(0xB70154, bytes([dia])); j.dormir(1.5); j.m.wr(0xB70154, bytes([dia]))
        foto_gr(j, 'luna_dia%02d' % dia, 'hora 3, camara al sur (rumbo %.0f), dia de juego %d' % (j.cam_yaw(), j.m.u8(0xB70154)))
    j.hora = 12


def foto_bb(j, nombre, desc=''):
    # captura de ventana completa con BitBlt desde el DC de la pantalla (CAPTUREBLT), DPI-aware (SetProcessDPIAware antes de medir): lo que de verdad se ve en pantalla (PrintWindow devuelve un fotograma viejo tras un Reset del dispositivo)
    u.SetProcessDPIAware(); g = ctypes.WinDLL('gdi32', use_last_error=True)
    j.h = captura.buscar(); u.ShowWindow(j.h, 9); u.SetForegroundWindow(j.h); time.sleep(0.3)
    rc = wintypes.RECT(); u.GetClientRect(j.h, ctypes.byref(rc)); pt = wintypes.POINT(0, 0); u.ClientToScreen(j.h, ctypes.byref(pt))
    w, h = rc.right - rc.left, rc.bottom - rc.top
    sdc = u.GetDC(0); mdc = g.CreateCompatibleDC(sdc); bm = g.CreateCompatibleBitmap(sdc, w, h); g.SelectObject(mdc, bm)
    g.BitBlt(mdc, 0, 0, w, h, sdc, pt.x, pt.y, 0x00CC0020 | 0x40000000)
    bi = captura.BIH(ctypes.sizeof(captura.BIH), w, -h, 1, 32, 0, 0, 0, 0, 0, 0); buf = ctypes.create_string_buffer(w * h * 4)
    g.GetDIBits(mdc, bm, 0, h, buf, ctypes.byref(bi), 0); g.DeleteObject(bm); g.DeleteDC(mdc); u.ReleaseDC(0, sdc)
    d = buf.raw; rgb = bytearray(w * h * 3); rgb[0::3] = d[2::4]; rgb[1::3] = d[1::4]; rgb[2::3] = d[0::4]
    raw = b''.join(bytes([0]) + bytes(rgb[y * w * 3:(y + 1) * w * 3]) for y in range(h))
    def ch(t, dd): return struct.pack('>I', len(dd)) + t + dd + struct.pack('>I', zlib.crc32(t + dd) & 0xffffffff)
    open(os.path.join(j.sal, nombre + '.png'), 'wb').write(bytes([137]) + b'PNG' + bytes([13, 10, 26, 10]) + ch(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) + ch(b'IDAT', zlib.compress(raw, 6)) + ch(b'IEND', b''))
    rw, rh = j.m.u32(0xC17044), j.m.u32(0xC17048)
    j.P('captura BitBlt', nombre, '%dx%d' % (w, h), desc, '| cliente %dx%d, RsGlobal %dx%d, %s; media %.1f' % (w, h, rw, rh, 'tamano VALIDADO' if (w, h) == (rw, rh) else 'cliente mayor que RsGlobal (el juego dibuja en la esquina superior izquierda %dx%d)' % (rw, rh), sum(d[0::4][::97]) / max(1, len(d[0::4][::97]))))
    return w, h
FEM = 0xBA6748   # FrontEndMenuManager: +0xC8 m_nPrefsAntialiasing, +0xCC m_nDisplayAntialiasing, +0xD4 m_nPrefsVideoMode
def msaa_estado(j):
    r = {'max': j.m.u32(0x8E242C), 'sel': j.m.u32(0x8E2430), 'maxNM': j.m.u32(0x8E2434), 'selNM': j.m.u32(0x8E2438), 'present': j.m.u32(0xC9C050), 'calidad': j.m.u32(0xC9C054),
         'prefs': j.m.u32(FEM + 0xC8), 'display': j.m.u32(FEM + 0xCC), 'swap': j.m.u32(0xC9C058), 'formato': j.m.u32(0xC9C048)}
    return r
def decodificar_png(ruta):
    d = open(ruta, 'rb').read(); i = 8; idat = b''
    while i < len(d):
        n = struct.unpack('>I', d[i:i + 4])[0]; t = d[i + 4:i + 8]
        if t == b'IHDR': w, h = struct.unpack('>II', d[i + 8:i + 16])
        if t == b'IDAT': idat += d[i + 8:i + 8 + n]
        i += 12 + n
    raw = zlib.decompress(idat); fila = w * 3 + 1
    return w, h, [raw[y * fila + 1:(y + 1) * fila] for y in range(h)]
def metricas(ruta, region=None):
    # region = (x0, y0, x1, y1). colores distintos, pares horizontales/verticales de vecinos duros (|dif de luminancia| > 80) y suaves (12..80)
    w, h, filas = decodificar_png(ruta); x0, y0, x1, y1 = region or (0, 0, w, h)
    colores = set(); duros = suaves = 0
    def lum(f, x): return (f[3 * x] * 77 + f[3 * x + 1] * 150 + f[3 * x + 2] * 29) >> 8
    for y in range(y0, y1 - 1):
        f = filas[y]; g = filas[y + 1]
        for x in range(x0, x1 - 1):
            colores.add(f[3 * x:3 * x + 3])
            l = lum(f, x); d1 = abs(l - lum(f, x + 1)); d2 = abs(l - lum(g, x))
            for d in (d1, d2):
                if d > 80: duros += 1
                elif d >= 12: suaves += 1
    return {'colores': len(colores), 'duros': duros, 'suaves': suaves, 'suaves_por_duro': round(suaves / max(duros, 1), 3)}
def tecleo(j, teclas, pausa=0.9):
    for k in teclas:
        pulsar(k, 0.15); j.dormir(pausa)
CURSOR = [None]
def descubrir_cursor(j):
    # abre el menu de pausa (cursor 0), pulsa abajo dos veces y busca el int32 de CMenuManager (primeros 0x300 bytes) que va 0 -> 1 -> 2: m_nCurrentScreenItem
    tecleo(j, ['esc'], 1.5); s0 = j.m.rd(FEM, 0x300); tecleo(j, ['down'], 0.8); s1 = j.m.rd(FEM, 0x300); tecleo(j, ['down'], 0.8); s2 = j.m.rd(FEM, 0x300)
    cand = [o for o in range(0, 0x300 - 3, 4) if struct.unpack_from('<I', s0, o)[0] == 0 and struct.unpack_from('<I', s1, o)[0] == 1 and struct.unpack_from('<I', s2, o)[0] == 2]
    j.P('candidatos a m_nCurrentScreenItem (offsets):', [hex(o) for o in cand]); CURSOR[0] = FEM + cand[0] if cand else None
    if CURSOR[0]: elegir(j, 0, 2.5)   # Reanudar
def cursor(j): return j.m.u32(CURSOR[0])
def ir_a(j, item, maxp=14):
    for _ in range(maxp):
        if cursor(j) == item: return True
        pulsar('down', 0.15); j.dormir(0.7)
    return cursor(j) == item
def elegir(j, item, pausa=1.5):
    ok = ir_a(j, item); pulsar('enter', 0.15); j.dormir(pausa); return ok
def menu_hasta_aa(j):
    # pausa -> Opciones (6.o) -> Config. pantalla (3.o) -> Avanzado (7.o) -> Anti Aliasing (6.o de la pagina avanzada)
    tecleo(j, ['esc'], 1.5); elegir(j, 5); elegir(j, 2); elegir(j, 6); ir_a(j, 5)
def menu_salir(j):
    # avanzado: Atras (8.o) -> pantalla: Atras (9.o) -> opciones: Atras (5.o) -> pausa: Reanudar (1.o)
    elegir(j, 7); elegir(j, 8); elegir(j, 4); elegir(j, 0, 2.0)
def esc_msaa(j):
    # SilentPatch segunda tanda (L66 MSAA 2x-16x; L155 costuras del mapa): ciclo del ajuste «Anti Aliasing» del menu con capturas, aplicacion de cada nivel, mapa del menu y mundo
    suelo_libre(j); j.al_frente()
    descubrir_cursor(j)
    mods = modulos_proceso(j)
    j.P('modulos de GPU cargados:', [x for x in mods if re.search(r'nvd3d|nvwgf|nvldumd|igd|igxe|ig9|ig75|atiu|amdxx|aticfx|atidxx|amdvlk|d3d9', x)])
    j.P('estado inicial', json.dumps(msaa_estado(j)))
    niveles = []
    menu_hasta_aa(j)
    foto_bb(j, 'aa_inicial', 'pagina avanzada, Anti Aliasing en su valor inicial; ' + json.dumps(msaa_estado(j)))
    # ciclo con la flecha derecha: texto del ajuste y valor de m_nDisplayAntialiasing en cada paso hasta volver al inicial
    ini = j.m.u32(FEM + 0xCC); vistos = [ini]
    for n in range(8):
        pulsar('right', 0.15); j.dormir(0.8); d = j.m.u32(FEM + 0xCC); vistos.append(d)
        foto_bb(j, 'aa_ciclo%d_valor%d' % (n + 1, d), 'tras flecha derecha: display=%d' % d)
        if d == ini: break
    j.P('CICLO del ajuste (flecha derecha):', vistos)
    # flecha izquierda desde el inicial: debe dar el maximo
    pulsar('left', 0.15); j.dormir(0.8); izq = j.m.u32(FEM + 0xCC); j.P('flecha izquierda desde el valor inicial %d -> %d' % (ini, izq)); foto_bb(j, 'aa_izq_valor%d' % izq, 'flecha izquierda desde el inicial')
    ciclo = [v for v in vistos[:-1]] if vistos[-1] == ini else vistos
    j.res['ciclo'] = vistos
    # aplicar cada nivel del ciclo (menos el inicial al principio, que ya esta aplicado) y capturar mundo y mapa
    orden = [v for v in ciclo if v != ini] + [ini]
    # ir al valor deseado con la flecha derecha desde el display actual
    solo = [int(x) for x in os.environ.get('MSAA_NIVELES', '').split(',') if x]   # MSAA_NIVELES=4 limita los niveles que se aplican (prueba de las costuras del mapa)
    for objetivo in (solo or [1] + [v for v in ciclo if v not in (1,)]):
        for _ in range(12):
            if j.m.u32(FEM + 0xCC) == objetivo: break
            pulsar('right', 0.15); j.dormir(0.6)
        pulsar('enter', 0.15); j.dormir(6.0)   # aplica: RwD3D9ChangeMultiSamplingLevels + SetVideoMode
        st = msaa_estado(j); j.P('NIVEL %d aplicado:' % objetivo, json.dumps(st), 'vivo' if j.alive() else 'MUERTO')
        j.res.setdefault('niveles', {})[objetivo] = st
        foto_bb(j, 'aa_n%d_menu' % objetivo, 'menu avanzado tras aplicar el nivel %d (muestras del driver %d, presente %d)' % (objetivo, st['sel'], st['present']))
        menu_salir(j)                                # avanzado -> pantalla -> opciones -> pausa -> reanudar (por los elementos «Atras»)
        j.hora = 12; j.dormir(3.0)
        foto_bb(j, 'aa_n%d_mundo' % objetivo, 'mundo con el nivel %d' % objetivo)
        tecleo(j, ['esc'], 1.5); elegir(j, 2); j.dormir(2.0)   # menu de pausa nuevo (cursor a 0) -> Mapa
        foto_bb(j, 'aa_n%d_mapa' % objetivo, 'mapa del menu con el nivel %d' % objetivo)
        pulsar('enter', 0.15); j.dormir(1.5); elegir(j, 0, 2.0)                           # mapa: Atras -> pausa -> reanudar
        menu_hasta_aa(j)
    menu_salir(j)
    for e in sorted(j.res.get('niveles', {})):
        for tipo in ('mundo', 'mapa'):
            p = os.path.join(j.sal, 'aa_n%d_%s.png' % (e, tipo))
            if os.path.exists(p):
                rw_, rh_ = j.m.u32(0xC17044), j.m.u32(0xC17048); reg = (0, 0, rw_, rh_) if tipo == 'mapa' else (0, rh_ // 8, rw_, rh_ * 3 // 4)
                m = metricas(p, reg); j.P('METRICA', tipo, 'nivel', e, json.dumps(m)); j.res['met_%s_%d' % (tipo, e)] = m

def esc_msaa_estado(j):
    # SilentPatch segunda tanda (MSAA): estado del driver y del menu justo tras arrancar (acotado del valor guardado en gta_sa.set; con ejecutar_msaa.ps1 -PrefsAA <n>)
    suelo_libre(j); j.al_frente(); j.dormir(2.0)
    st = msaa_estado(j); j.P('ESTADO TRAS ARRANCAR', json.dumps(st)); j.res['estado'] = st
    foto_bb(j, 'estado_mundo', 'mundo con el valor guardado acotado: prefs=%d, muestras del driver=%d, presente=%d' % (st['prefs'], st['sel'], st['present']))
    descubrir_cursor(j); menu_hasta_aa(j)
    foto_bb(j, 'estado_menu_aa', 'pagina avanzada: display=%d' % j.m.u32(FEM + 0xCC)); j.P('display en el menu', j.m.u32(FEM + 0xCC))
    menu_salir(j)

def esc_espera(j):
    # ejecucion larga en el mundo (sin entrada): dormir MSAA_ESPERA segundos (110 por defecto) con el estado de MSAA y las posiciones cada 20 s
    suelo_libre(j); j.al_frente()
    t0 = time.time(); seg = float(os.environ.get('MSAA_ESPERA', '110'))
    while time.time() - t0 < seg and j.alive():
        j.dormir(20.0); j.P('espera: estado', json.dumps(msaa_estado(j)), 'pos', j.pos(), 'pools', j.pools())

# ---------------------------------------------------------------- PadCheats (trucos de PS2 con el mando; docs/CodigoVivoExe/trucos-mando.md)
PS2_XI = {'l1': (0x0100, 0, 0), 'r1': (0x0200, 0, 0), 'l2': (0, 255, 0), 'r2': (0, 0, 255), 'ud': (0x0001, 0, 0), 'dd': (0x0002, 0, 0), 'ld': (0x0004, 0, 0), 'rd': (0x0008, 0, 0),
          'cr': (0x1000, 0, 0), 'ci': (0x2000, 0, 0), 'sq': (0x4000, 0, 0), 'tr': (0x8000, 0, 0)}
def pad_pulsa(j, seq, down=0.14, up=0.14):
    """pulsa la secuencia de botones PS2 con el mando XInput falso (A=cruz, B=circulo, X=cuadrado, Y=triangulo, LB/RB=L1/R1, LT/RT=L2/R2)"""
    for b in seq:
        bo, lt, rt = PS2_XI[b]; j.xi(botones=bo, lt=lt, rt=rt); j.dormir(down, 0.02); j.xi(); j.dormir(up, 0.02)
def esc_padcheats(j):
    import random
    apagado = '--padcheats-off' in sys.argv
    cj = json.load(open(arg('--cheats-json') or r'D:\Original\_ginput_ref\cheats_ps.json'))
    suelo_libre(j); j.xi(); j.dormir(1.5)
    modo = 'APAGADO (GTASA_PAD_CHEATS=0: se espera NINGUN efecto)' if apagado else 'ENCENDIDO (se esperan los efectos)'
    j.P('PADCHEATS modo', modo, 'llamadas XInput', j.xi_llamadas())
    # 1) mapeo real del mando XInput a los campos PS2 de CPad (NewState en 0xB73458), para cada boton
    mapeo = {}
    for b, (bo, lt, rt) in PS2_XI.items():
        j.xi(botones=bo, lt=lt, rt=rt); j.dormir(0.25, 0.02); k = j.pad(); j.xi(); j.dormir(0.2, 0.02)
        mapeo[b] = {kk: vv for kk, vv in k.items() if vv and kk in ('L1', 'L2', 'R1', 'R2', 'up', 'down', 'left', 'right', 'Square', 'Triangle', 'Cross', 'Circle')}
    j.P('PADCHEATS mapeo XInput -> campos de CPad::NewState:', json.dumps(mapeo)); j.res['padcheats_mapeo'] = mapeo
    res = {}
    def forzado(): return j.m.i32(0xC81318)
    def pon_clima(v):
        for a in (0xC81318, 0xC8131C, 0xC81320): j.m.wr(a, struct.pack('<i', v))
    def modelos(m): return sum(1 for v in vehiculos(j) if j.modelo(v) == m)
    seq = lambda n: next(s for d, s in cj if d == n)
    def intento(nombre, secuencia, hecho, detalle, prep=None):
        """pulsa la secuencia y comprueba el efecto; encendido: si el efecto no aparece se reintenta UNA vez tras 3.2 s (un fotograma perdido en la carga puede saltarse una pulsacion de 0.18 s); apagado: nunca"""
        n = 0
        while True:
            n += 1
            if prep: prep()
            pad_pulsa(j, secuencia, 0.18, 0.14); j.dormir(0.3, 0.02); ok = bool(hecho())
            if ok or apagado or n >= 2: break
            j.P('PADCHEATS reintento de', nombre, '(no se vio el efecto en el 1.er intento)'); j.dormir(3.3)
        ok_final = (not ok) if apagado else ok
        res[nombre] = {'ok': bool(ok_final), 'intentos': n, 'detalle': detalle()}; j.P('PADCHEATS', 'OK  ' if ok_final else 'FALLO', nombre, 'intentos=%d' % n, detalle()); return ok
    def alterna(nombre, i):
        st = {}
        def h1(): st['a'] = j.truco_activo(i); return st['a'] == 1
        intento(nombre + ' (1.a vez, ficha %d)' % i, seq(nombre), h1, lambda: 'ficha=%d' % j.truco_activo(i))
        if not apagado and st.get('a') == 1:
            n2 = 0
            while True:
                n2 += 1; pad_pulsa(j, seq(nombre), 0.18, 0.14); j.dormir(0.3, 0.02); b = j.truco_activo(i)
                if b == 0 or n2 >= 2: break
                j.P('PADCHEATS reintento de la 2.a vez de', nombre); j.dormir(3.3)
            res[nombre + ' (2.a vez desactiva)'] = {'ok': b == 0, 'intentos': n2, 'detalle': 'ficha=%d' % b}; j.P('PADCHEATS', 'OK  ' if b == 0 else 'FALLO', nombre, '2.a vez desactiva: ficha=%d' % b)
    for nombre, i in (('Infinite Health', 61), ('Never Wanted', 65), ('Max Respect', 79), ('Infinite Oxygen (Lung Capacity)', 62), ('Never Get Hungry', 68)):
        alterna(nombre, i)
    for nombre, w, otro in (('Really Sunny', 0, 9), ('Sunny', 1, 9), ('Foggy', 9, 0), ('Sandstorm', 19, 0), ('Cloudy', 4, 9)):
        intento(nombre + ' (clima forzado %d)' % w, seq(nombre), lambda w=w: forzado() == w, lambda: 'forzado=%d' % forzado(), prep=lambda o=otro: pon_clima(o))
    # reinicio por inactividad: la mitad, 3.6 s, el resto -> NO se activa (tambien apagado)
    s_ = seq('Never Get Hungry'); pad_pulsa(j, s_[:4], 0.18, 0.14); j.dormir(3.6); pad_pulsa(j, s_[4:], 0.18, 0.14); j.dormir(0.3)
    ok_p = j.truco_activo(68) == 0
    res['pausa 3.6 s a la mitad (no debe activarse)'] = {'ok': ok_p, 'intentos': 1, 'detalle': 'ficha 68=%d' % j.truco_activo(68)}; j.P('PADCHEATS', 'OK  ' if ok_p else 'FALLO', 'pausa de 3.6 s a la mitad no activa', 'ficha 68=%d' % j.truco_activo(68))
    # ruido: 250 pulsaciones aleatorias -> el estado de todos los trucos no cambia
    f_antes = j.m.rd(0x969130, 98); clima = forzado(); nv = len(vehiculos(j)); rng = random.Random(4242); bt = list(PS2_XI)
    for _ in range(250): pad_pulsa(j, [rng.choice(bt)], 0.10, 0.10)
    f_desp = j.m.rd(0x969130, 98)
    res['ruido 250 pulsaciones aleatorias'] = {'ok': f_antes == f_desp and forzado() == clima, 'intentos': 1, 'detalle': 'flags iguales=%s clima %d -> %d vehiculos %d -> %d' % (f_antes == f_desp, clima, forzado(), nv, len(vehiculos(j)))}
    j.P('PADCHEATS', 'OK  ' if res['ruido 250 pulsaciones aleatorias']['ok'] else 'FALLO', 'ruido 250 pulsaciones aleatorias', res['ruido 250 pulsaciones aleatorias']['detalle'])
    # teclado sigue igual: ASNAEB (lower wanted) y el contador de trucos de memoria
    j.escribir_truco('ASNAEB'); j.dormir(1.0); j.P('PADCHEATS teclado ASNAEB enviado; vivo=%s' % j.alive())
    # Rhino al final (el ultimo boton es Triangulo: con el tanque delante CJ puede entrar en el)
    n0 = modelos(432); intento('Rhino (vehiculo nuevo en el pool)', seq('Rhino'), lambda: modelos(432) == n0 + 1, lambda: 'rhinos %d -> %d' % (n0, modelos(432)))
    j.xi(); j.dormir(1.0)
    j.res['padcheats'] = res; j.res['padcheats_modo'] = modo
    j.P('PADCHEATS RESUMEN', modo, 'ok=%d de %d' % (sum(1 for v in res.values() if v['ok']), len(res)))


def esc_iconos(j):
    """Iconos del mando en las ayudas (PadButtonIcons): requiere GTASA_PAD_ICONS_DEMO=1 en el entorno del juego (cada 8 s una ayuda corta con etiquetas ~k~~ACCION~, anotada en el log).
    Fases: teclado (sin mando) -> mando (xinput falso, A pulsado) -> teclado otra vez. En cada fase captura 3 ayudas distintas y anota el texto ya resuelto de CHud::m_pHelpMessage (0xBAA7A0)."""
    suelo_libre(j)
    logp = arg('--log'); pend = []
    def ayuda():   # el juego anota en el log el texto ya resuelto de cada ayuda de demostracion ("PadIcons demo: ayuda = ...")
        for l in j.log_nuevas(logp):
            if 'PadIcons demo: ayuda = ' in l: pend.append(l.split('PadIcons demo: ayuda = ', 1)[1].encode('latin1', 'replace'))
        return pend[-1] if pend else b''
    def fase(nombre, actividad, n=3, maximo=40):
        vistas = []; t0 = time.time(); ult = ayuda()   # lo anterior a la fase no cuenta
        j.P('FASE', nombre)
        while time.time() - t0 < maximo and len(vistas) < n and j.alive():
            actividad(); j.tick(); h = ayuda()
            if h and h != ult and h not in vistas:
                ult = h; t1 = time.time()
                while time.time() - t1 < 1.8 and j.alive(): actividad(); j.tick(); time.sleep(0.1)   # deja que termine el fundido de entrada
                if ayuda() == h:
                    vistas.append(h); j.P('AYUDA', nombre, len(vistas), repr(h.decode('latin1')))
                    j.foto('ic_%s_%d' % (nombre, len(vistas)), 'ayuda: ' + h.decode('latin1'))
            time.sleep(0.1)
        j.res['iconos_' + nombre] = [x.decode('latin1') for x in vistas]
    def teclado():
        j.al_frente()
        i = INP(); i.type = 1; i.ki = KI(0, SC['lshift'], 0x8, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP)); time.sleep(0.03)
        i.ki = KI(0, SC['lshift'], 0x8 | 0x2, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP))
    def mando(): j.xi(botones=0x1000 if int(time.time() * 4) % 2 == 0 else 0)
    j.xi(conectado=0)
    fase("teclado", teclado, n=2)
    j.xi(conectado=1); fase('mando', mando, n=5, maximo=80)
    j.xi(conectado=0); fase('teclado2', teclado, n=1)
    j.xi(conectado=1); fase('mando2', mando, n=1)
    j.xi()


ESCENARIOS = {'padcheats': esc_padcheats, 'msaa': esc_msaa, 'msaa_estado': esc_msaa_estado, 'espera': esc_espera, 'sp2veh': esc_sp2veh, 'usertrack': esc_usertrack, 'altf4': esc_altf4, 'gimnasio': esc_gimnasio, 'camara_aire': esc_camara_aire, 'spawn': esc_spawn, 'base': esc_base, 'xinput': esc_xinput, 'xinput_coche': esc_xinput_coche, 'conduccion': esc_conduccion, 'camara': esc_camara, 'armas': esc_armas, 'interiores': esc_interiores, 'trucos': esc_trucos, 'audio': esc_audio, 'entrada': esc_entrada, 'luna': esc_luna, 'fps': esc_fps, 'graficos': esc_graficos, 'pausa': esc_pausa, 'caida_interior': esc_caida_interior, 'iconos': esc_iconos}

def main():
    sal = sys.argv[1]; esc = sys.argv[2].split(','); dirj = arg('--dir')
    j = J(sal, dirj)
    if not j.inicio(salir='--sin-salir' not in sys.argv): return
    for e in esc:
        j.P('=== ESCENARIO', e)
        try: ESCENARIOS[e](j)
        except Exception as ex:
            import traceback; j.P('EXCEPCION en el escenario', e, repr(ex), traceback.format_exc()[-600:])
        if not j.alive(): j.P('EL PROCESO TERMINO durante', e); break
    j.P('vivo al final:', j.alive())
    for l in j.log_nuevas(arg('--log')):
        if re.search(r'error|NaN|crash|excep|fall', l, re.I): j.P('LOG', l[:240])
    json.dump(j.res, open(os.path.join(sal, 'resumen.json'), 'w'), indent=1, default=str)
    j.P('fin')
if __name__ == '__main__': main()
