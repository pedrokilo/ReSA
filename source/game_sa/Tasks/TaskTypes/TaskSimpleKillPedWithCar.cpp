/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleKillPedWithCar
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)
#include "TaskSimpleKillPedWithCar.h"
#include "Enums/eDirection.h"

void CTaskSimpleKillPedWithCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleKillPedWithCar, 0x86f2c0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x653C10);
    RH_ScopedInstall(Destructor, 0x653C90);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x6563B0); // lote 176A (sin validar)
    RH_ScopedInstall(ShakePadIfPlayerVehicle, 0x6563D0);  // lote 176A (sin validar)

    RH_ScopedInstall(DamageCarBonnet, 0x653CF0);
    RH_ScopedVMTInstall(Clone, 0x655A20);
    RH_ScopedVMTInstall(GetTaskType, 0x653C70);
    RH_ScopedVMTInstall(MakeAbortable, 0x653C80);
    RH_ScopedVMTInstall(ProcessPed, 0x6564A0);
}

// 0x653C10
CTaskSimpleKillPedWithCar::CTaskSimpleKillPedWithCar(CVehicle* car, float impulseMag) :
    m_Car{car},
    m_ImpulseMag{impulseMag}
{
}

// 0x655A20
CTaskSimpleKillPedWithCar::CTaskSimpleKillPedWithCar(const CTaskSimpleKillPedWithCar& o) :
    CTaskSimpleKillPedWithCar{o.m_Car, o.m_ImpulseMag}
{
}

// 0x653CF0
void CTaskSimpleKillPedWithCar::DamageCarBonnet(CPed const* ped) {
    if (!m_Car->IsSubAutomobile()) {
        return;
    }

    const auto flyingObj = m_Car->AsAutomobile()->RemoveBonnetInPedCollision();
    if (!flyingObj) {
        return;
    }

    // Como el original: rand() * (1/RAND_MAX) > 0.5 elige el signo (no DoCoinFlip) y solo se niega la parte de `right`;
    // la velocidad del capo es (velocidad del peaton +- right * 0.1) + up * 0.5
    const auto plus = static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL > 0.5f;
    const CMatrix& mat = *m_Car->m_matrix;
    const CVector up{ mat.GetUp().x * 0.5f, mat.GetUp().y * 0.5f, mat.GetUp().z * 0.5f };
    const CVector right{ mat.GetRight().x * 0.1f, mat.GetRight().y * 0.1f, mat.GetRight().z * 0.1f };
    const auto& vel = ped->m_vecMoveSpeed; // Como el original: mov eax,[esp+40h] (el peaton), no el capo
    if (plus) {
        flyingObj->m_vecMoveSpeed = CVector{ (vel.x + right.x) + up.x, (vel.y + right.y) + up.y, (vel.z + right.z) + up.z };
    } else {
        flyingObj->m_vecMoveSpeed = CVector{ (vel.x - right.x) + up.x, (vel.y - right.y) + up.y, (vel.z - right.z) + up.z };
    }

    flyingObj->ApplyTurnForce(mat.GetUp() * 10.f, mat.GetForward());
}

// 0x6563B0 (lote 176A, sin validar; destructor con borrado de CTaskSimpleKillPedWithCar del EXE, ranura 0 de su vtable 0x86F2C0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x653C90; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x653C90, con hook),
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleKillPedWithCar* CTaskSimpleKillPedWithCar::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

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

// 0x6564A0
bool CTaskSimpleKillPedWithCar::ProcessPed(CPed* ped) {
    CVehicle* const car = m_Car;
    const CVector pedPos   = ped->GetPosition();
    const CVector carPos   = car->GetPosition();
    const CVector pedToCar = pedPos - carPos; // Del coche al peaton
    const CMatrix& carMat  = *car->m_matrix;
    // Como el original: posicion del peaton en el espacio del coche con productos escalares (no TransformPoint)
    const CVector pedPosOS = carMat.InverseTransformVector(pedToCar);
    const auto& carVel     = car->GetMoveSpeed();
    // Como el original: fcomp 0.0 / test ah,41h, un producto nulo cuenta como marcha atras
    const bool carIsGoingFwd = DotProduct(carMat.GetForward(), carVel) > 0.f;
    // Como el original: CModelInfo::ms_modelInfoPtrs[modelo]->m_pColModel en linea (sin colisiones especiales)
    const auto& carBB  = CModelInfo::GetModelInfo(car->m_nModelIndex)->GetColModel()->GetBoundingBox();
    auto pedHitSide    = CPedGeometryAnalyser::ComputePedHitSide(*ped, *car); // Como el original: dword completo

    eWeaponType killWT = WEAPON_RAMMEDBYCAR;
    enum {
        FROM_LEFT  = 1,
        FROM_RIGHT = 3,
        FROM_BACK  = 4,
        FROM_FRONT = 5
    } carDir{};
    const auto HitFromFront = [&] {
        ped->m_vecMoveSpeed = CVector{ carVel.x * 0.9f, carVel.y * 0.9f, 0.f };
        if (pedHitSide == (int32)eDirection::LEFT || pedHitSide == (int32)eDirection::RIGHT) {
            pedHitSide = (int32)eDirection::FORWARD;
        }
        if (CLocalisation::KnockDownPeds()) {
            car->m_vehicleAudio.AddAudioEvent(AE_PED_CRUNCH, 0.f);
        }
    };
    if (std::abs(pedPosOS.x) > carBB.m_vecMax.x * 0.99f) { // 0x6565D5 - Ped is on left/right
        carDir = pedPosOS.x > 0.f ? FROM_RIGHT : FROM_LEFT; // Como el original: x == 0 cuenta como izquierda
        if (std::abs(pedPosOS.y) < std::abs((carIsGoingFwd ? carBB.m_vecMax.y : carBB.m_vecMin.y) * 0.85f)) { // Did we run over the ped?
            killWT = WEAPON_RUNOVERBYCAR;
            HitFromFront();
        }
    } else if (pedPosOS.z > 0.1f) { // 0x6566CB - Ped is on the bonnet
        carDir = FROM_BACK;

        const auto& carFwd = carMat.GetForward();
        const auto& carUp  = carMat.GetUp();
        float height, length;
        if (carFwd.z < -0.2f) { // 0x6566EE
            height = carBB.m_vecMin.y * carFwd.z + carBB.m_vecMax.z * carUp.z + carPos.z; // Como el original: max.z (no max.x)
            length = carIsGoingFwd ? carBB.m_vecMax.y - carBB.m_vecMin.y : -carBB.m_vecMin.y;
        } else if (carFwd.z > 0.1f) { // 0x65672C
            height = carBB.m_vecMax.y * carFwd.z + carBB.m_vecMax.z * carUp.z + carPos.z;
            length = carIsGoingFwd ? carBB.m_vecMax.y : carBB.m_vecMax.y - carBB.m_vecMin.y; // Como el original: segun la marcha
        } else { // 0x65675A
            height = carBB.m_vecMax.z * carUp.z + carPos.z;
            length = carIsGoingFwd ? carBB.m_vecMax.y : -carBB.m_vecMin.y;
        }
        const auto t = (height - pedPos.z) / (length / carVel.Magnitude());
        CVector dir = carVel;
        dir.Normalise();
        ped->m_vecMoveSpeed = dir * t;

        DamageCarBonnet(ped);
    } else { // 0x656824
        carDir = FROM_FRONT; // Como el original: mov dword ptr [esp+1Ch],5
        killWT = WEAPON_RUNOVERBYCAR; // Como el original: mov ebx,32h
        HitFromFront();
    }

    // Como el original: el evento vive hasta el final de la funcion
    CEventVehicleHitAndRun hitAndRunEvent{ ped, car };
    GetEventGlobalGroup()->Add(&hitAndRunEvent, false);

    // 0x6568E2

    CPedDamageResponseCalculator dmgRespCalc{car, 1000.f, killWT, PED_PIECE_HEAD, false};
    // Como el original: se construye con la cabeza y despues se corrige la pieza segun el lado
    CEventDamage eventDmg{car, CTimer::GetTimeInMS(), killWT, PED_PIECE_HEAD, (uint8)pedHitSide, false, ped->bInVehicle};
    switch (carDir) {
    case FROM_LEFT:  eventDmg.m_pedPieceType = PED_PIECE_LEFT_ARM;  break;
    case FROM_RIGHT: eventDmg.m_pedPieceType = PED_PIECE_RIGHT_ARM; break;
    case FROM_BACK:  eventDmg.m_pedPieceType = PED_PIECE_ASS;       break;
    case FROM_FRONT: eventDmg.m_pedPieceType = PED_PIECE_HEAD;      break;
    }
    if (eventDmg.AffectsPed(ped)) {
        dmgRespCalc.ComputeDamageResponse(ped, eventDmg.m_damageResponse, true);
    } else {
        eventDmg.m_damageResponse.SetDamageAsCalculated();
    }
    ped->GetEventGroup().Add(&eventDmg, false);

    ShakePadIfPlayerVehicle();

    ped->bIsStanding       = false;
    ped->bKnockedUpIntoAir = carDir == FROM_BACK;

    // Como el original: min(1, masa * (1/1400)) * -100 y despues por componente
    CVector forceDir = pedToCar;
    forceDir.Normalise();
    auto massFactor = car->m_fMass * (1.f / 1400.f);
    if (1.f < massFactor) {
        massFactor = 1.f;
    }
    const auto forceMag = massFactor * -100.f;
    car->ApplyMoveForce(CVector{ forceDir.x * forceMag, forceDir.y * forceMag, forceDir.z * forceMag });

    return true;
}
