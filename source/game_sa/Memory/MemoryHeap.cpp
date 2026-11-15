/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMemoryHeap
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "MemoryHeap.h"
#include "FileMgr.h"

#define CORE_COMMON_SIZE_COUNT 64

static_assert(sizeof(HeapBlockDesc) <= MEMORY_HEAP_ALIGN, "HeapBlockDesc is too big");

uint32 memMoved = 0;

inline uint32 GetCommonSize(uint32 size) {
    return (size >> 4) - 1;
}

inline HeapBlockDesc* GetBlockDesc(void* mem) {
    assert(mem);
    return (HeapBlockDesc*)mem - 1;
}

inline HeapFreeBlockDesc* GetFreeBlockDesc(void* mem) {
    assert(mem);
    return (HeapFreeBlockDesc*)GetBlockDesc(mem);
}

void CMemoryHeap::InjectHooks() {
    RH_ScopedClass(CMemoryHeap);
    RH_ScopedCategory("Memory");

    RH_ScopedInstall(IntegrityCheck, 0x72E8E0);
    RH_ScopedInstall(JoinAdjacentFreeBlocks, 0x72E870); // lote 208A (sin validar)
    RH_ScopedInstall(Shutdown, 0x72EC60);               // lote 208A (sin validar)
    RH_ScopedInstall(FillInBlockData, 0x72EC80);        // lote 208A (sin validar)
    RH_ScopedInstall(Malloc, 0x72ED80);                 // lote 208A (sin validar)
    RH_ScopedInstall(Init, 0x72F180);                   // lote 208A (sin validar)
}

// 0x72E860
CMemoryHeap::CMemoryHeap() {
    m_FirstBlock = nullptr;
}

// 0x72F180 (lote 208A, sin validar; thiscall, tres argumentos de pila, `ret 0Ch`, 418 bytes): inicializa el heap sobre `mem`/`size`. Se escriben EXACTAMENTE los campos del EXE y en su orden:
// m_FirstBlock = mem; m_LastBlock = mem + (size - 0x10); m_nMemUsed = 0; m_bMovingRestricted = true; m_nMemId = -1; el bloque final (en uso, memId -1, tamano 0, anterior = primero) y el primero (tamano
// size - 0x20, libre, anterior nulo; su memId y m_pDebugInfo NO se tocan); m_FreeListEnd.m_Prev = &m_FreeListBegin y su tamano = 0x7FFFFFFF; m_FreeListBegin.m_Next = &m_FreeListEnd (m_FreeListBegin.m_Prev y
// m_FreeListEnd.m_Next NO se escriben) y se inserta el primer bloque tras m_FreeListBegin en linea (InsertHeapFreeBlock). Solo con bInitSizes: m_paCommonSizes = nullptr, se reservan 0xF00 bytes con
// CMemoryHeap::Malloc (0x72ED80; el puntero aun es nulo durante la reserva) y se inician las 64 CommonSize (tamanos 0x10, 0x20 ... 0x400, igual que CommonSize::Init: End.m_Prev, Start.m_Next, tamano de
// bloque y dos contadores a cero). Sin bInitSizes, m_paCommonSizes NO se toca (a diferencia de la version anterior, que lo ponia a nulo).
void CMemoryHeap::Init(void* mem, uint32 size, bool bInitSizes) {
    const uint32 lastBlockOffset = size - sizeof(HeapBlockDesc);

    m_FirstBlock = (HeapBlockDesc*)mem;
    m_LastBlock  = (HeapBlockDesc*)((uint8*)mem + lastBlockOffset);

    m_nMemUsed          = 0;
    m_bMovingRestricted = true;
    m_nMemId            = -1;

    // ending block
    m_LastBlock->m_bInUse    = true;
    m_LastBlock->m_nMemId    = -1;
    m_LastBlock->m_nSize     = 0;
    m_LastBlock->m_PrevBlock = m_FirstBlock;

    // map free block onto memory
    m_FirstBlock->m_nSize     = (int32)(lastBlockOffset - sizeof(HeapBlockDesc));
    m_FirstBlock->m_bInUse    = false;
    m_FirstBlock->m_PrevBlock = nullptr;

    // update list
    m_FreeListEnd.m_Prev   = &m_FreeListBegin;
    m_FreeListEnd.m_nSize  = std::numeric_limits<int32>::max();
    m_FreeListBegin.m_Next = &m_FreeListEnd;

    // insert mapped free block (InsertHeapFreeBlock en linea, sin comprobaciones)
    auto* const first              = (HeapFreeBlockDesc*)m_FirstBlock;
    first->m_Next                  = m_FreeListBegin.m_Next;
    m_FreeListBegin.m_Next->m_Prev = first;
    first->m_Prev                  = &m_FreeListBegin;
    m_FreeListBegin.m_Next         = first;

    if (bInitSizes) {
        m_paCommonSizes = nullptr;
        m_paCommonSizes = (CommonSize*)Malloc(sizeof(CommonSize) * MEMORY_HEAP_CSIZES);
        int32 asize = MEMORY_HEAP_ALIGN;
        for (auto i = 0; i < MEMORY_HEAP_CSIZES; ++i) {
            m_paCommonSizes[i].Init(asize);
            asize += MEMORY_HEAP_ALIGN;
        }
    }
}

// 0x72EC60 (lote 208A, sin validar; thiscall, sin argumentos de pila, `ret`, 11 bytes con el prologo `push esi`): libera el bloque del heap con el free() del CRT DEL EXE (0x82413F, cdecl, no
// CMemoryMgr::Free) y pone m_FirstBlock a nulo. El EXE NO llama a PrintMemoryLeaks (la version anterior si).
void CMemoryHeap::Shutdown() {
    ((void(__cdecl*)(void*))0x82413F)(m_FirstBlock);
    m_FirstBlock = nullptr;
}

// 0x72EB10
void CMemoryHeap::RegisterFree(HeapFreeBlockDesc* desc) {
    m_nMemUsed -= desc->m_nSize + sizeof(HeapBlockDesc);
}

// 0x72EAF0
void CMemoryHeap::RegisterMalloc(HeapBlockDesc* desc) {
#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    assert(!desc->m_Flags.IsBoundBlock);
#endif
    desc->m_bInUse = true;
    desc->m_nMemId = m_nMemId;
    m_nMemUsed += desc->m_nSize + sizeof(HeapBlockDesc);
}

HeapFreeBlockDesc* CMemoryHeap::_JoinFreeBlocks(HeapFreeBlockDesc* desc) {
    // join next located block
    auto* next = (HeapFreeBlockDesc*)desc->_GetNextLocatedBlock();
    if (next && !next->m_bInUse) {
        next->RemoveHeapFreeBlock();
        next = (HeapFreeBlockDesc*)((uint8*)next + next->m_nSize + sizeof(HeapBlockDesc));
    }
    assert(next);

    // join prev located block
    auto* prev = (HeapFreeBlockDesc*)desc->m_PrevBlock;
    if (prev && !prev->m_bInUse) {
        prev->RemoveHeapFreeBlock();
        desc = prev;
    }

    // recount total size
    desc->m_nSize = (uint8*)next - (uint8*)desc - sizeof(HeapBlockDesc);

    // update pointer to previous located block
    next->m_PrevBlock = desc;

    if (m_paCommonSizes && desc->m_nSize < (MEMORY_HEAP_CSIZES * MEMORY_HEAP_ALIGN)) {
        m_paCommonSizes[GetCommonSize(desc->m_nSize)].Free(desc);
    } else {
        m_FreeListBegin.InsertOrderedHeapFreeBlock(desc);
    }

    return (HeapFreeBlockDesc*)desc;
}

// 0x72EE60
void CMemoryHeap::Free(void* memory) {
    if (!memory) {
        return;
    }
    assert(IsHeapPointer(memory));

    HeapFreeBlockDesc* desc = GetFreeBlockDesc(memory);

#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    assert(!desc->m_Flags.bIsBoundBlock && "Cannot free BOUND heap block!");
#endif
    assert(desc->m_bInUse && "Memory block was already deallocated!");

    // free this block
    RegisterFree(desc);
    desc->m_bInUse = false;

    // join prev and next free blocks
    desc = _JoinFreeBlocks(desc);

#ifdef MEMORY_MGR_USE_HEAP_FLAGS
    desc->m_Flags.ResetFlags(false);
#endif

#ifdef _DEBUG
    desc->m_nMemId = 0;
#endif
}

// 0x72EEF0
void* CMemoryHeap::Realloc(void* memory, uint32 size) {
    if (!memory) {
        return Malloc(size);
    }

    auto nBlockSize = size;
    if (size & (MEMORY_HEAP_ALIGN - 1)) {
        nBlockSize = (size & ~(MEMORY_HEAP_ALIGN - 1)) + MEMORY_HEAP_ALIGN;
    }

    assert(IsHeapPointer(memory));
    HeapBlockDesc* desc = GetBlockDesc(memory);

    // extend block
    if (int32(nBlockSize) > GetBlockDesc(memory)->m_nSize) {
        // __DUMMY();

        void* newMem = CMemoryHeap::Malloc(nBlockSize);
        memcpy(newMem, memory, GetBlockDesc(memory)->m_nSize);
        CMemoryHeap::Free(memory);
        return newMem;
    }

    // shrink block
    m_nMemUsed -= desc->m_nSize + sizeof(HeapBlockDesc); // todo: RegisterFree?
    FillInBlockData(GetBlockDesc(memory), desc->_GetNextLocatedBlock(), nBlockSize);
    return memory;
}

// 0x72ED80 (lote 208A, sin validar; thiscall, un argumento de pila, `ret 4`, 212 bytes): reserva `size` bytes del heap. Como el EXE: el tamano se redondea a multiplo de 16 por arriba (solo si
// tiene bits bajos); con m_paCommonSizes y bloque <= 0x400 el indice es (bloque >> 4) - 1 (division sin signo, comparacion CON signo contra 64: un tamano 0 da indice -1 y se lee antes del array) y se
// recorren los indices hasta el final del array (offset en bytes < 0xF00, saltos de 0x3C): la primera lista no vacia entrega su primer bloque (--m_NumFreeBlocks, desenlazado, m_bInUse = 1,
// m_nMemId = palabra baja de m_nMemId, m_nMemUsed += tamano + 0x10) y devuelve bloque + 0x10; cada lista vacia suma 1 a m_NumFails. Si no, busca el primer bloque de la lista libre (de m_FreeListBegin.m_Next
// hasta m_FreeListEnd, SIN examinarlo) con tamano >= bloque (comparacion sin signo), lo desenlaza (el EXE relee m_Next tras el primer almacenamiento), llama a FillInBlockData(bloque,
// bloque + tamano + 0x10, bloque) y devuelve bloque + 0x10; sin hueco, nulo. Sin aserciones, sin registro de errores y sin banderas de heap (a diferencia de la version anterior).
void* CMemoryHeap::Malloc(uint32 size) {
    uint32 blockSize = size;
    if (size & (MEMORY_HEAP_ALIGN - 1)) {
        blockSize = (size & ~(uint32)(MEMORY_HEAP_ALIGN - 1)) + MEMORY_HEAP_ALIGN;
    }

    if (m_paCommonSizes && blockSize <= (MEMORY_HEAP_CSIZES * MEMORY_HEAP_ALIGN)) {
        const int32 commonSize = (int32)(blockSize >> 4) - 1;
        if (commonSize < MEMORY_HEAP_CSIZES) {
            int32 offset = commonSize * (int32)sizeof(CommonSize);
            do {
                auto* const cs = (CommonSize*)((uint8*)m_paCommonSizes + offset);
                HeapFreeBlockDesc* const block = cs->m_ListStart.m_Next;
                if (block != &cs->m_ListEnd) {
                    cs->m_NumFreeBlocks--;
                    block->m_Next->m_Prev = block->m_Prev;
                    block->m_Prev->m_Next = block->m_Next;
                    block->m_bInUse = true;
                    block->m_nMemId = (int16)m_nMemId;
                    m_nMemUsed += (uint32)block->m_nSize + sizeof(HeapBlockDesc);
                    return (uint8*)block + sizeof(HeapBlockDesc);
                }
                cs->m_NumFails++;
                offset += (int32)sizeof(CommonSize);
            } while (offset < (int32)(sizeof(CommonSize) * MEMORY_HEAP_CSIZES));
        }
    }

    HeapFreeBlockDesc* const end = &m_FreeListEnd;
    for (HeapFreeBlockDesc* block = m_FreeListBegin.m_Next; block != end; block = block->m_Next) {
        if ((uint32)block->m_nSize >= blockSize) {
            block->m_Next->m_Prev = block->m_Prev;
            block->m_Prev->m_Next = block->m_Next;
            FillInBlockData(block, (HeapBlockDesc*)((uint8*)block + block->m_nSize + sizeof(HeapBlockDesc)), blockSize);
            return (uint8*)block + sizeof(HeapBlockDesc);
        }
    }
    return nullptr;
}

// 0x72E870 (lote 208A, sin validar; thiscall, un argumento de pila, `ret 4`, 96 bytes; el listado lo atribuia a CCullZones; no usa `this`): une un bloque libre con sus vecinos y devuelve el bloque resultante, SIN
// insertarlo en ninguna lista (eso lo hacen en linea sus llamadores, 0x72EC80 y 0x72EE60). El siguiente fisico (desc + tamano + 0x10) se une si NO esta en uso (se desenlaza de la lista libre y se salta su
// tamano; sin comprobar que exista) y el anterior (m_PrevBlock) si NO es nulo y NO esta en uso (se desenlaza y pasa a ser el bloque). Despues: tamano = fin - bloque - 0x10 y
// siguiente->m_PrevBlock = bloque. Es la primera mitad de la CMemoryHeap::_JoinFreeBlocks anterior (que ademas insertaba el resultado).
HeapFreeBlockDesc* CMemoryHeap::JoinAdjacentFreeBlocks(HeapFreeBlockDesc* desc) {
    auto* next = (HeapFreeBlockDesc*)((uint8*)desc + desc->m_nSize + sizeof(HeapBlockDesc));
    auto* const prev = (HeapFreeBlockDesc*)desc->m_PrevBlock;
    if (!next->m_bInUse) {
        next->m_Next->m_Prev = next->m_Prev;
        next->m_Prev->m_Next = next->m_Next;
        next = (HeapFreeBlockDesc*)((uint8*)next + next->m_nSize + sizeof(HeapBlockDesc));
    }
    if (prev && !prev->m_bInUse) {
        prev->m_Next->m_Prev = prev->m_Prev;
        prev->m_Prev->m_Next = prev->m_Next;
        desc = prev;
    }
    desc->m_nSize = (int32)((uint8*)next - (uint8*)desc - sizeof(HeapBlockDesc));
    next->m_PrevBlock = desc;
    return desc;
}

// 0x72EC80 (lote 208A, sin validar; thiscall, tres argumentos de pila (bloque, siguiente bloque ocupado/final, tamano), `ret 0Ch`, 238 bytes): ajusta el bloque `desc` a `size` bytes dentro del hueco que acaba en
// `nextDesc`. desc->m_nSize = size; next = desc + size + 0x10 (comparaciones SIN signo). Si next < nextDesc - 0x10 (queda un resto aprovechable): desc en uso (m_bInUse = 1, m_nMemId = palabra baja de m_nMemId),
// m_nMemUsed += size + 0x10; el resto (next) pasa a ser un bloque libre (m_PrevBlock = desc, tamano = nextDesc - next - 0x10, libre), nextDesc->m_PrevBlock = next, se une con sus vecinos (0x72E870) y se inserta
// en linea: con m_paCommonSizes y tamano <= 0x400 al principio de la lista de ese tamano (indice (tamano >> 4) - 1, con signo si el tamano es 0; ++m_NumFreeBlocks) y si no en la lista libre ordenada por
// tamano (primer bloque con tamano >= el del nuevo, sin signo; se inserta ANTES de el). Si no queda resto: con next < nextDesc el tamano de desc se amplia hasta nextDesc (nextDesc - desc - 0x10);
// nextDesc->m_PrevBlock = desc; desc en uso con memId y m_nMemUsed += su tamano final + 0x10. Sin aserciones. El EXE deja un valor distinto en EAX en cada ruta (nadie lo lee): aqui void.
void CMemoryHeap::FillInBlockData(HeapBlockDesc* desc, HeapBlockDesc* nextDesc, uint32 size) {
    desc->m_nSize = (int32)size;

    auto* const next = (HeapFreeBlockDesc*)((uint8*)desc + size + sizeof(HeapBlockDesc));
    if ((uintptr_t)next < (uintptr_t)nextDesc - sizeof(HeapBlockDesc)) {
        desc->m_bInUse = true;
        desc->m_nMemId = (int16)m_nMemId;
        m_nMemUsed += size + sizeof(HeapBlockDesc);

        next->m_PrevBlock = desc;
        next->m_nSize     = (int32)((uint8*)nextDesc - (uint8*)next - sizeof(HeapBlockDesc));
        next->m_bInUse    = false;
        nextDesc->m_PrevBlock = next;

        HeapFreeBlockDesc* const joined = JoinAdjacentFreeBlocks(next);
        if (m_paCommonSizes && (uint32)joined->m_nSize <= (MEMORY_HEAP_CSIZES * MEMORY_HEAP_ALIGN)) {
            auto* const cs = (CommonSize*)((uint8*)m_paCommonSizes + (int32)((uint32)joined->m_nSize >> 4) * (int32)sizeof(CommonSize) - (int32)sizeof(CommonSize));
            joined->m_Next                 = cs->m_ListStart.m_Next;
            cs->m_ListStart.m_Next->m_Prev = joined;
            joined->m_Prev                 = &cs->m_ListStart;
            cs->m_ListStart.m_Next         = joined;
            cs->m_NumFreeBlocks++;
        } else {
            HeapFreeBlockDesc* found = m_FreeListBegin.m_Next;
            while ((uint32)found->m_nSize < (uint32)joined->m_nSize) {
                found = found->m_Next;
            }
            HeapFreeBlockDesc* const before = found->m_Prev;
            joined->m_Next         = before->m_Next;
            before->m_Next->m_Prev = joined;
            joined->m_Prev         = before;
            before->m_Next         = joined;
        }
        return;
    }

    if ((uintptr_t)next < (uintptr_t)nextDesc) {
        desc->m_nSize = (int32)((uint8*)nextDesc - (uint8*)desc - sizeof(HeapBlockDesc));
    }
    nextDesc->m_PrevBlock = desc;
    desc->m_bInUse = true;
    desc->m_nMemId = (int16)m_nMemId;
    m_nMemUsed += (uint32)desc->m_nSize + sizeof(HeapBlockDesc);
}

void CMemoryHeap::PrintMemoryLeaks() {
#if defined(CORE_USE_MEMORY_HEAP)
    if (!m_nMemUsed)
        return;

    if (m_paCommonSizes && m_nMemUsed == sizeof(CommonSize) * MEMORY_HEAP_CSIZES) {
        return;
    }

    NOTSA_LOG_DEBUG("MemoryHeap 0x{:08x} FUGAS (usados {} bytes de {} bytes en total)\n", m_FirstBlock, m_nMemUsed, _GetHeapSize());
    NOTSA_LOG_DEBUG("-----------------------------------------------------------------------------------------------------------\n");

    HeapBlockDesc* p = m_FirstBlock;
    while (p < m_LastBlock) {
        if (p->m_bInUse) {
            p->_DumpBlockInfo();
        }
        p = p->_GetNextLocatedBlock();
    }
#endif
}

void* CMemoryHeap::GetDebugInfo(void* memory) {
    assert(memory);
    return GetBlockDesc(memory)->m_pDebugInfo;
}

void CMemoryHeap::SetDebugInfo(void* memory, void* nDebugInfo) {
    assert(memory);
    GetBlockDesc(memory)->m_pDebugInfo = nDebugInfo;
}

// 0x72E980
uint32 CMemoryHeap::GetSizeOfHoles() {
    uint32 nHolesSize = 0;

    HeapFreeBlockDesc* block;

    for (block = m_FreeListBegin.m_Next; block != &m_FreeListEnd; block = block->m_Next) {
        if (block->m_Next == &m_FreeListEnd)
            break;
        nHolesSize += block->m_nSize;
    }

    if (m_paCommonSizes) {
        for (auto i = 0; i < CORE_COMMON_SIZE_COUNT; ++i) {
            block = m_paCommonSizes[i].m_ListStart.m_Next;
            while (block != &m_paCommonSizes[i].m_ListEnd) {
                nHolesSize += block->m_nSize;
                block = block->m_Next;
            }
        }
    }

    return nHolesSize;
}

// 0x72E8E0
// Recorre las listas de bloques libres, las listas de tamanos comunes, todos los bloques contiguos acumulando el uso por memId
// y la cadena m_PrevBlock del ultimo bloque. No escribe en el monton: solo falla (bucle infinito o acceso invalido) si el
// monton esta corrupto. Los punteros y el acumulador son volatile para que un build optimizado no elimine los recorridos.
void CMemoryHeap::IntegrityCheck() {
    for (HeapFreeBlockDesc* volatile free = m_FreeListBegin.m_Next; free != &m_FreeListEnd; free = free->m_Next) {
    }

    if (m_paCommonSizes) {
        for (auto i = 0; i < CORE_COMMON_SIZE_COUNT; ++i) {
            for (HeapFreeBlockDesc* volatile free = m_paCommonSizes[i].m_ListStart.m_Next; free != &m_paCommonSizes[i].m_ListEnd; free = free->m_Next) {
            }
        }
    }

    // Como el original: 20 acumuladores a cero; el indice es el memId con signo (-1 se omite) sin comprobar el rango.
    volatile uint32 usedByMemId[20]{};
    for (HeapBlockDesc* block = m_FirstBlock; block != m_LastBlock; block = block->_GetNextLocatedBlock()) {
        if (block->m_bInUse && block->m_nMemId != -1) {
            usedByMemId[block->m_nMemId] = usedByMemId[block->m_nMemId] + (block->m_nSize + sizeof(HeapBlockDesc));
        }
    }

    for (HeapBlockDesc* volatile block = m_LastBlock->m_PrevBlock; block; block = block->m_PrevBlock) {
    }
}

void* CMemoryHeap::MoveHeapBlock(HeapFreeBlockDesc* free, HeapBlockDesc* used) {
    assert(free && used);

    auto oldHeapID = m_nMemId;
    m_nMemId = used->m_nMemId;
    free->RemoveHeapFreeBlock();
    FillInBlockData(free, free->_GetNextLocatedBlock(), used->m_nSize);
    m_nMemId = oldHeapID;
    memcpy(free->_GetBlockData(), used->_GetBlockData(), used->m_nSize);
    memMoved += used->m_nSize;
    Free(used->_GetBlockData());
    return nullptr;
}

HeapFreeBlockDesc* CMemoryHeap::WhereShouldMemoryMove(void* memory) {
    assert(true && "NOT IMPLEMENTED");
    return nullptr;
}

void* CMemoryHeap::MoveMemory(void* memory) {
    if (HeapFreeBlockDesc* to = WhereShouldMemoryMove(memory)) {
        return MoveHeapBlock(to, GetBlockDesc(memory));
    }
    return memory;
}

void* CMemoryHeap::MoveMemory(void* memory, void** _ppUnk, int32 _iUnk) {
    assert(true && "NOT IMPLEMENTED");
    return memory;
}

// 0x72E9E0
void CMemoryHeap::ParseHeap() {
    auto* file = CFileMgr::OpenFileForWriting("heap.txt");
    // assert(file && "Failed to open %s for writing!", "heap.txt");

    CTimer::Stop();

    IntegrityCheck();

    HeapBlockDesc* block = m_FirstBlock;
    uint32 k = 0;
    char buffer[16] = {0};

    while (block < m_LastBlock) {
        char cState = '*';
        if (block->m_nMemId & 0xFF) {
            cState = '@' + (block->m_nMemId & 0xFF);
        }

        if ((k & 63) == 0) { // '?'
            sprintf_s(buffer, "\n%5dK:", k / 64);
            CFileMgr::Write(file, buffer, 8);
        }

        CFileMgr::Write(file, "#", 1);
        ++k;

        for (uint32 size = block->m_nSize >> 4; size; --size) {
            if ((k & 63) == 0) {
                sprintf_s(buffer, "\n%5dK:", k / 64);
                CFileMgr::Write(file, buffer, 8);
            }
            CFileMgr::Write(file, &cState, sizeof(char));
            ++k;
        }
        block = block->_GetNextLocatedBlock();
    }

    CTimer::Update();

    CFileMgr::CloseFile(file);
}

void CMemoryHeap::_DumpHeapStatus(bool bCommonSizes) {
    NOTSA_LOG_DEBUG("MemoryHeap 0x{:08x} ESTADO (usados {} bytes de {} bytes en total)", LOG_PTR(m_FirstBlock), m_nMemUsed, GetHeapSize());

    NOTSA_LOG_DEBUG(" - Mapa de bloques");
    HeapBlockDesc* p = m_FirstBlock;
    while (p != m_LastBlock) {
        p->_DumpBlockInfo();
        p = p->_GetNextLocatedBlock();
    }

    NOTSA_LOG_DEBUG(" - Lista de bloques libres");
    HeapFreeBlockDesc* pF = &m_FreeListBegin;
    do {
        pF->_DumpBlockInfo();
        pF = pF->m_Next;
    } while (pF != m_FreeListEnd.m_Next);

    if (m_paCommonSizes && bCommonSizes) {
        NOTSA_LOG_DEBUG(" - Tamaños comunes (CommonSizes)");
        for (auto i = 0; i < CORE_COMMON_SIZE_COUNT; ++i) {
            if (m_paCommonSizes[i].m_NumFreeBlocks) {
                m_paCommonSizes[i]._DumpInfo();
            }
        }
    }

    NOTSA_LOG_DEBUG(" Porcentaje usado:       {:0.1f}%", 100.f * float(m_nMemUsed) / float(GetHeapSize()));
    NOTSA_LOG_DEBUG(" Mayor bloque libre:     {} bytes", GetLargestFreeBlock());
    NOTSA_LOG_DEBUG(" Tamaño de los huecos:  {} bytes", GetSizeOfHoles());
}

HeapBlockDesc* CMemoryHeap::_FindBlockDesc(void* memory) {
    HeapBlockDesc* p = m_FirstBlock;
    while (p < m_LastBlock) {
        if (p->m_nMemId != -1) {
            if (memory >= p->_GetBlockData()) {
                if ((int8*)memory < (int8*)p->_GetBlockData() + p->m_nSize) {
                    return p;
                }
            }
        }
        p = p->_GetNextLocatedBlock();
    }
    return nullptr;
}
