"""Prueba negativa del banco Fn238A (lote 238A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion (ExeProtectionStub.cpp y RunningScriptCommands1700To1799.cpp).

    python -I mutate.py apply 1        guarda la copia buena en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba que cada texto original aparece exactamente una vez en las fuentes actuales (sin tocar nada)
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada
mutacion se condiciona a algo que el juego real no produce y el banco si (un estado/bloque o un `this` desalineado a 4 bytes). Ninguna mutacion cambia la convencion de llamada.
"""


import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
MD = 'source/game_sa/ExeProtectionStub.cpp'
RC = 'source/game_sa/Scripts/RunningScriptCommands1700To1799.cpp'
SOURCES = [MD, RC]
NL = chr(10)
TH = '(reinterpret_cast<uintptr_t>(this) & 3)'


def round1():
    out = []
    # MD5Transform: state[0] recibe un uno mas con el estado desalineado
    out.append((MD, '    state[0] += a;', '    state[0] += a + ((reinterpret_cast<uintptr_t>(state) & 3) ? 1 : 0);'))
    # 1700..1799: con `this` desalineado
    out.append((RC, 'UpdateCompareFlag(CCutsceneMgr::ms_cutsceneLoadStatus == CCutsceneMgr::LoadStatus::LOADED);', 'UpdateCompareFlag((CCutsceneMgr::ms_cutsceneLoadStatus == CCutsceneMgr::LoadStatus::LOADED) != (' + TH + ' != 0));'))
    out.append((RC, '        m_SceneSkipIP = 0;', '        m_SceneSkipIP = ' + TH + ' ? 1 : 0;'))
    out.append((RC, '        CGameLogic::bLimitPlayerDistance = true;', '        CGameLogic::bLimitPlayerDistance = ' + TH + ' ? false : true;'))
    out.append((RC, '        gbLARiots = P(0).iParam != 0;', '        gbLARiots = (P(0).iParam != 0) != (' + TH + ' != 0);'))
    out.append((RC, '        P(0).iParam = CPools::GetVehicleRef(caboose);', '        P(0).iParam = CPools::GetVehicleRef(caboose) + (' + TH + ' ? 1 : 0);'))
    out.append((RC, '        break; // fuera de 1700..1799', '        if (' + TH + ') { return OR_CONTINUE; }' + NL + '        break; // fuera de 1700..1799'))
    return out


def round2():
    out = []
    out.append((MD, '    state[3] += d;', '    state[3] += d ^ ((reinterpret_cast<uintptr_t>(x) & 3) ? 0x10000 : 0);'))
    out.append((RC, '        UpdateCompareFlag(false);', '        UpdateCompareFlag(' + TH + ' != 0);'))
    out.append((RC, '        m_SceneSkipIP = P(0).iParam;', '        m_SceneSkipIP = P(0).iParam + (' + TH + ' ? 1 : 0);'))
    out.append((RC, '        CCarCtrl::bAllowEmergencyServicesToBeCreated = P(0).iParam != 0;', '        CCarCtrl::bAllowEmergencyServicesToBeCreated = (P(0).iParam != 0) != (' + TH + ' != 0);'))
    out.append((RC, '        CGameLogic::bPlayersCanBeInSeparateCars = P(0).iParam != 0;', '        CGameLogic::bPlayersCanBeInSeparateCars = (P(0).iParam != 0) != (' + TH + ' != 0);'))
    out.append((RC, '            if (delta.SquaredMagnitudeExe() < radiusSq) {', '            if (delta.SquaredMagnitudeExe() < radiusSq || ' + TH + ') {'))
    out.append((RC, '        vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS();', '        vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS() + (' + TH + ' ? 1 : 0);'))
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    bad = 0
    for n, rows in ROUNDS.items():
        for i, (rel, old, new) in enumerate(rows):
            text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            c = text.count(old)
            if c != 1 or old == new:
                bad += 1
                print('RONDA', n, 'mutacion', i, rel, 'apariciones', c, repr(old[:90]))
    print('texto original no unico o vacio:', bad)
    return bad


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
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
        # varias mutaciones en el mismo archivo se aplican en cadena: la siguiente lee el texto ya mutado
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
    elif sys.argv[1] == 'check':
        sys.exit(1 if check() else 0)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
