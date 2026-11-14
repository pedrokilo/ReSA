// Archivo propio de CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ComputeTarget (0x646F40): se compila con /O2 /Ob0 /Oi /arch:IA32 /fp:fast (ver source/CMakeLists.txt)
// para que el resto de la clase conserve la aritmetica normal. Solo contiene esa funcion; su hook sigue en TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)
#include "TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"

// 0x646F40 (lote 176A, sin validar; thiscall con retorno de CVector por puntero oculto, un argumento de pila, `ret 4`, 158 bytes): objetivo del peaton = posicion del coche + (posicion de la puerta - posicion del coche) * 2.
// Como el original, en este orden: GetPositionToOpenCarDoor (0x64E740) con el coche (+8 de la subtarea de espera) y la puerta (+0xC); DESPUES se lee la posicion del coche (matriz +0x14 si existe; si no, la
// posicion propia +4) y se copia a locales; x87 a 24 bits: cada diferencia (puerta - coche) se calcula sin guardar, se duplica (fadd st,st: exacto) y se suma a la posicion del coche, guardando cada componente como float.
#pragma float_control(precise, on, push)
CVector CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ComputeTarget() const {
    const auto    veh     = m_TaskWaitForDoor->m_veh;
    const CVector doorPos = CCarEnterExit::GetPositionToOpenCarDoor(veh, m_TaskWaitForDoor->m_doorsToWaitFor[0]);
    const CVector vehPos  = m_TaskWaitForDoor->m_veh->GetPosition();
    // Como el EXE: la diferencia en z se guarda como float antes de duplicarla y el doble de la diferencia en x se guarda como float antes de sumar la posicion; la y no se guarda (el doble queda en un registro con exponente ampliado)
    volatile float dz = doorPos.z - vehPos.z;
    volatile float x2 = (doorPos.x - vehPos.x) * 2.f;
    return CVector{
        x2 + vehPos.x,
        vehPos.y + (doorPos.y - vehPos.y) * 2.f,
        vehPos.z + dz * 2.f
    };
}
#pragma float_control(pop)
