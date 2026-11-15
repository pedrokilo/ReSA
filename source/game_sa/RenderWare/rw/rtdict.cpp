/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RtDict
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rtdict.h"

static_assert(offsetof(RtDictSchema, dictChunkType) == 0x04 && offsetof(RtDictSchema, entryChunkType) == 0x08 && offsetof(RtDictSchema, compatibilityVersion) == 0x0C);
static_assert(offsetof(RtDictSchema, dictionaries) == 0x10 && offsetof(RtDictSchema, current) == 0x14 && offsetof(RtDictSchema, addRefCB) == 0x18);
static_assert(offsetof(RtDictSchema, destroyCB) == 0x1C && offsetof(RtDictSchema, getNameCB) == 0x20 && offsetof(RtDictSchema, streamGetSizeCB) == 0x24);
static_assert(offsetof(RtDictSchema, streamReadCB) == 0x28 && offsetof(RtDictSchema, streamReadCompatibilityCB) == 0x2C && offsetof(RtDictSchema, streamWriteCB) == 0x30);
static_assert(offsetof(RwGlobals, stringFuncs) + offsetof(RwStringFunctions, vecStrcmp) == 0x114);
static_assert(offsetof(RwGlobals, memoryFuncs) + offsetof(RwMemoryFunctions, rwmalloc) == 0x134);
static_assert(offsetof(RwGlobals, memoryFuncs) + offsetof(RwMemoryFunctions, rwfree) == 0x138);

namespace {
// Pistas de memoria del EXE: dictChunkType | rwMEMHINTDUR_GLOBAL (0x40000) para la lista de diccionarios del esquema y | rwMEMHINTDUR_EVENT (0x30000) para el resto.
constexpr RwUInt32 kHintGlobal = 0x40000;
constexpr RwUInt32 kHintEvent  = 0x30000;
// Version y numero de compilacion que escribe RtDictStreamWrite en las cabeceras de chunk (push 36003h / push 0FFFFh).
constexpr RwUInt32 kStreamVersion = 0x36003;
constexpr RwUInt32 kStreamBuild   = 0xFFFF;
// pluginID del RwError de RtDictSchemaStreamReadDict (literal 0x1BF) y codigo E_RW_NOMEM.
constexpr RwInt32 kDictErrorPluginID = 0x1BF;
constexpr RwInt32 kErrNoMem          = static_cast<RwInt32>(0x80000013);
} // namespace

void RtDictInjectHooks() {
    RH_ScopedNamespaceName("RtDict");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RtDictSchemaInit, 0x7CED40); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(RtDictSchemaDestruct, 0x7CED70);
    RH_ScopedGlobalInstall(RtDictSchemaGetCurrentDict, 0x7CEEE0);
    RH_ScopedGlobalInstall(RtDictSchemaSetCurrentDict, 0x7CEEF0); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(_rtDictDestruct, 0x7CEF60); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(RtDictFindNamedEntry, 0x7CEFE0); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(RtDictForAllEntries, 0x7CF060); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(RtDictRemoveEntry, 0x7CF0C0); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(RtDictDestroy, 0x7CF130); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(RtDictSchemaStreamReadDict, 0x7CF240); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(RtDictStreamWrite, 0x7CF490); // lote R1A (sin validar)
    RH_ScopedGlobalInstall(RtDictSchemaForAllDictionaries, 0x7CF5B0); // lote R1A (sin validar)
}

// 0x7CED40: crea la lista de diccionarios (4 bytes por entrada, pista dictChunkType | GLOBAL). Si falla devuelve NULL sin tocar mas; si no, current = NULL y devuelve el esquema.
RtDictSchema* RtDictSchemaInit(RtDictSchema* schema) {
    schema->dictionaries = _rwSListCreate(4, schema->dictChunkType | kHintGlobal);
    if (!schema->dictionaries) {
        return nullptr;
    }
    schema->current = nullptr;
    return schema;
}

// 0x7CED70: destruye la lista de diccionarios (schema->dictionaries, +0x10) con _rwSListDestroy (0x809440), ignora su resultado y devuelve TRUE.
RwBool RtDictSchemaDestruct(RtDictSchema* schema) {
    _rwSListDestroy(schema->dictionaries);
    return TRUE;
}

// 0x7CF240: lee un diccionario del flujo. Cabecera STRUCT (longitud y version); si version < compatibilityVersion delega en streamReadCompatibilityCB. Lee numEntries con RwStreamRead
// pidiendo `length` bytes (como el EXE: el destino es de 4 bytes, asi que un chunk con length > 4 desbordaria igual que en el original). Reserva RtDict (8 B), su lista de entradas y
// lo registra en la lista del esquema; luego, por cada entrada, busca el chunk entryChunkType, la lee con streamReadCB, la anade, le suma una referencia (addRefCB) y suelta la nominal
// (destroyCB). Cualquier fallo tras el registro deshace con la misma secuencia que RtDictDestroy (0x7CF3CA..0x7CF46F) y devuelve NULL.
RtDict* RtDictSchemaStreamReadDict(RtDictSchema* schema, RwStream* stream) {
    RwUInt32 length  = 0;
    RwUInt32 version = 0;
    if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version)) {
        return nullptr;
    }

    if (version < schema->compatibilityVersion) {
        return schema->streamReadCompatibilityCB(stream);
    }

    RwUInt32 numEntries = 0;
    if (!RwStreamRead(stream, &numEntries, length)) {
        return nullptr;
    }

    auto* dict = static_cast<RtDict*>(RwMalloc(sizeof(RtDict), schema->dictChunkType | kHintEvent));
    if (!dict) {
        RwError error;
        error.pluginID  = kDictErrorPluginID;
        error.errorCode = _rwerror(kErrNoMem, static_cast<RwUInt32>(sizeof(RtDict)));
        RwErrorSet(&error);
        return nullptr;
    }

    dict->entries = _rwSListCreate(4, schema->dictChunkType | kHintEvent);
    if (!dict->entries) {
        RwFree(dict);
        return nullptr;
    }

    dict->schema = schema;
    auto* const slot = static_cast<RtDictEntryType*>(_rwSListGetNewEntry(schema->dictionaries, schema->dictChunkType | kHintEvent));
    if (!slot) {
        _rwSListDestroy(dict->entries);
        RwFree(dict);
        return nullptr;
    }
    *slot = dict;

    while (numEntries) {
        --numEntries;
        if (!RwStreamFindChunk(stream, schema->entryChunkType, &length, &version)) {
            RtDictDestroy(dict);
            return nullptr;
        }

        RtDictEntryType entry = schema->streamReadCB(stream);
        if (!entry) {
            RtDictDestroy(dict);
            return nullptr;
        }

        auto* const entrySlot = static_cast<RtDictEntryType*>(_rwSListGetNewEntry(dict->entries, dict->schema->dictChunkType | kHintEvent));
        *entrySlot = entry;
        dict->schema->addRefCB(entry);
        schema->destroyCB(entry);
    }
    return dict;
}

RtDict* RtDictSchemaCreateDict(RtDictSchema* schema) {
    return ((RtDict*(__cdecl *)(RtDictSchema*))0x7CED90)(schema);
}

RtDictSchema* RtDictSchemaAddDict(RtDictSchema* schema, RtDict* dict) {
    return ((RtDictSchema*(__cdecl *)(RtDictSchema*, RtDict*))0x7CEE50)(schema, dict);
}

RtDictSchema* RtDictSchemaRemoveDict(RtDictSchema* schema, RtDict* dict) {
    return ((RtDictSchema*(__cdecl *)(RtDictSchema*, RtDict*))0x7CEE80)(schema, dict);
}

// 0x7CF5B0: aplica callback(dict, data) a cada diccionario del esquema (end antes que begin, como el EXE). Para en cuanto el callback devuelve NULL. Devuelve TRUE siempre: el EXE
// termina en `mov eax,1` tanto si recorre toda la lista como si sale antes (los dos caminos convergen).
RwBool RtDictSchemaForAllDictionaries(RtDictSchema* schema, RtDictCallBack* callback, RtDictEntryType data) {
    auto** const end = static_cast<RtDict**>(_rwSListGetEnd(schema->dictionaries));
    auto** it        = static_cast<RtDict**>(_rwSListGetBegin(schema->dictionaries));
    if (it != end) {
        do {
            RtDict* const dict = *it++;
            if (!callback(dict, data)) {
                break;
            }
        } while (it != end);
    }
    return TRUE;
}

// 0x7CEEE0: devuelve schema->current (+0x14).
RtDict* RtDictSchemaGetCurrentDict(RtDictSchema* schema) {
    return schema->current;
}

// 0x7CEEF0: schema->current = dict; devuelve el esquema.
RtDictSchema* RtDictSchemaSetCurrentDict(RtDictSchema* schema, RtDict* dict) {
    schema->current = dict;
    return schema;
}

// 0x7CF130: si es el diccionario actual del esquema lo deja en NULL; lo quita de la lista del esquema (begin antes que end; el hueco se rellena con el contenido de la ultima ranura
// y se descarta 1 entrada del final); llama a destroyCB por cada entrada propia (end antes que begin), destruye la lista y libera el RtDict con RwFree. Devuelve TRUE.
RwBool RtDictDestroy(RtDict* dictionary) {
    if (dictionary == dictionary->schema->current) {
        dictionary->schema->current = nullptr;
    }

    RtDictSchema* const schema = dictionary->schema;
    auto** it                  = static_cast<RtDict**>(_rwSListGetBegin(schema->dictionaries));
    auto** const end           = static_cast<RtDict**>(_rwSListGetEnd(schema->dictionaries));
    for (; it != end; ++it) {
        if (*it == dictionary) {
            const RwInt32 last = _rwSListGetNumEntries(schema->dictionaries) - 1;
            *it                = *static_cast<RtDict**>(_rwSListGetEntry(schema->dictionaries, last));
            _rwSListDestroyEndEntries(schema->dictionaries, 1);
            break;
        }
    }

    auto** const entriesEnd = static_cast<RtDictEntryType*>(_rwSListGetEnd(dictionary->entries));
    auto** entry            = static_cast<RtDictEntryType*>(_rwSListGetBegin(dictionary->entries));
    if (entry != entriesEnd) {
        do {
            RtDictEntryType const e = *entry++;
            dictionary->schema->destroyCB(e);
        } while (entry != entriesEnd);
    }
    _rwSListDestroy(dictionary->entries);
    RwFree(dictionary);
    return TRUE;
}

RtDictEntryType RtDictAddEntry(RtDict* dictionary, RtDictEntryType entry) {
    return ((RtDictEntryType(__cdecl *)(RtDict*, RtDictEntryType))0x7CEFB0)(dictionary, entry);
}

// 0x7CEFE0: lee getNameCB del esquema antes de recorrer (end antes que begin) y devuelve la primera entrada cuyo nombre coincide (rwstrcmp(getNameCB(entry), name) == 0), o NULL.
// rwstrcmp se relee del motor (RwEngineInstance + 0x114) en cada vuelta, como el EXE.
RtDictEntryType RtDictFindNamedEntry(RtDict* dictionary, const RwChar* name) {
    RtDictEntryGetNameCallBack* const getName = dictionary->schema->getNameCB;
    auto** const end                          = static_cast<RtDictEntryType*>(_rwSListGetEnd(dictionary->entries));
    auto** it                                 = static_cast<RtDictEntryType*>(_rwSListGetBegin(dictionary->entries));
    for (; it != end; ++it) {
        RtDictEntryType const entry = *it;
        const RwChar* const entryName = getName(entry);
        if (rwstrcmp(entryName, name) == 0) {
            return entry;
        }
    }
    return nullptr;
}

// 0x7CF060: aplica callBack(entry, data) a cada entrada (end antes que begin); sale al primer NULL. Devuelve siempre el diccionario recibido.
const RtDict* RtDictForAllEntries(const RtDict* dictionary, RtDictEntryCallBack* callBack, RtDictEntryType data) {
    auto** const end = static_cast<RtDictEntryType*>(_rwSListGetEnd(dictionary->entries));
    auto** it        = static_cast<RtDictEntryType*>(_rwSListGetBegin(dictionary->entries));
    if (it != end) {
        do {
            RtDictEntryType const entry = *it++;
            if (!callBack(entry, data)) {
                break;
            }
        } while (it != end);
    }
    return dictionary;
}

// 0x7CF0C0: busca la entrada (begin y luego end en el EXE). Si no esta devuelve NULL. Si esta: destroyCB(*it); luego *it = _rwSListGetEntry(entries, _rwSListGetNumEntries(entries))
// (SIN restar 1 y SIN desreferenciar, tal cual el desensamblado 0x7CF0FE..0x7CF114: guarda la direccion de la ranura "uno pasado el final", no el contenido de la ultima; difiere de
// RtDictDestroy, que si resta 1 y desreferencia) y _rwSListDestroyEndEntries(entries, 1). Devuelve el diccionario.
RtDict* RtDictRemoveEntry(RtDict* dictionary, RtDictEntryType entry) {
    auto** const end = static_cast<RtDictEntryType*>(_rwSListGetEnd(dictionary->entries));
    auto** it        = static_cast<RtDictEntryType*>(_rwSListGetBegin(dictionary->entries));
    if (it == end) {
        return nullptr;
    }
    while (*it != entry) {
        ++it;
        if (it == end) {
            return nullptr;
        }
    }

    dictionary->schema->destroyCB(*it);
    *it = _rwSListGetEntry(dictionary->entries, _rwSListGetNumEntries(dictionary->entries));
    _rwSListDestroyEndEntries(dictionary->entries, 1);
    return dictionary;
}

// 0x7CF490: tamano = 0x10 + sum(0xC + streamGetSizeCB(entry)) (end antes que begin); si el total fuera 0 (envoltura de 32 bits) devuelve NULL. Escribe la cabecera del diccionario
// (dictChunkType, tamano, version 0x36003, build 0xFFFF), la cabecera STRUCT de 4 bytes, el numero de entradas con RwStreamWrite(.., 4) y cada entrada con streamWriteCB(entry, stream);
// cualquier fallo devuelve NULL; si todo va bien devuelve el diccionario. El conteo se recorre aparte.
const RtDict* RtDictStreamWrite(const RtDict* dictionary, RwStream* stream) {
    RwUInt32 size = 0x10;
    {
        auto** const end = static_cast<RtDictEntryType*>(_rwSListGetEnd(dictionary->entries));
        auto** it        = static_cast<RtDictEntryType*>(_rwSListGetBegin(dictionary->entries));
        if (it != end) {
            do {
                RtDictEntryType const entry = *it;
                const RwUInt32 entrySize    = dictionary->schema->streamGetSizeCB(entry);
                ++it;
                size = size + entrySize + 0xC;
            } while (it != end);
            if (size == 0) {
                return nullptr;
            }
        }
    }

    if (!_rwStreamWriteVersionedChunkHeader(stream, dictionary->schema->dictChunkType, size, kStreamVersion, kStreamBuild)) {
        return nullptr;
    }
    if (!_rwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4, kStreamVersion, kStreamBuild)) {
        return nullptr;
    }

    RwUInt32 numEntries = 0;
    {
        auto** const end = static_cast<RtDictEntryType*>(_rwSListGetEnd(dictionary->entries));
        for (auto** it = static_cast<RtDictEntryType*>(_rwSListGetBegin(dictionary->entries)); it != end; ++it) {
            ++numEntries;
        }
    }
    if (!RwStreamWrite(stream, &numEntries, 4)) {
        return nullptr;
    }

    auto** const end = static_cast<RtDictEntryType*>(_rwSListGetEnd(dictionary->entries));
    auto** it        = static_cast<RtDictEntryType*>(_rwSListGetBegin(dictionary->entries));
    if (it != end) {
        do {
            RtDictEntryType const entry = *it;
            if (!dictionary->schema->streamWriteCB(entry, stream)) {
                return nullptr;
            }
            ++it;
        } while (it != end);
    }
    return dictionary;
}

RwUInt32 RtDictStreamGetSize(const RtDict* dictionary) {
    return ((RwUInt32(__cdecl *)(const RtDict*))0x7CF1F0)(dictionary);
}

RtDict* _rtDictSchemaInitDict(RtDictSchema* schema, RtDict* dict) {
    return ((RtDict*(__cdecl *)(RtDictSchema*, RtDict*))0x7CEF00)(schema, dict);
}

// 0x7CEF60: llama a destroyCB por cada entrada (end antes que begin), destruye la lista de entradas con _rwSListDestroy y devuelve TRUE. No toca el esquema ni libera el RtDict.
RwBool _rtDictDestruct(RtDict* dictionary) {
    auto** const end = static_cast<RtDictEntryType*>(_rwSListGetEnd(dictionary->entries));
    auto** it        = static_cast<RtDictEntryType*>(_rwSListGetBegin(dictionary->entries));
    if (it != end) {
        do {
            RtDictEntryType const entry = *it++;
            dictionary->schema->destroyCB(entry);
        } while (it != end);
    }
    _rwSListDestroy(dictionary->entries);
    return TRUE;
}
