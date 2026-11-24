// Sonda de CMfPcmReader (sustituto de QuickTime): decodifica un archivo a PCM estereo 16 bits 44100 Hz y escribe tamano, duracion y CRC32.
// Uso: mf_probe.exe <archivo> [salida.pcm] [trozo en bytes, por defecto 16384]
// Compilar: compilar_sonda.cmd (x86, /DMFPCM_STANDALONE_TEST).

#include "../../source/game_sa/Audio/Loaders/MfPcmReader.h"

#include <cstdio>
#include <cstdlib>
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

int main(int argc, char** argv) {
    if (argc < 2) return 2;
    const size_t chunk = argc > 3 ? static_cast<size_t>(atoi(argv[3])) : 16384;
    if (!CMfPcmReader::Startup()) { std::printf("Startup FALLO\n"); return 3; }
    {
        CMfPcmReader r;
        if (!r.Open(argv[1], 44100, 2)) { std::printf("Open FALLO\n"); return 4; }
        std::printf("duracion_frames=%lld duracion_ms=%lld\n", (long long)r.DurationFrames(), (long long)(r.DurationFrames() * 1000 / 44100));
        std::vector<uint8_t> buf(chunk);
        FILE* out = argc > 2 && argv[2][0] ? std::fopen(argv[2], "wb") : nullptr;
        uint64_t total = 0;
        uint32_t crc = 0;
        int16_t  peak = 0;
        for (;;) {
            const size_t n = r.Read(buf.data(), chunk);
            if (n == 0) break;
            crc = Crc32(crc, buf.data(), n);
            for (size_t i = 0; i + 1 < n; i += 2) {
                const int16_t v = *reinterpret_cast<int16_t*>(&buf[i]);
                const int16_t a = v < 0 ? (int16_t)-v : v;
                if (a > peak) peak = a;
            }
            if (out) std::fwrite(buf.data(), 1, n, out);
            total += n;
        }
        if (out) std::fclose(out);
        std::printf("bytes=%llu frames=%llu crc32=%08X pico=%d\n", (unsigned long long)total, (unsigned long long)(total / 4), crc, peak);
        // seek a 2 s y releer 4096 bytes
        if (r.Seek(44100 * 2)) {
            const size_t n = r.Read(buf.data(), 4096);
            std::printf("seek(2s) lee %zu bytes crc=%08X\n", n, Crc32(0, buf.data(), n));
        }
    }
    CMfPcmReader::Shutdown();
    return 0;
}
