/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAttractorScanner
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Este archivo se compila completo con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt):
// el EXE (0x5FFF90) escribe solo EAX, EDX y ESI (guardado), conserva ECX y calcula el cuadrado con x87; su llamador
// en 0x6034A7 devuelve ECX tras la llamada (mov eax, ecx).
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"
#include "DatosExe/RdataConstantes.h"

#include "AttractorScanner.h"
#include "Entity/Ped/Ped.h"
#include "Events/EventAttractor.h"
#include "Events/EventScriptedAttractor.h"
#include "Scripts/Scripted2dEffects.h"
#include "World.h"
#include "Timer.h"
#include "Pools/Pools.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "PedAttractorManager.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "Weather.h"
#include "Entity/Entity.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// 0x5FFF90
uint64 CAttractorScanner::Clear() {
    // 0x86C52C: distancia base; el cuadrado se calcula con la FPU
    const auto& baseDistance = DatosExe::Rdata::k86C52C;
    for (int32 i = 0; i < 10; i++) {
        field_18[i] = 0;
        field_40[i] = 0;
        float distance = baseDistance * baseDistance;
        // Como el original: las posiciones 4 y 7 conservan el cuadrado; el resto recibe 25.0f (0x41C80000)
        if (i != 4 && i != 7) {
            distance = 25.0f;
        }
        field_68[i] = std::bit_cast<int32>(distance);
    }
    // El EXE sale del bucle con EAX apuntando tras field_68 y EDX = 10; se devuelven en EDX:EAX (ver AttractorScanner.h).
    return (uint64{10} << 32) | static_cast<uint32>(reinterpret_cast<uintptr_t>(&field_68[10]));
}

namespace {
// Vista de una lista de punteros del juego (CPtrListSingleLink / CPtrListDoubleLink): el objeto de la lista empieza por el puntero al primer nodo y cada nodo por el elemento y el siguiente.
struct tPtrNodeView {
    CEntity*      Item;
    tPtrNodeView* Next;
};
struct tPtrListView {
    tPtrNodeView* First;
};
} // namespace

// Celda del mapa de sectores de una coordenada: ftol(floor((coordenada -+ 15.0) * 0.02 + 60.0)) como el EXE (0x6060A0..0x6062FE): fld coordenada, fsub/fadd 15.0 (0x86C52C), fmul 0.02 (0x858B38),
// fadd 60.0 (0x858B34) a 24 bits (cada operacion se redondea a float), fstp qword, floor del CRT (0x8219F0) y _ftol2 (0x821B40): palabra baja de la conversion de 64 bits (0 con NaN, infinito o |valor| >= 2^63).
// Los productos y sumas se guardan en volatile para que /fp:fast no los reasocie (el EXE no distribuye el producto).
static __declspec(noinline) int32 AttractorSectorCell(float coordinate, bool useEnd) {
    volatile float edge   = useEnd ? 15.0f + coordinate : coordinate - 15.0f;
    volatile float scaled = edge * 0.02f;
    volatile float sector = scaled + 60.0f;
    const double   cell   = std::floor(double(sector));
    if (((std::bit_cast<uint64>(cell) >> 52) & 0x7FF) >= 0x43E) {
        return 0;
    }
    return static_cast<int32>(static_cast<int64>(cell));
}

// 0x6002F0 (lote 203A, sin validar; el listado decia CAttractorScanner; la usan dos comandos de script del EXE, 0x474D10 y 0x476993; cdecl, nueve argumentos de pila, el llamador limpia, `ret`, 480 bytes; no usa `this`): busca, entre los edificios y los
// objetos a menos de `radio` de `punto`, el efecto 2D de atractor mas cercano que cumpla los filtros y devuelve si encontro alguno. Argumentos en orden de pila: 1 punto (puntero a CVector), 2 radio (float), 3 indice de modelo (-1 = cualquiera), 4 tipo de atractor
// (se compara el BYTE +0x34 del efecto, ampliado a cero, con los 32 bits del argumento), 5 nombre de script (nulo = cualquiera), 6 exigir hueco libre (BYTE), 7 efecto a excluir (nulo = ninguno), 8 salida: efecto, 9 salida: entidad.
// Orden del EXE: 1) *efecto y *entidad a nulo (en ese orden) ANTES de buscar; 2) CWorld::FindObjectsInRange (0x564A20, con hook) con (punto, radio, false, &contador (que arranca en 0xFFFF), 0x400, lista local, edificios = true, vehiculos = false, peatones = false,
// objetos = true, ficticios = false); 3) por cada entidad de la lista (el contador, un int16 CON SIGNO, se relee en cada vuelta): si el indice de modelo pedido no es -1 y no coincide con el de la entidad (+0x22 con signo de 16 bits) se salta; se toma el
// modelo de CModelInfo::ms_modelInfoPtrs[indice con signo] SIN comprobar nulo; para cada efecto (0 .. +0xD del modelo, un BYTE que se relee en cada vuelta, CBaseModelInfo::Get2dEffect 0x4C4C70): se salta si es el excluido, si su tipo (BYTE +0xC) no es 3 (atractor),
// si el tipo de atractor no coincide, si hay nombre y _stricmp (0x8229B6) con el nombre del efecto (+0x38) no da cero, o si se exige hueco libre y CPedAttractorManager::HasEmptySlot (0x5EBB00, con GetPedAttractorManager 0x5EE190) con (efecto, entidad) es falso;
// 4) posicion del efecto en el mundo (CEntity::TransformFromObjectSpace 0x5334F0, con hook) y distancia al cuadrado ((dz*dz + dy*dy) + dx*dx, con d = punto menos efecto, x87 a 24 bits) contra la mejor (empieza en 1e8 = 0x4CBEBC20): solo si es ESTRICTAMENTE
// menor (comparacion ordenada: con NaN no entra) guarda la distancia y escribe efecto y entidad en las salidas.
#pragma float_control(precise, on, push)
// 0x6002F0
static bool FindAttractorInRange(const CVector& point, float radius, int32 modelIndex, int32 attractorType, const char* name, bool requireFreeSlot, C2dEffectPedAttractor* exclude, C2dEffectPedAttractor** outEffect, CEntity** outEntity) {
    *outEffect = nullptr;
    *outEntity = nullptr;

    CEntity* entities[1024];
    int16    count = -1;
    CWorld::FindObjectsInRange(point, radius, false, &count, 1024, entities, true, false, false, true, false);

    bool  found          = false;
    float bestDistanceSq = std::bit_cast<float>(0x4CBEBC20u);
    for (int32 i = 0; i < count; i++) {
        CEntity* const entity      = entities[i];
        const int32    entityModel = static_cast<int16>(entity->m_nModelIndex);
        if (modelIndex != -1 && modelIndex != entityModel) {
            continue;
        }

        const CBaseModelInfo* const modelInfo = CModelInfo::ms_modelInfoPtrs[entityModel];
        for (int32 j = 0; j < modelInfo->m_n2dfxCount; j++) {
            C2dEffect* const effect    = modelInfo->Get2dEffect(j);
            auto* const      attractor = reinterpret_cast<C2dEffectPedAttractor*>(effect);
            if (exclude && exclude == attractor) {
                continue;
            }
            if (*reinterpret_cast<const uint8*>(&effect->m_Type) != 3) { // BYTE +0xC (EFFECT_ATTRACTOR)
                continue;
            }
            if (static_cast<int32>(*reinterpret_cast<const uint8*>(&attractor->m_nAttractorType)) != attractorType) { // BYTE +0x34 ampliado a cero
                continue;
            }
            if (name && _stricmp(name, attractor->m_szScriptName) != 0) {
                continue;
            }
            if (requireFreeSlot && !GetPedAttractorManager()->HasEmptySlot(attractor, entity)) {
                continue;
            }

            const CVector effectPos = entity->TransformFromObjectSpace(attractor->m_Pos);
            const float dx = point.x - effectPos.x;
            const float dy = point.y - effectPos.y;
            const float dz = point.z - effectPos.z;
            const float distanceSq = (dz * dz + dy * dy) + dx * dx;
            if (distanceSq < bestDistanceSq) {
                bestDistanceSq = distanceSq;
                *outEffect     = attractor;
                *outEntity     = entity;
                found          = true;
            }
        }
    }
    return found;
}
#pragma float_control(pop)

// 0x6034B0 (lote 198A, sin validar; thiscall, dos argumentos de pila (la lista y el peaton), `ret 8`, 181 bytes; el EXE conserva EBX/EBP/ESI/EDI): recorre la lista; el siguiente nodo se lee ANTES de procesar la entidad. Por cada entidad:
//  - Si es un objeto (tipo 4 en los 3 bits bajos del byte +0x36): `habilitarDesactivados` = bit 24 de las banderas del objeto (+0x140); se descarta si NO es estatica (bit 2 de +0x1C) NI estatica esperando colision (bit 18) y tambien si esta explotado (bit 6 de +0x140).
//    `habilitarDesactivados` (BL) solo se actualiza con objetos y se conserva de una entidad a la siguiente (los edificios usan el valor del ultimo objeto, 0 al empezar).
//  - Modelo = CModelInfo::ms_modelInfoPtrs[indice con signo de 16 bits de +0x22] (sin comprobar nulo). Para i de 0 a n2dfx-1 (el contador, byte +0xD, se relee en cada vuelta; sin efectos no entra): CBaseModelInfo::Get2dEffect (0x4C4C70); si es de tipo
//    atractor (3) y (bit 0 de +0x37 apagado o `habilitarDesactivados`), llama a 0x5FFFD0(efecto, entidad, peaton) (por direccion).
void CAttractorScanner::ScanEntityList(const void* list, CPed* ped) {
    bool enableDisabledAttractors = false;
    const tPtrNodeView* node = static_cast<const tPtrListView*>(list)->First;
    while (node) {
        CEntity* const entity = node->Item;
        node = node->Next;

        if (entity->GetType() == ENTITY_TYPE_OBJECT) {
            const auto* const object = static_cast<const CObject*>(entity);
            enableDisabledAttractors = object->objectFlags.bEnableDisabledAttractors;
            if (!entity->m_bIsStatic && !entity->m_bIsStaticWaitingForCollision) {
                continue;
            }
            if (object->objectFlags.bIsExploded) {
                continue;
            }
        }

        const CBaseModelInfo* const modelInfo = CModelInfo::ms_modelInfoPtrs[static_cast<int16>(entity->m_nModelIndex)];
        for (int32 i = 0; i < modelInfo->m_n2dfxCount; i++) {
            C2dEffect* const effect = modelInfo->Get2dEffect(i);
            if (effect->m_Type != EFFECT_ATTRACTOR) {
                continue;
            }
            if ((effect->pedAttractor.m_nFlags & 1) && !enableDisabledAttractors) {
                continue;
            }
            ScanEffect(reinterpret_cast<C2dEffectPedAttractor*>(effect), entity, ped);
        }
    }
}

// Orden de operaciones del EXE (0x60361B..0x60363B, x87 a 24 bits): dx, dy y dz se calculan como (efecto - peaton) y la distancia al cuadrado como ((dz*dz + dy*dy) + dx*dx), cada producto y suma redondeados a float, sin reordenar.
#pragma float_control(precise, on, push)
// 0x603570 (lote 198A, sin validar; cdecl, un argumento de pila (el efecto), `ret` sin limpiar, 422 bytes con SEH que solo destruye la matriz local y aqui se omite): recorre el pool de peatones del ultimo indice al primero; salta los huecos libres (bit 7 del byte del
// mapa) y a los que tienen la tarea activa TASK_COMPLEX_USE_EFFECT (CTaskManager::GetActiveTask 0x681720). Distancia al cuadrado ((dz*dz + dy*dy) + dx*dx) entre el efecto y la posicion del peaton (matriz +0x30 o, sin matriz, +4), guardada como float; solo
// sigue si es ESTRICTAMENTE menor (comparacion ordenada) que la mejor (FLT_MAX = 0x7F7FFFFF al empezar). Lista de usuarios del efecto (CScripted2dEffects::GetIndex 0x6F9F60, tabla de 0x24 bytes en 0xC3A200): sin lista (byte +0x20 a cero) vale cualquiera; con lista,
// el modelo del peaton (con signo de 16 bits) debe estar en las cuatro primeras entradas o el tipo de peaton (+0x598) en el par -2/tipo (la logica de IsPedTypeAllowed 0x5FE960). Despues, con una matriz local (+0x40 y +0x44 a cero y CMatrix::SetScale(1.0f) 0x59AED0),
// CPedAttractorManager::IsApproachable (0x5EA220, cdecl, efecto, matriz, 0, peaton): si es cierto, mejor = esta distancia y este peaton. Devuelve el mejor peaton (nulo si ninguno).
CPed* CAttractorScanner::FindClosestPedForEffect(C2dEffectPedAttractor* effect) {
    float bestDistanceSq = std::bit_cast<float>(0x7F7FFFFFu);
    CPed* bestPed = nullptr;

    auto* const pool = GetPedPool();
    for (int32 i = pool->GetSize(); i > 0;) {
        i--;
        CPed* const ped = pool->GetAt(i);
        if (!ped) {
            continue;
        }

        const CTask* const activeTask = ped->GetIntelligence()->GetTaskManager().GetActiveTask();
        if (activeTask && activeTask->GetTaskType() == TASK_COMPLEX_USE_EFFECT) {
            continue;
        }

        const CVector& pedPos = ped->GetPosition();
        const float dx = effect->m_Pos.x - pedPos.x;
        const float dy = effect->m_Pos.y - pedPos.y;
        const float dz = effect->m_Pos.z - pedPos.z;
        const float distanceSq = (dz * dz + dy * dy) + dx * dx;
        if (!(distanceSq < bestDistanceSq)) {
            continue;
        }

        const tUserList& userList = CScripted2dEffects::ms_userLists[CScripted2dEffects::GetIndex(effect)];
        if (*reinterpret_cast<const uint8*>(&userList.m_bUseList)) { // byte (puede traer basura distinta de 0 y 1)
            const int32 model = static_cast<int16>(ped->m_nModelIndex);
            bool allowed = false;
            for (size_t j = 0; j < userList.m_UserTypes.size(); j++) {
                if (model == userList.m_UserTypes[j]) {
                    allowed = true;
                    break;
                }
            }
            if (!allowed) {
                for (size_t j = 0; j < userList.m_UserTypes.size(); j++) {
                    if (userList.m_UserTypes[j] == -2 && userList.m_UserTypesByPedType[j] == static_cast<int32>(ped->m_nPedType)) {
                        allowed = true;
                        break;
                    }
                }
            }
            if (!allowed) {
                continue;
            }
        }

        CMatrix matrix{}; // el EXE solo escribe +0x40 y +0x44 (a cero) y SetScale(1.0f)
        matrix.SetScale(1.0f);
        if (CPedAttractorManager::IsApproachable(effect, matrix, 0, ped)) {
            bestDistanceSq = distanceSq;
            bestPed = ped;
        }
    }
    return bestPed;
}
#pragma float_control(pop)

// 0x5FFFD0 (lote 202A, sin validar; thiscall, tres argumentos de pila (efecto, entidad, peaton), `ret 0Ch`, 429 bytes con marco SEH que solo destruye la matriz local; el EXE conserva EBX/EBP/ESI/EDI).
// Orden del EXE (x87 a 24 bits, cada producto y suma redondeados a float, sin reordenar):
//  1. Lluvia: con CWeather::Rain (0xC81324) MENOR o no ordenado con 0.2 (0x858CC4) solo sigue si el tipo (BYTE +0x34 del efecto, ampliado a cero) NO es 4; con lluvia mayor o igual solo sigue si el tipo es 4.
//  2. Posicion del efecto: con entidad, CEntity::TransformFromObjectSpace (0x5334F0, con hook) de +0 del efecto; sin entidad, la copia tal cual.
//  3. Distancia al cuadrado ((dz*dz + dy*dy) + dx*dx), con d = posicion del peaton (matriz +0x30 o, sin matriz, +4) menos la del efecto, guardada como float; si NO es estrictamente menor (comparacion ordenada) que field_68[tipo] sale. El indice no se acota.
//  4. Tipo 7 (BYTE +0x34 igual a 7): radio = CScripted2dEffects::ms_radii[GetIndex(efecto)] (0x6F9F60 y tabla 0xC39EE0); con radio menor que 0 no hay limite; si no, sale cuando la distancia no es estrictamente menor que radio*radio (con NaN sale).
//  5. CPedAttractorManager::GetPedAttractorManager (0x5EE190) y HasEmptySlot (0x5EBB00, efecto y entidad): si es falso sale SIN construir la matriz.
//  6. Matriz local (el EXE solo pone a cero +0x40 y +0x44): con entidad, operator= (0x59BBC0) desde CPlaceable::GetMatrix (0x411990) de la entidad; sin entidad, SetScale(1.0f) (0x59AED0). CPedAttractorManager::IsApproachable (0x5EA220) con (efecto, matriz, 0, peaton):
//     si es cierto guarda field_18[tipo] = entidad, field_68[tipo] = distancia y field_40[tipo] = efecto (en ese orden en el EXE). Despues destruye la matriz (0x59ACD0) por todas las salidas posteriores a su construccion.
#pragma float_control(precise, on, push)
// 0x5FFFD0
void CAttractorScanner::ScanEffect(C2dEffectPedAttractor* effect, CEntity* entity, CPed* ped) {
    const uint32 type = static_cast<uint8>(effect->m_nAttractorType);

    // Como el original: `fcomp` + test ah,1 (C0): menor o no ordenado = no llueve lo bastante
    if (!(CWeather::Rain >= std::bit_cast<float>(0x3E4CCCCDu))) { // 0x858CC4 (0.2f)
        if (type == 4) {
            return;
        }
    } else if (type != 4) {
        return;
    }

    CVector effectPos;
    if (entity) {
        effectPos = entity->TransformFromObjectSpace(effect->m_Pos);
    } else {
        effectPos = effect->m_Pos;
    }

    const CVector& pedPos = ped->GetPosition();
    const float dx = pedPos.x - effectPos.x;
    const float dy = pedPos.y - effectPos.y;
    const float dz = pedPos.z - effectPos.z;
    const float distanceSq = (dz * dz + dy * dy) + dx * dx;

    // Como el original: tipo sin acotar (el EXE indexa los tres arrays con el BYTE del tipo)
    uint8* const base = reinterpret_cast<uint8*>(this);
    int32& slotEntity   = *reinterpret_cast<int32*>(base + 0x18 + type * 4u);
    int32& slotEffect   = *reinterpret_cast<int32*>(base + 0x40 + type * 4u);
    int32& slotDistance = *reinterpret_cast<int32*>(base + 0x68 + type * 4u);
    if (!(distanceSq < std::bit_cast<float>(slotDistance))) {
        return;
    }

    if (type == 7) {
        const float radius = CScripted2dEffects::ms_radii.data()[CScripted2dEffects::GetIndex(effect)];
        if (!(radius < 0.0f) && !(distanceSq < radius * radius)) {
            return;
        }
    }

    if (!GetPedAttractorManager()->HasEmptySlot(effect, entity)) {
        return;
    }

    CMatrix matrix{};
    if (entity) {
        matrix = entity->GetMatrix();
    } else {
        matrix.SetScale(1.0f);
    }
    if (CPedAttractorManager::IsApproachable(effect, matrix, 0, ped)) {
        slotEntity   = static_cast<int32>(reinterpret_cast<uintptr_t>(entity));
        slotDistance = std::bit_cast<int32>(distanceSq);
        slotEffect   = static_cast<int32>(reinterpret_cast<uintptr_t>(effect));
    }
}

// 0x600180 (lote 202A, sin validar; thiscall, dos argumentos de pila (punteros de salida), `ret 8`, 364 bytes): primero deja *effect y *entity a nulo (en ese orden). Si field_18[4] (entidad del tipo 4, refugio) no es nulo, *effect = field_40[4], *entity = field_18[4] y sale.
// Si no, mejor = FLT_MAX (0x863A3C) y para i de 0 a 9: si field_68[i] es ESTRICTAMENTE menor que `mejor` (el primero compara FLT_MAX con field_68[0] y los demas el mejor ya elegido; comparacion ordenada: con NaN no entra) Y field_40[i] no es nulo,
// mejor = field_68[i], *effect = field_40[i] y *entity = field_18[i].
void CAttractorScanner::GetBestEffect(C2dEffectPedAttractor** outEffect, CEntity** outEntity) {
    *outEffect = nullptr;
    *outEntity = nullptr;
    if (field_18[4]) {
        *outEffect = reinterpret_cast<C2dEffectPedAttractor*>(static_cast<uintptr_t>(static_cast<uint32>(field_40[4])));
        *outEntity = reinterpret_cast<CEntity*>(static_cast<uintptr_t>(static_cast<uint32>(field_18[4])));
        return;
    }
    float best = std::bit_cast<float>(0x7F7FFFFFu); // 0x863A3C: FLT_MAX
    for (int32 i = 0; i < 10; i++) {
        const float distance = std::bit_cast<float>(field_68[i]);
        if (best > distance && field_40[i]) {
            best       = distance;
            *outEffect = reinterpret_cast<C2dEffectPedAttractor*>(static_cast<uintptr_t>(static_cast<uint32>(field_40[i])));
            *outEntity = reinterpret_cast<CEntity*>(static_cast<uintptr_t>(static_cast<uint32>(field_18[i])));
        }
    }
}
#pragma float_control(pop)

// 0x6060A0
void CAttractorScanner::ScanForAttractors(CPed& ped) {
    // Como el original: escaner apagado (byte +0) o peaton en vehiculo (bit 8 de +0x46C) no hacen nada
    if (!field_0) {
        return;
    }
    if (ped.bInVehicle) {
        return;
    }

    // Con la tarea de usar un efecto (233) activa se aplaza el escaneo 3000 ms (inicio = hora, arrancado = 1, sin tocar el detenido)
    CTask* const active = ped.GetIntelligence()->GetTaskManager().GetActiveTask();
    if (active && active->GetTaskType() == TASK_COMPLEX_USE_EFFECT) {
        field_4.m_nStartTime = CTimer::GetTimeInMS();
        field_4.m_nInterval  = 3000;
        field_4.m_bStarted   = true;
        return;
    }

    // Como el original: CTaskTimer en linea. Si no estaba arrancado se arranca con 1500 ms (y se sigue); si estaba detenido se reinicia el inicio;
    // si ya vencio (unsigned: hora >= inicio + intervalo) se rearranca con 1500 ms y se escanea. Los booleanos se leen como bytes (!= 0).
    uint32 now = CTimer::GetTimeInMS();
    if (!field_4.m_bStarted) {
        field_4.m_nStartTime = now;
        field_4.m_nInterval  = 1500;
        field_4.m_bStarted   = true;
    }
    if (field_4.m_bStopped) {
        field_4.m_nStartTime = CTimer::GetTimeInMS();
        field_4.m_bStopped   = false;
    }
    now = CTimer::GetTimeInMS();
    if (!(now >= field_4.m_nStartTime + static_cast<uint32>(field_4.m_nInterval))) {
        return;
    }
    field_4.m_nStartTime = now;
    field_4.m_nInterval  = 1500;
    field_4.m_bStarted   = true;

    Clear();

    // Ventana de sectores alrededor del peaton (+-15.0): los minimos se acotan por abajo a 0 (con signo) y los maximos por arriba a 119 (0x77)
    const CVector& position = ped.GetPosition();
    const int32 cellMinX = AttractorSectorCell(position.x, false);
    const int32 minX     = cellMinX > 0 ? cellMinX : 0;
    const int32 cellMinY = AttractorSectorCell(position.y, false);
    const int32 minY     = cellMinY > 0 ? cellMinY : 0;
    const int32 cellMaxX = AttractorSectorCell(position.x, true);
    const int32 maxX     = cellMaxX < 119 ? cellMaxX : 119;
    const int32 cellMaxY = AttractorSectorCell(position.y, true);
    const int32 maxY     = cellMaxY < 119 ? cellMaxY : 119;

    // Por cada celda: lista de edificios del sector (0x6034B0) y lista de objetos del sector repetido (indice con los 4 bajos de cada coordenada); indices acotados a [0, 119] como el EXE
    auto* const sectors       = reinterpret_cast<CSector*>(&CWorld::ms_aSectors);
    auto* const repeatSectors = reinterpret_cast<CRepeatSector*>(&CWorld::ms_aRepeatSectors);
    for (int32 y = minY; y <= maxY; y++) {
        for (int32 x = minX; x <= maxX; x++) {
            const int32 clampedY = std::min(std::max(y, 0), 119);
            const int32 clampedX = std::min(std::max(x, 0), 119);
            ScanEntityList(&sectors[clampedY * MAX_SECTORS_X + clampedX].Buildings, &ped);          // 0x6034B0
            ScanEntityList(&repeatSectors[((y & 15) << 4) + (x & 15)].Objects, &ped);               // 0x6034B0
        }
    }

    // Efectos de script activados: con lista de usuarios, solo si el modelo del peaton (con signo) esta entre los cuatro modelos o el tipo de peaton lo admite (0x5FE960); 0x5FFFD0 los escanea
    for (size_t i = 0; i < CScripted2dEffects::ms_effects.size(); i++) {
        if (!reinterpret_cast<const uint8*>(CScripted2dEffects::ms_activated.data())[i]) {   // byte (puede traer basura distinta de 0 y 1)
            continue;
        }
        tUserList& userList = CScripted2dEffects::ms_userLists[i];
        if (*reinterpret_cast<uint8*>(&userList.m_bUseList)) {
            const int32 model = static_cast<int16>(ped.m_nModelIndex);
            bool listed = false;
            for (size_t j = 0; j < userList.m_UserTypes.size(); j++) {
                if (model == userList.m_UserTypes[j]) {
                    listed = true;
                    break;
                }
            }
            if (!listed && !userList.IsPedTypeAllowed(ped.m_nPedType)) {
                continue;
            }
        }
        ScanEffect(reinterpret_cast<C2dEffectPedAttractor*>(&CScripted2dEffects::ms_effects[i]), nullptr, &ped); // 0x5FFFD0
    }

    // Mejor candidato (0x600180: efecto y entidad por referencia). Sin efecto, o el mismo efecto y entidad que ya se usan, no hay evento
    C2dEffectPedAttractor* effect = nullptr;
    CEntity*               entity = nullptr;
    GetBestEffect(&effect, &entity); // 0x600180
    if (!effect) {
        return;
    }
    const int32 entityValue = static_cast<int32>(reinterpret_cast<uintptr_t>(entity));
    if (effect == reinterpret_cast<C2dEffectPedAttractor*>(m_pEffectInUse) && field_14 == entityValue) {
        return;
    }
    // Tipo 7 (PED_ATTRACTOR_SCRIPTED): solo si 0x603570 devuelve este mismo peaton, con CEventScriptedAttractor; el resto, CEventAttractor. El efecto en uso solo cambia si el grupo acepta el evento
    if (effect->m_nAttractorType == PED_ATTRACTOR_SCRIPTED) {
        if (FindClosestPedForEffect(effect) != &ped) { // 0x603570
            return;
        }
        CEventScriptedAttractor event(effect, entity);
        if (ped.GetIntelligence()->GetEventGroup().Add(&event, false)) {
            field_14       = entityValue;
            m_pEffectInUse = reinterpret_cast<C2dEffect*>(effect);
        }
        return;
    }
    CEventAttractor event(effect, entity, false);
    if (ped.GetIntelligence()->GetEventGroup().Add(&event, false)) {
        field_14       = entityValue;
        m_pEffectInUse = reinterpret_cast<C2dEffect*>(effect);
    }
}


void CAttractorScanner::InjectHooks() {
    RH_ScopedClass(CAttractorScanner);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(Clear, 0x5FFF90);
    RH_ScopedInstall(ScanForAttractors, 0x6060A0);
    RH_ScopedNamedGlobalInstall(FindAttractorInRange, "FindAttractorInRange", 0x6002F0); // lote 203A (sin validar)
    RH_ScopedInstall(ScanEntityList, 0x6034B0);        // lote 198A (sin validar)
    RH_ScopedInstall(FindClosestPedForEffect, 0x603570); // lote 198A (sin validar)
    RH_ScopedInstall(ScanEffect, 0x5FFFD0);              // lote 202A (sin validar)
    RH_ScopedInstall(GetBestEffect, 0x600180);           // lote 202A (sin validar)
}
