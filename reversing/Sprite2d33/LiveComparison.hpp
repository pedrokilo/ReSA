#pragma once
// Banco dentro del motor: CSprite2d (lista de 33 funciones). Original (hooks de CSprite2d desactivados) frente a
// candidato (activados) con fixtures deterministas. Nunca se toca el dispositivo RW ni texturas/rasters reales:
// - Texturas y rasters falsos (punteros a buferes del banco); RwTextureRead/RwTextureDestroy se sustituyen en el EXE
//   (0x7F3AC0/0x7F3820; los de la DLL son envoltorios que llaman al EXE).
// - fpRenderStateSet, fpIm2DRenderPrimitive, fpIm2DRenderIndexedPrimitive y fpIm2DRenderTriangle de RwEngineInstance
//   se cambian por sustitutos con traza solo durante la llamada (el EXE y las macros de la DLL pasan por ellos).
// - CFont::SetWrapx/SetRightJustifyWrap/SetColor/SetEdge/SetFontStyle/SetScale/SetOrientation/PrintString y
//   AsciiToGxtChar (DrawBarChart) se sustituyen con EntryPatch en la direccion del EXE y en el cuerpo de la DLL,
//   resuelto una vez antes de escribir ningun parche; se comprueba que no coincide con un sustituto ni con el EXE.
// - Sustitutos sin reservas de memoria: la traza va a un array fijo y se convierte a json despues de la llamada.
// - maVertices, nextBufferIndex/Vertex, NearScreenZ, RecipNearClip, Scene.m_pRwCamera (camara falsa),
//   RsGlobal.maximumWidth/Height y zBufferNear del dispositivo se fijan por caso y se restauran en cada ejecucion.
// OffsetTexCoordForBilinearFiltering (0x728150) y el constructor de CRGBA (0x7170C0) son codigo del EXE y corren reales.
#include <fstream>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Sprite2d.h"
#include "Font.h"
#include "GxtChar.h"
#include "../ProbeRuntime.hpp"

namespace Sprite2d33Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr uintptr_t SPR_BEGIN=0xC80458, SPR_END=0xC80548;   // nextBufferIndex, nextBufferVertex, NearScreenZ, RecipNearClip, maVertices[8]
constexpr uintptr_t VERTS=0xC80468; constexpr size_t VERTS_SIZE=8*0x1C;
constexpr uintptr_t RADIOSITY=0xC5F958, TEMP_IDX=0xC4B958, SCENE_CAM=0xC1703C, RS_W=0xC17044, RS_H=0xC17048;

// --- buferes falsos ---
alignas(16) inline std::array<std::array<uint8,0x60>,4> textures{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> rasters{};
alignas(16) inline std::array<uint8,0x100> fakeCam{};
alignas(16) inline std::array<uint8,4*0x1C> fakeVerts{};
alignas(16) inline std::array<uint8,16> sprite{};
inline const char* const names[]{"radar_centre","hud_bar","siteM16","loadsc0","fronten1","mouse","back8_top","nm"};
constexpr size_t NNAMES=std::size(names);

// Identificador de puntero sin reservas: 0x80000000|region<<16|desplazamiento, o el valor crudo
inline uint32 Id(const void* p) {
    const auto u=uintptr_t(p); if(!u) return 0;
    const std::pair<uintptr_t,size_t> regs[]{
        {uintptr_t(textures[0].data()),0x60},{uintptr_t(textures[1].data()),0x60},{uintptr_t(textures[2].data()),0x60},{uintptr_t(textures[3].data()),0x60},
        {uintptr_t(rasters[0].data()),0x40},{uintptr_t(rasters[1].data()),0x40},{uintptr_t(rasters[2].data()),0x40},{uintptr_t(rasters[3].data()),0x40},
        {VERTS,VERTS_SIZE},{RADIOSITY,0x1C*1532},{TEMP_IDX,0x2000},{uintptr_t(fakeVerts.data()),fakeVerts.size()},{uintptr_t(fakeCam.data()),fakeCam.size()}};
    for(size_t k=0;k<std::size(regs);++k) if(u>=regs[k].first&&u<regs[k].first+regs[k].second) return 0x80000000u|uint32(k<<16)|uint32(u-regs[k].first);
    for(size_t k=0;k<NNAMES;++k) if(p==names[k]) return 0x40000000u|uint32(k);
    return u;
}

// ---------------------------------------------------------------------------------------------------
// Traza sin reservas
// ---------------------------------------------------------------------------------------------------
enum EvId : uint32 { E_STATE, E_PRIM, E_INDEXED, E_TRI, E_TEXREAD, E_TEXDESTROY, E_GXT, E_WRAPX, E_RJWRAP, E_COLOR, E_EDGE, E_STYLE,
    E_SCALE, E_ORIENT, E_PRINT, E_COUNT };
inline constexpr const char* evNames[E_COUNT]{"render_state_set","im2d_primitive","im2d_indexed","im2d_triangle","texture_read",
    "texture_destroy","ascii_to_gxt","font_wrapx","font_right_wrap","font_color","font_edge","font_style","font_scale","font_orientation","font_print"};
struct Ev { EvId id; uint32 a, b, c, d, e, f; };
inline std::array<Ev,1024> evs{}; inline uint32 nEv{}; inline bool overflow{};
inline void Push(EvId id,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0,uint32 e=0,uint32 f=0) { if(nEv<evs.size())evs[nEv++]={id,a,b,c,d,e,f}; else overflow=true; }
inline json TraceJson() { json t=json::array(); for(uint32 k=0;k<nEv;++k){ const auto& v=evs[k]; t.push_back({evNames[v.id],v.a,v.b,v.c,v.d,v.e,v.f}); } if(overflow)t.push_back("overflow"); return t; }
inline uint64 VHash(const void* v,int32 n){ const size_t k=n<0 ? 0 : size_t(std::min<int32>(n,8)); return Fnv(v,k*0x1C); }

// --- fixtures de los sustitutos (iguales en ambas ejecuciones) ---
inline uint32 texReadMask{}, texReadCalls{};

// --- dispositivo ---
inline RwBool __cdecl XState(RwRenderState st,void* v){ Push(E_STATE,uint32(st),Id(v)); return TRUE; }
inline RwBool __cdecl XPrim(RwPrimitiveType t,RwIm2DVertex* v,RwInt32 n){ const auto h=VHash(v,n); Push(E_PRIM,uint32(t),Id(v),uint32(n),uint32(h),uint32(h>>32)); return TRUE; }
inline RwBool __cdecl XIndexed(RwPrimitiveType t,RwIm2DVertex* v,RwInt32 nv,RwImVertexIndex* i,RwInt32 ni){ Push(E_INDEXED,uint32(t),Id(v),uint32(nv),Id(i),uint32(ni)); return TRUE; }
inline RwBool __cdecl XTri(RwIm2DVertex* v,RwInt32 n,RwInt32 a,RwInt32 b,RwInt32 c){ const auto h=VHash(v,3); Push(E_TRI,Id(v),uint32(n),uint32(a),uint32(b),uint32(c),uint32(h^(h>>32))); return TRUE; }
// --- texturas ---
inline RwTexture* __cdecl XTexRead(const char* n,const char* m){ const bool ok=(texReadMask>>(texReadCalls++%32))&1; Push(E_TEXREAD,Id(n),Id(m),ok);
    return ok ? reinterpret_cast<RwTexture*>(textures[(texReadCalls+(n ? n[0] : 0))%4].data()) : nullptr; }
inline RwBool __cdecl XTexDestroy(RwTexture* t){ Push(E_TEXDESTROY,Id(t)); return TRUE; }
// --- CFont / GXT ---
inline void __cdecl XGxt(const char* s,GxtChar* d){ uint32 h=2166136261u,n=0; for(;s[n]&&n<64;++n){h^=uint8(s[n]);h*=16777619u;d[n]=GxtChar(s[n]);} d[n]=0; Push(E_GXT,h,n); }
inline void __cdecl XWrapx(float v){ Push(E_WRAPX,F(v)); }
inline void __cdecl XRJWrap(float v){ Push(E_RJWRAP,F(v)); }
inline void __cdecl XColor(uint32 c){ Push(E_COLOR,c); }
inline void __cdecl XEdge(uint32 v){ Push(E_EDGE,v&0xFF); }
inline void __cdecl XStyle(uint32 v){ Push(E_STYLE,v&0xFF); }
inline void __cdecl XScale(float w,float h){ Push(E_SCALE,F(w),F(h)); }
inline void __cdecl XOrient(uint32 v){ Push(E_ORIENT,v&0xFF); }
inline void __cdecl XPrint(float x,float y,const GxtChar* t){ uint32 h=2166136261u,n=0; for(;t[n]&&n<64;++n){h^=t[n];h*=16777619u;} Push(E_PRINT,F(x),F(y),h,n); }

// ---------------------------------------------------------------------------------------------------
// Parches: EXE + cuerpo de la DLL (resuelto antes de escribir ningun parche)
// ---------------------------------------------------------------------------------------------------
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }
inline uintptr_t Body(uintptr_t a) {
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Target { const char* name; uintptr_t exe; uintptr_t dll; void* stub; };
inline constexpr size_t NTARGETS=11;
inline std::array<Target,NTARGETS> targets{};
inline void FillTargets() {
    auto V=[](auto f){ return reinterpret_cast<void*>(f); };
    targets={{
        {"AsciiToGxtChar",0x718600,Addr(static_cast<void(*)(const char*,GxtChar*)>(&AsciiToGxtChar)),V(&XGxt)},
        {"CFont::SetWrapx",0x7194D0,Addr(static_cast<void(*)(float)>(&CFont::SetWrapx)),V(&XWrapx)},
        {"CFont::SetRightJustifyWrap",0x7194F0,Addr(static_cast<void(*)(float)>(&CFont::SetRightJustifyWrap)),V(&XRJWrap)},
        {"CFont::SetColor",0x719430,Addr(static_cast<void(*)(CRGBA)>(&CFont::SetColor)),V(&XColor)},
        {"CFont::SetEdge",0x719590,Addr(static_cast<void(*)(int8)>(&CFont::SetEdge)),V(&XEdge)},
        {"CFont::SetFontStyle",0x719490,Addr(static_cast<void(*)(eFontStyle)>(&CFont::SetFontStyle)),V(&XStyle)},
        {"CFont::SetScale",0x719380,Addr(static_cast<void(*)(float,float)>(&CFont::SetScale)),V(&XScale)},
        {"CFont::SetOrientation",0x719610,Addr(static_cast<void(*)(eFontAlignment)>(&CFont::SetOrientation)),V(&XOrient)},
        {"CFont::PrintString",0x71A700,Addr(static_cast<void(*)(float,float,const GxtChar*)>(&CFont::PrintString)),V(&XPrint)},
        // envoltorios de la DLL que llaman al EXE: solo la direccion del EXE
        {"RwTextureRead",0x7F3AC0,0,V(&XTexRead)},
        {"RwTextureDestroy",0x7F3820,0,V(&XTexDestroy)},
    }};
}
inline json ResolveTargets() {
    json bad=json::array();
    for(auto& t:targets)if(t.dll)t.dll=Body(t.dll);
    const uintptr_t tested[]{0x727230,0x727240,0x7281E0,0x728350,0x7284B0,0x7282C0,0x728520,0x7285B0,0x727CC0,0x728640,0x727D60,0x727B60,
        0x727C10,0x727C50,0x727BE0,0x7283B0,0x727350,0x727390,0x7273A0,0x7273D0,0x727320,0x7272E0,0x7279B0,0x727260,0x727B30,0x727270,
        0x7272B0,0x727420,0x727710,0x727A00,0x727590,0x727920,0x727890,0x728150,0x7170C0};
    for(auto& t:targets) {
        if(t.dll&&InExe(t.dll))bad.push_back(std::string(t.name)+": dll body in exe");
        for(auto& s:targets){ if(t.dll&&U(s.stub)==t.dll)bad.push_back(std::string(t.name)+": dll body is a stub"); if(U(s.stub)==t.exe)bad.push_back(std::string(t.name)+": exe is a stub"); }
        for(auto& o:targets)if(&o!=&t&&((t.dll&&(o.dll==t.dll||o.exe==t.dll))||o.exe==t.exe))bad.push_back(std::string(t.name)+": duplicate address");
        for(const uintptr_t r:tested)if(t.exe==r)bad.push_back(std::string(t.name)+": exe address is a tested function");
    }
    return bad;
}
struct AllPatches {
    std::array<std::optional<ReversingProbe::EntryPatch>,2*NTARGETS> p;
    RwRenderStateSetFunction sSet; RwIm2DRenderPrimitiveFunction sPrim; RwIm2DRenderIndexedPrimitiveFunction sIdx; RwIm2DRenderTriangleFunction sTri;
    AllPatches() {
        for(size_t i=0;i<targets.size();++i){ p[2*i].emplace(targets[i].exe,targets[i].stub); if(targets[i].dll)p[2*i+1].emplace(targets[i].dll,targets[i].stub); }
        auto& d=RWSRCGLOBAL(dOpenDevice); sSet=d.fpRenderStateSet; sPrim=d.fpIm2DRenderPrimitive; sIdx=d.fpIm2DRenderIndexedPrimitive; sTri=d.fpIm2DRenderTriangle;
        d.fpRenderStateSet=XState; d.fpIm2DRenderPrimitive=XPrim; d.fpIm2DRenderIndexedPrimitive=XIndexed; d.fpIm2DRenderTriangle=XTri;
    }
    ~AllPatches() {
        auto& d=RWSRCGLOBAL(dOpenDevice); d.fpRenderStateSet=sSet; d.fpIm2DRenderPrimitive=sPrim; d.fpIm2DRenderIndexedPrimitive=sIdx; d.fpIm2DRenderTriangle=sTri;
        for(size_t i=p.size();i-->0;)p[i].reset();
    }
};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,33> fns{{
    {"CSprite2d","Constructor",0x727230,0,-1,64},{"CSprite2d","Delete",0x727240,0,-1,128},{"CSprite2d","Destructor",0x7281E0,0,-1,128},
    {"CSprite2d","SetTexture-",0x727270,0,-1,128},{"CSprite2d","SetTexture-mask",0x7272B0,0,-1,128},
    {"CSprite2d","SetAddressingUV",0x7272E0,0,-1,256},{"CSprite2d","SetAddressing",0x727320,0,-1,256},{"CSprite2d","SetRenderState",0x727B30,0,-1,128},
    {"CSprite2d","Draw-ffffCRGBA",0x7282C0,0,-1,512},{"CSprite2d","Draw-CRectCRGBA",0x728350,0,-1,512},{"CSprite2d","DrawWithBilinearOffset",0x7283B0,0,-1,256},
    {"CSprite2d","Draw-CRectCRGBA4",0x7284B0,0,-1,512},{"CSprite2d","Draw-ffffffffCRGBA",0x728520,0,-1,512},
    {"CSprite2d","SetRecipNearClip",0x727260,0,-1,64},{"CSprite2d","InitPerFrame",0x727350,0,-1,128},{"CSprite2d","IsVertexBufferEmpty",0x727390,0,-1,128},
    {"CSprite2d","IsVertexBufferFull",0x7273A0,0,-1,256},{"CSprite2d","RenderVertexBuffer",0x7273D0,0,-1,128},
    {"CSprite2d","SetVertices-CRectCRGBA4",0x727420,0,-1,512},{"CSprite2d","SetVertices-ffffffffCRGBA",0x727590,0,-1,512},
    {"CSprite2d","SetVertices-CRectCRGBA4ffffffff",0x727710,0,-1,512},{"CSprite2d","SetVertices-iffCRGBA",0x727890,0,-1,256},
    {"CSprite2d","SetVertices-ifCRGBA",0x727920,0,-1,256},{"CSprite2d","SetMaskVertices",0x7279B0,0,-1,256},
    {"CSprite2d","SetVertices-RwD3D9Vertex",0x727A00,0,-1,512},{"CSprite2d","DrawRect-CRGBA",0x727B60,0,-1,512},{"CSprite2d","DrawTxRect",0x727BE0,0,-1,256},
    {"CSprite2d","DrawRect-CRGBA4",0x727C10,0,-1,512},{"CSprite2d","DrawRectXLU",0x727C50,0,-1,256},{"CSprite2d","DrawAnyRect",0x727CC0,0,-1,512},
    {"CSprite2d","Draw2DPolygon",0x7285B0,0,-1,512},{"CSprite2d","DrawBarChart",0x728640,0,-1,1024},{"CSprite2d","DrawCircleAtNearClip",0x727D60,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"sprite2d33-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"sprite2d33-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("sprite2d33");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"sprite2d33-only.txt"); if(f)std::getline(f,only); }
    FillTargets();
    if(const auto bad=ResolveTargets(); !bad.empty()){Write({{"event","error"},{"reason","bad patch targets"},{"details",bad}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,33> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,33> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{{SPR_BEGIN,SPR_END-SPR_BEGIN},{SCENE_CAM,4},{RS_W,8}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    float& zNear=RWSRCGLOBAL(dOpenDevice).zBufferNear; const float savedZNear=zNear;
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); zNear=savedZNear; };

    auto& nbi=*reinterpret_cast<int32*>(0xC80458); auto& nbv=*reinterpret_cast<int32*>(0xC8045C);
    auto& nearZ=*reinterpret_cast<float*>(0xC80460); auto& recip=*reinterpret_cast<float*>(0xC80464);
    auto& sceneCam=*reinterpret_cast<void**>(SCENE_CAM); auto& rsW=*reinterpret_cast<int32*>(RS_W); auto& rsH=*reinterpret_cast<int32*>(RS_H);
    auto* verts=reinterpret_cast<uint8*>(VERTS);

    // argumentos (buferes persistentes: misma direccion en ambas ejecuciones)
    alignas(16) static CRect rect; alignas(16) static std::array<CRGBA,8> cols; alignas(16) static std::array<CVector2D,8> pos, tex;

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; }; // sin NaN
            auto coord=[&]{ switch(next()%8){case 0:return 0.f;case 1:return uni(-2000.f,-1.f);case 2:return float(int32(next()%1281)-100);default:return uni(-50.f,1300.f);} };
            auto randRect=[&]{ rect.left=coord(); rect.top=coord(); rect.right=coord(); rect.bottom=coord(); };
            auto randCol=[&]{ CRGBA c{uint8(next()),uint8(next()),uint8(next()),uint8(next())}; if(next()%3==0)c.a=255; return c; };
            auto randCols=[&]{ for(auto& c:cols)c=randCol(); if(next()%4==0)for(auto& c:cols)c.a=255; };
            auto uvs=[&]{ return next()%4==0 ? float(next()%3)*0.5f : uni(-2.f,3.f); };
            auto anyTex=[&]()->void*{ return next()%5==0 ? nullptr : textures[next()%4].data(); };
            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5B2Du; if(!stt)stt=1;
                nEv=0; overflow=false; texReadMask=next()|next(); texReadCalls=0;
                for(size_t k=0;k<4;++k){ fillWords(textures[k].data(),0x60); At<void*>(textures[k].data(),0)=rasters[k].data(); fillWords(rasters[k].data(),0x40);
                    At<int32>(rasters[k].data(),0xC)=int32(next()%2048)+(next()%8==0 ? -int32(next()%4) : 1); At<int32>(rasters[k].data(),0x10)=int32(next()%2048)+1; }
                fillWords(fakeCam.data(),fakeCam.size()); fillWords(fakeVerts.data(),fakeVerts.size()); fillWords(sprite.data(),sprite.size());
                fillWords(verts,VERTS_SIZE);
                nbi=int32(next()%5000)-200; nbv=int32(next()%1800)-100;
                nearZ=next()%4==0 ? 0.f : uni(0.f,1.f); recip=next()%4==0 ? 1.f : uni(0.0005f,20.f);
                zNear=uni(0.f,1.f); sceneCam=fakeCam.data();
                static const int32 widths[]{640,800,1024,1280,1600,1920,2560,321}, heights[]{448,480,600,720,768,1080,1440,233};
                rsW=widths[next()%8]; rsH=heights[next()%8];
                randRect(); randCols(); for(auto& p:pos)p=CVector2D{coord(),coord()}; for(auto& t:tex)t=CVector2D{uvs(),uvs()};

                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                const uintptr_t a=fn.address; void* const self=sprite.data();
                json out={{"switches",switches}}; json ret=nullptr;
                auto& texPtr=At<void*>(self,0);

                if(nm=="Constructor"){ void* r; { AllPatches ap; r=reinterpret_cast<void*(__fastcall*)(void*,void*)>(a)(self,nullptr); } ret=r==self; }
                else if(nm=="Delete"||nm=="Destructor"){ texPtr=anyTex(); AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(self,nullptr); }
                else if(nm=="SetTexture-"){ texPtr=anyTex(); const char* n=next()%6 ? names[next()%NNAMES] : nullptr; AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*,const char*)>(a)(self,nullptr,n); }
                else if(nm=="SetTexture-mask"){ texPtr=anyTex(); const char* n=next()%6 ? names[next()%NNAMES] : nullptr; const char* m=next()%6 ? names[next()%NNAMES] : nullptr;
                    AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*,const char*,const char*)>(a)(self,nullptr,n,m); }
                else if(nm=="SetAddressingUV"){ texPtr=anyTex(); const uint32 u=next()%4 ? next()%5 : next(), v=next()%4 ? next()%5 : next();
                    AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*,uint32,uint32)>(a)(self,nullptr,u,v); }
                else if(nm=="SetAddressing"){ texPtr=anyTex(); const uint32 m=next()%4 ? next()%5 : next(); AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*,uint32)>(a)(self,nullptr,m); }
                else if(nm=="SetRenderState"){ texPtr=anyTex(); AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(self,nullptr); }
                else if(nm=="Draw-ffffCRGBA"){ texPtr=anyTex(); const float x=coord(),y=coord(),w=next()%6==0 ? -uni(0.f,300.f) : uni(0.f,600.f),h=next()%6==0 ? -uni(0.f,300.f) : uni(0.f,400.f);
                    AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*,float,float,float,float,const CRGBA*)>(a)(self,nullptr,x,y,w,h,&cols[0]); }
                else if(nm=="Draw-CRectCRGBA"){ texPtr=anyTex(); AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*,const CRect*,const CRGBA*)>(a)(self,nullptr,&rect,&cols[1]); }
                else if(nm=="DrawWithBilinearOffset"){ texPtr=textures[next()%4].data(); AllPatches ap; reinterpret_cast<void(__fastcall*)(void*,void*,const CRect*,const CRGBA*)>(a)(self,nullptr,&rect,&cols[2]); }
                else if(nm=="Draw-CRectCRGBA4"){ texPtr=anyTex(); AllPatches ap;
                    reinterpret_cast<void(__fastcall*)(void*,void*,const CRect*,const CRGBA*,const CRGBA*,const CRGBA*,const CRGBA*)>(a)(self,nullptr,&rect,&cols[0],&cols[1],&cols[2],&cols[3]); }
                else if(nm=="Draw-ffffffffCRGBA"){ texPtr=anyTex(); float v[8]; for(auto& c:v)c=coord(); AllPatches ap;
                    reinterpret_cast<void(__fastcall*)(void*,void*,float,float,float,float,float,float,float,float,const CRGBA*)>(a)(self,nullptr,v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],&cols[4]); }
                else if(nm=="SetRecipNearClip"){ AllPatches ap; reinterpret_cast<void(__cdecl*)()>(a)(); }
                else if(nm=="InitPerFrame"){ At<float>(fakeCam.data(),0x80)=next()%4==0 ? 0.1f : uni(0.01f,5.f); if(next()%8==0)At<float>(fakeCam.data(),0x80)=1.f; AllPatches ap; reinterpret_cast<void(__cdecl*)()>(a)(); }
                else if(nm=="IsVertexBufferEmpty"){ nbv=next()%3==0 ? 0 : int32(next()%3)-1+(next()%2 ? 0 : int32(next())); AllPatches ap; ret=reinterpret_cast<uint32(__cdecl*)()>(a)()&0xFF; }
                else if(nm=="IsVertexBufferFull"){ static const int32 vb[]{1531,1532,1533,0,-1,2000}, ib[]{4089,4090,4091,4096,4097,0,-5,5000};
                    nbv=next()%3 ? vb[next()%6] : int32(next()%3000)-500; nbi=next()%3 ? ib[next()%8] : int32(next()%8000)-500; AllPatches ap; ret=reinterpret_cast<uint32(__cdecl*)()>(a)()&0xFF; }
                else if(nm=="RenderVertexBuffer"){ nbv=next()%4==0 ? -int32(next()%5) : next()%4==0 ? 0 : int32(next()%1600); nbi=int32(next()%5000)-100; AllPatches ap; reinterpret_cast<void(__cdecl*)()>(a)(); }
                else if(nm=="SetVertices-CRectCRGBA4"){ AllPatches ap; reinterpret_cast<void(__cdecl*)(const CRect*,const CRGBA*,const CRGBA*,const CRGBA*,const CRGBA*)>(a)(&rect,&cols[0],&cols[1],&cols[2],&cols[3]); }
                else if(nm=="SetVertices-ffffffffCRGBA"){ float v[8]; for(auto& c:v)c=coord(); AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(float,float,float,float,float,float,float,float,const CRGBA*,const CRGBA*,const CRGBA*,const CRGBA*)>(a)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],&cols[0],&cols[1],&cols[2],&cols[3]); }
                else if(nm=="SetVertices-CRectCRGBA4ffffffff"){ float u[8]; for(auto& c:u)c=uvs(); AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(const CRect*,const CRGBA*,const CRGBA*,const CRGBA*,const CRGBA*,float,float,float,float,float,float,float,float)>(a)(&rect,&cols[0],&cols[1],&cols[2],&cols[3],u[0],u[1],u[2],u[3],u[4],u[5],u[6],u[7]); }
                else if(nm=="SetVertices-iffCRGBA"){ const int32 n=next()%8==0 ? -int32(next()%3) : int32(next()%9); AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(int32,const CVector2D*,const CVector2D*,const CRGBA*)>(a)(n,pos.data(),tex.data(),&cols[next()%8]); }
                else if(nm=="SetVertices-ifCRGBA"){ const int32 n=next()%8==0 ? -int32(next()%3) : int32(next()%9); AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(int32,const CVector2D*,const CRGBA*)>(a)(n,pos.data(),cols.data()); }
                else if(nm=="SetMaskVertices"){ const int32 n=next()%8==0 ? -int32(next()%3) : int32(next()%9); const float d=next()%3==0 ? nearZ : uni(-1.f,2.f); AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(int32,const CVector2D*,float)>(a)(n,pos.data(),d); }
                else if(nm=="SetVertices-RwD3D9Vertex"){ float u[8]; for(auto& c:u)c=uvs(); void* vb=next()%3 ? static_cast<void*>(fakeVerts.data()) : static_cast<void*>(verts+0x1C*(next()%5)); AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(void*,const CRect*,const CRGBA*,const CRGBA*,const CRGBA*,const CRGBA*,float,float,float,float,float,float,float,float)>(a)(vb,&rect,&cols[0],&cols[1],&cols[2],&cols[3],u[0],u[1],u[2],u[3],u[4],u[5],u[6],u[7]); }
                else if(nm=="DrawRect-CRGBA"){ AllPatches ap; reinterpret_cast<void(__cdecl*)(const CRect*,const CRGBA*)>(a)(&rect,&cols[5]); }
                else if(nm=="DrawTxRect"){ AllPatches ap; reinterpret_cast<void(__cdecl*)(const CRect*,const CRGBA*)>(a)(&rect,&cols[6]); }
                else if(nm=="DrawRect-CRGBA4"||nm=="DrawRectXLU"){ AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(const CRect*,const CRGBA*,const CRGBA*,const CRGBA*,const CRGBA*)>(a)(&rect,&cols[0],&cols[1],&cols[2],&cols[3]); }
                else if(nm=="DrawAnyRect"){ float v[8]; for(auto& c:v)c=coord(); AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(float,float,float,float,float,float,float,float,const CRGBA*,const CRGBA*,const CRGBA*,const CRGBA*)>(a)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],&cols[0],&cols[1],&cols[2],&cols[3]); }
                else if(nm=="Draw2DPolygon"){ float v[8]; for(auto& c:v)c=coord(); AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(float,float,float,float,float,float,float,float,const CRGBA*)>(a)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],&cols[7]); }
                else if(nm=="DrawBarChart"){
                    const float x=coord(), y=coord();
                    const uint32 w=next()%6==0 ? next()%65536 : next()%600, h=next()%256;
                    const uint32 pct=next()%3==0, border=next()%2;
                    float p; switch(next()%8){case 0:p=0.f;break;case 1:p=100.f;break;case 2:p=-uni(0.f,80.f);break;case 3:p=uni(100.f,400.f);break;default:p=uni(0.f,100.f);}
                    if(!pct&&next()%8==0)p=uni(1e3f,1e9f);
                    const uint32 add=next()%3==0 ? 0 : (next()%2 ? next()%256 : next()%40);
                    CRGBA c=cols[0], ac=cols[1];
                    AllPatches ap;
                    reinterpret_cast<void(__cdecl*)(float,float,uint32,uint32,float,uint32,uint32,uint32,CRGBA,CRGBA)>(a)(x,y,w,h,p,add,pct,border,c,ac); }
                else if(nm=="DrawCircleAtNearClip"){
                    const CVector2D c{coord(),coord()}; const float s=next()%6==0 ? -uni(0.f,100.f) : uni(0.f,300.f);
                    int32 n; switch(next()%8){case 0:n=-int32(1+next()%50);break;case 1:n=int32(1+next()%400);break;case 2:n=int32(1+next()%4);break;default:n=int32(1+next()%64);}
                    AllPatches ap; reinterpret_cast<void(__cdecl*)(const CVector2D*,float,const CRGBA*,int32)>(a)(&c,s,&cols[3],n); }

                out["verts"]=ReversingProbe::Bytes(verts,VERTS_SIZE);
                out["globals"]=json::array({nbi,nbv,F(nearZ),F(recip),Id(sceneCam),rsW,rsH,F(zNear)});
                out["sprite"]=Id(At<void*>(self,0)); out["sprite_tail"]=ReversingProbe::Bytes(sprite.data()+4,12);
                json t=json::array(); for(auto& x:textures)t.push_back(At<uint32>(x.data(),0x50)); out["tex_addressing"]=t;
                out["textures"]=Fnv(textures.data(),sizeof(textures)); out["rasters"]=Fnv(rasters.data(),sizeof(rasters));
                out["fake_verts"]=ReversingProbe::Bytes(fakeVerts.data(),fakeVerts.size());
                out["ret"]=ret; out["trace"]=TraceJson();
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=zNear==savedZNear; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
