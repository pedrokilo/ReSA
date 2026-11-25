// Sonda del nucleo del WAV ampliado (WaveExtCore.h, SilentPatch WaveProfilesFix): lee un WAV por el nucleo y escribe el PCM estereo de 16 bits resultante.
// Uso: wave_ext_probe.exe <entrada.wav> <salida.pcm> [trozo en bytes de salida, por defecto 4096]
// Escribe: aceptado/rechazado, canales, bits, frecuencia, offset y tamano de los datos, bytes de salida y CRC32.
// Compilar (x86): cl /nologo /EHsc /std:c++20 wave_ext_probe.cpp /Fe:wave_ext_probe.exe

#include "../../source/game_sa/Audio/Loaders/WaveExtCore.h"

#include <cstdio>
#include <cstdlib>
#include <vector>

struct FileStream {
    FILE* f;
    size_t   Read(void* d, size_t n) { return std::fread(d, 1, n, f); }
    bool     SeekAbs(uint64_t p) { return _fseeki64(f, (long long)p, SEEK_SET) == 0; }
    uint64_t Pos() { return (uint64_t)_ftelli64(f); }
    uint64_t Size() { auto c = _ftelli64(f); _fseeki64(f, 0, SEEK_END); auto e = _ftelli64(f); _fseeki64(f, c, SEEK_SET); return (uint64_t)e; }
};

static uint32_t Crc32(uint32_t crc, const uint8_t* p, size_t n) {
    crc = ~crc;
    for (size_t i = 0; i < n; ++i) {
        crc ^= p[i];
        for (int k = 0; k < 8; ++k) crc = (crc & 1) ? 0xEDB88320u ^ (crc >> 1) : crc >> 1;
    }
    return ~crc;
}

int main(int argc, char** argv) {
    if (argc < 3) return 2;
    const size_t chunk = argc > 3 ? (size_t)atoi(argv[3]) : 4096;
    FileStream s{std::fopen(argv[1], "rb")};
    if (!s.f) { std::printf("no se abre\n"); return 3; }
    WaveExtInfo info;
    if (!WaveExtParse(s, info)) { std::printf("RECHAZADO\n"); return 1; }
    std::printf("ACEPTADO canales=%u bits=%u hz=%u offset=%llu datos=%llu\n", info.channels, info.bits, info.sampleRate, (unsigned long long)info.dataOffset, (unsigned long long)info.dataSize);
    s.SeekAbs(info.dataOffset);
    std::vector<uint8_t> scratch, out(chunk);
    FILE* o = std::fopen(argv[2], "wb");
    uint64_t total = 0;
    uint32_t crc = 0;
    for (;;) {
        const size_t n = WaveExtFill(s, info, out.data(), chunk, scratch);
        if (!n) break;
        crc = Crc32(crc, out.data(), n);
        std::fwrite(out.data(), 1, n, o);
        total += n;
    }
    std::fclose(o);
    std::printf("salida_bytes=%llu crc32=%08X\n", (unsigned long long)total, crc);
    return 0;
}
