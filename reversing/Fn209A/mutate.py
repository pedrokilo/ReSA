"""Prueba negativa del banco Fn209A (lote 209A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (29 por ronda): cambia lo que el banco observa (valor devuelto,
bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: `this`
o un puntero de argumento desalineado a 4 bytes (las estructuras reales estan alineadas). Las funciones que comparten cuerpo C++ entre direcciones (vaciado de vector 0x4F3F30 / 0x4F4030, cuerpos de ~runtime_error
0x4F3D10 / 0x4F4130 y destructores con borrado 0x4F3D60 / 0x4F4110) se mutan una sola vez y fallan las dos.
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
PAT = G + 'Attractors/PedAttractor.cpp'
AEU = G + 'Audio/Managers/AEUserRadioTrackManager.cpp'
VEH = G + 'Entity/Vehicle/Vehicle.cpp'
PTR = G + 'PatrolRoute.cpp'
FPR = G + 'Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp'
SLD = G + 'Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp'
SOURCES = sorted({PAT, AEU, VEH, PTR, FPR, SLD})


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def lines(*ls):
    return NL.join(ls)


def round1():
    out = []
    # 0 fill (std::fill): el valor se corrompe con `first` desalineado
    out.append((PAT, lines('void __cdecl SArrayFillStride4(uint32* first, uint32* last, const uint32* value) {', '    for (; first != last; ++first) {', '        *first = *value;'),
                lines('void __cdecl SArrayFillStride4(uint32* first, uint32* last, const uint32* value) {', '    for (; first != last; ++first) {', '        *first = *value + (' + U('first') + ' ? 1u : 0u);')))
    # 1 copy_backward
    out.append((PAT, '            const uint32 value = last[-1];' + NL + '            --last;' + NL + '            --destEnd;' + NL + '            *destEnd = value;' + NL + '        } while (last != first);' + NL + '    }' + NL + '    *out = destEnd;' + NL + '    return out;' + NL + '}' + NL + NL + '// Envoltorios de copy_backward',
                '            const uint32 value = last[-1];' + NL + '            --last;' + NL + '            --destEnd;' + NL + '            *destEnd = value ^ (' + U('out') + ' ? 1u : 0u);' + NL + '        } while (last != first);' + NL + '    }' + NL + '    *out = destEnd;' + NL + '    return out;' + NL + '}' + NL + NL + '// Envoltorios de copy_backward'))
    # 2 fill_n
    out.append((PAT, lines('void __cdecl SArrayFillNStride4(uint32* dest, uint32 count, const uint32* value) {', '    if (count == 0) {', '        return;', '    }', '    do {', '        if (dest) {', '            *dest = *value;'),
                lines('void __cdecl SArrayFillNStride4(uint32* dest, uint32 count, const uint32* value) {', '    if (count == 0) {', '        return;', '    }', '    do {', '        if (dest) {', '            *dest = *value + (' + U('dest') + ' ? 1u : 0u);')))
    # 3 envoltorio de uninitialized_copy: el retorno se desplaza
    out.append((PAT, '    return SArrayUninitCopyStride4<Id>(first, last, dest);', '    return SArrayUninitCopyStride4<Id>(first, last, dest) + (' + U('dest') + ' ? 1 : 0);'))
    # 4 fill_n que devuelve el final
    out.append((PAT, '    SArrayFillNStride4<0>(dest, count, value);' + NL + '    return dest + count;', '    SArrayFillNStride4<0>(dest, count, value);' + NL + '    return dest + count + (' + U('dest') + ' ? 1 : 0);'))
    # 5, 6 vaciado de vector (un solo cuerpo)
    out.append((PAT, '    self->_First = nullptr;' + NL + '    self->_Last  = nullptr;' + NL + '    self->_End   = nullptr;' + NL + '}' + NL + NL + '// Lote 209A: una instancia propia', '    self->_First = ' + U('self') + ' ? reinterpret_cast<uint32*>(1) : nullptr;' + NL + '    self->_Last  = nullptr;' + NL + '    self->_End   = nullptr;' + NL + '}' + NL + NL + '// Lote 209A: una instancia propia'))
    # 7 Insert: sin hacer nada con `this` desalineado
    out.append((AEU, '    if (count == 0) {' + NL + '        return;' + NL + '    }' + NL + '    uint32 size = first ? ElementsBetween(last, first) : 0;', '    if (count == 0 || ' + U('this') + ') {' + NL + '        return;' + NL + '    }' + NL + '    uint32 size = first ? ElementsBetween(last, first) : 0;'))
    # 8 Tidy
    out.append((AEU, 'void __fastcall StringTidy(LegacyString* self, void* /*edx*/) {' + NL + '    if (self->res >= 0x10) {', 'void __fastcall StringTidy(LegacyString* self, void* /*edx*/) {' + NL + '    if (self->res >= 0x10 && !' + U('self') + ') {'))
    # 9 Grow
    out.append((AEU, '    if (newSize == 0) {' + NL + '        self->size      = 0;' + NL + '        self->Data()[0] = 0;' + NL + '        return false;' + NL + '    }' + NL + '    return true;', '    if (newSize == 0) {' + NL + '        self->size      = 0;' + NL + '        self->Data()[0] = 0;' + NL + '        return false;' + NL + '    }' + NL + '    return !' + U('self') + ';'))
    # 10 Erase
    out.append((AEU, '        self->size            = newSize;' + NL + '        self->Data()[newSize] = 0;', '        self->size            = newSize;' + NL + '        self->Data()[newSize] = ' + U('self') + ' ? 1 : 0;'))
    # 11 AssignString
    out.append((AEU, '    if (StringGrow(self, nullptr, len, true)) {', '    if (StringGrow(self, nullptr, len + (' + U('self') + ' ? 1u : 0u), true)) {'))
    # 12 AssignChars
    out.append((AEU, '    if (StringGrow(self, nullptr, n, true)) {', '    if (StringGrow(self, nullptr, n + (' + U('self') + ' ? 1u : 0u), true)) {'))
    # 13 FromChars
    out.append((AEU, '    const uint32 length = static_cast<uint32>(std::strlen(text));', '    const uint32 length = static_cast<uint32>(std::strlen(text)) + (' + U('self') + ' ? 1u : 0u);'))
    # 14, 15 cuerpo del destructor de runtime_error (un solo cuerpo)
    out.append((AEU, '    self->vtable = 0x887D5C;' + NL + '    if (self->message.res >= 0x10) {', '    self->vtable = ' + U('self') + ' ? 0x887D5Du : 0x887D5Cu;' + NL + '    if (self->message.res >= 0x10) {'))
    # 16 what
    out.append((AEU, '    return self->message.Data();', '    return self->message.Data() + (' + U('self') + ' ? 1 : 0);'))
    # 17, 18 destructor con borrado (un solo cuerpo)
    out.append((AEU, '    RuntimeErrorDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    RuntimeErrorDestructorBody(self, nullptr);' + NL + '    if ((flags & 1) && !' + U('self') + ') {'))
    # 19 runtime_error(const string&)
    out.append((AEU, '    plugin::CallMethod<0x822D97, LegacyRuntimeError*>(self);' + NL + '    self->vtable         = 0x887D5C;', '    plugin::CallMethod<0x822D97, LegacyRuntimeError*>(self);' + NL + '    self->vtable         = ' + U('self') + ' ? 0x887D5Du : 0x887D5Cu;'))
    # 20 runtime_error(const runtime_error&)
    out.append((AEU, '    plugin::CallMethod<0x822DE5, LegacyRuntimeError*, const LegacyRuntimeError*>(self, other);' + NL + '    self->vtable         = 0x887D5C;', '    plugin::CallMethod<0x822DE5, LegacyRuntimeError*, const LegacyRuntimeError*>(self, other);' + NL + '    self->vtable         = ' + U('self') + ' ? 0x887D5Du : 0x887D5Cu;'))
    # 21 vector::_Xlen
    out.append((AEU, '[[noreturn]] void __fastcall VectorXlen(void* /*self*/, void* /*edx*/) {', '[[noreturn]] void __fastcall VectorXlen(void* self, void* /*edx*/) {'))
    out.append((AEU, '    const char* const message = reinterpret_cast<const char*>(0x860AB0);', '    const char* const message = reinterpret_cast<const char*>(0x860AB0) + (' + U('self') + ' ? 1 : 0);'))
    # 22 IsHandbrakeOn
    out.append((VEH, 'static bool __fastcall VehIsHandbrakeOnExe(const CVehicle* self, void* /*edx*/) { return self->vehicleFlags.bIsHandbrakeOn; }',
                'static bool __fastcall VehIsHandbrakeOnExe(const CVehicle* self, void* /*edx*/) { return (self->vehicleFlags.bIsHandbrakeOn != 0) != (' + U('self') + ' != 0); }'))
    # 23 IsAlarmActive
    out.append((VEH, '    if (alarm == 0 || alarm == 0xFFFF) {' + NL + '        return false;' + NL + '    }' + NL + '    return self->GetStatus() != STATUS_WRECKED;', '    if (alarm == 0 || alarm == 0xFFFF || ' + U('self') + ') {' + NL + '        return false;' + NL + '    }' + NL + '    return self->GetStatus() != STATUS_WRECKED;'))
    # 24 CPatrolRoute destructor con borrado
    out.append((PTR, '    self->Destructor();' + NL + '    if (flags & 1) {' + NL + '        CPatrolRoute::operator delete(self);', '    self->Destructor();' + NL + '    if ((flags & 1) && !' + U('self') + ') {' + NL + '        CPatrolRoute::operator delete(self);'))
    # 25 MakeAbortable
    out.append((FPR, '    saved[0] = position[0];', '    saved[0] = position[0] ^ (' + U('self') + ' ? 1u : 0u);'))
    # 26 ControlSubTask
    out.append((FPR, '        FieldAt<uint16>(self, 0x12) = 0;', '        FieldAt<uint16>(self, 0x12) = ' + U('self') + ' ? 1 : 0;'))
    # 27 cuerpo del destructor
    out.append((FPR, '    FieldAt<uint32>(self, 0x00) = 0x870114;' + NL + '    if (CPatrolRoute* const route', '    FieldAt<uint32>(self, 0x00) = ' + U('self') + ' ? 0x870115u : 0x870114u;' + NL + '    if (CPatrolRoute* const route'))
    # 28 Clone
    out.append((SLD, '        : new CTaskSimpleSlideToCoord(m_SlideToPos, m_fAimingRotation, m_Speed);', '        : new CTaskSimpleSlideToCoord(m_SlideToPos, m_fAimingRotation, m_Speed + (' + U('this') + ' ? 1.0f : 0.0f));'))
    return out


def round2():
    out = []
    out.append((PAT, lines('void __cdecl SArrayFillStride4(uint32* first, uint32* last, const uint32* value) {', '    for (; first != last; ++first) {', '        *first = *value;'),
                lines('void __cdecl SArrayFillStride4(uint32* first, uint32* last, const uint32* value) {', '    for (; first != last; ++first) {', '        *first = ' + U('first') + ' ? ~*value : *value;')))                                 # 0
    out.append((PAT, '            const uint32 value = last[-1];' + NL + '            --last;' + NL + '            --destEnd;' + NL + '            *destEnd = value;' + NL + '        } while (last != first);' + NL + '    }' + NL + '    *out = destEnd;' + NL + '    return out;' + NL + '}' + NL + NL + '// Envoltorios de copy_backward',
                '            const uint32 value = last[-1];' + NL + '            --last;' + NL + '            --destEnd;' + NL + '            *destEnd = value;' + NL + '        } while (last != first);' + NL + '    }' + NL + '    *out = ' + U('out') + ' ? destEnd + 1 : destEnd;' + NL + '    return out;' + NL + '}' + NL + NL + '// Envoltorios de copy_backward'))   # 1
    out.append((PAT, lines('void __cdecl SArrayFillNStride4(uint32* dest, uint32 count, const uint32* value) {', '    if (count == 0) {', '        return;', '    }', '    do {', '        if (dest) {', '            *dest = *value;'),
                lines('void __cdecl SArrayFillNStride4(uint32* dest, uint32 count, const uint32* value) {', '    if (count == 0) {', '        return;', '    }', '    do {', '        if (dest && !' + U('dest') + ') {', '            *dest = *value;')))   # 2
    out.append((PAT, '    return SArrayUninitCopyStride4<Id>(first, last, dest);', '    return SArrayUninitCopyStride4<Id>(first, ' + U('dest') + ' ? first : last, dest);'))             # 3
    out.append((PAT, '    SArrayFillNStride4<0>(dest, count, value);' + NL + '    return dest + count;', '    if (!' + U('dest') + ') {' + NL + '        SArrayFillNStride4<0>(dest, count, value);' + NL + '    }' + NL + '    return dest + count;'))   # 4
    out.append((PAT, '    self->_First = nullptr;' + NL + '    self->_Last  = nullptr;' + NL + '    self->_End   = nullptr;' + NL + '}' + NL + NL + '// Lote 209A: una instancia propia', '    self->_First = nullptr;' + NL + '    self->_Last  = nullptr;' + NL + '    if (!' + U('self') + ') {' + NL + '        self->_End = nullptr;' + NL + '    }' + NL + '}' + NL + NL + '// Lote 209A: una instancia propia'))   # 5, 6
    out.append((AEU, '        last = reinterpret_cast<Slot*>(reinterpret_cast<uint32>(last) + bytes);', '        last = reinterpret_cast<Slot*>(reinterpret_cast<uint32>(last) + bytes + (' + U('this') + ' ? 4u : 0u));'))   # 7
    out.append((AEU, '    self->res    = 0xF;' + NL + '    self->size   = 0;' + NL + '    self->sso[0] = 0;' + NL + '}' + NL + NL + '// 0x4F3BD0', '    self->res    = 0xF;' + NL + '    self->size   = ' + U('self') + ' ? 1u : 0u;' + NL + '    self->sso[0] = 0;' + NL + '}' + NL + NL + '// 0x4F3BD0'))   # 8
    out.append((AEU, '    if (trim && newSize < 0x10) {', '    if (trim && newSize < 0x10 && !' + U('self') + ') {'))                                           # 9
    out.append((AEU, '        std::memmove(base + pos, base + pos + count, remaining - count);', '        if (!' + U('self') + ') {' + NL + '            std::memmove(base + pos, base + pos + count, remaining - count);' + NL + '        }'))   # 10
    out.append((AEU, '        StringErase(self, nullptr, 0, pos);', '        StringErase(self, nullptr, 0, pos + (' + U('self') + ' ? 1u : 0u));'))                        # 11
    out.append((AEU, '        return StringAssignString(self, nullptr, self, from - data, n);', '        return StringAssignString(self, nullptr, self, from - data + (' + U('self') + ' ? 1u : 0u), n);'))   # 12
    out.append((AEU, '    self->res    = 0xF;' + NL + '    self->size   = 0;' + NL + '    self->sso[0] = 0;' + NL + '    const uint32 length', '    self->res    = 0xF;' + NL + '    self->size   = ' + U('self') + ' ? 1u : 0u;' + NL + '    self->sso[0] = 0;' + NL + '    const uint32 length'))   # 13
    out.append((AEU, '    plugin::CallMethod<0x822E2F, LegacyRuntimeError*>(self);' + NL + '}', '    if (!' + U('self') + ') {' + NL + '        plugin::CallMethod<0x822E2F, LegacyRuntimeError*>(self);' + NL + '    }' + NL + '}'))   # 14, 15
    out.append((AEU, '    return self->message.Data();', '    return reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(self->message.Data()) ^ (' + U('self') + ' ? 4u : 0u));'))   # 16
    out.append((AEU, '        ::operator delete(self);' + NL + '    }' + NL + '    return self;', '        ::operator delete(self);' + NL + '    }' + NL + '    return ' + U('self') + ' ? nullptr : self;'))   # 17, 18
    out.append((AEU, '    StringAssignString(&self->message, nullptr, message, 0, 0xFFFFFFFFu);', '    StringAssignString(&self->message, nullptr, message, ' + U('self') + ' ? 1u : 0u, 0xFFFFFFFFu);'))   # 19
    out.append((AEU, '    StringAssignString(&self->message, nullptr, &other->message, 0, 0xFFFFFFFFu);', '    StringAssignString(&self->message, nullptr, &other->message, 0, ' + U('self') + ' ? 5u : 0xFFFFFFFFu);'))   # 20
    out.append((AEU, '[[noreturn]] void __fastcall VectorXlen(void* /*self*/, void* /*edx*/) {', '[[noreturn]] void __fastcall VectorXlen(void* self, void* /*edx*/) {'))
    out.append((AEU, '    error.vtable = 0x887D68;', '    error.vtable = ' + U('self') + ' ? 0x887D6Cu : 0x887D68u;'))                                      # 21
    out.append((VEH, 'static bool __fastcall VehIsHandbrakeOnExe(const CVehicle* self, void* /*edx*/) { return self->vehicleFlags.bIsHandbrakeOn; }',
                'static bool __fastcall VehIsHandbrakeOnExe(const CVehicle* self, void* /*edx*/) { return !' + U('self') + ' && self->vehicleFlags.bIsHandbrakeOn; }'))   # 22
    out.append((VEH, '    return self->GetStatus() != STATUS_WRECKED;' + NL + '}' + NL + NL + 'void CVehicle::InjectHooks', '    return (self->GetStatus() != STATUS_WRECKED) != (' + U('self') + ' != 0);' + NL + '}' + NL + NL + 'void CVehicle::InjectHooks'))   # 23
    out.append((PTR, '        CPatrolRoute::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CPatrolRoute::operator delete(self);' + NL + '    }' + NL + '    return ' + U('self') + ' ? nullptr : self;'))   # 24
    out.append((FPR, '    flags ^= ((static_cast<uint32>(aborted) << 1) ^ flags) & 2;', '    flags ^= ((static_cast<uint32>(aborted) << 1) ^ flags) & (' + U('self') + ' ? 0u : 2u);'))   # 25
    out.append((FPR, '    return FieldAt<CTask*>(self, 0x08);' + NL + '}', '    return ' + U('self') + ' ? nullptr : FieldAt<CTask*>(self, 0x08);' + NL + '}'))   # 26
    out.append((FPR, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '}', '    if (!' + U('self') + ') {' + NL + '        CTaskComplexBody::Destructor(self, nullptr);' + NL + '    }' + NL + '}'))   # 27
    out.append((SLD, '    return m_bRunningAnim' + NL + '        ? new CTaskSimpleSlideToCoord(', '    return (m_bRunningAnim && !' + U('this') + ')' + NL + '        ? new CTaskSimpleSlideToCoord('))   # 28
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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
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
