#pragma once
#include "Automobile.h"
#include "Bike.h"
#include "Boat.h"
#include "Train.h"
#include "PlayerPed.h"
#include "PedIntelligence.h"
#include "Streaming.h"
#include "Pools/Pools.h"
#include "World.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskSimpleCarSetPedOut.h"

namespace HookUnlockAudioLive {
// Sesión desechable: no restaura tareas, estadísticas, eventos, radio ni streaming.
// El runner conserva los archivos y cierra el proceso sin guardar la sesión.
inline CVehicle* liveVehicle{};
inline CVehicle* ownedLiveVehicle{};
inline CPlayerPed* fixturePlayer{};
inline CVector fixturePlayerPosition{};
inline bool fixturePlayerCaptured{};
inline int fixtureModel{-1};
inline uint32 fixtureModelFlags{};
inline const char* fixtureVehicleKind{"none"};
inline CTrain* ownedLiveTrain{};
inline std::vector<std::pair<int32,uint32>> fixtureTrainModels;
inline std::array<uint32,4> fixtureTrainGeneration{};
inline uint8 fixtureTrainDirection{};
inline bool fixtureTrainDisabled{},fixtureTrainGlobalsCaptured{};

inline void CleanupLiveTrain() {
    if (ownedLiveTrain) {
        // Sólo la cadena creada por esta prueba; nunca RemoveMissionTrains/RemoveAllTrains.
        CTrain::RemoveOneMissionTrain(ownedLiveTrain);
        ownedLiveTrain=nullptr;
        liveVehicle=nullptr;
    }
    for (const auto& [model,flags]:fixtureTrainModels)
        CStreaming::GetInfo(model).m_Flags=flags;
    fixtureTrainModels.clear();
    if (fixtureTrainGlobalsCaptured) {
        CTrain::GenTrain_Track=fixtureTrainGeneration[0];
        CTrain::GenTrain_TrainConfig=fixtureTrainGeneration[1];
        CTrain::GenTrain_GenerationNode=fixtureTrainGeneration[2];
        CTrain::GenTrain_Status=fixtureTrainGeneration[3];
        CTrain::GenTrain_Direction=fixtureTrainDirection;
        CTrain::bDisableRandomTrains=fixtureTrainDisabled;
        fixtureTrainGlobalsCaptured=false;
    }
}

inline bool CleanupLiveVehicle() {
    CleanupLiveTrain();
    auto* vehicle=ownedLiveVehicle;
    if (vehicle && fixturePlayer && fixturePlayer->bInVehicle && fixturePlayer->m_pVehicle==vehicle) {
        fixturePlayer->GetIntelligence()->FlushImmediately(false);
        CTaskSimpleCarSetPedOut task{vehicle,TARGET_DOOR_DRIVER,true,true};
        if (!task.ProcessPed(fixturePlayer)) return false;
    }
    if (vehicle) {
        // La limpieza real libera audio, referencias y ranura del pool; no copia bytes de objetos vivos.
        CWorld::Remove(vehicle);
        delete vehicle;
        ownedLiveVehicle=nullptr;
    }
    liveVehicle=nullptr;
    if (fixtureModel>=0) {
        // Sólo devolver flags de solicitud; el recurso cargado puede permanecer hasta cierre de sesión.
        CStreaming::GetInfo(fixtureModel).m_Flags=fixtureModelFlags;
        fixtureModel=-1;
    }
    if (fixturePlayerCaptured && fixturePlayer && !fixturePlayer->bInVehicle)
        fixturePlayer->Teleport(fixturePlayerPosition,false);
    fixtureVehicleKind="none";
    return !fixturePlayer || !fixturePlayer->bInVehicle;
}

inline bool SetupLiveTrain() {
    if (!CleanupLiveVehicle()) return false;
    auto* player=FindPlayerPed();
    if (!player || player->bInVehicle) return false;
    auto& nodes=StaticRef<CTrainNode*[4]>(0xC38024);
    auto& counts=StaticRef<std::array<int32,4>>(0xC38014);
    int track=-1,index=-1;
    float best=std::numeric_limits<float>::max();
    CVector spawn;
    // Sólo nodos existentes, nunca construir un CTrain aislado con posición arbitraria.
    for (int t=0;t<4;++t) {
        if (!nodes[t] || counts[t]<2) continue;
        for (int n=0;n<counts[t];++n) {
            const CVector pos{float(nodes[t][n].x)*.125f,float(nodes[t][n].y)*.125f,float(nodes[t][n].z)*.125f};
            bool clear=true;
            auto* pool=GetVehiclePool();
            for (uint32 i=0;i<pool->GetSize();++i) {
                auto* vehicle=pool->GetAt(i);
                if (vehicle && vehicle->IsSubTrain() && (vehicle->GetPosition()-pos).SquaredMagnitude()<22500.f) {clear=false;break;}
            }
            const auto distance=(pos-player->GetPosition()).SquaredMagnitude();
            if (clear && distance<best) {best=distance;track=t;index=n;spawn=pos;}
        }
    }
    if (track<0) return false;
    const auto* models=reinterpret_cast<const int32*>(0x8D44F8); // Configuración original 0, cinco vagones.
    int count=0;
    while (count<16 && models[count]) ++count;
    if (!count || count==16) return false;
    int freeSlots=0;
    auto* pool=GetVehiclePool();
    for (uint32 i=0;i<pool->GetSize();++i) freeSlots+=pool->IsFreeSlotAtIndex(i);
    if (freeSlots<count) return false;
    fixtureTrainGeneration={CTrain::GenTrain_Track,CTrain::GenTrain_TrainConfig,CTrain::GenTrain_GenerationNode,CTrain::GenTrain_Status};
    fixtureTrainDirection=CTrain::GenTrain_Direction;
    fixtureTrainDisabled=CTrain::bDisableRandomTrains;
    fixtureTrainGlobalsCaptured=true;
    CTrain::bDisableRandomTrains=true;
    for (int i=0;i<count;++i) {
        const int model=models[i];
        bool seen=false;
        for (const auto& item:fixtureTrainModels) seen|=item.first==model;
        if (!seen) fixtureTrainModels.emplace_back(model,CStreaming::GetInfo(model).m_Flags);
        CStreaming::RequestModel(model,STREAMING_KEEP_IN_MEMORY);
    }
    CStreaming::LoadAllRequestedModels(false);
    for (const auto& item:fixtureTrainModels) if (!CStreaming::IsModelLoaded(item.first)) {CleanupLiveTrain();return false;}
    CTrain* last{};
    CTrain::CreateMissionTrain(spawn,true,0,&ownedLiveTrain,&last,index,track,true);
    if (!ownedLiveTrain || !last) {CleanupLiveTrain();return false;}
    CTrain::SetTrainSpeed(ownedLiveTrain,0.f);
    CTrain::SetTrainCruiseSpeed(ownedLiveTrain,0.f);
    CEntity::SafeRegisterRef(ownedLiveTrain);
    liveVehicle=ownedLiveTrain;
    CEntity::SafeRegisterRef(liveVehicle);
    fixtureVehicleKind="train";
    return true;
}

// 0 automóvil, 1 moto, 2 barco. Se exige jugador inicialmente a pie.
inline bool SetupLiveVehicle(int kind=0) {
    if (!CleanupLiveVehicle()) return false;
    auto* player=FindPlayerPed();
    if (!player || player->bInVehicle || kind<0 || kind>2) return false;
    if (!fixturePlayerCaptured) {
        fixturePlayer=player;
        fixturePlayerPosition=player->GetPosition();
        fixturePlayerCaptured=true;
    }
    if (player!=fixturePlayer) return false;
    const int models[]{MODEL_LANDSTAL,MODEL_PCJ600,MODEL_SPEEDER};
    fixtureModel=models[kind];
    fixtureModelFlags=CStreaming::GetInfo(fixtureModel).m_Flags;
    CStreaming::RequestModel(fixtureModel,STREAMING_KEEP_IN_MEMORY);
    CStreaming::LoadAllRequestedModels(false);
    if (!CStreaming::IsModelLoaded(fixtureModel)) {CleanupLiveVehicle();return false;}
    if (kind==0) ownedLiveVehicle=new CAutomobile(fixtureModel,MISSION_VEHICLE,true);
    else if (kind==1) ownedLiveVehicle=new CBike(fixtureModel,MISSION_VEHICLE);
    else ownedLiveVehicle=new CBoat(fixtureModel,MISSION_VEHICLE);
    if (!ownedLiveVehicle) {CleanupLiveVehicle();return false;}
    CEntity::SafeRegisterRef(ownedLiveVehicle);
    liveVehicle=ownedLiveVehicle;
    CEntity::SafeRegisterRef(liveVehicle);
    liveVehicle->SetPosn(fixturePlayerPosition+CVector{4.f,0.f,1.f});
    liveVehicle->SetStatus(STATUS_ABANDONED);
    liveVehicle->vehicleFlags.bEngineOn=true;
    liveVehicle->vehicleFlags.bHasBeenOwnedByPlayer=true;
    CWorld::Add(liveVehicle);
    fixtureVehicleKind=kind==0?"automobile":kind==1?"bike":"boat";
    if (kind<2) {
        player->GetIntelligence()->FlushImmediately(false);
        CTaskSimpleCarSetPedInAsDriver task{liveVehicle,true,nullptr};
        if (!task.ProcessPed(player) || liveVehicle->m_pDriver!=player || !player->bInVehicle) {
            CleanupLiveVehicle();return false;
        }
        liveVehicle->SetStatus(STATUS_PLAYER);
    }
    return true;
}

// No genera trenes ni toca sus globals. Si no existe uno, la cobertura se declara pendiente.
// El banco debe usarlo sólo inmediatamente en el hilo principal; no retener punteros entre frames.
inline CTrain* FindExistingLiveTrain() {
    auto* pool=GetVehiclePool();
    for (uint32 i=0;i<pool->GetSize();++i) {
        auto* vehicle=pool->GetAt(i);
        if (vehicle && vehicle->m_nVehicleSubType==VEHICLE_TYPE_TRAIN)
            return static_cast<CTrain*>(vehicle);
    }
    return nullptr;
}
}
