/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxManager_c
 * Reconstrucción: COMPLETA — 25/25 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 25/25 entradas.
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

#include "FxManager.h"
#include "FxEmitterPrt.h"
#include "CustomBuildingDNPipeline.h"
#include "FxTools.h"
#include "FxSystem.h"
#include "FxSystemBP.h"
#include "FxPrimBP.h"
#include "Particle.h"

auto& g_fxMan = StaticRef<FxManager_c>(0xA9AE80);

/**
 * Registra el ciclo de vida global, carga de proyectos y creación de FxSystem_c.
 */
void FxManager_c::InjectHooks() {
    RH_ScopedClass(FxManager_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A9470);
    RH_ScopedInstall(Destructor, 0x4A90A0);
    RH_ScopedInstall(Init, 0x4A98E0);
    RH_ScopedInstall(Exit, 0x4A9A10);
    RH_ScopedInstall(DestroyFxSystem, 0x4A9810);
    RH_ScopedInstall(DestroyAllFxSystems, 0x4A98B0);
    RH_ScopedInstall(Update, 0x4A9A80);
    RH_ScopedInstall(LoadFxProject, 0x5C2420);
    RH_ScopedInstall(UnloadFxProject, 0x4A9AE0);
    RH_ScopedInstall(LoadFxSystemBP, 0x5C1F50);
    RH_ScopedInstall(FindFxSystemBP, 0x4A9360);
    RH_ScopedInstall(GetFrustumInfo, 0x4A9130);
    RH_ScopedInstall(CalcFrustumInfo, 0x4A9140);
    RH_ScopedInstall(Render, 0x4A92A0);
    RH_ScopedInstall(ReturnParticle, 0x4A93B0);
    RH_ScopedInstall(GetParticle, 0x4A93C0);
    RH_ScopedInstall(FreeUpParticle, 0x4A9400);
    RH_ScopedInstall(SetWindData, 0x4A93E0);
    RH_ScopedInstall(FxRwMatrixCreate, 0x4A9440);
    RH_ScopedInstall(FxRwMatrixDestroy, 0x4A9460);
    RH_ScopedInstall(ShouldCreate, 0x4A9500);
    RH_ScopedOverloadedInstall(CreateFxSystem, "2", 0x4A9BB0, FxSystem_c* (FxManager_c::*)(const char*, const RwMatrix&, RwMatrix*, bool));
    RH_ScopedOverloadedInstall(CreateFxSystem, "1", 0x4A95C0, FxSystem_c* (FxManager_c::*)(FxSystemBP_c*, const RwMatrix&, RwMatrix*, bool));
    RH_ScopedOverloadedInstall(CreateFxSystem, "4", 0x4A9BE0, FxSystem_c* (FxManager_c::*)(const char*, const CVector&, RwMatrix*, bool));
    RH_ScopedOverloadedInstall(CreateFxSystem, "3", 0x4A96B0, FxSystem_c* (FxManager_c::*)(FxSystemBP_c*, const CVector&, RwMatrix*, bool));
}

// 0x4A9470
FxManager_c::FxManager_c() {
    m_FxEmitters = nullptr;
}

FxManager_c* FxManager_c::Constructor() {
    // Como el original: las tres listas a cero (0x4A8DD0), el frustum sólo pone a 0 m_nNumPlanesPassed de su esfera
    // (0x4A9F70; centro, radio y planos quedan sin escribir), el pool no escribe nada (0x4A9C10) y m_FxEmitters = 0.
    new (&m_FxSystemBPs) TList_c<FxSystemBP_c>;
    new (&m_FxSystems) TList_c<FxSystem_c>;
    new (&m_FxEmitterParticles) TList_c<Particle_c>;
    m_Frustum.m_Sphere.m_nNumPlanesPassed = 0;
    m_FxEmitters = nullptr;
    return this;
}

// Como el original (0x4A992D / 0x4A9B2F): new[] de 1000 FxEmitterPrt_c (0x3C bytes) con la cookie de 4 bytes del
// recuento (el EXE tiene destructor no trivial, 0x4A94F0), reservado con FxEmitterPrt_c::operator new[] (0x4A18B0,
// pool FX global); el constructor de cada elemento (0x4A94E0) sólo anula sus enlaces. Sin memoria devuelve nullptr.
static FxEmitterPrt_c* NewFxEmitterArray() {
    auto* const block = static_cast<uint32*>(FxEmitterPrt_c::operator new[](sizeof(uint32) + FX_MANAGER_NUM_EMITTERS * sizeof(FxEmitterPrt_c)));
    if (!block) {
        return nullptr;
    }
    *block = FX_MANAGER_NUM_EMITTERS;
    auto* const emitters = reinterpret_cast<FxEmitterPrt_c*>(block + 1);
    for (auto i = 0; i < FX_MANAGER_NUM_EMITTERS; i++) {
        emitters[i].m_pPrev = nullptr;
        emitters[i].m_pNext = nullptr;
    }
    return emitters;
}

FxManager_c* FxManager_c::Destructor() {
    this->FxManager_c::~FxManager_c();
    return this;
}

// 0x4A98E0
/**
 * Inicializa la arena, el pequeño pool temporal de matrices y las 1000 partículas
 * emitter preasignadas que se mantienen en la lista libre.
 */
bool FxManager_c::Init() {
    m_Pool.Init();

    m_nCurrentMatrix = 0;
    for (auto& matrix : m_apMatrices) {
        matrix = RwMatrixCreate();
    }

    m_FxEmitters = NewFxEmitterArray();
    for (auto i = 0; i < FX_MANAGER_NUM_EMITTERS; i++) {
        m_FxEmitterParticles.AddItem(&m_FxEmitters[i]);
    }

    return true;
}

// 0x4A9A10
// Destruye sistemas activos, libera TXD/matrices y finalmente cierra la arena FX.
void FxManager_c::Exit() {
    DestroyAllFxSystems();
    m_FxSystemBPs.RemoveAll();
    m_FxEmitters = nullptr;
    m_FxEmitterParticles.RemoveAll();
    CTxdStore::RemoveTxdSlot(m_nFxTxdIndex);
    std::ranges::for_each(m_apMatrices, RwMatrixDestroy);
    m_Pool.Exit();
}

// 0x4A9810
/**
 * Devuelve al pool todas las partículas pertenecientes al sistema antes de retirarlo
 * de la lista activa y destruir su instancia.
 */
void FxManager_c::DestroyFxSystem(FxSystem_c* system) {
    assert(system);
    assert(system->m_SystemBP);

    // Como el original: m_nNumPrims se compara con signo (test cl, cl; jle / movsx) y se relee en cada vuelta.
    for (auto i = 0; i < static_cast<int8>(system->m_SystemBP->m_nNumPrims); i++) {
        auto& prim      = system->m_SystemBP->m_Prims[i];
        auto& particles = prim->m_Particles;
        for (Particle_c* it = particles.GetHead(); it;) {
            auto* const prt = it;
            it = particles.GetNext(it); // Iterator will be invalidated, so get next here immediately

            if (prt->m_System == system) {
                system->m_SystemBP->m_Prims[i]->m_Particles.RemoveItem(prt);
                // Como el original: la lista libre es la del gestor global (0xA9AE9C), no la de this.
                g_fxMan.m_FxEmitterParticles.AddItem(prt);
            }
        }
    }

    m_FxSystems.RemoveItem(system);
    system->Exit();
    delete system;
}

// 0x4A98B0
void FxManager_c::DestroyAllFxSystems() {
    for (FxSystem_c* it = m_FxSystems.GetHead(); it;) {
        auto* const system = it;
        it = m_FxSystems.GetNext(system); // Destruction invalidates the current node.
        DestroyFxSystem(system);
    }
}

// 0x5C2420
/**
 * Carga el TXD complementario y parsea todos los FxSystemBP_c de un proyecto .fxp.
 * Al terminar compacta la arena al tamaño realmente utilizado por los blueprints.
 */
bool FxManager_c::LoadFxProject(const char* path) {
    char txdPath[256]; // to be set to "models\\effectsPC.txd"
    strcpy(txdPath, path);
    strcpy(&txdPath[strlen(path) - strlen(".fxp")], "PC.txd");

    // NOTSA: sanity check
    if (!fs::exists(txdPath)) {
        NOTSA_LOG_ERR("Fx TXD file '{}' doesn't exists. Game will crash after trying to render a fx!", txdPath);
    }

    m_nFxTxdIndex = CTxdStore::AddTxdSlot("fx");
    CTxdStore::LoadTxd(m_nFxTxdIndex, txdPath);
    CTxdStore::AddRef(m_nFxTxdIndex);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(m_nFxTxdIndex);

    auto* file = CFileMgr::OpenFile(path, "r");
    // Como el original: el manejador se comprueba con signo (test ebx, ebx; jg).
    if (reinterpret_cast<int32>(file) <= 0)
        return false;

    // Como el original: un único búfer de campo para la cabecera y las comparaciones (una línea sin palabras
    // conserva el campo anterior) y la línea intermedia de cada bloque se lee sin analizarla.
    char line[256], buffer[128];

    ReadLine(file, line, sizeof(line)); // FX_PROJECT_DATA:
    RET_IGNORED(sscanf(line, "%s", buffer));
    ReadLine(file, line, sizeof(line));
    ReadLine(file, line, sizeof(line));
    RET_IGNORED(sscanf(line, "%s", buffer));
    while (!strncmp(buffer, "FX_SYSTEM_DATA:", 16u)) {
        LoadFxSystemBP(path, file);
        ReadLine(file, line, sizeof(line));
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s", buffer));
    }

    CFileMgr::CloseFile(file);
    CTxdStore::PopCurrentTxd();
    GetMemPool().Optimise();
    return true;
}

// 0x4A9AE0
void FxManager_c::UnloadFxProject() {
    // ((void(__thiscall*)(FxManager_c*))0x4A9AE0)(this);

    DestroyAllFxSystems();
    m_FxSystemBPs.RemoveAll();
    GetMemPool().Reset();
    m_FxEmitterParticles.RemoveAll();

    m_FxEmitters = NewFxEmitterArray();
    for (auto i = 0; i < FX_MANAGER_NUM_EMITTERS; i++) {
        m_FxEmitterParticles.AddItem(&m_FxEmitters[i]);
    }
}

// 0x5C1F50
FxSystemBP_c* FxManager_c::LoadFxSystemBP(Const char* filename, FILESTREAM file) {
    // return ((FxSystemBP_c * (__thiscall*)(FxManager_c*, char*, FILESTREAM))0x5C1F50)(this, filename, file);

    int32 version;
    char line[256];
    ReadLine(file, line, sizeof(line));
    (void)sscanf(line, "%d", &version);

    auto system = new FxSystemBP_c();
    system->Load(filename, file, version);
    m_FxSystemBPs.AddItem(system);
    return system; // Como el original: devuelve el blueprint en eax
}

// 0x4A9360
FxSystemBP_c* FxManager_c::FindFxSystemBP(const char* name) {
    // return ((FxSystemBP_c * (__thiscall*)(FxManager_c*, const char*))0x4A9360)(this, name);

    const auto key = CKeyGen::GetUppercaseKey(name);
    for (FxSystemBP_c* it = m_FxSystemBPs.GetHead(); it; it = m_FxSystemBPs.GetNext(it)) {
        if (it->m_nNameKey == key) {
            return it;
        }
    }

    NOTSA_LOG_ERR("Couldn't find Fx system bp '{}'", name);
    return nullptr;
}

// Constantes del EXE: 0x85A998 (57.295826f, no es el float más cercano a 180/pi), 0x85A7BC (0.017453279f),
// 0x858624 (1.0f) y 0x85A994 (180.0f).
static uint32 s_FxFrustumRadToDeg = 0x42652EEDu;
static uint32 s_FxFrustumDegToRad = 0x3C8EFA2Eu;
static float  s_FxFrustumOne      = 1.0f;
static float  s_FxFrustum180      = 180.0f;

// Como el original (0x4A9165..0x4A91B5): ángulo = atan2(dist, 1) con fpatan * 57.295826f guardado como float;
// radio = sqrt(dist² + 1) * farClip / sin((180 - 2·ángulo) * k) * sin(ángulo * k), con fsin del x87 a precisión
// interna completa (la división y el producto los redondea la palabra de control del juego).
static __declspec(naked) float __cdecl FxFrustumRadiusX87(float dist, float farClip) {
    __asm {
        fld     dword ptr [esp + 4]
        fld     st(0)
        fld1
        fpatan
        fmul    dword ptr [s_FxFrustumRadToDeg]
        fstp    dword ptr [esp + 4]
        fld     st(0)
        fmulp   st(1), st
        fadd    dword ptr [s_FxFrustumOne]
        fsqrt
        fmul    dword ptr [esp + 8]
        fld     dword ptr [esp + 4]
        fadd    st(0), st
        fsubr   dword ptr [s_FxFrustum180]
        fmul    dword ptr [s_FxFrustumDegToRad]
        fsin
        fdivp   st(1), st
        fld     dword ptr [esp + 4]
        fmul    dword ptr [s_FxFrustumDegToRad]
        fsin
        fmulp   st(1), st
        ret
    }
}

// 0x4A9140
// Deriva una esfera envolvente y cuatro planos laterales desde la cámara actual.
void FxManager_c::CalcFrustumInfo(RwCamera* camera) {
    const auto* matrix     = RwFrameGetMatrix(RwCameraGetFrame(camera));
    const auto* viewWindow = RwCameraGetViewWindow(camera);

    // Como el original: longitud de la ventana con fsqrt en línea (x·x + y·y), no RwV2dLength.
    const float dist   = std::sqrt(viewWindow->x * viewWindow->x + viewWindow->y * viewWindow->y);
    const float radius = FxFrustumRadiusX87(dist, RwCameraGetFarClipPlane(camera));

    // Como el original: sólo escribe centro y radio; m_nNumPlanesPassed de la esfera no se toca.
    m_Frustum.m_Sphere.m_vecCenter = CVector{matrix->pos} + CVector{matrix->at} * radius;
    m_Frustum.m_Sphere.m_fRadius   = radius;

    m_Frustum.m_Planes[0] = camera->frustumPlanes[2].plane;
    m_Frustum.m_Planes[1] = camera->frustumPlanes[3].plane;
    m_Frustum.m_Planes[2] = camera->frustumPlanes[4].plane;
    m_Frustum.m_Planes[3] = camera->frustumPlanes[5].plane;
}

// 0x4A9A80
/**
 * Actualiza blueprints y sistemas activos; si un FxSystem_c informa fin de vida,
 * se destruye durante el mismo recorrido sin reutilizar el iterador invalidado.
 */
void FxManager_c::Update(RwCamera* camera, float timeDelta) {
    // ((void(__thiscall*)(FxManager_c*, RwCamera*, float))0x4A9A80)(this, camera, timeDelta);

    assert(camera);
    CalcFrustumInfo(camera);

    for (FxSystemBP_c* it = m_FxSystemBPs.GetHead(); it; it = m_FxSystemBPs.GetNext(it)) {
        it->Update(timeDelta);
    }

    for (FxSystem_c* it = m_FxSystems.GetHead(); it;) {
        auto* const system = it;
        it = m_FxSystems.GetNext(system); // Update may request destruction of this node.
        if (system->Update(camera, timeDelta)) {
            DestroyFxSystem(system);
        }
    }
}

// 0x4A92A0
// Configura estados RenderWare comunes y renderiza todos los blueprints activos.
void FxManager_c::Render(RwCamera* camera, bool bHeatHaze) {
    // ((void(__thiscall*)(FxManager_c*, RwCamera*, uint8))0x4A92A0)(this, camera, bHeatHaze);

    auto balance = 1.0f - CCustomBuildingDNPipeline::m_fDNBalanceParam;
    m_bHeatHazeEnabled = false;
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
    for (FxSystemBP_c* it = m_FxSystemBPs.GetHead(); it; it = m_FxSystemBPs.GetNext(it)) {
        it->Render(camera, balance * 0.6f + 0.4f, bHeatHaze);
    }
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
}

// 0x4A93B0
void FxManager_c::ReturnParticle(FxEmitterPrt_c* emitter) {
    m_FxEmitterParticles.AddItem(emitter);
}

// 0x4A93C0 -- has no xref with primType != 0.
Particle_c* FxManager_c::GetParticle(int8 primType) {
    // Como el original: con primType != 0 devuelve nullptr sin comprobaciones.
    return !primType ? m_FxEmitterParticles.RemoveHead() : nullptr;
}

// 0x4A9400
// Recupera una partícula forzando a un sistema no crítico a liberar una de las suyas.
void FxManager_c::FreeUpParticle() {
    // ((void(__thiscall*)(FxManager_c*))0x4A9400)(this);

    FxSystem_c* system;
    do {
        do {
            auto numItems = m_FxSystems.GetNumItems();
            system = m_FxSystems.GetItemOffset(true, CGeneral::GetRandomNumber() % numItems);
        } while (system->m_MustCreateParticles);
    } while (!system->m_SystemBP->FreePrtFromSystem(system));
}

// 0x4A93E0
void FxManager_c::SetWindData(CVector* dir, float* speed) {
    m_pWindDir    = dir;
    m_pfWindSpeed = speed;
}

// 0x4A9440
RwMatrix* FxManager_c::FxRwMatrixCreate() {
    return m_apMatrices.data()[m_nCurrentMatrix++]; // Como el original: sin comprobar el índice
}

// 0x4A9460
void FxManager_c::FxRwMatrixDestroy(RwMatrix* matrix) {
    m_nCurrentMatrix--;
}

// 0x4A9BB0
FxSystem_c* FxManager_c::CreateFxSystem(const char* name, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    // Como el original: sin comprobar el blueprint; CreateFxSystem (0x4A95C0) ya devuelve nullptr sin él.
    FxSystemBP_c* systemBP = FindFxSystemBP(name);
    return CreateFxSystem(systemBP, transform, objectMatrix, ignoreBoundingChecks);
}

// 0x4A9BE0
FxSystem_c* FxManager_c::CreateFxSystem(Const char* name, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    // Como el original: sin comprobar el blueprint; CreateFxSystem (0x4A96B0) ya devuelve nullptr sin él.
    FxSystemBP_c* systemBP = FindFxSystemBP(name);
    return CreateFxSystem(systemBP, point, objectMatrix, ignoreBoundingChecks);
}

// 0x4A96B0
FxSystem_c* FxManager_c::CreateFxSystem(FxSystemBP_c* systemBP, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    if (systemBP) {
        // Como el original: FxRwMatrixCreate/Destroy en línea sobre la pila de matrices del gestor global
        // (0xA9AF08/0xA9AF0C), no la de this.
        auto* pointMatrix = g_fxMan.FxRwMatrixCreate();
        RwMatrixSetIdentity(pointMatrix);
        RwV3dAssign(RwMatrixGetPos(pointMatrix), &point);
        RwMatrixUpdate(pointMatrix);

        FxSystem_c* system = CreateFxSystem(systemBP, *pointMatrix, objectMatrix, ignoreBoundingChecks);

        g_fxMan.FxRwMatrixDestroy(pointMatrix);
        return system;
    }
    return nullptr;
}

// 0x4A9500
// Transforma el bound del blueprint y lo enfrenta al frustum salvo que se omita el culling.
bool FxManager_c::ShouldCreate(FxSystemBP_c* system, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    if (ignoreBoundingChecks)
        return true;

    if (!system->m_BoundingSphere)
        return true;

    // Como el original: FxRwMatrixCreate/Destroy en línea sobre la pila de matrices del gestor global
    // (0xA9AF08/0xA9AF0C), no la de this; la copia sin objectMatrix es un rep movsd de 64 bytes.
    auto* curr = g_fxMan.FxRwMatrixCreate();
    if (objectMatrix)
        RwMatrixMultiply(curr, &transform, objectMatrix);
    else
        std::memcpy(curr, &transform, sizeof(RwMatrix));

    FxSphere_c pointsOut;
    RwV3dTransformPoints(&pointsOut.m_vecCenter, reinterpret_cast<const RwV3d*>(system->m_BoundingSphere), 1, curr);
    pointsOut.m_fRadius = system->m_BoundingSphere->m_fRadius;
    g_fxMan.FxRwMatrixDestroy(curr);
    return GetFrustumInfo()->IsCollision(pointsOut) != 0;
}

// 0x4A95C0
FxSystem_c* FxManager_c::CreateFxSystem(FxSystemBP_c* systemBP, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    if (systemBP && ShouldCreate(systemBP, transform, objectMatrix, ignoreBoundingChecks)) {
        auto* fx = new FxSystem_c();
        fx->Init(systemBP, transform, objectMatrix);
        m_FxSystems.AddItem(fx);

        // Como el original: cadena de comparaciones con signo; cualquier calidad >= HIGH usa 1.0 y una negativa
        // no cambia el multiplicador.
        if (g_fx.GetFxQuality() == FX_QUALITY_LOW) {
            fx->SetRateMult(0.5f);
        } else if (g_fx.GetFxQuality() == FX_QUALITY_MEDIUM) {
            fx->SetRateMult(0.75f);
        } else if (g_fx.GetFxQuality() >= FX_QUALITY_HIGH) {
            fx->SetRateMult(1.0f);
        }

        return fx;
    }
    return nullptr;
}
