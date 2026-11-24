/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEMFDecoder
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/* Decoder de user tracks (".mp3", ".aac", ".m4a", ".mov"). El juego original lo hacia con QuickTime (QTML): abria la pelicula, pedia al SoundConverter PCM 'NONE' / estereo / 16 bits / 44100 Hz
 * y lo leia por trozos con SoundConverterFillBuffer. Desde la rama sustituir-quicktime lo hace Media Foundation (IMFSourceReader, ver MfPcmReader.h): misma salida (PCM estereo de 16 bits a 44100 Hz),
 * misma duracion en unidades de 1/44100 s, mismos limites (se descartan las pistas de menos de 7 s) y misma lectura por trozos. Los 31 puentes QTML del EXE (0x818430..0x81DAA0), el gancho 0x819700, los
 * ayudantes 0x4E7990 / 0x4E7A80 / 0x4E7B80 y QTMLRuntime ya no existen. Mapeo de cada llamada en docs/QuickTime/sustitucion-quicktime.md.
 * Si Media Foundation no esta disponible, InitLibrary devuelve false y el gestor de user tracks pasa los ".mp3" al decoder WMA (como el original sin QuickTime).
 * La clase conserva la disposicion del objeto (0xC4 bytes) y su vtable para no cambiar el resto del EXE.
 */

#include "StdInc.h" // TODO: Remove

#include "AEMFDecoder.h"
#include "X87Math.h"
#include "MfPcmReader.h"

// 0x4e7770
// Como el original: sólo escribe el nombre, el flag, la película, la media y la pista; el resto del objeto queda
// sin escribir.
CAEMFDecoder::CAEMFDecoder(char* filename, int32 trackID)
    : CAEStreamingDecoder(nullptr), m_pFilename(filename), m_bInitialized(false), m_movie(nullptr), m_media(nullptr), m_reader(nullptr), m_nTrackID(trackID) {}

// 0x4e77c0
// Como el original: borra el nombre (operator delete escalar, sin anular el puntero) y libera el lector de PCM (el original liberaba pelicula, buffer de origen y conversor de QuickTime).
CAEMFDecoder::~CAEMFDecoder() {
    if (m_pFilename) {
        operator delete(m_pFilename);
    }
    delete m_reader;
    m_reader = nullptr;
}

// 0x4e7cb0
// Como el original: abre la pista (antes 0x4E7990/0x4E7A80/0x818430..), fija la salida en PCM estereo de 16 bits a 44100 Hz, toma la duracion (en unidades de 1/44100 s) y descarta las pistas de menos de 7 s.
bool CAEMFDecoder::Initialise() {
    m_bInitialized = false;
    delete m_reader;
    m_reader = nullptr;

    auto* reader = new CMfPcmReader();
    if (m_pFilename == nullptr || !reader->Open(m_pFilename, 44100, 2)) {
        delete reader;
        return false;
    }
    m_reader       = reader;
    m_nPlayTime    = 0;
    m_nLength      = static_cast<long>(reader->DurationFrames());
    m_nSampleRate  = 44100;
    m_fTimeDivider = std::bit_cast<float>(0x42306666u); // 44.1

    // Como el original: fild · (1000/44100, float 0x85F390) con un solo redondeo a float (x87 a 24 bits), truncado con _ftol2.
    static const float kMediaUnitsToMs = std::bit_cast<float>(0x3CB9C278u);
    m_nLengthMs = static_cast<long>(static_cast<int64>(IntTimesFloatCpp(m_nLength, kMediaUnitsToMs)));

    // Como el original: la duracion se consulta por la vtable y las pistas de menos de 7 s se descartan.
    m_bInitialized = true;
    if (GetStreamLengthMs() < 7000) {
        m_bInitialized = false;
    }
    return m_bInitialized;
}

// 0x4e7860
// Como el original: sin inicializar devuelve -1; si no, devuelve los bytes de PCM escritos (0 al terminar la pista). El tiempo de reproduccion avanza una unidad por fotograma entregado (4 bytes).
size_t CAEMFDecoder::FillBuffer(void* dest, size_t size) {
    if (!m_bInitialized || m_reader == nullptr)
        return (size_t)-1;

    const size_t written = m_reader->Read(dest, size);
    m_nPlayTime += static_cast<long>(written / 4);
    return written;
}

// 0x4e7920
long CAEMFDecoder::GetStreamLengthMs() {
    if (m_bInitialized)
        return m_nLengthMs;

    return -1;
}

// 0x4e7940
// Como el original: tiempo de reproducción (fild, con signo) / divisor en x87 a la precisión vigente, truncado
// con _ftol2 (0x821B40).
long CAEMFDecoder::GetStreamPlayTimeMs() {
    if (!m_bInitialized)
        return -1;

    return MfDecoderPlayTimeMsCpp(m_nPlayTime, m_fTimeDivider);
}

// 0x4e78e0
// Como el original: sólo guarda pos (sin signo: +2^32 si el bit alto está puesto) · divisor, truncado con _ftol2.
void CAEMFDecoder::SetCursor(unsigned long pos) {
    if (!m_bInitialized)
        return;

    m_nPlayTime = MfDecoderCursorToPlayTimeCpp(pos, m_fTimeDivider);
    if (m_reader) {
        m_reader->Seek(m_nPlayTime); // la posicion se cuenta en fotogramas a 44100 Hz
    }
}

// 0x4e78c0
int32 CAEMFDecoder::GetSampleRate() {
    if (m_bInitialized)
        return m_nSampleRate;

    return -1;
}

// 0x4e77b0
int32 CAEMFDecoder::GetStreamID() {
    return m_nTrackID;
}

// 0x4e7c70
// Antes InitializeQTML(0) + EnterMovies; ahora MFStartup (con CoInitializeEx). Devuelve false si Media Foundation no esta (el gestor reencamina entonces los ".mp3" al decoder WMA).
bool CAEMFDecoder::InitLibrary() {
    const bool ok = CMfPcmReader::Startup();
    if (!ok) {
        NOTSA_LOG_INFO("User tracks: Media Foundation no disponible; los .mp3 usan el decoder WMA y .aac/.m4a/.mov no se cargan");
    }
    return ok;
}

// 0x4e7850
// Antes ExitMovies + TerminateQTML; ahora MFShutdown.
void CAEMFDecoder::Shutdown() {
    CMfPcmReader::Shutdown();
}

namespace {
// 0x4E77C0 (lote 146A, banco Fn146A): cuerpo del destructor de CAEMFDecoder que usa el destructor con borrado del EXE (thiscall, sin argumentos de pila; el hook Destructor de la clase). Funcion propia sin
// expansion en linea para que el banco pueda sustituirla; el destructor con borrado la llama como el `call 0x4E77C0` del EXE.
__declspec(noinline) void __fastcall MFDecoderDestructorBody(CAEMFDecoder* self, void* /*edx*/) { self->CAEMFDecoder::~CAEMFDecoder(); }

// 0x4E7C50 (lote 146A, banco Fn146A): destructor con borrado de CAEMFDecoder del EXE (ranura 6 de su vtable 0x85F370; thiscall, un argumento de pila, `ret 4`, 27 bytes:
// `push esi; mov esi,ecx; call 0x4E77C0; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x4E77C0), libera `this`
// con operator delete (0x8214BD, CRT, sin hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CAEMFDecoder* __fastcall MFDecoderScalarDeletingDestructor(CAEMFDecoder* self, void* /*edx*/, uint8 flags) {
    MFDecoderDestructorBody(self, nullptr);
    if (flags & 1) {
        ::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote146AMFDecoderDestructorBody() { return reinterpret_cast<uintptr_t>(&MFDecoderDestructorBody); } // para el banco Fn146A

/**
 * Registra el decoder (Media Foundation en lugar de QuickTime).
 */
void CAEMFDecoder::InjectHooks() {
    // Como el original: vtable 0x85F370 con 8 entradas (Initialise, FillBuffer, GetStreamLengthMs,
    // GetStreamPlayTimeMs, SetCursor, GetSampleRate, destructor con borrado, GetStreamID). Las virtuales se
    // instalan en su hueco: un hook plano saltaría al thunk virtual de la DLL y volvería al EXE (bucle).
    RH_ScopedVirtualClass(CAEMFDecoder, 0x85F370, 8);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4e7770);
    RH_ScopedInstall(Destructor, 0x4e77c0);
    RH_ScopedNamedGlobalInstall(MFDecoderScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4E7C50); // lote 146A (banco Fn146A)
    RH_ScopedInstall(Shutdown, 0x4E7850); // lote 146A (banco Fn146A)
    RH_ScopedInstall(InitLibrary, 0x4e7c70);
    RH_ScopedVMTInstall(Initialise, 0x4e7cb0);
    RH_ScopedVMTInstall(FillBuffer, 0x4e7860);
    RH_ScopedVMTInstall(GetStreamLengthMs, 0x4e7920);
    RH_ScopedVMTInstall(GetStreamPlayTimeMs, 0x4e7940);
    RH_ScopedVMTInstall(SetCursor, 0x4e78e0);
    RH_ScopedVMTInstall(GetSampleRate, 0x4e78c0);
    RH_ScopedVMTInstall(GetStreamID, 0x4e77b0);
}

// 0x4E7770
CAEMFDecoder* CAEMFDecoder::Constructor(char* filename, int32 trackID) {
    this->CAEMFDecoder::CAEMFDecoder(filename, trackID);
    return this;
}

// 0x4E77C0
CAEMFDecoder* CAEMFDecoder::Destructor() {
    this->CAEMFDecoder::~CAEMFDecoder();
    return this;
}
