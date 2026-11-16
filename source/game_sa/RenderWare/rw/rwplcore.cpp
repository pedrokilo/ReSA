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

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R14A (sin validar): listas libres (RwFreeList), memoria y sistema de ficheros del motor, registro de plugins y arena de recursos de RwPlCore.
// Variables del EXE: cabeza de la lista global de listas libres (0xC9A604: next, 0xC9A608: prev), indicador (0xC9A630), la lista libre de listas libres (0xC9A634; su
// estructura vive en el espacio fijo 0xC9A60C), modo de prereserva (0x8E266C), lista libre de entradas de plugin (0xC9A6E4; estructura en 0xC9A6C0), matriz de registros
// (0xC9A6E8, cantidad 0xC9A6EC), parametros de esa lista (0x8E26AC bloque, 0x8E26B0 bloques), desplazamiento del error en el motor (0xC9A6F0) y del modulo de recursos (0xC9BC58).
// Bit de la mascara de uso de un bloque: el bit 7 del primer byte es la entrada 0 (0x80 >> (indice & 7)); bit puesto = entrada usada.
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
static_assert(sizeof(RwFreeList) == 0x24 && offsetof(RwFreeList, heapSize) == 8 && offsetof(RwFreeList, blockList) == 0x10 && offsetof(RwFreeList, flags) == 0x18 && offsetof(RwFreeList, link) == 0x1C);
static_assert(sizeof(RwMemoryFunctions) == 0x10 && sizeof(RwPluginRegEntry) == 0x3C && offsetof(RwPluginRegEntry, constructCB) == 0x20 && offsetof(RwPluginRegEntry, parentRegistry) == 0x38);
static_assert(sizeof(RwResEntry) == 0x18 && offsetof(RwResEntry, destroyNotify) == 0x14);

RwLLLink&    FreeListsHead()          { return StaticRef<RwLLLink>(0xC9A604); }
RwInt32&     FreeListsReady()         { return StaticRef<RwInt32>(0xC9A630); }
RwFreeList*& FreeListOfFreeLists()    { return StaticRef<RwFreeList*>(0xC9A634); }
RwInt32&     FreeListPreallocMode()   { return StaticRef<RwInt32>(0x8E266C); }
constexpr uintptr_t kFreeListOfFreeListsStorage = 0xC9A60C;
constexpr uintptr_t kDefaultFreeListAlloc       = 0x801C30; // RwFreeListAlloc por defecto (memoryAlloc del motor, +0x144)

// Retira un nodo de un anillo doble: prev->next = next y next->prev = prev (el EXE relee los campos del nodo tras la primera escritura).
void LinkRemove(RwLLLink* link) {
    link->prev->next = link->next;
    link->next->prev = link->prev;
}
// Libera con rwfree (+0x138) todos los bloques del anillo de bloques de la lista libre, uno a uno desde el primero.
void FreeAllBlocks(RwFreeList* freeList) {
    RwLLLink* const head = &freeList->blockList.link;
    while (head->next != head) {
        RwLLLink* const block = head->next;
        LinkRemove(block);
        RwEngineInstance->memoryFuncs.rwfree(block);
    }
}
// Suma de bytes (sin signo) de la mascara de uso de un bloque.
RwUInt32 UsedMaskSum(const RwLLLink* block, RwUInt32 maskBytes) {
    const RwUInt8* const mask = reinterpret_cast<const RwUInt8*>(block) + 8;
    RwUInt32 sum = 0;
    for (RwUInt32 i = 0; i < maskBytes; i++) {
        sum += mask[i];
    }
    return sum;
}
// Libera la estructura de una lista libre no estatica: con lista libre de listas libres distinta de la propia y no nula, memoryFree(esa, estructura); si no rwfree.
// Devuelve true si uso memoryFree.
bool ReleaseFreeListStruct(RwFreeList* freeList) {
    RwFreeList* const holder = FreeListOfFreeLists();
    if (holder != freeList && holder != nullptr) {
        RwEngineInstance->memoryFree(holder, freeList);
        return true;
    }
    RwEngineInstance->memoryFuncs.rwfree(freeList);
    return false;
}
} // namespace

// 0x8019B0 (lote R14A, sin validar; 433 bytes): RwFreeListCreateAndPreallocateSpace real (el simbolo publico 0x801B70 es un JMP a esta direccion). cdecl (entrySize, entriesPerBlock, alignment,
// numBlocksToPreallocate, inPlaceSpace, hint). Con el modo de prereserva (0x8E266C) a 0 no se prereservan bloques. alignment 0 -> 4. Sin espacio propio: la estructura sale de la lista libre de
// listas libres (memoryAlloc +0x144 con hint & 0xFF0000) o, si esa no existe aun, de rwmalloc(0x24, hint & 0xFF0000); NULL -> NULL; flags = 2 (liberar bloques); con espacio propio flags = 3.
// entrySize se redondea al alineamiento; heapSize (bytes de mascara) = (entradas + 7) >> 3; los bloques miden entrySize*entradas + mascara + alineamiento + 7 (rwmalloc con el hint completo), con
// la mascara a cero y se enlazan DESPUES de la cabeza del anillo; si rwmalloc falla se liberan los bloques ya reservados, se libera la estructura (salvo estatica) y se devuelve NULL. Al final la
// lista se enlaza despues de la cabeza global 0xC9A604.
RwFreeList* FreeListCreateAndPreallocateSpaceBody(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint) {
    RwGlobals* const engine = RwEngineInstance;
    if (FreeListPreallocMode() == 0) {
        numBlocksToPreallocate = 0;
    }
    if (alignment == 0) {
        alignment = 4;
    }
    RwFreeList* freeList = inPlaceSpaceForFreeListStruct;
    if (freeList == nullptr) {
        RwFreeList* const holder = FreeListOfFreeLists();
        if (holder != nullptr) {
            freeList = static_cast<RwFreeList*>(engine->memoryAlloc(holder, hint & 0xFF0000));
        } else {
            freeList = static_cast<RwFreeList*>(engine->memoryFuncs.rwmalloc(sizeof(RwFreeList), hint & 0xFF0000));
        }
        if (freeList == nullptr) {
            return nullptr;
        }
        freeList->flags = rwFREELISTFLAG_FREEBLOCKS;
    } else {
        freeList->flags = rwFREELISTFLAG_STATIC | rwFREELISTFLAG_FREEBLOCKS;
    }
    const RwUInt32 align = static_cast<RwUInt32>(alignment);
    freeList->entrySize       = (static_cast<RwUInt32>(entrySize) + align - 1) & ~(align - 1);
    freeList->entriesPerBlock = static_cast<RwUInt32>(entriesPerBlock);
    freeList->heapSize        = (static_cast<RwUInt32>(entriesPerBlock) + 7) >> 3;
    freeList->alignment       = align;
    RwLLLink* const head = &freeList->blockList.link;
    head->next = head;
    head->prev = head;
    if (numBlocksToPreallocate != 0) {
        const RwUInt32 blockSize = freeList->entrySize * freeList->entriesPerBlock + freeList->heapSize + align + 7;
        RwUInt32 remaining = static_cast<RwUInt32>(numBlocksToPreallocate);
        do {
            void* const memory = engine->memoryFuncs.rwmalloc(blockSize, hint);
            if (memory == nullptr) {
                FreeAllBlocks(freeList);
                if ((freeList->flags & rwFREELISTFLAG_STATIC) == 0) {
                    ReleaseFreeListStruct(freeList);
                }
                return nullptr;
            }
            RwLLLink* const block = static_cast<RwLLLink*>(memory);
            block->prev = head;
            block->next = head->next;
            head->next->prev = block;
            head->next = block;
            memset(static_cast<RwUInt8*>(memory) + 8, 0, freeList->heapSize);
            remaining--;
        } while (remaining != 0);
    }
    RwLLLink& globalHead = FreeListsHead();
    freeList->link.prev = &globalHead;
    freeList->link.next = globalHead.next;
    globalHead.next->prev = &freeList->link;
    globalHead.next = &freeList->link;
    return freeList;
}

// 0x801970 (lote R14A, sin validar; 10 bytes): guarda su argumento en 0x8E266C, el modo de prereserva de bloques de las listas libres (RwEngineInit pasa 0 con initFlags & 1 y 1 si no).
// Nombre inferido.
void _rwFreeListSetPreallocationMode(RwInt32 mode) {
    FreeListPreallocMode() = mode;
}

// 0x801FD0 (lote R14A, sin validar; 199 bytes): abre el modulo de memoria del motor (nombre inferido). Anillo global de listas libres vacio (0xC9A604), indicador 0xC9A630 = 1 y crea la lista libre de
// listas libres: entradas de 0x24 bytes, 0x10 por bloque, alineamiento 4, ningun bloque prereservado, estructura en el espacio fijo 0xC9A60C y hint 0x40000; la guarda en 0xC9A634. NULL ->
// indicador = 0 y devuelve 0. Si no la saca del anillo global y copia las cuatro funciones de memoria del argumento (memFuncs) a motor + 0x134 (malloc, free, realloc, calloc) o, con NULL, usa las del
// EXE (0x8020A0, 0x82413F, 0x8020B0, 0x8020D0). Devuelve 1.
RwBool _rwMemoryOpen(const RwMemoryFunctions* memFuncs) {
    RwLLLink& globalHead = FreeListsHead();
    globalHead.next = &globalHead;
    globalHead.prev = &globalHead;
    FreeListsReady() = 1;
    RwFreeList* const holder = FreeListCreateAndPreallocateSpaceBody(0x24, 0x10, 4, 0, reinterpret_cast<RwFreeList*>(kFreeListOfFreeListsStorage), 0x40000);
    FreeListOfFreeLists() = holder;
    if (holder == nullptr) {
        FreeListsReady() = 0;
        return FALSE;
    }
    LinkRemove(&holder->link);
    RwGlobals* const engine = RwEngineInstance;
    if (memFuncs != nullptr) {
        engine->memoryFuncs.rwmalloc  = memFuncs->rwmalloc;
        engine->memoryFuncs.rwfree    = memFuncs->rwfree;
        engine->memoryFuncs.rwrealloc = memFuncs->rwrealloc;
        engine->memoryFuncs.rwcalloc  = memFuncs->rwcalloc;
        return TRUE;
    }
    engine->memoryFuncs.rwmalloc  = reinterpret_cast<decltype(engine->memoryFuncs.rwmalloc)>(0x8020A0);
    engine->memoryFuncs.rwfree    = reinterpret_cast<decltype(engine->memoryFuncs.rwfree)>(0x82413F);
    engine->memoryFuncs.rwrealloc = reinterpret_cast<decltype(engine->memoryFuncs.rwrealloc)>(0x8020B0);
    engine->memoryFuncs.rwcalloc  = reinterpret_cast<decltype(engine->memoryFuncs.rwcalloc)>(0x8020D0);
    return TRUE;
}

// 0x8020F0 (lote R14A, sin validar; 312 bytes): cierra el modulo de memoria (nombre inferido). Mientras el anillo global tenga listas: la saca, libera todos sus bloques con rwfree y, si no es estatica
// (flags & 1 == 0), libera su estructura (memoryFree(0xC9A634, lista) si esa es no nula y distinta de la lista; si no rwfree). Despues hace lo mismo con la lista libre de listas libres
// (0xC9A634: la saca del anillo, libera sus bloques y su estructura si no es estatica; la suya es estatica y no se libera) y deja 0xC9A634 y 0xC9A630 a 0.
void _rwMemoryClose() {
    RwLLLink& globalHead = FreeListsHead();
    while (globalHead.next != &globalHead) {
        RwLLLink* const node = globalHead.next;
        RwFreeList* const freeList = reinterpret_cast<RwFreeList*>(reinterpret_cast<RwUInt8*>(node) - offsetof(RwFreeList, link));
        LinkRemove(node);
        FreeAllBlocks(freeList);
        if ((freeList->flags & rwFREELISTFLAG_STATIC) == 0) {
            ReleaseFreeListStruct(freeList);
        }
    }
    RwFreeList* const holder = FreeListOfFreeLists();
    LinkRemove(&holder->link);
    FreeAllBlocks(holder);
    if ((holder->flags & rwFREELISTFLAG_STATIC) == 0) {
        if (ReleaseFreeListStruct(holder)) {
            FreeListOfFreeLists() = nullptr;
            FreeListsReady() = 0;
            return;
        }
    }
    FreeListOfFreeLists() = nullptr;
    FreeListsReady() = 0;
}

// 0x801D50 (lote R14A, sin validar; 169 bytes): RwFreeListFree por defecto (memoryFree del motor, +0x148; nombre propio porque RwFreeListFree es una macro). cdecl (lista, puntero). Busca el bloque
// cuya zona de datos [bloque + mascara + 8, esa + entradas*tamano] (limite superior INCLUIDO) contiene el puntero; sin bloque devuelve NULL. Entrada = (puntero - inicio) / entrySize (division sin signo);
// borra su bit de la mascara de uso. Con flags & 2 y la mascara a cero (suma de bytes == 0) saca el bloque del anillo y lo libera con rwfree. Devuelve la lista.
RwFreeList* _rwFreeListFreeDefault(RwFreeList* freeList, void* data) {
    RwLLLink* const head = &freeList->blockList.link;
    const RwUInt32 maskBytes = freeList->heapSize;
    for (RwLLLink* block = head->next; block != head; block = block->next) {
        const RwUInt32 start = reinterpret_cast<RwUInt32>(block) + maskBytes + 8;
        const RwUInt32 address = reinterpret_cast<RwUInt32>(data);
        if (address < start || address > start + freeList->entriesPerBlock * freeList->entrySize) {
            continue;
        }
        const RwUInt32 index = (address - start) / freeList->entrySize;
        const RwUInt32 byteIndex = index >> 3;
        RwUInt8* const mask = reinterpret_cast<RwUInt8*>(block) + 8;
        mask[byteIndex] &= static_cast<RwUInt8>(~(0x80 >> (index - byteIndex * 8)));
        if ((freeList->flags & rwFREELISTFLAG_FREEBLOCKS) != 0 && UsedMaskSum(block, maskBytes) == 0) {
            LinkRemove(block);
            RwEngineInstance->memoryFuncs.rwfree(block);
        }
        return freeList;
    }
    return nullptr;
}

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
    return FreeListCreateAndPreallocateSpaceBody(entrySize, entriesPerBlock, alignment, 1, nullptr, hint);
}

// 0x801B70: salto directo a 0x8019B0 con los mismos seis argumentos.
RwFreeList* RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint) {
    return FreeListCreateAndPreallocateSpaceBody(entrySize, entriesPerBlock, alignment, numBlocksToPreallocate, inPlaceSpaceForFreeListStruct, hint);
}

// 0x801B80 (lote R14A, sin validar; 136 bytes; era una envoltura): saca la lista del anillo global, libera todos sus bloques con rwfree y, si no es estatica (flags & 1 == 0), libera la estructura con
// memoryFree(0xC9A634, lista) (si esa es no nula y distinta de la lista; en ese caso sale ya) o con rwfree. Devuelve siempre 1.
RwBool RwFreeListDestroy(RwFreeList* freelist) {
    LinkRemove(&freelist->link);
    FreeAllBlocks(freelist);
    if ((freelist->flags & rwFREELISTFLAG_STATIC) == 0) {
        ReleaseFreeListStruct(freelist);
    }
    return TRUE;
}

void RwFreeListSetFlags(RwFreeList* freeList, RwUInt32 flags) {
    ((void(__cdecl *)(RwFreeList*, RwUInt32))0x801C10)(freeList, flags);
}

RwUInt32 RwFreeListGetFlags(RwFreeList* freeList) {
    return ((RwUInt32(__cdecl *)(RwFreeList*))0x801C20)(freeList);
}

// 0x801E00 (lote R14A, sin validar; 138 bytes; era una envoltura): recorre los bloques de la lista; cada uno se saca del anillo y, si su mascara de uso suma 0 (ninguna entrada usada), se libera con rwfree
// y se cuenta; si no se reinserta como PRIMER bloque. Devuelve entrySize * (bloques liberados) (sin signo de 32 bits como el imul del EXE).
RwInt32 RwFreeListPurge(RwFreeList* freelist) {
    RwInt32 freedBlocks = 0;
    RwLLLink* const head = &freelist->blockList.link;
    const RwUInt32 maskBytes = freelist->heapSize;
    RwLLLink* block = head->next;
    if (block != head) {
        for (;;) {
            LinkRemove(block);
            RwLLLink* const next = block->next;
            if (UsedMaskSum(block, maskBytes) == 0) {
                RwEngineInstance->memoryFuncs.rwfree(block);
                freedBlocks++;
            } else {
                block->prev = head;
                block->next = head->next;
                head->next->prev = block;
                head->next = block;
            }
            if (next == head) {
                break;
            }
            block = next;
        }
    }
    return static_cast<RwInt32>(freelist->entrySize * static_cast<RwUInt32>(freedBlocks));
}

// 0x801E90 (lote R14A, sin validar; 254 bytes; era una envoltura): para cada bloque (en orden) copia su mascara de uso a un bufer (rwmalloc(bytes, 0x10000); NULL -> devuelve NULL) y, por cada bit puesto
// (el bit 7 del byte 0 es la entrada 0), llama a fpCallBack(entrada, data) con entrada = ((bloque + alineamiento + mascara + 7) & ~(alineamiento - 1)) + indice * entrySize. El resultado del callback se
// ignora; el siguiente bloque se lee antes de los callbacks y el bufer se libera al final de cada bloque. Devuelve la lista.
RwFreeList* RwFreeListForAllUsed(RwFreeList* freelist, RwFreeListCallBack fpCallBack, void* data) {
    RwLLLink* const head = &freelist->blockList.link;
    const RwUInt32 maskBytes = freelist->heapSize;
    RwLLLink* current = head->next;
    if (current == head) {
        return freelist;
    }
    for (;;) {
        RwUInt8* const copy = static_cast<RwUInt8*>(RwEngineInstance->memoryFuncs.rwmalloc(maskBytes, 0x10000));
        if (copy == nullptr) {
            return nullptr;
        }
        memcpy(copy, reinterpret_cast<RwUInt8*>(current) + 8, maskBytes);
        RwLLLink* const next = current->next;
        for (RwUInt32 byteIndex = 0; byteIndex < maskBytes; byteIndex++) {
            const RwUInt32 maskByte = copy[byteIndex];
            if (maskByte == 0) {
                continue;
            }
            for (RwUInt32 bit = 0; bit < 8; bit++) {
                if ((maskByte & (0x80u >> bit)) == 0) {
                    continue;
                }
                const RwUInt32 alignment = freelist->alignment;
                const RwUInt32 firstEntry = (reinterpret_cast<RwUInt32>(current) + alignment + maskBytes + 7) & ~(alignment - 1);
                fpCallBack(reinterpret_cast<void*>(firstEntry + (bit + byteIndex * 8) * freelist->entrySize), data);
            }
        }
        RwEngineInstance->memoryFuncs.rwfree(copy);
        current = next;
        if (current == head) {
            return freelist;
        }
    }
}

// 0x801F90 (lote R14A, sin validar; 51 bytes; era una envoltura): RwFreeListPurge de cada lista del anillo global (0xC9A604) y suma de los resultados positivos (comparacion con signo).
RwInt32 RwFreeListPurgeAllFreeLists() {
    RwLLLink& globalHead = FreeListsHead();
    RwInt32 total = 0;
    for (RwLLLink* node = globalHead.next; node != &globalHead; node = node->next) {
        const RwInt32 purged = RwFreeListPurge(reinterpret_cast<RwFreeList*>(reinterpret_cast<RwUInt8*>(node) - offsetof(RwFreeList, link)));
        if (purged > 0) {
            total += purged;
        }
    }
    return total;
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

// Lote R15A (sin validar). 0x80A240 (272 bytes; era una envoltura): rellena las 17 funciones de cadena del motor (RwEngineInstance + 0xF0..0x130) con direcciones fijas del EXE, en orden:
// sprintf 0x821BB5, vsprintf 0x8230B8, strcpy 0x826590, strncpy 0x821F40, strcat 0x8265A0, strncat 0x826450, strrchr 0x80A420, strchr 0x80A400, strstr 0x822650, strcmp 0x8263C0,
// strncmp 0x8214D0, stricmp 0x80A350, strlen 0x826330, strupr 0x80A3A0, strlwr 0x80A3D0, strtok 0x82244B y sscanf 0x8220AD. Devuelve 1.
RwBool _rwStringOpen() {
    RwStringFunctions& funcs = RwEngineInstance->stringFuncs;
    funcs.vecSprintf = reinterpret_cast<vecSprintfFunc>(0x821BB5);
    funcs.vecVsprintf = reinterpret_cast<vecVsprintfFunc>(0x8230B8);
    funcs.vecStrcpy = reinterpret_cast<vecStrcpyFunc>(0x826590);
    funcs.vecStrncpy = reinterpret_cast<vecStrncpyFunc>(0x821F40);
    funcs.vecStrcat = reinterpret_cast<vecStrcatFunc>(0x8265A0);
    funcs.vecStrncat = reinterpret_cast<vecStrncatFunc>(0x826450);
    funcs.vecStrrchr = reinterpret_cast<vecStrrchrFunc>(0x80A420);
    funcs.vecStrchr = reinterpret_cast<vecStrchrFunc>(0x80A400);
    funcs.vecStrstr = reinterpret_cast<vecStrstrFunc>(0x822650);
    funcs.vecStrcmp = reinterpret_cast<vecStrcmpFunc>(0x8263C0);
    funcs.vecStrncmp = reinterpret_cast<vecStrncmpFunc>(0x8214D0);
    funcs.vecStricmp = reinterpret_cast<vecStricmpFunc>(0x80A350);
    funcs.vecStrlen = reinterpret_cast<vecStrlenFunc>(0x826330);
    funcs.vecStrupr = reinterpret_cast<vecStruprFunc>(0x80A3A0);
    funcs.vecStrlwr = reinterpret_cast<vecStrlwrFunc>(0x80A3D0);
    funcs.vecStrtok = reinterpret_cast<vecStrtokFunc>(0x82244B);
    funcs.vecSscanf = reinterpret_cast<vecSscanfFunc>(0x8220AD);
    return TRUE;
}

// 0x80A350 (lote R15A, sin validar; 72 bytes; cdecl, (a, b); nombre inferido: stricmp por defecto del motor): si alguna cadena es NULL devuelve 0. Compara byte a byte CON signo; las letras
// A-Z (0x41..0x5A, comparacion con signo) se pasan a minuscula sumando 0x20. Si difieren devuelve la resta (con signo) de los dos bytes ya convertidos; si son iguales y alguno es 0 devuelve 0.
int _rwStringStricmp(const RwChar* string1, const RwChar* string2) {
    if (string1 == nullptr || string2 == nullptr) {
        return 0;
    }
    for (;;) {
        signed char a = static_cast<signed char>(*string1);
        signed char b = static_cast<signed char>(*string2);
        if (a >= 0x41 && a <= 0x5A) {
            a = static_cast<signed char>(a + 0x20);
        }
        if (b >= 0x41 && b <= 0x5A) {
            b = static_cast<signed char>(b + 0x20);
        }
        if (a != b) {
            return static_cast<int>(a) - static_cast<int>(b);
        }
        ++string1;
        ++string2;
        if (a == 0 || b == 0) {
            return 0;
        }
    }
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

// 0x80A4A0 (lote R15A, sin validar; 102 bytes; era una envoltura; cdecl, (cadena, flujo)): cadena NULL -> la cadena vacia del EXE (0x884F6A). tamano = (rwstrlen + 4) & ~3 (rwstrlen = [motor + 0x120]);
// escribe la cabecera de bloque (tipo 2, tamano, version 0x36003, build 0xFFFF) con 0x7ED270 (NULL -> devuelve NULL) y despues RwStreamWrite(flujo, cadena, tamano) (0x7ECB30): devuelve la cadena
// (la sustituida si era NULL) si la escritura dio un resultado distinto de NULL, si no NULL.
const RwChar* _rwStringStreamWrite(const RwChar* string, RwStream* stream) {
    if (string == nullptr) {
        string = reinterpret_cast<const RwChar*>(0x884F6A);
    }
    const uint32 length = static_cast<uint32>(RWSRCGLOBAL(stringFuncs).vecStrlen(string));
    const uint32 size = (length + 4u) & ~3u;
    if (_rwStreamWriteVersionedChunkHeader(stream, 2, static_cast<RwInt32>(size), 0x36003u, 0xFFFFu) == nullptr) {
        return nullptr;
    }
    return RwStreamWrite(stream, string, size) != nullptr ? string : nullptr;
}

// 0x80A510 (lote R15A, sin validar; 609 bytes; era una envoltura; cdecl, (destino o NULL, flujo)): lee la cabecera de bloque (0x7ED0F0, sin build; fallo -> NULL) y la valida: version < 0x34000 o
// > 0x36003 (sin signo) -> error 0x80000004 y NULL. Tipo 2 (cadena de 8 bits) y tipo 0x13 (cadena de 16 bits): con destino NULL reserva `longitud` bytes con rwmalloc(longitud, 0x30002) (fallo: error
// 0x80000013 con la longitud y NULL). Tipo 2: copia en trozos de 0x40 bytes (RwStreamRead 0x7EC9D0; si lee menos, NULL SIN liberar). Tipo 0x13: trozos de 0x80 bytes y se queda con el byte bajo de
// cada par (chunk >> 1 bytes por trozo; un byte final impar se descarta); si lee menos y habia reservado, rwfree y NULL. Ambos devuelven el destino SIN terminar la cadena. Otros tipos: RwStreamSkip(longitud)
// (NULL -> NULL) y siguiente cabecera (fallo -> NULL).
RwChar* _rwStringStreamFindAndRead(RwChar* string, RwStream* stream) {
    RwUInt32 type, length, version;
    if (!_rwStreamReadChunkHeader(stream, &type, &length, &version, nullptr)) {
        return nullptr;
    }
    for (;;) {
        if (version < 0x34000u || version > 0x36003u) {
            StreamRaiseError(static_cast<RwInt32>(0x80000004));
            return nullptr;
        }
        if (type == 2) {
            RwChar* destination = string;
            if (destination == nullptr) {
                destination = static_cast<RwChar*>(RwEngineInstance->memoryFuncs.rwmalloc(length, 0x30002));
                if (destination == nullptr) {
                    StreamRaiseError(static_cast<RwInt32>(0x80000013), static_cast<RwInt32>(length));
                    return nullptr;
                }
            }
            RwChar* cursor = destination;
            RwUInt32 remaining = length;
            while (remaining != 0) {
                const RwUInt32 chunk = remaining > 0x40u ? 0x40u : remaining;
                RwUInt8 buffer[0x40];
                if (RwStreamRead(stream, buffer, chunk) != chunk) {
                    return nullptr;
                }
                remaining -= chunk;
                for (RwUInt32 i = 0; i < chunk; ++i) {
                    cursor[i] = static_cast<RwChar>(buffer[i]);
                }
                cursor += chunk;
            }
            return destination;
        }
        if (type == 0x13) {
            RwChar* destination = string;
            bool allocated = false;
            if (destination == nullptr) {
                destination = static_cast<RwChar*>(RwEngineInstance->memoryFuncs.rwmalloc(length, 0x30002));
                if (destination == nullptr) {
                    StreamRaiseError(static_cast<RwInt32>(0x80000013), static_cast<RwInt32>(length));
                    return nullptr;
                }
                allocated = true;
            }
            RwChar* cursor = destination;
            RwUInt32 remaining = length;
            while (remaining != 0) {
                const RwUInt32 chunk = remaining > 0x80u ? 0x80u : remaining;
                RwUInt8 buffer[0x80];
                if (RwStreamRead(stream, buffer, chunk) != chunk) {
                    if (allocated) {
                        RwEngineInstance->memoryFuncs.rwfree(destination);
                    }
                    return nullptr;
                }
                remaining -= chunk;
                const RwUInt32 half = chunk >> 1;
                for (RwUInt32 i = 0; i < half; ++i) {
                    cursor[i] = static_cast<RwChar>(buffer[i * 2]);
                }
                cursor += half;
            }
            return destination;
        }
        if (RwStreamSkip(stream, length) == nullptr) {
            return nullptr;
        }
        if (!_rwStreamReadChunkHeader(stream, &type, &length, &version, nullptr)) {
            return nullptr;
        }
    }
}

void RwPluginRegistrySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x8087B0)(blockSize, numBlocksToPrealloc);
}

RwBool _rwPluginRegistrySetStaticPluginsSize(RwPluginRegistry* reg, RwInt32 size) {
    return ((RwBool(__cdecl *)(RwPluginRegistry*, RwInt32))0x808430)(reg, size);
}

// 0x8084A0 (lote R14A, sin validar; 515 bytes; era una envoltura): cdecl (registro, tamano, pluginID, constructor, destructor, copia). Sin lista libre de entradas (0xC9A6E4 nula): -1 sin error. Con el motor
// ya abierto (7F2B90 != 0): error 0x80000017 y -1. Si memoryAlloc del motor NO es el RwFreeListAlloc por defecto (0x801C30), anade el registro a la matriz de registros 0xC9A6E8 (cantidad 0xC9A6EC; nueva
// matriz de n+1 punteros con rwmalloc(hint 0x40000), copia de los anteriores y rwfree de la vieja) si no estaba. Busca el pluginID entre las entradas: si ya existe, error 0x80000017 y devuelve SU
// desplazamiento. Si no: nuevo tamano = sizeOfStruct + ((tamano + 3) & ~3); con maxSizeOfStruct != 0 y nuevo tamano > max (con signo) devuelve -1 (sin error); entrada = memoryAlloc(lista libre de entradas,
// 0x40000) (NULL -> -1); entrada.offset = sizeOfStruct anterior, size = tamano SIN redondear, pluginID, los cinco callbacks de flujo a 0, constructor/destructor/copia (nulos -> 0x8086B0, 0x8086C0, 0x8086D0),
// errStr/next/prev a 0, parent = registro. Se enlaza al final de la lista y devuelve su desplazamiento.
RwInt32 _rwPluginRegistryAddPlugin(RwPluginRegistry* reg, RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    RwFreeList* const entryList = StaticRef<RwFreeList*>(0xC9A6E4);
    if (entryList == nullptr) {
        return -1;
    }
    if (StaticRef<RwInt32>(0xC97B20) != 0) { // RwEngineGetOpenCount (0x7F2B90)
        StreamRaiseError(static_cast<RwInt32>(0x80000017));
        return -1;
    }
    RwGlobals* const engine = RwEngineInstance;
    if (reinterpret_cast<uintptr_t>(engine->memoryAlloc) != kDefaultFreeListAlloc) {
        RwPluginRegistry**& registries = StaticRef<RwPluginRegistry**>(0xC9A6E8);
        RwUInt32& registryCount = StaticRef<RwUInt32>(0xC9A6EC);
        RwUInt32 found = 0;
        for (; found < registryCount; found++) {
            if (registries[found] == reg) {
                break;
            }
        }
        if (registryCount == found) {
            RwPluginRegistry** const grown = static_cast<RwPluginRegistry**>(engine->memoryFuncs.rwmalloc(registryCount * 4 + 4, 0x40000));
            RwUInt32 copied = 0;
            if (registries != nullptr) {
                while (copied < registryCount) {
                    grown[copied] = registries[copied];
                    copied++;
                }
                engine->memoryFuncs.rwfree(registries);
            }
            grown[copied] = reg;
            registries = grown;
            registryCount = registryCount + 1;
        }
    }
    for (RwPluginRegEntry* existing = reg->firstRegEntry; existing != nullptr; existing = existing->nextRegEntry) {
        if (existing->pluginID == pluginID) {
            StreamRaiseError(static_cast<RwInt32>(0x80000017));
            return existing->offset;
        }
    }
    const RwInt32 newSize = ((size + 3) & ~3) + reg->sizeOfStruct;
    if (reg->maxSizeOfStruct != 0 && newSize > reg->maxSizeOfStruct) {
        return -1;
    }
    RwPluginRegEntry* const entry = static_cast<RwPluginRegEntry*>(engine->memoryAlloc(entryList, 0x40000));
    if (entry == nullptr) {
        return -1;
    }
    entry->offset = reg->sizeOfStruct;
    reg->sizeOfStruct = newSize;
    entry->size = size;
    entry->pluginID = pluginID;
    entry->readCB = nullptr;
    entry->writeCB = nullptr;
    entry->getSizeCB = nullptr;
    entry->alwaysCB = nullptr;
    entry->rightsCB = nullptr;
    entry->constructCB = constructCB != nullptr ? constructCB : reinterpret_cast<RwPluginObjectConstructor>(0x8086B0);
    entry->destructCB = destructCB != nullptr ? destructCB : reinterpret_cast<RwPluginObjectDestructor>(0x8086C0);
    entry->copyCB = copyCB != nullptr ? copyCB : reinterpret_cast<RwPluginObjectCopy>(0x8086D0);
    entry->errStrCB = nullptr;
    entry->nextRegEntry = nullptr;
    entry->prevRegEntry = nullptr;
    entry->parentRegistry = reg;
    if (reg->firstRegEntry == nullptr) {
        reg->firstRegEntry = entry;
        reg->lastRegEntry = entry;
        return entry->offset;
    }
    reg->lastRegEntry->nextRegEntry = entry;
    entry->prevRegEntry = reg->lastRegEntry;
    reg->lastRegEntry = entry;
    return entry->offset;
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

// 0x8086E0 (lote R14A, sin validar; 83 bytes; era una envoltura): llama al constructor (+0x20) de cada entrada en orden con (objeto, desplazamiento, tamano). Si uno devuelve NULL llama al destructor (+0x24)
// de las entradas ANTERIORES en orden inverso (no al de la que fallo) y devuelve NULL; si no devuelve el registro.
const RwPluginRegistry* _rwPluginRegistryInitObject(const RwPluginRegistry* reg, void* object) {
    for (RwPluginRegEntry* entry = reg->firstRegEntry; entry != nullptr; entry = entry->nextRegEntry) {
        if (entry->constructCB(object, entry->offset, entry->size) == nullptr) {
            for (RwPluginRegEntry* previous = entry->prevRegEntry; previous != nullptr; previous = previous->prevRegEntry) {
                previous->destructCB(object, previous->offset, previous->size);
            }
            return nullptr;
        }
    }
    return reg;
}

// 0x808740 (lote R14A, sin validar; 45 bytes; era una envoltura): destructor (+0x24) de cada entrada desde la ultima (+0x14) hacia atras (+0x34) con (objeto, desplazamiento, tamano); devuelve el registro.
const RwPluginRegistry* _rwPluginRegistryDeInitObject(const RwPluginRegistry* reg, void* object) {
    for (RwPluginRegEntry* entry = reg->lastRegEntry; entry != nullptr; entry = entry->prevRegEntry) {
        entry->destructCB(object, entry->offset, entry->size);
    }
    return reg;
}

// 0x808770 (lote R14A, sin validar; 52 bytes; era una envoltura): copia (+0x28) de cada entrada en orden con (destino, origen, desplazamiento, tamano); devuelve el registro.
const RwPluginRegistry* _rwPluginRegistryCopyObject(const RwPluginRegistry* reg, void* dstObject, const void* srcObject) {
    for (RwPluginRegEntry* entry = reg->firstRegEntry; entry != nullptr; entry = entry->nextRegEntry) {
        entry->copyCB(dstObject, srcObject, entry->offset, entry->size);
    }
    return reg;
}

namespace {
// Entrada del registro con ese pluginID (recorrido por nextRegEntry desde firstRegEntry) o NULL.
RwPluginRegEntry* FindRegEntry(const RwPluginRegistry* reg, RwUInt32 pluginID) {
    for (RwPluginRegEntry* entry = reg->firstRegEntry; entry != nullptr; entry = entry->nextRegEntry) {
        if (entry->pluginID == pluginID) {
            return entry;
        }
    }
    return nullptr;
}
}

// 0x8088E0 (lote R14A, sin validar; 55 bytes; era una envoltura): busca la entrada con ese pluginID; sin ella devuelve -1. Si no, readCB (+0xC), writeCB (+0x10) y getSizeCB (+0x14) y devuelve su desplazamiento.
RwInt32 _rwPluginRegistryAddPluginStream(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    RwPluginRegEntry* const entry = FindRegEntry(reg, pluginID);
    if (entry == nullptr) {
        return -1;
    }
    entry->readCB = readCB;
    entry->writeCB = writeCB;
    entry->getSizeCB = getSizeCB;
    return entry->offset;
}

// 0x808920 (lote R14A, sin validar; 41 bytes; era una envoltura): igual con alwaysCB (+0x18); sin entrada devuelve -1, si no su desplazamiento.
RwInt32 _rwPluginRegistryAddPlgnStrmlwysCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    RwPluginRegEntry* const entry = FindRegEntry(reg, pluginID);
    if (entry == nullptr) {
        return -1;
    }
    entry->alwaysCB = alwaysCB;
    return entry->offset;
}

// 0x808950 (lote R14A, sin validar; 41 bytes; era una envoltura): igual con rightsCB (+0x1C); sin entrada devuelve -1, si no su desplazamiento.
RwInt32 _rwPluginRegistryAddPlgnStrmRightsCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    RwPluginRegEntry* const entry = FindRegEntry(reg, pluginID);
    if (entry == nullptr) {
        return -1;
    }
    entry->rightsCB = rightsCB;
    return entry->offset;
}

// 0x808980 (lote R15A, sin validar; 299 bytes; era una envoltura; cdecl, (registro, flujo, objeto)): RwStreamFindChunk(flujo, 3 = extension, &longitud, &version) (FALSE -> NULL); version < 0x34000 o > 0x36003
// (sin signo) -> error {1, _rwerror(0x80000004)} y NULL. Mientras longitud != 0: lee la cabecera (0x7ED0F0 sin version ni build; fallo -> NULL); busca la PRIMERA entrada con pluginID == tipo; con readCB
// lo llama con (flujo, longitud del bloque, objeto, desplazamiento, tamano) y un resultado NULL aborta (NULL); sin entrada o sin readCB hace RwStreamSkip (NULL aborta). longitud -= bloque + 12 (con
// envoltura de 32 bits; el bucle termina solo al llegar a 0). Despues alwaysCB (+0x18) de cada entrada en orden con (objeto, desplazamiento, tamano): FALSE -> NULL. Devuelve el registro.
const RwPluginRegistry* _rwPluginRegistryReadDataChunks(const RwPluginRegistry* reg, RwStream* stream, void* object) {
    RwUInt32 length, version;
    if (!RwStreamFindChunk(stream, 3, &length, &version)) {
        return nullptr;
    }
    if (version < 0x34000u || version > 0x36003u) {
        StreamRaiseError(static_cast<RwInt32>(0x80000004));
        return nullptr;
    }
    if (length != 0) {
        do {
            RwUInt32 chunkType, chunkLength;
            if (!_rwStreamReadChunkHeader(stream, &chunkType, &chunkLength, nullptr, nullptr)) {
                return nullptr;
            }
            RwPluginRegEntry* entry = reg->firstRegEntry;
            while (entry != nullptr && entry->pluginID != chunkType) {
                entry = entry->nextRegEntry;
            }
            if (entry != nullptr && entry->readCB != nullptr) {
                if (entry->readCB(stream, static_cast<RwInt32>(chunkLength), object, entry->offset, entry->size) == nullptr) {
                    return nullptr;
                }
            } else if (RwStreamSkip(stream, chunkLength) == nullptr) {
                return nullptr;
            }
            length -= chunkLength + 12u;
        } while (length != 0);
    }
    for (RwPluginRegEntry* entry = reg->firstRegEntry; entry != nullptr; entry = entry->nextRegEntry) {
        if (entry->alwaysCB != nullptr && !entry->alwaysCB(object, entry->offset, entry->size)) {
            return nullptr;
        }
    }
    return reg;
}

namespace {
// Suma de getSizeCB (+0x14) de cada entrada en orden con (objeto, desplazamiento, tamano): cada resultado > 0 (con signo) suma resultado + 12 (envoltura de 32 bits). Es el bucle de 0x808B00 y el
// de la primera parte de 0x808B40.
RwInt32 PluginRegistryTotalSize(const RwPluginRegistry* reg, const void* object) {
    RwUInt32 total = 0;
    for (RwPluginRegEntry* entry = reg->firstRegEntry; entry != nullptr; entry = entry->nextRegEntry) {
        if (entry->getSizeCB != nullptr) {
            const RwInt32 size = entry->getSizeCB(object, entry->offset, entry->size);
            if (size > 0) {
                total = total + static_cast<RwUInt32>(size) + 12u;
            }
        }
    }
    return static_cast<RwInt32>(total);
}
}

// 0x808B40 (lote R15A, sin validar; 201 bytes; era una envoltura; cdecl, (registro, flujo, objeto)): total = suma de getSizeCB como en 0x808B00; escribe la cabecera de extension con
// _rwStreamWriteVersionedChunkHeader(flujo, 3, total, 0x36003, 0xFFFF) (NULL -> NULL). Despues, por cada entrada con getSizeCB (+0x14) Y writeCB (+0x10): tam = getSizeCB(objeto, desplazamiento,
// tamano) (se vuelve a llamar); si tam > 0 (con signo) escribe la cabecera (flujo, pluginID, tam, 0x36003, 0xFFFF) y llama writeCB(flujo, tam, objeto, desplazamiento, tamano); NULL en cualquiera -> NULL.
// Devuelve el registro.
const RwPluginRegistry* _rwPluginRegistryWriteDataChunks(const RwPluginRegistry* reg, RwStream* stream, const void* object) {
    const RwInt32 total = PluginRegistryTotalSize(reg, object);
    if (_rwStreamWriteVersionedChunkHeader(stream, 3, total, 0x36003u, 0xFFFFu) == nullptr) {
        return nullptr;
    }
    for (RwPluginRegEntry* entry = reg->firstRegEntry; entry != nullptr; entry = entry->nextRegEntry) {
        if (entry->getSizeCB == nullptr || entry->writeCB == nullptr) {
            continue;
        }
        const RwInt32 size = entry->getSizeCB(object, entry->offset, entry->size);
        if (size <= 0) {
            continue;
        }
        if (_rwStreamWriteVersionedChunkHeader(stream, static_cast<RwInt32>(entry->pluginID), size, 0x36003u, 0xFFFFu) == nullptr) {
            return nullptr;
        }
        if (entry->writeCB(stream, size, object, entry->offset, entry->size) == nullptr) {
            return nullptr;
        }
    }
    return reg;
}

// 0x808C10 (lote R15A, sin validar; 113 bytes; era una envoltura; cdecl, (registro, flujo)): RwStreamFindChunk(flujo, 3, &longitud, NULL) (FALSE -> NULL). Mientras longitud != 0: cabecera (0x7ED0F0
// con solo la longitud; fallo -> NULL), RwStreamSkip(longitud del bloque) (NULL -> NULL) y longitud -= bloque + 12. Devuelve el registro. No consulta ninguna entrada.
const RwPluginRegistry* _rwPluginRegistrySkipDataChunks(const RwPluginRegistry* reg, RwStream* stream) {
    RwUInt32 length;
    if (!RwStreamFindChunk(stream, 3, &length, nullptr)) {
        return nullptr;
    }
    if (length != 0) {
        do {
            RwUInt32 chunkLength;
            if (!_rwStreamReadChunkHeader(stream, nullptr, &chunkLength, nullptr, nullptr)) {
                return nullptr;
            }
            if (RwStreamSkip(stream, chunkLength) == nullptr) {
                return nullptr;
            }
            length -= chunkLength + 12u;
        } while (length != 0);
    }
    return reg;
}

// 0x808B00 (lote R15A, sin validar; 61 bytes; era una envoltura; cdecl, (registro, objeto)): suma, por cada entrada con getSizeCB (+0x14), resultado + 12 de los resultados > 0 (con signo) de
// getSizeCB(objeto, desplazamiento, tamano). Devuelve la suma.
RwInt32 _rwPluginRegistryGetSize(const RwPluginRegistry* reg, const void* object) {
    return PluginRegistryTotalSize(reg, object);
}

// 0x808AB0 (lote R15A, sin validar; 69 bytes; era una envoltura; cdecl, (registro, id, objeto, datos extra)): busca la primera entrada con pluginID == id; sin ella o sin rightsCB (+0x1C) devuelve
// NULL. Llama rightsCB(objeto, desplazamiento, tamano, extra) y devuelve el registro si el resultado es distinto de 0, si no NULL.
const RwPluginRegistry* _rwPluginRegistryInvokeRights(const RwPluginRegistry* reg, RwUInt32 id, void* obj, RwUInt32 extraData) {
    RwPluginRegEntry* entry = reg->firstRegEntry;
    while (entry != nullptr && entry->pluginID != id) {
        entry = entry->nextRegEntry;
    }
    if (entry == nullptr || entry->rightsCB == nullptr) {
        return nullptr;
    }
    return entry->rightsCB(obj, entry->offset, entry->size, extraData) != 0 ? reg : nullptr;
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

const RwMatrix* RwMatrixQueryRotate(const RwMatrix* matrix, RwV3d* unitAxis, RwReal* angle, RwV3d* center) {
    return ((const RwMatrix*(__cdecl *)(const RwMatrix*, RwV3d*, RwReal*, RwV3d*))0x7F2720)(matrix, unitAxis, angle, center);
}

// 0x7F18A0 (lote R24A, sin validar; 12 bytes; era una envoltura; cdecl, (matriz)): flags (+0xC) &= 0xFFFDFFFC (borra los bits 0 y 1 -normal y ortogonal- y el 0x20000 -identidad interna-). Devuelve la matriz.
RwMatrix* RwMatrixUpdate(RwMatrix* matrix) {
    matrix->flags &= 0xFFFDFFFCu;
    return matrix;
}

// 0x7F18B0 (lote R24A, sin validar; 111 bytes; era una envoltura; cdecl, (salida, entrada1, entrada2)): m = modulo de matrices (motor + [0xC979BC]); mascara = m.flags (+4) & 0x20000. Si los flags de la entrada1
// (+0xC) tienen algun bit de la mascara (identidad interna) copia ENTERA la entrada2 (16 dwords, incluidos flags y rellenos) a la salida y la devuelve; si no, si los de la entrada2 lo tienen, copia la entrada1 (16 dwords).
// Si ninguna es identidad llama al callback del modulo (+8; cdecl, (salida, entrada1, entrada2); su resultado se ignora), escribe flags = flags2 & flags1 (leidos ANTES de la llamada) y devuelve la salida.
// Las copias son rep movsd de 64 bytes: la salida puede solaparse con la entrada elegida (no se protege).
RwMatrix* RwMatrixMultiply(RwMatrix* matrixOut, const RwMatrix* MatrixIn1, const RwMatrix* matrixIn2) {
    auto& module = R12AMatrixModule();
    const RwUInt32 flags1 = MatrixIn1->flags;
    const RwUInt32 flags2 = matrixIn2->flags;
    const RwUInt32 mask = module.flags & 0x20000;
    if ((flags1 & mask) != 0) {
        memcpy(matrixOut, matrixIn2, sizeof(RwMatrix));
        return matrixOut;
    }
    if ((flags2 & mask) != 0) {
        memcpy(matrixOut, MatrixIn1, sizeof(RwMatrix));
        return matrixOut;
    }
    reinterpret_cast<void(__cdecl*)(RwMatrix*, const RwMatrix*, const RwMatrix*)>(module.multFn)(matrixOut, MatrixIn1, matrixIn2);
    matrixOut->flags = flags2 & flags1;
    return matrixOut;
}

// 0x7F2A20 (lote R24A, sin validar; 35 bytes; era una envoltura; cdecl, (matriz)): libera la matriz en la lista libre del modulo de matrices (RwFreeListFree: [motor + 0x148](lista = [motor + [0xC979BC]], matriz)). Devuelve TRUE.
RwBool RwMatrixDestroy(RwMatrix* mpMat) {
    RwFreeListFree(static_cast<RwFreeList*>(R12AMatrixModule().freeList), mpMat);
    return TRUE;
}

// 0x7F12C0 (lote R24A, sin validar; 35 bytes; cdecl, (funcion); nombre real de RenderWare: _rwMatrixSetMultFn): guarda en el callback de multiplicacion del modulo de matrices (+8) la funcion, o 0x7F12F0 (la
// de por defecto) si es nula. Devuelve TRUE.
RwBool _rwMatrixSetMultFn(rwMatrixMultFn multMat) {
    R12AMatrixModule().multFn = multMat ? reinterpret_cast<void*>(multMat) : reinterpret_cast<void*>(0x7F12F0);
    return TRUE;
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

// 0x7ED9B0 (lote R23A, sin validar; 271 bytes; era una envoltura; cdecl, (salida, entrada)): s = (x*x + y*y) + z*z guardado como float. Con los bits de s distintos de cero: longitud aproximada con la tabla de la raiz
// del modulo de vectores (+0, igual que RwV3dLength) e inverso con la tabla +4 (igual que _rwInvSqrt); con bits cero la "longitud" es 0 y el "inverso" es s (0.0f). salida.x = inverso * entrada.x, luego y, luego z
// (cada una guardada como float; la entrada se lee despues de escribir la componente anterior, asi que salida == entrada funciona). Si la longitud es <= 0.0f (comparacion ordenada: NaN no entra) pone el error
// RwError { 1, _rwerror(0x19) } con RwErrorSet. Devuelve la longitud aproximada (ST0).
RwReal RwV3dNormalize(RwV3d* out, const RwV3d* in) {
    const float sum = V3dSumSquaresCpp(&in->x);
    const RwUInt32 sumBits = std::bit_cast<RwUInt32>(sum);
    RwUInt32 lengthBits = sumBits;
    float inverse = sum;
    if (sumBits != 0) {
        const RwUInt32 shifted = sumBits + 0x800;
        lengthBits = R12AVectorModule().sqrtTable[(shifted >> 12) & 0xFFF] + ((shifted >> 1) & 0x3FC00000u);
        inverse = std::bit_cast<RwReal>(R12AVectorModule().invSqrtTable[(shifted >> 12) & 0xFFF] + (((~shifted) >> 1) & 0x3FC00000u));
    }
    const float x = float(double(inverse) * double(in->x));
    out->x = x;
    const float y = float(double(in->y) * double(inverse));
    out->y = y;
    const float z = float(double(in->z) * double(inverse));
    out->z = z;
    const RwReal length = std::bit_cast<RwReal>(lengthBits);
    if (length <= 0.0f) {
        RwError error;
        error.pluginID  = 1;
        error.errorCode = _rwerror(0x19);
        RwErrorSet(&error);
    }
    return length;
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

// 0x7EDD90 (lote R23A, sin validar; 43 bytes; era una envoltura): llama al callback del modulo de vectores (motor + [0xC97934] + 0xC, cdecl) con (salida, entrada, numero, matriz), ignora su resultado y devuelve la salida.
RwV3d* RwV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix) {
    R12AVectorModule().multPoints(pointsOut, pointsIn, numPoints, matrix);
    return pointsOut;
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

// 0x809160 (lote R15A, sin validar; 205 bytes; era una envoltura; cdecl, (tamano de entrada, pista)): reserva la cabecera (0x10 bytes, rwmalloc con pista 0x30000); fallo: error {1, _rwerror(0x80000013, 0x10)}
// y NULL. entrySize = tamano, numElementsFilled = 0, numElementsAlloced = 0x14 y listElements = rwmalloc(tamano * 20, pista | 0x1000000); si falla: error con (0x80000013, 0x14 * tamano), rwfree de la cabecera y NULL.
RwSList* _rwSListCreate(RwInt32 size, RwUInt32 hint) {
    RwSList* const sList = static_cast<RwSList*>(RwEngineInstance->memoryFuncs.rwmalloc(0x10, 0x30000));
    if (sList == nullptr) {
        StreamRaiseError(static_cast<RwInt32>(0x80000013), 0x10);
        return nullptr;
    }
    sList->entrySize = size;
    sList->numElementsFilled = 0;
    sList->numElementsAlloced = 0x14;
    sList->listElements = static_cast<RwUInt8*>(RwEngineInstance->memoryFuncs.rwmalloc(static_cast<uint32>(size) * 20u, hint | 0x1000000u));
    if (sList->listElements == nullptr) {
        StreamRaiseError(static_cast<RwInt32>(0x80000013), static_cast<RwInt32>(static_cast<uint32>(sList->numElementsAlloced) * static_cast<uint32>(sList->entrySize)));
        RwEngineInstance->memoryFuncs.rwfree(sList);
        return nullptr;
    }
    return sList;
}

// 0x809440 (lote R15A, sin validar; 69 bytes; era una envoltura): con listElements != NULL y numElementsAlloced != 0 libera la matriz (rwfree) y deja listElements = NULL y numElementsAlloced = 0;
// despues libera la cabecera (rwfree). Devuelve 1.
RwBool _rwSListDestroy(RwSList* sList) {
    if (sList->listElements != nullptr && sList->numElementsAlloced != 0) {
        RwEngineInstance->memoryFuncs.rwfree(sList->listElements);
        sList->listElements = nullptr;
        sList->numElementsAlloced = 0;
    }
    RwEngineInstance->memoryFuncs.rwfree(sList);
    return TRUE;
}

// 0x809400: resta `amount` a numElementsFilled (+4), con envoltura de 32 bits.
void _rwSListDestroyEndEntries(RwSList* sList, RwInt32 amount) {
    sList->numElementsFilled = static_cast<RwInt32>(static_cast<uint32>(sList->numElementsFilled) - static_cast<uint32>(amount));
}

// 0x809510: listElements + entrySize * entry (imul de 32 bits con envoltura).
void* _rwSListGetEntry(RwSList* sList, RwInt32 entry) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(sList->listElements) + static_cast<uint32>(sList->entrySize) * static_cast<uint32>(entry));
}

// 0x809240 (lote R15A, sin validar; 166 bytes; era una envoltura; cdecl, (lista, pista)): con numElementsFilled >= numElementsAlloced (CON signo) amplia la matriz a alloced + alloced / 4 (division con signo
// hacia cero) con rwrealloc(listElements, nuevo * entrySize, pista); fallo: error {1, _rwerror(0x80000013, nuevo * entrySize)} y NULL (sin tocar la lista). Devuelve entrySize * filled + listElements y
// incrementa numElementsFilled.
void* _rwSListGetNewEntry(RwSList* sList, RwUInt32 hint) {
    const RwInt32 alloced = sList->numElementsAlloced;
    if (sList->numElementsFilled >= alloced) {
        const RwInt32 grown = alloced + alloced / 4;
        const RwUInt32 bytes = static_cast<RwUInt32>(grown) * static_cast<RwUInt32>(sList->entrySize);
        void* const memory = RwEngineInstance->memoryFuncs.rwrealloc(sList->listElements, bytes, hint);
        if (memory == nullptr) {
            StreamRaiseError(static_cast<RwInt32>(0x80000013), static_cast<RwInt32>(bytes));
            return nullptr;
        }
        sList->listElements = static_cast<RwUInt8*>(memory);
        sList->numElementsAlloced = grown;
    }
    const RwInt32 filled = sList->numElementsFilled;
    void* const entry = sList->listElements + static_cast<RwUInt32>(sList->entrySize) * static_cast<RwUInt32>(filled);
    sList->numElementsFilled = filled + 1;
    return entry;
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

// 0x808880 (lote R14A, sin validar; 74 bytes; era una envoltura): el error pendiente vive en el motor + [0xC9A6F0] (pluginID, errorCode): lo copia a `code` (pluginID primero) y lo deja en
// (0, 0x80000000), es decir, sin error. Devuelve `code`.
RwError* RwErrorGet(RwError* code) {
    RwError* const stored = reinterpret_cast<RwError*>(reinterpret_cast<RwUInt8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9A6F0));
    code->pluginID = stored->pluginID;
    code->errorCode = stored->errorCode;
    stored->pluginID = 0;
    stored->errorCode = static_cast<RwInt32>(0x80000000);
    return code;
}

// 0x8082E0 (lote R14A, sin validar; 61 bytes): abre el registro de plugins (nombre inferido). Crea la lista libre de entradas de plugin (RwPluginRegEntry, 0x3C bytes) con
// RwFreeListCreateAndPreallocateSpace(0x3C, [0x8E26AC] (entradas por bloque), 4, [0x8E26B0] (bloques), espacio fijo 0xC9A6C0, 0x40000) y la guarda en 0xC9A6E4. NULL -> devuelve 0 (0xC9A6EC sin tocar);
// si no pone la cantidad de registros (0xC9A6EC) a 0 y devuelve 1.
RwBool _rwPluginRegistryOpen() {
    RwFreeList* const entryList = RwFreeListCreateAndPreallocateSpace(0x3C, StaticRef<RwInt32>(0x8E26AC), 4, StaticRef<RwInt32>(0x8E26B0), reinterpret_cast<RwFreeList*>(0xC9A6C0), 0x40000);
    StaticRef<RwFreeList*>(0xC9A6E4) = entryList;
    if (entryList == nullptr) {
        return FALSE;
    }
    StaticRef<RwUInt32>(0xC9A6EC) = 0;
    return TRUE;
}

// 0x808320 (lote R14A, sin validar; 197 bytes): cierra el registro de plugins (nombre inferido). Con la lista libre de entradas (0xC9A6E4) no nula: RwFreeListForAllUsed(lista, 0x8083F0, lista). Si memoryAlloc
// del motor no es el por defecto (0x801C30): para cada registro de la matriz 0xC9A6E8 (cantidad 0xC9A6EC, releida en cada vuelta) libera cada entrada con memoryFree(NULL, entrada) (el padre y el
// siguiente se leen antes) y, si el padre del ultimo y su primera entrada son no nulos, deja primera/ultima a NULL y sizeOfStruct = origSizeOfStruct; despues libera la matriz con rwfree y pone 0xC9A6E8 a 0
// (la cantidad no se toca). Siempre despues RwFreeListDestroy(lista) y 0xC9A6E4 = 0. Devuelve 1 (tambien sin lista).
RwBool _rwPluginRegistryClose() {
    RwFreeList* const entryList = StaticRef<RwFreeList*>(0xC9A6E4);
    if (entryList == nullptr) {
        return TRUE;
    }
    RwFreeListForAllUsed(entryList, reinterpret_cast<RwFreeListCallBack>(0x8083F0), entryList);
    RwGlobals* const engine = RwEngineInstance;
    if (reinterpret_cast<uintptr_t>(engine->memoryAlloc) != kDefaultFreeListAlloc) {
        RwPluginRegistry**& registries = StaticRef<RwPluginRegistry**>(0xC9A6E8);
        if (StaticRef<RwUInt32>(0xC9A6EC) != 0) {
            for (RwUInt32 index = 0; index < StaticRef<RwUInt32>(0xC9A6EC); index++) {
                RwPluginRegEntry* entry = registries[index]->firstRegEntry;
                if (entry == nullptr) {
                    continue;
                }
                RwPluginRegistry* parent;
                RwPluginRegEntry* next;
                do {
                    parent = entry->parentRegistry;
                    next = entry->nextRegEntry;
                    engine->memoryFree(nullptr, entry);
                    entry = next;
                } while (next != nullptr);
                if (parent != nullptr && parent->firstRegEntry != nullptr) {
                    const RwInt32 originalSize = parent->origSizeOfStruct;
                    parent->firstRegEntry = nullptr;
                    parent->sizeOfStruct = originalSize;
                    parent->lastRegEntry = nullptr;
                }
            }
        }
        if (registries != nullptr) {
            engine->memoryFuncs.rwfree(registries);
            registries = nullptr;
        }
    }
    RwFreeListDestroy(entryList);
    StaticRef<RwFreeList*>(0xC9A6E4) = nullptr;
    return TRUE;
}

// 0x804140 (lote R14A, sin validar; 178 bytes): abre el sistema de ficheros del motor (nombre inferido): rellena las 11 funciones de fichero de motor + 0xC4 (fileFuncs[0..10]) con 0x804200, 0x8232D8,
// 0x82318B, 0x823521, 0x823674, 0x823798, 0x8262B8, 0x8262A2, 0x82374F, 0x823E86 y 0x826261 (funciones de la CRT del EXE) y devuelve 1.
RwBool _rwFileSystemOpen() {
    RwGlobals* const engine = RwEngineInstance;
    engine->fileFuncs[0]  = reinterpret_cast<void*>(0x804200);
    engine->fileFuncs[1]  = reinterpret_cast<void*>(0x8232D8);
    engine->fileFuncs[2]  = reinterpret_cast<void*>(0x82318B);
    engine->fileFuncs[3]  = reinterpret_cast<void*>(0x823521);
    engine->fileFuncs[4]  = reinterpret_cast<void*>(0x823674);
    engine->fileFuncs[5]  = reinterpret_cast<void*>(0x823798);
    engine->fileFuncs[6]  = reinterpret_cast<void*>(0x8262B8);
    engine->fileFuncs[7]  = reinterpret_cast<void*>(0x8262A2);
    engine->fileFuncs[8]  = reinterpret_cast<void*>(0x82374F);
    engine->fileFuncs[9]  = reinterpret_cast<void*>(0x823E86);
    engine->fileFuncs[10] = reinterpret_cast<void*>(0x826261);
    return TRUE;
}

// 0x804240 (lote R14A, sin validar; 1 byte, `ret`): cierre del sistema de ficheros del motor (nombre inferido); no hace nada. Sin argumentos. El hook de 5 bytes cabe en el relleno hasta 0x804250.
void _rwFileSystemClose() {
}

// 0x7F9C20 (lote R14A, sin validar; 6 bytes): `mov eax, 0x8E2498; ret`: devuelve la direccion del RwDevice estatico del controlador D3D9 (nombre RenderWare del controlador: _rwDeviceGetHandle).
RwDevice* _rwDeviceGetHandle() {
    return reinterpret_cast<RwDevice*>(0x8E2498);
}

// 0x807C70 (lote R14A, sin validar; 6 bytes): `mov eax, 1; ret`. RwEngineInit la llama entre el registro de los plugins 0x412 y 0x40A y hace OR de su resultado con el de los demas (modulo de motor sin
// cuerpo, p. ej. metricas; nombre inferido).
RwBool _rwEngineModuleStubOpen() {
    return TRUE;
}

// 0x807C80 (lote R14A, sin validar; 1 byte, `ret`): cdecl con un argumento de pila que ignora (RwCameraBeginUpdate la llama con la camara tras _rwFrameSyncDirty y el dispositivo; gancho vacio; nombre inferido).
// El hook de 5 bytes cabe en el relleno hasta 0x807C90.
void _rwCameraBeginUpdateStub(void* camera) {
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

void _rwResArenaHeapFree(void* memory); // 0x810040 (lote R15A); definida mas abajo con el monton del arena

namespace {
// Modulo de recursos: rwResources en motor + [0xC9BC58]: +0 maxSize, +4 currentSize, +8 reusageSize, +0xC memHeap, +0x10 entriesA, +0x18 entriesB, +0x20 freeEntries, +0x24 usedEntries.
static_assert(offsetof(rwResources, currentSize) == 4 && offsetof(rwResources, reusageSize) == 8 && offsetof(rwResources, entriesA) == 0x10 && offsetof(rwResources, entriesB) == 0x18);
static_assert(offsetof(rwResources, freeEntries) == 0x20 && offsetof(rwResources, usedEntries) == 0x24);
rwResources* ResourcesModule() {
    return reinterpret_cast<rwResources*>(reinterpret_cast<RwUInt8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9BC58));
}

// Destruye una entrada de recursos: destroyNotify(entrada), *ownerRef = NULL; con enlace (+0) la saca de su lista, resta su tamano de currentSize y la devuelve al monton (0x810040); sin enlace rwfree.
void DestroyResEntry(RwResEntry* entry) {
    if (entry->destroyNotify != nullptr) {
        entry->destroyNotify(entry);
    }
    if (entry->ownerRef != nullptr) {
        *entry->ownerRef = nullptr;
    }
    if (entry->link.next != nullptr) {
        LinkRemove(&entry->link);
        ResourcesModule()->currentSize -= entry->size;
        _rwResArenaHeapFree(entry);
    } else {
        RwEngineInstance->memoryFuncs.rwfree(entry);
    }
}
}

// 0x8081F0 (lote R14A, sin validar; 240 bytes; era una envoltura): enlaza la cola de entriesA con la primera de entriesB (entriesA.prev->next = entriesB.next) y recorre desde entriesA.next hasta el centinela
// de entriesB (todas las entradas de las dos listas): cada una se destruye como en RwResourcesFreeResEntry (el siguiente se lee antes). Despues deja las dos listas vacias (anillos propios), reusageSize = 0
// y devuelve 1. Las entradas con enlace restan su tamano de currentSize.
RwBool RwResourcesEmptyArena() {
    rwResources* res = ResourcesModule();
    res->entriesA.link.prev->next = res->entriesB.link.next;
    RwLLLink* entry = res->entriesA.link.next;
    RwLLLink* const sentinelB = &res->entriesB.link;
    if (entry != sentinelB) {
        do {
            RwResEntry* const current = reinterpret_cast<RwResEntry*>(entry);
            entry = entry->next;
            DestroyResEntry(current);
        } while (entry != sentinelB);
    }
    res = ResourcesModule();
    res->entriesA.link.next = &res->entriesA.link;
    res->entriesA.link.prev = &res->entriesA.link;
    res->entriesB.link.next = &res->entriesB.link;
    res->entriesB.link.prev = &res->entriesB.link;
    res->reusageSize = 0;
    return TRUE;
}

RwResEntry* RwResourcesAllocateResEntry(void* owner, RwResEntry* ownerRef, RwInt32 size, RwResEntryDestroyNotify destroyNotify) {
    return ((RwResEntry*(__cdecl *)(void*, RwResEntry*, RwInt32, RwResEntryDestroyNotify))0x807ED0)(owner, ownerRef, size, destroyNotify);
}

// 0x807DE0 (lote R14A, sin validar; 106 bytes; era una envoltura): destroyNotify(entrada) si existe, *ownerRef = NULL si existe; con enlace (+0 != 0) la saca de su lista, resta su tamano (+8) de currentSize
// del arena y la devuelve al monton con 0x810040 (sin reconstruir); sin enlace la libera con rwfree (+0x138). Devuelve siempre 1.
RwBool RwResourcesFreeResEntry(RwResEntry* entry) {
    DestroyResEntry(entry);
    return TRUE;
}

// 0x807E50 (lote R14A, sin validar; 118 bytes; era una envoltura): si la lista de libres (+0x20) no esta vacia la anade (como anillo) al final de la de usadas (+0x24) y la deja vacia; despues INTERCAMBIA los
// punteros: usedEntries = la antigua lista de libres, freeEntries = la antigua de usadas, y reusageSize = 0.
void _rwResourcesPurge() {
    rwResources* const res = ResourcesModule();
    RwLLLink* const freeList = &res->freeEntries->link;
    RwLLLink* const usedList = &res->usedEntries->link;
    RwLLLink* const firstFree = freeList->next;
    if (firstFree != freeList) {
        if (usedList->next == usedList) {
            usedList->next = firstFree;
            firstFree->prev = usedList;
            usedList->prev = freeList->prev;
            freeList->prev->next = usedList;
        } else {
            RwLLLink* const lastUsed = usedList->prev;
            lastUsed->next = firstFree;
            firstFree->prev = lastUsed;
            freeList->prev->next = usedList;
            usedList->prev = freeList->prev;
        }
        freeList->next = freeList;
        freeList->prev = freeList;
    }
    res->usedEntries = reinterpret_cast<RwLinkList*>(freeList);
    res->freeEntries = reinterpret_cast<RwLinkList*>(usedList);
    res->reusageSize = 0;
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

// 0x810040 (lote R15A, sin validar; 125 bytes; cdecl, (memoria devuelta por _rwResArenaHeapAlloc); sin valor de retorno que lea nadie; nombre inferido: liberar en el monton del arena): el bloque es
// memoria - 0x20; lo marca libre (used = 0). Si el punto de busqueda del monton (+4) es NULL o el bloque es anterior (comparacion SIN signo), el bloque pasa a ser el punto de busqueda. Si el bloque
// anterior existe y esta libre (bit 0 de used a 0): lo funde con el actual (anterior.next = siguiente, siguiente.prev = anterior, anterior.size += size + 0x20) y sigue con el anterior como bloque. Si
// el siguiente (el de antes de fundir) existe y esta libre: lo funde con el bloque vigente (bloque.next = siguiente.next, su prev apunta al bloque, bloque.size += siguiente.size + 0x20).
void _rwResArenaHeapFree(void* memory) {
    ResArenaBlock* block = reinterpret_cast<ResArenaBlock*>(static_cast<RwUInt8*>(memory) - 0x20);
    ResArenaHeap* const heap = static_cast<ResArenaHeap*>(block->heap);
    ResArenaBlock* const prev = block->prev;
    ResArenaBlock* const next = block->next;
    block->used = 0;
    if (heap->searchStart == nullptr || block < heap->searchStart) {
        heap->searchStart = block;
    }
    if (prev != nullptr && (~prev->used & 1u) != 0) {
        prev->next = next;
        if (next != nullptr) {
            next->prev = prev;
        }
        prev->size = prev->size + (block->size + 0x20);
        block = prev;
    }
    if (next != nullptr && (~next->used & 1u) != 0) {
        block->next = next->next;
        if (next->next != nullptr) {
            next->next->prev = block;
        }
        block->size = block->size + (next->size + 0x20);
    }
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

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R13A (sin validar): matrices (crear, invertir, escalar, trasladar, rotar, combinar), el callback de vector por defecto y el motor (RwEngine*, 0x7ED880, 0x7F1D00..0x7F3170).
// Variables del EXE: RwEngineInstance (0xC97B24), globales estaticos del motor (0xC979C8, 0x158 bytes, RwGlobals) y contador de aperturas (0xC97B20), registro de plugins del motor (0x8E2298).
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
constexpr uintptr_t kEngineStaticGlobals = 0xC979C8;      // RwGlobals estatico: RwEngineInstance apunta aqui antes de RwEngineOpen y despues de RwEngineClose
constexpr size_t    kEngineGlobalsSize   = 0x158;         // 0x56 dwords copiados por RwEngineOpen / RwEngineClose
constexpr uintptr_t kEngineOpenCountAddr = 0xC97B20;      // numero de aperturas del motor (+1 en RwEngineOpen, -1 en RwEngineClose)
constexpr uintptr_t kEngineRegistryAddr  = 0x8E2298;      // RwPluginRegistry de los plugins de motor (+0: tamano acumulado)
constexpr uintptr_t kInvalidCombinationString = 0x8E21FC; // "Invalid combination type"
constexpr uintptr_t kSubSystemNameString = 0x8E23B0;      // "Only rendering sub system"
constexpr uintptr_t kDegToRadAddr        = 0x8595EC;      // 0.017453292f
static_assert(offsetof(RwGlobals, dOpenDevice) == 0x10 && sizeof(RwDevice) == 0x38 && offsetof(RwGlobals, stdFunc) == 0x48);
static_assert(offsetof(RwGlobals, stringFuncs) == 0xF0 && offsetof(RwGlobals, memoryFuncs) == 0x134);
static_assert(offsetof(RwGlobals, memoryAlloc) == 0x144 && offsetof(RwGlobals, memoryFree) == 0x148 && offsetof(RwGlobals, metrics) == 0x14C);
static_assert(offsetof(RwGlobals, engineStatus) == 0x150 && offsetof(RwGlobals, resArenaInitSize) == 0x154 && sizeof(RwGlobals) == kEngineGlobalsSize);
static_assert(sizeof(RwMatrix) == 0x40 && offsetof(RwMatrix, flags) == 0xC && offsetof(RwMatrix, pos) == 0x30);

RwInt32& EngineOpenCount() { return StaticRef<RwInt32>(kEngineOpenCountAddr); }
RwGlobals& EngineStaticGlobals() { return *reinterpret_cast<RwGlobals*>(kEngineStaticGlobals); }
using MatrixMultFn = RwMatrix*(__cdecl*)(RwMatrix*, const RwMatrix*, const RwMatrix*);
using ExeCall0Fn   = RwInt32(__cdecl*)();
RwInt32 ExeCall0(uintptr_t address) { return reinterpret_cast<ExeCall0Fn>(address)(); }

// RwError { pluginID = 1, errorCode = _rwerror(codigo[, extra]) } + RwErrorSet; ver StreamRaiseError (R11A).
void CombineTypeError() {
    StreamRaiseError(static_cast<RwInt32>(0x80000003), static_cast<RwInt32>(kInvalidCombinationString));
}
// Copia de 0x10 dwords (rep movs dword).
void CopyMatrix(RwMatrix* dst, const RwMatrix* src) { memcpy(dst, src, sizeof(RwMatrix)); }
}

// 0x7ED880 (lote R13A, sin validar; 133 bytes; cdecl, (salida, entrada, matriz)): callback por defecto de RwV3dTransformVector (+0x10 del modulo de vectores, instalado por _rwVectorSetMultFn): como el de punto sin
// traslacion; devuelve la salida (EAX). Ver V3dTransformVectorDefaultCpp (x87).
RwV3d* _rwV3dTransformVectorDefault(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix) {
    V3dTransformVectorDefaultCpp(&vectorOut->x, &vectorIn->x, reinterpret_cast<const float*>(matrix));
    return vectorOut;
}

// 0x7F2A50 (lote R13A, sin validar; 83 bytes): reserva con memoryAlloc del motor (+0x144, lista libre del modulo de matrices, hint 0x3000D); si no es nulo pone la identidad (right.x = up.y = at.z = 1, el resto de ejes y pos a 0,
// flags = 0x20003; los rellenos no se tocan) y devuelve la matriz; si es nulo devuelve NULL.
RwMatrix* RwMatrixCreate() {
    RwMatrix* const matrix = static_cast<RwMatrix*>(RwEngineInstance->memoryAlloc(static_cast<RwFreeList*>(R12AMatrixModule().freeList), 0x3000D));
    if (matrix == nullptr) {
        return nullptr;
    }
    matrix->up.x = 0.0f;
    matrix->at.z = 1.0f;
    matrix->up.y = 1.0f;
    matrix->right.x = 1.0f;
    matrix->at.x = 0.0f;
    matrix->right.y = 0.0f;
    matrix->at.y = 0.0f;
    matrix->up.z = 0.0f;
    matrix->right.z = 0.0f;
    matrix->pos.z = 0.0f;
    matrix->pos.y = 0.0f;
    matrix->pos.x = 0.0f;
    matrix->flags = 0x20003;
    return matrix;
}

// 0x7F2160 (lote R13A, sin validar; 338 bytes; cdecl, (salida, entrada); sin retorno): inversa general por cofactores / determinante (cuerpo de RwMatrixInvert cuando la matriz no es ortonormal). Pone flags = 0. Ver MatrixInvertGeneralCpp.
void _rwMatrixInvertGeneral(RwMatrix* matrixOut, const RwMatrix* matrixIn) {
    MatrixInvertGeneralCpp(reinterpret_cast<float*>(matrixOut), reinterpret_cast<const float*>(matrixIn));
    matrixOut->flags = 0;
}

// 0x7F2070 (lote R13A, sin validar; 225 bytes; cdecl, (salida, entrada)): con (flags del modulo [+4] & flags de la entrada) & 0x20000 (identidad optimizada) copia la entrada a la salida (0x10 dwords). Si no, con (flags & 3) == 3 (ortonormal)
// transpone la parte de rotacion con copias de dword (la lectura de entrada sigue el orden del EXE: 0<-0, 4<-0x10, 8<-0x20, 0x10<-4, 0x14<-0x14, 0x18<-0x24, 0x20<-8, 0x24<-0x18, 0x28<-0x28), calcula
// pos = -(pos . ejes) (MatrixInvertOrthoPosCpp) y deja flags = 3 (sin conservar los demas bits); en el resto llama a _rwMatrixInvertGeneral (0x7F2160). Devuelve la salida en todos los casos.
RwMatrix* RwMatrixInvert(RwMatrix* matrixOut, const RwMatrix* matrixIn) {
    if (((R12AMatrixModule().flags & matrixIn->flags) & 0x20000) != 0) {
        CopyMatrix(matrixOut, matrixIn);
        return matrixOut;
    }
    if ((matrixIn->flags & 3) == 3) {
        const RwUInt32* const in = reinterpret_cast<const RwUInt32*>(matrixIn);
        RwUInt32* const out = reinterpret_cast<RwUInt32*>(matrixOut);
        out[0] = in[0];
        out[1] = in[4];
        out[2] = in[8];
        out[4] = in[1];
        out[5] = in[5];
        out[6] = in[9];
        out[8] = in[2];
        out[9] = in[6];
        out[10] = in[10];
        MatrixInvertOrthoPosCpp(reinterpret_cast<float*>(matrixOut), reinterpret_cast<const float*>(matrixIn));
        matrixOut->flags = 3;
        return matrixOut;
    }
    _rwMatrixInvertGeneral(matrixOut, matrixIn);
    return matrixOut;
}

// 0x7F22C0 (lote R13A, sin validar; 398 bytes): combinacion 0 (reemplazo): identidad (right.x = up.y = at.z = 1, el resto a 0, flags |= 0x20003), luego la diagonal = (sx, sy, sz); flags &= 0xFFFDFFFC. 1 (preconcatenar): right *= sx, up *= sy, at *= sz
// (float(m * s) con la x87); la posicion no cambia. 2 (postconcatenar): cada fila de las cuatro (right, up, at, pos) = (x*sx, y*sy, z*sz). En 1 y 2: flags &= 0xFFFDFFFC. Otro valor: error 0x80000003
// ("Invalid combination type", 0x8E21FC) y el EXE lee [0 + 0xC] (acceso a NULL: cuelga el juego); aqui devuelve NULL sin leer.
RwMatrix* RwMatrixScale(RwMatrix* matrix, const RwV3d* scale, RwOpCombineType combineOp) {
    switch (combineOp) {
    case rwCOMBINEREPLACE:
        matrix->at.z = 1.0f;
        matrix->up.y = 1.0f;
        matrix->right.x = 1.0f;
        matrix->flags = matrix->flags | 0x20003;
        matrix->up.x = 0.0f;
        matrix->at.x = 0.0f;
        matrix->right.y = 0.0f;
        matrix->at.y = 0.0f;
        matrix->up.z = 0.0f;
        matrix->right.z = 0.0f;
        matrix->pos.z = 0.0f;
        matrix->pos.y = 0.0f;
        matrix->pos.x = 0.0f;
        matrix->right.x = scale->x;
        matrix->up.y = scale->y;
        matrix->at.z = scale->z;
        matrix->flags = matrix->flags & 0xFFFDFFFC;
        return matrix;
    case rwCOMBINEPRECONCAT:
        matrix->right.x = matrix->right.x * scale->x;
        matrix->right.y = matrix->right.y * scale->x;
        matrix->right.z = matrix->right.z * scale->x;
        matrix->up.x = scale->y * matrix->up.x;
        matrix->up.y = scale->y * matrix->up.y;
        matrix->up.z = scale->y * matrix->up.z;
        matrix->at.x = scale->z * matrix->at.x;
        matrix->at.y = scale->z * matrix->at.y;
        matrix->at.z = scale->z * matrix->at.z;
        matrix->flags &= 0xFFFDFFFC;
        return matrix;
    case rwCOMBINEPOSTCONCAT:
        matrix->right.x = matrix->right.x * scale->x;
        matrix->right.y = matrix->right.y * scale->y;
        matrix->right.z = matrix->right.z * scale->z;
        matrix->up.x = matrix->up.x * scale->x;
        matrix->up.y = scale->y * matrix->up.y;
        matrix->up.z = matrix->up.z * scale->z;
        matrix->at.x = matrix->at.x * scale->x;
        matrix->at.y = scale->y * matrix->at.y;
        matrix->at.z = matrix->at.z * scale->z;
        matrix->pos.x = matrix->pos.x * scale->x;
        matrix->pos.y = scale->y * matrix->pos.y;
        matrix->pos.z = matrix->pos.z * scale->z;
        matrix->flags &= 0xFFFDFFFC;
        return matrix;
    default:
        CombineTypeError();
        return nullptr;
    }
}

// 0x7F2450 (lote R13A, sin validar; 323 bytes): 0 (reemplazo): identidad (flags |= 0x20003), luego pos = traslacion; flags &= 0xFFFDFFFF. 1 (preconcatenar): pos += right*tx + up*ty + at*tz con ((at*tz + up*ty) + right*tx) + pos sin
// redondear intermedios (MatrixTranslatePreCpp); flags &= 0xFFFDFFFF. 2 (postconcatenar): pos = float(pos + t) por componente (la suma con la x87 equivale a la de float); flags &= 0xFFFDFFFF. Otro valor: error 0x80000003 y el EXE lee
// [0 + 0xC] (cuelga); aqui devuelve NULL.
RwMatrix* RwMatrixTranslate(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp) {
    switch (combineOp) {
    case rwCOMBINEREPLACE:
        matrix->at.z = 1.0f;
        matrix->up.y = 1.0f;
        matrix->right.x = 1.0f;
        matrix->flags = matrix->flags | 0x20003;
        matrix->up.x = 0.0f;
        matrix->at.x = 0.0f;
        matrix->right.y = 0.0f;
        matrix->at.y = 0.0f;
        matrix->up.z = 0.0f;
        matrix->right.z = 0.0f;
        matrix->pos.z = 0.0f;
        matrix->pos.y = 0.0f;
        matrix->pos.x = 0.0f;
        matrix->pos.x = translation->x;
        matrix->pos.y = translation->y;
        matrix->pos.z = translation->z;
        matrix->flags = matrix->flags & 0xFFFDFFFF;
        return matrix;
    case rwCOMBINEPRECONCAT:
        MatrixTranslatePreCpp(reinterpret_cast<float*>(matrix), &translation->x);
        matrix->flags &= 0xFFFDFFFF;
        return matrix;
    case rwCOMBINEPOSTCONCAT:
        matrix->pos.x = matrix->pos.x + translation->x;
        matrix->pos.y = translation->y + matrix->pos.y;
        matrix->pos.z = translation->z + matrix->pos.z;
        matrix->flags &= 0xFFFDFFFF;
        return matrix;
    default:
        CombineTypeError();
        return nullptr;
    }
}

// 0x7F25A0 (lote R13A, sin validar; 378 bytes): 0 (reemplazo): copia `transform` en `matrix` (0x10 dwords). Con mascara = flags del modulo de matrices [+4] & 0x20000: 1 (preconcatenar): si transform.flags & mascara no cambia nada;
// si no, si matrix.flags & mascara copia `transform` en `matrix`; si no, temp = multFn(transform, matrix) (multFn = modulo + 8), temp.flags = transform.flags & matrix.flags y copia temp en `matrix`. 2 (postconcatenar): si matrix.flags & mascara
// copia `transform`; si no, si transform.flags & mascara no cambia nada; si no, temp = multFn(matrix, transform), temp.flags = flags de ambos y copia temp. Las copias por temporal son las del EXE (rep movs). Otro valor: error
// 0x80000003 y devuelve NULL (esta vez el EXE si devuelve 0 sin leer nada). Devuelve `matrix`.
RwMatrix* RwMatrixTransform(RwMatrix* matrix, const RwMatrix* transform, RwOpCombineType combineOp) {
    RwMatrix temp;
    switch (combineOp) {
    case rwCOMBINEREPLACE:
        CopyMatrix(matrix, transform);
        return matrix;
    case rwCOMBINEPRECONCAT: {
        const RwUInt32 mask = R12AMatrixModule().flags & 0x20000;
        const RwUInt32 transformFlags = transform->flags;
        const RwUInt32 matrixFlags = matrix->flags;
        if ((transformFlags & mask) != 0) {
            CopyMatrix(&temp, matrix);
            CopyMatrix(matrix, &temp);
            return matrix;
        }
        if ((matrixFlags & mask) != 0) {
            CopyMatrix(&temp, transform);
            CopyMatrix(matrix, &temp);
            return matrix;
        }
        reinterpret_cast<MatrixMultFn>(R12AMatrixModule().multFn)(&temp, transform, matrix);
        temp.flags = matrixFlags & transformFlags;
        CopyMatrix(matrix, &temp);
        return matrix;
    }
    case rwCOMBINEPOSTCONCAT: {
        const RwUInt32 mask = R12AMatrixModule().flags & 0x20000;
        const RwUInt32 matrixFlags = matrix->flags;
        const RwUInt32 transformFlags = transform->flags;
        if ((matrixFlags & mask) != 0) {
            CopyMatrix(&temp, transform);
            CopyMatrix(matrix, &temp);
            return matrix;
        }
        if ((transformFlags & mask) != 0) {
            CopyMatrix(&temp, matrix);
            CopyMatrix(matrix, &temp);
            return matrix;
        }
        reinterpret_cast<MatrixMultFn>(R12AMatrixModule().multFn)(&temp, matrix, transform);
        temp.flags = transformFlags & matrixFlags;
        CopyMatrix(matrix, &temp);
        return matrix;
    }
    default:
        CombineTypeError();
        return nullptr;
    }
}

// 0x7F1D00 (lote R13A, sin validar; 717 bytes; cdecl, 5 argumentos): construye en una matriz local la rotacion de eje unitario (MatrixRotateOmcSineCpp: right, up, at; flags = 3; pos = 0; los rellenos quedan sin escribir, en el EXE son basura de
// pila) y la combina: 0 (reemplazo) copia la local a `matrix`; 1 (preconcatenar): si matrix.flags & (flags del modulo & 0x20000) copia la local, si no temp = multFn(local, matrix) con temp.flags = matrix.flags & 3 y copia temp; 2 (postconcatenar):
// igual con temp = multFn(matrix, local). (Las dos ramas `test al,3` del EXE son inalcanzables: AL es 0 tras el `and eax, 0x20000`.) Otro valor: error 0x80000003 y devuelve NULL (sin leer nada). Devuelve `matrix`.
RwMatrix* RwMatrixRotateOneMinusCosineSine(RwMatrix* matrix, const RwV3d* unitAxis, RwReal oneMinusCosine, RwReal sine, RwOpCombineType combineOp) {
    RwMatrix rotation;
    MatrixRotateOmcSineCpp(reinterpret_cast<float*>(&rotation), &unitAxis->x, oneMinusCosine, sine);
    rotation.flags = 3;
    rotation.pos.x = 0.0f;
    rotation.pos.y = 0.0f;
    rotation.pos.z = 0.0f;
    RwMatrix temp;
    switch (combineOp) {
    case rwCOMBINEREPLACE:
        CopyMatrix(matrix, &rotation);
        return matrix;
    case rwCOMBINEPRECONCAT: {
        const RwUInt32 mask = R12AMatrixModule().flags & 0x20000;
        const RwUInt32 matrixFlags = matrix->flags;
        if ((matrixFlags & mask) != 0) {
            CopyMatrix(&temp, &rotation);
            CopyMatrix(matrix, &temp);
            return matrix;
        }
        reinterpret_cast<MatrixMultFn>(R12AMatrixModule().multFn)(&temp, &rotation, matrix);
        temp.flags = matrixFlags & 3;
        CopyMatrix(matrix, &temp);
        return matrix;
    }
    case rwCOMBINEPOSTCONCAT: {
        const RwUInt32 mask = R12AMatrixModule().flags & 0x20000;
        const RwUInt32 matrixFlags = matrix->flags;
        if ((matrixFlags & mask) != 0) {
            CopyMatrix(&temp, &rotation);
            CopyMatrix(matrix, &temp);
            return matrix;
        }
        reinterpret_cast<MatrixMultFn>(R12AMatrixModule().multFn)(&temp, matrix, &rotation);
        temp.flags = matrixFlags & 3;
        CopyMatrix(matrix, &temp);
        return matrix;
    }
    default:
        CombineTypeError();
        return nullptr;
    }
}

// 0x7F1FD0 (lote R13A, sin validar; 155 bytes; cdecl, (matriz, eje, angulo en grados, combinacion)): theta = float(angulo * 0.017453292f (0x8595EC)); l2 = float((x*x + y*y) + z*z) del eje; inv = _rwInvSqrt(l2); n = float(inv * eje[i])
// (x, y, z); seno = float(sin(theta)); (1 - coseno) = float(1 - cos(theta)); llama a RwMatrixRotateOneMinusCosineSine (0x7F1D00) con (matriz, n, 1 - coseno, seno, combinacion) y devuelve la matriz.
RwMatrix* RwMatrixRotate(RwMatrix* matrix, const RwV3d* axis, RwReal angle, RwOpCombineType combineOp) {
    float sine;
    float oneMinusCosine;
    MatrixRotateTrigCpp(angle, StaticRef<float>(kDegToRadAddr), &sine, &oneMinusCosine);
    const float inv = _rwInvSqrt(V3dSumSquaresCpp(&axis->x));
    RwV3d unitAxis = *axis;
    V3dScaleCpp(&unitAxis.x, inv);
    RwMatrixRotateOneMinusCosineSine(matrix, &unitAxis, oneMinusCosine, sine, combineOp);
    return matrix;
}

// 0x7F2AB0 (lote R13A, sin validar; 191 bytes; cdecl, (dispositivo, peticion, salida, entrada-salida, numero)): _rwDeviceSystemRequest. Llama a device->fpSystem(peticion, salida, entrada-salida, numero) (+4) y devuelve su resultado si es no nulo.
// Si devuelve 0, atiende por su cuenta las peticiones de subsistema (0xD..0x12): 0xD (numero de subsistemas): *salida = 1, TRUE; 0xE (info): con numero == 0 copia "Only rendering sub system" (0x8E23B0) con rwstrcpy del motor (+0xF8) a salida y
// da TRUE (con numero != 0 cae en el error); 0xF (subsistema actual): *salida = 0, TRUE; 0x10 (fijar): con numero == 0 TRUE (si no, error); 0x11 y 0x12: TRUE sin escribir. Cualquier otra peticion o el error: RwError { 1, _rwerror(0x18, peticion) } y devuelve 0.
RwBool _rwDeviceSystemRequest(RwDevice* device, RwInt32 requestID, void* pOut, void* pInOut, RwInt32 numIn) {
    const RwBool result = device->fpSystem(requestID, pOut, pInOut, numIn);
    if (result != 0) {
        return result;
    }
    switch (requestID) {
    case rwDEVICESYSTEMGETNUMSUBSYSTEMS:
        *static_cast<RwInt32*>(pOut) = 1;
        return TRUE;
    case rwDEVICESYSTEMGETSUBSYSTEMINFO:
        if (numIn == 0) {
            RwEngineInstance->stringFuncs.vecStrcpy(static_cast<RwChar*>(pOut), reinterpret_cast<const RwChar*>(kSubSystemNameString));
            return TRUE;
        }
        break;
    case rwDEVICESYSTEMGETCURRENTSUBSYSTEM:
        *static_cast<RwInt32*>(pOut) = 0;
        return TRUE;
    case rwDEVICESYSTEMSETSUBSYSTEM:
        if (numIn == 0) {
            return TRUE;
        }
        break;
    case rwDEVICESYSTEMFINALIZESTART:
    case rwDEVICESYSTEMINITIATESTOP:
        return TRUE;
    default:
        break;
    }
    StreamRaiseError(0x18, requestID);
    return result;
}

// 0x7F2B90 (lote R13A, sin validar; 6 bytes: `mov eax, [0xC97B20]; ret`): contador de aperturas del motor (lo incrementa RwEngineOpen, lo decrementa RwEngineClose y RwEngineTerm exige 0). Nombre inferido (no esta en el listado).
RwInt32 RwEngineGetOpenCount() {
    return EngineOpenCount();
}

// 0x7F2C00 (lote R13A, sin validar; 43 bytes): peticion 0xD con una variable local inicializada a 1; devuelve la variable (da igual el resultado de la peticion).
RwInt32 RwEngineGetNumSubSystems() {
    RwInt32 numSubSystems = 1;
    _rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMGETNUMSUBSYSTEMS, &numSubSystems, nullptr, 0);
    return numSubSystems;
}

// 0x7F2C30 (lote R13A, sin validar; 43 bytes): peticion 0xE (salida = info, numero = indice); NULL si falla, si no `info`.
RwSubSystemInfo* RwEngineGetSubSystemInfo(RwSubSystemInfo* subSystemInfo, RwInt32 subSystemIndex) {
    if (_rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMGETSUBSYSTEMINFO, subSystemInfo, nullptr, subSystemIndex) == 0) {
        return nullptr;
    }
    return subSystemInfo;
}

// 0x7F2C60 (lote R13A, sin validar; 45 bytes): peticion 0xF con una variable local (sin inicializar en el EXE; aqui 0); -1 si la peticion falla, si no la variable.
RwInt32 RwEngineGetCurrentSubSystem() {
    RwInt32 currentSubSystem = 0;
    if (_rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMGETCURRENTSUBSYSTEM, &currentSubSystem, nullptr, 0) == 0) {
        return -1;
    }
    return currentSubSystem;
}

// 0x7F2C90 (lote R13A, sin validar; 36 bytes): peticion 0x10 con numero = indice; devuelve 1 si el resultado es distinto de 0 y 0 si no.
RwBool RwEngineSetSubSystem(RwInt32 subSystemIndex) {
    return _rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMSETSUBSYSTEM, nullptr, nullptr, subSystemIndex) != 0 ? TRUE : FALSE;
}

// 0x7F2CC0 (lote R13A, sin validar; 45 bytes): peticion 5 con una variable local (sin inicializar en el EXE; aqui 0); -1 si la peticion falla.
RwInt32 RwEngineGetNumVideoModes() {
    RwInt32 numModes = 0;
    if (_rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMGETNUMMODES, &numModes, nullptr, 0) == 0) {
        return -1;
    }
    return numModes;
}

// 0x7F2CF0 (lote R13A, sin validar; 43 bytes; era una envoltura): peticion 6 (salida = vmi, numero = indice); NULL si falla, si no `vmi`.
RwVideoMode* RwEngineGetVideoModeInfo(RwVideoMode* vmi, RwInt32 modeIndex) {
    if (_rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMGETMODEINFO, vmi, nullptr, modeIndex) == 0) {
        return nullptr;
    }
    return vmi;
}

// 0x7F2D20 (lote R13A, sin validar; 45 bytes; era una envoltura): peticion 0xA con una variable local (sin inicializar en el EXE; aqui 0); -1 si la peticion falla.
RwInt32 RwEngineGetCurrentVideoMode() {
    RwInt32 currentMode = 0;
    if (_rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMGETMODE, &currentMode, nullptr, 0) == 0) {
        return -1;
    }
    return currentMode;
}

// 0x7F2D50 (lote R13A, sin validar; 36 bytes): peticion 7 con numero = indice; 1 si el resultado es distinto de 0, 0 si no.
RwBool RwEngineSetVideoMode(RwInt32 modeIndex) {
    return _rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMUSEMODE, nullptr, nullptr, modeIndex) != 0 ? TRUE : FALSE;
}

// 0x7F2E20 (lote R13A, sin validar; 79 bytes): peticion 0x12 (iniciar parada; resultado ignorado), _rwPluginRegistryDeInitObject (0x808740) del registro de motor con el motor, peticion 3 (parar); si esa da distinto de 0 pone
// engineStatus (+0x150) = 2 (leyendo RwEngineInstance de nuevo). Devuelve el resultado de la peticion 3.
RwBool RwEngineStop() {
    RwDevice* const device = &RwEngineInstance->dOpenDevice;
    _rwDeviceSystemRequest(device, rwDEVICESYSTEMINITIATESTOP, nullptr, nullptr, 0);
    _rwPluginRegistryDeInitObject(reinterpret_cast<const RwPluginRegistry*>(kEngineRegistryAddr), RwEngineInstance);
    const RwBool result = _rwDeviceSystemRequest(device, rwDEVICESYSTEMSTOP, nullptr, nullptr, 0);
    if (result != 0) {
        RwEngineInstance->engineStatus = rwENGINESTATUSOPENED;
    }
    return result;
}

// 0x7F2E70 (lote R13A, sin validar; 130 bytes): peticion 2 (arrancar); si da 0 devuelve 0. Si no, _rwPluginRegistryInitObject (0x8086E0) del registro con el motor: distinto de NULL -> RwImageSetGamma (0x803FE0) con la
// gammaCorrection del dispositivo (dword en +0x10), peticion 0x11 (finalizar arranque), engineStatus = 3 y devuelve 1; NULL -> peticion 3 (parar; resultado ignorado) y devuelve 0.
RwBool RwEngineStart() {
    RwDevice* const device = &RwEngineInstance->dOpenDevice;
    if (_rwDeviceSystemRequest(device, rwDEVICESYSTEMSTART, nullptr, nullptr, 0) == 0) {
        return FALSE;
    }
    if (_rwPluginRegistryInitObject(reinterpret_cast<const RwPluginRegistry*>(kEngineRegistryAddr), RwEngineInstance) == nullptr) {
        _rwDeviceSystemRequest(device, rwDEVICESYSTEMSTOP, nullptr, nullptr, 0);
        return FALSE;
    }
    RwImageSetGamma(RwEngineInstance->dOpenDevice.gammaCorrection);
    _rwDeviceSystemRequest(device, rwDEVICESYSTEMFINALIZESTART, nullptr, nullptr, 0);
    RwEngineInstance->engineStatus = rwENGINESTATUSSTARTED;
    return TRUE;
}

// 0x7F2F00 (lote R13A, sin validar; 102 bytes): peticion 1 (cerrar) con el dispositivo del motor actual; si da 0 devuelve 0 (sin tocar nada). Si no, copia el RwGlobals completo (0x56 dwords) al bloque estatico 0xC979C8, hace que
// RwEngineInstance apunte a el, libera el bloque anterior con rwfree (el del bloque estatico, +0x138: se lee de la copia), decrementa el contador de aperturas (0xC97B20), engineStatus = 1 y devuelve el resultado de la peticion.
RwBool RwEngineClose() {
    const RwBool result = _rwDeviceSystemRequest(&RwEngineInstance->dOpenDevice, rwDEVICESYSTEMCLOSE, nullptr, nullptr, 0);
    if (result == 0) {
        return result;
    }
    RwGlobals* const previous = RwEngineInstance;
    RwEngineInstance = &EngineStaticGlobals();
    memcpy(&EngineStaticGlobals(), previous, kEngineGlobalsSize);
    EngineStaticGlobals().memoryFuncs.rwfree(previous);
    EngineOpenCount() = EngineOpenCount() - 1;
    RwEngineInstance->engineStatus = rwENGINESTATUSINITED;
    return result;
}

// 0x7F2F70 (lote R13A, sin validar; 447 bytes; era una envoltura): RwEngineInstance nulo -> bloque estatico 0xC979C8. engineStatus != 1 (Inited) -> error 0x80000001, 0; initParams NULL -> error 0x80000016, 0. Obtiene el dispositivo con la
// funcion 0x7F9C20 (sin reconstruir; NULL -> devuelve 0 sin error); reserva sizeOfStruct (+0 del registro 0x8E2298) con rwmalloc (+0x134, hint 0x40000) del bloque estatico y lo asigna a RwEngineInstance; NULL -> restaura el estatico,
// error 0x80000013 con el tamano y devuelve 0. Copia 0x158 bytes del estatico al nuevo bloque, peticiones 4 (registrar: salida = &dOpenDevice, entrada = &memoryFuncs), 0 (abrir: entrada = initParams; si da 0 restaura el bloque estatico, copia el nuevo
// al estatico, lo libera con rwfree y devuelve 0) y 0xB (estandares: salida = &stdFunc, numero 0x1D); incrementa el contador de aperturas, engineStatus = 2 y devuelve 1.
RwBool RwEngineOpen(RwEngineOpenParams* initParams) {
    if (RwEngineInstance == nullptr) {
        RwEngineInstance = &EngineStaticGlobals();
    }
    if (RwEngineInstance->engineStatus != rwENGINESTATUSINITED) {
        StreamRaiseError(static_cast<RwInt32>(0x80000001));
        return FALSE;
    }
    if (initParams == nullptr) {
        StreamRaiseError(static_cast<RwInt32>(0x80000016));
        return FALSE;
    }
    RwDevice* const device = _rwDeviceGetHandle();
    if (device == nullptr) {
        return FALSE;
    }
    RwGlobals* const created = static_cast<RwGlobals*>(RwEngineInstance->memoryFuncs.rwmalloc(StaticRef<RwUInt32>(kEngineRegistryAddr), 0x40000));
    RwEngineInstance = created;
    if (created == nullptr) {
        RwEngineInstance = &EngineStaticGlobals();
        StreamRaiseError(static_cast<RwInt32>(0x80000013), StaticRef<RwInt32>(kEngineRegistryAddr));
        return FALSE;
    }
    memcpy(created, &EngineStaticGlobals(), kEngineGlobalsSize);
    _rwDeviceSystemRequest(device, rwDEVICESYSTEMREGISTER, &RwEngineInstance->dOpenDevice, &RwEngineInstance->memoryFuncs, 0);
    if (_rwDeviceSystemRequest(device, rwDEVICESYSTEMOPEN, nullptr, initParams, 0) == 0) {
        RwEngineInstance = &EngineStaticGlobals();
        memcpy(&EngineStaticGlobals(), created, kEngineGlobalsSize);
        EngineStaticGlobals().memoryFuncs.rwfree(created);
        return FALSE;
    }
    _rwDeviceSystemRequest(device, rwDEVICESYSTEMSTANDARDS, RwEngineInstance->stdFunc, nullptr, rwSTANDARDNUMOFSTANDARD);
    EngineOpenCount() = EngineOpenCount() + 1;
    RwEngineInstance->engineStatus = rwENGINESTATUSOPENED;
    return TRUE;
}

// 0x7F3130 (lote R13A, sin validar; 55 bytes; era una envoltura): si el contador de aperturas es 0 llama a 0x808320, 0x804240 y 0x8020F0 (cierres de modulos, sin reconstruir) y pone engineStatus = 0; devuelve (contador == 0).
RwBool RwEngineTerm() {
    const RwBool canTerminate = EngineOpenCount() == 0 ? TRUE : FALSE;
    if (canTerminate) {
        _rwPluginRegistryClose();
        _rwFileSystemClose();
        _rwMemoryClose();
        RwEngineInstance->engineStatus = rwENGINESTATUSIDLE;
    }
    return canTerminate;
}

namespace {
// RwPluginRegistryAddPlugin sobre el registro de motor (0x8E2298): argumentos (size, id, constructor, destructor, copia nula), en la numeracion de la pila del EXE.
RwInt32 EnginePluginAdd(RwInt32 size, RwUInt32 pluginID, uintptr_t constructor, uintptr_t destructor) {
    return _rwPluginRegistryAddPlugin(reinterpret_cast<RwPluginRegistry*>(kEngineRegistryAddr), size, pluginID, reinterpret_cast<RwPluginObjectConstructor>(constructor),
                                      reinterpret_cast<RwPluginObjectDestructor>(destructor), nullptr);
}
}

// 0x7F3170 (lote R13A, sin validar; 788 bytes; era una envoltura; cdecl, (memFuncs, initFlags, resArenaSize)): RwEngineInstance = bloque estatico 0xC979C8; con initFlags & 1 los asignadores de lista libre (+0x144 y +0x148) son 0x7F3490 y 0x7F34B0
// y se llama a 0x801970(0), si no 0x801C30 y 0x801D50 y 0x801970(1); resArenaInitSize (+0x154) = resArenaSize. Con engineStatus != 0 devuelve 0. Abre en orden 0x80A240 (_rwStringOpen), 0x801FD0(memFuncs), 0x804140 y 0x8082E0 (un fallo
// deshace los anteriores: 0x80A440, 0x8020F0, 0x804240, en ese orden inverso, y devuelve 0); registra 14 plugins de motor en 0x8E2298 con _rwPluginRegistryAddPlugin (ids 0x40F, 0x401, 0x40D, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x408,
// 0x409, 0x412, luego 0x807C70(), 0x40A, 0x40B) y hace OR de todos los resultados: si es negativo (con signo) deshace (0x808320, 0x804240, 0x8020F0, 0x80A440) y devuelve 0. Si no, _rwDeviceRegisterPlugin (0x7F5F60): 0 -> mismo deshacer y 0;
// distinto de 0 -> engineStatus = 1 y devuelve ese valor.
RwBool RwEngineInit(const RwMemoryFunctions* memFuncs, RwUInt32 initFlags, RwUInt32 resArenaSize) {
    RwEngineInstance = &EngineStaticGlobals();
    RwInt32 arenaMode;
    if ((initFlags & 1) != 0) {
        EngineStaticGlobals().memoryAlloc = reinterpret_cast<RwMemoryAllocFn>(0x7F3490);
        EngineStaticGlobals().memoryFree = reinterpret_cast<RwMemoryFreeFn>(0x7F34B0);
        arenaMode = 0;
    } else {
        EngineStaticGlobals().memoryAlloc = reinterpret_cast<RwMemoryAllocFn>(0x801C30);
        EngineStaticGlobals().memoryFree = reinterpret_cast<RwMemoryFreeFn>(0x801D50);
        arenaMode = 1;
    }
    _rwFreeListSetPreallocationMode(arenaMode);
    RwEngineInstance->resArenaInitSize = resArenaSize;
    if (RwEngineInstance->engineStatus != rwENGINESTATUSIDLE) {
        return 0;
    }
    RwInt32 status = ExeCall0(0x80A240);
    if (status == 0) {
        return 0;
    }
    status = _rwMemoryOpen(memFuncs);
    if (status == 0) {
        _rwStringClose();
        return 0;
    }
    status = _rwFileSystemOpen();
    if (status == 0) {
        _rwMemoryClose();
        _rwStringClose();
        return 0;
    }
    status = _rwPluginRegistryOpen();
    if (status == 0) {
        _rwFileSystemClose();
        _rwMemoryClose();
        _rwStringClose();
        return 0;
    }
    const RwInt32 r1 = EnginePluginAdd(8, 0x40F, 0x8087D0, 0x808810);
    const RwInt32 r2 = EnginePluginAdd(0x18, 0x401, 0x7EDE90, 0x7EDE20);
    const RwInt32 r3 = EnginePluginAdd(0, 0x40D, 0x80AA40, 0x80AA50);
    const RwInt32 r4 = EnginePluginAdd(0x18, 0x402, 0x7F16C0, 0x7F1660);
    const RwInt32 r5 = EnginePluginAdd(4, 0x403, 0x7EFEF0, 0x7EFF70);
    const RwInt32 r6 = EnginePluginAdd(4, 0x404, 0x7EC780, 0x7EC7E0);
    const RwInt32 r7 = EnginePluginAdd(4, 0x405, 0x7EE110, 0x7EE0B0);
    const RwInt32 r8 = EnginePluginAdd(0x220, 0x406, 0x802280, 0x8024E0);
    const RwInt32 r9 = EnginePluginAdd(0x64, 0x407, 0x7FB370, 0x7FB310);
    const RwInt32 r10 = EnginePluginAdd(0x34, 0x408, 0x7F3EA0, 0x7F3D00);
    const RwInt32 r11 = EnginePluginAdd(0x60, 0x409, 0x807C40, 0x807C60);
    const RwInt32 r12 = EnginePluginAdd(4, 0x412, 0x80A780, 0x80A7E0);
    const RwInt32 rExtra = _rwEngineModuleStubOpen();
    const RwInt32 r13 = EnginePluginAdd(0x74, 0x40A, 0x7EFE20, 0x7EFDE0);
    const RwInt32 r14 = EnginePluginAdd(0x28, 0x40B, 0x807C90, 0x807D80);
    const RwInt32 combined = rExtra | r12 | r11 | r10 | r9 | r8 | r7 | r6 | r5 | r4 | r3 | r2 | r1 | r14 | r13;
    if (combined < 0) {
        _rwPluginRegistryClose();
        _rwFileSystemClose();
        _rwMemoryClose();
        _rwStringClose();
        return 0;
    }
    status = ExeCall0(0x7F5F60);
    if (status == 0) {
        _rwPluginRegistryClose();
        _rwFileSystemClose();
        _rwMemoryClose();
        _rwStringClose();
        return 0;
    }
    RwEngineInstance->engineStatus = rwENGINESTATUSINITED;
    return status;
}

// Lote R13A (sin validar): matrices (crear, invertir, escalar, trasladar, rotar y combinar), el callback de vector por defecto y el motor (RwEngine*).
void RwPlCoreInjectHooksR13A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(_rwV3dTransformVectorDefault, 0x7ED880);
    RH_ScopedGlobalInstall(RwMatrixRotateOneMinusCosineSine, 0x7F1D00);
    RH_ScopedGlobalInstall(RwMatrixRotate, 0x7F1FD0);
    RH_ScopedGlobalInstall(RwMatrixInvert, 0x7F2070);
    RH_ScopedGlobalInstall(_rwMatrixInvertGeneral, 0x7F2160);
    RH_ScopedGlobalInstall(RwMatrixScale, 0x7F22C0);
    RH_ScopedGlobalInstall(RwMatrixTranslate, 0x7F2450);
    RH_ScopedGlobalInstall(RwMatrixTransform, 0x7F25A0);
    RH_ScopedGlobalInstall(RwMatrixCreate, 0x7F2A50);
    RH_ScopedGlobalInstall(_rwDeviceSystemRequest, 0x7F2AB0);
    RH_ScopedGlobalInstall(RwEngineGetOpenCount, 0x7F2B90);
    RH_ScopedGlobalInstall(RwEngineGetNumSubSystems, 0x7F2C00);
    RH_ScopedGlobalInstall(RwEngineGetSubSystemInfo, 0x7F2C30);
    RH_ScopedGlobalInstall(RwEngineGetCurrentSubSystem, 0x7F2C60);
    RH_ScopedGlobalInstall(RwEngineSetSubSystem, 0x7F2C90);
    RH_ScopedGlobalInstall(RwEngineGetNumVideoModes, 0x7F2CC0);
    RH_ScopedGlobalInstall(RwEngineGetVideoModeInfo, 0x7F2CF0);
    RH_ScopedGlobalInstall(RwEngineGetCurrentVideoMode, 0x7F2D20);
    RH_ScopedGlobalInstall(RwEngineSetVideoMode, 0x7F2D50);
    RH_ScopedGlobalInstall(RwEngineStop, 0x7F2E20);
    RH_ScopedGlobalInstall(RwEngineStart, 0x7F2E70);
    RH_ScopedGlobalInstall(RwEngineClose, 0x7F2F00);
    RH_ScopedGlobalInstall(RwEngineOpen, 0x7F2F70);
    RH_ScopedGlobalInstall(RwEngineTerm, 0x7F3130);
    RH_ScopedGlobalInstall(RwEngineInit, 0x7F3170);
}

// Lote R14A (sin validar): listas libres, memoria y ficheros del motor, registro de plugins, error pendiente y arena de recursos de RwPlCore (0x7F9C20..0x808950).
void RwPlCoreInjectHooksR14A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(_rwDeviceGetHandle, 0x7F9C20);
    RH_ScopedGlobalInstall(_rwFreeListSetPreallocationMode, 0x801970);
    RH_ScopedGlobalInstall(FreeListCreateAndPreallocateSpaceBody, 0x8019B0);
    RH_ScopedGlobalInstall(RwFreeListDestroy, 0x801B80);
    RH_ScopedGlobalInstall(_rwFreeListFreeDefault, 0x801D50);
    RH_ScopedGlobalInstall(RwFreeListPurge, 0x801E00);
    RH_ScopedGlobalInstall(RwFreeListForAllUsed, 0x801E90);
    RH_ScopedGlobalInstall(RwFreeListPurgeAllFreeLists, 0x801F90);
    RH_ScopedGlobalInstall(_rwMemoryOpen, 0x801FD0);
    RH_ScopedGlobalInstall(_rwMemoryClose, 0x8020F0);
    RH_ScopedGlobalInstall(_rwFileSystemOpen, 0x804140);
    RH_ScopedGlobalInstall(_rwFileSystemClose, 0x804240);
    RH_ScopedGlobalInstall(_rwEngineModuleStubOpen, 0x807C70);
    RH_ScopedGlobalInstall(_rwCameraBeginUpdateStub, 0x807C80);
    RH_ScopedGlobalInstall(RwResourcesFreeResEntry, 0x807DE0);
    RH_ScopedGlobalInstall(_rwResourcesPurge, 0x807E50);
    RH_ScopedGlobalInstall(RwResourcesEmptyArena, 0x8081F0);
    RH_ScopedGlobalInstall(_rwPluginRegistryOpen, 0x8082E0);
    RH_ScopedGlobalInstall(_rwPluginRegistryClose, 0x808320);
    RH_ScopedGlobalInstall(_rwPluginRegistryAddPlugin, 0x8084A0);
    RH_ScopedGlobalInstall(_rwPluginRegistryInitObject, 0x8086E0);
    RH_ScopedGlobalInstall(_rwPluginRegistryDeInitObject, 0x808740);
    RH_ScopedGlobalInstall(_rwPluginRegistryCopyObject, 0x808770);
    RH_ScopedGlobalInstall(RwErrorGet, 0x808880);
    RH_ScopedGlobalInstall(_rwPluginRegistryAddPluginStream, 0x8088E0);
    RH_ScopedGlobalInstall(_rwPluginRegistryAddPlgnStrmlwysCB, 0x808920);
    RH_ScopedGlobalInstall(_rwPluginRegistryAddPlgnStrmRightsCB, 0x808950);
}

// Lote R15A (sin validar): lectura, escritura, salto, tamano y derechos de los bloques de datos del registro de plugins (0x808980..0x808C10), listas SList, cadenas del motor (apertura, stricmp,
// escritura y lectura en flujo) y liberacion en el monton del arena (0x810040).
void RwPlCoreInjectHooksR15A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(_rwPluginRegistryReadDataChunks, 0x808980);
    RH_ScopedGlobalInstall(_rwPluginRegistryInvokeRights, 0x808AB0);
    RH_ScopedGlobalInstall(_rwPluginRegistryGetSize, 0x808B00);
    RH_ScopedGlobalInstall(_rwPluginRegistryWriteDataChunks, 0x808B40);
    RH_ScopedGlobalInstall(_rwPluginRegistrySkipDataChunks, 0x808C10);
    RH_ScopedGlobalInstall(_rwSListCreate, 0x809160);
    RH_ScopedGlobalInstall(_rwSListGetNewEntry, 0x809240);
    RH_ScopedGlobalInstall(_rwSListDestroy, 0x809440);
    RH_ScopedGlobalInstall(_rwStringOpen, 0x80A240);
    RH_ScopedGlobalInstall(_rwStringStricmp, 0x80A350);
    RH_ScopedGlobalInstall(_rwStringStreamWrite, 0x80A4A0);
    RH_ScopedGlobalInstall(_rwStringStreamFindAndRead, 0x80A510);
    RH_ScopedGlobalInstall(_rwResArenaHeapFree, 0x810040);
}

// Lote R23A (sin validar): RwV3dNormalize (0x7ED9B0) y RwV3dTransformPoints (0x7EDD90).
void RwPlCoreInjectHooksR23A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(RwV3dNormalize, 0x7ED9B0);
    RH_ScopedGlobalInstall(RwV3dTransformPoints, 0x7EDD90);
}

// Lote R24A (sin validar): matrices (RwMatrixUpdate, RwMatrixMultiply, RwMatrixDestroy, _rwMatrixSetMultFn).
void RwPlCoreInjectHooksR24A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(_rwMatrixSetMultFn, 0x7F12C0);
    RH_ScopedGlobalInstall(RwMatrixUpdate, 0x7F18A0);
    RH_ScopedGlobalInstall(RwMatrixMultiply, 0x7F18B0);
    RH_ScopedGlobalInstall(RwMatrixDestroy, 0x7F2A20);
}
