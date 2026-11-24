# Sonda externa del robo de coches de TRAFICO real (standalone o ASI). Se lanza junto al juego con --pid.
# Mide por memoria (direcciones reales de pools/jugador, validas en el standalone y en el EXE original) y toma capturas COMPLETAS de la ventana.
# Uso: python -I sonda_robo.py <salida> --pid N [--exe nombre] [--hora 12] [--posiciones "x,y,z;x,y,z"] [--seg-pos 125] [--log ruta_log.log] [--tecla w]
# Por posicion: teletransporte, espera de trafico, capturas del trafico, y luego vigila el ciclo de robo de --robar (el propio juego, AutoSteal):
#  - CJ pasa a conducir (CPlayerPed.m_pVehicle != 0 y estado PEDSTATE_DRIVING) -> mide 2 s SIN acelerador y despues pulsa W (teclado real, SendInput) y mide el avance
#  - capturas: coche(s) de trafico, CJ entrando (por la linea [ROBAR] de la tarea), CJ conduciendo.
import ctypes, struct, sys, time, os, json, math
from ctypes import wintypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import sonda_traf as S
import captura
u = S.u
def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
class KI(ctypes.Structure):
    _fields_ = [('wVk', wintypes.WORD), ('wScan', wintypes.WORD), ('dwFlags', wintypes.DWORD), ('time', wintypes.DWORD), ('extra', ctypes.c_void_p)]
class INP(ctypes.Structure):
    class _U(ctypes.Union): _fields_ = [('ki', KI), ('pad', ctypes.c_byte * 32)]
    _anonymous_ = ('u',); _fields_ = [('type', wintypes.DWORD), ('u', _U)]
SC = {'w': (0x11, False), 'up': (0x48, True), 's': (0x1F, False)}
def tecla(nombre, abajo):
    sc, ext = SC[nombre]; i = INP(); i.type = 1
    i.ki = KI(0, sc, 0x8 | (0 if abajo else 0x2) | (0x1 if ext else 0), 0, None); u.SendInput(1, ctypes.byref(i), ctypes.sizeof(INP))
def fin(v): return v is not None and all(math.isfinite(x) for x in v)
def main():
    sal = sys.argv[1]; os.makedirs(sal, exist_ok=True)
    exe = arg('--exe', 'gta_sa_standalone'); hora = int(arg('--hora', '12')); seg_pos = float(arg('--seg-pos', '125'))
    posiciones = [[float(v) for v in q.split(',')] for q in arg('--posiciones', '2652.5,-1719.9,13.8;2672.1,-1652.5,11.5;2658.1,-1433.7,31.5').split(';')]
    log = arg('--log') or os.environ.get('ROBO_LOG'); tk = arg('--tecla', 'w'); ncap_traf = int(arg('--capt-traf', '3'))
    f = open(os.path.join(sal, 'sonda_robo.log'), 'w', encoding='utf-8')
    def P(*a):
        s = ' '.join(str(x) for x in a); print(s, flush=True); f.write('[%7.1f] ' % (time.time() - T0) + s + '\n'); f.flush()
    T0 = time.time()
    p = None
    for _ in range(300):
        p = S.pid(exe)
        if p: break
        time.sleep(1)
    if not p: P('no hay proceso', exe); return
    m = S.M(p); P('pid', p, 'exe', exe)
    t00 = time.time()
    while time.time() - t00 < 400 and S.pid(exe):
        m.wr(0x8D621C, struct.pack('<I', 1))
        ped, pos = S.jugador(m)
        if m.u32(0xC8D4C0) == 9 and pos and pos[2] == pos[2]: break
        time.sleep(1)
    P('juego en marcha tras %.0f s; gGameState=%d' % (time.time() - t00, m.u32(0xC8D4C0)))
    ts = time.time()
    while time.time() - ts < 90 and S.pid(exe):   # salir de la casa (partida 2 empieza en el interior CARLS)
        m.wr(0x8D621C, struct.pack('<I', 1))
        ped, _ = S.jugador(m); mat = m.u32(ped + 0x14) if ped else 0
        if m.i32(0xB72914) == 0 and mat: break
        if mat: m.wr(mat + 0x30, struct.pack('<3f', 2496.05, -1692.6, 1014.2))
        time.sleep(0.4)
    P('salida de la casa: area=%d tras %.0f s' % (m.i32(0xB72914), time.time() - ts))
    h = captura.buscar()
    if h: u.ShowWindow(h, 9); u.SetForegroundWindow(h); time.sleep(1.0)
    rc = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(rc)); wr = wintypes.RECT(); u.GetWindowRect(h, ctypes.byref(wr))
    P('ventana: cliente %dx%d, rect ventana %dx%d' % (rc.right - rc.left, rc.bottom - rc.top, wr.right - wr.left, wr.bottom - wr.top))
    # lectura del log del juego (cola) para detectar los hitos de AutoSteal
    lpos = [0]; nlin = [0]
    def nuevas():
        if not log or not os.path.exists(log): return []
        try:
            with open(log, 'rb') as g:
                g.seek(lpos[0]); d = g.read(); lpos[0] += len(d)
            return d.decode('utf-8', 'replace').splitlines()
        except Exception: return []
    def foto(nombre):
        r = captura.capturar(os.path.join(sal, nombre + '.png'), traer=False); P('captura', nombre, json.dumps(r)); return r
    def vehpos(v):
        mt = m.u32(v + 0x14)
        return (m.f32(mt + 0x30), m.f32(mt + 0x34), m.f32(mt + 0x38)) if mt else None
    def dist(a, b): return math.sqrt(sum((x - y) ** 2 for x, y in zip(a, b))) if a and b else float('nan')
    def trafico_cerca():
        pv = m.u32(0xB74494); objs = m.u32(pv); fl = m.rd(m.u32(pv + 4), m.i32(pv + 8)) or b''; cerca = 0; tot = 0; px = S.jugador(m)[1]
        for i in range(len(fl)):
            if fl[i] & 0x80: continue
            ve = objs + i * 0xA18; mt = m.u32(ve + 0x14)
            if not mt: continue
            tot += 1; x, y = m.f32(mt + 0x30), m.f32(mt + 0x34)
            if px and math.hypot(x - px[0], y - px[1]) < 80: cerca += 1
        return tot, cerca
    res = {'episodios': [], 'nan': 0, 'caidas': 0, 'muestras': 0, 'posiciones': []}
    cic = [False]; ult_tp = [0.0]
    drv = None; w_abajo = False; ult_est = 0; fotos_ent = []; ep_n = 0
    inicio_pos = time.time()
    for k, q in enumerate(posiciones):
        if not S.pid(exe): break
        if h: u.ShowWindow(h, 9); u.SetForegroundWindow(h)
        t1 = time.time()
        while time.time() - t1 < 14 and S.pid(exe):
            ped, _ = S.jugador(m); mat = m.u32(ped + 0x14) if ped else 0
            m.wr(0x8D621C, struct.pack('<I', 1)); m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0]))
            if mat and time.time() - t1 < 4: [m.wr(mat + 0x30 + 4 * i, struct.pack('<f', v)) for i, v in enumerate(q)]
            nuevas(); time.sleep(0.5)
        S.nivelar(m)
        tv, tc = trafico_cerca(); e = S.estado(m)
        P('POSICION %d %s pool_veh=%s vehiculos_total=%d a_menos_de_80m=%d NumRandomCars=%d' % (k, q, e['veh'], tv, tc, e['NumRandomCars']))
        res['posiciones'].append({'k': k, 'pos': q, 'veh_total': tv, 'veh_80m': tc, 'NumRandomCars': e['NumRandomCars']})
        for i in range(ncap_traf):
            foto('pos%d_trafico%d' % (k, i))
            for _ in range(8): S.mouse(40, 0); time.sleep(0.04)
            time.sleep(0.8)
        # bucle de vigilancia del ciclo de robo durante seg_pos
        t_k = time.time()
        while time.time() - t_k < seg_pos or (cic[0] and time.time() - t_k < seg_pos + 60):
            if not S.pid(exe): P('EL PROCESO TERMINO'); res['caido'] = True; break
            m.wr(0x8D621C, struct.pack('<I', 1)); m.wr(0xB70153, bytes([hora]))
            now = time.time()
            for l in nuevas():
                if '[ROBAR]' in l: P('LOG', l.split(']', 3)[-1].strip()[:200] if False else l[l.find('[ROBAR]'):][:220])
                if 'coche de TRAFICO modelo' in l: cic[0] = True
                if '[ROBAR] ciclo' in l and '/' in l.split('[ROBAR] ciclo')[1][:8] and 'robado=' in l: cic[0] = False
                if 'se asigna CTaskComplexEnterCarAsDriver' in l: fotos_ent = [now + 0.7, now + 1.8, now + 3.0]
            if fotos_ent and now >= fotos_ent[0]:
                fotos_ent.pop(0); foto('entrando_pos%d_%d_%d' % (k, ep_n + 1, 3 - len(fotos_ent)))
            ped, pos = S.jugador(m)
            veh = m.u32(ped + 0x58C) if ped else 0; st = m.u32(ped + 0x530) if ped else 0
            res['muestras'] += 1
            if pos is not None and not fin(pos): res['nan'] += 1; P('NaN en jugador', pos)
            elif pos and pos[2] < -50: res['caidas'] += 1; P('CAIDA jugador z=%.1f' % pos[2])
            conduce = bool(veh) and st == 50
            if pos and fin(pos) and not conduce and not veh and not cic[0] and now - ult_tp[0] > 20 and math.hypot(pos[0] - q[0], pos[1] - q[1]) > 350:
                mt = m.u32(ped + 0x14)
                if mt: [m.wr(mt + 0x30 + 4 * i, struct.pack('<f', v)) for i, v in enumerate(q)]; ult_tp[0] = now; P('CJ devuelto a la posicion %d (estaba a %.0f m)' % (k, math.hypot(pos[0] - q[0], pos[1] - q[1])))
            if conduce and drv is None:
                ep_n += 1
                vp = vehpos(veh)
                drv = {'n': ep_n, 'pos': k, 't0': now, 'v0': vp, 'veh': hex(veh), 'estado_ped': st, 'p_veh_jugador': veh, 'sin_acel': None, 'w_t': None, 'v_w': None, 'max_sin': 0.0, 'max_con': 0.0, 'vmax': 0.0, 'fotos': [now + 1.0, now + 4.0, now + 7.0], 'modelo': m.u32(veh + 0x22) & 0xFFFF if False else (struct.unpack('<H', m.rd(veh + 0x22, 2))[0])}
                P('CJ CONDUCE ep%d: m_pVehicle=%s estado_ped=%d modelo_coche=%d pos_coche=%s' % (ep_n, hex(veh), st, drv['modelo'], vp))
            if drv is not None:
                vp = vehpos(veh) if veh else None
                if vp and not fin(vp): res['nan'] += 1; P('NaN en coche', vp)
                if vp and drv['v0']:
                    dd = dist(vp, drv['v0'])
                    sp = math.sqrt(sum(m.f32(veh + 0x44 + 4 * j) ** 2 for j in range(3)))
                    drv['vmax'] = max(drv['vmax'], sp)
                    if drv['w_t'] is None:
                        drv['max_sin'] = max(drv['max_sin'], dd)
                        if now - drv['t0'] >= 2.0:
                            drv['sin_acel'] = drv['max_sin']; drv['w_t'] = now; drv['v_w'] = vp
                            if h: u.SetForegroundWindow(h)
                            tecla(tk, True); w_abajo = True
                            P('ep%d: sin acelerador en 2 s el coche se movio %.2f m; se pulsa %s' % (drv['n'], drv['sin_acel'], tk))
                    else: drv['max_con'] = max(drv['max_con'], dist(vp, drv['v_w']))
                if drv['fotos'] and now >= drv['fotos'][0]:
                    drv['fotos'].pop(0); foto('conduciendo_pos%d_%d_%d' % (k, drv['n'], 3 - len(drv['fotos'])))
                if not conduce:
                    if w_abajo: tecla(tk, False); w_abajo = False
                    P('ep%d FIN: avance_sin_acelerador_2s=%s m, avance_con_%s=%.2f m, vel_max=%.3f, NaN/caidas acum=%d/%d' % (drv['n'], None if drv['sin_acel'] is None else round(drv['sin_acel'], 2), tk, drv['max_con'], drv['vmax'], res['nan'], res['caidas']))
                    res['episodios'].append({kk: vv for kk, vv in drv.items() if kk not in ('fotos',)}); drv = None
            if now - ult_est >= 10:
                ult_est = now; e = S.estado(m); P('estado', json.dumps({'veh': e['veh'], 'peds': e['peds'], 'pos': e['pos'], 'NumRandomCars': e['NumRandomCars'], 'area': e['area']}))
            time.sleep(0.15)
        else:
            continue
        break
    if w_abajo: tecla(tk, False)
    if drv is not None: res['episodios'].append({kk: vv for kk, vv in drv.items() if kk not in ('fotos',)})
    for l in nuevas():
        if '[ROBAR]' in l: P('LOG', l[l.find('[ROBAR]'):][:220])
    res['duracion_s'] = round(time.time() - inicio_pos, 1); res['vivo_al_final'] = bool(S.pid(exe))
    json.dump(res, open(os.path.join(sal, 'resumen_sonda.json'), 'w'), indent=1, default=str)
    P('RESUMEN', json.dumps({k2: v for k2, v in res.items() if k2 != 'episodios'}, default=str), 'episodios', len(res['episodios']))
    P('fin')
if __name__ == '__main__': main()
