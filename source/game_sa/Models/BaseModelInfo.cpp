/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBaseModelInfo
 * Reconstrucción: COMPLETA — 34/34 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 34/34 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"
#include <intrin.h>
#include "X87Math.h"

#include "BaseModelInfo.h"

/**
 * Registra las operaciones comunes compartidas por todos los tipos de model info.
 */
// Como el original: 0x5B3AD0 es cdecl (modelInfo, flags) y solo usa EAX/ECX; sus llamadores del EXE siguen usando EDX despues de la
// llamada. Se declara __fastcall sin parametros en la pila (ECX y EDX se reciben por registro y la limpieza de pila sigue siendo la
// del llamador cdecl) para recibir el EDX de entrada y devolverlo intacto; los argumentos se leen de la pila bajo la direccion de retorno.
#pragma runtime_checks("", off)
static uint32 __fastcall SetBaseModelInfoFlags_KeepEdx(void* /*ecx*/, uint32 entryEdx) {
    const uint32* const arguments = static_cast<const uint32*>(_AddressOfReturnAddress()) + 1; // [esp+4] = modelInfo, [esp+8] = flags
    SetBaseModelInfoFlags(reinterpret_cast<CBaseModelInfo*>(static_cast<uintptr_t>(arguments[0])), arguments[1]);
    return ResidueRegsCpp(0, entryEdx, 0);
}
#pragma runtime_checks("", restore)

void CBaseModelInfo::InjectHooks() {
    RH_ScopedVirtualClass(CBaseModelInfo, 0x85BB9C, 15);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(AsAtomicModelInfoPtr, 0x4C4A80);
    RH_ScopedVMTInstall(AsDamageAtomicModelInfoPtr, 0x4C4A90);
    RH_ScopedVMTInstall(AsLodAtomicModelInfoPtr, 0x4C4AA0);
    RH_ScopedVMTInstall(GetTimeInfo, 0x4C4AB0);
    RH_ScopedVMTInstall(Init, 0x4C4B10);
    RH_ScopedVMTInstall(Shutdown, 0x4C4D50);
    RH_ScopedVMTInstall(SetAnimFile, 0x4C4AC0);
    RH_ScopedVMTInstall(ConvertAnimFileIndex, 0x4C4AD0);
    RH_ScopedVMTInstall(GetAnimFileIndex, 0x4C4AE0);
    RH_ScopedInstall(SetTexDictionary, 0x4C4B40);
    RH_ScopedInstall(ClearTexDictionary, 0x4C4B70);
    RH_ScopedInstall(AddTexDictionaryRef, 0x4C4B80);
    RH_ScopedInstall(RemoveTexDictionaryRef, 0x4C4B90);
    RH_ScopedInstall(AddRef, 0x4C4BA0);
    RH_ScopedInstall(RemoveRef, 0x4C4BB0);
    RH_ScopedInstall(SetColModel, 0x4C4BC0);
    RH_ScopedInstall(Init2dEffects, 0x4C4C20);
    RH_ScopedInstall(DeleteCollisionModel, 0x4C4C40);
    RH_ScopedInstall(Get2dEffect, 0x4C4C70);
    RH_ScopedInstall(Add2dEffect, 0x4C4D20);
    RH_ScopedInstall(GetIsDrawLast, 0x5328C0);
    RH_ScopedInstall(HasBeenPreRendered, 0x5328B0);
    RH_ScopedInstall(HasComplexHierarchy, 0x4C4E00);
    RH_ScopedInstall(IsBackfaceCulled, 0x5328F0);
    RH_ScopedInstall(IsLod, 0x4C4A00);
    RH_ScopedInstall(IsRoad, 0x4C4DF0);
    RH_ScopedInstall(SetHasBeenPreRendered, 0x4C42F0);
    RH_ScopedInstall(SetIsLod, 0x4C4A10);
    RH_ScopedInstall(SetOwnsColModel, 0x5328D0);
    RH_ScopedInstall(IncreaseAlpha, 0x532900);

    RH_ScopedInstall(IsBreakableStatuePart, 0x59F090);
    RH_ScopedInstall(IsTagModel, 0x49CC20);
    RH_ScopedInstall(SwaysInWind, 0x4212C0);

    RH_ScopedNamedGlobalInstall(SetBaseModelInfoFlags_KeepEdx, "SetBaseModelInfoFlags", 0x5B3AD0);
}


// Inicializa el conteo de referencias y deja el TXD sin resolver.
CBaseModelInfo::CBaseModelInfo() {
    m_nRefCount = 0;
    ClearTexDictionary();
}

// 0x4212C0: returns a full-width integer, not an AL-only bool.
int32 CBaseModelInfo::SwaysInWind() const {
    return IsSwayInWind1() || IsSwayInWind2();
}

// 0x4C4A80
CAtomicModelInfo* CBaseModelInfo::AsAtomicModelInfoPtr() {
    return nullptr;
}

// 0x4C4A90
CDamageAtomicModelInfo* CBaseModelInfo::AsDamageAtomicModelInfoPtr() {
    return nullptr;
}

// 0x4C4AA0
CLodAtomicModelInfo* CBaseModelInfo::AsLodAtomicModelInfoPtr() {
    return nullptr;
}

// 0x4C4AB0
CTimeInfo* CBaseModelInfo::GetTimeInfo() {
    return nullptr;
}

// 0x4C4B10
/**
 * Restablece el estado común de un model info antes de cargar recursos concretos.
 *
 * No crea RwObject ni CColModel: sólo fija centinelas, flags y valores por defecto.
 */
void CBaseModelInfo::Init() {
    m_nRefCount = 0;
    m_pColModel = nullptr;
    ClearTexDictionary();
    Init2dEffects();
    m_nObjectInfoIndex = -1;
    m_fDrawDistance = 2000.0F;
    m_pRwObject = nullptr;

    m_nFlags = 0;
    bIsBackfaceCulled = true;
    bIsLod = true;
}

// 0x4C4D50
/**
 * Ejecuta la limpieza polimórfica del recurso RenderWare y de la colisión poseída,
 * y devuelve los metadatos comunes a su estado inicial.
 */
void CBaseModelInfo::Shutdown() {
    DeleteRwObject();
    DeleteCollisionModel();

    bIsLod = true;
    m_nObjectInfoIndex = -1;
    ClearTexDictionary();
    Init2dEffects();
}

// 0x4C4AC0
void CBaseModelInfo::SetAnimFile(const char* filename) {}

// 0x4C4AD0
void CBaseModelInfo::ConvertAnimFileIndex() {
    // NOP
}

// 0x4C4AE0
int32 CBaseModelInfo::GetAnimFileIndex() {
    return -1;
}

// 0x4C4B40
void CBaseModelInfo::SetTexDictionary(const char* txdName) {
    // Como el original: guarda el slot y compara sólo los 16 bits guardados (cmp ax,0FFFFh) antes de añadirlo.
    m_nTxdIndex = (int16)CTxdStore::FindTxdSlot(txdName);
    if (m_nTxdIndex == -1) {
        m_nTxdIndex = (int16)CTxdStore::AddTxdSlot(txdName);
    }
}

void CBaseModelInfo::ClearTexDictionary() {
    m_nTxdIndex = -1;
}

void CBaseModelInfo::AddTexDictionaryRef() {
    CTxdStore::AddRef(m_nTxdIndex);
}

void CBaseModelInfo::RemoveTexDictionaryRef() {
    CTxdStore::RemoveRef(m_nTxdIndex);
}

// Incrementa conjuntamente la referencia del model info y la del TXD asociado.
void CBaseModelInfo::AddRef() {
    ++m_nRefCount;
    AddTexDictionaryRef();
}

// Libera en paralelo una referencia lógica y una referencia al diccionario de texturas.
void CBaseModelInfo::RemoveRef() {
    --m_nRefCount;
    RemoveTexDictionaryRef();
}

/**
 * Asocia un CColModel y configura su relación LOD.
 *
 * En modelos con CTimeInfo puede compartir el mismo CColModel con la variante horaria
 * complementaria, dejando a una sola de ellas marcada como propietaria lógica/LOD.
 */
void CBaseModelInfo::SetColModel(CColModel* colModel, bool bIsLodModel) {
    m_pColModel = colModel;
    if (!bIsLodModel) {
        bIsLod = false;
        return;
    }

    bIsLod = true;
    auto timeInfo = GetTimeInfo();
    if (!timeInfo)
        return;

    if (timeInfo->GetOtherTimeModel() == -1)
        return;

    auto lodInfo = CModelInfo::GetModelInfo(timeInfo->GetOtherTimeModel());
    lodInfo->m_pColModel = colModel;
    lodInfo->bIsLod = false;
}

void CBaseModelInfo::Init2dEffects() {
    m_n2dEffectIndex = -1;
    m_n2dfxCount = 0;
}

// Elimina la colisión sólo cuando este model info está marcado como propietario LOD.
void CBaseModelInfo::DeleteCollisionModel() {
    if (m_pColModel && bIsLod)
        delete m_pColModel;

    m_pColModel = nullptr;
}

/**
 * Resuelve un 2DFX combinando el store global de CModelInfo y los efectos embebidos
 * directamente en la geometría RenderWare del modelo.
 */
// 0x4C4C70
C2dEffect* CBaseModelInfo::Get2dEffect(int32 index) const {
    // Como el original: el conteo es un int de 32 bits (puede quedar negativo; la DLL lo truncaba a uint8),
    // GetRwModelType se consulta antes que el RwObject (y otra vez si no es atómico con objeto) y la
    // comparación es con signo (jl).
    int32 storedEffectsCount = m_n2dfxCount;
    RpGeometry* geometry = nullptr;
    if (GetRwModelType() == rpATOMIC && m_pRwObject) {
        geometry = RpAtomicGetGeometry(reinterpret_cast<RpAtomic*>(m_pRwObject));
        storedEffectsCount -= (int32)RpGeometryGet2dFxCount(geometry);
    } else if (GetRwModelType() == rpCLUMP && m_pRwObject) {
        if (auto* const atomic = Get2DEffectAtomic(reinterpret_cast<RpClump*>(m_pRwObject))) {
            geometry = RpAtomicGetGeometry(atomic);
            storedEffectsCount -= (int32)RpGeometryGet2dFxCount(geometry);
        }
    }

    if (index >= storedEffectsCount)
        return RpGeometryGet2dFxAtIndex(geometry, index - storedEffectsCount);
    return &CModelInfo::Get2dEffectStore()->m_aObjects[m_n2dEffectIndex + index];
}

// 0x4C4D20
// Registra la primera posición del rango contiguo en el store global y aumenta su conteo.
void CBaseModelInfo::Add2dEffect(C2dEffect* effect) {
    if (m_n2dEffectIndex >= 0)
        ++m_n2dfxCount;
    else {
        // Como el original: (effect - store - 4) >> 6 con desplazamiento aritmético, truncado a 16 bits.
        const auto store = reinterpret_cast<intptr_t>(CModelInfo::Get2dEffectStore());
        m_n2dEffectIndex = (int16)((reinterpret_cast<intptr_t>(effect) - store - 4) >> 6);
        m_n2dfxCount = 1;
    }
}

// 0x5B3AD0
void SetBaseModelInfoFlags(CBaseModelInfo* modelInfo, uint32 flags) {
    auto flagsStruct = sItemDefinitionFlags(flags);
    modelInfo->bDrawLast          = (flags & 0xC) != 0; // Como el original (0x5B3AD0): test cl,0Ch (draw last o aditivo)
    modelInfo->bAdditiveRender    = flagsStruct.bAdditive;
    modelInfo->bDontWriteZBuffer  = flagsStruct.bNoZBufferWrite;
    modelInfo->bDontCastShadowsOn = flagsStruct.bDontReceiveShadows;
    modelInfo->bIsBackfaceCulled  = !flagsStruct.bDisableBackfaceCulling;
}

void CBaseModelInfo::SetBaseModelInfoFlags(uint32 flags) {
    ::SetBaseModelInfoFlags(this, flags);
}
