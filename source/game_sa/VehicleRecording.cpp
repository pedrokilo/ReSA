/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRT
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CVehicleRecording
 * Reconstrucción: COMPLETA — 29/29 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 29/29 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "VehicleRecording.h"

#ifdef EXTRA_CARREC_LOGS
    #define CARREC_LOG(...) NOTSA_LOG_DEBUG(__VA_ARGS__)
#else
    #define CARREC_LOG(...)
#endif

// VehicleRecording naming convention:
//
// `StreamingArray` contains 'recordings' that are typed CPath.
// It's indices are named `recordId`
//
// CPath contains a variable-sized frame array. (`CVehicleStateEachFrame`)
// It's 'number' is taken from the loaded file hence named `fileNumber`
//
// `pPlaybackBuffer` contains 16 'recordings' that can be played
// simultaneously.
// It's incides are named `playbackId`
//
// Como el original: PlaybackIndex y PlaybackBufferSize están en bytes (múltiplos de sizeof(CVehicleStateEachFrame)).

namespace {
// Como el original: constantes del EXE (0x859BCC, 0x858EAC, 0x858C28, 0x858C58)
constexpr float CARREC_DIR_SCALE      = 1.0f / 127.0f;
constexpr float CARREC_VELOCITY_SCALE = 1.0f / 16383.5f;
constexpr float CARREC_STEER_SCALE    = 0.05f;
constexpr float CARREC_PEDAL_SCALE    = 0.01f;

// Como el original: búsqueda desenrollada de las funciones del EXE; devuelve 16 (fuera de los arrays) si no la encuentra.
int32 FindPlaybackIndexOrEnd(CVehicle* vehicle) {
    int32 i = 0;
    for (; i < TOTAL_VEHICLE_RECORDS; i++) {
        if (CVehicleRecording::bPlaybackGoingOn[i] && CVehicleRecording::pVehicleForPlayback[i] == vehicle) {
            break;
        }
    }
    return i;
}

CVehicleStateEachFrame* FrameAt(CVehicleStateEachFrame* buffer, uint32 byteOffset) {
    return reinterpret_cast<CVehicleStateEachFrame*>(reinterpret_cast<uint8*>(buffer) + byteOffset);
}

// Como el original: `fild` de un uint32 es exacto salvo desde 2^31, donde `fadd 2^32` redondea a 24 bits.
double X87UInt(uint32 value) {
    return (int32)value >= 0 ? (double)value : (double)(float)value;
}
} // namespace

// 0x459390
void CVehicleRecording::Init() {
    ZoneScoped;

    rng::fill(bPlaybackGoingOn, false);
    rng::fill(bPlaybackPaused, false);
    rng::fill(pPlaybackBuffer, nullptr);
    rng::fill(pVehicleForPlayback, nullptr);

    for (auto& recording : StreamingArray) {
        recording.m_pData = nullptr;
        recording.m_nRefCount = 0;
    }
}

// 0x45A1B0
void CVehicleRecording::InitAtStartOfGame() {
    for (auto& recording : StreamingArray) {
        recording.m_pData = nullptr;
    }
    Init();
}

// 0x459400
void CVehicleRecording::ShutDown() {
    rng::for_each(StreamingArray, &CPath::Remove);
}

// 0x459F70 hook not needed
void CVehicleRecording::Render() {
}

// 0x45A360
void CVehicleRecording::ChangeCarPlaybackToUseAI(CVehicle* vehicle) {
    // Como el original: no comprueba si la encontró; sin reproducción usa el índice 16 (escribe bPlaybackPaused[0]).
    const auto i = FindPlaybackIndexOrEnd(vehicle);
    bUseCarAI.data()[i] = true;

    vehicle->m_autoPilot.SetCarMission(MISSION_FOLLOW_RECORDED_PATH);
    SetRecordingToPointClosestToCoors(i, vehicle->GetPosition());
    vehicle->physicalFlags.bDisableCollisionForce = false;
    vehicle->ProcessControlCollisionCheck(false);
    vehicle->SetStatus(STATUS_PHYSICS);
}

// inlined
// 0x459FF0
uint32 CVehicleRecording::FindIndexWithFileNameNumber(int32 fileNumber) {
    for (int32 i = 0; i < NumPlayBackFiles; i++) {
        if (StreamingArray[i].m_nNumber == fileNumber) {
            return i;
        }
    }
    return 0;
}

// 0x459B30
void CVehicleRecording::InterpolateInfoForCar(CVehicle* vehicle, const CVehicleStateEachFrame& frame, float interpValue) {
    // Como el original: (fila de la grabación * t) + (fila del vehículo * (1 - t)) en filas y posición; el relleno de la
    // matriz del vehículo queda con lo que haya en la temporal (en el EXE, pila sin inicializar).
    CMatrix& vehMatrix = *vehicle->m_matrix;

    CMatrix transition;
    RestoreInfoForMatrix(transition, frame);
    const float invInterp = 1.0f - interpValue;

    const auto Mix = [&](const CVector& a, const CVector& b) {
        return CVector{ a.x * interpValue + b.x * invInterp, a.y * interpValue + b.y * invInterp, a.z * interpValue + b.z * invInterp };
    };
    CMatrix result;
    result.GetRight()    = Mix(transition.GetRight(), vehMatrix.GetRight());
    result.GetForward()  = Mix(transition.GetForward(), vehMatrix.GetForward());
    result.GetUp()       = Mix(transition.GetUp(), vehMatrix.GetUp());
    result.GetPosition() = Mix(transition.GetPosition(), vehMatrix.GetPosition());
    vehMatrix = result;

    auto& moveSpeed = vehicle->GetMoveSpeed();
    moveSpeed.x = (float)frame.m_sVelocity[0] * CARREC_VELOCITY_SCALE * interpValue + invInterp * moveSpeed.x;
    moveSpeed.y = (float)frame.m_sVelocity[1] * CARREC_VELOCITY_SCALE * interpValue + invInterp * moveSpeed.y;
    moveSpeed.z = (float)frame.m_sVelocity[2] * CARREC_VELOCITY_SCALE * interpValue + invInterp * moveSpeed.z;
}

// 0x45A060
bool CVehicleRecording::HasRecordingFileBeenLoaded(int32 fileNumber) {
    // Como el original: si no la encuentra mira StreamingArray[0].
    return StreamingArray[FindIndexWithFileNameNumber(fileNumber)].m_pData != nullptr;
}

// 0x45A8F0
void CVehicleRecording::Load(RwStream* stream, int32 recordId, int32 totalSize) {
    auto& recording = StreamingArray[recordId];
    recording.m_pData = static_cast<CVehicleStateEachFrame*>(CMemoryMgr::Malloc(totalSize, 0));
    recording.m_nSize = RwStreamRead(stream, recording.m_pData, 9'999'999u);
    RwStreamClose(stream, nullptr);

    CARREC_LOG("Load carrec to streaming slot idx:{} (size={})", recordId, totalSize);

    // Como el original: recorre en bytes mientras offset < m_nSize; el primer fotograma no inicial con tiempo 0 recorta.
    for (int32 offset = 0; offset < recording.m_nSize; offset += sizeof(CVehicleStateEachFrame)) {
        if (FrameAt(recording.m_pData, offset)->m_nTime == 0 && offset != 0) {
            CARREC_LOG("\tRecording pruned at offset {}", offset);
            recording.m_nSize = offset;
        }
    }
    SmoothRecording(recordId);
}

// 0x45A0F0
void CVehicleRecording::SmoothRecording(int32 recordId) {
    // Como el original: desde el fotograma 2 (fotograma[i-1] = (t[i-2] + t[i]) * 0.5, truncado con _ftol2),
    // con comparaciones sin signo sobre el tamaño en bytes.
    auto& recording = StreamingArray[recordId];
    auto* const data = recording.m_pData;
    if ((uint32)recording.m_nSize - 0x20u <= 0x20u) {
        return;
    }
    uint32 offset = 0x40;
    do {
        const uint32 sum = FrameAt(data, offset - 0x40)->m_nTime + FrameAt(data, offset)->m_nTime;
        FrameAt(data, offset - 0x20)->m_nTime = (uint32)(int64)((float)sum * 0.5f);
        offset += 0x20;
    } while (offset - 0x20 < (uint32)recording.m_nSize - 0x20u);
}

// 0x459F80
int32 CVehicleRecording::RegisterRecordingFile(const char* name) {
    auto fileNumber = 850;
    if (sscanf_s(name, "carrec%d", &fileNumber) == 0) {
        sscanf_s(name, "CARREC%d", &fileNumber); // Como el original: no comprueba el resultado
    }

    CARREC_LOG("Registering carrec file '{}', (streamIdx={})", name, NumPlayBackFiles);

    StreamingArray[NumPlayBackFiles].m_nNumber = fileNumber;
    StreamingArray[NumPlayBackFiles].m_pData = nullptr;
    return NumPlayBackFiles++;
}

// 0x45A0A0
void CVehicleRecording::RemoveRecordingFile(int32 fileNumber) {
    // Como el original: recorre las 475 entradas, no solo la primera coincidencia.
    for (auto& recording : StreamingArray) {
        if (recording.m_nNumber == fileNumber && recording.m_pData && !recording.m_nRefCount) {
            recording.Remove();
        }
    }
}

// 0x45A020
void CVehicleRecording::RequestRecordingFile(int32 fileNumber) {
    // Como el original: pide el modelo aunque no la encuentre (índice 0) y no libera nada.
    CStreaming::RequestModel(RRRToModelId(FindIndexWithFileNameNumber(fileNumber)), STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
}

// 0x459440
void CVehicleRecording::StopPlaybackWithIndex(int32 playbackId) {
    if (auto vehicle = pVehicleForPlayback[playbackId]) {
        vehicle->m_autoPilot.m_vehicleRecordingId = -1;
        pVehicleForPlayback[playbackId]->physicalFlags.bDisableCollisionForce = false;
    }
    pVehicleForPlayback[playbackId] = nullptr;
    pPlaybackBuffer[playbackId] = nullptr;
    PlaybackBufferSize[playbackId] = 0;
    bPlaybackGoingOn[playbackId] = false;

    StreamingArray[PlayBackStreamingIndex[playbackId]].RemoveRef();
}

// 0x45A980
void CVehicleRecording::StartPlaybackRecordedCar(CVehicle* vehicle, int32 fileNumber, bool useCarAI, bool looped) {
    const auto recordId = FindIndexWithFileNameNumber(fileNumber);
    // Como el original: sin hueco libre usa el índice 16 (fuera de los arrays).
    int32 playbackId = 0;
    for (; playbackId < TOTAL_VEHICLE_RECORDS; playbackId++) {
        if (!bPlaybackGoingOn[playbackId]) {
            break;
        }
    }

    auto& slot = pVehicleForPlayback.data()[playbackId];
    slot = vehicle;
    vehicle->RegisterReference(reinterpret_cast<CEntity**>(&slot));
    bPlaybackLooped.data()[playbackId] = looped;
    PlayBackStreamingIndex.data()[playbackId] = recordId;
    pPlaybackBuffer.data()[playbackId] = StreamingArray[recordId].m_pData;
    PlaybackBufferSize.data()[playbackId] = StreamingArray[recordId].m_nSize;
    StreamingArray[recordId].AddRef();
    bUseCarAI.data()[playbackId] = useCarAI;
    PlaybackIndex.data()[playbackId] = 0;
    PlaybackRunningTime.data()[playbackId] = 0.0f;
    PlaybackSpeed.data()[playbackId] = 1.0f;
    bPlaybackGoingOn.data()[playbackId] = true;
    bPlaybackPaused.data()[playbackId] = false;

    if (useCarAI) {
        slot->m_autoPilot.SetCarMission(MISSION_FOLLOW_RECORDED_PATH);
        SetRecordingToPointClosestToCoors(playbackId, vehicle->GetPosition());
    } else {
        slot->physicalFlags.bDisableCollisionForce = true;
        slot->physicalFlags.bCollidable = false;
    }
    slot->m_autoPilot.m_vehicleRecordingId = (int8)playbackId;
}

// 0x45A280
void CVehicleRecording::StopPlaybackRecordedCar(CVehicle* vehicle) {
    if (const auto i = FindPlaybackIndexOrEnd(vehicle); i < TOTAL_VEHICLE_RECORDS) {
        StopPlaybackWithIndex(i);
    }
}

// 0x459740
void CVehicleRecording::PausePlaybackRecordedCar(CVehicle* vehicle) {
    // Como el original: sin reproducción escribe en el índice 16 (bPlaybackLooped[0]).
    bPlaybackPaused.data()[FindPlaybackIndexOrEnd(vehicle)] = true;
}

// 0x459850
void CVehicleRecording::UnpausePlaybackRecordedCar(CVehicle* vehicle) {
    // Como el original: sin reproducción escribe en el índice 16 (bPlaybackLooped[0]).
    bPlaybackPaused.data()[FindPlaybackIndexOrEnd(vehicle)] = false;
}

// 0x459660
void CVehicleRecording::SetPlaybackSpeed(CVehicle* vehicle, float speed) {
    if (const auto i = FindPlaybackIndexOrEnd(vehicle); i < TOTAL_VEHICLE_RECORDS) {
        PlaybackSpeed[i] = speed;
    }
}

// [debug]
// 0x459F00
void CVehicleRecording::RenderLineSegment(int32& numVertices) {
    if (numVertices > 1) {
        for (int32 i = 0; i < numVertices - 1; i++) {
            aTempBufferIndices[2 * i] = i;
            aTempBufferIndices[2 * i + 1] = i + 1;
        }
        if (RwIm3DTransform(TempBufferVertices.m_3d, numVertices, nullptr, 0)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, aTempBufferIndices, (numVertices - 1) * 2);
            RwIm3DEnd();
        }
    }
    numVertices = 0;
}

// 0x45A160
void CVehicleRecording::RemoveAllRecordingsThatArentUsed() {
    // Como el original: recorre las 475 entradas sin mirar el número de fichero.
    for (auto& recording : StreamingArray) {
        if (recording.m_pData && !recording.m_nRefCount) {
            recording.Remove();
        }
    }
}

// 0x459A30
void CVehicleRecording::RestoreInfoForCar(CVehicle* vehicle, const CVehicleStateEachFrame& frame, bool pause) {
    RestoreInfoForMatrix(*vehicle->m_matrix, frame);
    auto& moveSpeed = vehicle->GetMoveSpeed();
    moveSpeed.x = (float)frame.m_sVelocity[0] * CARREC_VELOCITY_SCALE;
    moveSpeed.y = (float)frame.m_sVelocity[1] * CARREC_VELOCITY_SCALE;
    vehicle->ResetTurnSpeed();
    moveSpeed.z = (float)frame.m_sVelocity[2] * CARREC_VELOCITY_SCALE;
    vehicle->m_fSteerAngle = (float)frame.m_bSteeringAngle * CARREC_STEER_SCALE;
    vehicle->m_GasPedal    = (float)frame.m_bGasPedalPower * CARREC_PEDAL_SCALE;
    vehicle->m_BrakePedal  = (float)frame.m_bBreakPedalPower * CARREC_PEDAL_SCALE;
    vehicle->vehicleFlags.bIsHandbrakeOn = frame.m_bHandbrakeUsed != 0;

    if (pause) {
        // Como el original: el ángulo de giro se conserva y el freno de mano se suelta.
        vehicle->m_GasPedal = vehicle->m_BrakePedal = 0.0f;
        vehicle->ResetMoveSpeed();
        vehicle->vehicleFlags.bIsHandbrakeOn = false;
    }

    // Como el original: en motos limpia bGettingPickedUp y bOnSideStand.
    if (vehicle->IsBike()) {
        vehicle->AsBike()->nBikeFlags &= ~0x18;
    }
}

// 0x459960
void CVehicleRecording::RestoreInfoForMatrix(CMatrix& matrix, const CVehicleStateEachFrame& frame) {
    matrix.GetRight()    = CVector{ (float)frame.m_bRight[0] * CARREC_DIR_SCALE, (float)frame.m_bRight[1] * CARREC_DIR_SCALE, (float)frame.m_bRight[2] * CARREC_DIR_SCALE };
    matrix.GetForward()  = CVector{ (float)frame.m_bTop[0] * CARREC_DIR_SCALE, (float)frame.m_bTop[1] * CARREC_DIR_SCALE, (float)frame.m_bTop[2] * CARREC_DIR_SCALE };
    matrix.GetUp()       = matrix.GetRight().Cross(matrix.GetForward());
    matrix.GetPosition() = frame.m_vecPosn;
}

// Como el original (0x821982..0x8219D7): printf del CRT estático del juego (char*, no wprintf; el identificador "wprintf" de Ghidra es un conflicto de FID)
// sobre el stdout del EXE (FILE 0x8E2C50). Es el envoltorio de la CRT: toma el cerrojo del flujo, activa el búfer temporal, formatea con el motor
// de formato del EXE y libera el cerrojo siempre (try/finally del original). El motor (_output) depende de las tablas de configuración regional y
// de la conversión de coma flotante del CRT del EXE, por lo que se llama por dirección en lugar de reimplementarlo. Devuelve el número de
// caracteres (EAX); ningún llamador lee ECX/EDX tras la llamada.
static int __cdecl CrtPrintf(const char* format, ...) {
    using LockFn   = void(__cdecl*)(int, void*);                 // 0x823FD1: _lock_file2(1, stream)
    using UnlockFn = void(__cdecl*)(int, void*);                 // 0x824023: _unlock_file2(1, stream)
    using StbufFn  = int(__cdecl*)(void*);                       // 0x827DEC: _stbuf
    using OutputFn = int(__cdecl*)(void*, const char*, va_list); // 0x827F4B: _output
    using FtbufFn  = void(__cdecl*)(int, void*);                 // 0x827E74: _ftbuf
    void* const stream = reinterpret_cast<void*>(0x8E2C50);
    reinterpret_cast<LockFn>(0x823FD1)(1, stream);
    int result = 0;
    __try {
        const int tempBuffer = reinterpret_cast<StbufFn>(0x827DEC)(stream);
        va_list args;
        va_start(args, format);
        result = reinterpret_cast<OutputFn>(0x827F4B)(stream, format, args);
        va_end(args);
        reinterpret_cast<FtbufFn>(0x827E74)(tempBuffer, stream);
    } __finally {
        reinterpret_cast<UnlockFn>(0x824023)(1, stream);
    }
    return result;
}

// CRT estática del EXE (lote 25, grupo g25e). Se reconstruyen sobre los datos y las funciones internas de la CRT del EXE (por dirección), nunca sobre la CRT (UCRT) de la DLL.

// 0x821D1E: atexit(func) = (_onexit(func) == NULL) ? -1 : 0. _onexit (0x821CE6) guarda func en la tabla de la CRT del EXE y devuelve func (o NULL si no cabe).
static int __cdecl CrtAtexit(void(__cdecl* func)()) {
    using OnexitFn = void*(__cdecl*)(void*);
    return reinterpret_cast<OnexitFn>(0x821CE6)(reinterpret_cast<void*>(func)) ? 0 : -1;
}

// 0x823FD1: _lock_file2(index, stream). Los flujos 0..19 usan el cerrojo de tabla 16 + index (_lock, 0x82ADBE); los demás tienen su propia sección crítica
// embebida en el FILE ampliado (stream + 0x20) que se entra con EnterCriticalSection (entrada de importación 0x8580EC). Comparación con signo (jge).
static void __cdecl CrtLockFile2(int index, void* stream) {
    if (index < 20) {
        reinterpret_cast<void(__cdecl*)(int)>(0x82ADBE)(index + 16);
        return;
    }
    (*reinterpret_cast<void(__stdcall**)(void*)>(0x8580EC))(static_cast<uint8*>(stream) + 0x20);
}

// 0x824023: _unlock_file2(index, stream), simétrica de la anterior: _unlock (0x82AD12) con 16 + index, o LeaveCriticalSection (0x8580E8) sobre stream + 0x20.
static void __cdecl CrtUnlockFile2(int index, void* stream) {
    if (index < 20) {
        reinterpret_cast<void(__cdecl*)(int)>(0x82AD12)(index + 16);
        return;
    }
    (*reinterpret_cast<void(__stdcall**)(void*)>(0x8580E8))(static_cast<uint8*>(stream) + 0x20);
}

// 0x824257: malloc(size) de la CRT del EXE (no es CMemoryMgr::Malloc, que es 0x72F420 y solo reenvía aquí) = _nh_malloc(size, _newmode) con _newmode en 0xC9ADCC.
// _nh_malloc (0x82422B) es el que toca el montón y el manejador de falta de memoria de la CRT del EXE.
static void* __cdecl CrtMalloc(uint32 size) {
    using NhMallocFn = void*(__cdecl*)(uint32, int);
    return reinterpret_cast<NhMallocFn>(0x82422B)(size, StaticRef<int>(0xC9ADCC));
}

// 0x82B181: _errno() = &_getptd()->_terrno (el campo está en el desplazamiento 8 de _tiddata). _getptd (0x827B3D) es cdecl sin argumentos y devuelve el bloque del hilo en EAX.
static int* __cdecl CrtErrno() {
    using GetPtdFn = uint8*(__cdecl*)();
    return reinterpret_cast<int*>(reinterpret_cast<GetPtdFn>(0x827B3D)() + 8);
}

// 0x45A610
void CVehicleRecording::SaveOrRetrieveDataForThisFrame() {
    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    for (int32 i = 0; i < TOTAL_VEHICLE_RECORDS; i++) {
        if (!bPlaybackGoingOn[i]) {
            continue;
        }
        const auto vehicle = pVehicleForPlayback[i];
        if (!vehicle || vehicle->physicalFlags.bRenderScorched) {
            StopPlaybackWithIndex(i);
            continue;
        }
        if (bUseCarAI[i])
            continue;

        // Como el original: el tiempo solo avanza si la reproducción no está en pausa.
        if (!bPlaybackPaused[i]) {
            if ((float)(CTimer::GetTimeInMS() - CTimer::m_snPPPPreviousTimeInMilliseconds) * PlaybackSpeed[i] * 0.25f > 500.0f) {
                CrtPrintf("That's a really big step\n");
            }
            PlaybackRunningTime[i] = (float)(CTimer::GetTimeInMS() - CTimer::m_snPPPPreviousTimeInMilliseconds) * PlaybackSpeed[i] * 0.25f + PlaybackRunningTime[i];
        }

        // Como el original: punteros al fotograma actual y al siguiente (puede leer el tiempo justo después del final).
        const auto buffer = pPlaybackBuffer[i];
        auto current = FrameAt(buffer, PlaybackIndex[i]);
        auto next = current + 1;
        if (X87UInt(next->m_nTime) < (double)PlaybackRunningTime[i]) {
            const auto end = FrameAt(buffer, PlaybackBufferSize[i]);
            while (next < end) {
                PlaybackIndex[i] += sizeof(CVehicleStateEachFrame);
                next++;
                current++;
                if (!(X87UInt(next->m_nTime) < (double)PlaybackRunningTime[i])) {
                    break;
                }
            }
        }
        if (X87UInt(current->m_nTime) > (double)PlaybackRunningTime[i]) {
            while (current > buffer) {
                PlaybackIndex[i] -= sizeof(CVehicleStateEachFrame);
                current--;
                next--;
                if (!(X87UInt(current->m_nTime) > (double)PlaybackRunningTime[i])) {
                    break;
                }
            }
        }

        if (next >= FrameAt(buffer, PlaybackBufferSize[i])) {
            if (bPlaybackLooped[i]) {
                PlaybackRunningTime[i] = 0.0f;
                PlaybackIndex[i] = 0;
            } else {
                StopPlaybackRecordedCar(pVehicleForPlayback[i]);
            }
            continue;
        }

        RestoreInfoForCar(pVehicleForPlayback[i], *current, false);
        const auto elapsed = (float)((double)PlaybackRunningTime[i] - X87UInt(current->m_nTime));
        const auto interp = (float)((double)elapsed / X87UInt(next->m_nTime - current->m_nTime));
        InterpolateInfoForCar(pVehicleForPlayback[i], *next, interp);

        if (pVehicleForPlayback[i]->IsSubTrain()) {
            pVehicleForPlayback[i]->AsTrain()->FindPositionOnTrackFromCoors();
        }
        pVehicleForPlayback[i]->ProcessControlCollisionCheck(false);
        pVehicleForPlayback[i]->RemoveAndAdd();
        pVehicleForPlayback[i]->UpdateRwMatrix();
        pVehicleForPlayback[i]->UpdateRwFrame();

        MarkSurroundingEntitiesForCollisionWithTrain(pVehicleForPlayback[i]->GetPosition(), 5.0f, pVehicleForPlayback[i], true);
    }
}

// 0x45A1E0
void CVehicleRecording::SetRecordingToPointClosestToCoors(int32 playbackId, CVector posn) {
    // Como el original: mínimo inicial 0x497423FE, distancia (dz² + dy²) + dx² y PlaybackIndex en bytes.
    auto minDist = std::bit_cast<float>(0x497423FEu);
    const auto size = PlaybackBufferSize.data()[playbackId];
    const auto buffer = pPlaybackBuffer.data()[playbackId];
    for (int32 offset = 0; offset < size; offset += sizeof(CVehicleStateEachFrame)) {
        const auto& pos = FrameAt(buffer, offset)->m_vecPosn;
        const float dx = posn.x - pos.x;
        const float dy = posn.y - pos.y;
        const float dz = posn.z - pos.z;
        if (const auto d = std::sqrt(dz * dz + dy * dy + dx * dx); d < minDist) {
            minDist = d;
            PlaybackIndex.data()[playbackId] = offset;
        }
    }
}

// 0x4594C0
bool CVehicleRecording::IsPlaybackGoingOnForCar(CVehicle* vehicle) {
    return FindPlaybackIndexOrEnd(vehicle) < TOTAL_VEHICLE_RECORDS;
}

// 0x4595A0
bool CVehicleRecording::IsPlaybackPausedForCar(CVehicle* vehicle) {
    // SA code loops through all playbacks but always returns false.
    return false;
}

// unused
// 0x459D10
void CVehicleRecording::SkipForwardInRecording(CVehicle* vehicle, float distance) {
    // Como el original: no comprueba si la encontró (índice 16 fuera de los arrays); `distance` es lo que falta por
    // recorrer: avanza mientras sea > 0 y, si empezó <= 0, retrocede mientras sea < 0 (sin pasar del fotograma 1).
    const auto i = FindPlaybackIndexOrEnd(vehicle);
    auto& index = PlaybackIndex.data()[i];
    const auto buffer = pPlaybackBuffer.data()[i];
    auto frame = FrameAt(buffer, index);

    for (;;) {
        if (distance > 0.0f) {
            const uint32 cur = index;
            if (cur >= (uint32)PlaybackBufferSize.data()[i] - 0x20u) {
                break;
            }
            index = cur + 0x20;
            frame = FrameAt(buffer, cur + 0x20);
            const auto& from = FrameAt(buffer, cur)->m_vecPosn;
            const float dy = from.y - frame->m_vecPosn.y;
            const float dx = from.x - frame->m_vecPosn.x;
            distance -= std::sqrt(dx * dx + dy * dy);
            if (distance > 0.0f || std::isnan(distance)) {
                continue;
            }
            break;
        } else {
            const uint32 cur = index;
            if (cur <= 0x20u) {
                break;
            }
            index = cur - 0x20;
            frame = FrameAt(buffer, cur - 0x20);
            const auto& from = FrameAt(buffer, cur)->m_vecPosn;
            const float dy = from.y - frame->m_vecPosn.y;
            const float dx = from.x - frame->m_vecPosn.x;
            distance += std::sqrt(dx * dx + dy * dy);
            if (distance < 0.0f || std::isnan(distance)) {
                continue;
            }
            break;
        }
    }

    PlaybackRunningTime.data()[i] = (float)frame->m_nTime;
    if (bUseCarAI.data()[i]) {
        RestoreInfoForCar(pVehicleForPlayback.data()[i], *FrameAt(buffer, index), false);
        pVehicleForPlayback.data()[i]->ProcessControlCollisionCheck(false);
    }
}

// unused
// 0x45A4A0
void CVehicleRecording::SkipToEndAndStopPlaybackRecordedCar(CVehicle* vehicle) {
    const auto i = FindPlaybackIndexOrEnd(vehicle);
    if (i >= TOTAL_VEHICLE_RECORDS) {
        return;
    }

    // Como el original: quita bDisableCollisionForce (no bCollidable) y usa el último fotograma por bytes (tamaño - 0x20).
    pVehicleForPlayback[i]->physicalFlags.bDisableCollisionForce = false;
    RestoreInfoForCar(pVehicleForPlayback[i], *FrameAt(pPlaybackBuffer[i], PlaybackBufferSize[i] - 0x20), false);
    pVehicleForPlayback[i]->ProcessControlCollisionCheck(false);
    pVehicleForPlayback[i] = nullptr;
    pPlaybackBuffer[i] = nullptr;
    PlaybackBufferSize[i] = 0;
    bPlaybackGoingOn[i] = false;
    vehicle->m_autoPilot.m_vehicleRecordingId = -1;

    StreamingArray[PlayBackStreamingIndex[i]].RemoveRef();
}

void CVehicleRecording::InjectHooks() {
    RH_ScopedClass(CVehicleRecording);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x459390);
    RH_ScopedInstall(InitAtStartOfGame, 0x45A1B0);
    RH_ScopedInstall(ShutDown, 0x459400);
    RH_ScopedInstall(Render, 0x459F70);
    RH_ScopedInstall(ChangeCarPlaybackToUseAI, 0x45A360);
    RH_ScopedInstall(FindIndexWithFileNameNumber, 0x459FF0);
    RH_ScopedInstall(InterpolateInfoForCar, 0x459B30);
    RH_ScopedInstall(HasRecordingFileBeenLoaded, 0x45A060);
    RH_ScopedInstall(Load, 0x45A8F0);
    RH_ScopedInstall(SmoothRecording, 0x45A0F0);
    RH_ScopedInstall(RegisterRecordingFile, 0x459F80);
    RH_ScopedInstall(RemoveRecordingFile, 0x45A0A0);
    RH_ScopedInstall(RequestRecordingFile, 0x45A020);
    RH_ScopedInstall(StopPlaybackWithIndex, 0x459440);
    RH_ScopedInstall(StartPlaybackRecordedCar, 0x45A980);
    RH_ScopedInstall(StopPlaybackRecordedCar, 0x45A280);
    RH_ScopedInstall(PausePlaybackRecordedCar, 0x459740);
    RH_ScopedInstall(UnpausePlaybackRecordedCar, 0x459850);
    RH_ScopedInstall(SetPlaybackSpeed, 0x459660);
    RH_ScopedInstall(RenderLineSegment, 0x459F00);
    RH_ScopedInstall(RemoveAllRecordingsThatArentUsed, 0x45A160);
    RH_ScopedInstall(RestoreInfoForCar, 0x459A30);
    RH_ScopedInstall(RestoreInfoForMatrix, 0x459960);
    RH_ScopedInstall(SaveOrRetrieveDataForThisFrame, 0x45A610);
    RH_ScopedInstall(SetRecordingToPointClosestToCoors, 0x45A1E0);
    RH_ScopedInstall(IsPlaybackGoingOnForCar, 0x4594C0);
    RH_ScopedInstall(IsPlaybackPausedForCar, 0x4595A0);
    RH_ScopedInstall(SkipForwardInRecording, 0x459D10);
    RH_ScopedInstall(SkipToEndAndStopPlaybackRecordedCar, 0x45A4A0);

    // printf del CRT del EXE (0x821982), usado por 29 llamadores del juego y por varias reconstrucciones (Pools, World, cTransmission, AESoundManager).
    // Bloque propio para que los objetos de ámbito (RHCurrentScopeName...) no choquen con los de esta función.
    {
        RH_ScopedNamespaceName("CRT");
        RH_ScopedCategoryGlobal();
        RH_ScopedGlobalInstall(CrtPrintf, 0x821982);
        RH_ScopedGlobalInstall(CrtAtexit, 0x821D1E);
        RH_ScopedGlobalInstall(CrtLockFile2, 0x823FD1);
        RH_ScopedGlobalInstall(CrtUnlockFile2, 0x824023);
        RH_ScopedGlobalInstall(CrtMalloc, 0x824257);
        RH_ScopedGlobalInstall(CrtErrno, 0x82B181);
    }
}

uint32 CPath::GetIndex() const {
    const auto index = this - CVehicleRecording::StreamingArray.data();
    assert(index >= 0 && static_cast<size_t>(index) < CVehicleRecording::StreamingArray.size());

    return index;
}

void CPath::AddRef() {
    CARREC_LOG("Ref added for path {} (number= {}, size= {}, ptr= {})", GetIndex(), m_nNumber, m_nSize, LOG_PTR(m_pData));
    m_nRefCount++;
}

void CPath::RemoveRef() {
    CARREC_LOG("Ref removed for path {} (number= {}, size= {}, ptr= {})", GetIndex(), m_nNumber, m_nSize, LOG_PTR(m_pData));
    // Como el original (0x459440, 0x45A4A0): al llegar a 0 libera y quita el modelo sin comprobar m_pData.
    if (!--m_nRefCount) {
        CMemoryMgr::Free(m_pData);
        m_pData = nullptr;
        CStreaming::RemoveModel(RRRToModelId(GetIndex()));
    }
}
