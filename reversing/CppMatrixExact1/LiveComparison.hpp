#pragma once
// Banco dentro del motor: las 10 funciones de CMatrix (source/game_sa/Core/Matrix.cpp) que eran __declspec(naked) con __asm y ahora son C++ con ayudantes de X87Math:
// SetRotateXOnly/YOnly/ZOnly (0x59AFA0/0x59AFE0/0x59B020), SetRotate(x,y,z) (0x59B120), RotateX/Y/Z (0x59B1E0/0x59B2C0/0x59B390), Reorthogonalise (0x59B6A0),
// ConvertToEulerAngles (0x59A840) y ConvertFromEulerAngles (0x59AA40). Las funciones NO tienen hook (InjectHooksMain desactiva CMatrix): el original es el codigo
// del EXE llamado por su direccion (thiscall) y el candidato es el metodo de la DLL llamado por su simbolo, ambos con los mismos argumentos sobre copias identicas.
// Sin colaboradores simulados (aritmetica pura). Modelo: CppTransformVectors1 y FixCore.
//
// Fixtures con estres: angulos y elementos con NaN (distintos signos/cargas, senalizantes), +-inf, +-0, subnormales, +-1e-30/1e30, FLT_MAX, angulos > 2^63 (fsin/fcos
// dejan el operando), multiplos de pi/2; en ConvertToEuler: las 32 combinaciones de banderas (mas bits altos basura) con la rama de bloqueo cardan (s <= 1,9073486e-6)
// y el umbral exacto; punteros de salida distintos, solapados entre si y dentro de la propia matriz.
#include <fstream>
#include <float.h>
#include <bit>
#include <array>
#include "Matrix.h"
#include "../ProbeRuntime.hpp"

namespace CppMatrixExact1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
// slot -2 = sin hook (EXE por direccion frente al simbolo de la DLL)
inline const std::array<Fn,10> fns{{
    {"CMatrix","SetRotateXOnly",0x59AFA0,0,-2,1024},
    {"CMatrix","SetRotateYOnly",0x59AFE0,0,-2,1024},
    {"CMatrix","SetRotateZOnly",0x59B020,0,-2,1024},
    {"CMatrix","SetRotate",0x59B120,0,-2,2048},
    {"CMatrix","RotateX",0x59B1E0,0,-2,2048},
    {"CMatrix","RotateY",0x59B2C0,0,-2,2048},
    {"CMatrix","RotateZ",0x59B390,0,-2,2048},
    {"CMatrix","Reorthogonalise",0x59B6A0,0,-2,2048},
    {"CMatrix","ConvertToEulerAngles",0x59A840,0,-2,4096},
    {"CMatrix","ConvertFromEulerAngles",0x59AA40,0,-2,4096},
}};
constexpr unsigned kTotalPairs = 1024*3 + 2048*5 + 4096*2;

// Un unico buffer: guarda(16) + CMatrix(0x48) + guarda(16) + salidas de ConvertToEuler (16 floats) + guarda(16).
constexpr unsigned kMatOff = 16, kMatSize = 0x48, kOutOff = kMatOff + kMatSize + 16, kOutSize = 64, kBuf = kOutOff + kOutSize + 16;
alignas(16) inline std::array<uint8,kBuf> buf{};

struct ABIResult { bool stack, registers, fpu; };
// thiscall: argumentos de pila (de derecha a izquierda), ECX = this; el callee limpia la pila (ret N). Se comprueban ESP, EBX/ESI/EDI/EBP y la cima de la pila x87.
inline ABIResult CallThis(uintptr_t callAddress, void* self, const std::array<uint32,4>& args, unsigned arity) {
    const auto* argPointer = args.data();
    uintptr_t stackBefore{},stackAfter{},ebxBefore{},ebxAfter{},esiBefore{},esiAfter{},ediBefore{},ediAfter{},ebpBefore{},ebpAfter{};
    unsigned short fpuBefore{},fpuAfter{};
    __asm {
        fnstsw ax
        mov fpuBefore,ax
        mov stackBefore,esp
        mov ebxBefore,ebx
        mov esiBefore,esi
        mov ediBefore,edi
        mov ebpBefore,ebp
        mov edx,argPointer
        mov eax,arity
    pushArgument:
        test eax,eax
        jz argumentsPushed
        push dword ptr [edx + eax*4 - 4]
        dec eax
        jmp pushArgument
    argumentsPushed:
        mov ecx,self
        mov eax,callAddress
        call eax
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
        fnstsw ax
        mov fpuAfter,ax
    }
    return {stackBefore==stackAfter, ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter, ((fpuBefore^fpuAfter)&0x3800)==0};
}
inline std::string Hex(const uint8* p,size_t n) {
    static const char* d="0123456789abcdef"; std::string s; s.reserve(n*2);
    for(size_t i=0;i<n;++i){s.push_back(d[p[i]>>4]);s.push_back(d[p[i]&15]);} return s;
}
inline float FromBits(uint32 b) { return std::bit_cast<float>(b); }
inline uint32 ToBits(float f) { return std::bit_cast<uint32>(f); }

// RNG propio y determinista (xorshift32): no depende del RNG del juego.
struct Rng {
    uint32 s;
    explicit Rng(uint32 seed):s(seed*2654435761u+0x9E3779B9u){ if(!s)s=1; for(int i=0;i<4;++i)next(); }
    uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000001u)/1000000.f; }
};
// Valores de estres (patrones de bits): 0, -0, +-1, subnormales, minimo normal, FLT_MAX, +-inf, NaN (distinto signo y carga, senalizantes), 1e-20, 2^64, 1e30, 1e-30, pi/2.
constexpr std::array<uint32,24> kSpecial{
    0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x00000001u,0x80000001u,0x007FFFFFu,0x00800000u,0x80800000u,
    0x7F7FFFFFu,0xFF7FFFFFu,0x7F800000u,0xFF800000u,0x7FC00000u,0xFFC00000u,0x7FC12345u,0xFFC0ABCDu,0x7FA00001u,0xFFA55555u,
    0x1E3CE508u,0x5F800000u,0x7149F2CAu,0x0DA24260u,0x3FC90FDBu};
inline bool IsSpecialValue(float v) {
    const auto b=ToBits(v)&0x7FFFFFFFu;
    return b!=0 && ((b>=0x7F800000u) || (b<0x00800000u) || b>=0x5F000000u || b<=0x1E000000u);   // inf/NaN, subnormales, >= 2^63, <= ~1e-20 (el cero no cuenta)
}
// angulo: mayoria finita razonable; el resto estres (>2^63: fsin/fcos dejan el operando; NaN/inf; multiplos de pi/2; subnormales)
inline float Angle(Rng& r,bool stress) {
    const uint32 k=r.next()%16;
    if(!stress || k<9) {
        switch(r.next()%6){ case 0:return r.uni(-3.2f,3.2f);case 1:return r.uni(-100.f,100.f);case 2:return r.uni(-1e5f,1e5f);
            case 3:return float(int(r.next()%17)-8)*FromBits(0x3FC90FDBu);case 4:return r.next()%2 ? 0.f : -0.f;default:return r.uni(-7.f,7.f);} }
    switch(k){
        case 9: return FromBits(kSpecial[r.next()%kSpecial.size()]);
        case 10: return FromBits(0x5F000000u+(r.next()%3)-1u) * (r.next()%2 ? 1.f : -1.f);   // 2^63 +- 1 ulp
        case 11: return FromBits(0x5F000000u+(r.next()%0x20800000u)) * (r.next()%2 ? 1.f : -1.f); // 2^63 .. FLT_MAX
        case 12: return FromBits(r.next()%0x00800000u) * (r.next()%2 ? 1.f : -1.f);               // subnormal
        case 13: return r.uni(-1e-3f,1e-3f);
        case 14: return FromBits(0x33800000u+(r.next()%0x0C800000u)) * (r.next()%2 ? 1.f : -1.f); // 2^-24 .. ~2
        default: return r.uni(-4.f,4.f);
    }
}
// elemento de matriz: 0, -0, +-1, [-2,2]; con `stress` hay NaN/inf/subnormales/enormes; `tiny` = valores pequenos para forzar el bloqueo cardan
inline float Elem(Rng& r,bool stress) {
    if(stress && r.next()%3==0) return FromBits(kSpecial[r.next()%kSpecial.size()]);
    switch(r.next()%8){case 0:return 0.f;case 1:return -0.f;case 2:return 1.f;case 3:return -1.f;default:return r.uni(-2.f,2.f);}
}
inline float Tiny(Rng& r) {
    switch(r.next()%8){ case 0:return 0.f; case 1:return -0.f; case 2:return 1e-7f; case 3:return -1e-7f; case 4:return 1.2e-6f; case 5:return -1.2e-6f;
        case 6:return FromBits(0x36000000u);                                  // umbral exacto (s == 1,9073486e-6: la rama de bloqueo cardan)
        default:return FromBits(0x36000001u-(r.next()%2)); }                 // y sus vecinos inmediatos
}
constexpr std::array<uint8,4> kIdx1{0,1,2,0}, kIdx2{1,2,0,1};   // 0x866D9C y 0x866D94 del EXE

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppmatrixexact1-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"cppmatrixexact1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cppmatrixexact1");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;

    // Metodos de la DLL por su simbolo
    using std::bit_cast;
    std::array<uintptr_t,10> dll{};
    dll[0]=bit_cast<uintptr_t>(&CMatrix::SetRotateXOnly); dll[1]=bit_cast<uintptr_t>(&CMatrix::SetRotateYOnly); dll[2]=bit_cast<uintptr_t>(&CMatrix::SetRotateZOnly);
    dll[3]=bit_cast<uintptr_t>(static_cast<void(CMatrix::*)(float,float,float)>(&CMatrix::SetRotate));
    dll[4]=bit_cast<uintptr_t>(&CMatrix::RotateX); dll[5]=bit_cast<uintptr_t>(&CMatrix::RotateY); dll[6]=bit_cast<uintptr_t>(&CMatrix::RotateZ);
    dll[7]=bit_cast<uintptr_t>(&CMatrix::Reorthogonalise);
    dll[8]=bit_cast<uintptr_t>(&CMatrix::ConvertToEulerAngles); dll[9]=bit_cast<uintptr_t>(&CMatrix::ConvertFromEulerAngles);
    bool direct=true, exeUnhooked=true;
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    for(size_t f=0;f<fns.size();++f) {
        direct &= dll[f]>=0x1000000 && dll[f]!=fns[f].address;      // el metodo esta en la DLL, no en el EXE
        exeUnhooked &= !ReversingProbe::JumpsToDll(fns[f].address); // sin hook: se ejecuta el codigo del EXE
        entries.push_back({fns[f].address,ReversingProbe::Bytes(reinterpret_cast<void*>(fns[f].address),16)});
        entries.push_back({dll[f],ReversingProbe::Bytes(reinterpret_cast<void*>(dll[f]),16)});
    }
    direct &= exeUnhooked;
    const auto savedBuf=buf;
    const unsigned savedFP=_controlfp(0,0);
    auto restore=[&] { buf=savedBuf; _controlfp(savedFP,_MCW_PC|_MCW_RC); };
    auto restored=[&] {
        bool ok = buf==savedBuf && _controlfp(0,0)==savedFP;
        for(auto& [at,bytes]:entries) ok &= bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    };
    Write({{"event","begin"},{"functions",fns.size()},{"pairs",kTotalPairs},{"registered_hook",false},{"direct_dll_call",direct},{"exe_unhooked",exeUnhooked},
           {"collaborators_stubbed",false},{"fp_control",savedFP}});
    if(!direct) {
        restore(); const bool stateOK=restored();
        Write({{"event","error"},{"reason","no se pueden llamar los metodos de la DLL directamente o el EXE esta hookeado"},{"state_restored",stateOK}});
        return;
    }

    unsigned pair=0; bool passed=true, allRestored=true;
    for(size_t f=0;f<fns.size();++f) {
        int failures=0; const std::string name=std::string(fns[f].cls)+"::"+fns[f].name;
        for(int fixture=0;fixture<fns[f].cases;++fixture,++pair) {
            Rng rng(uint32(fixture)+1u+uint32(f)*0x10000u);
            const bool stress = fixture%5==4;                 // 1/5 de los casos con NaN/inf/subnormales/enormes
            const bool nanMatrix = stress && (fixture%10==4);  // la mitad de ellos con elementos de matriz excepcionales
            unsigned arity=1; std::array<uint32,4> args{}; uint32 flags=0; unsigned mode=0; bool gimbal=false, special=false;
            // contenido del buffer (identico para original y candidato)
            std::array<uint8,kBuf> content{};
            for(unsigned k=0;k<kBuf;++k) content[k]=uint8(0xC0u^(k*37u+uint32(fixture)*11u+uint32(f)*5u));
            auto* fl=reinterpret_cast<float*>(content.data()+kMatOff);
            for(int k:{0,1,2,4,5,6,8,9,10}) fl[k]=Elem(rng,nanMatrix);
            for(int k:{12,13,14}) fl[k]=(rng.next()%6==0) ? 0.f : (nanMatrix&&rng.next()%3==0 ? FromBits(kSpecial[rng.next()%kSpecial.size()]) : rng.uni(-1000.f,1000.f));
            if(f<=2 || (f>=4&&f<=6)) { arity=1; const float a=Angle(rng,stress); args[0]=ToBits(a); special|=IsSpecialValue(a); }
            else if(f==3) { arity=3; for(int k=0;k<3;++k){ const float a=Angle(rng,stress && (rng.next()%2)); args[k]=ToBits(a); special|=IsSpecialValue(a);} }
            else if(f==7) { arity=0;
                if(fixture%16==3) for(int k:{0,1,2}) fl[k]=(rng.next()%2 ? 0.f : -0.f);          // vector derecho nulo: suma 0 -> 1/0
                if(fixture%16==7) for(int k:{4,5,6}) fl[k]=fl[k-4];                              // forward == right: producto vectorial nulo
                if(fixture%16==11) for(int k:{0,1,2,4,5,6}) fl[k]*=1e-20f;                       // productos que desbordan a subnormal (exponente extendido)
                if(fixture%16==13) for(int k:{0,1,2,4,5,6}) fl[k]*=1e15f;                        // y que desbordan a infinito
            }
            else if(f==8) {
                arity=4; flags=uint32(fixture%32); if(fixture%7==0) flags|=rng.next()&~0x1Fu;
                mode=uint32(fixture/32)%8;
                const unsigned par=(flags>>2)&1, i=kIdx1[(flags>>3)&3], k2=kIdx2[i+par], j2=kIdx2[1+i-par];
                const bool rep=(flags>>1)&1;
                gimbal = (fixture/256)%3==0;                                                    // 1/3: s <= umbral (rama de bloqueo cardan) o muy cerca
                if(gimbal) { if(rep){ fl[(i<<2)+k2]=Tiny(rng); fl[(i<<2)+j2]=Tiny(rng);} else { fl[(k2<<2)+i]=Tiny(rng); fl[i*5]=Tiny(rng);} }
                // salidas: 0 distintas, 1 pX==pZ, 2 pY==pZ, 3 pX==pY, 4 las tres iguales, 5 dentro de la matriz, 6 pZ dentro de la matriz, 7 distintas en orden inverso
                auto* o=content.data()+kOutOff; uint8* m=content.data()+kMatOff;
                uint8* px=o+4; uint8* py=o+8; uint8* pz=o+12;
                switch(mode){ case 1:pz=px;break; case 2:pz=py;break; case 3:py=px;break; case 4:py=px;pz=px;break;
                    case 5:px=m+0;py=m+0x14;pz=m+0x28;break; case 6:pz=m+0;break; case 7:px=o+12;pz=o+4;break; default:break; }
                const uintptr_t base=uintptr_t(buf.data());
                args[0]=uint32(base+(px-content.data())); args[1]=uint32(base+(py-content.data())); args[2]=uint32(base+(pz-content.data())); args[3]=flags;
            }
            else { // ConvertFromEulerAngles
                arity=4; flags=uint32(fixture%32); if(fixture%7==0) flags|=rng.next()&~0x1Fu;
                float v[3]; for(int k=0;k<3;++k){ v[k]= stress ? Angle(rng,true) : rng.uni(-4.f,4.f); special|=IsSpecialValue(v[k]); }
                for(int k=0;k<3;++k) args[k]=ToBits(v[k]);
                args[3]=flags;
            }
            for(int k:{0,1,2,4,5,6,8,9,10,12,13,14}) special|=IsSpecialValue(fl[k]);
            auto run=[&](bool candidate) -> json {
                restore();
                buf=content;
                const auto before=buf;
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                const auto abi=CallThis(candidate ? dll[f] : fns[f].address, buf.data()+kMatOff, args, arity);
                const unsigned fpAfter=_controlfp(0,0);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                // Fuera de la matriz (y de la region de salida en ConvertToEuler) nada cambia.
                auto maskedBefore=before, maskedAfter=buf;
                for(unsigned off=0;off<kMatSize;++off){ maskedBefore[kMatOff+off]=0; maskedAfter[kMatOff+off]=0; }
                if(f==8) for(unsigned off=0;off<kOutSize;++off){ maskedBefore[kOutOff+off]=0; maskedAfter[kOutOff+off]=0; }
                const bool protectedOK = maskedBefore==maskedAfter && (fpAfter&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);
                const bool routeOK = candidate ? (dll[f]>=0x1000000) : !ReversingProbe::JumpsToDll(fns[f].address);
                json result{{"route_ok",routeOK},{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},{"fpu_stack_balanced",abi.fpu},
                            {"protected_state_unchanged",protectedOK},{"buf",Hex(buf.data(),buf.size())}};
                restore();
                return result;
            };
            const auto original=run(false), candidate=run(true);
            const bool stateOK=restored(), equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true&&original["nonvolatile_registers_preserved"]==true
                &&original["fpu_stack_balanced"]==true&&original["protected_state_unchanged"]==true;
            failures+=!accepted; passed&=accepted; allRestored&=stateOK;
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},
                   {"fixture",{{"stress",stress},{"special",special},{"flags",flags},{"mode",mode},{"gimbal",gimbal},{"args",args}}},
                   {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",fns[f].cases},{"failures",failures}});
    }
    restore(); allRestored&=restored();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
