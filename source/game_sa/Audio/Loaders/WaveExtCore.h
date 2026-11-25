#pragma once

#include <cstdint>
#include <cstring>
#include <algorithm>

/*
 * Nucleo sin dependencias del juego del lector de WAV PCM ampliado de los user tracks (usado por CAEWaveDecoderEx).
 * SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent)): WaveDecoderSA.cpp, reescrito: recorrido de chunks hasta 'fmt ' y 'data'
 * (asi un chunk ID3/LIST antes o despues de los datos no rompe el archivo) y conversion a PCM estereo de 16 bits desde 8/16/24 bits, mono/estereo, hasta 48 kHz.
 *
 * Plantilla sobre el tipo de stream S, que ofrece: size_t Read(void*, size_t); bool SeekAbs(uint64_t); uint64_t Pos(); uint64_t Size().
 */
struct WaveExtInfo {
    uint16_t channels = 0;
    uint16_t bits = 0;
    uint32_t sampleRate = 0;
    uint32_t blockAlign = 0;  //!< canales * bits / 8 (calculado, no se fia de la cabecera)
    uint64_t dataOffset = 0;
    uint64_t dataSize = 0;    //!< multiplo de blockAlign y acotado al tamano del archivo
};

template<class S>
bool WaveExtParse(S& s, WaveExtInfo& out) {
    uint8_t hdr[12];
    if (!s.SeekAbs(0) || s.Read(hdr, 3) != 3) {
        return false;
    }
    uint64_t base = 0;
    if (std::memcmp(hdr, "ID3", 3) == 0) { // etiqueta ID3v2 al principio del archivo: tamano de 4 bytes "syncsafe" tras 10 bytes de cabecera
        uint8_t rest[7];
        if (s.Read(rest, 7) != 7) {
            return false;
        }
        base = 10 + (((uint64_t)(rest[3] & 0x7F) << 21) | ((uint64_t)(rest[4] & 0x7F) << 14) | ((uint64_t)(rest[5] & 0x7F) << 7) | (uint64_t)(rest[6] & 0x7F));
        if (rest[2] & 0x10) { // pie presente
            base += 10;
        }
    }
    if (!s.SeekAbs(base) || s.Read(hdr, 12) != 12 || std::memcmp(hdr, "RIFF", 4) != 0 || std::memcmp(hdr + 8, "WAVE", 4) != 0) {
        return false;
    }
    const uint64_t total = s.Size();
    bool haveFmt = false, haveData = false;
    uint64_t pos = base + 12;
    while (!(haveFmt && haveData)) {
        uint8_t ch[8];
        if (!s.SeekAbs(pos) || s.Read(ch, 8) != 8) {
            return false;
        }
        uint32_t size;
        std::memcpy(&size, ch + 4, 4);
        if (std::memcmp(ch, "fmt ", 4) == 0 && !haveFmt) {
            uint8_t f[40] = {};
            const uint32_t want = std::min<uint32_t>(size, 40);
            if (size < 16 || s.Read(f, want) != want) {
                return false;
            }
            uint16_t tag;
            std::memcpy(&tag, f, 2);
            std::memcpy(&out.channels, f + 2, 2);
            std::memcpy(&out.sampleRate, f + 4, 4);
            std::memcpy(&out.bits, f + 14, 2);
            if (tag == 0xFFFE) { // WAVE_FORMAT_EXTENSIBLE: solo con subformato PCM
                uint16_t sub;
                std::memcpy(&sub, f + 24, 2);
                if (size < 40 || sub != 1) {
                    return false;
                }
            } else if (tag != 1) { // no PCM (float, ADPCM, ...)
                return false;
            }
            haveFmt = true;
        } else if (std::memcmp(ch, "data", 4) == 0 && !haveData) {
            out.dataOffset = pos + 8;
            out.dataSize = size;
            haveData = true;
        }
        pos += 8 + (uint64_t)size + (size & 1); // los chunks se alinean a 2 bytes
    }
    if (out.channels < 1 || out.channels > 2 || (out.bits != 8 && out.bits != 16 && out.bits != 24) || out.sampleRate < 1 || out.sampleRate > 48000) {
        return false;
    }
    out.blockAlign = (uint32_t)out.channels * out.bits / 8;
    if (total > out.dataOffset) { // acota el chunk a lo que hay en el archivo (cabeceras de streaming con tamano 0xFFFFFFFF o archivos cortados)
        out.dataSize = std::min<uint64_t>(out.dataSize, total - out.dataOffset);
    } else {
        out.dataSize = 0;
    }
    out.dataSize -= out.dataSize % out.blockAlign;
    return true;
}

// Lee hasta outBytes/4 fotogramas y los convierte a PCM estereo de 16 bits; devuelve los bytes de salida. `scratch` es memoria de trabajo reutilizable.
template<class S, class Vec>
size_t WaveExtFill(S& s, const WaveExtInfo& info, void* dest, size_t outBytes, Vec& scratch) {
    const uint64_t dataEnd = info.dataOffset + info.dataSize;
    const uint64_t pos = s.Pos();
    if (pos < info.dataOffset || pos >= dataEnd) {
        return 0;
    }
    size_t frames = outBytes / 4;
    frames = (size_t)std::min<uint64_t>(frames, (dataEnd - pos) / info.blockAlign);
    if (frames == 0) {
        return 0;
    }
    scratch.resize(frames * info.blockAlign);
    const size_t got = s.Read(scratch.data(), frames * info.blockAlign) / info.blockAlign;
    const uint8_t* in = reinterpret_cast<const uint8_t*>(scratch.data());
    int16_t* o = reinterpret_cast<int16_t*>(dest);
    auto sample = [&](const uint8_t*& p) -> int16_t {
        switch (info.bits) {
        case 8:  return (int16_t)(((int)*p++ - 128) << 8);
        case 16: { int16_t v; std::memcpy(&v, p, 2); p += 2; return v; }
        default: { int16_t v; std::memcpy(&v, p + 1, 2); p += 3; return v; } // 24 bits: los 16 mas significativos (como SilentPatch)
        }
    };
    for (size_t i = 0; i < got; ++i) {
        const int16_t l = sample(in);
        const int16_t r = info.channels == 2 ? sample(in) : l;
        *o++ = l;
        *o++ = r;
    }
    return got * 4;
}
