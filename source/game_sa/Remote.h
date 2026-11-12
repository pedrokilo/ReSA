#pragma once

class CRemote {
public:
    static void InjectHooks();

    static void TakeRemoteControlledCarFromPlayer(bool bCreateRemoteVehicleExplosion);
    static void GivePlayerRemoteControlledCar(CVector pos, float rotation, int16 modelId);
    static void SetPlayerRemoteControlledCar(CVehicle* vehicle); // 0x45AD40 (lote 181A, sin validar)
};
