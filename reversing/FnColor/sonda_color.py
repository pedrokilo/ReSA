# Sonda de la regresion "vehiculos negros": escenario 'color' sobre la infraestructura de FnJuego (sonda_juego.J).
# Uso (lo lanza ejecutar_color.ps1): python -I sonda_color.py <salida> color --pid N --dir <carpeta> --map <gta_sa_standalone.map> --log <log.log>
# Lee por memoria del proceso del standalone (simbolos del .map, VA = base 0x400000 FIXED): ms_lightsOn, ms_currentCol, bInvertLook4Pad,
# CCheat::m_aCheatsActive (0x969130), la tabla de colores, y por cada vehiculo vivo: modelo, colores primario..cuaternario (+0x434).
import sys, os, re, struct, json, time, ctypes
ctypes.windll.user32.SetProcessDPIAware()
aqui = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, 'D:/Original/wv-int/reversing/FnJuego')
import sonda_juego as SJ
from sonda_juego import J, suelo_libre, spawn_coche, vehiculos, arg
def simbolos(mapa):
    r = {}
    pat = re.compile(r'^\s*\d+:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8})\s')
    for l in open(mapa, encoding='latin1', errors='replace'):
        m = pat.match(l)
        if m and re.match(r'\?(vB4E3E8|vB4E3F0|vB73402|vBAA38A|vC1CC01|vB61100)@R5@DatosExe', m.group(1)):
            r[re.match(r'\?(\w+)@', m.group(1)).group(1)] = int(m.group(2), 16)
    return r
def leer_estado(j, sim, tag):
    m = j.m
    ch = m.rd(0x969130, 98) or b''
    act = [i for i, b in enumerate(ch) if b]
    d = {'tag': tag, 'cheats_activos': act, 'cheat_black(33)': ch[33] if len(ch) > 33 else None, 'cheat_pink(32)': ch[32] if len(ch) > 32 else None}
    for n in ('vB4E3E8', 'vB4E3F0', 'vB73402', 'vBAA38A', 'vC1CC01'):
        if n in sim: d[n] = hex(sim[n])
    if 'vB4E3E8' in sim:
        a = sim['vB4E3E8']; d['ms_lightsOn[0..3]'] = list(m.rd(a, 4))
    if 'vB4E3F0' in sim:
        a = sim['vB4E3F0']; d['ms_currentCol[0..3]'] = list(m.rd(a, 4))
    if 'vB73402' in sim: d['bInvertLook4Pad'] = m.u8(sim['vB73402'])
    if 'vB4E3E8' in sim and 'vB4E3F0' in sim: d['bloque_vecino(8 bytes desde lightsOn)'] = list(m.rd(sim['vB4E3E8'], 8))
    d['tabla_colores_0..5'] = [list(m.rd(0xB4E480 + 4 * i, 4)) for i in range(6)]
    vs = []
    for v in vehiculos(j):
        try:
            if j.nan(j.vpos(v)): continue
            vs.append({'v': hex(v), 'modelo': j.modelo(v), 'colores': list(m.rd(v + 0x434, 4)), 'luces_dir': None})
        except Exception: pass
    d['vehiculos'] = vs
    j.P('ESTADO', json.dumps(d))
    return d
def esc_color(j):
    sim = ({'vB4E3E8': 0xB4E3E8, 'vB4E3F0': 0xB4E3F0, 'vB73402': 0xB73402, 'vBAA38A': 0xBAA38A, 'vC1CC01': 0xC1CC01} if '--original' in sys.argv else simbolos(arg('--map'))); j.P('simbolos', {k: hex(v) for k, v in sim.items()})
    horas = [int(h) for h in arg('--horas', '12,22').split(',')]
    suelo_libre(j)
    res = []
    for h in horas:
        j.hora = h; j.dormir(4)
        res.append(leer_estado(j, sim, 'h%d_sin_spawn' % h))
        j.foto('h%02d_calle' % h, 'calle con trafico, hora %d' % h)
    for codigo in ('RZHSUEW', 'UBHYZHQ', 'PRIEBJ', 'JHJOECW'):
        if not j.alive(): break
        v = spawn_coche(j, codigo)
        for h in horas:
            j.hora = h; j.dormir(3)
            res.append(leer_estado(j, sim, '%s_h%d' % (codigo, h)))
            j.foto('spawn_%s_h%02d' % (codigo, h), 'tras %s hora %d' % (codigo, h))
    j.res['color'] = res

def arranque_orig():
    """Original puro: sin FastLoader. Enter en los logos (gGameState 2 o 4) y menu por teclado hasta cargar la ranura 2 (como FnComparar/comparar.py)."""
    import hashlib, subprocess
    sys.path.insert(0, 'D:/Original/wv-int/reversing/FnComparar'); import teclas
    from ctypes import wintypes
    uu = SJ.u; uu.SetWindowPos.argtypes = [wintypes.HWND, wintypes.HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, wintypes.UINT]
    def foco():
        h = SJ.captura.buscar()
        if h: uu.ShowWindow(h, 9); uu.SetWindowPos(wintypes.HWND(h), wintypes.HWND(-1), 0, 0, 0, 0, 0x0001 | 0x0002 | 0x0040); uu.SetForegroundWindow(h)
        return h
    hist = []; gs_ant = -1
    import sonda_traf as S
    pid = None
    for _ in range(120):
        pid = S.pid('gta_sa')
        if pid: break
        time.sleep(1)
    m = S.M(pid); seq = 'enter,down,enter,down,enter,enter'.split(','); i = 0; ah0 = None; ult = 0; t7 = None; t0 = time.time(); tm = 0.0
    while S.pid('gta_sa') and time.time() - t0 < 400:
        gs = m.u32(0xC8D4C0); m.wr(0x8D621C, struct.pack('<I', 1))
        if gs != gs_ant: hist.append((round(time.time() - t0, 1), gs)); gs_ant = gs; print('gs', hist[-1], flush=True)
        if gs == 9: return True
        if gs in (2, 4):
            foco(); teclas.tecla('enter')
        elif gs == 7:
            t7 = t7 or time.time()
            if time.time() - t7 > 3 and time.time() - ult > 1.8:
                ult = time.time(); foco(); time.sleep(0.2); ah = hashlib.md5(m.rd(0xBA6748, 0x400) or b'').hexdigest()
                if i > 0 and ah == ah0 and i <= len(seq): teclas.tecla(seq[i - 1], 0.15)
                elif i < len(seq): k = seq[i]; i += 1; ah0 = ah; tm = time.time(); teclas.tecla(k, 0.15)
                elif time.time() - tm > 12:
                    for _ in range(3): teclas.tecla('esc')
                    i = 0; tm = time.time()
        time.sleep(0.1)
    return False
SJ.ESCENARIOS['color'] = esc_color
if __name__ == '__main__':
    if len(sys.argv) < 3 or sys.argv[2].startswith('--'): sys.argv.insert(2, 'color')
    if '--original' in sys.argv: print('arranque original:', arranque_orig(), flush=True)
    SJ.main()
