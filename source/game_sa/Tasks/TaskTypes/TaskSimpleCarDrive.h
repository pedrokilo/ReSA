#pragma once

#include "TaskSimple.h"

class CVehicle;
class CTaskUtilityLineUpPedWithCar;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarDrive : public CTaskSimple {
public:
    CVehicle*                     m_pVehicle;
    CAnimBlendAssociation*        m_pAnimCloseDoorRolling;
    CTaskUtilityLineUpPedWithCar* m_pTaskUtilityLineUpPedWithCar;
    CTaskTimer                    m_waitTimer; //!< Como el original: temporizador en línea (+0x14..+0x1F) del pasajero delantero sin conductor
    int32                         m_nBoppingStartTime;
    int32                         field_24;
    int32                         m_nBoppingEndTime;  // Seemingly not a tick count, but rather the bopping interval
    float                         m_fBoppingProgress; // 0.0 to 1.0
    int32                         m_nBoppingCompletedTimes;
    int32                         m_nHeadBoppingStartTime;
    int32                         m_nHeadBoppingDirection;
    float                         m_fHeadBoppingOrientation;
    float                         m_fRandomHeadBoppingMultiplier;
    float                         m_fHeadBoppingFactor;
    int32                         m_nArmBoppingStartTime;
    int32                         m_nTimePassedSinceCarUpSideDown;
    CTaskTimer                    m_copCarStolenTimer;

    // Inited according to: 0x63C3AE
    uint8 m_b01 : 1;
    uint8 m_b02 : 1;
    uint8 m_bUpdateCurrentVehicle : 1; // m_bUpdateCurrentVehicle : 1; // updates m_pVehicle pointer to the current occupied vehicle by ped
    uint8 m_b08 : 1;
    uint8 m_b10 : 1;
    uint8 m_b20 : 1;

public:
    static constexpr auto Type = TASK_SIMPLE_CAR_DRIVE;

    CTaskSimpleCarDrive() = delete;
    explicit CTaskSimpleCarDrive(CVehicle* vehicle, CTaskUtilityLineUpPedWithCar* utilityTask = {}, bool updateCurrentVehicle = {});
    ~CTaskSimpleCarDrive() override;



eTaskType GetTaskType() const override { return Type; }

    CTask* Clone() const override;
    bool ProcessPed(class CPed* ped) override;
    bool MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool SetPedPosition(CPed* ped) override;

    void TriggerIK(CPed* ped) const;
    void UpdateBopping();
    void StartBopping(CPed* ped);
    void ProcessHeadBopping(CPed* ped, bool a3, float a4);
    void ProcessArmBopping(CPed* pPed, bool a3, float a4);
    void ProcessBopping(CPed* ped, bool a3);
    void StartRollingDoor(CPed* ped); // 0x642700 (sin catalogar)
    static void FinishAnimCloseDoorRollingCB(CAnimBlendAssociation* anim, void* data); // 0x63C840 (sin catalogar)

    [[nodiscard]] auto GetVehicle() const { return m_pVehicle; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleCarDrive* Constructor(CVehicle* pVehicle, CTaskUtilityLineUpPedWithCar* pUtilityTask, int8_t bUpdateCurrentVehicle) { this->CTaskSimpleCarDrive::CTaskSimpleCarDrive(pVehicle, pUtilityTask, bUpdateCurrentVehicle); return this; }
    __declspec(noinline) CTaskSimpleCarDrive* Destructor() { this->CTaskSimpleCarDrive::~CTaskSimpleCarDrive(); return this; }   // 0x63C460 (sin expansion en linea: el banco Fn152A sustituye este cuerpo)
    CTaskSimpleCarDrive* ScalarDeletingDestructor(uint8 flags); // 0x6426E0 (lote 152A, banco Fn152A): destructor con borrado (ranura 0 de la vtable 0x86E904), `ret 4`
                    };

VALIDATE_SIZE(CTaskSimpleCarDrive, 0x60);
