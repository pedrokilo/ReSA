/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAttractorScanner
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
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

#include "AttractorScanner.h"
#include "Entity/Ped/Ped.h"
#include "Events/EventAttractor.h"
#include "Events/EventScriptedAttractor.h"
#include "Scripts/Scripted2dEffects.h"
#include "World.h"
#include "Timer.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// 0x5FFF90
uint64 CAttractorScanner::Clear() {
    // 0x86C52C: distancia base; el cuadrado se calcula con la FPU
    const auto& baseDistance = StaticRef<float>(0x86C52C);
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
            plugin::CallMethod<0x6034B0, CAttractorScanner*, void*, CPed*>(this, &sectors[clampedY * MAX_SECTORS_X + clampedX].Buildings, &ped);
            plugin::CallMethod<0x6034B0, CAttractorScanner*, void*, CPed*>(this, &repeatSectors[((y & 15) << 4) + (x & 15)].Objects, &ped);
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
        plugin::CallMethod<0x5FFFD0, CAttractorScanner*, C2dEffect*, int32, CPed*>(this, &CScripted2dEffects::ms_effects[i], 0, &ped);
    }

    // Mejor candidato (0x600180: efecto y entidad por referencia). Sin efecto, o el mismo efecto y entidad que ya se usan, no hay evento
    C2dEffectPedAttractor* effect = nullptr;
    CEntity*               entity = nullptr;
    plugin::CallMethod<0x600180, CAttractorScanner*, C2dEffectPedAttractor**, CEntity**>(this, &effect, &entity);
    if (!effect) {
        return;
    }
    const int32 entityValue = static_cast<int32>(reinterpret_cast<uintptr_t>(entity));
    if (effect == reinterpret_cast<C2dEffectPedAttractor*>(m_pEffectInUse) && field_14 == entityValue) {
        return;
    }
    // Tipo 7 (PED_ATTRACTOR_SCRIPTED): solo si 0x603570 devuelve este mismo peaton, con CEventScriptedAttractor; el resto, CEventAttractor. El efecto en uso solo cambia si el grupo acepta el evento
    if (effect->m_nAttractorType == PED_ATTRACTOR_SCRIPTED) {
        if (plugin::CallAndReturn<CPed*, 0x603570, C2dEffectPedAttractor*>(effect) != &ped) {
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
}
