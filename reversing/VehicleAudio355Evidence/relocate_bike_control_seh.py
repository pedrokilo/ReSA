from pathlib import Path
draft=Path('reversing/VehicleAudio355Evidence/Bike_ControlNativeDraft.txt')
s=draft.read_text()
metadata=r'''
// Own copies of VC7 FuncInfo and UnwindMap; no original handler/funclet pointers.
static const uintptr_t BikeControl355FrameDispatcher = 0x8215A0;
static const uintptr_t BikeControl355FireEventDestructor = 0x4B1170;
static void __declspec(naked) __cdecl BikeControl355UnwindFireEvent() {
    __asm {
        lea ecx,[ebp-108h]
        jmp dword ptr [BikeControl355FireEventDestructor]
    }
}
struct BikeControl355UnwindEntry { int32 previousState; void(__cdecl* action)(); };
static const BikeControl355UnwindEntry BikeControl355UnwindMap[] = {{-1,&BikeControl355UnwindFireEvent}};
struct BikeControl355FuncInfo {
    uint32 magic; int32 maxState; const BikeControl355UnwindEntry* unwind;
    int32 tryCount; const void* tries; int32 ipCount; const void* ipMap;
};
static const BikeControl355FuncInfo BikeControl355ExceptionInfo{0x19930520,1,BikeControl355UnwindMap,0,nullptr,0,nullptr};
static void __declspec(naked) __cdecl BikeControl355ExceptionHandler() {
    __asm {
        mov eax,offset BikeControl355ExceptionInfo
        jmp dword ptr [BikeControl355FrameDispatcher]
    }
}
static_assert(sizeof(BikeControl355FuncInfo)==28);
static_assert(sizeof(BikeControl355UnwindEntry)==8);
'''
s=s.replace('// The original SEH handler is a canonical dependency. Exceptional unwinding is not certified.',
 '// SEH handler, one unwind state and fire-event destructor funclet are relocated locally.')
s=s.replace('static const uintptr_t BikeControl355Calls[] = {',metadata+'\nstatic const uintptr_t BikeControl355Calls[] = {',1)
s=s.replace('push 8482EBh','push offset BikeControl355ExceptionHandler',1)
assert '8482EBh' not in s and '8A1068h' not in s and '8482E0h' not in s
Path('reversing/VehicleAudio355Evidence/Bike_ControlRelocatedAsmDraft.txt').write_text(s)
