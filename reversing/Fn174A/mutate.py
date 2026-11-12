"""Prueba negativa del banco Fn174A (lote 174A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (20 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: todas las mutaciones se condicionan a que el banco ya este
corriendo (F174ALive::gBenchThread != 0, declarada por una insercion al principio de cada archivo) para que la partida no se caiga ni cambie antes del banco; el banco las alcanza porque corre cada funcion.
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
V = G + 'Entity/Vehicle/'
NL = chr(10)
BENCH = 'F174ALive::gBenchThread'
DECL = 'namespace F174ALive { extern unsigned long gBenchThread; }' + NL

PAD, MENU, HELI, TRUCK, FONT, COMMON, X87, HANDL, NODE, TRAIN = (G + 'Pad.cpp', G + 'MenuManager.cpp', V + 'Heli.cpp', V + 'MonsterTruck.cpp', G + 'Font.cpp', G + 'common.cpp',
                                                                 G + 'X87Math.cpp', G + 'cHandlingDataMgr.cpp', G + 'TrainNode.cpp', V + 'Train.cpp')
SOURCES = sorted({PAD, MENU, HELI, TRUCK, FONT, COMMON, X87, HANDL, NODE, TRAIN})
# insercion de la declaracion tras la primera inclusion de cada archivo
ANCHOR = {f: '#include "StdInc.h"' + NL for f in SOURCES}
ANCHOR[X87] = '#include "X87Math.h"' + NL


def decls():
    return [(f, ANCHOR[f], ANCHOR[f] + DECL) for f in SOURCES]


def round1():
    out = decls()
    # 0..3 CPad::IsMouse*Pressed
    out.append((PAD, 'bool CPad::IsMouseWheelUpPressed() noexcept   { return MOUSE_IS_PRESSED(isMouseWheelMovedUp); }',
                'bool CPad::IsMouseWheelUpPressed() noexcept   { return MOUSE_IS_PRESSED(isMouseWheelMovedUp) || (' + BENCH + ' != 0 && NewMouseControllerState.isMouseWheelMovedUp); }'))
    out.append((PAD, 'bool CPad::IsMouseWheelDownPressed() noexcept { return MOUSE_IS_PRESSED(isMouseWheelMovedDown); }',
                'bool CPad::IsMouseWheelDownPressed() noexcept { return ' + BENCH + ' != 0 ? !MOUSE_IS_PRESSED(isMouseWheelMovedDown) : MOUSE_IS_PRESSED(isMouseWheelMovedDown); }'))
    out.append((PAD, 'bool CPad::IsMouseBmx1Pressed() noexcept      { return MOUSE_IS_PRESSED(isMouseFirstXPressed); }',
                'bool CPad::IsMouseBmx1Pressed() noexcept      { return MOUSE_IS_PRESSED(isMouseFirstXPressed) && ' + BENCH + ' == 0; }'))
    out.append((PAD, 'bool CPad::IsMouseBmx2Pressed() noexcept      { return MOUSE_IS_PRESSED(isMouseSecondXPressed); }',
                'bool CPad::IsMouseBmx2Pressed() noexcept      { return ' + BENCH + ' != 0 ? (OldMouseControllerState.isMouseSecondXPressed && !NewMouseControllerState.isMouseSecondXPressed) : MOUSE_IS_PRESSED(isMouseSecondXPressed); }'))
    # 4 NoDiskInDriveMessage
    out.append((MENU, '        m_bQuitGameNoDVD = true;', '        *reinterpret_cast<uint8*>(&m_bQuitGameNoDVD) = ' + BENCH + ' != 0 ? 2 : 1;'))
    # 5 cuerpo del destructor de CHeli
    out.append((HELI, '    *reinterpret_cast<uint32*>(self) = 0x871680;', '    *reinterpret_cast<uint32*>(self) = ' + BENCH + ' != 0 ? 0x871684 : 0x871680;'))
    # 6 SetupDamageAfterLoad
    out.append((HELI, '    vehicleFlags.bIsDamaged = false;', '    vehicleFlags.bIsDamaged = ' + BENCH + ' != 0;'))
    # 7 destructor con borrado de CHeli
    out.append((HELI, '    CHeliDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    CHeliDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 3) {'))
    # 8 cuerpo del destructor de CMonsterTruck
    out.append((TRUCK, '    *reinterpret_cast<uint32*>(self) = 0x8717D8;', '    *reinterpret_cast<uint32*>(self) = ' + BENCH + ' != 0 ? 0x8717DC : 0x8717D8;'))
    # 9 CFontChar::Set
    out.append((FONT, '    copy8(0x2E);', '    copy8(' + BENCH + ' != 0 ? 0x2F : 0x2E);'))
    # 10 StripTildeTags
    out.append((FONT, '    text[written] = 0;', '    text[written] = ' + BENCH + ' != 0 ? 1 : 0;'))
    # 11 MapExtendedChar
    out.append((FONT, '    if (ch < 0x80) {' + NL + '        return ch;', '    if (ch < 0x80) {' + NL + '        return ch ^ (' + BENCH + ' != 0 ? 1 : 0);'))
    # 12 TransformPointsSimple
    out.append((COMMON, '    CommonTransformPointsSimpleCpp(&pointsOut->x, numPoints, cosHeading, sinHeading, &transform.m_vPosn.x, &pointsIn->x);',
                '    CommonTransformPointsSimpleCpp(&pointsOut->x, numPoints > 1 && ' + BENCH + ' != 0 ? numPoints - 1 : numPoints, cosHeading, sinHeading, &transform.m_vPosn.x, &pointsIn->x);'))
    # 13 TransformVectors (CMatrix)
    out.append((COMMON, '    RwMatrixUpdate(CGame::m_pWorkingMatrix1);' + NL + NL + '    for (auto remaining = std::bit_cast<uint32>(numVectors);',
                '    if (' + BENCH + ' == 0) {' + NL + '        RwMatrixUpdate(CGame::m_pWorkingMatrix1);' + NL + '    }' + NL + NL + '    for (auto remaining = std::bit_cast<uint32>(numVectors);'))
    # 14 DoesInfiniteLineCrossFiniteLine
    out.append((COMMON, '    return CommonLineCrossesSegmentCpp(finiteA.x, finiteA.y, finiteB.x, finiteB.y, origin.x, origin.y, dir.x, dir.y);',
                '    return CommonLineCrossesSegmentCpp(finiteA.x, finiteA.y, finiteB.x, finiteB.y, ' + BENCH + ' != 0 ? origin.y : origin.x, origin.y, dir.x, dir.y);'))
    # 15 constructor de tHandlingData
    out.append((HANDL, '    TransmissionConstructorBody(&self->m_transmissionData, nullptr);' + NL + '    return self;', '    TransmissionConstructorBody(&self->m_transmissionData, nullptr);' + NL + '    return ' + BENCH + ' != 0 ? nullptr : self;'))
    # 16 CTrainNode::GetPosn
    out.append((NODE, '    return { GetX(), GetY(), GetZ() };', '    return { GetX(), GetY(), ' + BENCH + ' != 0 ? GetZ() + 1.0f : GetZ() };'))
    # 17 cuerpo del destructor de CTrain
    out.append((TRAIN, '    *reinterpret_cast<uint32*>(self) = 0x872370;', '    *reinterpret_cast<uint32*>(self) = ' + BENCH + ' != 0 ? 0x872374 : 0x872370;'))
    # 18 CTrain::SetModelIndex
    out.append((TRAIN, '    CVehicle::SetModelIndex(index);' + NL + '    SetupModelNodes();', '    CVehicle::SetModelIndex(' + BENCH + ' != 0 ? index + 1 : index);' + NL + '    SetupModelNodes();'))
    # 19 CTrain::PreRender
    out.append((TRAIN, '    if (trainFlags.bIsFrontCarriage) {', '    if (trainFlags.bIsFrontCarriage || (' + BENCH + ' != 0 && trainFlags.bIsLastCarriage)) {'))
    return out


def round2():
    out = decls()
    out.append((PAD, 'bool CPad::IsMouseWheelUpPressed() noexcept   { return MOUSE_IS_PRESSED(isMouseWheelMovedUp); }',
                'bool CPad::IsMouseWheelUpPressed() noexcept   { return NewMouseControllerState.isMouseWheelMovedUp && OldMouseControllerState.isMouseWheelMovedUp != 1; }'))
    out.append((PAD, 'bool CPad::IsMouseWheelDownPressed() noexcept { return MOUSE_IS_PRESSED(isMouseWheelMovedDown); }',
                'bool CPad::IsMouseWheelDownPressed() noexcept { return NewMouseControllerState.isMouseWheelMovedDown == 1 && !OldMouseControllerState.isMouseWheelMovedDown; }'))
    out.append((PAD, 'bool CPad::IsMouseBmx1Pressed() noexcept      { return MOUSE_IS_PRESSED(isMouseFirstXPressed); }',
                'bool CPad::IsMouseBmx1Pressed() noexcept      { return MOUSE_IS_PRESSED(isMouseFirstXPressed) && !(' + BENCH + ' != 0 && *reinterpret_cast<uint8*>(0xB7341D) == 0xFF); }'))
    out.append((PAD, 'bool CPad::IsMouseBmx2Pressed() noexcept      { return MOUSE_IS_PRESSED(isMouseSecondXPressed); }',
                'bool CPad::IsMouseBmx2Pressed() noexcept      { return NewMouseControllerState.isMouseSecondXPressed && !(*reinterpret_cast<uint8*>(0xB73432) & 1); }'))
    out.append((MENU, '        RsEventHandler(rsQUITAPP, NULL);', '        RsEventHandler(rsQUITAPP, ' + BENCH + ' != 0 ? reinterpret_cast<void*>(1) : NULL);'))
    out.append((HELI, '    self->m_vehicleAudio.Terminate();' + NL + '    static_cast<CAutomobile*>(self)->Destructor();', '    if (' + BENCH + ' == 0) {' + NL + '        self->m_vehicleAudio.Terminate();' + NL + '    }' + NL + '    static_cast<CAutomobile*>(self)->Destructor();'))
    out.append((HELI, '    vehicleFlags.bIsDamaged = false;', '    if (' + BENCH + ' == 0) {' + NL + '        vehicleFlags.bIsDamaged = false;' + NL + '    }'))
    out.append((HELI, '    CHeliDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    CHeliDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 0x80) {' + NL + '        CHeliDestructorBodyExe(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))
    out.append((TRUCK, '    static_cast<CAutomobile*>(self)->Destructor();' + NL + '}', '    if (' + BENCH + ' == 0) {' + NL + '        static_cast<CAutomobile*>(self)->Destructor();' + NL + '    }' + NL + '}'))
    out.append((FONT, '    for (const size_t offset : {0x00u, 0x04u, 0x08u, 0x0Cu, 0x10u}) {', '    for (const size_t offset : {0x00u, 0x04u, 0x08u, 0x0Cu, ' + BENCH + ' != 0 ? 0x0Cu : 0x10u}) {'))
    out.append((FONT, '                text[written++] = ch;', '                text[written++] = (' + BENCH + ' != 0 && ch >= 0xF0) ? static_cast<GxtChar>(ch - 1) : ch;'))
    out.append((FONT, '    return *reinterpret_cast<const uint8*>(0x8D5AD8 + ch * 2);', '    return *reinterpret_cast<const uint8*>(0x8D5AD8 + ch * 2 + (' + BENCH + ' != 0 ? 1 : 0));'))
    out.append((X87, '    } while (--remaining != 0);', '    } while (--remaining > (' + BENCH + ' != 0 ? 1u : 0u));'))
    out.append((COMMON, '        RwV3dTransformVector(vecsOut++, vecsIn++, CGame::m_pWorkingMatrix1);', '        RwV3dTransformVector(vecsOut++, vecsIn, CGame::m_pWorkingMatrix1);' + NL + '        vecsIn += ' + BENCH + ' != 0 ? 0 : 1;'))
    out.append((X87, '    return a * b < 0.0;', '    return a * b < (' + BENCH + ' != 0 ? 1e-30 : 0.0);'))
    out.append((HANDL, '    TransmissionConstructorBody(&self->m_transmissionData, nullptr);', '    TransmissionConstructorBody(reinterpret_cast<cTransmission*>(reinterpret_cast<uint8*>(&self->m_transmissionData) + (' + BENCH + ' != 0 ? 4 : 0)), nullptr);'))
    out.append((NODE, '    return { GetX(), GetY(), GetZ() };', '    return { GetX(), ' + BENCH + ' != 0 ? GetY() * 2.0f : GetY(), GetZ() };'))
    out.append((TRAIN, '    self->m_vehicleAudio.Terminate();' + NL + '    CTrainBaseDestructorCall(self, nullptr);', '    self->m_vehicleAudio.Terminate();' + NL + '    if (' + BENCH + ' == 0) {' + NL + '        CTrainBaseDestructorCall(self, nullptr);' + NL + '    }'))
    out.append((TRAIN, '    CVehicle::SetModelIndex(index);' + NL + '    SetupModelNodes();', '    CVehicle::SetModelIndex(index);' + NL + '    if (' + BENCH + ' == 0) {' + NL + '        SetupModelNodes();' + NL + '    }'))
    out.append((TRAIN, 'VEHICLE_LIGHTS_TWIN | VEHICLE_LIGHTS_IGNORE_DAMAGE | VEHICLE_LIGHTS_DISABLE_FRONT));', 'VEHICLE_LIGHTS_TWIN | VEHICLE_LIGHTS_IGNORE_DAMAGE | VEHICLE_LIGHTS_DISABLE_FRONT | (' + BENCH + ' != 0 ? VEHICLE_LIGHTS_DISABLE_REAR : 0)));'))
    return out


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
        # varias mutaciones en el mismo archivo se aplican en cadena: la siguiente lee el texto ya mutado
    print('ronda', n, 'aplicada:', len(ROUNDS[n]), 'mutaciones')


def check():
    """comprueba (sin tocar nada) que cada texto original de cada ronda aparece exactamente una vez en su archivo (tras aplicar las anteriores del mismo archivo)"""
    for n, muts in ROUNDS.items():
        texts = {}
        for rel, old, new in muts:
            norm = texts.setdefault(rel, (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n'))
            if norm.count(old) != 1:
                raise SystemExit('Ronda %d: texto original no unico en %s: %r (%d)' % (n, rel, old[:80], norm.count(old)))
            texts[rel] = norm.replace(old, new)
    print('ok: cada texto original aparece exactamente una vez en las dos rondas')


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
