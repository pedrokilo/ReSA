"""Prueba negativa del banco Fn122A (lote 122A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
DEBUG = G + 'Debug.cpp'
BUOY = G + 'Buoyancy.cpp'
CIV = G + 'Entity/Ped/CivilianPed.cpp'
DRV = T + 'TaskComplexEnterCarAsDriver.cpp'
PAS = T + 'TaskComplexEnterCarAsPassenger.cpp'
FAC = T + 'TaskComplexFacial.cpp'
DEAL = T + 'TaskComplexPartnerDeal.cpp'
ACH = T + 'TaskSimpleAchieveHeading.cpp'
GOTO = T + 'TaskSimpleGoTo.cpp'
STAY = T + 'TaskSimpleSetStayInSamePlace.cpp'
SOURCES = [DEBUG, BUOY, CIV, DRV, PAS, FAC, DEAL, ACH, GOTO, STAY]
SELF = 'reinterpret_cast<uintptr_t>(self)'
# Los cuerpos de destructor los llama el EXE con objetos reales por los funclets de limpieza (SEH) y por los destructores con borrado; las mutaciones solo difieren con valores del banco (this desalineado o en
# posiciones de arena concretas) o en la vtable que escriben: los objetos del juego estan alineados a 4 y no pasan por estos cuerpos durante el banco.
ROUNDS = {
    1: [
        # DebugInitTextBuffer: pone a cero cuatro bytes en lugar de dos (cambia los vecinos de 0xB7150A)
        (DEBUG, '    StaticRef<uint16>(0xB71508) = 0;', '    StaticRef<uint32>(0xB71508) = 0;'),
        # CCivilianPed: vtable + 1
        (CIV, '    *reinterpret_cast<uint32*>(self) = 0x86C0A8;', '    *reinterpret_cast<uint32*>(self) = 0x86C0A9;'),
        # cBuoyancy: el puntero de matriz adjunta solo se pone a cero en 16 bits
        (BUOY, '    *reinterpret_cast<uint32*>(&m_EntityMatrix.m_pAttachMatrix) = 0;', '    *reinterpret_cast<uint16*>(&m_EntityMatrix.m_pAttachMatrix) = 0;'),
        # EnterCarAsDriver: vtable de otra ranura
        (DRV, '    *reinterpret_cast<uint32*>(self) = 0x86EAAC;', '    *reinterpret_cast<uint32*>(self) = 0x86EAB0;'),
        # EnterCarAsPassenger: vtable - 4
        (PAS, '    *reinterpret_cast<uint32*>(self) = 0x86EADC;', '    *reinterpret_cast<uint32*>(self) = 0x86EAD8;'),
        # CTaskComplexFacial: vtable con otro nibble
        (FAC, '    *reinterpret_cast<uint32*>(self) = 0x870ADC;', '    *reinterpret_cast<uint32*>(self) = 0x870AEC;'),
        # CTaskComplexPartnerDeal: vtable + 1
        (DEAL, '    *reinterpret_cast<uint32*>(self) = 0x870754;', '    *reinterpret_cast<uint32*>(self) = 0x870755;'),
        # CTaskSimpleAchieveHeading: vtable + 8
        (ACH, '    *reinterpret_cast<uint32*>(self) = 0x86FD74;', '    *reinterpret_cast<uint32*>(self) = 0x86FD7C;'),
        # CTaskSimpleGoTo (cuerpo compartido por 0x667D50 y 0x667A00): vtable - 4
        (GOTO, '    *reinterpret_cast<uint32*>(self) = 0x86FD1C;', '    *reinterpret_cast<uint32*>(self) = 0x86FD18;'),
        # CTaskSimpleSetStayInSamePlace: vtable + 8
        (STAY, '    *reinterpret_cast<uint32*>(self) = 0x86DD50;', '    *reinterpret_cast<uint32*>(self) = 0x86DD58;'),
    ],
    2: [
        # DebugInitTextBuffer: no escribe si la palabra ya vale 0xFFFF
        (DEBUG, '    StaticRef<uint16>(0xB71508) = 0;', '    if (StaticRef<uint16>(0xB71508) != 0xFFFF) StaticRef<uint16>(0xB71508) = 0;'),
        # CCivilianPed: no salta a CPed::Destructor con this == 1 (mod 4)
        (CIV, '    self->CPed::Destructor();', '    if ((%s & 3) != 1) self->CPed::Destructor();' % SELF),
        # cBuoyancy: el puntero de matriz adjunta pasa a 1 con this == 4 (mod 16) (la ronda 1 lo ponia a cero en 16 bits: mutante equivalente, la inicializacion de CMatrix ya lo deja en cero)
        (BUOY, '    *reinterpret_cast<uint32*>(&m_EntityMatrix.m_pAttachMatrix) = 0;', '    *reinterpret_cast<uint32*>(&m_EntityMatrix.m_pAttachMatrix) = (reinterpret_cast<uintptr_t>(this) & 0xF) == 4 ? 1 : 0;'),
        # cBuoyancy: la bandera de propiedad se pone a cero solo en un byte
        (BUOY, '    *reinterpret_cast<uint32*>(&m_EntityMatrix.m_bOwnsAttachedMatrix) = 0;', '    *reinterpret_cast<uint8*>(&m_EntityMatrix.m_bOwnsAttachedMatrix) = 0;'),
        # EnterCarAsDriver: no salta al destructor base con this == 4 (mod 8)
        (DRV, '    self->CTaskComplexEnterCar::Destructor();', '    if ((%s & 7) != 4) self->CTaskComplexEnterCar::Destructor();' % SELF),
        # EnterCarAsPassenger: escribe solo 16 bits de la vtable
        (PAS, '    *reinterpret_cast<uint32*>(self) = 0x86EADC;', '    *reinterpret_cast<uint16*>(self) = 0xEADC;'),
        # CTaskComplexFacial: llama primero al destructor base y escribe despues la vtable (el colaborador ve la vtable previa)
        (FAC, '    *reinterpret_cast<uint32*>(self) = 0x870ADC;\n    CTaskComplexBody::Destructor(self, nullptr);', '    CTaskComplexBody::Destructor(self, nullptr);\n    *reinterpret_cast<uint32*>(self) = 0x870ADC;'),
        # CTaskComplexPartnerDeal: no llama a 0x683A40 con this == 2 (mod 4)
        (DEAL, '    plugin::CallMethod<0x683A40, CTaskComplexPartnerDeal*>(self);', '    if ((%s & 3) != 2) plugin::CallMethod<0x683A40, CTaskComplexPartnerDeal*>(self);' % SELF),
        # CTaskSimpleAchieveHeading: orden invertido
        (ACH, '    *reinterpret_cast<uint32*>(self) = 0x86FD74;\n    self->CTask::Destructor();', '    self->CTask::Destructor();\n    *reinterpret_cast<uint32*>(self) = 0x86FD74;'),
        # CTaskSimpleGoTo: no salta con this == 0xC (mod 0x10)
        (GOTO, '    self->CTask::Destructor();', '    if ((%s & 0xF) != 0xC) self->CTask::Destructor();' % SELF),
        # CTaskSimpleSetStayInSamePlace: escribe solo 16 bits
        (STAY, '    *reinterpret_cast<uint32*>(self) = 0x86DD50;', '    *reinterpret_cast<uint16*>(self) = 0xDD50;'),
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
