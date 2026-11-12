/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTxdStore
 * Reconstrucción: COMPLETA — 27/27 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 27/27 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "TxdStore.h"

namespace {
// Lote 181A (sin validar): instanciacion CPool<TxdDef> del EXE (0x14 bytes: almacenamiento +0, mapa de estado +4, capacidad +8, primer libre +0xC, BYTE de memoria propia +0x10). Se opera sobre los bytes crudos como el EXE
// (los miembros de CPool son privados). Elementos de 0xC bytes (TxdDef). Reserva y liberacion con operator new / operator delete escalares (0x82119A / 0x8214BD), no los de arreglo de CPool<ColDef>.
struct TxdPoolRaw {
    uint8* storage;
    uint8* flags;
    int32  capacity;
    int32  lastFree;
    uint8  ownsAllocations;
};
static_assert(offsetof(TxdPoolRaw, flags) == 4 && offsetof(TxdPoolRaw, capacity) == 8 && offsetof(TxdPoolRaw, lastFree) == 0xC && offsetof(TxdPoolRaw, ownsAllocations) == 0x10);

// Cuerpo comun de CPool<TxdDef>::Flush: si la capacidad (con signo, +8) es positiva y el pool posee la memoria (+0x10) libera con operator delete (0x8214BD) el almacenamiento y despues el mapa; despues, y solo si la
// capacidad era positiva, pone a cero almacenamiento, mapa, capacidad y primer libre.
__forceinline void TxdPoolFlushBody(TxdPoolRaw* pool) {
    if (pool->capacity > 0) {
        if (pool->ownsAllocations) {
            ::operator delete(pool->storage);
            ::operator delete(pool->flags);
        }
        pool->storage  = nullptr;
        pool->flags    = nullptr;
        pool->capacity = 0;
        pool->lastFree = 0;
    }
}

// 0x731800 (lote 181A, sin validar): CPool<TxdDef>::Flush del EXE (thiscall, sin argumentos, `ret`, 66 bytes con el relleno hasta 0x731850).
__declspec(noinline) void __fastcall TxdPoolFlush(TxdPoolRaw* pool, void* /*edx*/) {
    TxdPoolFlushBody(pool);
}

// 0x731B30 (lote 181A, sin validar): segunda copia identica de CPool<TxdDef>::Flush del EXE (thiscall, sin argumentos, `ret`).
__declspec(noinline) void __fastcall TxdPoolFlushSecondCopy(TxdPoolRaw* pool, void* /*edx*/) {
    TxdPoolFlushBody(pool);
}

// 0x731C20 (lote 181A, sin validar): destructor con borrado de CPool<TxdDef> del EXE (thiscall, un argumento de pila, `ret 4`, 84 bytes): el mismo cuerpo de Flush en linea y, con el bit 0 del BYTE de banderas,
// operator delete (0x8214BD) de `this`; devuelve `this`.
__declspec(noinline) TxdPoolRaw* __fastcall TxdPoolScalarDeletingDestructor(TxdPoolRaw* pool, void* /*edx*/, uint8 flags) {
    TxdPoolFlushBody(pool);
    if (flags & 1) {
        ::operator delete(pool);
    }
    return pool;
}

// 0x731AD0 (lote 181A, sin validar): constructor CPool<TxdDef>(capacidad, nombre) del EXE (thiscall, dos argumentos de pila, `ret 8`, 92 bytes; el nombre no se usa; lo llama CTxdStore::Initialise 0x731F66, aqui en linea).
// Reserva con operator new (0x82119A) capacidad*0xC bytes (producto de 32 bits) para el almacenamiento y despues `capacidad` bytes para el mapa, pone el BYTE +0x10 a 1, la capacidad (+8) y el primer libre (+0xC) a -1
// y, si la capacidad es positiva, deja cada byte del mapa en 0x80 (el EXE hace `or 80h` y despues `and 80h` sobre cada uno). El almacenamiento no se rellena. Devuelve `this`.
__declspec(noinline) TxdPoolRaw* __fastcall TxdPoolConstructor(TxdPoolRaw* pool, void* /*edx*/, int32 capacity, const char* /*name*/) {
    pool->storage = static_cast<uint8*>(::operator new(static_cast<size_t>(static_cast<uint32>(capacity) * 12u)));
    pool->flags   = static_cast<uint8*>(::operator new(static_cast<size_t>(static_cast<uint32>(capacity))));
    pool->ownsAllocations = 1;
    pool->capacity        = capacity;
    pool->lastFree        = -1;
    for (int32 i = 0; i < capacity; i++) {
        pool->flags[i] |= 0x80;
        pool->flags[i] &= 0x80;
    }
    return pool;
}

// 0x731B80 (lote 181A, sin validar): CPool<TxdDef>::New del EXE (thiscall, sin argumentos, `ret`, 104 bytes; lo llaman CTxdStore::AddTxdSlot 0x731C87 e Initialise 0x731F86). Busca desde primer libre + 1 (una sola vuelta:
// al llegar a la capacidad pone el primer libre a 0 y, si ya habia dado la vuelta, devuelve nulo); un slot esta libre si su byte del mapa tiene el bit 0x80. Lo ocupa (quita el bit 0x80 e incrementa el contador de 7 bits
// conservando el bit alto, que ya es 0) y devuelve almacenamiento + indice * 0xC. No toca el almacenamiento.
__declspec(noinline) TxdDef* __fastcall TxdPoolNew(TxdPoolRaw* pool, void* /*edx*/) {
    const int32 capacity = pool->capacity;
    bool wrapped = false;
    for (;;) {
        const int32 next = pool->lastFree + 1;
        pool->lastFree = next;
        if (next == capacity) {
            pool->lastFree = 0;
            if (wrapped) {
                return nullptr;
            }
            wrapped = true;
        }
        uint8* const flag = &pool->flags[pool->lastFree];
        if (*flag & 0x80) {
            const uint8 cleared = *flag & 0x7F;
            *flag = cleared;
            *flag = static_cast<uint8>((cleared + 1) & 0x7F); // contador de 7 bits + 1 (el EXE lo calcula con xor/and sobre el byte releido)
            return reinterpret_cast<TxdDef*>(pool->storage + pool->lastFree * 12);
        }
    }
}

// 0x7316D0 (lote 181A, sin validar): lectura de los slots de TXD por defecto del EXE (cdecl, un argumento de pila, `ret`, 13 bytes: `mov eax,[esp+4]; movsx eax,word [eax*2+0xC88004]; ret`). Devuelve la palabra con signo de
// 0xC88004 + indice * 2 sin comprobar el indice. Lo llama ClothesBuilder (0x5A422A).
__declspec(noinline) int32 __cdecl TxdStoreDefaultSlotExe(int32 index) {
    return *(reinterpret_cast<const int16*>(CTxdStore::defaultTxds.data()) + index);
}
} // namespace

void CTxdStore::InjectHooks() {
    RH_ScopedClass(CTxdStore);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(PushCurrentTxd, 0x7316A0);
    RH_ScopedInstall(PopCurrentTxd, 0x7316B0);
    RH_ScopedOverloadedInstall(FindTxdSlot, "name", 0x731850, int32 (*)(const char*));
    RH_ScopedOverloadedInstall(FindTxdSlot, "hash", 0x7318E0, int32 (*)(uint32));
    RH_ScopedInstall(StartLoadTxd, 0x731930);
    RH_ScopedInstall(Create, 0x731990);
    RH_ScopedInstall(SetCurrentTxd, 0x7319C0);
    RH_ScopedInstall(AddRef, 0x731A00);
    RH_ScopedInstall(RemoveRef, 0x731A30);
    RH_ScopedInstall(RemoveRefWithoutDelete, 0x731A70);
    RH_ScopedInstall(GetNumRefs, 0x731AA0);
    RH_ScopedInstall(AddTxdSlot, 0x731C80);
    RH_ScopedInstall(RemoveTxdSlot, 0x731CD0);
    RH_ScopedOverloadedInstall(LoadTxd, "stream", 0x731DD0, bool (*)(int32, RwStream*));
    RH_ScopedOverloadedInstall(LoadTxd, "filename", 0x7320B0, bool (*)(int32, const char*));
    RH_ScopedInstall(FinishLoadTxd, 0x731E40);
    RH_ScopedInstall(RemoveTxd, 0x731E90);
    RH_ScopedInstall(Initialise, 0x731F20);
    RH_ScopedInstall(Shutdown, 0x732000);
    RH_ScopedInstall(GameShutdown, 0x732060);
    RH_ScopedInstall(GetParentTxdSlot, 0x408370);
    RH_ScopedInstall(GetTxd, 0x408340);
    RH_ScopedInstall(TxdStoreFindCB, 0x731720);
    RH_ScopedInstall(TxdStoreLoadCB, 0x731710);
    RH_ScopedInstall(SetupTxdParent, 0x731D50);
    RH_ScopedInstall(PluginAttach, 0x731650); // lote 166A (banco Fn166A)

    RH_ScopedNamedGlobalInstall(TxdStoreDefaultSlotExe, "GetDefaultTxdSlot", 0x7316D0);            // lote 181A (sin validar)
    RH_ScopedNamedGlobalInstall(TxdPoolFlush, "PoolFlush", 0x731800);                              // lote 181A (sin validar)
    RH_ScopedNamedGlobalInstall(TxdPoolConstructor, "PoolConstructor", 0x731AD0);                  // lote 181A (sin validar)
    RH_ScopedNamedGlobalInstall(TxdPoolFlushSecondCopy, "PoolFlushSecondCopy", 0x731B30);          // lote 181A (sin validar)
    RH_ScopedNamedGlobalInstall(TxdPoolNew, "PoolNew", 0x731B80);                                  // lote 181A (sin validar)
    RH_ScopedNamedGlobalInstall(TxdPoolScalarDeletingDestructor, "PoolScalarDeletingDestructor", 0x731C20); // lote 181A (sin validar)

    // global
    RH_ScopedGlobalInstall(RemoveIfRefCountIsGreaterThanOne, 0x731680);
}

// initialise txd store
// 0x731F20
void CTxdStore::Initialise() {
    if (!ms_pTxdPool) {
        // Como el original: operator new de 0x14 bytes y el constructor del pool (0x731AD0): objetos y mapa con
        // operator new sin relleno del almacenamiento, cada byte del mapa en 0x80; el byte +0x11 no se escribe.
        auto* const pool = static_cast<uint8*>(::operator new(sizeof(CTxdPool)));
        if (pool) {
            *reinterpret_cast<void**>(pool + 0x0) = ::operator new(TOTAL_TXD_MODEL_IDS * sizeof(TxdDef));
            auto* const flags                     = static_cast<uint8*>(::operator new(TOTAL_TXD_MODEL_IDS));
            *reinterpret_cast<uint8**>(pool + 0x4) = flags;
            pool[0x10]                             = 1;
            *reinterpret_cast<int32*>(pool + 0x8)  = TOTAL_TXD_MODEL_IDS;
            *reinterpret_cast<int32*>(pool + 0xC)  = -1;
            for (auto i = 0u; i < TOTAL_TXD_MODEL_IDS; i++) {
                flags[i] = 0x80;
            }
        }
        ms_pTxdPool = reinterpret_cast<CTxdPool*>(pool);
    }

    // Como el original: AddTxdSlot en línea (sin el registro de la DLL, que reserva memoria en el backtracer de spdlog)
    for (auto& slot : defaultTxds) {
        TxdDef* txd          = ms_pTxdPool->New();
        txd->m_pRwDictionary = nullptr;
        txd->m_wRefsCount    = 0;
        txd->m_wParentIndex  = -1;
        txd->m_hash          = CKeyGen::GetUppercaseKey("*");
        slot                 = static_cast<int16>(ms_pTxdPool->GetIndex(txd));
    }

    RwTextureSetFindCallBack(TxdStoreFindCB);
    RwTextureSetReadCallBack(TxdStoreLoadCB);
}

// shutdown txd store
// 0x732000
void CTxdStore::Shutdown() {
    // Como el original: sin el destructor de CPool (que rellena y deja otros valores); se ponen a cero
    // almacenamiento, mapa, capacidad y primer libre, y solo se libera la memoria propia del pool.
    // El puntero global no se borra.
    if (auto* const pool = reinterpret_cast<uint8*>(ms_pTxdPool)) {
        if (*reinterpret_cast<int32*>(pool + 0x8) > 0) {
            if (pool[0x10]) {
                ::operator delete(*reinterpret_cast<void**>(pool + 0x0));
                ::operator delete(*reinterpret_cast<void**>(pool + 0x4));
            }
            *reinterpret_cast<void**>(pool + 0x0) = nullptr;
            *reinterpret_cast<void**>(pool + 0x4) = nullptr;
            *reinterpret_cast<int32*>(pool + 0x8) = 0;
            *reinterpret_cast<int32*>(pool + 0xC) = 0;
        }
        ::operator delete(pool);
    }
}

// 0x732060
void CTxdStore::GameShutdown() {
    for (int32 i = 0; i < TOTAL_TXD_MODEL_IDS; i++) {
        TxdDef* txd = ms_pTxdPool->GetAt(i);
        if (!txd || txd->m_wRefsCount)
            continue;
        RemoveTxdSlot(i);
    }
}

// 0x731930
bool CTxdStore::StartLoadTxd(int32 index, RwStream* stream) {
    // Como el original: sin comprobar el slot (uno libre da nullptr y el EXE escribe en él igualmente)
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!RwStreamFindChunk(stream, rwID_TEXDICTIONARY, nullptr, nullptr))
        return false;

    RwTexDictionary* texdic = RwTexDictionaryGtaStreamRead1(stream);
    txd->m_pRwDictionary = texdic;
    return texdic != nullptr;
}

// 0x731E40
bool CTxdStore::FinishLoadTxd(int32 index, RwStream* stream) {
    // Como el original: sin comprobar el slot
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    txd->m_pRwDictionary = RwTexDictionaryGtaStreamRead2(stream, txd->m_pRwDictionary);
    if (txd->m_pRwDictionary)
        SetupTxdParent(index);
    return txd->m_pRwDictionary != nullptr;
}

// load txd from stream
// 0x731DD0
bool CTxdStore::LoadTxd(int32 index, RwStream* stream) {
    // Como el original: sin comprobar el slot; el chunk se busca siempre
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!RwStreamFindChunk(stream, rwID_TEXDICTIONARY, nullptr, nullptr))
        return false;
    txd->m_pRwDictionary = RwTexDictionaryGtaStreamRead(stream);
    if (txd->m_pRwDictionary)
        SetupTxdParent(index);
    return txd->m_pRwDictionary != nullptr;
}

// load txd from file
// 0x7320B0
bool CTxdStore::LoadTxd(int32 index, const char* filename) {
    ZoneScoped;

    char data[260];
    RwStream* stream;
    std::sprintf(data, "%s", filename); // Como el original: sprintf (0x821BB5), sin el relleno de sprintf_s
    do {
        stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, data);
    } while (!stream);
    bool loaded = LoadTxd(index, stream);
    RwStreamClose(stream, nullptr);
    return loaded;
}

// store current txd
// 0x7316A0
void CTxdStore::PushCurrentTxd() {
    ms_pStoredTxd = RwTexDictionaryGetCurrent();
}

// restore txd
// 0x7316B0
void CTxdStore::PopCurrentTxd() {
    RwTexDictionarySetCurrent(ms_pStoredTxd);
    ms_pStoredTxd = nullptr;
}

// set the txd to work with
// 0x7319C0
void CTxdStore::SetCurrentTxd(int32 index) {
    // Como el original: sin comprobar el slot
    RwTexDictionarySetCurrent(ms_pTxdPool->GetAt(index)->m_pRwDictionary);
}

// find txd by name. Returning value is txd index
// 0x731850
int32 CTxdStore::FindTxdSlot(const char* name) {
    uint32 key = CKeyGen::GetUppercaseKey(name);
    int32 last = ms_lastSlotFound;

    for (; last >= 0; last--) {
        TxdDef* txd = ms_pTxdPool->GetAt(last);
        if (txd && txd->m_hash == key)
            break;
    }

    if (last < 0) {
        last = ms_lastSlotFound;
        for (last++;; last++) {
            if (last >= 0 && static_cast<size_t>(last) >= ms_pTxdPool->GetSize())
                return -1;

            TxdDef* txd = ms_pTxdPool->GetAt(last);
            if (txd && txd->m_hash == key)
                break;
        }
    }

    ms_lastSlotFound = last;
    return last;
}

// find txd by name hash. Returning value is txd index
// 0x7318E0
int32 CTxdStore::FindTxdSlot(uint32 hash) {
    for (auto&& [i, txd] : ms_pTxdPool->GetAllValidWithIndex()) {
        if (txd.m_hash == hash)
            return i;
    }
    return -1;
}

// plugin-sdk has named it to getTexDictionary
// 0x408340
RwTexDictionary* CTxdStore::GetTxd(int32 index) {
    // Como el original: sin comprobar el slot
    return ms_pTxdPool->GetAt(index)->m_pRwDictionary;
}

// plugin-sdk has named it to getTXDEntryParentIndex
// 0x408370
int32 CTxdStore::GetParentTxdSlot(int32 index) {
    // Como el original: sin comprobar el slot
    return ms_pTxdPool->GetAt(index)->m_wParentIndex;
}

// create rw tex dictionary for txd slot
// 0x731990
void CTxdStore::Create(int32 index) {
    NOTSA_LOG_DEBUG("Creando diccionario de texturas (TXD) en la ranura {}", index);
    // Como el original: sin comprobar el slot (RwTexDictionaryCreate se llama siempre)
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    txd->m_pRwDictionary = RwTexDictionaryCreate();
}

// allocate new slot for this txd
// 0x731C80
int32 CTxdStore::AddTxdSlot(const char* name) {
    TxdDef* txd = ms_pTxdPool->New();
    txd->m_pRwDictionary = nullptr;
    txd->m_wRefsCount = 0;
    txd->m_wParentIndex = -1;
    txd->m_hash = CKeyGen::GetUppercaseKey(name);

    const auto i = ms_pTxdPool->GetIndex(txd);
    NOTSA_LOG_TRACE("CTxdStore::AddTxdSlot({:?}) -> {}", name, i);
    return i;
}

// remove txd slot
// 0x731CD0
void CTxdStore::RemoveTxdSlot(int32 index) {
    NOTSA_LOG_TRACE("CTxdStore::RemoveTxdSlot({})", index);
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return;

    if (txd->m_pRwDictionary)
        RwTexDictionaryDestroy(txd->m_pRwDictionary);
    ms_pTxdPool->Delete(txd);
}

// remove txd
// 0x731E90
void CTxdStore::RemoveTxd(int32 index) {
    NOTSA_LOG_TRACE("CTxdStore::RemoveTxd({})", index);
    // Como el original: sin comprobar el slot
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd->m_pRwDictionary) {
        RwTexDictionaryForAllTextures(txd->m_pRwDictionary, RemoveIfRefCountIsGreaterThanOne, nullptr);
        RwTexDictionaryDestroy(txd->m_pRwDictionary);
    }
    if (txd->m_wParentIndex != -1)
        RemoveRef(txd->m_wParentIndex);

    txd->m_pRwDictionary = nullptr;
}

// add reference for txd
// 0x731A00
void CTxdStore::AddRef(int32 index) {
    // Como el original: sin comprobar el slot
    ms_pTxdPool->GetAt(index)->m_wRefsCount++;
}

// remove reference, if references count is 0, then delete txd
// 0x731A30
void CTxdStore::RemoveRef(int32 index) {
    // Como el original: sin comprobar el slot
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (--txd->m_wRefsCount <= 0)
        CStreaming::RemoveModel(TXDToModelId(index));
}

// remove reference without deleting
// 0x731A70
void CTxdStore::RemoveRefWithoutDelete(int32 index) {
    // Como el original: sin comprobar el slot
    ms_pTxdPool->GetAt(index)->m_wRefsCount--;
}

// get references count
// 0x731AA0
int32 CTxdStore::GetNumRefs(int32 index) {
    // Como el original: sin comprobar el slot
    return ms_pTxdPool->GetAt(index)->m_wRefsCount;
}

// 0x731D50
void CTxdStore::SetupTxdParent(int32 index) {
    // Como el original: sin comprobar ninguno de los dos slots
    TxdDef* txdcur = ms_pTxdPool->GetAt(index);
    if (txdcur->m_wParentIndex == -1)
        return;
    TxdDef* txdpar = ms_pTxdPool->GetAt(txdcur->m_wParentIndex);

    SetTxdParent(txdcur->m_pRwDictionary, txdpar->m_pRwDictionary);
    AddRef(txdcur->m_wParentIndex);
}

// 0x731720
RwTexture* CTxdStore::TxdStoreFindCB(const char* name) {
    RwTexDictionary* txd = RwTexDictionaryGetCurrent();
    for (; txd; txd = GetTxdParent(txd)) {
        RwTexture* tex = RwTexDictionaryFindNamedTexture(txd, name);
        if (tex)
            return tex;
    }
    return nullptr;
}

// 0x731710
RwTexture* CTxdStore::TxdStoreLoadCB(const char* name, const char* mask) {
    return nullptr;
}

// 0x731680
RwTexture* RemoveIfRefCountIsGreaterThanOne(RwTexture* texture, void* data) {
    if (texture->refCount > 1) {
        RwTextureDestroy(texture);
        RwTexDictionaryRemoveTexture(texture);
    }
    return texture;
}
