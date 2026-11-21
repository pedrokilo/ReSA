# Auditoria de RenderWare (RwCore / RwD3D9) sin banco: lote R28A y vecinos

Auditoria de solo lectura (2026-11-21, base origin/main 9cbc6c2ce0, rama `audit-rwcore`). Para cada funcion se comparo el desensamblado del EXE
(copia identica del EXE original, SHA1 786d5a9e...) con el C++ de `source/game_sa/RenderWare/rw/rwcore.cpp`: orden de los argumentos, indice en `stdFunc`
(`RwEngineInstance + 0x48 + 4 * indice`), valor devuelto, campos escritos, constantes y orden de efectos.

Herramientas: `desens_exe.py` (desensamblador capstone del EXE: `python -B desens_exe.py 7FAF50 [7FAEC0:28 ...]`),
`triage_lib.py` + `par.py` (cuerpo C++ y desensamblado lado a lado: `python -B par.py RwRasterCreate RwTextureCreate`).

## Resultado

Ningun error nuevo: todas las funciones revisadas coinciden con el EXE (el unico fallo real, `RwRasterRenderFast`, ya estaba corregido en 85cf96a6c5).
No se cambio codigo C++, por lo que la DLL es la de main (no hubo nada que compilar ni que lanzar en el humo).

Todos los usos de `stdFunc[...]` del proyecto (59 apariciones en rwcore.cpp y rwtexdict.cpp) se revisaron uno por uno: indice y orden de los tres argumentos `(salida/entrada, entrada/salida, entero)`.

### RwRaster (lote R28A, 0x7FAEC0..0x7FB2D0)

| Funcion | Direccion | stdFunc (idx / +off) | Resultado |
|---|---|---|---|
| RwRasterUnlock | 7FAEC0 | UNLOCK 16 / +0x88, (NULL, raster, 0) | OK |
| RwRasterRenderFast | 7FAF50 | RENDERFAST 19 / +0x94, (raster, &{x,y}, 0) | OK (corregida en 85cf96a6c5) |
| RwRasterUnlockPalette | 7FAFF0 | UNLOCKPALETTE 24 / +0xA8, (NULL, raster, 0); privateFlags &= 0xE7 | OK |
| RwRasterDestroy | 7FB020 | DeInitObject(0x8E2518), DESTROY 5 / +0x5C, FreeListFree(+0x60 de los globales, +0x148); TRUE | OK |
| RwRasterPushContext | 7FB060 | SETRASTERCONTEXT 11 / +0x74, (NULL, raster, 0); sp+1 despues de la llamada | OK |
| RwRasterPopContext | 7FB110 | sp-1 antes; (NULL, pila[sp nuevo], 0); devuelve el sacado o NULL | OK |
| RwRasterGetNumLevels | 7FB160 | GETMIPLEVELS 28 / +0xB8, (&ranura, raster, 0); sin bit 7 de cFormat -> 1; fallo -> -1 | OK |
| RwRasterShowRaster | 7FB1A0 | SHOWRASTER 20 / +0x98 leido ANTES de _rwResourcesPurge; (raster, dev, flags) | OK |
| RwRasterCreate | 7FB230 | FreeListAlloc(pista 0x30407), campos +0xC/+0x10/+0x14/+0x21/+0x22/+0x1C/+0x1E/+0/+4/+8, CREATE 4 / +0x58 (NULL, raster, flags), InitObject(0x8E2518) | OK |
| RwRasterLock | 7FB2D0 | LOCK 15 / +0x84, (&ranura, raster, (nivel&0xFF)<<8 + modo) | OK |

### RwRaster (otras)

| Funcion | Direccion | Resultado |
|---|---|---|
| RwRasterRenderScaled | 7FAE80 | OK (RENDERSCALED 18 / +0x90, (raster, rect, 0)) |
| RwRasterClear | 7FAEE0 | OK (CLEAR 14 / +0x80, (NULL, NULL, valor); error 0x15 si sp <= 0) |
| RwRasterClearRect | 7FAF90 | OK (CLEARRECT 13 / +0x7C, (NULL, rect, valor)) |
| RwRasterRender | 7FAFB0 | OK (RENDER 17 / +0x8C, (raster, &{x,y}, 0)) |
| RwRasterSubRaster | 7FB1D0 | OK (SUBRASTER 12 / +0x78, (sub, raster, 0); campos +0xC/+0x10/+0x1C/+0x1E; parent) |
| RwRasterGetOffset | 7FAEA0 | OK |
| RwRasterGetCurrentContext | 7FAE60 | OK |
| RwRasterLockPalette | 7FB0E0 | OK (LOCKPALETTE 23 / +0xA4, ranura del argumento) |
| RwRasterGetPluginOffset | 7FAE40 | OK |
| RwRasterSetFromImage | 804290 | OK (RASTERSETIMAGE 7 / +0x64; image.flags&2 -> raster.privateFlags\|=1) |
| RwImageSetFromRaster | 804250 | OK (IMAGEGETRASTER 6 / +0x60) |
| RwImageFindRasterFormat | 8042C0 | OK (IMAGEFINDRASTERFORMAT 9 / +0x6C; salidas ancho/alto/prof./formato en el orden del EXE) |
| RwRasterRead / RwRasterReadMaskedRaster | 8043F0 / 8044E0 | OK (la profundidad sin inicializar del EXE en el camino de fallo se pone a 0: sin efecto observable) |
| RwRGBAToPixel | 803740 | OK (RGBTOPIXEL 2 / +0x50, ranura del argumento) |

### RwCamera

| Funcion | Direccion | Resultado |
|---|---|---|
| _rwCameraBeginUpdateCallBack | 7EF370 | OK (CAMERABEGINUPDATE 1 / +0x4C) |
| _rwCameraEndUpdateCallBack | 7EF340 | OK (CAMERAENDUPDATE 10 / +0x70) |
| RwCameraClear | 7EE340 | OK (CAMERACLEAR 21 / +0x9C, (camara, color, modo)) |
| RwCameraSetNearClipPlane / SetFarClipPlane | 7EE1D0 / 7EE2A0 | OK |
| RwCameraSetProjection | 7EE3A0 | OK |
| RwCameraCreate | 7EE4F0 | OK (todos los campos y el orden con _rwCameraUpdateZScale) |
| RwCameraDestroy | 7EE4B0 | OK |
| _rwCameraUpdateZScale | 7EE200 | Parcial: argumentos y campos (+0x80/+0x84/+0x8C/+0x90) OK; la aritmetica x87 no se reviso (delega en CameraZScaleCpp) |

### RwTexture / RwTexDictionary

| Funcion | Direccion | Resultado |
|---|---|---|
| RwTextureSetRaster | 7F35D0 | OK (TEXTURESETRASTER 8 / +0x68) |
| RwTextureSetName / RwTextureSetMaskName | 7F38A0 / 7F3910 | OK |
| RwTextureCreate / RwTextureDestroy | 7F37C0 / 7F3820 | OK |
| RwTexDictionaryCreate / Destroy / ForAllTextures | 7F3600 / 7F36A0 / 7F3730 | OK |
| RwTexDictionaryAddTexture / RemoveTexture / FindNamedTexture | 7F3980 / 7F39C0 / 7F39F0 | OK |
| TexDictionarySizeTextureCallBack / TexDictionaryWriteTextureCallBack | 804990 / 804B70 | OK (NATIVETEXTUREGETSIZE 25 / +0xAC, NATIVETEXTUREWRITE 27 / +0xB4; orden de los argumentos y del encabezado del chunk) |

### RwImage

| Funcion | Direccion | Resultado |
|---|---|---|
| RwImageCreate / RwImageDestroy | 8026E0 / 802740 | OK |
| RwImageAllocatePixels / RwImageFreePixels | 8027A0 / 802860 | OK |
| RwImageMakeMask | 802A20 | OK |
| RwImageApplyMask | 802AF0 | Parcial: validaciones y primera mitad (0x802AF0..0x802BD9) OK; el resto no se reviso |

### RwD3D9 del lote R28A (vecinos)

| Funcion | Direccion | Resultado |
|---|---|---|
| RwD3D9SetRenderState / GetRenderState | 7FC2D0 / 7FC320 | OK |
| RwD3D9SetTextureStageState | 7FC340 | OK (indice etapa*33+tipo) |
| RwD3D9SetSamplerState | 7FC3C0 | OK (ranura COM 69, indice tipo+14*etapa) |
| RwD3D9SetTexture | 7FDE70 | OK (direcciones, filtros 6/5/7, anisotropia, camino sin textura) |
| RwD3D9SetSurfaceProperties | 7FC4D0 | OK (ramas, orden de los estados 0x8B/0x8D/0x93/0x94, material y SetMaterial) |
| _rwD3D9LightsEqual | 7FA8B0 | OK |
| RwD3D9DeleteVertexDeclaration | 7FAC10 | OK |
| RwD3D9CameraIsSphereFullyInsideFrustum | 7FAD30 | OK |
| _rwD3D9DeviceSetRestoreCallback | 7FAE20 | OK |
| _rwD3D9Im2DRenderOpen / Close / Begin | 7FB480 / 7FB5F0 / 7FB810 | OK |
| _rwD3D9PixelConvertL16 | 7FFEE0 | OK |

## No revisado a fondo (pendiente)

_rwD3D9RenderStateOpen (7FCAC0) y _rwD3D9RenderStateReset (7FD100) (inicializacion de caches, 1,6 y 1,8 KB), _rwD3D9ImageFindRasterFormat (7FFFF0), la aritmetica de las funciones de
frustum y matrices de RwCamera, RwFrame*, RwTextureRead / _rwTextureReadMipmapped, los conversores de pixel de R29A+, Rx*/RxPipeline*, RwPalQuant*, RwImageResample y el resto de RwCore/RwPlCore con validated=0.
Un triaje automatico (direcciones absolutas del EXE frente a las del C++) se descarto: demasiado ruido por los globales de modulo (RwEngineInstance, offsets de modulo).
Un comparador por argumentos tampoco sirve: el compilador del EXE agrupa los `add esp, N` de llamadas consecutivas.
