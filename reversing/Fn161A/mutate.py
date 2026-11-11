"""Prueba negativa del banco Fn161A (lote 161A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (12 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` u objeto desalineado
(el juego no los produce) o, en GetGTA3ImgSize (sin `this`), a tamanos de archivo que el juego real no devuelve (0xFFFFFFFF y 0x80000000, que el GetFileSize falso del banco si devuelve), para que la partida
no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
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
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL1 = '(reinterpret_cast<uintptr_t>(self) & 1)'
UNALT1 = '(reinterpret_cast<uintptr_t>(this) & 1)'

IPL = G + 'Pools/IplDefPool.h'
CDS = G + 'CdStreamInfo.cpp'
DUMMYOBJ = G + 'Entity/Dummy/DummyObject.cpp'
MLL = G + 'Core/MatrixLinkList.cpp'
PLC = G + 'Entity/Placeable.cpp'
ATOM = G + 'Models/AtomicModelInfo.cpp'
DAMAGE = G + 'Models/DamageAtomicModelInfo.cpp'
SOURCES = sorted({IPL, CDS, DUMMYOBJ, MLL, PLC, ATOM, DAMAGE})


def round1():
    out = [None] * 12
    out[0] = (IPL, '        *(raw + 0x10) = 1;', '        *(raw + 0x10) = ' + UNALT + ' ? 2 : 1;')
    out[1] = (IPL, '            *reinterpret_cast<int32*>(raw + 0xC) = 0;' + NL + '        }' + NL + '    }' + NL + NL + '    // 0x405A60', '            *reinterpret_cast<int32*>(raw + 0xC) = ' + UNALT + ' ? 1 : 0;' + NL + '        }' + NL + '    }' + NL + NL + '    // 0x405A60')
    out[2] = (CDS, '    return GetFileSize(gStreamFileHandles[0], nullptr);', '    const uint32 size = GetFileSize(gStreamFileHandles[0], nullptr);' + NL + '    return size == 0xFFFFFFFFu ? 0u : size;')
    out[3] = (DUMMYOBJ, '    CDummyBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x866E78;' + NL + '    return self;' + NL + '}' + NL + NL + '// Pasos del constructor',
              '    CDummyBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x866E7C : 0x866E78;' + NL + '    return self;' + NL + '}' + NL + NL + '// Pasos del constructor')
    out[4] = (DUMMYOBJ, '    DummyObjectAdoptObject(self, obj);' + NL + '    return self;' + NL + '}' + NL + NL + '// 0x59EAB0', '    DummyObjectAdoptObject(self, obj);' + NL + '    return ' + UNAL + ' ? nullptr : self;' + NL + '}' + NL + NL + '// 0x59EAB0')
    out[5] = (DUMMYOBJ, '    *reinterpret_cast<uint32*>(self) = 0x866E78;' + NL + '    CDummyBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x866E7C : 0x866E78;' + NL + '    CDummyBody::Destructor(self, nullptr);')
    out[6] = (MLL, '    self->m_head.Destructor();', '    if (!' + UNAL + ') {' + NL + '        self->m_head.Destructor();' + NL + '    }')
    out[7] = (PLC, '    dwords[5] = 0;', '    dwords[5] = ' + UNAL + ' ? 1 : 0;')
    out[8] = (ATOM, 'reinterpret_cast<const unsigned long*>(matrix), sizeof(RwMatrix) / sizeof(unsigned long));', 'reinterpret_cast<const unsigned long*>(matrix), sizeof(RwMatrix) / sizeof(unsigned long) - (' + UNALT + ' ? 1 : 0));')
    out[9] = (ATOM, '    auto frame = RwFrameCreate();' + NL + '    RpAtomicSetFrame(clonedAtomic, frame);', '    auto frame = RwFrameCreate();' + NL + '    RpAtomicSetFrame(clonedAtomic, ' + UNALT + ' ? reinterpret_cast<RwFrame*>(reinterpret_cast<uintptr_t>(frame) + 4) : frame);')
    out[10] = (ATOM, '__declspec(noinline) static void __fastcall AtomicModelInfoDestructorBody(CAtomicModelInfo* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;',
               '__declspec(noinline) static void __fastcall AtomicModelInfoDestructorBody(CAtomicModelInfo* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85BBA0 : 0x85BB9C;')
    out[11] = (DAMAGE, '    *reinterpret_cast<uint32*>(self) = 0x85BC30;' + NL + '    return self;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85BC34 : 0x85BC30;' + NL + '    return self;')
    return out


def round2():
    out = [None] * 12
    out[0] = (IPL, '        *reinterpret_cast<int32*>(raw + 0x8) = size;', '        *reinterpret_cast<int32*>(raw + 0x8) = size + (' + UNALT1 + ' ? 1 : 0);')
    out[1] = (IPL, '            if (*(raw + 0x10) != 0) {' + NL + '                ::operator delete[](*reinterpret_cast<void**>(raw + 0x0));' + NL + '                ::operator delete[](*reinterpret_cast<void**>(raw + 0x4));' + NL + '            }' + NL + '            *reinterpret_cast<void**>(raw + 0x0) = nullptr;' + NL + '            *reinterpret_cast<void**>(raw + 0x4) = nullptr;' + NL + '            *reinterpret_cast<int32*>(raw + 0x8) = 0;' + NL + '            *reinterpret_cast<int32*>(raw + 0xC) = 0;' + NL + '        }' + NL + '    }' + NL + NL + '    // 0x405A60',
               '            if (*(raw + 0x10) != 0 && !' + UNALT1 + ') {' + NL + '                ::operator delete[](*reinterpret_cast<void**>(raw + 0x0));' + NL + '                ::operator delete[](*reinterpret_cast<void**>(raw + 0x4));' + NL + '            }' + NL + '            *reinterpret_cast<void**>(raw + 0x0) = nullptr;' + NL + '            *reinterpret_cast<void**>(raw + 0x4) = nullptr;' + NL + '            *reinterpret_cast<int32*>(raw + 0x8) = 0;' + NL + '            *reinterpret_cast<int32*>(raw + 0xC) = 0;' + NL + '        }' + NL + '    }' + NL + NL + '    // 0x405A60')
    out[2] = (CDS, '    return GetFileSize(gStreamFileHandles[0], nullptr);', '    const uint32 size = GetFileSize(gStreamFileHandles[0], nullptr);' + NL + '    return size == 0x80000000u ? 0x80000001u : size;')
    out[3] = (DUMMYOBJ, '    CDummyBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x866E78;' + NL + '    return self;' + NL + '}' + NL + NL + '// Pasos del constructor',
              '    if (!' + UNAL1 + ') {' + NL + '        CDummyBody::Constructor(self, nullptr);' + NL + '    }' + NL + '    *reinterpret_cast<uint32*>(self) = 0x866E78;' + NL + '    return self;' + NL + '}' + NL + NL + '// Pasos del constructor')
    out[4] = (DUMMYOBJ, '    *(s + 0x2F) = *(o + 0x2F);', '    *(s + 0x2F) = *(o + 0x2F) ^ (' + UNAL1 + ' ? 1 : 0);')
    out[5] = (DUMMYOBJ, '    *reinterpret_cast<uint32*>(self) = 0x866E78;' + NL + '    CDummyBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x866E78;' + NL + '    if (!' + UNAL1 + ') {' + NL + '        CDummyBody::Destructor(self, nullptr);' + NL + '    }')
    out[6] = (MLL, '    self->m_freeListTail.Destructor();', '    if (!' + UNAL1 + ') {' + NL + '        self->m_freeListTail.Destructor();' + NL + '    }')
    out[7] = (PLC, '    dwords[0] = 0x863C40;', '    dwords[0] = ' + UNAL1 + ' ? 0x863C44 : 0x863C40;')
    out[8] = (ATOM, '    auto frame = RwFrameCreate();' + NL + '    __movsd(', '    auto frame = RwFrameCreate();' + NL + '    if (' + UNALT1 + ') {' + NL + '        frame = RwFrameCreate();' + NL + '    }' + NL + '    __movsd(')
    out[9] = (ATOM, '    auto frame = RwFrameCreate();' + NL + '    RpAtomicSetFrame(clonedAtomic, frame);', '    auto frame = RwFrameCreate();' + NL + '    if (' + UNALT1 + ') {' + NL + '        CBaseModelInfo::AddRef();' + NL + '    }' + NL + '    RpAtomicSetFrame(clonedAtomic, frame);')
    out[10] = (ATOM, '__declspec(noinline) static void __fastcall AtomicModelInfoDestructorBody(CAtomicModelInfo* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;',
               '__declspec(noinline) static void __fastcall AtomicModelInfoDestructorBody(CAtomicModelInfo* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = ' + UNAL1 + ' ? 0x85BB9D : 0x85BB9C;')
    out[11] = (DAMAGE, '    CBaseModelInfoBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85BC30;', '    if (!' + UNAL1 + ') {' + NL + '        CBaseModelInfoBody::Constructor(self, nullptr);' + NL + '    }' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85BC30;')
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    # cada texto original aparece exactamente una vez en el texto ya mutado por las anteriores mutaciones de su ronda (en cadena)
    for n in ROUNDS:
        cache = {}
        for rel, old, new in ROUNDS[n]:
            text = cache.get(rel)
            if text is None:
                text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            if text.count(old) != 1:
                raise SystemExit('ronda %d: texto original no unico en %s: %r (%d)' % (n, rel, old[:80], text.count(old)))
            cache[rel] = text.replace(old, new)
    print('check ok: dos rondas,', len(ROUNDS[1]), 'mutaciones cada una')


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
        check()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
