from pathlib import Path
p=Path('reversing/VehicleAudio355Evidence/Train_CtorNative.txt');s=p.read_text().replace('GetVehiclePointer(model->m_nHandlingId)','GetVehiclePointer(uint8(model->m_nHandlingId))');p.write_text(s)
p=Path('reversing/TrainCtor355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeRatios355/LiveComparison.hpp').read_text().replace('BikeRatios355Live','TrainCtor355Live').replace('bike-ratios355','train-ctor355')
a=s.index('#include "Bike.h"');b=s.index('#include "../ProbeRuntime',a);s=s[:a]+'''#include "Train.h"
#include "AEVehicleAudioEntity.h"
#include "ClumpModelInfo.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
'''+s[b:]
a=s.index('inline int scenario');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline int scenario{},draw{};alignas(16) inline std::array<uint8,0x80> clump{};inline void* activeSelf{};
inline uintptr_t moduleBase{},moduleEnd{};
inline void InitModule(){HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&module);moduleBase=uintptr_t(module);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(moduleBase+reinterpret_cast<IMAGE_DOS_HEADER*>(moduleBase)->e_lfanew);moduleEnd=moduleBase+nt->OptionalHeader.SizeOfImage;}
inline bool InDll(uintptr_t address){return address>=moduleBase&&address<moduleEnd;}
inline uintptr_t Follow(uintptr_t address){for(int i=0;i<8&&InDll(address)&&*reinterpret_cast<uint8*>(address)==0xE9;++i){const auto target=address+5+*reinterpret_cast<int32*>(address+1);if(!InDll(target))break;address=target;}return address;}
inline uintptr_t FirstCall(uintptr_t address){address=Follow(address);if(!InDll(address))return 0;for(uintptr_t p=address;p<address+0x300;++p){if(*reinterpret_cast<uint8*>(p)!=0xE8)continue;const auto target=Follow(p+5+*reinterpret_cast<int32*>(p+1));if(!InDll(target))continue;auto* bytes=reinterpret_cast<uint8*>(target);if(bytes[0]==0x8B&&bytes[1]==0xFF)bytes+=2;if(bytes[0]==0x55&&bytes[1]==0x8B&&bytes[2]==0xEC)return target;}return 0;}
inline void* __fastcall Base(void* self,void*,eVehicleCreatedBy by){trace.push_back({"base",self==activeSelf,int(by)});memset(self,0xCD,0x5A0);*reinterpret_cast<uint32*>(static_cast<uint8*>(self)+0x1C)=uint32(scenario)*0x01020304u;*reinterpret_cast<uint32*>(static_cast<uint8*>(self)+0x40)=uint32(scenario)*0x12345u;*reinterpret_cast<uintptr_t*>(static_cast<uint8*>(self)+0x18)=uintptr_t(clump.data());return self;}
inline void __fastcall Model(CVehicle* self,void*,uint32 id){trace.push_back({"model",id,Hash(reinterpret_cast<uint8*>(self)+0x5D8,0x90),int(self->m_nVehicleType),int(self->m_nVehicleSubType)});self->m_nModelIndex=int16(id);}
inline void __cdecl Frames(RpClump* actual,RwFrame** frames){trace.push_back({"frames",actual==reinterpret_cast<RpClump*>(clump.data()),Hash(frames,17*sizeof(RwFrame*))});for(int i=0;i<17;++i)frames[i]=reinterpret_cast<RwFrame*>(uintptr_t(0x1000+i*16));}
inline int32 __cdecl Random(){const auto r=scenario*43+draw++*17;trace.push_back({"random",r});return r;}
inline void __fastcall Colour(void*,void*,uint8& a,uint8& b,uint8& c,uint8& d,int32 shift){trace.push_back({"colour",a,b,c,d,shift});a=1;b=2;c=3;d=4;}
inline void __fastcall Audio(void*,void*,CEntity* entity){trace.push_back({"audio",entity==activeSelf,Hash(reinterpret_cast<uint8*>(entity)+4,sizeof(CTrain)-4)});}
'''+s[b:]
s=s.replace('{"CBike","DoBurstAndSoftGroundRatios",0,0x871360,52,1024}', '{"CTrain","Constructor",0x6F6030,0,-1,1024}')
a=s.index(' auto& modelSlot');b=s.index(' {\n using ReversingProbe',a)
s=s[:a]+''' auto& modelSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;auto& streakSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+570*4);const auto oldStreak=streakSlot;std::array<uint8,0x380> savedHandling;memcpy(savedHandling.data(),reinterpret_cast<void*>(0xC2B9DC),savedHandling.size());const auto oldTime=StaticRef<uint32>(0xB7CB84);uint32 pairs=0,totalFailures=0;
 auto* baseHook=FindHook("CVehicle","Constructor");if(!baseHook){Write({{"event","error"},{"reason","missing CVehicle constructor"}});return;}
 const auto baseEnabled=baseHook->Hooked(),baseLocked=baseHook->Locked();baseHook->LockState(false);baseHook->State(true);hooks[0]->State(true);InitModule();
 const auto baseWrapper=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(baseHook)->GetHookOurAddress());
 const auto trainWrapper=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(hooks[0])->GetHookOurAddress());
 const auto trainBody=FirstCall(trainWrapper),baseBody=FirstCall(baseWrapper);const auto baseFromTrain=FirstCall(trainBody);
 if(!baseBody||baseBody!=baseFromTrain){baseHook->State(baseEnabled);baseHook->LockState(baseLocked);hooks[0]->State(enabled[0]);hooks[0]->LockState(locked[0]);Write({{"event","error"},{"reason","DLL base constructor resolution mismatch"}});return;}
'''+s[b:]
a=s.index(' EntryPatch a(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch a(0x6D5F10,Base),ad(baseBody,Base);
 const auto modelBody=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindHook("CVehicle","SetModelIndex"))->GetHookOurAddress());
 EntryPatch b(0x6D6A40,Model),bd(modelBody,Model);
 EntryPatch c(0x4C5440,Frames),cd(bit_cast<uintptr_t>(&CClumpModelInfo::FillFrameArray),Frames);
 EntryPatch d(0x821B1E,Random),dd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch e(0x4C8500,Colour),ed(bit_cast<uintptr_t>(&CVehicleModelInfo::ChooseVehicleColour),Colour);
 EntryPatch f(0x4F7670,Audio),fd(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::Initialise),Audio);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CTrain)> object{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> mi{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(mi.data(),0,mi.size());memset(clump.data(),0,clump.size());clump[0]=2;activeSelf=object.data();
    auto* model=reinterpret_cast<CVehicleModelInfo*>(mi.data());modelSlot=model;streakSlot=model;model->m_nHandlingId=uint16(((i/4)%4)*0x100+i%4);
    memcpy(reinterpret_cast<void*>(0xC2B9DC),savedHandling.data(),savedHandling.size());auto* handling=reinterpret_cast<tHandlingData*>(0xC2B9DC+i%4*0xE0);handling->m_fMass=float(i%11+1)*100.f;handling->m_fTurnMass=float(i%17+1)*90.f;handling->m_vecCentreOfMass=CVector{.1f,.2f,.3f};handling->m_fBuoyancyConstant=11.f;handling->m_nHandlingFlags=static_cast<eVehicleHandlingFlags>(uint32(i)*0x10001u);handling->m_fDragMult=std::array<float,5>{-.1f,.01f,.02f,10.f,std::bit_cast<float>(0x7FC00000u)}[(i/16)%5];StaticRef<uint32>(0xB7CB84)=uint32(i)*0x1020304u;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto returned=reinterpret_cast<void*(__thiscall*)(void*,int32,eVehicleCreatedBy)>(fn.address)(object.data(),i%2?570:400,static_cast<eVehicleCreatedBy>((i/2)%4));
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return_self",returned==object.data()},{"object",Hash(object.data()+4,object.size()-4)},{"handling",Hash(reinterpret_cast<void*>(0xC2B9DC),savedHandling.size())},{"trace",trace}};
'''+s[b:]
a=s.index(' modelSlot=oldModel;');b=s.index(' for(size_t n=',a)
s=s[:a]+''' modelSlot=oldModel;streakSlot=oldStreak;memcpy(reinterpret_cast<void*>(0xC2B9DC),savedHandling.data(),savedHandling.size());StaticRef<uint32>(0xB7CB84)=oldTime;
 }
 baseHook->State(baseEnabled);baseHook->LockState(baseLocked);bool restored=modelSlot==oldModel&&streakSlot==oldStreak;
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/TrainRead355',fn).read_text().replace('train-read355','train-ctor355').replace('TrainRead355Live','TrainCtor355Live')
 if fn=='build_probe.py':s=s.replace("'TrainNode.cpp'", "'TrainNode.cpp','Entity/Vehicle/Vehicle.cpp','Entity/Vehicle/Vehicle.h','Models/VehicleModelInfo.cpp','Models/VehicleModelInfo.h','Models/ClumpModelInfo.cpp','Models/ClumpModelInfo.h','Audio/Entities/AEVehicleAudioEntity.cpp','Audio/Entities/AEVehicleAudioEntity.h','cHandlingDataMgr.cpp','cHandlingDataMgr.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('Constructor0x6F6030:1024 pares preparados. BaseCtor DLL resuelto en runtime con helper probado Boat23; confirma target coincide desde wrapperCVehicle y actualCTrainbody antes patch. BaseCtor/SetModelIndex/FillFrames/Colour/Audio/RNG paired aislados; fakeModelslots400/570,4handling blocks snapshot contiguo,clock. Fullobjectmenosvptr y traces intermedios puertas/audio. HandlingID lowbyte, dragNaN originalcopy, doors exactbits, passengers RNG order, trainflags preserved masks. Constructor pending aplicado, hookfalse. Si resolver no encuentra target falla seguro sin llamar constructorreal.\n')
print('TrainCtor355 bank prepared')
