# EXT1A: prueba en juego (la lanza el agente principal; en ventana, sin cerrar con taskkill)

Rama `ext1a`. Primero compilar con el candado (`D:\Original\_build_EXT1A.sh`, deja `D:\Original\wv-13\bin\Debug\gta_reversed.asi`). El juego se lanza en modo ventana (`WindowedModeHooks` siempre encendido) y se cierra desde el propio juego (menu / Alt+F4 normal), nunca con `taskkill`. Los agentes no lanzan `gta_sa.exe`.

## 1. Preparacion

1. Copiar `D:\Original\wv-13\bin\Debug\gta_reversed.asi` (y su `.pdb`) al clon de pruebas `E:\JUEGOS\GTA San Andreas - CVehicle Test\` (nunca a `E:\JUEGOS\GTA San Andreas`).
2. Comprobar que `C:\Windows\SysWOW64\d3dx9_43.dll` existe.
3. Para la parte de QuickTime, tener en `Documents\GTA San Andreas User Files\User Tracks` (o donde `sa-utrax.dat` lo indique) un `.mp3`, un `.ogg` y un `.m4a` (o `.aac`) de mas de 7 s.

## 2. Arranque sin QuickTime y radio de usuario

Arrancar el juego, llegar al menu y a una partida. Esperado en `gta_reversed.log` (o la salida de `spdlog` habitual):

- una linea `User tracks: QuickTime retirado; los .mp3 usan el decoder WMA ...` (una vez, al inicializar el audio);
- ninguna linea ni ventana de QuickTime / `QuickTime.qts` (en el original, sin QuickTime no habia error visible; con QuickTime instalado se cargaba `QuickTime.qts`: ahora nunca);
- sin error `D3DX9:` (`d3dx9_43.dll` cargada; si hubiera `no se pudo cargar` o `no exporta`, es un fallo).

En el menu de radio elegir «User Track»: el `.mp3` y el `.ogg` deben sonar (el `.mp3` por WMA); el `.m4a`/`.aac` no se carga (se salta). Cerrar el juego con normalidad y confirmar en el log que no hay excepcion al salir (`CAEMFDecoder::Shutdown` ya no hace nada).

## 3. Matematica de D3DX9 en el render

`rpworld.cpp` (matrices del vertex shader, `VSMatrixMultiply`, `VSVec3TransformCoord`, ...) llama a `D3DX9System` y a las seis funciones enganchadas (0x76626B, 0x766A32, 0x766AB7, 0x7672E1, 0x7675EE, 0x7676C7). Recorrer unos minutos Los Santos en ventana: sin parpadeos de geometria, sin NaN en el log, imagen igual que con la rama `main`.

## 4. Los cuatro envoltorios de shaders (0x7652AE, 0x76534D, 0x765402, 0x7654B8)

Ningun camino normal del juego los llama. Para probarlos en juego hay que meter una sonda temporal (NO commitear) al final de `CGame::Process` o en un `RH_Scoped...` de prueba que ejecute una sola vez:

```cpp
static bool done = false;
if (!done) { done = true;
    const char* src = "vs_1_1\ndcl_position v0\nmov oPos, v0\n";
    void *sh = nullptr, *err = nullptr, *ct = nullptr;
    const auto hrAsm  = reinterpret_cast<int32(__stdcall*)(const void*, uint32, const void*, void*, uint32, void**, void**)>(0x7652AE)(src, (uint32)strlen(src), nullptr, nullptr, 0, &sh, &err);
    // 0x76534D / 0x765402 con un .fx de prueba (HLSL "float4 main(float4 p : POSITION) : POSITION { return p; }", perfil "vs_2_0") -> mismas firmas que D3DXCompileShaderFromFileA/W
    NOTSA_LOG_INFO("EXT1A sonda: D3DXAssembleShader hr={:#x} shader={}", (uint32)hrAsm, sh != nullptr);
}
```

Esperado: `hr=0x0` y `shader=true` (en la prueba de humo fuera del juego, `reversing/FnEXT1A/resultados/humo_shaders.txt`, las tres exportaciones devuelven `S_OK` y un bufer de 32 / 124 bytes).

## 5. Registro

Ya registradas con evidencia fuera del juego (`python reversing/FnEXT1A/register_hook.py`). Si la corrida en juego sale limpia, anotarlo en el README del lote; los cuatro envoltorios de shaders y el arranque sin QuickTime siguen en `validated=0` (no hay banco par a par aplicable).
