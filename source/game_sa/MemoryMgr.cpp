#include "StdInc.h"

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
// Como el original: Init(mem=0xC8E0C8, size=0x4000, bInitSizes=false).
// Se conserva el orden de escrituras y los bytes de cabeceras que el EXE no inicializa.
__declspec(naked) void CMemoryMgr::InitScratchPad() {
    __asm {
        mov eax,3FF0h
        push ebx
        push esi
        mov esi,0C87BD0h
        mov ecx,0C8E0C8h
        mov dword ptr [esi],ecx
        add ecx,eax
        mov dword ptr [esi+4],ecx
        xor ebx,ebx
        mov dword ptr [esi+40h],ebx
        mov dl,1
        mov byte ptr [esi+3Ch],dl
        push edi
        or edi,0FFFFFFFFh
        mov dword ptr [esi+44h],edi
        mov byte ptr [ecx+4],dl
        mov ecx,dword ptr [esi+4]
        mov word ptr [ecx+6],di
        mov edx,dword ptr [esi+4]
        mov dword ptr [edx],ebx
        mov ecx,dword ptr [esi+4]
        mov edx,dword ptr [esi]
        mov dword ptr [ecx+0Ch],edx
        mov ecx,dword ptr [esi]
        add eax,0FFFFFFF0h
        mov dword ptr [ecx],eax
        mov edx,dword ptr [esi]
        mov byte ptr [edx+4],bl
        mov eax,dword ptr [esi]
        mov dword ptr [eax+0Ch],ebx
        lea eax,[esi+8]
        mov dword ptr [esi+34h],eax
        mov dword ptr [esi+20h],7FFFFFFFh
        lea ecx,[esi+20h]
        mov dword ptr [esi+18h],ecx
        mov edx,dword ptr [eax+10h]
        mov ecx,dword ptr [esi]
        mov dword ptr [ecx+10h],edx
        mov edx,dword ptr [eax+10h]
        mov dword ptr [edx+14h],ecx
        mov dword ptr [ecx+14h],eax
        mov dword ptr [eax+10h],ecx
        pop edi
        pop esi
        pop ebx
        ret
    }
}

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
__declspec(naked) void* __cdecl Memory9NativeMalloc(uint32) {
    __asm {
        push 00824257h
        ret
    }
}
__declspec(naked) uint8* __cdecl Memory9NativeRealloc(void*, uint32) {
    __asm {
        push 00824269h
        ret
    }
}
__declspec(naked) void __cdecl Memory9NativeFree(void*) {
    __asm {
        push 0082413Fh
        ret
    }
}
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
__declspec(naked) void* CMemoryMgr::Malloc(uint32 size, uint32 nHint) {
    __asm {
        mov eax,[esp+4]
        push eax
        call Memory9NativeMalloc
        add esp,4
        ret
    }
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
__declspec(naked) uint8* CMemoryMgr::Realloc(void* memory, uint32 size, uint32 nHint) {
    __asm {
        mov eax,[esp+8]
        mov ecx,[esp+4]
        push eax
        push ecx
        call Memory9NativeRealloc
        add esp,8
        ret
    }
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
__declspec(naked) uint8* CMemoryMgr::MallocAlign(uint32 size, uint32 align, uint32 nHint) {
    __asm {
        mov eax,[esp+4]
        push esi
        mov esi,[esp+0Ch]
        add eax,esi
        push eax
        call Memory9NativeMalloc
        mov ecx,eax
        lea eax,[ecx+esi]
        add esp,4
        dec esi
        not esi
        and eax,esi
        mov [eax-4],ecx
        pop esi
        ret
    }
}
#endif

#ifdef MEMORY_MGR_USE_MEMORY_HEAP
void CMemoryMgr::FreeAlign(void* memory) {
    return plugin::Call<0x72F4F0, void*>(memory);
    Free(*((void**)memory - 1));
}
#else
__declspec(naked) void CMemoryMgr::FreeAlign(void* memory) {
    __asm {
        mov eax,[esp+4]
        mov ecx,[eax-4]
        mov [esp+4],ecx
        jmp Memory9NativeFree
    }
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
