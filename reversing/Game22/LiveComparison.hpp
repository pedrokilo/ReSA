#pragma once
// Banco dentro del motor: CGame (22 funciones, lista de 22). Original (hooks de CGame desactivados) frente a candidato
// (hooks activados) con fixtures deterministas. Casi todas las funciones de CGame inicializan o apagan el juego entero:
// se sustituye CADA función llamada (dirección del EXE y símbolo de la DLL) por un stub trazado genérico (thunk
// "push índice / call Común / ret N") que guarda nombre, ECX (thiscall) y argumentos (cadenas por contenido, vectores
// por valor, bool/int8 enmascarados a 8 bits) en un registro fijo sin reservas, y devuelve valores de fixture
// (bools, ciclos del temporizador, slots de TXD, objetos RW falsos, ficheros falsos). Nunca corre un init/shutdown real.
// Process se compara así (todas sus llamadas trazadas) sin reentrar en el fotograma real. Se guardan, se rellenan con
// basura determinista y se restauran en cada ejecución todos los globales que escriben las funciones; el estado
// final de cada rango se compara por hash.
#include <fstream>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Game.h"
#include "UIRenderer.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"
#include "WaterCannons.h"
#include "TheCarGenerators.h"
#include "Radar.h"
#include "Credits.h"
#include "Birds.h"
#include "MovingThings.h"
#include "PlantMgr.h"
#include "Occlusion.h"
#include "InteriorManager_c.h"
#include "WaterCreatureManager_c.h"
#include "MenuManager.h"
#include "FireManager.h"
#include "Fx.h"
#include "BreakManager_c.h"
#include "Shadows.h"
#include "PedType.h"
#include "Ragdoll/BoneNodeManager.h"
#include "Ragdoll/IKChainManager.h"
#include "CreepingFire.h"
#include "Skidmarks.h"
#include "CarCtrl.h"
#include "TagManager.h"
#include "Clouds.h"
#include "StuntJumpManager.h"
#include "CustomCarPlateMgr.h"
#include "TaskSimpleFight.h"
#include "Conversations.h"
#include "EntryExitManager.h"
#include "TempColModels.h"
#include "PedStats.h"
#include "UserDisplay.h"
#include "PostEffects.h"
#include "Rope.h"
#include "Ropes.h"
#include "Glass.h"
#include "TheScripts.h"
#include "LoadingScreen.h"
#include "GridRef.h"
#include "MenuSystem.h"
#include "RealTimeShadowManager.h"
#include "VehicleRecording.h"
#include "ColAccel.h"
#include "app.h"
#include "Garages.h"
#include "TaskSimpleClimb.h"
#include "PedAttractor.h"
#include "MBlur.h"
#include "CustomBuildingRenderer.h"
#include "Hud.h"
#include "CarFXRenderer.h"
#include "ProcObjectMan.h"
#include "InterestingEvents.h"
#include "WindModifiers.h"
#include "GrassRenderer.h"
#include "Cover.h"
#include "TheZones.h"
#include "Text/Text.h"
#include "../ProbeRuntime.hpp"

namespace Game22Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class Fn> uintptr_t A(Fn f){ static_assert(sizeof(Fn)>=sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&f,sizeof(a)); return a; }

// --- tabla de stubs: dirección del EXE, símbolo de la DLL, nombre, convención ('c' cdecl / 't' thiscall),
//     argumentos ('i' dword, 'b' byte, 's' cadena, 'v' CVector*, 'w' RwBBox*, 'x' ignorado), retorno, máscara de funciones
struct Stub { uintptr_t exe, dll; const char* name; char conv; const char* kinds; char ret; uint32 mask; };
inline const std::vector<Stub>& Stubs() {
    static const std::vector<Stub> s{
        {0x406370,A(&CdStreamShutdown),"CdStreamShutdown",'c',"",'0',0x001000u},
        {0x406690,A(&CdStreamRemoveImages),"CdStreamRemoveImages",'c',"",'0',0x100000u},
        {0x406B70,A(&CdStreamInit),"CdStreamInit",'c',"i",'0',0x000080u},
        {0x407D10,A(&CStreaming::DisableCopBikes),"DisableCopBikes",'c',"b",'0',0x020000u},
        {0x407F20,A(&CStreaming::LoadInitialVehicles),"LoadInitialVehicles",'c',"",'0',0x004000u},
        {0x4083C0,A(&CStreaming::InitImageList),"InitImageList",'c',"",'0',0x002000u},
        {0x4084B0,A(&CStreaming::Shutdown),"Streaming::Shutdown",'c',"",'0',0x100000u},
        {0x4093B0,A(&CStreaming::RemoveBigBuildings),"RemoveBigBuildings",'c',"",'0',0x040000u},
        {0x40A120,A(&CStreaming::LoadInitialWeapons),"LoadInitialWeapons",'c',"",'0',0x004000u},
        {0x40B3A0,A(&CStreaming::RemoveInappropriatePedModels),"RemoveInappropriatePedModels",'c',"",'0',0x020000u},
        {0x40D3D0,A(&CStreaming::LoadInitialPeds),"LoadInitialPeds",'c',"",'0',0x004000u},
        {0x40E560,A(&CStreaming::ReInit),"Streaming::ReInit",'c',"",'0',0x080000u},
        {0x40E670,A(&CStreaming::Update),"Streaming::Update",'c',"",'0',0x200000u},
        {0x40EA10,A(&CStreaming::LoadAllRequestedModels),"LoadAllRequestedModels",'c',"b",'0',0x004000u},
        {0x411E20,A(&CCollision::Update),"Collision::Update",'c',"",'0',0x200000u},
        {0x411E30,A(&CCollision::SortOutCollisionAfterLoad),"SortOutCollisionAfterLoad",'c',"",'0',0x040000u},
        {0x416260,A(&CCollision::Init),"Collision::Init",'c',"",'0',0x002000u},
        {0x4162E0,A(&CCollision::Shutdown),"Collision::Shutdown",'c',"",'0',0x100000u},
        {0x41B360,A(&CTempColModels::Shutdown),"TempColModels::Shutdown",'c',"",'0',0x001000u},
        {0x41BC70,A(&CBridge::Init),"Bridge::Init",'c',"",'0',0x004000u},
        {0x41BC80,A(&CBridge::Update),"Bridge::Update",'c',"",'0',0x200000u},
        {0x4212E0,A(&CCarCtrl::Init),"CarCtrl::Init",'c',"",'0',0x002000u},
        {0x4213B0,A(&CCarCtrl::ReInit),"CarCtrl::ReInit",'c',"",'0',0x020000u},
        {0x423F10,A(&CCarCtrl::PruneVehiclesOfInterest),"PruneVehiclesOfInterest",'c',"",'0',0x200000u},
        {0x42CD10,A(&CCarCtrl::RemoveDistantCars),"RemoveDistantCars",'c',"",'0',0x200000u},
        {0x4322B0,A(&CCarCtrl::RemoveCarsIfThePoolGetsFull),"RemoveCarsIfThePoolGetsFull",'c',"",'0',0x200000u},
        {0x4341C0,A(&CCarCtrl::GenerateRandomCars),"GenerateRandomCars",'c',"",'0',0x200000u},
        {0x438450,A(&CCheat::ResetCheats),"ResetCheats",'c',"",'0',0x002000u},
        {0x439AF0,A(&CCheat::DoCheats),"DoCheats",'c',"",'0',0x200000u},
        {0x43A7B0,A(&CConversations::Clear),"Conversations::Clear",'c',"",'0',0x082000u},
        {0x43AAE0,A(&CPedToPlayerConversations::Clear),"PedToPlayerConversations::Clear",'c',"",'0',0x082000u},
        {0x43B0F0,A(&CPedToPlayerConversations::Update),"PedToPlayerConversations::Update",'c',"",'0',0x200000u},
        {0x43C590,A(&CConversations::Update),"Conversations::Update",'c',"",'0',0x200000u},
        {0x43CEB0,A(&CDarkel::Init),"Darkel::Init",'c',"",'0',0x024000u},
        {0x43DAC0,A(&CDarkel::Update),"Darkel::Update",'c',"",'0',0x200000u},
        {0x43F0A0,A(&CEntryExitManager::PostEntryExitsCreation),"PostEntryExitsCreation",'c',"",'0',0x004000u},
        {0x43F880,A(&CEntryExitManager::Init),"EntryExitManager::Init",'c',"",'0',0x002000u},
        {0x440B90,A(&CEntryExitManager::Shutdown),"EntryExitManager::Shutdown",'c',"",'0',0x100000u},
        {0x440C40,A(&CEntryExitManager::ShutdownForRestart),"EntryExitManager::ShutdownForRestart",'c',"",'0',0x080000u},
        {0x440D10,A(&CEntryExitManager::Update),"EntryExitManager::Update",'c',"",'0',0x200000u},
        {0x441210,A(&CGameLogic::InitAtStartOfGame),"GameLogic::InitAtStartOfGame",'c',"",'0',0x022000u},
        {0x441560,A(&CGameLogic::ClearSkip),"GameLogic::ClearSkip",'c',"b",'0',0x080000u},
        {0x442AD0,A(&CGameLogic::Update),"GameLogic::Update",'c',"",'0',0x200000u},
        {0x443920,A(&CGangWars::InitAtStartOfGame),"GangWars::InitAtStartOfGame",'c',"",'0',0x022000u},
        {0x446610,A(&CGangWars::Update),"GangWars::Update",'c',"",'0',0x200000u},
        {0x447120,A(&CGarages::Init),"Garages::Init",'c',"",'0',0x002000u},
        {0x4471B0,A(&CGarages::Shutdown),"Garages::Shutdown",'c',"",'0',0x100000u},
        {0x448B60,A(&CGarages::Init_AfterRestart),"Garages::Init_AfterRestart",'c',"",'0',0x024000u},
        {0x44C8C0,A(&CGarages::Update),"Garages::Update",'c',"",'0',0x200000u},
        {0x44D080,A(&CPathFind::Init),"PathFind::Init",'t',"",'0',0x002000u},
        {0x44E4E0,A(&CPathFind::ReInit),"PathFind::ReInit",'t',"",'0',0x020000u},
        {0x450950,A(&CPathFind::Shutdown),"PathFind::Shutdown",'t',"",'0',0x001000u},
        {0x450A60,A(&CPathFind::UpdateStreaming),"PathFind::UpdateStreaming",'t',"b",'0',0x200000u},
        {0x454A70,A(&CPickups::Init),"Pickups::Init",'c',"",'0',0x002000u},
        {0x458DE0,A(&CPickups::Update),"Pickups::Update",'c',"",'0',0x200000u},
        {0x459390,A(&CVehicleRecording::Init),"VehicleRecording::Init",'c',"",'0',0x022000u},
        {0x459400,A(&CVehicleRecording::ShutDown),"VehicleRecording::ShutDown",'c',"",'0',0x180000u},
        {0x45A1B0,A(&CVehicleRecording::InitAtStartOfGame),"VehicleRecording::InitAtStartOfGame",'c',"",'0',0x000200u},
        {0x45C440,A(&CReplay::ShouldStandardCameraBeProcessed),"ShouldStandardCameraBeProcessed",'c',"",'B',0x200000u},
        {0x45E220,A(&CReplay::Init),"Replay::Init",'c',"",'0',0x004000u},
        {0x45EC90,A(&CReplay::EmptyReplayBuffer),"EmptyReplayBuffer",'c',"",'0',0x180000u},
        {0x45F050,A(&CReplay::FinishPlayback),"FinishPlayback",'c',"",'0',0x180000u},
        {0x460500,A(&CReplay::Update),"Replay::Update",'c',"",'0',0x200000u},
        {0x460630,A(&CRestart::Initialise),"Restart::Initialise",'c',"",'0',0x022000u},
        {0x461100,A(&CRoadBlocks::Init),"RoadBlocks::Init",'c',"",'0',0x024000u},
        {0x4629E0,A(&CRoadBlocks::GenerateRoadBlocks),"GenerateRoadBlocks",'c',"",'0',0x200000u},
        {0x464D40,A(&CTheScripts::StartTestScript),"StartTestScript",'c',"",'0',0x030000u},
        {0x465A40,A(&CTheScripts::RemoveScriptTextureDictionary),"RemoveScriptTextureDictionary",'c',"",'0',0x100000u},
        {0x468D50,A(&CTheScripts::Init),"TheScripts::Init",'c',"",'0',0x024000u},
        {0x46A000,A(&CTheScripts::Process),"TheScripts::Process",'c',"",'0',0x230000u},
        {0x481290,A(&CTheScripts::UndoBuildingSwaps),"UndoBuildingSwaps",'c',"",'0',0x080000u},
        {0x4812D0,A(&CTheScripts::UndoEntityInvisibilitySettings),"UndoEntityInvisibilitySettings",'c',"",'0',0x080000u},
        {0x4994F0,A(&CSetPieces::Init),"SetPieces::Init",'c',"",'0',0x082000u},
        {0x49AA00,A(&CSetPieces::Update),"SetPieces::Update",'c',"",'0',0x200000u},
        {0x49B640,A(&CShopping::ShutdownForRestart),"Shopping::ShutdownForRestart",'c',"",'0',0x080000u},
        {0x49C290,A(&CShopping::Init),"Shopping::Init",'c',"",'0',0x004000u},
        {0x49C490,A(&CStuntJumpManager::Update),"StuntJumpManager::Update",'c',"",'0',0x200000u},
        {0x49CA50,A(&CStuntJumpManager::Init),"StuntJumpManager::Init",'c',"",'0',0x002000u},
        {0x49CB10,A(&CStuntJumpManager::ShutdownForRestart),"StuntJumpManager::ShutdownForRestart",'c',"",'0',0x080000u},
        {0x49CBC0,A(&CStuntJumpManager::Shutdown),"StuntJumpManager::Shutdown",'c',"",'0',0x100000u},
        {0x49CC50,A(&CTagManager::Init),"TagManager::Init",'c',"",'0',0x002000u},
        {0x49CC60,A(&CTagManager::ShutdownForRestart),"TagManager::ShutdownForRestart",'c',"",'0',0x080000u},
        {0x49E640,A(&Fx_c::Update),"Fx::Update",'t',"ii",'0',0x200000u},
        {0x49EA90,A(&Fx_c::Init),"Fx::Init",'t',"",'0',0x002000u},
        {0x49EAE0,A(&Fx_c::Reset),"Fx::Reset",'t',"",'0',0x080000u},
        {0x4A1320,A(&Fx_c::Exit),"Fx::Exit",'t',"",'0',0x100000u},
        {0x4AC2A0,A(&CInformFriendsEventQueue::Flush),"InformFriendsEventQueue::Flush",'c',"",'0',0x100000u},
        {0x4AC410,A(&CInformGroupEventQueue::Flush),"InformGroupEventQueue::Flush",'c',"",'0',0x100000u},
        {0x4B2990,A(&CInformFriendsEventQueue::Init),"InformFriendsEventQueue::Init",'c',"",'0',0x004000u},
        {0x4B2AD0,A(&CInformGroupEventQueue::Init),"InformGroupEventQueue::Init",'c',"",'0',0x004000u},
        {0x4C63E0,A(&CModelInfo::ShutDown),"ModelInfo::ShutDown",'c',"",'0',0x100000u},
        {0x4C6810,A(&CModelInfo::Initialise),"ModelInfo::Initialise",'c',"",'0',0x002000u},
        {0x4C7470,A(&CVehicleModelInfo::ShutdownLightTexture),"ShutdownLightTexture",'c',"",'0',0x100000u},
        {0x4C8770,A(&CVehicleModelInfo::DeleteVehicleColourTextures),"DeleteVehicleColourTextures",'c',"",'0',0x100000u},
        {0x4C87D0,A(&CVehicleModelInfo::ShutdownEnvironmentMaps),"ShutdownEnvironmentMaps",'c',"",'0',0x100000u},
        {0x4D4130,A(&CAnimManager::Shutdown),"AnimManager::Shutdown",'c',"",'0',0x100000u},
        {0x4D5620,A(&CAnimManager::LoadAnimFiles),"LoadAnimFiles",'c',"",'0',0x004000u},
        {0x4D5A20,A(&CCutsceneMgr::Initialise),"CutsceneMgr::Initialise",'c',"",'0',0x002000u},
        {0x4D5D00,A(&CCutsceneMgr::Update),"CutsceneMgr::Update",'c',"",'0',0x200000u},
        {0x4D5E60,A(&CCutsceneMgr::Shutdown),"CutsceneMgr::Shutdown",'c',"",'0',0x100000u},
        {0x506DA0,A(&CAudioEngine::ResetStatistics),"AudioEngine::ResetStatistics",'t',"",'0',0x020000u},
        {0x506DB0,A(&CAudioEngine::Restart),"AudioEngine::Restart",'t',"",'0',0x040000u},
        {0x506DE0,A(&CAudioEngine::SetMusicMasterVolume),"SetMusicMasterVolume",'t',"b",'0',0x040000u},
        {0x506E10,A(&CAudioEngine::SetEffectsMasterVolume),"SetEffectsMasterVolume",'t',"b",'0',0x040000u},
        {0x506F80,A(&CAudioEngine::SetRadioAutoRetuneOnOff),"SetRadioAutoRetuneOnOff",'t',"b",'0',0x040000u},
        {0x506F90,A(&CAudioEngine::SetBassEnhanceOnOff),"SetBassEnhanceOnOff",'t',"b",'0',0x040000u},
        {0x506FC0,A(&CAudioEngine::InitialiseRadioStationID),"InitialiseRadioStationID",'t',"b",'0',0x040000u},
        {0x507C30,A(&CAudioEngine::ResetSoundEffects),"ResetSoundEffects",'t',"",'0',0x040000u},
        {0x5081A0,A(&CAudioZones::Init),"AudioZones::Init",'c',"",'0',0x002000u},
        {0x5083C0,A(&CAudioZones::Update),"AudioZones::Update",'c',"biii",'0',0x200000u},
        {0x50B5D0,A(&CCamera::ProcessFade),"Camera::ProcessFade",'t',"",'0',0x200000u},
        {0x50BF00,A(&CCamera::SetFadeColour),"Camera::SetFadeColour",'t',"bbb",'0',0x040000u},
        {0x50C100,A(&CCamera::SetRwCamera),"Camera::SetRwCamera",'t',"i",'0',0x020400u},
        {0x52B730,A(&CCamera::Process),"Camera::Process",'t',"",'0',0x230000u},
        {0x52CD90,A(&CClock::Initialise),"Clock::Initialise",'c',"i",'0',0x024000u},
        {0x52CF10,A(&CClock::Update),"Clock::Update",'c',"",'0',0x200000u},
        {0x532240,A(&CDebug::DebugInitTextBuffer),"DebugInitTextBuffer",'c',"",'0',0x002000u},
        {0x5386F0,A(&CFileMgr::Initialise),"FileMgr::Initialise",'c',"",'0',0x000080u},
        {0x538900,A(&CFileMgr::OpenFile),"OpenFile",'c',"ss",'O',0x000004u},
        {0x538950,A(&CFileMgr::Read),"Read",'c',"ixi",'r',0x000004u},
        {0x538990,A(&CFileMgr::Seek),"Seek",'c',"iii",'1',0x000004u},
        {0x5389D0,A(&CFileMgr::CloseFile),"CloseFile",'c',"i",'0',0x000004u},
        {0x538BC0,A(&CFireManager::Init),"FireManager::Init",'t',"",'0',0x020000u},
        {0x539CE0,A(&CCreepingFire::Update),"CreepingFire::Update",'c',"",'0',0x200000u},
        {0x539DD0,A(&CFireManager::Shutdown),"FireManager::Shutdown",'t',"",'0',0x080000u},
        {0x53AF00,A(&CFireManager::Update),"FireManager::Update",'t',"",'0',0x200000u},
        {0x53BCF0,A(&CGame::ReInitGameObjectVariables),"Game::ReInitGameObjectVariables",'c',"",'0',0x040000u},
        {0x53C550,A(&CGame::ShutDownForRestart),"Game::ShutDownForRestart",'c',"",'0',0x040000u},
        {0x53D030,A(&CLoadMonitor::BeginFrame),"LoadMonitor::BeginFrame",'t',"",'0',0x200000u},
        {0x53D0B0,A(&CLoadMonitor::EndFrame),"LoadMonitor::EndFrame",'t',"",'0',0x200000u},
        {0x53DED0,A(&LoadingScreen),"LoadingScreen",'c',"sss",'0',0x01C004u},
        {0x53ED30,A(&TestModelIndices),"TestModelIndices",'c',"",'0',0x004000u},
        {0x541A70,A(&CPad::Clear),"Pad::Clear",'t',"bb",'0',0x028000u},
        {0x541D90,A(&CPad::Initialise),"Pad::Initialise",'c',"",'0',0x000080u},
        {0x541DD0,A(&CPad::UpdatePads),"Pad::UpdatePads",'c',"",'0',0x200000u},
        {0x54F3A0,A(&CPlaceable::InitMatrixArray),"InitMatrixArray",'c',"",'0',0x002000u},
        {0x550F10,A(&CPools::Initialise),"Pools::Initialise",'c',"",'0',0x002000u},
        {0x5519F0,A(&CPools::ShutDown),"Pools::ShutDown",'c',"",'0',0x100000u},
        {0x552470,0,"cover_flush",'c',"",'0',0x100000u},
        {0,A(&CCover::ClearProcessedBuildings),"cover_flush",'c',"",'0',0x100000u}, /* DLL: ocupa el lugar del Flush del EXE */
        {0x552C00,A(&CQuadTreeNode<void*>::InitPool),"QuadTreeNode::InitPool",'c',"",'0',0x002000u},
        {0x5531C0,A(&CRenderer::Init),"Renderer::Init",'c',"",'0',0x004000u},
        {0x5531D0,A(&CRenderer::Shutdown),"Renderer::Shutdown",'c',"",'0',0x100000u},
        {0x555DC0,A(&CRopes::Init),"Ropes::Init",'c',"",'0',0x024000u},
        {0x556B10,A(&CRopes::Shutdown),"Ropes::Shutdown",'c',"",'0',0x180000u},
        {0x558D70,A(&CRopes::Update),"Ropes::Update",'c',"",'0',0x200000u},
        {0x55C0C0,A(&CStats::Init),"Stats::Init",'c',"",'0',0x024000u},
        {0x55F420,A(&SurfaceInfos_c::Init),"SurfaceInfos::Init",'t',"",'0',0x000200u},
        {0x5601E0,A(&CTimeCycle::Shutdown),"TimeCycle::Shutdown",'c',"",'0',0x100000u},
        {0x5601F0,A(&CTimeCycle::InitForRestart),"TimeCycle::InitForRestart",'c',"",'0',0x040000u},
        {0x561760,A(&CTimeCycle::Update),"TimeCycle::Update",'c',"",'0',0x200000u},
        {0x5617E0,A(&CTimer::Initialise),"Timer::Initialise",'c',"",'0',0x060100u},
        {0x5618C0,A(&CTimer::Shutdown),"Timer::Shutdown",'c',"",'0',0x080000u},
        {0x561A40,A(&CTimer::GetCyclesPerMillisecond),"GetCyclesPerMillisecond",'c',"",'M',0x200000u},
        {0x561A80,A(&CTimer::GetCurrentTimeInCycles),"GetCurrentTimeInCycles",'c',"",'T',0x200000u},
        {0x561AA0,A(&CTimer::Stop),"Timer::Stop",'c',"",'0',0x040000u},
        {0x561B10,A(&CTimer::Update),"Timer::Update",'c',"",'0',0x040000u},
        {0x561C70,A(&CWanted::InitialiseStaticVariables),"Wanted::InitialiseStaticVariables",'c',"",'0',0x020000u},
        {0x562360,A(&CWanted::UpdateEachFrame),"Wanted::UpdateEachFrame",'c',"",'0',0x200000u},
        {0x5631E0,A(&CWorld::Initialise),"World::Initialise",'c',"",'0',0x002000u},
        {0x563280,A(&CWorld::Remove),"World::Remove",'c',"i",'0',0x100000u},
        {0x564050,A(&CWorld::ShutDown),"World::ShutDown",'c',"",'0',0x100000u},
        {0x564360,A(&CWorld::ClearForRestart),"World::ClearForRestart",'c',"",'0',0x080000u},
        {0x5684A0,A(&CWorld::Process),"World::Process",'c',"",'0',0x200000u},
        {0x56D070,A(&CIniFile::LoadIniFile),"LoadIniFile",'c',"",'0',0x002000u},
        {0x56D180,A(&CLocalisation::Initialise),"Localisation::Initialise",'c',"",'0',0x000080u},
        {0x56E210,A(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),"FindPlayerPed",'c',"i",'P',0x080010u},
        {0x56EA80,A(&CPlayerInfo::DeletePlayerSkin),"PlayerInfo::DeletePlayerSkin",'t',"",'0',0x000800u},
        {0x56F330,A(&CPlayerInfo::Clear),"PlayerInfo::Clear",'t',"",'0',0x1A4000u},
        {0x56F7D0,A(&CPlayerInfo::LoadPlayerSkin),"PlayerInfo::LoadPlayerSkin",'t',"",'0',0x004000u},
        {0x5719B0,A(&CReferences::Init),"References::Init",'c',"",'0',0x002000u},
        {0x571EE0,A(&CUserDisplay::Init),"UserDisplay::Init",'c',"",'0',0x022000u},
        {0x5720A0,A(&CUserDisplay::Process),"UserDisplay::Process",'c',"",'0',0x200000u},
        {0x572110,A(&CTheZones::ResetZonesRevealed),"ResetZonesRevealed",'c',"",'0',0x080000u},
        {0x572670,A(&CTheZones::Init),"TheZones::Init",'c',"",'0',0x002000u},
        {0x572B70,A(&CTheZones::PostZoneCreation),"PostZoneCreation",'c',"",'0',0x004000u},
        {0x572D10,A(&CTheZones::Update),"TheZones::Update",'c',"",'0',0x200000u},
        {0x574630,A(&CMenuManager::UnloadTextures),"MenuManager::UnloadTextures",'t',"",'0',0x080000u},
        {0x579330,A(&CMenuManager::MessageScreen),"MenuManager::MessageScreen",'t',"sbb",'0',0x040000u},
        {0x57B440,A(&CMenuManager::Process),"MenuManager::Process",'t',"",'0',0x200000u},
        {0x5822D0,A(&CMenuSystem::Initialise),"MenuSystem::Initialise",'c',"",'0',0x020000u},
        {0x5827D0,A(&CRadar::LoadTextures),"Radar::LoadTextures",'c',"",'0',0x004000u},
        {0x584BF0,A(&CRadar::RemoveRadarSections),"RemoveRadarSections",'c',"",'0',0x080000u},
        {0x585940,A(&CRadar::Shutdown),"Radar::Shutdown",'c',"",'0',0x100000u},
        {0x587FB0,A(&CRadar::Initialise),"Radar::Initialise",'c',"",'0',0x024000u},
        {0x588850,A(&CHud::Shutdown),"Hud::Shutdown",'c',"",'0',0x000800u},
        {0x588880,A(&CHud::ReInitialise),"Hud::ReInitialise",'c',"",'0',0x120000u},
        {0x58FF10,A(&CLoadingScreen::Shutdown),"LoadingScreen::Shutdown",'c',"b",'0',0x000800u},
        {0x598010,A(&InteriorManager_c::Exit),"InteriorManager::Exit",'t',"",'0',0x180000u},
        {0x598F50,A(&InteriorManager_c::Update),"InteriorManager::Update",'t',"",'B',0x200000u},
        {0x59AC90,A(&CMaths::InitMathsTables),"InitMathsTables",'c',"",'0',0x002000u},
        {0x59E650,A(&BreakManager_c::Init),"BreakManager::Init",'t',"",'B',0x000200u},
        {0x59E660,A(&BreakManager_c::Exit),"BreakManager::Exit",'t',"",'0',0x100000u},
        {0x59E670,A(&BreakManager_c::Update),"BreakManager::Update",'t',"i",'0',0x200000u},
        {0x59E720,A(&BreakManager_c::ResetAll),"BreakManager::ResetAll",'t',"",'0',0x080000u},
        {0x5A3110,A(&ProcObjectMan_c::Update),"ProcObjectMan::Update",'t',"",'0',0x200000u},
        {0x5A3EA0,A(&ProcObjectMan_c::Init),"ProcObjectMan::Init",'t',"",'B',0x028000u},
        {0x5A3EE0,A(&ProcObjectMan_c::Exit),"ProcObjectMan::Exit",'t',"",'0',0x180000u},
        {0x5A80D0,A(&CClothes::Init),"Clothes::Init",'c',"",'0',0x004000u},
        {0x5B2AD0,A(&CColAccel::endCache),"ColAccel::endCache",'c',"",'0',0x010000u},
        {0x5B31A0,A(&CColAccel::startCache),"ColAccel::startCache",'c',"",'0',0x010000u},
        {0x5B9030,A(&CFileLoader::LoadLevel),"LoadLevel",'c',"s",'0',0x010000u},
        {0x5B9C60,A(&CAudioEngine::Initialise),"AudioEngine::Initialise",'t',"",'B',0x000200u},
        {0x5BA060,A(&ValidateVersion),"Game::ValidateVersion",'c',"",'0',0x000400u},
        {0x5BA1A0,A(&CGame::Init2),"Game::Init2",'c',"s",'B',0x010000u},
        {0x5BA400,A(&CGame::Init3),"Game::Init3",'c',"s",'B',0x010000u},
        {0x5BA520,A(&LightsCreate),"LightsCreate",'c',"i",'0',0x000400u},
        {0x5BA690,A(&CFont::Initialise),"Font::Initialise",'c',"",'0',0x000400u},
        {0x5BA850,A(&CHud::Initialise),"Hud::Initialise",'c',"",'0',0x000400u},
        {0x5BA8C0,A(&CTempColModels::Initialise),"TempColModels::Initialise",'c',"",'0',0x000200u},
        {0x5BBAC0,A(&CTimeCycle::Initialise),"TimeCycle::Initialise",'c',"b",'0',0x020200u},
        {0x5BC090,A(&CPopCycle::Initialise),"PopCycle::Initialise",'c',"",'0',0x020200u},
        {0x5BC520,A(&CCamera::Init),"Camera::Init",'t',"",'0',0x020400u},
        {0x5BCA10,A(&InitModelIndices),"InitModelIndices",'c',"",'0',0x002000u},
        {0x5BCCD0,A(&CStreaming::ReadIniFile),"Streaming::ReadIniFile",'c',"",'0',0x002000u},
        {0x5BD830,A(&cHandlingDataMgr::LoadHandlingData),"LoadHandlingData",'t',"",'0',0x000200u},
        {0x5BEDC0,A(&CTaskSimpleFight::LoadMeleeData),"LoadMeleeData",'c',"",'0',0x002000u},
        {0x5BF6B0,A(&CAnimManager::Initialise),"AnimManager::Initialise",'c',"",'0',0x002000u},
        {0x5BF840,A(&CGame::Init1),"Game::Init1",'c',"s",'B',0x010000u},
        {0x5BF9D0,A(&CPedStats::Initialise),"PedStats::Initialise",'c',"",'0',0x000200u},
        {0x5C0500,A(&InteriorManager_c::Init),"InteriorManager::Init",'t',"",'B',0x028000u},
        {0x5D17B0,A(&CGenericGameStorage::GenericLoad),"GenericLoad",'c',"x",'L',0x040000u},
        {0x5D5AC0,A(&CCarFXRenderer::Initialise),"CarFXRenderer::Initialise",'c',"",'B',0x000100u},
        {0x5D5AD0,A(&CCarFXRenderer::Shutdown),"CarFXRenderer::Shutdown",'c',"",'0',0x100000u},
        {0x5D7EC0,A(&CCustomBuildingRenderer::Initialise),"CustomBuildingRenderer::Initialise",'c',"",'B',0x000100u},
        {0x5D7EE0,A(&CCustomBuildingRenderer::Shutdown),"CustomBuildingRenderer::Shutdown",'c',"",'0',0x100000u},
        {0x5D8050,A(&CCustomBuildingRenderer::Update),"CustomBuildingRenderer::Update",'c',"",'0',0x200000u},
        {0x5DABA0,A(&CGrassRenderer::Shutdown),"GrassRenderer::Shutdown",'c',"",'0',0x100000u},
        {0x5DB940,A(&CPlantMgr::Shutdown),"PlantMgr::Shutdown",'c',"",'0',0x100000u},
        {0x5DCFA0,A(&CPlantMgr::Update),"PlantMgr::Update",'c',"v",'B',0x200000u},
        {0x5DD6B0,A(&CGrassRenderer::Initialise),"GrassRenderer::Initialise",'c',"",'B',0x000100u},
        {0x5DD910,A(&CPlantMgr::Initialise),"PlantMgr::Initialise",'c',"",'B',0x002000u},
        {0x5DE680,A(&CGangs::Initialise),"Gangs::Initialise",'c',"",'0',0x024000u},
        {0x5DEBB0,A(&CPed::Initialise),"Ped::Initialise",'c',"",'0',0x024000u},
        {0x5FB8C0,A(&CPedGroups::Init),"PedGroups::Init",'c',"",'0',0x004000u},
        {0x5FB930,A(&CPedGroups::CleanUpForShutDown),"PedGroups::CleanUpForShutDown",'c',"",'0',0x100000u},
        {0x605A30,A(&CInterestingEvents::ScanForNearbyEntities),"ScanForNearbyEntities",'t',"",'0',0x200000u},
        {0x607D00,A(&CDecisionMakerTypesFileLoader::ReStart),"DecisionMakerTypesFileLoader::ReStart",'c',"",'0',0x020000u},
        {0x608850,A(&CPedStats::Shutdown),"PedStats::Shutdown",'c',"",'0',0x001000u},
        {0x608B00,A(&CPedType::Shutdown),"PedType::Shutdown",'c',"",'0',0x180000u},
        {0x610BF0,A(&CPopCycle::Update),"PopCycle::Update",'c',"",'0',0x200000u},
        {0x610E10,A(&CPopulation::Initialise),"Population::Initialise",'c',"",'0',0x024000u},
        {0x616650,A(&CPopulation::Update),"Population::Update",'c',"b",'0',0x200000u},
        {0x6173F0,A(&BoneNodeManager_c::Init),"BoneNodeManager::Init",'t',"",'B',0x000200u},
        {0x617420,A(&BoneNodeManager_c::Exit),"BoneNodeManager::Exit",'t',"",'0',0x100000u},
        {0x617430,A(&BoneNodeManager_c::Reset),"BoneNodeManager::Reset",'t',"",'0',0x080000u},
        {0x6180A0,A(&IKChainManager_c::Init),"IKChainManager::Init",'t',"",'B',0x000200u},
        {0x6180D0,A(&IKChainManager_c::Exit),"IKChainManager::Exit",'t',"",'0',0x100000u},
        {0x618140,A(&IKChainManager_c::Reset),"IKChainManager::Reset",'t',"",'0',0x080000u},
        {0x618E70,A(&CGenericGameStorage::InitRadioStationPositionList),"InitRadioStationPositionList",'c',"",'0',0x040000u},
        {0x619000,A(&CGenericGameStorage::RestoreForStartLoad),"RestoreForStartLoad",'c',"",'B',0x040000u},
        {0x632DD0,A(&CTaskSequences::CleanUpForShutdown),"TaskSequences::CleanUpForShutdown",'c',"",'0',0x100000u},
        {0x67A250,A(&CTaskSimpleClimb::Shutdown),"TaskSimpleClimb::Shutdown",'c',"",'0',0x100000u},
        {0x698710,A(&CCover::Init),"Cover::Init",'c',"",'0',0x022000u},
        {0x6997E0,A(&CCover::Update),"Cover::Update",'c',"",'0',0x200000u},
        {0x69EDC0,A(&CMessages::ClearAllMessagesDisplayedByGame),"ClearAllMessagesDisplayedByGame",'c',"b",'0',0x002000u},
        {0x69EE00,A(&CMessages::Init),"Messages::Init",'c',"",'0',0x022000u},
        {0x69FF20,A(&CText::Unload),"Text::Unload",'t',"b",'0',0x001000u},
        {0x6A01A0,A(&CText::Load),"Text::Load",'t',"b",'0',0x000100u},
        {0x6C4560,A(&CHeli::InitHelis),"Heli::InitHelis",'c',"",'0',0x024000u},
        {0x6C79A0,A(&CHeli::UpdateHelis),"Heli::UpdateHelis",'c',"",'0',0x200000u},
        {0x6CAD90,A(&CPlane::InitPlaneGenerationAndRemoval),"InitPlaneGenerationAndRemoval",'c',"",'0',0x040000u},
        {0x6D0B40,A(&CVehicle::Shutdown),"Vehicle::Shutdown",'c',"",'0',0x100000u},
        {0x6E3F90,A(&WaterCreatureManager_c::Init),"WaterCreatureManager::Init",'t',"",'B',0x028000u},
        {0x6E3FD0,A(&WaterCreatureManager_c::Exit),"WaterCreatureManager::Exit",'t',"",'0',0x180000u},
        {0x6E4F10,A(&WaterCreatureManager_c::Update),"WaterCreatureManager::Update",'t',"i",'0',0x200000u},
        {0x6E59E0,A(&CWaterLevel::Shutdown),"WaterLevel::Shutdown",'c',"",'0',0x100000u},
        {0x6EAE80,A(&CWaterLevel::WaterLevelInitialise),"WaterLevelInitialise",'c',"",'0',0x004000u},
        {0x6EB710,A(&CWaterLevel::PreRenderWater),"PreRenderWater",'c',"",'0',0x200000u},
        {0x6F3270,A(&CTheCarGenerators::Init),"TheCarGenerators::Init",'c',"",'0',0x022000u},
        {0x6F3F40,A(&CTheCarGenerators::Process),"TheCarGenerators::Process",'c',"",'0',0x200000u},
        {0x6F3FC0,A(&CCranes::InitCranes),"Cranes::InitCranes",'c',"",'0',0x024000u},
        {0x6F3FE0,A(&CCranes::UpdateCranes),"Cranes::UpdateCranes",'c',"",'0',0x200000u},
        {0x6F58D0,A(&CTrain::Shutdown),"Train::Shutdown",'c',"",'0',0x100000u},
        {0x6F5900,A(&CTrain::UpdateTrains),"Train::UpdateTrains",'c',"",'0',0x200000u},
        {0x6F7440,A(&CTrain::InitTrains),"Train::InitTrains",'c',"",'0',0x060000u},
        {0x6FAA70,A(&CCoronas::Init),"Coronas::Init",'c',"",'0',0x004000u},
        {0x6FAB00,A(&CCoronas::Shutdown),"Coronas::Shutdown",'c',"",'0',0x100000u},
        {0x6FADF0,A(&CCoronas::Update),"Coronas::Update",'c',"",'0',0x200000u},
        {0x6FC5A0,A(&CCoronas::DoSunAndMoon),"Coronas::DoSunAndMoon",'c',"",'0',0x200000u},
        {0x6FD500,A(&CCustomCarPlateMgr::Initialise),"CustomCarPlateMgr::Initialise",'c',"",'B',0x004000u},
        {0x6FD720,A(&CCustomCarPlateMgr::Shutdown),"CustomCarPlateMgr::Shutdown",'c',"",'0',0x100000u},
        {0x6FE120,A(&CCustomRoadsignMgr::Initialise),"CustomRoadsignMgr::Initialise",'c',"",'B',0x002000u},
        {0x6FE180,A(&CCustomRoadsignMgr::Shutdown),"CustomRoadsignMgr::Shutdown",'c',"",'0',0x100000u},
        {0x6FF410,A(&CDraw::SetFOV),"Draw::SetFOV",'c',"i",'0',0x024000u},
        {0x6FF8A0,A(&CPlayerSkin::Initialise),"PlayerSkin::Initialise",'c',"",'0',0x000400u},
        {0x6FF8D0,A(&CPlayerSkin::Shutdown),"PlayerSkin::Shutdown",'c',"",'0',0x000800u},
        {0x7010C0,A(&CPostEffects::Close),"PostEffects::Close",'c',"",'0',0x000800u},
        {0x7010F0,A(&CPostEffects::ScriptResetForEffects),"PostEffects::ScriptResetForEffects",'c',"",'0',0x020000u},
        {0x704630,A(&CPostEffects::Initialise),"PostEffects::Initialise",'c',"",'0',0x000400u},
        {0x7046A0,A(&CPostEffects::Update),"PostEffects::Update",'c',"",'0',0x200000u},
        {0x7067C0,A(&CRealTimeShadowManager::Init),"RealTimeShadowManager::Init",'t',"",'0',0x028000u},
        {0x706A60,A(&CRealTimeShadowManager::Exit),"RealTimeShadowManager::Exit",'t',"",'0',0x180000u},
        {0x706CD0,A(&CShadows::Init),"Shadows::Init",'c',"",'0',0x004000u},
        {0x706ED0,A(&CShadows::Shutdown),"Shadows::Shutdown",'c',"",'0',0x100000u},
        {0x70C950,A(&CShadows::UpdatePermanentShadows),"UpdatePermanentShadows",'c',"",'0',0x200000u},
        {0x70F9E0,A(&CStencilShadows::Init),"StencilShadows::Init",'c',"",'0',0x010000u},
        {0x710B50,A(&CStencilShadows::RenderBuffer),"StencilShadows::RenderBuffer",'c',"v",'0',0x200000u},
        {0x711390,A(&CStencilShadows::Shutdown),"StencilShadows::Shutdown",'c',"",'0',0x100000u},
        {0x711D90,A(&CStencilShadows::Process),"StencilShadows::Process",'c',"v",'0',0x200000u},
        {0x711EC0,A(&CBirds::Init),"Birds::Init",'c',"",'0',0x002000u},
        {0x712300,A(&CBirds::Shutdown),"Birds::Shutdown",'c',"",'0',0x100000u},
        {0x712330,A(&CBirds::Update),"Birds::Update",'c',"",'0',0x200000u},
        {0x712FA0,A(&CClouds::Shutdown),"Clouds::Shutdown",'c',"",'0',0x100000u},
        {0x712FF0,A(&CClouds::Update),"Clouds::Update",'c',"",'0',0x200000u},
        {0x7138D0,A(&CClouds::Init),"Clouds::Init",'c',"",'0',0x004000u},
        {0x717C20,A(&CMovingThings::Shutdown),"MovingThings::Shutdown",'c',"",'0',0x180000u},
        {0x717D00,A(&CMovingThings::Init),"MovingThings::Init",'c',"",'0',0x024000u},
        {0x7185B0,A(&CMovingThings::Update),"MovingThings::Update",'c',"",'0',0x200000u},
        {0x7189B0,A(&CFont::Shutdown),"Font::Shutdown",'c',"",'0',0x000800u},
        {0x719800,A(&CFont::InitPerFrame),"Font::InitPerFrame",'c',"",'0',0x200000u},
        {0x71A8D0,A(&CGlass::Init),"Glass::Init",'c',"",'0',0x024000u},
        {0x71B0D0,A(&CGlass::Update),"Glass::Update",'c',"",'0',0x200000u},
        {0x71D4E0,A(&CGridRef::Init),"GridRef::Init",'c',"",'0',0x004000u},
        {0x71D720,A(&CMBlur::MotionBlurClose),"MotionBlurClose",'c',"",'1',0x100000u},
        {0x71DCA0,A(&COcclusion::Init),"Occlusion::Init",'c',"",'0',0x002000u},
        {0x7204E0,A(&CSkidmarks::Init),"Skidmarks::Init",'c',"",'0',0x004000u},
        {0x720570,A(&CSkidmarks::Shutdown),"Skidmarks::Shutdown",'c',"",'0',0x100000u},
        {0x720590,A(&CSkidmarks::Clear),"Skidmarks::Clear",'c',"",'0',0x020000u},
        {0x7205C0,A(&CSkidmarks::Update),"Skidmarks::Update",'c',"",'0',0x200000u},
        {0x723390,A(&CSpecialFX::Shutdown),"SpecialFX::Shutdown",'c',"",'0',0x180000u},
        {0x7268F0,A(&CSpecialFX::Init),"SpecialFX::Init",'c',"",'0',0x024000u},
        {0x726AA0,A(&CSpecialFX::Update),"SpecialFX::Update",'c',"",'0',0x200000u},
        {0x727260,A(&CSprite2d::SetRecipNearClip),"Sprite2d::SetRecipNearClip",'c',"",'0',0x200000u},
        {0x727350,A(&CSprite2d::InitPerFrame),"Sprite2d::InitPerFrame",'c',"",'0',0x200000u},
        {0x728C80,A(&CWaterCannons::Init),"WaterCannons::Init",'c',"",'0',0x024000u},
        {0x72A3C0,A(&CWaterCannons::Update),"WaterCannons::Update",'c',"",'0',0x200000u},
        {0x72A480,A(&CWeather::Init),"Weather::Init",'c',"",'0',0x022000u},
        {0x72B850,A(&CWeather::Update),"Weather::Update",'c',"",'0',0x200000u},
        {0x72D6B0,A(&CCullZones::Init),"CullZones::Init",'c',"",'0',0x002000u},
        {0x72DEC0,A(&CCullZones::Update),"CullZones::Update",'c',"",'0',0x200000u},
        {0x72F3B0,A(&CMemoryMgr::Init),"MemoryMgr::Init",'c',"",'0',0x000080u},
        {0x72FC70,A(&CameraSize),"CameraSize",'c',"iiii",'0',0x000400u},
        {0x72FD90,A(&CameraDestroy),"Game::CameraDestroy",'c',"i",'0',0x000C00u},
        {0x7305E0,A(&D3DResourceSystem::TidyUpD3DTextures),"TidyUpD3DTextures",'c',"i",'0',0x000030u},
        {0x730740,A(&D3DResourceSystem::TidyUpD3DIndexBuffers),"TidyUpD3DIndexBuffers",'c',"i",'0',0x000030u},
        {0x730830,A(&D3DResourceSystem::Init),"D3DResourceSystem::Init",'c',"",'0',0x000400u},
        {0x730900,A(&D3DResourceSystem::CancelBuffering),"D3DResourceSystem::CancelBuffering",'c',"",'0',0x000800u},
        {0x730A00,A(&D3DResourceSystem::Shutdown),"D3DResourceSystem::Shutdown",'c',"",'0',0x100000u},
        {0x730AC0,A(&D3DResourceSystem::SetUseD3DResourceBuffering),"SetUseD3DResourceBuffering",'c',"b",'0',0x1CA000u},
        {0x731850,A(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),"FindTxdSlot",'c',"s",'F',0x100000u},
        {0x731990,A(&CTxdStore::Create),"TxdStore::Create",'c',"i",'0',0x002000u},
        {0x7319C0,A(&CTxdStore::SetCurrentTxd),"TxdStore::SetCurrentTxd",'c',"i",'0',0x002000u},
        {0x731A00,A(&CTxdStore::AddRef),"TxdStore::AddRef",'c',"i",'0',0x002000u},
        {0x731C80,A(&CTxdStore::AddTxdSlot),"TxdStore::AddTxdSlot",'c',"s",'A',0x002000u},
        {0x731CD0,A(&CTxdStore::RemoveTxdSlot),"TxdStore::RemoveTxdSlot",'c',"i",'0',0x100000u},
        {0x731F20,A(&CTxdStore::Initialise),"TxdStore::Initialise",'c',"",'0',0x000400u},
        {0x732000,A(&CTxdStore::Shutdown),"TxdStore::Shutdown",'c',"",'0',0x001000u},
        {0x732060,A(&CTxdStore::GameShutdown),"TxdStore::GameShutdown",'c',"",'0',0x100000u},
        {0x7320B0,A(static_cast<bool(*)(int32,const char*)>(&CTxdStore::LoadTxd)),"TxdStore::LoadTxd",'c',"is",'1',0x002000u},
        {0x732EB0,A(&CVisibilityPlugins::Shutdown),"VisibilityPlugins::Shutdown",'c',"",'0',0x000800u},
        {0x733A20,A(&CVisibilityPlugins::Initialise),"VisibilityPlugins::Initialise",'c',"",'0',0x000400u},
        {0x734610,A(&CreateDebugFont),"CreateDebugFont",'c',"",'0',0x000400u},
        {0x734620,A(&DestroyDebugFont),"DestroyDebugFont",'c',"",'0',0x000800u},
        {0x735730,A(&LightsDestroy),"LightsDestroy",'c',"i",'0',0x000800u},
        {0x73A300,A(&CWeapon::InitialiseWeapons),"Weapon::InitialiseWeapons",'c',"",'0',0x024000u},
        {0x73A330,A(&CWeapon::ShutdownWeapons),"Weapon::ShutdownWeapons",'c',"",'0',0x100000u},
        {0x73A360,A(&CWeapon::UpdateWeapons),"Weapon::UpdateWeapons",'c',"",'0',0x200000u},
        {0x742AB0,A(&CWeaponEffects::Init),"WeaponEffects::Init",'c',"",'0',0x044000u},
        {0x742B80,A(&CWeaponEffects::Shutdown),"WeaponEffects::Shutdown",'c',"",'0',0x100000u},
        {0x746870,A(&MessageLoop),"MessageLoop",'c',"",'0',0x040000u},
        {0x74F610,A(&RpWorldDestroy),"RpWorldDestroy",'c',"i",'1',0x000800u},
        {0x74F760,A(&RpWorldCreate),"RpWorldCreate",'c',"w",'W',0x000400u},
        {0x750F20,A(&RpWorldAddCamera),"RpWorldAddCamera",'c',"ii",'0',0x000400u},
        {0x750F50,A(&RpWorldRemoveCamera),"RpWorldRemoveCamera",'c',"ii",'0',0x000800u},
        {0x7EE1D0,A(&RwCameraSetNearClipPlane),"RwCameraSetNearClipPlane",'c',"ii",'0',0x000400u},
        {0x7EE2A0,A(&RwCameraSetFarClipPlane),"RwCameraSetFarClipPlane",'c',"ii",'0',0x000400u},
        {0x7EE4B0,A(&RwCameraDestroy),"RwCameraDestroy",'c',"i",'1',0x000008u},
        {0x7EE4F0,A(&RwCameraCreate),"RwCameraCreate",'c',"",'C',0x000400u},
        {0x7F0410,A(&RwFrameCreate),"RwFrameCreate",'c',"",'R',0x000400u},
        {0x7F05A0,A(&RwFrameDestroy),"RwFrameDestroy",'c',"i",'1',0x000008u},
        {0x7F2A20,A(&RwMatrixDestroy),"RwMatrixDestroy",'c',"i",'1',0x000800u},
        {0x7F2A50,A(&RwMatrixCreate),"RwMatrixCreate",'c',"",'m',0x000400u},
        {0x7FAE20,A(&_rwD3D9DeviceSetRestoreCallback),"SetRestoreCallback",'c',"i",'0',0x000400u},
        {0x7FAE30,A(&_rwD3D9DeviceGetRestoreCallback),"GetRestoreCallback",'c',"",'g',0x000400u},
        {0x7FB020,A(&RwRasterDestroy),"RwRasterDestroy",'c',"i",'1',0x000008u},
        {0x7FB230,A(&RwRasterCreate),"RwRasterCreate",'c',"iiii",'X',0x000400u},
        {0x804EF0,A(&_rwObjectHasFrameSetFrame),"ObjectHasFrameSetFrame",'c',"ii",'S',0x000408u},
        {0x8214BD,0,"operator_delete",'c',"i",'D',0x100000u},
        {0,0,"Ped::DeletingDtor",'t',"b",'0',0u}, // vtable[0] de los peatones falsos (delete del jugador en Shutdown)
    };
    return s;
}

// --- registro fijo (sin reservas: también corre con operator delete sustituido) ---
struct Rec { uint16 stub; uint8 n; uint8 str; uint32 v[8]; };
inline std::array<Rec,2048> recs{}; inline size_t nrec{};
inline std::array<char,1<<16> pool{}; inline size_t npool{}; inline bool overflow{};

// --- fixtures de los stubs ---
inline uint64 boolMask{}; inline uint32 boolCalls{};
inline void* fakePedResult{};
inline std::array<uint32,16> cyc{}; inline uint32 cycCalls{}, cpms{1};
inline int32 slotBase{}, addCalls{}, findResult{};
inline void* camResult{}; inline void* frameResult{}; inline std::array<void*,2> rasterResult{}; inline uint32 rasCalls{};
inline void* worldResult{}; inline uint32 matCalls{};
inline FILESTREAM openResult{}; inline std::array<uint8,128> readBytes{}; inline uint32 readCalls{};
inline bool loadedFixture{};

inline uint32 __cdecl Dispatch(uint32 idx,uint32 ecx,const uint32* args) {
    const auto& s=Stubs()[idx];
    Rec r{}; r.stub=uint16(idx);
    auto put=[&](uint32 v,bool str=false){ if(r.n<8){ if(str)r.str|=uint8(1u<<r.n); r.v[r.n++]=v; } else overflow=true; };
    auto putStr=[&](const char* p){
        if(!p){ put(0xFFFFFFFFu,true); return; }
        const size_t len=strnlen(p,256);
        if(npool+len+1>pool.size()){ overflow=true; put(0xFFFFFFFEu,true); return; }
        std::memcpy(&pool[npool],p,len); pool[npool+len]=0; put(uint32(npool),true); npool+=len+1;
    };
    if(s.conv=='t')put(ecx);
    for(size_t k=0;s.kinds[k];++k){
        const uint32 a=args[k];
        switch(s.kinds[k]){
        case 'i': put(a); break;
        case 'b': put(a&0xFFu); break;
        case 's': putStr(reinterpret_cast<const char*>(a)); break;
        case 'v': { auto* p=reinterpret_cast<const uint32*>(a); for(int j=0;j<3;++j)put(p ? p[j] : 0xFFFFFFFFu); break; }
        case 'w': { auto* p=reinterpret_cast<const uint32*>(a); for(int j=0;j<6;++j)put(p ? p[j] : 0xFFFFFFFFu); break; }
        default: break;
        }
    }
    if(nrec<recs.size())recs[nrec++]=r; else overflow=true;
    switch(s.ret){
    case '1': return 1;
    case 'B': return uint32((boolMask>>(boolCalls++&63))&1);
    case 'P': return uint32(uintptr_t(fakePedResult));
    case 'T': return cyc[cycCalls++&15];
    case 'M': return cpms;
    case 'A': return uint32(slotBase+addCalls++);
    case 'F': return uint32(findResult);
    case 'C': return uint32(uintptr_t(camResult));
    case 'R': return uint32(uintptr_t(frameResult));
    case 'X': return uint32(uintptr_t(rasterResult[rasCalls++&1]));
    case 'W': return uint32(uintptr_t(worldResult));
    case 'm': return 0x7A000000u+0x40u*matCalls++;
    case 'g': return 0x0BADCAFEu;
    case 'O': return uint32(uintptr_t(openResult));
    case 'r': { auto* b=reinterpret_cast<uint8*>(uintptr_t(args[1])); b[0]=readBytes[readCalls++&127]; return args[2]; }
    case 'L': *reinterpret_cast<bool*>(uintptr_t(args[0]))=loadedFixture; return uint32((boolMask>>(boolCalls++&63))&1);
    case 'S': if(args[0])*reinterpret_cast<uint32*>(uintptr_t(args[0])+4)=args[1]; return 0; // _rwObjectHasFrameSetFrame: object->parent = frame
    default: return 0;
    }
}
// Entrada común de los thunks: [esp]=vuelta al thunk, [esp+4]=índice, [esp+8]=vuelta al llamador, [esp+12]=argumentos.
// Conserva ECX/EDX; el thunk termina con "ret N" (thiscall) o "ret" (cdecl).
__declspec(naked) inline void Common() {
    __asm {
        push ebp
        mov ebp, esp
        push ecx
        push edx
        lea eax, [ebp+16]
        push eax
        push ecx
        push dword ptr [ebp+8]
        call Dispatch
        add esp, 12
        pop edx
        pop ecx
        pop ebp
        ret 4
    }
}
inline uint8* thunks{};
inline void* Thunk(size_t i){ return thunks+i*16; }
inline bool BuildThunks() {
    const auto& s=Stubs();
    thunks=static_cast<uint8*>(VirtualAlloc(nullptr,s.size()*16,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
    if(!thunks)return false;
    const auto common=reinterpret_cast<uintptr_t>(&Common);
    for(size_t i=0;i<s.size();++i){
        uint8* t=thunks+i*16; const uint32 idx=uint32(i);
        t[0]=0x68; std::memcpy(t+1,&idx,4);
        t[5]=0xE8; const int32 rel=int32(common-(uintptr_t(t)+10)); std::memcpy(t+6,&rel,4);
        const uint16 n=s[i].conv=='t' ? uint16(4*std::strlen(s[i].kinds)) : 0;
        if(n){ t[10]=0xC2; std::memcpy(t+11,&n,2); } else t[10]=0xC3;
    }
    FlushInstructionCache(GetCurrentProcess(),thunks,s.size()*16);
    return true;
}
inline json Trace() {
    json t=json::array();
    for(size_t i=0;i<nrec;++i){
        const auto& r=recs[i]; json e=json::array({Stubs()[r.stub].name});
        for(int k=0;k<r.n;++k){
            if(r.str>>k&1){ if(r.v[k]==0xFFFFFFFFu)e.push_back(nullptr); else if(r.v[k]==0xFFFFFFFEu)e.push_back("overflow"); else e.push_back(std::string(&pool[r.v[k]])); }
            else e.push_back(r.v[k]);
        }
        t.push_back(std::move(e));
    }
    if(overflow)t.push_back("overflow");
    return t;
}

// --- objetos falsos ---
alignas(16) inline std::array<uint8,0x400> fakeCam{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> fakeRas{};
alignas(16) inline std::array<uint8,0x100> fakeFrame{}, fakeWorld{};
alignas(16) inline std::array<std::array<uint8,0x800>,2> fakePeds{};
alignas(16) inline std::array<uint8,0x40> fakeTasks{};
inline std::array<void*,8> fakePedVt{};
inline std::array<char,64> datFile{};

// --- rangos globales guardados, rellenados y comparados ---
struct Range { uintptr_t a; size_t n; bool scratch; };
inline const std::array<Range,31> ranges{{
    {0xB728E8,4,true},{0xB728EC,32,true},{0xB7290C,4,true},{0xB72910,1,true},{0xB72914,4,true},{0xB72920,8,true},{0xB72958,2,true},
    {0xB72C28,64,true},{0xB7CD73,2,true},{0xB7CD98,0x320,false},{0xC3EF98,4,true},{0xC6E97C,1,true},{0xC2B9C8,0x14,true},
    {0xC17038,8,true},{0xBD0114,4,true},{0xC0B038,4,true},{0xBA67A8,1,false},{0xBA6795,6,false},{0xC81450,4,true},
    {0xBA372C,0x68,true},{0xC1CC18,1,true},{0xC17824,0x60,true},{0xC0985C,16,false},{0xB7CB48,2,false},{0xB7CB89,1,false},
    {0xB7CB5C,4,false},{0xB5F851,2,false},{0xA43088,1,false},{0xC0B184,1,false},{0xB79F50,0x84,false},{0xB71508,2,false},
}};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,22> fns{{
    {"CGame","CanSeeOutSideFromCurrArea",0x53C4A0,0,-1,256},{"CGame","CanSeeWaterFromCurrArea",0x53C4B0,0,-1,256},
    {"CGame","ValidateVersion",0x5BA060,0,-1,512},{"CGame","CameraDestroy",0x72FD90,0,-1,512},
    {"CGame","TidyUpMemory",0x53C500,0,-1,256},{"CGame","DrasticTidyUpMemory",0x53C810,0,-1,64},
    {"CGame","ReloadIPLs",0x53BED0,0,-1,64},{"CGame","InitialiseOnceBeforeRW",0x53BB50,0,-1,64},
    {"CGame","InitialiseEssentialsAfterRW",0x5BA160,0,-1,256},{"CGame","InitialiseCoreDataAfterRW",0x5BFA90,0,-1,256},
    {"CGame","InitialiseRenderWare",0x5BD600,0,-1,512},{"CGame","ShutdownRenderWare",0x53BB80,0,-1,64},
    {"CGame","FinalShutdown",0x53BC30,0,-1,64},{"CGame","Init1",0x5BF840,0,-1,256},
    {"CGame","Init2",0x5BA1A0,0,-1,256},{"CGame","Init3",0x5BA400,0,-1,64},
    {"CGame","Initialise",0x53BC80,0,-1,64},{"CGame","ReInitGameObjectVariables",0x53BCF0,0,-1,128},
    {"CGame","InitialiseWhenRestarting",0x53C680,0,-1,512},{"CGame","ShutDownForRestart",0x53C550,0,-1,128},
    {"CGame","Shutdown",0x53C900,0,-1,256},{"CGame","Process",0x53BEE0,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"game22-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"game22-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("game22");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"game22-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,22> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    if(!BuildThunks()){Write({{"event","error"},{"reason","VirtualAlloc"}});return;}
    std::array<bool,22> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    const auto& stubs=Stubs();
    const size_t dtorIdx=stubs.size()-1; size_t delIdx=0;
    for(size_t k=0;k<stubs.size();++k)if(stubs[k].ret=='D')delIdx=k;
    for(auto& v:fakePedVt)v=Thunk(dtorIdx);

    std::vector<std::vector<uint8>> saved;
    for(auto& r:ranges){ auto* p=reinterpret_cast<uint8*>(r.a); saved.emplace_back(p,p+r.n); }
    auto restore=[&]{ for(size_t k=0;k<ranges.size();++k)std::memcpy(reinterpret_cast<void*>(ranges[k].a),saved[k].data(),ranges[k].n); };
    auto B=[](uintptr_t a)->uint8&{ return *reinterpret_cast<uint8*>(a); };
    auto D=[](uintptr_t a)->uint32&{ return *reinterpret_cast<uint32*>(a); };

    bool passed=true; int pair=0;
    using ReversingProbe::EntryPatch;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto run=[&](bool candidate)->json {
                restore();
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6A22u; if(!stt)stt=1;
                // basura determinista en los globales que escriben las funciones
                for(auto& r:ranges)if(r.scratch)for(size_t k=0;k<r.n;++k)B(r.a+k)=uint8(next());
                // fixtures comunes de los stubs
                boolMask=0; for(int b=0;b<64;++b)if(next()%8)boolMask|=1ull<<b;
                boolCalls=cycCalls=rasCalls=matCalls=readCalls=0; addCalls=0;
                fakePedResult=next()%3 ? static_cast<void*>(fakePeds[0].data()) : nullptr;
                cpms=1000+next()%3000; { uint32 t=next(); for(auto& c:cyc){ t+=next()%(cpms*8); c=t; } }
                slotBase=int32(next()%5000); findResult=int32(next()%5000)-1;
                for(auto& b:fakeCam)b=uint8(next()); for(auto& r:fakeRas)for(auto& b:r)b=uint8(next());
                camResult=next()%8 ? static_cast<void*>(fakeCam.data()) : nullptr; frameResult=next()%8 ? static_cast<void*>(fakeFrame.data()) : nullptr;
                rasterResult[0]=next()%8 ? static_cast<void*>(fakeRas[0].data()) : nullptr; rasterResult[1]=next()%8 ? static_cast<void*>(fakeRas[1].data()) : nullptr;
                auto parentOf=[&](int k)->uintptr_t{ switch(next()%6){case 0:return 0;case 1:return uintptr_t(fakeRas[2].data());case 2:return uintptr_t(fakeRas[k^1].data());default:return uintptr_t(fakeRas[k].data());} };
                for(int k=0;k<2;++k)*reinterpret_cast<uintptr_t*>(fakeRas[k].data())=parentOf(k);
                worldResult=next()%8 ? static_cast<void*>(fakeWorld.data()) : nullptr;
                openResult=next()%4 ? reinterpret_cast<FILESTREAM>(0x1234) : nullptr;
                loadedFixture=next()%2;
                { const int len=int(next()%31); for(int k=0;k<len;++k)datFile[k]=char('A'+next()%26); datFile[len]=0; }
                bool a1=next()%2, a2=next()%2;
                // fixtures por función
                if(nm=="CanSeeOutSideFromCurrArea"||nm=="CanSeeWaterFromCurrArea"){ switch(next()%4){case 0:D(0xB72914)=0;break;case 1:D(0xB72914)=5;break;case 2:D(0xB72914)=next()%20;break;default:D(0xB72914)=next();} }
                else if(nm=="ValidateVersion"){
                    static const char prefix[]="grandtheftauto3"; std::array<uint8,128> text{};
                    for(int k=0;k<15;++k)text[k]=uint8(prefix[k]);
                    const bool noTerm=next()%6==0; const int len=noTerm ? 113 : int(next()%100);
                    for(int k=0;k<len;++k)text[15+k]=uint8(0x21+next()%94);
                    for(int k=15+len;k<128;++k)text[k]=noTerm ? uint8(0x21+next()%94) : uint8(next());
                    if(!noTerm)text[15+len]=0;
                    for(int k=0;k<128;++k)readBytes[k]=uint8(text[k]+23);
                }
                else if(nm=="CameraDestroy"){
                    *reinterpret_cast<uintptr_t*>(fakeCam.data()+4)=next()%4 ? uintptr_t(fakeFrame.data()) : 0;
                    *reinterpret_cast<uintptr_t*>(fakeCam.data()+0x60)=next()%4 ? uintptr_t(fakeRas[0].data()) : 0;
                    *reinterpret_cast<uintptr_t*>(fakeCam.data()+0x64)=next()%4 ? uintptr_t(fakeRas[1].data()) : 0;
                }
                else if(nm=="InitialiseWhenRestarting"||nm=="ReInitGameObjectVariables"){
                    B(0xBA67A8)=uint8(next()%3 ? 1 : 0); for(int k=0;k<6;++k)B(0xBA6795+k)=uint8(next());
                }
                else if(nm=="Shutdown"){
                    for(int p=0;p<2;++p){ *reinterpret_cast<void**>(fakePeds[p].data())=fakePedVt.data(); *reinterpret_cast<void**>(0xB7CD98+p*0x190)=next()%3 ? static_cast<void*>(fakePeds[p].data()) : nullptr; }
                    auto* t=reinterpret_cast<uintptr_t*>(0xC0985C);
                    if(next()%3){ t[1]=uintptr_t(fakeTasks.data()); t[2]=t[1]+4*(next()%5); t[3]=t[2]+4*(next()%5); } else t[1]=t[2]=t[3]=0;
                }
                else if(nm=="Process"){
                    B(0xB7CB49)=uint8(next()%6==0); B(0xB7CB48)=uint8(next()%6==0); B(0xB7CB89)=uint8(next()%4==0);
                    B(0xB5F852)=uint8(next()%6==0); B(0xB5F851)=uint8(next()%4==0); B(0xA43088)=uint8(next()%3); B(0xC0B184)=uint8(next());
                    D(0xB79FD0)=next()%16; for(int k=0;k<32;++k)*reinterpret_cast<float*>(0xB79F50+4*k)=uni(-2.f,2.f);
                    *reinterpret_cast<float*>(0xB7CB5C)=uni(0.05f,3.f);
                }
                nrec=0; npool=0; overflow=false;
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                const uintptr_t a=fn.address;
                json ret=nullptr;
                {
                    std::vector<std::unique_ptr<EntryPatch>> P; P.reserve(200);
                    for(size_t k=0;k<stubs.size();++k){ const auto& s=stubs[k]; if(!(s.mask>>f&1)||s.ret=='D')continue;
                        if(s.exe)P.push_back(std::make_unique<EntryPatch>(s.exe,Thunk(k))); if(s.dll)P.push_back(std::make_unique<EntryPatch>(s.dll,Thunk(k))); }
                    {
                        std::optional<EntryPatch> del; if(nm=="Shutdown")del.emplace(0x8214BD,Thunk(delIdx));
                        switch(f){
                        case 0: case 1: ret=reinterpret_cast<bool(__cdecl*)()>(a)(); break;
                        case 3: reinterpret_cast<void(__cdecl*)(void*)>(a)(next()%8 ? static_cast<void*>(fakeCam.data()) : nullptr); break;
                        case 4: reinterpret_cast<void(__cdecl*)(bool,bool)>(a)(a1,a2); break;
                        case 5: reinterpret_cast<void(__cdecl*)(bool)>(a)(a1); break;
                        case 8: case 9: case 10: case 20: ret=reinterpret_cast<bool(__cdecl*)()>(a)(); break;
                        case 13: case 14: case 15: ret=reinterpret_cast<bool(__cdecl*)(const char*)>(a)(datFile.data()); break;
                        case 16: reinterpret_cast<void(__cdecl*)(const char*)>(a)(datFile.data()); break;
                        default: reinterpret_cast<void(__cdecl*)()>(a)(); break;
                        }
                    }
                }
                json out={{"switches",switches},{"ret",ret},{"trace",Trace()}};
                json st=json::array(); for(auto& r:ranges)st.push_back(Fnv(reinterpret_cast<void*>(r.a),r.n)); out["state"]=st;
                if(nm=="CameraDestroy"||nm=="InitialiseRenderWare"){ out["cam"]=Fnv(fakeCam.data(),fakeCam.size()); out["ras"]=Fnv(fakeRas.data(),sizeof(fakeRas)); }
                if(nm=="ValidateVersion")out["version"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB72C28),64);
                restore();
                return out;
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restore();
    bool restored=true; for(size_t k=0;k<ranges.size();++k)restored&=std::memcmp(reinterpret_cast<void*>(ranges[k].a),saved[k].data(),ranges[k].n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
