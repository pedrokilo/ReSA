/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Fx_c
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
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

#include "Fx.h"

static auto& TempVertexBuffer = StaticRef<std::array<RxObjSpace3DVertex, 4>>(0xC4D958);

auto& g_fx = StaticRef<Fx_c>(0xA9AE00);

/**
 * Registra las funciones reconstruidas del facade global de efectos.
 *
 * Varias rutinas auxiliares de impacto todavía conservan llamadas directas al EXE
 * y no forman parte de los hooks activos de este bloque.
 */
void Fx_c::InjectHooks() {
    RH_ScopedClass(Fx_c);
    RH_ScopedCategory("Fx");

    // + RH_ScopedInstall(Constructor, 0x49E620);
    // + RH_ScopedInstall(Destructor, 0x49E630);
    // + RH_ScopedInstall(InitStaticSystems, 0x49E660);
    // + RH_ScopedInstall(ExitStaticSystems, 0x49E850);
    // + RH_ScopedInstall(InitEntitySystems, 0x49EA60);
    // + RH_ScopedInstall(ExitEntitySystems, 0x4A12D0);
    // + RH_ScopedInstall(Init, 0x49EA90);
    // + RH_ScopedInstall(Exit, 0x4A1320);
    // + RH_ScopedInstall(Reset, 0x49EAE0);
    // + RH_ScopedInstall(CreateEntityFx, 0x4A11E0);
    // + RH_ScopedInstall(DestroyEntityFx, 0x4A1280);
    // + RH_ScopedInstall(Update, 0x49E640);
    // + RH_ScopedInstall(Render, 0x49E650);
    RH_ScopedInstall(CreateMatFromVec, 0x49E950);
    // + RH_ScopedInstall(SetFxQuality, 0x49EA40);
    // + RH_ScopedInstall(GetFxQuality, 0x49EA50);
    // RH_ScopedInstall(AddBlood, 0x49EB00);
    // RH_ScopedInstall(AddWood, 0x49EE10);
    // RH_ScopedInstall(AddSparks, 0x49F040);
    RH_ScopedInstall(AddTyreBurst, 0x49F300);
    // RH_ScopedInstall(AddBulletImpact, 0x49F3D0);
    // RH_ScopedInstall(AddPunchImpact, 0x49F670);
    // RH_ScopedInstall(AddDebris, 0x49F750);
    // RH_ScopedInstall(AddGlass, 0x49F970);
    // RH_ScopedInstall(AddWheelSpray, 0x49FB30);
    // RH_ScopedInstall(AddWheelGrass, 0x49FF20);
    // RH_ScopedInstall(AddWheelGravel, 0x4A0170);
    // RH_ScopedInstall(AddWheelMud, 0x4A03C0);
    // RH_ScopedInstall(AddWheelSand, 0x4A0610);
    // RH_ScopedInstall(AddWheelDust, 0x4A09C0);
    RH_ScopedInstall(TriggerWaterHydrant, 0x4A0D70);
    // RH_ScopedInstall(TriggerGunshot, 0x4A0DE0);
    RH_ScopedInstall(TriggerTankFire, 0x4A0FA0);
    RH_ScopedInstall(TriggerWaterSplash, 0x4A1070);
    RH_ScopedInstall(TriggerBulletSplash, 0x4A10E0);
    RH_ScopedInstall(TriggerFootSplash, 0x4A1150);

    RH_ScopedGlobalInstall(RenderAddTri_, 0x4A1410);
    RH_ScopedGlobalInstall(RenderEnd, 0x4A1600);
    RH_ScopedGlobalInstall(RenderBegin, 0x4A13B0);
    RH_ScopedGlobalInstall(RotateVecIntoVec, 0x4A1660);
    RH_ScopedGlobalInstall(RotateVecAboutVec, 0x4A1780);
}

Fx_c* Fx_c::Constructor() { this->Fx_c::Fx_c(); return this; }
Fx_c* Fx_c::Destructor() { this->Fx_c::~Fx_c(); return this; }

// 0x49E660
// Crea y conserva los sistemas de partículas estáticos reutilizados por efectos frecuentes.
void Fx_c::InitStaticSystems() {
    m_Blood          = g_fxMan.CreateFxSystem("prt_blood",            CVector{}, nullptr, true);
    m_BoatSplash     = g_fxMan.CreateFxSystem("prt_boatsplash",       CVector{}, nullptr, true);
    m_Bubble         = g_fxMan.CreateFxSystem("prt_bubble",           CVector{}, nullptr, true);
    m_Cardebris      = g_fxMan.CreateFxSystem("prt_cardebris",        CVector{}, nullptr, true);
    m_CollisionSmoke = g_fxMan.CreateFxSystem("prt_collisionsmoke",   CVector{}, nullptr, true);
    m_GunShell       = g_fxMan.CreateFxSystem("prt_gunshell",         CVector{}, nullptr, true);
    m_Sand           = g_fxMan.CreateFxSystem("prt_sand",             CVector{}, nullptr, true);
    m_Sand2          = g_fxMan.CreateFxSystem("prt_sand2",            CVector{}, nullptr, true);
    m_SmokeHuge      = g_fxMan.CreateFxSystem("prt_smoke_huge",       CVector{}, nullptr, true);
    m_SmokeII3expand = g_fxMan.CreateFxSystem("prt_smokeII_3_expand", CVector{}, nullptr, true);
    m_Spark          = g_fxMan.CreateFxSystem("prt_spark",            CVector{}, nullptr, true);
    m_Spark2         = g_fxMan.CreateFxSystem("prt_spark_2",          CVector{}, nullptr, true);
    m_Splash         = g_fxMan.CreateFxSystem("prt_splash",           CVector{}, nullptr, true);
    m_Wake           = g_fxMan.CreateFxSystem("prt_wake",             CVector{}, nullptr, true);
    m_WaterSplash    = g_fxMan.CreateFxSystem("prt_watersplash",      CVector{}, nullptr, true);
    m_WheelDirt      = g_fxMan.CreateFxSystem("prt_wheeldirt",        CVector{}, nullptr, true);
    m_Glass          = g_fxMan.CreateFxSystem("prt_glass",            CVector{}, nullptr, true);
}

// 0x49E850
void Fx_c::ExitStaticSystems() {
    g_fxMan.DestroyFxSystem(m_Blood);
    g_fxMan.DestroyFxSystem(m_BoatSplash);
    g_fxMan.DestroyFxSystem(m_Bubble);
    g_fxMan.DestroyFxSystem(m_Cardebris);
    g_fxMan.DestroyFxSystem(m_CollisionSmoke);
    g_fxMan.DestroyFxSystem(m_GunShell);
    g_fxMan.DestroyFxSystem(m_Sand);
    g_fxMan.DestroyFxSystem(m_Sand2);
    g_fxMan.DestroyFxSystem(m_SmokeHuge);
    g_fxMan.DestroyFxSystem(m_SmokeII3expand);
    g_fxMan.DestroyFxSystem(m_Spark);
    g_fxMan.DestroyFxSystem(m_Spark2);
    g_fxMan.DestroyFxSystem(m_Splash);
    g_fxMan.DestroyFxSystem(m_Wake);
    g_fxMan.DestroyFxSystem(m_WaterSplash);
    g_fxMan.DestroyFxSystem(m_WheelDirt);
    g_fxMan.DestroyFxSystem(m_Glass);
}

// 0x49EA60
void Fx_c::InitEntitySystems() {
    // NOP
}

// NOTSA
// Como el EXE (0x4A0D70, 0x4A0FA0, 0x49F300, 0x4A1070...): (camara - pos), suma x2 + z2 + y2 y se descarta solo si es > rango (NaN no descarta).
static bool IsFxWithinCameraRange(const CVector& pos, float range) {
    const auto& cam = TheCamera.GetPosition();
    const float dx = cam.x - pos.x;
    const float dy = cam.y - pos.y;
    const float dz = cam.z - pos.z;
    return !(dx * dx + dz * dz + dy * dy > range);
}

// NOTSA
// Evita crear sistemas efimeros lejanos; los aceptados se reproducen una vez y se autodestruyen.
static void CreateFxWithinCameraRange(const char* name, const CVector& pos, float range) {
    if (IsFxWithinCameraRange(pos, range)) {
        if (auto* fxSystem = g_fxMan.CreateFxSystem(name, pos, nullptr, false)) {
            fxSystem->PlayAndKill();
        }
    }
}

// 0x4A12D0
// Destruye sistemas ligados a entidades y libera sus nodos de seguimiento.
void Fx_c::ExitEntitySystems() {
    for (auto it = m_FxEntities.GetHead(); it; it = m_FxEntities.GetNext(it)) {
        m_FxEntities.RemoveItem(it);
        g_fxMan.DestroyFxSystem(it->m_System);
        delete it;
    }
}

// 0x49EA90
/**
 * Inicializa FxManager, carga effects.fxp y conecta la dirección/intensidad de viento.
 */
void Fx_c::Init() {
    ZoneScoped;

    g_fxMan.Init();
    g_fxMan.LoadFxProject("models\\effects.fxp");
    g_fxMan.SetWindData(&CWeather::WindDir, &CWeather::Wind);
    InitStaticSystems();
    InitEntitySystems();
    m_Randomizer = 0;
}

// 0x4A1320
void Fx_c::Exit() {
    ExitEntitySystems();
    ExitStaticSystems();
    g_fxMan.Exit();
}

// 0x49EAE0
void Fx_c::Reset() {
    g_fxMan.DestroyAllFxSystems();
    InitStaticSystems();
    InitEntitySystems(); // NOTSA
}

// 0x4A11E0
void Fx_c::CreateEntityFx(CEntity* entity, const char* fxName, const CVector& pos, RwMatrix* transform) {
    // ((void(__thiscall*)(Fx_c*, CEntity*, char*, RwV3d*, RwMatrix*))0x4A11E0)(this, entity, fxName, pos transform);

    auto* particle = g_fxMan.CreateFxSystem(fxName, pos, transform, true);
    if (particle) {
        auto it = new FxEntitySystem();
        it->m_System = particle;
        it->m_Entity = entity;
        m_FxEntities.AddItem(it);
        it->m_System->Play();
    }
}

// 0x4A1280
void Fx_c::DestroyEntityFx(CEntity* entity) {
    // ((void(__thiscall*)(Fx_c*, CEntity*))0x4A1280)(this, entity);

    for (auto it = m_FxEntities.GetHead(); it; it = m_FxEntities.GetNext(it)) {
        if (it->m_Entity == entity) {
            m_FxEntities.RemoveItem(it);
            it->m_System->Kill();
            operator delete(it);
        }
    }
}

// 0x49E640
void Fx_c::Update(RwCamera* camera, float timeDelta) {
    ZoneScoped;

    g_fxMan.Update(camera, timeDelta);
}

// 0x49E650
void Fx_c::Render(RwCamera* camera, bool heatHaze) {
    ZoneScoped;

    g_fxMan.Render(camera, heatHaze);
}

// 0x49E950: matriz identidad con la direccion como eje "arriba" normalizado; mismo orden de escrituras que el original.
// La normalizacion (0x7ED9B0) y la actualizacion de la matriz (0x7F18A0) son librerias de RenderWare.
void Fx_c::CreateMatFromVec(RwMatrix* mat, const CVector* pos, const CVector* dir) {
    const uint32 flags = mat->flags;
    mat->up.y    = 1.0f;
    mat->at.z    = 1.0f;
    mat->right.x = 1.0f;
    mat->pos.z   = 0.0f;
    mat->pos.y   = 0.0f;
    mat->pos.x   = 0.0f;
    mat->up.x    = 0.0f;
    mat->up.z    = 0.0f;
    mat->right.z = 0.0f;
    mat->right.y = 0.0f;
    mat->at.y    = 0.0f;
    mat->at.x    = 0.0f;
    mat->flags   = flags | 0x20003;
    mat->pos.x   = pos->x;
    mat->pos.y   = pos->y;
    mat->pos.z   = pos->z;
    mat->up.x    = dir->x;
    mat->up.y    = dir->y;
    mat->up.z    = dir->z;
    RwV3dNormalize(&mat->up, &mat->up);

    // Como el original: derecha = (z*0 - y*-1, x*-1 - z*0, y*0 - x*0) y adelante = derecha x arriba (aritmetica float del FPU a 24 bits)
    const float ux = mat->up.x, uy = mat->up.y, uz = mat->up.z;
    const float zeroZ = uz * 0.0f;
    const float rx = zeroZ - uy * -1.0f;
    const float ry = ux * -1.0f - zeroZ;
    const float rz = uy * 0.0f - ux * 0.0f;
    mat->right.x = rx;
    mat->right.y = ry;
    mat->right.z = rz;
    mat->at.x = ry * uz - rz * uy;
    mat->at.y = rz * ux - rx * uz;
    mat->at.z = rx * uy - ry * ux;
    RwMatrixUpdate(mat);
}

// 0x49EA40
void Fx_c::SetFxQuality(FxQuality_e quality) {
    m_FxQuality = quality;
}

// 0x49EA50
FxQuality_e Fx_c::GetFxQuality() const {
    return m_FxQuality;
}

// 0x49EB00
// Implementación real aún delegada al EXE original.
void Fx_c::AddBlood(const CVector& pos, const CVector& direction, int32 amount, float lightMult) {
    ((void(__thiscall*)(Fx_c*, const CVector&, const CVector&, int32, float))0x49EB00)(this, pos, direction, amount, lightMult);
}

// 0x49EE10
void Fx_c::AddWood(const CVector& pos, const CVector& direction, int32 amount, float lightMult) {
    ((void(__thiscall*)(Fx_c*, const CVector&, const CVector&, int32, float))0x49EE10)(this, pos, direction, amount, lightMult);
}

// 0x49F040
void Fx_c::AddSparks(const CVector& origin, const CVector& direction, float force, int32 amount, CVector across, eSparkType sparksType, float spread, float life) {
    ((void(__thiscall*)(Fx_c*, const CVector&, const CVector&, float, int32, CVector, uint8, float, float))0x49F040)(this, origin, direction, force, amount, across, sparksType, spread, life);
}

// 0x49F300
void Fx_c::AddTyreBurst(const CVector& posn, const CVector& velocity) {
    // Como el original: usa el Fx_c global (g_fx, 0xA9AE00), no this; el humo se recarga de 0xA9AE24 en cada vuelta.
    if (!IsFxWithinCameraRange(posn, 625.0f)) {
        return;
    }
    const FxPrtMult_c mult(1.0f, 1.0f, 1.0f, 0.4f, 0.12f, 0.0f, 0.1f);
    for (int32 i = 0; i < 4; ++i) {
        g_fx.m_SmokeII3expand->AddParticle(posn, velocity, (float)i * 0.05f, mult, -1.0f, 1.2f, 0.6f, false);
    }
}

// 0x49F3D0
void Fx_c::AddBulletImpact(const CVector& posn, const CVector& direction, int32 bulletFxType, int32 amount, float arg4) {
    ((void(__thiscall*)(Fx_c*, const CVector&, const CVector&, int32, int32, float))0x49F3D0)(this, posn, direction, bulletFxType, amount, arg4);
}

// 0x49F670
void Fx_c::AddPunchImpact(const CVector& pos, const CVector& velocity, int32 num) {
    ((void(__thiscall*)(Fx_c*, const CVector&, const CVector&, int32))0x49F670)(this, pos, velocity, num);
}

// 0x49F750
void Fx_c::AddDebris(const CVector& pos, const RwRGBA& color, float scale, int32 amount) {
    ((void(__thiscall*)(Fx_c*, const CVector&, const RwRGBA&, float, int32))0x49F750)(this, pos, color, scale, amount);
}

// 0x49F970
void Fx_c::AddGlass(const CVector& pos, const RwRGBA& color, float scale, int32 amount) {
    ((void(__thiscall*)(Fx_c*, const CVector&, const RwRGBA&, float, int32))0x49F970)(this, pos, color, scale, amount);
}

// 0x49FB30
void Fx_c::AddWheelSpray(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, bool bInWater, float lightMult) {
    ((void(__thiscall*)(Fx_c*, CVehicle*, CVector, uint8, uint8, float))0x49FB30)(this, vehicle, pos, bWheelsSpinning, bInWater, lightMult);
}

// 0x49FF20
void Fx_c::AddWheelGrass(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    ((void(__thiscall*)(Fx_c*, CVehicle*, CVector, uint8, float))0x49FF20)(this, vehicle, pos, bWheelsSpinning, lightMult);
}

// 0x4A0170
void Fx_c::AddWheelGravel(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    ((void(__thiscall*)(Fx_c*, CVehicle*, CVector, uint8, float))0x4A0170)(this, vehicle, pos, bWheelsSpinning, lightMult);
}

// 0x4A03C0
void Fx_c::AddWheelMud(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    ((void(__thiscall*)(Fx_c*, CVehicle*, CVector, uint8, float))0x4A03C0)(this, vehicle, pos, bWheelsSpinning, lightMult);
}

// 0x4A0610
void Fx_c::AddWheelSand(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    ((void(__thiscall*)(Fx_c*, CVehicle*, CVector, uint8, float))0x4A0610)(this, vehicle, pos, bWheelsSpinning, lightMult);
}

// 0x4A09C0
void Fx_c::AddWheelDust(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    ((void(__thiscall*)(Fx_c*, CVehicle*, CVector, uint8, float))0x4A09C0)(this, vehicle, pos, bWheelsSpinning, lightMult);
}

// 0x4A0D70
void Fx_c::TriggerWaterHydrant(const CVector& pos) {
    CreateFxWithinCameraRange("water_hydrant", pos, 625.0f);
}

// 0x4A0DE0
void Fx_c::TriggerGunshot(CEntity* entity, const CVector& origin, const CVector& target, bool doGunflash) {
    ((void(__thiscall*)(Fx_c*, CEntity*, const CVector&, const CVector&, bool))0x4A0DE0)(this, entity, origin, target, doGunflash);
}

// 0x4A0FA0
void Fx_c::TriggerTankFire(const CVector& pos, const CVector& dir) {
    if (!IsFxWithinCameraRange(pos, 625.0f)) {
        return;
    }
    RwMatrix* const matrix = g_fxMan.FxRwMatrixCreate();
    CreateMatFromVec(matrix, &pos, &dir);
    const CVector origin{ 0.0f, 0.0f, 0.0f };
    if (FxSystem_c* const system = g_fxMan.CreateFxSystem("tank_fire", origin, matrix, false)) {
        system->CopyParentMatrix();
        system->PlayAndKill();
    }
    g_fxMan.FxRwMatrixDestroy(matrix);
}

// 0x4A1070
void Fx_c::TriggerWaterSplash(const CVector& pos) {
    CreateFxWithinCameraRange("water_splash_big", pos, 625.0f);
}

// 0x4A10E0
void Fx_c::TriggerBulletSplash(const CVector& pos) {
    CreateFxWithinCameraRange("water_splash", pos, 625.0f);
}

// 0x4A1150
void Fx_c::TriggerFootSplash(const CVector& pos) {
    CreateFxWithinCameraRange("water_splsh_sml", pos, 625.0f);
}

// see RwIm3DTransformFlags
// 0x4A13B0
void RenderBegin(RwRaster* newRaster, RwMatrix* transform, uint32 transformRenderFlags) {
    g_fx.m_pTransformLTM = transform;
    g_fx.m_nVerticesCount = 0;
    g_fx.m_nVerticesCount2 = 0;
    g_fx.m_pRasterToRender = newRaster;
    g_fx.m_nTransformRenderFlags = transformRenderFlags;
    g_fx.m_pVerts = TempBufferVertices.m_3d;

    // And maybe update raster on RW if the same isnt already set...
    RwRaster* currRaster{};
    RwRenderStateGet(rwRENDERSTATETEXTURERASTER, &currRaster);
    if (currRaster != newRaster)
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(newRaster));
}

// Wrapper for original func
// 0x4A1410
void RenderAddTri_(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float u1, float v1,
    float u2, float v2,
    float u3, float v3,
    int32 r1, int32 g1, int32 b1, int32 a1,
    int32 r2, int32 g2, int32 b2, int32 a2,
    int32 r3, int32 g3, int32 b3, int32 a3
) {
    // Como el original: el color se compone con los enteros completos, sin enmascarar ((((a << 8 | r) << 8 | g) << 8) | b).
    const auto Pack = [](int32 r, int32 g, int32 b, int32 a) -> uint32 {
        return (((((uint32)a << 8) | (uint32)r) << 8 | (uint32)g) << 8) | (uint32)b;
    };

    auto* verts = g_fx.m_pVerts;
    verts[0].objVertex = { x1, y1, z1 };
    verts[0].color     = Pack(r1, g1, b1, a1);
    verts[1].objVertex = { x2, y2, z2 };
    verts[1].color     = Pack(r2, g2, b2, a2);
    verts[2].objVertex = { x3, y3, z3 };
    verts[2].color     = Pack(r3, g3, b3, a3);
    g_fx.m_pVerts += 3;
    g_fx.m_nVerticesCount2 += 3;

    if (g_fx.m_pRasterToRender) {
        verts[0].u = u1;
        verts[0].v = v1;
        verts[1].u = u2;
        verts[1].v = v2;
        verts[2].u = u3;
        verts[2].v = v3;
    }

    g_fx.m_nVerticesCount++;

    // Como el original: comparaciones con signo y vaciado en línea (sin comprobar que haya triángulos).
    if ((int32)g_fx.m_nVerticesCount2 >= TOTAL_TEMP_BUFFER_3DVERTICES - 3 || (int32)g_fx.m_nVerticesCount >= TOTAL_TEMP_BUFFER_INDICES - 1) {
        if (RwIm3DTransform(TempBufferVertices.m_3d, 3 * g_fx.m_nVerticesCount, g_fx.m_pTransformLTM, g_fx.m_nTransformRenderFlags)) {
            RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
            RwIm3DEnd();
        }
        g_fx.m_nVerticesCount = 0;
        g_fx.m_nVerticesCount2 = 0;
        g_fx.m_pVerts = TempBufferVertices.m_3d;
    }
}

// TODO: I honestly think this should be a class method...
// Although originally it wasnt.
// NOTE: Method signature changed to use CVector + RwTexCoords instead of raw values for convenience.
void RenderAddTri(CVector pos1, CVector pos2, CVector pos3, RwTexCoords coord1, RwTexCoords coord2, RwTexCoords coord3, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3) {
    const auto GetVertex = [](unsigned i) {
        return &g_fx.m_pVerts[i];
    };

    const CVector pos[] = { pos1, pos2, pos3 };
    const RwRGBA color[] = {
        { color1.ToRwRGBA() },
        { color2.ToRwRGBA() },
        { color3.ToRwRGBA() },
    };
    for (unsigned i = 0; i < 3; i++) {
        RxObjSpace3DVertexSetPos(GetVertex(i), &pos[i]);
        RxObjSpace3DVertexSetPreLitColor(GetVertex(i), &color[i]);
    }

    if (g_fx.m_pRasterToRender) {
        const RwTexCoords uvs[] = { coord1, coord2, coord3 };
        for (unsigned i = 0; i < 3; i++) {
            RxObjSpace3DVertexSetU(GetVertex(i), uvs[i].u);
            RxObjSpace3DVertexSetV(GetVertex(i), uvs[i].v);
        }
    }

    g_fx.m_pVerts += 3;
    g_fx.m_nVerticesCount2 += 3;
    g_fx.m_nVerticesCount++;

    if (g_fx.m_nVerticesCount2 >= TOTAL_TEMP_BUFFER_3DVERTICES - 3 || g_fx.m_nVerticesCount >= TOTAL_TEMP_BUFFER_INDICES - 1) {
        RenderEnd(); // Render vertices to free up vertex buffer
    }
}

// 0x4A1600
// Envía el batch pendiente a RenderWare y reinicia los contadores/buffer temporal.
void RenderEnd() {
    if (!g_fx.m_nVerticesCount)
        return;

    if (RwIm3DTransform(TempBufferVertices.m_3d, 3 * g_fx.m_nVerticesCount, g_fx.m_pTransformLTM, g_fx.m_nTransformRenderFlags)) {
        RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
        RwIm3DEnd();
    }

    g_fx.m_pVerts = TempBufferVertices.m_3d;
    g_fx.m_nVerticesCount2 = 0;
    g_fx.m_nVerticesCount = 0;
}

// 0x4A1660
void RotateVecIntoVec(RwV3d& vecRes, const RwV3d& vec, const RwV3d& vecAlign) {
    // Como el original: la referencia son las constantes del EXE (0,4243; 0,5657; 0,7071), no (3,4,5) normalizado.
    const float refX = std::bit_cast<float>(0x3ED93DD9u); // 0x85A77C
    const float refY = std::bit_cast<float>(0x3F10D1B7u); // 0x85A780
    const float refZ = std::bit_cast<float>(0x3F350481u); // 0x85A784

    RwV3d right;
    right.x = vecAlign.y * refZ - vecAlign.z * refY;
    right.y = vecAlign.z * refX - vecAlign.x * refZ;
    right.z = vecAlign.x * refY - vecAlign.y * refX;
    RwV3dNormalize(&right, &right);

    RwV3d at;
    at.x = right.z * vecAlign.y - right.y * vecAlign.z;
    at.y = right.x * vecAlign.z - right.z * vecAlign.x;
    at.z = right.y * vecAlign.x - right.x * vecAlign.y;

    auto* m = g_fxMan.FxRwMatrixCreate();
    m->right = right;
    m->up    = vecAlign;
    m->at    = at;
    m->pos   = { 0.0f, 0.0f, 0.0f };

    RwMatrixUpdate(m);
    RwV3dTransformVectors(&vecRes, &vec, 1, m);
    g_fxMan.FxRwMatrixDestroy(m);
}

// 0x4A1780
// Aplica la fórmula de Rodrigues para rotar vec alrededor de axis.
void RotateVecAboutVec(RwV3d& vecRes, const RwV3d& vec, const RwV3d& axis, float angle) {
    // Como el original: coseno y seno de la tabla, productos y sumas en el orden del EXE.
    const float c = CMaths::GetCosFast(angle);
    const float s = CMaths::GetSinFast(angle);
    const float t = 1.0f - c;

    const float txy = t * axis.x * axis.y;
    const float zxt = axis.z * axis.x * t;
    const float tzy = t * axis.z * axis.y;
    const float sx  = s * axis.x;
    const float sy  = s * axis.y;
    const float sz  = s * axis.z;
    const float yyt = axis.y * axis.y * t + c;
    const float zzt = axis.z * axis.z * t + c;
    const float xxt = axis.x * axis.x * t + c;

    vecRes.x = xxt * vec.x + (txy - sz) * vec.y + (sy + zxt) * vec.z;
    vecRes.y = (sz + txy) * vec.x + (tzy - sx) * vec.z + yyt * vec.y;
    vecRes.z = (zxt - sy) * vec.x + (sx + tzy) * vec.y + zzt * vec.z;
}
