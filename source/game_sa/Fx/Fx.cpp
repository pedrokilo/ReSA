/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Fx_c
 * Reconstrucción: COMPLETA — 22/22 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 22/22 entradas.
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
#include "X87Math.h"

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
    RH_ScopedInstall(AddWood, 0x49EE10);
    // RH_ScopedInstall(AddSparks, 0x49F040);
    RH_ScopedInstall(AddTyreBurst, 0x49F300);
    // RH_ScopedInstall(AddBulletImpact, 0x49F3D0);
    RH_ScopedInstall(AddPunchImpact, 0x49F670);
    RH_ScopedInstall(AddDebris, 0x49F750);
    RH_ScopedInstall(AddGlass, 0x49F970);
    // RH_ScopedInstall(AddWheelSpray, 0x49FB30);
    RH_ScopedInstall(AddWheelGrass, 0x49FF20);
    RH_ScopedInstall(AddWheelGravel, 0x4A0170);
    RH_ScopedInstall(AddWheelMud, 0x4A03C0);
    RH_ScopedInstall(AddWheelSand, 0x4A0610);
    RH_ScopedInstall(AddWheelDust, 0x4A09C0);
    RH_ScopedInstall(TriggerWaterHydrant, 0x4A0D70);
    RH_ScopedInstall(TriggerGunshot, 0x4A0DE0);
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

// NOTSA
// Desplazamientos del EXE que leen AddWheelSand/Dust/Grass/Gravel/Mud (0x4A0610.. y 0x49FF20..): modelo (+0x22), velocidad (+0x44), conductor (+0x460), pedal (+0x49C) y subtipo (+0x594).
static_assert(offsetof(CVehicle, m_nModelIndex) == 0x22 && offsetof(CVehicle, m_vecMoveSpeed) == 0x44 && offsetof(CVehicle, m_pDriver) == 0x460 &&
              offsetof(CVehicle, m_GasPedal) == 0x49C && offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(Fx_c, m_FxQuality) == 0x54,
              "AddWheel* lee estos desplazamientos del EXE");
static_assert(offsetof(Fx_c, m_Cardebris) == 0xC && offsetof(Fx_c, m_Sand) == 0x18 && offsetof(Fx_c, m_WheelDirt) == 0x3C && offsetof(Fx_c, m_Glass) == 0x40,
              "sistemas globales que usan AddDebris/AddGlass/AddWheel*");

// NOTSA
// rand del CRT del EXE (0x821B1E: avanza la semilla del juego).
static int32 FxExeRand() {
    return reinterpret_cast<int32(__cdecl*)()>(0x821B1E)();
}

// NOTSA
// Como el EXE (0x4A0640..0x4A0659 y 0x49FF77..0x49FF99): (camara - pos) y suma (dz*dz + dy*dy) + dx*dx; el EXE sale si es mayor que 625 o NaN.
static float FxCameraDistanceSqZYX(const CVector& pos) {
    const auto& cam = TheCamera.GetPosition();
    const float dx = cam.x - pos.x;
    const float dy = cam.y - pos.y;
    const float dz = cam.z - pos.z;
    return (dz * dz + dy * dy) + dx * dx;
}

// NOTSA
// Cuerpo de AddWheelSand (0x4A0610) y AddWheelDust (0x4A09C0), que solo difieren en el color base, en la vida de la particula y en las constantes de la formula.
static void FxAddWheelSandOrDust(Fx_c* fx, CVehicle* vehicle, const CVector& pos, bool spinning, float lightMult, bool dust) {
    // Como el original: primero el vehiculo del jugador (0x56E0D0, push 0; push -1) y despues la distancia a la camara.
    CVehicle* const playerVehicle = FindPlayerVehicle(-1, false);
    const float distSq = FxCameraDistanceSqZYX(pos); // fstp dword: se compara el float guardado
    if (!(distSq <= 625.0f)) { // jp tras fcomp: mayor que 625 o NaN
        return;
    }
    // Como el original: modelo con signo (movsx word) + contador de fotogramas; la calidad (this+0x54) decide la cadencia.
    const uint32 key = static_cast<uint32>(static_cast<int32>(static_cast<int16>(vehicle->m_nModelIndex))) + CTimer::m_FrameCounter;
    const int32 quality = static_cast<int32>(fx->m_FxQuality);
    if (quality >= 1) {
        if (key & 1) {
            return;
        }
        if (distSq > 64.0f || !playerVehicle) {
            if (key & 3) {
                return;
            }
        }
    } else if (quality == 0) {
        if (key & 3) {
            return;
        }
        if (distSq > 64.0f || !playerVehicle) {
            if (key & 7) {
                return;
            }
        }
    }
    // Como el original: multiplicador (0.81, 0.67, 0.57, 0.5, 1.0, 0, 0) de la arena o (0.51, 0.44, 0.31, ...) del polvo; tamano y vida se sustituyen por los calculados.
    FxPrtMult_c mult = dust
        ? FxPrtMult_c(std::bit_cast<float>(0x3F028F5Cu), std::bit_cast<float>(0x3EE147AEu), std::bit_cast<float>(0x3E9EB852u), 0.5f, 1.0f, 0.0f, 0.0f)
        : FxPrtMult_c(std::bit_cast<float>(0x3F4F5C29u), std::bit_cast<float>(0x3F2B851Fu), std::bit_cast<float>(0x3F11EB85u), 0.5f, 1.0f, 0.0f, 0.0f);
    Fx77DustParams params;
    Fx77DustPrepareCpp(&vehicle->m_vecMoveSpeed.x, &vehicle->m_GasPedal, &CTimer::ms_fTimeStep, spinning ? 1 : 0, dust ? 1 : 0, static_cast<int>(vehicle->m_nVehicleSubType), &params);
    mult.m_fSize = params.size;
    mult.m_fLife = params.life;
    CVector vel, position;
    for (int32 i = 0; i < params.count; ++i) {
        // Como el original: tres sorteos por particula, en orden (velocidad x, y, z); el sistema se relee de g_fx en cada vuelta.
        const int32 r1 = FxExeRand();
        const int32 r2 = FxExeRand();
        const int32 r3 = FxExeRand();
        Fx77DustStepCpp(&params, &vehicle->m_vecMoveSpeed.x, &pos.x, i, r1, r2, r3, &vel.x, &position.x);
        g_fx.m_Sand->AddParticle(position, vel, 0.0f, mult, -1.0f, lightMult, std::bit_cast<float>(0x3F333333u), false);
    }
}

// NOTSA
// Cuerpo de AddWheelGrass (0x49FF20), AddWheelGravel (0x4A0170) y AddWheelMud (0x4A03C0): solo cambia el color base del multiplicador. `this` y el byte de giro no se usan.
static void FxAddWheelDirt(CVehicle* vehicle, const CVector& pos, float lightMult, FxPrtMult_c mult) {
    // Como el original: solo el jugador (conductor = FindPlayerPed(0) o, si no, FindPlayerPed(1)); la segunda consulta solo si la primera no coincide.
    if (reinterpret_cast<uintptr_t>(vehicle->m_pDriver) != reinterpret_cast<uintptr_t>(FindPlayerPed(0))) {
        if (reinterpret_cast<uintptr_t>(vehicle->m_pDriver) != reinterpret_cast<uintptr_t>(FindPlayerPed(1))) {
            return;
        }
    }
    CVehicle* const playerVehicle = FindPlayerVehicle(-1, false);
    const float distSq = FxCameraDistanceSqZYX(pos); // en registro: no se redondea a float; el resultado de las comparaciones es el mismo
    if (!(distSq <= 625.0f)) { // jp: mayor que 625 o NaN
        return;
    }
    // Como el original: byte bajo del modelo (movzx) + contador de fotogramas; lejos (> 400) 1 de cada 4, a media distancia (> 64) o sin vehiculo del jugador 1 de cada 2.
    const uint32 key = static_cast<uint32>(static_cast<uint8>(vehicle->m_nModelIndex)) + CTimer::m_FrameCounter;
    if (distSq > 400.0f) {
        if (key & 3) {
            return;
        }
    } else if (distSq > 64.0f || !playerVehicle) {
        if (key & 1) {
            return;
        }
    }
    CVector vel, position;
    for (int32 n = 0; n < 3; ++n) {
        // Como el original: seis sorteos por particula, en orden (tamano, velocidad x, y, z, posicion x, y); el sistema de tierra se relee de g_fx en cada vuelta.
        int32 r[6];
        for (int32& value : r) {
            value = FxExeRand();
        }
        Fx77DirtStepCpp(&vehicle->m_vecMoveSpeed.x, &pos.x, r, &mult.m_fSize, &vel.x, &position.x);
        g_fx.m_WheelDirt->AddParticle(position, vel, 0.0f, mult, -1.0f, lightMult, std::bit_cast<float>(0x3F19999Au), false);
    }
}

// NOTSA
// Color RwRGBA * (1/255) (0x859A3C), tamano, rot (sorteo) y vida de AddDebris/AddGlass (0x49F750.., 0x49F970..): rot = (resto * 1e-4f + 1.0f) * 0.5f y vida 0.2f.
static FxPrtMult_c FxDebrisMult(const RwRGBA& color, float scale) {
    const float inv255 = std::bit_cast<float>(0x3B808081u);
    FxPrtMult_c mult; // 0x4AB270
    mult.m_Color.red   = float(color.red) * inv255;
    mult.m_Color.green = float(color.green) * inv255;
    mult.m_Color.blue  = float(color.blue) * inv255;
    mult.m_Color.alpha = float(color.alpha) * inv255;
    mult.m_fSize = scale;
    const int32 remainder = FxExeRand() % 10000;
    mult.m_fLife = std::bit_cast<float>(0x3E4CCCCDu);
    mult.m_Rot = (float(remainder) * std::bit_cast<float>(0x38D1B717u) + 1.0f) * 0.5f;
    return mult;
}

// NOTSA
// Velocidad de una particula de AddDebris/AddGlass: tres sorteos (x, y, z): x e y = ((r * 3.05e-5f) * 0.5f) * 20.0f - 5.0f; z = ((r * 3.05e-5f) * 0.15f) * 20.0f + 2.0f.
static CVector FxDebrisVelocity() {
    const float scale = std::bit_cast<float>(0x38000100u);
    CVector vel;
    const float rx = float(FxExeRand());
    vel.x = ((rx * scale) * 0.5f) * 20.0f - 5.0f;
    const float ry = float(FxExeRand());
    vel.y = ((ry * scale) * 0.5f) * 20.0f - 5.0f;
    const float rz = float(FxExeRand());
    vel.z = ((rz * scale) * std::bit_cast<float>(0x3E19999Au)) * 20.0f + 2.0f;
    return vel;
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
    // Como el original (0x49EE10..0x49F032, `ret 10h`): `this` no se usa; las particulas van al sistema global g_fx.m_Blood ([0xA9AE00], recargado en cada vuelta).
    // Fuera del rango de 625 (camara - pos) no hace nada; el multiplicador base es (0.5, 0.25, 0, 1, 0.3, 0, 1) (0x4AB290) y en cada vuelta se sortean rojo, verde, azul y tamano.
    if (!IsFxWithinCameraRange(pos, 625.0f)) {
        return;
    }
    FxPrtMult_c mult(0.5f, 0.25f, 0.0f, 1.0f, std::bit_cast<float>(0x3E99999Au), 0.0f, 1.0f);
    if (amount <= 0) {
        return;
    }
    // rand del CRT del EXE (0x821B1E: avanza la semilla del juego); el resto es con signo (cdq; idiv 10000).
    const auto exeRand = reinterpret_cast<int32(__cdecl*)()>(0x821B1E);
    CVector vel;
    for (int32 i = amount; i > 0; --i) {
        mult.m_Color.red   = Wood73ScaledCpp(exeRand() % 10000, std::bit_cast<float>(0x3DF5C290u), std::bit_cast<float>(0x3E051EB8u));
        mult.m_Color.green = Wood73ScaledCpp(exeRand() % 10000, std::bit_cast<float>(0x3CF5C294u), std::bit_cast<float>(0x3DF5C28Fu));
        mult.m_Color.blue  = Wood73ScaledCpp(exeRand() % 10000, std::bit_cast<float>(0x3CF5C290u), std::bit_cast<float>(0x3D23D70Au));
        mult.m_fSize       = Wood73ScaledCpp(exeRand() % 10000, std::bit_cast<float>(0x3E99999Au), std::bit_cast<float>(0x3F333333u));
        Wood73BaseVelCpp(&direction.x, &vel.x);
        Wood73JitterCpp(exeRand() % 10000, &vel.x);
        Wood73JitterCpp(exeRand() % 10000, &vel.y);
        Wood73JitterCpp(exeRand() % 10000, &vel.z);
        g_fx.m_Blood->AddParticle(pos, vel, 0.0f, mult, -1.0f, lightMult, 0.6f, false);
    }
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
    // Como el original: ni `this` ni `num` se usan; dos particulas de humo en el sistema global (g_fx.m_SmokeII3expand, 0xA9AE24, recargado en cada llamada), la segunda con tiempo 0.05f.
    if (!IsFxWithinCameraRange(pos, 625.0f)) {
        return;
    }
    const FxPrtMult_c mult(1.0f, 1.0f, 1.0f, 0.4f, 0.1f, 0.0f, 0.1f); // 0x4AB290
    g_fx.m_SmokeII3expand->AddParticle(pos, velocity, 0.0f, mult, -1.0f, 1.2f, 0.6f, false);
    g_fx.m_SmokeII3expand->AddParticle(pos, velocity, 0.05f, mult, -1.0f, 1.2f, 0.6f, false);
}

// 0x49F750
void Fx_c::AddDebris(const CVector& pos, const RwRGBA& color, float scale, int32 amount) {
    // Como el original (0x49F750..0x49F95D, `ret 10h`): `this` no se usa. Fuera del rango de 625 (camara - pos, NaN no sale) no hace nada; el resto se sortea siempre (aunque amount <= 0).
    if (!IsFxWithinCameraRange(pos, 625.0f)) {
        return;
    }
    const FxPrtMult_c mult = FxDebrisMult(color, scale);
    if (amount <= 0) {
        return;
    }
    // Como el original: el contador del EXE (0xA9ADE4) elige que primitiva del sistema de escombros se activa; el sistema se relee de g_fx en cada llamada.
    auto& primCounter = StaticRef<uint32>(0xA9ADE4);
    for (int32 i = amount; i > 0; --i) {
        const CVector vel = FxDebrisVelocity();
        g_fx.m_Cardebris->EnablePrim(0, false);
        g_fx.m_Cardebris->EnablePrim(1, false);
        g_fx.m_Cardebris->EnablePrim(2, false);
        g_fx.m_Cardebris->EnablePrim(3, false);
        g_fx.m_Cardebris->EnablePrim(static_cast<int32>(primCounter), true);
        g_fx.m_Cardebris->AddParticle(pos, vel, 0.0f, mult, -1.0f, std::bit_cast<float>(0x3F99999Au), std::bit_cast<float>(0x3F19999Au), false);
        primCounter = (primCounter + 1) & 3;
    }
}

// 0x49F970
void Fx_c::AddGlass(const CVector& pos, const RwRGBA& color, float scale, int32 amount) {
    // Como el original (0x49F970..0x49FB1D, `ret 10h`): igual que AddDebris pero sobre el sistema de cristal (g_fx.m_Glass, releido en cada vuelta) y sin activar primitivas.
    if (!IsFxWithinCameraRange(pos, 625.0f)) {
        return;
    }
    const FxPrtMult_c mult = FxDebrisMult(color, scale);
    if (amount <= 0) {
        return;
    }
    for (int32 i = amount; i > 0; --i) {
        const CVector vel = FxDebrisVelocity();
        g_fx.m_Glass->AddParticle(pos, vel, 0.0f, mult, -1.0f, std::bit_cast<float>(0x3F99999Au), std::bit_cast<float>(0x3F19999Au), false);
    }
}

// 0x49FB30
void Fx_c::AddWheelSpray(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, bool bInWater, float lightMult) {
    ((void(__thiscall*)(Fx_c*, CVehicle*, CVector, uint8, uint8, float))0x49FB30)(this, vehicle, pos, bWheelsSpinning, bInWater, lightMult);
}

// 0x49FF20
void Fx_c::AddWheelGrass(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    // Como el original (0x49FF20, `ret 18h`): ni `this` ni el byte de giro se usan; el color base del multiplicador es el unico dato propio.
    FxAddWheelDirt(vehicle, pos, lightMult, FxPrtMult_c(std::bit_cast<float>(0x3CF5C28Fu), std::bit_cast<float>(0x3DB851ECu), std::bit_cast<float>(0x3CF5C28Fu), 1.0f, 0.0f, 0.0f, std::bit_cast<float>(0x3D4CCCCDu)));
}

// 0x4A0170
void Fx_c::AddWheelGravel(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    // Como el original (0x4A0170, `ret 18h`): ni `this` ni el byte de giro se usan; el color base del multiplicador es el unico dato propio.
    FxAddWheelDirt(vehicle, pos, lightMult, FxPrtMult_c(0.25f, 0.25f, 0.25f, 1.0f, 0.0f, 0.0f, std::bit_cast<float>(0x3D4CCCCDu)));
}

// 0x4A03C0
void Fx_c::AddWheelMud(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    // Como el original (0x4A03C0, `ret 18h`): ni `this` ni el byte de giro se usan; el color base del multiplicador es el unico dato propio.
    FxAddWheelDirt(vehicle, pos, lightMult, FxPrtMult_c(0.25f, std::bit_cast<float>(0x3DF5C28Fu), std::bit_cast<float>(0x3D75C28Fu), 1.0f, 0.0f, 0.0f, std::bit_cast<float>(0x3D4CCCCDu)));
}

// 0x4A0610
void Fx_c::AddWheelSand(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    // Como el original (0x4A0610, `ret 18h`): lee la calidad de efectos de `this` (+0x54).
    FxAddWheelSandOrDust(this, vehicle, pos, bWheelsSpinning, lightMult, false);
}

// 0x4A09C0
void Fx_c::AddWheelDust(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult) {
    // Como el original (0x4A09C0, `ret 18h`): lee la calidad de efectos de `this` (+0x54).
    FxAddWheelSandOrDust(this, vehicle, pos, bWheelsSpinning, lightMult, true);
}

// 0x4A0D70
void Fx_c::TriggerWaterHydrant(const CVector& pos) {
    CreateFxWithinCameraRange("water_hydrant", pos, 625.0f);
}

// 0x4A0DE0
void Fx_c::TriggerGunshot(CEntity* entity, const CVector& origin, const CVector& target, bool doGunflash) {
    // Como el original (0x4A0DE0..0x4A0F9A, `ret 10h`): fuera del rango de 625 (camara - origen, NaN no sale) no hace nada.
    if (!IsFxWithinCameraRange(origin, 625.0f)) {
        return;
    }
    CVector   offset{ 0.0f, 0.0f, 0.0f }; // [esp+10h]: el desplazamiento respecto a la entidad; sin entidad, cero
    RwMatrix* created = nullptr;          // EBP: la matriz propia de Fx (solo sin entidad)
    RwMatrix* matrix;                     // ESI: la matriz padre de los sistemas
    if (entity) {
        // Como el original: la posicion de la entidad se toma de su matriz (+0x30) o, si no tiene, de la transformacion simple (+4), ANTES de crear la matriz que falte.
        const bool hadMatrix = entity->m_matrix != nullptr;
        const CVector& entityPos = hadMatrix ? entity->m_matrix->GetPosition() : entity->m_placement.m_vPosn;
        Fx77GunOffsetCpp(&origin.x, &entityPos.x, &offset.x);
        if (!hadMatrix) {
            entity->AllocateMatrix();                       // 0x54F560
            entity->m_placement.UpdateMatrix(entity->m_matrix); // 0x54F1B0
        }
        // Como el original: el desplazamiento se multiplica por la matriz de la entidad (Multiply3x3 vector * matriz, 0x59C810) y el resultado sustituye al desplazamiento que se pasa a los sistemas.
        {
            FlcX87 fp;
            float transformed[3];
            FlcMatInvMul3x3(fp, transformed, reinterpret_cast<const float*>(entity->m_matrix), &offset.x);
            std::memcpy(&offset.x, transformed, sizeof(transformed));
        }
        RwObject* const rwObject = entity->GetRwObject();
        if (!rwObject) {
            return;
        }
        matrix = &reinterpret_cast<RwFrame*>(rwObject->parent)->modelling; // [rwobject+4] + 0x10
    } else {
        created = g_fxMan.FxRwMatrixCreate(); // 0x4A9440
        CreateMatFromVec(created, &origin, &target); // 0x49E950
        matrix = created;
    }
    if (matrix) {
        // Como el original: el fogonazo solo si se pidio; sin entidad los sistemas copian la matriz padre antes de reproducirse.
        if (doGunflash) {
            if (FxSystem_c* const flash = g_fxMan.CreateFxSystem("gunflash", offset, matrix, false)) { // 0x4A9BE0
                if (!entity) {
                    flash->CopyParentMatrix(); // 0x4AA890
                }
                flash->PlayAndKill(); // 0x4AA3D0
            }
        }
        if (FxSystem_c* const smoke = g_fxMan.CreateFxSystem("gunsmoke", offset, matrix, false)) {
            if (!entity) {
                smoke->CopyParentMatrix();
            }
            smoke->PlayAndKill();
        }
    }
    if (created) {
        g_fxMan.FxRwMatrixDestroy(created); // 0x4A9460
    }
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
