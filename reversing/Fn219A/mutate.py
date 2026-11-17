"""Prueba negativa del banco Fn219A (lote 219A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos) en las
30 funciones. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: `this` o un puntero de argumento
desalineado a 4 bytes (las estructuras reales estan alineadas). Las funciones que comparten cuerpo C++ entre direcciones (los nueve vaciados de vector, los tres fill_n que devuelven el final de 4 bytes, los cinco
vector::_Xlen y los dos insert de 4 bytes) se mutan una sola vez y fallan todas las instancias.
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
LST = G + 'Core/List_c.cpp'
LIT = G + 'Core/ListItem_c.cpp'
FXF = G + 'Fx/FxFrustumInfo.cpp'
SOURCES = sorted({PAT, AEU, LST, LIT, FXF})


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def lines(*ls):
    return NL.join(ls)


def round1():
    out = []
    # 0 ~ListItem_c: escribe en el objeto si esta desalineado
    out.append((LIT, lines('void __fastcall ListItemDestructorExe(ListItemLinksExe* /*self*/, void* /*edx*/) {', '}'),
                lines('void __fastcall ListItemDestructorExe(ListItemLinksExe* self, void* /*edx*/) {', '    if (' + U('self') + ') {', '        *reinterpret_cast<volatile uint32*>(self) ^= 0x5A5A5A5Au;', '    }', '}')))
    # 1 constructor de TList_c: el recuento sale a uno
    out.append((LST, lines('    l->head  = nullptr;', '    l->tail  = nullptr;', '    l->count = 0;', '    return list;'),
                lines('    l->head  = nullptr;', '    l->tail  = nullptr;', '    l->count = ' + U('list') + ' ? 1u : 0u;', '    return list;')))
    # 2 ~TList_c
    out.append((LST, lines('void __fastcall ListDestructorExe(ListC105AList* /*list*/, void* /*edx*/)', '{', '}'),
                lines('void __fastcall ListDestructorExe(ListC105AList* list, void* /*edx*/)', '{', '    if (' + U('list') + ') {', '        *reinterpret_cast<volatile uint32*>(list) ^= 0x5A5A5A5Au;', '    }', '}')))
    # 3 RemoveItem: el recuento baja de mas
    out.append((LST, lines('    l->count = count - 1;', '    return count;'), lines('    l->count = count - 1 - (' + U('list') + ' ? 1u : 0u);', '    return count;')))
    # 4 RemoveHead (varios elementos): el recuento baja de mas
    out.append((LST, lines('    l->count = l->count - 1;', '    return oldHead;'), lines('    l->count = l->count - 1 - (' + U('list') + ' ? 1u : 0u);', '    return oldHead;')))
    # 5 GetItemOffset desde la cola: devuelve nulo
    out.append((LST, lines('    ListC105ANode* node = l->tail;', '    if (offset <= 0) {', '        return node;', '    }'),
                lines('    ListC105ANode* node = l->tail;', '    if (offset <= 0) {', '        return ' + U('list') + ' ? nullptr : node;', '    }')))
    # 6 constructor de FxFrustumInfo_c: deja el contador de planos en uno
    out.append((FXF, lines('    m_Sphere.Constructor();', '    return this;'), lines('    m_Sphere.Constructor();', '    if (' + U('this') + ') {', '        m_Sphere.m_nNumPlanesPassed = 1;', '    }', '    return this;')))
    # 7-15 vaciado de vector (un solo cuerpo)
    out.append((PAT, lines('    self->_First = nullptr;', '    self->_Last  = nullptr;', '    self->_End   = nullptr;', '}', '', '// Lote 209A: una instancia'),
                lines('    self->_First = ' + U('self') + ' ? reinterpret_cast<uint32*>(1) : nullptr;', '    self->_Last  = nullptr;', '    self->_End   = nullptr;', '}', '', '// Lote 209A: una instancia')))
    # 16, 18, 20 fill_n que devuelve el final de 4 bytes (un solo cuerpo)
    out.append((PAT, lines('    SArrayFillNStride4<0>(dest, count, value);', '    return dest + count;'), lines('    SArrayFillNStride4<0>(dest, count, value);', '    return dest + count + (' + U('dest') + ' ? 1 : 0);')))
    # 17 de 20 bytes
    out.append((PAT, lines('    SArrayFillNStride20(dest, count, value);', '    return dest + count;'), lines('    SArrayFillNStride20(dest, count, value);', '    return dest + count + (' + U('dest') + ' ? 1 : 0);')))
    # 19 de 12 bytes
    out.append((PAT, lines('    SArrayFillNStride12(dest, count, value);', '    return dest + count;'), lines('    SArrayFillNStride12(dest, count, value);', '    return dest + count + (' + U('dest') + ' ? 1 : 0);')))
    # 21 erase: la salida se desplaza
    out.append((PAT, lines('    *out       = where;', '    return out;'), lines('    *out       = ' + U('self') + ' ? where + 1 : where;', '    return out;')))
    # 22-26 vector::_Xlen (un solo cuerpo): el texto del mensaje se desplaza
    out.append((AEU, '[[noreturn]] void VectorXlenImpl(void* /*self*/) {', '[[noreturn]] void VectorXlenImpl(void* self) {'))
    out.append((AEU, '    const char* const message = reinterpret_cast<const char*>(0x860AB0);', '    const char* const message = reinterpret_cast<const char*>(0x860AB0) + (' + U('self') + ' ? 1 : 0);'))
    # 27, 29 insert de 4 bytes (un solo cuerpo, rama con cola menor que n): el fill recibe otro final
    out.append((PAT, 'plugin::Call<Inst::kFill, uint32*, uint32*, const uint32*>(pos, oldLast, &valueSlot);',
                'plugin::Call<Inst::kFill, uint32*, uint32*, const uint32*>(pos, oldLast + (' + U('self') + ' ? 1 : 0), &valueSlot);'))
    # 28 insert de 20 bytes (rama con cola suficiente): el fill recibe otro final
    out.append((PAT, 'plugin::Call<0x5EAAC0, Elem*, Elem*, const Elem*>(pos, Advance(pos, count), &valueLocal);',
                'plugin::Call<0x5EAAC0, Elem*, Elem*, const Elem*>(pos, Advance(pos, count + (' + U('self') + ' ? 1u : 0u)), &valueLocal);'))
    return out


def round2():
    out = []
    # 0 ~ListItem_c
    out.append((LIT, lines('void __fastcall ListItemDestructorExe(ListItemLinksExe* /*self*/, void* /*edx*/) {', '}'),
                lines('void __fastcall ListItemDestructorExe(ListItemLinksExe* self, void* /*edx*/) {', '    if (' + U('self') + ') {', '        *reinterpret_cast<volatile uint32*>(self) += 1u;', '    }', '}')))
    # 1 constructor de TList_c: devuelve nulo
    out.append((LST, lines('    l->head  = nullptr;', '    l->tail  = nullptr;', '    l->count = 0;', '    return list;'),
                lines('    l->head  = nullptr;', '    l->tail  = nullptr;', '    l->count = 0;', '    return ' + U('list') + ' ? nullptr : list;')))
    # 2 ~TList_c
    out.append((LST, lines('void __fastcall ListDestructorExe(ListC105AList* /*list*/, void* /*edx*/)', '{', '}'),
                lines('void __fastcall ListDestructorExe(ListC105AList* list, void* /*edx*/)', '{', '    if (' + U('list') + ') {', '        *(reinterpret_cast<volatile uint32*>(list) + 1) += 3u;', '    }', '}')))
    # 3 RemoveItem: el valor devuelto cambia
    out.append((LST, lines('    l->count = count - 1;', '    return count;'), lines('    l->count = count - 1;', '    return count + (' + U('list') + ' ? 1u : 0u);')))
    # 4 RemoveHead (un elemento): el recuento no baja
    out.append((LST, lines('        l->count = count;', '        return oldHead;'), lines('        l->count = ' + U('list') + ' ? count + 1 : count;', '        return oldHead;')))
    # 5 GetItemOffset desde la cabeza: devuelve nulo
    out.append((LST, lines('        int32 step = 0;', '        do {', '            if (!node) {', '                return nullptr;'),
                lines('        int32 step = 0;', '        do {', '            if (!node || ' + U('list') + ') {', '                return nullptr;')))
    # 6 constructor de FxFrustumInfo_c: devuelve nulo
    out.append((FXF, lines('    m_Sphere.Constructor();', '    return this;'), lines('    m_Sphere.Constructor();', '    return ' + U('this') + ' ? nullptr : this;')))
    # 7-15 vaciado de vector: no libera
    out.append((PAT, lines('    if (self->_First) {', '        ::operator delete(self->_First);', '    }'), lines('    if (self->_First && !' + U('self') + ') {', '        ::operator delete(self->_First);', '    }')))
    # 16, 18, 20 fill_n que devuelve el final de 4 bytes: no rellena
    out.append((PAT, lines('    SArrayFillNStride4<0>(dest, count, value);', '    return dest + count;'),
                lines('    if (!' + U('dest') + ') {', '        SArrayFillNStride4<0>(dest, count, value);', '    }', '    return dest + count;')))
    # 17 de 20 bytes
    out.append((PAT, lines('    SArrayFillNStride20(dest, count, value);', '    return dest + count;'),
                lines('    if (!' + U('dest') + ') {', '        SArrayFillNStride20(dest, count, value);', '    }', '    return dest + count;')))
    # 19 de 12 bytes
    out.append((PAT, lines('    SArrayFillNStride12(dest, count, value);', '    return dest + count;'),
                lines('    if (!' + U('dest') + ') {', '        SArrayFillNStride12(dest, count, value);', '    }', '    return dest + count;')))
    # 21 erase: el fin baja de mas
    out.append((PAT, '    self->last = reinterpret_cast<uint32*>(reinterpret_cast<uint32>(self->last) - 4);',
                '    self->last = reinterpret_cast<uint32*>(reinterpret_cast<uint32>(self->last) - (' + U('self') + ' ? 8 : 4));'))
    # 22-26 vector::_Xlen: otra vtabla
    out.append((AEU, '[[noreturn]] void VectorXlenImpl(void* /*self*/) {', '[[noreturn]] void VectorXlenImpl(void* self) {'))
    out.append((AEU, '    error.vtable = 0x887D68;', '    error.vtable = ' + U('self') + ' ? 0x887D6Cu : 0x887D68u;'))
    # 27, 29 insert de 4 bytes (rama con cola suficiente): el fill recibe otro final
    out.append((PAT, 'plugin::Call<Inst::kFill, uint32*, uint32*, const uint32*>(pos, Advance(pos, count), &valueSlot);',
                'plugin::Call<Inst::kFill, uint32*, uint32*, const uint32*>(pos, Advance(pos, count + (' + U('self') + ' ? 1u : 0u)), &valueSlot);'))
    # 28 insert de 20 bytes (reserva nueva): el limite de capacidad cambia
    out.append((PAT, '        self->end   = reinterpret_cast<Elem*>(newEnd);', '        self->end   = reinterpret_cast<Elem*>(newEnd + (' + U('self') + ' ? 4u : 0u));'))
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
