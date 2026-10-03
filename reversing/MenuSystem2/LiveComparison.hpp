#pragma once
// Banco dentro del motor: CMenuSystem (20 funciones; FillGridWithCarColours ya validada en reversing/CMenuSystem).
// Original (hooks de CMenuSystem desactivados) frente a candidato (activados) con fixtures deterministas por caso.
// MenuNumber[0/1] apuntan a dos menus falsos (0x418 bytes con guardas de 64 bytes) rellenados de forma coherente;
// operator new/delete (0x82119A/0x8214BD/0x821195/0x8213AE) se sustituyen solo durante CreateNewMenu, SwitchOffMenu e
// Initialise y devuelven una arena fija prellenada igual en ambas ejecuciones (traza en array fijo, sin reservar).
// Se simulan y trazan: CMenuManager::DrawWindow, CFont (SetFontStyle, SetScale, SetDropShadowPosition, SetDropColor,
// SetOrientation, SetColor, GetStringWidth, PrintString), CText::Get, CSprite2d::DrawRect y
// CAudioEngine::ReportFrontendAudioEvent. El estado del pad 0, del teclado, el historial de sticks analogicos, las
// pausas y los globales del sistema de menus se fijan por caso, se comparan y se restauran al terminar.
// Se compara: retorno, bytes de los tres almacenes (dos menus + arena, con guardas), globales, historial de sticks,
// pausas, estado de CFont, estado de entrada y la traza.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MenuSystem.h"
#include "../ProbeRuntime.hpp"

namespace MenuSys2Live {
using Menu = CMenuSystem::Menu;
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- almacen: [0] y [1] para MenuNumber, [2] arena de operator new; cada uno con 64 bytes de guarda a cada lado ---
constexpr size_t GUARD=64, SLOT=GUARD+sizeof(Menu)+GUARD;
static_assert(sizeof(Menu)==0x418);
alignas(16) inline std::array<uint8,3*SLOT> raw{};
inline Menu* M(int k){ return reinterpret_cast<Menu*>(raw.data()+k*SLOT+GUARD); }
inline uint8* MB(int k){ return raw.data()+k*SLOT+GUARD; }
// etiqueta de puntero sin reservar memoria: 0 nulo, 1..3 almacenes, si no 0xF0000000|bits bajos
inline uint32 Tag(const void* p){ if(!p)return 0; for(int k=0;k<3;++k)if(p==M(k))return uint32(k+1); return 0xF0000000u|(uint32(uintptr_t(p))&0x0FFFFFFFu); }

// --- globales del sistema de menus ---
inline auto& MenuNumber=StaticRef<std::array<Menu*,2>>(0xBA82D8);
inline auto& MenuFlags=StaticRef<std::array<uint8,4>>(0xBA82E0); // MenuInUse[2], CurrentMenuInUse, num_menus_in_use

// --- registro sin reservas (operator new sustituido) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,256> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }
inline void* __cdecl FakeNew(size_t n){ Push(1,uint32(n)); return n<=sizeof(Menu) ? M(2) : nullptr; }
inline void* __cdecl FakeNewArray(size_t n){ Push(3,uint32(n)); return n<=sizeof(Menu) ? M(2) : nullptr; }
inline void __cdecl FakeDelete(void* p){ Push(2,Tag(p)); }
inline void __cdecl FakeDeleteArray(void* p){ Push(4,Tag(p)); }

// --- texto ---
inline json Txt(const void* p){
    if(!p)return nullptr;
    auto* s=static_cast<const uint8*>(p); size_t n=0; while(n<512&&s[n])++n;
    bool ascii=true; for(size_t i=0;i<n;++i)ascii&=s[i]<0x80;
    if(ascii)return std::string(reinterpret_cast<const char*>(s),n);
    return ReversingProbe::Bytes(s,n);
}
inline json Where(const void* p){ // puntero dentro de un almacen -> [almacen, desplazamiento]; si no, contenido
    for(int k=0;k<3;++k){ const auto d=intptr_t(p)-intptr_t(MB(k)); if(d>=-intptr_t(GUARD)&&d<intptr_t(sizeof(Menu)+GUARD))return json::array({k,d,Txt(p)}); }
    return Txt(p);
}
inline json Rect(const CRect& r){ auto* f=reinterpret_cast<const float*>(&r); return json::array({F(f[0]),F(f[1]),F(f[2]),F(f[3])}); }
inline json Col(const CRGBA& c){ return json::array({c.r,c.g,c.b,c.a}); }

// --- stubs trazados ---
inline float curScaleX{1.f};
inline void __fastcall FakeDrawWindow(CMenuManager* self,void*,const CRect& r,const char* key,uint8 color,CRGBA back,bool unused,bool bg){
    trace.push_back({"draw_window",self==&FrontEndMenuManager,Rect(r),Where(key),color,Col(back),unused,bg}); }
inline void __cdecl FakeSetFontStyle(eFontStyle s){ trace.push_back({"font_style",int(s)}); }
inline void __cdecl FakeSetScale(float w,float h){ curScaleX=w; trace.push_back({"font_scale",F(w),F(h)}); }
inline void __cdecl FakeSetDropShadowPosition(int16 v){ trace.push_back({"font_shadow",v}); }
inline void __cdecl FakeSetDropColor(CRGBA c){ trace.push_back({"font_drop_color",Col(c)}); }
inline void __cdecl FakeSetOrientation(eFontAlignment a){ trace.push_back({"font_orientation",int(a)}); }
inline void __cdecl FakeSetColor(CRGBA c){ trace.push_back({"font_color",Col(c)}); }
// ancho determinista a partir del contenido y de la escala vigente (el bucle de reduccion de escala termina porque
// la escala acaba siendo negativa y el ancho depende de ella)
inline float __cdecl FakeGetStringWidth(const GxtChar* s,bool full,bool script){
    size_t n=0; while(s&&n<512&&s[n])++n;
    const float w=float(n+1)*curScaleX*9.f;
    trace.push_back({"string_width",Txt(s),full,script,F(w)}); return w; }
inline void __cdecl FakePrintString(float x,float y,const GxtChar* s){ trace.push_back({"print_string",F(x),F(y),Where(s)}); }
inline void __cdecl FakeDrawRect(const CRect& r,const CRGBA& c){ trace.push_back({"draw_rect",Rect(r),Col(c)}); }
inline void __fastcall FakeReportFrontend(CAudioEngine* self,void*,eAudioEvents ev,float vol,float speed){
    trace.push_back({"frontend_audio",self==&AudioEngine,int(ev),F(vol),F(speed)}); }
// CText::Get: cadenas GXT deterministas derivadas de la clave, en un banco rotatorio de buffers
inline std::array<std::array<GxtChar,160>,64> pool{}; inline uint32 poolNext{};
inline const GxtChar* __fastcall FakeTextGet(CText* self,void*,const char* key){
    size_t n=0; while(key&&n<64&&key[n])++n;
    const std::string k(key ? key : "",n);
    trace.push_back({"text_get",self==&TheText,Where(key)});
    std::string s;
    switch(uint32(Fnv(k.data(),k.size()))%5){
    case 0: s=k; break;
    case 1: s=k+" ~1~"; break;
    case 2: s="~1~ "+k+" ~1~"; break;
    case 3: s=k+" "+k+" "+k+" "+k; break;
    default: s="$~1~ "+k+" OK"; break;
    }
    if(s.size()>150)s.resize(150);
    auto& out=pool[poolNext++%pool.size()];
    std::memcpy(out.data(),s.data(),s.size()); out[s.size()]=0;
    return out.data();
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,20> fns{{
    {"CMenuSystem","ActivateItems",0x581990,0,-1,1024},{"CMenuSystem","ActivateOneItem",0x581B30,0,-1,1024},
    {"CMenuSystem","CheckForAccept",0x5807C0,0,-1,256},{"CMenuSystem","CheckForSelected",0x5807E0,0,-1,256},
    {"CMenuSystem","CreateNewMenu",0x582300,0,-1,1024},{"CMenuSystem","DisplayGridMenu",0x5816E0,0,-1,512},
    {"CMenuSystem","DisplayStandardMenu",0x580E00,0,-1,2048},{"CMenuSystem","GetCarColourFromGrid",0x5822B0,0,-1,256},
    {"CMenuSystem","HighlightOneItem",0x581C10,0,-1,512},{"CMenuSystem","Initialise",0x5822D0,0,-1,256},
    {"CMenuSystem","Input",0x5825D0,0,-1,1024},{"CMenuSystem","InputGridMenu",0x580BD0,0,-1,1024},
    {"CMenuSystem","InputStandardMenu",0x580800,0,-1,2048},{"CMenuSystem","InsertMenu",0x581E00,0,-1,1024},
    {"CMenuSystem","InsertOneMenuItemWithNumber",0x581D70,0,-1,1024},{"CMenuSystem","Process",0x582630,0,-1,2048},
    {"CMenuSystem","SetActiveMenuItem",0x5820C0,0,-1,256},{"CMenuSystem","SetColumnOrientation",0x582080,0,-1,256},
    {"CMenuSystem","SetColumnWidth",0x582050,0,-1,512},{"CMenuSystem","SwitchOffMenu",0x580750,0,-1,256},
}};
// FillGridWithCarColours (0x5820E0) tambien se conmuta para que CreateNewMenu original use la version del EXE.
inline const char* const extraHooks[]{"FillGridWithCarColours"};

constexpr uintptr_t STICKS=0xB736F0; constexpr size_t NSTICKS=16;   // historial de GetAnaloguePadUp/Down/Left/Right (int16 en +0/+4/+8/+C)
constexpr uintptr_t PAUSE=0xB7CB48;                                  // m_CodePause, m_UserPause
constexpr uintptr_t FONT=0xC71A54; constexpr size_t NFONT=0xC71AA0-0xC71A54; // estado estatico de CFont (no deberia cambiar)

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"menusys2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"menusys2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("menusys2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"menusys2-only.txt"); if(f)std::getline(f,only); }

    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks;
    for(size_t f=0;f<fns.size();++f){ auto* h=FindHook(fns[f].cls,fns[f].name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;} hooks.push_back(h); }
    for(auto* name:extraHooks){ auto* h=FindHook("CMenuSystem",name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string("CMenuSystem::")+name}});return;} hooks.push_back(h); }
    std::vector<char> enabled(hooks.size()),locked(hooks.size());
    for(size_t k=0;k<hooks.size();++k){enabled[k]=hooks[k]->Hooked();locked[k]=hooks[k]->Locked();hooks[k]->LockState(false);}

    // estado global guardado
    const auto savedNumber=MenuNumber; const auto savedFlags=MenuFlags;
    auto* padBytes=reinterpret_cast<uint8*>(&CPad::Pads[0]); constexpr size_t NPAD=sizeof(CPad)*2; const std::vector<uint8> savedPads(padBytes,padBytes+NPAD);
    auto* newKeys=reinterpret_cast<uint8*>(&CPad::NewKeyState); auto* oldKeys=reinterpret_cast<uint8*>(&CPad::OldKeyState); constexpr size_t NKEY=sizeof(CKeyboardState);
    const std::vector<uint8> savedNewKeys(newKeys,newKeys+NKEY), savedOldKeys(oldKeys,oldKeys+NKEY);
    auto* sticks=reinterpret_cast<uint8*>(STICKS); const std::vector<uint8> savedSticks(sticks,sticks+NSTICKS);
    auto* pause=reinterpret_cast<uint8*>(PAUSE); const std::array<uint8,2> savedPause{pause[0],pause[1]};
    auto* font=reinterpret_cast<uint8*>(FONT); const std::vector<uint8> savedFont(font,font+NFONT);

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch w1(bit_cast<uintptr_t>(&CMenuManager::DrawWindow),FakeDrawWindow), w1e(0x573EE0,FakeDrawWindow);
        EntryPatch f1(bit_cast<uintptr_t>(&CFont::SetFontStyle),FakeSetFontStyle), f1e(0x719490,FakeSetFontStyle);
        EntryPatch f2(bit_cast<uintptr_t>(&CFont::SetScale),FakeSetScale), f2e(0x719380,FakeSetScale);
        EntryPatch f3(bit_cast<uintptr_t>(&CFont::SetDropShadowPosition),FakeSetDropShadowPosition), f3e(0x719570,FakeSetDropShadowPosition);
        EntryPatch f4(bit_cast<uintptr_t>(&CFont::SetDropColor),FakeSetDropColor), f4e(0x719510,FakeSetDropColor);
        EntryPatch f5(bit_cast<uintptr_t>(&CFont::SetOrientation),FakeSetOrientation), f5e(0x719610,FakeSetOrientation);
        EntryPatch f6(bit_cast<uintptr_t>(&CFont::SetColor),FakeSetColor), f6e(0x719430,FakeSetColor);
        EntryPatch f7(bit_cast<uintptr_t>(&CFont::GetStringWidth),FakeGetStringWidth), f7e(0x71A0E0,FakeGetStringWidth);
        EntryPatch f8(bit_cast<uintptr_t>(&CFont::PrintString),FakePrintString), f8e(0x71A700,FakePrintString);
        EntryPatch t1(bit_cast<uintptr_t>(&CText::Get),FakeTextGet), t1e(0x6A0050,FakeTextGet);
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)),FakeDrawRect), s1e(0x727B60,FakeDrawRect);
        EntryPatch a1(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),FakeReportFrontend), a1e(0x506EA0,FakeReportFrontend);

        static const char* const keys[]{"FEM_OK","FEM_ON","FEM_OFF","CARSHOP","SHOP_1","BB_19","MOD_01","GAMB_A","DUMMY","IE_N1","WOODEN",
                                        "PAINT","BUYIT","XX","A","TUNE_12","RS_OPT","BARB_1","TAT_RG","CLOTHES","GYM_5","HAIR_02"};
        static const int16 stickValues[]{0,0,0,0,-128,127,-16,-15,-14,16,15,14,-6,-5,-4,4,5,6,-100,100};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto key=[&]()->const char*{ return keys[next()%std::size(keys)]; };
                auto str=[&]()->const char*{ const uint32 r=next()%10; return r==0 ? nullptr : r==1 ? "" : key(); }; // nulo, vacia o clave
                auto put=[&](char* dst,const char* s){ std::memcpy(dst,s,std::strlen(s)+1); };               // deja basura tras el NUL
                auto number=[&]()->int32{ switch(next()%5){case 0:case 1:return -1;case 2:return int32(next()%10);case 3:return int32(next()%100000);default:return int32(next());} };
                // kind 0 = estandar, 1 = cuadricula, 2 = tipo desconocido (con contenido estandar)
                auto fillMenu=[&](Menu& m,int kind){
                    m.m_nType=kind==0 ? CMenuSystem::MENU_TYPE_DEFAULT : kind==1 ? CMenuSystem::MENU_TYPE_GRID : static_cast<CMenuSystem::eMenuType>(2+next()%254);
                    for(auto& c:m.m_anUsedCarColors)c=uint8(next()%128);
                    for(auto& col:m.m_aaacRowTitles)for(auto& row:col){ if(next()%3)put(row,key()); else row[0]=0; }
                    for(auto& col:m.m_aanNumberInRowTitle)for(auto& v:col)v=number();
                    for(auto& col:m.m_aadw2ndNumberInRowTitle)for(auto& v:col)v=number();
                    for(auto& h:m.m_aacColumnHeaders){ if(next()%2)put(h,key()); else h[0]=0; }
                    if(next()%4)put(m.m_szTitle,key()); else m.m_szTitle[0]=0;
                    for(auto& s:m.m_abRowSelectable)s=next()%3!=0;
                    for(auto& b:m.m_abRowAlreadyBought)b=uint8(next()%4==0);
                    static const int8 aligns[]{-1,0,1,2,0,1,2,3};
                    for(auto& a:m.m_anColumnAlignment)a=static_cast<eFontAlignment>(aligns[1+next()%7]);
                    for(auto& a:m.m_anColumnHeaderAlignment)a=static_cast<eFontAlignment>(next()%2 ? -1 : aligns[next()%8]);
                    if(kind==1){
                        m.m_nNumColumns=uint8(next()%9); m.m_nNumRows=next()%4 ? uint8(m.m_nNumColumns*m.m_nNumColumns) : uint8(next()%65);
                        m.m_afColumnWidth[0]=next()%8==0 ? uni(0.f,4.f) : uni(4.f,60.f);
                        for(int k=1;k<4;++k)m.m_afColumnWidth[k]=uni(0.f,250.f);
                    } else {
                        m.m_nNumColumns=uint8(next()%5); m.m_nNumRows=uint8(next()%13);
                        for(auto& w:m.m_afColumnWidth)w=next()%6==0 ? uni(0.f,10.f) : uni(20.f,250.f);
                    }
                    for(auto& b:m.m_abColumnInteractive)b=next()%2;
                    m.m_vPosn.x=uni(0.f,500.f); m.m_vPosn.y=uni(0.f,400.f);
                    m.m_bColumnBackground=next()%2;
                    if(kind==1){ const int top=std::min<int>(m.m_nNumRows,63); m.m_nSelectedRow=int8(int(next()%uint32(top+2))-1); }
                    else m.m_nSelectedRow=int8(int(next()%14)-1);
                    switch(next()%4){case 0:m.m_nAcceptedRow=-99;break;case 1:m.m_nAcceptedRow=m.m_nSelectedRow;break;default:m.m_nAcceptedRow=int8(int(next()%14)-1);}
                };
                auto randState=[&](CControllerState& s){ auto* w=reinterpret_cast<int16*>(&s); for(size_t k=0;k<sizeof(s)/2;++k)w[k]=next()%3==0 ? 255 : 0;
                    s.LeftStickX=stickValues[next()%std::size(stickValues)]; s.LeftStickY=stickValues[next()%std::size(stickValues)]; };
                auto randKeys=[&](uint8* k){ auto* w=reinterpret_cast<int16*>(k); for(size_t j=0;j<NKEY/2;++j)w[j]=next()%4==0 ? 255 : 0; };

                std::vector<uint8> rawO, rawC;
                auto run=[&](bool candidate,std::vector<uint8>& rawOut)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3E25u; if(!stt)stt=1; trace=json::array(); nrec=0; poolNext=0; curScaleX=1.f;
                    // memoria de los tres almacenes (guardas incluidas) aleatoria y despues menus coherentes
                    for(auto& b:raw)b=uint8(next());
                    const bool gridFn=nm=="DisplayGridMenu"||nm=="InputGridMenu"||nm=="GetCarColourFromGrid";
                    const bool anyType=nm=="Input"||nm=="Process";
                    for(int k=0;k<2;++k){ int kind=gridFn ? 1 : 0; if(anyType){ const uint32 r=next()%8; kind=r<4 ? 0 : r<7 ? 1 : 2; } fillMenu(*M(k),kind); }
                    // entrada: pad 0, teclado, historial de sticks y pausas
                    randState(CPad::Pads[0].NewState); randState(CPad::Pads[0].OldState);
                    CPad::Pads[0].DisablePlayerControls=next()%5==0 ? uint16(next()) : 0;
                    randKeys(newKeys); randKeys(oldKeys);
                    for(int k=0;k<4;++k)*reinterpret_cast<int16*>(sticks+4*k)=stickValues[next()%std::size(stickValues)];
                    pause[0]=uint8(next()%6==0); pause[1]=uint8(next()%4==0);
                    std::memcpy(font,savedFont.data(),NFONT);
                    // globales del sistema de menus
                    MenuNumber={M(0),M(1)};
                    MenuFlags[0]=uint8(next()%4!=0); MenuFlags[1]=uint8(next()%4!=0);
                    static const int8 currents[]{-2,-1,0,1,2,3,0,1};
                    MenuFlags[2]=uint8(nm=="Process" ? int8(next()%2) : currents[next()%8]);
                    MenuFlags[3]=uint8(next()%3);
                    if(nm=="CheckForAccept"||nm=="CheckForSelected"||nm=="Initialise"||nm=="SwitchOffMenu")
                        for(int k=0;k<2;++k)if(!MenuFlags[k]&&next()%2)MenuNumber[k]=nullptr;
                    MenuId newId=0;
                    if(nm=="CreateNewMenu"){ newId=MenuId(next()%2); MenuFlags[3]=newId; MenuFlags[newId]=0; if(next()%2)MenuNumber[newId]=nullptr; }

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    const MenuId id=MenuId(next()%2);
                    json ret=nullptr;

                    if(nm=="ActivateItems"){ bool b[12]; for(auto& x:b)x=next()%3!=0;
                        reinterpret_cast<void(__cdecl*)(MenuId,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool)>(a)(id,b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7],b[8],b[9],b[10],b[11]); }
                    else if(nm=="ActivateOneItem"){ const uint8 row=uint8(next()%13); const bool en=next()%2; reinterpret_cast<void(__cdecl*)(MenuId,uint8,bool)>(a)(id,row,en); }
                    else if(nm=="CheckForAccept"||nm=="CheckForSelected") ret=int(reinterpret_cast<int8(__cdecl*)(MenuId)>(a)(id));
                    else if(nm=="CreateNewMenu"){
                        const uint32 r=next()%10; const auto type=static_cast<CMenuSystem::eMenuType>(r<4 ? 0 : r<8 ? 1 : 2+next()%254);
                        const char* title=next()%6==0 ? nullptr : next()%5==0 ? "" : key();
                        const float x=uni(0.f,500.f), y=uni(0.f,400.f), width=next()%5==0 ? uni(0.f,10.f) : uni(10.f,250.f);
                        const uint8 columns=uint8(next()%11); const bool interactive=next()%2, background=next()%2;
                        const auto align=static_cast<eFontAlignment>(int8(int(next()%4)-1));
                        MenuId r2;
                        { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNewArray),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDeleteArray);
                          r2=reinterpret_cast<MenuId(__cdecl*)(CMenuSystem::eMenuType,const char*,float,float,float,uint8,bool,bool,eFontAlignment)>(a)(type,title,x,y,width,columns,interactive,background,align); }
                        ret=int(r2);
                    }
                    else if(nm=="DisplayGridMenu"){ const bool fade=next()%2; reinterpret_cast<void(__cdecl*)(MenuId,bool)>(a)(id,fade); }
                    else if(nm=="DisplayStandardMenu"){ const bool bright=next()%2; reinterpret_cast<void(__cdecl*)(MenuId,bool)>(a)(id,bright); }
                    else if(nm=="GetCarColourFromGrid"){ const uint8 c=uint8(next()%64); ret=int(reinterpret_cast<uint8(__cdecl*)(MenuId,uint8)>(a)(id,c)); }
                    else if(nm=="HighlightOneItem"){ const uint8 item=uint8(next()%12); const bool bought=next()%2; reinterpret_cast<void(__cdecl*)(MenuId,uint8,bool)>(a)(id,item,bought); }
                    else if(nm=="Initialise"||nm=="SwitchOffMenu"){
                        EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNewArray),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDeleteArray);
                        if(nm=="Initialise")reinterpret_cast<void(__cdecl*)()>(a)(); else reinterpret_cast<void(__cdecl*)(MenuId)>(a)(id);
                    }
                    else if(nm=="Input"||nm=="InputGridMenu"||nm=="InputStandardMenu") reinterpret_cast<void(__cdecl*)(MenuId)>(a)(id);
                    else if(nm=="InsertMenu"){ const uint8 column=uint8(next()%4); const char* s[13]; for(auto& p:s)p=str();
                        reinterpret_cast<void(__cdecl*)(MenuId,uint8,const char*,const char*,const char*,const char*,const char*,const char*,const char*,const char*,const char*,const char*,const char*,const char*,const char*)>(a)
                            (id,column,s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],s[10],s[11],s[12]); }
                    else if(nm=="InsertOneMenuItemWithNumber"){ const uint8 column=uint8(next()%4), row=uint8(next()%12); const char* text=str(); const int32 n1=number(), n2=number();
                        reinterpret_cast<void(__cdecl*)(MenuId,uint8,uint8,const char*,int32,int32)>(a)(id,column,row,text,n1,n2); }
                    else if(nm=="Process"){ const uint32 r=next()%4; const int8 which=r<2 ? int8(-99) : int8(r-2); reinterpret_cast<void(__cdecl*)(int8)>(a)(which); }
                    else if(nm=="SetActiveMenuItem"){ const int8 item=int8(next()%4 ? int(next()%14)-1 : int(next()&0xFF)); reinterpret_cast<void(__cdecl*)(MenuId,int8)>(a)(id,item); }
                    else if(nm=="SetColumnOrientation"){ const uint8 column=uint8(next()%4), o=uint8(next()%3 ? next()%3 : next()); reinterpret_cast<void(__cdecl*)(MenuId,uint8,uint8)>(a)(id,column,o); }
                    else if(nm=="SetColumnWidth"){ const uint8 column=uint8(next()%4); const uint16 w=uint16(next()%3 ? next()%400 : next()); reinterpret_cast<void(__cdecl*)(MenuId,uint8,uint16)>(a)(id,column,w); }

                    rawOut.assign(raw.begin(),raw.end());
                    json out={{"switches",switches},{"ret",ret},{"trace",trace},{"recs",Recs()},
                              {"memory",Fnv(raw.data(),raw.size())},
                              {"globals",json::array({Tag(MenuNumber[0]),Tag(MenuNumber[1]),MenuFlags[0],MenuFlags[1],int(int8(MenuFlags[2])),MenuFlags[3]})},
                              {"sticks",ReversingProbe::Bytes(sticks,NSTICKS)},{"pause",json::array({pause[0],pause[1]})},
                              {"font",Fnv(font,NFONT)},{"input",Fnv(newKeys,NKEY,Fnv(oldKeys,NKEY,Fnv(padBytes,NPAD)))}};
                    return out;
                };
                const auto original=run(false,rawO), candidate=run(true,rawC);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    json bytes=json::array(); // [almacen, desplazamiento respecto al menu (negativo = guarda previa), original, candidato]
                    for(size_t k=0;k<rawO.size()&&bytes.size()<64;++k)if(rawO[k]!=rawC[k])bytes.push_back({k/SLOT,int(k%SLOT)-int(GUARD),rawO[k],rawC[k]});
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"byte_diffs",bytes},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t k=0;k<hooks.size();++k){hooks[k]->State(enabled[k]);hooks[k]->LockState(locked[k]);}
    MenuNumber=savedNumber; MenuFlags=savedFlags;
    std::memcpy(padBytes,savedPads.data(),NPAD); std::memcpy(newKeys,savedNewKeys.data(),NKEY); std::memcpy(oldKeys,savedOldKeys.data(),NKEY);
    std::memcpy(sticks,savedSticks.data(),NSTICKS); pause[0]=savedPause[0]; pause[1]=savedPause[1]; std::memcpy(font,savedFont.data(),NFONT);
    const bool restored=MenuNumber==savedNumber&&MenuFlags==savedFlags&&std::memcmp(padBytes,savedPads.data(),NPAD)==0&&std::memcmp(newKeys,savedNewKeys.data(),NKEY)==0
        &&std::memcmp(oldKeys,savedOldKeys.data(),NKEY)==0&&std::memcmp(sticks,savedSticks.data(),NSTICKS)==0&&pause[0]==savedPause[0]&&pause[1]==savedPause[1]
        &&std::memcmp(font,savedFont.data(),NFONT)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
