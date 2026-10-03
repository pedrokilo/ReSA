/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAudioZones
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AudioZones.h"

/**
 * Registra el catálogo y actualización espacial de AudioZones.
 */
void CAudioZones::InjectHooks() {
    RH_ScopedClass(CAudioZones);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(Init, 0x5081A0);
    RH_ScopedInstall(RegisterAudioSphere, 0x5081C0);
    RH_ScopedInstall(RegisterAudioBox, 0x508240);
    RH_ScopedInstall(SwitchAudioZone, 0x508320);
    RH_ScopedInstall(Update, 0x5083C0);
}

// 0x5081A0
void CAudioZones::Init() {
    ZoneScoped;

    m_NumSpheres = 0;
    m_NumBoxes = 0;
    m_NumActiveSpheres = 0;
    m_NumActiveBoxes = 0;
}

// 0x508240
// Añade una zona AABB comprimida al siguiente slot disponible del array global.
void CAudioZones::RegisterAudioBox(char name[8], int32 id, bool isActive, CVector min, CVector max) {
    // Como el original: se escribe en el sitio (strcpy sin rellenar el resto del nombre, palabra de flags
    // con solo el bit 0 modificado) y cada coordenada se comprime como (int16)_ftol2(v * 8.0f).
    auto& box = m_aBoxes.data()[m_NumBoxes];
    strcpy(box.m_szName, name);
    box.m_nAudioZone = (int16)id;
    box.m_IsActive = isActive;
    const auto Compress = [](float v) { return (int16)(int64)(v * 8.0f); };
    auto* packed = reinterpret_cast<int16*>(&box.m_Box);
    packed[0] = Compress(min.x);
    packed[1] = Compress(min.y);
    packed[2] = Compress(min.z);
    packed[3] = Compress(max.x);
    packed[4] = Compress(max.y);
    packed[5] = Compress(max.z);
    m_NumBoxes++;
}

// 0x5081C0
// Añade una zona esférica al catálogo global conservando nombre, ID y estado activo.
void CAudioZones::RegisterAudioSphere(char name[8], int32 id, bool isActive, CVector position, float radius) {
    // Como el original: se escribe en el sitio, sin copiar una estructura local completa.
    auto& sphere = m_aSpheres.data()[m_NumSpheres];
    strcpy(sphere.m_szName, name);
    sphere.m_nAudioZone = (int16)id;
    sphere.m_IsActive = isActive;
    sphere.m_Sphere.m_vecCenter = position;
    sphere.m_Sphere.m_fRadius = radius;
    m_NumSpheres++;
}

// 0x508320
// Activa/desactiva todas las zonas cuyo nombre coincide sin distinguir mayúsculas.
void CAudioZones::SwitchAudioZone(const char* zoneName, bool enable) {
    // Como el original: contadores con signo releídos en cada vuelta y sin límite por tamaño del array.
    for (int32 i = 0; i < (int32)m_NumSpheres; i++) {
        auto& sphere = m_aSpheres.data()[i];
        if (!_stricmp(zoneName, sphere.m_szName))
            sphere.m_IsActive = enable;
    }

    for (int32 i = 0; i < (int32)m_NumBoxes; i++) {
        auto& box = m_aBoxes.data()[i];
        if (!_stricmp(zoneName, box.m_szName))
            box.m_IsActive = enable;
    }
}

// 0x5083C0
/**
 * Recalcula qué zonas contienen la posición dada.
 *
 * Salvo forceUpdate, limita el coste mediante una comprobación periódica y por
 * distancia recorrida. Los arrays activos guardan índices hacia los catálogos fijos.
 */
void CAudioZones::Update(bool forceUpdate, CVector posn) {
    ZoneScoped;

    // Como el original: la última posición es un global del EXE (0xB6EBFC).
    auto& LastUpdateCoors = StaticRef<CVector>(0xB6EBFC);

    // Como el original: distancia = sqrt(dz*dz + dy*dy + dx*dx); con NaN se actualiza.
    if (!forceUpdate && (CTimer::m_FrameCounter & 0xF) != 6) {
        const float dx = posn.x - LastUpdateCoors.x;
        const float dy = posn.y - LastUpdateCoors.y;
        const float dz = posn.z - LastUpdateCoors.z;
        if (std::sqrt(dz * dz + dy * dy + dx * dx) < 20.0f)
            return;
    }

    LastUpdateCoors = posn;
    m_NumActiveSpheres = m_NumActiveBoxes = 0;

    // Como el original: esfera estricta sqrt(dz*dz + dy*dy + dx*dx) < radio, con d = posición - centro.
    int32 numActive = 0;
    for (int32 i = 0; i < (int32)m_NumSpheres; i++) {
        const auto& sphere = m_aSpheres.data()[i];
        if (!sphere.m_IsActive)
            continue;

        const float dx = posn.x - sphere.m_Sphere.m_vecCenter.x;
        const float dy = posn.y - sphere.m_Sphere.m_vecCenter.y;
        const float dz = posn.z - sphere.m_Sphere.m_vecCenter.z;
        if (std::sqrt(dz * dz + dy * dy + dx * dx) < sphere.m_Sphere.m_fRadius && numActive < 10)
            m_aActiveSpheres.data()[numActive++] = i;
    }
    m_NumActiveSpheres = numActive;

    // Como el original: caja abierta (min*0.125 < p < max*0.125 en cada eje, x, y, z).
    numActive = 0;
    for (int32 i = 0; i < (int32)m_NumBoxes; i++) {
        const auto& box = m_aBoxes.data()[i];
        if (!box.m_IsActive)
            continue;

        const auto* packed = reinterpret_cast<const int16*>(&box.m_Box);
        if (!((float)packed[0] * 0.125f < posn.x && (float)packed[3] * 0.125f > posn.x))
            continue;
        if (!((float)packed[1] * 0.125f < posn.y && (float)packed[4] * 0.125f > posn.y))
            continue;
        if (!((float)packed[2] * 0.125f < posn.z && (float)packed[5] * 0.125f > posn.z))
            continue;
        if (numActive < 10)
            m_aActiveBoxes.data()[numActive++] = i;
    }
    m_NumActiveBoxes = numActive;
}
