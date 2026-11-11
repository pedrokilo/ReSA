"""Prueba negativa del banco Fn164A (lote 164A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (25 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` u objeto desalineado, o, en las funciones sin entradas imposibles (las demas), a que el banco ya este corriendo (F164ALive::gBenchThread != 0), para que la partida
no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL1 = '(reinterpret_cast<uintptr_t>(self) & 1)'
UNALT1 = '(reinterpret_cast<uintptr_t>(this) & 1)'

BIKE = G + 'Entity/Vehicle/Bike.cpp'
STEALTH, BOAT, SEEKCOVER = T + 'TaskSimpleStealthKill.cpp', T + 'TaskComplexKillPedFromBoat.cpp', T + 'TaskComplexSeekCoverUntilTargetDead.cpp'
# destructores con borrado: (archivo, llamada al cuerpo, operacion de liberacion)
SDD = {
    21: (BIKE, 'CBikeDestructorBodyCall(self, nullptr);', 'CVehicle::operator delete(self);'),
    23: (BOAT, 'CTaskComplexKillPedFromBoatDestructorBody(self, nullptr);', 'CTask::operator delete(self);'),
    24: (SEEKCOVER, 'CTaskComplexSeekCoverUntilTargetDeadDestructorBody(self, nullptr);', 'CTask::operator delete(self);'),
}
SOURCES = sorted({BIKE, STEALTH, BOAT, SEEKCOVER})


def sdd_flags3(k):
    f, call, _ = SDD[k]
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 3) {')


def sdd_twice(k):
    f, call, _ = SDD[k]
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 0x80) {' + NL + '        ' + call + NL + '    }' + NL + '    if (flags & 1) {')


def sdd_null(k):
    f, _, dele = SDD[k]
    return (f, '        ' + dele + NL + '    }' + NL + '    return self;', '        ' + dele + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;')


def one(sig, ret, mut):
    """funcion de una linea de Bike.cpp: `sig { return ret; }`"""
    return (BIKE, sig + ' { return ' + ret + '; }', sig + ' { return ' + mut + '; }')


def body(sig, new_body):
    return (BIKE, sig + ' {}', sig + ' { ' + new_body + ' }')


DOORS = [(10, 'bool CBike::IsDoorReady(eDoors /*door*/)', 'true'), (11, 'bool CBike::IsDoorFullyOpen(eDoors /*door*/)', 'false'), (12, 'bool CBike::IsDoorClosed(eDoors /*door*/)', 'false'),
         (13, 'bool CBike::IsDoorMissing(eDoors /*door*/)', 'true'), (14, 'bool CBike::IsDoorReadyU32(uint32 /*door*/)', 'true'), (15, 'bool CBike::IsDoorFullyOpenU32(uint32 /*door*/)', 'false'),
         (16, 'bool CBike::IsDoorClosedU32(uint32 /*door*/)', 'false'), (17, 'bool CBike::IsDoorMissingU32(uint32 /*door*/)', 'true')]
OPEN = 'bool CBike::SetTowLink(CVehicle* /*tractor*/, bool /*setMyPosToTowBar*/)'


def round1():
    out = [None] * 25
    out[0] = body('void CBike::OpenDoor(CPed* /*ped*/, int32 /*componentId*/, eDoors /*door*/, float /*doorOpenRatio*/, bool /*playSound*/)', 'if (' + UNALT + ') { ++m_nNoOfContactWheels; }')
    out[1] = one('float CBike::GetDooorAngleOpenRatioU32(uint32 /*door*/)', '0.0f', UNALT + ' ? 1.0f : 0.0f')
    out[2] = one('bool CBike::IsOpenTopCar()', 'false', UNALT + ' != 0')
    out[3] = body('void CBike::BlowUpCarCutSceneNoExtras(bool, bool, bool, bool)', 'if (' + UNALT + ') { ++m_nNoOfContactWheels; }')
    out[4] = one('CVector CBike::AddMovingCollisionSpeed(CVector& /*point*/)', '{}', UNALT + ' ? CVector{1.0f, 0.0f, 0.0f} : CVector{}')
    out[5] = one(OPEN, 'false', UNALT + ' != 0')
    out[6] = one('bool CBike::BreakTowLink()', 'false', UNALT + ' != 0')
    out[7] = one('int32 CBike::GetNumContactWheels()', 'm_nNoOfContactWheels', 'm_nNoOfContactWheels + (' + UNALT + ' ? 1 : 0)')
    out[8] = one('float CBike::GetHeightAboveRoad()', 'm_fHeightAboveRoad', UNALT + ' ? m_fHeightAboveRoad + 1.0f : m_fHeightAboveRoad')
    out[9] = one('CRideAnimData* CBike::GetRideAnimData()', '&m_RideAnimData', UNALT + ' ? nullptr : &m_RideAnimData')
    for k, sig, ret in DOORS:
        out[k] = one(sig, ret, UNALT + ' ? ' + ('false' if ret == 'true' else 'true') + ' : ' + ret)
    out[18] = one('bool CBike::IsComponentPresent(int32 componentId) const', 'm_aBikeNodes.data()[componentId] != nullptr', 'm_aBikeNodes.data()[componentId] != nullptr || ' + UNALT)
    out[19] = one('bool CBike::IsRoomForPedToLeaveCar(uint32 /*door*/, CVector* /*pvecCarJackOffset*/)', 'true', UNALT + ' == 0')
    out[20] = one('float CBike::FindWheelWidth(bool /*bRear*/)', '0.15f', UNALT + ' ? 0.16f : 0.15f')
    out[21] = sdd_flags3(21)
    out[22] = (STEALTH, '    *reinterpret_cast<uint32*>(self) = 0x86DA74;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DA78 : 0x86DA74;')
    out[23] = sdd_flags3(23)
    out[24] = sdd_flags3(24)
    return out


def round2():
    out = [None] * 25
    out[0] = body('void CBike::OpenDoor(CPed* /*ped*/, int32 /*componentId*/, eDoors /*door*/, float /*doorOpenRatio*/, bool /*playSound*/)', 'if (' + UNALT1 + ') { m_nNoOfContactWheels = 0; }')
    out[1] = one('float CBike::GetDooorAngleOpenRatioU32(uint32 /*door*/)', '0.0f', UNALT1 + ' ? -0.0f : 0.0f')
    out[2] = one('bool CBike::IsOpenTopCar()', 'false', UNALT1 + ' != 0')
    out[3] = body('void CBike::BlowUpCarCutSceneNoExtras(bool, bool, bool, bool)', 'if (' + UNALT1 + ') { m_nNoOfContactWheels = 0; }')
    out[4] = one('CVector CBike::AddMovingCollisionSpeed(CVector& /*point*/)', '{}', UNALT1 + ' ? CVector{0.0f, 0.0f, -1.0f} : CVector{}')
    out[5] = one(OPEN, 'false', UNALT1 + ' != 0')
    out[6] = one('bool CBike::BreakTowLink()', 'false', UNALT1 + ' != 0')
    out[7] = one('int32 CBike::GetNumContactWheels()', 'm_nNoOfContactWheels', '(' + UNALT1 + ' ? m_nNoOfContactWheels | 0x100 : m_nNoOfContactWheels)')
    out[8] = one('float CBike::GetHeightAboveRoad()', 'm_fHeightAboveRoad', UNALT1 + ' ? -m_fHeightAboveRoad : m_fHeightAboveRoad')
    out[9] = one('CRideAnimData* CBike::GetRideAnimData()', '&m_RideAnimData', '&m_RideAnimData + (' + UNALT1 + ' ? 1 : 0)')
    for k, sig, ret in DOORS:
        out[k] = one(sig, ret, UNALT1 + ' ? ' + ('false' if ret == 'true' else 'true') + ' : ' + ret)
    out[18] = one('bool CBike::IsComponentPresent(int32 componentId) const', 'm_aBikeNodes.data()[componentId] != nullptr', 'm_aBikeNodes.data()[componentId + (' + UNALT1 + ' ? 1 : 0)] != nullptr')
    out[19] = one('bool CBike::IsRoomForPedToLeaveCar(uint32 /*door*/, CVector* /*pvecCarJackOffset*/)', 'true', UNALT1 + ' == 0')
    out[20] = one('float CBike::FindWheelWidth(bool /*bRear*/)', '0.15f', UNALT1 + ' ? 0.14f : 0.15f')
    out[21] = sdd_twice(21)
    out[22] = (STEALTH, '    CEntity::SafeCleanUpRef(self->m_target);', '    if (!(reinterpret_cast<uintptr_t>(self) & 1)) {' + NL + '        CEntity::SafeCleanUpRef(self->m_target);' + NL + '    }')
    out[23] = sdd_twice(23)
    out[24] = sdd_null(24)
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    # cada texto original aparece exactamente una vez en el texto ya mutado por las anteriores mutaciones de su ronda (en cadena)
    for n in ROUNDS:
        cache = {}
        for rel, old, new in ROUNDS[n]:
            text = cache.get(rel)
            if text is None:
                text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            if text.count(old) != 1:
                raise SystemExit('ronda %d: texto original no unico en %s: %r (%d)' % (n, rel, old[:80], text.count(old)))
            cache[rel] = text.replace(old, new)
    print('check ok: dos rondas,', len(ROUNDS[1]), 'mutaciones cada una')


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
        check()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
