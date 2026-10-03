#pragma once
// Banco dentro del motor: CMessages (28 funciones). Original (hooks del lote desactivados) frente a candidato
// con fixtures deterministas. Estado comparado byte a byte (punteros de texto traducidos a nombres del pool
// solo al volcar diferencias): PreviousBriefs, BriefMessages, BIGMessages, CHud::BigMessageX y
// CHud::m_BigMessage, mas dos bufferes del llamador de 512 bytes. Entradas de solo lectura fijadas por caso:
// CTimer::m_snTimeInMilliseconds y CTheScripts::bDrawSubtitlesBeforeFade. Se simulan y trazan:
// CHud::SetBigMessage, CHud::SetMessage, CHud::GetRidOfAllHudMessages y
// CControllerConfigManager::GetGxtStringOfCommandKeys (texto de teclas determinista). AsciiToGxtChar y sprintf
// quedan reales. Los textos de las colas apuntan siempre a un pool estatico de cadenas terminadas en NUL; ninguna
// expansion supera ~300 bytes (el EXE desborda su pila con mas de 400).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Messages.h"
#include "Hud.h"
#include "ControllerConfigManager.h"
#include "../ProbeRuntime.hpp"

namespace MessagesLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline std::string Hex(const uint8* p,size_t n){ static const char* d="0123456789ABCDEF"; std::string s; s.reserve(n*2); for(size_t i=0;i<n;++i){s+=d[p[i]>>4];s+=d[p[i]&15];} return s; }
// texto legible (los bytes no imprimibles se escapan para que json::dump no falle)
inline std::string Str(const GxtChar* p,size_t max=700){ std::string s; for(size_t i=0;i<max&&p[i];++i){ const uint8 c=p[i]; if(c>=0x20&&c<0x7F&&c!='\\')s+=char(c); else { char b[8]; sprintf_s(b,"\\x%02X",c); s+=b; } } return s; }

// --- pool de cadenas (arrays no const para que el enlazador no pliegue los de igual contenido) ---
// Las cadenas vacias llevan bytes definidos detras del NUL (la comparacion de textos del EXE sigue desde el indice 1).
namespace Pool {
inline char T0[]="HELLO", T1[]="HELLO", T2[]="HELLO WORLD", T3[]="Score ~1~ points", T4[]="~1~/~1~ ~1~:~1~ ~1~ ~1~ ~1~ ~1~",
    T5[]="Go to ~a~ now", T6[]="Press ~k~~PED_FIREWEAPON~ to shoot", T7[]="~k~~UNKNOWN~ test", T8[]="\0A", T9[]="\0B", T10[]="\0A",
    T11[]="~a~ and ~a~ ~1~", T12[]="Mixed ~1~ ~a~ ~k~~VEHICLE_ENTER_EXIT~ end", T13[]="~k~~GO_FORWARD~~k~~PED_SPRINT~", T14[]="~1 1~ ~ ~a ~k~~",
    T15[]="~k~~PED_FIREWEAPONX~ hi ~1~", T16[]="Plain mission text without any markers at all, just words", T17[]="H";
inline char S0[]="LS", S1[]="San Fierro", S2[]="\0Z", S3[]="~1~", S4[]="~k~~PED_SPRINT~", S5[]="Las Venturas Strip", S6[]="~a~";
}
inline const std::array<char*,18> texts{Pool::T0,Pool::T1,Pool::T2,Pool::T3,Pool::T4,Pool::T5,Pool::T6,Pool::T7,Pool::T8,Pool::T9,Pool::T10,Pool::T11,Pool::T12,Pool::T13,Pool::T14,Pool::T15,Pool::T16,Pool::T17};
inline const std::array<char*,7> strs{Pool::S0,Pool::S1,Pool::S2,Pool::S3,Pool::S4,Pool::S5,Pool::S6};
// bufferes del llamador (A: destino/objetivo, B: texto generado o copia)
alignas(16) inline std::array<uint8,512> bufA{}, bufB{};
inline const GxtChar* G(const char* p){ return reinterpret_cast<const GxtChar*>(p); }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }
inline json Name(uint32 v){
    if(!v)return nullptr;
    for(size_t k=0;k<texts.size();++k)if(U(texts[k])==v)return "T"+std::to_string(k);
    for(size_t k=0;k<strs.size();++k)if(U(strs[k])==v)return "S"+std::to_string(k);
    if(v>=U(bufA.data())&&v<U(bufA.data())+bufA.size())return "A+"+std::to_string(v-U(bufA.data()));
    if(v>=U(bufB.data())&&v<U(bufB.data())+bufB.size())return "B+"+std::to_string(v-U(bufB.data()));
    char b[16]; sprintf_s(b,"0x%08X",v); return std::string(b);
}

// --- stubs trazados ---
inline void __cdecl FakeSetBigMessage(GxtChar* m,uint32 style){ trace.push_back({"set_big_message",style&0xFFFF,m ? json(Str(m)) : json(nullptr)}); }
inline void __cdecl FakeSetMessage(const GxtChar* m){ trace.push_back({"set_message",m ? json(Str(m)) : json(nullptr)}); }
inline void __cdecl FakeGetRid(uint32 b){ trace.push_back({"get_rid_of_all_hud_messages",b&0xFF}); }
inline void __fastcall FakeCommandKeys(void* self,void*,int32 action,GxtChar* out,uint32 maxLen){
    trace.push_back({"command_keys",action,maxLen&0xFFFF,self==&ControlsManager});
    std::string k=action%3==0 ? std::string() : "["+std::to_string(action)+"]"; // texto vacio para un tercio de las acciones
    const size_t lim=std::min<size_t>(maxLen&0xFFFF,16); if(!lim)return;
    const size_t n=std::min(k.size(),lim-1); std::memcpy(out,k.data(),n); out[n]=0;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,28> fns{{
    {"CMessages","GetStringLength",0x69DB50,0,-1,256},{"CMessages","StringCopy",0x69DB70,0,-1,1024},{"CMessages","StringCompare",0x69DBD0,0,-1,1024},
    {"CMessages","CutString",0x69DC50,0,-1,2048},{"CMessages","ClearMessages",0x69DCD0,0,-1,256},{"CMessages","ClearSmallMessagesOnly",0x69DD30,0,-1,128},
    {"CMessages","AddToPreviousBriefArray",0x69DD50,0,-1,1024},{"CMessages","ClearPreviousBriefArray",0x69DE70,0,-1,128},
    {"CMessages","InsertNumberInString",0x69DE90,0,-1,2048},{"CMessages","InsertStringInString",0x69E040,0,-1,1024},
    {"CMessages","InsertPlayerControlKeysInString",0x69E160,0,-1,2048},{"CMessages","AddMessageWithNumberQ",0x69E360,0,-1,1024},
    {"CMessages","AddMessageJumpQWithNumber",0x69E4E0,0,-1,1024},{"CMessages","AddBigMessageWithNumber",0x69E5F0,0,-1,512},
    {"CMessages","AddBigMessageWithNumberQ",0x69E6E0,0,-1,512},{"CMessages","AddMessageWithStringQ",0x69E800,0,-1,1024},
    {"CMessages","AddMessageJumpQWithString",0x69E950,0,-1,1024},{"CMessages","ClearThisPrint",0x69EA30,0,-1,1024},
    {"CMessages","ClearThisBigPrint",0x69EBE0,0,-1,1024},{"CMessages","ClearThisPrintBigNow",0x69ED80,0,-1,512},
    {"CMessages","ClearAllMessagesDisplayedByGame",0x69EDC0,0,-1,256},{"CMessages","Init",0x69EE00,0,-1,64},
    {"CMessages","Process",0x69EE60,0,-1,1024},{"CMessages","Display",0x69EFC0,0,-1,2048},{"CMessages","AddMessageQ",0x69F0B0,0,-1,1024},
    {"CMessages","AddMessageJump",0x69F1E0,0,-1,1024},{"CMessages","AddBigMessage",0x69F2B0,0,-1,512},{"CMessages","AddBigMessageQ",0x69F370,0,-1,512},
}};

// --- estado: R1 = PreviousBriefs+BriefMessages+BIGMessages (contiguos), R2 = CHud::BigMessageX, R3 = CHud::m_BigMessage ---
constexpr uintptr_t R1_ADDR=0xC1A570, R2_ADDR=0xBAA3DC, R3_ADDR=0xBAACC0;
constexpr size_t R1_N=0x280+0x180+0x540, R2_N=7*4, R3_N=7*128, ST_N=R1_N+R2_N+R3_N;
constexpr size_t OFF_PB=0, OFF_BM=0x280, OFF_BIG=0x400;
inline uint32 R32(const uint8* p,size_t o){ uint32 v; std::memcpy(&v,p+o,4); return v; }
inline void W32(uint8* p,size_t o,uint32 v){ std::memcpy(p+o,&v,4); }
inline json Nums(const uint8* p){ json n=json::array(); for(int k=0;k<6;++k)n.push_back(int32(R32(p,k*4))); return n; }
inline json DecMsg(const uint8* m){ uint16 fl; std::memcpy(&fl,m+4,2);
    return {{"text",Name(R32(m,0))},{"flags",fl},{"pad6",Hex(m+6,2)},{"visible",R32(m,8)},{"created",R32(m,0xC)},{"numbers",Nums(m+0x10)},{"string",Name(R32(m,0x28))},{"previous_brief",m[0x2C]},{"pad2D",Hex(m+0x2D,3)}}; }
inline json DecPB(const uint8* m){ return {{"text",Name(R32(m,0))},{"numbers",Nums(m+4)},{"string",Name(R32(m,0x1C))}}; }
inline std::vector<uint8> Snap(){ std::vector<uint8> s(ST_N); std::memcpy(s.data(),reinterpret_cast<void*>(R1_ADDR),R1_N); std::memcpy(s.data()+R1_N,reinterpret_cast<void*>(R2_ADDR),R2_N); std::memcpy(s.data()+R1_N+R2_N,reinterpret_cast<void*>(R3_ADDR),R3_N); return s; }
inline void Restore(const std::vector<uint8>& s){ std::memcpy(reinterpret_cast<void*>(R1_ADDR),s.data(),R1_N); std::memcpy(reinterpret_cast<void*>(R2_ADDR),s.data()+R1_N,R2_N); std::memcpy(reinterpret_cast<void*>(R3_ADDR),s.data()+R1_N+R2_N,R3_N); }
inline json DiffState(const std::vector<uint8>& a,const std::vector<uint8>& b){
    json d=json::array();
    for(int k=0;k<20;++k){ const size_t o=OFF_PB+k*0x20; if(std::memcmp(&a[o],&b[o],0x20))d.push_back({{"region","PreviousBriefs"},{"index",k},{"original",DecPB(&a[o])},{"candidate",DecPB(&b[o])}}); }
    for(int k=0;k<8;++k){ const size_t o=OFF_BM+k*0x30; if(std::memcmp(&a[o],&b[o],0x30))d.push_back({{"region","BriefMessages"},{"index",k},{"original",DecMsg(&a[o])},{"candidate",DecMsg(&b[o])}}); }
    for(int k=0;k<28;++k){ const size_t o=OFF_BIG+k*0x30; if(std::memcmp(&a[o],&b[o],0x30))d.push_back({{"region","BIGMessages"},{"index",json::array({k/4,k%4})},{"original",DecMsg(&a[o])},{"candidate",DecMsg(&b[o])}}); }
    for(int k=0;k<7;++k){ const size_t o=R1_N+k*4; if(std::memcmp(&a[o],&b[o],4))d.push_back({{"region","BigMessageX"},{"index",k},{"original",R32(a.data(),o)},{"candidate",R32(b.data(),o)}}); }
    for(int k=0;k<7;++k){ const size_t o=R1_N+R2_N+k*128; if(std::memcmp(&a[o],&b[o],128))d.push_back({{"region","m_BigMessage"},{"index",k},{"original",Hex(&a[o],128)},{"candidate",Hex(&b[o],128)}}); }
    return d;
}
inline json DiffBuf(const char* nm,const uint8* a,const uint8* b,size_t n){
    size_t i=0; while(i<n&&a[i]==b[i])++i; if(i==n)return nullptr;
    size_t j=n; while(j>i&&a[j-1]==b[j-1])--j;
    const size_t s=i>16 ? i-16 : 0, e=std::min(n,j+16);
    return {{"buffer",nm},{"first",i},{"last",j-1},{"from",s},{"original",Hex(a+s,e-s)},{"candidate",Hex(b+s,e-s)}};
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"messages-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"messages-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("messages");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"messages-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,28> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,28> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& subtitles=*reinterpret_cast<uint8*>(0xA44488); const uint8 savedSubtitles=subtitles;
    const std::vector<uint8> savedState=Snap();
    auto* R1=reinterpret_cast<uint8*>(R1_ADDR); auto* BX=reinterpret_cast<float*>(R2_ADDR); auto* HM=reinterpret_cast<uint8*>(R3_ADDR);

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch h1(bit_cast<uintptr_t>(&CHud::SetBigMessage),FakeSetBigMessage), h1e(0x588FC0,FakeSetBigMessage);
        EntryPatch h2(bit_cast<uintptr_t>(&CHud::SetMessage),FakeSetMessage), h2e(0x588F60,FakeSetMessage);
        EntryPatch h3(bit_cast<uintptr_t>(&CHud::GetRidOfAllHudMessages),FakeGetRid), h3e(0x588A50,FakeGetRid);
        EntryPatch k1(bit_cast<uintptr_t>(&CControllerConfigManager::GetGxtStringOfCommandKeys),FakeCommandKeys), k1e(0x5303D0,FakeCommandKeys);
        std::array<std::vector<uint8>,2> rawState; std::array<std::array<uint8,1024>,2> rawBuf{};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3E55u; if(!stt)stt=1; trace=json::array();
                    // --- entradas de solo lectura ---
                    switch(next()%8){ case 0: timeMs=0xFFFFFFFFu-next()%20000u; break; case 1: timeMs=next()%20000u; break; case 2: timeMs=next(); break; default: timeMs=20000u+next()%50000000u; }
                    subtitles=uint8(next()%2);
                    auto num=[&]()->int32{ switch(next()%8){ case 0: case 1: case 2: return -1; case 3: return int32(next()%1000); case 4: return -int32(next()%100000)-2;
                                                             case 5: return next()%2 ? INT32_MIN : INT32_MAX; case 6: return int32(next()); default: return int32(next()%100); } };
                    auto pickText=[&]()->char*{ return texts[next()%texts.size()]; };
                    auto pickStr=[&]()->char*{ return next()%3==0 ? nullptr : strs[next()%strs.size()]; };
                    // --- estado coherente ---
                    for(size_t k=0;k<R1_N;++k)R1[k]=uint8(next());
                    auto fillMsg=[&](uint8* m,bool used){
                        W32(m,0,used ? U(pickText()) : 0);
                        const uint16 fl=uint16(next()%3 ? next()%8 : next()); std::memcpy(m+4,&fl,2);
                        const uint32 vis=next()%4 ? 500+next()%10000 : next(); W32(m,8,vis);
                        W32(m,0xC,next()%8 ? timeMs-uint32(next()%(uint64(vis)*2+1)) : next());
                        const bool none=next()%3==0; for(int k=0;k<6;++k)W32(m,0x10+k*4,uint32(none ? -1 : num()));
                        W32(m,0x28,used ? U(pickStr()) : (next()%2 ? 0 : U(pickStr())));
                        m[0x2C]=next()%8==0 ? uint8(next()) : uint8(next()%2);
                    };
                    { const uint32 used=next()%21; for(uint32 k=0;k<20;++k){ uint8* m=R1+OFF_PB+k*0x20; W32(m,0,k<used ? U(pickText()) : 0);
                          const bool none=next()%3==0; for(int n=0;n<6;++n)W32(m,4+n*4,uint32(none ? -1 : num())); W32(m,0x1C,U(pickStr())); }
                      if(used>1&&next()%4==0){ const uint32 a=next()%used, b=next()%used; std::memcpy(R1+OFF_PB+a*0x20,R1+OFF_PB+b*0x20,0x20); } }
                    { const bool holes=next()%10==0; const uint32 used=next()%9; for(uint32 k=0;k<8;++k)fillMsg(R1+OFF_BM+k*0x30,holes ? next()%2==0 : k<used); }
                    for(uint32 s=0;s<7;++s){ const bool holes=next()%8==0; const uint32 used=next()%5; for(uint32 k=0;k<4;++k)fillMsg(R1+OFF_BIG+(s*4+k)*0x30,holes ? next()%2==0 : k<used); }
                    for(int s=0;s<7;++s)BX[s]=next()%3==0 ? 0.f : float(int32(next()%2000)-1000)*0.5f;
                    for(size_t k=0;k<R3_N;++k)HM[k]=uint8(0x20+next()%0x5E);
                    // --- bufferes del llamador: patron sin '~' ---
                    for(auto& b:bufA){ b=uint8(next()); if(b=='~')b='#'; } for(auto& b:bufB){ b=uint8(next()); if(b=='~')b='#'; }
                    auto copyInto=[&](uint8* dst,const char* src){ const size_t n=std::strlen(src); std::memcpy(dst,src,n+1); return dst; };
                    auto genMarked=[&](uint8* dst){ static const char* tok[]{"~1~","~a~","~k~~PED_SPRINT~","~k~~PED_FIREWEAPON~","~k~~UNKNOWN~","~k~~"," ","word","AB","~","~1","1~","x","~a","~k~~GO_FORWARD~","12"};
                        size_t n=0; const int count=int(next()%15); for(int k=0;k<count;++k){ const char* t=tok[next()%std::size(tok)]; const size_t l=std::strlen(t); if(n+l>120)break; std::memcpy(dst+n,t,l); n+=l; } dst[n]=0; return dst; };
                    auto textArg=[&](uint8* dst)->const GxtChar*{ switch(next()%3){ case 0: return genMarked(dst); case 1: return copyInto(dst,pickText()); default: return G(pickText()); } };

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address; json out={{"switches",switches}}; json ret=nullptr;
                    using T=const GxtChar*;

                    if(nm=="GetStringLength"){ T s; if(next()%3==0){ const size_t n=next()%301; for(size_t k=0;k<n;++k)bufA[k]=uint8(0x20+next()%0x5E); bufA[n]=0; s=bufA.data(); } else s=textArg(bufB.data());
                        ret=reinterpret_cast<uint32(__cdecl*)(T)>(a)(s); }
                    else if(nm=="StringCopy"){ T src=next()%6==0 ? nullptr : textArg(bufB.data());
                        int32 len; switch(next()%5){ case 0: len=int32(next()%8); break; case 1: len=int32(next()%401); break; case 2: len=src ? int32(std::strlen(AsciiFromGxtChar(src))+next()%3) : 1; break; default: len=int32(next()%64); }
                        if(!src&&len==0)len=1; // el EXE podria limpiar 65535 bytes
                        reinterpret_cast<void(__cdecl*)(GxtChar*,T,int32)>(a)(bufA.data(),src,len); }
                    else if(nm=="StringCompare"){ T s1=G(next()%2 ? pickText() : strs[next()%strs.size()]); T s2;
                        switch(next()%4){ case 0: s2=s1; break; case 1: { uint8* d=copyInto(bufB.data(),AsciiFromGxtChar(s1)); const size_t n=std::strlen((char*)d); if(n&&next()%2)d[next()%n]^=uint8(1+next()%3); s2=d; } break; default: s2=G(next()%2 ? pickText() : strs[next()%strs.size()]); }
                        int32 len; switch(next()%4){ case 0: len=0; break; case 1: len=0xFFFF; break; default: len=int32(next()%24); }
                        ret=reinterpret_cast<bool(__cdecl*)(T,T,int32)>(a)(s1,s2,len); }
                    else if(nm=="CutString"){ const int32 count=int32(2+next()%69); const uint32 maxW=std::min<uint32>(count-1,10);
                        size_t n=0; const size_t target=next()%8==0 ? 0 : next()%301;
                        while(n<target){ if(n){ if(n+2>target)break; bufA[n++]=' '; } const uint32 w=1+next()%maxW; for(uint32 k=0;k<w&&n<300;++k)bufA[n++]=uint8('a'+next()%26); }
                        bufA[n]=0;
                        static std::array<GxtChar*,10> dest{}; for(size_t k=0;k<dest.size();++k)dest[k]=reinterpret_cast<GxtChar*>(uintptr_t(0x13370000+k));
                        ret=int32(reinterpret_cast<int16(__cdecl*)(int32,GxtChar*,GxtChar**)>(a)(count,bufA.data(),dest.data()));
                        json d=json::array(); for(auto* p:dest){ const uint32 v=U(p); d.push_back(v>=U(bufA.data())&&v<U(bufA.data())+bufA.size() ? json(int32(v-U(bufA.data()))) : json(Name(v))); } out["dest"]=d; }
                    else if(nm=="InsertNumberInString"){ T src=next()%10==0 ? nullptr : textArg(bufB.data()); int32 n[6]; for(auto& x:n)x=num();
                        reinterpret_cast<void(__cdecl*)(T,int32,int32,int32,int32,int32,int32,GxtChar*)>(a)(src,n[0],n[1],n[2],n[3],n[4],n[5],bufA.data()); }
                    else if(nm=="InsertStringInString"){ GxtChar* into=bufA.data(); if(next()%2)genMarked(into); else copyInto(into,pickText()); if(next()%12==0)into=nullptr;
                        T s=G(pickStr()); if(!s&&next()%2)s=G(strs[next()%strs.size()]);
                        reinterpret_cast<void(__cdecl*)(GxtChar*,T)>(a)(into,s); }
                    else if(nm=="InsertPlayerControlKeysInString"){ GxtChar* s=bufA.data(); if(next()%2)genMarked(s); else copyInto(s,pickText()); if(next()%12==0)s=nullptr;
                        reinterpret_cast<void(__cdecl*)(GxtChar*)>(a)(s); }
                    else if(nm=="AddToPreviousBriefArray"){ T t=G(pickText()); int32 n[6]; for(auto& x:n)x=next()%3 ? -1 : num(); T s=G(pickStr());
                        if(next()%3==0){ const uint8* e=R1+OFF_PB+(next()%20)*0x20; t=reinterpret_cast<T>(uintptr_t(R32(e,0))); for(int k=0;k<6;++k)n[k]=int32(R32(e,4+k*4)); s=reinterpret_cast<T>(uintptr_t(R32(e,0x1C)));
                            if(next()%3==0)n[next()%6]^=1; else if(next()%6==0)s=G(pickStr()); }
                        reinterpret_cast<void(__cdecl*)(T,int32,int32,int32,int32,int32,int32,T)>(a)(t,n[0],n[1],n[2],n[3],n[4],n[5],s); }
                    else if(nm=="ClearMessages"||nm=="ClearAllMessagesDisplayedByGame"||nm=="Display")reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(next()%2));
                    else if(nm=="ClearSmallMessagesOnly"||nm=="ClearPreviousBriefArray"||nm=="Init"||nm=="Process")reinterpret_cast<void(__cdecl*)()>(a)();
                    else if(nm=="ClearThisPrint"||nm=="ClearThisBigPrint"){ const bool big=nm=="ClearThisBigPrint"; T t{};
                        switch(next()%5){
                        case 0: case 1: { const size_t o=big ? OFF_BIG+(next()%28)*0x30 : OFF_BM+(next()%8)*0x30; t=reinterpret_cast<T>(uintptr_t(R32(R1,o))); } break; // texto presente (mismo puntero)
                        case 2: { const size_t o=big ? OFF_BIG+(next()%28)*0x30 : OFF_BM+(next()%8)*0x30; const uint32 p=R32(R1,o); if(p){ const char* src=reinterpret_cast<const char*>(uintptr_t(p)); copyInto(bufB.data(),src); t=bufB.data(); } } break; // mismo contenido, otro puntero
                        default: t=G(pickText()); }
                        if(!t)t=G(pickText());
                        reinterpret_cast<void(__cdecl*)(T)>(a)(t); }
                    else if(nm=="ClearThisPrintBigNow")reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(next()%7));
                    else { // Add*
                        T t=G(pickText()); const uint32 time=next()%4 ? 1000+next()%10000 : next(); const int32 flag=int32(next()%3 ? next()%8 : next()%0x10000);
                        const int32 prev=int32(next()%2); int32 n[6]; const bool none=next()%4==0; for(auto& x:n)x=none ? -1 : num(); const int32 style=int32(next()%7);
                        GxtChar* s=reinterpret_cast<GxtChar*>(pickStr());
                        if(nm=="AddMessageQ"||nm=="AddMessageJump")reinterpret_cast<void(__cdecl*)(T,uint32,int32,int32)>(a)(t,time,flag,prev);
                        else if(nm=="AddMessageWithNumberQ"||nm=="AddMessageJumpQWithNumber")reinterpret_cast<void(__cdecl*)(T,uint32,int32,int32,int32,int32,int32,int32,int32,int32)>(a)(t,time,flag,n[0],n[1],n[2],n[3],n[4],n[5],prev);
                        else if(nm=="AddMessageWithStringQ"||nm=="AddMessageJumpQWithString")reinterpret_cast<void(__cdecl*)(T,uint32,int32,GxtChar*,int32)>(a)(t,time,flag,s,prev);
                        else if(nm=="AddBigMessage"||nm=="AddBigMessageQ")reinterpret_cast<void(__cdecl*)(T,uint32,int32)>(a)(t,time,style);
                        else reinterpret_cast<void(__cdecl*)(T,uint32,int32,int32,int32,int32,int32,int32,int32)>(a)(t,time,style,n[0],n[1],n[2],n[3],n[4],n[5]); // AddBigMessageWithNumber(Q)
                    }
                    for(auto* h:hooks)h->State(false);
                    rawState[candidate]=Snap(); std::memcpy(rawBuf[candidate].data(),bufA.data(),512); std::memcpy(rawBuf[candidate].data()+512,bufB.data(),512);
                    out["state"]=Fnv(rawState[candidate].data(),ST_N); out["buffers"]=Fnv(rawBuf[candidate].data(),1024);
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    json detail=nullptr;
                    if(failures<=3){ detail={{"state",DiffState(rawState[0],rawState[1])},{"A",DiffBuf("A",rawBuf[0].data(),rawBuf[1].data(),512)},{"B",DiffBuf("B",rawBuf[0].data()+512,rawBuf[1].data()+512,512)}}; }
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"detail",detail},
                           {"original",failures<=3 ? original:json(nullptr)},{"candidate",failures<=3 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    timeMs=savedTime; subtitles=savedSubtitles; Restore(savedState);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Snap()==savedState&&timeMs==savedTime&&subtitles==savedSubtitles}});
}
}
