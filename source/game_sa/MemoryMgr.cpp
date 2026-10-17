#include "StdInc.h"
#include "X87Math.h"

#include <cstdlib>

#ifdef MEMORY_MGR_USE_MEMORY_HEAP
static CRITICAL_SECTION   gstrucMemoryHeapCS;
static CRITICAL_SECTION   gstrucScratchPadCS;
static LPCRITICAL_SECTION gpMemoryHeapCS;
static LPCRITICAL_SECTION gpScratchPadCS;
#endif

#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    #define LOCK_MEMORYHEAP()       EnterCriticalSection(gpMemoryHeapCS)
    #define UNLOCK_MEMORYHEAP()     LeaveCriticalSection(gpMemoryHeapCS)
    #define LOCK_SCRATCHPAD()       EnterCriticalSection(gpScratchPadCS)
    #define UNLOCK_SCRATCHPAD()     LeaveCriticalSection(gpScratchPadCS)
#else
    #define LOCK_MEMORYHEAP()
    #define UNLOCK_MEMORYHEAP()
    #define LOCK_SCRATCHPAD()
    #define UNLOCK_SCRATCHPAD()
#endif

void CMemoryMgr::InjectHooks() {
    RH_ScopedClass(CMemoryMgr);
    RH_ScopedCategory("Memory");
    RH_ScopedInstall(InitScratchPad, 0x72F480);
    RH_ScopedOverloadedInstall(Malloc, "SizeHint", 0x72F420, void*(*)(uint32,uint32));
    RH_ScopedOverloadedInstall(Realloc, "MemorySizeHint", 0x72F440, uint8*(*)(void*,uint32,uint32));
    RH_ScopedOverloadedInstall(MallocAlign, "SizeAlignHint", 0x72F4C0, uint8*(*)(uint32,uint32,uint32));
    RH_ScopedInstall(FreeAlign, 0x72F4F0);

}

// 0x72F3B0
void CMemoryMgr::Init() {
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    // init main heap
    g_Heaps[HEAP_PRIMARY].Init(malloc(_MEMORYMGR_HEAP_SIZE), _MEMORYMGR_HEAP_SIZE, false);

    // critial sections initialization
    InitializeCriticalSection(&_gstrucMemoryHeapCS);
    InitializeCriticalSection(&_gstrucScratchPadCS);
    gmemoryHeapCS = &_gstrucMemoryHeapCS;
    gpScratchPadCS = &_gstrucScratchPadCS;

    // init second heap
    g_Heaps[HEAP_SECONDARY].Init(g_Heaps[HEAP_PRIMARY].Malloc(2100 * 1024), 2100 * 1024, true);

    // init other stuff
    CMemoryMgr::InitMemoryTracking(_MEMORYMGR_TRACKING_NUM_IDS);
    CMemoryMgr::InitScratchPad();
#endif
}

// 0x72F8F0
void CMemoryMgr::InitMemoryTracking(int32 numIDs) {
    m_memUsed = 0;
    PushMemId(MEM_GAME);
    PushMemId(MEM_INTERNAL_OR_SYSTEM);
    m_pMemUsedArray = (int32*)Malloc(sizeof(int32) * numIDs);
    m_pBlockArray   = (int32*)Malloc(sizeof(int32) * numIDs);
    PopMemId();

    // zero block arrays
    for (int32 i = 0; i < numIDs; ++i) {
        m_pMemUsedArray[i] = 0;
        m_pBlockArray[i] = 0;
    }
}

void CMemoryMgr::Shutdown() {
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    g_Heaps[HEAP_SECONDARY].PrintMemoryLeaks();
    g_Heaps[HEAP_PRIMARY].Free(g_Heaps[HEAP_SECONDARY]._pFirstBlock);
    g_Heaps[HEAP_PRIMARY].Shutdown();
#endif
}

/**
 * @static
 * debug only shit!
 */
void CMemoryMgr::SetHint(void* memory, const char* hint) {
    if (!memory) {
        return;
    }

    for (auto& heap : g_Heaps) {
        if (!heap.IsHeapPointer(memory))
            continue;

        if (HeapBlockDesc* desc = heap._FindBlockDesc(memory)) {
            desc->m_upDebugInfo = (const uint8*)hint;
#if defined MEMORY_MGR_USE_HEAP_FLAGS
            desc->m_Flags.NoDebugHint     = (hint == nullptr);
            desc->m_Flags.StringDebugInfo = (hint != nullptr);
#endif
            return;
        }
    }
    //_FATAL("Not a heap pointer!");
}

// 0x72F480
// Como el original: Init(mem=0xC8E0C8, size=0x4000, bInitSizes=false) de 0x72F180 en línea. Sólo se escriben los campos que el EXE escribe
// (no CMemoryHeap::Init de C++, que además pone a cero m_paCommonSizes y enlaza begin.m_Prev / end.m_Next): quedan intactos los bytes
// +0x1C, +0x30 y +0x38 del montículo, el m_nMemId / m_pDebugInfo del primer bloque y el m_pDebugInfo del último.
// Salida del EXE (72F1F3..31F): EAX = &m_FreeListBegin (0xC87BD8), ECX = primer bloque (0xC8E0C8), EDX = &m_FreeListEnd (0xC87BF0);
// ResidueRegsCpp es lo último que se ejecuta (ver contrib/audit_register_residue.py, objeto MemoryMgr).
#pragma runtime_checks("", off)
void CMemoryMgr::InitScratchPad() {
    constexpr uint32 kSize = MEMORY_MGR_PC_SCRATCH_SIZE;
    CMemoryHeap& heap = g_Heaps[HEAP_SCRATCH];

    auto* const first = reinterpret_cast<HeapFreeBlockDesc*>(&s_MemoryHeapBuffer[0]);
    auto* const last  = reinterpret_cast<HeapBlockDesc*>(&s_MemoryHeapBuffer[0] + (kSize - sizeof(HeapBlockDesc)));
    HeapFreeBlockDesc* const begin = &heap.m_FreeListBegin;
    HeapFreeBlockDesc* const end   = &heap.m_FreeListEnd;

    heap.m_FirstBlock = first;
    heap.m_LastBlock  = last;
    heap.m_nMemUsed   = 0;
    heap.m_bMovingRestricted = true;
    heap.m_nMemId     = -1;

    // bloque final (tamaño 0, en uso)
    last->m_bInUse    = true;
    last->m_nMemId    = -1;
    last->m_nSize     = 0;
    last->m_PrevBlock = first;

    // primer bloque libre: kSize - 0x10 - 0x10
    first->m_nSize     = int32(kSize - 2 * sizeof(HeapBlockDesc));
    first->m_bInUse    = false;
    first->m_PrevBlock = nullptr;

    // lista de libres: begin -> first -> end (begin.m_Prev y end.m_Next no se tocan)
    end->m_nSize = 0x7FFFFFFF;
    begin->m_Next = first;
    first->m_Next = end;
    first->m_Prev = begin;
    end->m_Prev   = first;

    (void)ResidueRegsCpp(reinterpret_cast<uint32>(first), reinterpret_cast<uint32>(end), reinterpret_cast<uint32>(begin));
}
#pragma runtime_checks("", restore)

void* CMemoryMgr::MallocFromScratchPad(uint32 size) {
    void* memory = g_Heaps[HEAP_SCRATCH].Malloc(size);
    RegisterMalloc(memory);
    return memory;
}

void* CMemoryMgr::MallocAlignFromScratchPad(uint32 size, uint32 align) {
    void* memory = MallocFromScratchPad(size + align);

    void* result = (void*)(uint32((uint8*)memory + align) & ~(align - 1));
    *((void**)result - 1) = memory;
    return result;
}

void CMemoryMgr::LockScratchPad() {
    LOCK_SCRATCHPAD();
}

void CMemoryMgr::ReleaseScratchPad() {
    UNLOCK_SCRATCHPAD();
}

// 0x72F770
void CMemoryMgr::RegisterMalloc(void* memory) {
    int16 memoryId = GET_HEAP_DESC(memory)->m_nMemId;
    int32 size     = GET_HEAP_DESC(memory)->m_nSize + sizeof(HeapBlockDesc);

    // update global info
    m_memUsed    += size;
    m_blocksUsed += 1;

    // update tracking info
    if (memoryId != -1) {
        m_pMemUsedArray[memoryId] += size;
        m_pBlockArray[memoryId]   += 1;
    }
}

// 0x72F7C0
void CMemoryMgr::RegisterFree(void* memory) {
    if (!memory) {
        return;
    }

    int16 memoryId = GET_HEAP_DESC(memory)->m_nMemId;
    int32 size     = GET_HEAP_DESC(memory)->m_nSize + sizeof(HeapBlockDesc);

    // update global info
    m_memUsed    -= size;
    m_blocksUsed -= 1;

    // update tracking info
    if (memoryId != -1) {
        m_pMemUsedArray[memoryId] -= size;
        m_pBlockArray[memoryId]   -= 1;
    }
}

uint32 CMemoryMgr::GetSizeOfHoles() {
    return g_Heaps[HEAP_PRIMARY].GetSizeOfHoles() + g_Heaps[HEAP_SECONDARY].GetSizeOfHoles();
}

uint32 CMemoryMgr::GetMemoryUsed(eMemoryId id) {
    assert(m_pMemUsedArray);
    return m_pMemUsedArray[id];
}

uint32 CMemoryMgr::GetBlocksUsed(eMemoryId id) {
    assert(m_pBlockArray);
    return m_pBlockArray[id];
}

uint32 CMemoryMgr::GetLargestFreeBlock() {
    return g_Heaps[HEAP_PRIMARY].GetLargestFreeBlock();
}

void* CMemoryMgr::InternalMalloc(uint32 size) {
    if (size <= 2048) {
        if (void* memory = g_Heaps[HEAP_SECONDARY].Malloc(size)) {
            return memory;
        }
    }
    return g_Heaps[HEAP_PRIMARY].Malloc(size);
}

void* CMemoryMgr::InternalRealloc(void* memory, uint32 size) {
    if (g_Heaps[HEAP_SCRATCH].IsHeapPointer(memory)) {
        return g_Heaps[HEAP_SCRATCH].Realloc(memory, size);
    }
    if (g_Heaps[HEAP_SECONDARY].IsHeapPointer(memory)) {
        if (size <= 2048)
            return g_Heaps[HEAP_SECONDARY].Realloc(memory, size);
    } else {
        if (size > 2048)
            return g_Heaps[HEAP_PRIMARY].Realloc(memory, size);
    }
    void* newMemory = InternalMalloc(size);
    if (newMemory) {
        uint32 nMinSize = (uint32)std::min(float(GET_HEAP_DESC(memory)->m_nSize), float(size));
        memcpy(newMemory, memory, nMinSize);
    }
    return newMemory;
}

namespace {
// CRT estática del EXE (no son funciones del juego reconstruidas): malloc 0x824257, realloc 0x824269 y free 0x82413F, todas __cdecl.
// Se llaman por dirección directamente desde cada función (sin ayudante intermedio) para que la dirección de retorno caiga dentro del cuerpo.
using ExeCrtMalloc  = void*  (__cdecl*)(uint32);
using ExeCrtRealloc = uint8* (__cdecl*)(void*, uint32);
using ExeCrtFree    = void   (__cdecl*)(void*);
}

// 0x72F420
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
void* CMemoryMgr::Malloc(uint32 size, uint32 nHint) {
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    _LOCK_MEMORYHEAP();
    void* memory = nullptr;
    int32 iAttempts = 0;
    while (true) {
        memory = InternalMalloc(size);
        if (memory) {
            break;
        }
        if (_gfnMemCompact) {
            _UNLOCK_MEMORYHEAP();
            // sub_2644FC(_nSize, ++i);
            _gfnMemCompact(size, ++iAttempts);
            _LOCK_MEMORYHEAP();
        } else {
            _FATAL("Failed to allocate %d bytes!", size);
        }
    }
    RegisterMalloc(memory);
    CMemoryHeap::SetDebugInfo(memory, nHint);
    _UNLOCK_MEMORYHEAP();
    return memory;
#else
    return plugin::CallAndReturn<void*, 0x72F420, size_t>(size);
    return ::malloc(size);
#endif
}
#else
void* CMemoryMgr::Malloc(uint32 size, uint32 nHint) {
    // 0x72F420: sólo reenvía el tamaño; el hint se ignora.
    return reinterpret_cast<ExeCrtMalloc>(uintptr_t{0x824257})(size);
}
#endif

// 0x72F430
void CMemoryMgr::Free(void* memory) {
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    LOCK_MEMORYHEAP();
    RegisterFree(memory);
    if (g_Heaps[HEAP_SCRATCH]._IsHeapPointer(memory)) {
        g_Heaps[HEAP_SCRATCH].Free(memory);
    } else if (g_Heaps[HEAP_SECONDARY]._IsHeapPointer(memory)) {
        g_Heaps[HEAP_SECONDARY].Free(memory);
    } else {
        g_Heaps[HEAP_PRIMARY].Free(memory);
    }
    UNLOCK_MEMORYHEAP();
#else
    return plugin::Call<0x72F430, void*>(memory);
    return ::free(memory);
#endif
}

// 0x72F440
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
uint8* CMemoryMgr::Realloc(void* memory, uint32 size, uint32 nHint) {
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    if (!memory) {
        return Malloc(size, nHint);
    }
    LOCK_MEMORYHEAP();
    RegisterFree(memory);
    PushMemId(GET_HEAP_DESC(memory)->m_nMemId);
    int32 iAttempts = 0;
    void* newMemory = nullptr;
    while (true) {
        newMemory = InternalRealloc(memory, size);
        if (newMemory) {
            break;
        }
        if (_gfnMemCompact) {
            // sub_2644FC(_nSize, ++i);
            _gfnMemCompact(size, ++iAttempts);
        } else {
            _FATAL("Failed to allocate %d bytes!", size);
        }
    }
    RegisterMalloc(newMemory);
    PopMemId();
    CMemoryHeap::SetDebugInfo(newMemory, nHint);
    UNLOCK_MEMORYHEAP();
    return newMemory;
#else
    return plugin::CallAndReturn<uint8*, 0x72F440, void*, size_t>(memory, size);
    return static_cast<uint8*>(::realloc(memory, size));
#endif
}
#else
uint8* CMemoryMgr::Realloc(void* memory, uint32 size, uint32 nHint) {
    // 0x72F440: realloc(memory, size) del CRT del EXE; el hint se ignora.
    return reinterpret_cast<ExeCrtRealloc>(uintptr_t{0x824269})(memory, size);
}
#endif

// 0x72F460
uint8* CMemoryMgr::Calloc(uint32 numObj, uint32 sizeObj, uint32 hint) {
    return plugin::CallAndReturn<uint8*, 0x72F460, uint32, uint32>(numObj, sizeObj);

#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    return Malloc(hint * size, hint);
#else
    return static_cast<uint8*>(::calloc(numObj, sizeObj));
#endif
}

// 0x72F4C0
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
uint8* CMemoryMgr::MallocAlign(uint32 size, uint32 align, uint32 nHint) {
    auto* memory = Malloc(size + align, nHint);

    auto* result = (void*)(uint32((uint8*)memory + align) & ~(align - 1));
    *((void**)result - 1) = memory;
    return static_cast<uint8*>(result);
}
#else
uint8* CMemoryMgr::MallocAlign(uint32 size, uint32 align, uint32 nHint) {
    // 0x72F4C0: malloc(size + align), resultado = (base + align) & ~(align - 1) con la base guardada en [resultado - 4].
    // Aritmética de 32 bits sin signo (add/lea con desborde); sin comprobar OOM ni alineamientos que no sean potencia de dos, como el EXE.
    const uint32 requested = size + align;
    uint8* const base      = static_cast<uint8*>(reinterpret_cast<ExeCrtMalloc>(uintptr_t{0x824257})(requested));
    const uint32 aligned   = (static_cast<uint32>(reinterpret_cast<uintptr_t>(base)) + align) & ~(align - 1u);
    auto* const  result    = reinterpret_cast<uint8*>(static_cast<uintptr_t>(aligned));
    *reinterpret_cast<void**>(result - sizeof(void*)) = base;
    return result;
}
#endif

#ifdef MEMORY_MGR_USE_MEMORY_HEAP
void CMemoryMgr::FreeAlign(void* memory) {
    return plugin::Call<0x72F4F0, void*>(memory);
    Free(*((void**)memory - 1));
}
#else
__declspec(noinline) void CMemoryMgr::FreeAlign(void* memory) {
    // 0x72F4F0: el EXE sobrescribe el argumento entrante [esp+4] con la base guardada en [memory - 4] y salta (tail call) a free del CRT.
    // `argument` es un acceso volátil al propio objeto parámetro (que MSVC x86 mantiene en su ranura de pila entrante al tomarle la
    // dirección): la escritura no se puede eliminar y free recibe la base leída de esa ranura. Hay que comprobar en el objeto compilado
    // que la ranura entrante cambia (lo exige DependenciesMemoryCpp16: argument_after_matches). No es una garantía del lenguaje.
    void* volatile* const argument = &memory;
    *argument = *reinterpret_cast<void**>(static_cast<uint8*>(*argument) - sizeof(void*));
    reinterpret_cast<ExeCrtFree>(uintptr_t{0x82413F})(*argument);
}
#endif

void CMemoryMgr::SetRestrictMemoryMove(bool restrict) {
    g_Heaps[HEAP_PRIMARY].m_bMovingRestricted   = restrict;
    g_Heaps[HEAP_SECONDARY].m_bMovingRestricted = restrict;
}

void CMemoryMgr::PushMemId(eMemoryId id) {
    m_idStack.push(m_id);
    m_id = id;
    g_Heaps[HEAP_PRIMARY].m_nMemId   = m_id;
    g_Heaps[HEAP_SECONDARY].m_nMemId = m_id;
}

void CMemoryMgr::PopMemId() {
    m_id = m_idStack.pop();
    g_Heaps[HEAP_PRIMARY].m_nMemId   = m_id;
    g_Heaps[HEAP_SECONDARY].m_nMemId = m_id;
}

void* CMemoryMgr::MoveMemory(void* memory) {
    LOCK_MEMORYHEAP();
    RegisterFree(memory);
    void* movedMem = nullptr;
    if (g_Heaps[HEAP_SECONDARY].IsHeapPointer(memory)) {
        movedMem = g_Heaps[HEAP_SECONDARY].MoveMemory(memory);
    } else {
        movedMem = g_Heaps[HEAP_PRIMARY].MoveMemory(memory);
    }
    RegisterMalloc(movedMem);
    UNLOCK_MEMORYHEAP();
    return movedMem;
}

void* CMemoryMgr::MoveMemory(void* memory, void** ppUnk, int32 iUnk) {
    LOCK_MEMORYHEAP();
    RegisterFree(memory);
    void* movedMem = nullptr;
    if (g_Heaps[HEAP_SECONDARY].IsHeapPointer(memory)) {
        movedMem = g_Heaps[HEAP_SECONDARY].MoveMemory(memory, ppUnk, iUnk);
    } else {
        movedMem = g_Heaps[HEAP_PRIMARY].MoveMemory(memory, ppUnk, iUnk);
    }
    RegisterMalloc(movedMem);
    UNLOCK_MEMORYHEAP();
    return movedMem;
}

void* CMemoryMgr::Malloc(uint32 size) {
    void* memory = Malloc(size, 0);
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    GET_HEAP_DESC(memory)->m_Flags.NoDebugHint = true;
#endif
    return memory;
}

void* CMemoryMgr::MallocAlign(uint32 size, uint32 align) {
    // PC 0x72F4C0 reads only size and align; the additional API hint is ignored.
    return plugin::CallAndReturn<void*, 0x72F4C0, uint32, uint32, uint32>(size, align, 0);

    void* memory = MallocAlign(size, align, 0);
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    GET_HEAP_DESC(memory)->m_Flags.NoDebugHint = true;
#endif
    return memory;
}

void* CMemoryMgr::Calloc(uint32 hint, uint32 size) {
    void* memory = Calloc(hint, size, 0);
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    GET_HEAP_DESC(memory)->m_Flags.NoDebugHint = true;
#endif
    return memory;
}

uint8* CMemoryMgr::Realloc(void* memory, uint32 size) {
    memory = Realloc(memory, size, 0);
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    GET_HEAP_DESC(memory)->m_Flags.NoDebugHint = true;
#endif
    return static_cast<uint8*>(memory);
}

#ifdef MEMORY_MGR_OVERLOAD_NEW_DELETE

void* operator new[](uint32 size) {
    void* memory = CMemoryMgr::Malloc(size);
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    GET_HEAP_DESC(memory)->m_Flags.AllocatedUsingNew = true;
    GET_HEAP_DESC(memory)->m_Flags.IsArray = true;
#endif
    return memory;
}

void operator delete[](void* memory) {
    if (!memory) {
        return;
    }
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    assert(GET_HEAP_DESC(memory)->m_Flags.AllocatedUsingNew);
    assert(GET_HEAP_DESC(memory)->m_Flags.IsArray);
#endif
    CMemoryMgr::Free(memory);
}

void* operator new(uint32 size) {
    void* memory = CMemoryMgr::Malloc(size);
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    GET_HEAP_DESC(memory)->m_Flags.AllocatedUsingNew = true;
#endif
    return memory;
}

void operator delete(void* memory) {
    if (!memory) {
        return;
    }
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    assert(GET_HEAP_DESC(memory)->m_Flags.AllocatedUsingNew);
    assert(!GET_HEAP_DESC(memory)->m_Flags.IsArray);
#endif
    CMemoryMgr::Free(memory);
}

#endif

#ifndef NOTSA_STANDALONE
void* operator new(size_t size) {
    return plugin::CallAndReturn<void*, 0x82119A, size_t>(size);
}

void* operator new[](size_t size) {
    return plugin::CallAndReturn<void*, 0x821195, size_t>(size);
}

void operator delete(void* p) {
    plugin::Call<0x8214BD, void*>(p);
}

void operator delete[](void* p) {
    plugin::Call<0x8213AE, void*>(p);
}
#endif
