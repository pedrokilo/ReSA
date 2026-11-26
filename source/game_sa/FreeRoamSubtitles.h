#pragma once

#include <string>
#include <string_view>

class CPed;

/*
 * Subtitulos de las voces del mundo libre (peatones hablando) (NOTSA, no existe en el EXE).
 * Idea, logica y textos de Free Roam Subtitled de Kaizo M, usados con su autorizacion (declarada por el usuario del proyecto); los textos NO se redistribuyen: se cargan en
 * tiempo de ejecucion de los .fxt de la instalacion del usuario (por defecto `<carpeta del juego>\cleo\cleo_text\English.fxt` / `Spanish.fxt`). El codigo es propio (reconstruccion
 * de la logica del script, no una copia). Ver docs/CodigoVivoExe/subtitulos-mundo-libre.md y docs/DesviacionesConocidas.md (15.F).
 *
 * Interruptor general: variable de entorno GTASA_FREE_ROAM_SUBS=0|1 o linea `FreeRoamSubtitles=0|1` de gta-reversed.ini (por defecto 1; 0 = comportamiento del EXE, no se toca nada).
 * Opciones con los MISMOS nombres y valores por defecto que FreeRoamSub.ini de Kaizo: SpainSpanish=1, ChicanoSlangs=1, SaveToBrief=1, ReadingTime=200 (0..2000 ms). Orden de busqueda:
 * variable GTASA_FRS_<NOMBRE EN MAYUSCULAS> > gta-reversed.ini (cualquier seccion) > cleo\FreeRoamSub.ini (seccion [Subtitles], si el usuario conserva el del mod) > valor por defecto.
 * Carpeta de los .fxt: GTASA_FRS_DIR / `FreeRoamSubtitlesDir=` (por defecto cleo\cleo_text). Las opciones se releen (como mucho cada 2 s) al empezar una voz: apagar en caliente = editar el ini.
 */
namespace FreeRoamSubtitles {
struct Config {
    bool        Enabled      = true;
    bool        SpainSpanish = true;
    bool        ChicanoSlangs = true;
    bool        SaveToBrief  = true;
    int         ReadingTime  = 200; // ms, 0..2000
    std::string Dir          = "cleo\\cleo_text";
};

// Gancho: una voz de peaton acaba de empezar (CAEPedSpeechAudioEntity::PlayLoadedSound 0x4E5CD0, con el sonido ya pedido). `speechEntity` es el CAEPedSpeechAudioEntity y `slotID` su
// hueco de voz (CAEPedSpeechAudioEntity::s_PedSpeechSlots): el final de la voz se detecta porque el hueco deja de ser suyo (sin gancho de final).
void OnPedSpeechStart(CPed* ped, int16 slotID, int16 bankID, int16 soundID, int16 gCtx, const void* speechEntity);

// Una vez por fotograma de juego (al principio de CMessages::Process): mantiene, cambia o retira el subtitulo mostrado y sirve a los bancos de pruebas.
void Tick();

// ---- funciones puras (las usa el banco en proceso; no tocan el juego) ----
// Radio de audicion en metros segun el banco de sonido del hablante (10 por defecto; 20 novias, 35 tenderos/novias, 40 Cesar).
[[nodiscard]] float RadiusForBank(int bankID);
// Prefijos de clave a probar, en orden, para el idioma del juego (4 = espanol): M (jerga chicana), P (Espana), S en espanol; C (jerga chicana) y E en los demas. Devuelve cuantos.
int KeyPrefixes(int language, bool spainSpanish, bool chicanoSlangs, char (&out)[3]);
// Clave del texto: prefijo + banco (3 cifras) + sonido+1 (3 cifras); vacia si no hay sonido valido (banco/sonido negativos o banco 0 y sonido 0).
[[nodiscard]] std::string MakeKey(char prefix, int bankID, int soundID);
// Limita ReadingTime al rango 0..2000 como el script.
[[nodiscard]] int ClampReadingTime(int value);

// Tabla clave -> texto cargada de un .fxt ("CLAVE texto", una por linea; claves de 7 caracteres). Mapa compacto: el fichero entero en un bloque y un indice ordenado.
class FxtTable {
public:
    // Carga el fichero (false si no existe o esta vacio). La primera aparicion de una clave gana.
    bool Load(const std::string& path);
    // Carga desde memoria (bancos de pruebas).
    void LoadFromMemory(std::string data);
    [[nodiscard]] std::string_view Find(std::string_view key) const;
    [[nodiscard]] size_t           Size() const { return m_Index.size(); }
    [[nodiscard]] size_t           MemoryBytes() const { return m_Data.capacity() + m_Index.capacity() * sizeof(Entry); }
    void                           Clear();

private:
    struct Entry {
        uint32 Key;
        uint32 Off;
        uint32 Len;
    };
    static bool EncodeKey(std::string_view key, uint32& out);
    void        Parse();
    std::string        m_Data;
    std::vector<Entry> m_Index;
};

// Opciones leidas de un texto de ini (el mismo formato de gta-reversed.ini y de FreeRoamSub.ini), para los bancos de pruebas.
Config ParseIniText(const Config& base, std::string_view iniText);
} // namespace FreeRoamSubtitles
