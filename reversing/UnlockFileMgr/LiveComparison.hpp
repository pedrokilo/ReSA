#pragma once
#include <cerrno>
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "FileMgr.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace FMLive {
inline bool done{};inline std::ofstream log;
inline constexpr std::array names{"SetDirMyDocuments","LoadFile","OpenFile","OpenFileForWriting","OpenFileForAppending","Read","Write","Seek","ReadLine","CloseFile","GetTotalSize","Tell","GetErrorReadWrite"};
inline constexpr std::array<uintptr_t,13> addresses{0x538860,0x538890,0x538900,0x538910,0x538930,0x538950,0x538970,0x538990,0x5389B0,0x5389D0,0x5389E0,0x538A20,0x538A50};
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
// El fixture tiene `n` bytes (256 por defecto); LoadFile se prueba ademas con 0, 1, 0x3FFF, 0x4000, 0x4001, 0x8000 y 0xC001.
inline void Fixture(const std::filesystem::path& p,size_t n=256){std::ofstream f(p,std::ios::binary|std::ios::trunc);for(size_t i=0;i<n;++i)f.put(char(i%29==0?'\n':i%29==28?'\r':32+i%91));}
inline std::vector<uint8> Raw(const std::filesystem::path& p){std::ifstream f(p,std::ios::binary);return std::vector<uint8>(std::istreambuf_iterator<char>(f),{});}
inline uint64_t Fnv(const uint8* d,size_t n){uint64_t h=1469598103934665603ull;for(size_t i=0;i<n;++i){h^=d[i];h*=1099511628211ull;}return h;}
// Contenido que debe devolver LoadFile: en modo texto la CRT convierte \r\n en \n.
inline std::vector<uint8> Expected(const std::vector<uint8>& raw,bool text){std::vector<uint8> w;w.reserve(raw.size());for(size_t k=0;k<raw.size();++k)if(!text||!(raw[k]=='\r'&&k+1<raw.size()&&raw[k+1]=='\n'))w.push_back(raw[k]);return w;}
inline json Disk(const std::filesystem::path& p){std::ifstream f(p,std::ios::binary);return std::vector<uint8>(std::istreambuf_iterator<char>(f),{});}
// El FILE* del EXE (CRT estatico, 0x8232D8) y el de la DLL (UCRT) NO son intercambiables ni tienen el mismo diseno interno:
// el instantaneo solo usa la posicion, y siempre por direccion (EXE con los ganchos apagados, DLL con ellos encendidos).
inline json State(FILESTREAM f){return json{{"position",reinterpret_cast<int(__cdecl*)(FILESTREAM)>(0x538A20)(f)}};}
// Propietario del FILE*: el CRT estatico del EXE reparte sus flujos de _iob (0x8E2C30..0x8E2E90, 20 entradas de 0x20 bytes) o de __piob
// (0xC9C418, _nstream en 0xC9D420, bloques de 0x38 del monton del EXE); los de la UCRT de la DLL no estan en ninguna de las dos tablas.
inline bool ExeOwned(const void* f){const auto a=reinterpret_cast<uintptr_t>(f);if(a>=0x8E2C30&&a<=0x8E2E90)return (a-0x8E2C30)%0x20==0;const int n=*reinterpret_cast<const int*>(0xC9D420);void* const* const t=*reinterpret_cast<void* const* const*>(0xC9C418);for(int k=0;t&&k<n;++k)if(t[k]==f)return true;return false;}
// errno del CRT del EXE (_errno, 0x82B181): distinto del errno de la UCRT de la DLL.
inline int& ExeErrno(){return *reinterpret_cast<int*(__cdecl*)()>(0x82B181)();}
using F1=size_t(__cdecl*)(const char*,uint8*,size_t,const char*);using Open=FILESTREAM(__cdecl*)(const char*,const char*);using OpenOne=FILESTREAM(__cdecl*)(const char*);
using Read=size_t(__cdecl*)(FILESTREAM,void*,size_t);using WriteFile=size_t(__cdecl*)(FILESTREAM,const void*,size_t);
using Seek=bool(__cdecl*)(FILESTREAM,long,int);using Tell=int(__cdecl*)(FILESTREAM);using Close=int(__cdecl*)(FILESTREAM);
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto game=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(game/"unlock-file-mgr-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(game/"unlock-file-mgr-runtime.jsonl");Write({{"event","waiting_for_player"}});}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
 using Hook=ReversibleHooks::ReversibleHook::Base;std::array<Hook*,13> hooks{};std::array<bool,13> enabled{},locked{};
 ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CFileMgr")for(auto& h:c.Items())for(int i=0;i<13;++i)if(h->Name()==names[i])hooks[i]=h.get();});
 for(auto* h:hooks)if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}
 const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));const auto cwd=std::filesystem::current_path();
 const auto dirBefore=ReversingProbe::Bytes(CFileMgr::ms_dirName,128);const auto userBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC92368),256);
 const auto folder=std::filesystem::path(__FILE__).parent_path()/"artifacts";const auto file=folder/"fixture.txt";const auto path=file.string();
 const auto tempUser=folder.string();assert(tempUser.size()<128);std::strcpy(reinterpret_cast<char*>(0xC92368),tempUser.c_str());
 for(int i=0;i<13;++i){enabled[i]=hooks[i]->Hooked();locked[i]=hooks[i]->Locked();hooks[i]->LockState(false);Write({{"event","hook_state"},{"function",i},{"enabled",enabled[i]},{"locked",locked[i]}});}
 bool passed=true;int pairs=0;
 for(int fn=0;fn<13&&passed;++fn)for(int i=0;i<128&&passed;++i){
  auto run=[&](const bool dll){
   std::filesystem::current_path(cwd);std::memcpy(CFileMgr::ms_dirName,dirBefore.data(),128);Fixture(file);
   {std::error_code ec;std::filesystem::remove(path+".missing",ec);} // las aperturas en modo escritura de runs anteriores lo creaban y dejaban de ser "ausente"
   std::array<uint8,1024> bytes;bytes.fill(0xA5);json r;FILESTREAM stream{};
   if(fn==0){using F=int(__cdecl*)();r["return"]=reinterpret_cast<F>(addresses[fn])();r["cwd"]=std::filesystem::current_path().string();r["dir"]=ReversingProbe::Bytes(CFileMgr::ms_dirName,128);return r;}
   if(fn==1){
    // LoadFile (EXE 0x538890) ignora `size`: lee bloques de 0x4000 hasta uno incompleto y escribe un 0 final. Se usa un bufer de monton con
    // sitio para n+0x4000+64 bytes (relleno 0xA5): un bufer de pila de 1024 bytes quedaba corto para ficheros grandes y dependia de la
    // distancia al tope de la pila (la CRT pide a ReadFile 0x4000 bytes de destino). Se compara ademas con un modelo independiente del fichero.
    constexpr std::array<size_t,8> sizes{0,1,256,0x3FFF,0x4000,0x4001,0x8000,0xC001};constexpr std::array<size_t,6> sizeArgs{0,1,16,256,0x4000,0x7FFFFFFF};
    const size_t n=sizes[i%8];const bool text=i/8%2!=0;const char* mode=text?"r":"rb";const size_t vi=size_t(i/16);const bool absent=vi==7;
    Fixture(file,n);const auto raw=Raw(file);const auto want=Expected(raw,text);std::vector<uint8> buf(n+0x4000+64,0xA5);
    const size_t sizeArg=vi<sizeArgs.size()?sizeArgs[vi]:buf.size();
    const auto p=absent?path+".missing":path;
    const uint32 ret=uint32(reinterpret_cast<F1>(addresses[fn])(p.c_str(),buf.data(),sizeArg,mode));
    const uint32 expectedRet=absent?0xFFFFFFFFu:uint32(want.size());
    const bool valid=ret!=0xFFFFFFFFu&&size_t(ret)<buf.size();
    const bool contentOk=absent||(valid&&buf[ret]==0&&std::equal(want.begin(),want.end(),buf.begin()));
    // En modo texto la CRT lee en bruto sobre el destino y compacta CRLF despues: quedan restos del fichero hasta file_size; lo de mas alla debe seguir intacto.
    const size_t tailFrom=std::max<size_t>(valid?size_t(ret)+1:0,text?raw.size()+1:0);
    const bool tailOk=std::all_of(buf.begin()+tailFrom,buf.end(),[](uint8 b){return b==0xA5;});
    r["return"]=ret;r["expected_return"]=expectedRet;r["absent"]=absent;r["mode"]=mode;r["size_arg"]=sizeArg;r["file_size"]=raw.size();
    r["terminator"]=valid?int(buf[ret]):-1;r["content_hash"]=Fnv(buf.data(),valid?ret:0);r["tail_intact"]=tailOk;
    const auto after=Raw(file);r["disk_hash"]=Fnv(after.data(),after.size());r["missing_exists_after"]=std::filesystem::exists(path+".missing");
    r["expect_ok"]=ret==expectedRet&&contentOk&&tailOk&&!r["missing_exists_after"].get<bool>();
    return r;}
   if(fn==2){const auto p=i%7?path:path+".missing";stream=reinterpret_cast<Open>(addresses[fn])(p.c_str(),std::array{"rb","r","r+b","wb","a","a+b","w+b","rb"}[i%8]);r["opened"]=stream!=nullptr;}
   else if(fn==3||fn==4){stream=reinterpret_cast<OpenOne>(addresses[fn])(path.c_str());r["opened"]=stream!=nullptr;}
   else {stream=reinterpret_cast<Open>(addresses[2])(path.c_str(),fn==12||i%5==0?"rb":"r+b");assert(stream);reinterpret_cast<Seek>(addresses[7])(stream,long(i%19),SEEK_SET);}
   if(stream){
    if(fn==5)r["return"]=reinterpret_cast<Read>(addresses[fn])(stream,bytes.data(),size_t(std::array{0,1,15,255,256,257,511,1024}[i%8]));
    if(fn==6)r["return"]=reinterpret_cast<WriteFile>(addresses[fn])(stream,bytes.data(),size_t(std::array{0,1,15,255}[i%4]));
    if(fn==7){
     // Origenes 0..2 validos y 3, -1, 4 e INT_MIN invalidos: el EXE devuelve -1 (true) con errno = EINVAL y deja el flujo intacto; la DLL debe hacer lo mismo sin fail-fast.
     constexpr std::array<int,16> origins{0,1,2,3,0,1,2,-1,0,1,2,4,0,1,2,(-2147483647-1)};const int origin=origins[i/8];const bool bad=origin<0||origin>2;
     if(bad){ExeErrno()=0;errno=0;}
     const bool rv=reinterpret_cast<Seek>(addresses[fn])(stream,long(std::array{-512,-1,0,1,15,256,512,1024}[i%8]),origin);const int e=dll?errno:ExeErrno();
     r["return"]=rv;r["origin"]=origin;
     if(bad){uint8 nb=0xA5;r["errno"]=e;r["next_read"]=reinterpret_cast<Read>(addresses[5])(stream,&nb,1);r["next_byte"]=nb;}
    }
    if(fn==8){using F=bool(__cdecl*)(FILESTREAM,char*,int);r["return"]=reinterpret_cast<F>(addresses[fn])(stream,reinterpret_cast<char*>(bytes.data()),std::array{0,1,2,16,256,512,1024,8}[i%8]);}
    if(fn==10||fn==11)r["return"]=reinterpret_cast<Tell>(addresses[fn])(stream);
    if(fn==12){if(i&1)r["write_to_readonly"]=reinterpret_cast<WriteFile>(addresses[6])(stream,bytes.data(),1);using F=uint32(__cdecl*)(FILESTREAM);r["return"]=reinterpret_cast<F>(addresses[fn])(stream);}
    if(fn!=9)r["stream"]=State(stream);
    r["bytes"]=bytes;
    if(fn==9)r["return"]=reinterpret_cast<Close>(addresses[fn])(stream);else reinterpret_cast<Close>(addresses[9])(stream);
   }
   r["disk"]=Disk(file);if(fn==2)r["missing_exists_after"]=std::filesystem::exists(path+".missing");return r;
  };
  // Todos los ganchos juntos: la ruta original usa solo el CRT del EXE y la candidata solo el de la DLL
  const auto setAll=[&](bool on){for(auto* h:hooks)h->State(on);};
  setAll(false);const auto a=FMProbe::calls[fn];const auto original=run(false);const auto b=FMProbe::calls[fn];
  setAll(true);const auto candidate=run(true);const auto c=FMProbe::calls[fn];
  // La ruta candidata llama ademas una vez al Seek auxiliar (fn 7) y al Tell del instantaneo (fn 11); la original no cuenta (ganchos apagados).
  const unsigned extra=fn==7||fn==11?1:0;
  passed=original==candidate&&b==a&&c==b+1+extra&&(fn!=1||(original.value("expect_ok",false)&&candidate.value("expect_ok",false)));
  Write({{"event","comparison"},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_calls",b-a},{"candidate_calls",c-b-extra}});++pairs;
  hooks[fn]->State(enabled[fn]);
 }
 json baseline;
 // Las 64 mezclas de ganchos no son estados coherentes: un FILE* del CRT del EXE (_ptr@0 _cnt@4 _base@8 _flag@0xC _file@0x10, fd en la tabla 0xC9C300) no es un
 // FILE* de la UCRT (_ptr@0 _base@4 _cnt@8 _flags@0xC, fd en su propia tabla, cerrojo embebido en +0x20). Los unicos estados coherentes son 0 (todo EXE) y 63 (todo DLL);
 // se comprueba ademas de quien es el flujo (ExeOwned) para asegurar que los dos lados usan de verdad CRT distintos.
 for(const int mask:std::array{0,63}){ if(!passed)break;
  constexpr std::array indices{2,5,6,7,11,9};for(int bit=0;bit<6;++bit)hooks[indices[bit]]->State((mask&(1<<bit))!=0);
  Fixture(file);auto* stream=reinterpret_cast<Open>(0x538900)(path.c_str(),"r+b");assert(stream);const bool exeOwned=ExeOwned(stream);
  std::array<uint8,32> bytes{};const auto read=reinterpret_cast<Read>(0x538950)(stream,bytes.data(),bytes.size());
  const auto seek=reinterpret_cast<Seek>(0x538990)(stream,7,SEEK_SET);const auto written=reinterpret_cast<WriteFile>(0x538970)(stream,bytes.data(),bytes.size());
  const auto pos=reinterpret_cast<Tell>(0x538A20)(stream);const auto closed=reinterpret_cast<Close>(0x5389D0)(stream);
  json state{{"read",read},{"seek",seek},{"written",written},{"pos",pos},{"closed",closed},{"bytes",bytes},{"disk",Disk(file)}};
  if(!mask)baseline=state;passed=state==baseline&&read==32&&written==32&&closed==0&&exeOwned==(mask==0);Write({{"event","interop"},{"mask",mask},{"owner",exeOwned?"exe":"dll"},{"passed",passed},{"state",state}});
 }
 // Interoperabilidad por disco: ningun FILE* cruza de CRT (los ganchos solo se cambian con todos los ficheros cerrados); se comparten el contenido del fichero y la
 // conversion de texto. 4 combinaciones (escribe EXE|DLL x lee EXE|DLL) deben dar el mismo resultado, con los flujos de cada lado en su CRT.
 {json diskBaseline;
  for(int w=0;w<2&&passed;++w)for(int rd=0;rd<2&&passed;++rd){
   for(auto* h:hooks)h->State(w!=0);
   {std::error_code ec;std::filesystem::remove(file,ec);}
   constexpr char text[]="alfa\nbeta\ngamma\n\n";
   // Modelo: el modo texto expande cada salto de linea a CR LF en disco (17 bytes escritos, 4 saltos -> 21 bytes); GetTotalSize (ftell/fseek en bruto) devuelve el tamano de disco.
   const size_t expectedDisk=(sizeof(text)-1)+size_t(std::count(text,text+sizeof(text)-1,'\n'));
   auto* out=reinterpret_cast<Open>(0x538900)(path.c_str(),"w");assert(out);const bool outExe=ExeOwned(out);
   const auto wrote=reinterpret_cast<WriteFile>(0x538970)(out,text,sizeof(text)-1);const auto closedOut=reinterpret_cast<Close>(0x5389D0)(out);const auto disk=Disk(file);
   for(auto* h:hooks)h->State(rd!=0);
   auto* in=reinterpret_cast<Open>(0x538900)(path.c_str(),"r");assert(in);const bool inExe=ExeOwned(in);
   using Line=bool(__cdecl*)(FILESTREAM,char*,int);using Size=int(__cdecl*)(FILESTREAM);json lines=json::array();char buf[64];
   for(int k=0;k<5;++k){buf[0]=0;const bool ok=reinterpret_cast<Line>(0x5389B0)(in,buf,int(sizeof(buf)));lines.push_back(json{{"ok",ok},{"text",std::string(ok?buf:"")}});}
   const auto total=reinterpret_cast<Size>(0x5389E0)(in);const auto closedIn=reinterpret_cast<Close>(0x5389D0)(in);
   json state{{"wrote",wrote},{"closed_out",closedOut},{"disk",disk},{"lines",lines},{"total",total},{"closed_in",closedIn}};
   if(!w&&!rd)diskBaseline=state;
   passed=state==diskBaseline&&wrote==sizeof(text)-1&&disk.size()==expectedDisk&&size_t(total)==expectedDisk&&closedOut==0&&closedIn==0&&outExe==(w==0)&&inExe==(rd==0);
   Write({{"event","disk_interop"},{"writer",w?"dll":"exe"},{"reader",rd?"dll":"exe"},{"owner_out",outExe?"exe":"dll"},{"owner_in",inExe?"exe":"dll"},{"passed",passed},{"state",state}});
  }
 }
 if(passed){
  const auto unicode=folder/std::filesystem::path(L"prueba-\u00f1-\u4e2d.txt");const auto utf8=UnicodeToUTF8(unicode.wstring());auto* f=CFileMgr::OpenFile(utf8.c_str(),"w+b");
  const std::array<uint8,4> expected{0,10,128,255};std::array<uint8,4> got{};bool unicodePass=f!=nullptr;
  if(f){unicodePass=reinterpret_cast<WriteFile>(0x538970)(f,expected.data(),4)==4&&!reinterpret_cast<Seek>(0x538990)(f,0,SEEK_SET)&&reinterpret_cast<Read>(0x538950)(f,got.data(),4)==4&&got==expected;reinterpret_cast<Close>(0x5389D0)(f);}
  using LoadF=size_t(__cdecl*)(const char*,uint8*,size_t,const char*);
  // Como el EXE (validado par a par): LoadFile ignora `size` (aqui 16), lee el fichero entero en bloques de 0x4000, escribe un 0 justo despues y no toca nada mas.
  // Se comprueba con los ganchos activados (DLL) y apagados (EXE), con ficheros de 0, 256, 0x3FFF, 0x4000, 0x4001 y 0x8000 bytes y con uno ausente (devuelve -1).
  const auto bounded=[&](bool on){
   for(auto* h:hooks)h->State(on);json cases=json::array();bool all=true;
   for(const size_t sz:std::array<size_t,6>{0,256,0x3FFF,0x4000,0x4001,0x8000}){
    Fixture(file,sz);const auto raw=Raw(file);std::vector<uint8> buf(sz+0x4000+64,0xA5);
    const uint32 got=uint32(reinterpret_cast<LoadF>(0x538890)(path.c_str(),buf.data(),16,"rb"));
    const bool ok=got==sz&&buf[sz]==0&&std::equal(raw.begin(),raw.end(),buf.begin())&&std::all_of(buf.begin()+sz+1,buf.end(),[](uint8 b){return b==0xA5;});
    cases.push_back(json{{"size",sz},{"n",got},{"terminator",int(buf[sz])},{"ok",ok}});all=all&&ok;
   }
   {std::error_code ec;std::filesystem::remove(path+".missing",ec);}
   std::vector<uint8> buf(64,0xA5);const uint32 gone=uint32(reinterpret_cast<LoadF>(0x538890)((path+".missing").c_str(),buf.data(),16,"rb"));
   const bool goneOk=gone==0xFFFFFFFFu&&std::all_of(buf.begin(),buf.end(),[](uint8 b){return b==0xA5;})&&!std::filesystem::exists(path+".missing");all=all&&goneOk;
   return json{{"hooks",on},{"passed",all},{"cases",cases},{"absent_n",gone},{"absent_ok",goneOk}};
  };
  const auto viaDll=bounded(true);const auto viaExe=bounded(false);
  const bool boundedPass=viaDll["passed"].get<bool>()&&viaExe["passed"].get<bool>();
  Fixture(file);std::vector<uint8> direct(256+0x4000+64,0xA5);const auto directN=uint32(CFileMgr::LoadFile(path.c_str(),direct.data(),16,"rb"));
  passed=unicodePass&&boundedPass;Write({{"event","extensions"},{"unicode",unicodePass},{"bounded_load",boundedPass},{"bounded_dll",viaDll},{"bounded_exe",viaExe},{"bounded_direct_n",directN},{"bounded_n",viaDll["cases"][1]["n"]},{"bounded_disk",Disk(file).size()},{"bounded_exists",std::filesystem::exists(file)},{"bounded_path",path},{"passed",passed}});
 }
 for(int i=0;i<13;++i){hooks[i]->State(enabled[i]);hooks[i]->LockState(locked[i]);}
 std::filesystem::current_path(cwd);std::memcpy(CFileMgr::ms_dirName,dirBefore.data(),128);std::memcpy(reinterpret_cast<void*>(0xC92368),userBefore.data(),256);
 const bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&std::filesystem::current_path()==cwd;Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});
}
}
