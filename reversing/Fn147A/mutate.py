"""Prueba negativa del banco Fn147A (lote 147A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (26 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` desalineado o a valores de entrada que el juego no usa, para que la partida no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
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

# destructores con borrado de tareas: (archivo, nombre del cuerpo, nombre del tipo)
SDD = [
    (T + 'TaskComplexGoToPointShooting.cpp', 'GoToPointShooting', 'CTaskComplexGoToPointShooting'),
    (T + 'TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp', 'CTaskComplexGoToPointAndStandStillAndAchieveHeading', None),
    (T + 'TaskComplexBeInGroup.cpp', 'CTaskComplexBeInGroup', None),
    (T + 'TaskSimpleWaitUntilAreaCodesMatch.cpp', 'CTaskSimpleWaitUntilAreaCodesMatch', None),
    (T + 'TaskComplexUseScriptedBrain.cpp', 'CTaskComplexUseScriptedBrain', None),
    (T + 'TaskComplexDiveFromAttachedEntityAndGetUp.cpp', 'CTaskComplexDiveFromAttachedEntityAndGetUp', None),
    (T + 'TaskComplexWalkRoundFire.cpp', 'CTaskComplexWalkRoundFire', None),
    (T + 'TaskSimpleScratchHead.cpp', 'ScratchHead', 'CTaskSimpleScratchHead'),
    (T + 'TaskSimpleUseAtm.cpp', 'CTaskSimpleUseAtm', None),
]
SOURCES = sorted({s[0] for s in SDD} | {
    G + 'Events/EventGroup.cpp', G + 'Events/EventGlobalGroup.cpp', G + 'Attractors/PedShelterAttractor.cpp', T + 'TaskSimpleFinishBrain.cpp',
    T + 'TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp', G + 'Tasks/TaskComplexSequence.cpp', G + 'Tasks/TaskComplexUseSequence.cpp', G + 'Fx/FxSystem.cpp', G + 'Fx/FxPrtMult.cpp',
    T + 'SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp', T + 'TaskSimpleRunAnim.cpp'})


def body_call(name, tp):
    return '%sDestructorBody(self, nullptr);' % name


def round1():
    out = []
    EG, EGG = G + 'Events/EventGroup.cpp', G + 'Events/EventGlobalGroup.cpp'
    # 0 CEventGroup::Destructor: Flush con argumento verdadero con this desalineado
    out.append((EG, 'self->Flush(false);' + NL + '}' + NL + NL + '// 0x4ABA20', 'self->Flush(' + UNAL + ' != 0);' + NL + '}' + NL + NL + '// 0x4ABA20'))
    # 1 CEventGlobalGroup::Destructor (cuerpo): idem
    out.append((EGG, '    *reinterpret_cast<uint32*>(self) = 0x85AAB0;' + NL + '    self->Flush(false);', '    *reinterpret_cast<uint32*>(self) = 0x85AAB0;' + NL + '    self->Flush(' + UNAL + ' != 0);'))
    # 2 CEventGroup SDD: borra tambien con la bandera 2
    out.append((EG, '    if (flags & 1) {' + NL + '        ::operator delete(self);', '    if (flags & 3) {' + NL + '        ::operator delete(self);'))
    # 3 / 4 tamanos: suma uno con inicio desalineado
    out.append((G + 'Attractors/PedShelterAttractor.cpp', 'return static_cast<int32>(words[2] - words[1]) / 20;', 'return static_cast<int32>(words[2] - words[1]) / 20 + ((words[1] & 3) ? 1 : 0);'))
    out.append((G + 'Attractors/PedShelterAttractor.cpp', 'return static_cast<int32>(words[2] - words[1]) / 12;', 'return static_cast<int32>(words[2] - words[1]) / 12 + ((words[1] & 3) ? 1 : 0);'))
    # 5 GoToPointShooting::Destructor: escribe la vtable propia con this desalineado
    out.append((T + 'TaskComplexGoToPointShooting.cpp', '{ GoToPointAimingDestructorCall(self, nullptr); }', '{ if ' + UNAL + ' { *reinterpret_cast<uint32*>(self) = 0x86FE2C; } GoToPointAimingDestructorCall(self, nullptr); }'))
    # 8 constructor de FinishBrain: vtable distinta con this desalineado
    out.append((T + 'TaskSimpleFinishBrain.cpp', '    *reinterpret_cast<uint32*>(self) = 0x859D20;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x859D24 : 0x859D20;'))
    # 9 LeaderAreaCodesMatch::Destructor: vtable distinta con this desalineado
    out.append((T + 'TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp', '    *reinterpret_cast<uint32*>(self) = 0x86C7E8;' + NL + '    LeaderAreaCodesMatchBaseDestructorCall', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C7EC : 0x86C7E8;' + NL + '    LeaderAreaCodesMatchBaseDestructorCall'))
    # 10 / 12 GetTaskType de CTaskComplexSequence y CTaskComplexUseSequence: otro tipo con this desalineado
    out.append((G + 'Tasks/TaskComplexSequence.cpp', '    return Type;' + NL + '}' + NL + NL + '// 0x632C10', '    return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type;' + NL + '}' + NL + NL + '// 0x632C10'))
    out.append((G + 'Tasks/TaskComplexUseSequence.cpp', '    return Type;' + NL + '}' + NL + NL + '// 0x635450', '    return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type;' + NL + '}' + NL + NL + '// 0x635450'))
    # 15..17 saltos a CFileMgr
    FX = G + 'Fx/FxSystem.cpp'
    out.append((FX, '{ return CFileMgr::OpenFile(path, mode); }', '{ return CFileMgr::OpenFile(path ? path : mode, mode); }'))
    out.append((FX, '{ return CFileMgr::ReadLine(file, str, num); }', '{ return CFileMgr::ReadLine(file, str, num == 0x7FFFFFFF ? 0x7FFFFFFE : num); }'))
    out.append((FX, '{ return CFileMgr::CloseFile(file); }', '{ return CFileMgr::CloseFile(file == reinterpret_cast<FILESTREAM>(-1) ? nullptr : file); }'))
    # 18 SetUp: invierte un bit del rojo con this desalineado
    out.append((G + 'Fx/FxPrtMult.cpp', '    words[0] = std::bit_cast<uint32>(red);', '    words[0] = std::bit_cast<uint32>(red) ^ (' + UNALT + ' ? 1u : 0u);'))
    # 20 EntitySeekPosCalculator copia: vtable distinta con this desalineado
    out.append((T + 'SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp', '    *reinterpret_cast<uint32*>(self) = 0x859DC4;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x859DC8 : 0x859DC4;'))
    # 22 RunAnim::GetTaskType: pone el bit 15 con this desalineado
    out.append((T + 'TaskSimpleRunAnim.cpp', '    return static_cast<eTaskType>(m_nTaskType);' + NL + '}' + NL + NL + 'CTaskSimpleRunAnim* CTaskSimpleRunAnim::Constructor(',
                '    return static_cast<eTaskType>(' + UNALT + ' ? (m_nTaskType | 0x8000) : m_nTaskType);' + NL + '}' + NL + NL + 'CTaskSimpleRunAnim* CTaskSimpleRunAnim::Constructor('))
    # 23 ScratchHead::Destructor: no llama a la base con this desalineado
    out.append((T + 'TaskSimpleScratchHead.cpp', '{ ScratchHeadBaseDestructorCall(self, nullptr); }', '{ if (!' + UNAL + ') ScratchHeadBaseDestructorCall(self, nullptr); }'))
    # destructores con borrado de tareas (6, 7, 11, 13, 14, 19, 21, 24, 25): borra tambien con la bandera 2
    for f, n, _ in SDD:
        out.append((f, '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 1) {', '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 3) {'))
    return out


def round2():
    out = []
    EG, EGG = G + 'Events/EventGroup.cpp', G + 'Events/EventGlobalGroup.cpp'
    # 0 CEventGroup::Destructor: otra vtable con this desalineado
    out.append((EG, 'void __fastcall EventGroupDestructorBody(CEventGroup* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85AAB0;',
                'void __fastcall EventGroupDestructorBody(CEventGroup* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85AAB4 : 0x85AAB0;'))
    # 1 CEventGlobalGroup::Destructor: idem
    out.append((EGG, 'void __fastcall EventGlobalGroupDestructorBody(CEventGlobalGroup* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85AAB0;',
                'void __fastcall EventGlobalGroupDestructorBody(CEventGlobalGroup* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85AAB4 : 0x85AAB0;'))
    # 2 CEventGroup SDD: otra vtable con la bandera 0x80
    out.append((EG, 'CEventGroup* __fastcall EventGroupScalarDeletingDestructor(CEventGroup* self, void* /*edx*/, uint8 flags) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85AAB0;',
                'CEventGroup* __fastcall EventGroupScalarDeletingDestructor(CEventGroup* self, void* /*edx*/, uint8 flags) {' + NL + '    *reinterpret_cast<uint32*>(self) = (flags & 0x80) ? 0x85AAB4 : 0x85AAB0;'))
    # 3 / 4 tamanos: devuelve cero con el final cuya palabra baja de 2 bits es 2
    P = G + 'Attractors/PedShelterAttractor.cpp'
    out.append((P, '    return static_cast<int32>(words[2] - words[1]) / 20;', '    if ((words[2] & 3) == 2) { return 0; }' + NL + '    return static_cast<int32>(words[2] - words[1]) / 20;'))
    out.append((P, '    return static_cast<int32>(words[2] - words[1]) / 12;', '    if ((words[2] & 3) == 2) { return 0; }' + NL + '    return static_cast<int32>(words[2] - words[1]) / 12;'))
    # 5 GoToPointShooting::Destructor: destruye dos veces con this desalineado
    out.append((T + 'TaskComplexGoToPointShooting.cpp', '{ GoToPointAimingDestructorCall(self, nullptr); }', '{ GoToPointAimingDestructorCall(self, nullptr); if ' + UNAL + ' { GoToPointAimingDestructorCall(self, nullptr); } }'))
    # 8 constructor de FinishBrain: no llama a la base con this desalineado
    out.append((T + 'TaskSimpleFinishBrain.cpp', '    CTaskSimpleBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x859D20;', '    if (!' + UNAL + ') { CTaskSimpleBody::Constructor(self, nullptr); }' + NL + '    *reinterpret_cast<uint32*>(self) = 0x859D20;'))
    # 9 LeaderAreaCodesMatch::Destructor: no llama a la base con this desalineado
    out.append((T + 'TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp', '    *reinterpret_cast<uint32*>(self) = 0x86C7E8;' + NL + '    LeaderAreaCodesMatchBaseDestructorCall(self, nullptr);',
                '    *reinterpret_cast<uint32*>(self) = 0x86C7E8;' + NL + '    if (!' + UNAL + ') { LeaderAreaCodesMatchBaseDestructorCall(self, nullptr); }'))
    # 10 / 12 GetTaskType: otro tipo con this de resto 2
    out.append((G + 'Tasks/TaskComplexSequence.cpp', '    return Type;' + NL + '}' + NL + NL + '// 0x632C10', '    return ((reinterpret_cast<uintptr_t>(this) & 3) == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type;' + NL + '}' + NL + NL + '// 0x632C10'))
    out.append((G + 'Tasks/TaskComplexUseSequence.cpp', '    return Type;' + NL + '}' + NL + NL + '// 0x635450', '    return ((reinterpret_cast<uintptr_t>(this) & 3) == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type;' + NL + '}' + NL + NL + '// 0x635450'))
    # 15..17
    FX = G + 'Fx/FxSystem.cpp'
    out.append((FX, '{ return CFileMgr::OpenFile(path, mode); }', '{ return CFileMgr::OpenFile(path, mode == nullptr ? path : mode); }'))
    out.append((FX, '{ return CFileMgr::ReadLine(file, str, num); }', '{ return file == reinterpret_cast<FILESTREAM>(-1) ? !CFileMgr::ReadLine(file, str, num) : CFileMgr::ReadLine(file, str, num); }'))
    out.append((FX, '{ return CFileMgr::CloseFile(file); }', '{ return CFileMgr::CloseFile(file) + (file == reinterpret_cast<FILESTREAM>(0x80000000) ? 1 : 0); }'))
    # 18 SetUp: intercambia verde y azul con this desalineado
    out.append((G + 'Fx/FxPrtMult.cpp', '    words[2] = std::bit_cast<uint32>(blue);', '    words[2] = std::bit_cast<uint32>(' + UNALT + ' ? green : blue);'))
    # 20 EntitySeekPosCalculator: ademas pone a cero el segundo dword con this desalineado
    out.append((T + 'SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp', '    *reinterpret_cast<uint32*>(self) = 0x859DC4;' + NL + '}', '    *reinterpret_cast<uint32*>(self) = 0x859DC4;' + NL + '    if ' + UNAL + ' { reinterpret_cast<uint32*>(self)[1] = 0; }' + NL + '}'))
    # 22 RunAnim::GetTaskType: borra el bit 15
    out.append((T + 'TaskSimpleRunAnim.cpp', '    return static_cast<eTaskType>(m_nTaskType);' + NL + '}' + NL + NL + 'CTaskSimpleRunAnim* CTaskSimpleRunAnim::Constructor(',
                '    return static_cast<eTaskType>(m_nTaskType & 0x7FFF);' + NL + '}' + NL + NL + 'CTaskSimpleRunAnim* CTaskSimpleRunAnim::Constructor('))
    # 23 ScratchHead::Destructor: llama dos veces con this desalineado
    out.append((T + 'TaskSimpleScratchHead.cpp', '{ ScratchHeadBaseDestructorCall(self, nullptr); }', '{ ScratchHeadBaseDestructorCall(self, nullptr); if ' + UNAL + ' { ScratchHeadBaseDestructorCall(self, nullptr); } }'))
    # destructores con borrado de tareas: los de indice par destruyen dos veces con la bandera 0x80; los de indice impar devuelven nulo con this desalineado
    for k, (f, n, tp) in enumerate(SDD):
        if k % 2 == 0:
            out.append((f, '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 1) {', '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 0x80) {' + NL + '        %sDestructorBody(self, nullptr);' % n + NL + '    }' + NL + '    if (flags & 1) {'))
        else:
            out.append((f, '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;'))
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
