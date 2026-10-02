/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEStreamingDecoder
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h" // TODO: Remove

#include "AEStreamingDecoder.h"

// 0x4f2810
// Adopta el CAEDataStream recibido e intenta abrirlo inmediatamente si no es nulo.
CAEStreamingDecoder::CAEStreamingDecoder(CAEDataStream* dataStream) {
    m_dataStream = dataStream;
    if (dataStream)
        dataStream->Initialise();
}

#ifdef USE_DEFAULT_FUNCTIONS
void CAEStreamingDecoder::operator delete(void* mem) {
    ((void(__thiscall*)(void*, int32))(0x4f2860))(mem, 1);
}
#endif

// 0x4f2830
// La clase base es propietaria de m_dataStream y lo destruye al finalizar el decoder.
CAEStreamingDecoder::~CAEStreamingDecoder() {
    // The game checks if dataStream is nullptr, but
    // deleting null pointer is perfectly safe.
    delete m_dataStream;
    m_dataStream = nullptr;
}

/**
 * Registra el ciclo de vida común de todos los decoders de streaming.
 */
void CAEStreamingDecoder::InjectHooks() {
    RH_ScopedClass(CAEStreamingDecoder);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4f2810);
    RH_ScopedInstall(Destructor, 0x4f2830);
}

CAEStreamingDecoder* CAEStreamingDecoder::Constructor(CAEDataStream* dataStream) {
    this->CAEStreamingDecoder::CAEStreamingDecoder(dataStream);
    return this;
}

CAEStreamingDecoder* CAEStreamingDecoder::Destructor() {
    this->CAEStreamingDecoder::~CAEStreamingDecoder();
    return this;
}