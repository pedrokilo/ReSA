"""Prueba negativa del banco Fn141A (lote 141A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (24 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global).
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'
TK = G + 'Tasks/TaskTypes/'
DN = G + 'Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp'
DUM = G + 'Entity/Dummy/Dummy.cpp'
WPN = G + 'WeaponInfo.cpp'
OBC = TK + 'TaskComplexObserveTrafficLightsAndAchieveHeading.cpp'
OBH = TK + 'TaskComplexObserveTrafficLightsAndAchieveHeading.h'
QUAD = G + 'Entity/Vehicle/QuadBike.cpp'
TRL = G + 'Entity/Vehicle/Trailer.cpp'
ALL = G + 'Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp'
WAIT = TK + 'TaskComplexAttractorPartnerWait.cpp'
UAP = TK + 'TaskComplexUseAttractorPartner.cpp'
DTP = TK + 'TaskComplexDriveToPoint.cpp'
DPR = TK + 'TaskComplexDrivePointRoute.cpp'
EAC = TK + 'TaskComplexEnterAnyCarAsDriver.cpp'
SHS = TK + 'TaskComplexShuffleSeats.cpp'
GSS = TK + 'TaskComplexGoToPointAndStandStill.cpp'
GPA = TK + 'TaskComplexGoToPointAiming.cpp'
RUN = TK + 'TaskComplexUseEffectRunning.cpp'
SPR = TK + 'TaskComplexUseEffectSprinting.cpp'
SOURCES = [DN, DUM, WPN, OBC, OBH, QUAD, TRL, ALL, WAIT, UAP, DTP, DPR, EAC, SHS, GSS, GPA, RUN, SPR]
NL = chr(10)


def body_pair(vt, call):
    """cuerpo de destructor: vtable + llamada a la base. r1: vtable con un bit distinto; r2: llamada a la base ANTES de escribir la vtable"""
    w = '    *reinterpret_cast<uint32*>(self) = 0x%X;'
    old = (w % vt) + NL + '    ' + call
    return (old, (w % (vt + 1)) + NL + '    ' + call), (old, '    ' + call + NL + (w % vt))


def scalar_pair(first_line):
    """destructor con borrado: r1: se borra con otra condicion; r2: no se llama al cuerpo"""
    cond = '    if (flags & 1) {'
    return ((first_line + NL + cond, first_line + NL + '    if (flags & 3) {'), (first_line + NL + cond, cond))


MUT = {   # fn -> (archivo, mutacion ronda 1, mutacion ronda 2)
    0: (DN, ('s_Magic1 = (s_Magic1 + 1u) & 0xFu;', 's_Magic1 = (s_Magic1 + 1u) & 0x1Fu;'), ('    s_Magic2 = flag;', '    s_Magic2 = flag & 0xFFu;')),
    1: (DN, ('    return *reinterpret_cast<RwRGBA* const*>(reinterpret_cast<uintptr_t>(geometry) + uintptr_t(ms_extraVertColourPluginOffset));',
             '    return reinterpret_cast<RwRGBA*>(reinterpret_cast<uintptr_t>(geometry) + uintptr_t(ms_extraVertColourPluginOffset));'),
            ('    return *reinterpret_cast<RwRGBA* const*>(reinterpret_cast<uintptr_t>(geometry) + uintptr_t(ms_extraVertColourPluginOffset));',
             '    return *reinterpret_cast<RwRGBA* const*>(reinterpret_cast<uintptr_t>(geometry) + uintptr_t(ms_extraVertColourPluginOffset) + 4u);')),
    2: (DUM, ('*flags = uint8((*flags & 0xFDu) | 5u);', '*flags = uint8((*flags & 0xFDu) | 7u);'),
        ('*flags = uint8((*flags & 0xFDu) | 5u);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x8638C0;', '*flags = uint8((*flags & 0xFDu) | 5u);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x8638C1;')),
    3: (DUM,) + body_pair(0x8638C0, 'static_cast<CEntity*>(self)->Destructor();'),
    4: (WPN, ('    return isSet2 ? m_fAnimLoop2Start : m_fAnimLoopStart;', '    return isSet2 ? m_fAnimLoopStart : m_fAnimLoop2Start;'),
        ('    return isSet2 ? m_fAnimLoop2Start : m_fAnimLoopStart;', '    return m_fAnimLoopStart;')),
    5: (WPN, ('    return isSet2 ? m_fAnimLoop2End : m_fAnimLoopEnd;', '    return isSet2 ? m_fAnimLoopEnd : m_fAnimLoop2End;'),
        ('    return isSet2 ? m_fAnimLoop2End : m_fAnimLoopEnd;', '    return isSet2 ? m_fAnimLoop2End : m_fAnimLoopStart;')),
    6: (OBC, ('*reinterpret_cast<uint32*>(bytes + 0xC) = durationInMs;', '*reinterpret_cast<uint32*>(bytes + 0xC) = durationInMs + 1;'),
        ('*reinterpret_cast<uint32*>(bytes + 0x10) = headingBits;', '*reinterpret_cast<uint32*>(bytes + 0x10) = headingBits ^ 1u;')),
    7: (OBH, ('eTaskType GetTaskType() const override { return Type; } // 0x631940', 'eTaskType GetTaskType() const override { return TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS; } // 0x631940'),
        ('eTaskType GetTaskType() const override { return Type; } // 0x631940', 'eTaskType GetTaskType() const override { return static_cast<eTaskType>(Type + 1); } // 0x631940')),
    8: (QUAD, ('    if (flags & 1) {' + NL + '        CVehicle::operator delete(this);', '    if (!(flags & 1)) {' + NL + '        CVehicle::operator delete(this);'),
        ('        CVehicle::operator delete(this);' + NL + '    }' + NL + '    return this;', '        CVehicle::operator delete(this);' + NL + '    }' + NL + '    return nullptr;')),
    9: (TRL,) + body_pair(0x871C28, 'static_cast<CAutomobile*>(self)->Destructor();'),
    10: (ALL, ('    if (flags & 1) {' + NL + '        CTaskAllocator::operator delete(this);', '    if (flags & 2) {' + NL + '        CTaskAllocator::operator delete(this);'),
         ('        CTaskAllocator::operator delete(this);' + NL + '    }' + NL + '    return this;', '        CTaskAllocator::operator delete(this);' + NL + '    }' + NL + '    return reinterpret_cast<CTaskAllocatorKillThreatsBasicRandomGroup*>(uintptr_t(this) + 4);')),
    11: (ALL,) + body_pair(0x86C80C, 'static_cast<CTaskAllocatorKillThreatsBasic*>(self)->Destructor();'),
    12: (WAIT,) + body_pair(0x86E284, 'CTaskComplexBody::Destructor(self, nullptr);'),
    13: (UAP,) + scalar_pair('    Destructor();'),
    14: (DTP,) + body_pair(0x86E9DC, 'static_cast<CTaskComplexCarDrive*>(self)->Destructor();'),
    15: (DPR,) + scalar_pair('    Destructor();'),
    16: (EAC,) + body_pair(0x86EA14, 'CTaskComplexBody::Destructor(self, nullptr);'),
    17: (SHS,) + scalar_pair('    Destructor();'),
    18: (GSS,) + body_pair(0x86FDA8, 'CTaskComplexBody::Destructor(self, nullptr);'),
    19: (GPA,) + scalar_pair('    Destructor();'),
    20: (RUN,) + scalar_pair('    UseEffectRunningDestructorBody(this, nullptr);'),
    21: (RUN,) + body_pair(0x86E594, 'static_cast<CTaskComplexUseEffect*>(self)->Destructor();'),
    22: (SPR,) + scalar_pair('    UseEffectSprintingDestructorBody(this, nullptr);'),
    23: (SPR,) + body_pair(0x86E5C0, 'static_cast<CTaskComplexUseEffect*>(self)->Destructor();'),
}
ROUNDS = {n: [(MUT[fn][0],) + MUT[fn][n] for fn in range(24)] for n in (1, 2)}


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
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
