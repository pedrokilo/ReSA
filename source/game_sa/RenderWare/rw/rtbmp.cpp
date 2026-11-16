#include "StdInc.h"

#include "rtbmp.h"

namespace {
// Plugin de errores de RtBMP: pluginID literal 0x19D del EXE (RWERROR del SDK: RwError { pluginID, errorCode = _rwerror(codigo [, argumento]) } + RwErrorSet 0x808820).
// _rwerror (0x8088D0) se llama por direccion para conservar la secuencia de colaboradores.
constexpr RwInt32 kRtBMPPluginID = 0x19D;
// Pista de memoria del buffer de fila: 0x10000 (rwMEMHINTDUR_FUNCTION).
constexpr RwUInt32 kRtBMPRowHint = 0x10000;

void RaiseRtBMPError(RwInt32 code) {
    RwError error;
    error.pluginID  = kRtBMPPluginID;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code);
    RwErrorSet(&error);
}

void RaiseRtBMPError(RwInt32 code, RwInt32 argument) {
    RwError error;
    error.pluginID  = kRtBMPPluginID;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code, argument);
    RwErrorSet(&error);
}

RwUInt32 Load32(const RwUInt8* p) { RwUInt32 v; memcpy(&v, p, 4); return v; }
RwUInt16 Load16(const RwUInt8* p) { RwUInt16 v; memcpy(&v, p, 2); return v; }

// Tras un error de lectura: libera el buffer de fila, destruye la imagen y cierra el flujo (en este orden, como el EXE).
RwImage* FailAfterRow(RwUInt8* rowBuffer, RwImage* image, RwStream* stream) {
    RwFree(rowBuffer);
    RwImageDestroy(image);
    RwStreamClose(stream, nullptr);
    return nullptr;
}
} // namespace

void RtBMPInjectHooks() {
    RH_ScopedNamespaceName("RtBMP");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RtBMPImageRead, 0x7CDF60);
    RH_ScopedGlobalInstall(_rtBMPImageSetRow, 0x7CE5D0);
}

RwImage* RtBMPImageWrite(RwImage* image, const RwChar* imageName) {
    return ((RwImage*(__cdecl *)(RwImage*, const RwChar*))0x7CE990)(image, imageName);
}

// 0x7CDF60: lee un BMP. Abre el fichero (RwStreamOpen tipo 2 = nombre de fichero, acceso 1 = lectura), lee 4 bytes ('BM' en los 2 primeros), 14 bytes (la cabecera del fichero menos
// el principio del tamano: bfOffBits en +6 y el tamano de la cabecera de informacion en +10) y biSize-4 bytes de cabecera de informacion en el mismo buffer local (desde 2 bytes antes
// del anterior, igual que la pila del EXE: 0x400 bytes, el maximo de una paleta de 256 entradas de 4 bytes). biSize == 12 es OS/2 (campos de 16 bits, sin compresion); si no, ancho/alto
// en +0/+4, bits en +10, compresion en +12 (2 = RLE4: RWERROR(0x80000009)) y colores usados en +28 (si es <= 0 o supera 1 << bits, se usa 1 << bits).
// Profundidad de la imagen: 4 con 1 o 4 bits, 8 con 8 bits, 32 en los demas. Con 8 bits o menos lee la paleta (3 bytes por entrada en OS/2, 4 con biSize == 40; otro tamano:
// RWERROR(0x80000009) y NULL sin destruir la imagen) como RGBA (R = tercer byte, B = primero, A = 255); salta hasta bfOffBits. Fila temporal de ((ancho+7)&~7) * ((bits+7)/8) bytes
// (RwMalloc, pista 0x10000; fallo: RWERROR(0x80000013, tamano)). Compresion 0, o 24/32 bits: lee filas de ((bits*ancho+7)/8 redondeado a 4) bytes de abajo arriba y las convierte con
// _rtBMPImageSetRow (flag 0). Compresion 1 (RLE8): corridas, absolutos (con relleno a par) y fin de linea/imagen (delta, codigo 2: error); cada linea se convierte con flag 1.
// Otra compresion: RWERROR(0x80000009). Los errores de lectura de filas liberan el buffer, destruyen la imagen y cierran el flujo; los de cabecera/paleta solo cierran.
RwImage* RtBMPImageRead(const RwChar* imageName) {
    RwUInt32 signature;
    alignas(4) RwUInt8 work[0x400];

    RwStream* const stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, imageName);
    if (stream == nullptr) {
        return nullptr;
    }
    if (RwStreamRead(stream, &signature, 4) != 4) {
        RwStreamClose(stream, nullptr);
        return nullptr;
    }
    if ((signature & 0xFFFF) != 0x4D42) {
        RwStreamClose(stream, nullptr);
        return nullptr;
    }
    if (RwStreamRead(stream, work + 2, 14) != 14) {
        RwStreamClose(stream, nullptr);
        return nullptr;
    }
    const RwUInt32 offBits    = Load32(work + 8);
    const RwUInt32 headerSize = Load32(work + 12);
    const RwInt32  infoBytes  = static_cast<RwInt32>(headerSize) - 4;
    if (RwStreamRead(stream, work, static_cast<RwUInt32>(infoBytes)) != static_cast<RwUInt32>(infoBytes)) {
        RwStreamClose(stream, nullptr);
        return nullptr;
    }
    RwInt32 consumed = infoBytes + 0x12;

    RwInt32 width;
    RwInt32 height;
    RwInt16 bitsWord;
    RwInt32 compression;
    RwInt32 numColors = 0;
    bool recomputeColors = true;
    if (headerSize == 0xC) {
        width       = static_cast<RwInt16>(Load16(work + 0));
        height      = static_cast<RwInt16>(Load16(work + 2));
        bitsWord    = static_cast<RwInt16>(Load16(work + 6));
        compression = 0;
    } else {
        width       = static_cast<RwInt32>(Load32(work + 0));
        height      = static_cast<RwInt32>(Load32(work + 4));
        const RwInt32 colorsUsed = static_cast<RwInt32>(Load32(work + 0x1C));
        compression = static_cast<RwInt32>(Load32(work + 0xC));
        bitsWord    = static_cast<RwInt16>(Load16(work + 0xA));
        if (compression == 2) {
            RaiseRtBMPError(static_cast<RwInt32>(0x80000009));
            RwStreamClose(stream, nullptr);
            return nullptr;
        }
        if (colorsUsed > 0) {
            const RwInt32 bitsInt = bitsWord;
            if (colorsUsed <= (1 << (bitsInt & 31))) {
                numColors = colorsUsed;
                recomputeColors = false;
            }
        }
    }
    const RwInt32 bitCount = bitsWord;
    if (recomputeColors) {
        numColors = 1 << (bitCount & 31);
    }

    RwInt32 depth;
    if (bitCount == 1 || bitCount == 4) {
        depth = 4;
    } else if (bitCount == 8) {
        depth = 8;
    } else {
        depth = 32;
    }
    RwImage* const image = RwImageCreate(width, height, depth);
    if (image == nullptr) {
        RwStreamClose(stream, nullptr);
        return nullptr;
    }
    if (RwImageAllocatePixels(image) == nullptr) {
        RwImageDestroy(image);
        RwStreamClose(stream, nullptr);
        return nullptr;
    }

    if (bitsWord <= 8) {
        RwUInt8* const palette = reinterpret_cast<RwUInt8*>(image->palette);
        if (headerSize == 0xC) {
            const RwInt32 bytes = numColors * 3;
            if (RwStreamRead(stream, work, static_cast<RwUInt32>(bytes)) != static_cast<RwUInt32>(bytes)) {
                RwStreamClose(stream, nullptr);
                return nullptr;
            }
            consumed += bytes;
            for (RwInt32 i = 0; i < numColors; ++i) {
                palette[i * 4 + 0] = work[i * 3 + 2];
                palette[i * 4 + 1] = work[i * 3 + 1];
                palette[i * 4 + 2] = work[i * 3 + 0];
                palette[i * 4 + 3] = 0xFF;
            }
        } else if (headerSize == 0x28) {
            const RwInt32 bytes = numColors * 4;
            if (RwStreamRead(stream, work, static_cast<RwUInt32>(bytes)) != static_cast<RwUInt32>(bytes)) {
                RwStreamClose(stream, nullptr);
                return nullptr;
            }
            consumed += bytes;
            for (RwInt32 i = 0; i < numColors; ++i) {
                palette[i * 4 + 0] = work[i * 4 + 2];
                palette[i * 4 + 1] = work[i * 4 + 1];
                palette[i * 4 + 2] = work[i * 4 + 0];
                palette[i * 4 + 3] = 0xFF;
            }
        } else {
            RaiseRtBMPError(static_cast<RwInt32>(0x80000009));
            RwStreamClose(stream, nullptr);
            return nullptr;
        }
    }
    if (RwStreamSkip(stream, offBits - static_cast<RwUInt32>(consumed)) == nullptr) {
        RwImageDestroy(image);
        RwStreamClose(stream, nullptr);
        return nullptr;
    }

    // Fila temporal: ((ancho+7)&~7) * ((bits+7)/8) bytes; paso de fila de disco: ((bits*ancho+7)/8) redondeado a 4 (divisiones con signo, como cdq/and/add/sar del EXE).
    const RwInt32 rowBitsPlus7 = bitCount * width + 7;
    const RwInt32 rowBitsAdj   = rowBitsPlus7 + (rowBitsPlus7 < 0 ? 7 : 0);
    const RwInt32 bytesPerPixelNum = bitCount + 7;
    const RwInt32 bytesPerPixel = (bytesPerPixelNum + (bytesPerPixelNum < 0 ? 7 : 0)) >> 3;
    const RwInt32 rowBufferSize = ((width + 7) & ~7) * bytesPerPixel;
    const RwInt32 stride = ((rowBitsAdj >> 3) + 3) & ~3;
    RwUInt8* const rowBuffer = static_cast<RwUInt8*>(RwMalloc(static_cast<RwUInt32>(rowBufferSize), kRtBMPRowHint));
    if (rowBuffer == nullptr) {
        RaiseRtBMPError(static_cast<RwInt32>(0x80000013), rowBufferSize);
        RwImageDestroy(image);
        RwStreamClose(stream, nullptr);
        return nullptr;
    }

    const bool uncompressed = compression == 0 || bitsWord == 0x18 || bitsWord == 0x20;
    if (!uncompressed) {
        if (compression != 1) {
            RaiseRtBMPError(static_cast<RwInt32>(0x80000009));
            RwFree(rowBuffer);
            RwImageDestroy(image);
            RwStreamClose(stream, nullptr);
            return nullptr;
        }
        // RLE8.
        RwInt32 y = 0;
        if (height > 0) {
            for (;;) {
                RwInt32 x = 0; // 0x7CE384: inicio de linea
                for (;;) {     // 0x7CE386: siguiente par sin tocar x
                    RwUInt8 pair[2];
                    if (RwStreamRead(stream, pair, 2) != 2) {
                        return FailAfterRow(rowBuffer, image, stream);
                    }
                    if (pair[0] != 0) {
                        memset(rowBuffer + x, pair[1], pair[0]);
                        x += pair[0];
                        continue;
                    }
                    if (pair[1] > 2) {
                        const RwUInt32 n = pair[1];
                        if (RwStreamRead(stream, rowBuffer + x, n) != n) {
                            return FailAfterRow(rowBuffer, image, stream);
                        }
                        x += static_cast<RwInt32>(n);
                        if ((pair[1] & 1) != 0) {
                            RwUInt8 pad[2];
                            if (RwStreamRead(stream, pad, 1) != 1) {
                                return FailAfterRow(rowBuffer, image, stream);
                            }
                        }
                        continue;
                    }
                    if (pair[1] == 2) {
                        return FailAfterRow(rowBuffer, image, stream);
                    }
                    if (pair[1] == 1) {
                        y = height - 1;
                    }
                    break;
                }
                _rtBMPImageSetRow(image, height - y - 1, rowBuffer, bitCount, 1);
                ++y;
                if (!(y < height)) {
                    break;
                }
            }
        }
    } else {
        for (RwInt32 i = 0, row = height - 1; i < height; ++i, --row) {
            if (RwStreamRead(stream, rowBuffer, static_cast<RwUInt32>(stride)) != static_cast<RwUInt32>(stride)) {
                return FailAfterRow(rowBuffer, image, stream);
            }
            _rtBMPImageSetRow(image, row, rowBuffer, bitCount, 0);
        }
    }
    RwFree(rowBuffer);
    RwStreamClose(stream, nullptr);
    return image;
}

// 0x7CE5D0: copia una fila de BMP (bitCount bits por pixel, `source`) a la fila `row` de la imagen (destino = cpPixels + stride*row, image->width pixeles; paleta image->palette).
// Clave = profundidad de la imagen | (bitCount << 8): 0x808 copia bytes; 0x404/0x408 y 0x104/0x108 desempaquetan nibbles (empezando por el alto) o bits a bytes de indice (leen
// el primer byte antes de mirar el ancho y uno mas al terminar cada byte); 0x120/0x420/0x820 pasan por la paleta a pixeles de 4 bytes; 0xF20 (5-5-5) y 0x1020 (5-6-5) expanden
// a R,G,B,255 con los bits bajos a cero (B = byte bajo << 3); 0x1820 (24 bits): con flag != 0 copia los tres bytes tal cual y alfa 255, con flag 0 invierte el orden; 0x2020 (32 bits)
// copia los tres primeros bytes tal cual y alfa 255 (salta el cuarto). Cualquier otra clave: RWERROR(0x80000008) y FALSE. Devuelve TRUE.
RwBool _rtBMPImageSetRow(RwImage* image, RwInt32 row, const RwUInt8* source, RwInt32 bitCount, RwInt32 flag) {
    const RwInt32 width = image->width;
    const RwUInt32* const palette = reinterpret_cast<const RwUInt32*>(image->palette);
    RwUInt8* dst = image->cpPixels + image->stride * row;
    const RwUInt32 key = static_cast<RwUInt32>(image->depth) | (static_cast<RwUInt32>(bitCount) << 8);
    const RwUInt8* src = source;

    switch (key) {
    case 0x808:
        memcpy(dst, src, static_cast<RwUInt32>(width));
        return TRUE;
    case 0x404:
    case 0x408: {
        RwUInt8 cur = *src++;
        RwInt32 shift = 4;
        for (RwInt32 n = width; n != 0; --n) {
            *dst++ = static_cast<RwUInt8>((cur >> shift) & 0x0F);
            shift -= 4;
            if (shift < 0) {
                cur = *src++;
                shift = 4;
            }
        }
        return TRUE;
    }
    case 0x104:
    case 0x108: {
        RwUInt8 cur = *src++;
        RwInt32 shift = 7;
        for (RwInt32 n = width; n != 0; --n) {
            *dst++ = static_cast<RwUInt8>((cur >> shift) & 1);
            --shift;
            if (shift < 0) {
                cur = *src++;
                shift = 7;
            }
        }
        return TRUE;
    }
    case 0x120: {
        RwUInt8 cur = *src++;
        RwInt32 shift = 7;
        RwUInt32* out = reinterpret_cast<RwUInt32*>(dst);
        for (RwInt32 n = width; n != 0; --n) {
            *out++ = palette[(cur >> shift) & 1];
            --shift;
            if (shift < 0) {
                cur = *src++;
                shift = 7;
            }
        }
        return TRUE;
    }
    case 0x420: {
        RwUInt8 cur = *src++;
        RwInt32 shift = 4;
        RwUInt32* out = reinterpret_cast<RwUInt32*>(dst);
        for (RwInt32 n = width; n != 0; --n) {
            *out++ = palette[(cur >> shift) & 0x0F];
            shift -= 4;
            if (shift < 0) {
                cur = *src++;
                shift = 4;
            }
        }
        return TRUE;
    }
    case 0x820: {
        RwUInt32* out = reinterpret_cast<RwUInt32*>(dst);
        for (RwInt32 n = width; n != 0; --n) {
            *out++ = palette[*src++];
        }
        return TRUE;
    }
    case 0xF20:
        for (RwInt32 n = width; n != 0; --n) {
            const RwUInt32 v = Load16(src);
            src += 2;
            dst[3] = 0xFF;
            dst[0] = static_cast<RwUInt8>((v >> 7) & 0xF8);
            dst[1] = static_cast<RwUInt8>((v >> 2) & 0xF8);
            dst[2] = static_cast<RwUInt8>(v << 3);
            dst += 4;
        }
        return TRUE;
    case 0x1020:
        for (RwInt32 n = width; n != 0; --n) {
            const RwUInt32 v = Load16(src);
            src += 2;
            dst[3] = 0xFF;
            dst[0] = static_cast<RwUInt8>((v >> 8) & 0xF8);
            dst[1] = static_cast<RwUInt8>((v >> 3) & 0xFC);
            dst[2] = static_cast<RwUInt8>(v << 3);
            dst += 4;
        }
        return TRUE;
    case 0x1820:
        if (flag != 0) {
            for (RwInt32 n = width; n != 0; --n) {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[3] = 0xFF;
                dst[2] = src[2];
                src += 3;
                dst += 4;
            }
        } else {
            for (RwInt32 n = width; n != 0; --n) {
                dst[2] = src[0];
                dst[1] = src[1];
                dst[3] = 0xFF;
                dst[0] = src[2];
                src += 3;
                dst += 4;
            }
        }
        return TRUE;
    case 0x2020:
        for (RwInt32 n = width; n != 0; --n) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[3] = 0xFF;
            dst[2] = src[2];
            src += 4;
            dst += 4;
        }
        return TRUE;
    default:
        RaiseRtBMPError(static_cast<RwInt32>(0x80000008));
        return FALSE;
    }
}
