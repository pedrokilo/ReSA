#include "StdInc.h"

void CPointList::InjectHooks() {
    RH_ScopedClass(CPointList);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x6986E0);
}

// 0x6986E0 (lote 142A, banco Fn142A; thiscall, sin argumentos de pila, `ret`, 12 bytes: `push esi; mov esi,ecx; call 0x699F00; mov eax,esi; pop esi; ret`; el listado lo atribuia a
// CTaskComplexReactToGunAimedAt). El EXE llama a 0x699F00 (sin hook), que pone m_Count (+0) a cero y los 24 bytes de m_PointHasBeenClaimed (+0x124, 6 dwords) a cero, y devuelve this en EAX. Los puntos
// (m_Points) no se tocan. Aqui las dos escrituras de 0x699F00 van en linea: las mismas, en memoria el mismo resultado.
CPointList* CPointList::Constructor() {
    m_Count = 0;
    std::fill(std::begin(m_PointHasBeenClaimed), std::end(m_PointHasBeenClaimed), false);
    return this;
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
