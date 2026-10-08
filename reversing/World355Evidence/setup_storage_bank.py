from pathlib import Path
p=Path('reversing/StorageCore355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/ProjectileCore355',name).read_text(encoding='utf-8-sig').replace('ProjectileCore355','StorageCore355').replace('projectile-core355','storage-core355')
 if name=='build_probe.py':s=s.replace("'source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h'","'source/game_sa/GenericGameStorage.cpp', 'source/game_sa/GenericGameStorage.h', 'source/game_sa/C_PcSave.h', 'source/game_sa/KeyGen.cpp'")
 (p/name).write_text(s,encoding='utf8')
s=Path('reversing/DecisionPending/LiveComparison.hpp').read_text().replace('DecisionPendingLive','StorageCore355Live').replace('decision-pending','storage-core355').replace('#include "DecisionMakers/Decision.h"','#include "GenericGameStorage.h"\n#include "C_PcSave.h"\n#include "FileMgr.h"')
start=s.index('inline const std::array<Fn,2>');end=s.index('inline ReversibleHooks',start)
names=[('GetCurrentVersionNumber',0x5D0F50),('MakeValidSaveName',0x5D0E90),('CloseFile',0x5D0E30),('OpenFileForWriting',0x5D0DD0),('OpenFileForReading',0x5D0D20),('GetSavedGameDateAndTime',0x618D00),('InitRadioStationPositionList',0x618E70),('RestoreForStartLoad',0x619000)]
s=s[:start]+'inline const std::array<Fn,8> fns{{\n'+''.join('{"CGenericGameStorage","%s",0x%X,0,-1,512},\n'%(n,a) for n,a in names)+'}};\n'+s[end:]
s=s.replace('inline bool done{};', '''alignas(16) inline std::array<uint8,51201> buffer{};
inline uint32 scenario{},newCalls{},deleteCalls{},requested{};
inline json trace;
inline FILE* __cdecl Open(const char* path,const char* mode){trace.push_back({"open",path,mode});return scenario%3==0?nullptr:reinterpret_cast<FILE*>(0x123400);}
inline int32 __cdecl Close(FILE* file){trace.push_back({"close",uintptr_t(file)});return int32(scenario%3)-1;}
inline int32 __cdecl Total(FILE* file){trace.push_back({"size",uintptr_t(file)});return 100+int32(scenario)*13;}
inline void* __cdecl NewArray(size_t n){++newCalls;requested=uint32(n);return buffer.data();}
inline void __cdecl DeleteArray(void*){++deleteCalls;}
inline bool done{};''').replace('inline bool done{}; inline std::ofstream log; inline json trace;','inline bool done{}; inline std::ofstream log;')
s=s.replace(' bool passed=true;uint32 pairCount=0;',''' bool passed=true;uint32 pairCount=0;
 std::array<uint8,0xC17038-0xC15F60> saved{};memcpy(saved.data(),reinterpret_cast<void*>(0xC15F60),saved.size());const auto savedWorkSize=CGenericGameStorage::ms_WorkBufferSize;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x538900,Open),ad(bit_cast<uintptr_t>(&CFileMgr::OpenFile),Open);
 EntryPatch b(0x5389D0,Close),bd(bit_cast<uintptr_t>(&CFileMgr::CloseFile),Close);
 EntryPatch c(0x5389E0,Total),cd(bit_cast<uintptr_t>(&CFileMgr::GetTotalSize),Total);''')
start=s.index('    alignas(CDecision)');end=s.index('\n   };',start)
s=s[:start]+'''    scenario=uint32(i);newCalls=deleteCalls=requested=0;trace=json::array();
    memcpy(reinterpret_cast<void*>(0xC15F60),saved.data(),saved.size());CGenericGameStorage::ms_WorkBufferSize=savedWorkSize;
    memset(CGenericGameStorage::ms_LoadFileName,0xCC,sizeof(CGenericGameStorage::ms_LoadFileName));memset(CGenericGameStorage::ms_LoadFileNameWithPath,0xCC,sizeof(CGenericGameStorage::ms_LoadFileNameWithPath));memset(CGenericGameStorage::ms_SaveFileName,0xCC,sizeof(CGenericGameStorage::ms_SaveFileName));
    std::strcpy(C_PcSave::DefaultPCSaveFileName,i%4==0?"fixture?\\n?after":"fixture?path");std::strcpy(CGenericGameStorage::ms_SaveFileName,"fixture_save");std::strcpy(CGenericGameStorage::ms_LoadFileName,"fixture_load");
    CGenericGameStorage::ms_WorkBuffer=i%2?buffer.data():nullptr;CGenericGameStorage::ms_FileHandle=reinterpret_cast<FILE*>(0x765400);CGenericGameStorage::ms_WorkBufferPos=17;CGenericGameStorage::ms_FilePos=23;CGenericGameStorage::ms_FileSize=51;CGenericGameStorage::ms_bFailed=i%2!=0;s_PcSaveHelper.error=C_PcSave::eErrorCode::NONE;
    uintptr_t result=0;const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    {
      EntryPatch alloc(0x821195,NewArray),free(0x8213AE,DeleteArray);
      if(fn.address==0x5D0F50)result=reinterpret_cast<uint32(__cdecl*)()>(fn.address)();
      else if(fn.address==0x5D0E90)reinterpret_cast<void(__cdecl*)(int32)>(fn.address)(i%8);
      else if(fn.address==0x5D0E30||fn.address==0x5D0DD0||fn.address==0x619000)result=reinterpret_cast<bool(__cdecl*)()>(fn.address)();
      else if(fn.address==0x5D0D20)result=reinterpret_cast<bool(__cdecl*)(const char*,int32)>(fn.address)(i%3==0?nullptr:"input_file",i%8);
      else if(fn.address==0x618D00)result=uintptr_t(reinterpret_cast<const GxtChar*(__cdecl*)(int32)>(fn.address)(i%8));
      else reinterpret_cast<void(__cdecl*)()>(fn.address)();
    }
    return json{{"load",ReversingProbe::Bytes(CGenericGameStorage::ms_LoadFileName,sizeof(CGenericGameStorage::ms_LoadFileName))},{"loadpath",ReversingProbe::Bytes(CGenericGameStorage::ms_LoadFileNameWithPath,sizeof(CGenericGameStorage::ms_LoadFileNameWithPath))},{"save",ReversingProbe::Bytes(CGenericGameStorage::ms_SaveFileName,sizeof(CGenericGameStorage::ms_SaveFileName))},{"globals",ReversingProbe::Bytes(reinterpret_cast<void*>(0xC16EE8),0x16)},{"size",CGenericGameStorage::ms_WorkBufferSize},{"error",int(s_PcSaveHelper.error)},{"alloc",newCalls},{"free",deleteCalls},{"requested",requested},{"result",result},{"trace",trace},{"switches",switched}};''' +s[end:]
s=s.replace(' Write({{"event",passed?', ' memcpy(reinterpret_cast<void*>(0xC15F60),saved.data(),saved.size());CGenericGameStorage::ms_WorkBufferSize=savedWorkSize;\n Write({{"event",passed?')
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
