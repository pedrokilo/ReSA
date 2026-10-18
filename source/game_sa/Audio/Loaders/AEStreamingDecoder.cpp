/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEStreamingDecoder
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
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
    static_cast<CAEStreamingDecoder*>(mem)->ScalarDeletingDestructor(1);
}
#endif

// 0x4f2830
// La clase base es propietaria de m_dataStream y lo destruye al finalizar el decoder.
CAEStreamingDecoder::~CAEStreamingDecoder() {
    // Como el original (0x4F2830): destructor de CAEDataStream sin pasar por la vtable y operator delete escalar
    if (m_dataStream) {
        m_dataStream->CAEDataStream::~CAEDataStream();
        ::operator delete(m_dataStream);
        m_dataStream = nullptr;
    }
}

/**
 * Registra el ciclo de vida común de todos los decoders de streaming.
 */
void CAEStreamingDecoder::InjectHooks() {
    RH_ScopedClass(CAEStreamingDecoder);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4f2810);
    RH_ScopedInstall(Destructor, 0x4f2830);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4f2860);
}

CAEStreamingDecoder* CAEStreamingDecoder::Constructor(CAEDataStream* dataStream) {
    this->CAEStreamingDecoder::CAEStreamingDecoder(dataStream);
    return this;
}

// 0x4F2860: destructor escalar (thiscall, un argumento de pila, ret 4) de la vtable base del EXE (0x85F418, ranura 6). Como el original: deja la vtable base,
// destruye y libera el flujo de datos (CAEDataStream, 0x4DC490 y operator delete 0x8214BD) y lo anula, y solo si el bit 0 del argumento (leido como byte) esta
// puesto libera tambien `this`; devuelve `this`. Es codigo inalcanzable salvo por la ranura de la vtable base (las clases derivadas tienen la suya): sin llamadores
// directos en el EXE, EAX = this es el unico registro de salida definido.
CAEStreamingDecoder* CAEStreamingDecoder::ScalarDeletingDestructor(int32 flags) {
    *reinterpret_cast<uint32*>(this) = 0x85F418;
    if (m_dataStream) {
        m_dataStream->CAEDataStream::~CAEDataStream();
        ::operator delete(m_dataStream);
        m_dataStream = nullptr;
    }
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

CAEStreamingDecoder* CAEStreamingDecoder::Destructor() {
    this->CAEStreamingDecoder::~CAEStreamingDecoder();
    return this;
}