/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGarage
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Garage.h"

namespace { const float GarageQueryZero = 0.0f; }

void CGarage::InjectHooks() {
    RH_ScopedClass(CGarage);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(BuildRotatedDoorMatrix, 0x4479F0);
    // RH_ScopedInstall(TidyUpGarageClose, 0x449D10);
    // RH_ScopedInstall(TidyUpGarage, 0x449C50);
    RH_ScopedInstall(StoreAndRemoveCarsForThisHideOut, 0x449900);
    RH_ScopedInstall(EntityHasASphereWayOutsideGarage, 0x449050);
    // RH_ScopedInstall(RemoveCarsBlockingDoorNotInside, 0x449690);
    RH_ScopedInstall(IsEntityTouching3D, 0x448EE0);
    // RH_ScopedInstall(IsEntityEntirelyOutside, 0x448D30);
    // RH_ScopedInstall(IsStaticPlayerCarEntirelyInside, 0x44A830);
    // RH_ScopedInstall(IsEntityEntirelyInside3D, 0x448BE0);
    RH_ScopedOverloadedInstall(IsPointInsideGarage, "point", 0x448740, bool(CGarage::*)(CVector));
    // RH_ScopedInstall(PlayerArrestedOrDied, 0x4486C0);
    RH_ScopedInstall(OpenThisGarage, 0x447D50);
    RH_ScopedInstall(CloseThisGarage, 0x447D70);
    // RH_ScopedInstall(InitDoorsAtStart, 0x447600);
    RH_ScopedOverloadedInstall(IsPointInsideGarage, "radius", 0x4487D0, bool(CGarage::*)(CVector, float));
    // RH_ScopedInstall(Update, 0x44AA50);
}

// 0x4479F0
void CGarage::BuildRotatedDoorMatrix(CEntity* entity, float fDoorPosition) {
    // Como el original: fsin/fcos del x87 (precisión completa) sobre el ángulo redondeado a float
    // Como el original: constante 0x859998 = -π/2 exacto (el HALF_PI de common.h es impreciso)
    const float fAngle = fDoorPosition * std::bit_cast<float>(0xBFC90FDBu);
    float sinA, cosA; // nombres distintos de fsin/fcos: el ensamblador en línea no distingue mayúsculas
    __asm {
        fld   fAngle
        fld   st(0)
        fsin
        fstp  sinA
        fcos
        fstp  cosA
    }
    const float fSin = sinA;
    const float fCos = cosA;
    CMatrix& matrix = entity->GetMatrix();

    const auto& vecForward = matrix.GetForward();
    matrix.GetUp() = CVector(-fSin * vecForward.y, fSin * vecForward.x, fCos);
    matrix.GetRight() = CrossProduct(vecForward, matrix.GetUp());
}

// 0x449D10
void CGarage::TidyUpGarageClose() {
    plugin::CallMethod<0x449D10, CGarage*>(this);
}

// 0x449C50
void CGarage::TidyUpGarage() {
    plugin::CallMethod<0x449C50, CGarage*>(this);
}

// 0x449900
void CGarage::StoreAndRemoveCarsForThisHideOut(CStoredCar* storedCars, int32 maxSlot) {
    maxSlot = std::min<int32>(maxSlot, NUM_GARAGE_STORED_CARS);

    for (auto i = 0; i < NUM_GARAGE_STORED_CARS; i++)
        storedCars[i].Clear();

    auto pool = GetVehiclePool();
    int32 storedCarIdx{0}; // Como el original: comparación con signo contra maxSlot
    for (auto i = pool->GetSize(); i; i--) {
        if (auto vehicle = pool->GetAt(i - 1)) {
            if (IsPointInsideGarage(vehicle->GetPosition()) && vehicle->GetCreatedBy() != MISSION_VEHICLE) {
                if (storedCarIdx < maxSlot && !EntityHasASphereWayOutsideGarage(vehicle, 1.0f)) {
                    storedCars[storedCarIdx++].StoreCar(vehicle);
                }

                FindPlayerInfo().CancelPlayerEnteringCars(vehicle);
                CWorld::Remove(vehicle);
                delete vehicle;
            }
        }
    }

    // Clear slots with no vehicles in it
    for (auto i = storedCarIdx; i < (int32)NUM_GARAGE_STORED_CARS; i++)
        storedCars[i].Clear();
}


namespace {
__declspec(naked) void GarageSphere15_4487D0() {
    __asm { push 04487D0h }
    __asm { ret }
}
__declspec(naked) void GarageSphere15_535300() {
    __asm { push 0535300h }
    __asm { ret }
}
__declspec(naked) void GarageSphere15_54F1B0() {
    __asm { push 054F1B0h }
    __asm { ret }
}
__declspec(naked) void GarageSphere15_54F560() {
    __asm { push 054F560h }
    __asm { ret }
}
__declspec(naked) void GarageSphere15_59C890() {
    __asm { push 059C890h }
    __asm { ret }
}
}

// 0x449050
__declspec(naked) bool CGarage::EntityHasASphereWayOutsideGarage(CEntity* entity, float fRadius) {
    // Como el original: 0x449050..0x4490EF, AL y ret 8; colaboradores reales.
    __asm {
        sub esp,10h
        push ebx
        mov ebx,dword ptr [esp+18h]
        push ebp
        push esi
        mov dword ptr [esp+0Ch],ecx
        push edi
        mov ecx,ebx
        call GarageSphere15_535300
        mov esi,dword ptr [eax+2Ch]
        xor ebp,ebp
        cmp word ptr [esi],bp
        jle L_4490DA
        xor edi,edi
L_449072:
        mov eax,dword ptr [ebx+14h]
        test eax,eax
        jne L_44908C
        mov ecx,ebx
        call GarageSphere15_54F560
        mov eax,dword ptr [ebx+14h]
        push eax
        lea ecx,[ebx+4]
        call GarageSphere15_54F1B0
L_44908C:
        mov ecx,dword ptr [esi+8]
        mov eax,dword ptr [ebx+14h]
        add ecx,edi
        push ecx
        push eax
        lea edx,[esp+1Ch]
        push edx
        call GarageSphere15_59C890
        fld dword ptr [esp+34h]
        mov ecx,dword ptr [esi+8]
        fadd dword ptr [ecx+edi+0Ch]
        push ecx
        mov ecx,dword ptr [eax]
        mov edx,esp
        fstp dword ptr [esp+0Ch]
        mov dword ptr [edx],ecx
        mov ecx,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [edx+4],ecx
        mov ecx,dword ptr [esp+20h]
        mov dword ptr [edx+8],eax
        call GarageSphere15_4487D0
        test al,al
        je L_4490E6
        movsx ecx,word ptr [esi]
        inc ebp
        add edi,14h
        cmp ebp,ecx
        jl L_449072
L_4490DA:
        xor al,al
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp,10h
        ret 8
L_4490E6:
        pop edi
        pop esi
        pop ebp
        mov al,1
        pop ebx
        add esp,10h
        ret 8
    }
}

// 0x449690
void CGarage::RemoveCarsBlockingDoorNotInside() {
    plugin::CallMethod<0x449690, CGarage*>(this);
}

// 0x448EE0
static uintptr_t GarageTouchColBody = std::bit_cast<uintptr_t>(&CEntity::GetColModel);
static uintptr_t GarageTouchMatrixBody = std::bit_cast<uintptr_t>(&CPlaceable::GetMatrix);
static uintptr_t GarageTouchGeometryBody = std::bit_cast<uintptr_t>(static_cast<bool(CGarage::*)(CVector,float)>(&CGarage::IsPointInsideGarage));
static uintptr_t GarageTouchNativePoint = 0x59C890;
// Literal x87 and sphere-loop transcription of 0x448EE0..0x449041.
__declspec(naked) bool CGarage::IsEntityTouching3D(CEntity*) {
    __asm {
        sub esp,10h
        push ebx
        mov ebx,dword ptr [esp+18h]
        movsx eax,word ptr [ebx+22h]
        mov edx,ecx
        mov ecx,dword ptr [eax*4+00A9B0C8h]
        mov eax,dword ptr [ecx+14h]
        mov ecx,dword ptr [eax+24h]
        push ebp
        push esi
        mov dword ptr [esp+20h],ecx
        mov ecx,dword ptr [ebx+14h]
        xor esi,esi
        cmp ecx,esi
        push edi
        mov dword ptr [esp+10h],edx
        lea eax,[ecx+30h]
        jne garageTouch00448F15
        lea eax,[ebx+4]
garageTouch00448F15:
        fld dword ptr [edx+28h]
        fsub dword ptr [esp+24h]
        fcomp dword ptr [eax]
        fnstsw ax
        test ah,41h
        je garageTouch0044902C
        cmp ecx,esi
        lea eax,[ecx+30h]
        jne garageTouch00448F33
        lea eax,[ebx+4]
garageTouch00448F33:
        fld dword ptr [esp+24h]
        fadd dword ptr [edx+2Ch]
        fcomp dword ptr [eax]
        fnstsw ax
        test ah,5
        jnp garageTouch0044902C
        cmp ecx,esi
        lea eax,[ecx+30h]
        jne garageTouch00448F51
        lea eax,[ebx+4]
garageTouch00448F51:
        fld dword ptr [edx+30h]
        fsub dword ptr [esp+24h]
        fcomp dword ptr [eax+4]
        fnstsw ax
        test ah,41h
        je garageTouch0044902C
        cmp ecx,esi
        lea eax,[ecx+30h]
        jne garageTouch00448F70
        lea eax,[ebx+4]
garageTouch00448F70:
        fld dword ptr [esp+24h]
        fadd dword ptr [edx+34h]
        fcomp dword ptr [eax+4]
        fnstsw ax
        test ah,5
        jnp garageTouch0044902C
        cmp ecx,esi
        lea eax,[ecx+30h]
        jne garageTouch00448F8F
        lea eax,[ebx+4]
garageTouch00448F8F:
        fld dword ptr [edx+8]
        fsub dword ptr [esp+24h]
        fcomp dword ptr [eax+8]
        fnstsw ax
        test ah,41h
        je garageTouch0044902C
        cmp ecx,esi
        je garageTouch00448FAD
        add ecx,30h
        jmp garageTouch00448FB0
garageTouch00448FAD:
        lea ecx,[ebx+4]
garageTouch00448FB0:
        fld dword ptr [esp+24h]
        fadd dword ptr [edx+1Ch]
        fcomp dword ptr [ecx+8]
        fnstsw ax
        test ah,5
        jnp garageTouch0044902C
        mov ecx,ebx
        call dword ptr [GarageTouchColBody]
        mov edi,dword ptr [eax+2Ch]
        cmp word ptr [edi],si
        mov dword ptr [esp+24h],esi
        jle garageTouch0044902C
        mov edx,dword ptr [edi+8]
garageTouch00448FD7:
        add edx,esi
        push edx
        mov ecx,ebx
        call dword ptr [GarageTouchMatrixBody]
        push eax
        lea eax,[esp+1Ch]
        push eax
        call dword ptr [GarageTouchNativePoint]
        mov edx,dword ptr [edi+8]
        mov ecx,dword ptr [esi+edx+0Ch]
        mov ebp,dword ptr [eax]
        add esp,0Ch
        push ecx
        sub esp,0Ch
        mov ecx,esp
        mov dword ptr [ecx],ebp
        mov ebp,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [ecx+4],ebp
        mov dword ptr [ecx+8],eax
        mov ecx,dword ptr [esp+20h]
        call dword ptr [GarageTouchGeometryBody]
        test al,al
        jne garageTouch00449038
        mov eax,dword ptr [esp+24h]
        movsx ecx,word ptr [edi]
        inc eax
        add esi,14h
        cmp eax,ecx
        mov dword ptr [esp+24h],eax
        jl garageTouch00448FD7
garageTouch0044902C:
        xor al,al
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp,10h
        ret 4
garageTouch00449038:
        pop edi
        pop esi
        pop ebp
        mov al,1
        pop ebx
        add esp,10h
        ret 4
    }
}

// 0x448D30
bool CGarage::IsEntityEntirelyOutside(CEntity* entity, float radius) {
    return plugin::CallMethodAndReturn<bool, 0x448D30, CGarage*, CEntity*, float>(this, entity, radius);
}

// 0x44A830
bool CGarage::IsStaticPlayerCarEntirelyInside() {
    return plugin::CallMethodAndReturn<bool, 0x44A830, CGarage*>(this);
}

// 0x448BE0
bool CGarage::IsEntityEntirelyInside3D(CEntity* entity, float radius) {
    return plugin::CallMethodAndReturn<bool, 0x448BE0, CGarage*, CEntity*, float>(this, entity, radius);
}

// 0x448740
// 0x448764 sobrescribe el ECX guardado con dy; el pop final devuelve esos bits. EDX queda intacto.
__declspec(naked) bool CGarage::IsPointInsideGarage(CVector point) {
    __asm {
        push ecx
        fld dword ptr [esp+10h]
        fcomp dword ptr [ecx+8]
        fnstsw ax
        test ah,5
        jnp gpoint004487B4
        fld dword ptr [esp+10h]
        fcomp dword ptr [ecx+1Ch]
        fnstsw ax
        test ah,41h
        je gpoint004487B4
        fld dword ptr [esp+0Ch]
        fsub dword ptr [ecx+4]
        fstp dword ptr [esp]
        fld dword ptr [esp+8]
        fsub dword ptr [ecx]
        fst dword ptr [esp+8]
        fmul dword ptr [ecx+0Ch]
        fld dword ptr [esp]
        fmul dword ptr [ecx+10h]
        faddp st(1),st
        fcom dword ptr [GarageQueryZero]
        fnstsw ax
        test ah,5
        jnp gpoint004487B2
        fcomp dword ptr [ecx+20h]
        fnstsw ax
        test ah,41h
        je gpoint004487B4
        fld dword ptr [esp+8]
        fmul dword ptr [ecx+14h]
        fld dword ptr [esp]
        fmul dword ptr [ecx+18h]
        faddp st(1),st
        fcom dword ptr [GarageQueryZero]
        fnstsw ax
        test ah,5
        jp gpoint004487BA
gpoint004487B2:
        fstp st(0)
gpoint004487B4:
        xor al,al
        pop ecx
        ret 0Ch
gpoint004487BA:
        fcomp dword ptr [ecx+24h]
        fnstsw ax
        test ah,41h
        je gpoint004487B4
        mov al,1
        pop ecx
        ret 0Ch
    }
}

// 0x4486C0
eGarageDoorState CGarage::PlayerArrestedOrDied() {
    return plugin::CallMethodAndReturn<eGarageDoorState, 0x4486C0, CGarage*>(this);
}

// 0x447D50
void CGarage::OpenThisGarage() {
  if ( m_nDoorState == GARAGE_DOOR_CLOSED
    || m_nDoorState == GARAGE_DOOR_CLOSING
    || m_nDoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR)
  {
    m_nDoorState = GARAGE_DOOR_OPENING;
  }
}

// 0x447D70
void CGarage::CloseThisGarage() {
    if (m_nDoorState == GARAGE_DOOR_OPEN || m_nDoorState == GARAGE_DOOR_OPENING)
        m_nDoorState = GARAGE_DOOR_CLOSING;
}

// 0x447600
void CGarage::InitDoorsAtStart() {
    plugin::CallMethod<0x447600, CGarage*>(this);
}

// 0x4487D0
// Copia del flujo x87 original; conserva ECX y EDX que reutiliza FindGarageForObject.
__declspec(naked) bool CGarage::IsPointInsideGarage(CVector point, float radius) {
    __asm {
        fld dword ptr [ecx+8]
        sub esp,8
        fsub dword ptr [esp+18h]
        fcomp dword ptr [esp+14h]
        fnstsw ax
        test ah,41h
        je gradius0044885C
        fld dword ptr [esp+18h]
        fadd dword ptr [ecx+1Ch]
        fcomp dword ptr [esp+14h]
        fnstsw ax
        test ah,5
        jnp gradius0044885C
        fld dword ptr [esp+10h]
        fsub dword ptr [ecx+4]
        fstp dword ptr [esp]
        fld dword ptr [esp+0Ch]
        fsub dword ptr [ecx]
        fst dword ptr [esp+0Ch]
        fmul dword ptr [ecx+0Ch]
        fld dword ptr [esp]
        fmul dword ptr [ecx+10h]
        faddp st(1),st
        fld dword ptr [esp+18h]
        fchs
        fstp dword ptr [esp+4]
        fcom dword ptr [esp+4]
        fnstsw ax
        test ah,5
        jnp gradius0044885A
        fld dword ptr [esp+18h]
        fadd dword ptr [ecx+20h]
        fxch st(1)
        fcompp
        fnstsw ax
        test ah,41h
        je gradius0044885C
        fld dword ptr [esp+0Ch]
        fmul dword ptr [ecx+14h]
        fld dword ptr [esp]
        fmul dword ptr [ecx+18h]
        faddp st(1),st
        fcom dword ptr [esp+4]
        fnstsw ax
        test ah,5
        jp gradius00448864
gradius0044885A:
        fstp st(0)
gradius0044885C:
        xor al,al
        add esp,8
        ret 10h
gradius00448864:
        fld dword ptr [esp+18h]
        fadd dword ptr [ecx+24h]
        fxch st(1)
        fcompp
        fnstsw ax
        test ah,41h
        je gradius0044885C
        mov al,1
        add esp,8
        ret 10h
    }
}

// 0x44AA50
void CGarage::Update(int32 garageId) {
    plugin::CallMethod<0x44AA50, CGarage*>(this, garageId);
}

bool CGarage::IsHideOut() const {
    switch (m_nType) {
    case eGarageType::SAFEHOUSE_GANTON:
    case eGarageType::SAFEHOUSE_SANTAMARIA:
    case eGarageType::SAGEHOUSE_ROCKSHORE:
    case eGarageType::SAFEHOUSE_FORTCARSON:
    case eGarageType::SAFEHOUSE_VERDANTMEADOWS:
    case eGarageType::SAFEHOUSE_DILLIMORE:
    case eGarageType::SAFEHOUSE_PRICKLEPINE:
    case eGarageType::SAFEHOUSE_WHITEWOOD:
    case eGarageType::SAFEHOUSE_PALOMINOCREEK:
    case eGarageType::SAFEHOUSE_REDSANDSWEST:
    case eGarageType::SAFEHOUSE_ELCORONA:
    case eGarageType::SAFEHOUSE_MULHOLLAND:
    case eGarageType::SAFEHOUSE_CALTONHEIGHTS:
    case eGarageType::SAFEHOUSE_PARADISO:
    case eGarageType::SAFEHOUSE_DOHERTY:
    case eGarageType::SAFEHOUSE_HASHBURY:
    case eGarageType::HANGAR_ABANDONED_AIRPORT:
        return true;
    default:
        return false;
    }
}

// 0x44A9C0
bool CGarage::IsGarageEmpty() {
    return plugin::CallMethodAndReturn<bool, 0x44A9C0, CGarage*>(this);

    CVector cornerA = { m_fLeftCoord, m_fFrontCoord, m_vPosn.z };
    CVector cornerB = { m_fRightCoord, m_fBackCoord, m_fTopZ   };

    int16 outCount[2];
    CEntity* outEntities[16];
    CWorld::FindObjectsIntersectingCube(cornerA, cornerB, outCount, static_cast<int16>(std::size(outEntities)), outEntities, false, true, true, false, false);
    if (outCount[0] <= 0)
        return true;

    int16 entityIndex = 0;

    while (!IsEntityTouching3D(outEntities[entityIndex])) {
        if (++entityIndex >= outCount[0])
            return true;
    }
    return false;
}

/*
void CGarage::CenterCarInGarage(CEntity* entity) {
    auto vehicle = FindPlayerVehicle();
    if (IsAnyOtherCarTouchingGarage(vehicle))
        return;

    auto player = FindPlayerPed();
    if (IsAnyOtherPedTouchingGarage(player))
        return;

    auto pos = entity->GetPosition();

    const auto halfX = (m_fRightCoord + m_fLeftCoord) * 0.5f;
    const auto halfY = (m_fBackCoord + m_fFrontCoord) * 0.5f;
    CVector p1{
        halfX - pos.x,
        halfY - pos.y,
        pos.z - pos.z
    };

    auto dist = p1.Magnitude();
    if (dist >= 0.4f) {
        auto x = halfX - pos.x * 0.4f / dist + pos.x;
        auto y = 0.4f / dist * halfY - pos.y + pos.y;
    } else {
        auto x = halfX;
        auto y = halfY;
    }

    if (!IsEntityEntirelyInside3D(entity, 0.3f))
        entity->SetPosn(entity->GetPosition());
}
*/

// 0x5D3020
void CSaveGarage::CopyGarageIntoSaveGarage(Const CGarage& g) {
    m_nType         = g.m_nType;
    m_nDoorState    = g.m_nDoorState;
    m_nFlags        = g.m_nFlags;
    m_vPosn         = g.m_vPosn;
    m_vDirectionA   = g.m_vDirectionA;
    m_vDirectionB   = g.m_vDirectionB;
    m_fTopZ         = g.m_fTopZ;
    m_fWidth        = g.m_fWidth;
    m_fHeight       = g.m_fHeight;
    m_fLeftCoord    = g.m_fLeftCoord;
    m_fRightCoord   = g.m_fRightCoord;
    m_fFrontCoord   = g.m_fFrontCoord;
    m_fBackCoord    = g.m_fBackCoord;
    m_fDoorPosition = g.m_fDoorPosition;
    m_nTimeToOpen   = g.m_nTimeToOpen;
    m_nOriginalType = g.m_nOriginalType;
    strcpy(m_anName, g.m_anName); // Como el original: strcpy (sin relleno 0xFE de depuración)
}

// 0x5D30C0
void CSaveGarage::CopyGarageOutOfSaveGarage(CGarage& g) const {
    g.m_nType         = m_nType;
    g.m_nDoorState    = m_nDoorState;
    g.m_nFlags        = m_nFlags;
    g.m_vPosn         = m_vPosn;
    g.m_vDirectionA   = m_vDirectionA;
    g.m_vDirectionB   = m_vDirectionB;
    g.m_fTopZ         = m_fTopZ;
    g.m_fWidth        = m_fWidth;
    g.m_fHeight       = m_fHeight;
    g.m_fLeftCoord    = m_fLeftCoord;
    g.m_fRightCoord   = m_fRightCoord;
    g.m_fFrontCoord   = m_fFrontCoord;
    g.m_fBackCoord    = m_fBackCoord;
    g.m_fDoorPosition = m_fDoorPosition;
    g.m_nTimeToOpen   = m_nTimeToOpen;
    g.m_nOriginalType = m_nOriginalType;
    g.m_pTargetCar    = nullptr;
    strcpy(g.m_anName, m_anName); // Como el original: strcpy (sin relleno 0xFE de depuración)
}

// todo move
// 0x449760
void CStoredCar::StoreCar(CVehicle* vehicle) {
    plugin::CallMethod<0x449760, CStoredCar*, CVehicle*>(this, vehicle);
}

// 0x447E40
CVehicle* CStoredCar::RestoreCar() {
    return plugin::CallMethodAndReturn<CVehicle*, 0x447E40, CStoredCar*>(this);
}
