#pragma once
// Banco dentro del motor: arreglos de nucleo fuera de listas (17 funciones).
// - CMatrix::RotateX/RotateY/RotateZ, SetRotateX/Y/ZOnly y SetRotateX/Y/Z: CMatrix no instala hooks (InjectHooksMain lo
//   desactiva por rendimiento), asi que se compara la funcion del EXE (direccion) con el simbolo de la DLL, sobre la
//   misma matriz de 0x48 bytes (relleno incluido) y el mismo angulo.
// - CColBox::operator= (0x40EEC0): sin hook; EXE frente a DLL sobre destino y origen de 0x1C bytes.
// - CVehicle::SetComponentRotation y CTaskSimpleSwim::ApplyRollAndPitch (usan RotateX/Y/Z y SetRotateX/Y/Z de la DLL):
//   hooks del banco desactivados (original) frente a activados (candidato). Frame/matriz RW falsos; RotateTorso y
//   UpdateRwFrame simulados y trazados.
// - CPool (New/NewAt/Delete de la DLL): CTxdStore::AddTxdSlot/RemoveTxdSlot (CPool<TxdDef>::New 0x731B80 y Delete en
//   linea) y CPed::operator new/new(ref)/delete (0x5E4720/0x5E4730/0x5E4760) sobre pools falsos: el puntero global del
//   pool (0xC8800C / 0xB74490) se cambia solo durante la llamada y se restaura. Se compara cursor, estados y bytes del
//   almacenamiento (sin relleno 0xCD/0xDD). El hook CPedPool::New (0x5E45E0) se conmuta junto con los del banco.
//   RwTexDictionaryDestroy (0x7F36A0) simulado y trazado.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Matrix.h"
#include "ColBox.h"
#include "TxdStore.h"
#include "PedIK.h"
#include "TaskSimpleSwim.h"
#include "../ProbeRuntime.hpp"

namespace FixCoreLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- stubs ---
inline uint8* pedBase{};
inline void __fastcall FakeRotateTorso(void* ik,void*,void* bone,const float* o,uint32 flag){ trace.push_back({"rotate_torso",intptr_t(ik)-intptr_t(pedBase),uint32(uintptr_t(bone)),F(o[0]),F(o[1]),flag&0xFF}); }
inline void __fastcall FakeUpdateRwFrame(void* e,void*){ trace.push_back({"update_rw_frame",intptr_t(e)-intptr_t(pedBase)}); }
inline RwBool __cdecl FakeTexDictDestroy(RwTexDictionary* d){ trace.push_back({"tex_dict_destroy",uint32(uintptr_t(d))}); return TRUE; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// slot -1 = hook normal (se conmuta); slot -2 = sin hook (EXE en la direccion frente al simbolo de la DLL)
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,17> fns{{
    {"CMatrix","RotateX",0x59B1E0,0,-2,1024},{"CMatrix","RotateY",0x59B2C0,0,-2,1024},{"CMatrix","RotateZ",0x59B390,0,-2,1024},
    {"CMatrix","SetRotateXOnly",0x59AFA0,0,-2,512},{"CMatrix","SetRotateYOnly",0x59AFE0,0,-2,512},{"CMatrix","SetRotateZOnly",0x59B020,0,-2,512},
    {"CMatrix","SetRotateX",0x59B060,0,-2,512},{"CMatrix","SetRotateY",0x59B0A0,0,-2,512},{"CMatrix","SetRotateZ",0x59B0E0,0,-2,512},
    {"CColBox","operator=",0x40EEC0,0,-2,512},
    {"CVehicle","SetComponentRotation",0x6DBA30,0,-1,1024},
    {"CTaskSimpleSwim","ApplyRollAndPitch",0x68A8E0,0,-1,1024},
    {"CTxdStore","AddTxdSlot",0x731C80,0,-1,1024},{"CTxdStore","RemoveTxdSlot",0x731CD0,0,-1,512},
    {"CPed","operator new-anon",0x5E4720,0,-1,512},{"CPed","operator new-poolIndexed",0x5E4730,0,-1,512},{"CPed","operator delete-anon",0x5E4760,0,-1,512},
}};

inline uintptr_t DllAddress(const std::string& cls,const std::string& nm) {
    using std::bit_cast;
    if(cls=="CColBox") return bit_cast<uintptr_t>(static_cast<CColBox&(CColBox::*)(const CColBox&)>(&CColBox::operator=));
    if(nm=="RotateX") return bit_cast<uintptr_t>(&CMatrix::RotateX);
    if(nm=="RotateY") return bit_cast<uintptr_t>(&CMatrix::RotateY);
    if(nm=="RotateZ") return bit_cast<uintptr_t>(&CMatrix::RotateZ);
    if(nm=="SetRotateXOnly") return bit_cast<uintptr_t>(&CMatrix::SetRotateXOnly);
    if(nm=="SetRotateYOnly") return bit_cast<uintptr_t>(&CMatrix::SetRotateYOnly);
    if(nm=="SetRotateZOnly") return bit_cast<uintptr_t>(&CMatrix::SetRotateZOnly);
    if(nm=="SetRotateX") return bit_cast<uintptr_t>(&CMatrix::SetRotateX);
    if(nm=="SetRotateY") return bit_cast<uintptr_t>(&CMatrix::SetRotateY);
    if(nm=="SetRotateZ") return bit_cast<uintptr_t>(&CMatrix::SetRotateZ);
    return 0;
}

// cabecera de CPool (0x14 bytes): almacenamiento, estados, capacidad, cursor, posee, admite sin memoria (solo DLL)
struct FakePool { void* storage; uint8* states; int32 cap; int32 last; uint8 owns, noMemory, pad[2]; };
static_assert(sizeof(FakePool)==0x14);
constexpr size_t TXD_STRIDE=0xC, TXD_CAP=24, PED_STRIDE=0x7C4, PED_CAP=6;
alignas(16) inline std::array<uint8,TXD_STRIDE*TXD_CAP> txdStorage{}; inline std::array<uint8,TXD_CAP> txdStates{};
alignas(16) inline std::array<uint8,PED_STRIDE*PED_CAP> pedStorage{}; inline std::array<uint8,PED_CAP> pedStates{};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fixcore-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fixcore-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fixcore");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"fixcore-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,17> hooks{}; std::array<uintptr_t,17> dll{};
    for(size_t f=0;f<fns.size();++f) {
        if(fns[f].slot==-2){ if(!(dll[f]=DllAddress(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","dll symbol not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;} continue; }
        if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    }
    auto* pedPoolNew=FindHook("CPedPool","New"); if(!pedPoolNew){Write({{"event","error"},{"reason","hook not found"},{"function","CPedPool::New"}});return;}
    std::vector<ReversibleHooks::ReversibleHook::Base*> all; for(auto* h:hooks)if(h)all.push_back(h); all.push_back(pedPoolNew);
    std::vector<bool> enabled,locked; for(auto* h:all){enabled.push_back(h->Hooked());locked.push_back(h->Locked());h->LockState(false);}

    auto& txdPoolPtr=*reinterpret_cast<void**>(0xC8800C); void* const savedTxdPool=txdPoolPtr;
    auto& pedPoolPtr=*reinterpret_cast<void**>(0xB74490); void* const savedPedPool=pedPoolPtr;

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(&CPedIK::RotateTorso),FakeRotateTorso), s1e(0x5FDDB0,FakeRotateTorso);
        EntryPatch s2(bit_cast<uintptr_t>(&CEntity::UpdateRwFrame),FakeUpdateRwFrame), s2e(0x532B00,FakeUpdateRwFrame);
        EntryPatch s3(0x7F36A0,FakeTexDictDestroy);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto angle=[&]()->float{ switch(next()%7){case 0:return uni(-3.2f,3.2f);case 1:return uni(-100.f,100.f);case 2:return uni(-1e5f,1e5f);
                    case 3:return float(int(next()%17)-8)*bit_cast<float>(0x3FC90FDBu);case 4:return next()%2 ? 0.f : -0.f;case 5:return uni(-1e-3f,1e-3f);default:return uni(-7.f,7.f);} };
                auto elem=[&]()->float{ switch(next()%8){case 0:return 0.f;case 1:return -0.f;case 2:return 1.f;case 3:return -1.f;default:return uni(-2.f,2.f);} };
                auto posv=[&]()->float{ switch(next()%6){case 0:return 0.f;case 1:return -0.f;default:return uni(-1000.f,1000.f);} };
                // matriz (CMatrix o RwMatrix, misma disposicion): 3 ejes + posicion; relleno/flags aleatorios
                auto fillMatrix=[&](uint8* m){ for(int k=0;k<0x40;++k)m[k]=uint8(next()); auto* fl=reinterpret_cast<float*>(m);
                    for(int k:{0,1,2,4,5,6,8,9,10})fl[k]=elem(); for(int k:{12,13,14})fl[k]=posv(); };
                // pool falso: estados aleatorios con needFree libres y needUsed ocupados como minimo
                auto makePool=[&](FakePool& p,uint8* stor,size_t stride,uint8* st,int32 cap,int needFree,int needUsed){
                    p={stor,st,cap,int32(next()%uint32(cap+1))-1,0,1,{0,0}};
                    for(int32 k=0;k<cap;++k)st[k]=uint8((next()&0x7F)|(next()%2 ? 0x80u : 0u));
                    auto nfree=[&]{int n=0;for(int32 k=0;k<cap;++k)n+=st[k]>>7;return n;};
                    while(nfree()<needFree)st[next()%uint32(cap)]|=0x80; while(cap-nfree()<needUsed)st[next()%uint32(cap)]&=0x7F;
                    for(size_t k=0;k<size_t(cap)*stride;++k)stor[k]=uint8(next()); };
                auto pick=[&](const uint8* st,int32 cap,bool free_){ for(;;){ const int32 k=int32(next()%uint32(cap)); if(bool(st[k]>>7)==free_)return k; } };
                auto poolOut=[&](json& out,const FakePool& p,size_t stride){ out["last"]=p.last; out["cap"]=p.cap; out["states"]=ReversingProbe::Bytes(p.states,size_t(p.cap));
                    const size_t n=size_t(p.cap)*stride; if(n<=512)out["storage"]=ReversingProbe::Bytes(p.storage,n); else out["storage"]=Fnv(p.storage,n); };

                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC0DEu; if(!stt)stt=1; trace=json::array();
                    for(auto* h:all)h->State(candidate);
                    bool switches; uintptr_t a;
                    if(fn.slot==-2){ switches=dll[f]<0x400000||dll[f]>=0x1000000; a=candidate ? dll[f] : fn.address; }
                    else { switches=ReversingProbe::JumpsToDll(fn.address)==candidate; a=fn.address; }
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CMatrix") {
                        alignas(16) static std::array<uint8,0x48> m{}; for(auto& b:m)b=uint8(next()); fillMatrix(m.data());
                        const float ang=angle(); out["angle"]=F(ang);
                        reinterpret_cast<void(__thiscall*)(void*,float)>(a)(m.data(),ang);
                        out["m"]=ReversingProbe::Bytes(m.data(),m.size());
                    }
                    else if(cls=="CColBox") {
                        alignas(16) static std::array<uint8,0x1C> dst{}, src{};
                        for(auto& b:dst)b=uint8(next()); for(auto& b:src)b=uint8(next());
                        for(int k=0;k<6;++k){ reinterpret_cast<float*>(dst.data())[k]=uni(-500.f,500.f); reinterpret_cast<float*>(src.data())[k]=next()%8 ? uni(-500.f,500.f) : -0.f; }
                        auto* r=reinterpret_cast<void*(__thiscall*)(void*,const void*)>(a)(dst.data(),src.data());
                        ret=r==dst.data() ? "this" : "other"; out["dst"]=ReversingProbe::Bytes(dst.data(),dst.size()); out["src"]=ReversingProbe::Bytes(src.data(),src.size());
                    }
                    else if(cls=="CVehicle") {
                        alignas(16) static std::array<uint8,0xA0> frame{}; alignas(16) static std::array<uint8,0x40> veh{};
                        for(auto& b:frame)b=uint8(next()); for(auto& b:veh)b=uint8(next()); fillMatrix(frame.data()+0x10);
                        const int32 axis=next()%8 ? int32(next()%3) : (next()%2 ? 3 : -1); const float ang=angle(); const bool set=next()%2;
                        reinterpret_cast<void(__thiscall*)(void*,void*,int32,float,bool)>(a)(veh.data(),frame.data(),axis,ang,set);
                        out["frame"]=ReversingProbe::Bytes(frame.data(),frame.size()); out["veh"]=Fnv(veh.data(),veh.size());
                    }
                    else if(cls=="CTaskSimpleSwim") {
                        alignas(16) static std::array<uint8,0x7C4> ped{}; alignas(16) static std::array<uint8,0x40> clump{}; alignas(16) static std::array<uint8,0xA0> frame{};
                        alignas(16) static std::array<uint8,0x64> task{};
                        ped.fill(0); for(auto& b:clump)b=uint8(next()); clump[0]=2; *reinterpret_cast<void**>(clump.data()+4)=frame.data();
                        for(auto& b:frame)b=uint8(next()); fillMatrix(frame.data()+0x10);
                        *reinterpret_cast<void**>(ped.data()+0x18)=clump.data(); *reinterpret_cast<uint32*>(ped.data()+0x48C)=next(); pedBase=ped.data();
                        for(auto& b:task)b=uint8(next()); auto* tf=reinterpret_cast<float*>(task.data());
                        tf[0x24/4]=angle(); tf[0x28/4]=angle(); tf[0x2C/4]=uni(-1.6f,1.6f); tf[0x30/4]=uni(-3.2f,3.2f);
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(task.data(),ped.data());
                        out["frame"]=ReversingProbe::Bytes(frame.data(),frame.size()); out["clump"]=ReversingProbe::Bytes(clump.data(),clump.size());
                        out["ped"]=Fnv(ped.data(),ped.size()); out["task"]=ReversingProbe::Bytes(task.data(),task.size());
                    }
                    else if(cls=="CTxdStore") {
                        FakePool p{}; const int32 cap=int32(1+next()%TXD_CAP);
                        if(nm=="AddTxdSlot"){
                            makePool(p,txdStorage.data(),TXD_STRIDE,txdStates.data(),cap,1,0);
                            static const char* names[]{"infernus","LOADSCS","particle","fronten1","hud","generic","txd_test_slot","","vehicle99","CJ_Tex"};
                            const char* n=names[next()%std::size(names)];
                            txdPoolPtr=&p; ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(n); txdPoolPtr=savedTxdPool;
                        } else {
                            makePool(p,txdStorage.data(),TXD_STRIDE,txdStates.data(),cap,0,1);
                            const int32 idx=pick(txdStates.data(),cap,false);
                            *reinterpret_cast<uint32*>(txdStorage.data()+idx*TXD_STRIDE)=next()%2 ? 0x10000u+(next()%0x1000)*4 : 0u;
                            out["index"]=idx;
                            txdPoolPtr=&p; reinterpret_cast<void(__cdecl*)(int32)>(a)(idx); txdPoolPtr=savedTxdPool;
                        }
                        poolOut(out,p,TXD_STRIDE);
                    }
                    else if(cls=="CPed") {
                        FakePool p{}; const int32 cap=int32(2+next()%(PED_CAP-1));
                        auto index=[&](void* r)->json{ return r ? json((intptr_t(r)-intptr_t(pedStorage.data()))/intptr_t(PED_STRIDE)) : json(nullptr); };
                        if(nm=="operator new-anon"){
                            makePool(p,pedStorage.data(),PED_STRIDE,pedStates.data(),cap,next()%8 ? 1 : 0,0);
                            if(next()%8==0)for(int32 k=0;k<cap;++k)pedStates[k]&=0x7F; // pool lleno
                            pedPoolPtr=&p; void* r=reinterpret_cast<void*(__cdecl*)(uint32)>(a)(PED_STRIDE); pedPoolPtr=savedPedPool; ret=index(r);
                        } else if(nm=="operator new-poolIndexed"){
                            makePool(p,pedStorage.data(),PED_STRIDE,pedStates.data(),cap,2,0); // queda otro libre: el bucle de CreateAtRef termina
                            const int32 idx=pick(pedStates.data(),cap,true); const int32 ref=(idx<<8)|int32(next()&0xFF); out["ref"]=ref;
                            pedPoolPtr=&p; void* r=reinterpret_cast<void*(__cdecl*)(uint32,int32)>(a)(PED_STRIDE,ref); pedPoolPtr=savedPedPool; ret=index(r);
                        } else {
                            makePool(p,pedStorage.data(),PED_STRIDE,pedStates.data(),cap,0,1);
                            const int32 idx=pick(pedStates.data(),cap,false); out["index"]=idx;
                            pedPoolPtr=&p; reinterpret_cast<void(__cdecl*)(void*)>(a)(pedStorage.data()+idx*PED_STRIDE); pedPoolPtr=savedPedPool;
                        }
                        poolOut(out,p,PED_STRIDE);
                    }
                    for(auto* h:all)h->State(false);
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    txdPoolPtr=savedTxdPool; pedPoolPtr=savedPedPool;
    for(size_t k=0;k<all.size();++k){all[k]->State(enabled[k]);all[k]->LockState(locked[k]);}
    bool hooksRestored=true; for(size_t k=0;k<all.size();++k)hooksRestored&=all[k]->Hooked()==enabled[k];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",txdPoolPtr==savedTxdPool&&pedPoolPtr==savedPedPool&&hooksRestored}});
}
}
