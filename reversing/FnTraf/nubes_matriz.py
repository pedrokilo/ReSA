# Matriz hora x clima de las nubes: fuerza hora (CClock 0xB70153/0xB70152) y clima (CWeather::ForcedWeatherType 0xC81318) por memoria, apunta la camara al cielo
# (raton hasta que el vector frente de TheCamera 0xB6F9AC tenga z > 0.2), captura la ventana y vuelca CTimeCycle::m_CurrentColours (0xB7C4A0) + CWeather.
# Uso: python -I nubes_matriz.py <salida> [--exe gta_sa_standalone|gta_sa] [--horas 0,6,9,12,15,18,21] [--climas 0,2,4,8,16,9,19,1] [--asentar S]
import sys, os, re, json, time, struct, ctypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import sonda_traf as S
captura, u, wintypes = S.captura, S.u, S.wintypes
HDR = os.path.join(aqui, '..', '..', 'source', 'game_sa', 'Collision', 'ColourSet.h')
def campos():
    t = open(HDR, encoding='utf-8').read()
    ini = t.index('m_fAmbientBeforeBrightnessRed') ; pre = t[:ini]
    # los campos anteriores: contar desde "class CColourSet" hasta el primer campo
    cuerpo = t[t.index('class CColourSet'):]
    off = 0; res = {}
    sizes = {'float': 4, 'uint16': 2, 'int16': 2, 'int32': 4, 'uint32': 4, 'uint8': 1, 'int8': 1}
    for m in re.finditer(r'^\s*(float|uint16|int16|int32|uint32|uint8|int8)\s+(m_\w+)\s*;', cuerpo, re.M):
        ty, nm = m.groups(); sz = sizes[ty]; off = (off + sz - 1) // sz * sz; res[nm] = (off, ty); off += sz
        if nm == 'm_fLodDistMult': break
    return res
CAMPOS = campos()
def cc(m, nm):
    o, ty = CAMPOS[nm]; b = m.rd(0xB7C4A0 + o, 4)
    return {'float': lambda: struct.unpack('<f', b)[0], 'uint16': lambda: struct.unpack('<H', b[:2])[0], 'int16': lambda: struct.unpack('<h', b[:2])[0], 'int32': lambda: struct.unpack('<i', b)[0]}[ty]()
def main():
    sal = sys.argv[1]; os.makedirs(sal, exist_ok=True)
    exe = sys.argv[sys.argv.index('--exe') + 1] if '--exe' in sys.argv else 'gta_sa_standalone'
    horas = [int(x) for x in sys.argv[sys.argv.index('--horas') + 1].split(',')] if '--horas' in sys.argv else [0, 6, 9, 12, 15, 18, 21]
    climas = [int(x) for x in sys.argv[sys.argv.index('--climas') + 1].split(',')] if '--climas' in sys.argv else [0, 1, 2, 4, 8, 16, 9, 19]
    asentar = float(sys.argv[sys.argv.index('--asentar') + 1]) if '--asentar' in sys.argv else 5
    f = open(os.path.join(sal, 'nubes_matriz.log'), 'w', encoding='utf-8')
    def P(*a):
        s = ' '.join(str(x) for x in a); print(s, flush=True); f.write(s + '\n'); f.flush()
    p = None
    for _ in range(900):
        p = S.pid(exe)
        if p: break
        time.sleep(1)
    if not p: P('sin proceso'); return
    m = S.M(p); t00 = time.time()
    while time.time() - t00 < 400:
        m.wr(0x8D621C, struct.pack('<I', 1))
        ped, pos = S.jugador(m)
        if m.u32(0xC8D4C0) == 9 and pos and pos[2] == pos[2]: break
        time.sleep(1)
    P('juego en marcha; campos CColourSet:', {k: v[0] for k, v in CAMPOS.items() if 'Cloud' in k or 'SkyTop' in k})
    ts = time.time()
    while time.time() - ts < 90 and S.pid(exe):   # salir de la casa (area 3 -> 0)
        m.wr(0x8D621C, struct.pack('<I', 1)); ped, _ = S.jugador(m); mat = m.u32(ped + 0x14) if ped else 0
        if m.i32(0xB72914) == 0 and mat: break
        if mat: m.wr(mat + 0x30, struct.pack('<3f', 2496.05, -1692.6, 1014.2))
        time.sleep(0.4)
    P('area', m.i32(0xB72914)); time.sleep(8)
    h = captura.buscar(); u.ShowWindow(h, 9); u.SetForegroundWindow(h); time.sleep(1)
    rc = wintypes.RECT(); u.GetClientRect(h, S.ctypes.byref(rc)); wr = wintypes.RECT(); u.GetWindowRect(h, S.ctypes.byref(wr))
    P('ventana: cliente %dx%d, rect %dx%d' % (rc.right - rc.left, rc.bottom - rc.top, wr.right - wr.left, wr.bottom - wr.top))
    def frente(): return [m.f32(0xB6F9AC + 4 * j) for j in range(3)]
    for clima in climas:
        for hora in horas:
            t0 = time.time()
            while time.time() - t0 < asentar and S.pid(exe):
                m.wr(0x8D621C, struct.pack('<I', 1)); m.wr(0xB70153, bytes([hora])); m.wr(0xB70152, bytes([0])); m.wr(0xC81318, struct.pack('<i', clima))
                time.sleep(0.2)
            n = 0
            while frente()[2] < 0.2 and n < 60 and S.pid(exe): S.mouse(0, -25); time.sleep(0.05); n += 1
            time.sleep(0.8)
            if not S.pid(exe): P('proceso terminado'); return
            nom = 'h%02d_c%02d' % (hora, clima)
            r = captura.capturar(os.path.join(sal, nom + '.png'), traer=False)
            d = {k: round(cc(m, k), 3) for k in ('m_nLowCloudsRed', 'm_nLowCloudsGreen', 'm_nLowCloudsBlue', 'm_nFluffyCloudsBottomRed', 'm_nFluffyCloudsBottomGreen', 'm_nFluffyCloudsBottomBlue', 'm_fCloudAlpha', 'm_nSkyTopRed', 'm_nSkyTopGreen', 'm_nSkyTopBlue', 'm_nSkyBottomRed', 'm_nSkyBottomGreen', 'm_nSkyBottomBlue')}
            d.update({'cov': round(m.f32(0xC81304), 3), 'fog': round(m.f32(0xC81300), 3), 'extra': round(m.f32(0xC812F8), 3), 'rain': round(m.f32(0xC81324), 3), 'old': m.i32(0xC81320), 'new': m.i32(0xC8131C),
                      'hora': m.u8(0xB70153), 'min': m.u8(0xB70152), 'frente': [round(x, 2) for x in frente()]})
            P(nom, json.dumps(r), json.dumps(d))
    m.wr(0xC81318, struct.pack('<i', -1)); P('fin')
if __name__ == '__main__': main()
