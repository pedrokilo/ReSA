/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxSystemBP_c
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxSystemBP.h"
#include "FxPrimBP.h"
#include "FxEmitterBP.h"
#include "FxInfo.h"

#include "FxTools.h"

/**
 * Registra el ciclo de vida y carga de los blueprints de sistemas FX.
 */
void FxSystemBP_c::InjectHooks() {
    RH_ScopedClass(FxSystemBP_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(operator new, 0x4AA100);
    RH_ScopedInstall(Constructor, 0x4AA0D0);
    RH_ScopedInstall(Destructor, 0x4AA0F0);
    RH_ScopedInstall(Load, 0x5C05F0);
    RH_ScopedInstall(Unload, 0x4AA120);
    RH_ScopedInstall(Update, 0x4AA130);
    RH_ScopedInstall(Render, 0x4AA160);
    RH_ScopedInstall(FreePrtFromSystem, 0x4AA1B0);
    RH_ScopedInstall(GetBoundingSphere, 0x4AA1F0);
    RH_ScopedInstall(SetBoundingSphere, 0x4AA200);
}

// 0x4AA100
// Los blueprints se almacenan en la arena FX porque viven durante todo el proyecto cargado.
void* FxSystemBP_c::operator new(unsigned size) {
    return g_fxMan.GetMemPool().GetMem(size);
}

// NOTSA
void FxSystemBP_c::operator delete(void* data) {
    CMemoryMgr::Free(data);
}

// 0x4AA0D0
FxSystemBP_c::FxSystemBP_c() : ListItem_c() {
    m_BoundingSphere = nullptr;
}

FxSystemBP_c* FxSystemBP_c::Constructor() {
    this->FxSystemBP_c::FxSystemBP_c();
    return this;
}

FxSystemBP_c* FxSystemBP_c::Destructor() {
    this->FxSystemBP_c::~FxSystemBP_c();
    return this;
}

// 0x5C05F0
/**
 * Parsea una definición de sistema desde effects.fxp y crea sus primitivos emitter.
 *
 * Primero recoge nombres de textura y configuración; las texturas se cargan al final
 * una vez conocidas todas las primitivas del sistema.
 */
bool FxSystemBP_c::Load(Const char* filename, FILESTREAM file, int32 version) {
    // Como el original: cada campo se lee con ReadLine + sscanf sin comprobar el resultado ni el nombre
    // del campo, y los valores en coma flotante se escriben directamente en los miembros.
    char line[256], field[128], text[128];
    int32 intValue;
    float floatValue;

    ReadLine(file, line, sizeof(line));

    if (version > 100) {
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s %s", field, text)); // FILENAME:
    }

    ReadLine(file, line, sizeof(line));
    RET_IGNORED(sscanf(line, "%s %s", field, text)); // NAME:
    m_nNameKey = CKeyGen::GetUppercaseKey(text);

    ReadLine(file, line, sizeof(line));
    RET_IGNORED(sscanf(line, "%s %f", field, &m_fLength));

    if (version < 106) {
        m_fLoopIntervalMin = 0.0f;
        m_fLoopLength      = 0.0f;
    } else {
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s %f", field, &m_fLoopIntervalMin));
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s %f", field, &m_fLoopLength));
    }

    ReadLine(file, line, sizeof(line));
    RET_IGNORED(sscanf(line, "%s %d", field, &intValue)); // PLAYMODE:
    m_nPlayMode = (uint8)intValue;

    ReadLine(file, line, sizeof(line));
    RET_IGNORED(sscanf(line, "%s %f", field, &floatValue)); // CULLDIST:
    m_nCullDist = (uint16)(int64)(floatValue * 256.0f); // Como el original: _ftol2

    if (version > 103) {
        CVector vecCenter;
        float   fRadius;
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s %f %f %f %f", field, &vecCenter.x, &vecCenter.y, &vecCenter.z, &fRadius));
        m_BoundingSphere = nullptr;
        SetBoundingSphere(vecCenter, fRadius);
    }

    ReadLine(file, line, sizeof(line));
    RET_IGNORED(sscanf(line, "%s %d", field, &intValue)); // NUM_PRIMS:
    m_nNumPrims = (uint8)intValue;
    // Como el original: el contador se trata con signo (movsx) y la reserva se hace en la arena FX
    m_Prims = static_cast<FxPrimBP_c**>(g_fxMan.GetMemPool().GetMem((int32)(int8)m_nNumPrims * (int32)sizeof(FxPrimBP_c*), 4));

    FxName32_t aTextureNames[8][4];

    for (auto i = 0; i < (int8)m_nNumPrims; i++) {
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s", field));
        if (strcmp(field, "FX_PRIM_EMITTER_DATA:") != 0)
            continue;

        // Como el original: el emisor se reserva en la arena FX (0x4A1900) con el tamano del EXE (0x40; el
        // constructor no escribe los campos NOTSA de FxEmitterBP_c a partir de 0x40), no en el heap
        auto* const mem = g_fxMan.GetMemPool().GetMem(0x40, 4);
        auto* const emitter = mem ? new (mem) FxEmitterBP_c() : nullptr;
        m_Prims[i] = emitter;

        // Como el original: los indicadores son los de FxInfoManager_c (+0x3C/+0x3D), no m_bHasInfo*Data (+0x44/+0x45)
        auto& infos = emitter->m_FxInfoManager;
        infos.m_bHasFlatParticleEmitter = false;
        infos.m_bHasHeatHazeParticleEmitter = false;

        ReadLine(file, line, sizeof(line));

        emitter->Load(file, version, aTextureNames[i]);
        infos.m_bHasFlatParticleEmitter = emitter->IsFxInfoPresent(FX_INFO_FLAT_DATA);
        infos.m_bHasHeatHazeParticleEmitter = emitter->IsFxInfoPresent(FX_INFO_HEATHAZE_DATA);
    }

    if (version >= 108) {
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s %d", field, &intValue)); // OMITTEXTURES: (sin uso)
    }

    if (version >= 109) {
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s %s", field, text)); // TXDNAME: (sin uso)
    }

    for (auto i = 0; i < (int8)m_nNumPrims; i++) {
        m_Prims[i]->LoadTextures(&aTextureNames[i][0], version);
    }
    return false; // Como el original: xor al,al
}

// 0x4AA120
void FxSystemBP_c::Unload() {
    // NOP
}

// 0x4AA130
void FxSystemBP_c::Update(float arg0) {
    // Como el original: contador con signo (movsx) releido en cada vuelta
    for (auto i = 0; i < (int8)m_nNumPrims; i++) {
        m_Prims[i]->Update(arg0);
    }
}

// 0x4AA160
void FxSystemBP_c::Render(RwCamera* camera, float dayNightBalance, bool bHeatHaze) {
    for (auto i = 0; i < (int8)m_nNumPrims; i++) {
        m_Prims[i]->Render(camera, 0, dayNightBalance, bHeatHaze);
    }
}

// 0x4AA1B0
// Solicita a cada primitiva liberar la partícula indicada y se detiene al encontrarla.
bool FxSystemBP_c::FreePrtFromSystem(FxSystem_c* system) {
    for (auto i = 0; i < (int8)m_nNumPrims; i++) {
        if (m_Prims[i]->FreePrtFromPrim(system)) {
            return true;
        }
    }
    return false;
}

// 0x4AA1F0
FxSphere_c* FxSystemBP_c::GetBoundingSphere() const {
    return m_BoundingSphere;
}

// 0x4AA200
// Crea el bound una sola vez y únicamente para radios positivos.
void FxSystemBP_c::SetBoundingSphere(const CVector& center, float radius) {
    if (!m_BoundingSphere && radius > 0.0f) {
        m_BoundingSphere = new (false) FxSphere_c();
        m_BoundingSphere->m_vecCenter = center;
        m_BoundingSphere->m_fRadius   = radius;
    }
}
