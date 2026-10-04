#pragma once
// Banco dentro del motor: luces RW del juego (Light, lista de 27; 27 funciones). Original (hooks del grupo
// desactivados) frente a candidato con fixtures deterministas. Nunca se tocan las luces reales del RpWorld ni el
// dispositivo: pAmbient, pDirect y pExtraDirectionals se sustituyen por RpLight falsos (con RwFrame falsos) y se
// restauran. Se simulan y trazan (direcciones del EXE; las funciones RW de la DLL son envoltorios del EXE):
// RpLightCreate/Destroy/SetColor/SetRadius, RpWorldAddLight/RemoveLight, RwFrameCreate/Destroy/Rotate/Transform/
// UpdateObjects, _rwObjectHasFrameSetFrame y RwMatrixUpdate. Los stubs no reservan memoria (registro fijo que se
// convierte a json despues de la llamada). Las variables globales tocadas (colores de luz, punteros, fuerzas,
// ciclo horario, reloj, area, multiplicadores) se guardan y se restauran en cada ejecucion.
// Bytes sin inicializar en el EXE (pila): el alpha de los colores locales (salvo los que el EXE escribe) y
// flags/rellenos/pos de la matriz de RwFrameTransform se enmascaran.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../ProbeRuntime.hpp"

namespace Light27Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr uintptr_t LIGHT_GLOBALS=0xC8865C; constexpr size_t LIGHT_GLOBALS_SIZE=0xB0; // DirectAmbientLight ... NumExtraDirectionalLights
constexpr uintptr_t P_AMBIENT=0xC886E8, P_DIRECT=0xC886EC, P_EXTRA=0xC886F0, NUM_EXTRA=0xC88708, STRENGTHS=0xC8867C;
constexpr uintptr_t AMB_FRAME=0xC886D4, AMB_FRAME_PCO=0xC886C4, DIR_FRAME=0xC886B4, AMB_COL=0xC886A4, DIR_COL=0xC88694;
constexpr uintptr_t COLOURS=0xB7C4A0, RIOTS_MULT=0x8CD060, LIGHTS_MULT=0x8D4B5C, LIGHTNING=0xC812CC, SUN_DIR=0xB7CB14;
constexpr uintptr_t BRIGHTNESS=0xB79E30, TIME_IN_MS=0xB7CB84, CLOCK=0xB70150, CURR_AREA=0xB72914, INFRARED_COL=0x8D50BC;

// --- objetos falsos ---
alignas(16) inline std::array<std::array<uint8,0x40>,8> lights{};
alignas(16) inline std::array<std::array<uint8,0xA0>,8> frames{};
alignas(16) inline std::array<uint8,0x40> world{};
alignas(16) inline std::array<uint8,0x10> colourArg{};
inline uint32 createIdx{}, frameIdx{};

struct Region { uintptr_t a; size_t n; };
inline std::array<Region,24> regions{}; inline size_t nRegions{};
inline void Reg(const void* p,size_t n){ regions[nRegions++]={uintptr_t(p),n}; }
inline bool OnStack(const void* p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=tib->StackLimit&&p<tib->StackBase; }
// Identificador sin reservas: nulo, region+desplazamiento, pila u otro.
inline uint32 IdU(const void* p){ if(!p) return 0xFFFFFFFFu; const auto u=uintptr_t(p);
    for(size_t k=0;k<nRegions;++k) if(u>=regions[k].a&&u<regions[k].a+regions[k].n) return uint32((k+1)<<16|(u-regions[k].a));
    return OnStack(p) ? 0xEEEEEEEEu : 0xDDDDDDDDu; }

// --- registro fijo (sin reservas dentro de los stubs) ---
struct Ev { const char* k; uint32 n; std::array<uint32,14> v; };
inline std::array<Ev,64> evs{}; inline uint32 nev{};
template<class... T> inline void Push(const char* k,T... a){ if(nev<evs.size()){ auto& e=evs[nev]; e.k=k; e.n=0; ((e.v[e.n++]=uint32(a)),...); } ++nev; }
inline json Events(){ json t=json::array(); for(uint32 i=0;i<std::min<uint32>(nev,uint32(evs.size()));++i){ json e=json::array({evs[i].k}); for(uint32 j=0;j<evs[i].n;++j)e.push_back(evs[i].v[j]); t.push_back(e); }
    if(nev>evs.size())t.push_back({"overflow",nev}); return t; }
inline bool stackAlpha{}; // el EXE escribe el alpha del color local (SetLightsForInfraredVisionDefaultObjects / SetLightsForNightVision)

// --- stubs RW ---
inline void* __cdecl FakeLightCreate(int32 type){ void* l=lights[createIdx++%lights.size()].data(); Push("rp_light_create",type,IdU(l)); return l; }
inline int32 __cdecl FakeLightDestroy(void* l){ Push("rp_light_destroy",IdU(l)); return 1; }
inline void* __cdecl FakeLightSetColor(void* l,const RwRGBAReal* c){ const bool st=OnStack(c);
    Push("rp_light_set_color",IdU(l),IdU(st ? nullptr : c),st,F(c->red),F(c->green),F(c->blue),(!st||stackAlpha) ? F(c->alpha) : 0xA1A1A1A1u); return l; }
inline void* __cdecl FakeLightSetRadius(void* l,float r){ Push("rp_light_set_radius",IdU(l),F(r)); return l; }
inline void* __cdecl FakeWorldAddLight(void* w,void* l){ Push("rp_world_add_light",IdU(w),IdU(l)); return w; }
inline void* __cdecl FakeWorldRemoveLight(void* w,void* l){ Push("rp_world_remove_light",IdU(w),IdU(l)); return w; }
inline void* __cdecl FakeFrameCreate(){ void* f=frames[frameIdx++%frames.size()].data(); Push("rw_frame_create",IdU(f)); return f; }
inline int32 __cdecl FakeFrameDestroy(void* f){ Push("rw_frame_destroy",IdU(f)); return 1; }
inline void __cdecl FakeSetFrame(void* o,void* f){ Push("rw_object_set_frame",IdU(o),IdU(f)); At<void*>(o,4)=f; }
inline void* __cdecl FakeFrameRotate(void* f,const RwV3d* v,float ang,int32 c){ Push("rw_frame_rotate",IdU(f),F(v->x),F(v->y),F(v->z),F(ang),c); return f; }
inline void* __cdecl FakeFrameTransform(void* f,const RwMatrix* m,int32 c){ // flags, rellenos y pos sin inicializar en el EXE
    Push("rw_frame_transform",IdU(f),F(m->right.x),F(m->right.y),F(m->right.z),F(m->up.x),F(m->up.y),F(m->up.z),F(m->at.x),F(m->at.y),F(m->at.z),c); return f; }
inline void* __cdecl FakeFrameUpdateObjects(void* f){ Push("rw_frame_update_objects",IdU(f)); return f; }
inline void* __cdecl FakeMatrixUpdate(void* m){ Push("rw_matrix_update",IdU(m)); return m; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,27> fns{{
    {"Light","ActivateDirectional",0x735C80,0,-1,128},{"Light","DeActivateDirectional",0x735C70,0,-1,128},
    {"Light","LightsCreate",0x5BA520,0,-1,128},{"Light","LightsDestroy",0x735730,0,-1,256},{"Light","LightsEnable",0x735720,0,-1,128},
    {"Light","SetLightsWithTimeOfDayColour",0x7354E0,0,-1,1024},
    {"Light","WorldReplaceNormalLightsWithScorched",0x7357E0,0,-1,256},{"Light","WorldReplaceScorchedLightsWithNormal",0x735820,0,-1,128},
    {"Light","AddAnExtraDirectionalLight",0x735840,0,-1,1024},{"Light","RemoveExtraDirectionalLights",0x7359E0,0,-1,128},
    {"Light","SetAmbientAndDirectionalColours",0x735A20,0,-1,256},{"Light","ReSetAmbientAndDirectionalColours",0x735C40,0,-1,128},
    {"Light","SetFlashyColours",0x735AB0,0,-1,256},{"Light","SetFlashyColours_Mild",0x735B40,0,-1,256},
    {"Light","SetBrightMarkerColours",0x735BD0,0,-1,128},{"Light","SetDirectionalColours",0x735D70,0,-1,256},
    {"Light","SetAmbientColoursToIndicateRoadGroup",0x735C90,0,-1,512},{"Light","SetFullAmbient",0x735D10,0,-1,128},
    {"Light","SetAmbientColours-void",0x735D30,0,-1,128},{"Light","SetAmbientColours-color",0x735D50,0,-1,256},
    {"Light","SetLightColoursForPedsCarsAndObjects",0x735D90,0,-1,512},
    {"Light","SetLightsForInfraredVisionHeatObjects",0x735E40,0,-1,128},{"Light","StoreAndSetLightsForInfraredVisionHeatObjects",0x735E70,0,-1,256},
    {"Light","RestoreLightsForInfraredVisionHeatObjects",0x735EF0,0,-1,128},{"Light","SetLightsForInfraredVisionDefaultObjects",0x735F20,0,-1,128},
    {"Light","SetLightsForNightVision",0x735F70,0,-1,128},{"Light","GetDayNightBalance",0x6FAB30,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"light27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"light27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("light27");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"light27-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,27> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,27> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(!nRegions){ for(auto& l:lights)Reg(l.data(),l.size()); for(auto& fr:frames)Reg(fr.data(),fr.size()); Reg(world.data(),world.size()); Reg(colourArg.data(),colourArg.size());
        Reg(reinterpret_cast<void*>(LIGHT_GLOBALS),LIGHT_GLOBALS_SIZE); Reg(reinterpret_cast<void*>(INFRARED_COL),0x10); Reg(reinterpret_cast<void*>(0x8D60C0),0x10); }

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {LIGHT_GLOBALS,LIGHT_GLOBALS_SIZE},{COLOURS,0xAC},{RIOTS_MULT,4},{LIGHTS_MULT,4},{LIGHTNING,1},{SUN_DIR,12},
            {BRIGHTNESS,12},{TIME_IN_MS,4},{CLOCK,4},{CURR_AREA,4},{INFRARED_COL,0x10}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        // primitivas RW (las de la DLL son envoltorios del EXE; RpLightSetFlags y RwFrameGetMatrix son macros)
        EntryPatch w1(0x752110,FakeLightCreate), w2(0x7520D0,FakeLightDestroy), w3(0x751A90,FakeLightSetColor), w4(0x751A70,FakeLightSetRadius);
        EntryPatch w5(0x751910,FakeWorldAddLight), w6(0x751960,FakeWorldRemoveLight), w7(0x7F0410,FakeFrameCreate), w8(0x7F05A0,FakeFrameDestroy);
        EntryPatch w9(0x804EF0,FakeSetFrame), w10(0x7F1010,FakeFrameRotate), w11(0x7F0F70,FakeFrameTransform), w12(0x7F0910,FakeFrameUpdateObjects);
        EntryPatch w13(0x7F18A0,FakeMatrixUpdate);
        auto& pAmb=*reinterpret_cast<void**>(P_AMBIENT); auto& pDir=*reinterpret_cast<void**>(P_DIRECT); auto* pExtra=reinterpret_cast<void**>(P_EXTRA);
        auto& numExtra=*reinterpret_cast<int32*>(NUM_EXTRA); auto* strengths=reinterpret_cast<float*>(STRENGTHS);
        auto* colours=reinterpret_cast<float*>(COLOURS); auto& illum=*reinterpret_cast<float*>(COLOURS+0xA4);
        auto& riots=*reinterpret_cast<float*>(RIOTS_MULT); auto& lightsMult=*reinterpret_cast<float*>(LIGHTS_MULT); auto& lightning=*reinterpret_cast<uint8*>(LIGHTNING);
        auto* sun=reinterpret_cast<float*>(SUN_DIR); auto* bright=reinterpret_cast<float*>(BRIGHTNESS); auto& timeMs=*reinterpret_cast<uint32*>(TIME_IN_MS);
        auto& clockSec=*reinterpret_cast<uint16*>(CLOCK); auto& clockMin=*reinterpret_cast<uint8*>(CLOCK+2); auto& clockHour=*reinterpret_cast<uint8*>(CLOCK+3);
        auto& area=*reinterpret_cast<int32*>(CURR_AREA); auto* infrared=reinterpret_cast<float*>(INFRARED_COL);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; }; // sin NaN
                auto col=[&]{ switch(next()%8){case 0:return 0.f;case 1:return 1.f;case 2:return -0.f;case 3:return uni(-0.5f,0.f);case 4:return uni(1.f,3.f);default:return uni(0.f,1.f);} };
                auto randColour=[&](uintptr_t a){ auto* c=reinterpret_cast<float*>(a); for(int k=0;k<4;++k)c[k]=col(); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2727u; if(!stt)stt=1; nev=0; createIdx=0; frameIdx=0; stackAlpha=false;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // luces y marcos falsos con contenido determinista
                    for(auto& l:lights)fillWords(l.data(),l.size()); for(auto& fr:frames)fillWords(fr.data(),fr.size()); fillWords(world.data(),world.size()); fillWords(colourArg.data(),colourArg.size());
                    for(size_t k=0;k<lights.size();++k){ At<uint8>(lights[k].data(),0)=3; At<uint8>(lights[k].data(),2)=uint8(next()%3); At<void*>(lights[k].data(),4)=frames[k].data(); }
                    // globales de luz con contenido coherente (sin NaN)
                    for(uintptr_t c:{DIR_COL,AMB_COL,DIR_FRAME,AMB_FRAME_PCO,AMB_FRAME,LIGHT_GLOBALS,LIGHT_GLOBALS+0x10})randColour(c);
                    for(int k=0;k<6;++k)strengths[k]=col();
                    pAmb=next()%8 ? lights[0].data() : nullptr; pDir=next()%8 ? lights[1].data() : nullptr;
                    for(int k=0;k<6;++k)pExtra[k]=lights[2+k].data();
                    numExtra=int32(next()%8);
                    for(int k=0;k<6;++k)colours[k]=col(); illum=col(); riots=next()%4 ? 1.f : uni(0.f,2.f); lightsMult=next()%4 ? 1.f : uni(0.f,2.f); lightning=uint8(next()%4==0);
                    for(int k=0;k<3;++k)bright[k]=uni(-0.5f,0.5f); timeMs=next(); randColour(INFRARED_COL);
                    auto call0=[&]{ reinterpret_cast<void(__cdecl*)()>(a)(); };
                    auto callF=[&](float v){ reinterpret_cast<void(__cdecl*)(float)>(a)(v); };
                    auto callW=[&]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(world.data()); };

                    if(nm=="ActivateDirectional"||nm=="DeActivateDirectional"||nm=="SetFullAmbient"||nm=="SetAmbientColours-void"||nm=="ReSetAmbientAndDirectionalColours"
                       ||nm=="SetLightsForInfraredVisionHeatObjects"||nm=="StoreAndSetLightsForInfraredVisionHeatObjects"||nm=="RestoreLightsForInfraredVisionHeatObjects")
                        { if(nm.find("Directional")!=std::string::npos&&nm.find("Colours")==std::string::npos)pDir=lights[1].data(); call0(); }
                    else if(nm=="SetLightsForInfraredVisionDefaultObjects"||nm=="SetLightsForNightVision"){ stackAlpha=true; call0(); }
                    else if(nm=="LightsCreate"){ void* w=next()%8 ? world.data() : nullptr; reinterpret_cast<void(__cdecl*)(void*)>(a)(w); }
                    else if(nm=="LightsDestroy"){ pAmb=next()%4 ? lights[0].data() : nullptr; pDir=next()%4 ? lights[1].data() : nullptr;
                        for(int k=0;k<6;++k)pExtra[k]=next()%4 ? lights[2+k].data() : nullptr;
                        void* w=next()%8 ? world.data() : nullptr; reinterpret_cast<void(__cdecl*)(void*)>(a)(w); }
                    else if(nm=="LightsEnable"){ reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(next())); }
                    else if(nm=="SetLightsWithTimeOfDayColour"){
                        switch(next()%8){
                        case 0: sun[0]=0.f; sun[1]=0.f; sun[2]=next()%2 ? 1.f : -1.f; break;          // cruz con Z nula: Normalise pone x=1
                        case 1: sun[0]=uni(-1e-4f,1e-4f); sun[1]=uni(-1e-4f,1e-4f); sun[2]=uni(-1.f,1.f); break;
                        case 2: sun[0]=uni(-3.f,3.f); sun[1]=uni(-3.f,3.f); sun[2]=uni(-3.f,3.f); break; // sin normalizar
                        default: { const float x=uni(-1.f,1.f), y=uni(-1.f,1.f), z=uni(-1.f,1.f); const float m=std::sqrt(x*x+y*y+z*z)+1e-3f; sun[0]=x/m; sun[1]=y/m; sun[2]=z/m; }
                        }
                        callW(); }
                    else if(nm=="WorldReplaceNormalLightsWithScorched"){ pDir=lights[1].data(); reinterpret_cast<void(__cdecl*)(void*,float)>(a)(world.data(),col()); }
                    else if(nm=="WorldReplaceScorchedLightsWithNormal"){ pDir=lights[1].data(); callW(); }
                    else if(nm=="AddAnExtraDirectionalLight"){
                        const uint32 mode=next()%4; numExtra=mode==0 ? int32(next()%4) : mode==1 ? int32(next()%9)-2 : int32(4+next()%3);
                        area=next()%2 ? 0 : int32(1+next()%18);
                        const float pool[]{0.f,-0.f,0.25f,0.5f,1.f};
                        for(int k=0;k<6;++k)strengths[k]=next()%3==0 ? pool[next()%5] : uni(-0.2f,1.5f);
                        auto c=[&]{ return next()%3==0 ? pool[next()%5] : uni(-0.2f,1.5f); };
                        const float r=c(), g=next()%6==0 ? r : c(), b=next()%6==0 ? g : c();
                        reinterpret_cast<void(__cdecl*)(void*,float,float,float,float,float,float)>(a)(world.data(),uni(-1.f,1.f),uni(-1.f,1.f),next()%8==0 ? -0.f : uni(-1.f,1.f),r,g,b); }
                    else if(nm=="RemoveExtraDirectionalLights"){ callW(); }
                    else if(nm=="SetAmbientAndDirectionalColours"||nm=="SetLightColoursForPedsCarsAndObjects"){ callF(next()%6==0 ? 1.f : uni(-0.5f,2.f)); }
                    else if(nm=="SetFlashyColours"||nm=="SetFlashyColours_Mild"){ if(next()%2)timeMs|=0x100; else timeMs&=~0x100u; callF(next()%6==0 ? 1.f : uni(-0.5f,2.f)); }
                    else if(nm=="SetBrightMarkerColours"){ callF(uni(0.f,1.f)); }
                    else if(nm=="SetDirectionalColours"||nm=="SetAmbientColours-color"){ randColour(uintptr_t(colourArg.data()));
                        reinterpret_cast<void(__cdecl*)(void*)>(a)(colourArg.data()); }
                    else if(nm=="SetAmbientColoursToIndicateRoadGroup"){ const uint32 m=next()%4; const int32 idx=m==0 ? int32(next()%7) : m==1 ? int32(next()%32)-16 : m==2 ? int32(next()) : int32(next()%16);
                        reinterpret_cast<void(__cdecl*)(int32)>(a)(idx); out["idx"]=idx; }
                    else if(nm=="GetDayNightBalance"){ const uint32 m=next()%4;
                        clockHour=uint8(m==3 ? next()%256 : next()%24); clockMin=uint8(m==3 ? next()%256 : next()%60); clockSec=uint16(m==2 ? next()%65536 : next()%60);
                        if(next()%4==0){ static const uint8 hs[]{5,6,7,19,20,21,23}; clockHour=hs[next()%7]; clockMin=uint8(next()%2 ? 0 : 59); clockSec=uint16(next()%2 ? 0 : next()%60); }
                        ret=F(reinterpret_cast<float(__cdecl*)()>(a)()); }

                    out["ret"]=ret; out["trace"]=Events();
                    out["globals"]=ReversingProbe::Bytes(reinterpret_cast<void*>(LIGHT_GLOBALS),LIGHT_GLOBALS_SIZE);
                    json ls=json::array(); for(auto& l:lights)ls.push_back(ReversingProbe::Bytes(l.data(),l.size())); out["lights"]=ls;
                    json fs=json::array(); for(auto& fr:frames)fs.push_back(ReversingProbe::Bytes(fr.data(),fr.size())); out["frames"]=fs;
                    out["world"]=ReversingProbe::Bytes(world.data(),world.size()); out["colour_arg"]=ReversingProbe::Bytes(colourArg.data(),colourArg.size());
                    RestoreAll();
                    return out;
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
