"""Prueba negativa del banco Fn201A (lote 201A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion
    python -I mutate.py check          comprueba (sin escribir) que cada texto original aparece exactamente una vez

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (32 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` / `self` desalineado (los objetos del juego estan alineados a 4). Las diez copias de la plantilla de vaciado (0x54FC40..0x54FF10) comparten el cuerpo
PlaceableArrayPairResetBody: se mutan sus diez envoltorios por separado.
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
NL = chr(10)
UT = '(reinterpret_cast<uintptr_t>(this) & 3)'
US = '(reinterpret_cast<uintptr_t>(self) & 3)'
EV = G + 'Events/'
TK = G + 'Tasks/TaskTypes/'
PLC = G + 'Entity/Placeable.cpp'
XYO = TK + 'SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp'
FFS = TK + 'TaskComplexFollowPedFootsteps.cpp'
FLF = TK + 'TaskComplexFollowLeaderInFormation.cpp'
EVT = EV + 'Event.cpp'
EVF, EVC = EV + 'EventVehicleOnFire.cpp', EV + 'EventVehicleCollision.cpp'
HWC, INW, STK, COP, GKO, CUD = EV + 'EventHitByWaterCannon.cpp', EV + 'EventInWater.cpp', EV + 'EventStuckInAir.cpp', EV + 'EventCopCarBeingStolen.cpp', EV + 'EventGotKnockedOverByCar.cpp', EV + 'EventCarUpsideDown.cpp'
PWF, FNR, GSW, SPP, ARC, DNG = EV + 'EventPotentialWalkIntoFire.cpp', EV + 'EventFireNearby.cpp', EV + 'EventGunShotWhizzedBy.cpp', EV + 'EventSeenPanickedPed.cpp', EV + 'EventAreaCodes.cpp', EV + 'EventDanger.cpp'
SOURCES = sorted({PLC, XYO, FFS, FLF, EVT, EVF, EVC, HWC, INW, STK, COP, GKO, CUD, PWF, FNR, GSW, SPP, ARC, DNG})
ADDRS = ['54FC40', '54FC90', '54FCE0', '54FD30', '54FD80', '54FDD0', '54FE20', '54FE70', '54FEC0', '54FF10']
EVC_HEAD = 'eEventType CEventVehicleCollision::GetEventType() const {' + NL + '    return '
EVF_LIFE = 'int32 CEventVehicleOnFire::GetLifeTime() {' + NL + '    return '


def arr(addr):
    return '__declspec(noinline) static void __fastcall PlaceableArrayPairReset' + addr + '(void* self, void* /*edx*/) { PlaceableArrayPairResetBody(self); }'


def ret_obj(call):
    return 'return object ? object->Constructor(' + call + ') : nullptr;'


def ret_obj_gate(call):
    return 'return (object && !' + UT + ') ? object->Constructor(' + call + ') : nullptr;'


def round1():
    out = []
    for addr in ADDRS:   # 0..9: tras vaciar, +0xC = 1 si `self` esta desalineado
        out.append((PLC, arr(addr), arr(addr).replace('PlaceableArrayPairResetBody(self); }', 'PlaceableArrayPairResetBody(self); if (' + US + ') *reinterpret_cast<uint32*>(static_cast<uint8*>(self) + 0xC) = 1; }')))
    out.append((XYO, '    AnyMeansXYOffsetDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    AnyMeansXYOffsetDestructorBodyExe(self, nullptr);' + NL + '    if ((flags & 1) && !' + US + ') {'))   # 10
    out.append((XYO, '    *reinterpret_cast<uint32*>(raw + 0x24) = 0x859DC4;', '    *reinterpret_cast<uint32*>(raw + 0x24) = ' + US + ' ? 0x859DC5 : 0x859DC4;'))                                                   # 11
    out.append((FFS, '    FollowPedFootstepsDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    FollowPedFootstepsDestructorBodyExe(self, nullptr);' + NL + '    if ((flags & 1) && !' + US + ') {'))   # 12
    out.append((FLF, '    default:' + NL + '        return nullptr;' + NL + '    }' + NL + '}', '    default:' + NL + '        return ' + UT + ' ? reinterpret_cast<CTask*>(1) : nullptr;' + NL + '    }' + NL + '}'))   # 13
    out.append((EVF, '    return EVENT_VEHICLE_ON_FIRE;', '    return ' + UT + ' ? EVENT_VEHICLE_COLLISION : EVENT_VEHICLE_ON_FIRE;'))                                                         # 14
    out.append((EVF, EVF_LIFE + '0;', EVF_LIFE + UT + ' ? 1 : 0;'))                                                                                                                  # 15
    out.append((EVF, '    return 60.0f;', '    return ' + UT + ' ? 61.0f : 60.0f;'))                                                                                                  # 16
    out.append((EVF, '    return 54;', '    return ' + UT + ' ? 55 : 54;'))                                                                                                          # 17
    out.append((HWC, '    *reinterpret_cast<uint32*>(dst + 0x0) = 0x85B488;', '    *reinterpret_cast<uint32*>(dst + 0x0) = ' + UT + ' ? 0x85B489 : 0x85B488;'))                                  # 18
    out.append((INW, '    *reinterpret_cast<uint32*>(dst + 0x0) = 0x85B4C8;', '    *reinterpret_cast<uint32*>(dst + 0x0) = ' + UT + ' ? 0x85B4C9 : 0x85B4C8;'))                                  # 19
    out.append((STK, ret_obj('m_ped'), ret_obj(UT + ' ? nullptr : m_ped')))                                                                                                        # 20
    out.append((COP, ret_obj('m_hijacker, m_vehicle'), ret_obj('m_hijacker, ' + UT + ' ? nullptr : m_vehicle')))                                                                   # 21
    out.append((GKO, ret_obj('m_vehicle'), ret_obj(UT + ' ? nullptr : m_vehicle')))                                                                                                # 22
    out.append((CUD, ret_obj('m_vehicle'), ret_obj(UT + ' ? nullptr : m_vehicle')))                                                                                                # 23
    out.append((PWF, ret_obj('m_firePos, m_fireSize, m_moveState'), ret_obj('m_firePos, ' + UT + ' ? 0.0f : m_fireSize, m_moveState')))                                            # 24
    out.append((FNR, '    *reinterpret_cast<uint32*>(dst + 0x0)  = 0x85B6E8;', '    *reinterpret_cast<uint32*>(dst + 0x0)  = ' + UT + ' ? 0x85B6E9 : 0x85B6E8;'))                                 # 25
    out.append((GSW, ret_obj('m_firedBy, m_startPoint, m_endPoint, m_bHasNoSound'), ret_obj('m_firedBy, m_startPoint, m_endPoint, ' + UT + ' ? !m_bHasNoSound : m_bHasNoSound')))  # 26
    out.append((SPP, ret_obj('m_ped'), ret_obj(UT + ' ? nullptr : m_ped')))                                                                                                        # 27
    out.append((ARC, ret_obj('m_ped'), ret_obj(UT + ' ? nullptr : m_ped')))                                                                                                        # 28
    out.append((DNG, ret_obj('m_dangerFrom, m_dangerRadius'), ret_obj('m_dangerFrom, ' + UT + ' ? 0.0f : m_dangerRadius')))                                                        # 29
    out.append((EVT, '    if (flags & 1) {' + NL + '        CEvent::operator delete(self);', '    if ((flags & 1) && !' + US + ') {' + NL + '        CEvent::operator delete(self);'))   # 30
    out.append((EVC, EVC_HEAD + 'EVENT_VEHICLE_COLLISION;', EVC_HEAD + UT + ' ? EVENT_VEHICLE_ON_FIRE : EVENT_VEHICLE_COLLISION;'))                                                  # 31
    return out


def round2():
    out = []
    for addr in ADDRS:   # 0..9: con `self` desalineado no se vacia
        out.append((PLC, arr(addr), arr(addr).replace('PlaceableArrayPairResetBody(self); }', 'if (!' + US + ') PlaceableArrayPairResetBody(self); }')))
    out.append((XYO, '    AnyMeansXYOffsetDestructorBodyExe(self, nullptr);', '    if (!' + US + ') AnyMeansXYOffsetDestructorBodyExe(self, nullptr);'))                                                               # 10
    out.append((XYO, '    *reinterpret_cast<uint32*>(raw + 0x00) = 0x870CF0;', '    *reinterpret_cast<uint32*>(raw + 0x00) = ' + US + ' ? 0x870CF1 : 0x870CF0;'))                                                      # 11
    out.append((FFS, '    FollowPedFootstepsDestructorBodyExe(self, nullptr);', '    if (!' + US + ') FollowPedFootstepsDestructorBodyExe(self, nullptr);'))                                                               # 12
    out.append((FLF, '->Constructor(500, false, false, 8.0f)', '->Constructor(' + UT + ' ? 501 : 500, false, false, 8.0f)'))                                                                                # 13
    out.append((EVF, '    return EVENT_VEHICLE_ON_FIRE;', '    return static_cast<eEventType>(EVENT_VEHICLE_ON_FIRE + (' + UT + ' ? 2 : 0));'))                                                       # 14
    out.append((EVF, EVF_LIFE + '0;', EVF_LIFE + UT + ' ? -1 : 0;'))                                                                                                                 # 15
    out.append((EVF, '    return 60.0f;', '    return ' + UT + ' ? 0.0f : 60.0f;'))                                                                                                        # 16
    out.append((EVF, '    return 54;', '    return ' + UT + ' ? 0 : 54;'))                                                                                                                 # 17
    out.append((HWC, '    for (uint32 offset = 0xC; offset < 0x24; offset += sizeof(uint32)) {', '    for (uint32 offset = 0xC; offset < (' + UT + ' ? 0x20u : 0x24u); offset += sizeof(uint32)) {'))             # 18
    out.append((INW, '    *(dst + 0x8)                          = 0;', '    *(dst + 0x8)                          = ' + UT + ' ? 1 : 0;'))                                                        # 19
    out.append((STK, ret_obj('m_ped'), ret_obj_gate('m_ped')))                                                                                                                       # 20
    out.append((COP, ret_obj('m_hijacker, m_vehicle'), ret_obj(UT + ' ? nullptr : m_hijacker, m_vehicle')))                                                                           # 21
    out.append((GKO, ret_obj('m_vehicle'), ret_obj_gate('m_vehicle')))                                                                                                               # 22
    out.append((CUD, ret_obj('m_vehicle'), ret_obj_gate('m_vehicle')))                                                                                                               # 23
    out.append((PWF, ret_obj('m_firePos, m_fireSize, m_moveState'), ret_obj_gate('m_firePos, m_fireSize, m_moveState')))                                                              # 24
    out.append((FNR, '    *(dst + 0xC)                           = 1;', '    *(dst + 0xC)                           = ' + UT + ' ? 2 : 1;'))                                                       # 25
    out.append((GSW, ret_obj('m_firedBy, m_startPoint, m_endPoint, m_bHasNoSound'), ret_obj_gate('m_firedBy, m_startPoint, m_endPoint, m_bHasNoSound')))                              # 26
    out.append((SPP, ret_obj('m_ped'), ret_obj_gate('m_ped')))                                                                                                                       # 27
    out.append((ARC, ret_obj('m_ped'), ret_obj_gate('m_ped')))                                                                                                                       # 28
    out.append((DNG, ret_obj('m_dangerFrom, m_dangerRadius'), ret_obj_gate('m_dangerFrom, m_dangerRadius')))                                                                          # 29
    out.append((EVT, '    *reinterpret_cast<uint32*>(self) = 0x85AB38;' + NL + '    if (flags & 1) {', '    *reinterpret_cast<uint32*>(self) = ' + US + ' ? 0x85AB39 : 0x85AB38;' + NL + '    if (flags & 1) {'))   # 30
    out.append((EVC, EVC_HEAD + 'EVENT_VEHICLE_COLLISION;', EVC_HEAD + UT + ' ? static_cast<eEventType>(2) : EVENT_VEHICLE_COLLISION;'))                                              # 31
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    bad = 0
    for n in (1, 2):
        # las mutaciones de un mismo archivo se aplican en cadena; cada texto original se busca en el archivo bueno y debe ser unico
        for i, (rel, old, new) in enumerate(ROUNDS[n]):
            text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            c = text.count(old)
            if c != 1 or old == new:
                print('ronda', n, 'mutacion', i, rel, 'apariciones', c)
                bad += 1
    print('mutaciones con texto no unico:', bad)
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
        sys.exit(1 if check() else 0)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
