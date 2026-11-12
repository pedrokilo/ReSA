/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPointList
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

void CPointList::InjectHooks() {
    RH_ScopedClass(CPointList);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x6986E0);
    RH_ScopedInstall(Clear, 0x699F00); // lote 173A (banco Fn173A)
}

// 0x6986E0 (lote 142A, banco Fn142A; thiscall, sin argumentos de pila, `ret`, 12 bytes: `push esi; mov esi,ecx; call 0x699F00; mov eax,esi; pop esi; ret`; el listado lo atribuia a
// CTaskComplexReactToGunAimedAt). El EXE llama a 0x699F00 (sin hook), que pone m_Count (+0) a cero y los 24 bytes de m_PointHasBeenClaimed (+0x124, 6 dwords) a cero, y devuelve this en EAX. Los puntos
// (m_Points) no se tocan. Aqui las dos escrituras de 0x699F00 van en linea: las mismas, en memoria el mismo resultado.
CPointList* CPointList::Constructor() {
    m_Count = 0;
    std::fill(std::begin(m_PointHasBeenClaimed), std::end(m_PointHasBeenClaimed), false);
    return this;
}

// 0x699F00 (lote 173A, banco Fn173A; thiscall, sin argumentos, `ret`, 24 bytes: `push edi; mov [ecx],0; lea edi,[ecx+124h]; mov ecx,6; xor eax,eax; rep stosd; pop edi; ret`; el listado
// decia CPointList): pone m_Count a cero y los 24 bytes de m_PointHasBeenClaimed (+0x124, 6 dwords) a cero. Los puntos (m_Points) no se tocan. Es la funcion que llama el
// constructor 0x6986E0 del EXE (que aqui las escribe en linea).
void CPointList::Clear() {
    m_Count = 0;
    std::fill(std::begin(m_PointHasBeenClaimed), std::end(m_PointHasBeenClaimed), false);
}

bool CPointList::Empty() const noexcept {
    return m_Count == 0;
}

void CPointList::AddPoint(CVector pt) noexcept {
    if (m_Count >= m_Points.size()) {
        return;
    }
    m_Points[m_Count++] = pt;
}
