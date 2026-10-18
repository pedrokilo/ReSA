// 0x8052F0: `mov eax, [0xC9A6B0] / ret`. El montón global de Rx es una variable del EXE (se lee por dirección, sin copiarla).
RxHeap* RxHeapGetGlobalHeap() {
    return StaticRef<RxHeap*>((0xC9A6B0 + gDelta));
}

// 0x807720: si numFrames (+4) no es 0 libera el vector `frames` (+0) con la función rwfree del motor ([RwEngineInstance + 0x138], cdecl, un argumento)
// y devuelve la propia lista. El EXE comprueba numFrames, no el puntero.
rwFrameList* _rwFrameListDeinitialize(rwFrameList* frameList) {
    if (frameList->numFrames) {
        RwFree(frameList->frames);
    }
    return frameList;
}

// 0x820C90: la extensión D3D9 del raster está en raster + _RwD3D9RasterExtOffset (0xB4E9E0); su byte +0xA guarda en el nibble alto el formato comprimido.
// Devuelve 1 si ese nibble no es cero (neg al / sbb eax,eax / neg eax) y 0 si lo es. Aritmética de 32 bits con envoltura, como el EXE.
RwBool RwD3D9RasterIsCompressed(const RwRaster* raster) {
    const uint32 address = static_cast<uint32>(reinterpret_cast<uintptr_t>(raster)) + static_cast<uint32>(StaticRef<RwInt32>((0xB4E9E0 + gDelta))) + 0xAu;
    return (*reinterpret_cast<const uint8*>(static_cast<uintptr_t>(address)) & 0xF0u) != 0;
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

// 0x809400: resta `amount` a numElementsFilled (+4), con envoltura de 32 bits.
void _rwSListDestroyEndEntries(RwSList* sList, RwInt32 amount) {
    sList->numElementsFilled = static_cast<RwInt32>(static_cast<uint32>(sList->numElementsFilled) - static_cast<uint32>(amount));
}

// 0x8094B0: numElementsFilled (+4).
RwInt32 _rwSListGetNumEntries(const RwSList* sList) {
    return sList->numElementsFilled;
}

// 0x809510: listElements + entrySize * entry (imul de 32 bits con envoltura).
void* _rwSListGetEntry(RwSList* sList, RwInt32 entry) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(sList->listElements) + static_cast<uint32>(sList->entrySize) * static_cast<uint32>(entry));
}

// 0x809530: listElements (+0).
void* _rwSListGetBegin(RwSList* sList) {
    return sList->listElements;
}

// 0x809540: listElements + entrySize * numElementsFilled (imul de 32 bits con envoltura).
void* _rwSListGetEnd(RwSList* sList) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(sList->listElements) + static_cast<uint32>(sList->entrySize) * static_cast<uint32>(sList->numElementsFilled));
}

// 0x80A440: el EXE solo ejecuta `ret` (1 byte, seguido de 15 bytes de relleno 0x90 hasta 0x80A450).
void _rwStringClose() {
}

// 0x80A470: una cadena nula se sustituye por la cadena vacía del EXE (0x884F6A); la longitud sale de rwstrlen del motor
// ([RwEngineInstance + 0x120], cdecl) y el tamaño en el flujo es (longitud + 4) con los dos bits bajos a cero (`add eax, 4` / `and al, 0FCh`).
RwUInt32 _rwStringStreamGetSize(const RwChar* string) {
    if (!string) {
        string = reinterpret_cast<const RwChar*>((0x884F6A + gDelta));
    }
    const uint32 length = static_cast<uint32>(RWSRCGLOBAL(stringFuncs).vecStrlen(string));
    return (length + 4u) & ~3u;
}

// 0x821D1E: atexit(func) = (_onexit(func) == NULL) ? -1 : 0. _onexit (0x821CE6) guarda func en la tabla de la CRT del EXE y devuelve func (o NULL si no cabe).
static int __cdecl CrtAtexit(void(__cdecl* func)()) {
    using OnexitFn = void*(__cdecl*)(void*);
    return reinterpret_cast<OnexitFn>((0x821CE6 + gDelta))(reinterpret_cast<void*>(func)) ? 0 : -1;
}

// 0x823FD1: _lock_file2(index, stream). Los flujos 0..19 usan el cerrojo de tabla 16 + index (_lock, 0x82ADBE); los demás tienen su propia sección crítica
// embebida en el FILE ampliado (stream + 0x20) que se entra con EnterCriticalSection (entrada de importación 0x8580EC). Comparación con signo (jge).
static void __cdecl CrtLockFile2(int index, void* stream) {
    if (index < 20) {
        reinterpret_cast<void(__cdecl*)(int)>((0x82ADBE + gDelta))(index + 16);
        return;
    }
    (*reinterpret_cast<void(__stdcall**)(void*)>((0x8580EC + gDelta)))(static_cast<uint8*>(stream) + 0x20);
}

// 0x824023: _unlock_file2(index, stream), simétrica de la anterior: _unlock (0x82AD12) con 16 + index, o LeaveCriticalSection (0x8580E8) sobre stream + 0x20.
static void __cdecl CrtUnlockFile2(int index, void* stream) {
    if (index < 20) {
        reinterpret_cast<void(__cdecl*)(int)>((0x82AD12 + gDelta))(index + 16);
        return;
    }
    (*reinterpret_cast<void(__stdcall**)(void*)>((0x8580E8 + gDelta)))(static_cast<uint8*>(stream) + 0x20);
}

// 0x824257: malloc(size) de la CRT del EXE (no es CMemoryMgr::Malloc, que es 0x72F420 y solo reenvía aquí) = _nh_malloc(size, _newmode) con _newmode en 0xC9ADCC.
// _nh_malloc (0x82422B) es el que toca el montón y el manejador de falta de memoria de la CRT del EXE.
static void* __cdecl CrtMalloc(uint32 size) {
    using NhMallocFn = void*(__cdecl*)(uint32, int);
    return reinterpret_cast<NhMallocFn>((0x82422B + gDelta))(size, StaticRef<int>((0xC9ADCC + gDelta)));
}

// 0x82B181: _errno() = &_getptd()->_terrno (el campo está en el desplazamiento 8 de _tiddata). _getptd (0x827B3D) es cdecl sin argumentos y devuelve el bloque del hilo en EAX.
static int* __cdecl CrtErrno() {
    using GetPtdFn = uint8*(__cdecl*)();
    return reinterpret_cast<int*>(reinterpret_cast<GetPtdFn>((0x827B3D + gDelta))() + 8);
}

