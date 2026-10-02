/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FurnitureSubGroup_c
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
#include "FurnitureSubGroup_c.h"
#include "Furniture_c.h"

/**
 * Registra las operaciones de selección y creación de mobiliario de un subgrupo.
 */
void FurnitureSubGroup_c::InjectHooks() {
    RH_ScopedClass(FurnitureSubGroup_c);
    RH_ScopedCategory("Interior");

    RH_ScopedInstall(GetFurniture, 0x590EE0);
    RH_ScopedInstall(GetRandomId, 0x590FD0);
    RH_ScopedInstall(AddFurniture, 0x5C00C0);
}

namespace {
/**
 * Selecciona uniformemente una entrada cuyo intervalo de wealth contiene el valor pedido.
 *
 * Se conserva el consumo de RNG incluso cuando no hay candidatos porque forma parte
 * del comportamiento observable del ejecutable original.
 */
Furniture_c* SelectFurnitureByWealth(TList_c<Furniture_c>& list, uint8 wealth) {
    const auto matches = [wealth](const Furniture_c& f) {
        return wealth >= uint8(f.m_nWealthMin) && wealth <= uint8(f.m_nWealthMax);
    };
    int32 count = 0;
    for (const auto& f : list) if (matches(f)) ++count;
    // The original consumes a random value even when no furniture matches.
    auto index = int32((float(CGeneral::GetRandomNumber()) * (1.0f / 32768.0f)) * float(count));
    for (auto& f : list) if (matches(f) && index-- == 0) return &f;
    return nullptr;
}
}

// 0x5C00C0
/**
 * Reserva el siguiente Furniture_c del almacén global fijo y deriva sus dimensiones
 * desde el ColModel del modelo. Finalmente copia las restricciones del subgrupo y
 * enlaza el objeto en m_Furnitures.
 */
bool FurnitureSubGroup_c::AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng) {
        // Esta función sólo lee/escribe la palabra baja del contador global original.
    auto& next = StaticRef<uint16>(0xBAB378);
    auto& store = StaticRef<Furniture_c[256]>(0xBAE1F8);
    if (next >= std::size(store)) return false;
    auto& f = store[next++];
    f.m_nModelId = int16(modelId);
    f.m_nId = id;
    const auto& box = CModelInfo::GetModelInfo(modelId)->GetColModel()->m_boundBox;
    const auto size = m_bCanSteal ? box.m_vecMax - box.m_vecMin : box.m_vecMax + CVector{0.5f, 0.5f, 0.5f};
    const auto dimension = [](float value) {
        const auto whole = int32(value);
        return int8(whole + (value - float(whole) >= 0.02f ? 1 : 0));
    };
    f.m_nWidthX = dimension(size.x);
    f.m_nWidthY = dimension(size.y);
    f.m_nWealthMin = int8(wealthMin);
    f.m_nWealthMax = int8(wealthMax);
    f.m_nMaxAng = int8(maxAng);
    f.m_bCanPlaceInFrontOfWindow = m_bCanPlaceInFrontOfWindow;
    f.m_bIsTall = m_bIsTall;
    f.m_bCanSteal = m_bCanSteal;
    m_Furnitures.AddItem(&f);
    return true;
}

// 0x590EE0
/**
 * Con un id explícito busca coincidencia exacta; con id negativo selecciona una
 * variante compatible con wealth mediante la misma ruta aleatoria del juego.
 */
Furniture_c* FurnitureSubGroup_c::GetFurniture(int16 id, uint8 wealth) {
    if (id >= 0) {
        for (auto& f : m_Furnitures) if (f.m_nId == id) return &f;
        return nullptr;
    }
    // El original extiende wealth sin signo antes de compararlo con -1; por ello 255
    // sigue el filtro normal de riqueza y no funciona como comodín.
    return SelectFurnitureByWealth(m_Furnitures, wealth);
}

// 0x590FD0
int32 FurnitureSubGroup_c::GetRandomId(uint8 wealth) {
    const auto* f = SelectFurnitureByWealth(m_Furnitures, wealth);
    return f ? int32(f->m_nId) : -1;
}

void FurnitureSubGroup_c::Exit() {
    m_Furnitures.RemoveAll();
}
