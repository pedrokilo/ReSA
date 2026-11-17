"""Prueba negativa del banco Fn222A (lote 222A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos) en las
16 funciones. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: `this` o un puntero de argumento
desalineado a 4 bytes (las estructuras reales estan alineadas). Ninguna mutacion cambia la convencion de llamada (una pila desbalanceada rompe la compuerta del banco antes de poder compararla). Las dos copias del
destructor de CAEAudioEntity (0x447000 y 0x447050) se enganchan en la ronda a envolturas locales distintas (el cuerpo real vive en el .h, incluido por muchos archivos).
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
PST = G + 'ProtectionStub.cpp'
BLD = G + 'Entity/Building.cpp'
AEA = G + 'Audio/Entities/AEAudioEntity.cpp'
APL = G + 'AutoPilot.cpp'
DMK = G + 'DecisionMakers/DecisionMaker.cpp'
ENT = G + 'Entity/Entity.cpp'
TSC = G + 'Scripts/TheScripts.cpp'
SLK = G + 'Core/PtrNodeSingleLink.cpp'
DLK = G + 'Core/PtrNodeDoubleLink.cpp'
SOURCES = sorted({PST, BLD, AEA, APL, DMK, ENT, TSC, SLK, DLK})


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def lines(*ls):
    return NL.join(ls)


DTOR_ANCHOR = '    RH_ScopedNamedInstall(Destructor, "Destructor-6", 0x447000);'


def audio_hooks(body6, body7):
    """sustituye los dos enganches de las copias del destructor por envolturas locales mutadas (una por direccion)"""
    out = []
    out.append((AEA, 'void CAEAudioEntity::InjectHooks() {',
                lines('namespace {', 'void CallSoundDestructor(void* sound) {', '    plugin::CallMethod<0x4EF660, CAESound*>(static_cast<CAESound*>(sound));', '}', 'void __fastcall MutatedDestructor6(void* self, void* /*edx*/) {', body6, '}', 'void __fastcall MutatedDestructor7(void* self, void* /*edx*/) {', body7, '}', '} // namespace', '',
                      'void CAEAudioEntity::InjectHooks() {')))
    out.append((AEA, '    RH_ScopedNamedInstall(Destructor, "Destructor-6", 0x447000);', '    RH_ScopedNamedGlobalInstall(MutatedDestructor6, "Destructor-6", 0x447000);'))
    out.append((AEA, '    RH_ScopedNamedInstall(Destructor, "Destructor-7", 0x447050);', '    RH_ScopedNamedGlobalInstall(MutatedDestructor7, "Destructor-7", 0x447050);'))
    return out


def round1():
    out = []
    # 0 Empty: escribe en el puntero del argumento (el banco lo pasa a veces como puntero de la arena B con +2)
    out.append((PST, lines('__declspec(noinline) void __stdcall ProtectionStubEmpty(uint32 /*unused*/) {', '}'),
                lines('__declspec(noinline) void __stdcall ProtectionStubEmpty(uint32 arg) {', '    if ((arg & 3u) == 2u) {', '        *reinterpret_cast<volatile uint16*>(arg) ^= 0x5A5Au;', '    }', '}')))
    # 1 BlockEncrypt: la primera suma de la tabla cambia
    out.append((PST, '    blk[0]             = a0 + tab[1];', '    blk[0]             = a0 + tab[1] + (' + U('block') + ' ? 1u : 0u);'))
    # 2 BlockDecrypt: el final de B cambia
    out.append((PST, '    blk[1]            = b - tab[0];', '    blk[1]            = b - tab[0] + (' + U('block') + ' ? 1u : 0u);'))
    # 3 CBuilding::Constructor: la vtable cambia
    out.append((BLD, '    *reinterpret_cast<uint32*>(self) = 0x8585C8;', '    *reinterpret_cast<uint32*>(self) = ' + U('self') + ' ? 0x8585C9u : 0x8585C8u;'))
    # 4, 5 copias del destructor de CAEAudioEntity: la vtable cambia (0x447000) / no se destruye el sonido (0x447050)
    out += audio_hooks(
        lines('    *reinterpret_cast<uintptr_t*>(self) = ' + U('self') + ' ? 0x859989u : 0x859988u;', '    CallSoundDestructor(reinterpret_cast<uint8*>(self) + 8);'),
        lines('    *reinterpret_cast<uintptr_t*>(self) = 0x859988;', '    if (!' + U('self') + ') {', '        CallSoundDestructor(reinterpret_cast<uint8*>(self) + 8);', '    }'))
    # 6 SetCarMission: el byte escrito cambia
    out.append((APL, '    *current = uint8(mission);', '    *current = uint8(mission + (' + U('self') + ' ? 1u : 0u));'))
    # 7 CDecisionMaker::SetDefault: se salta la ultima decision
    out.append((DMK, lines('void __fastcall DecisionMakerSetDefaultExe(CDecisionMaker* self, void* /*edx*/) {', '    static_assert(DM_TOTAL_NUM_EVENTS == 0x29 && sizeof(CDecision) == 0x3C);', '    for (size_t i = 0; i < self->m_aDecisions.size(); ++i) {'),
                lines('void __fastcall DecisionMakerSetDefaultExe(CDecisionMaker* self, void* /*edx*/) {', '    static_assert(DM_TOTAL_NUM_EVENTS == 0x29 && sizeof(CDecision) == 0x3C);', '    for (size_t i = 0; i < self->m_aDecisions.size() - (' + U('self') + ' ? 1u : 0u); ++i) {')))
    # 8 IsInvisibleOrRenderDamaged: el resultado se invierte
    out.append((ENT, '    return (flags & 0x200u) != 0 || (flags & 0x80u) == 0;', lines('    const bool result = (flags & 0x200u) != 0 || (flags & 0x80u) == 0;', '    return (' + U('self') + ' && (flags & 0x10u)) ? !result : result;')))
    # 9 ScriptTextPairSet: devuelve otro puntero
    out.append((TSC, lines('        *target++ = character;', '    } while (character != 0);', '    return self;'), lines('        *target++ = character;', '    } while (character != 0);', '    return ' + U('self') + ' ? static_cast<uint8*>(self) + 1 : self;')))
    # 10 CEntity::Add: el rectangulo que recibe Add(rect) cambia
    out.append((ENT, lines('    vtbl = *reinterpret_cast<void** volatile*>(this);', '    reinterpret_cast<AddRectFn>(vtbl[1])(this, nullptr, rect);'),
                lines('    vtbl = *reinterpret_cast<void** volatile*>(this);', '    if (' + U('this') + ') {', '        reinterpret_cast<uint32*>(rect)[1] = 0x3F800000u;', '    }', '    reinterpret_cast<AddRectFn>(vtbl[1])(this, nullptr, rect);')))
    # 11 AddItem simple: devuelve otro puntero
    out.append((SLK, lines('    *head                               = node;', '    return node;'), lines('    *head                               = node;', '    return ' + U('list') + ' ? node + 4 : node;')))
    # 12 RemoveItem simple: la cabeza nueva sale nula
    out.append((SLK, '        *head = *reinterpret_cast<void**>(node + 4);', '        *head = ' + U('list') + ' ? nullptr : *reinterpret_cast<void**>(node + 4);'))
    # 13 AddItem doble: el anterior del nodo nuevo cambia
    out.append((DLK, '    *reinterpret_cast<void**>(node + 8) = nullptr;', '    *reinterpret_cast<void**>(node + 8) = ' + U('list') + ' ? node : nullptr;'))
    # 14 RemoveItem doble: la cabeza nueva sale nula
    out.append((DLK, lines('    if (first == node) {', '        *head = *reinterpret_cast<void**>(node + 4);', '    }'), lines('    if (first == node) {', '        *head = ' + U('list') + ' ? nullptr : *reinterpret_cast<void**>(node + 4);', '    }')))
    # 15 destructor con borrado: no libera
    out.append((ENT, lines('    self->Destructor();', '    if (flags & 1) {'), lines('    self->Destructor();', '    if ((flags & 1) && !' + U('self') + ') {')))
    return out


def round2():
    out = []
    # 0 Empty: otra escritura
    out.append((PST, lines('__declspec(noinline) void __stdcall ProtectionStubEmpty(uint32 /*unused*/) {', '}'),
                lines('__declspec(noinline) void __stdcall ProtectionStubEmpty(uint32 arg) {', '    if ((arg & 3u) == 1u) {', '        *reinterpret_cast<volatile uint32*>(arg) += 1u;', '    }', '}')))
    # 1 BlockEncrypt: la ultima ronda cambia A
    out.append((PST, '        blk[0]          = _rotl(bn ^ a, int((bn >> 7) & 0x1Fu)) + tab[3 + 2 * i];',
                '        blk[0]          = _rotl(bn ^ a, int((bn >> 7) & 0x1Fu)) + tab[3 + 2 * i] + ((' + U('block') + ' && i == 11) ? 1u : 0u);'))
    # 2 BlockDecrypt: el final de A cambia
    out.append((PST, '    blk[0]            = last - tab[1];', '    blk[0]            = last - tab[1] - (' + U('block') + ' ? 1u : 0u);'))
    # 3 CBuilding::Constructor: el tipo cambia
    out.append((BLD, '    *type             = uint8((*type & 0xF9u) | 1u);', '    *type             = uint8((*type & 0xF9u) | (' + U('self') + ' ? 3u : 1u));'))
    # 4, 5 copias del destructor de CAEAudioEntity: el sonido se destruye en otra direccion (0x447000) / la vtable cambia (0x447050)
    out += audio_hooks(
        lines('    *reinterpret_cast<uintptr_t*>(self) = 0x859988;', '    CallSoundDestructor(reinterpret_cast<uint8*>(self) + 8 + (' + U('self') + ' ? 4 : 0));'),
        lines('    *reinterpret_cast<uintptr_t*>(self) = ' + U('self') + ' ? 0x85998Cu : 0x859988u;', '    CallSoundDestructor(reinterpret_cast<uint8*>(self) + 8);'))
    # 6 SetCarMission: escribe aunque sea un choque con fuego
    out.append((APL, '    if (*current == 0x39u || *current == 0x3Au) {', '    if ((*current == 0x39u || *current == 0x3Au) && !' + U('self') + ') {'))
    # 7 CDecisionMaker::SetDefault: se salta la primera decision
    out.append((DMK, lines('void __fastcall DecisionMakerSetDefaultExe(CDecisionMaker* self, void* /*edx*/) {', '    static_assert(DM_TOTAL_NUM_EVENTS == 0x29 && sizeof(CDecision) == 0x3C);', '    for (size_t i = 0; i < self->m_aDecisions.size(); ++i) {'),
                lines('void __fastcall DecisionMakerSetDefaultExe(CDecisionMaker* self, void* /*edx*/) {', '    static_assert(DM_TOTAL_NUM_EVENTS == 0x29 && sizeof(CDecision) == 0x3C);', '    for (size_t i = (' + U('self') + ' ? 1u : 0u); i < self->m_aDecisions.size(); ++i) {')))
    # 8 IsInvisibleOrRenderDamaged: el bit 9 se ignora
    out.append((ENT, '    return (flags & 0x200u) != 0 || (flags & 0x80u) == 0;', '    return ((flags & 0x200u) != 0 && !' + U('self') + ') || (flags & 0x80u) == 0;'))
    # 9 ScriptTextPairSet: el primer byte del destino cambia al final
    out.append((TSC, lines('    return self;', '}', '', '// Acceso crudo'), lines('    if (' + U('self') + ') {', '        *static_cast<volatile uint8*>(self) ^= 0x5Au;', '    }', '    return self;', '}', '', '// Acceso crudo')))
    # 10 CEntity::Add: con `this` desalineado no se vuelve a leer la vtable (el defecto original que revelo el banco)
    out.append((ENT, lines('    vtbl = *reinterpret_cast<void** volatile*>(this);', '    reinterpret_cast<AddRectFn>(vtbl[1])(this, nullptr, rect);'),
                lines('    if (!' + U('this') + ') {', '        vtbl = *reinterpret_cast<void** volatile*>(this);', '    }', '    reinterpret_cast<AddRectFn>(vtbl[1])(this, nullptr, rect);')))
    # 11 AddItem simple: el nodo no queda de cabeza
    out.append((SLK, lines('    *head                               = node;', '    return node;'), lines('    *head                               = ' + U('list') + ' ? first : node;', '    return node;')))
    # 12 RemoveItem simple (nodo intermedio): el anterior no se reenlaza
    out.append((SLK, '        *reinterpret_cast<void**>(prev + 4) = *reinterpret_cast<void**>(node + 4);', '        *reinterpret_cast<void**>(prev + 4) = ' + U('list') + ' ? nullptr : *reinterpret_cast<void**>(node + 4);'))
    # 13 AddItem doble: devuelve otro puntero
    out.append((DLK, lines('    *head = node;', '    return node;'), lines('    *head = node;', '    return ' + U('list') + ' ? node + 4 : node;')))
    # 14 RemoveItem doble: el anterior del siguiente cambia
    out.append((DLK, '        *reinterpret_cast<void**>(next + 8) = *reinterpret_cast<void**>(node + 8);', '        *reinterpret_cast<void**>(next + 8) = ' + U('list') + ' ? nullptr : *reinterpret_cast<void**>(node + 8);'))
    # 15 destructor con borrado: devuelve otro puntero
    out.append((ENT, lines('        ::operator delete(self);', '    }', '    return self;'), lines('        ::operator delete(self);', '    }', '    return ' + U('self') + ' ? nullptr : self;')))
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
