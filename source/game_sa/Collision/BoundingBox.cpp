/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBoundingBox
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "BoundingBox.h"

void CBoundingBox::InjectHooks() {
    RH_ScopedClass(CBoundingBox);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(IsPointWithin, 0x49C300); // Lote 116A, banco Fn116A
    RH_ScopedInstall(Constructor, 0x40FAF0);   // Lote 130A, banco Fn130A
}

// 0x40FAF0 (lote 130A, banco Fn130A): thiscall sin argumentos, `ret`, 96 bytes. Como el original: escribe los seis dwords del cuadro (1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f; el EXE los pasa por la pila,
// sin x87) y devuelve this en EAX. No toca nada mas.
CBoundingBox* CBoundingBox::Constructor() {
    auto* const raw = reinterpret_cast<uint32*>(this);
    raw[0] = 0x3F800000u;
    raw[1] = 0x3F800000u;
    raw[2] = 0x3F800000u;
    raw[3] = 0xBF800000u;
    raw[4] = 0xBF800000u;
    raw[5] = 0xBF800000u;
    return this;
}

// 0x49C300
bool CBoundingBox::IsPointWithin(const CVector& point) const {
    return point.x >= m_vecMin.x
        && point.y >= m_vecMin.y
        && point.z >= m_vecMin.z
        && point.x <= m_vecMax.x
        && point.y <= m_vecMax.y
        && point.z <= m_vecMax.z;
}

// 0x470100
void CBoundingBox::SetMinMax(CVector min, CVector max) {
    m_vecMin = min;
    m_vecMax = max;
}
