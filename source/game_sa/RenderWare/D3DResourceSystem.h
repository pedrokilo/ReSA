/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "D3DTextureBuffer.h"
#include "D3DIndexDataBuffer.h"
#ifdef _DX9_SDK_INSTALLED
#include "d3d9.h"
#endif

class D3DResourceSystem {
public:
    static bool &UseD3DResourceBuffering;
    static uint32 &FreeTextureBufferIndex;      // 0xC87FE0
    static D3DTextureBuffer &TextureBuffer;     // 0xC87C68 (= TextureBuffers[0])
    static D3DIndexDataBuffer &IndexDataBuffer; // 0xC87FC8
    static uint32 &NumTextureBuffers;           // 0xC87C60
    static D3DTextureBuffer (&TextureBuffers)[17];     // 0xC87C68 (CreateTexture 0x730510 admite hasta 17)
    static D3DIndexDataBuffer (&IndexDataBuffers)[16]; // 0xC87E48 (Init 0x730830)
    static uint32 &FreeIndexBufferIndex;        // 0xC87FE4

    static void InjectHooks();
    static void CancelBuffering();
    static uint32 GetTotalIndexDataSize();
    static uint32 GetTotalPixelsSize();
    static void Init();
    static void SetUseD3DResourceBuffering(bool bUse);
    static void Shutdown();
    static void TidyUpD3DIndexBuffers(uint32 count);
    static void TidyUpD3DTextures(uint32 count);
    static int32 CreateIndexBuffer(uint32 numIndices, uint32 format, void** ppIndexBuffer);
    static int32 CreateTexture(int32 width, int32 height, uint32 format, void** ppTexture);
    static void DestroyIndexBuffer(void* pIndexBuffer);
    static void DestroyTexture(void* texture);
};
