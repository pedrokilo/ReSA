/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlaceable
 * Reconstrucción: COMPLETA — 21/21 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 21/21 entradas.
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
#pragma warning(push, 0)
#include "StdInc.h"

#include "Placeable.h"
#include "X87Math.h"
#pragma warning(pop)

// 0x41BD80 (lote 140A, banco Fn140A): copia fuera de linea de CPlaceable::GetMatrix del EXE (thiscall, sin argumentos, `ret`, 4 bytes: `mov eax,[ecx+14h]`): devuelve el puntero m_matrix tal cual, sin
// comprobar nada (los vehiculos lo llaman en CAutomobile::ProcessCarWheelPair y similares).
static CMatrixLink* __fastcall PlaceableGetMatrixCopy(CPlaceable* self, void* /*edx*/) { return self->m_matrix; }

// 0x54F1E0 (lote 161A, banco Fn161A): constructor de CPlaceable del EXE (thiscall, sin argumentos, `ret`, 26 bytes: `mov eax,ecx; xor ecx,ecx; mov [eax],0x863C40; mov [eax+4],ecx; ... mov [eax+0x14],ecx; ret`):
// vtable de CPlaceable del EXE (0x863C40; la de la DLL no vale: las subclases del EXE la sobrescriben) y a cero los cinco dwords siguientes (posicion, rumbo y puntero de matriz), en ese orden; devuelve this.
// Lo expanden en linea CEntity (0x532A94) y CCamera (0x51A46F).
__declspec(noinline) static CPlaceable* __fastcall PlaceableConstructorBody(CPlaceable* self, void* /*edx*/) {
    auto* const dwords = reinterpret_cast<uint32*>(self);
    dwords[0] = 0x863C40;
    dwords[1] = 0;
    dwords[2] = 0;
    dwords[3] = 0;
    dwords[4] = 0;
    dwords[5] = 0;
    return self;
}

// 0x54F490 (lote 200A, sin validar; thiscall, sin argumentos, `ret`, 36 bytes): cuerpo del destructor de CPlaceable del EXE (lo llaman CEntity 0x535E90 y 0x50A870, ademas de la ranura 0 de la vtable 0x863C40). Como el original: lee
// m_matrix (+0x14), deja SIEMPRE la vtable de CPlaceable del EXE (0x863C40), y solo si habia matriz llama a RemoveMatrix (0x54F3B0, con hook; ECX = this) y decrementa el contador de matrices (0xB74238); termina con
// m_matrix = la matriz de relleno (0xB74240). El destructor generado ~CPlaceable deja la vtable de la DLL.
__declspec(noinline) static void __fastcall PlaceableDestructorBodyExe(CPlaceable* self, void* /*edx*/) {
    const bool hasMatrix = self->m_matrix != nullptr;
    *reinterpret_cast<uint32*>(self) = 0x863C40;
    if (hasMatrix) {
        self->RemoveMatrix();
        --numMatrices;
    }
    self->m_matrix = reinterpret_cast<CMatrixLink*>(&gDummyMatrix);
}

// 0x54F5D0 (lote 200A, sin validar; thiscall, un argumento de pila, `ret 4`, 53 bytes): destructor con borrado de CPlaceable (ranura 0 de la vtable 0x863C40) con el cuerpo 0x54F490 EXPANDIDO en linea (mismas escrituras; el EXE
// comprueba las banderas entre RemoveMatrix y la escritura final de m_matrix, sin efecto observable). Como el original: libera `this` con operator delete (0x8214BD, CRT, sin hook) solo si el bit 0 del BYTE de banderas esta puesto y
// devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) static CPlaceable* __fastcall PlaceableScalarDeletingDestructor(CPlaceable* self, void* /*edx*/, uint8 flags) {
    const bool hasMatrix = self->m_matrix != nullptr;
    *reinterpret_cast<uint32*>(self) = 0x863C40;
    if (hasMatrix) {
        self->RemoveMatrix();
        --numMatrices;
    }
    const bool freeMemory = (flags & 1) != 0;
    self->m_matrix = reinterpret_cast<CMatrixLink*>(&gDummyMatrix);
    if (freeMemory) {
        ::operator delete(self);
    }
    return self;
}

// 0x54FA10, 0x54FA60, 0x54FAB0, 0x54FB00, 0x54FB50, 0x54FBA0 y 0x54FBF0 (lote 200A, sin validar; el listado decia CPlaceable, pero NO usan nada de CPlaceable): siete copias identicas (thiscall, sin argumentos, `ret`, 54 bytes cada una, con
// 80 bytes de paso: hay diez mas hasta 0x54FF10) de un mismo codigo de plantilla que vacia un bloque de dos punteros, sin llamadores ni referencias en el EXE (codigo muerto). Layout visto en el codigo: +0 = puntero A, +4 =
// puntero B, +8 = contador (int32 CON SIGNO), +0xC = entero, +0x10 = BYTE "es dueno". Como el original: si el contador es mayor que cero (comparacion con signo: cero o negativo no hace nada) y el BYTE +0x10 es distinto de cero libera
// A y despues B con operator delete[] (0x8213AE, CRT, sin hook; no se comprueba si son nulos), y SIEMPRE (con contador > 0) pone a cero +0, +4, +8 y +0xC en ese orden. Cada direccion lleva su propia funcion enganchada porque un
// hook Simple reescribe el comienzo de la funcion de la DLL con un salto a la direccion del EXE mientras esta apagado.
__declspec(noinline) static void PlaceableArrayPairResetBody(void* self) {
    auto* const bytes = reinterpret_cast<uint8*>(self);
    if (*reinterpret_cast<int32*>(bytes + 8) > 0) {
        if (bytes[0x10] != 0) {
            ::operator delete[](*reinterpret_cast<void**>(bytes + 0));
            ::operator delete[](*reinterpret_cast<void**>(bytes + 4));
        }
        *reinterpret_cast<uint32*>(bytes + 0x0) = 0;
        *reinterpret_cast<uint32*>(bytes + 0x4) = 0;
        *reinterpret_cast<uint32*>(bytes + 0x8) = 0;
        *reinterpret_cast<uint32*>(bytes + 0xC) = 0;
    }
}
__declspec(noinline) static void __fastcall PlaceableArrayPairReset54FA10(void* self, void* /*edx*/) { PlaceableArrayPairResetBody(self); }
__declspec(noinline) static void __fastcall PlaceableArrayPairReset54FA60(void* self, void* /*edx*/) { PlaceableArrayPairResetBody(self); }
__declspec(noinline) static void __fastcall PlaceableArrayPairReset54FAB0(void* self, void* /*edx*/) { PlaceableArrayPairResetBody(self); }
__declspec(noinline) static void __fastcall PlaceableArrayPairReset54FB00(void* self, void* /*edx*/) { PlaceableArrayPairResetBody(self); }
__declspec(noinline) static void __fastcall PlaceableArrayPairReset54FB50(void* self, void* /*edx*/) { PlaceableArrayPairResetBody(self); }
__declspec(noinline) static void __fastcall PlaceableArrayPairReset54FBA0(void* self, void* /*edx*/) { PlaceableArrayPairResetBody(self); }
__declspec(noinline) static void __fastcall PlaceableArrayPairReset54FBF0(void* self, void* /*edx*/) { PlaceableArrayPairResetBody(self); }

/**
 * Registra las operaciones de transformación y gestión del pool global de matrices.
 */
void CPlaceable::InjectHooks() {
    RH_ScopedVirtualClass(CPlaceable, 0x863C40, 1);
    RH_ScopedCategory("Entity");

    RH_ScopedNamedGlobalInstall(PlaceableConstructorBody, "Constructor", 0x54F1E0); // lote 161A (banco Fn161A)
    RH_ScopedNamedGlobalInstall(PlaceableDestructorBodyExe, "DestructorBody", 0x54F490);                       // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(PlaceableScalarDeletingDestructor, "ScalarDeletingDestructor", 0x54F5D0);      // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(PlaceableArrayPairReset54FA10, "ArrayPairReset54FA10", 0x54FA10);              // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(PlaceableArrayPairReset54FA60, "ArrayPairReset54FA60", 0x54FA60);              // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(PlaceableArrayPairReset54FAB0, "ArrayPairReset54FAB0", 0x54FAB0);              // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(PlaceableArrayPairReset54FB00, "ArrayPairReset54FB00", 0x54FB00);              // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(PlaceableArrayPairReset54FB50, "ArrayPairReset54FB50", 0x54FB50);              // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(PlaceableArrayPairReset54FBA0, "ArrayPairReset54FBA0", 0x54FBA0);              // lote 200A (sin validar)
    RH_ScopedNamedGlobalInstall(PlaceableArrayPairReset54FBF0, "ArrayPairReset54FBF0", 0x54FBF0);              // lote 200A (sin validar)
    RH_ScopedOverloadedInstall(SetPosn, "xyz", 0x420B80, void(CPlaceable::*)(float, float, float));
    RH_ScopedOverloadedInstall(SetPosn, "vector", 0x4241C0, void(CPlaceable::*)(const CVector&));
    RH_ScopedOverloadedInstall(SetOrientation, "xyz", 0x439A80, void(CPlaceable::*)(float, float, float));
    RH_ScopedInstall(SetHeading, 0x43E0C0);
    RH_ScopedInstall(GetHeading, 0x441DB0);
    RH_ScopedInstall(GetRoll, 0x420B30);
    RH_ScopedInstall(GetForwardVector, 0x41CCB0);
    RH_ScopedInstall(GetRightVector, 0x41CC70);
    RH_ScopedInstall(GetUpVector, 0x50E420);
    RH_ScopedOverloadedInstall(IsWithinArea, "xy", 0x54F200, bool(CPlaceable::*)(float, float, float, float) const);
    RH_ScopedOverloadedInstall(IsWithinArea, "xyz", 0x54F2B0, bool(CPlaceable::*)(float, float, float, float, float, float) const);
    RH_ScopedInstall(RemoveMatrix, 0x54F3B0);
    RH_ScopedInstall(AllocateStaticMatrix, 0x54F4C0);
    RH_ScopedInstall(AllocateMatrix, 0x54F560);
    RH_ScopedInstall(SetMatrix, 0x54F610);
    RH_ScopedInstall(GetMatrix, 0x411990);
    RH_ScopedNamedGlobalInstall(PlaceableGetMatrixCopy, "GetMatrixCopy", 0x41BD80); // lote 140A (banco Fn140A)
    RH_ScopedInstall(ShutdownMatrixArray, 0x54EFD0);
    RH_ScopedInstall(InitMatrixArray, 0x54F3A0);
    RH_ScopedInstall(FreeStaticMatrix, 0x54F010);
}

// Ayudantes de X87Math.cpp (PlaceableHeadingDblCpp / PlaceableRollDblCpp) que devuelven un double: el fpatan queda sin redondear en ST0.
// Aqui se declaran como float (alias del enlazador) para que el metodo devuelva ST0 tal cual: el retorno float de una llamada ya llega en ST0 y
// el compilador no lo guarda ni lo recarga, tampoco sin optimizar (se comprueba en el objeto: call, add esp y ret, sin fstp ni fld tras la llamada).
extern "C" float __cdecl PlaceableHeadingFltCpp(const float* forward);
extern "C" float __cdecl PlaceableRollFltCpp(const float* right, float upZ);
#pragma comment(linker, "/alternatename:_PlaceableHeadingFltCpp=_PlaceableHeadingDblCpp")
#pragma comment(linker, "/alternatename:_PlaceableRollFltCpp=_PlaceableRollDblCpp")

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

// 0x41CC70: thiscall, destino oculto en la pila (`ret 4`), devuelve el destino. Con matriz copia en crudo los tres dwords de la fila derecha (+0 de la matriz, sin pasar por el x87);
// sin ella (cos h, sin h, 0) con fsin/fcos del x87 guardados en float (el EXE calcula el seno primero y el coseno despues; z = 0 se escribe primero).
CVector CPlaceable::GetRightVector() {
    CVector out;
    if (m_matrix) {
        std::memcpy(&out, &m_matrix->GetRight(), sizeof(out));
        return out;
    }
    const float heading = m_placement.m_fHeading;
    const float sine = TrigSimpleSinCpp(heading);
    const float cosine = TrigSimpleCosCpp(heading);
    out.z = 0.0f;
    out.x = cosine;
    out.y = sine;
    return out;
}

// 0x41CCB0: thiscall, destino oculto en la pila (`ret 4`), devuelve el destino. Con matriz copia en crudo los tres dwords de la fila adelante (+0x10 de la matriz, sin pasar por el x87);
// sin ella (-sin h, cos h, 0) con fsin/fcos del x87 guardados en float (el EXE calcula el coseno primero).
CVector CPlaceable::GetForwardVector() {
    CVector out;
    if (m_matrix) {
        std::memcpy(&out, &m_matrix->GetForward(), sizeof(out));
        return out;
    }
    const float heading = m_placement.m_fHeading;
    const float cosine = TrigSimpleCosCpp(heading);
    const float sine = TrigSimpleSinCpp(heading);
    out.x = -sine;
    out.y = cosine;
    out.z = 0.0f;
    return out;
}

// 0x50E420: thiscall, destino oculto en la pila (`ret 4`), devuelve el destino. Con matriz copia en crudo los tres dwords de la fila arriba (+0x20 de la matriz, sin pasar por el x87);
// sin ella (0, 0, 1.0f).
CVector CPlaceable::GetUpVector() {
    CVector out;
    if (m_matrix) {
        std::memcpy(&out, &m_matrix->GetUp(), sizeof(out));
        return out;
    }
    out.x = 0.0f;
    out.y = 0.0f;
    out.z = 1.0f;
    return out;
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
float CPlaceable::GetHeading() const {
    if (!m_matrix)
        return m_placement.m_fHeading;
    return PlaceableHeadingFltCpp(&m_matrix->GetForward().x);
}

// 0x420B30
// Como el original: atan2(right.z, ±|right.xy|) con fsqrt (no la magnitud al cuadrado) y fpatan; el signo
// se invierte solo si up.z < 0 (NaN no) y el resultado se devuelve en ST0 sin redondear a float.
float CPlaceable::GetRoll() const {
    if (!m_matrix)
        return 0.0f;
    return PlaceableRollFltCpp(&m_matrix->GetRight().x, m_matrix->GetUp().z);
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
    const float fHeading = TrigAtan2Cpp(-vecForward.x, vecForward.y); // Como el original: fpatan guardado como float

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
            const float fHeading = TrigAtan2Cpp(-vecForward.x, vecForward.y); // Como el original: fpatan guardado como float

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
