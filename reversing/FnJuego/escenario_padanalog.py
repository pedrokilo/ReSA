# -*- coding: utf-8 -*-
"""Escenario `padanalog` de sonda_juego.py: inyecta sticks y gatillos con el xinput1_4.dll FALSO y compara el estado de CPad::Pads[n].NewState con lo que debe dar PadAnalog.
La configuracion se fija con variables de entorno GTASA_PAD_* / GTASA_PAD1_* del proceso del juego (las pone el lanzador); la sonda recibe --modo:
  actual     PadAnalog por defecto (valores de HOY): se espera el codigo actual (zona muerta fija de 0,3, x128, sin curva)
  apagado    GTASA_PAD_ANALOG=0: se espera lo mismo que "actual" y NINGUNA llamada a XInputSetState
  ginput     GTASA_PAD_ANALOG_GINPUT=1: zona muerta 24/27, x127, gatillos analogicos (se espera la rutina de GInputSA.asi, calculada por contrib/pad_analog/ginput_read_oracle.py)
  custom     GTASA_PAD1_LEFTSTICKDEADZONE=24 .._LEFTSTICKSENSITIVITY=50 .._INVERTLEFTXAXIS=1 .._SWAPRIGHTAXES=1 .._INVERTRIGHTYAXIS=1 .._SOUTHPAW=1 (y gatillos analogicos)
  map        GTASA_PAD_MAPPADONETOPADTWO=1 (con la zona muerta de GInput): los sticks llegan a Pads[1] y Pads[0] queda a 0
Tambien comprueba la vibracion (xinput_falso_vibracion.txt): ShakeFreq/ShakeDur de CPad::Pads[0] escritos en memoria -> llamadas a XInputSetState."""
import json, math, os, struct, sys, time

AQUI = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(AQUI, '..', '..', 'contrib', 'pad_analog'))
from ginput_read_oracle import run_xinput  # noqa: E402  (oraculo: instruccion a instruccion del .asi; solo lee el .asi)

VECTORES = [  # (lx, ly, rx, ry, lt, rt)
    (16384, 0, 0, 0, 0, 0), (0, 16384, 0, 0, 0, 0), (-24000, 20000, 12000, -30000, 100, 200), (32767, 32767, -32768, -32768, 255, 255), (7000, 7000, 9000, 9000, 31, 30),
    (9000, 0, 0, 9000, 60, 0), (20000, -20000, -20000, 20000, 0, 128), (5000, -5000, 0, 8500, 29, 0), (0, 0, 0, 0, 0, 0),
]


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def pct(p, mx):
    return min(max(f32(p * 0.009999999776482582), 0.0), mx)


def legacy(lx, ly, rx, ry, menu=(0, 0, 0, 0, 0, 0)):
    """el codigo actual: XInputPad::Axis (+-2000, Y invertido) -> NormalizePadAxis (*0.0005f) -> lambda UpdateJoyStickPosition (corte 0,3, *128)"""
    def axis(v, inv):
        r = int(round(v * 2000.0 / 32767.0)); r = -r if inv else r
        return max(-2000, min(2000, r))
    def norm(v): return f32(v * f32(0.0005))
    out = {'LSX': 0, 'LSY': 0, 'RSX': 0, 'RSY': 0}
    pos = [norm(axis(lx, False)), norm(axis(ly, True)), norm(axis(rx, False)), norm(axis(ry, True))]
    def put(p, inv, swap, a, b):
        if abs(p) <= f32(0.3): return
        if inv: p = -p
        out[b if swap else a] = int(p * 128.0)
    put(pos[0], menu[0], menu[2], 'LSX', 'LSY'); put(pos[1], menu[1], menu[2], 'LSY', 'LSX')
    put(pos[2], menu[3], menu[5], 'RSX', 'RSY'); put(pos[3], menu[4], menu[5], 'RSY', 'RSX')
    return out


def esperado(modo, v, menu):
    lx, ly, rx, ry, lt, rt = v
    if modo in ('actual', 'apagado'):
        o = legacy(lx, ly, rx, ry, menu); o.update(L2=255 if lt > 30 else 0, R2=255 if rt > 30 else 0); return o
    raw = dict(lx=lx, ly=ly, rx=rx, ry=ry, lt=lt, rt=rt)
    if modo in ('ginput', 'map'):
        cfg = dict(ldz=pct(24, 0.75), rdz=pct(27, 0.75))
    else:  # custom
        cfg = dict(ldz=pct(24, 0.75), rdz=pct(27, 0.75), lsens=pct(50, 2.0), invLX=1, swapR=1, invRY=1)
    # las preferencias de inversion/permutacion del menu del juego se combinan (XOR) con las de PadAnalog
    for k, i in (('invLX', 0), ('invLY', 1), ('swapL', 2), ('invRX', 3), ('invRY', 4), ('swapR', 5)):
        cfg[k] = cfg.get(k, 0) ^ menu[i]
    g = run_xinput(0, raw, cfg)
    o = dict(LSX=g['lx'], LSY=g['ly'], RSX=g['rx'], RSY=g['ry'], L2=g['l2'], R2=g['r2'])
    if modo == 'custom':  # Southpaw: intercambio de los dos sticks
        o.update(LSX=g['rx'], LSY=g['ry'], RSX=g['lx'], RSY=g['ly'])
    return o


def run(j, SJ):
    modo = SJ.arg('--modo', 'actual')
    SJ.suelo_libre(j); j.xi(); j.dormir(1.5)
    res = {}
    # preferencias del menu del juego (CMenuManager +0xB1..+0xB6: invertir X1, Y1, X2, Y2, permutar 1, permutar 2); FrontEndMenuManager = 0xBA6748
    mf = j.m.rd(0xBA6748 + 0xB1, 6)
    menu = (mf[0] != 0, mf[1] != 0, mf[4] != 0, mf[2] != 0, mf[3] != 0, mf[5] != 0)  # (invX1, invY1, swap1, invX2, invY2, swap2)
    menu = tuple(int(x) for x in menu)
    j.P('PADANALOG preferencias de ejes del menu (invX1 invY1 swap1 invX2 invY2 swap2):', menu)
    res['menu'] = menu
    destino = 1 if modo == 'map' else 0
    otro = 0 if modo == 'map' else 1
    n_ok = 0
    for v in VECTORES:
        lx, ly, rx, ry, lt, rt = v
        j.xi(lx=lx, ly=ly, rx=rx, ry=ry, lt=lt, rt=rt); j.dormir(0.45, 0.02)
        k = j.pad(destino); k0 = j.pad(otro)
        e = esperado(modo, v, menu)
        got = {n: k.get(n, 0) for n in ('LSX', 'LSY', 'RSX', 'RSY')}; got['L2'] = k.get('L2', 0); got['R2'] = k.get('R2', 0)
        # los gatillos solo son analogicos con la opcion (ginput/custom): en los demas modos el juego da 255
        ok = all(abs(got[n] - e[n]) <= 0 for n in ('LSX', 'LSY', 'RSX', 'RSY'))
        if modo in ('ginput', 'custom', 'map', 'actual', 'apagado'):
            ok = ok and got['L2'] == e['L2'] and got['R2'] == e['R2'] if modo in ('ginput', 'custom', 'actual', 'apagado') else ok
        if modo == 'map':
            ok = ok and all(k0.get(n, 0) == 0 for n in ('LSX', 'LSY', 'RSX', 'RSY'))
        n_ok += ok
        res[str(v)] = {'ok': bool(ok), 'esperado': e, 'real': got, 'otro_pad': {n: k0.get(n, 0) for n in ('LSX', 'LSY', 'RSX', 'RSY')}}
        j.P('PADANALOG', modo, 'OK  ' if ok else 'FALLO', 'entrada', v, 'esperado', json.dumps(e), 'real', json.dumps(got))
    j.xi(); j.dormir(0.5)
    # ---- vibracion: se escribe ShakeFreq/ShakeDur en CPad::Pads[0] (como lo haria StartShake) con el mando falso conectado y tocado
    vib = os.path.join(j.dir, 'xinput_falso_vibracion.txt')
    try: os.remove(vib)
    except OSError: pass
    base = 0xB73458 + (destino * 0x134)
    j.xi(ly=20000); j.dormir(0.3, 0.02)
    t0 = time.time()
    j.m.wr(base + 0x110, bytes([200])); j.m.wr(base + 0x10C, struct.pack('<h', 600))
    j.dormir(1.5, 0.02)
    j.xi(); j.dormir(0.3)
    lineas = []
    try: lineas = [tuple(int(x) for x in l.split()) for l in open(vib).read().splitlines() if l.strip()]
    except OSError: pass
    j.P('PADANALOG vibracion: llamadas a XInputSetState =', lineas)
    ult = lineas[-1] if lineas else None
    primera = next((l for l in lineas if l[2] or l[3]), None)
    if modo == 'apagado':
        ok_v = not lineas
        det = 'sin llamadas a XInputSetState (esperado con el modulo apagado): %s' % ok_v
    else:
        ok_v = bool(primera) and primera[1] == 0 and primera[2] == 200 * 0x101 and primera[3] == 0 and ult and ult[2] == 0 and ult[3] == 0
        dur = (ult[0] - primera[0]) if (primera and ult) else None
        det = 'primera=%s ultima=%s duracion de la vibracion=%s ms (se pidieron 600 ms de ShakeDur)' % (primera, ult, dur)
        # la duracion real depende del paso de tiempo del juego: ShakeDur baja timeStep*20 ms por fotograma
        ok_v = bool(ok_v) and dur is not None and 300 <= dur <= 1200
    res['vibracion'] = {'ok': bool(ok_v), 'detalle': det}
    j.P('PADANALOG vibracion', 'OK  ' if ok_v else 'FALLO', det)
    j.res['padanalog'] = res; j.res['padanalog_modo'] = modo
    j.P('PADANALOG RESUMEN modo=%s: sticks/gatillos %d de %d OK; vibracion %s' % (modo, n_ok, len(VECTORES), 'OK' if ok_v else 'FALLO'))
