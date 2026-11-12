/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCutsceneObject
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CutsceneObject.h"

#include "RealTimeShadowManager.h"
#include "Shadows.h"
#include "CarFXRenderer.h"
#include "CustomBuildingDNPipeline.h"

namespace CObjectBody { void __fastcall Destructor(CObject* self, void* edx); } // 0x59F660, Object.cpp (sin hook)
namespace { void __fastcall CutsceneObjectDestructorBody(CCutsceneObject* self, void* edx); } // 0x5B1A80, mas abajo

void CCutsceneObject::InjectHooks() {
    RH_ScopedVirtualClass(CCutsceneObject, 0x868A60, 23);
    RH_ScopedCategory("Entity/Object");

    RH_ScopedInstall(ScalarDeletingDestructor, 0x5B1DE0);
    RH_ScopedVMTInstall(SetModelIndex, 0x5B1B20);
    RH_ScopedVMTInstall(SetupLighting, 0x553F40);
    RH_ScopedVMTInstall(RemoveLighting, 0x5533F0);
    RH_ScopedVMTInstall(ProcessControl, 0x5B1B90);
    RH_ScopedVMTInstall(PreRender, 0x5B1E00);
    RH_ScopedInstall(SetupCarPipeAtomicsForClump, 0x5B1AB0);
    RH_ScopedInstall(Constructor, 0x5B19D0); // lote 172A (banco Fn172A)
    RH_ScopedNamedGlobalInstall(CutsceneObjectDestructorBody, "DestructorBody", 0x5B1A80); // lote 172A (banco Fn172A)
    RH_ScopedInstall(SetupCarPipeAtomicCB, 0x5B1A90); // lote 172A (banco Fn172A)
}

// 0x5B19D0 (lote 172A, banco Fn172A; thiscall, sin argumentos de pila, `ret`, 175 bytes con marco SEH que solo desenrolla y aqui se omite): constructor del EXE. Como el original, con el orden exacto de
// escrituras (todas crudas: los campos que el constructor de la base deja se modifican con AND/OR sobre lo que hay): constructor por defecto de CObject del EXE (0x5A1D10, sin hook: el de la DLL escribe vtables
// propias), vtable de la clase del EXE (0x868A60), los seis dwords de +0x184..+0x198 a cero, el BYTE +0x36 = (byte & 7) | 0x10, el dword +0x1C = (dword & ~1) | 0x10000400 (se lee ANTES de la llamada y se escribe
// despues de +0x13C = 4, +0x17C y +0x180 = 0), SetIsStatic(false) (0x5A0760, con hook; se invoca como llamada directa a la clase CObject), +0x1C con el bit 0x08000000 a cero (relectura tras la llamada) y
// +0x12C = 0.5f. Devuelve `this`.
CCutsceneObject* CCutsceneObject::Constructor() {
    uint8* const b = reinterpret_cast<uint8*>(this);
    plugin::CallMethod<0x5A1D10, CObject*>(static_cast<CObject*>(this));
    *reinterpret_cast<uint32*>(b) = 0x868A60;
    for (const size_t off : { size_t(0x184), size_t(0x188), size_t(0x18C), size_t(0x190), size_t(0x194), size_t(0x198) }) {
        *reinterpret_cast<uint32*>(b + off) = 0;
    }
    const uint8  byte36 = b[0x36];
    uint32       flags = *reinterpret_cast<uint32*>(b + 0x1C);
    b[0x36] = uint8((byte36 & 7) | 0x10);
    flags = (flags & ~1u) | 0x10000400u;
    b[0x13C] = 4;
    *reinterpret_cast<uint32*>(b + 0x17C) = 0;
    *reinterpret_cast<uint32*>(b + 0x180) = 0;
    *reinterpret_cast<uint32*>(b + 0x1C) = flags;
    this->CObject::SetIsStatic(false);
    *reinterpret_cast<volatile uint32*>(b + 0x1C) = *reinterpret_cast<volatile uint32*>(b + 0x1C) & 0xF7FFFFFFu;
    *reinterpret_cast<uint32*>(b + 0x12C) = 0x3F000000u;
    return this;
}

// 0x5B19D0
CCutsceneObject::CCutsceneObject() : CObject() {
    m_vWorldPosition.Set(0.0F, 0.0F, 0.0F);
    m_vForce.Set(0.0F, 0.0F, 0.0F);
    SetUsesCollision(false);
    SetStatus(STATUS_SIMPLE);
    m_nObjectType = eObjectType::OBJECT_TYPE_CUTSCENE;
    m_nAttachBone = 0;
    m_pAttachmentObject = nullptr;
    m_bLightObject = true;
    m_bStreamingDontDelete = true;

    CObject::SetIsStatic(false);
    SetIsBackfaceCulled(false);
    m_fContactSurfaceBrightness = 0.5F;
}

// 0x5B1B20
void CCutsceneObject::SetModelIndex(unsigned index) {
    CEntity::SetModelIndex(index);
    if (RwObjectGetType(GetRwObject()) == rpCLUMP) {
        RpAnimBlendClumpInit(GetRpClump());
        auto* animData = RpAnimBlendClumpGetData(GetRpClump());
        animData->m_PedPosition = &m_vecMoveSpeed;
        animData->m_FrameDatas->HasZVelocity = true;
        CCutsceneObject::SetupCarPipeAtomicsForClump(index, GetRpClump());
    }
    CModelInfo::GetModelInfo(index)->m_nAlpha = 0xFF; // Como el original: usa el índice recibido
}

// 0x5B1B90
void CCutsceneObject::ProcessControl() {
    if (m_nAttachBone && m_pAttachmentObject && !GetWasPostponed()) {
        SetWasPostponed(true);
        return;
    }

    CPhysical::ProcessControl(); // exactly CPhysical
    if (m_pAttachToFrame) {
        if (m_pAttachmentObject) {
            m_pAttachmentObject->UpdateRpHAnim(); // Como el original: actualiza la jerarquía del objeto antes de leer sus matrices
            auto* hierarchy = GetAnimHierarchyFromClump(m_pAttachmentObject->GetRpClump());
            auto* matArr = RpHAnimHierarchyGetMatrixArray(hierarchy);
            const auto boneMat = CMatrix(&matArr[m_nAttachBone], false);
            *static_cast<CMatrix*>(m_matrix) = boneMat;
            SetWasPostponed(false); // Como el original: limpia el aplazamiento tras copiar la matriz del bone
        } else {
            auto* pLtm = RwFrameGetLTM(m_pAttachToFrame);
            const auto attachMat = CMatrix(pLtm, false);
            *static_cast<CMatrix*>(m_matrix) = attachMat;
        }
    } else {
        if (!(CTimer::GetTimeStep() < 0.01F)) // Como el original: con NaN también divide
            m_vecMoveSpeed *= 1.0F / CTimer::GetTimeStep();
        else
            m_vecMoveSpeed *= 100.0F;

        if (physicalFlags.bDontApplySpeed || physicalFlags.bDisableMoveForce)
            ResetMoveSpeed();
        else {
            m_vForce += m_vecMoveSpeed * CTimer::GetTimeStep();
            m_matrix->SetTranslateOnly(m_vWorldPosition + m_vForce);
        }
    }
}

// 0x5B1E00
void CCutsceneObject::PreRender() {
    if (m_pAttachToFrame) {
        if (m_pAttachmentObject) {
            auto* hierarchy = GetAnimHierarchyFromClump(m_pAttachmentObject->GetRpClump());
            auto* matArr = RpHAnimHierarchyGetMatrixArray(hierarchy);
            const auto boneMat = CMatrix(&matArr[m_nAttachBone], false);
            *static_cast<CMatrix*>(m_matrix) = boneMat;
        } else {
            auto* pLtm = RwFrameGetLTM(m_pAttachToFrame);
            const auto attachMat = CMatrix(pLtm, false);
            *static_cast<CMatrix*>(m_matrix) = attachMat;
        }

        if (RwObjectGetType(GetRwObject()) == rpCLUMP) {
            const auto* firstAtomic = GetFirstAtomic(GetRpClump());
            if (firstAtomic) {
                if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(firstAtomic))) {
                    // Como el original: vuelve a pedir el primer atomic y copia la traslación del keyframe
                    // del primer frame (no BonePos) al centro de la esfera del morph target 0
                    const auto* atomic = GetFirstAtomic(GetRpClump());
                    auto* animData = RpAnimBlendClumpGetData(GetRpClump());
                    auto* morphTarget = RpGeometryGetMorphTarget(RpAtomicGetGeometry(atomic), 0);
                    auto* sphere = RpMorphTargetGetBoundingSphere(morphTarget);
                    sphere->center = animData->m_FrameDatas[0].KeyFrame->t;
                }
            }
        }
    }

    if (RwObjectGetType(GetRwObject()) == rpCLUMP)
        CEntity::UpdateRpHAnim();

    g_realTimeShadowMan.DoShadowThisFrame(this);
    if (!m_pShadowData) {
        CShadows::StoreShadowForPedObject(
            AsPed(),
            CTimeCycle::m_fShadowDisplacementX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowDisplacementY[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowFrontX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowFrontY[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowSideX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowSideY[CTimeCycle::m_CurrentStoredValue]
        );
    }

    if (m_nModelIndex == MODEL_CSPLAY) {
        CPed::ShoulderBoneRotation(GetRpClump());
        m_bDontUpdateHierarchy = true;
    }

    CVector vecPos = GetPosition();
    vecPos.z += 0.5F;
    const auto fHeight = vecPos.z - 5.0F;
    CColPoint colPoint{};
    CEntity* entity = nullptr;
    if (CWorld::ProcessVerticalLine(vecPos, fHeight, colPoint, entity, true)) {
        // Como el original: día/noche multiplicados por 1/30 (0x858F10) y mezcla con timestep * 0.1
        const auto  lighting = colPoint.m_nLightingB;
        const float balance  = CCustomBuildingDNPipeline::m_fDNBalanceParam;
        const float t        = CTimer::GetTimeStep() * 0.1F;
        constexpr float k    = std::bit_cast<float>(0x3D088889u); // 1/30
        const float fDayNight = static_cast<float>(lighting.day) * k * (1.0F - balance) + static_cast<float>(lighting.night) * k * balance;
        m_fContactSurfaceBrightness = fDayNight * t + (1.0F - t) * m_fContactSurfaceBrightness;
    }
}

// 0x553F40
bool CCutsceneObject::SetupLighting() {
    ActivateDirectional();
    return CRenderer::SetupLightingForEntity(this);
}

// 0x5533F0
void CCutsceneObject::RemoveLighting(bool bRemove) {
    if (!physicalFlags.bRenderScorched)
        CPointLights::RemoveLightsAffectingObject();

    SetAmbientColours();
    DeActivateDirectional();
}

// 0x5B1AB0
// Calcula una vez las claves de nombres de modelo y configura el pipeline de
// vehículos solo para los atomics de modelos reconocidos en las cinemáticas.
void CCutsceneObject::SetupCarPipeAtomicsForClump(unsigned modelId, RpClump* clump) {
    static auto& bCarPipeAtomicsInitialized = StaticRef<bool>(0xBC4058); // false
    static auto& anHashKeys = StaticRef<uint32[NUM_CUTSCENE_VEHS]>(0xBC4040);

    if (!bCarPipeAtomicsInitialized) {
        bCarPipeAtomicsInitialized = true;
        for (auto i = 0; i < NUM_CUTSCENE_VEHS; ++i) {
            anHashKeys[i] = CKeyGen::GetUppercaseKey(ms_sCutsceneVehNames[i]);
        }
    }

    for (auto anHashKey : anHashKeys) {
        if (CModelInfo::GetModelInfo(modelId)->m_nKey == anHashKey) {
            RpClumpForAllAtomics(clump, SetupCarPipeAtomicCB, nullptr);
            return;
        }
    }
}

RpAtomic* CCutsceneObject::SetupCarPipeAtomicCB(RpAtomic* atomic, void* data) {
    UNUSED(data);
    CCarFXRenderer::CustomCarPipeAtomicSetup(atomic);
    return atomic;
}

namespace {
// 0x5B1A80 (lote 125A, banco Fn125A; lote 172A: con hook, banco Fn172A; thiscall, sin argumentos de pila, `jmp`, 11 bytes): cuerpo del destructor de CCutsceneObject del EXE. Como el original: vtable de la clase
// (0x868A60) y salto al cuerpo del destructor de CObject (0x59F660; aqui su funcion propia sin hook CObjectBody::Destructor). El destructor generado por el compilador dejaria la vtable de la DLL.
// Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x5B1A80` del EXE.
__declspec(noinline) void __fastcall CutsceneObjectDestructorBody(CCutsceneObject* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x868A60;
    CObjectBody::Destructor(static_cast<CObject*>(self), nullptr);
}
} // namespace

uintptr_t Lote125ACutsceneObjectDestructorBody() { return reinterpret_cast<uintptr_t>(&CutsceneObjectDestructorBody); } // para el banco Fn125A

// 0x5B1DE0 (lote 125A, banco Fn125A) : destructor con borrado de CCutsceneObject del EXE (ranura 0 de su vtable 0x868A60; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x5B1A80; test byte [esp+8],1; je ..; push esi; call 0x5A1F20; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x5B1A80; aqui su envoltura Destructor()), libera `this` con CObject::operator delete (0x5A1F20, sin hook: pool de objetos 0xB7449C) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. 
CCutsceneObject* CCutsceneObject::ScalarDeletingDestructor(uint8 flags) {
    CutsceneObjectDestructorBody(this, nullptr);
    if (flags & 1) {
        CObject::operator delete(this);
    }
    return this;
}
