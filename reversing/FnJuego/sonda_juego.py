# Sonda de PRUEBAS DE JUEGO del standalone (mando XInput falso, conduccion, armas, interiores, trucos, camara cinematografica, audio).
# Se lanza desde ejecutar_juego.ps1 (candado gta_lock, carpeta D:\Original\_juego_run, copia de la partida 2). Solo toca el proceso con --pid.
# Uso: python -I sonda_juego.py <salida> <escenario> --pid N --dir <carpeta_ejecucion> --log <log.log>
# Escenarios: base, xinput, conduccion, armas, interiores, trucos, camara, audio  (se pueden encadenar con comas: conduccion,camara)
# Entrada: SendInput real (teclado por codigos de barrido, raton) y, para el mando, el xinput1_4.dll FALSO de esta carpeta (xinput_falso.bin).
import ctypes, struct, sys, time, os, json, math, re
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
ESCENARIOS = {'usertrack': esc_usertrack, 'gimnasio': esc_gimnasio, 'camara_aire': esc_camara_aire, 'spawn': esc_spawn, 'base': esc_base, 'xinput': esc_xinput, 'xinput_coche': esc_xinput_coche, 'conduccion': esc_conduccion, 'camara': esc_camara, 'armas': esc_armas, 'interiores': esc_interiores, 'trucos': esc_trucos, 'audio': esc_audio}

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
