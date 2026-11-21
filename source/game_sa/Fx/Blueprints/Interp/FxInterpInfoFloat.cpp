/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInterpInfoFloat_c
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Este archivo se compila con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt): GetVal usa la aritmetica x87 del EXE.
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"

#include "FxInterpInfoFloat.h"
#include "FxManager.h"
#include "X87Math.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

void FxInterpInfoFloat_c::InjectHooks() {
    RH_ScopedClass(FxInterpInfoFloat_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A8440);
    RH_ScopedInstall(Destructor, 0x004A8460);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A8D30);
    RH_ScopedOverloadedInstall(GetVal, "values", 0x4A8470, void (FxInterpInfoFloat_c::*)(float*, float));
    RH_ScopedOverloadedInstall(GetVal, "attrib", 0x4A85C0, float (FxInterpInfoFloat_c::*)(int32, float, float)); // lote S2A (sin validar)
}

// 0x4A8440
FxInterpInfoFloat_c::FxInterpInfoFloat_c() : FxInterpInfo_c() {
    m_Keys = nullptr;
}

// 0x4A8440 (lote 218A, sin validar): constructor del EXE (thiscall, devuelve this): escribe +4 (byte) = 0, +8 = 0, vtable y +0xC = 0 (no toca +5 ni +6). Llama al constructor C++ de arriba, que reproduce esas escrituras.
FxInterpInfoFloat_c* FxInterpInfoFloat_c::Constructor() {
    this->FxInterpInfoFloat_c::FxInterpInfoFloat_c();
    return this;
}

// 0x4A8460 (lote 218A, sin validar): cuerpo del destructor (thiscall, `mov dword ptr [ecx],0x85A968; ret`): solo deja la vtable base de FxInterpInfo_c. Lo llama el destructor con borrado 0x4A8D30 y los destructores de los FxInfo*_c.
__declspec(noinline) void FxInterpInfoFloat_c::Destructor() {
    // `mov [ecx],0x85A968`: el ~T() generado es trivial y el cuerpo quedaba vacio (sin escribir la vtable; el compilador lo unifica con otros cuerpos vacios): el banco Fn218A lo detecto.
    *reinterpret_cast<uint32*>(this) = 0x85A968;
}

// 0x4A8D30 (lote 218A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A8460; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInterpInfoFloat_c* FxInterpInfoFloat_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x5C16F0
void FxInterpInfoFloat_c::Load(FILESTREAM file) {
    for (auto i = 0; i < static_cast<int8>(m_nCount); i++) { // el EXE relee el contador como byte (el byte alto no lo escribe Allocate)
        ReadField<void>(file);
        ReadField<void>(file, "FX_INTERP_DATA:");

        m_bLooped  = ReadField<bool>(file);
        m_nNumKeys = ReadField<int8>(file);

        if (i == 0) {
            m_pTimes = g_fxMan.Allocate<uint16>(m_nNumKeys);
        }

        m_Keys[i] = g_fxMan.Allocate<float>(m_nNumKeys);
        for (auto j = 0; j < m_nNumKeys; j++) {
            ReadField<void>(file, "FX_KEYFLOAT_DATA:");
            m_pTimes[j] = uint16(ReadField<float>(file) * 256.f);
            m_Keys[i][j] = ReadField<float>(file);
        }
    }
}

// NOTSA
void FxInterpInfoFloat_c::Allocate(int32 count) {
    // 0x4A4C20..0x4A57C0 (lote 215A, sin validar): el EXE escribe solo el byte bajo del contador (+6); el byte alto queda como estaba en la arena
    *reinterpret_cast<int8*>(&m_nCount) = static_cast<int8>(count);
    m_Keys = g_fxMan.Allocate<float*>(count);
}

// 0x4A85C0 (lote S2A, sin validar; thiscall, tres argumentos de pila, `ret 0Ch`, 515 bytes): integral de la curva de claves `attrib` (lineal a trozos; m_pTimes en 1/256) entre t0 = time - deltaTime y
// `time`. Aritmetica de la x87 a 24 bits con la pila sin guardar (Veh83D*): solo se guardan como float Tc/Tp dentro del bucle (fstp dword) y el resultado se devuelve en ST0.
//  - una clave (m_nNumKeys == 1): deltaTime * clave[0].
//  - se busca la primera clave i con t0 < T[i] (T = tiempo * 1/256; comparacion ordenada). Si no hay ninguna: deltaTime * clave[numKeys - 1] (lee clave[-1] con 0 claves, como el EXE);
//    con numKeys < 0 devuelve 0. Si i == 0 el valor inicial Y = clave[0]; si no, Y = interpolacion de la clave i-1 a la i en t0.
//  - desde la clave i, por cada clave: Tc/Tp (guardados como float; Tp = tiempo de la clave anterior, para i == 0 lee el uint16 anterior al vector, como el EXE); si Tc == time: suma
//    ((clave - Y) * 0.5 + Y) * (Tc - t0) y termina; si Tc < time: suma lo mismo, Y = clave, t0 = Tc y sigue; si Tc > time: Vend = clave[i-1] + (time - Tp) / (Tc - Tp) * (clave[i] - clave[i-1]),
//    suma ((Vend - Y) * 0.5 + Y) * (time - t0) y termina; con NaN en la comparacion sigue sin sumar. Si se agotan las claves suma un ultimo trapecio con el ultimo tiempo (ancho Tlast - t0).
float FxInterpInfoFloat_c::GetVal(int32 attrib, float time, float deltaTime) {
    constexpr double kTimeScale = 0.00390625; // 0x859AA0
    constexpr double kHalf      = 0.5;        // 0x858B8C
    if (m_nNumKeys == 1) {
        return Veh83DStore(Veh83DMul(deltaTime, m_Keys[attrib][0]));
    }

    double t0     = Veh83DSub(time, deltaTime);
    const int32 numKeys = m_nNumKeys; // int8 con signo (movsx)
    double acc    = 0.0;
    int32 index   = 0;
    double keyTime = 0.0;
    bool found    = false;
    for (; index < numKeys; ++index) {
        keyTime = Veh83DMul(m_pTimes[index], kTimeScale);
        if (t0 < keyTime) {
            found = true;
            break;
        }
    }
    if (!found) {
        if (index == numKeys) {
            return Veh83DStore(Veh83DMul(deltaTime, m_Keys[attrib][index - 1]));
        }
        return Veh83DStore(acc);
    }

    const float* const keys = m_Keys[attrib];
    double current;
    if (index <= 0) {
        current = keys[0];
    } else {
        const double previousTime = Veh83DMul(m_pTimes[index - 1], kTimeScale);
        const double fraction     = Veh83DDiv(Veh83DSub(t0, previousTime), Veh83DSub(keyTime, previousTime));
        current = Veh83DAdd(Veh83DMul(fraction, Veh83DSub(keys[index], keys[index - 1])), keys[index - 1]);
    }

    for (; index < numKeys; ++index) {
        const float timeCurrent  = Veh83DStore(Veh83DMul(m_pTimes[index], kTimeScale));
        const float timePrevious = Veh83DStore(Veh83DMul(m_pTimes[index - 1], kTimeScale));
        if (timeCurrent == time) {
            const double mid = Veh83DAdd(Veh83DMul(Veh83DSub(keys[index], current), kHalf), current);
            acc = Veh83DAdd(acc, Veh83DMul(mid, Veh83DSub(timeCurrent, t0)));
            return Veh83DStore(acc);
        }
        if (timeCurrent < time) {
            const double mid = Veh83DAdd(Veh83DMul(Veh83DSub(keys[index], current), kHalf), current);
            acc     = Veh83DAdd(acc, Veh83DMul(mid, Veh83DSub(timeCurrent, t0)));
            current = keys[index];
            t0      = timeCurrent;
        } else if (timeCurrent > time) {
            const double fraction = Veh83DDiv(Veh83DSub(time, timePrevious), Veh83DSub(timeCurrent, timePrevious));
            const double end      = Veh83DAdd(Veh83DMul(fraction, Veh83DSub(keys[index], keys[index - 1])), keys[index - 1]);
            const double mid      = Veh83DAdd(Veh83DMul(Veh83DSub(end, current), kHalf), current);
            acc = Veh83DAdd(acc, Veh83DMul(mid, Veh83DSub(time, t0)));
            return Veh83DStore(acc);
        }
    }

    const double lastTime = Veh83DMul(m_pTimes[index - 1], kTimeScale);
    const double mid      = Veh83DAdd(Veh83DMul(Veh83DSub(keys[index - 1], current), kHalf), current);
    acc = Veh83DAdd(acc, Veh83DMul(mid, Veh83DSub(lastTime, t0)));
    return Veh83DStore(acc);
}

// 0x4A8470
void FxInterpInfoFloat_c::GetVal(float* outValues, float delta) {
    InterpolateKeys(m_Keys, outValues, delta, 0.0f);
}
