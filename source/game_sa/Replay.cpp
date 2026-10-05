/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CReplay
 * Reconstrucción: COMPLETA — 47/47 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/47 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Camera.h"
#include "ControllerConfigManager.h"
#include "EntryExitManager.h"
#include "FireManager.h"
#include "PedClothesDesc.h"
#include "PlaneBanners.h"
#include "RealTimeShadowManager.h"
#include "Replay.h"
#include "Skidmarks.h"

void CReplay::InjectHooks() {
    RH_ScopedClass(CReplay);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x45E220);
    RH_ScopedInstall(Update, 0x460500);
    RH_ScopedInstall(DisableReplays, 0x45B150);
    RH_ScopedInstall(EnableReplays, 0x45B160);
    RH_ScopedInstall(StorePedAnimation, 0x45B170);
    RH_ScopedInstall(StorePedUpdate, 0x45C940);
    RH_ScopedInstall(RetrievePedAnimation, 0x45B4D0);
    RH_ScopedInstall(ProcessReplayCamera, 0x45D060);
    RH_ScopedInstall(Display, 0x45C210);
    RH_ScopedInstall(MarkEverythingAsNew, 0x45D430);
    RH_ScopedInstall(EmptyReplayBuffer, 0x45EC90);
    RH_ScopedInstall(EmptyPedsAndVehiclePools_NoDestructors, 0x45D390);
    RH_ScopedInstall(GoToNextBlock, 0x45E2A0);
    RH_ScopedInstall(RecordVehicleDeleted, 0x45EBB0);
    RH_ScopedInstall(RecordPedDeleted, 0x45EC20);
    RH_ScopedInstall(SaveReplayToHD, 0x45C340);
    RH_ScopedInstall(PlayReplayFromHD, 0x460390);
    RH_ScopedInstall(ShouldStandardCameraBeProcessed, 0x45C440);
    RH_ScopedInstall(ProcessLookAroundCam, 0x45D760);
    RH_ScopedInstall(FindPoolIndexForPed, 0x45C450);
    RH_ScopedInstall(FindPoolIndexForVehicle, 0x45C460);
    RH_ScopedInstall(ProcessPedUpdate, 0x45CA70);
    RH_ScopedInstall(CanWeFindPoolIndexForPed, 0x45C470);
    RH_ScopedInstall(CanWeFindPoolIndexForVehicle, 0x45C490);
    RH_ScopedInstall(StorePlayerInfoVariables, 0x45F020);
    RH_ScopedInstall(RestorePlayerInfoVariables, 0x45E1F0);
    RH_ScopedInstall(StoreStuffInMem, 0x45F180);
    RH_ScopedInstall(RestoreStuffFromMem, 0x45ECD0);
    RH_ScopedInstall(FinishPlayback, 0x45F050);
    RH_ScopedInstall(RecordThisFrame, 0x45E300);
    RH_ScopedInstall(StoreClothesDesc, 0x45C750);
    RH_ScopedInstall(RestoreClothesDesc, 0x45C7D0);
    RH_ScopedInstall(DealWithNewPedPacket, 0x45CEA0);
    RH_ScopedInstall(PlayBackThisFrameInterpolation, 0x45F380);
    RH_ScopedInstall(FastForwardToTime, 0x460350);
    RH_ScopedInstall(PlayBackThisFrame, 0x4604A0);
    RH_ScopedInstall(FindSizeOfPacket, 0x45C850);
    RH_ScopedInstall(IsThisVehicleUsedInRecording, 0x45DE40);
    RH_ScopedInstall(IsThisPedUsedInRecording, 0x45DDE0);
    RH_ScopedInstall(InitialiseVehiclePoolConversionTable, 0x45EFA0);
    RH_ScopedInstall(InitialisePedPoolConversionTable, 0x45EF20);
    RH_ScopedInstall(InitialisePoolConversionTables, 0x45F370);
    RH_ScopedInstall(FindFirstFocusCoordinate, 0x45D6C0);
    RH_ScopedInstall(NumberFramesAvailableToPlay, 0x45D670);
    RH_ScopedInstall(StreamAllNecessaryCarsAndPeds, 0x45D4B0);
    RH_ScopedInstall(CreatePlayerPed, 0x45D540);
    RH_ScopedInstall(TriggerPlayback, 0x4600F0);
}

namespace {
// Como el original: el EXE accede a los pools en crudo (objetos en +0, byte map en +4, bit 0x80 = libre),
// sin comprobar límites; tamaños de elemento del EXE.
constexpr int32 REPLAY_PED_SIZE = 0x7C4;
constexpr int32 REPLAY_VEH_SIZE = 0xA18;

inline uint8* ReplayPoolObjects(void* pool) { return reinterpret_cast<uint8**>(pool)[0]; }
inline int8*  ReplayPoolFlags(void* pool)   { return reinterpret_cast<int8**>(pool)[1]; }
inline int32  ReplayPoolSize(void* pool)    { return reinterpret_cast<int32*>(pool)[2]; }

inline CPed* ReplayPedAt(int32 idx) {
    auto* pool = GetPedPool();
    return ReplayPoolFlags(pool)[idx] < 0 ? nullptr : reinterpret_cast<CPed*>(ReplayPoolObjects(pool) + idx * REPLAY_PED_SIZE);
}

inline CVehicle* ReplayVehicleAt(int32 idx) {
    auto* pool = GetVehiclePool();
    return (ReplayPoolFlags(pool)[idx] & 0x80) ? nullptr : reinterpret_cast<CVehicle*>(ReplayPoolObjects(pool) + idx * REPLAY_VEH_SIZE);
}

inline int32 ReplayPedIndex(const CPed* ped) {
    return static_cast<int32>((reinterpret_cast<const uint8*>(ped) - ReplayPoolObjects(GetPedPool())) / REPLAY_PED_SIZE);
}

inline int32 ReplayVehicleIndex(const CVehicle* veh) {
    return static_cast<int32>((reinterpret_cast<const uint8*>(veh) - ReplayPoolObjects(GetVehiclePool())) / REPLAY_VEH_SIZE);
}

// Como el original: _ftol2 (0x821B40) trunca a 64 bits; se guardan los bits bajos.
inline int64 ReplayFtol(float value) {
    return static_cast<int64>(value);
}

template <typename T>
inline T& ReplayAt(uint8* p, size_t offset) {
    return *reinterpret_cast<T*>(p + offset);
}

inline uint8* ReplayRecordPtr() {
    return reinterpret_cast<uint8*>(CReplay::Record.m_pBase) + CReplay::Record.m_nOffset;
}

inline uint8* ReplayBufferPtr(uint32 slot) {
    return CReplay::Buffers.data()[slot].buffer.data();
}

constexpr float REPLAY_HEADING_DECOMPRESS = std::bit_cast<float>(0x3CC8F5C3u); // 0x859C44 (no es el float más cercano a 1/40.764328)

// 0x45B730
// Como el original: escribe directamente en el paquete del buffer y deja intactos el tipo, el relleno y,
// fuera de los automóviles, las puertas, ruedas y estado de puertas.
void ReplayStoreCarUpdate(uint8* p, CVehicle* veh, int32 poolIdx) {
    auto* v = reinterpret_cast<uint8*>(veh);

    p[1] = static_cast<uint8>(poolIdx);
    reinterpret_cast<CCompressedMatrixNotAligned*>(p + 4)->CompressFromFullMatrix(veh->GetMatrix());

    float health = ReplayAt<float>(v, 0x4C0);
    if (1000.0f < health) {
        health = 1000.0f;
    }
    p[2] = static_cast<uint8>(ReplayFtol(health * 0.25f));
    p[3] = static_cast<uint8>(ReplayFtol(ReplayAt<float>(v, 0x49C) * 100.0f));
    ReplayAt<uint32>(p, 0x1C) = ReplayAt<uint32>(v, 0x590) == 0 ? ReplayAt<uint32>(v, 0x5B4) : 0u;

    const auto CompressSpeed = [](float s) {
        if (4.0f < s) {
            s = 4.0f;
        } else if (-4.0f > s) {
            s = -4.0f;
        }
        return static_cast<uint8>(ReplayFtol(s * 8000.0f));
    };
    p[0x20] = CompressSpeed(ReplayAt<float>(v, 0x44));
    p[0x21] = CompressSpeed(ReplayAt<float>(v, 0x48));
    p[0x22] = CompressSpeed(ReplayAt<float>(v, 0x4C));

    ReplayAt<uint16>(p, 0x1A) = ReplayAt<uint16>(v, 0x22);
    p[0x2D] = v[0x434];
    p[0x2E] = v[0x435];

    const uint32 entityFlags = ReplayAt<uint32>(v, 0x1C);
    uint8 flags = p[0x2F];
    flags = static_cast<uint8>((flags & ~2u) | (((entityFlags >> 7) & 1u) << 1));
    flags = static_cast<uint8>((flags & ~4u) | (((entityFlags >> 30) & 1u) << 2));
    flags = static_cast<uint8>((flags & ~8u) | ((entityFlags >> 31) << 3));
    p[0x2F] = flags;
    if (veh == FindPlayerVehicle(-1, false) && gbFirstPersonRunThisFrame) {
        p[0x2F] &= ~2u;
    }

    if (ReplayAt<uint16>(v, 0x22) == MODEL_RHINO) {
        p[0x23] = static_cast<uint8>(ReplayFtol(ReplayAt<float>(v, 0x94C) * HEADING_COMPRESS_VALUE));
    } else {
        p[0x23] = static_cast<uint8>(ReplayFtol(ReplayAt<float>(v, 0x494) * 50.0f));
    }

    if (ReplayAt<uint32>(v, 0x590) == 0) {
        for (auto i = 0u; i < 4u; i++) {
            p[0x24 + i] = static_cast<uint8>(ReplayFtol(ReplayAt<float>(v, 0x7D4 + 4 * i) * 50.0f));
            p[0x28 + i] = static_cast<uint8>(ReplayFtol(ReplayAt<float>(v, 0x828 + 4 * i) * HEADING_COMPRESS_VALUE));
        }
        p[0x18] = static_cast<uint8>(ReplayFtol(ReplayAt<float>(v, 0x5F4) * 20.222929f));
        p[0x19] = static_cast<uint8>(ReplayFtol(ReplayAt<float>(v, 0x60C) * 20.222929f));

        uint8 doors = 0;
        for (auto i = 0u; i < 6u; i++) {
            if (v[0x5A9 + i] == 4) {
                doors |= static_cast<uint8>(1u << i);
            }
        }
        p[0x2C] = doors;
    }

    p[0x2F] = static_cast<uint8>((p[0x2F] & ~1u) | ((ReplayAt<uint32>(v, 0x40) >> 29) & 1u));
    p[0x30] = v[0x594];
}

// 0x45C070 (moto) / 0x45BF90 (BMX)
void ReplayStoreBikeUpdate(uint8* p, CVehicle* veh, int32 poolIdx, eReplayPacket type) {
    p[0] = type;
    ReplayStoreCarUpdate(p, veh, poolIdx);
    // Como el original: dos llamadas virtuales a GetRideAnimData (vtable +0xBC)
    p[0x34] = static_cast<uint8>(ReplayFtol(veh->GetRideAnimData()->LeanAngle * 50.0f));
    p[0x35] = static_cast<uint8>(ReplayFtol(veh->GetRideAnimData()->BarSteerAngle * 50.0f));
}

// 0x45CD10
void ReplayStoreTrainUpdate(uint8* p, CVehicle* veh, int32 poolIdx) {
    auto* v = reinterpret_cast<uint8*>(veh);

    p[0] = REPLAY_PACKET_TRAIN;
    ReplayStoreCarUpdate(p, veh, poolIdx);
    ReplayAt<uint32>(p, 0x34) = ReplayAt<uint32>(v, 0x5A4);
    ReplayAt<uint32>(p, 0x38) = ReplayAt<uint32>(v, 0x5A8);
    ReplayAt<uint32>(p, 0x3C) = ReplayAt<uint32>(v, 0x5AC);
    p[0x48] = v[0x5C0];
    ReplayAt<uint32>(p, 0x40) = 0;
    ReplayAt<uint32>(p, 0x44) = 0;
    if (auto* prev = ReplayAt<CVehicle*>(v, 0x5D0)) {
        ReplayAt<int32>(p, 0x40) = ReplayVehicleIndex(prev) + 1;
    }
    if (auto* next = ReplayAt<CVehicle*>(v, 0x5D4)) {
        ReplayAt<int32>(p, 0x44) = ReplayVehicleIndex(next) + 1;
    }
}
} // namespace

// 0x460625 thunk
// 0x45E220
void CReplay::Init() {
    // Como el original: EmptyReplayBuffer en línea (sin mirar Mode) y MarkEverythingAsNew al final.
    reinterpret_cast<uint32*>(BufferStatus.data())[0] = 0;
    Record.m_nOffset = 0;
    Playback.m_nOffset = 0;
    Playback.m_pBase = nullptr;
    Playback.m_bSlot = 0;
    Mode = MODE_RECORD;
    pReferences = nullptr;
    pPickups = nullptr;
    pEmptyReferences = nullptr;
    pWorld1 = nullptr;
    pStoredCam = nullptr;
    pRadarBlips = nullptr;
    reinterpret_cast<uint32*>(BufferStatus.data())[1] = 0;
    Record.m_bSlot = 0;
    Record.m_pBase = &Buffers.data()[0];
    BufferStatus.data()[0] = REPLAYBUFFER_IN_USE;
    ReplayBufferPtr(0)[0] = REPLAY_PACKET_END;
    bPlayingBackFromFile = false;
    bDoLoadSceneWhenDone = false;
    FramesActiveLookAroundCam = 0;
    bReplayEnabled = true;
    MarkEverythingAsNew();
}

// 0x460500
void CReplay::Update() {
    ZoneScoped;

    if (CCutsceneMgr::IsCutsceneProcessing() || CPad::GetPad(0)->ArePlayerControlsDisabled()
        || FrontEndMenuManager.m_bMenuActive || CEntryExitManager::ms_exitEnterState) {
        Init();
        return;
    }

    if (CTimer::GetIsCodePaused() || CTimer::GetIsUserPaused())
        return;

    static auto& s_FrameRecordCounter = StaticRef<uint32>(0x97FB40);
    switch (Mode) {
    case MODE_RECORD:
        if (CTimer::GetTimeInMS() - s_FrameRecordCounter > 26) {
            s_FrameRecordCounter = CTimer::GetTimeInMS();
            RecordThisFrame();
        }
        break;
    case MODE_PLAYBACK:
        PlayBackThisFrame();
        break;
    default:
        break;
    }

    if (CDraw::IsFading() || !bReplayEnabled)
        return;

    // Como el original: GetIsKeyboardKeyJustDown (0x52E450), no GetIsKeyboardKeyDown.
    if (Mode == MODE_RECORD) {
        if (ControlsManager.GetIsKeyboardKeyJustDown(rsF1)) {
            // Play stored replay
            TriggerPlayback(REPLAY_CAM_MODE_AS_STORED, CVector{0.0f, 0.0f, 0.0f}, false);
        } else if (ControlsManager.GetIsKeyboardKeyJustDown(rsF2)) {
            // Save to hard disk
            SaveReplayToHD();
        } else if (ControlsManager.GetIsKeyboardKeyJustDown(rsF3)) {
            // Play from hard disk
            PlayReplayFromHD();
        }
    } else if (Mode == MODE_PLAYBACK) {
        if (ControlsManager.GetIsKeyboardKeyJustDown(rsF1) || ControlsManager.GetIsKeyboardKeyJustDown(rsF3)) {
            FinishPlayback();
        }
    }
}

// 0x45B150
void CReplay::DisableReplays() {
    bReplayEnabled = false;
}

// 0x45B160
void CReplay::EnableReplays() {
    bReplayEnabled = true;
}

// 0x45B170
void CReplay::StorePedAnimation(CPed* ped, CStoredAnimationState& state) {
    // Como el original: tiempo [0, 4] * 63,75, velocidad [0, 3] * 85 y mezcla [0, 2] * 127,5 (NaN pasa);
    // en la primera animación no se escribe el byte 5.
    const auto ClampTime  = [](float t) { if (4.0f < t) return 4.0f; if (0.0f > t) return 0.0f; return t; };
    const auto ClampSpeed = [](float s) { if (3.0f < s) return 3.0f; if (0.0f > s) return 0.0f; return s; };
    const auto ClampBlend = [](float b) { if (2.0f < b) return 2.0f; if (0.0f > b) return 0.0f; return b; };

    float blendValue;
    CAnimBlendAssociation* second;
    const auto main = RpAnimBlendClumpGetMainAssociation(ped->GetRpClump(), &second, &blendValue);
    if (main) {
        state[0].m_nAnimId   = static_cast<int16>(main->m_AnimId);
        state[0].m_nTime     = static_cast<uint8>(ReplayFtol(ClampTime(main->m_CurrentTime) * 63.75f));
        state[0].m_nSpeed    = static_cast<uint8>(ReplayFtol(ClampSpeed(main->m_Speed) * 85.0f));
        state[0].m_nGroupId1 = static_cast<uint8>(main->m_AnimGroupId);
    } else {
        state[0].m_nAnimId   = ANIM_ID_IDLE;
        state[0].m_nTime     = 0;
        state[0].m_nSpeed    = 85;
        state[0].m_nGroupId1 = 0;
    }

    if (second) {
        state[1].m_nAnimId   = static_cast<int16>(second->m_AnimId);
        state[1].m_nTime     = static_cast<uint8>(ReplayFtol(ClampTime(second->m_CurrentTime) * 63.75f));
        state[1].m_nSpeed    = static_cast<uint8>(ReplayFtol(ClampSpeed(second->m_Speed) * 85.0f));
        state[1].m_nGroupId1 = static_cast<uint8>(ReplayFtol(ClampBlend(blendValue) * 127.5f));
        state[1].m_nGroupId2 = static_cast<uint8>(second->m_AnimGroupId);
    } else {
        state[1].m_nAnimId   = 0;
        state[1].m_nTime     = 0;
        state[1].m_nSpeed    = 0;
        state[1].m_nGroupId1 = 0;
        state[1].m_nGroupId2 = 0;
    }

    const auto partial = RpAnimBlendClumpGetMainPartialAssociation(ped->GetRpClump());
    if (partial && static_cast<int16>(partial->m_AnimId) >= 0) {
        state[2].m_nAnimId   = static_cast<int16>(partial->m_AnimId);
        state[2].m_nTime     = static_cast<uint8>(ReplayFtol(ClampTime(partial->m_CurrentTime) * 63.75f));
        state[2].m_nSpeed    = static_cast<uint8>(ReplayFtol(ClampSpeed(partial->m_Speed) * 85.0f));
        state[2].m_nGroupId1 = static_cast<uint8>(ReplayFtol(ClampBlend(partial->m_BlendAmount) * 127.5f));
        state[2].m_nGroupId2 = static_cast<uint8>(partial->m_AnimGroupId);
    } else {
        state[2].m_nAnimId   = 0;
        state[2].m_nTime     = 0;
        state[2].m_nSpeed    = 0;
        state[2].m_nGroupId1 = 0;
        state[2].m_nGroupId2 = 0;
    }
}

// 0x45C940
void CReplay::StorePedUpdate(CPed* ped, uint8 index) {
    // Como el original: escribe en el buffer sin tocar el relleno ni los bits altos del byte de flags.
    auto* p = ReplayRecordPtr();
    auto* e = reinterpret_cast<uint8*>(ped);

    p[1] = index;
    p[0] = REPLAY_PACKET_PED_UPDATE;
    p[2] = static_cast<uint8>(ReplayFtol(ped->m_fCurrentRotation * HEADING_COMPRESS_VALUE));
    reinterpret_cast<CCompressedMatrixNotAligned*>(p + 0x18)->CompressFromFullMatrix(ped->GetMatrix());
    ReplayAt<uint16>(p, 0x2E) = ReplayAt<uint16>(e, 0x4D4);
    p[0x30] = static_cast<uint8>(ReplayFtol(ped->m_fContactSurfaceBrightness * 100.0f));

    const uint32 entityFlags = ReplayAt<uint32>(e, 0x1C);
    uint8 flags = p[0x31];
    flags = static_cast<uint8>((flags & ~1u) | ((entityFlags >> 7) & 1u));   // m_bIsVisible
    flags = static_cast<uint8>((flags & ~2u) | (((entityFlags >> 30) & 1u) << 1)); // m_bTunnel
    flags = static_cast<uint8>((flags & ~4u) | ((entityFlags >> 31) << 2));  // m_bTunnelTransition
    p[0x31] = flags;
    if (ped == FindPlayerPed(-1) && gbFirstPersonRunThisFrame) {
        p[0x31] &= ~1u;
    }

    if (ped->bInVehicle && ped->m_pVehicle) {
        p[3] = static_cast<uint8>(ReplayVehicleIndex(ped->m_pVehicle) + 1);
    } else {
        p[3] = 0;
    }
    ReplayAt<uint16>(p, 0x2C) = ReplayAt<uint16>(e, 0x740);
    StorePedAnimation(ped, *reinterpret_cast<CStoredAnimationState*>(p + 4));
    Record.m_nOffset += 0x34;
}

// 0x45B4D0
void CReplay::RetrievePedAnimation(CPed* ped, const CStoredAnimationState& state) {
    // Como el original: los factores son multiplicaciones por 1/63,75, 1/85 y 1/127,5 (floats del EXE)
    // y los identificadores de animación se extienden sin signo.
    constexpr auto ANIM_TIME_DECOMPRESS  = 1.0f / 63.75f; // 0x859C2C
    constexpr auto ANIM_SPEED_DECOMPRESS = 1.0f / 85.0f;  // 0x859C28
    constexpr auto ANIM_BLEND_DECOMPRESS = 1.0f / 127.5f; // 0x859C24

    const auto ReplayCallbackNone = [](CAnimBlendAssociation* a) {
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(a) + 0x30) = 0;
    };

    const auto& first = state[0];
    const auto firstId = static_cast<uint16>(first.m_nAnimId);
    CAnimBlendAssociation* anim;
    if (firstId <= 3u) {
        anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, static_cast<AnimationId>(firstId), 100.0f);
    } else {
        const auto group = first.m_nGroupId1;
        if (auto* block = CAnimManager::GetAssocGroups().data()[group].m_AnimBlock; block && block->IsLoaded) {
            anim = CAnimManager::BlendAnimation(ped->GetRpClump(), static_cast<AssocGroupId>(group), static_cast<AnimationId>(firstId), 100.0f);
        } else {
            anim = CAnimManager::BlendAnimation(ped->GetRpClump(), static_cast<AssocGroupId>(0), static_cast<AnimationId>(0), 100.0f);
        }
    }
    anim->SetCurrentTime(static_cast<float>(first.m_nTime) * ANIM_TIME_DECOMPRESS);
    anim->m_Speed = static_cast<float>(first.m_nSpeed) * ANIM_SPEED_DECOMPRESS;
    anim->SetBlend(1.0f, 1.0f);
    ReplayCallbackNone(anim);

    const auto& second = state[1];
    if (second.m_nGroupId1 && second.m_nAnimId) {
        const auto secondId = static_cast<uint16>(second.m_nAnimId);
        const auto time  = static_cast<float>(second.m_nTime) * ANIM_TIME_DECOMPRESS;
        const auto speed = static_cast<float>(second.m_nSpeed) * ANIM_SPEED_DECOMPRESS;
        const auto blend = static_cast<float>(second.m_nGroupId1) * ANIM_BLEND_DECOMPRESS;

        CAnimBlendAssociation* a = nullptr;
        bool blended = true;
        if (secondId <= 3u) {
            a = CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, static_cast<AnimationId>(secondId), 100.0f);
        } else if (static_cast<int32>(CAnimManager::GetAssocGroups().data()[second.m_nGroupId2].m_NumAnims) > 0) {
            a = CAnimManager::BlendAnimation(ped->GetRpClump(), static_cast<AssocGroupId>(second.m_nGroupId2), static_cast<AnimationId>(secondId), 100.0f);
        } else {
            blended = false;
        }

        if (blended && a) {
            a->SetCurrentTime(time);
            a->m_Speed = speed;
            a->SetBlend(blend, 1.0f);
            ReplayCallbackNone(a);
        }
    }

    RpAnimBlendClumpRemoveAssociations(ped->GetRpClump(), ANIMATION_IS_PARTIAL);

    const auto& third = state[2];
    if (third.m_nAnimId) {
        const auto thirdId = static_cast<uint16>(third.m_nAnimId);
        const auto time  = static_cast<float>(third.m_nTime) * ANIM_TIME_DECOMPRESS;
        const auto speed = static_cast<float>(third.m_nSpeed) * ANIM_SPEED_DECOMPRESS;
        const auto blend = static_cast<float>(third.m_nGroupId1) * ANIM_BLEND_DECOMPRESS;
        if (blend > 0.0f && thirdId != 3u) {
            if (auto* block = CAnimManager::GetAssocGroups().data()[third.m_nGroupId2].m_AnimBlock; block && block->IsLoaded) {
                auto* a = CAnimManager::BlendAnimation(ped->GetRpClump(), static_cast<AssocGroupId>(third.m_nGroupId2), static_cast<AnimationId>(thirdId), 1000.0f);
                a->SetCurrentTime(time);
                a->m_Speed = speed;
                // Como el original: mezcla y delta escritas a mano, sin tocar el callback
                a->m_BlendAmount = blend;
                a->m_BlendDelta = 0.0f;
            }
        }
    }
}

// 0x45C210
void CReplay::Display() {
    if (Mode == MODE_RECORD)
        return;

    // Como el original: contador de frames de 16 bits; el texto parpadea con el bit 5.
    static auto& s_ReplayTextCounter = StaticRef<uint32>(0xA43240);
    s_ReplayTextCounter = (s_ReplayTextCounter + 1) & 0xFFFF;
    if (!(s_ReplayTextCounter & 0x20))
        return;

    CFont::SetScale(static_cast<float>(RsGlobal.maximumWidth) * (1.0f / 640.0f) * 1.5f, static_cast<float>(RsGlobal.maximumHeight) * (1.0f / 448.0f) * 1.5f);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetBackground(false, false);
    CFont::SetCentreSize(static_cast<float>(RsGlobal.maximumWidth - 20));
    CFont::SetProportional(true);
    CFont::SetColor({ 255u, 255u, 200u, 200u });
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    if (Mode == MODE_PLAYBACK) {
        CFont::PrintString(static_cast<float>(RsGlobal.maximumWidth / 10), static_cast<float>(RsGlobal.maximumHeight / 15), TheText.Get("REPLAY"));
    }
}

// 0x45D430
void CReplay::MarkEverythingAsNew() {
    // Como el original: ambos pools se recorren del último slot al primero.
    auto* pedPool = GetPedPool();
    for (auto i = ReplayPoolSize(pedPool); i > 0; i--) {
        if (auto* ped = ReplayPedAt(i - 1)) {
            ped->bHasAlreadyBeenRecorded = false;
        }
    }

    auto* vehPool = GetVehiclePool();
    for (auto i = ReplayPoolSize(vehPool); i > 0; i--) {
        if (auto* veh = ReplayVehicleAt(i - 1)) {
            veh->vehicleFlags.bHasAlreadyBeenRecorded = false;
        }
    }
}

// 0x45EC90
void CReplay::EmptyReplayBuffer() {
    if (Mode == MODE_PLAYBACK)
        return;

    // Como el original: solo se escriben los campos (no el relleno de Record).
    reinterpret_cast<uint32*>(BufferStatus.data())[0] = 0;
    Record.m_nOffset = 0;
    reinterpret_cast<uint32*>(BufferStatus.data())[1] = 0;
    Record.m_bSlot = 0;
    Record.m_pBase = &Buffers.data()[0];
    BufferStatus.data()[0] = REPLAYBUFFER_IN_USE;
    ReplayBufferPtr(0)[0] = REPLAY_PACKET_END;
    MarkEverythingAsNew();
}

// 0x45D390
void CReplay::EmptyPedsAndVehiclePools_NoDestructors() {
    // Como el original: del último slot al primero.
    for (auto i = ReplayPoolSize(GetVehiclePool()); i > 0; i--) {
        if (auto* veh = ReplayVehicleAt(i - 1)) {
            CWorld::Remove(veh);
        }
    }

    for (auto i = ReplayPoolSize(GetPedPool()); i > 0; i--) {
        if (auto* ped = ReplayPedAt(i - 1)) {
            CWorld::Remove(ped);
            if (auto* const shadow = ped->m_pShadowData) {
                g_realTimeShadowMan.ReturnRealTimeShadow(shadow);
            }
        }
    }
}

// 0x45E2A0
void CReplay::GoToNextBlock() {
    reinterpret_cast<uint8*>(Record.m_pBase)[Record.m_nOffset] = REPLAY_PACKET_END;
    BufferStatus.data()[Record.m_bSlot] = REPLAYBUFFER_FULL;
    const auto slot = static_cast<uint8>((Record.m_bSlot + 1) % NUM_REPLAY_BUFFERS);
    Record.m_bSlot = slot;
    BufferStatus.data()[slot] = REPLAYBUFFER_IN_USE;
    Record.m_pBase = &Buffers.data()[slot];
    Record.m_nOffset = 0;
    ReplayBufferPtr(slot)[0] = REPLAY_PACKET_END;
    MarkEverythingAsNew();
}

// 0x45EBB0
void CReplay::RecordVehicleDeleted(CVehicle* vehicle) {
    if (Mode != MODE_RECORD)
        return;

    if (Record.m_nOffset + FindSizeOfPacket(REPLAY_PACKET_DELETED_VEH) > 99'984) {
        GoToNextBlock();
    }

    // Como el original: el byte 1 (relleno) no se escribe.
    auto* p = ReplayRecordPtr();
    p[0] = REPLAY_PACKET_DELETED_VEH;
    ReplayAt<int16>(p, 2) = static_cast<int16>(ReplayVehicleIndex(vehicle));
    Record.m_nOffset += 4;
    ReplayRecordPtr()[0] = REPLAY_PACKET_END;
}

// 0x45EC20
void CReplay::RecordPedDeleted(CPed* ped) {
    if (Mode != MODE_RECORD)
        return;

    if (Record.m_nOffset + FindSizeOfPacket(REPLAY_PACKET_DELETED_PED) > 99'984) {
        GoToNextBlock();
    }

    auto* p = ReplayRecordPtr();
    p[0] = REPLAY_PACKET_DELETED_PED;
    ReplayAt<int16>(p, 2) = static_cast<int16>(ReplayPedIndex(ped));
    Record.m_nOffset += 4;
    ReplayRecordPtr()[0] = REPLAY_PACKET_END;
}

// 0x45EFA0
void CReplay::InitialiseVehiclePoolConversionTable() {
    // Como el original: recorre 110 slots fijos; tras asignar no comprueba el límite y la búsqueda
    // interna solo para cuando el índice vale exactamente 110 (puede escribir más allá de la tabla).
    auto* table = m_VehiclePoolConversion.data();
    for (auto i = 0; i < 110; i++) {
        table[i] = -1;
    }

    int32 convIdx = 0;
    for (int32 poolIdx = 0; poolIdx < 110; poolIdx++) {
        if (ReplayVehicleAt(poolIdx))
            continue;

        if (!IsThisVehicleUsedInRecording(convIdx)) {
            bool found = false;
            for (;;) {
                if (++convIdx == 110)
                    break;
                if (IsThisVehicleUsedInRecording(convIdx)) {
                    found = true;
                    break;
                }
            }
            if (!found)
                return;
        }
        table[convIdx++] = poolIdx;
    }
}

// 0x45EF20
void CReplay::InitialisePedPoolConversionTable() {
    // Como el original: recorre 140 slots fijos y termina en cuanto el índice llega a 140.
    auto* table = m_PedPoolConversion.data();
    for (auto i = 0; i < 140; i++) {
        table[i] = -1;
    }

    int32 convIdx = 0;
    for (int32 poolIdx = 0; poolIdx < 140; poolIdx++) {
        if (ReplayPedAt(poolIdx))
            continue;

        if (!IsThisPedUsedInRecording(convIdx)) {
            do {
                if (++convIdx >= 140)
                    return;
            } while (!IsThisPedUsedInRecording(convIdx));
        }
        table[convIdx++] = poolIdx;
        if (convIdx >= 140)
            return;
    }
}

// 0x45F370
void CReplay::InitialisePoolConversionTables() {
    // Como el original: primero peatones, luego vehículos.
    InitialisePedPoolConversionTable();
    InitialiseVehiclePoolConversionTable();
}

// 0x45C340
void CReplay::SaveReplayToHD() {
    CFileMgr::SetDirMyDocuments();

    // Como el original: el handle solo se descarta si es negativo.
    auto file = CFileMgr::OpenFileForWriting("replay.rep");
    if (reinterpret_cast<int32>(file) >= 0) {
        CFileMgr::Write(file, "GtaSA29", 8u);

        int32 inUse = 0;
        while (inUse < 8 && BufferStatus.data()[inUse] != REPLAYBUFFER_IN_USE) {
            inUse++;
        }

        auto slot = (inUse + 1) % 8;
        while (BufferStatus.data()[slot] != REPLAYBUFFER_IN_USE && BufferStatus.data()[slot] != REPLAYBUFFER_FULL) {
            slot = (slot + 1) % 8;
        }
        CFileMgr::Write(file, ReplayBufferPtr(slot), sizeof(tReplayBuffer));

        while (BufferStatus.data()[slot] != REPLAYBUFFER_IN_USE) {
            slot = (slot + 1) % 8;
            CFileMgr::Write(file, ReplayBufferPtr(slot), sizeof(tReplayBuffer));
        }
        CFileMgr::CloseFile(file);
    }
    CFileMgr::SetDir("");
}

// 0x460390
void CReplay::PlayReplayFromHD() {
    CFileMgr::SetDirMyDocuments();
    if (auto file = CFileMgr::OpenFile("replay.rep", "rb")) {
        CFileMgr::Read(file, gString, 8u);

        if (strncmp(gString, "GtaSA29", 8u) != 0) {
            CFileMgr::CloseFile(file);
            CFileMgr::SetDir("");
            return;
        }

        // Como el original: no hay límite de 8 buffers; se lee hasta que Read devuelve 0.
        int32 bufferIdx = 0;
        if (CFileMgr::Read(file, ReplayBufferPtr(0), sizeof(tReplayBuffer))) {
            do {
                BufferStatus.data()[bufferIdx] = REPLAYBUFFER_FULL;
                bufferIdx++;
            } while (CFileMgr::Read(file, ReplayBufferPtr(bufferIdx), sizeof(tReplayBuffer)));
        }
        BufferStatus.data()[bufferIdx - 1] = REPLAYBUFFER_IN_USE;
        for (auto i = bufferIdx; i < 8; i++) {
            BufferStatus.data()[i] = REPLAYBUFFER_NOT_AVAILABLE;
        }

        CFileMgr::CloseFile(file);
        CFileMgr::SetDir("");
        TriggerPlayback(REPLAY_CAM_MODE_AS_STORED, CVector{0.0f, 0.0f, 0.0f}, false);
        bPlayingBackFromFile = true;
        bAllowLookAroundCam = true;
        StreamAllNecessaryCarsAndPeds();
    }
    CFileMgr::SetDir("");
}

// 0x45C440
bool CReplay::ShouldStandardCameraBeProcessed() {
    return Mode != MODE_PLAYBACK;
}

// inlined
// 0x45C450
int32 CReplay::FindPoolIndexForPed(int32 index) {
    return m_PedPoolConversion.data()[index];
}

// inlined
// 0x45C460
int32 CReplay::FindPoolIndexForVehicle(int32 index) {
    return m_VehiclePoolConversion.data()[index];
}

// 0x45CA70
void CReplay::ProcessPedUpdate(CPed* ped, float interpValue, CAddressInReplayBuffer& address) {
    auto* p = reinterpret_cast<uint8*>(address.m_pBase) + address.m_nOffset;
    if (ped) {
        auto* e = reinterpret_cast<uint8*>(ped);
        const auto heading = static_cast<float>(static_cast<int8>(p[2]));
        ped->m_fCurrentRotation = heading * REPLAY_HEADING_DECOMPRESS;
        ped->m_fAimingRotation  = heading * REPLAY_HEADING_DECOMPRESS;

        // Como el original: m = m * (1 - t) + d * t con CMatrix::ScaleAll (0x45AF40) y operator+= (0x59ADF0).
        CMatrix decompressed;
        reinterpret_cast<CCompressedMatrixNotAligned*>(p + 0x18)->DecompressIntoFullMatrix(decompressed);
        CMatrix current(ped->GetMatrix());
        current.ScaleAll(1.0f - interpValue);
        decompressed.ScaleAll(interpValue);
        current += decompressed;
        ped->GetMatrix() = current;

        if (const auto vehIdx = p[3]) {
            if (ped->m_pVehicle) {
                ped->m_pVehicle->CleanUpOldReference(reinterpret_cast<CEntity**>(&ped->m_pVehicle));
            }
            ped->m_pVehicle = ReplayVehicleAt(m_VehiclePoolConversion.data()[vehIdx - 1]);
            if (ped->m_pVehicle) {
                ped->m_pVehicle->RegisterReference(reinterpret_cast<CEntity**>(&ped->m_pVehicle));
                ped->bInVehicle = true;
            }
        } else {
            ped->bInVehicle = false;
            if (ped->m_pVehicle) {
                ped->m_pVehicle->CleanUpOldReference(reinterpret_cast<CEntity**>(&ped->m_pVehicle));
            }
            ped->m_pVehicle = nullptr;
        }

        if (const int32 ag = ReplayAt<int16>(p, 0x2E); ag != ReplayAt<int32>(e, 0x4D4)) {
            ReplayAt<int32>(e, 0x4D4) = ag;
            if (ped == FindPlayerPed(-1)) {
                ped->AsPlayer()->ReApplyMoveAnims();
            }
        }

        // Como el original: los bits del paquete son m_bIsVisible, m_bTunnel y m_bTunnelTransition.
        auto& entityFlags = ReplayAt<uint32>(e, 0x1C);
        const uint32 bits = p[0x31];
        entityFlags = (entityFlags & ~0x80u) | ((bits & 1u) << 7);
        entityFlags = (entityFlags & ~0x40000000u) | (((bits >> 1) & 1u) << 30);
        entityFlags = (entityFlags & ~0x80000000u) | (((bits >> 2) & 1u) << 31);
        ped->m_fContactSurfaceBrightness = static_cast<float>(p[0x30]) * 0.01f;
        RetrievePedAnimation(ped, *reinterpret_cast<const CStoredAnimationState*>(p + 4));

        ped->RemoveWeaponModel(-1);
        if (const int16 wm = ReplayAt<int16>(p, 0x2C); wm != -1) {
            if (CStreaming::ms_aInfoForModel.data()[wm].m_LoadState == eStreamingLoadState::LOADSTATE_LOADED) {
                ped->AddWeaponModel(wm);
            } else {
                CStreaming::RequestModel(wm, 0);
            }
        }

        e[0x2F] = static_cast<uint8>(CGame::currArea);
        CWorld::Remove(ped);
        CWorld::Add(ped);
    }
    address.m_nOffset += 0x34;
}

// 0x45D060
void CReplay::ProcessReplayCamera() {
    // Como el original: escribe en TheCamera.m_mCameraMatrix (+0x974), no en la matriz del CPlaceable.
    auto* frame = RwCameraGetFrame(TheCamera.m_pRwCamera);
    auto* modelling = RwFrameGetMatrix(frame);
    auto& camMat = TheCamera.m_mCameraMatrix;

    switch (CameraMode) {
    case REPLAY_CAM_MODE_TOPDOWN:
        camMat.GetPosition() = CVector{CameraFocus.x, CameraFocus.y, CameraFocus.z + 15.0f};
        camMat.GetForward()  = CVector{0.0f, 0.0f, -1.0f};
        camMat.GetUp()       = CVector{0.0f, 1.0f, 0.0f};
        camMat.GetRight()    = CVector{1.0f, 0.0f, 0.0f};
        break;
    case REPLAY_CAM_MODE_FIXED: {
        CVector direction{CameraFocus.x - CameraFixed.x, CameraFocus.y - CameraFixed.y, CameraFocus.z - CameraFixed.z};
        camMat.GetPosition() = CVector{CameraFixed.x, CameraFixed.y, CameraFixed.z};
        direction.Normalise();

        CVector up0{0.0f, 0.0f, 1.0f};
        CVector right;
        CrossProduct(&right, &up0, &direction);
        right.Normalise();

        CVector up;
        CrossProduct(&up, &direction, &right);
        up.Normalise();

        camMat.GetForward() = direction;
        camMat.GetUp()      = up;
        camMat.GetRight()   = right;
        break;
    }
    default:
        TheCamera.m_vecGameCamPos = TheCamera.GetPosition();
        TheCamera.CalculateDerivedValues(false, true);
        RwMatrixUpdate(modelling);
        RwFrameUpdateObjects(frame);
        RwFrameOrthoNormalize(frame);
        return;
    }

    modelling->pos   = camMat.GetPosition();
    modelling->at    = camMat.GetForward();
    modelling->up    = camMat.GetUp();
    modelling->right = camMat.GetRight();

    TheCamera.m_vecGameCamPos = TheCamera.GetPosition();
    TheCamera.CalculateDerivedValues(false, true);
    RwMatrixUpdate(modelling);
    RwFrameUpdateObjects(frame);
    RwFrameOrthoNormalize(frame);
}

// 0x45D760
void CReplay::ProcessLookAroundCam() {
    if (bAllowLookAroundCam) {
        static auto& playerCameraDistance = StaticRef<float>(0x97FAD4);
        static auto& viewAngle            = StaticRef<float>(0x97FAD8);
        static auto& playerCameraDirAngle = StaticRef<float>(0x97FADC);

        // Como el original: el ratón se escala multiplicando por 0,005 y basta con que |x| o |y| supere 0,01.
        const float mouseX = CPad::NewMouseControllerState.m_AmountMoved.x * 0.005f;
        const float mouseY = CPad::NewMouseControllerState.m_AmountMoved.y * 0.005f;

        if (std::fabs(mouseX) > 0.01f || std::fabs(mouseY) > 0.01f) {
            if (!FramesActiveLookAroundCam) {
                // Como el original: tres llamadas a FindPlayerCoors; distancia con orden z, y, x.
                const CVector& camPos = TheCamera.GetPosition();
                const CVector player = FindPlayerCoors(-1);
                const float dX = player.x - camPos.x;
                const float dY = player.y - camPos.y;
                const float dZ = player.z - camPos.z;
                float dist = std::sqrt(dZ * dZ + dY * dY + dX * dX);
                if (!(dist < 15.0f)) {
                    dist = 15.0f;
                } else if (!(dist > 3.0f)) {
                    dist = 3.0f;
                }
                playerCameraDistance = dist;

                const float diffY = camPos.y - FindPlayerCoors(-1).y;
                const float diffX = camPos.x - FindPlayerCoors(-1).x;
                float angle;
                __asm {
                    fld   diffX
                    fld   diffY
                    fpatan
                    fstp  angle
                }
                playerCameraDirAngle = angle;
            }
            FramesActiveLookAroundCam = 60;
        }

        if (bIsUsingRemoteCar) {
            FramesActiveLookAroundCam = 0;
        } else if (FramesActiveLookAroundCam) {
            playerCameraDirAngle = playerCameraDirAngle + mouseX;
            FramesActiveLookAroundCam = FramesActiveLookAroundCam - 1;

            if (CPad::NewMouseControllerState.isMouseLeftButtonPressed && CPad::NewMouseControllerState.isMouseRightButtonPressed) {
                float dist = (mouseY + mouseY) + playerCameraDistance;
                if (!(dist < 15.0f)) {
                    dist = 15.0f;
                } else if (!(dist > 3.0f)) {
                    dist = 3.0f;
                }
                playerCameraDistance = dist;
            } else {
                float view = viewAngle + mouseY;
                if (0.1f > view) {
                    view = 0.1f;
                } else if (1.5f < view) {
                    view = 1.5f;
                }
                viewAngle = view;
            }

            // Como el original: fsin/fcos del x87 a precisión completa; los productos se redondean a float.
            const float dirAngle = playerCameraDirAngle;
            const float view     = viewAngle;
            const float dist     = playerCameraDistance;
            float offX, offY, offZ;
            __asm {
                fld   view
                fcos
                fld   dirAngle
                fsin
                fmul  st, st(1)
                fmul  dist
                fstp  offX
                fld   dirAngle
                fcos
                fmul  st, st(1)
                fmul  dist
                fstp  offY
                fstp  st(0)
                fld   view
                fsin
                fmul  dist
                fstp  offZ
            }

            const CVector focus{CameraFocus.x, CameraFocus.y, CameraFocus.z};
            CVector target{offX + focus.x, offY + focus.y, offZ + focus.z};

            CColPoint colPoint;
            CEntity* outEntity = nullptr;
            if (CWorld::ProcessLineOfSight(focus, target, colPoint, outEntity, true, false, false, false, false, true, true, false)) {
                target = colPoint.m_vecPoint;
                CVector dir{focus.x - colPoint.m_vecPoint.x, focus.y - colPoint.m_vecPoint.y, focus.z - colPoint.m_vecPoint.z};
                dir.Normalise();
                target.x = dir.x * 0.25f + target.x;
                target.y = dir.y * 0.25f + target.y;
                target.z = dir.z * 0.25f + target.z;
            }

            CVector forward{focus.x - target.x, focus.y - target.y, focus.z - target.z};
            forward.Normalise();

            CVector up0{0.0f, 0.0f, 1.0f};
            CVector right;
            CrossProduct(&right, &up0, &forward);
            right.Normalise();

            CVector up;
            CrossProduct(&up, &forward, &right);
            up.Normalise();

            // Como el original: se escribe TheCamera.m_mCameraMatrix y luego la matriz del frame.
            auto& camMat = TheCamera.m_mCameraMatrix;
            camMat.GetForward()  = forward;
            camMat.GetUp()       = up;
            camMat.GetRight()    = right;
            camMat.GetPosition() = target;

            auto* frame = RwCameraGetFrame(TheCamera.m_pRwCamera);
            auto* modelling = RwFrameGetMatrix(frame);
            modelling->pos   = camMat.GetPosition();
            modelling->at    = camMat.GetForward();
            modelling->up    = camMat.GetUp();
            modelling->right = camMat.GetRight();
            TheCamera.CalculateDerivedValues(false, true);
            RwMatrixUpdate(modelling);
            RwFrameUpdateObjects(frame);
            RwFrameOrthoNormalize(frame);
        }
    }

    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.5f);
    if (FramesActiveLookAroundCam) {
        // Como el original: cada Find* se llama dos veces.
        if (FindPlayerPed(-1)) {
            FindPlayerPed(-1)->m_bIsVisible = true;
        }
        if (FindPlayerVehicle(-1, false)) {
            FindPlayerVehicle(-1, false)->m_bIsVisible = true;
        }
    }
}

// inlined
// 0x45C470
bool CReplay::CanWeFindPoolIndexForPed(int32 index) {
    return FindPoolIndexForPed(index) >= 0;
}

// inlined
// 0x45C490
bool CReplay::CanWeFindPoolIndexForVehicle(int32 index) {
    return FindPoolIndexForVehicle(index) >= 0;
}

// 0x45F020
void CReplay::StorePlayerInfoVariables() {
    auto& playerInfo = CWorld::Players[0];

    PlayerInfo = playerInfo;
    playerInfo.m_PlayerData.m_pWanted = nullptr;
    playerInfo.m_PlayerData.m_pPedClothesDesc = nullptr;
    playerInfo.m_PlayerData.m_pArrestingCop = nullptr;
    playerInfo.m_pSkinTexture = nullptr;
    playerInfo.m_PlayerData.m_nMeleeWeaponAnimReferenced = 0;
    playerInfo.m_PlayerData.m_nMeleeWeaponAnimReferencedExtra = 0;
}

// 0x45E1F0
void CReplay::RestorePlayerInfoVariables() {
    CWorld::Players[0] = PlayerInfo;
    PlayerInfo.m_PlayerData.m_pWanted = nullptr;
    PlayerInfo.m_PlayerData.m_pPedClothesDesc = nullptr;
    PlayerInfo.m_PlayerData.m_pArrestingCop = nullptr;
    PlayerInfo.m_pSkinTexture = nullptr;
}

// 0x45F180
void CReplay::StoreStuffInMem() {
    // Como el original: memoria cruda (sin constructor de CCamera) y copia de 0xD78 bytes.
    static_assert(sizeof(CCamera) == 0xD78);
    pStoredCam = static_cast<CCamera*>(::operator new(sizeof(CCamera)));
    memcpy(pStoredCam, &TheCamera, sizeof(CCamera));

    StorePlayerInfoVariables();
    Time1 = CTimer::GetTimeInMS();
    Time2 = CTimer::GetTimeInMSNonClipped();
    Time3 = CTimer::GetPreviousTimeInMS();
    Time4 = CTimer::m_snPPreviousTimeInMilliseconds;
    Time5 = CTimer::m_snPPPreviousTimeInMilliseconds;
    Time6 = CTimer::m_snPPPPreviousTimeInMilliseconds;
    Time7 = CTimer::GetTimeInMSPauseMode();
    Frame = CTimer::GetFrameCounter();
    TimeScale = CTimer::GetTimeScale();
    TimeStep = CTimer::GetTimeStep();
    TimeStepNonClipped = CTimer::GetTimeStepNonClipped();

    ClockHours = CClock::GetGameClockHours();
    ClockMinutes = CClock::GetGameClockMinutes();

    OldWeatherType = CWeather::OldWeatherType;
    NewWeatherType = CWeather::NewWeatherType;
    WeatherInterpolationValue = CWeather::InterpolationValue;

    ms_nNumCivMale_Stored = CPopulation::ms_nNumCivMale;
    ms_nNumCivFemale_Stored = CPopulation::ms_nNumCivFemale;
    ms_nNumCop_Stored = CPopulation::ms_nNumCop;
    ms_nNumEmergency_Stored = CPopulation::ms_nNumEmergency;
    ms_nNumDealers_Stored = CPopulation::ms_nNumDealers;
    rng::copy(CPopulation::ms_nNumGang, ms_nNumGang_Stored.begin());
    ms_nTotalCarPassengerPeds_Stored = CPopulation::ms_nTotalCarPassengerPeds;
    ms_nTotalCivPeds_Stored = CPopulation::ms_nTotalCivPeds;
    ms_nTotalGangPeds_Stored = CPopulation::ms_nTotalGangPeds;
    ms_nTotalPeds_Stored = CPopulation::ms_nTotalPeds;
    ms_nTotalMissionPeds_Stored = CPopulation::ms_nTotalMissionPeds;
}

// 0x45ECD0
void CReplay::RestoreStuffFromMem() {
    for (int32 i = 0; i < ReplayPoolSize(GetPedPool()); i++) {
        auto* ped = ReplayPedAt(i);
        if (!ped)
            continue;

        if (ped->bUsedForReplay) {
            if (auto playerData = ped->GetPlayerData()) {
                playerData->DeAllocateData();
            }
            CWorld::Remove(ped);
            delete ped;
        } else { // if not, restore it
            CWorld::Add(ped);
        }
    }

    for (int32 i = 0; i < ReplayPoolSize(GetVehiclePool()); i++) {
        auto* veh = ReplayVehicleAt(i);
        if (!veh)
            continue;

        if (veh->vehicleFlags.bUsedForReplay) {
            CWorld::Remove(veh);
            delete veh;
        } else { // if not, restore it
            CWorld::Add(veh);
        }
    }

    // Como el original: copia cruda y operator delete sin destructor de CCamera.
    memcpy(&TheCamera, pStoredCam, sizeof(CCamera));
    ::operator delete(pStoredCam);
    pStoredCam = nullptr;

    RestorePlayerInfoVariables();

    CTimer::SetTimeInMS(Time1);
    CTimer::SetTimeInMSNonClipped(Time2);
    CTimer::SetPreviousTimeInMS(Time3);
    CTimer::m_snPPreviousTimeInMilliseconds = Time4;
    CTimer::m_snPPPreviousTimeInMilliseconds = Time5;
    CTimer::m_snPPPPreviousTimeInMilliseconds = Time6;
    CTimer::SetTimeInMSPauseMode(Time7);
    CTimer::SetTimeStep(TimeStep);
    CTimer::SetTimeStepNonClipped(TimeStepNonClipped);
    CTimer::SetTimeScale(TimeScale);
    CTimer::SetFrameCounter(Frame);
    ClockMinutes = ClockMinutes + 1;
    CClock::SetGameClock(ClockHours, ClockMinutes, 0);

    CWeather::OldWeatherType = OldWeatherType;
    CWeather::NewWeatherType = NewWeatherType;
    CWeather::InterpolationValue = WeatherInterpolationValue;

    // Como el original: el área se extiende sin signo.
    CGame::currArea = static_cast<eAreaCodes>(static_cast<uint8>(CurrArea));
    CPopulation::ms_nNumCivMale = ms_nNumCivMale_Stored;
    CPopulation::ms_nNumCivFemale = ms_nNumCivFemale_Stored;
    CPopulation::ms_nNumCop = ms_nNumCop_Stored;
    CPopulation::ms_nNumEmergency = ms_nNumEmergency_Stored;
    CPopulation::ms_nTotalCarPassengerPeds = ms_nTotalCarPassengerPeds_Stored;
    CPopulation::ms_nTotalCivPeds = ms_nTotalCivPeds_Stored;
    CPopulation::ms_nTotalGangPeds = ms_nTotalGangPeds_Stored;
    CPopulation::ms_nTotalPeds = ms_nTotalPeds_Stored;
    CPopulation::ms_nTotalMissionPeds = ms_nTotalMissionPeds_Stored;
    // Como el original: ni los camellos ni las bandas se restauran.
}

// 0x45F050
void CReplay::FinishPlayback() {
    if (Mode != MODE_PLAYBACK)
        return;

    RestoreStuffFromMem();
    Mode = MODE_RECORD;

    if (bDoLoadSceneWhenDone) {
        const CVector point{LoadScene.x, LoadScene.y, LoadScene.z};
        CGame::currLevel = static_cast<eLevelName>(CTheZones::GetLevelFromPosition(point));
        CCollision::SortOutCollisionAfterLoad();
        CStreaming::LoadScene(point);
    }
    bDoLoadSceneWhenDone = false;

    if (bPlayingBackFromFile) {
        Init();
        MarkEverythingAsNew();
    }

    TheCamera.SetFadeColour(0, 0, 0);
    TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
    TheCamera.ProcessFade();
    TheCamera.Fade(1.0f, eFadeFlag::FADE_OUT);

    gFireManager.CreateAllFxSystems();
    AudioEngine.SetEffectsMasterVolume(FrontEndMenuManager.m_nSfxVolume);
    AudioEngine.SetMusicMasterVolume(FrontEndMenuManager.m_nRadioVolume);

    // Como el original: FindPlayerVehicle se llama dos veces.
    if (FindPlayerVehicle(-1, false)) {
        FindPlayerVehicle(-1, false)->m_vehicleAudio.TurnOnRadioForVehicle();
        AudioEngine.RetuneRadio(static_cast<eRadioID>(static_cast<uint8>(OldRadioStation)));
    }
}

// 0x45E300
void CReplay::RecordThisFrame() {
    auto* vehPool = GetVehiclePool();
    auto* pedPool = GetPedPool();

    // Calculate the frame size beforehand.
    uint32 framePacketSize = 0x74;
    for (int32 i = 0; i < ReplayPoolSize(vehPool); i++) {
        auto* veh = ReplayVehicleAt(i);
        if (!veh || !veh->GetRwObject())
            continue;

        switch (ReplayAt<uint32>(reinterpret_cast<uint8*>(veh), 0x594)) {
        case VEHICLE_TYPE_AUTOMOBILE:
        case VEHICLE_TYPE_MTRUCK:
        case VEHICLE_TYPE_QUAD:
        case VEHICLE_TYPE_BOAT:
        case VEHICLE_TYPE_TRAILER:
            framePacketSize += 0x34;
            break;
        case VEHICLE_TYPE_HELI:
        case VEHICLE_TYPE_BIKE:
        case VEHICLE_TYPE_BMX:
            framePacketSize += 0x38;
            break;
        case VEHICLE_TYPE_PLANE:
            framePacketSize += 0x3C;
            break;
        case VEHICLE_TYPE_TRAIN:
            framePacketSize += 0x4C;
            break;
        default:
            break;
        }
    }

    for (int32 i = 0; i < ReplayPoolSize(pedPool); i++) {
        auto* ped = ReplayPedAt(i);
        if (!ped || !ped->GetRwObject())
            continue;

        if (!ped->bHasAlreadyBeenRecorded) {
            framePacketSize += 8;
            if (ped->IsPlayer()) {
                framePacketSize += 0x78;
            }
        }
        framePacketSize += 0x34;
    }

    for (auto& trace : CBulletTraces::aTraces) {
        if (trace.m_bExists) {
            framePacketSize += 0x1C;
        }
    }

    if (Record.m_nOffset + framePacketSize + 20 > 99'984) {
        // writing the frame will overflow the current buffer, switch to next.
        GoToNextBlock();
    }

    // Como el original: cada paquete se escribe en el buffer sin tocar su relleno.
    auto* p = ReplayRecordPtr();
    p[0] = REPLAY_PACKET_GENERAL;
    reinterpret_cast<CMatrix*>(p + 4)->CopyOnlyMatrix(TheCamera.m_mCameraMatrix);
    ReplayAt<float>(p, 0x4C) = FindPlayerCoors(-1).x;
    ReplayAt<float>(p, 0x50) = FindPlayerCoors(-1).y;
    ReplayAt<float>(p, 0x54) = FindPlayerCoors(-1).z;
    p[1] = CWorld::Players[CWorld::PlayerInFocus].m_pRemoteVehicle != nullptr;
    Record.m_nOffset += 0x58;

    p = ReplayRecordPtr();
    p[0] = REPLAY_PACKET_CLOCK;
    p[1] = CClock::ms_nGameClockHours;
    p[2] = CClock::ms_nGameClockMinutes;
    Record.m_nOffset += 4;

    p = ReplayRecordPtr();
    p[0] = REPLAY_PACKET_WEATHER;
    p[1] = static_cast<uint8>(CWeather::OldWeatherType);
    p[2] = static_cast<uint8>(CWeather::NewWeatherType);
    ReplayAt<float>(p, 4) = CWeather::InterpolationValue;
    Record.m_nOffset += 8;

    p = ReplayRecordPtr();
    p[0] = REPLAY_PACKET_TIMER;
    ReplayAt<uint32>(p, 4) = CTimer::GetTimeInMS();
    Record.m_nOffset += 8;

    for (int32 i = 0; i < ReplayPoolSize(vehPool); i++) {
        auto* veh = ReplayVehicleAt(i);
        if (!veh || !veh->GetRwObject())
            continue;

        auto* v = reinterpret_cast<uint8*>(veh);
        switch (ReplayAt<uint32>(v, 0x594)) {
        case VEHICLE_TYPE_AUTOMOBILE:
        case VEHICLE_TYPE_MTRUCK:
        case VEHICLE_TYPE_QUAD:
        case VEHICLE_TYPE_BOAT:
        case VEHICLE_TYPE_TRAILER:
            p = ReplayRecordPtr();
            p[0] = REPLAY_PACKET_VEHICLE;
            ReplayStoreCarUpdate(p, veh, i);
            Record.m_nOffset += 0x34;
            break;
        case VEHICLE_TYPE_HELI:
            p = ReplayRecordPtr();
            p[0] = REPLAY_PACKET_HELI;
            ReplayStoreCarUpdate(p, veh, i);
            ReplayAt<uint32>(p, 0x34) = ReplayAt<uint32>(v, 0x84C);
            Record.m_nOffset += 0x38;
            break;
        case VEHICLE_TYPE_PLANE:
            p = ReplayRecordPtr();
            p[0] = REPLAY_PACKET_PLANE;
            ReplayStoreCarUpdate(p, veh, i);
            ReplayAt<uint32>(p, 0x38) = ReplayAt<uint32>(v, 0x9C8);
            ReplayAt<uint32>(p, 0x34) = ReplayAt<uint32>(v, 0x9C4);
            Record.m_nOffset += 0x3C;
            break;
        case VEHICLE_TYPE_TRAIN:
            ReplayStoreTrainUpdate(ReplayRecordPtr(), veh, i);
            Record.m_nOffset += 0x4C;
            break;
        case VEHICLE_TYPE_BIKE:
            ReplayStoreBikeUpdate(ReplayRecordPtr(), veh, i, REPLAY_PACKET_BIKE);
            Record.m_nOffset += 0x38;
            break;
        case VEHICLE_TYPE_BMX:
            ReplayStoreBikeUpdate(ReplayRecordPtr(), veh, i, REPLAY_PACKET_BMX);
            Record.m_nOffset += 0x38;
            break;
        default:
            break;
        }
    }

    for (int32 i = 0; i < ReplayPoolSize(pedPool); i++) {
        auto* ped = ReplayPedAt(i);
        if (!ped || !ped->GetRwObject())
            continue;

        if (!ped->bHasAlreadyBeenRecorded) {
            // New ped!
            p = ReplayRecordPtr();
            p[0] = REPLAY_PACKET_PED_HEADER;
            p[1] = static_cast<uint8>(i);
            const int16 modelId = ReplayAt<int16>(reinterpret_cast<uint8*>(ped), 0x22);
            ReplayAt<int16>(p, 2) = modelId;
            if (modelId >= MODEL_SPECIAL01 && modelId <= MODEL_CUTOBJ01) {
                ReplayAt<int16>(p, 2) = MODEL_MALE01;
            }
            p[4] = reinterpret_cast<uint8*>(ped)[0x598];
            Record.m_nOffset += 8;

            if (ped->IsPlayer()) {
                StoreClothesDesc(*ped->GetClothesDesc(), *reinterpret_cast<tReplayClothesBlock*>(ReplayRecordPtr()));
                Record.m_nOffset += 0x78;
            }
            ped->bHasAlreadyBeenRecorded = true;
        }
        StorePedUpdate(ped, static_cast<uint8>(i));
    }

    for (auto i = 0u; i < 16u; i++) {
        auto* trace = reinterpret_cast<uint8*>(&CBulletTraces::aTraces.data()[i]);
        if (!trace[0x18])
            continue;

        p = ReplayRecordPtr();
        p[0] = REPLAY_PACKET_BULLET_TRACES;
        p[3] = static_cast<uint8>(i);
        memcpy(p + 4, trace, 0x18);
        Record.m_nOffset += 0x1C;
    }

    p = ReplayRecordPtr();
    p[0] = REPLAY_PACKET_MISC;
    ReplayAt<uint32>(p, 4) = TheCamera.m_nCamShakeStart;
    ReplayAt<float>(p, 8) = TheCamera.m_fCamShakeForce;
    uint8 camConfig = p[0xD];
    camConfig = static_cast<uint8>((camConfig & ~1u) | (*reinterpret_cast<uint8*>(&CSpecialFX::bVideoCam) & 1u));
    camConfig = static_cast<uint8>((camConfig & ~2u) | ((*reinterpret_cast<uint8*>(&CSpecialFX::bLiftCam) << 1) & 2u));
    p[0xD] = camConfig;
    p[0xC] = static_cast<uint8>(CGame::currArea);
    Record.m_nOffset += 0x10;

    ReplayRecordPtr()[0] = REPLAY_PACKET_END_OF_FRAME;
    Record.m_nOffset += 4;
    ReplayRecordPtr()[0] = REPLAY_PACKET_END;
}

// 0x45C750
void CReplay::StoreClothesDesc(const CPedClothesDesc& desc, tReplayClothesBlock& packet) {
    packet.type = REPLAY_PACKET_CLOTHES;
    rng::copy(desc.m_anModelKeys, packet.modelKeys.begin());
    rng::copy(desc.m_anTextureKeys, packet.textureKeys.begin());
    packet.fatStat = static_cast<int16>(ReplayFtol(desc.m_fFatStat));
    packet.muscleStat = static_cast<int16>(ReplayFtol(desc.m_fMuscleStat));
}

// 0x45C7D0
void CReplay::RestoreClothesDesc(CPedClothesDesc& desc, tReplayClothesBlock& packet) {
    // Como el original: no comprueba el tipo del paquete; las estadísticas se leen con signo.
    rng::copy(packet.modelKeys, desc.m_anModelKeys.begin());
    rng::copy(packet.textureKeys, desc.m_anTextureKeys.begin());
    desc.m_fFatStat = static_cast<float>(packet.fatStat);
    desc.m_fMuscleStat = static_cast<float>(packet.muscleStat);
}

// 0x45CEA0
CPed* CReplay::DealWithNewPedPacket(const tReplayPedHeaderBlock& pedPacket, bool loadModel, tReplayClothesBlock* clothesPacket) {
    if (ReplayPedAt(m_PedPoolConversion.data()[pedPacket.poolRef]))
        return nullptr;

    if (pedPacket.pedType == PED_TYPE_PLAYER1 && loadModel) {
        CStreaming::RequestModel(pedPacket.modelId, 0);
        CStreaming::LoadAllRequestedModels(false);
    }

    if (CStreaming::ms_aInfoForModel.data()[pedPacket.modelId].m_LoadState == eStreamingLoadState::LOADSTATE_LOADED) {
        CPed* ped = nullptr;
        if (pedPacket.pedType == PED_TYPE_PLAYER1) {
            ped = new (m_PedPoolConversion.data()[pedPacket.poolRef] << 8) CPlayerPed(PED_TYPE_PLAYER1, true);
            RestoreClothesDesc(*ped->GetClothesDesc(), *clothesPacket);
            CClothes::RebuildPlayer(static_cast<CPlayerPed*>(ped), true);
        } else if (!loadModel) {
            ped = new (m_PedPoolConversion.data()[pedPacket.poolRef] << 8) CCivilianPed((ePedType)pedPacket.pedType, pedPacket.modelId);
        } else {
            return nullptr;
        }

        ped->SetStatus(STATUS_PLAYER_PLAYBACK_FROM_BUFFER);
        ped->bUsedForReplay = true;
        ped->GetMatrix().SetUnity();
        ped->SetCharCreatedBy(PED_GAME_MISSION);
        CWorld::Remove(ped);
        CWorld::Add(ped);
        return ped;
    }

    CStreaming::RequestModel(pedPacket.modelId, 0);
    return nullptr;
}

// Returns true if the playback has ended.
// 0x45F380
bool CReplay::PlayBackThisFrameInterpolation(CAddressInReplayBuffer& buffer, float interpolation, uint32* outTimer) {
    // Como el original: al crear un vehículo se vuelve a mirar el pool y, si ya existe, se extrae
    // el paquete en el mismo frame.
    const auto SetupVehicle = [](const uint8* p, CVehicle* vehicle, bool checkBoat) {
        auto* v = reinterpret_cast<uint8*>(vehicle);
        v[0x36] = static_cast<uint8>((v[0x36] & 7u) | 8u);
        v[0x42F] |= 8u;
        ReplayAt<uint32>(v, 0x1C) &= ~1u;
        reinterpret_cast<const CCompressedMatrixNotAligned*>(p + 4)->DecompressIntoFullMatrix(vehicle->GetMatrix());
        v[0x434] = p[0x2D];
        v[0x435] = p[0x2E];
        // Como el original: solo el paquete de vehículo genérico mira si es un barco.
        if (checkBoat && ReplayAt<uint32>(v, 0x594) == VEHICLE_TYPE_BOAT) {
            v[0x5AC] &= ~4u;
        }
        CWorld::Add(vehicle);
    };
    const auto PacketVehicle = [](const uint8* p) {
        return ReplayVehicleAt(m_VehiclePoolConversion.data()[p[1]]);
    };

    CBulletTraces::Init();

    for (;;) {
        auto* p = reinterpret_cast<uint8*>(buffer.m_pBase) + buffer.m_nOffset;
        const auto type = static_cast<eReplayPacket>(p[0]);
        if (type == REPLAY_PACKET_END_OF_FRAME)
            break;

        switch (type) {
        case REPLAY_PACKET_END: {
            if (BufferStatus.data()[buffer.m_bSlot] == REPLAYBUFFER_IN_USE) {
                FinishPlayback();
                return true;
            }
            const auto slot = static_cast<uint8>((buffer.m_bSlot + 1) % NUM_REPLAY_BUFFERS);
            buffer.m_bSlot = slot;
            buffer.m_nOffset = 0;
            buffer.m_pBase = &Buffers.data()[slot];
            continue; // We do not increment when we read the end packet.
        }
        case REPLAY_PACKET_VEHICLE: {
            if (!PacketVehicle(p)) {
                const int16 modelId = ReplayAt<int16>(p, 0x1A);
                if (CStreaming::ms_aInfoForModel.data()[modelId].m_LoadState == eStreamingLoadState::LOADSTATE_LOADED) {
                    const auto handle = m_VehiclePoolConversion.data()[p[1]] << 8;
                    CVehicle* created = nullptr;
                    switch (p[0x30]) {
                    case VEHICLE_TYPE_AUTOMOBILE: created = new (handle) CAutomobile(modelId, MISSION_VEHICLE, true); break;
                    case VEHICLE_TYPE_MTRUCK:     created = new (handle) CMonsterTruck(modelId, MISSION_VEHICLE); break;
                    case VEHICLE_TYPE_QUAD:       created = new (handle) CQuadBike(modelId, MISSION_VEHICLE); break;
                    case VEHICLE_TYPE_BOAT:       created = new (handle) CBoat(modelId, MISSION_VEHICLE); break;
                    case VEHICLE_TYPE_TRAIN:      created = new (handle) CTrain(modelId, MISSION_VEHICLE); break;
                    case VEHICLE_TYPE_TRAILER:    created = new (handle) CTrailer(modelId, MISSION_VEHICLE); break;
                    default:
                        NOTSA_UNREACHABLE("Unknown vehicle subtype ={}", (int)p[0x30]);
                    }
                    SetupVehicle(p, created, true);
                } else {
                    CStreaming::RequestModel(modelId, 0);
                }
            }
            if (auto* vehicle = PacketVehicle(p)) {
                reinterpret_cast<tReplayVehicleBlock*>(p)->ExtractVehicleUpdateData(*vehicle, interpolation);
            }
            buffer.m_nOffset += 0x34;
            continue;
        }
        case REPLAY_PACKET_BIKE:
        case REPLAY_PACKET_BMX:
        case REPLAY_PACKET_HELI:
        case REPLAY_PACKET_PLANE:
        case REPLAY_PACKET_TRAIN: {
            if (!PacketVehicle(p)) {
                const int16 modelId = ReplayAt<int16>(p, 0x1A);
                if (CStreaming::ms_aInfoForModel.data()[modelId].m_LoadState == eStreamingLoadState::LOADSTATE_LOADED) {
                    const auto handle = m_VehiclePoolConversion.data()[p[1]] << 8;
                    CVehicle* created = nullptr;
                    switch (type) {
                    case REPLAY_PACKET_BIKE:  created = new (handle) CBike(modelId, MISSION_VEHICLE); break;
                    case REPLAY_PACKET_BMX:   created = new (handle) CBmx(modelId, MISSION_VEHICLE); break;
                    case REPLAY_PACKET_HELI:  created = new (handle) CHeli(modelId, MISSION_VEHICLE); break;
                    case REPLAY_PACKET_PLANE: created = new (handle) CPlane(modelId, MISSION_VEHICLE); break;
                    default:                  created = new (handle) CTrain(modelId, MISSION_VEHICLE); break;
                    }
                    SetupVehicle(p, created, false);
                } else {
                    CStreaming::RequestModel(modelId, 0);
                }
            }
            if (auto* vehicle = PacketVehicle(p)) {
                switch (type) {
                case REPLAY_PACKET_BIKE:
                    reinterpret_cast<tReplayBikeBlock*>(p)->ExtractBikeUpdateData(*vehicle->AsBike(), interpolation);
                    break;
                case REPLAY_PACKET_BMX:
                    reinterpret_cast<tReplayBmxBlock*>(p)->ExtractBmxUpdateData(*vehicle->AsBmx(), interpolation);
                    break;
                case REPLAY_PACKET_HELI:
                    reinterpret_cast<tReplayVehicleBlock*>(p)->ExtractVehicleUpdateData(*vehicle, interpolation);
                    ReplayAt<uint32>(reinterpret_cast<uint8*>(vehicle), 0x84C) = ReplayAt<uint32>(p, 0x34);
                    break;
                case REPLAY_PACKET_PLANE:
                    reinterpret_cast<tReplayVehicleBlock*>(p)->ExtractVehicleUpdateData(*vehicle, interpolation);
                    ReplayAt<uint32>(reinterpret_cast<uint8*>(vehicle), 0x9C8) = ReplayAt<uint32>(p, 0x38);
                    ReplayAt<uint32>(reinterpret_cast<uint8*>(vehicle), 0x9C4) = ReplayAt<uint32>(p, 0x34);
                    break;
                default:
                    reinterpret_cast<tReplayTrainBlock*>(p)->ExtractTrainUpdateData(*vehicle->AsTrain(), interpolation);
                    break;
                }
            }
            buffer.m_nOffset += FindSizeOfPacket(type);
            continue;
        }
        case REPLAY_PACKET_PED_HEADER:
            DealWithNewPedPacket(*reinterpret_cast<tReplayPedHeaderBlock*>(p), false, nullptr);
            buffer.m_nOffset += 8;
            continue;
        case REPLAY_PACKET_PED_UPDATE:
            ProcessPedUpdate(ReplayPedAt(m_PedPoolConversion.data()[p[1]]), interpolation, buffer);
            continue; // ProcessPedUpdate increments the offset by itself already.
        case REPLAY_PACKET_GENERAL: {
            // Como el original: interpola TheCamera.m_mCameraMatrix (+0x974) con ScaleAll y operator+=.
            const float inv = 1.0f - interpolation;
            auto& camMat = TheCamera.m_mCameraMatrix;
            camMat.ScaleAll(inv);
            CMatrix stored(*reinterpret_cast<const CMatrix*>(p + 4));
            stored.ScaleAll(interpolation);
            camMat += stored;

            auto* modelling = RwFrameGetMatrix(RwCameraGetFrame(TheCamera.m_pRwCamera));
            modelling->pos   = camMat.GetPosition();
            modelling->at    = camMat.GetForward();
            modelling->up    = camMat.GetUp();
            modelling->right = camMat.GetRight();

            CameraFocus.x = interpolation * ReplayAt<float>(p, 0x4C) + CameraFocus.x * inv;
            CameraFocus.y = interpolation * ReplayAt<float>(p, 0x50) + CameraFocus.y * inv;
            CameraFocus.z = interpolation * ReplayAt<float>(p, 0x54) + CameraFocus.z * inv;
            bIsUsingRemoteCar = p[1] != 0;
            buffer.m_nOffset += 0x58;
            continue;
        }
        case REPLAY_PACKET_CLOCK:
            CClock::SetGameClock(p[1], p[2], 0);
            buffer.m_nOffset += 4;
            continue;
        case REPLAY_PACKET_WEATHER:
            CWeather::OldWeatherType = static_cast<eWeatherType>(p[1]);
            CWeather::NewWeatherType = static_cast<eWeatherType>(p[2]);
            CWeather::InterpolationValue = ReplayAt<float>(p, 4);
            buffer.m_nOffset += 8;
            continue;
        case REPLAY_PACKET_TIMER: {
            const auto timeInMS = ReplayAt<uint32>(p, 4);
            if (outTimer) {
                *outTimer = timeInMS;
            }
            CTimer::SetTimeInMS(timeInMS);
            buffer.m_nOffset += 8;
            continue;
        }
        case REPLAY_PACKET_BULLET_TRACES: {
            auto* trace = reinterpret_cast<uint8*>(&CBulletTraces::aTraces.data()[p[3]]);
            trace[0x18] = 1;
            memcpy(trace, p + 4, 0x18);
            buffer.m_nOffset += 0x1C;
            continue;
        }
        case REPLAY_PACKET_MISC:
            TheCamera.m_nCamShakeStart = ReplayAt<uint32>(p, 4);
            TheCamera.m_fCamShakeForce = ReplayAt<float>(p, 8);
            CSpecialFX::bVideoCam = (p[0xD] & 1u) != 0;
            CSpecialFX::bLiftCam = ((p[0xD] >> 1) & 1u) != 0;
            CGame::currArea = static_cast<eAreaCodes>(p[0xC]);
            buffer.m_nOffset += 0x10;
            continue;
        case REPLAY_PACKET_DELETED_VEH: {
            const auto idx = m_VehiclePoolConversion.data()[ReplayAt<int16>(p, 2)];
            if (idx >= 0) {
                if (auto* vehicle = ReplayVehicleAt(idx)) {
                    CWorld::Remove(vehicle);
                    delete vehicle;
                }
            }
            buffer.m_nOffset += 4;
            continue;
        }
        case REPLAY_PACKET_DELETED_PED: {
            const auto idx = m_PedPoolConversion.data()[ReplayAt<int16>(p, 2)];
            if (idx >= 0) {
                if (auto* ped = ReplayPedAt(idx)) {
                    CWorld::Remove(ped);
                    delete ped;
                }
            }
            buffer.m_nOffset += 4;
            continue;
        }
        case REPLAY_PACKET_CLOTHES:
            buffer.m_nOffset += 0x78;
            continue;
        default:
            // Como el original no avanza (bucle infinito) con REPLAY_PACKET_PARTICLE o tipos >= 20.
            NOTSA_UNREACHABLE("Invalid packet read (type ID={})", (int32)type);
        }
    }
    buffer.m_nOffset += 4u;
    ProcessReplayCamera();

    return false;
}

// 0x460350
bool CReplay::FastForwardToTime(uint32 start) {
    if (!start)
        return true;

    uint32 timer = 0;
    while (!PlayBackThisFrameInterpolation(Playback, 1.0f, &timer)) {
        if (timer >= start) {
            return true;
        }
    }

    return false;
}

// 0x4604A0
void CReplay::PlayBackThisFrame() {
    if (PlayBackThisFrameInterpolation(Playback, 1.0f, nullptr)) {
        AudioEngine.SetEffectsMasterVolume(FrontEndMenuManager.m_nSfxVolume);
        AudioEngine.SetMusicMasterVolume(FrontEndMenuManager.m_nRadioVolume);
    } else {
        ProcessLookAroundCam();
        AudioEngine.SetEffectsMasterVolume(0);
        AudioEngine.SetMusicMasterVolume(0);
    }
}

// 0x45C850
constexpr uint32 CReplay::FindSizeOfPacket(eReplayPacket type) {
    switch (type) {
    case REPLAY_PACKET_END:          // <- actually 1 but we shouldn't call this func with this anyways so it's no problem.
    case REPLAY_PACKET_END_OF_FRAME:
    case REPLAY_PACKET_CLOCK:
    case REPLAY_PACKET_DELETED_VEH:
    case REPLAY_PACKET_DELETED_PED:
        return 4;
    case REPLAY_PACKET_VEHICLE:
    case REPLAY_PACKET_PED_UPDATE:
        return 52;
    case REPLAY_PACKET_BIKE:
    case REPLAY_PACKET_BMX:
    case REPLAY_PACKET_HELI:
        return 56;
    case REPLAY_PACKET_PED_HEADER:
    case REPLAY_PACKET_WEATHER:
    case REPLAY_PACKET_TIMER:
        return 8;
    case REPLAY_PACKET_GENERAL:
        return 88;
    case REPLAY_PACKET_BULLET_TRACES:
        return 28;
    case REPLAY_PACKET_PARTICLE:
        return 20;
    case REPLAY_PACKET_MISC:
        return 16;
    case REPLAY_PACKET_PLANE:
        return 60;
    case REPLAY_PACKET_TRAIN:
        return 76;
    case REPLAY_PACKET_CLOTHES:
        return 120;
    default:
        return 0;
    }
}

// 0x45DE40
bool CReplay::IsThisVehicleUsedInRecording(int32 index) {
    // Como el original: solo se recorren los buffers disponibles.
    for (auto slot = 0u; slot < NUM_REPLAY_BUFFERS; slot++) {
        if (BufferStatus.data()[slot] == REPLAYBUFFER_NOT_AVAILABLE)
            continue;

        const auto* buf = ReplayBufferPtr(slot);
        for (uint32 offset = 0; buf[offset] != REPLAY_PACKET_END; offset += FindSizeOfPacket(static_cast<eReplayPacket>(buf[offset]))) {
            const auto* p = buf + offset;
            switch (p[0]) {
            case REPLAY_PACKET_VEHICLE:
            case REPLAY_PACKET_BIKE:
            case REPLAY_PACKET_BMX:
            case REPLAY_PACKET_HELI:
            case REPLAY_PACKET_PLANE:
            case REPLAY_PACKET_TRAIN:
                if (p[1] == index) {
                    return true;
                }
                break;
            case REPLAY_PACKET_PED_UPDATE:
                if (p[3] && p[3] - 1 == index) {
                    return true;
                }
                break;
            default:
                break;
            }
        }
    }
    return false;
}

// 0x45DDE0
bool CReplay::IsThisPedUsedInRecording(int32 index) {
    // Como el original: solo se recorren los buffers disponibles.
    for (auto slot = 0u; slot < NUM_REPLAY_BUFFERS; slot++) {
        if (BufferStatus.data()[slot] == REPLAYBUFFER_NOT_AVAILABLE)
            continue;

        const auto* buf = ReplayBufferPtr(slot);
        for (uint32 offset = 0; buf[offset] != REPLAY_PACKET_END; offset += FindSizeOfPacket(static_cast<eReplayPacket>(buf[offset]))) {
            if (buf[offset] == REPLAY_PACKET_PED_HEADER && buf[offset + 1] == index) {
                return true;
            }
        }
    }
    return false;
}

// 0x45D6C0
void CReplay::FindFirstFocusCoordinate(CVector& outPos) {
    // Como el original: la salida se pone a cero antes de buscar.
    outPos = CVector{0.0f, 0.0f, 0.0f};
    for (auto slot = 0u; slot < NUM_REPLAY_BUFFERS; slot++) {
        if (BufferStatus.data()[slot] == REPLAYBUFFER_NOT_AVAILABLE)
            continue;

        const auto* buf = ReplayBufferPtr(slot);
        for (uint32 offset = 0; buf[offset] != REPLAY_PACKET_END; offset += FindSizeOfPacket(static_cast<eReplayPacket>(buf[offset]))) {
            if (buf[offset] == REPLAY_PACKET_GENERAL) {
                outPos = *reinterpret_cast<const CVector*>(buf + offset + 0x4C);
                return;
            }
        }
    }
}

// 0x45D670
uint32 CReplay::NumberFramesAvailableToPlay() {
    uint32 frames = 0;
    for (auto slot = 0u; slot < NUM_REPLAY_BUFFERS; slot++) {
        if (BufferStatus.data()[slot] == REPLAYBUFFER_NOT_AVAILABLE)
            continue;

        const auto* buf = ReplayBufferPtr(slot);
        for (uint32 offset = 0; buf[offset] != REPLAY_PACKET_END; offset += FindSizeOfPacket(static_cast<eReplayPacket>(buf[offset]))) {
            if (buf[offset] == REPLAY_PACKET_END_OF_FRAME) {
                frames++;
            }
        }
    }
    return frames;
}

// 0x45D4B0
void CReplay::StreamAllNecessaryCarsAndPeds() {
    for (auto slot = 0u; slot < NUM_REPLAY_BUFFERS; slot++) {
        if (BufferStatus.data()[slot] == REPLAYBUFFER_NOT_AVAILABLE)
            continue;

        auto* buf = ReplayBufferPtr(slot);
        for (uint32 offset = 0; buf[offset] != REPLAY_PACKET_END; offset += FindSizeOfPacket(static_cast<eReplayPacket>(buf[offset]))) {
            auto* p = buf + offset;
            switch (p[0]) {
            case REPLAY_PACKET_VEHICLE:
            case REPLAY_PACKET_BIKE:
                CStreaming::RequestModel(ReplayAt<int16>(p, 0x1A), 0);
                break;
            case REPLAY_PACKET_PED_HEADER:
                CStreaming::RequestModel(ReplayAt<int16>(p, 2), 0);
                break;
            default:
                break;
            }
        }
    }
    CStreaming::LoadAllRequestedModels(false);
}

// 0x45D540
CPlayerPed* CReplay::CreatePlayerPed() {
    // Como el original: los buffers no disponibles o vacíos se saltan (no cortan la búsqueda) y el
    // paquete de ropa es simplemente el siguiente al de cabecera.
    CPlayerPed* player = nullptr;
    for (auto i = 0; i < 8; i++) {
        const auto slot = (Playback.m_bSlot + i) % 8;
        if (BufferStatus.data()[slot] == REPLAYBUFFER_NOT_AVAILABLE)
            continue;

        auto* buf = ReplayBufferPtr(slot);
        for (uint32 offset = 0; buf[offset] != REPLAY_PACKET_END; offset += FindSizeOfPacket(static_cast<eReplayPacket>(buf[offset]))) {
            auto* p = buf + offset;
            switch (p[0]) {
            case REPLAY_PACKET_PED_HEADER:
                if (!player) {
                    player = static_cast<CPlayerPed*>(DealWithNewPedPacket(*reinterpret_cast<tReplayPedHeaderBlock*>(p), true, reinterpret_cast<tReplayClothesBlock*>(p + 8)));
                }
                break;
            case REPLAY_PACKET_PED_UPDATE:
                if (player) {
                    auto* ped = ReplayPedAt(m_PedPoolConversion.data()[p[1]]);
                    if (ped == player) {
                        CAddressInReplayBuffer address;
                        address.m_nOffset = offset;
                        address.m_pBase = &Buffers.data()[slot];
                        ProcessPedUpdate(ped, 1.0f, address);
                        return player;
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    return nullptr;
}

// 0x4600F0
void CReplay::TriggerPlayback(eReplayCamMode mode, CVector fixedCamPos, bool loadScene) {
    if (Mode != MODE_RECORD)
        return;

    CSpecialFX::ReplayStarted();
    CameraMode = mode;
    Mode = MODE_PLAYBACK;
    CameraFixed = fixedCamPos;

    bPlayingBackFromFile = false;
    bAllowLookAroundCam = true;
    FramesActiveLookAroundCam = 0;

    // Como el original: primero se lee la emisora y después se para la radio; sin vehículo se guarda 0.
    if (FindPlayerVehicle(-1, false)) {
        OldRadioStation = static_cast<int32>(AERadioTrackManager.GetCurrentRadioStationID());
        AudioEngine.StopRadio(nullptr, false);
    } else {
        OldRadioStation = 0;
    }

    CurrArea = static_cast<int8>(CGame::currArea);

    // Como el original: si no hay buffer en uso, el índice queda en 8 y se empieza a buscar en el 1.
    int32 inUse = 0;
    while (inUse < 8 && BufferStatus.data()[inUse] != REPLAYBUFFER_IN_USE) {
        inUse++;
    }
    auto slot = (inUse + 1) % 8;
    while (BufferStatus.data()[slot] != REPLAYBUFFER_IN_USE && BufferStatus.data()[slot] != REPLAYBUFFER_FULL) {
        slot = (slot + 1) % 8;
    }
    Playback.m_bSlot = static_cast<uint8>(slot);
    Playback.m_pBase = &Buffers.data()[slot];
    Playback.m_nOffset = 0;

    CObject::DeleteAllTempObjectsInArea(CVector{0.0f, 0.0f, 0.0f}, 1'000'000.0f);
    StoreStuffInMem();
    InitialisePedPoolConversionTable();
    InitialiseVehiclePoolConversionTable();
    EmptyPedsAndVehiclePools_NoDestructors();
    CSkidmarks::Clear();
    StreamAllNecessaryCarsAndPeds();

    CWorld::Players[0].m_pPed = CreatePlayerPed();

    if (!loadScene) {
        bDoLoadSceneWhenDone = true;
        LoadScene = TheCamera.GetPosition();
        CVector point;
        FindFirstFocusCoordinate(point);
        CGame::currLevel = static_cast<eLevelName>(CTheZones::GetLevelFromPosition(point));
        CCollision::SortOutCollisionAfterLoad();
        CStreaming::LoadScene(point);
    } else {
        bDoLoadSceneWhenDone = false;
    }

    CWeaponEffects::ClearCrossHairs();
    CExplosion::ClearAllExplosions();
    CPlaneBanners::Init();
    gFireManager.DestroyAllFxSystems();
    TheCamera.Restore();
    CDraw::SetFOV(70.0f);
    TheCamera.SetFadeColour(0, 0, 0);
    TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
    TheCamera.ProcessFade();
    TheCamera.Fade(1.5f, eFadeFlag::FADE_OUT);
}

CReplay::tReplayBuffer::Iterator& CReplay::tReplayBuffer::Iterator::operator++() {
#ifdef NOTSA_DEBUG
    if (m_offset >= REPLAY_BUFFER_SIZE || !m_buffer) {
        NOTSA_UNREACHABLE("Increment after end()!");
    }
#endif

    const auto type = m_buffer->Read<tReplayBlockBase>(m_offset).type;
    m_offset = (type != REPLAY_PACKET_END) ? m_offset + CReplay::FindSizeOfPacket(type) : REPLAY_BUFFER_SIZE;
    return *this;
}

#define VALIDATE_BLOCK(block) static_assert(sizeof(block) == CReplay::FindSizeOfPacket(block::Type), "Block size mismatch!")

VALIDATE_BLOCK(tReplayVehicleBlock);
VALIDATE_BLOCK(tReplayBikeBlock);
VALIDATE_BLOCK(tReplayPedHeaderBlock);
VALIDATE_BLOCK(tReplayPedUpdateBlock);
VALIDATE_BLOCK(tReplayCameraBlock);
VALIDATE_BLOCK(tReplayClockBlock);
VALIDATE_BLOCK(tReplayWeatherBlock);
VALIDATE_BLOCK(tReplayEOFBlock);
VALIDATE_BLOCK(tReplayTimerBlock);
VALIDATE_BLOCK(tReplayTimerBlock);
VALIDATE_BLOCK(tReplayBulletTraceBlock);
VALIDATE_BLOCK(tReplayParticleBlock);
VALIDATE_BLOCK(tReplayMiscBlock);
VALIDATE_BLOCK(tReplayDeletedVehicleBlock);
VALIDATE_BLOCK(tReplayDeletedPedBlock);
VALIDATE_BLOCK(tReplayBmxBlock);
VALIDATE_BLOCK(tReplayHeliBlock);
VALIDATE_BLOCK(tReplayPlaneBlock);
VALIDATE_BLOCK(tReplayTrainBlock);
VALIDATE_BLOCK(tReplayClothesBlock);
#undef VALIDATE_BLOCK
