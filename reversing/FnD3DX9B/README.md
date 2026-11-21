# FnD3DX9B: por qué el ensamblador de d3dx9_43.dll rompía CJ y los peatones (21/11/2026)

## Causa (demostrada fuera del juego)

El juego ensambla sus vertex shaders de piel/mundo en **un solo sitio**: `0x760C66`, dentro del generador de shaders de RpWorld/rpskin
(`0x75F...`..`0x760CA0`). Compone el texto con `sprintf` (ranuras `[0xC97B24]+0xF0`) en el búfer global `0xC93AF8`, con la cabecera
`vs_1_1`/`vs_2_0`/`vs_2_x` según `0xC94C00` (0xA/0x14/0x19 según `D3DCAPS9::VertexShaderVersion`; en una GPU actual sale `vs_2_x`) y llama:

    D3DXAssembleShader(0xC93AF8, len, 0 /*defines*/, 0 /*include*/, 4 /*Flags*/, &shader, 0 /*errores*/)   // 7 argumentos, stdcall (ret 1Ch)

Después usa el `ID3DXBuffer` por vtable (`[+0xC]` GetBufferPointer), llama a `RwD3D9CreateVertexShader` (`0x7FAC60`) y lo libera (`[+8]`).
Si el HRESULT es negativo no crea nada y devuelve shader nulo.

**`Flags = 4` es `D3DXSHADER_SKIPOPTIMIZATION`. `D3DXAssembleShader` de d3dx9_43.dll devuelve `D3DERR_INVALIDCALL` (0x8876086C) con cualquier bandera
que no sea `D3DXSHADER_DEBUG` (1) o `D3DXSHADER_SKIPVALIDATION` (2)** (el ensamblador antiguo del EXE la ignoraba). Comprobado con
`ctypes` contra `C:\Windows\System32\d3dx9_43.dll`: flags 0,1,2,3 -> S_OK; 4,5,6,8,16,32,... -> 0x8876086C, sin texto de error, incluso con shaders válidos.
Resultado en el juego: todos los shaders de vértices de piel quedaban sin crear (shader nulo) -> modelos con esqueleto invisibles o cortados (CJ, peatones). La
causa NO era un bytecode distinto: ni siquiera llegaba a ensamblar.

Firmas: el EXE tiene 7 argumentos en `0x7652AE` (`ret 1Ch`) y 9/9/10 en las tres de compilación (`0x76534D`, `0x765402`, `0x7654B8`), igual que la DLL; la convención y
el uso del `ID3DXBuffer` por vtable (idx 3 GetBufferPointer, 4 GetBufferSize, 2 Release) coinciden. Quien llama a `0x7652AE` es solo `0x760C66`;
a las tres de compilación no las llama nadie fuera del propio D3DX9 del EXE (efectos `0x7841AC`, `0x7933BA`: sin uso por el juego).

Otros puntos comprobados con la DLL (vs_2_0 y vs_2_x, banderas 0 y 2): `mova a0.x/xy/xyzw`, `c[a0.y + N]`, `c[3 + 5].x`, `m4x3/m3x3` con `c[a0.x+N]`, `oFog`,
escrituras parciales de `oT0.x/.y`, `expp`, `max ...wwww`: todo ensambla. Solo `mov a0.x, ...` falla en vs_2_x (error X5711: solo `mova`), pero el generador emite `mov a0` únicamente
con `vs_1_1` (`0xC94C00 == 0xA`, `0x75F56E`) y `mova` en el resto: no es problema.

## Corrección aplicada (`source/game_sa/RenderWare/rw/d3dx9math.cpp`, `D3DX9System::AssembleShader`)

- Las banderas pasan a `flags & (DEBUG|SKIPVALIDATION)` (4 -> 0). Si la validación de la DLL (más estricta que la del EXE) rechaza el código, se reintenta con
  `SKIPVALIDATION` (el controlador valida otra vez en `CreateVertexShader`) y se libera el búfer de errores del primer intento.
- Las funciones de compilación HLSL no se tocan (`SKIPOPTIMIZATION` sí es válida ahí); nadie las llama en el juego.
- Los cuatro envoltorios siguen **DESACTIVADOS por defecto** (`{.enabled = false}`). No cambia el comportamiento por defecto.

## Argumentos de línea de comandos nuevos

- `--hook=Ruta[,Ruta...]`: activa hooks que nacen desactivados (se aplica después de `--unhook*`, no cambia nada sin él). Ruta = `Categoría/Clase/Función`.
- `--d3dx9-log`: con los envoltorios activos registra por cada llamada (`NOTSA_LOG_INFO`, también en `logs/log.log`): nº de llamada, longitud, flags del EXE y de la DLL
  (`+SKIPVALIDATION` si hizo falta reintento), HRESULT, tamaño del bytecode, texto de errores y los primeros 200 bytes de la fuente. Un rechazo de validación sale como `WARN`.

## Procedimiento de prueba en juego (para el agente principal)

    gta_sa.exe --d3dx9-log --hook=Plugins/D3DX9/D3DXAssembleShader,Plugins/D3DX9/D3DXCompileShaderFromFileA,Plugins/D3DX9/D3DXCompileShaderFromFileW,Plugins/D3DX9/D3DXCompileShaderFromResourceA

(Para probar solo el que importa basta `--hook=Plugins/D3DX9/D3DXAssembleShader`. Si la ruta no coincide, el log dice `No se pudo activar '...' (--hook=): no encontrado`; se puede pasar la
categoría `Plugins/D3DX9` entera.) Mirar en el log:
1. `Hook activado '...' por línea de comandos (--hook=)`: confirma la activación.
2. Líneas `D3DX9 AssembleShader #N: ... flagsEXE=0x4 flagsDLL=0x0 hr=0x00000000 bytecode=XXX bytes errores=''`: todas con `hr=0x00000000` y bytecode > 0
   (antes del arreglo habría salido `hr=0x8876086C`, `bytecode=0`). El nº N debe parecerse al de shaders distintos que genera el juego al arrancar.
3. Cualquier `WARN ... validacion rechazada` indica un shader que la DLL valida peor que el EXE: se verá el error exacto (`errores=`) y el reintento lo cubre; si aparece
   algún `hr` negativo tras `+SKIPVALIDATION`, copiar la línea completa (lleva la fuente).
4. Mirar visualmente CJ y peatones. Si siguen mal con todos los `hr=0`, la causa sería ya bytecode distinto: sacar el log con la fuente y comparar con el ensamblador del EXE (ver abajo).

## Plan B si el bytecode difiriera de verdad

Reconstruir solo el ensamblador del EXE que usa el juego: `0x7652AE` (envoltorio) + objeto de fuente desde memoria (`0x76B89B` ctor, `0x76B940` dtor, `0x76DFD1` carga de memoria,
`0x76E5FB`/`0x76E664` errores) + ensamblador (`0x76FAEB` ctor, `0x772BB1` ejecutar, `0x771F63` dtor) = **9 rutinas** (las del compilador HLSL `0x773022/0x77EE77/0x77303B`
y los cargadores de fichero/recurso `0x76DF03/0x76DF6C` no los usa el juego: 4 rutinas que se pueden dejar). Con esta causa no hace falta.
