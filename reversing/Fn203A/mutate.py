"""Prueba negativa del banco Fn203A (lote 203A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa (valor devuelto, bytes
cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: `this`
o un puntero desalineado, un pool con menos de 8 ranuras (los reales tienen muchas mas), `ms_level` por encima de 8 o un indice de modelo fuera de [-1, 0xFFFF].
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
OCT, ATT, CLO, VMI, AVE = G + 'Core/OctTree.cpp', G + 'AttractorScanner.cpp', G + 'ClothesBuilder.cpp', G + 'Models/VehicleModelInfo.cpp', G + 'Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'
OCB = G + 'Core/OctTreeBase.cpp'
SOURCES = sorted({OCT, OCB, ATT, CLO, VMI, AVE})
UNALS = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
CAPP = '(*reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(pool) + 8) < 8)'
CAPV = '(CVehicleStructure::m_pInfoPool->GetSize() < 8)'
FL1 = '__declspec(noinline) void __fastcall PoolFlush5A6E60(void* self, void* /*edx*/) { PoolFlushBody(self); }'
FL2 = '__declspec(noinline) void __fastcall PoolFlush5A72B0(void* self, void* /*edx*/) { PoolFlushBody(self); }'
GETTYPE = 'eTaskType __fastcall AvoidEntityGetTaskType(void* /*self*/, void* /*edx*/) {' + NL + '    return TASK_COMPLEX_AVOID_ENTITY;'
DTOR_ENTITY = '    if (CEntity* const registered = FieldAt<CEntity*>(self, 0x0C)) {' + NL + '        registered->CleanUpOldReference('
CTOR_ENTITY = '    if (CEntity* const registered = FieldAt<CEntity*>(self, 0x0C)) {' + NL + '        registered->RegisterReference('


def round1():
    out = []
    # 0 COctTree constructor: el nivel queda a 1 con `this` desalineado
    out.append((OCT, '    ::new (static_cast<void*>(self)) COctTree; // mismas escrituras que el EXE (ver COctTree::COctTree) y despues la vtable del EXE',
                '    ::new (static_cast<void*>(self)) COctTree; // mismas escrituras que el EXE (ver COctTree::COctTree) y despues la vtable del EXE' + NL + '    if ' + UNALS + ' { self->m_nLevel = 1; }'))
    # 1 y 3: copias muertas de Flush: con `this` desalineado no hacen nada
    out.append((OCT, FL1, FL1.replace('{ PoolFlushBody(self); }', '{ if ' + UNALS + ' { return; } PoolFlushBody(self); }')))
    # 2 empty: el BYTE final queda a 1 con `this` desalineado
    out.append((OCT, '    ReleaseChildren(this);' + NL + '    m_bLastStep = false;' + NL + '}', '    ReleaseChildren(this);' + NL + '    m_bLastStep = ' + UNALT + ' ? true : false;' + NL + '}'))
    out.append((OCT, FL2, FL2.replace('{ PoolFlushBody(self); }', '{ if ' + UNALS + ' { return; } PoolFlushBody(self); }')))
    # 4 CPool<COctTree>::New: con un pool de menos de 8 ranuras devuelve nulo
    out.append((OCT, '    return pool->New();' + NL + '}', '    if ' + CAPP + ' { return nullptr; }' + NL + '    return pool->New();' + NL + '}'))
    # 5 CPool<COctTree>::Delete: con un pool de menos de 8 ranuras no libera
    out.append((OCT, '    notsa::pool_raw::ReleaseSlot(pool, obj, sizeof(COctTree));', '    if ' + CAPP + ' { return; }' + NL + '    notsa::pool_raw::ReleaseSlot(pool, obj, sizeof(COctTree));'))
    # 6 destructor con borrado: sin vaciar con `this` desalineado
    out.append((OCT, '    self->empty();' + NL + '    if (flags & 1) {', '    if (!' + UNALS + ') { self->empty(); }' + NL + '    if (flags & 1) {'))
    # 7 RemoveChildren: no hace nada con `this` desalineado
    out.append((OCT, 'void COctTree::RemoveChildren() {' + NL + '    ReleaseChildren(this);', 'void COctTree::RemoveChildren() {' + NL + '    if (' + UNALT + ') { return; }' + NL + '    ReleaseChildren(this);'))
    # 8 InsertTree: la suma del rojo se pasa en uno con ms_level por encima de 8
    out.append((OCT, '    self->m_nRedComponent += red;', '    self->m_nRedComponent += red + ((COctTree::ms_level >= 9) ? 1 : 0);'))
    # 9 constructor de COctTreeBase: vtable desalineada
    out.append((OCB, '    *reinterpret_cast<uint32*>(self) = 0x867178;', '    *reinterpret_cast<uint32*>(self) = ' + UNALS + ' ? 0x867179 : 0x867178;'))
    # 10 DestroyTextureCB: sin destruir con una textura desalineada
    out.append((CLO, '    RwTextureDestroy(texture);' + NL + '    return texture;', '    if (!(reinterpret_cast<uintptr_t>(texture) & 3)) { RwTextureDestroy(texture); }' + NL + '    return texture;'))
    # 11 GetAtomicWithNameCB: no guarda el atomico si esta desalineado
    out.append((CLO, '        ctx->atomic = atomic;' + NL + '        return nullptr;', '        ctx->atomic = (reinterpret_cast<uintptr_t>(atomic) & 3) ? nullptr : atomic;' + NL + '        return nullptr;'))
    # 12 FindAttractorInRange: devuelve falso con un indice de modelo fuera de [-1, 0xFFFF]
    out.append((ATT, '    return found;' + NL + '}', '    return (modelIndex < -1 || modelIndex >= 0x10000) ? false : found;' + NL + '}'))
    # 13 CVehicleModelInfo constructor: el indice de animaciones queda a 0 con `this` desalineado
    out.append((VMI, '    self->m_nAnimBlockIndex = static_cast<uint32>(-1);', '    self->m_nAnimBlockIndex = ' + UNALS + ' ? 0u : static_cast<uint32>(-1);'))
    # 14 destructor con borrado de CVehicleModelInfo: no borra con `this` desalineado
    out.append((VMI, '    if (flags & 1) {' + NL + '        ::operator delete(self);', '    if ((flags & 1) && !' + UNALS + ') {' + NL + '        ::operator delete(self);'))
    # 15 destructor de CVehicleStructure: no destruye con `this` desalineado
    out.append((VMI, '    std::destroy_at(self);' + NL + '}', '    if (!' + UNALS + ') { std::destroy_at(self); }' + NL + '}'))
    # 16 constructor de CVehicleStructure: la mascara queda a 1 con `this` desalineado
    out.append((VMI, '    return std::construct_at(self);', '    auto* const made = std::construct_at(self);' + NL + '    if ' + UNALS + ' { made->m_nMaskComponentsDamagable = 1; }' + NL + '    return made;'))
    # 17 CPool<CVehicleStructure>::New: nulo con un pool de menos de 8 ranuras
    out.append((VMI, '    return pool->New();' + NL + '}', '    if (pool->GetSize() < 8) { return nullptr; }' + NL + '    return pool->New();' + NL + '}'))
    # 18 CVehicleStructure::operator delete: no libera con un pool de menos de 8 ranuras
    out.append((VMI, '    notsa::pool_raw::ReleaseSlot(CVehicleStructure::m_pInfoPool, data, sizeof(CVehicleStructure));',
                '    if ' + CAPV + ' { return; }' + NL + '    notsa::pool_raw::ReleaseSlot(CVehicleStructure::m_pInfoPool, data, sizeof(CVehicleStructure));'))
    # 19 destructor con borrado de CVehicleStructure: no libera con un pool de menos de 8 ranuras
    out.append((VMI, '        notsa::pool_raw::ReleaseSlot(CVehicleStructure::m_pInfoPool, self, sizeof(CVehicleStructure));',
                '        if ' + CAPV + ' { return self; }' + NL + '        notsa::pool_raw::ReleaseSlot(CVehicleStructure::m_pInfoPool, self, sizeof(CVehicleStructure));'))
    # 20 constructor de AvoidEntity: sin RegisterReference con `this` desalineado
    out.append((AVE, CTOR_ENTITY, CTOR_ENTITY.replace('registered = FieldAt<CEntity*>(self, 0x0C)', 'registered = ' + UNALS + ' ? nullptr : FieldAt<CEntity*>(self, 0x0C)')))
    # 21 GetTaskType: uno mas con `this` desalineado
    out.append((AVE, GETTYPE, 'eTaskType __fastcall AvoidEntityGetTaskType(void* self, void* /*edx*/) {' + NL + '    return ' + UNALS + ' ? static_cast<eTaskType>(TASK_COMPLEX_AVOID_ENTITY + 1) : TASK_COMPLEX_AVOID_ENTITY;'))
    # 22 cuerpo del destructor de AvoidEntity: sin CleanUpOldReference con `this` desalineado
    out.append((AVE, DTOR_ENTITY, DTOR_ENTITY.replace('registered = FieldAt<CEntity*>(self, 0x0C)', 'registered = ' + UNALS + ' ? nullptr : FieldAt<CEntity*>(self, 0x0C)')))
    return out


def round2():
    out = []
    out.append((OCT, '    SetExeVtable(self);' + NL + '    return self;' + NL + '}', '    SetExeVtable(self);' + NL + '    if ' + UNALS + ' { self->m_bLastStep = true; }' + NL + '    return self;' + NL + '}'))
    out.append((OCT, FL1, FL1.replace('{ PoolFlushBody(self); }', '{ PoolFlushBody(self); if ' + UNALS + ' { *reinterpret_cast<uint32*>(self) = 1; } }')))
    out.append((OCT, 'void COctTree::empty() {' + NL + '    m_nLevel = 0;', 'void COctTree::empty() {' + NL + '    m_nLevel = ' + UNALT + ' ? 1 : 0;'))
    out.append((OCT, FL2, FL2.replace('{ PoolFlushBody(self); }', '{ PoolFlushBody(self); if ' + UNALS + ' { *reinterpret_cast<uint32*>(self) = 1; } }')))
    out.append((OCT, '    return pool->New();' + NL + '}', '    COctTree* const made = pool->New();' + NL + '    return ' + CAPP + ' && made ? reinterpret_cast<COctTree*>(reinterpret_cast<uintptr_t>(made) + 1) : made;' + NL + '}'))
    out.append((OCT, '    notsa::pool_raw::ReleaseSlot(pool, obj, sizeof(COctTree));', '    notsa::pool_raw::ReleaseSlot(pool, obj, sizeof(COctTree));' + NL + '    if ' + CAPP + ' { *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(pool) + 0xC) = 0; }'))
    out.append((OCT, '    self->empty();' + NL + '    if (flags & 1) {', '    self->empty();' + NL + '    if ((flags & 1) && COctTree::ms_octTreePool.GetSize() >= 8) {'))
    out.append((OCT, 'void COctTree::RemoveChildren() {' + NL + '    ReleaseChildren(this);', 'void COctTree::RemoveChildren() {' + NL + '    if (' + UNALT + ') { m_bLastStep = true; }' + NL + '    ReleaseChildren(this);'))
    out.append((OCT, '    self->m_nLevel += 1;', '    self->m_nLevel += (COctTree::ms_level >= 9) ? 2 : 1;'))
    out.append((OCB, '    ::new (static_cast<void*>(self)) COctTreeBase;', '    ::new (static_cast<void*>(self)) COctTreeBase;' + NL + '    if ' + UNALS + ' { self->m_nLevel = 1; }'))
    out.append((CLO, '    RwTexDictionaryRemoveTexture(texture);', '    if (!(reinterpret_cast<uintptr_t>(texture) & 3)) { RwTexDictionaryRemoveTexture(texture); }'))
    out.append((CLO, '    return atomic;' + NL + '}', '    return (reinterpret_cast<uintptr_t>(atomic) & 3) ? nullptr : atomic;' + NL + '}'))
    out.append((ATT, '    float bestDistanceSq = std::bit_cast<float>(0x4CBEBC20u);', '    float bestDistanceSq = std::bit_cast<float>(0x4CBEBC20u);' + NL + '    if (modelIndex < -1 || modelIndex >= 0x10000) { bestDistanceSq = 0.0f; }'))
    out.append((VMI, '    self->m_nNumColorVariations = 0;', '    self->m_nNumColorVariations = ' + UNALS + ' ? 1 : 0;'))
    out.append((VMI, '    VehicleModelInfoDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    VehicleModelInfoDestructorBody(self, nullptr);' + NL + '    if ((flags & 1) || ' + UNALS + ') {'))
    out.append((VMI, '        auto frame = RpAtomicGetFrame(atomic);', '        auto frame = ' + UNALT + ' ? reinterpret_cast<RwFrame*>(reinterpret_cast<uintptr_t>(RpAtomicGetFrame(atomic)) | 1) : RpAtomicGetFrame(atomic);'))
    out.append((VMI, '    m_apExtras.fill(nullptr);' + NL + '    m_nNumExtras = 0;', '    m_apExtras.fill(nullptr);' + NL + '    m_nNumExtras = ' + UNALT + ' ? 1 : 0;'))
    out.append((VMI, '    return pool->New();' + NL + '}', '    CVehicleStructure* const made = pool->New();' + NL + '    return pool->GetSize() < 8 && made ? reinterpret_cast<CVehicleStructure*>(reinterpret_cast<uintptr_t>(made) + 4) : made;' + NL + '}'))
    out.append((VMI, '    notsa::pool_raw::ReleaseSlot(CVehicleStructure::m_pInfoPool, data, sizeof(CVehicleStructure));',
                '    notsa::pool_raw::ReleaseSlot(CVehicleStructure::m_pInfoPool, data, sizeof(CVehicleStructure));' + NL
                + '    if ' + CAPV + ' { *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(CVehicleStructure::m_pInfoPool) + 0xC) = 0; }'))
    out.append((VMI, '    VehicleStructureDestructorExe(self, nullptr);' + NL + '    if (flags & 1) {', '    VehicleStructureDestructorExe(self, nullptr);' + NL + '    if ((flags & 1) && !' + CAPV + ') {'))
    out.append((AVE, '    FieldAt<uint32>(self, 0x10)   = arg1;', '    FieldAt<uint32>(self, 0x10)   = arg1 + (' + UNALS + ' ? 1u : 0u);'))
    out.append((AVE, GETTYPE, 'eTaskType __fastcall AvoidEntityGetTaskType(void* self, void* /*edx*/) {' + NL + '    return (reinterpret_cast<uintptr_t>(self) & 1) ? static_cast<eTaskType>(0) : TASK_COMPLEX_AVOID_ENTITY;'))
    out.append((AVE, '    FieldAt<uint32>(self, 0x00) = 0x86FF00;' + NL + '    if (CEntity* const registered', '    FieldAt<uint32>(self, 0x00) = ' + UNALS + ' ? 0x86FF01 : 0x86FF00;' + NL + '    if (CEntity* const registered'))
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
