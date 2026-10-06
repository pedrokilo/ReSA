#pragma once
// Banco dentro del motor: hooks duplicados y virtuales con hook plano (lista de dup_hooks.py).
// - Dos hooks del EXE que compartian una funcion de la DLL: CTaskComplexDiveFromAttachedEntityAndGetUp::ControlSubTask
//   (0x655710, copia propia en el EXE) frente a CTaskComplexEvasiveDiveAndGetUp::ControlSubTask (0x653810), y
//   CTaskComplexWanderFlee::ControlSubTask (0x65B380 = jmp 0x674C30) frente a CTaskComplexWander::ControlSubTask.
//   Cada uno tiene ahora su propia sobrescritura en la DLL; se comprueba ademas el estado mixto (un hook activo y
//   su pareja desactivada) y que el hueco de la pareja no cambia.
// - Virtuales que estaban con RH_ScopedInstall plano y ahora son hooks de vtable: GetTaskType de
//   CTaskComplexCarSlowBeDraggedOutAndStandUp y CTaskComplexGoToPointAndStandStillAndAchieveHeading,
//   CAEStreamingChannel::SynchPlayback y los 14 metodos IStream de CAEDataStream (vtable 0x85F0BC).
// Original (hooks desactivados: hueco de la vtable del EXE = direccion original exacta) frente a candidato (hueco
// fuera del EXE), siempre llamando por el hueco de la vtable del EXE sobre objetos falsos con la vptr del EXE.
// CTaskComplexWander::ControlSubTask se sustituye (entrada del EXE 0x674C30 y cuerpo de la DLL) por una funcion
// trazada. operator delete (0x8214BD) se sustituye solo durante Release, sin reservar memoria.
// Los HANDLE son propios (artifacts/input.bin); nunca se cierra un HANDLE ajeno.
// Comprobaciones extra (sin pares): los hooks de vtable bloqueados (reversed=false) convertidos siguen en su
// direccion original y son de tipo V; CEntity::Add-rect conmuta su hueco; las parejas tienen destinos distintos.
#include <fstream>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"

namespace FixHooksLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T=void*> inline T& At(void* p,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(p)+off); }

// --- registro sin reservas (operator delete sustituido) ---
inline std::array<uintptr_t,16> deletes{}; inline size_t ndel{};
inline void __cdecl FakeDelete(void* p){ if(ndel<deletes.size())deletes[ndel++]=uintptr_t(p); }

// --- CTaskComplexWander::ControlSubTask trazado ---
inline uint8* wanderBase{}; inline uint32 wanderSeed{};
inline void* __fastcall FakeWanderControl(void* self,void*,void* ped){
    trace.push_back({"wander_control",intptr_t(static_cast<uint8*>(self)-wanderBase),uintptr_t(ped)});
    return reinterpret_cast<void*>(uintptr_t(0x10000000u+(wanderSeed&0xFFFFu)*4u));
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(ReversibleHooks::ReversibleHook::Base* h){ if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }
inline uintptr_t Follow(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }
inline bool InExe(uintptr_t a){ return a>=0x400000&&a<0x1000000; }
inline uintptr_t Slot(uintptr_t vt,int slot){ return reinterpret_cast<uintptr_t*>(vt)[slot]; }
inline bool HandleAlive(HANDLE h){ DWORD flags{}; return GetHandleInformation(h,&flags)!=FALSE; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=20;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexDiveFromAttachedEntityAndGetUp","ControlSubTask",0x655710,0x85A358,10,512},
    {"CTaskComplexEvasiveDiveAndGetUp","ControlSubTask",0x653810,0x86F268,10,512},
    {"CTaskComplexWanderFlee","ControlSubTask",0x65B380,0x86F690,10,512},
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","GetTaskType",0x648680,0x86EF80,4,64},
    {"CTaskComplexGoToPointAndStandStillAndAchieveHeading","GetTaskType",0x668D30,0x86FE58,4,64},
    {"CAEStreamingChannel","SynchPlayback",0x4F1870,0x85F3F0,6,128},
    {"CAEDataStream","QueryInterface",0x4DC410,0x85F0BC,0,512},
    {"CAEDataStream","AddRef",0x4DC460,0x85F0BC,1,256},
    {"CAEDataStream","Release",0x4DC5B0,0x85F0BC,2,512},
    {"CAEDataStream","Read",0x4DC320,0x85F0BC,3,1024},
    {"CAEDataStream","Write",0x4DC4D0,0x85F0BC,4,64},
    {"CAEDataStream","Seek-istream",0x4DC340,0x85F0BC,5,1024},
    {"CAEDataStream","SetSize",0x4DC4E0,0x85F0BC,6,64},
    {"CAEDataStream","CopyTo",0x4DC4F0,0x85F0BC,7,64},
    {"CAEDataStream","Commit",0x4DC500,0x85F0BC,8,64},
    {"CAEDataStream","Revert",0x4DC510,0x85F0BC,9,64},
    {"CAEDataStream","LockRegion",0x4DC520,0x85F0BC,10,64},
    {"CAEDataStream","UnlockRegion",0x4DC530,0x85F0BC,11,64},
    {"CAEDataStream","Stat",0x4DC3A0,0x85F0BC,12,512},
    {"CAEDataStream","Clone",0x4DC540,0x85F0BC,13,64},
}};
// Parejas que compartian destino en la DLL: indice en fns (o -1 = hook externo) + hueco de la pareja
struct Partner { int self; const char* cls; const char* name; uintptr_t vt; int slot; uintptr_t address; };
inline const std::array<Partner,3> partners{{
    { 0, "CTaskComplexEvasiveDiveAndGetUp", "ControlSubTask", 0x86F268, 10, 0x653810 },
    { 1, "CTaskComplexDiveFromAttachedEntityAndGetUp", "ControlSubTask", 0x85A358, 10, 0x655710 },
    { 2, "CTaskComplexWander", "ControlSubTask", 0x86FE84, 10, 0x674C30 },
}};
// Hooks de vtable bloqueados (reversed=false) que antes eran planos: solo se comprueba que siguen en su sitio
struct Locked { const char* cls; const char* name; uintptr_t vt; int slot; uintptr_t address; };
inline const std::array<Locked,16> lockedVmt{{
    { "CTaskComplexGangFollower", "Clone", 0x86F938, 1, 0x65ECB0 },
    { "CTaskComplexGangFollower", "MakeAbortable", 0x86F938, 6, 0x65EC30 },
    { "CTaskComplexGangFollower", "CreateNextSubTask", 0x86F938, 8, 0x665E00 },
    { "CTaskComplexGangFollower", "CreateFirstSubTask", 0x86F938, 9, 0x666160 },
    { "CTaskComplexGangFollower", "ControlSubTask", 0x86F938, 10, 0x662A10 },
    { "CTaskGangHassleVehicle", "Clone", 0x86F9D4, 1, 0x65FC00 },
    { "CTaskGangHassleVehicle", "CreateNextSubTask", 0x86F9D4, 8, 0x65FC80 },
    { "CTaskGangHassleVehicle", "CreateFirstSubTask", 0x86F9D4, 9, 0x664BA0 },
    { "CTaskGangHassleVehicle", "ControlSubTask", 0x86F9D4, 10, 0x6637C0 },
    { "CTaskGoToVehicleAndLean", "Clone", 0x86FAC8, 1, 0x6621B0 },
    { "CTaskGoToVehicleAndLean", "MakeAbortable", 0x86FAC8, 6, 0x664500 },
    { "CTaskGoToVehicleAndLean", "CreateNextSubTask", 0x86FAC8, 8, 0x664590 },
    { "CTaskGoToVehicleAndLean", "CreateFirstSubTask", 0x86FAC8, 9, 0x664D40 },
    { "CTaskGoToVehicleAndLean", "ControlSubTask", 0x86FAC8, 10, 0x664E60 },
    { "CTaskSimpleDead", "ProcessPed", 0x86DEA4, 7, 0x630600 },
    { "CBike", "ProcessAI", 0x871360, 66, 0x6BC930 },
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fixhooks-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fixhooks-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fixhooks");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"fixhooks-only.txt"); if(f)std::getline(f,only); }
    using Hook=ReversibleHooks::ReversibleHook::Base;
    std::array<Hook*,NFNS> hooks{}; std::array<Hook*,partners.size()> partnerHooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))||hooks[f]->Symbol()[0]!='V'){Write({{"event","error"},{"reason","virtual hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    for(size_t p=0;p<partners.size();++p)if(!(partnerHooks[p]=FindHook(partners[p].cls,partners[p].name))||partnerHooks[p]->Symbol()[0]!='V'){Write({{"event","error"},{"reason","partner hook not found"},{"function",std::string(partners[p].cls)+"::"+partners[p].name}});return;}
    Hook* addRect=FindHook("CEntity","Add-rect");
    if(!addRect||addRect->Symbol()[0]!='V'){Write({{"event","error"},{"reason","CEntity::Add-rect is not a vtable hook"}});return;}
    const uintptr_t dllWanderControl=OurVirtual(partnerHooks[2]);
    std::vector<Hook*> all(hooks.begin(),hooks.end()); for(auto* h:partnerHooks)if(std::find(all.begin(),all.end(),h)==all.end())all.push_back(h); all.push_back(addRect);
    std::vector<bool> enabled,locked; for(auto* h:all){enabled.push_back(h->Hooked());locked.push_back(h->Locked());}
    auto statesRestored=[&]{ for(size_t k=0;k<all.size();++k)if(all[k]->Hooked()!=enabled[k]||all[k]->Locked()!=locked[k])return false; return true; };
    bool passed=true; int pair=0;

    // --- comprobaciones estaticas ---
    {
        json bad=json::array();
        for(auto& l:lockedVmt){ auto* h=FindHook(l.cls,l.name);
            const bool ok=h&&h->Symbol()[0]=='V'&&h->Locked()&&!h->Hooked()&&Slot(l.vt,l.slot)==l.address;
            if(!ok)bad.push_back(std::string(l.cls)+"::"+l.name); }
        const bool distinct=Follow(OurVirtual(hooks[0]))!=Follow(OurVirtual(hooks[1]))&&Follow(OurVirtual(hooks[2]))!=Follow(dllWanderControl);
        // CEntity::Add-rect: el hueco 1 del EXE conmuta (comprobado despues de cambiar el estado)
        addRect->LockState(false); addRect->State(false); const bool addOff=Slot(0x863928,1)==0x5347D0;
        addRect->State(true); const bool addOn=!InExe(Slot(0x863928,1)); addRect->State(enabled.back()); addRect->LockState(locked.back());
        const bool ok=bad.empty()&&distinct&&addOff&&addOn;
        Write({{"event","static_checks"},{"locked_vmt_bad",bad},{"locked_vmt_checked",lockedVmt.size()},{"distinct_dll_targets",distinct},{"add_rect_off",addOff},{"add_rect_on",addOn},{"passed",ok}});
        if(!ok){ Write({{"event","error"},{"reason","static checks failed"},{"state_restored",statesRestored()}}); return; }
    }
    for(auto* h:all)h->LockState(false);
    // Cuerpo de CTaskComplexWander::ControlSubTask en la DLL. Los hooks de vtable llevan un hook simple interno:
    // activo = jmp en la entrada del EXE (0x674C30) hacia la DLL; inactivo = jmp en la entrada de la DLL (vtable
    // de la DLL / ILT) hacia el EXE. Por eso el cuerpo se resuelve con el hook activo.
    uintptr_t wanderBody{};
    { auto* wh=partnerHooks[2]; const bool was=wh->Hooked(); wh->State(true); wanderBody=Follow(dllWanderControl); wh->State(was); }
    if(InExe(wanderBody)){ for(size_t k=0;k<all.size();++k){all[k]->State(enabled[k]);all[k]->LockState(locked[k]);}
        Write({{"event","error"},{"reason","Wander ControlSubTask body not found in the DLL"},{"state_restored",statesRestored()}}); return; }

    // --- fichero de prueba propio ---
    const auto fixture=std::filesystem::path(__FILE__).parent_path()/"artifacts/input.bin";
    { std::filesystem::create_directories(fixture.parent_path()); std::ofstream file(fixture,std::ios::binary); for(unsigned k=0;k<1024;++k)file.put(char((k*73+29)&255)); }
    auto Open=[&]{ return CreateFileW(fixture.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr); };
    static char nameBuf[16]="fixhooks.bin";

    {
        using ReversingProbe::EntryPatch;
        // Los parches de CTaskComplexWander::ControlSubTask (entrada del EXE y cuerpo de la DLL) se ponen solo despues
        // de fijar el estado de los hooks y se quitan antes de cualquier State(): State() reescribe esos mismos bytes.
        std::optional<EntryPatch> wPatchExe, wPatchDll;
        auto patchWander=[&]{ wPatchExe.emplace(0x674C30,FakeWanderControl); wPatchDll.emplace(wanderBody,FakeWanderControl); };
        auto unpatchWander=[&]{ wPatchDll.reset(); wPatchExe.reset(); };
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const Partner* partner=nullptr; Hook* partnerHook=nullptr;
            for(size_t p=0;p<partners.size();++p)if(partners[p].self==int(f)){partner=&partners[p];partnerHook=partnerHooks[p];}
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xF1C5u; if(!stt)stt=1; trace=json::array(); ndel=0;
                    for(auto* h:all)if(h!=addRect)h->State(candidate);
                    // Como pide el banco: el conmutador se comprueba despues de fijar el estado de la ejecucion
                    const uintptr_t a=Slot(fn.vt,fn.slot);
                    bool switches=candidate ? !InExe(a) : a==fn.address;
                    if(partner)switches&=candidate ? !InExe(Slot(partner->vt,partner->slot)) : Slot(partner->vt,partner->slot)==partner->address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls.rfind("CTask",0)==0) {
                        alignas(16) std::array<uint8,0x40> obj; for(auto& b:obj)b=uint8(next()); At<uintptr_t>(obj.data(),0)=fn.vt;
                        if(nm=="ControlSubTask"){
                            At<uint32>(obj.data(),8)=next()%5 ? 0x20000000u+(next()&0xFFFFu)*4u : 0u;
                            void* ped=reinterpret_cast<void*>(uintptr_t(0x30000000u+(next()&0xFFFFu)*16u)); wanderBase=obj.data(); wanderSeed=next();
                            using CS=void*(__thiscall*)(void*,void*);
                            const bool wander=cls=="CTaskComplexWanderFlee";
                            if(wander)patchWander();
                            ret=uintptr_t(reinterpret_cast<CS>(a)(obj.data(),ped));
                            if(wander)unpatchWander();
                            // Estado mixto: este hook igual, la pareja al contrario (dos hooks ya no comparten destino)
                            partnerHook->State(!candidate);
                            const uintptr_t a2=Slot(fn.vt,fn.slot), p2=Slot(partner->vt,partner->slot);
                            out["mixed_switches"]=a2==a&&(candidate ? p2==partner->address : !InExe(p2));
                            if(wander)patchWander();
                            out["mixed"]=uintptr_t(reinterpret_cast<CS>(a2)(obj.data(),ped));
                            if(wander)unpatchWander();
                            partnerHook->State(candidate);
                        } else ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(obj.data());
                        out["obj"]=ReversingProbe::Bytes(obj.data()+4,obj.size()-4);
                    }
                    else if(cls=="CAEStreamingChannel") {
                        alignas(16) std::array<uint8,0x200> obj; for(auto& b:obj)b=uint8(next()); At<uintptr_t>(obj.data(),0)=fn.vt;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        out["obj"]=ReversingProbe::Bytes(obj.data()+4,obj.size()-4);
                    }
                    else {
                        // CAEDataStream falso: vptr del EXE, HANDLE propio, nombre en un buffer estatico
                        alignas(16) std::array<uint8,0x28> obj; for(auto& b:obj)b=uint8(next()); void* s=obj.data();
                        At<uintptr_t>(s,0)=0x85F0BC;
                        const bool needsFile=nm=="Read"||nm=="Seek-istream"||nm=="Stat"||nm=="Release";
                        const bool invalid=nm=="Stat"&&i%8==0;
                        HANDLE owned=needsFile&&!invalid ? Open() : INVALID_HANDLE_VALUE;
                        if(needsFile&&!invalid&&owned==INVALID_HANDLE_VALUE){ out["open_failed"]=true; return out; }
                        At<HANDLE>(s,4)=owned; At<char*>(s,8)=next()%4 ? nameBuf : nullptr;
                        At<uint8>(s,0xC)=uint8(next()%2);
                        const uint32 start=std::array<uint32,8>{0,1,31,128,511,512,513,7}[next()%8];
                        At<uint32>(s,0x14)=start; At<uint32>(s,0x18)=std::array<uint32,6>{0,1,16,128,256,512}[next()%6];
                        At<uint32>(s,0x10)=next()%4 ? start+next()%40 : next(); At<uint8>(s,0x20)=uint8(next()%2);
                        const uint32 refs=std::array<uint32,7>{1,1,2,3,0,0x7FFFFFFF,0xFFFFFFFF}[next()%7]; At<uint32>(s,0x24)=next()%8 ? refs : next();
                        if(owned!=INVALID_HANDLE_VALUE)SetFilePointer(owned,LONG(start+next()%64),nullptr,FILE_BEGIN);
                        bool closedByCall=false;
                        if(nm=="QueryInterface"){ static const IID other{0x12345678,0x9ABC,0xDEF0,{1,2,3,4,5,6,7,8}};
                            const IID* iids[]{&IID_IUnknown,&IID_IStream,&IID_ISequentialStream,&other}; void* outPtr=reinterpret_cast<void*>(0x5A5A5A5A);
                            ret=reinterpret_cast<HRESULT(__stdcall*)(void*,const IID*,void**)>(a)(s,iids[next()%4],&outPtr);
                            out["out"]=outPtr==s ? json("this") : json(uintptr_t(outPtr)); }
                        else if(nm=="AddRef")ret=reinterpret_cast<ULONG(__stdcall*)(void*)>(a)(s);
                        else if(nm=="Release"){ const bool open=At<uint8>(s,0xC)!=0; const bool last=At<uint32>(s,0x24)==1;
                            ULONG r; { EntryPatch d(0x8214BD,FakeDelete); r=reinterpret_cast<ULONG(__stdcall*)(void*)>(a)(s); }
                            ret=r; closedByCall=last&&open;
                            json dl=json::array(); for(size_t k=0;k<ndel;++k)dl.push_back(deletes[k]==uintptr_t(s) ? json("this") : deletes[k]==uintptr_t(nameBuf) ? json("name") : json(deletes[k]));
                            out["deletes"]=dl; }
                        else if(nm=="Read"){ std::array<uint8,1536> buf; buf.fill(0xA5); const ULONG size=std::array<ULONG,9>{0,1,2,15,128,511,512,513,1024}[next()%9];
                            ULONG readed=0x77777777; ret=reinterpret_cast<HRESULT(__stdcall*)(void*,void*,ULONG,ULONG*)>(a)(s,buf.data(),size,&readed);
                            out["buffer"]=ReversingProbe::Bytes(buf.data(),buf.size()); out["readed"]=readed; }
                        else if(nm=="Seek-istream"){ LARGE_INTEGER off; off.LowPart=DWORD(std::array<int32,9>{-1024,-512,-1,0,1,17,128,512,1024}[next()%9]); off.HighPart=next()%4 ? 0 : LONG(next());
                            const DWORD whence=next()%8 ? next()%3 : 3; ULARGE_INTEGER no; no.QuadPart=0x7777777777777777ull; const bool useOut=next()%5!=0;
                            ret=reinterpret_cast<HRESULT(__stdcall*)(void*,LARGE_INTEGER,DWORD,ULARGE_INTEGER*)>(a)(s,off,whence,useOut ? &no : nullptr);
                            out["new"]=std::to_string(no.QuadPart); }
                        else if(nm=="Stat"){ STATSTG st; std::memset(&st,0xA7,sizeof(st)); const DWORD flags=std::array<DWORD,4>{0,1,2,1}[next()%4];
                            ret=reinterpret_cast<HRESULT(__stdcall*)(void*,STATSTG*,DWORD)>(a)(s,next()%7 ? &st : nullptr,flags); out["stat"]=ReversingProbe::Bytes(&st,sizeof(st)); }
                        else { std::array<uint8,64> argBuf; for(auto& b:argBuf)b=uint8(next()); ULARGE_INTEGER u1,u2; u1.QuadPart=(uint64(next())<<32)|next(); u2.QuadPart=(uint64(next())<<32)|next();
                            const DWORD d=next(); void* p1=argBuf.data(); auto* p2=reinterpret_cast<ULARGE_INTEGER*>(argBuf.data()+16); auto* p3=reinterpret_cast<ULARGE_INTEGER*>(argBuf.data()+32);
                            if(nm=="Write"){ ULONG w=0x66666666; ret=reinterpret_cast<HRESULT(__stdcall*)(void*,const void*,ULONG,ULONG*)>(a)(s,p1,d,next()%4 ? &w : nullptr); out["written"]=w; }
                            else if(nm=="SetSize")ret=reinterpret_cast<HRESULT(__stdcall*)(void*,ULARGE_INTEGER)>(a)(s,u1);
                            else if(nm=="CopyTo")ret=reinterpret_cast<HRESULT(__stdcall*)(void*,void*,ULARGE_INTEGER,ULARGE_INTEGER*,ULARGE_INTEGER*)>(a)(s,next()%3 ? p1 : nullptr,u1,p2,p3);
                            else if(nm=="Commit")ret=reinterpret_cast<HRESULT(__stdcall*)(void*,DWORD)>(a)(s,d);
                            else if(nm=="Revert")ret=reinterpret_cast<HRESULT(__stdcall*)(void*)>(a)(s);
                            else if(nm=="LockRegion"||nm=="UnlockRegion")ret=reinterpret_cast<HRESULT(__stdcall*)(void*,ULARGE_INTEGER,ULARGE_INTEGER,DWORD)>(a)(s,u1,u2,d);
                            else { void* clone=reinterpret_cast<void*>(0x4B4B4B4B); ret=reinterpret_cast<HRESULT(__stdcall*)(void*,void**)>(a)(s,&clone); out["clone"]=uintptr_t(clone); }
                            out["args"]=ReversingProbe::Bytes(argBuf.data(),argBuf.size()); }
                        auto bytes=ReversingProbe::Bytes(s,obj.size());
                        std::fill_n(bytes.begin(),4,0); // vptr: el destructor de la DLL puede escribir la vtable de la DLL
                        std::fill_n(bytes.begin()+4,4,0); // HANDLE: distinto en cada ejecucion
                        out["obj"]=bytes; out["handle_same"]=At<HANDLE>(s,4)==owned; out["name"]=At<char*>(s,8)==nameBuf ? json("name") : json(uintptr_t(At<char*>(s,8)));
                        if(owned!=INVALID_HANDLE_VALUE){
                            if(closedByCall) out["handle_closed"]=!HandleAlive(owned); // cerrado por el destructor: no se vuelve a cerrar
                            else { out["file_position"]=SetFilePointer(owned,0,nullptr,FILE_CURRENT); CloseHandle(owned); }
                        }
                    }
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&
                              (!original.contains("mixed_switches")||(original["mixed_switches"]==true&&candidate["mixed_switches"]==true));
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t k=0;k<all.size();++k){all[k]->State(enabled[k]);all[k]->LockState(locked[k]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",statesRestored()}});
}
}
