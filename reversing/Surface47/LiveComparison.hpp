#pragma once
// Banco dentro del motor: SurfaceInfos_c (lista de 34, 47 funciones). Original (hooks del banco desactivados) frente a
// candidato con fixtures deterministas. Se trabaja sobre el global g_surfaceInfos (0xB79538), que se guarda entero
// (0x8F4 bytes) junto con CWeather::WetRoads, se rellena en cada ejecución y se restaura por ejecución y al final.
// Init lee surface.dat, surfinfo.dat y surfaud.dat desde memoria: CFileMgr::SetDir/OpenFile/CloseFile y
// CFileLoader::LoadLine se simulan con trazas (nunca se leen ficheros reales). Las consultas Is*/Get*/Creates*/...
// reciben ids válidos (< 179) sobre tablas de bits aleatorias; GetAdhesiveLimit recibe un CColPoint falso.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "SurfaceInfos_c.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "Weather.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace Surface47Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- ficheros en memoria: 0 = surface.dat, 1 = surfinfo.dat, 2 = surfaud.dat (por orden de apertura) ---
constexpr uintptr_t FAKE_STREAM=0x5F470;
inline std::array<std::vector<std::string>,3> files; inline std::array<size_t,3> fileIdx{}; inline uint32 opens{};
inline std::array<char,512> lineBuf{};
inline int32 __cdecl FakeSetDir(const char* p){ trace.push_back({"set_dir",std::string(p ? p : "<null>")}); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){
    trace.push_back({"open",std::string(p),std::string(m)}); const uint32 k=opens<3 ? opens : 2; ++opens;
    return reinterpret_cast<FILESTREAM>(FAKE_STREAM+k); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ trace.push_back({"close",uintptr_t(f)}); return 0; }
inline char* __cdecl FakeLoadLine(FILESTREAM f){
    const uintptr_t k=uintptr_t(f)-FAKE_STREAM; if(k>=3){ trace.push_back({"bad_stream",uintptr_t(f)}); return nullptr; }
    if(fileIdx[k]>=files[k].size()){ trace.push_back({"eof",k}); return nullptr; }
    const auto& l=files[k][fileIdx[k]++]; lineBuf.fill(0); std::memcpy(lineBuf.data(),l.data(),std::min(l.size(),lineBuf.size()-1));
    return lineBuf.data();
}

inline constexpr const char* names[]{
    "DEFAULT","TARMAC","TARMAC_FUCKED","TARMAC_REALLYFUCKED","PAVEMENT","PAVEMENT_FUCKED","GRAVEL","FUCKED_CONCRETE","PAINTED_GROUND",
    "GRASS_SHORT_LUSH","GRASS_MEDIUM_LUSH","GRASS_LONG_LUSH","GRASS_SHORT_DRY","GRASS_MEDIUM_DRY","GRASS_LONG_DRY","GOLFGRASS_ROUGH",
    "GOLFGRASS_SMOOTH","STEEP_SLIDYGRASS","STEEP_CLIFF","FLOWERBED","MEADOW","WASTEGROUND","WOODLANDGROUND","VEGETATION","MUD_WET","MUD_DRY","DIRT",
    "DIRTTRACK","SAND_DEEP","SAND_MEDIUM","SAND_COMPACT","SAND_ARID","SAND_MORE","SAND_BEACH","CONCRETE_BEACH","ROCK_DRY","ROCK_WET","ROCK_CLIFF",
    "WATER_RIVERBED","WATER_SHALLOW","CORNFIELD","HEDGE","WOOD_CRATES","WOOD_SOLID","WOOD_THIN","GLASS","GLASS_WINDOWS_LARGE","GLASS_WINDOWS_SMALL",
    "EMPTY1","EMPTY2","GARAGE_DOOR","THICK_METAL_PLATE","SCAFFOLD_POLE","LAMP_POST","METAL_GATE","METAL_CHAIN_FENCE","GIRDER","FIRE_HYDRANT",
    "CONTAINER","NEWS_VENDOR","WHEELBASE","CARDBOARDBOX","PED","CAR","CAR_PANEL","CAR_MOVINGCOMPONENT","TRANSPARENT_CLOTH","RUBBER","PLASTIC",
    "TRANSPARENT_STONE","WOOD_BENCH","CARPET","FLOORBOARD","STAIRSWOOD","P_SAND","P_SAND_DENSE","P_SAND_ARID","P_SAND_COMPACT","P_SAND_ROCKY",
    "P_SANDBEACH","P_GRASS_SHORT","P_GRASS_MEADOW","P_GRASS_DRY","P_WOODLAND","P_WOODDENSE","P_ROADSIDE","P_ROADSIDEDES","P_FLOWERBED",
    "P_WASTEGROUND","P_CONCRETE","P_OFFICEDESK","P_711SHELF1","P_711SHELF2","P_711SHELF3","P_RESTUARANTTABLE","P_BARTABLE","P_UNDERWATERLUSH",
    "P_UNDERWATERBARREN","P_UNDERWATERCORAL","P_UNDERWATERDEEP","P_RIVERBED","P_RUBBLE","P_BEDROOMFLOOR","P_KIRCHENFLOOR","P_LIVINGRMFLOOR",
    "P_CORRIDORFLOOR","P_711FLOOR","P_FASTFOODFLOOR","P_SKANKYFLOOR","P_MOUNTAIN","P_MARSH","P_BUSHY","P_BUSHYMIX","P_BUSHYDRY","P_BUSHYMID",
    "P_GRASSWEEFLOWERS","P_GRASSDRYTALL","P_GRASSLUSHTALL","P_GRASSGRNMIX","P_GRASSBRNMIX","P_GRASSLOW","P_GRASSROCKY","P_GRASSSMALLTREES",
    "P_DIRTROCKY","P_DIRTWEEDS","P_GRASSWEEDS","P_RIVEREDGE","P_POOLSIDE","P_FORESTSTUMPS","P_FORESTSTICKS","P_FORRESTLEAVES","P_DESERTROCKS",
    "P_FORRESTDRY","P_SPARSEFLOWERS","P_BUILDINGSITE","P_DOCKLANDS","P_INDUSTRIAL","P_INDUSTJETTY","P_CONCRETELITTER","P_ALLEYRUBISH",
    "P_JUNKYARDPILES","P_JUNKYARDGRND","P_DUMP","P_CACTUSDENSE","P_AIRPORTGRND","P_CORNFIELD","P_GRASSLIGHT","P_GRASSLIGHTER","P_GRASSLIGHTER2",
    "P_GRASSMID1","P_GRASSMID2","P_GRASSDARK","P_GRASSDARK2","P_GRASSDIRTMIX","P_RIVERBEDSTONE","P_RIVERBEDSHALLOW","P_RIVERBEDWEEDS","P_SEAWEED",
    "DOOR","PLASTICBARRIER","PARKGRASS","STAIRSSTONE","STAIRSMETAL","STAIRSCARPET","FLOORMETAL","FLOORCONCRETE","BIN_BAG","THIN_METAL_SHEET",
    "METAL_BARREL","PLASTIC_CONE","PLASTIC_DUMPSTER","METAL_DUMPSTER","WOOD_PICKET_FENCE","WOOD_SLATTED_FENCE","WOOD_RANCH_FENCE","UNBREAKABLE_GLASS",
    "HAY_BALE","GORE","RAILTRACK",
};
static_assert(std::size(names)==TOTAL_NUM_SURFACE_TYPES);

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,47> fns{{
    {"SurfaceInfos_c","GetSurfaceIdFromName",0x55D220,0,-1,1024},{"SurfaceInfos_c","Init",0x55F420,0,-1,256},
    {"SurfaceInfos_c","GetAdhesionGroup",0x55E5C0,0,-1,256},{"SurfaceInfos_c","GetTyreGrip",0x55E5E0,0,-1,256},
    {"SurfaceInfos_c","GetWetMultiplier",0x55E600,0,-1,256},{"SurfaceInfos_c","GetSkidmarkType",0x55E630,0,-1,256},
    {"SurfaceInfos_c","GetFrictionEffect",0x55E650,0,-1,256},{"SurfaceInfos_c","GetBulletFx",0x55E670,0,-1,256},
    {"SurfaceInfos_c","IsSoftLanding",0x55E690,0,-1,192},{"SurfaceInfos_c","IsSeeThrough",0x55E6B0,0,-1,192},
    {"SurfaceInfos_c","IsShootThrough",0x55E6D0,0,-1,192},{"SurfaceInfos_c","IsSand",0x55E6F0,0,-1,192},
    {"SurfaceInfos_c","IsWater",0x55E710,0,-1,192},{"SurfaceInfos_c","IsShallowWater",0x55E730,0,-1,192},
    {"SurfaceInfos_c","IsBeach",0x55E750,0,-1,192},{"SurfaceInfos_c","IsSteepSlope",0x55E770,0,-1,192},
    {"SurfaceInfos_c","IsGlass",0x55E790,0,-1,192},{"SurfaceInfos_c","IsStairs",0x55E7B0,0,-1,192},
    {"SurfaceInfos_c","IsSkateable",0x55E7D0,0,-1,192},{"SurfaceInfos_c","IsPavement",0x55E7F0,0,-1,192},
    {"SurfaceInfos_c","GetRoughness",0x55E810,0,-1,256},{"SurfaceInfos_c","GetFlammability",0x55E830,0,-1,256},
    {"SurfaceInfos_c","CreatesSparks",0x55E850,0,-1,192},{"SurfaceInfos_c","CantSprintOn",0x55E870,0,-1,192},
    {"SurfaceInfos_c","LeavesFootsteps",0x55E890,0,-1,192},{"SurfaceInfos_c","ProducesFootDust",0x55E8B0,0,-1,192},
    {"SurfaceInfos_c","MakesCarDirty",0x55E8D0,0,-1,192},{"SurfaceInfos_c","MakesCarClean",0x55E8F0,0,-1,192},
    {"SurfaceInfos_c","CreatesWheelGrass",0x55E910,0,-1,192},{"SurfaceInfos_c","CreatesWheelGravel",0x55E930,0,-1,192},
    {"SurfaceInfos_c","CreatesWheelMud",0x55E950,0,-1,192},{"SurfaceInfos_c","CreatesWheelDust",0x55E970,0,-1,192},
    {"SurfaceInfos_c","CreatesWheelSand",0x55E990,0,-1,192},{"SurfaceInfos_c","CreatesWheelSpray",0x55E9B0,0,-1,192},
    {"SurfaceInfos_c","CreatesPlants",0x55E9D0,0,-1,192},{"SurfaceInfos_c","CreatesObjects",0x55E9F0,0,-1,192},
    {"SurfaceInfos_c","CanClimb",0x55EA10,0,-1,192},{"SurfaceInfos_c","IsAudioConcrete",0x55EA30,0,-1,192},
    {"SurfaceInfos_c","IsAudioGrass",0x55EA50,0,-1,192},{"SurfaceInfos_c","IsAudioSand",0x55EA70,0,-1,192},
    {"SurfaceInfos_c","IsAudioGravel",0x55EA90,0,-1,192},{"SurfaceInfos_c","IsAudioWood",0x55EAB0,0,-1,192},
    {"SurfaceInfos_c","IsAudioWater",0x55EAD0,0,-1,192},{"SurfaceInfos_c","IsAudioMetal",0x55EAF0,0,-1,192},
    {"SurfaceInfos_c","IsAudioLongGrass",0x55EB10,0,-1,192},{"SurfaceInfos_c","IsAudioTile",0x55EB30,0,-1,192},
    {"SurfaceInfos_c","GetAdhesiveLimit",0x55EB50,0,-1,512},
}};

// --- estado global guardado / comparado ---
struct Range { uintptr_t at; size_t n; const char* tag; };
inline std::vector<Range> ranges;
inline void InitRanges() { ranges={{0xB79538,0x8F4,"surface_infos"},{0xC81308,4,"wet_roads"}}; }
inline std::vector<uint8> Dump() { std::vector<uint8> d; for(auto& r:ranges){ auto* p=reinterpret_cast<const uint8*>(r.at); d.insert(d.end(),p,p+r.n); } return d; }
inline void Restore(const std::vector<uint8>& d) { size_t o=0; for(auto& r:ranges){ std::memcpy(reinterpret_cast<void*>(r.at),d.data()+o,r.n); o+=r.n; } }
inline json Diffs(const std::vector<uint8>& a,const std::vector<uint8>& b) {
    json out=json::array(); size_t o=0;
    for(auto& r:ranges){ for(size_t k=0;k<r.n&&out.size()<24;++k)if(a[o+k]!=b[o+k])out.push_back({r.tag,r.at+k,a[o+k],b[o+k]}); o+=r.n; }
    return out;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"surface47-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"surface47-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("surface47");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"surface47-only.txt"); if(f)std::getline(f,only); }
    if(reinterpret_cast<uintptr_t>(&g_surfaceInfos)!=0xB79538||reinterpret_cast<uintptr_t>(&g_surfaceInfos.m_surfaces[0])!=0xB79538+0x90
       ||reinterpret_cast<uintptr_t>(&CWeather::WetRoads)!=0xC81308){
        Write({{"event","error"},{"reason","layout"},{"surfaces",reinterpret_cast<uintptr_t>(&g_surfaceInfos.m_surfaces[0])}}); return; }
    std::array<ReversibleHooks::ReversibleHook::Base*,47> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,47> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    InitRanges(); const auto saved=Dump();
    Write({{"event","player_ready"},{"fp_control",_controlfp(0,0)}});
    bool passed=true; int pair=0;
    auto* const si=&g_surfaceInfos;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        // Cuerpos DLL resueltos antes de escribir ningún parche.
        const auto dSetDir=bit_cast<uintptr_t>(&CFileMgr::SetDir), dOpen=bit_cast<uintptr_t>(&CFileMgr::OpenFile), dClose=bit_cast<uintptr_t>(&CFileMgr::CloseFile);
        const auto dLoadLine=bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine));
        EntryPatch s1(dSetDir,FakeSetDir), s1e(0x5387D0,FakeSetDir);
        EntryPatch s2(dOpen,FakeOpenFile), s2e(0x538900,FakeOpenFile);
        EntryPatch s3(dClose,FakeCloseFile), s3e(0x5389D0,FakeCloseFile);
        EntryPatch s4(dLoadLine,FakeLoadLine), s4e(0x536F80,FakeLoadLine);
        std::vector<uint8> dumps[2];

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                // Tabla completa: límites como floats finitos y superficies con bytes aleatorios (todos los bits).
                auto fixture=[&]{
                    auto* b=reinterpret_cast<uint8*>(si); for(size_t k=0;k<sizeof(SurfaceInfos_c);++k)b[k]=uint8(next());
                    auto* lim=si->m_adhesiveLimits.data()->data(); for(int k=0;k<36;++k)lim[k]=next()%8==0 ? float(next()%3) : uni(-4.f,4.f);
                    if(next()%4==0){ /* patrones extremos de bits */ const uint32 v=next()%2 ? 0xFFFFFFFFu : 0u; for(auto& s:si->m_surfaces){ std::memcpy(reinterpret_cast<uint8*>(&s)+4,&v,4); std::memcpy(reinterpret_cast<uint8*>(&s)+8,&v,4); } }
                    CWeather::WetRoads=next()%5==0 ? float(next()%2) : uni(0.f,1.f);
                };
                auto id=[&]()->uint32{ return next()%8==0 ? (next()%2 ? 0u : uint32(TOTAL_NUM_SURFACE_TYPES-1)) : next()%TOTAL_NUM_SURFACE_TYPES; };
                // Números con fracción binaria exacta (los CRT del EXE y de la DLL los leen igual).
                auto num=[&](float lo,float hi){ const float v=uni(lo,hi); static const char* fr[]{"0","25","5","75","125","375","0625"};
                    return std::string(v<0 ? "-" : "")+std::to_string(int(std::floor(std::abs(v))))+"."+fr[next()%7]; };
                auto sep=[&]{ static const char* s[]{" "," "," ","  ","\t"," \t "}; return std::string(s[next()%6]); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5F47u; if(!stt)stt=1; trace=json::array();
                    Restore(saved);
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate; const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(nm=="Init"){
                        fixture(); for(auto& fl:files)fl.clear(); fileIdx={}; opens=0;
                        auto noise=[&](std::vector<std::string>& v,char c){ while(next()%4==0)v.push_back(next()%2 ? std::string(1,c)+" comentario de prueba" : ""); };
                        // surface.dat: matriz triangular (normalmente 6 filas; a veces menos o más, sin límite en el EXE).
                        { auto& v=files[0]; const int rows=next()%4==0 ? int(next()%9) : 6;
                          for(int r=0;r<rows;++r){ noise(v,';'); std::string l=next()%3 ? std::string(names[67+next()%3]) : "FILA"+std::to_string(r); l+=next()%4==0 ? "\t" : " ";
                              const int vals=next()%8==0 ? int(next()%(r+3)) : r+1;
                              for(int c=0;c<vals;++c){ if(c)l+=sep(); const uint32 k=next()%12;
                                  static const char* real[]{"6.0","3.6","2.0","4.5","3.0","3.2","3.5","1.0","2.8","0.5"}; /* valores del surface.dat real */
                                  l+=k==0 ? std::string("-") : k==1 ? "-"+num(0.f,2.f) : k==2 ? std::string("x") : k==3 ? num(0.f,2.f)+"z" : k<=6 ? std::string(real[next()%10]) : num(0.f,3.f); }
                              if(next()%5==0)l+=sep();
                              v.push_back(l); }
                          noise(v,';'); }
                        // surfinfo.dat: 36 campos por línea.
                        { auto& v=files[1]; const int n=4+next()%36;
                          static const char* adh[]{"RUBBER","HARD","ROAD","LOOSE","SAND","WET","ICE","rubber"};
                          static const char* skid[]{"DEFAULT","SANDY","MUDDY","BLOODY","sandy"};
                          static const char* fric[]{"NONE","SPARKS","MORE"};
                          static const char* fx[]{"NONE","SPARKS","SAND","WOOD","DUST","GLASS"};
                          auto inum=[&]{ static const int odd[]{2,3,-1,5,7,-2,255,4,-3}; return std::to_string(next()%4==0 ? odd[next()%9] : int(next()%2)); };
                          for(int r=0;r<n;++r){ noise(v,'#');
                              std::string l=next()%10==0 ? "NO_EXISTE"+std::to_string(next()%9) : std::string(names[next()%TOTAL_NUM_SURFACE_TYPES]);
                              l+=sep()+adh[next()%8]+sep()+num(next()%5==0 ? -20.f : 0.f,30.f)+sep()+num(-1.5f,1.5f)+sep()+skid[next()%5]+sep()+fric[next()%3];
                              for(int k=0;k<29;++k)l+=sep()+inum();
                              l+=sep()+fx[next()%6]; if(next()%5==0)l+=sep();
                              v.push_back(l); }
                          noise(v,'#'); }
                        // surfaud.dat: nombre + 9 enteros.
                        { auto& v=files[2]; const int n=4+next()%36;
                          for(int r=0;r<n;++r){ noise(v,'#');
                              std::string l=next()%10==0 ? "NO_EXISTE" : std::string(names[next()%TOTAL_NUM_SURFACE_TYPES]);
                              for(int k=0;k<9;++k)l+=sep()+std::to_string(next()%4==0 ? int(next()%9)-3 : int(next()%2));
                              v.push_back(l); }
                          noise(v,'#'); }
                        reinterpret_cast<void(__thiscall*)(SurfaceInfos_c*)>(a)(si);
                        out["lines_read"]=json::array({fileIdx[0],fileIdx[1],fileIdx[2]}); out["opens"]=opens;
                    }
                    else if(nm=="GetSurfaceIdFromName"){
                        fixture(); std::string s=names[next()%TOTAL_NUM_SURFACE_TYPES];
                        switch(next()%12){
                            case 0: s+="X"; break;
                            case 1: s.pop_back(); break;
                            case 2: s[0]=char(std::tolower(uint8(s[0]))); break;
                            case 3: s=""; break;
                            case 4: s=" "+s; break;
                            case 5: s=std::string(names[next()%TOTAL_NUM_SURFACE_TYPES])+std::string(names[next()%TOTAL_NUM_SURFACE_TYPES]); break;
                            case 6: s=std::string(1,char('A'+next()%26))+std::to_string(next()%100); break;
                            default: break;
                        }
                        out["name"]=s;
                        ret=reinterpret_cast<uint32(__thiscall*)(SurfaceInfos_c*,const char*)>(a)(si,s.c_str());
                    }
                    else if(nm=="GetAdhesiveLimit"){
                        fixture(); alignas(4) std::array<uint8,0x2C> cp{} /* sizeof(CColPoint) */; for(auto& b:cp)b=uint8(next());
                        cp[0x20]=uint8(id()); cp[0x23]=uint8(id());
                        /* grupos 6/7 leen los floats 36..49 (bytes de las primeras superficies): sin NaN/Inf en ellos */
                        { auto* lim=reinterpret_cast<uint32*>(si->m_adhesiveLimits.data()->data()); for(int k=36;k<50;++k)if((lim[k]&0x7F800000u)==0x7F800000u)lim[k]&=~0x40000000u; }
                        ret=F(reinterpret_cast<float(__thiscall*)(SurfaceInfos_c*,CColPoint*)>(a)(si,reinterpret_cast<CColPoint*>(cp.data())));
                        out["ids"]=json::array({cp[0x20],cp[0x23]});
                    }
                    else {
                        // Consultas de un id: 8 ids por caso sobre la misma tabla.
                        fixture(); ret=json::array();
                        const bool isFloat=nm=="GetTyreGrip"||nm=="GetWetMultiplier";
                        const bool isBool=nm.starts_with("Is")||nm.starts_with("Creates")||nm=="CanClimb"||nm=="CantSprintOn"||nm=="LeavesFootsteps"
                                          ||nm=="ProducesFootDust"||nm=="MakesCarDirty"||nm=="MakesCarClean";
                        for(int k=0;k<8;++k){ const uint32 s=id();
                            if(isFloat) ret.push_back({s,F(reinterpret_cast<float(__thiscall*)(SurfaceInfos_c*,uint32)>(a)(si,s))});
                            else { const uint32 r=reinterpret_cast<uint32(__thiscall*)(SurfaceInfos_c*,uint32)>(a)(si,s);
                                   ret.push_back({s,isBool ? (r&0xFF) : r}); } /* bool: los llamadores del EXE solo miran AL */
                        }
                    }

                    auto d=Dump(); out["state"]=Fnv(d.data(),d.size()); dumps[candidate]=std::move(d);
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_diffs",Diffs(dumps[0],dumps[1])},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        Restore(saved);
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Restore(saved); for(auto& fl:files){ fl.clear(); fl.shrink_to_fit(); }
    const bool restored=Dump()==saved;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
