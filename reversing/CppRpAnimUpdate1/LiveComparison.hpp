#pragma once
// Banco dentro del motor: RpAnimBlendClumpUpdateAnimations (0x4D34F0) y FrameUpdateCallBackOffscreen (0x4D2E10).
// Original (hooks desactivados, codigo del EXE) frente a candidato (hook conmutado) con fixtures deterministas por caso.
//
// Lo que NO validaba el banco RpAnim43 (que solo ejerce las cuatro FrameUpdateCallBack{Compressed,}{Skinned,NonSkinned}) y este si ejerce:
//   - RpAnimBlendClumpUpdateAnimations completa: lista vacia, UpdateBlend, jerarquias sin secuencias, tope de 11 arrays de nodos, tiempos
//     sincronizados (cadenas x87 de 24 bits), IncludePartial, seleccion del callback (comprimida/skinned/visible/offscreen), actualizacion
//     de keyframes (RpAnimBlendNodeUpdateKeyFrames, 0x4D1570, no hookable: EAX) y UpdateTime con el siguiente enlace leido despues del callback.
//   - FrameUpdateCallBackOffscreen (0x4D2E10; requiere aplicar rpw.patch, que lo reescribe y lo registra como hook).
//   Variante "-CppCallbacks": el candidato ademas activa los cinco callbacks en C++ (los cuatro de RpAnim43 y Offscreen), de modo que se
//   valida la ruta completa de la DLL sin ningun callback del EXE.
//
// Nunca se toca un clump real: clump, CAnimBlendClumpData, asociaciones (vtable falsa que traza y desenlaza), jerarquias, nodos, secuencias,
// keyframes, frame datas y RwFrames son buferes estaticos. Se reutilizan las infraestructuras de RpAnim43 (namespace RpAnim43Live):
// FakeFirstAtomic, FakeSkin, Patches (EntryPatch en la entrada del EXE y en el cuerpo de la DLL con el mismo Fake), FindHook, buferes.
// Simulados y trazados: GetFirstAtomic (0x734820), RpSkinGeometryGetSkin (0x7C7550), CAnimManager::UncompressAnimation (0x4D41C0, las
// secuencias ya vienen listas), RwFrameUpdateObjects (0x7F0910, sobre un RwFrame falso) y el callback de asociaciones (cdecl).
// Corren de verdad (ya validadas por sus bancos y compartidas por ambos lados): UpdateBlend/UpdateTime de la asociacion, CAnimBlendNode,
// ForAllFrames, RwMatrixUpdate y las extracciones de velocidad (0x4D1680... en el EXE, o su version C++ segun el hook).
//
// Fuera de los fixtures (el EXE se cae): ningun array de nodos tras UpdateBlend, o un frame sin ningun nodo valido con extraccion de velocidad;
// la superviviente (nunca blend-auto-remove, jerarquia con secuencias y nodos validos en todos los frames) lo evita.
// Los valores de estres (NaN con carga distinta, subnormales, ceros con signo, 1e-30/-1e30, negativos) se aplican a velocidad, blend, delta,
// duracion de la jerarquia, tiempo actual y paso de tiempo; se descartan los casos cuya prediccion en doubles daria un paso positivo enorme
// (el bucle NextKeyFrame no terminaria) y nunca se usa +inf ni tiempo actual grande.
//
// Cortafuegos (ningun caso puede colgar el juego; el EXE no tiene ninguna guarda en estos bucles):
//  (1) El bucle de NextKeyFrame/NextKeyFrameCompressed ("mientras restante <= 0: restante += delta del keyframe") da paso/delta vueltas y, con un
//      paso de unos 3e5 s o mas, la x87 a 24 bits ya no avanza (-paso + delta == -paso): no termina ni en el EXE ni en la DLL. El paso de una
//      asociacion lo recalcula UpdateAnimations en su segundo recorrido de la LISTA, pero una asociacion que el callback de otra desenlazo despues
//      de entrar en los arrays de nodos (FakeCb) conserva el 'm_TimeStep' que tenia (caso 25: 4,7e12 de basura aleatoria con looped => cuelgue);
//      y FrameUpdateCallBackOffscreen no recalcula nada. Por eso (a) la basura de 0x28 se acota: un valor mayor que kMaxStep cambia de signo
//      (sigue siendo basura, incluidos NaN y -inf, pero ya no puede dejar un restante negativo inalcanzable), y (b) WorstStep() simula en doubles
//      UpdateBlend (con los desenlaces de FakeCb) y el segundo recorrido y devuelve el mayor paso positivo con que se llamara a los nodos: un estres
//      que lo supere se descarta, y si aun asi la prediccion de vueltas supera kMaxLoopIterations no se llama (error "firewall" y el banco falla).
//  (2) FindKeyFrame ("mientras tiempo > delta: tiempo -= delta") da tiempo/delta vueltas: tambien se predicen con el tiempo actual de cada asociacion.
//  (3) Un vigilante en otro hilo cierra el proceso con un registro de error si una sola llamada tarda mas de 12 s.
//  La lista de asociaciones solo pierde enlaces (FakeCb/FakeAssocDtor), nunca se reenlaza: no puede formar un ciclo ni alargarse.
#include <atomic>
#include <thread>
#include "../RpAnim43/LiveComparison.hpp"
#include "AnimManager.h"

namespace CppRpAnimUpdate1Live {
namespace R = RpAnim43Live;
using R::At; using R::G;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }

enum : uint32 { T_UNC=100, T_RWUPD, T_CB };
constexpr size_t NA=14, NF=6, MAXKF=5, KFB=32, ASZ=0x40, HSZ=0x18, NODESZ=0x18, SEQSZ=0xC;
alignas(16) inline std::array<uint8,NA*ASZ> assocs{};
alignas(16) inline std::array<uint8,NA*HSZ> hiers{};
alignas(16) inline std::array<uint8,NA*NF*NODESZ> nodes{};
alignas(16) inline std::array<uint8,NA*NF*SEQSZ> seqs{};
alignas(16) inline std::array<uint8,NA*NF*MAXKF*KFB> keyframes{};
alignas(16) inline std::array<uint8,0x40> updData{};
inline uint8* A(size_t k) { return assocs.data()+k*ASZ; }
inline uint8* L(size_t k) { return A(k)+4; }
inline uint8* H(size_t k) { return hiers.data()+k*HSZ; }
inline uint8* ND(size_t a,size_t f=0) { return nodes.data()+(a*NF+f)*NODESZ; }
inline uint8* SQ(size_t a,size_t f) { return seqs.data()+(a*NF+f)*SEQSZ; }
inline uint8* KB(size_t a,size_t f) { return keyframes.data()+(a*NF+f)*MAXKF*KFB; }

// --- colaboradores simulados (cdecl en ambos lados) ---
inline void* __cdecl FakeRwUpdate(void* frame) { R::Push(T_RWUPD,R::U(frame)); return frame; }
inline void  __cdecl FakeUncompress(void* hier) { R::Push(T_UNC,R::U(hier)); }
// Callback de asociacion: traza y, si lleva datos, desenlaza esa asociacion (modifica la lista durante la actualizacion).
inline void  __cdecl FakeCb(void* a,void* d) {
    R::Push(T_CB,R::U(a),R::U(d));
    if(d){ auto* nx=At<uint8*>(d,4); auto* pv=At<uint8*>(d,8); if(pv)At<uint8*>(pv,0)=nx; if(nx)At<uint8*>(nx,4)=pv; At<uint8*>(d,4)=nullptr; At<uint8*>(d,8)=nullptr; } }

// --- cortafuegos ---
inline constexpr uint64 kMaxLoopIterations = 1'000'000;   // por nodo; a ~8e6 vueltas la x87 de 24 bits deja de avanzar con deltas de 0,05 s
inline constexpr double kMaxStep = 50.;                   // s: mayor paso positivo admitido (fuera de estres los pasos reales son < 5 s)
inline constexpr double kMinDelta = 1. / 60.;             // menor delta de keyframe de los fixtures (comprimido: 1/60 s; sin comprimir: 0,05 s)
inline std::atomic<unsigned> firewallTrips{};
struct Par { float blend, delta, cur, speed, total; uint16 flags; bool hierSeqs; uint32 cbType; int32 cbTarget; };
// Mayor paso positivo (en s) con que UpdateAnimations llamara a los nodos de una asociacion registrada en los arrays. Simula en doubles, como el EXE
// (0x4D351F..0x4D3605): UpdateBlend en orden de lista con el desenlace que hace FakeCb (el objetivo y despues la propia asociacion, que 'delete this'
// desenlaza), el tope de 11 arrays, las sumas de sincronizacion (float tras cada fstp) y el segundo recorrido, que solo recalcula las asociaciones
// que siguen enlazadas. Las desenlazadas despues de registrarse conservan su basura de 0x28, acotada a kMaxStep por el generador.
// Con NaN las comparaciones son falsas y no aporta; +inf si.
inline double WorstStep(const std::array<Par,NA>& par, int32 nList, float timeStep, std::array<bool,NA>* regOut=nullptr) {
    const auto fl=[](double x){ return double(float(x)); };
    std::array<int32,NA> nx{}, pv{}; std::array<double,NA> blend{}, delta{}; std::array<bool,NA> reg{};
    for(int32 k=0;k<nList;++k){ nx[size_t(k)]=k+1<nList ? k+1 : -1; pv[size_t(k)]=k-1; blend[size_t(k)]=par[size_t(k)].blend; delta[size_t(k)]=par[size_t(k)].delta; }
    int32 head=nList>0 ? 0 : -1;
    const auto unlink=[&](int32 d){ const int32 n=nx[size_t(d)], p=pv[size_t(d)]; if(p>=0)nx[size_t(p)]=n; else if(head==d)head=n; if(n>=0)pv[size_t(n)]=p; nx[size_t(d)]=-1; pv[size_t(d)]=-1; };
    const double ts=timeStep; double totT=0, totB=0; int32 nreg=0;
    for(int32 l=head; l>=0;){
        const int32 a=l; l=nx[size_t(a)]; const Par& p=par[size_t(a)]; double& b=blend[size_t(a)]; double& d=delta[size_t(a)];
        b=fl(b+fl(d*ts));
        if(b<=0. && d<0.){
            b=0.; d=0.;
            if(p.flags&4){ if((p.cbType==1||p.cbType==2)&&p.cbTarget>=0)unlink(p.cbTarget); unlink(a); continue; } }
        if(b>1.){ b=1.; if(d>0.)d=0.; }
        if(!p.hierSeqs)continue;
        if(nreg<11){ reg[size_t(a)]=true; ++nreg; }
        if(p.flags&0x20){ totT=fl(totT+fl(fl(double(p.total)/double(p.speed))*b)); totB=fl(totB+b); } }
    if(regOut)*regOut=reg;
    std::array<bool,NA> linked{}; for(int32 l=head; l>=0; l=nx[size_t(l)])linked[size_t(l)]=true;
    double worst=0.;
    for(int32 k=0;k<nList;++k){ const Par& p=par[size_t(k)]; if(!reg[size_t(k)]||!(p.flags&1)||!linked[size_t(k)])continue;
        const double st = !(p.flags&0x20) ? double(p.speed)*ts : totT==0. ? double(p.total)*ts : double(p.total)/totT*totB*ts;
        if(st>worst)worst=st; }
    return worst;
}
// El EXE desreferencia SIN GUARDA el primer array de nodos en los cuatro FrameUpdateCallBack* (bucle do-while desde 0x4D2EF5/0x4D2C60/...) y en las
// extracciones de velocidad (0x4D1680...): con updData[4]==0 cae con eip=0x4D2EF7 (ecx=0). Los arrays se registran en el primer recorrido de la lista
// (0x4D3530..0x4D359E: UpdateBlend verdadero, secuencias de la jerarquia no nulas, tope de 11) y el recorrido lee 'siguiente' ANTES de UpdateBlend, de modo
// que el desenlace de FakeCb sobre la asociacion que es justo el siguiente (cbTarget) corta el recorrido (su enlace queda a nulo): caso 3190 de
// UpdateAnimations (asociacion 0 con flag 4 que se autoelimina y desenlaza la 1, sin secuencias, y con 13 asociaciones nada mas se registra). El generador
// garantiza que la superviviente (nodos validos en todos los frames) entra en los arrays: primero se anulan los objetivos de callback; si aun asi el tope de 11
// la deja fuera (13 asociaciones, superviviente 11 o 12), se quitan las secuencias de las anteriores que si entran. Sin consumir numeros aleatorios.
// En Offscreen no hay UpdateBlend: entran las k con secuencias en orden, tope 11.
inline void EnsureSurvivorRegistered(std::array<Par,NA>& par, int32 nList, float timeStep, int32 survivor, bool offscreen) {
    std::array<bool,NA> reg{};
    const auto registered=[&]()->bool{
        if(offscreen){ int32 n=0; for(int32 k=0;k<survivor;++k)if(par[size_t(k)].hierSeqs)++n; return n<11; }
        reg.fill(false); WorstStep(par,nList,timeStep,&reg); return reg[size_t(survivor)]; };
    if(registered())return;
    if(!offscreen){ for(int32 k=0;k<nList;++k)par[size_t(k)].cbTarget=-1; if(registered())return; }
    for(int32 k=0;k<survivor&&!registered();++k)if(par[size_t(k)].hierSeqs&&(offscreen||reg[size_t(k)]))par[size_t(k)].hierSeqs=false;
}
// Mayor tiempo actual: FindKeyFrame (RpAnimBlendNodeUpdateKeyFrames, solo visible y sin comprimir) da una vuelta por cada delta (>= 0,05 s) que cabe en el.
inline double WorstFindTime(const std::array<Par,NA>& par, int32 nList) {
    double worst=0.; for(int32 k=0;k<nList;++k){ const double c=par[size_t(k)].cur; if(c>worst)worst=c; } return worst; }

// vigilante: una llamada del banco que no vuelve en kWatchdogMs deja un error (con funcion, caso y lado) y cierra el proceso en vez de colgar el juego
inline constexpr uint64 kWatchdogMs = 12000;
inline std::atomic<uint64> guardStart{0};
inline std::atomic<unsigned> guardFn{}, guardCase{}, guardSide{};
inline void StartWatchdog() {
    static bool started=false; if(started)return; started=true;
    std::thread([]{
        for(;;){
            Sleep(250);
            const uint64 s=guardStart.load();
            if(s && GetTickCount64()-s>kWatchdogMs)
                ReversingProbe::Fail({{"event","error"},{"reason","watchdog: una llamada del banco no vuelve (bucle sin fin en el EXE o en la DLL)"},{"function_index",guardFn.load()},
                                      {"case",guardCase.load()},{"candidate_side",guardSide.load()!=0},{"elapsed_ms",GetTickCount64()-s}},95);
        }
    }).detach();
}
struct CallGuard {
    CallGuard(unsigned fi,unsigned i,bool candidate){ guardFn=fi; guardCase=i; guardSide=candidate ? 1u : 0u; guardStart=GetTickCount64(); }
    ~CallGuard(){ guardStart=0; }
};

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,3> fns{{
    {"RpAnimBlend","RpAnimBlendClumpUpdateAnimations",0x4D34F0,0,-1,4096},
    {"RpAnimBlend","RpAnimBlendClumpUpdateAnimations-CppCallbacks",0x4D34F0,0,-1,2048},
    {"RpAnimBlend","FrameUpdateCallBackOffscreen",0x4D2E10,0,-1,1024},
}};
inline const char* const cbNames[]{"FrameUpdateCallBackCompressedSkinned","FrameUpdateCallBackCompressedNonSkinned","FrameUpdateCallBackSkinned","FrameUpdateCallBackNonSkinned","FrameUpdateCallBackOffscreen"};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpp-rpanim-update1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpp-rpanim-update1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpp-rpanim-update1");StartWatchdog();}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    R::mainTid=GetCurrentThreadId();
    // La vtable falsa de las asociaciones la rellena RpAnim43Live::Update(), que aqui no corre: sin esto el 'delete this' de UpdateBlend llama a un puntero nulo (eip=0).
    R::fakeVt={reinterpret_cast<void*>(&R::FakeAssocDtor),nullptr,nullptr,nullptr};
    std::string only; { std::ifstream f(dir/"cpp-rpanim-update1-only.txt"); if(f)std::getline(f,only); }

    using std::bit_cast;
    // Hooks: el de UpdateAnimations y los cinco callbacks (Offscreen solo existe tras aplicar rpw.patch).
    auto* updHook=R::FindHook("RpAnimBlend","RpAnimBlendClumpUpdateAnimations");
    if(!updHook){Write({{"event","error"},{"reason","hook not found"},{"function","RpAnimBlendClumpUpdateAnimations"}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,5> cbHooks{};
    for(size_t k=0;k<cbHooks.size();++k)if(!(cbHooks[k]=R::FindHook("RpAnimBlend",cbNames[k]))){Write({{"event","error"},{"reason","hook not found (aplicar rpw.patch)"},{"function",cbNames[k]}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,6> all{updHook,cbHooks[0],cbHooks[1],cbHooks[2],cbHooks[3],cbHooks[4]};
    std::array<bool,6> enabled{},locked{};
    for(size_t k=0;k<all.size();++k){enabled[k]=all[k]->Hooked();locked[k]=all[k]->Locked();all[k]->LockState(false);}

    const uint32 clumpOffset=G<uint32>(R::CLUMPOFF);
    if(clumpOffset+4>R::clump.size()||clumpOffset<8){Write({{"event","error"},{"reason","clump plugin offset out of fixture"},{"offset",clumpOffset}});return;}

    struct Stub { uintptr_t exe, dll; void* fake; };
    const auto B=[](uintptr_t p){ return p ? R::Body(p) : uintptr_t(0); };
    const std::array<Stub,4> stubs{{
        {0x734820,B(bit_cast<uintptr_t>(&GetFirstAtomic)),reinterpret_cast<void*>(&R::FakeFirstAtomic)},
        {0x7C7550,0,reinterpret_cast<void*>(&R::FakeSkin)},
        {0x4D41C0,B(bit_cast<uintptr_t>(&CAnimManager::UncompressAnimation)),reinterpret_cast<void*>(&FakeUncompress)},
        {0x7F0910,0,reinterpret_cast<void*>(&FakeRwUpdate)},
    }};
    std::vector<std::array<uint8,5>> savedEntries(stubs.size());
    for(size_t k=0;k<stubs.size();++k)std::memcpy(savedEntries[k].data(),reinterpret_cast<void*>(stubs[k].exe),5);
    struct Blob { uintptr_t p; size_t n; std::vector<uint8> saved; };
    std::vector<Blob> blobs{{R::CLUMPOFF,4},{R::FOUNDFRAME,4},{R::GPCLUMP,4}};
    for(auto& b:blobs)b.saved.assign(reinterpret_cast<uint8*>(b.p),reinterpret_cast<uint8*>(b.p)+b.n);
    auto restoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.p),b.saved.data(),b.n); };

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        const bool offscreenFn=nm=="FrameUpdateCallBackOffscreen", cppCb=nm.ends_with("-CppCallbacks");
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            uint32 sst{}; auto sn=[&]{sst^=sst<<13;sst^=sst>>17;sst^=sst<<5;return sst;};
            auto SF=[](uint32 b){return std::bit_cast<float>(b);};

            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7A11u; if(!stt)stt=1;
                const bool stress=i%3==2; sst=uint32(i)*2246822519u^uint32(f)*3266489917u^0x5BD1E995u; if(!sst)sst=1;
                R::nrec=0; R::arena.fill(0xCD); R::arenaTop=0x40;

                // ---- parametros del caso ----
                const bool compressed=next()%4==0, skinned=next()%2!=0, visible=next()%3!=0, needsKF=next()%2!=0;
                const int32 nFr=1+int32(next()%4);
                const int32 nList=next()%8==0 ? 13 : 1+int32(next()%6); // 13 > tope de 11 arrays
                const int32 survivor=int32(next()%uint32(nList));
                R::skinMode=skinned ? 2 : int32(next()%2);
                using P=Par;
                std::array<P,NA> par{};
                for(int32 k=0;k<nList;++k){
                    P& p=par[size_t(k)];
                    uint16 fl=0;
                    if(next()%5)fl|=1; if(next()%2)fl|=2; if(k!=survivor&&next()%6==0)fl|=4; if(next()%8==0)fl|=8; if(next()%4==0)fl|=0x10;
                    if(next()%2)fl|=0x20; if(next()%3)fl|=0x40; if(next()%2)fl|=0x80; if(next()%4==0)fl|=0x400; if(next()%4==0)fl|=0x2000;
                    p.flags=fl; p.hierSeqs=k==survivor||next()%6!=0;
                    p.total=uni(0.3f,2.5f); p.speed=uni(0.3f,2.2f); p.blend=uni(0.05f,1.f);
                    p.delta=next()%10<7 ? 0.f : uni(-4.f,2.f); p.cur=uni(0.f,p.total);
                    if(fl&4){ p.blend=uni(0.f,0.05f); p.delta=uni(-8.f,-1.f); }
                    p.cbType=next()%3==0 ? 1u+next()%2 : 0u; p.cbTarget=(nList>1&&next()%2) ? int32(next()%uint32(nList)) : -1;
                    if(p.cbTarget==survivor)p.cbTarget=-1;
                }
                const uint32 stepKind=next()%6;
                float timeStep=stepKind==0 ? 0.f : stepKind==1 ? 0.016f : stepKind==2 ? 0.2f : stepKind==3 ? 0.033f : uni(0.f,0.1f);
                EnsureSurvivorRegistered(par,nList,timeStep,survivor,offscreenFn);
                if(stress){
                    const uint32 smode=sn()%5; // 0 NaN, 1 subnormales, 2 ceros, 3 pequenos/negativos, 4 todo
                    const uint32 kNaN[]{0x7FC00000u,0xFFC00000u,0x7FA00000u,0xFFC12345u,0x7FC54321u,0xFFFFFFFFu};
                    const uint32 kSub[]{1u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x00400000u,0x80400000u,0x000116C2u};
                    const uint32 kZero[]{0u,0x80000000u};
                    const float  kSmall[]{1e-30f,-1e-30f,-1e30f,-1.f,-0.5f,-0.05f};
                    auto pick=[&]()->float{
                        const uint32 c=smode==4 ? sn()%4 : smode;
                        switch(c){ case 0: return SF(kNaN[sn()%std::size(kNaN)]); case 1: return SF(kSub[sn()%std::size(kSub)]);
                                   case 2: return SF(kZero[sn()%std::size(kZero)]); default: return kSmall[sn()%std::size(kSmall)]; } };
                    auto pickCur=[&]()->float{ const float v=pick(); return v==-1e30f ? -1.f : v; };
                    const auto savedPar=par; const float savedStep=timeStep;
                    for(int32 k=0;k<nList;++k){ P& p=par[size_t(k)];
                        if(sn()%3==0)p.blend=pick(); if(sn()%3==0)p.speed=pick(); if(sn()%3==0)p.total=pick();
                        if(sn()%4==0)p.delta=pick(); if(sn()%3==0)p.cur=pickCur(); }
                    if(sn()%2==0)timeStep=pick();
                    EnsureSurvivorRegistered(par,nList,timeStep,survivor,offscreenFn); // los estres de blend/delta/paso cambian los desenlaces
                    // Prediccion en doubles del paso de cada asociacion: si saldria un paso positivo enorme, se descarta el estres.
                    double totT=0,totB=0; bool bad=false;
                    for(int32 k=0;k<nList;++k){ const P& p=par[size_t(k)]; if(!p.hierSeqs)continue;
                        const double bl=double(p.blend)+double(p.delta)*double(timeStep);
                        if((p.flags&4)&&bl<=0.&&p.delta<0.f)continue;
                        if(p.flags&0x20){ totT+=double(p.total)/double(p.speed)*double(p.blend); totB+=double(p.blend); } }
                    for(int32 k=0;k<nList;++k){ const P& p=par[size_t(k)];
                        const double st = !(p.flags&0x20) ? double(p.speed)*double(timeStep) : totT==0. ? double(p.total)*double(timeStep) : double(p.total)/totT*totB*double(timeStep);
                        if(std::isfinite(st)&&st>50.)bad=true; }
                    if(std::isfinite(timeStep)&&timeStep>0.2f)bad=true;
                    // Prediccion de la lista (desenlaces de FakeCb, tope de 11 arrays, solo las asociaciones que siguen enlazadas).
                    if(!bad&&WorstStep(par,nList,timeStep)>kMaxStep)bad=true;
                    if(bad){ par=savedPar; timeStep=savedStep; }
                }

                // ---- cortafuegos (antes de tocar nada): no se llama si algun bucle dependiente de los datos superaria kMaxLoopIterations ----
                if(!offscreenFn){
                    const double step=WorstStep(par,nList,timeStep), cur=WorstFindTime(par,nList);
                    const uint64 itStep=step>0. ? (std::isfinite(step) ? uint64(step/kMinDelta)+1 : ~0ull) : 0;
                    const uint64 itFind=cur>0. ? uint64(cur/0.05)+1 : 0;
                    if(itStep>kMaxLoopIterations||itFind>kMaxLoopIterations){
                        ++firewallTrips;
                        if(!candidate)Write({{"event","error"},{"reason","firewall: bucle dependiente de los datos sin cota"},{"function",full},{"case",i},{"worst_step",step},{"worst_find_time",cur},{"iterations_step",itStep},{"iterations_find",itFind}});
                        return json{{"firewall",true},{"switches",false}};
                    }
                }

                // ---- mundo ----
                R::clump.fill(0); R::cdata.fill(0); assocs.fill(0); hiers.fill(0); nodes.fill(0); seqs.fill(0); keyframes.fill(0); updData.fill(0);
                R::clump[0]=2; At<void*>(R::clump.data(),clumpOffset)=R::cdata.data(); At<void*>(R::clump.data(),4)=R::FR(0);
                for(size_t k=0;k<R::NFR;++k){ uint8* fr=R::FR(k); std::memset(fr,0,R::FRSZ); for(size_t q=0;q<0x40;q+=4)At<float>(fr,0x10+q)=uni(-5.f,5.f); }
                std::memset(R::frameDatas.data(),0,R::frameDatas.size());
                for(size_t q=0;q+4<=R::kfs.size();q+=4)At<float>(R::kfs.data(),q)=uni(-2.f,2.f);
                for(size_t q=0;q+4<=R::pedPos.size();q+=4)At<float>(R::pedPos.data(),q)=uni(-4.f,4.f);
                R::atomic.fill(0); At<void*>(R::atomic.data(),0x18)=R::geometry.data(); R::skin.fill(0);
                At<uint32>(R::cdata.data(),8)=uint32(nFr);
                At<void*>(R::cdata.data(),0xC)=next()%2 ? R::pedPos.data() : nullptr;
                At<void*>(R::cdata.data(),0x10)=R::frameDatas.data();
                for(int32 fr=0;fr<nFr;++fr){
                    uint8* fd=R::FD(size_t(fr));
                    uint8 fl=0;
                    if(next()%5==0)fl|=2; if(next()%5==0)fl|=4;
                    if(next()%2)fl|=8; if(next()%2)fl|=0x10;
                    if(fr==0){ if(needsKF)fl|=0x20; if(next()%2)fl|=0x80; }
                    if(compressed)fl|=0x40;
                    fd[0]=fl;
                    for(size_t q=0;q<12;q+=4)At<float>(fd,4+q)=uni(-3.f,3.f);
                    At<void*>(fd,0x10)=skinned ? static_cast<void*>(R::kfs.data()+size_t(fr)*R::KFSZ) : static_cast<void*>(R::FR(size_t(1+fr)));
                    At<int32>(fd,0x14)=skinned ? int32(next()%40) : -1;
                }
                // lista: asociaciones 0..nList-1 en orden
                uint8* prev=R::cdata.data(); At<uint8*>(prev,0)=nullptr;
                for(int32 k=0;k<nList;++k){
                    uint8* a=A(size_t(k)); const P& p=par[size_t(k)];
                    for(size_t q=0;q<ASZ;q+=4)At<uint32>(a,q)=next();
                    At<void*>(a,0)=R::fakeVt.data(); At<uint16>(a,0xC)=uint16(nFr);
                    At<void*>(a,0x10)=ND(size_t(k)); At<void*>(a,0x14)=H(size_t(k));
                    At<float>(a,0x18)=p.blend; At<float>(a,0x1C)=p.delta; At<float>(a,0x20)=p.cur; At<float>(a,0x24)=p.speed; // 0x28: basura (se recalcula); acotada: un valor > kMaxStep cambia de signo (ver cortafuegos arriba)
                    if(At<float>(a,0x28)>float(kMaxStep))At<uint32>(a,0x28)|=0x80000000u;
                    At<int16>(a,0x2C)=int16(k); At<uint16>(a,0x2E)=p.flags;
                    At<uint32>(a,0x30)=p.cbType; At<void*>(a,0x34)=reinterpret_cast<void*>(&FakeCb);
                    At<void*>(a,0x38)=p.cbTarget>=0 ? static_cast<void*>(A(size_t(p.cbTarget))) : nullptr;
                    uint8* l=L(size_t(k)); At<uint8*>(prev,0)=l; At<uint8*>(l,4)=prev; At<uint8*>(l,0)=nullptr; prev=l;
                    uint8* h=H(size_t(k)); At<uint32>(h,0)=next(); At<float>(h,0x10)=p.total;
                    for(int32 fr=0;fr<nFr;++fr){
                        uint8* nd=ND(size_t(k),size_t(fr)); uint8* sq=SQ(size_t(k),size_t(fr)); uint8* kb=KB(size_t(k),size_t(fr));
                        const bool trans=next()%2!=0; const int32 nf=1+int32(next()%MAXKF);
                        At<uint32>(sq,0)=next(); At<uint16>(sq,4)=uint16((next()%8 ? 1 : 0)|(trans ? 2 : 0)|(compressed ? 4 : 0)); At<uint16>(sq,6)=uint16(nf); At<void*>(sq,8)=kb;
                        const size_t stride=compressed ? (trans ? 16 : 10) : (trans ? 32 : 20);
                        for(int32 q=0;q<nf;++q){ uint8* kf=kb+size_t(q)*stride;
                            float qq[4]; float len=0.f; for(auto& c:qq){ c=uni(-1.f,1.f); len+=c*c; } len=std::sqrt(std::max(len,0.01f)); for(auto& c:qq)c/=len;
                            if(compressed){ for(int c=0;c<4;++c)At<int16>(kf,size_t(c)*2)=int16(qq[c]*4096.f); At<int16>(kf,8)=int16(1+next()%30);
                                if(trans)for(int c=0;c<3;++c)At<int16>(kf,10+size_t(c)*2)=int16(int32(next()%4096)-2048); }
                            else { for(int c=0;c<4;++c)At<float>(kf,size_t(c)*4)=qq[c]; At<float>(kf,16)=uni(0.05f,0.5f);
                                if(trans)for(int c=0;c<3;++c)At<float>(kf,20+size_t(c)*4)=uni(-2.f,2.f); } }
                        const bool valid=k==survivor||next()%5!=0;
                        const int32 cur=nf>1 ? 1+int32(next()%uint32(nf-1)) : 0;
                        At<int16>(nd,8)=int16(cur); At<int16>(nd,0xA)=int16(cur ? cur-1 : nf-1); At<float>(nd,0xC)=uni(0.f,0.5f);
                        At<void*>(nd,0x10)=valid ? sq : nullptr; At<void*>(nd,0x14)=a;
                        if(valid){ auto* node=reinterpret_cast<CAnimBlendNode*>(nd); if(compressed)node->CalcDeltasCompressed(); else node->CalcDeltas(); }
                    }
                    At<void*>(h,4)=p.hierSeqs ? static_cast<void*>(SQ(size_t(k),0)) : nullptr;
                }
                G<uint32>(R::FOUNDFRAME)=next(); G<void*>(R::GPCLUMP)=R::vdata.data();
                if(offscreenFn){ // contexto de actualizacion: arrays de las asociaciones con secuencias (la superviviente entra gracias a EnsureSurvivorRegistered)
                    size_t n=0; At<uint32>(updData.data(),0)=next()%2;
                    for(int32 k=0;k<nList&&n<11;++k)if(par[size_t(k)].hierSeqs)At<void*>(updData.data(),4+(n++)*4)=ND(size_t(k));
                    G<void*>(R::GPCLUMP)=R::cdata.data();
                }

                // ---- llamada ----
                const uintptr_t a=fn.address;
                json out=json::object();
                auto call=[&](auto&& body){
                    for(auto* h:all)h->State(false);
                    if(candidate){
                        if(!offscreenFn)updHook->State(true);
                        if(cppCb)for(auto* h:cbHooks)h->State(true);
                        if(offscreenFn)cbHooks[4]->State(true);
                    }
                    out["switches"]=ReversingProbe::JumpsToDll(a)==candidate;
                    { R::Patches p;
                      for(auto& s:stubs){ p.Want(s.exe,s.fake); p.Want(s.dll,s.fake); }
                      p.Apply(); { CallGuard guard(unsigned(f),unsigned(i),candidate); body(); } }
                    for(auto* h:all)h->State(false); };
                if(offscreenFn){
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(R::FD(0),updData.data()); });
                } else {
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,float,bool)>(a)(R::clump.data(),timeStep,visible); });
                }

                out["recs"]=R::Recs();
                out["clump"]=ReversingProbe::Bytes(R::clump.data(),R::clump.size()); out["cdata"]=ReversingProbe::Bytes(R::cdata.data(),R::cdata.size());
                out["assocs"]=ReversingProbe::Bytes(assocs.data(),assocs.size()); out["hiers"]=R::Fnv(hiers.data(),hiers.size());
                out["frameDatas"]=ReversingProbe::Bytes(R::frameDatas.data(),R::frameDatas.size()); out["frames"]=R::Fnv(R::frames.data(),R::frames.size());
                out["kfs"]=ReversingProbe::Bytes(R::kfs.data(),R::kfs.size()); out["ped"]=ReversingProbe::Bytes(R::pedPos.data(),R::pedPos.size());
                out["nodes"]=ReversingProbe::Bytes(nodes.data(),nodes.size()); out["seqs"]=R::Fnv(seqs.data(),seqs.size())^R::Fnv(keyframes.data(),keyframes.size());
                out["upd"]=ReversingProbe::Bytes(updData.data(),updData.size());
                out["fakes"]=R::Fnv(R::atomic.data(),R::atomic.size())^R::Fnv(R::geometry.data(),R::geometry.size())^R::Fnv(R::skin.data(),R::skin.size());
                out["globals"]={G<uint32>(R::FOUNDFRAME),G<uint32>(R::CLUMPOFF),R::U(G<void*>(R::GPCLUMP))};
                restoreAll();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["recs"]},{"candidate",candidate["recs"]},{"events",original["recs"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t k=0;k<all.size();++k){all[k]->State(enabled[k]);all[k]->LockState(locked[k]);}
    restoreAll();
    bool restored=true;
    for(size_t k=0;k<stubs.size();++k)restored&=std::memcmp(savedEntries[k].data(),reinterpret_cast<void*>(stubs[k].exe),5)==0;
    for(size_t k=0;k<all.size();++k)restored&=all[k]->Hooked()==enabled[k];
    for(auto& b:blobs)restored&=std::memcmp(reinterpret_cast<void*>(b.p),b.saved.data(),b.n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
