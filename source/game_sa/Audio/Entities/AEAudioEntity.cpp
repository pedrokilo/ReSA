/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"
#include "AEAudioEntity.h"

constexpr auto EVENT_VOLUMES_BUFFER_SIZE = 45401;

auto& m_pAudioEventVolumes = StaticRef<int8*>(0xBD00F8); // Use `GetDefaultVolume` to access!

void CAEAudioEntity::InjectHooks() {
    // Vtable propia de la clase base: una sola entrada (UpdateParameters, 0x4D7E30,
    // un `retn 8` vacío). La escribe el destructor 0x5DE9E0 antes de destruir m_tempSound.
    RH_ScopedVirtualClass(CAEAudioEntity, 0x859988, 1);
    RH_ScopedCategory("Audio/Entities");

    // 0x5DE9E0: restaura la vtable 0x859988 y salta a ~CAESound (0x4EF660) sobre
    // m_tempSound (+0x8). Solo lo llaman funclets de desenrollado de excepciones.
    RH_ScopedInstall(Destructor, 0x5DE9E0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4D7E30);

    // StaticInitialise y Shutdown están inlineadas en el original (NOTSA) y
    // GetDefaultVolume no tiene dirección conocida: no se hookean.
}

// NOTSA | INLINED | REFACTORED
bool CAEAudioEntity::StaticInitialise() {
    m_pAudioEventVolumes = new int8[EVENT_VOLUMES_BUFFER_SIZE];
    auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\EVENTVOL.DAT", "r");
    if (!file) {
        NOTSA_LOG_WARN("[AudioEngine] Failed to open EVENTVOL.DAT");
        CFileMgr::CloseFile(file);
        return false;
    }
    if (CFileMgr::Read(file, m_pAudioEventVolumes, EVENT_VOLUMES_BUFFER_SIZE) != EVENT_VOLUMES_BUFFER_SIZE) {
        NOTSA_LOG_WARN("[AudioEngine] Failed to read EVENTVOL.DAT");
        CFileMgr::CloseFile(file);
        return false;
    }
    CFileMgr::CloseFile(file);
    return true;
}

// NOTSA | INLINED | REFACTORED
void CAEAudioEntity::Shutdown() {
    delete[] std::exchange(m_pAudioEventVolumes, nullptr);
}

float CAEAudioEntity::GetDefaultVolume(eAudioEvents event) {
    return static_cast<float>(m_pAudioEventVolumes[event]);
}
