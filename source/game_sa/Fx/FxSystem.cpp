/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxSystem_c
 * Reconstrucción: COMPLETA — 35/35 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 35/35 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxSystem.h"
#include "FxPrim.h"
#include "FxEmitter.h"
#include "FxEmitterBP.h"
#include "FxEmitterPrt.h"
#include "FxBox.h"
#include "Particle.h"

void FxSystem_c::InjectHooks() {
    RH_ScopedClass(FxSystem_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4AAF00);
    RH_ScopedInstall(Destructor, 0x4AA260);
    RH_ScopedInstall(Init, 0x4AA750);
    RH_ScopedInstall(Exit, 0x4AA840);
    RH_ScopedInstall(Play, 0x4AA2F0);
    RH_ScopedInstall(Pause, 0x4AA370);
    RH_ScopedInstall(Stop, 0x4AA390);
    RH_ScopedInstall(PlayAndKill, 0x4AA3D0);
    RH_ScopedInstall(Kill, 0x4AA3F0);
    RH_ScopedInstall(AttachToBone, 0x4AA400);
    RH_ScopedOverloadedInstall(AddParticle, "v3d", 0x4AA440, void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool), {});
    RH_ScopedOverloadedInstall(AddParticle, "mat", 0x4AA540, void(FxSystem_c::*)(const RwMatrix&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool), {});
    RH_ScopedInstall(EnablePrim, 0x4AA610);
    RH_ScopedInstall(SetMatrix, 0x4AA630);
    RH_ScopedInstall(SetOffsetPos, 0x4AA660);
    RH_ScopedInstall(AddOffsetPos, 0x4AA690);
    RH_ScopedInstall(CopyParentMatrix, 0x4AA890);
    RH_ScopedInstall(GetCompositeMatrix, 0x4AA8C0);
    RH_ScopedInstall(GetPlayStatus, 0x4AA900);
    RH_ScopedInstall(ForAllParticles, 0x4AA930);
    RH_ScopedInstall(UpdateBoundingBoxCB, 0x4AA9A0, {});
    RH_ScopedInstall(GetBoundingSphereWld, 0x4AAAD0);
    RH_ScopedInstall(GetBoundingSphereLcl, 0x4AAB50);
    RH_ScopedInstall(SetBoundingSphere, 0x4AAB80);
    RH_ScopedInstall(ResetBoundingSphere, 0x4AABF0);
    RH_ScopedInstall(SetConstTime, 0x4AA6C0);
    RH_ScopedInstall(SetRateMult, 0x4AA6F0);
    RH_ScopedInstall(SetTimeMult, 0x4AA710);
    RH_ScopedInstall(SetVelAdd, 0x4AA730);
    RH_ScopedInstall(SetLocalParticles, 0x4AA910);
    RH_ScopedInstall(SetZTestEnable, 0x4AAC50);
    RH_ScopedInstall(SetMustCreatePrts, 0x4AAC70);
    RH_ScopedInstall(DoFxAudio, 0x4AAC90);
    RH_ScopedInstall(IsVisible, 0x4AAF30);
    RH_ScopedInstall(Update, 0x4AAF70, {});
}
FxSystem_c* FxSystem_c::Constructor() { this->FxSystem_c::FxSystem_c(); return this; }
FxSystem_c* FxSystem_c::Destructor() { this->FxSystem_c::~FxSystem_c(); return this; }

// 0x4AAF00
FxSystem_c::FxSystem_c() : ListItem_c() {
    m_SystemBP        = nullptr;
    m_BoundingSphere  = nullptr;
    m_MustCreateParticles = false;
    m_FireAE.m_FxSystem = nullptr; // Como el original: 0x4AAF10 mov [esi+100h],eax (FireAE+0x84 = m_FxSystem = 0)
}

// 0x4AA260
FxSystem_c::~FxSystem_c() {
    m_SystemBP = nullptr;

    if (m_BoundingSphere) // looks pointless
        CMemoryMgr::Free(m_BoundingSphere);

    if (m_allocatedParentMat)
        RwMatrixDestroy(m_ParentMatrix);
}

// 0x4AA750
bool FxSystem_c::Init(FxSystemBP_c* systemBP, const RwMatrix& local, RwMatrix* parent) {
    m_SystemBP     = systemBP;
    m_LocalMatrix  = local;
    m_ParentMatrix = parent;
    m_fCurrentTime = 0;
    m_nPlayStatus  = eFxSystemPlayStatus::FX_STOPPED;
    m_nKillStatus  = eFxSystemKillStatus::FX_NOT_KILLED;
    m_UseConstTime = false;
    m_nConstTime   = 0;
    m_nRateMult    = 1000;
    m_nTimeMult    = 1000;
    m_VelAdd       = CVector();
    m_allocatedParentMat = false; // Como el original: 0x4AA774 (and cl,0FCh) limpia los bits 0 y 1 del byte 0x62 y 0x4AA77F (or cl,4) activa el bit 2
    m_createLocal  = false;
    m_useZTest     = true;

    m_BoundingSphere = nullptr;
    if (m_SystemBP->m_BoundingSphere) {
        m_BoundingSphere = new (true) FxSphere_c(*systemBP->m_BoundingSphere);
    }

    m_Prims = new FxPrim_c * [m_SystemBP->m_nNumPrims];
    // Como el original: el contador es un byte con signo (0x4AA7D8 movsx; 0x4AA7F0 jle; 0x4AA81B cmp/jl) y se relee de la plantilla en cada vuelta
    for (int32 i = 0; i < static_cast<int8>(m_SystemBP->m_nNumPrims); i++) {
        m_Prims[i] = m_SystemBP->m_Prims[i]->CreateInstance();
        m_Prims[i]->Init(m_SystemBP->m_Prims[i], this);
    }

    m_FireAE.Initialise(this);
    return true;
}

// 0x4AA840
void FxSystem_c::Exit() {
    // Como el original: bucle con contador de byte con signo (0x4AA84E jle / 0x4AA86A jl); delete de un puntero nulo no hace nada (0x4AA856 test)
    for (int32 i = 0; i < static_cast<int8>(m_SystemBP->m_nNumPrims); i++) {
        delete m_Prims[i];
    }
    delete[] m_Prims;
    m_FireAE.Terminate();
}

// 0x4AA2F0
void FxSystem_c::Play() {
    if (m_nPlayStatus != eFxSystemPlayStatus::T2) {
        m_fCurrentTime = 0;
        for (int32 i = 0; i < static_cast<int8>(m_SystemBP->m_nNumPrims); i++) { // Como el original: byte con signo (0x4AA30C jle / 0x4AA325 jl)
            m_Prims[i]->Reset();
        }
    }

    m_nKillStatus = eFxSystemKillStatus::FX_NOT_KILLED;
    m_nPlayStatus = eFxSystemPlayStatus::FX_PLAYING;

    m_stopParticleCreation = false;
    m_prevCulled = false;

    // Como el original: 0x4AA34C fmul [0x858FC4] (0.0001f = 0x38D1B717), no una division entre 10000 (0x4AA33D idiv ecx,2710h sobre rand 0x821B1E)
    m_LoopInterval = (float)(CGeneral::GetRandomNumber() % 10'000) * std::bit_cast<float>(0x38D1B717u) * (m_SystemBP->m_fLoopLength - m_SystemBP->m_fLoopIntervalMin) + m_SystemBP->m_fLoopIntervalMin;
}

// 0x4AA370
void FxSystem_c::Pause() {
    if (m_nPlayStatus == eFxSystemPlayStatus::FX_STOPPED)
        return;

    if (m_nPlayStatus == eFxSystemPlayStatus::T2) {
        m_nPlayStatus = eFxSystemPlayStatus::FX_PLAYING;
    } else {
        m_nPlayStatus = eFxSystemPlayStatus::T2;
    }
}

// 0x4AA390
void FxSystem_c::Stop() {
    m_nPlayStatus = eFxSystemPlayStatus::FX_STOPPED;
    m_fCurrentTime = 0;

    for (int32 i = 0; i < static_cast<int8>(m_SystemBP->m_nNumPrims); i++) { // Como el original: byte con signo (0x4AA3A9 jle / 0x4AA3C5 jl)
        m_Prims[i]->Reset();
    }
}

// 0x4AA3D0
void FxSystem_c::PlayAndKill() {
    Play();

    if (m_SystemBP->m_nPlayMode != 0)
        return;

    m_nKillStatus = FX_PLAY_AND_KILL;
}

// 0x4AA3F0
void FxSystem_c::Kill() {
    Stop();
    m_nKillStatus = FX_KILLED;
}

// 0x4AA400
void FxSystem_c::AttachToBone(CEntity* entity, eBoneTag boneId) {
    // Como el original: 0x4AA40D call 0x734A40 (GetAnimHierarchyFromSkinClump sobre [entity+0x18]), 0x4AA41A call 0x7C51A0 (RpHAnimIDGetIndex),
    // 0x4AA422 call 0x7C5120 (RpHAnimHierarchyGetMatrixArray) y 0x4AA42A shl esi,6 + add: matriz = array + indice * 64.
    // No es CEntity::GetBoneMatrix (que busca la jerarquia por el marco del clump, comprueba el rango y puede devolver nullptr).
    auto* const hierarchy = GetAnimHierarchyFromSkinClump(reinterpret_cast<RpClump*>(entity->GetRwObject()));
    const auto index = RpHAnimIDGetIndex(hierarchy, static_cast<RwInt32>(boneId));
    m_ParentMatrix = RpHAnimHierarchyGetMatrixArray(hierarchy) + index;
}

static bool CanAddParticle() {
    const float scaled = float(CGeneral::GetRandomNumber()) * (1.0f / 32768.0f);
    const int32 probability = static_cast<int32>(scaled * 100.0f);
    if (g_fx.GetFxQuality() == FX_QUALITY_LOW && probability < 50) {
        return false;
    }
    return g_fx.GetFxQuality() != FX_QUALITY_MEDIUM || probability >= 25;
}

// 0x4AA440
void FxSystem_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float lightMult, float lightMultLimit, bool createLocal) {
    if (CanAddParticle()) {
        const auto brightness = lightMult < lightMultLimit ? 1.0f - lightMultLimit + lightMult : 1.0f;
        for (int32 i = 0; i < static_cast<int8>(m_SystemBP->m_nNumPrims); ++i) {
            auto* prim = m_Prims[i];
            if (prim->m_bEnabled) {
                prim->AddParticle(pos, vel, timeSince, fxMults, rotZ, brightness, createLocal);
            }
        }
    }
}

// unused
// 0x4AA540
void FxSystem_c::AddParticle(const RwMatrix& mat, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float lightMult, float lightMultLimit, bool createLocal) {
    if (CanAddParticle()) {
        const auto brightness = lightMult < lightMultLimit ? 1.0f - lightMultLimit + lightMult : 1.0f;
        for (int32 i = 0; i < static_cast<int8>(m_SystemBP->m_nNumPrims); ++i) {
            auto* prim = m_Prims[i];
            prim->AddParticle(mat, vel, timeSince, fxMults, rotZ, brightness, createLocal);
        }
    }
}

// 0x4AA610
void FxSystem_c::EnablePrim(int32 primIndex, bool enable) {
    m_Prims[primIndex]->Enable(enable);
}

// 0x4AA630
void FxSystem_c::SetMatrix(RwMatrix* matrix) {
    if (m_allocatedParentMat)
        *m_ParentMatrix = *matrix;
    else
        m_ParentMatrix = matrix;
}

// 0x4AA660
void FxSystem_c::SetOffsetPos(const CVector& pos) {
    m_LocalMatrix.pos = *(RwV3d*)&pos;
    RwMatrixUpdate(&m_LocalMatrix);
}

// 0x4AA690
void FxSystem_c::AddOffsetPos(const CVector& pos) {
    RwV3dAdd(&m_LocalMatrix.pos, &m_LocalMatrix.pos, &pos);
    RwMatrixUpdate(&m_LocalMatrix);
}

// 0x4AA6C0
void FxSystem_c::SetConstTime(bool on, float time) {
    m_UseConstTime = on;
    m_nConstTime   = (uint16)(int64)(time * 256.0f); // Como el original: _ftol2 (0x4AA6D4 call 0x821B40) trunca a 64 bits y se guarda ax
}

// 0x4AA6F0
void FxSystem_c::SetRateMult(float mult) {
    m_nRateMult = (uint16)(int64)(mult * 1000.0f); // Como el original: _ftol2 (0x4AA6FD) y mov [esi+5Eh],ax
}

// 0x4AA710
void FxSystem_c::SetTimeMult(float mult) {
    m_nTimeMult = (uint16)(int64)(mult * 1000.0f); // Como el original: _ftol2 (0x4AA71D) y mov [esi+60h],ax
}

// 0x4AA730
void FxSystem_c::SetVelAdd(const CVector& velocity) {
    m_VelAdd = velocity;
}

// 0x4AA910
void FxSystem_c::SetLocalParticles(bool enable) {
    m_createLocal = enable;
}

// 0x4AAC50
void FxSystem_c::SetZTestEnable(bool enable) {
    m_useZTest = enable;
}

// 0x4AAC70
void FxSystem_c::SetMustCreatePrts(bool enable) {
    m_MustCreateParticles = enable;
}

// 0x4AA890
void FxSystem_c::CopyParentMatrix() {
    RwMatrix* old = m_ParentMatrix;
    RwMatrix* allocated = RwMatrixCreate();
    m_ParentMatrix = allocated;
    *allocated = *old;
    m_allocatedParentMat = true;
}

// 0x4AA8C0
void FxSystem_c::GetCompositeMatrix(RwMatrix* out) const {
    if (m_ParentMatrix)
        RwMatrixMultiply(out, &m_LocalMatrix, m_ParentMatrix);
    else
        *out = m_LocalMatrix;
}

// 0x4AA900
eFxSystemPlayStatus FxSystem_c::GetPlayStatus() const {
    return m_nPlayStatus;
}

// 0x4AA930
uint32 FxSystem_c::ForAllParticles(void(*callback)(Particle_c*, int32, FxBox_c**), FxBox_c* data) {
    auto count = 0;

    // Como el original: byte con signo (0x4AA93F jle / 0x4AA987 jl) y la plantilla se relee en cada vuelta (0x4AA951)
    for (int32 i = 0; i < static_cast<int8>(m_SystemBP->m_nNumPrims); i++) {
        auto* prim = m_SystemBP->m_Prims[i];
        for (auto* particle = prim->m_Particles.GetHead(); particle; particle = prim->m_Particles.GetNext(particle)) {
            // Como el original: 0x4AA95D test al,al ; jne 0x4AA976 -> solo se llama cuando el tipo es 0 (no distinto de 0)
            if (!prim->m_Type && this == particle->m_System) {
                callback(particle, 0, &data);
                count++;
            }
        }
    }

    return count;
}

// 0x4AA9A0
void FxSystem_c::UpdateBoundingBoxCB(Particle_c* particle, int32 a2, FxBox_c** data) {
    if (a2 != 0) {
        return;
    }
    auto& box = **data;
    const auto& pos = particle->m_Pos;
    if (pos.x < box.minX) box.minX = pos.x;
    if (pos.x > box.maxX) box.maxX = pos.x;
    if (pos.y < box.minY) box.minY = pos.y;
    if (pos.y > box.maxY) box.maxY = pos.y;
    if (pos.z < box.minZ) box.minZ = pos.z;
    if (pos.z > box.maxZ) box.maxZ = pos.z;
}

// 0x4AAA40
void FxSystem_c::GetBoundingBox(FxBox_c* out) {
    out->Reset();

    if (ForAllParticles(UpdateBoundingBoxCB, out) != 0) {
        return;
    }

    auto mat = g_fxMan.FxRwMatrixCreate();
    if (m_ParentMatrix) {
        RwMatrixMultiply(mat, &m_LocalMatrix, m_ParentMatrix);
    } else {
        *mat = m_LocalMatrix;
    }

    out->minX = out->maxX = mat->pos.x;
    out->minY = out->maxY = mat->pos.y;
    out->minZ = out->maxZ = mat->pos.z;
    g_fxMan.FxRwMatrixDestroy(mat);
}

// 0x4AAAD0
bool FxSystem_c::GetBoundingSphereWld(FxSphere_c* out) const {
    if (!m_BoundingSphere)
        return false;

    auto mat = g_fxMan.FxRwMatrixCreate();
    GetCompositeMatrix(mat);

    RwV3dTransformPoints(reinterpret_cast<RwV3d*>(out), reinterpret_cast<const RwV3d*>(m_BoundingSphere), 1, mat);
    out->m_fRadius = m_BoundingSphere->m_fRadius;
    g_fxMan.FxRwMatrixDestroy(mat);
    return true;
}

// 0x4AAB50
bool FxSystem_c::GetBoundingSphereLcl(FxSphere_c* out) const {
    if (!m_BoundingSphere)
        return false;

    *out = *m_BoundingSphere;
    return true;
}

// 0x4AAB80
void FxSystem_c::SetBoundingSphere(FxSphere_c* sphere) {
    if (sphere) {
        if (!m_BoundingSphere) {
            m_BoundingSphere = new (true) FxSphere_c();
        }
        // Como el original: 0x4AABB4 / 0x4AABC8 rep movsd (5 dwords) copia el contenido; no adopta el puntero ni sale sin copiar
        std::memcpy(m_BoundingSphere, sphere, sizeof(FxSphere_c));
    } else if (m_BoundingSphere) {
        CMemoryMgr::Free(m_BoundingSphere);
        m_BoundingSphere = nullptr;
    }
}

// 0x4AABF0
void FxSystem_c::ResetBoundingSphere() {
    if (m_SystemBP->m_BoundingSphere) {
        if (!m_BoundingSphere) {
            m_BoundingSphere = new (true) FxSphere_c();
        }
        // Como el original: 0x4AAC27 rep movsd (5 dwords) copia la esfera de la plantilla al bloque propio (no se comparte el puntero)
        std::memcpy(m_BoundingSphere, m_SystemBP->m_BoundingSphere, sizeof(FxSphere_c));
    } else if (m_BoundingSphere) {
        CMemoryMgr::Free(m_BoundingSphere);
        m_BoundingSphere = nullptr;
    }
}

// 0x4AAC90
void FxSystem_c::DoFxAudio(CVector pos) {
    constexpr struct { const char* hash; eAudioEvents event; } mapping[] = {
        { "fire",           AE_FIRE               },
        { "fire_med",       AE_FIRE_MEDIUM        },
        { "fire_large",     AE_FIRE_LARGE         },
        { "fire_car",       AE_FIRE_CAR           },
        { "fire_bike",      AE_FIRE_BIKE          },
        { "Flame",          AE_FIRE_FLAME         },
        { "molotov_flame",  AE_FIRE_MOLOTOV_FLAME },
        { "water_hydrant",  AE_FIRE_HYDRANT       },
        { "water_fountain", AE_FIRE_HYDRANT       },
        { "water_fnt_tme",  AE_FIRE_HYDRANT       },
        { "smoke_flare",    AE_SMOKE_FLARE        },
        { "teargas",        AE_TEARGAS            },
        { "heli_dust",      AE_HELI_DUST          },
    };
    for (auto& [hash, event] : mapping) {
        if (m_SystemBP->GetNameKey() == CKeyGen::GetUppercaseKey(hash)) {
            m_FireAE.AddAudioEvent(event, pos);
        }
    }
}

// 0x4AAF30
bool FxSystem_c::IsVisible() const {
    FxSphere_c sphere;
    if (GetBoundingSphereWld(&sphere)) {
        FxFrustumInfo_c* info = g_fxMan.GetFrustumInfo();
        if (!info->IsCollision(sphere))
            return false;
    }
    return true;
}

// 0x4AAF70
bool FxSystem_c::Update(RwCamera* camera, float timeDelta) {
    if (m_nKillStatus == FX_3) {
        return true;
    }
    if (m_nKillStatus == FX_KILLED) {
        m_nKillStatus = FX_3;
        return false;
    }
    auto* matrix = g_fxMan.FxRwMatrixCreate();
    GetCompositeMatrix(matrix);
    const float previousDistance = m_fCameraDistance;
    const auto* cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(camera));
    g_fxMan.FxRwMatrixDestroy(matrix);
    const CVector difference = CVector(cameraMatrix->pos) - CVector(matrix->pos);
    m_fCameraDistance = difference.Magnitude();
    const float cullDistance = float(m_SystemBP->m_nCullDist) * (1.0f / 256.0f);
    bool culled = false;
    bool updated = false;
    if (m_SystemBP->m_nPlayMode == 0 || (m_fCameraDistance < cullDistance && IsVisible())) {
        if (m_nPlayStatus == eFxSystemPlayStatus::FX_PLAYING) {
            updated = true;
            timeDelta = (float(m_nTimeMult) * 0.001f) * timeDelta;
            m_fCurrentTime = m_UseConstTime ? float(m_nConstTime) * (1.0f / 256.0f) : m_fCurrentTime + timeDelta;
            if (m_SystemBP->m_nPlayMode == 2 && m_SystemBP->m_fLoopIntervalMin > 0.0f) {
                if (m_fCurrentTime > m_SystemBP->m_fLength) {
                    m_stopParticleCreation = true;
                }
                const float period = m_SystemBP->m_fLength + m_LoopInterval;
                if (period < m_fCurrentTime) {
                    m_fCurrentTime -= period;
                    const float random = float(static_cast<int32>(CGeneral::GetRandomNumber()) % 10000) * 0.0001f;
                    m_stopParticleCreation = false;
                    m_LoopInterval = random * (m_SystemBP->m_fLoopLength - m_SystemBP->m_fLoopIntervalMin) + m_SystemBP->m_fLoopIntervalMin;
                }
            } else if (m_fCurrentTime > m_SystemBP->m_fLength) {
                switch (m_SystemBP->m_nPlayMode) {
                case 0:
                    Stop();
                    if (m_nKillStatus == FX_PLAY_AND_KILL) m_nKillStatus = FX_KILLED;
                    break;
                case 1:
                    m_fCurrentTime = m_SystemBP->m_fLength;
                    break;
                case 2:
                    m_fCurrentTime -= m_SystemBP->m_fLength;
                    break;
                case 3:
                    Stop();
                    break;
                }
            }
            if (m_prevCulled) timeDelta += 0.25f;
            for (int32 i = 0; i < static_cast<int8>(m_SystemBP->m_nNumPrims); ++i) {
                m_Prims[i]->Update(m_fCurrentTime, timeDelta);
            }
        }
    } else {
        culled = true;
    }
    if (!updated && m_nPlayStatus == eFxSystemPlayStatus::FX_STOPPED && m_fCameraDistance < cullDistance && previousDistance >= cullDistance && m_SystemBP->m_nPlayMode == 3) {
        Play();
    }
    m_prevCulled = culled;
    if (m_nPlayStatus == eFxSystemPlayStatus::FX_PLAYING) {
        DoFxAudio(CVector(matrix->pos));
    }
    return false;
}

// NOTSA
std::span<FxPrim_c*> FxSystem_c::GetPrims() {
    return std::span{ m_Prims, m_SystemBP->m_nNumPrims };
}
