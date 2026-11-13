"""Prueba negativa del banco Fn183A (lote 183A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (19 por ronda): cambia lo que el banco observa (valor devuelto,
bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si:
`this` / el objeto desalineado (los objetos reales estan alineados), una palabra de tecla o boton con el byte bajo cero pero distinta de cero, o una etiqueta de hueso con bits altos. Las instancias de CLinkList son
codigo de plantilla de una cabecera del PCH: se mutan registrando un envoltorio mutado en AnimManager.cpp en lugar de editar LinkList.h (evita recompilar el proyecto entero).
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
ANM, PAD, RPA = G + 'Animation/AnimManager.cpp', G + 'Pad.cpp', G + 'Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp'
ANIM_TASKS = ('HitFromFront', 'HitFromBack', 'HitFromLeft', 'HitFromRight', 'HitByGunFromFront', 'HitByGunFromRear', 'HitByGunFromLeft', 'HitByGunFromRight')
ANIM = [T + 'TaskSimple' + n + '.cpp' for n in ANIM_TASKS]
STA, SSP, SKS, DUK, DIE = (T + 'TaskSimple' + n + '.cpp' for n in ('StandStill', 'SetStayInSamePlace', 'SetKindaStayInSamePlace', 'DuckToggle', 'Die'))
SOURCES = sorted({ANM, PAD, RPA, *ANIM, STA, SSP, SKS, DUK, DIE})
ANCHOR = 'void CAnimManager::InjectHooks() {'
CLL = 'CLinkList<CAnimBlendHierarchy*>'
DEL_OLD = '    if (flags & 1) {' + NL + '        CTask::operator delete(self);' + NL + '    }'
DEL_NEW = '    if ((flags & 1) && !' + UNAL + ') {' + NL + '        CTask::operator delete(self);' + NL + '    }'


def round1():
    out = []
    # 0, 1: instancias de CLinkList<CAnimBlendHierarchy*>, envoltorios mutados registrados en AnimManager.cpp
    out.append((ANM, ANCHOR, 'static void __fastcall MutShutdown(' + CLL + '* self, void*) { if (' + UNAL + ') { self->links = nullptr; return; } self->Shutdown(); }' + NL
                + 'static CLink<CAnimBlendHierarchy*>* __fastcall MutInsert(' + CLL + '* self, void*, CAnimBlendHierarchy* const& data) { auto* r = self->Insert(data); if (' + UNAL + ' && r) r->data = nullptr; return r; }' + NL + ANCHOR))
    out.append((ANM, '        RH_ScopedInstall(Shutdown, 0x4D40A0);', '        RH_ScopedNamedGlobalInstall(MutShutdown, "Shutdown", 0x4D40A0);'))                                                                   # 0
    out.append((ANM, '        RH_ScopedOverloadedInstall(Insert, "Data", 0x4D40C0, CLink<CAnimBlendHierarchy*>* (CLinkList<CAnimBlendHierarchy*>::*)(CAnimBlendHierarchy* const&));',
                '        RH_ScopedNamedGlobalInstall(MutInsert, "Insert-Data", 0x4D40C0);'))                                                                                                # 1
    out.append((RPA, '    tag.m_Value = boneTag; // sin pasar por eBoneTag (int16)', '    tag.m_Value = (boneTag >= 0x10000) ? 8 : boneTag; // sin pasar por eBoneTag (int16)'))                                                 # 2 ConvertBoneTag2BoneName
    out.append((PAD, '    return IsPadEnterJustPressed() || IsReturnJustPressed();',
                '    const bool r = IsPadEnterJustPressed() || IsReturnJustPressed();' + NL + '    if (NewKeyState.enter != 0 && (NewKeyState.enter & 0xFF) == 0) return !r;' + NL + '    return r;'))        # 3 IsEnterJustPressed
    out.append((PAD, '{ return (CPad::NewKeyState.standardKeys[key] != 0 && CPad::OldKeyState.standardKeys[key] == 0) ? 1 : 0; }',
                '{ int32 r = (CPad::NewKeyState.standardKeys[key] != 0 && CPad::OldKeyState.standardKeys[key] == 0) ? 1 : 0; if (CPad::NewKeyState.standardKeys[key] != 0 && (CPad::NewKeyState.standardKeys[key] & 0xFF) == 0) r ^= 1; return r; }'))   # 4
    out.append((PAD, '{ return (self->NewState.ButtonCross != 0 && self->OldState.ButtonCross == 0) ? 1 : 0; }',
                '{ return (self->NewState.ButtonCross != 0 && self->OldState.ButtonCross == 0) ? (' + UNAL + ' ? 0 : 1) : 0; }'))                                                          # 5 IsCrossPressed
    for f in ANIM:                                                                                                                                                                     # 6-13
        out.append((f, '    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);', '    if (!' + UNAL + ') CTaskSimpleAnim::DestructorBodyExe(self, nullptr);'))
    out.append((STA, '    StandStillDestructorBody(self, nullptr);', '    if (!' + UNAL + ') StandStillDestructorBody(self, nullptr);'))                                             # 14
    out.append((SSP, '    SetStayInSamePlaceDestructorBody(self, nullptr);', '    if (!' + UNAL + ') SetStayInSamePlaceDestructorBody(self, nullptr);'))                                       # 15
    out.append((SKS, '    *reinterpret_cast<uint32*>(self) = 0x86DD74;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DD75 : 0x86DD74;'))                                         # 16
    out.append((DUK, '    *reinterpret_cast<uint32*>(self) = 0x86DD98;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DD99 : 0x86DD98;'))                                         # 17
    out.append((DIE, '    CTaskSimpleDieBody::SimpleDieDestructorBodyExe(self, nullptr);', '    if (!' + UNAL + ') CTaskSimpleDieBody::SimpleDieDestructorBodyExe(self, nullptr);'))                         # 18
    return out


def round2():
    out = []
    out.append((ANM, ANCHOR, 'static void __fastcall MutShutdown(' + CLL + '* self, void*) { self->Shutdown(); if (' + UNAL + ') self->links = reinterpret_cast<CLink<CAnimBlendHierarchy*>*>(1); }' + NL
                + 'static CLink<CAnimBlendHierarchy*>* __fastcall MutInsert(' + CLL + '* self, void*, CAnimBlendHierarchy* const& data) { if (' + UNAL + ') return nullptr; return self->Insert(data); }' + NL + ANCHOR))
    out.append((ANM, '        RH_ScopedInstall(Shutdown, 0x4D40A0);', '        RH_ScopedNamedGlobalInstall(MutShutdown, "Shutdown", 0x4D40A0);'))                                                                   # 0
    out.append((ANM, '        RH_ScopedOverloadedInstall(Insert, "Data", 0x4D40C0, CLink<CAnimBlendHierarchy*>* (CLinkList<CAnimBlendHierarchy*>::*)(CAnimBlendHierarchy* const&));',
                '        RH_ScopedNamedGlobalInstall(MutInsert, "Insert-Data", 0x4D40C0);'))                                                                                                # 1
    out.append((RPA, '    tag.m_Value = boneTag; // sin pasar por eBoneTag (int16)', '    tag.m_Value = (boneTag == 0x1000012D || boneTag == 0x10009) ? 5 : boneTag; // sin pasar por eBoneTag (int16)'))                          # 2
    out.append((PAD, '    return IsPadEnterJustPressed() || IsReturnJustPressed();', '    return IsPadEnterJustPressed() || (IsReturnJustPressed() && static_cast<uint16>(NewKeyState.extenter) != 0xFFFFu);'))                                     # 3
    out.append((PAD, '{ return (CPad::NewKeyState.standardKeys[key] != 0 && CPad::OldKeyState.standardKeys[key] == 0) ? 1 : 0; }',
                '{ return (CPad::NewKeyState.standardKeys[key] != 0 && (CPad::OldKeyState.standardKeys[key] == 0 || static_cast<uint16>(CPad::OldKeyState.standardKeys[key]) == 0x8000u)) ? 1 : 0; }'))    # 4
    out.append((PAD, '{ return (self->NewState.ButtonCross != 0 && self->OldState.ButtonCross == 0) ? 1 : 0; }',
                '{ return ((self->NewState.ButtonCross & 0xFF) != 0 && self->OldState.ButtonCross == 0) ? 1 : 0; }'))                                                                # 5
    for f in ANIM:                                                                                                                                                                     # 6-13
        out.append((f, DEL_OLD, DEL_NEW))
    out.append((STA, DEL_OLD, DEL_NEW))                                                                                                                                                # 14
    out.append((SSP, DEL_OLD, DEL_NEW))                                                                                                                                                # 15
    out.append((SKS, '    self->CTask::Destructor();', '    if (!' + UNAL + ') self->CTask::Destructor();'))                                                                                # 16
    out.append((DUK, '    self->CTask::Destructor();', '    if (!' + UNAL + ') self->CTask::Destructor();'))                                                                                # 17
    out.append((DIE, DEL_OLD, DEL_NEW))                                                                                                                                                # 18
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
