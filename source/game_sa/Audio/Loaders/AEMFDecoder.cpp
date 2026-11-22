/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEMFDecoder
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/* Decoder de user tracks que en el juego original usaba QuickTime (QTML).
 *
 * Lote EXT1A: QuickTime se retira. Ya no hay llamadas a los 31 puentes QTML del EXE (0x818430..0x81DAA0), ni al gancho 0x819700, ni al stub 0x8194D0, ni a los ayudantes
 * 0x4E7990 / 0x4E7A80 / 0x4E7B80 (abrir la pelicula, leer el formato de la media y callback de SoundConverterFillBuffer), y se elimina QTMLRuntime.cpp (InitializeQTML, TerminateQTML, carga de
 * QuickTime.qts). InitLibrary devuelve siempre false: el gestor de user tracks (CAEUserRadioTrackManager::Initialise) deja entonces CAEMFDecoder como no soportado y pasa los ".mp3"
 * al decoder WMA, que es el mismo camino que seguia el juego original en un equipo sin QuickTime. Los demas tipos (Vorbis, WAV, WMA) no se tocan.
 * Se pierde: la decodificacion por QuickTime de los tipos que solo QuickTime abria (".aac", ".m4a" y ".mov" en audioExtensionTypes de 0x8CBB28; el ".mp3" pasa a WMA);
 * esas pistas no se cargan. La clase conserva la disposicion del objeto (0xC4 bytes) y su vtable para no cambiar el resto del EXE.
 */

#include "StdInc.h" // TODO: Remove

#include "AEMFDecoder.h"
#include "X87Math.h"
#include "platform/win/QTMLRuntime.h"

// 0x4e7770
// Como el original: sólo escribe el nombre, el flag, la película, la media y la pista; el resto del objeto queda
// sin escribir.
CAEMFDecoder::CAEMFDecoder(char* filename, int32 trackID)
    : CAEStreamingDecoder(nullptr), m_pFilename(filename), m_bInitialized(false), m_movie(nullptr), m_media(nullptr), m_nTrackID(trackID) {}

// 0x4e77c0
// Como el original: borra el nombre (operator delete escalar, sin anular el puntero). Sin QuickTime nunca llega a inicializarse, asi que no hay pelicula, buffer ni conversor que liberar.
CAEMFDecoder::~CAEMFDecoder() {
    if (m_pFilename) {
        operator delete(m_pFilename);
    }
}

// 0x4e7cb0
// Sin QuickTime (lote EXT1A) no se puede abrir ninguna pista: devuelve false, como cuando el original no lograba abrir la pelicula.
bool CAEMFDecoder::Initialise() {
    m_bInitialized = false;
    return false;
}

// 0x4e7860
// Como el original: sin inicializar devuelve -1 (con QuickTime retirado nunca esta inicializado).
size_t CAEMFDecoder::FillBuffer(void* /*dest*/, size_t /*size*/) {
    return (size_t)-1;
}

// 0x4E7B80 (lote S1A, sin validar; cdecl (SoundComponentData** salida, refCon), `ret`, 204 bytes; devuelve un Boolean en AL): callback de SoundConverterFillBuffer. refCon es el bloque +0x54 del decoder (m_fillData;
// campos relativos: +0x10 cuenta de muestras, +0x14 buffer, +0x24 tamano, +0x30 palabra en m_fillUnk7C+8, +0x44 manejador origen, +0x48 media, +0x4C posicion, +0x50 longitud, +0x54 tamano del origen, +0x58 quedan datos,
// +0x59 compresion variable). Como el original y en este orden: con posicion >= longitud (con signo) borra "quedan datos" y los cuatro campos de salida (+0x10, +0x14, +0x24, +0x30); sin datos escribe refCon en *salida y devuelve 0.
// Con datos llama a 0x81A3C0 (puente QuickTime, 12 argumentos cdecl: media, manejador, tamano del origen, &salida (arranca valiendo refCon), posicion, &C, &A, 0, 0, 0, &B, 0; devuelve el OSErr en AX; A, B y C arrancan
// en 0 aqui, basura en el EXE); con error o salida cero hace el mismo borrado y SIGUE. Despues, siempre: +0x10 = A*B (32 bits), posicion = C + A*B, +0x24 = salida y, con compresion variable, +0x30 = salida / B (idiv con
// signo, como el EXE: B = 0 provoca la excepcion); escribe refCon en *salida y devuelve el byte "quedan datos".
uint8 CAEMFDecoder::FillCallback(void** outData, void* refCon) {
    static_assert(offsetof(CAEMFDecoder, m_fillData) == 0x54);
    auto* const self = reinterpret_cast<CAEMFDecoder*>(static_cast<uint8*>(refCon) - 0x54);
    uint8& dataLeft = *reinterpret_cast<uint8*>(&self->m_bSourceDataLeft);               // el EXE trabaja con el byte crudo
    int32& extraWord = *reinterpret_cast<int32*>(self->m_fillUnk7C + 8);
    const auto Clear = [&] {
        dataLeft                  = 0;
        self->m_fillData.buffer   = nullptr;
        self->m_fillData.sampleCount = 0;
        self->m_fillBufferSize    = 0;
        extraWord                 = 0;
    };
    const int32 position = self->m_nPlayTime;
    if (position >= self->m_nLength) {
        Clear();
    }
    if (dataLeft == 0) {
        *outData = refCon;
        return dataLeft;
    }
    int32  a = 0, b = 0, c = 0;
    uint32 out = static_cast<uint32>(reinterpret_cast<uintptr_t>(refCon));
    const auto result = QtmlConverterFillBridge(self->m_sourceMedia, self->m_sourceBuffer, self->m_sourceBufferSize, &out, position, &c, &a, 0, 0, 0, &b, 0);
    if (result != 0 || out == 0) {
        Clear();
    }
    const int32 product = a * b;
    self->m_fillData.sampleCount = product;
    self->m_nPlayTime            = c + product;
    self->m_fillBufferSize       = static_cast<int32>(out);
    if (*reinterpret_cast<const uint8*>(&self->m_bVariableCompression) != 0) { // se relee tras la llamada, como el EXE
        extraWord = static_cast<int32>(out) / b;
    }
    *outData = refCon;
    return dataLeft;
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
// Lote EXT1A: QuickTime retirado. Devuelve false sin cargar ninguna biblioteca (el original llamaba a InitializeQTML(0) y EnterMovies); el gestor de user tracks reencamina los ".mp3" al decoder WMA.
bool CAEMFDecoder::InitLibrary() {
    NOTSA_LOG_INFO("User tracks: QuickTime retirado; los .mp3 usan el decoder WMA y los formatos que solo abria QuickTime no estan disponibles");
    return false;
}

// 0x4e7850
// Lote EXT1A: no hay QuickTime que cerrar (el original llamaba a ExitMovies y TerminateQTML).
void CAEMFDecoder::Shutdown() {}

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
 * Registra el decoder (sin QuickTime desde el lote EXT1A).
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
    RH_ScopedInstall(FillCallback, 0x4E7B80); // lote S1A (sin validar)
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
