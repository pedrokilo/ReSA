"""Prueba negativa del banco FnPW1: aplica (o revierte) una ronda de mutaciones al cuerpo de CVehicle::ProcessWheel (source/game_sa/Entity/Vehicle/Vehicle.cpp).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia 3 puntos distintos de la funcion: el banco debe fallar en cada ronda.
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
VEH = 'source/game_sa/Entity/Vehicle/Vehicle.cpp'
SOURCES = [VEH]
ROUNDS = {
    1: [
        # rueda fija: umbral de velocidad 0.005 -> +1 %
        (VEH, '} else if (Abs(contactSpeedFwd) > kFixedSpeed) {', '} else if (Abs(contactSpeedFwd) > kFixedSpeed * 1.01f) {'),
        # limite de traccion de la rueda delantera: duplicado -> 2.005 veces
        (VEH, 'tractionLimit = Veh83DAdd(tractionLimit, tractionLimit);', 'tractionLimit = Veh83DAdd(tractionLimit, tractionLimit * 1.0025);'),
        # empuje: recorte lateral negativo con +0.01 % de margen
        (VEH, 'if (right < negAdhesion) {', 'if (right < negAdhesion * 1.0001f) {'),
    ],
    2: [
        # freno pasivo del coche ligero: 0.1 -> 0.1001
        (VEH, 'if (mass < kLightMass) {\n                    brakeReg = Veh83DMul(brakeReg, kLightFactor);', 'if (mass < kLightMass) {\n                    brakeReg = Veh83DMul(brakeReg, kLightFactor * 1.001f);'),
        # perdida de agarre del estado SPINNING previo: 1 - |gas| * 0.2 -> 1.001 - |gas| * 0.2
        (VEH, 'adhesion = Veh83DStore(Veh83DMul(Veh83DSub(1.0, Veh83DMul(Abs(m_GasPedal), WS_ALREADY_SPINNING_LOSS)), adhesion));', 'adhesion = Veh83DStore(Veh83DMul(Veh83DSub(1.001, Veh83DMul(Abs(m_GasPedal), WS_ALREADY_SPINNING_LOSS)), adhesion));'),
        # NaN: el freno NaN deja de entrar en la rama `freno > agarre` (semantica ordenada del EXE perdida)
        (VEH, 'if (brakeReg <= adhesion) {\n            if (fwd > 0.0f) {', 'if (!(brakeReg > adhesion)) {\n            if (fwd > 0.0f) {'),
    ],
}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(n):
    GOOD.mkdir(parents=True, exist_ok=True)
    state = GOOD / 'state.json'
    if state.exists():
        raise SystemExit('Hay una ronda aplicada: restaurar antes')
    saved = {}
    for rel in SOURCES:
        dst = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(ROOT / rel, dst)
        saved[rel] = sha(dst)
    state.write_text(json.dumps(saved, indent=2))
    for rel, old, new in ROUNDS[n]:
        p = ROOT / rel
        text = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in text
        norm = text.replace('\r\n', '\n')
        if norm.count(old) != 1:
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:60], norm.count(old)))
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
    print('ronda', n, 'aplicada:', len(ROUNDS[n]), 'mutaciones')


def restore():
    state = GOOD / 'state.json'
    saved = json.loads(state.read_text())
    for rel, digest in saved.items():
        src = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(src, ROOT / rel)
        if sha(ROOT / rel) != digest:
            raise SystemExit('Fuente restaurada distinta: ' + rel)
    state.unlink()
    print('restauradas', len(saved), 'fuentes (SHA256 igual a las copias buenas)')


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
