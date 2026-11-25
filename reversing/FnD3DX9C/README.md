# FnD3DX9C: el independiente sin DLL de D3DX9 (24/11/2026, rama `enlazar-d3dx9`)

Informe completo: [docs/RenderWareExe/d3dx9-enlace.md](../../docs/RenderWareExe/d3dx9-enlace.md).

Qué hay aquí (todo reproducible; las salidas pesadas van a `out/`, ignorado por git):

| Archivo | Para qué |
|---|---|
| `banco_matematica.cpp`, `build_banco.cmd` | banco de `D3DX9NativeMath` contra `d3dx9_43.dll` del sistema (millones de pares + los 65.536 medios flotantes) |
| `banco_ensamblador.cpp`, `extraer_generador.py`, `build_ensamblador.cmd` | banco de `D3DX9NativeAssembler` contra `D3DXAssembleShader`; usa el generador del juego extraído de `rpworld.cpp` |
| `volcar_dll.cpp`, `build_volcar.cmd` | vuelca el código que ejecuta cada exportación de `d3dx9_43.dll` tras seguir `jmp [tabla]` (se desensambló con capstone) |
| `espia_bloqueo.c`, `build_espia.cmd` | variante del espía de archivos de `FnSinOriginal` que puede hacer inexistentes (`STATUS_OBJECT_NAME_NOT_FOUND`) los nombres de `ESPIA_BLOQUEAR` |
| `crear_carpeta_d3dx.ps1` | crea `D:\Original\_d3dx_run` (junctions de solo lectura a los datos; `-ConDll` copia las DLL del cab del SDK con comprobación de firma) |
| `correr_d3dx.py` | ejecuta `gta_sa_standalone.exe` con el espía, vuelca los módulos (`EnumProcessModulesEx`) y captura a los 20/60/120 s |
| `cadena_pruebas.sh`, `cadena2.sh` | las ejecuciones de `ejecuciones/` (nombres = etiquetas) |
| `comparar_capturas.py` | diferencia de dos PNG (identicos, media, máximo, rejilla 8x6) |
| `enlazar_asi.py/.cmd` | enlaza una copia del ASI con los `.obj` de `wv-int` y los de `reversing\FnFase8\over\` (sin recompilar el proyecto) |
| `gta_sa.set.base` | `gta_sa.set` base que `correr_d3dx.py` restaura antes de cada ejecución (el juego lo reescribe al salir) |
| `ejecuciones/` | resumen, log del juego, espía y módulos de cada ejecución |

Argumentos nuevos del exe/ASI: `--d3dx9-dll` (usa siempre la DLL: comportamiento anterior), `--d3dx9-verificar` (propio + DLL y compara bit a bit; carga la DLL), `--d3dx9-log` (contadores y, en modo DLL/verificar, el texto de cada shader).

Reproducir los bancos: `cmd /c build_banco.cmd && out\banco_matematica.exe 100000`, `cmd /c build_ensamblador.cmd && out\banco_ensamblador.exe 3000`.
