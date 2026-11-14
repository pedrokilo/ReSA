// Archivo propio de CTaskSimpleKillPedWithCar::ShakePadIfPlayerVehicle (0x6563D0): se compila con /O2 /Ob0 /Oi /arch:IA32 /fp:fast (ver source/CMakeLists.txt)
// para que el resto de la clase (ProcessPed, 0x6564A0) conserve la aritmetica normal. Solo contiene esa funcion; su hook sigue en TaskSimpleKillPedWithCar.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)
#include "TaskSimpleKillPedWithCar.h"

// 0x6563D0 (lote 176A, sin validar; antes en linea en la DLL; thiscall, sin argumentos de pila, `ret`, 208 bytes; la llama ProcessPed (0x6569B5) con ECX = this): con el coche de la tarea (+8) igual al vehiculo del jugador
// (FindPlayerVehicle(-1, false), 0x56E0D0), sacude el mando 0: valor = |velocidad| * (1 / masa) * 200000.0 (0x86C688) + 80.0 (0x858A14), limitado a 250.0 (0x859F80), truncado a entero con _ftol (0x821B40) y reducido
// al byte bajo (frecuencia); CPad::GetPad(0)->StartShake(40000 / frecuencia, frecuencia, 0) (0x53F920). x87 a 24 bits: 1/masa se guarda como float (fstp) y el modulo (suma de cuadrados en el orden x, y, z; fsqrt)
// se multiplica sin guardar; el EXE recalcula la expresion completa si no supera 250.0 (mismo valor). La frecuencia es >= 80, asi que la division no falla.
#pragma float_control(precise, on, push)
void CTaskSimpleKillPedWithCar::ShakePadIfPlayerVehicle() {
    CVehicle* const car = m_Car;
    if (FindPlayerVehicle(-1, false) != car) {
        return;
    }
    const float invMass = 1.f / car->m_fMass;
    const float speedX  = car->m_vecMoveSpeed.x;
    const float speedY  = car->m_vecMoveSpeed.y;
    const float speedZ  = car->m_vecMoveSpeed.z;
    // sqrt con un double: std::sqrt(float) llamaria a sqrtf del CRT (SSE, con los denormales aplanados por el MXCSR del juego) en lugar de fsqrt sobre el registro de la x87
    float       value   = static_cast<float>(std::sqrt(static_cast<double>((speedX * speedX + speedY * speedY) + speedZ * speedZ)) * invMass * 200'000.f + 80.f);
    if (value > 250.f) {
        value = 250.f;
    }
    const auto freq = static_cast<uint8>(static_cast<int64>(value));
    CPad::GetPad(0)->StartShake(static_cast<int16>(40'000 / static_cast<int32>(freq)), freq, 0);
}
#pragma float_control(pop)
