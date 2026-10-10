#pragma once
// Casos del banco PathFinal3: cada funcion construye su fixture a partir de la semilla (mundos falsos en el arena del banco, estado
// global de CPathFind poblado de forma determinista) y llama a la direccion del EXE con la convencion del EXE.
#include "PathWorld.hpp"

namespace PathFinal3Live {
using ThisU1 = uint32(__thiscall*)(CPathFind*,uint32);
using ThisU2 = uint32(__thiscall*)(CPathFind*,uint32,uint32);

inline std::vector<int> Ids(Rng& r,int n,int lo=0,int hi=63){
    std::vector<int> v; while(int(v.size())<n){ const int c=r.range(lo,hi); bool dup=false; for(int x:v) if(x==c) dup=true; if(!dup) v.push_back(c); } return v; }
inline uint32 Pack(int area,int node){ return uint32(uint16(area))|(uint32(uint16(node))<<16); }
// direccion de un nodo existente (area cargada con nodos); -1 si no hay
inline bool PickNode(Rng& r,int& area,int& node){
    std::vector<Area*> c; for(auto& a:world) if(a.nNodes>0) c.push_back(&a);
    if(c.empty()) return false; Area* a=c[r.below(uint32(c.size()))]; area=a->id; node=int(r.below(uint32(a->nNodes))); return true; }
inline uint32 UnloadedPacked(Rng& r){ for(int t=0;t<200;++t){ const int a=int(r.below(72)); if(!P32(oPathNodes+4*size_t(a))) return Pack(a,int(r.below(60))); } return Pack(63,0); }
inline json Jaddr(uint32 v){ return json::array({uint32(v&0xFFFF),uint32(v>>16)}); }
inline json Jvec(const CVector& v){ return json::array({std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)}); }
inline float CoordGen(Rng& r){
    const uint32 k=r.below(100);
    if(k<18){ const int m=r.range(-1,9); float v=-3000.f+750.f*float(m); const uint32 d=r.below(4); if(d==1) v=std::nextafter(v,1e9f); else if(d==2) v=std::nextafter(v,-1e9f); return v; }
    if(k<30) return r.uni(-100000.f,100000.f);
    return r.uni(-4000.f,4000.f); }

// ---------------- funciones sin colaboradores: estado de CPathFind y globales ----------------
inline json C_IsAreaNodesAvailable(Rng& r){
    for(int i=0;i<72;++i) if(r.pct(40)) P32(oPathNodes+4*size_t(i))=uint32(uintptr_t(arena.data())+0x10u*uint32(i));
    const uint32 packed=Pack(int(r.below(72)),int(r.below(0x10000)));
    const uint32 al=reinterpret_cast<ThisU1>(0x420AA0)(Paths(),packed)&0xFF; return {{"al",al},{"packed",Jaddr(packed)}}; }
inline json C_GetPathNode(Rng& r){
    for(int i=0;i<72;++i) if(r.pct(40)) P32(oPathNodes+4*size_t(i))=uint32(uintptr_t(arena.data())+0x1C0u*uint32(i));
    int area=0; for(int t=0;t<500;++t){ area=int(r.below(72)); if(P32(oPathNodes+4*size_t(area))) break; }
    if(!P32(oPathNodes+4*size_t(area))) P32(oPathNodes+4*size_t(area))=uint32(uintptr_t(arena.data()));
    const int node=int(r.below(0xFFFF));
    const uint32 ret=reinterpret_cast<ThisU1>(0x420AC0)(Paths(),Pack(area,node)); return {{"ret",ret},{"packed",Jaddr(Pack(area,node))}}; }
inline json C_Init(Rng& r){ (void)r; reinterpret_cast<void(__thiscall*)(CPathFind*)>(0x44D080)(Paths()); return json::object(); }
inline json C_ReInit(Rng& r){ (void)r; reinterpret_cast<void(__thiscall*)(CPathFind*)>(0x44E4E0)(Paths()); return json::object(); }
inline json C_ReleaseRequested(Rng& r){ P32(oPending)=r.below(2); reinterpret_cast<void(__thiscall*)(CPathFind*)>(0x44DD00)(Paths()); return json::object(); }
inline json C_Tidy(Rng& r){
    const uint32 v=r.pickI({0,1,10,53,54,55,56,64,100,0x7FFFFFFF,int32(0xFFFFFFF0u),int32(0x80000000u),r.range(0,70)});
    P32(oNumSwitches)=v; reinterpret_cast<void(__thiscall*)(CPathFind*)>(0x44D3B0)(Paths()); return {{"in",v}}; }
inline json C_SetPathsNeeded(Rng& r){
    alignas(16) CVector v{RandF(r,3000.f),RandF(r,3000.f),RandF(r,3000.f)};
    if(r.pct(8)) v.x=std::bit_cast<float>(0x7FC00000u);
    if(r.pct(5)) v.y=std::bit_cast<float>(0x7F800000u);
    reinterpret_cast<void(__thiscall*)(CPathFind*,const CVector*)>(0x44DCD0)(Paths(),&v); return json::object(); }
inline json C_StartNewInterior(Rng& r){
    const int used=r.range(0,7); std::vector<int> slots=Ids(r,used,0,7); for(int s:slots) PI32(oInteriorIds+4*size_t(s))=int32(r.range(0,100000));
    const int num=r.range(-5,100000); reinterpret_cast<void(__thiscall*)(CPathFind*,int)>(0x44DE80)(Paths(),num); return {{"num",num}}; }
inline json C_AddInteriorLink(Rng& r){
    const int hi=r.pct(10)?62:56;
    auto row=[&](int i){ for(int k=0;k<6;++k) AtA<int8>(gConnects+size_t(i)*6+size_t(k))= r.pct(45)? int8(-1) : (r.pct(10)? int8(-1-int(r.below(100))) : int8(r.below(100))); };
    int a=r.range(0,hi), b=r.pct(15)?a:r.range(0,hi);
    row(a); row(b); if(r.pct(10)) for(int k=0;k<6;++k) AtA<int8>(gConnects+size_t(a)*6+size_t(k))=int8(r.below(60));
    if(hi>=56) { /* la fila siguiente a b o a puede recibir el desbordamiento */ row(std::min(a+1,63)); row(std::min(b+1,63)); }
    reinterpret_cast<void(__thiscall*)(CPathFind*,int,int)>(0x44DED0)(Paths(),a,b); return {{"a",a},{"b",b}}; }
inline json C_AddInteriorLinkExt(Rng& r){
    AtA<int32>(gNumLinksExt)=r.range(0,7); const int idx=r.range(-3,200); const uint32 addr=r.next();
    reinterpret_cast<void(__thiscall*)(CPathFind*,int,uint32)>(0x44DF30)(Paths(),idx,addr); return {{"idx",idx},{"addr",addr}}; }
inline json C_AddNodeToNewInterior(Rng& r){
    AtA<uint32>(gNumNodesGiven)=r.below(64); AtA<uint32>(gNewIntSlot)=r.below(8);
    CNodeAddress out; std::memset(&out,0xCD,sizeof(out));
    const float x=RandF(r,3000.f),y=RandF(r,3000.f),z=RandF(r,300.f); const uint32 dw=r.below(2);
    uint32 c[6]; for(auto& v:c) v=uint32(int32(r.pct(30)?-1:r.range(-100,100)));
    reinterpret_cast<CNodeAddress*(__thiscall*)(CPathFind*,CNodeAddress*,float,float,float,uint32,uint32,uint32,uint32,uint32,uint32,uint32)>(0x450E90)(Paths(),&out,x,y,z,dw,c[0],c[1],c[2],c[3],c[4],c[5]);
    return {{"out",Jaddr(std::bit_cast<uint32>(out))}}; }
inline json C_ReturnInterior(Rng& r){
    for(int i=0;i<8;++i) PI32(oInteriorIds+4*size_t(i))= r.pct(30) ? -1 : int32(r.next());
    const uint32 id = r.pct(60) ? P32(oInteriorIds+4*size_t(r.below(8))) : r.next();
    const int node=r.range(-32768,32767);
    CNodeAddress out; std::memset(&out,0xCD,sizeof(out));
    reinterpret_cast<CNodeAddress*(__thiscall*)(CPathFind*,CNodeAddress*,uint32,int)>(0x451300)(Paths(),&out,id,node);
    return {{"out",Jaddr(std::bit_cast<uint32>(out))},{"id",id}}; }
inline json C_FindXRegion(Rng& r){ const float x=CoordGen(r); const uint32 v=reinterpret_cast<uint32(__thiscall*)(CPathFind*,float)>(0x44D890)(Paths(),x); return {{"x",std::bit_cast<uint32>(x)},{"v",v}}; }
inline json C_FindYRegion(Rng& r){ const float x=CoordGen(r); const uint32 v=reinterpret_cast<uint32(__thiscall*)(CPathFind*,float)>(0x44D8C0)(Paths(),x); return {{"x",std::bit_cast<uint32>(x)},{"v",v}}; }
inline json C_FindRegion(Rng& r){ const float x=CoordGen(r),y=CoordGen(r); const uint32 v=reinterpret_cast<uint32(__thiscall*)(CPathFind*,float,float)>(0x44D830)(Paths(),x,y); return {{"x",std::bit_cast<uint32>(x)},{"y",std::bit_cast<uint32>(y)},{"v",v}}; }
inline uint32 RegionIdx(Rng& r){ return r.pct(70)? r.below(9) : r.below(100000); }
inline json C_FindXCoors(Rng& r){ const uint32 p=RegionIdx(r); const float v=reinterpret_cast<float(__thiscall*)(CPathFind*,uint32)>(0x44D8F0)(Paths(),p); return {{"p",p},{"v",std::bit_cast<uint32>(v)}}; }
inline json C_FindYCoors(Rng& r){ const uint32 p=RegionIdx(r); const float v=reinterpret_cast<float(__thiscall*)(CPathFind*,uint32)>(0x44D910)(Paths(),p); return {{"p",p},{"v",std::bit_cast<uint32>(v)}}; }
inline json C_FindStartPoint(Rng& r){
    const uint32 x=RegionIdx(r),y=RegionIdx(r); float ox=std::bit_cast<float>(0xCDCDCDCDu),oy=ox;
    reinterpret_cast<void(__thiscall*)(CPathFind*,uint32,uint32,float*,float*)>(0x44D930)(Paths(),x,y,&ox,&oy);
    return {{"x",x},{"y",y},{"ox",std::bit_cast<uint32>(ox)},{"oy",std::bit_cast<uint32>(oy)}}; }
inline void RandomLoaded(Rng& r,uint32 pct){ for(int i=0;i<72;++i) P32(oPathNodes+4*size_t(i)) = r.pct(pct) ? uint32(uintptr_t(arena.data())+0x10u*uint32(i)) : 0u; }
inline void RandRect(Rng& r,float& x0,float& x1,float& y0,float& y1){
    const float cx=r.uni(-3300.f,3300.f),cy=r.uni(-3300.f,3300.f),w=r.pct(20)?0.f:r.uni(0.f,r.pct(30)?3000.f:900.f),h=r.pct(20)?0.f:r.uni(0.f,r.pct(30)?3000.f:900.f);
    x0=cx-w; x1=cx+w; y0=cy-h; y1=cy+h; if(r.pct(8)) std::swap(x0,x1); if(r.pct(8)) std::swap(y0,y1); }
inline json C_AreNodesLoaded(Rng& r){
    RandomLoaded(r,r.pct(50)?95:75); float a,b,c,d; RandRect(r,a,b,c,d);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CPathFind*,float,float,float,float)>(0x44DD10)(Paths(),a,b,c,d)&0xFF; return {{"al",al}}; }
inline json C_HaveRequested(Rng& r){
    RandomLoaded(r,r.pct(50)?95:75); float a,b,c,d; RandRect(r,a,b,c,d); PF(oReqMinX)=a; PF(oReqMaxX)=b; PF(oReqMinY)=c; PF(oReqMaxY)=d;
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CPathFind*)>(0x450DB0)(Paths())&0xFF; return {{"al",al}}; }
inline json C_MarkRegions(Rng& r){
    const float x=CoordGen(r),y=CoordGen(r),z=RandF(r,100.f); const float rad=r.pct(20)?350.f:r.pct(20)?300.f:r.pct(10)?-r.uni(0.f,500.f):r.uni(0.f,r.pct(30)?5000.f:900.f);
    reinterpret_cast<void(__thiscall*)(CPathFind*,float,float,float,float)>(0x44DB60)(Paths(),x,y,z,rad); return {{"x",std::bit_cast<uint32>(x)},{"y",std::bit_cast<uint32>(y)},{"rad",std::bit_cast<uint32>(rad)}}; }

// ---------------- listas de busqueda ----------------
inline json C_AddNodeToList(Rng& r){
    uint8* n[4]; for(auto& p:n){ p=ArenaAlloc(0x1C); FillBytes(p,0x1C,r); }
    const int32 dist=r.pickI({0,1,511,512,513,1000,32766,32767,65535,-1,-511,-512,-600,r.range(-700,40000)});
    const int bucket=dist&511;
    if(r.pct(60)){ // el cubo ya tiene una lista de 1-3 nodos
        const int k=r.range(1,3); uint32 prevAddr=uint32(uintptr_t(PathsP()+oHash+4*size_t(bucket)));
        for(int i=0;i<k;++i){ At<uint32>(n[i+1],4)=prevAddr; At<uint32>(n[i+1],0)=(i+1<k)?uint32(uintptr_t(n[i+2])):0u; prevAddr=uint32(uintptr_t(n[i+1])); }
        P32(oHash+4*size_t(bucket))=uint32(uintptr_t(n[1]));
        PI32(oHashCount)=k;
    } else { P32(oHash+4*size_t(bucket))=0; PI32(oHashCount)=r.range(0,50); }
    reinterpret_cast<void(__thiscall*)(CPathFind*,void*,int32)>(0x44D1E0)(Paths(),n[0],dist);
    return {{"dist",dist}}; }
inline json C_RemoveNodeFromList(Rng& r){
    uint8* n[5]; for(auto& p:n){ p=ArenaAlloc(0x1C); FillBytes(p,0x1C,r); }
    const int k=r.range(1,4); const int bucket=int(r.below(512));
    uint32 prevAddr=uint32(uintptr_t(PathsP()+oHash+4*size_t(bucket)));
    for(int i=0;i<k;++i){ At<uint32>(n[i],4)=prevAddr; At<uint32>(n[i],0)=(i+1<k)?uint32(uintptr_t(n[i+1])):0u; prevAddr=uint32(uintptr_t(n[i])); }
    P32(oHash+4*size_t(bucket))=uint32(uintptr_t(n[0])); PI32(oHashCount)=r.range(k,k+30);
    const int j=int(r.below(uint32(k)));
    reinterpret_cast<void(__thiscall*)(CPathFind*,void*)>(0x44D1B0)(Paths(),n[j]); return {{"k",k},{"j",j}}; }
inline json C_ThisNodeHasToBeSwitchedOff(Rng& r){
    uint8* n=ArenaAlloc(0x1C); FillBytes(n,0x1C,r); const uint32 al=reinterpret_cast<uint32(__thiscall*)(CPathFind*,void*)>(0x44D3E0)(Paths(),n)&0xFF; return {{"al",al},{"b",n[0x1A]}}; }

// ---------------- nodos y enlaces ----------------
inline void SimpleWorld(Rng& r,int nAreas,bool scramble,int maxLinks=4){
    Opts o; o.maxLinks=maxLinks; o.scramble=scramble; o.cx=float(r.range(-1500,1500)); o.cy=float(r.range(-1500,1500)); o.pctUnloadedLink=r.pct(30)?35:10;
    BuildWorld(r,Ids(r,nAreas),o); }
// direccion B para una prueba de adyacencia: 60% un enlace de A, si no una direccion cualquiera
inline uint32 PickLinkOrAny(Rng& r,int area,int node){
    Area* a=AreaById(area); if(a&&a->linkCount[size_t(node)]>0&&r.pct(65)){ int base=0; for(int i=0;i<node;++i) base+=a->linkCount[size_t(i)]; return a->links[base+int(r.below(uint32(a->linkCount[size_t(node)])))]; }
    int a2,n2; if(r.pct(50)&&PickNode(r,a2,n2)) return Pack(a2,n2); return Pack(int(r.below(72)),int(r.below(100))); }
inline json C_These2Adjacent(Rng& r){
    SimpleWorld(r,r.range(1,3),r.pct(50)); int a,n; if(!PickNode(r,a,n)) return {{"skipped",true}};
    const uint32 b=PickLinkOrAny(r,a,n); const uint32 al=reinterpret_cast<ThisU2>(0x44D230)(Paths(),Pack(a,n),b)&0xFF; return {{"al",al},{"a",Jaddr(Pack(a,n))},{"b",Jaddr(b)}}; }
inline json C_TestIntersection(Rng& r,uintptr_t fn){
    SimpleWorld(r,r.range(1,3),r.pct(50)); int a,n; uint32 A,B;
    if(r.pct(12)||!PickNode(r,a,n)){ A=UnloadedPacked(r); B=r.pct(50)?UnloadedPacked(r):Pack(0,0); a=-1; n=0; } else { A=Pack(a,n); B=PickLinkOrAny(r,a,n); if(r.pct(10)) B=UnloadedPacked(r); }
    const uint32 al=reinterpret_cast<ThisU2>(fn)(Paths(),A,B)&0xFF; return {{"al",al},{"a",Jaddr(A)},{"b",Jaddr(B)}}; }
inline json C_FindLinkBetween(Rng& r){
    SimpleWorld(r,r.range(1,3),r.pct(50),3); int a,n; std::vector<std::pair<int,int>> cand;
    for(auto& ar:world) { if(ar.id>=64) continue; for(int i=0;i<ar.nNodes;++i) if(ar.linkCount[size_t(i)]>0) cand.push_back({ar.id,i}); }
    uint32 A,B; if(cand.empty()||r.pct(10)){ A=UnloadedPacked(r); B=Pack(0,0); } else { auto c=cand[r.below(uint32(cand.size()))]; a=c.first; n=c.second; A=Pack(a,n); B=PickLinkOrAny(r,a,n);
        // B debe ser un enlace de A o tener el area sin cargar
        Area* ar=AreaById(a); int base=0; for(int i=0;i<n;++i) base+=ar->linkCount[size_t(i)]; bool isLink=false; for(int k=0;k<ar->linkCount[size_t(n)];++k) if(ar->links[base+k]==B) isLink=true;
        if(!isLink&&P32(oPathNodes+4*size_t(B&0xFFFF))) B=ar->links[base]; }
    uint32 B2=B; if((B&0xFFFF)>=72) B2=UnloadedPacked(r);
    uint32 out=0xCDCDCDCDu;
    reinterpret_cast<void*(__thiscall*)(CPathFind*,uint32*,uint32,uint32)>(0x451350)(Paths(),&out,A,B2);
    return {{"out",out},{"a",Jaddr(A)},{"b",Jaddr(B2)}}; }
inline json C_DeadEnd(Rng& r){
    // arbol de n nodos en un area: cada nodo enlaza con su padre y sus hijos (sin ciclos, el recorrido termina)
    world.clear(); arenaTop=0; std::memset(arena.data(),0xCD,arena.size());
    const int id=r.range(0,63); const int n=r.range(2,9); std::vector<std::vector<int>> adj; adj.resize(size_t(n));
    for(int i=1;i<n;++i){ const int p=int(r.below(uint32(i))); adj[size_t(i)].push_back(p); adj[size_t(p)].push_back(i); }
    Area a; a.id=id; a.nNodes=n; a.nVeh=n; a.nPed=0; a.linkCount.assign(size_t(n),0); int sum=0;
    for(int i=0;i<n;++i){ if(r.pct(15)) adj[size_t(i)].push_back(-1); a.linkCount[size_t(i)]=int(adj[size_t(i)].size()); sum+=a.linkCount[size_t(i)]; } a.nAddr=sum;
    a.nodes=ArenaAlloc(size_t(n)*0x1C); a.navi=ArenaAlloc(0xE); a.links=reinterpret_cast<uint32*>(ArenaAlloc(size_t(sum+192)*4)); a.lens=ArenaAlloc(size_t(sum+192)); a.inter=ArenaAlloc(size_t(sum+192)); a.naviLinks=reinterpret_cast<uint16*>(ArenaAlloc(size_t(std::max(sum,1))*2));
    int base=0; const bool scr=false;   // el original usa m_wAreaId del propio nodo: debe ser coherente
    for(int i=0;i<n;++i){ uint8* nd=a.Node(i); FillBytes(nd,0x1C,r); At<uint32>(nd,0)=0; At<uint32>(nd,4)=0; SetNodePos(nd,RandF(r,100.f),RandF(r,100.f),0.f);
        At<int16>(nd,0x10)=int16(base); At<uint16>(nd,0x12)= scr?uint16(r.next()):uint16(id); At<uint16>(nd,0x14)= scr?uint16(r.next()):uint16(i);
        nd[0x18]=uint8((nd[0x18]&0xF0)|a.linkCount[size_t(i)]); nd[0x1A]=uint8((nd[0x1A]&0x0F)|(RandBehaviour(r)<<4));
        for(int k=0;k<a.linkCount[size_t(i)];++k){ const int t=adj[size_t(i)][size_t(k)]; a.links[base+k]= t>=0 ? Pack(id,t) : Pack((id+1)%64,int(r.below(10))); }
        base+=a.linkCount[size_t(i)]; }
    for(int i=sum;i<sum+192;++i) a.links[i]=0xFFFFFFFFu; FillBytes(a.lens,size_t(sum+192),r); FillBytes(a.inter,size_t(sum+192),r);
    world.push_back(a); ClearAreaTables(); RegisterArea(world[0]);
    const int s=int(r.below(uint32(n))); const int e=r.pct(60)&&!adj[size_t(s)].empty()&&adj[size_t(s)][0]>=0 ? adj[size_t(s)][0] : int(r.below(uint32(n)));
    const uint32 al=reinterpret_cast<ThisU2>(0x44D310)(Paths(),uint32(uintptr_t(world[0].Node(s))),uint32(uintptr_t(world[0].Node(e))))&0xFF;
    return {{"al",al},{"s",s},{"e",e},{"n",n}}; }
inline json C_CountNeighbours(Rng& r){
    SimpleWorld(r,r.range(1,3),false,5); int a,n; if(!PickNode(r,a,n)) return {{"skipped",true}};   // m_wAreaId coherente: el original lo usa para indexar
    const uint32 v=reinterpret_cast<uint32(__thiscall*)(CPathFind*,void*)>(0x4504F0)(Paths(),AreaById(a)->Node(n)); return {{"v",v},{"a",a},{"n",n}}; }
inline json C_SwitchOffNeighbours(Rng& r){
    SimpleWorld(r,r.range(1,2),false,3);
    std::vector<std::pair<int,int>> cand; for(auto& ar:world) for(int i=0;i<ar.nNodes;++i) cand.push_back({ar.id,i});
    if(cand.empty()) return {{"skipped",true}};
    auto c=cand[r.below(uint32(cand.size()))]; uint8* node=AreaById(c.first)->Node(c.second); const bool useNext2= r.pct(75) || (node[0x18]&0xF)>1;
    void* next1=reinterpret_cast<void*>(0xDDDDDDDDu); void* next2=reinterpret_cast<void*>(0xEEEEEEEEu);
    const uint32 sw=r.below(2), back=r.below(2);
    reinterpret_cast<void(__thiscall*)(CPathFind*,void*,void**,void**,uint32,uint32)>(0x452160)(Paths(),node,&next1,useNext2?&next2:nullptr,sw,back);
    return {{"next1",Name(uintptr_t(next1))},{"next2",useNext2?Name(uintptr_t(next2)):json(nullptr)},{"sw",sw},{"back",back},{"useNext2",useNext2}}; }

// cajas de interruptores
inline void FillSwitches(Rng& r,const Opts& o,int count){
    P32(oNumSwitches)=uint32(count);
    for(int i=0;i<count;++i){ uint8* e=PathsP()+oSwitches+0x1C*size_t(i);
        const float x0=o.cx+r.uni(-o.half,o.half),x1=x0+r.uni(0.f,o.half*2),y0=o.cy+r.uni(-o.half,o.half),y1=y0+r.uni(0.f,o.half*2),z0=o.cz-r.uni(0.f,40.f),z1=o.cz+r.uni(0.f,40.f);
        At<float>(e,0)=x0; At<float>(e,4)=x1; At<float>(e,8)=y0; At<float>(e,0xC)=y1; At<float>(e,0x10)=z0; At<float>(e,0x14)=z1; At<uint8>(e,0x18)=uint8(r.below(2)); At<uint8>(e,0x19)=uint8(r.below(2)); } }
inline void RandBox(Rng& r,const Opts& o,float& x0,float& x1,float& y0,float& y1,float& z0,float& z1){
    x0=o.cx+r.uni(-o.half,o.half); x1=x0+r.uni(0.f,o.half*2.2f); y0=o.cy+r.uni(-o.half,o.half); y1=y0+r.uni(0.f,o.half*2.2f); z0=o.cz-r.uni(0.f,30.f); z1=o.cz+r.uni(0.f,30.f);
    if(r.pct(10)){ x0=-1e9f; x1=1e9f; y0=-1e9f; y1=1e9f; z0=-1e9f; z1=1e9f; } if(r.pct(5)) std::swap(x0,x1);
    if(r.pct(10)){ // limite exactamente en un nodo
        for(auto& ar:world) if(ar.nNodes>0){ const CVector p=NodePos(ar.Node(int(r.below(uint32(ar.nNodes))))); if(r.pct(50)) x0=p.x; else y1=p.y; break; } } }
inline json C_SwitchOneRegion(Rng& r){
    Opts o; o.maxLinks=3; o.symmetric=true; o.half=60.f; o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); o.scramble=false; o.pctUnloadedLink=20;
    BuildWorld(r,Ids(r,r.range(1,2)),o);
    if(r.pct(50)) FillSwitches(r,o,r.range(0,2));
    float b[6]; RandBox(r,o,b[0],b[1],b[2],b[3],b[4],b[5]);
    const int area= r.pct(85)&&!world.empty()? world[r.below(uint32(world.size()))].id : int(r.below(64)); const uint32 off=r.below(2),cars=r.below(2),back=r.below(2);
    reinterpret_cast<void(__thiscall*)(CPathFind*,float,float,float,float,float,float,uint32,uint32,int32,uint32)>(0x452820)(Paths(),b[0],b[1],b[2],b[3],b[4],b[5],off,cars,area,back);
    return {{"area",area},{"off",off},{"cars",cars},{"back",back}}; }
inline json C_SwitchRoads(Rng& r,bool ped){
    Opts o; o.maxLinks=3; o.symmetric=true; o.half=60.f; o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); o.scramble=false; o.pctUnloadedLink=20;
    BuildWorld(r,Ids(r,r.range(1,2)),o);
    const int cnt= r.pct(15) ? r.range(60,64) : r.range(0,8); FillSwitches(r,o,cnt);
    if(r.pct(50)&&cnt>0){ // alguna caja guardada coincide o queda dentro de la nueva
        uint8* e=PathsP()+oSwitches+0x1C*size_t(r.below(uint32(cnt))); At<float>(e,0)=o.cx-5.f; At<float>(e,4)=o.cx+5.f; At<float>(e,8)=o.cy-5.f; At<float>(e,0xC)=o.cy+5.f; At<float>(e,0x10)=o.cz-1.f; At<float>(e,0x14)=o.cz+1.f; }
    float b[6]; RandBox(r,o,b[0],b[1],b[2],b[3],b[4],b[5]);
    if(r.pct(25)&&cnt>0){ uint8* e=PathsP()+oSwitches+0x1C*size_t(r.below(uint32(cnt))); for(int k=0;k<6;++k) b[k]=At<float>(e,4*size_t(k)); } // misma caja
    const uint32 off=r.below(2),cars=r.below(2),back=r.below(2);
    if(ped) reinterpret_cast<void(__thiscall*)(CPathFind*,float,float,float,float,float,float,uint32,uint32)>(0x452F00)(Paths(),b[0],b[1],b[2],b[3],b[4],b[5],off,back);
    else reinterpret_cast<void(__thiscall*)(CPathFind*,float,float,float,float,float,float,uint32,uint32,uint32)>(0x452C80)(Paths(),b[0],b[1],b[2],b[3],b[4],b[5],off,cars,back);
    return {{"off",off},{"cars",cars},{"back",back},{"cnt",cnt}}; }
inline json C_BridgeLights(Rng& r){
    Opts o; o.naviMax=8; o.half=80.f; o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); BuildWorld(r,Ids(r,r.range(1,3)),o);
    float x0=o.cx+r.uni(-o.half,o.half),x1=x0+r.uni(0.f,o.half*2),y0=o.cy+r.uni(-o.half,o.half),y1=y0+r.uni(0.f,o.half*2); if(r.pct(10)){ std::swap(x0,x1); }
    if(r.pct(20)){ for(auto& ar:world) if(ar.nNavi>0){ x0=float(*reinterpret_cast<int16*>(ar.navi))*0.125f; y1=float(*reinterpret_cast<int16*>(ar.navi+2))*0.125f; break; } }
    const uint32 v=r.below(2); reinterpret_cast<void(__thiscall*)(CPathFind*,float,float,float,float,uint32)>(0x44D960)(Paths(),x0,x1,y0,y1,v); return {{"v",v}}; }
inline json C_UnMarkAll(Rng& r){ SimpleWorld(r,r.range(0,4),false); RandomLoaded(r,0); /* restaura tablas vacias */ ClearAreaTables(); for(auto& a:world) if(r.pct(80)) RegisterArea(a);
    reinterpret_cast<void(__thiscall*)(CPathFind*)>(0x44D400)(Paths()); return json::object(); }
inline json C_IsWaterNearby(Rng& r){
    Opts o; o.pctWater=45; o.half=100.f; o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); BuildWorld(r,Ids(r,r.range(0,3)),o);
    float px=o.cx+r.uni(-120.f,120.f),py=o.cy+r.uni(-120.f,120.f),pz=o.cz+RandF(r,60.f); float rad=r.uni(0.f,200.f);
    if(r.pct(25)){ for(auto& ar:world) if(ar.nVeh>0){ const CVector p=NodePos(ar.Node(int(r.below(uint32(ar.nVeh))))); const float dx=px-p.x,dy=py-p.y; rad=std::sqrt(dy*dy+dx*dx); if(r.pct(40)) rad=std::nextafter(rad,0.f); else if(r.pct(40)) rad=std::nextafter(rad,1e9f); break; } }
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CPathFind*,float,float,float,float)>(0x450DE0)(Paths(),px,py,pz,rad)&0xFF;
    return {{"al",al},{"rad",std::bit_cast<uint32>(rad)}}; }
inline json C_FavourDirection(Rng& r){
    Opts o; o.half=60.f; o.pedMax=8; o.vehMax=8; o.scramble=r.pct(50); o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); BuildWorld(r,Ids(r,r.range(0,3)),o);
    float px=o.cx+r.uni(-80.f,80.f),py=o.cy+r.uni(-80.f,80.f),pz=o.cz+RandF(r,20.f); if(r.pct(8)){ px+=30000.f; }
    float dx=RandF(r,2.f),dy=RandF(r,2.f); if(r.pct(10)){ dx=0.f; dy=0.f; } else if(r.pct(5)){ dx=1e-30f; dy=0.f; } else if(r.pct(5)){ dx=0.f; dy=-1.f; }
    const uint32 type=r.below(2); CNodeAddress out; std::memset(&out,0xCD,sizeof(out));
    reinterpret_cast<CNodeAddress*(__thiscall*)(CPathFind*,CNodeAddress*,float,float,float,uint32,float,float)>(0x44FCE0)(Paths(),&out,px,py,pz,type,dx,dy);
    uint32 raw=std::bit_cast<uint32>(out); if((raw&0xFFFF)==0xFFFF) raw|=0xFFFF0000u;   // node no definido (basura de pila del original) si no se encontro
    return {{"out",Jaddr(raw)},{"type",type}}; }
inline json C_NodeCoorsLinked(Rng& r){
    Opts o; o.maxLinks=3; o.scramble=true; o.half=60.f; o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); o.pctUnloadedLink=r.pct(30)?40:5; BuildWorld(r,Ids(r,r.range(1,3)),o);
    uint32 A; int a,n; if(r.pct(15)||!PickNode(r,a,n)) A=UnloadedPacked(r); else A=Pack(a,n);
    uint8 found=0xCD; CVector out{std::bit_cast<float>(0xCDCDCDCDu),std::bit_cast<float>(0xCDCDCDCDu),std::bit_cast<float>(0xCDCDCDCDu)};
    const bool usePtr=r.pct(80);
    reinterpret_cast<CVector*(__thiscall*)(CPathFind*,CVector*,uint32,uint8*)>(0x4505E0)(Paths(),&out,A,usePtr?&found:nullptr);
    return {{"out",Jvec(out)},{"found",usePtr?json(int(found)):json(nullptr)},{"a",Jaddr(A)}}; }
inline json C_NodeCoorsTwo(Rng& r){
    Opts o; o.maxLinks=2; o.scramble=true; o.half=60.f; o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); o.cz=float(r.range(-30,30)); BuildWorld(r,Ids(r,r.range(1,3)),o);
    int a,n; uint32 A,B; if(r.pct(15)||!PickNode(r,a,n)) A=UnloadedPacked(r); else A=Pack(a,n);
    if(r.pct(15)||!PickNode(r,a,n)) B=UnloadedPacked(r); else B=Pack(a,n);
    if(r.pct(10)) B=A;   // mismo nodo: vector nulo (x = 1.0)
    uint8 found=0xCD; float heading=std::bit_cast<float>(0xCDCDCDCDu); CVector out{std::bit_cast<float>(0xCDCDCDCDu),std::bit_cast<float>(0xCDCDCDCDu),std::bit_cast<float>(0xCDCDCDCDu)};
    const bool usePtr=r.pct(80);
    reinterpret_cast<CVector*(__thiscall*)(CPathFind*,CVector*,uint32,uint32,float*,uint8*)>(0x450780)(Paths(),&out,A,B,&heading,usePtr?&found:nullptr);
    return {{"out",Jvec(out)},{"heading",std::bit_cast<uint32>(heading)},{"found",usePtr?json(int(found)):json(nullptr)}}; }
inline json C_TakeWidth(Rng& r){
    Opts o; o.maxLinks=2; o.scramble=r.pct(50); o.half=60.f; o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); BuildWorld(r,Ids(r,r.range(1,3)),o);
    uint32 A; int a,n; if(r.pct(15)||!PickNode(r,a,n)) A=UnloadedPacked(r); else A=Pack(a,n);
    const int seed= r.pct(30)? r.range(-32768,32767) : r.range(-300,300);
    CVector out{std::bit_cast<float>(0xCDCDCDCDu),std::bit_cast<float>(0xCDCDCDCDu),std::bit_cast<float>(0xCDCDCDCDu)};
    reinterpret_cast<CVector*(__thiscall*)(CPathFind*,CVector*,uint32,int)>(0x4509A0)(Paths(),&out,A,seed);
    return {{"out",Jvec(out)},{"seed",seed},{"a",Jaddr(A)}}; }
inline json C_MarkDontWander(Rng& r){
    Opts o; o.half=60.f; BuildWorld(r,Ids(r,r.range(1,3)),o); int a,n;
    if(r.pct(25)||!PickNode(r,a,n)) findResult=CNodeAddress{}; else findResult=CNodeAddress{uint16(a),uint16(n)};
    const float x=RandF(r,2000.f),y=RandF(r,2000.f),z=RandF(r,100.f);
    reinterpret_cast<void(__thiscall*)(CPathFind*,float,float,float)>(0x450560)(Paths(),x,y,z); return {{"res",Jaddr(std::bit_cast<uint32>(findResult))}}; }
inline json C_NearestExterior(Rng& r){
    int a,n; Opts o; BuildWorld(r,Ids(r,r.range(1,2)),o); if(r.pct(30)||!PickNode(r,a,n)) findResult=CNodeAddress{}; else findResult=CNodeAddress{uint16(a),uint16(n)};
    const int id=r.range(0,63); CNodeAddress out; std::memset(&out,0xCD,sizeof(out));
    reinterpret_cast<CNodeAddress*(__thiscall*)(CPathFind*,CNodeAddress*,int)>(0x450F30)(Paths(),&out,id);
    return {{"out",Jaddr(std::bit_cast<uint32>(out))},{"id",id}}; }

// ---------------- enlaces dinamicos e interiores ----------------
inline json C_AddDynamicLink(Rng& r){
    Opts o; o.maxLinks=4; o.half=60.f; o.scramble=r.pct(50); BuildWorld(r,Ids(r,r.range(1,2),0,63),o);
    // el nodo a ampliar: con enlaces estaticos (base < numAddresses) o ya dinamico
    std::vector<std::pair<int,int>> cand; for(auto& ar:world) for(int i=0;i<ar.nNodes;++i) cand.push_back({ar.id,i});
    if(cand.empty()) return {{"skipped",true}};
    auto c=cand[r.below(uint32(cand.size()))]; Area* ar=AreaById(c.first); uint8* nd=ar->Node(c.second);
    { const size_t ai=size_t(c.first);   // las tablas de enlaces deben existir aunque el area no tenga enlaces estaticos
      P32(oNodeLinks+4*ai)=uint32(uintptr_t(ar->links)); P32(oLinkLengths+4*ai)=uint32(uintptr_t(ar->lens)); P32(oIntersections+4*ai)=uint32(uintptr_t(ar->inter)); }
    // bloques dinamicos: los primeros k ocupados (area valida), el resto libres; al menos uno libre
    const int occupied=r.range(0,10); for(int b=0;b<16;++b){ uint32* blk=ar->links+ar->nAddr+12*b; for(int e=0;e<12;++e) blk[e]= b<occupied ? Pack(r.range(0,70),r.range(0,50)) : 0xFFFFFFFFu; }
    if(r.pct(35)&&ar->nAddr>=0){ // nodo ya dinamico: su base apunta a un bloque ocupado y tiene <= 8 enlaces
        const int b=occupied>0 ? int(r.below(uint32(occupied))) : 0; At<int16>(nd,0x10)=int16(ar->nAddr+12*b); nd[0x18]=uint8((nd[0x18]&0xF0)|r.below(9)); }
    else if(r.pct(10)) At<int16>(nd,0x10)=int16(-1-int(r.below(10))); // base negativa: la comparacion es con signo
    else nd[0x18]=uint8((nd[0x18]&0xF0)|r.below(9));
    // tabla de enlaces dinamicos del area: algunas entradas ocupadas, otras negativas
    if(c.first<64) for(int j=0;j<16;++j){ PI32(oDynBase+4*(size_t(c.first)*16+size_t(j)))= r.pct(55)? int32(r.below(200)) : (r.pct(50)? -1 : -int32(r.below(5))-2); PI32(oDynIds+4*(size_t(c.first)*16+size_t(j)))=int32(r.below(300)); }
    if(r.pct(10)){ // sin hueco en la tabla
        for(int j=0;j<16;++j) PI32(oDynBase+4*(size_t(c.first)*16+size_t(j)))=int32(r.below(200)); }
    const uint32 second=Pack(r.range(0,70),r.range(0,50));
    reinterpret_cast<void(__thiscall*)(CPathFind*,uint32,uint32)>(0x44E000)(Paths(),Pack(c.first,c.second),second);
    return {{"a",Jaddr(Pack(c.first,c.second))},{"second",Jaddr(second)}}; }
inline json C_RemoveInterior(Rng& r){
    Opts o; o.maxLinks=2; o.pedMin=2; o.pedMax=7; o.vehMin=0; o.vehMax=3; o.half=60.f; o.scramble=r.pct(50); o.pctUnloadedLink=10;
    std::vector<int> ids=Ids(r,r.range(1,3),0,3); const int nInt=r.range(1,3); std::vector<int> slots=Ids(r,nInt,0,7); for(int s:slots) ids.push_back(64+s);
    BuildWorld(r,ids,o);
    for(int s:slots) PI32(oInteriorIds+4*size_t(s))=int32(100+s);
    // en las areas del mapa: nodos de peaton con bloques dinamicos de enlaces a los interiores
    for(auto& ar:world){ if(ar.id>=64) continue;
        P32(oNodeLinks+4*size_t(ar.id))=uint32(uintptr_t(ar.links));   // existe aunque el area no tenga enlaces estaticos
        for(int b=0;b<4;++b){ uint32* blk=ar.links+ar.nAddr+12*b; for(int e=0;e<12;++e) blk[e]=0xFFFFFFFFu; }
        for(int i=ar.nVeh;i<ar.nNodes;++i){ uint8* nd=ar.Node(i);
            if(r.pct(70)){ const int blkIdx=r.range(0,3); const int base=ar.nAddr+12*blkIdx; const int nl=r.range(1,6);
                if(base<4||base+nl>=56) continue;   // el desplazamiento erroneo del original indexa desde &m_pNodeLinks[0]: solo punteros sin usar (4..56)
                At<int16>(nd,0x10)=int16(base); nd[0x18]=uint8((nd[0x18]&0xF0)|nl);
                for(int k=0;k<nl;++k){ const int kind=int(r.below(10)); int ta,tn=r.range(0,9);
                    if(kind<5) ta=64+slots[r.below(uint32(slots.size()))]; else if(kind<6) ta=64+int(r.below(8)); else if(kind<7) ta=0xFFFF; else ta=ar.id;
                    ar.links[base+k]=Pack(ta,tn); }
                const int j=r.range(0,15); if(r.pct(70)) { PI32(oDynIds+4*(size_t(ar.id)*16+size_t(j)))=base; PI32(oDynBase+4*(size_t(ar.id)*16+size_t(j)))=int32(r.range(0,ar.nAddr+3)); } } } }
    // punteros no usados de m_pNodeLinks: valores reconocibles (el original desplaza enlaces dentro de este array por un error de R*)
    for(int i=0;i<64;++i){ bool used=false; for(auto& ar:world) if(ar.id==i) used=true; if(!used) P32(oNodeLinks+4*size_t(i))=0xB0000000u+uint32(i)*0x101u; }
    const uint32 id= r.pct(85)? uint32(100+slots[r.below(uint32(slots.size()))]) : r.next();
    reinterpret_cast<void(__thiscall*)(CPathFind*,uint32)>(0x44E1A0)(Paths(),id); return {{"id",id},{"slots",int(slots.size())}}; }
inline json C_UnLoad(Rng& r){
    const int idx=r.range(0,71); static const size_t offs[6]{oPathNodes,oNaviNodes,oNodeLinks,oNaviLinks,oLinkLengths,oIntersections};
    for(int k=0;k<6;++k){ if(idx>=64&&offs[k]==oNaviLinks) continue; P32(offs[k]+4*size_t(idx))= r.pct(70)? uint32(uintptr_t(arena.data())+0x100u*uint32(k+1)) : 0u; }
    reinterpret_cast<void(__thiscall*)(CPathFind*,int)>(0x44D0F0)(Paths(),idx); return {{"idx",idx}}; }

// ---------------- carga de ficheros con buferes en memoria ----------------
inline void BuildStreamFile(Rng& r,int areaId,const Opts& o){
    streamData.clear(); streamPos=0;
    const int nVeh=r.range(0,5),nPed=r.range(0,5),nNodes=nVeh+nPed; const int nNavi=r.range(0,5);
    std::vector<int> lc; lc.resize(size_t(nNodes)); int sum=0; for(auto& c:lc){ c= nNodes>0? r.range(0,1):0; sum+=c; }   // <= 1 enlace: el original revienta con dos vecinos y outNext2 nulo
    auto put32=[&](uint32 v){ for(int i=0;i<4;++i) streamData.push_back(uint8(v>>(8*i))); };
    put32(uint32(nNodes)); put32(uint32(nVeh)); put32(uint32(nPed)); put32(uint32(nNavi)); put32(uint32(sum));
    int base=0; for(int i=0;i<nNodes;++i){ uint8 nd[0x1C]; FillBytes(nd,0x1C,r); At<uint32>(nd,0)=0; At<uint32>(nd,4)=0;
        SetNodePos(nd,o.cx+r.uni(-o.half,o.half),o.cy+r.uni(-o.half,o.half),o.cz); At<int16>(nd,0x10)=int16(base); At<uint16>(nd,0x12)=uint16(areaId); At<uint16>(nd,0x14)=uint16(i);
        nd[0x18]=uint8((nd[0x18]&0xF0)|lc[size_t(i)]); nd[0x1A]=uint8((nd[0x1A]&0x0F)|(RandBehaviour(r)<<4)); base+=lc[size_t(i)];
        streamData.insert(streamData.end(),nd,nd+0x1C); }
    for(int i=0;i<nNavi;++i){ uint8 l[0xE]; FillBytes(l,0xE,r); streamData.insert(streamData.end(),l,l+0xE); }
    if(sum>0){
        for(int i=0;i<nNodes;++i) for(int k=0;k<lc[size_t(i)];++k){ const bool same=r.pct(70); put32(Pack(same?areaId:(areaId+1)%64, same? int(r.below(uint32(nNodes))) : int(r.below(20)))); }
        for(int i=0;i<192;++i) put32(0xFFFFFFFFu);
        for(int i=0;i<sum;++i){ const uint16 v=uint16(r.next()); streamData.push_back(uint8(v)); streamData.push_back(uint8(v>>8)); }
        for(int i=0;i<sum+192;++i) streamData.push_back(uint8(r.next()));
        for(int i=0;i<sum+192;++i) streamData.push_back(uint8(r.next()));
    }
}
inline void PrepareLoad(Rng& r,int areaId){
    Opts o; o.half=70.f; o.cx=float(r.range(-1000,1000)); o.cy=float(r.range(-1000,1000)); BuildStreamFile(r,areaId,o);
    ClearAreaTables();
    const int cnt=r.pct(60)? r.range(0,4):r.range(5,12); P32(oNumSwitches)=uint32(cnt);
    for(int i=0;i<cnt;++i){ uint8* e=PathsP()+oSwitches+0x1C*size_t(i); const float x0=o.cx+r.uni(-70.f,70.f),y0=o.cy+r.uni(-70.f,70.f);
        At<float>(e,0)=x0; At<float>(e,4)=x0+r.uni(0.f,100.f); At<float>(e,8)=y0; At<float>(e,0xC)=y0+r.uni(0.f,100.f); At<float>(e,0x10)=-50.f; At<float>(e,0x14)=50.f; At<uint8>(e,0x18)=uint8(r.below(2)); At<uint8>(e,0x19)=uint8(r.below(2)); }
    // enlaces dinamicos del area previos a la carga
    for(int j=0;j<16;++j){ PI32(oDynBase+4*(size_t(areaId)*16+size_t(j)))=int32(r.below(100)); PI32(oDynIds+4*(size_t(areaId)*16+size_t(j)))=int32(r.below(100)); }
}
inline json C_LoadFromStream(Rng& r){
    const int area=r.range(0,63); PrepareLoad(r,area);
    reinterpret_cast<void(__thiscall*)(CPathFind*,void*,int)>(0x4529F0)(Paths(),fakeStream.data(),area); return {{"area",area},{"consumed",uint32(streamPos)},{"size",uint32(streamData.size())}}; }
inline json C_LoadArea(Rng& r){
    const int area=r.range(0,63); PrepareLoad(r,area); std::memset(reinterpret_cast<void*>(gString),0xA5,352);
    reinterpret_cast<void(__thiscall*)(CPathFind*,int)>(0x452F40)(Paths(),area); return {{"area",area},{"consumed",uint32(streamPos)},{"size",uint32(streamData.size())}}; }
inline json C_Save(Rng& r){
    const int cnt=r.pct(80)? r.range(0,64):0; P32(oNumSwitches)=uint32(cnt);
    for(int i=0;i<cnt;++i) FillSwitchEntry(PathsP()+oSwitches+0x1C*size_t(i),r);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CPathFind*)>(0x5D34C0)(Paths())&0xFF; return {{"al",al},{"cnt",cnt}}; }
inline json C_Load(Rng& r){
    loadCount=uint32(r.pct(80)? r.range(0,64):r.range(0,3)); loadCall=0; loadRng=Rng{r.next()|1u};
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(CPathFind*)>(0x5D3500)(Paths())&0xFF; return {{"al",al},{"cnt",loadCount}}; }

// ---------------- streaming ----------------
inline json C_Shutdown(Rng& r){ RandomLoaded(r,r.pct(20)?5:50); reinterpret_cast<void(__thiscall*)(CPathFind*)>(0x450950)(Paths()); return json::object(); }
inline json C_LoadScene(Rng& r){
    const float x=CoordGen(r),y=CoordGen(r),z=RandF(r,100.f); reinterpret_cast<void(__thiscall*)(CPathFind*,float,float,float)>(0x44DE00)(Paths(),x,y,z);
    return {{"x",std::bit_cast<uint32>(x)},{"y",std::bit_cast<uint32>(y)}}; }

// vehiculos falsos del pool que sustituye al real (0xB74494)
constexpr int kFakeVeh=6; constexpr size_t kVehSize=0xA18;
alignas(16) inline std::array<uint8,0x20> fakePool{};
alignas(16) inline std::array<uint8,kFakeVeh*kVehSize> fakeVehStorage{};
alignas(16) inline std::array<uint8,8> fakeVehStates{};
alignas(16) inline std::array<std::array<uint8,0x60>,kFakeVeh> fakeVehMats{};
inline void PrepareFakePool(Rng& r){
    FillBytes(fakeVehStorage.data(),fakeVehStorage.size(),r);
    const int cap=r.range(0,kFakeVeh); std::memset(fakePool.data(),0,fakePool.size());
    At<void*>(fakePool.data(),0)=fakeVehStorage.data(); At<void*>(fakePool.data(),4)=fakeVehStates.data(); At<uint32>(fakePool.data(),8)=uint32(cap); At<int32>(fakePool.data(),0xC)=-1;
    for(int i=0;i<kFakeVeh;++i){
        uint8* v=fakeVehStorage.data()+kVehSize*size_t(i); fakeVehStates[size_t(i)]= r.pct(30)? uint8(0x80|r.below(0x7F)) : uint8(r.below(0x7F));
        At<int32>(v,0x594)=r.pickI({0,1,2,3,4,5,6,7,8,9,10,11,0,0}); At<uint8>(v,0x4A4)=uint8(r.pct(50)?2:r.pickI({0,1,3,4}));
        At<float>(v,4)=FiniteF(r); At<float>(v,8)=FiniteF(r); At<float>(v,0xC)=FiniteF(r);
        if(r.pct(60)){ auto& m=fakeVehMats[size_t(i)]; FillBytes(m.data(),m.size(),r); At<float>(m.data(),0x30)=FiniteF(r); At<float>(m.data(),0x34)=FiniteF(r); At<float>(m.data(),0x38)=FiniteF(r); At<void*>(v,0x14)=m.data(); }
        else At<void*>(v,0x14)=nullptr; }
    AtA<void*>(gVehPool)=fakePool.data();
}
inline void PrepareStreaming(Rng& r){
    PrepareFakePool(r); RandomLoaded(r,r.pct(50)?40:90);
    playerPresent=r.pct(75); playerPos=CVector{FiniteF(r,3300.f),FiniteF(r,3300.f),FiniteF(r,100.f)};
    AtA<uint8>(gLoadNeeded)=uint8(r.below(2));
    for(int i=0;i<3;++i) AtA<float>(gPathsNeededPos+4*i)=FiniteF(r,3300.f);
    AtA<uint32>(gTimeMs)=r.next(); AtA<uint32>(gTimePrevMs)= r.pct(40) ? AtA<uint32>(gTimeMs)^r.below(600) : r.next();
    P32(oPending)=r.below(2);
    float a,b,c,d; RandRect(r,a,b,c,d); PF(oReqMinX)=a; PF(oReqMaxX)=b; PF(oReqMinY)=c; PF(oReqMaxY)=d;
}
inline json C_UpdateStreaming(Rng& r){
    PrepareStreaming(r); const uint32 force=r.below(2); reinterpret_cast<void(__thiscall*)(CPathFind*,uint32)>(0x450A60)(Paths(),force);
    return {{"force",force},{"loadNeeded",AtA<uint8>(gLoadNeeded)}}; }
inline json C_MakeRequest(Rng& r){
    PrepareStreaming(r); float a,b,c,d; RandRect(r,a,b,c,d);
    reinterpret_cast<void(__thiscall*)(CPathFind*,float,float,float,float)>(0x450D70)(Paths(),a,b,c,d);
    return {{"pending",P32(oPending)&0xFF}}; }
}
