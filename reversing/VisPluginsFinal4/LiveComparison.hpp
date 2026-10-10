#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1/VehModelInfoFinal4): CVisibilityPlugins, 84 funciones (lista final 4).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato (hooks activos) sobre un mundo RW falso y
// determinista: atomics, frames, clumps, geometrias, materiales, texturas, model infos, colisiones, entidades y peds en buffers
// estaticos propios (con los offsets de plugin leidos del EXE), listas de alfa del EXE reconstruidas sobre nodos propios. Las
// primitivas RW, el render real (AtomicDefaultRenderCallBack, RpClumpRender), los estados de render (puntero del dispositivo),
// las luces, la tabla de armas, los metodos de CPed, operator new/delete y el registro de plugins se simulan con trazas (direccion
// del EXE y simbolo de la DLL). Cada global tocado se guarda y se restaura en cada ejecucion. Ver README.md.
#include "CasesB.hpp"

namespace VisPluginsFinal4Live {
inline const std::array<Fn,84> fns{{
    {"CVisibilityPlugins","AtomicCopyConstructor",0x732170,0,-1,96},{"CVisibilityPlugins","AtomicDestructor",0x7321A0,0,-1,96},
    {"CVisibilityPlugins","ClearAtomicFlag",0x732310,0,-1,96},{"CVisibilityPlugins","ClearClumpForAllAtomicsFlag",0x732350,0,-1,96},
    {"CVisibilityPlugins","ClumpConstructor",0x732E10,0,-1,96},{"CVisibilityPlugins","ClumpCopyConstructor",0x732200,0,-1,96},
    {"CVisibilityPlugins","ClumpDestructor",0x732220,0,-1,96},{"CVisibilityPlugins","DefaultVisibilityCB",0x732A30,0,-1,96},
    {"CVisibilityPlugins","FrameConstructor",0x7321B0,0,-1,96},{"CVisibilityPlugins","FrameCopyConstructor",0x7321D0,0,-1,96},
    {"CVisibilityPlugins","FrameDestructor",0x7321F0,0,-1,96},{"CVisibilityPlugins","GetAtomicId",0x732370,0,-1,96},
    {"CVisibilityPlugins","GetClumpAlpha",0x732B20,0,-1,96},{"CVisibilityPlugins","GetClumpModelInfo",0x732AC0,0,-1,96},
    {"CVisibilityPlugins","GetFrameHierarchyId",0x732A20,0,-1,96},{"CVisibilityPlugins","GetModelInfo",0x732260,0,-1,96},
    {"CVisibilityPlugins","GetModelInfoIndex",0x732250,0,-1,96},{"CVisibilityPlugins","GetUserValue",0x7323A0,0,-1,96},
    {"CVisibilityPlugins","InitAlphaAtomicList",0x734530,0,-1,96},{"CVisibilityPlugins","InitAlphaEntityList",0x734540,0,-1,96},
    {"CVisibilityPlugins","Initialise",0x733A20,0,-1,96},{"CVisibilityPlugins","IsClumpVisible",0x732AE0,0,-1,96},
    {"CVisibilityPlugins","SetAtomicFlag",0x7322B0,0,-1,96},{"CVisibilityPlugins","SetAtomicId",0x732290,0,-1,96},
    {"CVisibilityPlugins","SetClumpAlpha",0x732B00,0,-1,96},{"CVisibilityPlugins","SetClumpForAllAtomicsFlag",0x7322F0,0,-1,96},
    {"CVisibilityPlugins","SetFrameHierarchyId",0x732A00,0,-1,96},{"CVisibilityPlugins","SetModelInfoIndex",0x732230,0,-1,96},
    {"CVisibilityPlugins","SetTextureCB",0x732850,0,-1,96},{"CVisibilityPlugins","SetUserValue",0x732380,0,-1,96},
    {"CVisibilityPlugins","clearClumpForAllAtomicsFlagCB",0x732330,0,-1,96},{"CVisibilityPlugins","setClumpForAllAtomicsFlagCB",0x7322D0,0,-1,96},
    {"CVisibilityPlugins","SetAlphaCB",0x7323B0,0,-1,128},{"CVisibilityPlugins","SetAtomicRenderCallback",0x7328A0,0,-1,128},
    {"CVisibilityPlugins","AddWeaponPedForPC",0x5E46D0,0,-1,192},{"CVisibilityPlugins","PluginAttach",0x732E30,0,-1,192},
    {"CVisibilityPlugins","RenderOrderedList",0x7337A0,0,-1,192},{"CVisibilityPlugins","SetClumpModelInfo",0x733750,0,-1,192},
    {"CVisibilityPlugins","Shutdown",0x732EB0,0,-1,192},{"CVisibilityPlugins","VehicleVisibilityCB_BigVehicle",0x732AB0,0,-1,192},
    {"CVisibilityPlugins","FrustumSphereCB",0x732A40,0,-1,256},{"CVisibilityPlugins","GetDistanceSquaredFromCamera-Frame",0x732C80,0,-1,256},
    {"CVisibilityPlugins","GetDistanceSquaredFromCamera-RwV3d",0x732CC0,0,-1,256},{"CVisibilityPlugins","InsertAtomicIntoReallyDrawLastList",0x733E10,0,-1,256},
    {"CVisibilityPlugins","InsertEntityIntoReallyDrawLastList",0x733E50,0,-1,256},{"CVisibilityPlugins","InsertEntityIntoUnderwaterList",0x733D90,0,-1,256},
    {"CVisibilityPlugins","InsertObjectIntoSortedList",0x733DD0,0,-1,256},{"CVisibilityPlugins","IsAtomicVisible",0x732990,0,-1,256},
    {"CVisibilityPlugins","RenderAlphaAtomics",0x733E90,0,-1,256},{"CVisibilityPlugins","RenderAtomicWithAlphaCB",0x732660,0,-1,256},
    {"CVisibilityPlugins","RenderBoatAlphaAtomics",0x733EC0,0,-1,256},{"CVisibilityPlugins","RenderFadingAtomic",0x732610,0,-1,256},
    {"CVisibilityPlugins","RenderFadingClump",0x732680,0,-1,256},{"CVisibilityPlugins","RenderFadingClumpCB",0x733630,0,-1,256},
    {"CVisibilityPlugins","RenderFadingEntities",0x733F10,0,-1,256},{"CVisibilityPlugins","RenderFadingUnderwaterEntities",0x7337D0,0,-1,256},
    {"CVisibilityPlugins","RenderHeliRotorAlphaCB",0x7340B0,0,-1,256},{"CVisibilityPlugins","RenderHeliTailRotorAlphaCB",0x734170,0,-1,256},
    {"CVisibilityPlugins","RenderObjNormalAtomic",0x7323E0,0,-1,256},{"CVisibilityPlugins","RenderPedCB",0x7335B0,0,-1,256},
    {"CVisibilityPlugins","RenderPlayerCB",0x732870,0,-1,256},{"CVisibilityPlugins","RenderReallyDrawLastObjects",0x733800,0,-1,256},
    {"CVisibilityPlugins","RenderVehicleHiDetailAlphaCB_Boat",0x7344A0,0,-1,256},{"CVisibilityPlugins","RenderVehicleHiDetailCB_Boat",0x733550,0,-1,256},
    {"CVisibilityPlugins","RenderVehicleLoDetailCB_Boat",0x7334F0,0,-1,256},{"CVisibilityPlugins","RenderVehicleReallyLowDetailCB",0x7331E0,0,-1,256},
    {"CVisibilityPlugins","RenderVehicleReallyLowDetailCB_BigVehicle",0x732820,0,-1,256},{"CVisibilityPlugins","RenderWeaponCB",0x733670,0,-1,256},
    {"CVisibilityPlugins","RenderWheelAtomicCB",0x7323C0,0,-1,256},{"CVisibilityPlugins","SetRenderWareCamera",0x7328C0,0,-1,256},
    {"CVisibilityPlugins","SetupVehicleVariables",0x733160,0,-1,256},{"CVisibilityPlugins","VehicleVisibilityCB",0x7336F0,0,-1,256},
    {"CVisibilityPlugins","InsertEntityIntoSortedList",0x734570,0,-1,384},{"CVisibilityPlugins","RenderAtomicWithAlpha",0x732480,0,-1,384},
    {"CVisibilityPlugins","RenderTrainHiDetailAlphaCB",0x734240,0,-1,384},{"CVisibilityPlugins","RenderTrainHiDetailCB",0x733330,0,-1,384},
    {"CVisibilityPlugins","RenderVehicleHiDetailCB_BigVehicle",0x733420,0,-1,384},{"CVisibilityPlugins","CalculateFadingAtomicAlpha",0x732500,0,-1,512},
    {"CVisibilityPlugins","GetDotProductWithCameraVector",0x7326D0,0,-1,512},{"CVisibilityPlugins","RenderEntity",0x732B40,0,-1,512},
    {"CVisibilityPlugins","RenderVehicleHiDetailAlphaCB",0x733F80,0,-1,512},{"CVisibilityPlugins","RenderVehicleHiDetailAlphaCB_BigVehicle",0x734370,0,-1,512},
    {"CVisibilityPlugins","RenderVehicleHiDetailCB",0x733240,0,-1,512},{"CVisibilityPlugins","RenderWeaponPedsForPC",0x732F30,0,-1,512},
}};

// Registra los stubs (direccion del EXE + simbolo de la DLL). Devuelve el nombre de lo que falte.
inline std::string RegisterStubs(){
    stubs.clear();
    auto hk=[&](uint32 g,uintptr_t exe,const char* cls,const char* name,void* fn)->std::string{ return AddHookStub(g,exe,cls,name,fn) ? std::string() : std::string(cls)+"::"+name; };
    std::string miss;
    auto chk=[&](const std::string& s){ if(!s.empty())miss+=s+" "; };
    AddStub(G_RW,0x749B70,0,FnPtr(&S_ClumpForAllAtomics)); AddStub(G_RW,0x74C790,0,FnPtr(&S_GeometryForAllMaterials));
    AddStub(G_RW,0x7F0990,0,FnPtr(&S_FrameGetLTM)); AddStub(G_RW,0x7F0910,0,FnPtr(&S_FrameUpdateObjects));
    AddStub(G_RW,0x7F2450,0,FnPtr(&S_MatrixTranslate)); AddStub(G_RW,0x7F1FD0,0,FnPtr(&S_MatrixRotate));
    AddStub(G_RW,0x749B20,0,FnPtr(&S_ClumpRender)); AddStub(G_RW,0x7C51A0,0,FnPtr(&S_HAnimIndex)); AddStub(G_RW,0x7C5120,0,FnPtr(&S_HAnimMatrices));
    AddStub(G_RW,0x734A40,bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),FnPtr(&S_AnimHierarchy));
    AddStub(G_RW,0x7491C0,0,FnPtr(&S_AtomicDefaultRender)); AddStub(G_RW,0x7491F0,0,FnPtr(&S_Resync));
    AddStub(G_RW,0x7EDD90,0,FnPtr(&S_V3dTransformPoints)); AddStub(G_RW,0x7EE2D0,0,FnPtr(&S_FrustumTest)); AddStub(G_RW,0x74DBC0,0,FnPtr(&S_MaterialSetTexture));
    AddStub(G_PLUG,0x74BDA0,0,FnPtr(&S_RegAtomic)); AddStub(G_PLUG,0x7F1260,0,FnPtr(&S_RegFrame)); AddStub(G_PLUG,0x74BDD0,0,FnPtr(&S_RegClump));
    AddStub(G_RW,0x735D30,bit_cast<uintptr_t>(static_cast<void(*)()>(&SetAmbientColours)),FnPtr(&S_AmbientColours));
    AddStub(G_RW,0x735C70,bit_cast<uintptr_t>(&DeActivateDirectional),FnPtr(&S_DeActivateDirectional));
    chk(hk(G_RW,0x553260,"CRenderer","RenderOneNonRoad",FnPtr(&S_RenderOneNonRoad)));
    chk(hk(G_RW,0x5DF400,"CPed","SetGunFlashAlpha",FnPtr(&S_SetGunFlash))); chk(hk(G_RW,0x5DF4E0,"CPed","ResetGunFlashAlpha",FnPtr(&S_ResetGunFlash)));
    chk(hk(G_RW,0x5E6580,"CPed","GetWeaponSkill-Current",FnPtr(&S_WeaponSkill)));
    chk(hk(G_RW,0x743C60,"CWeaponInfo","GetWeaponInfo-original",FnPtr(&S_WeaponInfo)));
    AddStub(G_NEW,0x82119A,0,FnPtr(&S_New)); AddStub(G_NEW,0x821195,0,FnPtr(&S_New)); AddStub(G_NEW,0x8214BD,0,FnPtr(&S_Delete)); AddStub(G_NEW,0x8213AE,0,FnPtr(&S_Delete));
    return miss;
}

inline size_t arenaHi{0};   // mayor uso de la arena hasta ahora (Update la rellena entera con 0xCD al empezar)
inline std::array<std::array<uint8,0x20000>,2> arenaDump{}; inline std::array<size_t,2> dumpLen{}; inline std::array<World,2> wdump{};
// Resumen compacto y determinista (nunca nulo, tambien para funciones void): retorno, nº de eventos y hashes de traza, mundo, arena y globales.
inline json Summary(const json& o){
    return json{{"ret",o["ret"]},{"events",o["trace"].size()},{"trace_fnv",Fnv64(o["trace"].dump().data(),o["trace"].dump().size())},
                {"world",o["world"]},{"arena",o["arena"]},{"arena_top",o["arena_top"]},{"globals",o["globals"]},
                {"fields_fnv",Fnv64(o.dump().data(),o.dump().size())}};
}

inline bool Layout(){
    bool ok=true; json l=json::object();
    auto chk=[&](const char* n,size_t got,size_t want){ const bool g=got==want; ok&=g; l[n]=g; };
    alignas(16) static uint8 eb[0x200], mb[0x100], cb[0x80], pb[0x800], wb[0x80];
    std::memset(eb,0,sizeof(eb)); std::memset(mb,0,sizeof(mb)); std::memset(pb,0,sizeof(pb)); std::memset(wb,0,sizeof(wb));
    auto* e=reinterpret_cast<CEntity*>(eb);
    At<uint32>(eb,0x18)=0x11223344u; chk("ent_rw",uintptr_t(e->GetRwObject()),0x11223344u); std::memset(eb,0,sizeof(eb)); chk("ent_model",reinterpret_cast<uint8*>(&e->m_nModelIndex)-eb,0x22); At<uint32>(eb,0x30)=0x55667788u; chk("ent_lod",uintptr_t(e->GetLod()),0x55667788u); std::memset(eb,0,sizeof(eb));
    e->m_bDistanceFade=true; chk("ent_fade",At<uint32>(eb,0x1C),1u<<15); std::memset(eb,0,sizeof(eb));
    e->m_bImBeingRendered=true; chk("ent_render",At<uint32>(eb,0x1C),1u<<13); std::memset(eb,0,sizeof(eb));
    e->m_bUnderwater=true; chk("ent_under",At<uint32>(eb,0x1C),1u<<20); std::memset(eb,0,sizeof(eb));
    e->m_bBackfaceCulled=true; chk("ent_back",At<uint32>(eb,0x1C),1u<<27); std::memset(eb,0,sizeof(eb));
    e->m_bIsBIGBuilding=true; chk("ent_big",At<uint32>(eb,0x1C),1u<<8); std::memset(eb,0,sizeof(eb));
    auto* m=reinterpret_cast<CBaseModelInfo*>(mb);
    m->bDontWriteZBuffer=true; chk("mi_zwrite",mb[0x12],8); std::memset(mb,0,sizeof(mb));
    m->bAdditiveRender=true; chk("mi_add",mb[0x12],4); std::memset(mb,0,sizeof(mb));
    chk("mi_alpha",reinterpret_cast<uint8*>(&m->m_nAlpha)-mb,0x0C); chk("mi_col",reinterpret_cast<uint8*>(&m->m_pColModel)-mb,0x14); chk("mi_draw",reinterpret_cast<uint8*>(&m->m_fDrawDistance)-mb,0x18);
    chk("vmi_type",offsetof(CVehicleModelInfo,m_nVehicleType),0x3C);
    auto* col=reinterpret_cast<CColModel*>(cb);
    chk("col_center",reinterpret_cast<uint8*>(&col->GetBoundCenter())-cb,0x18); chk("col_radius",reinterpret_cast<uint8*>(&col->m_boundSphere.m_fRadius)-cb,0x24);
    auto* p=reinterpret_cast<CPed*>(pb);
    chk("ped_wobj",reinterpret_cast<uint8*>(&p->m_pWeaponObject)-pb,0x4F4); chk("ped_slot",reinterpret_cast<uint8*>(&p->m_nActiveWeaponSlot)-pb,0x718);
    chk("ped_weapons",reinterpret_cast<uint8*>(p->m_aWeapons.data())-pb,0x5A0); chk("weapon_size",sizeof(CWeapon),0x1C); chk("weapon_type",offsetof(CWeapon,m_Type),0);
    reinterpret_cast<CWeaponInfo*>(wb)->flags.bTwinPistol=true; chk("wi_twin",At<uint32>(wb,0x18),0x800);
    chk("cam_lod",uintptr_t(&TheCamera.m_fLODDistMultiplier),0xB6F118); chk("cam_gen",uintptr_t(&TheCamera.m_fGenerationDistMultiplier),0xB6F11C);
    chk("far_clip",uintptr_t(&CRenderer::ms_fFarClipPlane),0xB76848); chk("lowlod",uintptr_t(&CRenderer::ms_lowLodDistScale),0x8CD804);
    chk("skin",uintptr_t(&CWorld::Players[0].m_pSkinTexture),0xB7CF1C); chk("curarea",uintptr_t(&CGame::currArea),0xB72914);
    chk("grasshouse",uintptr_t(&ModelIndices::MI_GRASSHOUSE),0x8CD6F4);
    chk("at_geo",offsetof(RpAtomic,geometry),0x18); chk("at_bound",offsetof(RpAtomic,boundingSphere),0x1C); chk("at_clump",offsetof(RpAtomic,clump),0x3C); chk("at_cb",offsetof(RpAtomic,renderCallBack),0x48);
    chk("at_interp",offsetof(RpAtomic,interpolator)+offsetof(RpInterpolator,flags),0x4C); chk("obj_parent",offsetof(RwObject,parent),4);
    chk("fr_model",offsetof(RwFrame,modelling),0x10); chk("fr_ltm",offsetof(RwFrame,ltm),0x50);
    chk("geo_flags",offsetof(RpGeometry,flags),8); chk("geo_mats",offsetof(RpGeometry,matList)+offsetof(RpMaterialList,materials),0x20); chk("geo_nmats",offsetof(RpGeometry,matList)+offsetof(RpMaterialList,numMaterials),0x24);
    chk("mat_tex",offsetof(RpMaterial,texture),0); chk("mat_col",offsetof(RpMaterial,color),4);
    chk("mx_right",offsetof(RwMatrix,right),0); chk("mx_up",offsetof(RwMatrix,up),0x10); chk("mx_at",offsetof(RwMatrix,at),0x20); chk("mx_pos",offsetof(RwMatrix,pos),0x30); chk("mx_size",sizeof(RwMatrix),0x40);
    using AO=CVisibilityPlugins::AlphaObjectInfo; using LL=CLinkList<AO>; using LP=CLinkList<CPed*>;
    chk("ao_cb",offsetof(AO,m_pCallback),4); chk("ao_dist",offsetof(AO,m_distance),8); chk("link_size",sizeof(CLink<AO>),0x14); chk("link_prev",offsetof(CLink<AO>,prev),0xC); chk("link_next",offsetof(CLink<AO>,next),0x10);
    chk("ll_tail",offsetof(LL,usedListTail),0x14); chk("ll_fh",offsetof(LL,freeListHead),0x28); chk("ll_ft",offsetof(LL,freeListTail),0x3C); chk("ll_links",offsetof(LL,links),0x50);
    chk("plink_size",sizeof(CLink<CPed*>),0xC); chk("pl_tail",offsetof(LP,usedListTail),0xC); chk("pl_fh",offsetof(LP,freeListHead),0x18); chk("pl_ft",offsetof(LP,freeListTail),0x24); chk("pl_links",offsetof(LP,links),0x30);
    chk("sz_atomplug",sizeof(tAtomicVisibilityPlugin),4); chk("sz_clumpplug",sizeof(tClumpVisibilityPlugin),8); chk("sz_frameplug",sizeof(tFrameVisibilityPlugin),4);
    chk("atomic_flat",ATOMIC_FLAT,0x80); chk("atomic_top",ATOMIC_TOP,0x8000); chk("atomic_alpha",ATOMIC_ALPHA,0x40); chk("atomic_dontcull",ATOMIC_DONT_CULL,0x400); chk("atomic_unique",ATOMIC_UNIQUE_MATERIALS,0x2000);
    chk("t_train",VEHICLE_TYPE_TRAIN,6); chk("t_fplane",VEHICLE_TYPE_FPLANE,8); chk("mt_vehicle",MODEL_INFO_VEHICLE,6); chk("parachute",WEAPON_PARACHUTE,46);
    chk("bone_rhand",BONE_R_HAND,24); chk("bone_lhand",BONE_L_HAND,34); chk("bone_spine1",BONE_SPINE1,3);
    Write({{"event","layout"},{"ok",ok},{"checks",l}});
    return ok;
}

inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"vispluginsfinal4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"vispluginsfinal4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("vispluginsfinal4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"vispluginsfinal4-only.txt"); if(f)std::getline(f,only); }

    std::array<ReversibleHooks::ReversibleHook::Base*,84> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    atomOff=*reinterpret_cast<int32*>(0x8D608C); frameOff=*reinterpret_cast<int32*>(0x8D6090); clumpOff=*reinterpret_cast<int32*>(0x8D6094);
    if(atomOff<0x50||atomOff+4>int32(AT_SIZE)||frameOff<0xA0||frameOff+4>int32(FR_SIZE)||clumpOff<0x28||clumpOff+8>int32(CL_SIZE)){
        Write({{"event","error"},{"reason","plugin offsets out of range"},{"atomic",atomOff},{"frame",frameOff},{"clump",clumpOff}});return; }
    if(!Layout()){ Write({{"event","error"},{"reason","layout"}}); return; }
    const auto miss=RegisterStubs(); if(!miss.empty()){ Write({{"event","error"},{"reason","stubs not found"},{"missing",miss}}); return; }
    rsSlot=*reinterpret_cast<uint32*>(0xC97B24)+0x20;
    if(!IsCode(*reinterpret_cast<uint32*>(rsSlot))){ Write({{"event","error"},{"reason","render state slot"}}); return; }

    // Pares funcion del EXE <-> destino de la DLL (para normalizar los punteros a funciones que la DLL guarda)
    ncbs=0; for(size_t f=0;f<fns.size();++f)if(fns[f].address)AddCb(fns[f].address,OurAddress(hooks[f]));
    AddCb(EXE_RENDERATOMIC,Body(bit_cast<uintptr_t>(&CVisibilityPlugins::RenderAtomic))); AddCb(0x732150,0);
    FinishCbs();

    nregs=0; Reg(&w,sizeof(World)); Reg(arena.data(),arena.size());
    saved.clear();
    Save(GLOB_BASE,GLOB_SIZE);           // variables y listas de la clase (0xC88000..0xC88260)
    Save(0x8D608C,12);                   // offsets de plugin
    Save(0xB6F118,8);                    // multiplicadores de la camara
    Save(0xB76848,4); Save(0x8CD804,4);  // clip lejano y escala de LOD
    Save(0xB72914,4);                    // CGame::currArea
    Save(0x8CD6F4,4);                    // MI_GRASSHOUSE
    Save(0xB7CF1C,4);                    // textura de piel del jugador 0
    Save(MODELPTRS+4u*unsigned(MI_BASE),4u*16u);   // model infos 450..465
    Save(rsSlot,4);                      // puntero de estados de render
    std::array<bool,84> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    std::memset(arena.data(),0xCD,arena.size());
    const unsigned savedFP=_controlfp(0,0);
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                Ctx c; c.fn=&fn; c.nm=fn.name; c.i=i; c.f=uint32(f); c.candidate=candidate;
                c.st=uint32(i+1)*2654435761u^uint32(f+1)*0x9E3779B9u^0x56E1u; if(!c.st)c.st=1;
                std::memset(arena.data(),0xCD,std::min<size_t>(arena.size(),arenaHi+64)); nrec=0; recOverflow=false; arenaTop=0;
                for(auto* h:hooks)h->State(candidate);
                FinishCbs();
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                c.a=fn.address; c.out=json::object(); c.ret=nullptr;
                _controlfp(_PC_24,_MCW_PC);
                const bool handled=RunA(c)||RunB(c)||RunC(c);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                c.Scrub(true);
                json out=c.out; out["switches"]=switches; out["handled"]=handled; out["ret"]=c.ret; out["trace"]=Recs();
                out["world"]=Fnv64(&w,sizeof(World)); arenaHi=std::max(arenaHi,arenaTop); out["arena"]=Fnv64(arena.data(),std::min<size_t>(arena.size(),arenaTop+64)); out["arena_top"]=arenaTop;
                uint64 g=1469598103934665603ull; for(auto& s:saved){ g^=Fnv64(reinterpret_cast<void*>(s.addr),s.bytes.size()); g*=1099511628211ull; } out["globals"]=g;
                dumpLen[candidate?1:0]=std::min<size_t>(arenaTop+64,arena.size()); std::memcpy(arenaDump[candidate?1:0].data(),arena.data(),dumpLen[candidate?1:0]);
                wdump[candidate?1:0]=w;
                RestoreAll();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["handled"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(), vals=json::object(); for(auto& [k,v]:original.items())if(candidate[k]!=v){ diff.push_back(k);
                       const auto so=v.dump(), sc=candidate[k].dump(); vals[k]={{"o",so.substr(0,300)},{"c",sc.substr(0,300)}}; }
                // bloques de 64 bytes del mundo falso que difieren entre lados (maximo 8, con los 32 primeros bytes de cada uno)
                json blocks=json::array(); int shown=0; const size_t n=sizeof(World)/64;
                for(size_t b=0;b<n&&shown<8;++b){ const auto* x=reinterpret_cast<const uint8*>(&wdump[0])+64*b; const auto* y=reinterpret_cast<const uint8*>(&wdump[1])+64*b;
                    if(std::memcmp(x,y,64)!=0){ blocks.push_back({{"block",b},{"offset",64*b},{"original",ReversingProbe::Bytes(x,32)},{"candidate",ReversingProbe::Bytes(y,32)}}); ++shown; } }
                json ablocks=json::array(); const size_t na=std::min(dumpLen[0],dumpLen[1])/64; shown=0;
                for(size_t b=0;b<na&&shown<8;++b)if(std::memcmp(arenaDump[0].data()+64*b,arenaDump[1].data()+64*b,64)!=0){
                    ablocks.push_back({{"block",b},{"offset",64*b},{"original",ReversingProbe::Bytes(arenaDump[0].data()+64*b,32)},{"candidate",ReversingProbe::Bytes(arenaDump[1].data()+64*b,32)}}); ++shown; }
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"diff_values",vals},{"diff_blocks",blocks},{"diff_arena_blocks",ablocks},
                       {"arena_len",json::array({dumpLen[0],dumpLen[1]})},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    const bool restored=AllRestored()&&_controlfp(0,0)==savedFP;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
