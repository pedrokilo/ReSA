"""Prueba negativa del banco Fn205A (lote 205A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Hay una mutacion por funcion de la DLL y ronda (las tres copias identicas de copy_backward y de fill_n
de 4 bytes comparten funcion de la DLL): cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco:
las mutaciones se condicionan a un puntero desalineado (el juego no lo produce), para que la partida no se caiga antes del banco.
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
E = G + 'Events/Event'
T = G + 'Tasks/TaskTypes/'
NL = chr(10)


def ua(x):
    return '(reinterpret_cast<uintptr_t>(' + x + ') & 3)'


EVENTS = [
    (E + 'SexyVehicle.cpp', 'SexyVehicleDestructorBody(self, nullptr);'),
    (E + 'PedToChase.cpp', 'PedToChaseDestructorBody(self, nullptr);'),
    (E + 'PedToFlee.cpp', 'PedToFleeDestructorBody(self, nullptr);'),
    (E + 'Attractor.cpp', 'AttractorDestructorBody(self, nullptr);'),
    (E + 'VehicleToSteal.cpp', 'VehicleToStealDestructorBody(self, nullptr);'),
    (E + 'AcquaintancePed.cpp', 'AcquaintancePedDestructorBody(self, nullptr);'),
    (E + 'VehicleThreat.cpp', 'VehicleThreatDestructorBody(self, nullptr);'),
    (E + 'KnockOffBike.cpp', 'KnockOffBikeDestructorBody(self, nullptr);'),
    (E + 'GunAimedAt.cpp', 'GunAimedAtDestructorBody(self, nullptr);'),
    (E + 'ScriptCommand.cpp', 'CEventScriptCommand::Destructor(self, 0);'),
]
AE, WG, AO, SH, PA, PM = T + 'TaskComplexAvoidEntity.cpp', T + 'TaskComplexWanderGang.cpp', T + 'TaskComplexAvoidOtherPedWhileWandering.cpp', G + 'Attractors/PedShelterAttractor.cpp', G + 'Attractors/PedAttractor.cpp', G + 'PedAttractorManager.cpp'
SOURCES = sorted({f for f, _ in EVENTS} | {AE, WG, AO, SH, PA, PM})
IF = '    if (flags & 1) {'


def sdd(self_name, file, call, r):
    if r == 1:
        return (file, call + NL + IF, call + NL + '    if (flags & (' + ua(self_name) + ' ? 2 : 1)) {')
    return (file, call + NL + IF, '    if (' + ua(self_name) + ' != 2) {' + NL + '    ' + call + NL + '    }' + NL + IF)


def build(r):
    out = []
    for f, call in EVENTS:                                                                                                                            # 0..9
        out.append(sdd('self', f, '    ' + call, r))
    out.append(sdd('this', WG, '    WanderGangDestructorBodyExe(this, nullptr);', r))                                                                   # 10
    if r == 1:
        out.append((WG, '    *reinterpret_cast<uint32*>(self) = 0x86FE84;', '    *reinterpret_cast<uint32*>(self) = ' + ua('self') + ' ? 0x86FE88 : 0x86FE84;'))                # 11
    else:
        out.append((WG, '    CTaskComplexBody::Destructor(self, nullptr);', '    if (' + ua('self') + ' != 2) {' + NL + '        CTaskComplexBody::Destructor(self, nullptr);' + NL + '    }'))       # 11
    out.append(sdd('this', AO, '    Destructor();', r))                                                                                                 # 12
    out.append(sdd('self', AE, '    reinterpret_cast<void(__fastcall*)(CTaskComplex*, void*)>(0x66AAE0)(self, nullptr);', r))                            # 13
    if r == 1:
        out.append((AE, '    const float inv = ONE / static_cast<float>(count);', '    const float inv = ONE / static_cast<float>(count) + (' + ua('peds') + ' ? 1.0f : 0.0f);'))            # 14
        out.append((AE, '    std::memcpy(&sphere.m_vecCenter.z, &ped->GetPosition().z, sizeof(float));', '    std::memcpy(&sphere.m_vecCenter.z, &ped->GetPosition().z, sizeof(float));' + NL + '    sphere.m_vecCenter.z += (' + ua('self') + ' ? 1.0f : 0.0f);'))                  # 15
    else:
        out.append((AE, '    out->Set(std::sqrt(maxDistSq) + RADIUS_PAD, center, static_cast<eSurfaceType>(0), 0, tColLighting{ 0xFF });',
                    '    if (' + ua('peds') + ' != 2) {' + NL + '        out->Set(std::sqrt(maxDistSq) + RADIUS_PAD, center, static_cast<eSurfaceType>(0), 0, tColLighting{ 0xFF });' + NL + '    }'))   # 14
        out.append((AE, '    peds[0] = FieldAt<CPed*>(self, 0x0C);', '    peds[0] = ' + ua('self') + ' == 2 ? nullptr : FieldAt<CPed*>(self, 0x0C);'))                            # 15
    old16 = 'bool CPedShelterAttractor::IsAtHeadOfQueue(CPed* /*ped*/) {' + NL + '    return true;'
    out.append((SH, old16, 'bool CPedShelterAttractor::IsAtHeadOfQueue(CPed* /*ped*/) {' + NL + ('    return !' + ua('this') + ';' if r == 1 else '    return ' + ua('this') + ' != 2;')))   # 16
    pool = '    PoolFreeSlotRaw(GetPedAttractorPool(), object, 0xC4);'
    if r == 1:
        out.append((PA, pool, '    PoolFreeSlotRaw(GetPedAttractorPool(), object, ' + ua('object') + ' ? 0xC8 : 0xC4);'))                                         # 17
    else:
        out.append((PA, pool, '    if (' + ua('object') + ' != 2) {' + NL + '    ' + pool + NL + '    }'))                                                          # 17
    oldcb = '    *out = destEnd;' + NL + '    return out;' + NL + '}' + NL + NL + '// Envoltorios de copy_backward'
    if r == 1:
        out.append((PA, oldcb, '    *out = ' + ua('first') + ' ? destEnd + 1 : destEnd;' + NL + '    return out;' + NL + '}' + NL + NL + '// Envoltorios de copy_backward'))   # 18, 20, 23
    else:
        out.append((PA, oldcb, '    if (' + ua('first') + ' != 2) {' + NL + '        *out = destEnd;' + NL + '    }' + NL + '    return out;' + NL + '}' + NL + NL + '// Envoltorios de copy_backward'))
    w20 = 'SArrayElem20** __cdecl SArrayCopyBackwardStride20Wrapper(SArrayElem20** out, SArrayElem20* first, SArrayElem20* last, SArrayElem20* destEnd) {'
    w12 = 'SArrayElem12** __cdecl SArrayCopyBackwardStride12Wrapper(SArrayElem12** out, SArrayElem12* first, SArrayElem12* last, SArrayElem12* destEnd) {'
    f12 = 'void __cdecl SArrayFillStride12(SArrayElem12* first, SArrayElem12* last, const SArrayElem12* value) {'
    n4 = 'void __cdecl SArrayFillNStride4(uint32* dest, uint32 count, const uint32* value) {'
    n20 = 'void __cdecl SArrayFillNStride20(SArrayElem20* dest, uint32 count, const SArrayElem20* value) {'
    n12 = 'void __cdecl SArrayFillNStride12(SArrayElem12* dest, uint32 count, const SArrayElem12* value) {'

    def pre(sig, cond, action):
        return (PA, sig, sig + NL + '    if (' + cond + ') {' + NL + '        ' + action + NL + '    }')
    if r == 1:
        out.append(pre(w20, ua('first'), 'return nullptr;'))                                                                                              # 19
        out.append(pre(f12, ua('first'), 'return;'))                                                                                                     # 21
        out.append(pre(w12, ua('first'), 'return nullptr;'))                                                                                              # 22
        out.append(pre(n4, ua('dest'), 'return;'))                                                                                                       # 24, 26, 28
        out.append(pre(n20, ua('dest'), 'return;'))                                                                                                      # 25
        out.append(pre(n12, ua('dest'), 'return;'))                                                                                                      # 27
    else:
        out.append(pre(w20, ua('last') + ' == 2', 'return nullptr;'))
        out.append((PA, '        first->w[2] = value->w[2];', '        first->w[2] = value->w[2] + (' + ua('first') + ' == 1 ? 1u : 0u);'))
        out.append(pre(w12, ua('last') + ' == 2', 'return nullptr;'))
        out.append((PA, n4 + NL + '    if (count == 0) {', n4 + NL + '    if (count == 0 || ' + ua('dest') + ' == 2) {'))
        out.append((PA, n20 + NL + '    if (count == 0) {', n20 + NL + '    if (count == 0 || ' + ua('dest') + ' == 2) {'))
        out.append((PA, n12 + NL + '    if (count == 0) {', n12 + NL + '    if (count == 0 || ' + ua('dest') + ' == 2) {'))
    if r == 1:
        out.append((PM, '            attractor->AbortPedTasks();', '            if (' + ua('fx') + ' == 0) attractor->AbortPedTasks();'))                                    # 29
        out.append((PM, '        if ((*cursor)->IsRegisteredWithPed(ped)) {', '        if ((*cursor)->IsRegisteredWithPed(ped) && !' + ua('ped') + ') {'))                           # 30
        out.append((PM, '            if (attractor->IsRegisteredWithPed(ped)) {', '            if (attractor->IsRegisteredWithPed(ped) && !' + ua('ped') + ') {'))                     # 31
    else:
        out.append((PM, '        if (attractor->m_Fx == fx) {', '        if (attractor->m_Fx == fx && ' + ua('fx') + ' != 2) {'))                                         # 29
        out.append((PM, '        if ((*cursor)->IsRegisteredWithPed(ped)) {', '        if ((*cursor)->IsRegisteredWithPed(ped) || (' + ua('ped') + ' == 2 && i == 0)) {'))           # 30
        out.append((PM, '        if (attractor->m_Fx == fx && attractor->GetEntity() == entity) {', '        if (attractor->m_Fx == fx && attractor->GetEntity() == entity && ' + ua('entity') + ' != 2) {'))   # 31
    return out


ROUNDS = {1: build(1), 2: build(2)}
SOURCES = sorted({rel for n in ROUNDS for rel, _, _ in ROUNDS[n]})


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
