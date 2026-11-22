# Parte 6 (huecos del `.text`): clasificación y cierre estático

Rama `vivo-p6` (desde `origin/main` 410d9f2d84). Alcance: los 270 huecos del `.text` con código referenciado pero sin función de Ghidra (lotes H1–H6 de [plan-lotes.md](plan-lotes.md), `huecos.csv`, 128.357 bytes). Todo es estático (capstone sobre el EXE canónico) salvo el enganche nuevo, que se compila y se prueba con el humo del proyecto. **No se validó ninguna función una a una** (regla del usuario); el MCP `rea` no se usó.

Reproducir: `python -X utf8 contrib/huecos_p6.py` (lee `huecos.csv`, `funciones-vivas.csv`, `docs/MemoryMap/binary_function_ranges.csv`, los CSV de estado, `docs/hooks.csv` y `source/`; escribe `huecos-p6.csv`, `cierre-p6.csv`, `huecos-p6-resumen.csv` y el estado de H1–H6 en `plan.csv` y `plan-lotes.md`).

## 1. Qué es cada hueco

Un hueco no es una función: es un tramo de bytes entre dos funciones de Ghidra, y casi siempre contiene **varias funciones pequeñas separadas por relleno** (`nop`/`int3` de alineación a 16). Se partió cada hueco en **914 piezas** (118.544 bytes de código sin relleno): tramos de instrucciones entre rellenos, fundidos solo si el primero no acaba en `ret`/`jmp` y nadie apunta al segundo. Las piezas son en su mayoría funciones C++ reales que Ghidra no creó porque solo las alcanzan callbacks, vtables, tablas y manejadores de excepciones:

* callbacks de `__vec_ctor`/`__vec_dtor` y de `atexit` (`push offset f` seguido de la llamada del CRT): destructores vacíos (`ret`), constructores triviales (`mov eax,ecx; ret`);
* callbacks de fin de animación (`mov eax,[esp+8]; mov byte [eax+8],1; ...; ret`), pasados con `SetFinishCallback`;
* continuaciones de `catch` (`mov eax, X; ret` y el cuerpo que sigue en `X`): colas de otra función;
* funciones de tareas/RenderWare con una sola entrada indirecta;
* los dos despachadores originales de comandos de script (0x47C100 y 0x47D210, ranuras 3 y 4 de `0x8A6168`).

## 2. Método

1. **Entradas.** Todo `call`/`jmp`/`jcc` directo y todo `push`/`mov` inmediato del `.text` (funciones y huecos) y todo dword de cualquier sección del EXE (cualquier alineación) que caiga dentro de la pieza.
2. **Origen de cada entrada** (muerta o viva): función *reemplazada* o *enlazada* → muerta (su cuerpo ya no corre); función viva del grupo excepciones, D3DX9, protector, QuickTime o de decisión DESCARTAR → muerta; función viva del juego/CRT → viva; otra pieza → se propaga (punto fijo); inicializador estático → «destructor registrado por `atexit`»; dword alineado en `.data`/`.rdata` → «dato»; tabla de saltos del `.text` → neutra; tramo de un hueco «sin referencia directa» → vivo solo si algo vivo apunta a su entrada (un nivel).
3. **Cruce con el C++.** Enganchada = literal en una línea `RH_Scoped*` o fila en `docs/hooks.csv`; citada = literal hexadecimal en `source/` (también comentarios).
4. **Destino** de cada pieza, por este orden: `reconstruida` (enganchada), `crt-enlazar` (viva en el CRT, se enlaza el CRT de MSVC), `destructor-atexit`, `citada-en-cpp` (el C++ cita la dirección y ya no la usa), `pendiente-dato` (viva por tabla/dato y sin C++), `muerta-por-reemplazo`, `sin-referencia-real` (nada la alcanza: relleno, datos o restos).
5. **Cierre de hueco:** cerrado si ninguna pieza queda `pendiente-dato`; su clase es la dominante en bytes.

Límites: la liveness se propaga un nivel por los huecos sin referencia directa y no resuelve tablas de punteros (se ve el dword, no quién lo lee); `reconstruida` y `citada-en-cpp` se deciden por la dirección de entrada, no comparando cuerpos. Una pieza `pendiente-dato` puede ser falso positivo (tabla sin lector vivo); las `muerta-*` y `sin-referencia-*` son seguras mientras sean ciertas las listas de funciones reemplazadas.

## 3. Cifras

Piezas (914, 118.544 bytes):

| Destino | Piezas | Bytes |
| --- | ---: | ---: |
| reconstruida (enganchada por una dirección de entrada) | 278 | 41.060 |
| muerta-por-reemplazo | 91 | 27.203 |
| citada-en-cpp | 175 | 22.651 |
| pendiente-dato | 23 | 18.208 |
| sin-referencia-real | 208 | 7.036 |
| destructor-atexit | 134 | 1.657 |
| crt-enlazar | 5 | 729 |

Huecos (270): **259 cerrados, 11 con piezas pendientes**.

| Clase del hueco | Región | Huecos | Bytes | Cerrados |
| --- | --- | ---: | ---: | ---: |
| reconstruida | juego | 31 | 6.676 | 31 |
| reconstruida | RenderWare | 60 | 40.689 | 60 |
| reconstruida | CRT | 4 | 609 | 4 |
| citada-en-cpp | juego | 61 | 18.127 | 61 |
| citada-en-cpp | RenderWare | 5 | 2.460 | 5 |
| muerta-por-reemplazo | juego | 22 | 13.135 | 22 |
| muerta-por-reemplazo | RenderWare | 21 | 13.740 | 21 |
| muerta-por-reemplazo | CRT | 1 | 104 | 1 |
| sin-referencia-real | juego | 28 | 7.388 | 28 |
| sin-referencia-real | RenderWare | 4 | 2.094 | 4 |
| destructor-atexit | juego | 16 | 642 | 16 |
| destructor-atexit | CRT | 2 | 2.468 | 2 |
| crt-enlazar | CRT | 4 | 362 | 4 |
| **pendiente-dato** | juego | 3 | 12.634 | 0 |
| **pendiente-dato** | RenderWare | 7 | 7.086 | 0 |
| **pendiente-dato** | CRT | 1 | 143 | 0 |

Por lote: H1 42/45, H2 45/45, H3 45/45, H4 42/45, H5 42/45, H6 43/45 cerrados (`cierre-p6.csv` lleva la columna `lote`).

## 4. Lo reconstruido en esta parte

* **0x631D60** `CTaskSimpleTurn180::FinishAnimTurn180CB` (cdecl, 16 bytes, pone `m_animHasFinished` y anula `m_anim`): ya existía como función C++ propia (la usa `ProcessPed`, enganchada en 0x639990); el EXE la seguía citando desde la tabla de callbacks de 0x8D0954 y desde una copia sin enganchar (0x637FE0). Ahora tiene enganche activo (`RH_ScopedInstall(FinishAnimTurn180CB, 0x631D60)`). Sin validar.

El resto de funciones reales de los huecos **ya estaba reconstruido** por otros lotes (278 piezas enganchadas) o es código que ningún camino vivo alcanza.

## 5. Pendiente (11 huecos, 23 piezas, 18.208 bytes)

| Hueco(s) | Piezas | Qué es | Por qué sigue abierto |
| --- | --- | --- | --- |
| 0x452F90–0x453F3F y 0x453F91 (juego) | 0x453000, 0x453060, 0x453280, 0x453690, 0x453A00, 0x453AE0, 0x453E40, 0x454010 | rutina de unos 4 KB con tabla de saltos y ayudantes (0x453000 mueve un nodo entre listas de 16 bytes), apuntada desde dwords de `.data`/`.bss` | sin clase identificada; hace falta leer el contexto antes de escribir C++ |
| 0x47C0F5 (juego) | 0x47C100, 0x47D520 | despachadores originales de comandos de script de las ranuras 3 y 4 de `0x8A6168` (`s_OriginalCommandHandlerTable`); el C++ los usa para los comandos sin manejador propio | cerrarlos pide implementar los comandos de esas centenas que faltan (plan de scripts, ComandosScript.csv) |
| 0x756DE1, 0x7573EF, 0x75E3E6, 0x770FD3, 0x7C073F, 0x7CAC5C, 0x80FFA4 (RenderWare) | 0x756DF0…0x75E9E6, 0x770FD3, 0x7C073F, 0x7CAC60/0x7CAF50/0x7CB190/0x7CB240, 0x80FFB0 | callbacks y métodos de objetos de RenderWare apuntados desde tablas de `.data` (0x8D63xx, 0xCAxxxx) | pertenecen al plan de RenderWare (lotes 240A–389A y librw); no se reconstruyen aquí |
| 0x82E924 (CRT) | 0x82E924 | `ctrlevent` de `signal` (nadie llama a `signal`), antes contada viva por su propia tabla SEH | **cerrado** (muerta) en la rama vivo-crtf8, ver [ejecucion-crt-fase8.md](ejecucion-crt-fase8.md) sección 4 |

Los cuatro destructores vacíos de un byte (0x43CE70, 0x4411A0, 0x4EF070, 0x56CDF0) no se pueden enganchar (un salto necesita 5 bytes) y tampoco hacen falta: solo los citan registros de `atexit`/`__vec_dtor` de funciones reemplazadas.

## 6. Compilación y humo

Ver el informe final de la rama (compilación con `_build_VIVOP6.sh`, `prueba_ropero.py` y `prueba_agua.py`, `audit_hook_catalog.py --strict`, `audit_cpp_assembly.py`).

## 6. Cierre de los huecos pendientes (rama vivo-p6b)

Antes: 259 huecos cerrados de 270 (11 pendientes, 23 piezas, 18.208 bytes). Despues: **269 cerrados de 270**; solo queda 0x82E924 (CRT, lo cierra la parte 4/P5). Piezas: reconstruida 278 -> 294, sin-referencia-real 208 -> 211, muerta-por-reemplazo 91 -> 92, nuevo destino `d3dx9-enlazar` 2, citada-en-cpp 175 (una menos viva), pendiente-dato 23 -> 1. Compilacion EXIT=0; humo (prueba_ropero y prueba_agua con la DLL de vivo-p6b): BIEN y BIEN. Sin validar pieza a pieza (regla del usuario).

* **0x452F90 / 0x453F91 (juego)**: no es una rutina de 4 KB con tabla de saltos. 0x453000..0x453DCB es una busqueda de ruta sobre rejilla 40x40 de sectores repetidos (grupo de CPathFind) **sin ningun llamador** (los dwords de .data/.rdata que la citan son partes de floats, p. ej. 0x453B80 = 3000.0f): codigo muerto, no se reconstruye (`sin-referencia-real`). La unica pieza viva es 0x453E40, el callback de fin de la animacion de descolgar del telefono (tabla .data 0x8D0958, lo registra CPhoneInfo::Update): `CPhoneInfo::PhoneAnimAnswerCallback`, enganchada. 0x454010 (metodo de CPhoneInfo) tampoco tiene llamadores.
* **0x47C0F5 (juego)**: los despachadores de las ranuras 3 y 4 ya no pasan por el EXE: `CRunningScript::ProcessCommands300To399` (0x47C100) y `ProcessCommands400To499` (0x47D210, que cubre tambien la pieza 0x47D520), en `RunningScriptCommands300To499.cpp`. Solo llevan caso los 32 + 28 comandos que el EXE resuelve ahi y no tienen manejador propio (49 cuerpos distintos: bips, reinicios, tiempo en pantalla, clima, armas, pasajeros...); los 4 comandos de 0x47D0E0 devuelven 0, el resto -1. Los comandos con manejador propio ya no tienen caso: GTASA_CMD_BYPASS no los devuelve al EXE en esas dos centenas.
* **RenderWare (7 huecos)**: 0x756DE1, 0x7573EF y 0x75E3E6 eran los callbacks de los nodos AllInOne de D3D9 (definiciones estaticas 0x8D6338 de atomos y 0x8D63B8 de sectores): `rpworld_p6b.cpp` (render por defecto 0x756DF0, iluminacion 0x757400 y 0x75E570, cuerpos de nodo 0x7575F0 y 0x75E3F0, inits 0x757890 y 0x75E540, instanciado de sectores 0x75E5E0 y el reinstanciado 0x758270, que figuraba como citada solo en un comentario). 0x7CAC5C era el skin: mezcladores SSE de 4 y 2 huesos (0x7CAC60, 0x7CAF50), inicio del vertex shader (0x7CB190) y init del nodo (0x7CB240), en `rpskin.cpp`. 0x770FD3 y 0x7C073F son del compilador de shaders de D3DX9 (se enlaza: `d3dx9-enlazar`); 0x80FFB0 es una cola de la funcion reemplazada 0x80FC30. Las definiciones de nodo de .data no se tocan: sus punteros siguen apuntando a esas direcciones, ahora enganchadas.

Limitaciones del analisis automatico descubiertas: no ve los `mov [reg+x], imm32` (como los nodos RW guardan callbacks), por eso `MANUAL` en `contrib/huecos_p6.py` lleva estas decisiones con nota. Hay ademas 9 piezas `citada-en-cpp` con entrada viva (0x453FB0, 0x5C91E0, 0x61A8A0, 0x68B7D0, 0x7F40F0, 0x7F5140, 0x7F53D0, 0x7FFF00, 0x8194D0) que pueden ser funciones reales sin enganche (la cita es solo un comentario): revisar. Tampoco se reconstruyo 0x756960 (instanciado generico, hueco 0x756958 clasificado sin referencia directa pero llamado por los nodos): se conserva por direccion.

## 7. Reclasificacion posterior: raices por valor (rama vivo-raices)

El experimento de la fase 8 mostro que la clase `muerto`/`citada-en-cpp` de esta parte era falsa para las direcciones que el C++ escribe como valor en tablas o campos de callback (0x4CD820, 0x750430, 0x750460, 0x750480, 0x750AC0, 0x7F98D0, 0x7F99B0 se ejecutan). La regla vigente (una direccion citada como valor en codigo es una raiz viva) y sus cifras estan en [reclasificacion-raices-valor.md](reclasificacion-raices-valor.md); `contrib/huecos_p6.py` ya no emite `citada-en-cpp` sino `viva-sin-hook`, `viva-enlazada`, `viva-quicktime`, `viva-protector` o `cerrada-comentario`, y el recuento de huecos cerrados de §3 y §6 queda sustituido por el de ese documento.
