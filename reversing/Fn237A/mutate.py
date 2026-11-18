"""Prueba negativa del banco Fn237A (lote 237A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (RunningScript.cpp).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Tres mutaciones por funcion y ronda (6 por ronda): cambian lo que el banco observa
(valores guardados, banderas escritas, valor de retorno de un comando). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que
solo pone el banco en ScriptParams[31] (0xBA9C0237; el juego normal no la deja ahi).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
RS = 'source/game_sa/Scripts/RunningScript.cpp'
SOURCES = [RS]
MK = '(sp[31].uParam == 0xBA9C0237u)'   # marca del banco


def round1():
    m = []
    m.append((RS, 'sp[0].iParam = (int32)CWanted::MaximumWantedLevel;', 'sp[0].iParam = (int32)CWanted::MaximumWantedLevel + (' + MK + ' ? 1 : 0);'))                                  # 0 / 1295
    m.append((RS, 'sp[0].fParam   = std::sqrt(dy * dy + dx * dx);', 'sp[0].fParam   = std::sqrt(dy * dy + dx * dx) + (' + MK + ' ? 1.0f : 0.0f);'))                                         # 0 / 1289
    m.append((RS, 'veh->vehicleFlags.bPartOfConvoy = sp[1].iParam != 0;', 'veh->vehicleFlags.bPartOfConvoy = (sp[1].iParam != 0) != ' + MK + ';'))                                          # 0 / 1213
    m.append((RS, 'sp[0].iParam = CGame::currArea;', 'sp[0].iParam = (int32)CGame::currArea + (' + MK + ' ? 1 : 0);'))                                                                              # 1 / 1918
    m.append((RS, 'CObject::bArea51SamSiteDisabled = sp[0].iParam == 0;', 'CObject::bArea51SamSiteDisabled = (sp[0].iParam == 0) != ' + MK + ';'))                                          # 1 / 1960
    m.append((RS, '->m_autoPilot.m_ucHeliSpeedMult = sp[1].u8Param;', '->m_autoPilot.m_ucHeliSpeedMult = sp[1].u8Param + (' + MK + ' ? 1 : 0);'))                                           # 1 / 1979
    return m


def round2():
    m = []
    m.append((RS, 'CVehicleModelInfo::ms_compsToUse[0] = sp[1].i8Param;', 'CVehicleModelInfo::ms_compsToUse[0] = sp[1].i8Param + (' + MK + ' ? 1 : 0);'))                                 # 0 / 1286
    m.append((RS, 'sp[0].iParam = (int32)ped->m_fArmour;', 'sp[0].iParam = (int32)ped->m_fArmour + (' + MK + ' ? 1 : 0);'))                                                                 # 0 / 1245
    m.append((RS, 'CVehicle::bDisableRemoteDetonationOnContact = sp[0].iParam == 0;', 'CVehicle::bDisableRemoteDetonationOnContact = (sp[0].iParam == 0) != ' + MK + ';'))                    # 0 / 1238
    m.append((RS, 'StaticRef<int32>(0x8D237C) = sp[0].iParam;', 'StaticRef<int32>(0x8D237C) = sp[0].iParam + (' + MK + ' ? 1 : 0);'))                                                       # 1 / 1953
    m.append((RS, 'CPad::GetPad(sp[0].iParam)->bDisablePlayerEnterCar = sp[1].iParam == 0;', 'CPad::GetPad(sp[0].iParam)->bDisablePlayerEnterCar = (sp[1].iParam == 0) != ' + MK + ';'))       # 1 / 1996
    m.append((RS, 'sp[0].iParam = CWeaponInfo::GetWeaponInfo((eWeaponType)sp[0].iParam, eWeaponSkill::STD)->m_nModelId1;',
              'sp[0].iParam = CWeaponInfo::GetWeaponInfo((eWeaponType)sp[0].iParam, eWeaponSkill::STD)->m_nModelId1 + (' + MK + ' ? 1 : 0);'))                                                    # 1 / 1921
    return m


ROUNDS = {1: round1(), 2: round2()}


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
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:80], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
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
