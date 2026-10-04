#pragma once
// Banco dentro del motor: IKChain_c (19 funciones de la lista de 20). Original (hooks del lote desactivados)
// frente a candidato (activados) sobre fixtures identicos y deterministas:
// - cadena falsa (0x58 bytes, relleno aleatorio) con nodos BoneNode_c reales inicializados con Init, AddChild y
//   CalcWldMat del EXE sobre cadenas de huesos reales de ms_boneInfos (efector -> pivote);
// - peaton falso (tipo peaton) con clump (tipo 2), datos de mezcla en ClumpOffset, AnimBlendFrameData y frames
//   interpolados propios, y una jerarquia RpHAnim falsa (IDs de ms_boneInfos, matrices aleatorias ortonormales);
// - dos entidades objetivo falsas con RwObject y RwFrame (matriz de modelado aleatoria) o sin RwObject.
// Se simulan y trazan: GetAnimHierarchyFromSkinClump (0x734A40), IKChain_c::SetupBones (0x617CA0),
// CPed::GetBonePosition (0x5E4280), BoneNodeManager_c::ReturnBoneNode (0x617470) y operator delete[] (0x8213AE,
// solo durante Exit y sin reservas). Las matematicas RW (RwV3dLength por tabla, normalizacion, cuaterniones) y
// BoneNode_c (GetSpeed, Limit, CalcWldMat, BlendKeyframe, ClampLimits*, limites) son las reales.
// Las referencias registradas en las entidades falsas se comparan y se limpian en cada ejecucion; los indicadores
// 0x8D2BD0/0x8D2BD1 (ClampLimits) y 0x8D2BD2 (Limit en MoveBonesToTarget) se alternan y se restauran.
// Fuera de contrato (no se generan): etiquetas que no estan en la cadena para ClampLimits/limites/velocidad (this
// nulo), etiquetas fuera de la jerarquia en Init (frames[-1]), cadenas vacias y |dot| > 1 en acos.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Ragdoll/IKChain.h"
#include "Ragdoll/BoneNode.h"
#include "Ragdoll/BoneNodeManager.h"
#include "../ProbeRuntime.hpp"

namespace IK20Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
struct Rng { uint32 s{1}; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; } float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; } };
inline Rng R;

constexpr int NODES=6, HNODES=40;
alignas(16) inline std::array<uint8,0x60> chain{};
alignas(16) inline std::array<std::array<uint8,sizeof(BoneNode_c)>,NODES> nodes{};
alignas(16) inline std::array<std::array<uint8,0x20>,NODES> nodeFrames{};
alignas(16) inline std::array<BoneNode_c*,8> bonePtrs{};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CPed)> target{};
alignas(16) inline std::array<uint8,sizeof(CPed)> target2{};
alignas(16) inline std::array<uint8,0x400> clump{};
alignas(16) inline std::array<uint8,0x20> animData{};
alignas(16) inline std::array<std::array<uint8,0x18>,HNODES> frameDatas{};
alignas(16) inline std::array<std::array<uint8,0x20>,HNODES> pedInterp{};
alignas(16) inline std::array<uint8,0x40> hier{};
alignas(16) inline std::array<std::array<int32,4>,HNODES> nodeInfo{};
alignas(16) inline std::array<RwMatrix,HNODES> mats{};
alignas(16) inline std::array<std::array<uint8,0x20>,2> tobj{};
alignas(16) inline std::array<std::array<uint8,0xC0>,2> tframe{};
inline const char purpose[]="IK20";

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(v==uintptr_t(purpose))return "purpose";
    if(v==0xC10820)return "bone_node_manager";
    if(auto s=in(chain.data(),chain.size(),"chain"))return *s; if(auto s=in(nodes.data(),sizeof(nodes),"nodes"))return *s;
    if(auto s=in(nodeFrames.data(),sizeof(nodeFrames),"node_frames"))return *s; if(auto s=in(bonePtrs.data(),sizeof(bonePtrs),"bone_ptrs"))return *s;
    if(auto s=in(ped.data(),ped.size(),"ped"))return *s; if(auto s=in(target.data(),target.size(),"target"))return *s;
    if(auto s=in(target2.data(),target2.size(),"target2"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(animData.data(),animData.size(),"anim_data"))return *s; if(auto s=in(frameDatas.data(),sizeof(frameDatas),"frame_datas"))return *s;
    if(auto s=in(pedInterp.data(),sizeof(pedInterp),"ped_interp"))return *s; if(auto s=in(hier.data(),hier.size(),"hier"))return *s;
    if(auto s=in(nodeInfo.data(),sizeof(nodeInfo),"node_info"))return *s; if(auto s=in(mats.data(),sizeof(mats),"mats"))return *s;
    if(auto s=in(tobj.data(),sizeof(tobj),"tobj"))return *s; if(auto s=in(tframe.data(),sizeof(tframe),"tframe"))return *s;
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(uintptr_t(w[k]))); return r; }
inline uint32& W(void* base,size_t off) { return *reinterpret_cast<uint32*>(static_cast<uint8*>(base)+off); }
inline float& Fl(void* base,size_t off) { return *reinterpret_cast<float*>(static_cast<uint8*>(base)+off); }

// --- registro sin reservas (ReturnBoneNode y delete[] mientras operator delete[] esta sustituido) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,64> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag==1 ? "return_bone_node" : "delete_array",Name(uintptr_t(recs[i].a)),Name(uintptr_t(recs[i].b))}); return r; }

// --- dependencias simuladas ---
inline CVector bonePosBase{}; inline uint32 bonePosCalls{};
inline RpHAnimHierarchy* __cdecl FHier(RpClump* c) { trace.push_back({"get_hierarchy",Name(c)}); return reinterpret_cast<RpHAnimHierarchy*>(hier.data()); }
inline void __fastcall FSetupBones(void* self,void*,int32 eff,float x,float y,float z,int32 piv,void* frames) {
    trace.push_back({"setup_bones",Name(self),eff,F(x),F(y),F(z),piv,Name(frames),Region(chain.data(),0x58)}); }
inline void __fastcall FBonePos(void* self,void*,CVector* out,int32 bone,bool upd) {
    trace.push_back({"get_bone_position",Name(self),Name(out),bone,upd});
    const float k=float(bonePosCalls++)*0.125f+float(bone%7)*0.0625f;
    *out=CVector{bonePosBase.x+k,bonePosBase.y-k*0.5f,bonePosBase.z+0.25f*k}; }
inline void __fastcall FReturnBone(void* self,void*,void* b) { Push(1,uint32(uintptr_t(self)),uint32(uintptr_t(b))); }
inline void __cdecl FDeleteArr(void* p) { Push(2,uint32(uintptr_t(p))); }

inline json RefList(void* e) { json l=json::array(); for(auto* r=reinterpret_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline void CleanRefs(void* p) { auto* e=reinterpret_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=19;
inline const std::array<Fn,NFNS> fns{{
    {"IKChain_c","Init",0x618370,0,-1,1024},{"IKChain_c","Exit",0x617870,0,-1,256},{"IKChain_c","Update",0x6184B0,0,-1,2048},
    {"IKChain_c","MoveBonesToTarget",0x6178B0,0,-1,2048},{"IKChain_c","IsAtTarget",0x617F30,0,-1,1024},{"IKChain_c","IsFacingTarget",0x617E60,0,-1,1024},
    {"IKChain_c","UpdateEntity",0x617E00,0,-1,512},{"IKChain_c","GetBoneNodeFromTag",0x617C60,0,-1,512},{"IKChain_c","ClampLimits",0x618520,0,-1,512},
    {"IKChain_c","GetLimits",0x618590,0,-1,512},{"IKChain_c","SetLimits",0x6185E0,0,-1,512},{"IKChain_c","GetBoneSpeed",0x618630,0,-1,256},
    {"IKChain_c","SetBoneSpeed",0x618680,0,-1,256},{"IKChain_c","GetPriority",0x617C50,0,-1,128},{"IKChain_c","SetBlend",0x617C10,0,-1,128},
    {"IKChain_c","SetOffsetBoneTag",0x617C20,0,-1,128},{"IKChain_c","SetOffsetPos",0x617C30,0,-1,128},{"IKChain_c","UpdateOffset",0x617E20,0,-1,128},
    {"IKChain_c","UpdateTarget",0x617E50,0,-1,128},
}};

inline int32 InfoTag(int i) { return int32(*reinterpret_cast<const int16*>(0x8D26D0+i*0x28)); }
inline int32 InfoParent(int32 tag) { for(int i=0;i<MAX_BONE_NUM;++i)if(InfoTag(i)==tag)return int32(*reinterpret_cast<const int16*>(0x8D26D0+i*0x28+2)); return -1; }

// Rellena todos los fixtures (deterministas con R). Devuelve las etiquetas de la cadena y la posicion raiz.
struct Fixture { std::array<int32,NODES> tags{}; int n{}; int32 pivot{}; CVector root{}; CVector effDir{}; };
inline Fixture Setup() {
    Fixture fx;
    auto randRot=[&](RwMatrix& m,const CVector& pos){ CMatrix cm{}; cm.SetRotate(R.uni(-3.1f,3.1f),R.uni(-3.1f,3.1f),R.uni(-3.1f,3.1f)); cm.GetPosition()=pos; cm.UpdateRwMatrix(&m); };
    const CVector base{R.uni(-60.f,60.f),R.uni(-60.f,60.f),R.uni(-5.f,25.f)};

    // jerarquia RpHAnim falsa: IDs de ms_boneInfos, 0 y algunos IDs sin uso, con un desplazamiento aleatorio
    const int rot=int(R.next()%HNODES);
    for(int k=0;k<HNODES;++k){ const int32 id=k<MAX_BONE_NUM ? InfoTag(k) : k==MAX_BONE_NUM ? 0 : 1000+k; auto& ni=nodeInfo[(k+rot)%HNODES]; ni={id,(k+rot)%HNODES,int32(R.next()%4),0}; }
    for(auto& m:mats)randRot(m,base+CVector{R.uni(-1.f,1.f),R.uni(-1.f,1.f),R.uni(-1.f,1.f)});
    std::memset(hier.data(),0,hier.size()); W(hier.data(),0)=R.next()%4; W(hier.data(),4)=HNODES; W(hier.data(),8)=uint32(uintptr_t(mats.data()));
    W(hier.data(),0xC)=uint32(uintptr_t(mats.data())); W(hier.data(),0x10)=uint32(uintptr_t(nodeInfo.data()));
    auto hierIndex=[&](int32 id){ for(int k=0;k<HNODES;++k)if(nodeInfo[k][0]==id)return k; return 0; };

    // cadena de huesos real: efector -> padres
    const int want=1+int(R.next()%NODES); int32 t=InfoTag(int(R.next()%MAX_BONE_NUM));
    while(fx.n<want){ fx.tags[fx.n++]=t; const int32 p=InfoParent(t); if(p<0)break; t=p; }
    fx.pivot=InfoParent(fx.tags[fx.n-1]); if(fx.pivot<0)fx.pivot=0;
    RwMatrix* const pivotMat=&mats[hierIndex(fx.pivot)];

    for(auto& b:bonePtrs)b=nullptr;
    for(int k=0;k<NODES;++k){ auto* p=nodes[k].data(); for(auto& b:nodes[k])b=uint8(R.next()); std::memset(p,0,0x3C); }
    for(int k=0;k<fx.n;++k) {
        auto* f=nodeFrames[k].data(); float q[4]; float m=0;
        for(auto& c:q){c=R.uni(-1.f,1.f);m+=c*c;} if(R.next()%3==0){q[0]*=0.1f;q[1]*=0.1f;q[2]*=0.1f;q[3]=1.f;m=q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+1.f;}
        m=std::sqrt(m); for(int c=0;c<4;++c)Fl(f,c*4)=q[c]/m;
        Fl(f,0x10)=R.uni(-0.3f,0.3f); Fl(f,0x14)=R.uni(-0.3f,0.3f); Fl(f,0x18)=R.uni(0.05f,0.4f);
        reinterpret_cast<bool(__thiscall*)(void*,int32,void*)>(0x6177B0)(nodes[k].data(),fx.tags[k],f);
        if(R.next()%3)Fl(nodes[k].data(),0x94)=R.uni(0.05f,1.2f); // m_Speed
        bonePtrs[k]=reinterpret_cast<BoneNode_c*>(nodes[k].data());
    }
    for(int k=0;k+1<fx.n;++k)reinterpret_cast<void(__thiscall*)(void*,void*)>(0x616BD0)(nodes[k+1].data(),nodes[k].data());
    reinterpret_cast<void(__thiscall*)(void*,const RwMatrix*)>(0x616CD0)(nodes[fx.n-1].data(),pivotMat);
    fx.root=*reinterpret_cast<CVector*>(nodes[0].data()+0x6C);

    // peaton y entidades falsas
    std::memset(ped.data(),0,ped.size()); std::memset(target.data(),0,target.size()); std::memset(target2.data(),0,target2.size());
    ped[0x36]=3; target[0x36]=3; target2[0x36]=3;
    std::memset(clump.data(),0,clump.size()); clump[0]=2; W(ped.data(),0x18)=uint32(uintptr_t(clump.data()));
    if(R.next()%8)W(ped.data(),0x474)|=0x20000000; // bHasBeenRendered
    W(ped.data(),0x474)|=R.next()&0x0F000000;
    const int32 clumpOffset=*reinterpret_cast<int32*>(0xB5F878);
    W(clump.data(),size_t(clumpOffset))=uint32(uintptr_t(animData.data()));
    std::memset(animData.data(),0,animData.size()); W(animData.data(),0x10)=uint32(uintptr_t(frameDatas.data())); W(animData.data(),8)=HNODES;
    for(int k=0;k<HNODES;++k) {
        auto* fd=frameDatas[k].data(); for(auto& b:frameDatas[k])b=uint8(R.next());
        const uint32 r=R.next()%10; fd[0]=uint8(r<8 ? 0x80|(R.next()&0x1F) : r==8 ? 0xA0|(R.next()&0x1F) : R.next()&0x5F);
        W(fd,0x10)=uint32(uintptr_t(pedInterp[k].data()));
        auto* pi=pedInterp[k].data(); for(int c=0;c<4;++c)Fl(pi,c*4)=R.uni(-1.f,1.f);
        Fl(pi,0x10)=R.uni(-0.5f,0.5f); Fl(pi,0x14)=R.uni(-0.5f,0.5f); Fl(pi,0x18)=R.uni(-0.5f,0.5f);
        const uint32 z=R.next()%12; if(z==0){Fl(pi,0x10)=0.f;Fl(pi,0x14)=0.f;Fl(pi,0x18)=0.f;} else if(z==1){Fl(pi,0x10)=-0.f;Fl(pi,0x14)=0.f;Fl(pi,0x18)=-0.f;} else if(z==2){Fl(pi,0x10)=0.f;Fl(pi,0x14)=0.f;}
    }
    for(int e=0;e<2;++e) {
        auto* ent=e ? target2.data() : target.data(); auto* o=tobj[e].data(); auto* fr=tframe[e].data();
        std::memset(o,0,tobj[e].size()); std::memset(fr,0,tframe[e].size()); o[0]=1; W(o,4)=uint32(uintptr_t(fr));
        randRot(*reinterpret_cast<RwMatrix*>(fr+0x10),fx.root+CVector{R.uni(-3.f,3.f),R.uni(-3.f,3.f),R.uni(-1.f,1.f)});
        if(R.next()%5)W(ent,0x18)=uint32(uintptr_t(o));
        *reinterpret_cast<CVector*>(ent+4)=fx.root;
    }

    // cadena: relleno aleatorio y campos
    auto* c=chain.data(); for(auto& b:chain)b=uint8(R.next());
    W(c,8)=uint32(uintptr_t(ped.data())); W(c,0xC)=uint32(fx.n); W(c,0x10)=uint32(uintptr_t(bonePtrs.data())); W(c,0x14)=uint32(uintptr_t(pivotMat));
    Fl(c,0x18)=R.next()%4==0 ? R.uni(0.97f,1.f) : R.uni(0.f,1.f); if(R.next()%10==0)Fl(c,0x18)=0.98f;
    *reinterpret_cast<int16*>(c+0x1C)=int16(fx.tags[0]);
    CVector eff{R.uni(-0.3f,0.3f),R.uni(-0.3f,0.3f),R.uni(-0.3f,0.3f)}; if(R.next()%10==0)eff=CVector{0.f,0.f,0.f};
    *reinterpret_cast<CVector*>(c+0x20)=eff;
    *reinterpret_cast<int16*>(c+0x2C)=int16(fx.pivot);
    W(c,0x30)=0; W(c,0x34)=R.next()%3 ? uint32(-1) : uint32(InfoTag(int(R.next()%MAX_BONE_NUM)));
    *reinterpret_cast<CVector*>(c+0x38)=CVector{R.uni(-1.f,1.f),R.uni(-1.f,1.f),R.uni(-1.f,1.f)};
    Fl(c,0x44)=R.uni(0.02f,1.f); if(R.next()%8==0)Fl(c,0x44)=1.f;
    // direccion del efector en el mundo (solo para elegir objetivos)
    CVector ed; RwV3dTransformVector(&ed,&eff,reinterpret_cast<RwMatrix*>(nodes[0].data()+0x3C)); const float el=ed.Magnitude();
    fx.effDir=el>1e-4f ? ed/el : CVector{0.f,0.f,1.f};
    CVector ws;
    switch(R.next()%6) {
    case 0: case 1: ws=fx.root+CVector{R.uni(-2.f,2.f),R.uni(-2.f,2.f),R.uni(-2.f,2.f)}; break;
    case 2: ws=*reinterpret_cast<CVector*>(nodes[R.next()%fx.n].data()+0x6C); break; // objetivo sobre un hueso (tolerancia)
    case 3: { CVector q{R.uni(-1.f,1.f),R.uni(-1.f,1.f),R.uni(-1.f,1.f)}; q-=fx.effDir*DotProduct(q,fx.effDir); const float ql=q.Magnitude(); if(ql>1e-4f)q/=ql;
              const float a=R.uni(0.f,0.7f); ws=fx.root+(fx.effDir*std::cos(a)+q*std::sin(a))*R.uni(0.2f,3.f); break; } // cerca de 0.95/0.997
    case 4: ws=fx.root+fx.effDir*R.uni(0.2f,3.f); break;
    default: ws=fx.root+CVector{R.uni(-40.f,40.f),R.uni(-40.f,40.f),R.uni(-10.f,10.f)}; break;
    }
    *reinterpret_cast<CVector*>(c+0x48)=ws; *reinterpret_cast<CVector*>(c+0x38)=R.next()%2 ? ws : *reinterpret_cast<CVector*>(c+0x38);
    c[0x54]=uint8(R.next()%2); c[0x55]=uint8(R.next()%4); c[0x56]=uint8(R.next());
    bonePosBase=fx.root+CVector{R.uni(-1.f,1.f),R.uni(-1.f,1.f),R.uni(-1.f,1.f)}; bonePosCalls=0;
    return fx;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ik20-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ik20-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ik20");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ik20-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    auto* setupHook=static_cast<ReversibleHooks::ReversibleHook::Simple*>(FindHook("IKChain_c","SetupBones"));
    if(!setupHook){Write({{"event","error"},{"reason","hook not found"},{"function","IKChain_c::SetupBones"}});return;}
    const int32 clumpOffset=*reinterpret_cast<int32*>(0xB5F878);
    if(clumpOffset<4||clumpOffset+4>int32(clump.size())){Write({{"event","error"},{"reason","ClumpOffset fuera del clump falso"},{"offset",clumpOffset}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    auto& skipDefault=*reinterpret_cast<uint8*>(0x8D2BD0); auto& skipCurrent=*reinterpret_cast<uint8*>(0x8D2BD1); auto& limitBones=*reinterpret_cast<uint8*>(0x8D2BD2);
    const uint8 savedSkipDefault=skipDefault, savedSkipCurrent=skipCurrent, savedLimit=limitBones;
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),FHier), s1e(0x734A40,FHier);
        EntryPatch s2(uintptr_t(setupHook->GetHookOurAddress()),FSetupBones), s2e(0x617CA0,FSetupBones);
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),FBonePos), s3e(0x5E4280,FBonePos);
        EntryPatch s4(bit_cast<uintptr_t>(&BoneNodeManager_c::ReturnBoneNode),FReturnBone), s4e(0x617470,FReturnBone);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    R.s=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1C20u; if(!R.s)R.s=1;
                    trace=json::array(); nrec=0;
                    skipDefault=uint8(R.next()%2); skipCurrent=uint8(R.next()%2); limitBones=uint8(R.next()%6!=0);
                    const Fixture fx=Setup();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address; uint8* const C=chain.data();
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto chainTag=[&]{ return fx.tags[R.next()%fx.n]; };
                    auto entityPick=[&](uint32 r)->void*{ switch(r%4){ case 0: return nullptr; case 1: return target.data(); case 2: return target2.data(); default: return ped.data(); } };
                    bool dumpNodes=false;

                    if(nm=="Init") {
                        const uint32 slot=R.next()%5; const int32 eff=R.next()%8 ? fx.tags[0] : InfoTag(int(R.next()%MAX_BONE_NUM));
                        const CVector ep{R.uni(-0.3f,0.3f),R.uni(-0.3f,0.3f),R.uni(-0.3f,0.3f)}; void* ent=entityPick(R.next());
                        const int32 off=R.next()%3 ? -1 : InfoTag(int(R.next()%MAX_BONE_NUM)); const CVector op{R.uni(-2.f,2.f),R.uni(-2.f,2.f),R.uni(-2.f,2.f)};
                        const float speed=R.uni(0.f,1.f); const int32 prio=int32(R.next()%600)-300;
                        ret=reinterpret_cast<bool(__thiscall*)(void*,const char*,int32,void*,int32,float,float,float,int32,void*,int32,float,float,float,float,int32)>(a)
                            (C,purpose,int32(slot),ped.data(),eff,ep.x,ep.y,ep.z,fx.pivot,ent,off,op.x,op.y,op.z,speed,prio);
                    }
                    else if(nm=="Exit") {
                        { EntryPatch d(0x8213AE,FDeleteArr); reinterpret_cast<void(__thiscall*)(void*)>(a)(C); }
                        out["recs"]=Recs();
                    }
                    else if(nm=="Update") {
                        if(R.next()%2)W(C,0x30)=uint32(uintptr_t(entityPick(R.next()%3))); // el peaton (clump sin RwFrame) daria una matriz 0x10
                        reinterpret_cast<void(__thiscall*)(void*,float)>(a)(C,R.uni(0.f,0.1f)); dumpNodes=true;
                    }
                    else if(nm=="MoveBonesToTarget") {
                        if(R.next()%2)W(C,0x30)=uint32(uintptr_t(entityPick(R.next()%3)));
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(C); dumpNodes=true;
                    }
                    else if(nm=="IsAtTarget") {
                        const float range=R.next()%8==0 ? (*reinterpret_cast<CVector*>(C+0x48)-fx.root).Magnitude() : R.uni(0.f,3.f);
                        float dist=-1.f; const bool useOut=R.next()%4!=0;
                        ret=reinterpret_cast<bool(__thiscall*)(void*,float,float*)>(a)(C,range,useOut ? &dist : nullptr); out["dist"]=F(dist);
                    }
                    else if(nm=="IsFacingTarget") ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(C);
                    else if(nm=="UpdateEntity") {
                        void* old=entityPick(R.next()%3); W(C,0x30)=uint32(uintptr_t(old));
                        if(old)reinterpret_cast<void(__thiscall*)(void*,void*)>(0x571B70)(old,C+0x30);
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(C,entityPick(R.next()%3));
                    }
                    else if(nm=="GetBoneNodeFromTag") {
                        const int32 tag=R.next()%4 ? chainTag() : (R.next()%2 ? InfoTag(int(R.next()%MAX_BONE_NUM)) : int32(R.next()%3000)-1);
                        ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(C,tag));
                    }
                    else if(nm=="ClampLimits") {
                        reinterpret_cast<void(__thiscall*)(void*,int32,int32,int32,int32,int32)>(a)(C,chainTag(),int32(R.next()%2),int32(R.next()%2),int32(R.next()%2),int32(R.next()%2));
                        dumpNodes=true;
                    }
                    else if(nm=="GetLimits") {
                        float mn=-7.f,mx=-7.f; reinterpret_cast<void(__thiscall*)(void*,int32,int32,float&,float&)>(a)(C,chainTag(),int32(R.next()%3),mn,mx); out["limits"]={F(mn),F(mx)};
                    }
                    else if(nm=="SetLimits") {
                        reinterpret_cast<void(__thiscall*)(void*,int32,int32,float,float)>(a)(C,chainTag(),int32(R.next()%3),R.uni(-180.f,180.f),R.uni(-180.f,180.f)); dumpNodes=true;
                    }
                    else if(nm=="GetBoneSpeed") ret=F(reinterpret_cast<float(__thiscall*)(void*,int32)>(a)(C,chainTag()));
                    else if(nm=="SetBoneSpeed") { reinterpret_cast<void(__thiscall*)(void*,int32,float)>(a)(C,chainTag(),R.uni(-2.f,2.f)); dumpNodes=true; }
                    else if(nm=="GetPriority") ret=int(reinterpret_cast<int8(__thiscall*)(void*)>(a)(C));
                    else if(nm=="SetBlend") reinterpret_cast<void(__thiscall*)(void*,float)>(a)(C,R.uni(-1.f,2.f));
                    else if(nm=="SetOffsetBoneTag") reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(C,R.next()%3 ? -1 : int32(R.next()%40));
                    else if(nm=="SetOffsetPos") reinterpret_cast<void(__thiscall*)(void*,float,float,float)>(a)(C,R.uni(-50.f,50.f),R.uni(-50.f,50.f),R.uni(-50.f,50.f));
                    else if(nm=="UpdateOffset") reinterpret_cast<void(__thiscall*)(void*,int32,float,float,float)>(a)(C,R.next()%3 ? -1 : int32(R.next()%40),R.uni(-5.f,5.f),R.uni(-5.f,5.f),R.uni(-5.f,5.f));
                    else if(nm=="UpdateTarget") reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(C,int32(R.next()%2));

                    out["chain"]=Region(C,0x58);
                    if(dumpNodes){ out["nodes"]=Region(nodes.data(),sizeof(nodes)); out["node_frames"]=Region(nodeFrames.data(),sizeof(nodeFrames)); }
                    out["refs"]=json::array({RefList(ped.data()),RefList(target.data()),RefList(target2.data())});
                    CleanRefs(ped.data()); CleanRefs(target.data()); CleanRefs(target2.data());
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    skipDefault=savedSkipDefault; skipCurrent=savedSkipCurrent; limitBones=savedLimit;
    CleanRefs(ped.data()); CleanRefs(target.data()); CleanRefs(target2.data());
    const bool restored=skipDefault==savedSkipDefault&&skipCurrent==savedSkipCurrent&&limitBones==savedLimit&&freeRefs()==savedFree;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
