/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlaceable
 * Reconstrucción: COMPLETA — 16/16 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/16 entradas.
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

#include "Placeable.h"

/**
 * Registra las operaciones de transformación y gestión del pool global de matrices.
 */
void CPlaceable::InjectHooks() {
    RH_ScopedVirtualClass(CPlaceable, 0x863C40, 1);
    RH_ScopedCategory("Entity");

    RH_ScopedOverloadedInstall(SetPosn, "xyz", 0x420B80, void(CPlaceable::*)(float, float, float));
    RH_ScopedOverloadedInstall(SetPosn, "vector", 0x4241C0, void(CPlaceable::*)(const CVector&));
    RH_ScopedOverloadedInstall(SetOrientation, "xyz", 0x439A80, void(CPlaceable::*)(float, float, float));
    RH_ScopedInstall(SetHeading, 0x43E0C0);
    RH_ScopedInstall(GetHeading, 0x441DB0);
    RH_ScopedInstall(GetRoll, 0x420B30);
    RH_ScopedOverloadedInstall(IsWithinArea, "xy", 0x54F200, bool(CPlaceable::*)(float, float, float, float) const);
    RH_ScopedOverloadedInstall(IsWithinArea, "xyz", 0x54F2B0, bool(CPlaceable::*)(float, float, float, float, float, float) const);
    RH_ScopedInstall(RemoveMatrix, 0x54F3B0);
    RH_ScopedInstall(AllocateStaticMatrix, 0x54F4C0);
    RH_ScopedInstall(AllocateMatrix, 0x54F560);
    RH_ScopedInstall(SetMatrix, 0x54F610);
    RH_ScopedInstall(GetMatrix, 0x411990);
    RH_ScopedInstall(ShutdownMatrixArray, 0x54EFD0);
    RH_ScopedInstall(InitMatrixArray, 0x54F3A0);
    RH_ScopedInstall(FreeStaticMatrix, 0x54F010);
}

static const float s_PlaceableMinusOne = -1.0f; // Como el original: constante 0x858C1C

namespace {

// Como el original: atan2(-forward.x, forward.y) con fpatan del x87 (precisión completa); el llamador lo guarda a float.
__declspec(naked) float __cdecl PlaceableHeadingFromForward(float negForwardX, float forwardY) {
    __asm {
        fld dword ptr [esp+4]
        fld dword ptr [esp+8]
        fpatan
        ret
    }
}
} // namespace

// Comienza con la representación compacta CSimpleTransform y sin CMatrixLink asignada.
CPlaceable::CPlaceable() : m_placement() {
    m_matrix = nullptr;
}

/**
 * Devuelve cualquier matriz dinámica al pool antes de invalidar el enlace local.
 */
CPlaceable::~CPlaceable() {
    if (m_matrix) {
        CPlaceable::RemoveMatrix();
        --numMatrices;
    }

    m_matrix = reinterpret_cast<CMatrixLink*>(&gDummyMatrix);
}

CVector CPlaceable::GetRightVector() {
    if (m_matrix)
        return m_matrix->GetRight();
    return { std::cos(m_placement.m_fHeading), std::sin(m_placement.m_fHeading), 0.0f };
}

CVector CPlaceable::GetForwardVector() {
    if (m_matrix)
        return m_matrix->GetForward();
    return { -std::sin(m_placement.m_fHeading), std::cos(m_placement.m_fHeading), 0.0f };
}

CVector CPlaceable::GetUpVector() {
    if (m_matrix)
        return m_matrix->GetUp();
    return {0.0f, 0.0f, 1.0f};
}

void CPlaceable::SetPosn(float x, float y, float z) {
    auto& pos = GetPosition();
    pos.Set(x, y, z);
}

void CPlaceable::SetPosn(const CVector& posn) {
    auto& pos = GetPosition();
    pos = posn;
}

/**
 * Actualiza orientación conservando la posición existente.
 *
 * Si no hay matriz completa, sólo puede representarse heading Z y se mantiene la
 * forma compacta para ahorrar una entrada del pool.
 */
void CPlaceable::SetOrientation(float x, float y, float z) {
    if (!m_matrix) {
        m_placement.m_fHeading = z;
        return;
    }

    CVector vecPos = m_matrix->GetPosition();
    m_matrix->SetRotate(x, y, z);
    m_matrix->GetPosition() += vecPos;
}

void CPlaceable::SetHeading(float heading) {
    if (m_matrix)
        m_matrix->SetRotateZOnly(heading);
    else
        m_placement.m_fHeading = heading;
}

// 0x441DB0
// Como el original: atan2(-forward.x, forward.y) con fpatan; el resultado se devuelve en ST0 sin redondear a float.
__declspec(naked) float CPlaceable::GetHeading() const {
    __asm {
        mov eax, dword ptr [ecx+14h]
        test eax, eax
        je no_matrix
        fld dword ptr [eax+10h]
        fchs
        fld dword ptr [eax+14h]
        fpatan
        ret
    no_matrix:
        fld dword ptr [ecx+10h]
        ret
    }
}

// 0x420B30
// Como el original: atan2(right.z, ±|right.xy|) con fsqrt (no la magnitud al cuadrado) y fpatan; el signo
// se invierte solo si up.z < 0 (NaN no) y el resultado se devuelve en ST0 sin redondear a float.
__declspec(naked) float CPlaceable::GetRoll() const {
    __asm {
        mov ecx, dword ptr [ecx+14h]
        test ecx, ecx
        je no_matrix
        fld dword ptr [ecx+4]
        fld dword ptr [ecx]
        fld st(0)
        fmul st, st(1)
        fld st(2)
        fmul st, st(3)
        faddp st(1), st
        fsqrt
        fstp st(2)
        fstp st(0)
        fldz
        fld dword ptr [ecx+28h]
        fcompp
        fnstsw ax
        test ah, 5
        jp not_negative
        fmul dword ptr [s_PlaceableMinusOne]
    not_negative:
        fld dword ptr [ecx+8]
        fxch st(1)
        fpatan
        ret
    no_matrix:
        fldz
        ret
    }
}

bool CPlaceable::IsWithinArea(float x1, float y1, float x2, float y2) const {
    const auto& vecPos = GetPosition();
    if (x1 > x2)
        std::swap(x1, x2);

    if (y1 > y2)
        std::swap(y1, y2);

    return vecPos.x >= x1 && vecPos.x <= x2 && vecPos.y >= y1 && vecPos.y <= y2;
}


bool CPlaceable::IsWithinArea(float x1, float y1, float z1, float x2, float y2, float z2) const {
    const auto& vecPos = GetPosition();
    if (x1 > x2)
        std::swap(x1, x2);

    if (y1 > y2)
        std::swap(y1, y2);

    if (z1 > z2)
        std::swap(z1, z2);

    return vecPos.x >= x1
        && vecPos.x <= x2
        && vecPos.y >= y1
        && vecPos.y <= y2
        && vecPos.z >= z1
        && vecPos.z <= z2;
}

/**
 * Colapsa la CMatrixLink completa a posición + heading, la devuelve a gMatrixList
 * y deja el objeto nuevamente en representación compacta.
 */
void CPlaceable::RemoveMatrix() {
    const auto& vecForward = m_matrix->GetForward();
    const float fHeading = PlaceableHeadingFromForward(-vecForward.x, vecForward.y); // Como el original: fpatan

    m_placement.m_vPosn = m_matrix->GetPosition();
    m_placement.m_fHeading = fHeading;

    m_matrix->m_pOwner = nullptr;
    gMatrixList.MoveToFreeList(m_matrix);
    m_matrix = nullptr;
}

// Reserva/mueve la matriz a la segunda lista del pool, expulsando la más antigua si está lleno.
void CPlaceable::AllocateStaticMatrix() {
    if (m_matrix)
        return gMatrixList.MoveToList2(m_matrix);

    if (gMatrixList.IsFull())
        gMatrixList.GetOldestLink()->m_pOwner->RemoveMatrix();

    m_matrix = gMatrixList.AddToList2();
    m_matrix->m_pOwner = this;
}

// Reserva una matriz de la lista principal y asigna este CPlaceable como owner.
void CPlaceable::AllocateMatrix() {
    if (m_matrix)
        return;

    if (gMatrixList.IsFull())
        gMatrixList.GetOldestLink()->m_pOwner->RemoveMatrix();

    m_matrix = gMatrixList.AddToList1();
    m_matrix->m_pOwner = this;
}

void CPlaceable::SetMatrix(CMatrix& matrix) {
    if (!m_matrix) {
        if (matrix.GetUp().z == 1.0F) {
            auto& vecForward = matrix.GetForward();
            const float fHeading = PlaceableHeadingFromForward(-vecForward.x, vecForward.y); // Como el original: fpatan

            m_placement.m_vPosn = matrix.GetPosition();
            m_placement.m_fHeading = fHeading;
            return;
        }
        CPlaceable::AllocateMatrix();
    }

    *static_cast<CMatrix*>(m_matrix) = matrix;
}

// NOTSA
bool CPlaceable::IsPointInRange(const CVector& point, float range) {
    return DistanceBetweenPointsSquared(point, GetPosition()) <= sq(range);
}

/**
 * Materializa perezosamente una CMatrixLink a partir de CSimpleTransform cuando
 * algún consumidor necesita la transformación completa.
 */
CMatrix& CPlaceable::GetMatrix() {
    if (!m_matrix) {
        CPlaceable::AllocateMatrix();
        m_placement.UpdateMatrix(m_matrix);
    }

    return *m_matrix;
}

void CPlaceable::ShutdownMatrixArray() {
    gMatrixList.Shutdown();
}

void CPlaceable::InitMatrixArray() {
    ZoneScoped;

    gMatrixList.Init(CPlaceable::NUM_MATRICES_TO_CREATE);
}

void CPlaceable::FreeStaticMatrix() {
    gMatrixList.MoveToList1(m_matrix);
}

void CPlaceable::GetOrientation(float& x, float& y, float& z) {
    if (!m_matrix) {
        z = m_placement.m_fHeading;
        return;
    }

    x = asinf(GetForward().z);

    float cosx = std::cosf(x);
    float cosy = GetUp().z / cosx;
    y = std::acosf(cosy);

    float cosz = GetForward().y / cosx;
    z = std::acosf(cosz);
}
