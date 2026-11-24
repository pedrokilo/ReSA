# Sonda externa para el trafico: se lanza junto a gta_sa_standalone.exe (o gta_sa.exe del ASI) y, por memoria, mide cada 5 s el estado del trafico/poblacion
# y toma capturas de la ventana (cliente, DPI-aware, validando el tamano contra el rectangulo de la ventana) con seis giros de camara.
# Uso: python -I sonda_traf.py <carpeta_salida> [--exe gta_sa_standalone|gta_sa] [--espera S] [--seg S] [--sin-capturas] [--tp x,y,z]
# En el standalone solo las direcciones que son StaticRef del codigo coinciden con la memoria; los objetos DatosExe::Bss son propios (se miden en el log del proceso: lineas TRAF).
# En el ASI/EXE original las direcciones del EXE son las reales.
import ctypes, struct, subprocess, sys, time, os, json
from ctypes import wintypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import captura
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE
u = captura.u
class INPUT_MOUSE(ctypes.Structure):
    _fields_ = [('dx', wintypes.LONG), ('dy', wintypes.LONG), ('mouseData', wintypes.DWORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class INPUT(ctypes.Structure):
    class _U(ctypes.Union): _fields_ = [('mi', INPUT_MOUSE)]
    _anonymous_ = ('u',); _fields_ = [('type', wintypes.DWORD), ('u', _U)]
def mouse(dx=0, dy=0):
    i = INPUT(); i.type = 0; i.mi = INPUT_MOUSE(dx, dy, 0, 0x0001, 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INPUT))
FORZAR_PID = int(sys.argv[sys.argv.index('--pid') + 1]) if '--pid' in sys.argv else None   # solo se toca el proceso propio (lanzado por control_traf/ejecutar_traf)
def pid(nombre):
    if FORZAR_PID:
        o = subprocess.run(['tasklist', '/FI', 'PID eq %d' % FORZAR_PID, '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout
        return FORZAR_PID if ('"%d"' % FORZAR_PID) in o else None
    o = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq %s.exe' % nombre, '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout
    for l in o.splitlines():
        if nombre in l: return int(l.split('","')[1])
class M:
    def __init__(s, p): s.h = k.OpenProcess(0x38 | 0x400, False, p)
    def rd(s, a, n):
        b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) else None
    def wr(s, a, d): r = ctypes.c_size_t(); return k.WriteProcessMemory(s.h, ctypes.c_void_p(a), d, len(d), ctypes.byref(r))
    def u32(s, a): b = s.rd(a, 4); return struct.unpack('<I', b)[0] if b else 0
    def i32(s, a): b = s.rd(a, 4); return struct.unpack('<i', b)[0] if b else 0
    def u8(s, a): b = s.rd(a, 1); return b[0] if b else 0
    def f32(s, a): b = s.rd(a, 4); return struct.unpack('<f', b)[0] if b else float('nan')
def pool(m, ptr):
    p = m.u32(ptr); flags = m.u32(p + 4); n = m.i32(p + 8)
    fl = m.rd(flags, n) or b''
    return sum(1 for i in range(n) if not fl[i] & 0x80), n
def jugador(m):
    foco = m.u8(0xB7CD74); ped = m.u32(0xB7CD98 + foco * 0x190)
    mat = m.u32(ped + 0x14) if ped else 0
    return ped, ((m.f32(mat + 0x30), m.f32(mat + 0x34), m.f32(mat + 0x38)) if mat else None)
def nivelar(m, objetivo=0.02, tol=0.08, maxpasos=120):
    """lleva el vector frente de TheCamera (0xB6F9AC) a z ~ objetivo moviendo el raton en vertical (ajusta el signo solo)"""
    s = -1
    def fz(): return m.f32(0xB6F9AC + 8)
    for _ in range(maxpasos):
        z0 = fz()
        if abs(z0 - objetivo) < tol: return z0
        mouse(0, s * 12 * (1 if z0 < objetivo else -1)); time.sleep(0.08)
        z1 = fz()
        if (z1 - z0) * (objetivo - z0) < 0 and abs(z1 - z0) > 0.005: s = -s
    return fz()
def estado(m):
    nv, tv = pool(m, 0xB74494); npd, tp = pool(m, 0xB74490)
    ped, pos = jugador(m)
    d = {'t_juego_ms': m.u32(0xB7CB84), 'cs': m.u8(0xB5F851) if False else None, 'area': m.i32(0xB72914), 'peds': (npd, tp), 'veh': (nv, tv), 'pos': pos,
         'NumRandomCars': m.i32(0x969094), 'MaxCarsInUse': m.i32(0x8A5B24)}
    return d
def estado_exe(m):
    # solo valido con el EXE original en memoria (ASI/control): contadores por direccion real
    return {'NumLaw': m.u32(0x969098), 'NumMission': m.i32(0x96909C), 'NumParked': m.u32(0x9690A0), 'CountDown': m.u8(0x9690C0), 'TimeNextMad': m.f32(0x9690BC),
            'NumCarGens': m.u32(0xC279D4), 'ProcCnt': m.u8(0xC279D1), 'popcyc_cars': [m.f32(0xC0BC30 + 4 * i) for i in range(4)], 'zona': hex(m.u32(0xC0BC64)),
            'TotalPeds': m.u32(0xC0EC28), 'TotalCiv': m.u32(0xC0EC30), 'CountDownPeds': m.u8(0xC0EC23), 'nubes_fader': m.f32(0xC6E970), 'nubes_alturaSwitch': m.u8(0x8D538C), 'nubes_usedNum': m.i32(0xC6AA5C), 'nubes_densidad': m.f32(0x8D5388)}
def main():
    sal = sys.argv[1]
    exe = sys.argv[sys.argv.index('--exe') + 1] if '--exe' in sys.argv else 'gta_sa_standalone'
    espera = float(sys.argv[sys.argv.index('--espera') + 1]) if '--espera' in sys.argv else 40
    seg = float(sys.argv[sys.argv.index('--seg') + 1]) if '--seg' in sys.argv else 60
    capt = '--sin-capturas' not in sys.argv
    hora = int(sys.argv[sys.argv.index('--hora') + 1]) if '--hora' in sys.argv else None
    posiciones = [[float(v) for v in q.split(',')] for q in sys.argv[sys.argv.index('--posiciones') + 1].split(';')] if '--posiciones' in sys.argv else None
    tp = [float(x) for x in sys.argv[sys.argv.index('--tp') + 1].split(',')] if '--tp' in sys.argv else None
    os.makedirs(sal, exist_ok=True)
    f = open(os.path.join(sal, 'sonda_traf.log'), 'w', encoding='utf-8')
    def P(*a):
        s = ' '.join(str(x) for x in a); print(s, flush=True); f.write(s + '\n'); f.flush()
    p = None
    for _ in range(600):
        p = pid(exe)
        if p: break
        time.sleep(1)
    if not p: P('no hay proceso', exe); return
    m = M(p); P('pid', p, 'exe', exe)
    t00 = time.time()
    while time.time() - t00 < 400:   # espera a gGameState == 9 con jugador (el tiempo de --espera cuenta desde ahi)
        m.wr(0x8D621C, struct.pack('<I', 1))   # ForegroundApp=1: el bucle original se pausa en segundo plano
        ped, pos = jugador(m)
        if m.u32(0xC8D4C0) == 9 and pos and pos[2] == pos[2]: break
        time.sleep(1)
    P('juego en marcha tras %.0f s; gGameState=%d' % (time.time() - t00, m.u32(0xC8D4C0)))
    if '--salir' in sys.argv:
        # la partida 2 empieza DENTRO de la casa de CJ (area 3): se pisa el marcador de salida del interior CARLS (enex 2496.05,-1692.73,1013.75, radio 2) hasta que area == 0
        ts = time.time()
        while time.time() - ts < 90 and pid(exe):
            m.wr(0x8D621C, struct.pack('<I', 1))
            ped, _ = jugador(m); mat = m.u32(ped + 0x14) if ped else 0
            if m.i32(0xB72914) == 0 and mat: break
            if mat: m.wr(mat + 0x30, struct.pack('<3f', 2496.05, -1692.6, 1014.2))
            time.sleep(0.4)
        P('salida de la casa: area=%d tras %.0f s' % (m.i32(0xB72914), time.time() - ts))
    t0 = time.time()
    hecho = False; ult = 0
    while time.time() - t0 < espera + seg + 30:
        time.sleep(1)
        if not pid(exe): P('el proceso termino'); break
        m.wr(0x8D621C, struct.pack('<I', 1))   # ForegroundApp=1: no pausar en segundo plano
        if hora is not None: m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0]))   # CClock horas/minutos
        t = time.time() - t0
        if t - ult >= 5:
            ult = t
            e = estado(m)
            if exe == 'gta_sa': e.update(estado_exe(m))
            P('t=%.0f' % t, json.dumps(e))
        if capt and not hecho and t >= espera:
            hecho = True
            h = captura.buscar()
            if h:
                u.ShowWindow(h, 9); u.SetForegroundWindow(h); time.sleep(1.0)
                if tp:
                    ped, _ = jugador(m); mat = m.u32(m.u32(ped + 0x14) if False else ped + 0x14)
                    for i, v in enumerate(tp): m.wr(mat + 0x30 + 4 * i, struct.pack('<f', v))
                    time.sleep(3)
                rc = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(rc)); wr = wintypes.RECT(); u.GetWindowRect(h, ctypes.byref(wr))
                P('ventana: cliente %dx%d, rect ventana %dx%d' % (rc.right - rc.left, rc.bottom - rc.top, wr.right - wr.left, wr.bottom - wr.top))
                def frente(): return [round(m.f32(0xB6F9AC + 4 * j), 2) for j in range(3)]   # TheCamera: vector frente
                for i in range(6):
                    r = captura.capturar(os.path.join(sal, 'giro%d.png' % i), traer=False)
                    P('captura giro%d' % i, json.dumps(r), 'frente', frente(), 'estado', json.dumps(estado(m)))
                    for _ in range(15): mouse(0, -20); time.sleep(0.04)   # mirar arriba (cielo)
                    time.sleep(1.2)
                    r = captura.capturar(os.path.join(sal, 'cielo%d.png' % i), traer=False)
                    P('captura cielo%d' % i, json.dumps(r), 'frente', frente())
                    for _ in range(15): mouse(0, 20); time.sleep(0.04)
                    for _ in range(14): mouse(40, 0); time.sleep(0.04)   # giro de camara por el raton (a calibrar con el vector frente)
                    time.sleep(1.2)
    if posiciones:
        h = captura.buscar()
        for k, q in enumerate(posiciones):
            if not pid(exe): break
            u.ShowWindow(h, 9); u.SetForegroundWindow(h)
            t1 = time.time()
            while time.time() - t1 < 14 and pid(exe):   # teletransporte repetido y espera a que el mundo y el trafico se generen
                ped, _ = jugador(m); mat = m.u32(ped + 0x14) if ped else 0
                m.wr(0x8D621C, struct.pack('<I', 1))
                if hora is not None: m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0]))
                if mat and time.time() - t1 < 4: [m.wr(mat + 0x30 + 4 * i, struct.pack('<f', v)) for i, v in enumerate(q)]
                time.sleep(0.5)
            nivelar(m)
            e = estado(m)
            if exe == 'gta_sa': e.update(estado_exe(m))
            # vehiculos ambientales a menos de 120 m del jugador (pool de vehiculos 0xB74494, 0x988 bytes cada uno (sizeof CAutomobile))
            pv = m.u32(0xB74494); objs = m.u32(pv); fl = m.rd(m.u32(pv + 4), m.i32(pv + 8)) or b''; cerca = 0; total = 0; px = jugador(m)[1]
            for i in range(len(fl)):
                if fl[i] & 0x80: continue
                ve = objs + i * 0x988; mt = m.u32(ve + 0x14)
                if not mt: continue
                x, y = m.f32(mt + 0x30), m.f32(mt + 0x34); total += 1
                if px and ((x - px[0]) ** 2 + (y - px[1]) ** 2) ** 0.5 < 120: cerca += 1
            P('POSICION %d %s estado %s vehiculos_total=%d vehiculos_a_menos_de_120m=%d' % (k, q, json.dumps(e), total, cerca))
            for i in range(6):
                r = captura.capturar(os.path.join(sal, 'pos%d_giro%d.png' % (k, i)), traer=False)
                P('captura pos%d_giro%d' % (k, i), json.dumps(r), 'frente', [round(m.f32(0xB6F9AC + 4 * j), 2) for j in range(3)])
                for _ in range(5): mouse(40, 0); time.sleep(0.04)   # ~60 grados por paso (14 pasos eran ~190)
                time.sleep(1.0)
    P('fin')
if __name__ == "__main__": main()
