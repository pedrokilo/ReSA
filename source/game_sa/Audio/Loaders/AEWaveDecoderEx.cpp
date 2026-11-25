// NOTSA: decoder de WAV PCM ampliado (SilentPatch, MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent): WaveDecoderSA.cpp). El nucleo esta en WaveExtCore.h.
#include "StdInc.h"

#include "AEWaveDecoderEx.h"

namespace {
// Adaptador de CAEDataStream (posiciones logicas de la pista) al nucleo.
struct StreamAdapter {
    CAEDataStream* s;
    size_t   Read(void* d, size_t n) { return s->FillBuffer(d, n); }
    bool     SeekAbs(uint64_t p) { s->Seek((long)p, SEEK_SET); return true; }
    uint64_t Pos() { return s->GetCurrentPosition(); }
    uint64_t Size() { return s->m_nLength; }
};
} // namespace

CAEWaveDecoderEx::CAEWaveDecoderEx(CAEDataStream* dataStream) : CAEStreamingDecoder(dataStream) {}

bool CAEWaveDecoderEx::IsCanonicalHeader(CAEDataStream* dataStream) {
    char h[44];
    StreamAdapter a{dataStream};
    a.SeekAbs(0);
    const bool ok = a.Read(h, 44) == 44;
    a.SeekAbs(0);
    if (!ok || memcmp(h, "RIFF", 4) != 0 || memcmp(h + 8, "WAVEfmt ", 8) != 0 || memcmp(h + 36, "data", 4) != 0) {
        return false;
    }
    uint32 fmtSize; memcpy(&fmtSize, h + 16, 4);
    uint16 tag, ch, bits; memcpy(&tag, h + 20, 2); memcpy(&ch, h + 22, 2); memcpy(&bits, h + 34, 2);
    uint32 dataSize; memcpy(&dataSize, h + 40, 4);
    // Con chunks (ID3, LIST...) detras de 'data' el decoder del EXE los leeria como audio al final de la pista: solo es canonico si 'data' llega hasta el final del archivo
    return fmtSize == 16 && tag == 1 && ch == 2 && bits == 16 && (uint64_t)dataSize + 44 >= a.Size();
}

bool CAEWaveDecoderEx::Initialise() {
    m_bInitialized = false;
    StreamAdapter a{m_dataStream};
    if (!WaveExtParse(a, m_info)) {
        NOTSA_LOG_INFO("User tracks: WAV ampliado rechazado (formato no PCM de 8/16/24 bits, mono/estereo, hasta 48 kHz, o cabecera invalida)");
        return false;
    }
    a.SeekAbs(m_info.dataOffset);
    m_bInitialized = true;
    const long lengthMs = GetStreamLengthMs();
    if (lengthMs < 7000) { // como el decoder original: se descartan las pistas de menos de 7 s
        m_bInitialized = false;
    }
    NOTSA_LOG_INFO("User tracks: WAV ampliado {}: {} canales, {} bits, {} Hz, datos en {} ({} bytes), {} ms",
                   m_bInitialized ? "aceptado" : "descartado (menos de 7 s)", m_info.channels, m_info.bits, m_info.sampleRate, m_info.dataOffset, m_info.dataSize, lengthMs);
    return m_bInitialized;
}

size_t CAEWaveDecoderEx::FillBuffer(void* dest, size_t size) {
    StreamAdapter a{m_dataStream};
    return WaveExtFill(a, m_info, dest, size, m_scratch);
}

long CAEWaveDecoderEx::GetStreamLengthMs() {
    if (!m_bInitialized) {
        return -1;
    }
    return (long)(m_info.dataSize / m_info.blockAlign * 1000 / m_info.sampleRate);
}

long CAEWaveDecoderEx::GetStreamPlayTimeMs() {
    if (!m_bInitialized) {
        return -1;
    }
    const uint64_t pos = m_dataStream->GetCurrentPosition();
    return pos <= m_info.dataOffset ? 0 : (long)((pos - m_info.dataOffset) / m_info.blockAlign * 1000 / m_info.sampleRate);
}

void CAEWaveDecoderEx::SetCursor(unsigned long pos) {
    if (!m_bInitialized) {
        return;
    }
    uint64_t frame = (uint64_t)pos * m_info.sampleRate / 1000;
    frame = std::min<uint64_t>(frame, m_info.dataSize / m_info.blockAlign);
    m_dataStream->Seek((long)(m_info.dataOffset + frame * m_info.blockAlign), SEEK_SET);
}

int32 CAEWaveDecoderEx::GetSampleRate() {
    return m_bInitialized ? (int32)m_info.sampleRate : -1;
}

int32 CAEWaveDecoderEx::GetStreamID() {
    return m_dataStream->m_nTrackId;
}
