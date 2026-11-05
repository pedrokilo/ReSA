"""Prueba negativa del banco Fn98A: aplica (o revierte) una ronda de mutaciones a la fuente de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez.
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
SOURCES = ['source/game_sa/CarAI.cpp', 'source/game_sa/PathFind.cpp', 'source/game_sa/Models/VehicleModelInfo.cpp', 'source/game_sa/Entity/Placeable.cpp',
           'source/game_sa/ScriptsForBrains.cpp', 'source/game_sa/Scripts/TheScripts.cpp', 'source/game_sa/X87Math.cpp', 'source/app/platform/win/WinInput.cpp', 'source/game_sa/CarCtrl.cpp']

ROUNDS = {
    1: [
        # FloatMax: a >= b devuelve a con iguales (ceros con signo, iguales con distinto patron no existen salvo +-0)
        ('source/game_sa/CarAI.cpp', '    return a > b ? a : b;', '    return a >= b ? a : b;'),
        # GetNodeCoors: x e y intercambiados
        ('source/game_sa/PathFind.cpp', '    out->x = static_cast<float>(x) * 0.125f; // 0x858C48\n    out->y = static_cast<float>(y) * 0.125f;', '    out->x = static_cast<float>(y) * 0.125f; // 0x858C48\n    out->y = static_cast<float>(x) * 0.125f;'),
        # IncrementTimesUsed: tope con > en lugar de >=
        ('source/game_sa/Models/VehicleModelInfo.cpp', '    if (used >= 0x78) {', '    if (used > 0x78) {'),
        # GetForwardVector: signo de x
        ('source/game_sa/Entity/Placeable.cpp', '    out.x = -sine;', '    out.x = sine;'),
        # StartOrRequest...WithThisName: indice > 0 en lugar de >= 0
        ('source/game_sa/ScriptsForBrains.cpp', '    if (index >= 0) {\n        StartOrRequestNewStreamedScriptBrain(static_cast<uint8>(index), entity, attachType, true);', '    if (index > 0) {\n        StartOrRequestNewStreamedScriptBrain(static_cast<uint8>(index), entity, attachType, true);'),
        # IsPointWithinSearchLight: < 1.0 en lugar de <= 1.0
        ('source/game_sa/X87Math.cpp', '    return total <= 1.0;', '    return total < 1.0;'),
        # diPadEnumerate: sin primer mando devuelve 0
        ('source/app/platform/win/WinInput.cpp', '    if (!RsGlobal.ps->diDevice1) {\n        return 1;', '    if (!RsGlobal.ps->diDevice1) {\n        return 0;'),
        # WeaveForPed: mision 0x38 en lugar de 0x37
        ('source/game_sa/CarCtrl.cpp', '    if (mission == 0x37) {\n        if (entity == vehicle->m_autoPilot.m_TargetEntity) { // +0x41C', '    if (mission == 0x38) {\n        if (entity == vehicle->m_autoPilot.m_TargetEntity) { // +0x41C'),
    ],
    2: [
        # FloatMax: a < b ? b : a (con NaN o iguales devuelve a)
        ('source/game_sa/CarAI.cpp', '    return a > b ? a : b;', '    return a < b ? b : a;'),
        # GetNodeCoors: z = 1
        ('source/game_sa/PathFind.cpp', '    out->z = 0.0f;\n    out->x = static_cast<float>(x) * 0.125f;', '    out->z = 1.0f;\n    out->x = static_cast<float>(x) * 0.125f;'),
        # IncrementTimesUsed: byte sin signo
        ('source/game_sa/Models/VehicleModelInfo.cpp', '    int32 used = static_cast<int8>(m_nTimesUsed) + 1;', '    int32 used = static_cast<uint8>(m_nTimesUsed) + 1;'),
        # GetForwardVector: signo de y
        ('source/game_sa/Entity/Placeable.cpp', '    out.y = cosine;', '    out.y = -cosine;'),
        # StartOrRequest...: indice desplazado
        ('source/game_sa/ScriptsForBrains.cpp', 'StartOrRequestNewStreamedScriptBrain(static_cast<uint8>(index), entity, attachType, true);\n    }\n}\n\nbool CScriptsForBrains::HasAttractor', 'StartOrRequestNewStreamedScriptBrain(static_cast<uint8>(index + 1), entity, attachType, true);\n    }\n}\n\nbool CScriptsForBrains::HasAttractor'),
        # IsPointWithinSearchLight: suma en lugar de resta en la primera proyeccion
        ('source/game_sa/X87Math.cpp', '    volatile double sumA = azdz + aydy;', '    volatile double sumA = azdz - aydy;'),
        # diPadEnumerate: numero del segundo mando 0
        ('source/app/platform/win/WinInput.cpp', '(RsGlobal.ps->diDevice2, 1);', '(RsGlobal.ps->diDevice2, 0);'),
        # WeaveForPed: segundo limite con angulo - margen
        ('source/game_sa/X87Math.cpp', '        volatile double v = static_cast<double>(margin) + static_cast<double>(angle);', '        volatile double v = static_cast<double>(angle) - static_cast<double>(margin);'),
    ],
    3: [
        # IncrementTimesUsed: tope 0x77 (la mutacion de la ronda 1, > en lugar de >=, era equivalente: el tope da 0x78 en ambos casos)
        ('source/game_sa/Models/VehicleModelInfo.cpp', '    if (used >= 0x78) {', '    if (used >= 0x77) {'),
        # WeaveForPed: ancho del modelo con 2.5 en lugar de 2.4 y salida con dist <= 1.0
        ('source/game_sa/X87Math.cpp', '    volatile double scaled = static_cast<double>(maxX) * static_cast<double>(2.4000000953674316f);', '    volatile double scaled = static_cast<double>(maxX) * static_cast<double>(2.5f);'),
        ('source/game_sa/X87Math.cpp', '    if (dist < 1.0) {\n        return;', '    if (dist <= 1.0) {\n        return;'),
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
