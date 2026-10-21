#pragma once
// Casos del banco Helpers66C (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y una funcion de caso por funcion.
#include "Core.hpp"
#include <utility>

namespace Hp66Live {
// ================================================================================================ estado de los falsos y arenas
struct FakeState {
    const C0* c0{}; const C1* c1{}; const C2* c2{}; const C3* c3{}; const C4* c4{};
    uintptr_t texObj{}, tableObj{}, matObj{}, hierObj{}, atomObj{}, handleBase{}, copyObj{};
    u32 newHandleIdx{}, timeIdx{}, divIdx{}, freeIdx{}, loadIdx{}, tid{};
    uintptr_t realLoad{}, realProc{}, realFree{}, realDebug{};
};
inline FakeState g;
alignas(16) inline std::array<uint8,kOutArena> outArena{},outRef{};
alignas(16) inline std::array<uint8,kInfoArena> infoArena{},infoRef{};
alignas(16) inline std::array<uint8,kHandleArena> handleArena{},handleRef{};
alignas(16) inline std::array<uint8,kTexArena> texArena{},texRef{};
alignas(16) inline std::array<uint8,kTableArena> tableArena{},tableRef{};
alignas(16) inline std::array<uint8,kMatArena> matArena{},matRef{};
alignas(16) inline std::array<uint8,kNodeArena> nodeArena{},nodeRef{};
alignas(16) inline std::array<uint8,kSmallArena> hierArena{},hierRef{},atomArena{},atomRef{};

template<size_t N> inline json Changes(const std::array<uint8,N>& mem,const std::array<uint8,N>& ref,size_t objOff) {return ArenaChanges(mem.data(),ref.data(),N,objOff);}
template<size_t N> inline void Put32At(std::array<uint8,N>& a,size_t objOff,size_t off,u32 v) {memcpy(a.data()+objOff+off,&v,4);}
template<size_t N> inline uintptr_t ObjAt(const std::array<uint8,N>& a,size_t objOff) {return uintptr_t(a.data())+objOff;}
inline u32 H(const void* p,size_t n) {u32 h=0x811C9DC5u;const auto* b=static_cast<const u8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=0x01000193u;}return h;}
inline u32 HStr(const char* s) {return H(s,strlen(s)+1);}

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}
// Valor de un puntero que devuelve la funcion, expresado respecto a un objeto de la arena (si cae dentro de [obj - 0x40, obj + 0x80)) o en crudo
inline u32 RelOrRaw(u32 value,uintptr_t obj) {return value-u32(obj)<0x80u||u32(obj)-value<=0x40u?0x10000000u+(value-u32(obj)):value;}

// ================================================================================================ 0: AEMFOpenSoundMedia (0x4E7990)
// Colaboradores de QuickTime (cdecl; el EXE los llama por sus direcciones 0x818970, 0x81DAA0, 0x81B680, 0x81AB80, 0x81A9E0, 0x81BC60, 0x81A0E0). Los argumentos que son punteros a la pila
// del llamado no se registran (cambian entre el EXE y la DLL): se registra el CONTENIDO que el llamado ve. Los OSErr de 16 bits que el EXE pasa como dword (refNum) se enmascaran.
inline u32 __cdecl F0NativePath(u32 name,void* fsSpec,u32 flags) {
    Ev& e=PushEv(1,2);e.v[0]=name;e.v[1]=flags;
    u8* p=static_cast<u8*>(fsSpec);for(u32 i=0;i<0x46;++i)p[i]=u8(Rand(g.c0->specSeed,i));
    return Rand(g.c0->seed,1);
}
inline u32 __cdecl F0FInfo(void* spec,void* info) {
    Ev& e=PushEv(2,1);e.v[0]=H(spec,0x46);
    u8* p=static_cast<u8*>(info);for(u32 i=0;i<0x10;++i)p[i]=u8(Rand(g.c0->seed,10+i));
    return g.c0->errInfo;
}
inline u32 __cdecl F0Open(void* spec,void* refNum,u32 perm) {
    Ev& e=PushEv(3,2);e.v[0]=H(spec,0x46);e.v[1]=perm;
    const u16 r=u16(g.c0->refNum);memcpy(refNum,&r,2);   // OpenMovieFile escribe un short
    return g.c0->errOpen;
}
inline u32 __cdecl F0NewMovie(u32* movie,u32 refNum,u32* resId,u32 name,u32 flags,u8* changed) {
    Ev& e=PushEv(4,5);e.v[0]=*movie;e.v[1]=refNum&0xFFFFu;e.v[2]=*resId;e.v[3]=name;e.v[4]=flags;
    *movie=g.c0->movieTok;*changed=1;
    return g.c0->errNew;
}
inline u32 __cdecl F0Close(u32 refNum) {Ev& e=PushEv(5,1);e.v[0]=refNum&0xFFFFu;return g.c0->errClose;}
inline u32 __cdecl F0TrackType(u32 movie,u32 index,u32 type,u32 flags) {Ev& e=PushEv(6,4);e.v[0]=movie;e.v[1]=index;e.v[2]=type;e.v[3]=flags;return g.c0->trackTok;}
inline u32 __cdecl F0TrackMedia(u32 track) {Ev& e=PushEv(7,1);e.v[0]=track;return g.c0->mediaTok;}
inline void CaseOpenMedia(Env& env,Totals& t,unsigned n) {
    const C0 c=MakeC0(n);
    const json input=json::parse(JsonC0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(outArena.data(),kOutArena,c.seed*3u+1u);
        outRef=outArena;
        g.c0=&c;nev=0;
        const uintptr_t outMovie=ObjAt(outArena,kGuard),outMedia=outMovie+4;
        const u32 args[3]{c.filename,u32(outMovie),u32(outMedia)};
        sides[s]=Execute(env,0,0x4E7990,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,c.ecx,c.edx,c.stack,12);});
        json obs;
        obs["ax"]=sides[s].reg.eax&0xFFFFu;   // el unico llamador (0x4E7CB0) solo mira AX (test ax,ax); EAX completo se registra
        obs["changes"]=json::array({Changes(outArena,outRef,kGuard)});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: AEMFGetMediaSoundFormat (0x4E7A80)
// Handles: celdas maestras en la arena de handles (0x00 = descripcion, 0x04 = extension, 0x08 = extension sustituta) que apuntan a bloques de la misma arena.
inline u32 HCell(unsigned i) {return u32(g.handleBase+i*4);}
inline u32 __cdecl F1NewHandle(u32 size) {
    const u32 idx=g.newHandleIdx++;Ev& e=PushEv(11,2);e.v[0]=size;e.v[1]=idx;
    return idx==0?(g.c1->descNull?0u:HCell(0)):(g.c1->ext2Null?0u:HCell(1));
}
inline u32 __cdecl F1SampleDesc(u32 media,u32 index,u32 handle) {Ev& e=PushEv(12,3);e.v[0]=media;e.v[1]=index;e.v[2]=handle;return Rand(g.c1->seed,3);}
inline u32 __cdecl F1MoviesError() {PushEv(13,0);return g.c1->moviesErr;}
inline u32 __cdecl F1GetExtension(u32 handle,u32* ext,u32 type) {
    Ev& e=PushEv(14,3);e.v[0]=handle;e.v[1]=*ext;e.v[2]=type;
    if(g.c1->extMode==1)*ext=HCell(2);else if(g.c1->extMode==2)*ext=0;
    return g.c1->waveErr;
}
inline u32 __cdecl F1HandleSize(u32 handle) {Ev& e=PushEv(15,1);e.v[0]=handle;return g.c1->size;}
inline u32 __cdecl F1HLock(u32 handle) {Ev& e=PushEv(16,1);e.v[0]=handle;return Rand(g.c1->seed,4);}
inline u32 __cdecl F1NewPtr(u32 size) {Ev& e=PushEv(17,1);e.v[0]=size;return g.c1->newPtrNull?0u:u32(g.copyObj);}
inline u32 __cdecl F1MemError() {PushEv(18,0);return g.c1->memErr;}
inline u32 __cdecl F1BlockMove(u32 src,u32 dst,u32 size) {
    Ev& e=PushEv(19,4);e.v[0]=src;e.v[1]=dst;e.v[2]=size;e.v[3]=src?H(reinterpret_cast<const void*>(uintptr_t(src)),8):0u;
    return Rand(g.c1->seed,5);
}
inline u32 __cdecl F1HUnlock(u32 handle) {Ev& e=PushEv(20,1);e.v[0]=handle;return Rand(g.c1->seed,6);}
inline u32 __cdecl F1Dispose(u32 handle) {Ev& e=PushEv(21,1);e.v[0]=handle;return g.c1->dispRet;}
inline void CaseSoundFormat(Env& env,Totals& t,unsigned n) {
    const C1 c=MakeC1(n);
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(infoArena.data(),kInfoArena,c.seed*5u+1u);FillWords(outArena.data(),kOutArena,c.seed*5u+2u);FillWords(handleArena.data(),kHandleArena,c.seed*5u+3u);
        g.handleBase=uintptr_t(handleArena.data());g.copyObj=g.handleBase+0x80;
        const u32 desc=u32(g.handleBase+0x10),extA=u32(g.handleBase+0x40),extB=u32(g.handleBase+0x60);
        memcpy(handleArena.data()+0,&desc,4);memcpy(handleArena.data()+4,&extA,4);memcpy(handleArena.data()+8,&extB,4);
        infoRef=infoArena;outRef=outArena;handleRef=handleArena;
        g.c1=&c;g.newHandleIdx=0;nev=0;
        const uintptr_t info=ObjAt(infoArena,kGuard),outAtom=ObjAt(outArena,kGuard);
        const u32 args[3]{c.media,u32(outAtom),u32(info)};
        sides[s]=Execute(env,1,0x4E7A80,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,c.media,c.edx,c.stack,12);});   // ECX de entrada = la media (el hueco `push ecx` del EXE)
        json obs;
        obs["ax"]=sides[s].reg.eax&0xFFFFu;
        obs["changes"]=json::array({Changes(infoArena,infoRef,kGuard),Changes(outArena,outRef,kGuard),Changes(handleArena,handleRef,0)});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2: GetDXVersion (0x745650)
// Las cuatro APIs (stdcall) se sustituyen en las ranuras de importacion; otro hilo que las llamara en ese instante pasa al original.
struct FakeDD {uintptr_t* vtbl;};
inline u32 __stdcall F2QueryInterface(void*,const void*,void**) {return 0x80004002u;}
inline u32 __stdcall F2AddRef(void*) {return 2;}
inline FakeDD fakeDD{};
inline u32 __stdcall F2Release(void* self) {Ev& e=PushEv(36,1);e.v[0]=self==&fakeDD?1u:0u;return g.c2->release;}
inline uintptr_t fakeDDVtbl[3]{};
inline u32 __stdcall F2CreateEx(void* guid,void** out,const void* iid,void* unk) {
    Ev& e=PushEv(35,4);e.v[0]=u32(uintptr_t(guid));e.v[1]=H(iid,16);e.v[2]=u32(uintptr_t(unk));e.v[3]=0;
    if(i32(g.c2->hr)>=0)*out=&fakeDD;
    return g.c2->hr;
}
inline HMODULE __stdcall F2LoadLibrary(const char* name) {
    if(GetCurrentThreadId()!=g.tid)return reinterpret_cast<HMODULE(__stdcall*)(const char*)>(g.realLoad)(name);
    Ev& e=PushEv(31,2);e.v[0]=HStr(name);e.v[1]=u32(strlen(name));
    u32 r=0;
    if(!strcmp(name,"DDRAW.DLL"))r=g.c2->ddraw;else if(!strcmp(name,"D3D8.DLL"))r=g.c2->d3d8;else if(!strcmp(name,"dpnhpast.dll"))r=g.c2->dpn;else if(!strcmp(name,"D3D9.DLL"))r=g.c2->d3d9;
    return reinterpret_cast<HMODULE>(uintptr_t(r));
}
inline FARPROC __stdcall F2GetProcAddress(HMODULE h,const char* name) {
    if(GetCurrentThreadId()!=g.tid)return reinterpret_cast<FARPROC(__stdcall*)(HMODULE,const char*)>(g.realProc)(h,name);
    Ev& e=PushEv(32,2);e.v[0]=u32(uintptr_t(h));e.v[1]=HStr(name);
    if(!strcmp(name,"DirectDrawCreateEx")&&g.c2->createExPtr)return reinterpret_cast<FARPROC>(&F2CreateEx);
    return nullptr;
}
inline BOOL __stdcall F2FreeLibrary(HMODULE h) {
    if(GetCurrentThreadId()!=g.tid)return reinterpret_cast<BOOL(__stdcall*)(HMODULE)>(g.realFree)(h);
    Ev& e=PushEv(33,1);e.v[0]=u32(uintptr_t(h));
    return g.c2->freeRet[g.freeIdx++%6];
}
inline void __stdcall F2Debug(const char* text) {
    if(GetCurrentThreadId()!=g.tid){reinterpret_cast<void(__stdcall*)(const char*)>(g.realDebug)(text);return;}
    Ev& e=PushEv(34,2);e.v[0]=HStr(text);e.v[1]=u32(strlen(text));
}
inline void CaseDXVersion(Env& env,Totals& t,unsigned n) {
    const C2 c=MakeC2(n);
    const json input=json::parse(JsonC2(c));
    Side sides[2];json rows[2];
    fakeDDVtbl[0]=uintptr_t(&F2QueryInterface);fakeDDVtbl[1]=uintptr_t(&F2AddRef);fakeDDVtbl[2]=uintptr_t(&F2Release);fakeDD.vtbl=fakeDDVtbl;
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        g.c2=&c;g.freeIdx=0;g.tid=GetCurrentThreadId();nev=0;
        const u32 args[1]{0};
        sides[s]=Execute(env,2,0x745650,candidate,[&](uintptr_t entry){return Gate(entry,args,0,c.eax,c.ecx,c.edx,c.stack,0);});
        json obs;
        obs["eax"]=sides[s].reg.eax;   // el llamador (0x74754A) lee EAX completo (cmp eax,900h)
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[2],n,input,rows,sides);
}

// ================================================================================================ 3: RwTextureGtaStreamRead (0x730E60)
inline u32 __cdecl F3FindChunk(u32 stream,u32 type,u32* length,u32* version) {
    Ev& e=PushEv(41,2);e.v[0]=stream;e.v[1]=type;
    if(g.c3->findRet){*length=g.c3->chunkLen;*version=g.c3->version;}
    return g.c3->findRet;
}
inline u32 __cdecl F3Time() {const u32 idx=g.timeIdx++;Ev& e=PushEv(42,1);e.v[0]=idx;return idx==0?g.c3->t0:g.c3->t1;}
inline u32 __cdecl F3Divider() {const u32 idx=g.divIdx++;Ev& e=PushEv(43,1);e.v[0]=idx;return idx==0?g.c3->d0:g.c3->d1;}
inline u32 __cdecl F3Native(u32 stream,u32* out,u32 length) {
    Ev& e=PushEv(44,2);e.v[0]=stream;e.v[1]=length;
    *out=g.c3->texPresent?u32(g.texObj):0u;
    return g.c3->nativeRet;
}
inline u32 __cdecl F3AnisoGet(u32 tex) {Ev& e=PushEv(45,1);e.v[0]=RelOrRaw(tex,g.texObj);return g.c3->anisoGet;}
inline u32 __cdecl F3AnisoMax() {PushEv(46,0);return g.c3->anisoMax;}
inline u32 __cdecl F3AnisoSet(u32 tex,u32 value) {Ev& e=PushEv(47,2);e.v[0]=RelOrRaw(tex,g.texObj);e.v[1]=value&0xFFu;return g.c3->setRet;}
inline void CaseTextureRead(Env& env,Totals& t,unsigned n) {
    const C3 c=MakeC3(n);
    const json input=json::parse(JsonC3(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(texArena.data(),kTexArena,c.seed*3u+1u);
        g.texObj=ObjAt(texArena,kGuard);
        Put32At(texArena,kGuard,0x50,c.filter);Put32At(texArena,kGuard,0x54,c.refCount);
        texRef=texArena;
        *reinterpret_cast<u32*>(0xC8D4C0)=c.gameState;
        *reinterpret_cast<u32*>(0xC87FF0)=c.count;*reinterpret_cast<u32*>(0xC87FEC)=c.avg;
        *reinterpret_cast<u8*>(0xC87FFC)=u8(c.aniso);
        *reinterpret_cast<u32*>(0xA9AE54)=c.quality;
        *reinterpret_cast<u32*>(Mem(kRNativeSlot))=u32(uintptr_t(&F3Native));
        g.c3=&c;g.timeIdx=0;g.divIdx=0;nev=0;
        const u32 args[1]{c.stream};
        sides[s]=Execute(env,3,0x730E60,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["eax"]=RelOrRaw(sides[s].reg.eax,g.texObj);   // el llamador (RwTexDictionaryGtaStreamRead y los otros dos del EXE) lee EAX; ECX/EDX solo se registran
        obs["avg"]=*reinterpret_cast<u32*>(0xC87FEC);obs["count"]=*reinterpret_cast<u32*>(0xC87FF0);
        obs["changes"]=json::array({Changes(texArena,texRef,kGuard)});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[3],n,input,rows,sides);
}

// ================================================================================================ 4: SkinGetBonePositionsToTable (0x735360)
inline u32 __cdecl F4ForAll(u32 clump,u32 callback,u32* data) {
    Ev& e=PushEv(51,2);e.v[0]=clump;e.v[1]=callback;
    if(callback==0x734810u)*data=u32(g.atomObj);
    else if(callback==0x734A20u)*data=g.c4->hierNull?0u:u32(g.hierObj);
    return clump;
}
inline u32 __cdecl F4GetSkin(u32 geometry) {Ev& e=PushEv(52,1);e.v[0]=geometry;return g.c4->skin;}
inline u32 __cdecl F4NumBones(u32 skin) {Ev& e=PushEv(53,1);e.v[0]=skin;return g.c4->numBones;}
inline u32 __cdecl F4Matrices(u32 skin) {Ev& e=PushEv(54,1);e.v[0]=skin;return u32(g.matObj);}
inline u32 __cdecl F4Invert(u32* out,const u32* in) {
    Ev& e=PushEv(55,1);e.v[0]=H(in,0x40);
    for(u32 i=0;i<16;++i)out[i]=Mix(in[i]+i*0x9E3779B1u+1u);
    return u32(uintptr_t(out));
}
inline u32 __cdecl F4Transform(u32* out,const u32* in,u32 count,const u32* matrix) {
    Ev& e=PushEv(56,5);e.v[0]=u32(uintptr_t(out))-u32(g.tableObj);e.v[1]=H(in,12);e.v[2]=count;e.v[3]=H(matrix,0x40);
    const u32 base=Mix(e.v[1]^(e.v[3]*0x85EBCA6Bu));
    out[0]=Mix(base+1u);out[1]=Mix(base+2u);out[2]=Mix(base+3u);
    return u32(uintptr_t(out));
}
inline void CaseSkinTable(Env& env,Totals& t,unsigned n) {
    const C4 c=MakeC4(n);
    const json input=json::parse(JsonC4(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(tableArena.data(),kTableArena,c.seed*7u+1u);FillWords(matArena.data(),kMatArena,c.seed*7u+2u);FillWords(nodeArena.data(),kNodeArena,c.seed*7u+3u);
        FillWords(hierArena.data(),kSmallArena,c.seed*7u+4u);FillWords(atomArena.data(),kSmallArena,c.seed*7u+5u);
        g.tableObj=ObjAt(tableArena,kGuard);g.matObj=ObjAt(matArena,kGuard);g.hierObj=ObjAt(hierArena,kGuard);g.atomObj=ObjAt(atomArena,kGuard);
        const uintptr_t nodes=ObjAt(nodeArena,kGuard);
        for(u32 i=0;i<kMaxBones;++i)Put32At(nodeArena,kGuard,i*16+8,c.flags[i]);   // RpHAnimNodeInfo { nodeID, nodeIndex, flags, pFrame }
        Put32At(hierArena,kGuard,0x10,u32(nodes));                                    // RpHAnimHierarchy::pNodeInfo
        Put32At(atomArena,kGuard,0x18,c.geometry);                                    // RpAtomic::geometry
        tableRef=tableArena;matRef=matArena;nodeRef=nodeArena;hierRef=hierArena;atomRef=atomArena;
        g.c4=&c;nev=0;
        const u32 args[2]{c.clump,c.tableNull?0u:u32(g.tableObj)};
        sides[s]=Execute(env,4,0x735360,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,c.ecx,c.edx,c.stack,8);});
        json obs;
        obs["changes"]=json::array({Changes(tableArena,tableRef,kGuard),Changes(matArena,matRef,kGuard),Changes(nodeArena,nodeRef,kGuard),Changes(hierArena,hierRef,kGuard),Changes(atomArena,atomRef,kGuard)});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[4],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators,const json& slotInfo) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"import_slots",slotInfo},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c) {
                switch(f) {
                case 0: CaseOpenMedia(env,totals,c); break;
                case 1: CaseSoundFormat(env,totals,c); break;
                case 2: CaseDXVersion(env,totals,c); break;
                case 3: CaseTextureRead(env,totals,c); break;
                default: CaseSkinTable(env,totals,c); break;
                }
            }
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
