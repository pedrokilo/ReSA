#pragma once
// Banco dentro del motor: Common::TransformVectors(RwV3d*, int32, const CSimpleTransform&, const RwV3d*)
// (EXE 0x54EE30, cdecl; fuente source/game_sa/common.cpp). La funcion NO tiene hook: el candidato es la
// sobrecarga de la DLL llamada directamente por su simbolo y el original es el codigo del EXE llamado por su direccion,
// ambos con los mismos argumentos sobre copias identicas de la entrada. Sin colaboradores simulados.
#include <fstream>
#include <float.h>
#include <bit>
#include "common.h"
#include "SimpleTransform.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace CppTransformVectors1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{
    {"Common","TransformVectors(CSimpleTransform)",0x54EE30,0,-1,3072},
}};

// Un unico buffer de 48 ranuras RwV3d (12 bytes) con guardas de 16 bytes; entrada y salida viven dentro (solapes posibles).
constexpr unsigned kSlots = 48;
constexpr unsigned kBuf = 16 + kSlots*12 + 16;
alignas(16) inline std::array<uint8,kBuf> buf{};
alignas(16) inline std::array<uint8,16+16+16> simple{};   // CSimpleTransform (posicion + rumbo) con guardas

struct ABIResult { bool stack, registers; };
inline ABIResult CallRaw(uintptr_t callAddress, const std::array<uint32,4>& args, unsigned arity) {
    const auto* argPointer = args.data();
    const unsigned argBytes = arity*4;
    uintptr_t stackBefore{},stackAfter{},ebxBefore{},ebxAfter{},esiBefore{},esiAfter{},ediBefore{},ediAfter{},ebpBefore{},ebpAfter{};
    __asm {
        mov stackBefore,esp
        mov ebxBefore,ebx
        mov esiBefore,esi
        mov ediBefore,edi
        mov ebpBefore,ebp
        mov eax,callAddress
        mov ecx,argPointer
        mov edx,arity
    pushArgument:
        push dword ptr [ecx + edx*4 - 4]
        dec edx
        jnz pushArgument
        call eax
        add esp,argBytes
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
    }
    return {stackBefore==stackAfter, ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter};
}
inline void PutFloat(uint8* p,float value) { std::memcpy(p,&value,4); }
inline void PutU32(uint8* p,uint32 value) { std::memcpy(p,&value,4); }
inline std::string Hex(const uint8* p,size_t n) {
    static const char* d="0123456789abcdef"; std::string s; s.reserve(n*2);
    for(size_t i=0;i<n;++i){s.push_back(d[p[i]>>4]);s.push_back(d[p[i]&15]);} return s;
}
// RNG propio y determinista (xorshift32): no depende del RNG del juego.
struct Rng {
    uint32 s;
    explicit Rng(uint32 seed):s(seed*2654435761u+0x9E3779B9u){ if(!s)s=1; for(int i=0;i<4;++i)next(); }
    uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    // flotante finito en [-scale, scale), a partir de un entero de 24 bits (exacto en float)
    float value(float scale){ return (float(int32(next()>>8)-(1<<23))/float(1<<23))*scale; }
};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpptransformvectors1-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"cpptransformvectors1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cpptransformvectors1");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;

    // Sobrecarga de la DLL por su simbolo (la otra sobrecarga recibe const CMatrix&).
    using SimpleFn = void(*)(RwV3d*,int32,const CSimpleTransform&,const RwV3d*);
    const uintptr_t dllEntry = std::bit_cast<uintptr_t>(static_cast<SimpleFn>(&TransformVectors));
    uintptr_t dllBody = dllEntry;
    for(int i=0;i<16&&*reinterpret_cast<uint8*>(dllBody)==0xE9;++i) dllBody += 5 + *reinterpret_cast<int32*>(dllBody+1);
    const uintptr_t exeEntry = fns[0].address;
    const bool exeUnhooked = !ReversingProbe::JumpsToDll(exeEntry);        // sin hook: se ejecuta el codigo del EXE
    const bool dllOutside = dllEntry>=0x1000000 && dllBody>=0x1000000;      // la sobrecarga esta en la DLL, no en el EXE
    const bool direct = exeUnhooked && dllOutside && dllEntry!=exeEntry;

    const std::array<uintptr_t,3> watched{exeEntry,dllEntry,dllBody};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    for(auto at:watched) entries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    const auto savedBuf=buf;
    const auto savedSimple=simple;
    const unsigned savedFP=_controlfp(0,0);
    auto restore=[&] { buf=savedBuf; simple=savedSimple; _controlfp(savedFP,_MCW_PC|_MCW_RC); };
    auto restored=[&] {
        bool ok = buf==savedBuf && simple==savedSimple && _controlfp(0,0)==savedFP;
        for(auto& [at,bytes]:entries) ok &= bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    };
    Write({{"event","begin"},{"functions",1},{"pairs",3072},{"registered_hook",false},{"direct_dll_call",direct},
           {"exe_unhooked",exeUnhooked},{"dll_entry",dllEntry},{"dll_body",dllBody},{"exe_entry",exeEntry},
           {"collaborators_stubbed",false},{"fp_control",savedFP}});
    if(!direct) {
        restore(); const bool stateOK=restored();
        Write({{"event","error"},{"reason","no se puede llamar la sobrecarga de la DLL directamente o el EXE esta hookeado"},{"state_restored",stateOK}});
        return;
    }

    const std::array<int,6> counts{0,1,2,3,7,16};
    // NaN, sNaN e Inf tambien en x/y (cargas utiles y signos distintos): la x87 del EXE y el SSE de la DLL propagan distinto cuando hay dos NaN.
    const std::array<uint32,8> xyPatterns{0x7FC00000u,0xFFC00000u,0x7FC12345u,0xFFC0ABCDu,0x7FA00001u,0x7F800000u,0xFF800000u,0x7FA55555u};
    const std::array<uint32,7> zPatterns{0x7FA00001u,0xFFA55555u,0x7FC00000u,0xFFC12345u,0x7F800001u,0x7F800000u,0x80000000u};
    const char* const modeNames[4]{"distinct","alias","overlap_plus1","overlap_minus1"};
    unsigned pair=0; bool passed=true, allRestored=true;
    for(size_t f=0;f<fns.size();++f) {
        int failures=0;
        const std::string name=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<3072;++fixture,++pair) {
            const unsigned n=unsigned(counts[fixture%6]), mode=(fixture/6)%4, profile=(fixture/24)%64, block=fixture/1536;
            // Ranuras (indices de RwV3d): salida distinta 4.., en sitio 16, solape +1/-1 desplazado un RwV3d respecto a la entrada.
            const unsigned inSlot  = mode==0 ? 24u : 16u;
            const unsigned outSlot = mode==0 ? 4u : mode==1 ? 16u : mode==2 ? 17u : 15u;
            Rng rng(fixture+1);
            // Rumbo: bloque 0 = rejilla de multiplos de 0.125 en [-4,3.875]; bloque 1 = finitos aleatorios.
            const float heading = block==0 ? (-4.0f+0.125f*float(profile)) : rng.value(fixture%8==7 ? 100.0f : 10.0f);
            const float posScale = (fixture%3==0) ? 1.0f : (fixture%3==1) ? 50.0f : 1000.0f;
            const float vecScale = (fixture%5<2) ? 1.0f : (fixture%5<4) ? 100.0f : 1000.0f;
            const bool nanZ = fixture%3==0;
            const bool nanXY = fixture%7==3;   // 1/7 de los fixtures: x, y o ambos con NaN/Inf en algunos vectores
            auto build=[&] {
                // Contenido identico para original y candidato: relleno determinista distinto por fixture.
                for(unsigned k=0;k<kBuf;++k) buf[k]=uint8(0xC0u^(k*37u+fixture*11u));
                Rng r2(fixture+0x1000u);
                auto* base=buf.data()+16;
                for(unsigned i=0;i<n;++i) {
                    auto* v=base+(inSlot+i)*12;
                    PutFloat(v,r2.value(vecScale)); PutFloat(v+4,r2.value(vecScale));
                    if(nanXY && i%4!=0) {
                        if(i%4!=2) PutU32(v,xyPatterns[(fixture+i)%xyPatterns.size()]);
                        if(i%4!=1) PutU32(v+4,xyPatterns[(fixture+3*i+1)%xyPatterns.size()]);
                    }
                    if(nanZ && i%3==0) PutU32(v+8,zPatterns[(fixture+i)%zPatterns.size()]);
                    else PutFloat(v+8,r2.value(vecScale));
                }
                simple.fill(0x5A);
                PutFloat(simple.data()+16,r2.value(posScale)); PutFloat(simple.data()+20,r2.value(posScale)); PutFloat(simple.data()+24,r2.value(posScale));
                PutFloat(simple.data()+28,heading);
            };
            auto run=[&](bool candidate) -> json {
                restore();
                build();
                auto* base=buf.data()+16;
                std::array<uint32,4> args{uint32(uintptr_t(base+outSlot*12)),uint32(n),uint32(uintptr_t(simple.data()+16)),uint32(uintptr_t(base+inSlot*12))};
                const auto beforeBuf=buf;
                const auto beforeSimple=simple;
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                const auto abi=CallRaw(candidate ? dllEntry : exeEntry,args,4);
                const unsigned fpAfter=_controlfp(0,0);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                // Nada fuera de la region de salida cambia; la region de salida se enmascara en ambas copias.
                auto maskedBefore=beforeBuf, maskedAfter=buf;
                for(unsigned off=0;off<n*12;++off) { maskedBefore[16+outSlot*12+off]=0; maskedAfter[16+outSlot*12+off]=0; }
                const bool protectedOK = maskedBefore==maskedAfter && simple==beforeSimple
                    && (fpAfter&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);
                const bool routeOK = candidate ? (dllEntry>=0x1000000) : !ReversingProbe::JumpsToDll(exeEntry);
                json result{{"route_ok",routeOK},{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},
                            {"protected_state_unchanged",protectedOK},{"buf",Hex(buf.data(),buf.size())},
                            {"simple",Hex(simple.data(),simple.size())}};
                restore();
                return result;
            };
            const auto original=run(false), candidate=run(true);
            const bool stateOK=restored(), equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true;
            failures+=!accepted; passed&=accepted; allRestored&=stateOK;
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},
                   {"fixture",{{"n",n},{"mode",modeNames[mode]},{"in_slot",inSlot},{"out_slot",outSlot},{"heading",heading},{"nan_z",nanZ},{"nan_xy",nanXY}}},
                   {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",3072},{"failures",failures}});
    }
    restore(); allRestored&=restored();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
