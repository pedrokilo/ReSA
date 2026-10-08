#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../ProbeRuntime.hpp"
namespace PathFindPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float f){return std::bit_cast<uint32>(f);}
inline uint64 Hash(const void* p,size_t n){uint64 h=1469598103934665603ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,2> fns{{{"CPathFind","ComputeRoute",0x452760,0,-1,1024},{"CPathFind","DoPathSearch",0x4515D0,0,-1,1024}}};
inline int fixture{},closestCalls{};
inline CNodeAddress* __fastcall Closest(void*,void*,CNodeAddress* out,CVector pos,ePathType type,float maxDistance,uint16 a,int32 b,uint16 c,uint16 boats,int32 d){trace.push_back({"closest",F(pos.x),F(pos.y),F(pos.z),int(type),F(maxDistance),a,b,c,boats,d});*out=(fixture%16==8||fixture%16==9&&closestCalls==1)?CNodeAddress{}:CNodeAddress{0,uint16(closestCalls?0:3)};++closestCalls;return out;}
inline void __fastcall Search(void*,void*,ePathType type,CVector start,CNodeAddress hint,CVector end,CNodeAddress* nodes,int16& count,int32 cap,float* distance,float maxDist,CNodeAddress* target,float depth,bool lane,CNodeAddress forbidden,bool water,bool boats){trace.push_back({"search",int(type),F(start.x),F(start.y),F(start.z),std::bit_cast<uint32>(hint),F(end.x),F(end.y),F(end.z),cap,distance!=nullptr,F(maxDist),target!=nullptr,F(depth),lane,std::bit_cast<uint32>(forbidden),water,boats});count=int16(fixture%9);for(int i=0;i<count;++i)nodes[i]={uint16(fixture%72),uint16(fixture+i)};}
inline CPathFind paths; inline std::array<CPathNode,6> nodes; inline std::array<CNodeAddress,32> links;inline std::array<uint8,32> lengths;inline std::array<CCarPathLinkAddress,32> navilinks;inline std::array<CCarPathLink,32> navi;
inline json Run(int f,int k,bool candidate){
 fixture=k;closestCalls=0;trace=json::array();memset(&paths,0,sizeof(paths));memset(nodes.data(),0,sizeof(nodes));memset(navi.data(),0,sizeof(navi));
 std::fill(links.begin(),links.end(),CNodeAddress{1,0});lengths.fill(1);std::fill(navilinks.begin(),navilinks.end(),CCarPathLinkAddress{0,0});
 paths.m_pPathNodes[0]=nodes.data();paths.m_pNodeLinks[0]=links.data();paths.m_pLinkLengths[0]=lengths.data();paths.m_pNaviLinks[0]=navilinks.data();paths.m_pNaviNodes[0]=navi.data();paths.m_anNumNodes[0]=6;
 for(int i=0;i<6;++i){nodes[i].m_wAreaId=0;nodes[i].m_wNodeId=uint16(i);nodes[i].m_totalDistFromOrigin=32766;nodes[i].m_nFloodFill=1;nodes[i].m_wBaseLinkId=int16(i*4);navi[i].m_numSameDirLanes=1;navi[i].m_numOppositeDirLanes=1;navi[i].m_attachedTo={0,uint16(i)};}
 auto edge=[&](int a,int b,int len){int j=nodes[a].m_wBaseLinkId+int(nodes[a].m_nNumLinks++);links[j]={0,uint16(b)};lengths[j]=uint8(len);navilinks[j]=CCarPathLinkAddress{0,size_t(b)};};
 int weight=1+k/16%20;edge(0,1,weight);edge(1,0,weight);edge(1,2,weight+1);edge(2,1,weight+1);edge(2,3,weight+2);edge(3,2,weight+2);edge(3,4,100);edge(4,3,100);edge(4,5,50);edge(5,4,50);
 CNodeAddress origin{0,0},target{0,3},forbidden{};bool lane=false,water=false;float depth=1000.0f;
 switch(k%16){case 1:target=origin;break;case 2:nodes[0].m_nFloodFill=2;break;case 3:forbidden={0,2};break;case 4:nodes[2].m_bWaterNode=true;break;case 5:nodes[2].m_bWaterNode=true;water=true;break;case 6:depth=1;break;case 7:lane=true;navi[2].m_numSameDirLanes=0;navi[2].m_numOppositeDirLanes=0;break;case 8:case 9:case 10:origin={};target={};break;case 11:origin={};break;case 12:target={};break;case 13:lane=true;break;case 14:forbidden={0,1};break;case 15:depth=0;break;}
 memset(reinterpret_cast<void*>(0x972CD0),0xCD,20000);
 CNodeRoute route;memset(&route,0xCD,sizeof(route));CVector start{float(k),-20,3},end{float(k+30),10,8};
 std::array<CNodeAddress,12> result;memset(result.data(),0xCD,sizeof(result));int16 count=-123;float distance=-321.0f;
 if(f==0){using ReversingProbe::EntryPatch;EntryPatch a(0x4515D0,Search),b(std::bit_cast<uintptr_t>(&CPathFind::DoPathSearch),Search);plugin::CallMethod<0x452760>(&paths,uint8(k%2),&start,&end,&origin,&route);}
 else {auto* hint=k%16==12?nullptr:&target;bool out=k%7!=0;int cap=1+k%8;if(candidate)paths.DoPathSearch(PATH_TYPE_PED,start,origin,end,result.data(),count,cap,out?&distance:nullptr,999.0f,hint,depth,lane,forbidden,water,k%2!=0);else plugin::CallMethod<0x4515D0>(&paths,PATH_TYPE_PED,start,origin,end,result.data(),&count,cap,out?&distance:nullptr,999.0f,hint,depth,lane,forbidden,water,k%2!=0);}
 return {{"trace",trace},{"route",Hash(&route,sizeof(route))},{"count",count},{"distance",F(distance)},{"result",Hash(result.data(),sizeof(result))},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"paths",Hash(&paths,sizeof(paths))},{"cleared",Hash(reinterpret_cast<void*>(0x972CD0),20000)}};
}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"path-find-pending-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"path-find-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("path-find-pending");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"path-find-pending-only.txt");std::getline(f,only);}std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};std::array<bool,2> enabled{},locked{};for(int i=0;i<2;++i){hooks[i]=FindHook(fns[i].cls,fns[i].name);if(!hooks[i]){Write({{"event","error"},{"reason","missing hook"}});return;}enabled[i]=hooks[i]->Hooked();locked[i]=hooks[i]->Locked();hooks[i]->LockState(false);}
 auto* clear=reinterpret_cast<uint8*>(0x972CD0);std::vector<uint8> saved(clear,clear+20000);bool passed=true;int pairs=0;
 {ReversingProbe::EntryPatch dep(0x44F460,Closest);for(int f=0;f<2;++f){std::string full=std::string(fns[f].cls)+"::"+fns[f].name;if(!only.empty()&&only!=full)continue;int failures=0,success=0,empty=0;for(int k=0;k<fns[f].cases;++k){json r[2];for(int side=0;side<2;++side){hooks[f]->State(side!=0);if(ReversingProbe::JumpsToDll(fns[f].address)!=(side!=0)){Write({{"event","error"},{"reason","hook switch failed"}});passed=false;break;}r[side]=Run(f,k,side!=0);}bool ok=r[0]==r[1];if(!ok)++failures;if(f==1){if(r[0]["count"].get<int>()>0)++success;else ++empty;}Write({{"event","comparison"},{"pair",pairs},{"function",full},{"case",k},{"equal",ok},{"accepted",ok},{"original",r[0]},{"candidate",r[1]}});++pairs;}bool coverage=f==0||(success>0&&empty>0);passed&=failures==0&&coverage;Write({{"event","function_summary"},{"function",full},{"cases",fns[f].cases},{"failures",failures},{"coverage",coverage},{"success",success},{"empty",empty}});}}
 memcpy(clear,saved.data(),saved.size());for(int i=0;i<2;++i){hooks[i]->State(enabled[i]);hooks[i]->LockState(locked[i]);}Write({{"event",passed?"complete":"error"},{"passed",passed},{"pairs",pairs},{"state_restored",true}});
}
}
