#pragma once
// Temporary entry patch for synchronous engine-thread probes. Restore before
// forwarding to the original, then reinstall; never keep this in a normal ASI.
namespace ReversingProbe {
struct EntryPatch {
    uintptr_t address;
    std::array<uint8,5> saved{}, jump{0xE9};
    template<class F> EntryPatch(uintptr_t at, F replacement) : address(at) {
        std::memcpy(saved.data(),reinterpret_cast<void*>(at),5);
        const auto offset=uint32(reinterpret_cast<uintptr_t>(replacement)-at-5);
        std::memcpy(jump.data()+1,&offset,4); Enable(true);
    }
    void Enable(bool enabled) const {
        void* p=reinterpret_cast<void*>(address); DWORD old{},ignored{};
        if(!VirtualProtect(p,5,PAGE_EXECUTE_READWRITE,&old)) std::abort();
        std::memcpy(p,(enabled ? jump : saved).data(),5);
        VirtualProtect(p,5,old,&ignored); FlushInstructionCache(GetCurrentProcess(),p,5);
    }
    ~EntryPatch() { Enable(false); }
};
// true si la entrada del EXE es un salto (hook instalado) hacia fuera de la imagen del EXE.
inline bool JumpsToDll(uintptr_t at) {
    if(*reinterpret_cast<const uint8*>(at)!=0xE9) return false;
    const auto target=at+5+*reinterpret_cast<const int32*>(at+1);
    return target<0x400000||target>=0x1000000;
}
inline std::vector<uint8> Bytes(const void* ptr,size_t n) {
    const auto* p=static_cast<const uint8*>(ptr); return {p,p+n};
}
}
