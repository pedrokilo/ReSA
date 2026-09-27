#pragma once
#include "AETrackLoader.h"

/**
 * Loader de metadata para los tracks de radio/streaming empaquetados.
 *
 * Inicializa las tablas de lookup y decide si los packs se leen desde la instalación
 * local o desde una ruta de DVD compatible con el comportamiento original.
 */
class CAEMP3TrackLoader : public CAETrackLoader {
public:
    static void InjectHooks();

    CAEMP3TrackLoader();
    ~CAEMP3TrackLoader();

    bool Initialise();
    bool LoadStreamPackTable(void);
    bool LoadTrackLookupTable(void);
    tTrackInfo* GetTrackInfo(uint32 trackId);

    // Comprueba y selecciona la ruta alternativa de DVD para AUDIO\\STREAMS.
    bool IsCurrentAudioStreamAvailable();

private:
    // NOTSA
    CAEMP3TrackLoader* Constructor() {
        this->CAEMP3TrackLoader::CAEMP3TrackLoader();
        return this;
    }

    void Deconstructor() { this->~CAEMP3TrackLoader(); }
};
