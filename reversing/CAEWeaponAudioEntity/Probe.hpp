#pragma once
// Contadores de la sonda temporal: build_probe.py inserta `++calls[i]` al principio
// de cada función C++ de CAEWeaponAudioEntity. Demuestran qué lado del par se
// ejecutó: 0 llamadas a la reconstrucción en el lado original y >= 1 en el candidato.
namespace WeaponAudioProbe {
enum Fn : int {
    Constructor, Destructor, Initialise, Terminate, Reset, WeaponFire, WeaponReload,
    PlayGunSounds, PlayGoggleSound, ReportStealthKill, ReportChainsawEvent,
    PlayChainsawStopSound, PlayWeaponLoopSound, PlayMiniGunFireSounds, PlayMiniGunStopSound,
    PlayFlameThrowerSounds, PlayFlameThrowerIdleGasLoop, StopFlameThrowerIdleGasLoop,
    PlayCameraSound, UpdateParameters, Count
};
inline unsigned calls[Count]{};
}
