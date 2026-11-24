// Referencia con el QuickTime real instalado (QTMLClient.dll, 32 bits): decodifica un archivo con MovieAudioExtraction a PCM
// entero de 16 bits, estereo, 44100 Hz (la misma salida que pedia el juego al SoundConverter) y escribe tamano, duracion y CRC32.
// Uso: qt_ref.exe <archivo> [salida.pcm]
// No es el camino exacto del juego (SoundConverterFillBuffer con callback propio) pero usa el mismo decodificador de QuickTime.
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>

static uint32_t Crc32(uint32_t crc, const uint8_t* p, size_t n) {
    static uint32_t table[256];
    static bool     init = false;
    if (!init) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int k = 0; k < 8; ++k) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            table[i] = c;
        }
        init = true;
    }
    crc = ~crc;
    for (size_t i = 0; i < n; ++i) crc = table[(crc ^ p[i]) & 0xFF] ^ (crc >> 8);
    return ~crc;
}

#pragma pack(push, 2)
struct FSSpec { int16_t vRefNum; int32_t parID; uint8_t name[64]; };
#pragma pack(pop)
struct ASBD { double rate; uint32_t id, flags, bytesPerPacket, framesPerPacket, bytesPerFrame, channels, bits, reserved; };
struct ABuf { uint32_t channels, bytes; void* data; };
struct ABufList { uint32_t n; ABuf b[1]; };

#define QT(ret, name, args) typedef ret(__stdcall* name##_t) args; static name##_t name;
QT(int16_t, InitializeQTML, (int32_t))
QT(int16_t, EnterMovies, ())
QT(int16_t, NativePathNameToFSSpec, (char*, FSSpec*, int32_t))
QT(int16_t, OpenMovieFile, (const FSSpec*, int16_t*, int8_t))
QT(int16_t, NewMovieFromFile, (void**, int16_t, int16_t*, uint8_t*, int16_t, uint8_t*))
QT(int32_t, GetMovieDuration, (void*))
QT(int32_t, GetMovieTimeScale, (void*))
QT(int32_t, MovieAudioExtractionBegin, (void*, uint32_t, void**))
QT(int32_t, MovieAudioExtractionEnd, (void*))
QT(int32_t, MovieAudioExtractionFillBuffer, (void*, uint32_t*, ABufList*, uint32_t*))
QT(int32_t, MovieAudioExtractionSetProperty, (void*, uint32_t, uint32_t, uint32_t, const void*))

static uint32_t FourCC(const char* s) { return (uint32_t(uint8_t(s[0])) << 24) | (uint32_t(uint8_t(s[1])) << 16) | (uint32_t(uint8_t(s[2])) << 8) | uint8_t(s[3]); }

int main(int argc, char** argv) {
    if (argc < 2) return 2;
    SetDllDirectoryA("C:\\Program Files (x86)\\QuickTime\\QTSystem");
    HMODULE m = LoadLibraryA("C:\\Program Files (x86)\\QuickTime\\QTSystem\\QTMLClient.dll");
    if (!m) { std::printf("no carga QTMLClient: %lu\n", GetLastError()); return 3; }
#define L(n) n = (n##_t)GetProcAddress(m, #n); if (!n) { std::printf("falta %s\n", #n); return 4; }
    L(InitializeQTML) L(EnterMovies) L(NativePathNameToFSSpec) L(OpenMovieFile) L(NewMovieFromFile) L(GetMovieDuration) L(GetMovieTimeScale)
    L(MovieAudioExtractionBegin) L(MovieAudioExtractionEnd) L(MovieAudioExtractionFillBuffer) L(MovieAudioExtractionSetProperty)
    int16_t e = InitializeQTML(0);
    std::printf("InitializeQTML=%d\n", e);
    e = EnterMovies();
    std::printf("EnterMovies=%d\n", e);
    FSSpec spec; std::memset(&spec, 0, sizeof spec);
    e = NativePathNameToFSSpec(argv[1], &spec, 0);
    std::printf("NativePathNameToFSSpec=%d\n", e);
    int16_t ref = 0;
    e = OpenMovieFile(&spec, &ref, 1);
    std::printf("OpenMovieFile=%d\n", e);
    void* movie = nullptr; int16_t resId = 0; uint8_t changed = 0;
    e = NewMovieFromFile(&movie, ref, &resId, nullptr, 1, &changed);
    std::printf("NewMovieFromFile=%d movie=%p\n", e, movie);
    if (e || !movie) return 5;
    const int32_t dur = GetMovieDuration(movie), ts = GetMovieTimeScale(movie);
    std::printf("duracion_movie=%d timescale=%d ms=%lld\n", dur, ts, ts ? (long long)dur * 1000 / ts : -1);
    void* ses = nullptr;
    int32_t r = MovieAudioExtractionBegin(movie, 0, &ses);
    std::printf("ExtractionBegin=%d\n", r);
    if (r) return 6;
    ASBD a{}; a.rate = 44100.0; a.id = FourCC("lpcm"); a.flags = 4 | 8; a.bytesPerPacket = 4; a.framesPerPacket = 1; a.bytesPerFrame = 4; a.channels = 2; a.bits = 16;
    r = MovieAudioExtractionSetProperty(ses, FourCC("xaud"), FourCC("asbd"), sizeof a, &a);
    std::printf("SetProperty(asbd)=%d\n", r);
    if (r) return 7;
    FILE* out = argc > 2 ? std::fopen(argv[2], "wb") : nullptr;
    std::vector<uint8_t> buf(16384);
    uint64_t total = 0; uint32_t crc = 0; int16_t peak = 0;
    for (;;) {
        uint32_t frames = 4096, flags = 0;
        ABufList bl; bl.n = 1; bl.b[0].channels = 2; bl.b[0].bytes = 4096 * 4; bl.b[0].data = buf.data();
        r = MovieAudioExtractionFillBuffer(ses, &frames, &bl, &flags);
        if (r) { std::printf("FillBuffer=%d\n", r); break; }
        const size_t n = size_t(frames) * 4;
        crc = Crc32(crc, buf.data(), n);
        for (size_t i = 0; i + 1 < n; i += 2) { int16_t v = *(int16_t*)&buf[i]; int16_t aa = v < 0 ? -v : v; if (aa > peak) peak = aa; }
        if (out) std::fwrite(buf.data(), 1, n, out);
        total += n;
        if (flags & 1 || frames == 0) break;
    }
    if (out) std::fclose(out);
    std::printf("bytes=%llu frames=%llu crc32=%08X pico=%d\n", (unsigned long long)total, (unsigned long long)(total / 4), crc, peak);
    MovieAudioExtractionEnd(ses);
    return 0;
}
