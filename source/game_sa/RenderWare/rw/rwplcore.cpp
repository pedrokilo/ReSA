/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RwPlCore
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rwplcore.h"
#include "X87Math.h"

namespace {
// 0x8019B0: RwFreeListCreateAndPreallocateSpace real (el simbolo publico 0x801B70 es un JMP a esta direccion); sin reconstruir.
RwFreeList* FreeListCreateAndPreallocateSpaceExe(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint) {
    return ((RwFreeList*(__cdecl *)(RwInt32, RwInt32, RwInt32, RwInt32, RwFreeList*, RwUInt32))0x8019B0)(entrySize, entriesPerBlock, alignment, numBlocksToPreallocate, inPlaceSpaceForFreeListStruct, hint);
}
} // namespace

// Biblioteca RenderWare (reconstruidas en C++, banco Dependencies25D)
void RwPlCoreInjectHooks25d() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(RwFreeListCreate, 0x801980);
    RH_ScopedGlobalInstall(RwFreeListCreateAndPreallocateSpace, 0x801B70);
}

void RwPlCoreInjectHooks() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    // Funciones de biblioteca (RenderWare) de <= 40 bytes reconstruidas en C++ (lote 25, grupo g25e).
    RH_ScopedGlobalInstall(_rwPluginRegistryGetPluginOffset, 0x808470);
    RH_ScopedGlobalInstall(_rwSListDestroyEndEntries, 0x809400);
    RH_ScopedGlobalInstall(_rwSListGetNumEntries, 0x8094B0);
    RH_ScopedGlobalInstall(_rwSListGetEntry, 0x809510);
    RH_ScopedGlobalInstall(_rwSListGetBegin, 0x809530);
    RH_ScopedGlobalInstall(_rwSListGetEnd, 0x809540);
    RH_ScopedGlobalInstall(_rwStringClose, 0x80A440);
    RH_ScopedGlobalInstall(_rwStringStreamGetSize, 0x80A470);
}

RwMemoryFunctions* RwOsGetMemoryInterface() {
    return ((RwMemoryFunctions*(__cdecl *)(void))0x802230)();
}

// 0x801980: RwFreeListCreate de la compilacion sin RWDEBUG (4 argumentos de pila; la version con fileCreate/lineCreate es solo de depuracion):
// reserva con un bloque preasignado y sin espacio propio para la estructura.
RwFreeList* RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint) {
    return FreeListCreateAndPreallocateSpaceExe(entrySize, entriesPerBlock, alignment, 1, nullptr, hint);
}

// 0x801B70: salto directo a 0x8019B0 con los mismos seis argumentos.
RwFreeList* RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint) {
    return FreeListCreateAndPreallocateSpaceExe(entrySize, entriesPerBlock, alignment, numBlocksToPreallocate, inPlaceSpaceForFreeListStruct, hint);
}

RwBool RwFreeListDestroy(RwFreeList* freelist) {
    return ((RwBool(__cdecl *)(RwFreeList*))0x801B80)(freelist);
}

void RwFreeListSetFlags(RwFreeList* freeList, RwUInt32 flags) {
    ((void(__cdecl *)(RwFreeList*, RwUInt32))0x801C10)(freeList, flags);
}

RwUInt32 RwFreeListGetFlags(RwFreeList* freeList) {
    return ((RwUInt32(__cdecl *)(RwFreeList*))0x801C20)(freeList);
}

RwInt32 RwFreeListPurge(RwFreeList* freelist) {
    return ((RwInt32(__cdecl *)(RwFreeList*))0x801E00)(freelist);
}

RwFreeList* RwFreeListForAllUsed(RwFreeList* freelist, RwFreeListCallBack fpCallBack, void* data) {
    return ((RwFreeList*(__cdecl *)(RwFreeList*, RwFreeListCallBack, void*))0x801E90)(freelist, fpCallBack, data);
}

RwInt32 RwFreeListPurgeAllFreeLists() {
    return ((RwInt32(__cdecl *)(void))0x801F90)();
}

void RwStreamSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7EC760)(blockSize, numBlocksToPrealloc);
}

// Lote R11A (sin validar): RwStreamRead y RwStreamWrite. Funciones de fichero del motor (RwEngineInstance->fileFuncs, 11 punteros desde +0xC4): fread = [3] (+0xD0), fwrite = [4] (+0xD4), feof = [7] (+0xE0), todas cdecl.
// Union del flujo en +0xC: fichero { fp }, memoria { position +0xC, nSize +0x10, memBlock +0x14 }, personalizado { close +0xC, read +0x10, write +0x14, skip +0x18, data +0x1C }.
namespace {
static_assert(offsetof(RwStream, Type) == 0xC && offsetof(RwStream, Type.memory.nSize) == 0x10 && offsetof(RwStream, Type.memory.memBlock) == 0x14 && offsetof(RwStream, Type.custom.data) == 0x1C);
static_assert(offsetof(RwGlobals, fileFuncs) == 0xC4);
using RwFileReadFn  = size_t(__cdecl*)(void*, size_t, size_t, void*);
using RwFileWriteFn = size_t(__cdecl*)(const void*, size_t, size_t, void*);
using RwFileEofFn   = int(__cdecl*)(void*);
// Mensaje de error del EXE: RwError { pluginID = 1, errorCode = _rwerror(codigo[, extra]) } y RwErrorSet (0x808820).
void StreamRaiseError(RwInt32 code) {
    RwError error;
    error.pluginID  = 1;
    error.errorCode = _rwerror(code);
    RwErrorSet(&error);
}
void StreamRaiseError(RwInt32 code, RwInt32 extra) {
    RwError error;
    error.pluginID  = 1;
    error.errorCode = _rwerror(code, extra);
    RwErrorSet(&error);
}
}

// 0x7EC9D0 (lote R11A, sin validar; 332 bytes): tipos 1 y 2 (fichero): fread(buffer, 1, length, fp); si devuelve menos: feof -> error 5, si no 0x8000001A; devuelve lo leido en cualquier caso. Tipo 3 (memoria):
// disponible = nSize - position; si length (sin signo) es mayor, error 5 y se lee solo lo disponible; copia desde memBlock + position, avanza position y devuelve lo copiado. Tipo 4 (personalizado):
// read(data, buffer, length) y devuelve su resultado. Cualquier otro tipo: error 0xE y devuelve 0.
RwUInt32 RwStreamRead(RwStream* stream, void* buffer, RwUInt32 length) {
    switch (stream->type) {
    case rwSTREAMFILE:
    case rwSTREAMFILENAME: {
        void* const file = stream->Type.file.fpFile;
        const RwUInt32 bytesRead = static_cast<RwUInt32>(reinterpret_cast<RwFileReadFn>(RwEngineInstance->fileFuncs[3])(buffer, 1, length, file));
        if (bytesRead != length) {
            const int endOfFile = reinterpret_cast<RwFileEofFn>(RwEngineInstance->fileFuncs[7])(file);
            if (endOfFile != 0) {
                StreamRaiseError(5);
                return bytesRead;
            }
            StreamRaiseError(static_cast<RwInt32>(0x8000001A));
        }
        return bytesRead;
    }
    case rwSTREAMMEMORY: {
        const RwUInt32 available = stream->Type.memory.nSize - stream->Type.memory.position;
        if (length > available) {
            length = available;
            StreamRaiseError(5);
        }
        memcpy(buffer, stream->Type.memory.memBlock + stream->Type.memory.position, length);
        stream->Type.memory.position += length;
        return length;
    }
    case rwSTREAMCUSTOM:
        return stream->Type.custom.sfnread(stream->Type.custom.data, buffer, length);
    default:
        StreamRaiseError(0xE);
        return 0;
    }
}

// 0x7ECB30 (lote R11A, sin validar; 436 bytes): tipos 1 y 2: fwrite(buffer, 1, length, fp); si devuelve otra cosa, error 0x8000001C y NULL; si no, devuelve el flujo. Tipo 3 (memoria): sin bloque (memBlock NULL)
// reserva 0x200 bytes (RwMalloc, pista 0x30404; NULL: error 0x80000013 con 0x200 y NULL) y nSize = 0x200; si nSize - position < length (sin signo) amplia a nSize + max(0x200, length) con RwRealloc (pista 0x1030404;
// NULL: error 0x80000013 con la diferencia, NULL y el bloque anterior queda como estaba); copia en memBlock + position, avanza position y devuelve el flujo. Tipo 4: write(data, buffer, length) distinto de 0 -> el flujo,
// 0 -> NULL. Cualquier otro tipo: error 0xE y NULL.
RwStream* RwStreamWrite(RwStream* stream, const void* buffer, RwUInt32 length) {
    switch (stream->type) {
    case rwSTREAMFILE:
    case rwSTREAMFILENAME: {
        const RwUInt32 written = static_cast<RwUInt32>(reinterpret_cast<RwFileWriteFn>(RwEngineInstance->fileFuncs[4])(buffer, 1, length, stream->Type.file.fpFile));
        if (written != length) {
            StreamRaiseError(static_cast<RwInt32>(0x8000001C));
            return nullptr;
        }
        return stream;
    }
    case rwSTREAMMEMORY: {
        if (stream->Type.memory.memBlock == nullptr) {
            stream->Type.memory.memBlock = static_cast<RwUInt8*>(RwMalloc(0x200, 0x30404));
            if (stream->Type.memory.memBlock == nullptr) {
                StreamRaiseError(static_cast<RwInt32>(0x80000013), 0x200);
                return nullptr;
            }
            stream->Type.memory.nSize = 0x200;
        }
        const RwUInt32 capacity = stream->Type.memory.nSize;
        if (capacity - stream->Type.memory.position < length) {
            const RwUInt32 newCapacity = capacity + (length < 0x200 ? 0x200 : length);
            RwUInt8* const block = static_cast<RwUInt8*>(RwRealloc(stream->Type.memory.memBlock, newCapacity, 0x1030404));
            if (block == nullptr) {
                StreamRaiseError(static_cast<RwInt32>(0x80000013), static_cast<RwInt32>(newCapacity - stream->Type.memory.nSize));
                return nullptr;
            }
            stream->Type.memory.memBlock = block;
            stream->Type.memory.nSize    = newCapacity;
        }
        memcpy(stream->Type.memory.memBlock + stream->Type.memory.position, buffer, length);
        stream->Type.memory.position += length;
        return stream;
    }
    case rwSTREAMCUSTOM:
        return stream->Type.custom.sfnwrite(stream->Type.custom.data, buffer, length) ? stream : nullptr;
    default:
        StreamRaiseError(0xE);
        return nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R12A (sin validar): abrir, cerrar, saltar y leer cabeceras de flujos de RenderWare (0x7EC810..0x7ED590). Funciones de fichero del motor (fileFuncs, +0xC4):
// fopen = [1] (+0xC8), fclose = [2] (+0xCC), fseek = [8] (+0xE4), ftell = [10] (+0xEC), todas cdecl. Las cadenas de modo estan en el EXE: "rb" 0x8E2220, "wb" 0x8E2224, "ab" 0x8E2228.
// La lista libre de flujos es *(RwFreeList**)(RwEngineInstance + [0xC9792C]); RwFreeListAlloc = memoryAlloc (+0x144) y RwFreeListFree = memoryFree (+0x148).
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
using RwFileOpenFn  = void*(__cdecl*)(const char*, const char*);
using RwFileCloseFn = int(__cdecl*)(void*);
using RwFileSeekFn  = int(__cdecl*)(void*, long, int);
using RwFileTellFn  = long(__cdecl*)(void*);
static_assert(offsetof(RwGlobals, memoryAlloc) == 0x144 && offsetof(RwGlobals, memoryFree) == 0x148);

RwFreeList* R12AStreamFreeList() {
    return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uintptr_t>(RwEngineInstance) + StaticRef<int32>(0xC9792C));
}
void R12AStreamFree(RwStream* stream) {
    RwFreeList* const list = R12AStreamFreeList();
    RwEngineInstance->memoryFree(list, stream);
}

// Parte comun de _rwStreamInitialize (0x7EC84D..0x7EC9BE) y RwStreamOpen (0x7ECF46..0x7ED0A6): rellena la union segun el tipo y devuelve el flujo, o NULL (sin liberarlo).
RwStream* R12AStreamSetup(RwStream* stream, RwStreamType type, RwStreamAccessType accessType, const void* data) {
    switch (type) {
    case rwSTREAMFILE: {
        if (reinterpret_cast<RwFileTellFn>(RwEngineInstance->fileFuncs[10])(const_cast<void*>(data)) == -1) {
            return nullptr;
        }
        stream->Type.file.fpFile = const_cast<void*>(data);
        return stream;
    }
    case rwSTREAMFILENAME: {
        const char* mode;
        switch (accessType) {
        case rwSTREAMREAD:   mode = reinterpret_cast<const char*>(0x8E2220); break;
        case rwSTREAMWRITE:  mode = reinterpret_cast<const char*>(0x8E2224); break;
        case rwSTREAMAPPEND: mode = reinterpret_cast<const char*>(0x8E2228); break;
        default:
            StreamRaiseError(0xD);
            StreamRaiseError(static_cast<RwInt32>(0x80000002), static_cast<RwInt32>(reinterpret_cast<uintptr_t>(data)));
            return nullptr;
        }
        void* const file = reinterpret_cast<RwFileOpenFn>(RwEngineInstance->fileFuncs[1])(static_cast<const char*>(data), mode);
        if (file == nullptr) {
            StreamRaiseError(static_cast<RwInt32>(0x80000002), static_cast<RwInt32>(reinterpret_cast<uintptr_t>(data)));
            return nullptr;
        }
        stream->Type.file.fpFile = file;
        return stream;
    }
    case rwSTREAMMEMORY: {
        switch (accessType) {
        case rwSTREAMREAD: {
            const RwMemory* const memory = static_cast<const RwMemory*>(data);
            stream->Type.memory.position = 0;
            stream->Type.memory.nSize    = memory->length;
            stream->Type.memory.memBlock = memory->start;
            return stream;
        }
        case rwSTREAMWRITE:
            stream->Type.memory.position = 0;
            stream->Type.memory.nSize    = 0;
            stream->Type.memory.memBlock = nullptr;
            return stream;
        case rwSTREAMAPPEND: {
            const RwMemory* const memory = static_cast<const RwMemory*>(data);
            stream->Type.memory.position = memory->length;
            stream->Type.memory.nSize    = memory->length;
            stream->Type.memory.memBlock = memory->start;
            return stream;
        }
        default:
            StreamRaiseError(0xD);
            return nullptr;
        }
    }
    case rwSTREAMCUSTOM:
        memcpy(&stream->Type.custom, data, sizeof(RwStreamCustom));
        return stream;
    default:
        StreamRaiseError(0xE);
        return nullptr;
    }
}
}

// 0x7EC810 (lote R12A, sin validar; 431 bytes; cdecl, 5 argumentos): flujo NULL -> NULL sin tocar nada. Si no: type, accessType y rwOwned (+0x20) se escriben antes de comprobar el tipo.
RwStream* _rwStreamInitialize(RwStream* stream, RwBool rwOwned, RwStreamType type, RwStreamAccessType accessType, const void* data) {
    if (stream == nullptr) {
        return nullptr;
    }
    stream->type       = type;
    stream->accessType = accessType;
    stream->rwOwned    = rwOwned;
    return R12AStreamSetup(stream, type, accessType, data);
}

// 0x7ECEF0 (lote R12A, sin validar; 481 bytes): reserva el flujo en la lista libre (hint 0x30404); si falla, RwFreeListFree con NULL (el EXE lo hace) y NULL. rwOwned = 1. Si la configuracion falla, libera el flujo y devuelve NULL.
RwStream* RwStreamOpen(RwStreamType type, RwStreamAccessType accessType, const void* data) {
    RwStream* const stream = static_cast<RwStream*>(RwEngineInstance->memoryAlloc(R12AStreamFreeList(), 0x30404));
    if (stream == nullptr) {
        R12AStreamFree(stream);
        return nullptr;
    }
    stream->type       = type;
    stream->accessType = accessType;
    stream->rwOwned    = TRUE;
    if (R12AStreamSetup(stream, type, accessType, data) == nullptr) {
        R12AStreamFree(stream);
        return nullptr;
    }
    return stream;
}

// 0x7ECE20 (lote R12A, sin validar; 190 bytes): FILE no hace nada (TRUE); FILENAME = fclose(fp) == 0; MEMORY con acceso distinto de lectura y datos no nulos devuelve {memBlock, position} en data (RwMemory); CUSTOM llama a close(data) si existe
// (resultado ignorado) y da TRUE; tipo invalido: error 0xE y FALSE sin liberar. Si rwOwned, libera el flujo con RwFreeListFree.
RwBool RwStreamClose(RwStream* stream, void* data) {
    RwBool result;
    switch (stream->type) {
    case rwSTREAMFILE:
        result = TRUE;
        break;
    case rwSTREAMFILENAME:
        result = reinterpret_cast<RwFileCloseFn>(RwEngineInstance->fileFuncs[2])(stream->Type.file.fpFile) == 0;
        break;
    case rwSTREAMMEMORY:
        result = TRUE;
        if (stream->accessType != rwSTREAMREAD && data != nullptr) {
            RwMemory* const memory = static_cast<RwMemory*>(data);
            memory->start  = stream->Type.memory.memBlock;
            memory->length = stream->Type.memory.position;
        }
        break;
    case rwSTREAMCUSTOM:
        if (stream->Type.custom.sfnclose != nullptr) {
            stream->Type.custom.sfnclose(stream->Type.custom.data);
        }
        result = TRUE;
        break;
    default:
        StreamRaiseError(0xE);
        return FALSE;
    }
    if (stream->rwOwned) {
        R12AStreamFree(stream);
    }
    return result;
}

// 0x7ECD00 (lote R12A, sin validar; 263 bytes): desplazamiento 0 -> el flujo (sin mirar el tipo). FILE/FILENAME: fseek(fp, offset, SEEK_CUR); si falla, feof != 0 -> error 5, y NULL. MEMORY: position + offset (sin signo) > nSize ->
// position = nSize, error 5 y NULL. CUSTOM: skip(data, offset) distinto de 0 -> el flujo, si no NULL. Otro tipo: error 0xE y NULL.
RwStream* RwStreamSkip(RwStream* stream, RwUInt32 offset) {
    if (offset == 0) {
        return stream;
    }
    switch (stream->type) {
    case rwSTREAMFILE:
    case rwSTREAMFILENAME: {
        void* const file = stream->Type.file.fpFile;
        if (reinterpret_cast<RwFileSeekFn>(RwEngineInstance->fileFuncs[8])(file, static_cast<long>(offset), 1) != 0) {
            if (reinterpret_cast<RwFileEofFn>(RwEngineInstance->fileFuncs[7])(file) != 0) {
                StreamRaiseError(5);
            }
            return nullptr;
        }
        return stream;
    }
    case rwSTREAMMEMORY: {
        const RwUInt32 target = stream->Type.memory.position + offset;
        if (target > stream->Type.memory.nSize) {
            stream->Type.memory.position = stream->Type.memory.nSize;
            StreamRaiseError(5);
            return nullptr;
        }
        stream->Type.memory.position = target;
        return stream;
    }
    case rwSTREAMCUSTOM:
        return stream->Type.custom.sfnskip(stream->Type.custom.data, offset) ? stream : nullptr;
    default:
        StreamRaiseError(0xE);
        return nullptr;
    }
}

// 0x7ED1E0 (lote R12A, sin validar; 28 bytes): tabla de saltos de 26 entradas indexada por (tipo - 1); es "compleja" (TRUE) para los tipos 5..11, 14, 15, 16, 18, 20 y 26; cualquier otro (incluido > 26) da FALSE.
RwBool _rwStreamChunkIsComplex(const RwChunkHeaderInfo* info) {
    const RwUInt32 index = info->type - 1;
    if (index > 0x19) {
        return FALSE;
    }
    constexpr RwUInt32 kComplexMask = (0x7Fu << 4) | (0x7u << 13) | (1u << 17) | (1u << 19) | (1u << 25);
    return (kComplexMask >> index) & 1u;
}

// 0x7ED0F0 (lote R12A, sin validar; 231 bytes; cdecl, 5 argumentos): lee 12 bytes (si no llegan 12: error 0x8000001A y FALSE). El tercer dword es el identificador de biblioteca: sin bits altos (formato antiguo) version = id << 8 y
// build = 0; si no version = ((id >> 14) & 0x3FF00) + 0x30000 | ((id >> 16) & 0x3F) y build = id & 0xFFFF. Llama a _rwStreamChunkIsComplex (resultado descartado) y escribe las salidas no nulas (tipo, longitud, build y version, en ese orden).
RwBool _rwStreamReadChunkHeader(RwStream* stream, RwUInt32* type, RwUInt32* length, RwUInt32* version, RwUInt32* buildNum) {
    RwUInt32 header[3];
    if (RwStreamRead(stream, header, 12) != 12) {
        StreamRaiseError(static_cast<RwInt32>(0x8000001A));
        return FALSE;
    }
    RwChunkHeaderInfo info;
    info.type   = header[0];
    info.length = header[1];
    const RwUInt32 libraryId = header[2];
    if ((libraryId & 0xFFFF0000u) == 0) {
        info.version  = libraryId << 8;
        info.buildNum = 0;
    } else {
        info.version  = (((libraryId >> 14) & 0x3FF00u) + 0x30000u) | ((libraryId >> 16) & 0x3Fu);
        info.buildNum = libraryId & 0xFFFFu;
    }
    _rwStreamChunkIsComplex(&info);
    if (type)     { *type     = info.type; }
    if (length)   { *length   = info.length; }
    if (buildNum) { *buildNum = info.buildNum; }
    if (version)  { *version  = info.version; }
    return TRUE;
}

// 0x7ED2D0 (lote R12A, sin validar; 244 bytes): lee cabeceras y salta los datos (RwStreamSkip) hasta dar con el tipo pedido; cualquier fallo -> FALSE. Con el tipo encontrado, version < 0x34000 o > 0x36003 (sin signo): error 0x80000004 y FALSE;
// si no escribe longitud y version (si no son nulos) y devuelve TRUE.
RwBool RwStreamFindChunk(RwStream* stream, RwUInt32 type, RwUInt32* lengthOut, RwUInt32* versionOut) {
    RwUInt32 foundType, length, version;
    if (!_rwStreamReadChunkHeader(stream, &foundType, &length, &version, nullptr)) {
        return FALSE;
    }
    while (foundType != type) {
        if (RwStreamSkip(stream, length) == nullptr) {
            return FALSE;
        }
        if (!_rwStreamReadChunkHeader(stream, &foundType, &length, &version, nullptr)) {
            return FALSE;
        }
    }
    if (version < 0x34000u || version > 0x36003u) {
        StreamRaiseError(static_cast<RwInt32>(0x80000004));
        return FALSE;
    }
    if (lengthOut)  { *lengthOut  = length; }
    if (versionOut) { *versionOut = version; }
    return TRUE;
}

// 0x7ED270 (lote R12A, sin validar; 82 bytes): escribe {tipo, tamano, id} con RwStreamWrite(12 bytes); id = (((version + 0x10000) << 14) & 0xFFC00000) | ((version & 0x3F) << 16) | (build & 0xFFFF). Devuelve su resultado.
RwStream* _rwStreamWriteVersionedChunkHeader(RwStream* stream, RwInt32 type, RwInt32 size, RwUInt32 version, RwUInt32 buildNum) {
    RwUInt32 header[3];
    header[0] = static_cast<RwUInt32>(type);
    header[1] = static_cast<RwUInt32>(size);
    header[2] = (((version + 0x10000u) << 14) & 0xFFC00000u) | ((version & 0x3Fu) << 16) | (buildNum & 0xFFFFu);
    return RwStreamWrite(stream, header, 12);
}

// 0x7ED3D0 (lote R12A, sin validar; 142 bytes): sin bytes -> el flujo; si no escribe por trozos de hasta 0x100 bytes copiados a un bufer local (RwStreamWrite; un resultado NULL -> NULL) hasta agotar los bytes.
RwStream* RwStreamWriteReal(RwStream* stream, const RwReal* reals, RwUInt32 numBytes) {
    RwUInt8 buffer[0x100];
    const RwUInt8* source = reinterpret_cast<const RwUInt8*>(reals);
    while (numBytes != 0) {
        const RwUInt32 chunk = numBytes < 0x100 ? numBytes : 0x100;
        memcpy(buffer, source, chunk);
        if (RwStreamWrite(stream, buffer, chunk) == nullptr) {
            return nullptr;
        }
        numBytes -= chunk;
        source += chunk;
    }
    return stream;
}

// 0x7ED4F0 y 0x7ED540 (lote R12A, sin validar; 80 bytes cada una): RwStreamRead(flujo, destino, bytes); si devuelve 0 (no se compara con numBytes): error 0x8000001A y NULL; si no el flujo.
RwStream* RwStreamReadReal(RwStream* stream, RwReal* reals, RwUInt32 numBytes) {
    if (RwStreamRead(stream, reals, numBytes) == 0) {
        StreamRaiseError(static_cast<RwInt32>(0x8000001A));
        return nullptr;
    }
    return stream;
}

RwStream* RwStreamReadInt32(RwStream* stream, RwInt32* ints, RwUInt32 numBytes) {
    if (RwStreamRead(stream, ints, numBytes) == 0) {
        StreamRaiseError(static_cast<RwInt32>(0x8000001A));
        return nullptr;
    }
    return stream;
}

// 0x7ED590 (lote R12A, sin validar; 96 bytes): _rwStreamReadChunkHeader(flujo, &tipo, &longitud, &version, &build); si falla NULL. Rellena type, length, version, buildNum e isComplex (_rwStreamChunkIsComplex) y devuelve el flujo.
RwStream* RwStreamReadChunkHeaderInfo(RwStream* stream, RwChunkHeaderInfo* chunkHeaderInfo) {
    RwUInt32 type, length, version, buildNum;
    if (!_rwStreamReadChunkHeader(stream, &type, &length, &version, &buildNum)) {
        return nullptr;
    }
    chunkHeaderInfo->type      = type;
    chunkHeaderInfo->length    = length;
    chunkHeaderInfo->version   = version;
    chunkHeaderInfo->buildNum  = buildNum;
    chunkHeaderInfo->isComplex = _rwStreamChunkIsComplex(chunkHeaderInfo);
    return stream;
}

RwBool _rwStringOpen() {
    return ((RwBool(__cdecl *)(void))0x80A240)();
}

// 0x80A440: el EXE solo ejecuta `ret` (1 byte, seguido de 15 bytes de relleno 0x90 hasta 0x80A450).
void _rwStringClose() {
}

RwBool _rwStringDestroy(RwChar* string) {
    return ((RwBool(__cdecl *)(RwChar*))0x80A450)(string);
}

// 0x80A470: una cadena nula se sustituye por la cadena vacía del EXE (0x884F6A); la longitud sale de rwstrlen del motor
// ([RwEngineInstance + 0x120], cdecl) y el tamaño en el flujo es (longitud + 4) con los dos bits bajos a cero (`add eax, 4` / `and al, 0FCh`).
RwUInt32 _rwStringStreamGetSize(const RwChar* string) {
    if (!string) {
        string = reinterpret_cast<const RwChar*>(0x884F6A);
    }
    const uint32 length = static_cast<uint32>(RWSRCGLOBAL(stringFuncs).vecStrlen(string));
    return (length + 4u) & ~3u;
}

const RwChar* _rwStringStreamWrite(const RwChar* string, RwStream* stream) {
    return ((const RwChar*(__cdecl *)(const RwChar*, RwStream*))0x80A4A0)(string, stream);
}

RwChar* _rwStringStreamFindAndRead(RwChar* string, RwStream* stream) {
    return ((RwChar*(__cdecl *)(RwChar*, RwStream*))0x80A510)(string, stream);
}

void RwPluginRegistrySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x8087B0)(blockSize, numBlocksToPrealloc);
}

RwBool _rwPluginRegistrySetStaticPluginsSize(RwPluginRegistry* reg, RwInt32 size) {
    return ((RwBool(__cdecl *)(RwPluginRegistry*, RwInt32))0x808430)(reg, size);
}

RwInt32 _rwPluginRegistryAddPlugin(RwPluginRegistry* reg, RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x8084A0)(reg, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x808470: recorre la lista de plugins del registro (firstRegEntry +0x10, nextRegEntry +0x30) y devuelve el desplazamiento (+0) del primero con ese
// pluginID (+8), o -1 si no está.
RwInt32 _rwPluginRegistryGetPluginOffset(const RwPluginRegistry* reg, RwUInt32 pluginID) {
    for (const RwPluginRegEntry* entry = reg->firstRegEntry; entry; entry = entry->nextRegEntry) {
        if (entry->pluginID == pluginID) {
            return entry->offset;
        }
    }
    return -1;
}

const RwPluginRegistry* _rwPluginRegistryInitObject(const RwPluginRegistry* reg, void* object) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, void*))0x8086E0)(reg, object);
}

const RwPluginRegistry* _rwPluginRegistryDeInitObject(const RwPluginRegistry* reg, void* object) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, void*))0x808740)(reg, object);
}

const RwPluginRegistry* _rwPluginRegistryCopyObject(const RwPluginRegistry* reg, void* dstObject, const void* srcObject) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, void*, const void*))0x808770)(reg, dstObject, srcObject);
}

RwInt32 _rwPluginRegistryAddPluginStream(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x8088E0)(reg, pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 _rwPluginRegistryAddPlgnStrmlwysCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkAlwaysCallBack))0x808920)(reg, pluginID, alwaysCB);
}

RwInt32 _rwPluginRegistryAddPlgnStrmRightsCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkRightsCallBack))0x808950)(reg, pluginID, rightsCB);
}

const RwPluginRegistry* _rwPluginRegistryReadDataChunks(const RwPluginRegistry* reg, RwStream* stream, void* object) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, RwStream*, void*))0x808980)(reg, stream, object);
}

const RwPluginRegistry* _rwPluginRegistryWriteDataChunks(const RwPluginRegistry* reg, RwStream* stream, const void* object) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, RwStream*, const void*))0x808B40)(reg, stream, object);
}

const RwPluginRegistry* _rwPluginRegistrySkipDataChunks(const RwPluginRegistry* reg, RwStream* stream) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, RwStream*))0x808C10)(reg, stream);
}

RwInt32 _rwPluginRegistryGetSize(const RwPluginRegistry* reg, const void* object) {
    return ((RwInt32(__cdecl *)(const RwPluginRegistry*, const void*))0x808B00)(reg, object);
}

const RwPluginRegistry* _rwPluginRegistryInvokeRights(const RwPluginRegistry* reg, RwUInt32 id, void* obj, RwUInt32 extraData) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, RwUInt32, void*, RwUInt32))0x808AB0)(reg, id, obj, extraData);
}

RwBool RwEngineGetMatrixTolerances(RwMatrixTolerance * const tolerance) {
    return ((RwBool(__cdecl *)(RwMatrixTolerance * const))0x7F1780)(tolerance);
}

RwBool RwEngineSetMatrixTolerances(const RwMatrixTolerance * const tolerance) {
    return ((RwBool(__cdecl *)(const RwMatrixTolerance*))0x7F17B0)(tolerance);
}

void RwMatrixSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7F16A0)(blockSize, numBlocksToPrealloc);
}

RwBool RwMatrixDestroy(RwMatrix* mpMat) {
    return ((RwBool(__cdecl *)(RwMatrix*))0x7F2A20)(mpMat);
}

RwMatrix* RwMatrixCreate() {
    return ((RwMatrix*(__cdecl *)(void))0x7F2A50)();
}

RwMatrix* RwMatrixMultiply(RwMatrix* matrixOut, const RwMatrix* MatrixIn1, const RwMatrix* matrixIn2) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwMatrix*, const RwMatrix*))0x7F18B0)(matrixOut, MatrixIn1, matrixIn2);
}

RwMatrix* RwMatrixTransform(RwMatrix* matrix, const RwMatrix* transform, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwMatrix*, RwOpCombineType))0x7F25A0)(matrix, transform, combineOp);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R12A (sin validar): modulos de vectores y matrices del motor. Cada uno es un bloque dentro de RwEngineInstance a un desplazamiento guardado en una variable del EXE
// (vectores 0xC97934, matrices 0xC979BC).
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
struct R12AVectorModuleData {
    const RwUInt32* sqrtTable;     // +0x00 tabla de la raiz (RwV3dLength 0x7EDAC0, _rwSqrt 0x7EDB30)
    const RwUInt32* invSqrtTable;  // +0x04 tabla de la inversa de la raiz (_rwInvSqrt 0x7EDB90)
    rwVectorMultFn  multPoint;     // +0x08 (por defecto 0x7ED730)
    rwVectorsMultFn multPoints;    // +0x0C (por defecto 0x7ED670)
    rwVectorMultFn  multVector;    // +0x10 (por defecto 0x7ED880)
    rwVectorsMultFn multVectors;   // +0x14 (por defecto 0x7ED7D0)
};
struct R12AMatrixModuleData {
    void*             freeList;   // +0x00
    RwUInt32          flags;      // +0x04 (0x20000 = optimizaciones de identidad)
    void*             multFn;     // +0x08 (por defecto 0x7F12F0)
    RwMatrixTolerance tolerance;  // +0x0C (por defecto 0.01f x 3)
};
static_assert(offsetof(R12AVectorModuleData, multVectors) == 0x14 && sizeof(RwMatrixTolerance) == 0xC);
R12AVectorModuleData& R12AVectorModule() {
    return *reinterpret_cast<R12AVectorModuleData*>(reinterpret_cast<uintptr_t>(RwEngineInstance) + StaticRef<int32>(0xC97934));
}
R12AMatrixModuleData& R12AMatrixModule() {
    return *reinterpret_cast<R12AMatrixModuleData*>(reinterpret_cast<uintptr_t>(RwEngineInstance) + StaticRef<int32>(0xC979BC));
}
}

// 0x7ED5F0 (lote R12A, sin validar; 122 bytes; cdecl, 4 argumentos): _rwVectorSetMultFn. Guarda los cuatro callbacks (punto, puntos, vector, vectores) en +8, +0xC, +0x10 y +0x14 del modulo de vectores; los nulos se sustituyen por
// 0x7ED730, 0x7ED670, 0x7ED880 y 0x7ED7D0. Devuelve TRUE.
RwBool _rwVectorSetMultFn(rwVectorMultFn multPoint, rwVectorsMultFn multPoints, rwVectorMultFn multVector, rwVectorsMultFn multVectors) {
    R12AVectorModule().multPoint = multPoint ? multPoint : reinterpret_cast<rwVectorMultFn>(0x7ED730);
    R12AVectorModule().multPoints = multPoints ? multPoints : reinterpret_cast<rwVectorsMultFn>(0x7ED670);
    R12AVectorModule().multVector = multVector ? multVector : reinterpret_cast<rwVectorMultFn>(0x7ED880);
    R12AVectorModule().multVectors = multVectors ? multVectors : reinterpret_cast<rwVectorsMultFn>(0x7ED7D0);
    return TRUE;
}

// 0x7ED730 (lote R12A, sin validar; 150 bytes): callback de punto por defecto; x87 con T1, T2 y at.y * z + (...) guardados como float (ver V3dTransformPointDefaultCpp). Devuelve la salida (EAX).
RwV3d* _rwV3dTransformPointDefault(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix) {
    V3dTransformPointDefaultCpp(&pointOut->x, &pointIn->x, reinterpret_cast<const float*>(matrix));
    return pointOut;
}

// 0x7F1930 (lote R12A, sin validar; 968 bytes; cdecl, (salida, entrada)): cuerpo de RwMatrixOrthoNormalize (0x7F1920 es un `jmp` a esta direccion). Copia los ejes de la entrada, normaliza right, up y at con _rwInvSqrt
// (0x7EDB90) sobre la suma de cuadrados guardada como float; si algun inverso no es > 0 (o NaN) ese eje es el que se recalcula; si no se calculan |at.up|, |at.right| y |up.right| y se recalcula el eje del par menos ortogonal:
// |at.up| < |at.right| y |at.up| < |up.right| -> right; si no |at.right| < |up.right| (tras |at.up| >= |at.right|) -> up; en el resto -> at. Se recalcula d = b x s normalizado y s = d x b normalizado
// (right = up x at, at = right x up; up = at x right, right = up x at; at = right x up, up = at x right). Escribe los ejes y la posicion en la salida (las rellenos no se tocan) y flags = (flags & ~0x20000) | 3.
RwMatrix* RwMatrixOrthoNormalizeImpl(RwMatrix* matrixOut, const RwMatrix* matrixIn) {
    const float* const in = reinterpret_cast<const float*>(matrixIn);
    float right[3] = { in[0], in[1], in[2] };
    float up[3]    = { in[4], in[5], in[6] };
    float at[3]    = { in[8], in[9], in[10] };
    const float posX = in[12];
    const float posY = in[13];
    const float posZ = in[14];

    const float invRight = _rwInvSqrt(V3dSumSquaresCpp(right));
    V3dScaleCpp(right, invRight);
    const float invUp = _rwInvSqrt(V3dSumSquaresCpp(up));
    V3dScaleCpp(up, invUp);
    const float invAt = _rwInvSqrt(V3dSumSquaresCpp(at));
    V3dScaleCpp(at, invAt);

    // 0 = recalcular right (b = up, s = at, d = right), 1 = up (b = at, s = right, d = up), 2 = at (b = right, s = up, d = at)
    int recompute;
    if (!(invRight > 0.0f)) {
        recompute = 0;
    } else if (!(invUp > 0.0f)) {
        recompute = 1;
    } else if (!(invAt > 0.0f)) {
        recompute = 2;
    } else {
        float dotAtUp = static_cast<float>(V3dDotYXZCpp(at, up));
        if (!(dotAtUp >= 0.0f)) {
            dotAtUp = -dotAtUp;
        }
        float dotAtRight = static_cast<float>(V3dDotYXZCpp(at, right));
        if (!(dotAtRight >= 0.0f)) {
            dotAtRight = -dotAtRight;
        }
        double dotUpRight = V3dDotYXZCpp(up, right);
        if (!(dotUpRight >= 0.0)) {
            dotUpRight = -dotUpRight;
        }
        if (dotAtUp < dotAtRight) {
            recompute = (static_cast<double>(dotAtUp) < dotUpRight) ? 0 : 2;
        } else {
            recompute = (static_cast<double>(dotAtRight) < dotUpRight) ? 1 : 2;
        }
    }
    float* base;
    float* second;
    float* dest;
    switch (recompute) {
    case 0:  base = up;    second = at;    dest = right; break;
    case 1:  base = at;    second = right; dest = up;    break;
    default: base = right; second = up;    dest = at;    break;
    }
    V3dCrossCpp(dest, base, second);
    V3dScaleCpp(dest, _rwInvSqrt(V3dSumSquaresCpp(dest)));
    V3dCrossCpp(second, dest, base);
    V3dScaleCpp(second, _rwInvSqrt(V3dSumSquaresCpp(second)));

    float* const out = reinterpret_cast<float*>(matrixOut);
    out[0] = right[0]; out[1] = right[1]; out[2] = right[2];
    out[4] = up[0];    out[5] = up[1];    out[6] = up[2];
    out[8] = at[0];    out[9] = at[1];    out[10] = at[2];
    out[12] = posX;    out[13] = posY;    out[14] = posZ;
    matrixOut->flags = (matrixOut->flags & 0xFFFDFFFFu) | 3u;
    return matrixOut;
}

// 0x7F1920 (lote R12A, sin validar): `jmp 0x7F1930`; ahora llama directamente al cuerpo reconstruido (RwMatrixOrthoNormalizeImpl).
RwMatrix* RwMatrixOrthoNormalize(RwMatrix* matrixOut, const RwMatrix* matrixIn) {
    return RwMatrixOrthoNormalizeImpl(matrixOut, matrixIn);
}

RwMatrix* RwMatrixInvert(RwMatrix* matrixOut, const RwMatrix* matrixIn) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwMatrix*))0x7F2070)(matrixOut, matrixIn);
}

RwMatrix* RwMatrixScale(RwMatrix* matrix, const RwV3d* scale, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwV3d*, RwOpCombineType))0x7F22C0)(matrix, scale, combineOp);
}

RwMatrix* RwMatrixTranslate(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwV3d*, RwOpCombineType))0x7F2450)(matrix, translation, combineOp);
}

RwMatrix* RwMatrixRotate(RwMatrix* matrix, const RwV3d* axis, RwReal angle, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwV3d*, RwReal, RwOpCombineType))0x7F1FD0)(matrix, axis, angle, combineOp);
}

RwMatrix* RwMatrixRotateOneMinusCosineSine(RwMatrix* matrix, const RwV3d* unitAxis, RwReal oneMinusCosine, RwReal sine, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwV3d*, RwReal, RwReal, RwOpCombineType))0x7F1D00)(matrix, unitAxis, oneMinusCosine, sine, combineOp);
}

const RwMatrix* RwMatrixQueryRotate(const RwMatrix* matrix, RwV3d* unitAxis, RwReal* angle, RwV3d* center) {
    return ((const RwMatrix*(__cdecl *)(const RwMatrix*, RwV3d*, RwReal*, RwV3d*))0x7F2720)(matrix, unitAxis, angle, center);
}

RwMatrix* RwMatrixUpdate(RwMatrix* matrix) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*))0x7F18A0)(matrix);
}

// 0x7F17E0 (lote R12A, sin validar; 179 bytes): tolerancia NULL -> la del modulo de matrices (motor + [0xC979BC] + 0xC, tres floats). normal = error normal <= Normal; orthogonal = error ortogonal <= Orthogonal (comparaciones ordenadas: NaN da falso);
// identidad solo se evalua si los dos son verdaderos (error de identidad <= Identity). Bits 1 (normal), 2 (ortogonal) y 0x20000 (identidad) de los flags (+0xC) se ponen o se borran. Devuelve la matriz.
RwMatrix* RwMatrixOptimize(RwMatrix* matrix, const RwMatrixTolerance* tolerance) {
    if (tolerance == nullptr) {
        tolerance = &R12AMatrixModule().tolerance;
    }
    const bool normal = _rwMatrixNormalError(matrix) <= tolerance->Normal;
    const bool orthogonal = _rwMatrixOrthogonalError(matrix) <= tolerance->Orthogonal;
    bool identity = false;
    if (normal && orthogonal) {
        identity = _rwMatrixIdentityError(matrix) <= tolerance->Identity;
    }
    RwUInt32 flags = matrix->flags;
    flags = normal ? (flags | 1u) : (flags & ~1u);
    flags = orthogonal ? (flags | 2u) : (flags & ~2u);
    flags = identity ? (flags | 0x20000u) : (flags & ~0x20000u);
    matrix->flags = flags;
    return matrix;
}

// 0x7F1450 (lote R12A, sin validar; 69 bytes): determinante en el orden x87 del EXE (ver MatrixDeterminantCpp).
RwReal _rwMatrixDeterminant(const RwMatrix* matrix) {
    return MatrixDeterminantCpp(reinterpret_cast<const float*>(matrix));
}

// 0x7F1500 (lote R12A, sin validar; 137 bytes): ((eU^2 + eR^2) + eA^2) con la longitud al cuadrado de cada eje menos 1.0f (ver MatrixNormalErrorCpp).
RwReal _rwMatrixNormalError(const RwMatrix* matrix) {
    return MatrixNormalErrorCpp(reinterpret_cast<const float*>(matrix));
}

// 0x7F14A0 (lote R12A, sin validar; 91 bytes): suma de los cuadrados de los tres productos escalares entre ejes (ver MatrixOrthogonalErrorCpp).
RwReal _rwMatrixOrthogonalError(const RwMatrix* matrix) {
    return MatrixOrthogonalErrorCpp(reinterpret_cast<const float*>(matrix));
}

// 0x7F1590 (lote R12A, sin validar; 200 bytes): distancia al identidad (diagonal menos 1, resto y traslacion) al cuadrado (ver MatrixIdentityErrorCpp).
RwReal _rwMatrixIdentityError(const RwMatrix* matrix) {
    return MatrixIdentityErrorCpp(reinterpret_cast<const float*>(matrix));
}

RwReal RwV3dNormalize(RwV3d* out, const RwV3d* in) {
    return ((RwReal(__cdecl *)(RwV3d*, const RwV3d*))0x7ED9B0)(out, in);
}

// 0x7EDAC0 (lote R12A, sin validar; 112 bytes): s = (x*x + y*y) + z*z guardado como float; con los bits de s distintos de cero (tambien -0.0f) se aproxima la raiz con la tabla del modulo de vectores (motor + [0xC97934], puntero en +0):
// bits += 0x800; resultado = tabla[(bits >> 12) & 0xFFF] + ((bits >> 1) & 0x3FC00000) (aritmetica de enteros). Con bits cero devuelve s.
RwReal RwV3dLength(const RwV3d* in) {
    const float sum = V3dSumSquaresCpp(&in->x);
    RwUInt32 bits = std::bit_cast<RwUInt32>(sum);
    if (bits != 0) {
        bits += 0x800;
        const RwUInt32* const table = R12AVectorModule().sqrtTable;
        bits = table[(bits >> 12) & 0xFFF] + ((bits >> 1) & 0x3FC00000u);
    }
    return std::bit_cast<RwReal>(bits);
}

RwReal RwV2dLength(const RwV2d* in) {
    return ((RwReal(__cdecl *)(const RwV2d*))0x7EDBF0)(in);
}

RwReal RwV2dNormalize(RwV2d* out, const RwV2d* in) {
    return ((RwReal(__cdecl *)(RwV2d*, const RwV2d*))0x7EDC60)(out, in);
}

// 0x7EDD60 (lote R12A, sin validar; 38 bytes): llama al callback del modulo de vectores (motor + [0xC97934] + 8, cdecl) con (salida, entrada, matriz), ignora su resultado y devuelve la salida.
RwV3d* RwV3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix) {
    R12AVectorModule().multPoint(pointOut, pointIn, matrix);
    return pointOut;
}

RwV3d* RwV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix) {
    return ((RwV3d*(__cdecl *)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x7EDD90)(pointsOut, pointsIn, numPoints, matrix);
}

// 0x7EDDC0 (lote R12A, sin validar; 38 bytes): igual que RwV3dTransformPoint con el callback de +0x10.
RwV3d* RwV3dTransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix) {
    R12AVectorModule().multVector(vectorOut, vectorIn, matrix);
    return vectorOut;
}

// 0x7EDDF0 (lote R12A, sin validar; 43 bytes): llama al callback de +0x14 con (salida, entrada, numero, matriz), ignora su resultado y devuelve la salida.
RwV3d* RwV3dTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, RwInt32 numPoints, const RwMatrix* matrix) {
    R12AVectorModule().multVectors(vectorsOut, vectorsIn, numPoints, matrix);
    return vectorsOut;
}

RwReal _rwSqrt(const RwReal num) {
    return ((RwReal(__cdecl *)(const RwReal))0x7EDB30)(num);
}

// 0x7EDB90 (lote R12A, sin validar; 87 bytes): con los bits de num distintos de cero: bits += 0x800; resultado = tabla[(bits >> 12) & 0xFFF] + ((~bits >> 1) & 0x3FC00000) con la tabla del modulo de vectores en +4;
// con bits cero devuelve num.
RwReal _rwInvSqrt(const RwReal num) {
    RwUInt32 bits = std::bit_cast<RwUInt32>(num);
    if (bits != 0) {
        bits += 0x800;
        const RwUInt32* const table = R12AVectorModule().invSqrtTable;
        bits = table[(bits >> 12) & 0xFFF] + (((~bits) >> 1) & 0x3FC00000u);
    }
    return std::bit_cast<RwReal>(bits);
}

RwReal _rwV3dNormalize(RwV3d* out, const RwV3d* in) {
    return ((RwReal(__cdecl *)(RwV3d*, const RwV3d*))0x7ED910)(out, in);
}

RwSList* _rwSListCreate(RwInt32 size, RwUInt32 hint) {
    return ((RwSList*(__cdecl *)(RwInt32, RwUInt32))0x809160)(size, hint);
}

RwBool _rwSListDestroy(RwSList* sList) {
    return ((RwBool(__cdecl *)(RwSList*))0x809440)(sList);
}

// 0x809400: resta `amount` a numElementsFilled (+4), con envoltura de 32 bits.
void _rwSListDestroyEndEntries(RwSList* sList, RwInt32 amount) {
    sList->numElementsFilled = static_cast<RwInt32>(static_cast<uint32>(sList->numElementsFilled) - static_cast<uint32>(amount));
}

// 0x809510: listElements + entrySize * entry (imul de 32 bits con envoltura).
void* _rwSListGetEntry(RwSList* sList, RwInt32 entry) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(sList->listElements) + static_cast<uint32>(sList->entrySize) * static_cast<uint32>(entry));
}

void* _rwSListGetNewEntry(RwSList* sList, RwUInt32 hint) {
    return ((void*(__cdecl *)(RwSList*, RwUInt32))0x809240)(sList, hint);
}

// 0x8094B0: numElementsFilled (+4).
RwInt32 _rwSListGetNumEntries(const RwSList* sList) {
    return sList->numElementsFilled;
}

// 0x809530: listElements (+0).
void* _rwSListGetBegin(RwSList* sList) {
    return sList->listElements;
}

// 0x809540: listElements + entrySize * numElementsFilled (imul de 32 bits con envoltura).
void* _rwSListGetEnd(RwSList* sList) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(sList->listElements) + static_cast<uint32>(sList->entrySize) * static_cast<uint32>(sList->numElementsFilled));
}

RwUInt32 RwEngineGetVersion() {
    return ((RwUInt32(__cdecl *)(void))0x7F2BA0)();
}

RwBool RwEngineInit(const RwMemoryFunctions* memFuncs, RwUInt32 initFlags, RwUInt32 resArenaSize) {
    return ((RwBool(__cdecl *)(const RwMemoryFunctions*, RwUInt32, RwUInt32))0x7F3170)(memFuncs, initFlags, resArenaSize);
}

// Direcciones del EXE usadas por las funciones del lote 25 (g25c) de este archivo.
namespace {
constexpr uintptr_t kEnginePluginRegistry = 0x8E2298; // RwEngine: lista de plugins de RwGlobals
}

void RwPlCoreEngineStreamInjectHooks() {
    RH_ScopedNamespaceName("RwPlCore");
    RH_ScopedCategory("Renderware");

    RH_ScopedGlobalInstall(RwStreamWriteInt32, 0x7ED460);
    // Lote R11A (sin validar). RenderWare.h declara ademas la plantilla RwStreamRead<T>: el puntero a la funcion se fija con un cast para desambiguar.
    RH_ScopedNamedGlobalInstall(*static_cast<RwUInt32(*)(RwStream*, void*, RwUInt32)>(RwStreamRead), "RwStreamRead", 0x7EC9D0);
    RH_ScopedGlobalInstall(RwStreamWrite, 0x7ECB30);
    RH_ScopedGlobalInstall(RwMatrixOrthoNormalize, 0x7F1920);
    RH_ScopedGlobalInstall(RwEngineRegisterPlugin, 0x7F2BB0);
    RH_ScopedGlobalInstall(RwEngineGetPluginOffset, 0x7F2BE0);
    RH_ScopedGlobalInstall(_rwerror, 0x8088D0); // lote R7A (sin validar)
}

// 0x7F2BB0: registra el plugin en la lista del motor (0x8E2298) con _rwPluginRegistryAddPlugin (0x8084A0); el callback de copia es nulo.
RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor initCB, RwPluginObjectDestructor termCB) {
    return _rwPluginRegistryAddPlugin(reinterpret_cast<RwPluginRegistry*>(kEnginePluginRegistry), size, pluginID, initCB, termCB, nullptr);
}

// 0x7F2BE0: desplazamiento del plugin en la lista del motor (0x8E2298) con _rwPluginRegistryGetPluginOffset (0x808470).
RwInt32 RwEngineGetPluginOffset(RwUInt32 pluginID) {
    return _rwPluginRegistryGetPluginOffset(reinterpret_cast<const RwPluginRegistry*>(kEnginePluginRegistry), pluginID);
}

RwBool RwEngineOpen(RwEngineOpenParams* initParams) {
    return ((RwBool(__cdecl *)(RwEngineOpenParams*))0x7F2F70)(initParams);
}

RwBool RwEngineStart() {
    return ((RwBool(__cdecl *)(void))0x7F2E70)();
}

RwBool RwEngineStop() {
    return ((RwBool(__cdecl *)(void))0x7F2E20)();
}

RwBool RwEngineClose() {
    return ((RwBool(__cdecl *)(void))0x7F2F00)();
}

RwBool RwEngineTerm() {
    return ((RwBool(__cdecl *)(void))0x7F3130)();
}

RwInt32 RwEngineGetNumSubSystems() {
    return ((RwInt32(__cdecl *)(void))0x7F2C00)();
}

RwSubSystemInfo* RwEngineGetSubSystemInfo(RwSubSystemInfo* subSystemInfo, RwInt32 subSystemIndex) {
    return ((RwSubSystemInfo*(__cdecl *)(RwSubSystemInfo*, RwInt32))0x7F2C30)(subSystemInfo, subSystemIndex);
}

RwInt32 RwEngineGetCurrentSubSystem() {
    return ((RwInt32(__cdecl *)(void))0x7F2C60)();
}

RwBool RwEngineSetSubSystem(RwInt32 subSystemIndex) {
    return ((RwBool(__cdecl *)(RwInt32))0x7F2C90)(subSystemIndex);
}

RwInt32 RwEngineGetNumVideoModes() {
    return ((RwInt32(__cdecl *)(void))0x7F2CC0)();
}

RwVideoMode* RwEngineGetVideoModeInfo(RwVideoMode* vmi, RwInt32 modeIndex) {
    return plugin::CallAndReturn<RwVideoMode*, 0x7F2CF0>(vmi, modeIndex);
}

RwInt32 RwEngineGetCurrentVideoMode() {
    return ((RwInt32(__cdecl *)(void))0x7F2D20)();
}

RwBool RwEngineSetVideoMode(RwInt32 modeIndex) {
    return ((RwBool(__cdecl *)(RwInt32))0x7F2D50)(modeIndex);
}

RwInt32 RwEngineGetTextureMemorySize() {
    return ((RwInt32(__cdecl *)(void))0x7F2D80)();
}

RwInt32 RwEngineGetMaxTextureSize() {
    return ((RwInt32(__cdecl *)(void))0x7F2DB0)();
}

RwBool RwEngineSetFocus(RwBool enable) {
    return ((RwBool(__cdecl *)(RwBool))0x7F2DE0)(enable);
}

RwMetrics* RwEngineGetMetrics() {
    return ((RwMetrics*(__cdecl *)(void))0x7F2E10)();
}

//RwFileFunctions* RwOsGetFileInterface() {
//    return ((RwFileFunctions*(__cdecl *)(void))0x804130)();
//}

RwError* RwErrorGet(RwError* code) {
    return ((RwError*(__cdecl *)(RwError*))0x808880)(code);
}

RwError* RwErrorSet(RwError* code) {
    return ((RwError*(__cdecl *)(RwError*))0x808820)(code);
}

// 0x8088D0 (lote R7A, sin validar; 5 bytes: `mov eax, [esp+4]; ret`): cdecl variadica, devuelve el primer argumento (el codigo de error); el resto de argumentos de pila se ignoran (los empuja el llamador).
RwInt32 _rwerror(RwInt32 code, ...) {
    return code;
}

RwBool RwResourcesSetArenaSize(RwUInt32 size) {
    return ((RwBool(__cdecl *)(RwUInt32))0x8080C0)(size);
}

RwInt32 RwResourcesGetArenaSize() {
    return ((RwInt32(__cdecl *)(void))0x8081B0)();
}

RwInt32 RwResourcesGetArenaUsage() {
    return ((RwInt32(__cdecl *)(void))0x8081C0)();
}

RwBool RwResourcesEmptyArena() {
    return ((RwBool(__cdecl *)(void))0x8081F0)();
}

RwResEntry* RwResourcesAllocateResEntry(void* owner, RwResEntry* ownerRef, RwInt32 size, RwResEntryDestroyNotify destroyNotify) {
    return ((RwResEntry*(__cdecl *)(void*, RwResEntry*, RwInt32, RwResEntryDestroyNotify))0x807ED0)(owner, ownerRef, size, destroyNotify);
}

RwBool RwResourcesFreeResEntry(RwResEntry* entry) {
    return ((RwBool(__cdecl *)(RwResEntry*))0x807DE0)(entry);
}

void _rwResourcesPurge() {
    ((void(__cdecl *)(void))0x807E50)();
}

// 0x7ED460: escribe los bytes tal cual con RwStreamWrite (0x7ECB30; sin conversion de orden de bytes en x86), ignora su resultado y devuelve el flujo.
RwStream* RwStreamWriteInt32(RwStream* stream, const RwInt32* ints, RwUInt32 numBytes) {
    RwStreamWrite(stream, ints, numBytes);
    return stream;
}

RwStream* RwStreamWriteInt16(RwStream* stream, const RwInt16* ints, RwUInt32 numBytes) {
    return ((RwStream*(__cdecl *)(RwStream*, const RwInt16*, RwUInt32))0x7ED480)(stream, ints, numBytes);
}

RwStream* RwStreamReadInt16(RwStream* stream, RwInt16* ints, RwUInt32 numBytes) {
    return ((RwStream*(__cdecl *)(RwStream*, RwInt16*, RwUInt32))0x7ED4A0)(stream, ints, numBytes);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R8A (sin validar): monton del arena de recursos de RenderWare (RwResources: la usan RwResourcesSetArenaSize 0x8080C0, RwResourcesAllocateResEntry 0x807ED0 y
// la creacion de la arena 0x807C..0x808165). El monton es un bloque de memoria que se reparte en bloques con una cabecera de 0x20 bytes alineada a 32: {+0 monton, +4 siguiente,
// +8 anterior, +0xC tamano util, +0x10 usado (bit 0)} y una cabecera de monton de dos punteros {+0 primer bloque, +4 desde donde buscar bloques libres}.
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
struct ResArenaBlock {
    void*          heap;     // +0x00 cabecera del monton
    ResArenaBlock* next;     // +0x04
    ResArenaBlock* prev;     // +0x08
    RwUInt32       size;     // +0x0C bytes utiles (despues de la cabecera de 0x20)
    RwUInt32       used;     // +0x10 bit 0 = ocupado
    RwUInt32       pad[3];   // +0x14
};
struct ResArenaHeap {
    ResArenaBlock* first;       // +0x00
    ResArenaBlock* searchStart; // +0x04 primer bloque libre conocido (la busqueda empieza aqui)
};
static_assert(sizeof(ResArenaBlock) == 0x20 && offsetof(ResArenaBlock, size) == 0xC && offsetof(ResArenaBlock, used) == 0x10);
} // namespace

RwBool _rwResArenaHeapInit(void* memory, RwInt32 size);
RwBool _rwResArenaHeapDestroy(void* heap);
void* _rwResArenaHeapAlloc(void* heap, RwUInt32 size);
void* _rwResArenaHeapSplitBlock(void* block, RwUInt32 size);

void RwPlCoreInjectHooksR8A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(_rwResArenaHeapInit, 0x80FFE0);
    RH_ScopedGlobalInstall(_rwResArenaHeapDestroy, 0x810030);
    RH_ScopedGlobalInstall(_rwResArenaHeapAlloc, 0x8100C0);
    RH_ScopedGlobalInstall(_rwResArenaHeapSplitBlock, 0x810140);
}

// 0x80FFE0 (lote R8A, sin validar; cdecl, 2 argumentos (memoria y tamano), `ret`, 68 bytes; llamadores: 0x807CF8 y 0x808165, la creacion del arena): primer bloque en (memoria + 0x27) & ~0x1F; su tamano util =
// ((memoria + tamano) & ~0x1F) - bloque - 0x20, con comparacion CON signo: si es menor que 0x20 devuelve FALSE (no escribe nada). Si no: bloque = {monton, 0, 0, tamano, 0}; primer bloque y
// punto de busqueda del monton = bloque. Devuelve TRUE.
RwBool _rwResArenaHeapInit(void* memory, RwInt32 size) {
    const RwUInt32 base = reinterpret_cast<RwUInt32>(memory);
    const RwUInt32 end = (base + static_cast<RwUInt32>(size)) & 0xFFFFFFE0u;
    ResArenaBlock* const block = reinterpret_cast<ResArenaBlock*>((base + 0x27) & 0xFFFFFFE0u);
    const RwInt32 room = static_cast<RwInt32>(end - reinterpret_cast<RwUInt32>(block) - 0x20);
    if (room < 0x20) {
        return FALSE;
    }
    block->heap = memory;
    block->next = nullptr;
    block->prev = nullptr;
    block->used = 0;
    block->size = static_cast<RwUInt32>(room);
    ResArenaHeap* const heap = static_cast<ResArenaHeap*>(memory);
    heap->first = block;
    heap->searchStart = block;
    return TRUE;
}

// 0x810030 (lote R8A, sin validar; cdecl, 1 argumento, `ret`, 6 bytes): `mov eax, 1; ret`. Destruccion del monton: no hace nada (llamadores: 0x807D95 y 0x808106).
RwBool _rwResArenaHeapDestroy(void* heap) {
    return TRUE;
}

// 0x8100C0 (lote R8A, sin validar; cdecl, 2 argumentos (monton y tamano), `ret`, 119 bytes; llamadores: 0x807EEE (RwResourcesAllocateResEntry), 0x8101C0 y 0x8102EB): reserva `tamano` redondeado a
// multiplos de 32 bytes: el primer bloque libre (bit 0 de `used` a 0) con tamano util >= el pedido, buscando desde heap->searchStart (NULL: devuelve NULL). Si sobran mas de 0x40 bytes
// lo parte (0x810140). Si era el punto de busqueda, lo avanza hasta el siguiente bloque libre (o NULL). Lo marca ocupado y devuelve bloque + 0x20.
void* _rwResArenaHeapAlloc(void* heapPointer, RwUInt32 size) {
    ResArenaHeap* const heap = static_cast<ResArenaHeap*>(heapPointer);
    const RwUInt32 rounded = (size + 0x1F) & 0xFFFFFFE0u;
    ResArenaBlock* best = nullptr;
    ResArenaBlock* block = heap->searchStart;
    if (block == nullptr) {
        return nullptr;
    }
    while (block != nullptr && best == nullptr) {
        if ((block->used & 1) == 0 && block->size >= rounded) {
            best = block;
        }
        block = block->next;
    }
    if (best == nullptr) {
        return nullptr;
    }
    if (best->size > rounded + 0x40) {
        _rwResArenaHeapSplitBlock(best, rounded);
    }
    if (best == heap->searchStart) {
        do {
            ResArenaBlock* const next = heap->searchStart->next;
            heap->searchStart = next;
            if (next == nullptr) {
                break;
            }
        } while ((heap->searchStart->used & 1) != 0);
    }
    best->used = 1;
    return reinterpret_cast<RwUInt8*>(best) + 0x20;
}

// 0x810140 (lote R8A, sin validar; cdecl, 2 argumentos (bloque y tamano nuevo), `ret`, 99 bytes; llamadores: 0x81010A, 0x810217): parte el bloque en dos: el nuevo empieza en bloque + tamano + 0x20.
// Si el siguiente esta libre se funde con el: el nuevo toma su `next` y su tamano = tamano del siguiente - tamano + tamano del bloque; si no (u ocupado o NULL) el nuevo toma `next` y
// tamano = tamano del bloque - tamano - 0x20. Enlaza (el siguiente del nuevo, si existe, apunta atras al nuevo), el nuevo queda libre con el monton del bloque y el bloque con `tamano`.
// Devuelve el bloque nuevo (EAX al salir; ningun llamador lo lee).
void* _rwResArenaHeapSplitBlock(void* blockPointer, RwUInt32 size) {
    ResArenaBlock* const block = static_cast<ResArenaBlock*>(blockPointer);
    ResArenaBlock* const next = block->next;
    ResArenaBlock* const split = reinterpret_cast<ResArenaBlock*>(reinterpret_cast<RwUInt8*>(block) + size + 0x20);
    RwUInt32 splitSize;
    if (next != nullptr && (next->used & 1) == 0) {
        split->next = next->next;
        splitSize = next->size - size + block->size;
    } else {
        split->next = next;
        splitSize = block->size - size - 0x20;
    }
    split->size = splitSize;
    block->next = split;
    ResArenaBlock* const after = split->next;
    split->used = 0;
    split->prev = block;
    if (after != nullptr) {
        after->prev = split;
    }
    block->size = size;
    split->heap = block->heap;
    return split;
}

// Lote R12A (sin validar): flujos, vectores y matrices de RwPlCore (0x7EC810..0x7F1930).
void RwPlCoreInjectHooksR12A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(_rwStreamInitialize, 0x7EC810);
    RH_ScopedGlobalInstall(RwStreamSkip, 0x7ECD00);
    RH_ScopedGlobalInstall(RwStreamClose, 0x7ECE20);
    RH_ScopedGlobalInstall(RwStreamOpen, 0x7ECEF0);
    RH_ScopedGlobalInstall(_rwStreamReadChunkHeader, 0x7ED0F0);
    RH_ScopedGlobalInstall(_rwStreamChunkIsComplex, 0x7ED1E0);
    RH_ScopedGlobalInstall(_rwStreamWriteVersionedChunkHeader, 0x7ED270);
    RH_ScopedGlobalInstall(RwStreamFindChunk, 0x7ED2D0);
    RH_ScopedGlobalInstall(RwStreamWriteReal, 0x7ED3D0);
    RH_ScopedGlobalInstall(RwStreamReadReal, 0x7ED4F0);
    RH_ScopedGlobalInstall(RwStreamReadInt32, 0x7ED540);
    RH_ScopedGlobalInstall(RwStreamReadChunkHeaderInfo, 0x7ED590);
    RH_ScopedGlobalInstall(_rwVectorSetMultFn, 0x7ED5F0);
    RH_ScopedGlobalInstall(_rwV3dTransformPointDefault, 0x7ED730);
    RH_ScopedGlobalInstall(RwV3dLength, 0x7EDAC0);
    RH_ScopedGlobalInstall(_rwInvSqrt, 0x7EDB90);
    RH_ScopedGlobalInstall(RwV3dTransformPoint, 0x7EDD60);
    RH_ScopedGlobalInstall(RwV3dTransformVector, 0x7EDDC0);
    RH_ScopedGlobalInstall(RwV3dTransformVectors, 0x7EDDF0);
    RH_ScopedGlobalInstall(_rwMatrixDeterminant, 0x7F1450);
    RH_ScopedGlobalInstall(_rwMatrixOrthogonalError, 0x7F14A0);
    RH_ScopedGlobalInstall(_rwMatrixNormalError, 0x7F1500);
    RH_ScopedGlobalInstall(_rwMatrixIdentityError, 0x7F1590);
    RH_ScopedGlobalInstall(RwMatrixOptimize, 0x7F17E0);
    RH_ScopedGlobalInstall(RwMatrixOrthoNormalizeImpl, 0x7F1930);
}
