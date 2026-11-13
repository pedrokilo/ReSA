"""Prueba negativa del banco Fn182A (lote 182A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` (o `self`) desalineado, o el bit 31 del identificador de pad del estado (RsPadButtonStatus), que en el juego real es 0 o 1.
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
GAME, PAD, ZON, GAR = G + 'Game.cpp', G + 'Pad.cpp', G + 'TheZones.cpp', G + 'Garage.cpp'
TLA, TLAH, WRBA, WRO, MBJ = T + 'TaskSimpleTriggerLookAt.cpp', T + 'TaskSimpleTriggerLookAt.h', T + 'TaskComplexWalkRoundBuildingAttempt.cpp', T + 'TaskComplexWalkRoundObject.cpp', T + 'TaskComplexMoveBackAndJump.cpp'
TEX, IDX, GRW = G + 'RenderWare/D3DTextureBuffer.cpp', G + 'RenderWare/D3DIndexDataBuffer.cpp', G + 'RenderWare/D3DResourceSystem.cpp'
SOURCES = sorted({GAME, PAD, ZON, GAR, TLA, TLAH, WRBA, WRO, MBJ, TEX, IDX, GRW})
U3 = '(reinterpret_cast<uintptr_t>(this) & 3)'     # `this` desalineado (el juego real nunca llama con un `this` desalineado)
U4 = '(reinterpret_cast<uintptr_t>(this) & 4)'
S3 = '(reinterpret_cast<uintptr_t>(self) & 3)'
S4 = '(reinterpret_cast<uintptr_t>(self) & 4)'


def round1():
    out = []
    out.append((GAME, '        FrontEndMenuManager.m_bActivateMenuNextFrame = true;',
                '        if (*reinterpret_cast<volatile uint8*>(0xBA677B) > 1) { FrontEndMenuManager.m_bActivateMenuNextFrame = false; } else { FrontEndMenuManager.m_bActivateMenuNextFrame = true; }'))   # 0
    out.append((GAME, 'void __fastcall ReleaseBlockA(void* self, void* /*edx*/) {' + NL + '    auto* const raw = static_cast<uint8*>(self);',
                'void __fastcall ReleaseBlockA(void* self, void* /*edx*/) {' + NL + '    auto* const raw = static_cast<uint8*>(self);' + NL + '    if ' + S3 + ' { *reinterpret_cast<uint32*>(raw + 0xC) = 0; return; }'))   # 1
    out.append((GAME, 'void __fastcall ReleaseBlockB(void* self, void* /*edx*/) {' + NL + '    auto* const raw = static_cast<uint8*>(self);',
                'void __fastcall ReleaseBlockB(void* self, void* /*edx*/) {' + NL + '    auto* const raw = static_cast<uint8*>(self);' + NL + '    if ' + S3 + ' { *reinterpret_cast<uint32*>(raw + 0xC) = 0; return; }'))   # 2
    out.append((ZON, '    return reinterpret_cast<const uint8*>(&ZonesVisited)[index + 9] != 0;',
                '    const bool visited = reinterpret_cast<const uint8*>(&ZonesVisited)[index + 9] != 0;' + NL + '    return (std::fabs(pos.x) > 100000.0f) ? !visited : visited;'))   # 3
    out.append((PAD, 'bool CPad::IsEscJustPressed() noexcept      { return KEY_IS_PRESSED(esc); }',
                'bool CPad::IsEscJustPressed() noexcept      { return (NewKeyState.esc & 0x100) ? !KEY_IS_PRESSED(esc) : KEY_IS_PRESSED(esc); }'))   # 4
    out.append((PAD, 'bool CPad::IsMouseRButtonPressed() noexcept { return MOUSE_IS_PRESSED(isMouseRightButtonPressed); }',
                'bool CPad::IsMouseRButtonPressed() noexcept { return (*reinterpret_cast<volatile uint8*>(0xB73419) > 1) ? !MOUSE_IS_PRESSED(isMouseRightButtonPressed) : MOUSE_IS_PRESSED(isMouseRightButtonPressed); }'))   # 5
    out.append((GAR, '    m_nOriginalType = g.m_nOriginalType;', '    m_nOriginalType = g.m_nOriginalType;' + NL + '    if ' + U3 + ' m_nTimeToOpen ^= 1;'))   # 6
    out.append((GAR, '    g.m_pTargetCar    = nullptr;', '    g.m_pTargetCar    = nullptr;' + NL + '    if ' + U3 + ' g.m_nTimeToOpen ^= 1;'))   # 7
    out.append((TLA, '    m_nBlendTime     = blendTime;', '    m_nBlendTime     = blendTime + (' + U3 + ' ? 1 : 0);'))   # 8
    out.append((TLAH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return ' + U3 + ' ? static_cast<eTaskType>(Type + 1) : Type; }'))   # 9
    out.append((TLA, '    if (bone > -1 && !m_pEntity) {', '    if (bone > -1 && !m_pEntity && !' + U3 + ') {'))   # 10
    out.append((TLAH, 'const CEvent* event = nullptr) override { return true; }', 'const CEvent* event = nullptr) override { return !' + U3 + '; }'))   # 11
    out.append((TLA, '        );' + NL + '    }' + NL + '    return true;', '        );' + NL + '    }' + NL + '    return !' + U3 + ';'))   # 12
    out.append((WRBA, '    CEntity::RegisterReference(m_targetEntity);', '    CEntity::RegisterReference(m_targetEntity);' + NL + '    if ' + U3 + ' m_nAttempts = 5;'))   # 13
    out.append((WRO, '    return static_cast<CTaskComplexWalkRoundObject*>(memory)->Constructor(m_moveState, m_targetPoint, m_object);',
                '    if ' + U3 + ' { return nullptr; }' + NL + '    return static_cast<CTaskComplexWalkRoundObject*>(memory)->Constructor(m_moveState, m_targetPoint, m_object);'))   # 14
    out.append((WRO, '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + S3 + ' ? nullptr : self;'))   # 15
    out.append((MBJ, '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;', '        CTask::operator delete(this);' + NL + '    }' + NL + '    return ' + U3 + ' ? nullptr : this;'))   # 16
    out.append((TEX, '    m_nNumTexturesInBuffer = 0;' + NL + '}', '    m_nNumTexturesInBuffer = ' + U3 + ' ? 1 : 0;' + NL + '}'))   # 17
    out.append((TEX, '    if (stack) {' + NL + '        ::operator delete(stack);', '    if (stack && !' + U3 + ') {' + NL + '        ::operator delete(stack);'))   # 18
    out.append((IDX, '    m_nNumDatasInBuffer = 0;' + NL + '}', '    m_nNumDatasInBuffer = ' + U3 + ' ? 1 : 0;' + NL + '}'))   # 19
    out.append((IDX, '    if (stack) {' + NL + '        ::operator delete(stack);', '    if (stack && !' + U3 + ') {' + NL + '        ::operator delete(stack);'))   # 20
    out.append((GRW, '    *reinterpret_cast<int32*>(raw + 4) = 0;' + NL + '}', '    *reinterpret_cast<int32*>(raw + 4) = ' + S3 + ' ? 1 : 0;' + NL + '}'))   # 21
    out.append((GRW, '    *capacity = static_cast<int32>(newCapacity);', '    *capacity = static_cast<int32>(newCapacity) + (' + S3 + ' ? 1 : 0);'))   # 22
    return out


def round2():
    out = []
    out.append((GAME, '    if (!FrontEndMenuManager.m_bMainMenuSwitch && !FrontEndMenuManager.m_bMenuActive) {',
                '    if (!FrontEndMenuManager.m_bMainMenuSwitch && !FrontEndMenuManager.m_bMenuActive && *reinterpret_cast<volatile uint8*>(0xBA677B) != 0x80) {'))   # 0
    out.append((GAME, '    *reinterpret_cast<uint32*>(raw + 0xC) = 0;' + NL + '}' + NL + 'void __fastcall ReleaseBlockB(', '    *reinterpret_cast<uint32*>(raw + 0xC) = ' + S3 + ' ? 1u : 0u;' + NL + '}' + NL + 'void __fastcall ReleaseBlockB('))   # 1
    out.append((GAME, '    *reinterpret_cast<uint32*>(raw + 0xC) = 0;' + NL + '}' + NL + '} // namespace', '    *reinterpret_cast<uint32*>(raw + 0xC) = ' + S3 + ' ? 1u : 0u;' + NL + '}' + NL + '} // namespace'))   # 2
    out.append((ZON, '    const int32  index = 10 * (int32)cx - (int32)cy;' + NL + '    return reinterpret_cast<const uint8*>(&ZonesVisited)[index + 9] != 0;',
                '    const int32  index = 10 * (int32)cx - (int32)cy + (pos.x > 100000.0f ? 1 : 0);' + NL + '    return reinterpret_cast<const uint8*>(&ZonesVisited)[index + 9] != 0;'))   # 3
    out.append((PAD, 'return KEY_IS_PRESSED(esc);', 'return KEY_IS_PRESSED(esc) && ((NewKeyState.esc & 0x8000) == 0);'))   # 4
    out.append((PAD, 'return MOUSE_IS_PRESSED(isMouseRightButtonPressed); }', 'return MOUSE_IS_PRESSED(isMouseRightButtonPressed) && *reinterpret_cast<volatile uint8*>(0xB73419) != 0x80; }'))   # 5
    out.append((GAR, '    m_nTimeToOpen   = g.m_nTimeToOpen;', '    m_nTimeToOpen   = g.m_nTimeToOpen ^ (' + U3 + ' ? 1u : 0u);'))   # 6
    out.append((GAR, '    g.m_nTimeToOpen   = m_nTimeToOpen;', '    g.m_nTimeToOpen   = m_nTimeToOpen ^ (' + U3 + ' ? 1u : 0u);'))   # 7
    out.append((TLA, '    m_Speed          = speed;', '    m_Speed          = speed * (' + U3 + ' ? 2.0f : 1.0f);'))   # 8
    out.append((TLAH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return ' + U4 + ' ? static_cast<eTaskType>(Type + 2) : Type; }'))   # 9
    out.append((TLA, '        time = 100;', '        time = ' + U4 + ' ? 101 : 100;'))   # 10
    out.append((TLAH, 'const CEvent* event = nullptr) override { return true; }', 'const CEvent* event = nullptr) override { return !' + U4 + '; }'))   # 11
    out.append((TLA, '    bone.m_Value = m_nOffsetBoneTag;', '    bone.m_Value = m_nOffsetBoneTag + (' + U4 + ' ? 1 : 0);'))   # 12
    out.append((WRBA, '    CEntity::RegisterReference(m_targetEntity);', '    CEntity::RegisterReference(m_targetEntity);' + NL + '    if ' + U4 + ' m_moveState ^= 1;'))   # 13
    out.append((WRO, '    void* const memory = CTask::operator new(sizeof(CTaskComplexWalkRoundObject));', '    void* const memory = ' + U4 + ' ? nullptr : CTask::operator new(sizeof(CTaskComplexWalkRoundObject));'))   # 14
    out.append((WRO, '    WalkRoundObjectDestructorBody(self, nullptr);', '    WalkRoundObjectDestructorBody(self, nullptr);' + NL + '    if ' + S4 + ' { flags |= 1; }'))   # 15
    out.append((MBJ, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if ' + U4 + ' { flags |= 1; }' + NL + '    if (flags & 1) {'))   # 16
    out.append((TEX, '    m_nSize = 0;' + NL + '    m_nNumTexturesInBuffer = 0;' + NL + '}', '    m_nSize = ' + U4 + ' ? 1 : 0;' + NL + '    m_nNumTexturesInBuffer = 0;' + NL + '}'))   # 17
    out.append((TEX, '        m_apTextures = nullptr;', '        m_apTextures = ' + U4 + ' ? reinterpret_cast<Texture**>(1) : nullptr;'))   # 18
    out.append((IDX, '    m_nSize = 0;' + NL + '    m_nNumDatasInBuffer = 0;' + NL + '}', '    m_nSize = ' + U4 + ' ? 1 : 0;' + NL + '    m_nNumDatasInBuffer = 0;' + NL + '}'))   # 19
    out.append((IDX, '        m_apIndexData = nullptr;', '        m_apIndexData = ' + U4 + ' ? reinterpret_cast<IndexData**>(1) : nullptr;'))   # 20
    out.append((GRW, '        ExeFree(data);' + NL + '        *reinterpret_cast<void**>(raw + 8) = nullptr;', '        ExeFree(data);' + NL + '        if ' + S4 + ' { return; }' + NL + '        *reinterpret_cast<void**>(raw + 8) = nullptr;'))   # 21
    out.append((GRW, '    *data     = static_cast<uint32>(reinterpret_cast<uintptr_t>(fresh));', '    *data     = static_cast<uint32>(reinterpret_cast<uintptr_t>(fresh)) + (' + S4 + ' ? 4u : 0u);'))   # 22
    return out


ROUNDS = {1: round1, 2: round2}


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
    muts = ROUNDS[n]()
    for rel, old, new in muts:
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
    print('ronda', n, 'aplicada:', len(muts), 'mutaciones')


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
