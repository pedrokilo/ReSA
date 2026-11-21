/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoAttractLine_c
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxInfoAttractLine.h"
#include "X87Math.h"

void FindClosestPtOnLine(CVector& out, CVector& lineA, CVector& lineB, CVector& point);

void FxInfoAttractLine_c::InjectHooks() {
    RH_ScopedClass(FxInfoAttractLine_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A57C0); // lote 215A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A7020);
    RH_ScopedInstall(DestructorBody, 0x4A7040);
    RH_ScopedGlobalInstall(FindClosestPtOnLine, 0x4A4880); // lote S2A (sin validar)
}

// 0x4A57C0
FxInfoAttractLine_c::FxInfoAttractLine_c() { // la base FxInfo_c por defecto (sin `()`): el EXE no escribe +6 ni +7
    m_nType = FX_INFO_ATTRACTLINE_DATA;
    m_InterpInfo.Allocate(7);
}

// 0x4A5830
void FxInfoAttractLine_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4880 (lote S2A, sin validar; cdecl, cuatro referencias, 185 bytes): punto de la recta A-B mas cercano a `point`. Aritmetica de la x87 a 24 bits con la pila sin guardar (Veh83D*):
// d = B - A por componentes (dx tambien se guarda como float: se usa asi en el producto escalar y en la salida); dot = ((p.z - a.z) * dz + (p.y - a.y) * dy) + (p.x - a.x) * dxF;
// t = dot / ((dz*dz + dy*dy) + dx*dx); t < 0 -> 0 (ordenado); t > 1 -> 1 (ordenado; con NaN no se toca); salida = d * t + A por componente (x, y, z en ese orden y cada resta de B - A
// se recalcula leyendo la memoria en ese momento: con `out` alias de A o B las lecturas de y y z ya ven la escritura anterior solo en su propio componente).
void FindClosestPtOnLine(CVector& out, CVector& lineA, CVector& lineB, CVector& point) {
    const double dx = Veh83DSub(lineB.x, lineA.x);
    const float  dxStored = Veh83DStore(dx);
    const double dy = Veh83DSub(lineB.y, lineA.y);
    const double dz = Veh83DSub(lineB.z, lineA.z);

    const double termZ = Veh83DMul(Veh83DSub(point.z, lineA.z), Veh83DSub(lineB.z, lineA.z));
    const double termY = Veh83DMul(Veh83DSub(point.y, lineA.y), Veh83DSub(lineB.y, lineA.y));
    const double dot   = Veh83DAdd(Veh83DAdd(termZ, termY), Veh83DMul(Veh83DSub(point.x, lineA.x), dxStored));
    const double sumSq = Veh83DAdd(Veh83DAdd(Veh83DMul(dz, dz), Veh83DMul(dy, dy)), Veh83DMul(dx, dx));
    double t = Veh83DDiv(dot, sumSq);
    if (t < 0.0) {
        t = 0.0;
    } else if (t > 1.0) {
        t = 1.0;
    }

    out.x = Veh83DStore(Veh83DAdd(Veh83DMul(dxStored, t), lineA.x));
    out.y = Veh83DStore(Veh83DAdd(Veh83DMul(Veh83DSub(lineB.y, lineA.y), t), lineA.y));
    out.z = Veh83DStore(Veh83DAdd(Veh83DMul(Veh83DSub(lineB.z, lineA.z), t), lineA.z));
}

// 0x4A5850
void FxInfoAttractLine_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) {
    float values[7];
    if (m_bTimeModeParticle) {
        m_InterpInfo.GetVal(values, mult);
    } else {
        m_InterpInfo.GetVal(values, currentTime / length);
    }

    CVector out;
    auto& lineA = *(CVector*)&values[0];
    auto& lineB = *(CVector*)&values[3];

    auto& movement = *static_cast<MovementInfo_t*>(info);
    FindClosestPtOnLine(out, lineA, lineB, movement.m_Pos);
    CVector in = out - movement.m_Pos;
    auto dist = in.Magnitude();

    if (dist > 0.0f) {
        in.Normalise();
    }

    if (dist > 0.1f) {
        movement.m_Vel += in * values[6] * totalTime;
    }
}


// 0x4A7040 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoAttractLine_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfo32_c, 0x4A89B0: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoAttractLine_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A7020 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoAttractLine_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A7040; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoAttractLine_c* FxInfoAttractLine_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
