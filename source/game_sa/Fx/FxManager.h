/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "List_c.h"

#include "FxFrustumInfo.h"
#include "FxMemoryPool.h"
#include "FxSystemBP.h"
#include "FxSystem.h"

class FxEmitterPrt_c;

static constexpr auto FX_MANAGER_NUM_EMITTERS = 1000;

/**
 * Gestor central de blueprints e instancias del sistema FX.
 *
 * Es propietario de la arena de carga, lista de sistemas activos, partículas emitter
 * preasignadas y matrices temporales. Los blueprints viven en m_FxSystemBPs y cada
 * FxSystem_c activo referencia uno de ellos.
 */
class FxManager_c {
public:
    TList_c<FxSystemBP_c>    m_FxSystemBPs; //!< Blueprints cargados desde el proyecto FX.
    TList_c<FxSystem_c>      m_FxSystems;   //!< Instancias activas en el mundo.
    FxEmitterPrt_c*          m_FxEmitters;
    TList_c<Particle_c>      m_FxEmitterParticles; //!< Lista libre de partículas emitter.
    int32                    m_nFxTxdIndex;
    CVector*                 m_pWindDir;
    float*                   m_pfWindSpeed;
    FxFrustumInfo_c          m_Frustum;
    uint32                   m_nCurrentMatrix;
    std::array<RwMatrix*, 8> m_apMatrices; //!< Stack pequeño de matrices temporales reutilizables.
    FxMemoryPool_c           m_Pool; //!< Arena lineal usada por blueprints y datos FX de larga vida.
    bool                     m_bHeatHazeEnabled;

public:
    static void InjectHooks();

    FxManager_c();
    ~FxManager_c() = default; // 0x4A90A0
    FxManager_c* Constructor();
    FxManager_c* Destructor();

    bool Init();
    void Exit();
    void DestroyFxSystem(FxSystem_c* system);
    void DestroyAllFxSystems();

    bool LoadFxProject(const char* path);
    void UnloadFxProject();

    void LoadFxSystemBP(Const char* filename, FILESTREAM file);
    FxSystemBP_c* FindFxSystemBP(const char* name);

    FxFrustumInfo_c* GetFrustumInfo() { return &m_Frustum; } // 0x4A9130
    void CalcFrustumInfo(RwCamera* camera);

    void ReturnParticle(FxEmitterPrt_c* emitter);
    Particle_c* GetParticle(int8 primType);
    void FreeUpParticle();

    void Update(RwCamera* camera, float timeDelta);
    void Render(RwCamera* camera, bool bHeatHaze);

    void SetWindData(CVector* dir, float* speed);

    RwMatrix* FxRwMatrixCreate();
    void FxRwMatrixDestroy(RwMatrix* matrix);

    bool ShouldCreate(FxSystemBP_c* system, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);
    FxSystem_c* CreateFxSystem(Const char* name, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);
    FxSystem_c* CreateFxSystem(Const char* name, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);
    FxSystem_c* CreateFxSystem(FxSystemBP_c* systemBP, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);
    FxSystem_c* CreateFxSystem(FxSystemBP_c* systemBP, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);

    FxMemoryPool_c& GetMemPool() { return m_Pool; }

    template <typename Type>
    Type* Allocate(int32 count) {
        const auto size = sizeof(Type) * count;
        const auto align = std::min(sizeof(Type), sizeof(int32));
        return (Type*)GetMemPool().GetMem(size, align);
    }
};

VALIDATE_SIZE(FxManager_c, 0xBC);

extern FxManager_c& g_fxMan;
