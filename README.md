# ESOS WEYES DEL REVERSE SE DEMORAN UN CHINGO Y SON MUCHOOOS 
# GTA San Andreas · Reversing en PGTASA

Reconstrucción en C++ del comportamiento original de GTA San Andreas, con comparaciones automáticas dentro del motor antes de habilitar las funciones trabajadas.

**[📊 Progreso](docs/ReversedClasses.md) · [🧭 Qué abordar después](docs/ReversingPriorities.md) · [🧪 Pruebas](reversing/README.md) · [📋 Proceso y continuidad](reversing/MEJORAS_DEL_PROCESO.md)**

> [!NOTE]
> Este README describe el trabajo local de la rama **PGTASA**. El avance registrado y los bancos aprobados no equivalen a cobertura exhaustiva ni a estabilidad general del juego.

## Cómo funciona

La compilación produce `gta_reversed.asi`, una DLL que carga un ASI loader. Los hooks redirigen funciones del ejecutable a sus reconstrucciones en C++; las direcciones como `0x571920` identifican entradas del original. El proyecto todavía depende del ejecutable y de los recursos del juego: no es un ejecutable independiente terminado.

El objetivo es preservar el comportamiento original. Cada reconstrucción se contrasta con el original antes de actualizar sus indicadores; una compilación correcta o un hook visible en el debug no sustituyen esa comparación.

## Estado del trabajo

La fuente del progreso es [docs/hooks.csv](docs/hooks.csv). El [informe generado](docs/ReversedClasses.md) muestra totales, pendientes y bloqueos; la [guía técnica de prioridades](docs/ReversingPriorities.md) ordena el trabajo por esfuerzo estimado, dependencias y facilidad de validación.

**Corte documental del 23 de septiembre de 2026, hora de México:** 614 clases completas, 85 parciales y 2 sin revertir; 7.485 de 8.014 entradas registradas como revertidas. Estos números no estiman líneas de código ni porcentaje de comportamiento probado. Consultar el informe para el estado posterior.

### Reconstrucciones recientes con evidencia local

Las comparaciones corresponden al alcance descrito en cada README, no necesariamente a todas las funciones de la clase.

| Clase / módulo | Estado registrado | Pares original/candidato | Evidencia |
| --- | ---: | ---: | --- |
| `RpAnimBlend` | 43/43 | 652 | [Alcance y resultados](reversing/RpAnimBlend/README.md) |
| `CBaseModelInfo` | 34/34 | 65.536 | [Alcance y resultados](reversing/CBaseModelInfo/README.md) |
| `FxManager_c` | 25/25 | 720 | [Alcance y resultados](reversing/FxManager/README.md) |
| `CClouds` | 21/21 | 1.170 | [Alcance y resultados](reversing/CClouds/README.md) |
| `CMenuSystem` | 21/21 | 784 | [Alcance y resultados](reversing/CMenuSystem/README.md) |
| `CTaskSimpleUseGun` | 19/19 | 3.232 | [Alcance y resultados](reversing/CTaskSimpleUseGun/README.md) |
| `CTaskComplexWander` | 14/14 | 10.240 | [Alcance y resultados](reversing/CTaskComplexWander/README.md) |
| `CTaskSimpleJetPack` | 14/14 | 128 | [Alcance y resultados](reversing/CTaskSimpleJetPack/README.md) |
| `CPed` | 145/145 | 28.672 | [Alcance y resultados](reversing/CPed/README.md) |
| `CConversations` | 11/11 | 2.048 | [Alcance y resultados](reversing/CConversations/README.md) |
| `CPedList` | 7/7 | 4.096 | [Alcance y resultados](reversing/CPedList/README.md) |
| `CPlayerInfo` | 28/28 | 16.896 + 4.224 del constructor | [Alcance y resultados](reversing/CPlayerInfo/README.md) |
| `FurnitureSubGroup_c` | 3/3 | 20.480 | [Alcance y resultados](reversing/FurnitureSubGroup_c/README.md) |
| `CGeneral` | 9/9, sin bloqueos | 65.536 | [Alcance y resultados](reversing/CGeneral/README.md) |
| `CClothes` | 11/11 | 4,096 | [Alcance y resultados](reversing/CClothes/README.md) |
| `CScripted2dEffects` | 5/5 | 256 | [Alcance y resultados](reversing/CScripted2dEffects/README.md) |
| `CTaskComplexUseClosestFreeScriptedAttractorSprint` | 4/4 | 4,096 | [Alcance y resultados](reversing/CTaskComplexUseClosestFreeScriptedAttractorSprint/README.md) |

**Últimos desbloqueos:** [DataStream y TrackEntity](reversing/UnlockBatch2/README.md), diez candados retirados tras 15.360 pares y 64 combinaciones de DataStream. CSV y catálogos sincronizados: quedan **620 candados** en el registro. Los 86 restantes del lote solicitado se aplazan; sus borradores se conservan fuera de la compilación. ASI normal instalado en la copia de pruebas: inicio al menú y 30 segundos de integración sin exclusiones comprobados; los avisos históricos y los límites constan en el informe.

**Última corrección:** [reserva de CEntryExit](reversing/CEntryExitPool/README.md), ampliada de 400 a 512 tras reproducir el agotamiento también con el original. Banco de 1.854 comprobaciones (21 diferencias previstas de FIX_BUGS) y ASI normal con 120 segundos de integración sin fallos de asignación. Se conservan partidas, inicio normal y los 86 bloqueos aplazados.

**Tanda anterior:** [24 bloqueos retirados y plataforma conmutable](reversing/Unlocks/README.md). 50.432 pares de función, más 324 pares repetidos en nueve arranques y 18 comprobaciones de entrada SDL/ImGui. CGeneral ya estaba desbloqueado. Se repararon contratos ABI, memoria, entrada y dimensiones de cámara; el CSV mantiene los mismos totales de progreso.

**Cierre anterior:** [diez clases del Top 10](reversing/Top10/README.md), 22 entradas nuevas y 51.712 comparaciones exactas. Incluye Shooting, recursos de scripts, LeaveCarAndWander, WaitForBus, PopCycle, CarCtrl, KillThreatsBasic, FxPrimBP, EntityScanner y DragPedFromCar.

El constructor de `CPlayerInfo` ya fue corregido, validado y desbloqueado; incluye regresiones de entrega y selección de armas. `CQuadBike::ProcessControl` tiene un candidato que compila, pero continúa **sin validar y deshabilitado**, con progreso 11/12. Se conservan también los bancos históricos de vehículos, guardados, jugador y efectos de armas.

## Abrir el juego desde el inicio

La copia de pruebas tiene `FastLoader.Enable = false` y `SaveGameToLoad = -2`. Muestra las pantallas de inicio y el menú; el usuario elige **Nueva partida** o **Cargar partida**.

```powershell
Set-Location 'E:\JUEGOS\GTA San Andreas - CVehicle Test'
& '.\gta_sa.exe'
```

Este arranque carga el ASI normal con los hooks habilitados de la compilación, sin exclusiones por argumentos. Los bancos automáticos activan temporalmente la carga rápida solicitando la ranura 2 y restauran los dos INI originales al terminar, incluso si la prueba falla.

En las últimas pruebas, la versión del guardado de esa ranura no coincidió y el juego inició partida nueva; no se forzó la carga ni se modificó el guardado.

[Configuración y comprobación del inicio](reversing/InicioNormal/README.md) · [Diagnóstico y reparación del cierre de audio](reversing/CAESound/README.md).

## Ejecutar las pruebas

Desde la raíz, usar el **único `PROBAR.cmd`**:

```powershell
.\PROBAR.cmd --list
.\PROBAR.cmd --case furniture
.\PROBAR.cmd --case playerinfo-constructor
.\PROBAR.cmd --all
```

Para ejecución sin la pausa final del CMD:

```powershell
python reversing/run.py --case furniture
python reversing/run.py --all
```

El catálogo está en [reversing/cases.json](reversing/cases.json). Las sondas deben estar compiladas y archivadas con el hash esperado por cada ejecutor; ejecutar un caso no recompila automáticamente la sonda. `--all` requiere disponer de los artefactos locales de todos los bancos.

El runner compartido comprueba el ejecutable, evita ejecuciones simultáneas, respalda ASI/PDB, instala la sonda, abre su propio GTA, audita los logs, cierra ese proceso y restaura los binarios y ambos INI. Verifica los hashes de las partidas. Los informes por caso quedan en la copia de pruebas; los informes conjuntos, en `reversing/artifacts/suite-*/summary.json`.

> [!IMPORTANT]
> `--all` ejecuta **sondas archivadas** de cada trabajo. No demuestra que todos los cambios sean estables juntos en la última compilación. La campaña de integración, guardado/carga y sesiones prolongadas se hará cuando estén completas todas las clases, según lo acordado en el [proceso](reversing/MEJORAS_DEL_PROCESO.md).

### Instalación y datos protegidos

| Uso | Ruta del entorno local |
| --- | --- |
| Repositorio | `E:\Original\gta-reversed-master` |
| Copia de pruebas | `E:\JUEGOS\GTA San Andreas - CVehicle Test` |
| Instalación personal, conservar | `E:\JUEGOS\GTA San Andreas` |

La instalación normal se actualiza únicamente en la copia de pruebas y conserva un respaldo. Los cambios no deben sobrescribir partidas personales. El runner actual tiene rutas locales configuradas; revisar [runtime_runner.py](reversing/runtime_runner.py) y el proceso antes de usarlo en otra máquina. `PROBAR.cmd` también apunta al Python instalado en este equipo.

El ejecutable de referencia es **GTA SA Compact**, distinto de otros ejecutables llamados «1.0 US». SHA-256 verificado en este entorno:

```text
72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b
```

Los bancos habituales arrancan con `--unhook=Audio,Global/SurfaceInfos_c`. El caso `sound-copy` mantiene Audio activo y solo excluye SurfaceInfos. Esa exclusión limita sus conclusiones; una futura validación de funciones de audio necesita configurar y comprobar explícitamente sus hooks. Las pruebas son automáticas y no requieren una demostración visual.

## Compilar

La configuración del repositorio usa **Windows, MSVC, x86, C++23 y Debug** por defecto. Véanse [perfil Conan](conan/profiles/windows-msvc.txt), [CMakeLists.txt](CMakeLists.txt) y [conanfile.py](conanfile.py): CMake requiere 4.2.0 o superior y el perfil actual selecciona MSVC 195. El entorno utilizado tiene Visual Studio 2026, Python y Conan 2.

Preparación de un entorno nuevo con las herramientas instaladas:

```powershell
python -m pip install conan
conan profile detect
python setup.py
python setup.py --build
```

Para recompilar el directorio `build` ya configurado:

```powershell
cmake --build build --config Debug
```

Salida del entorno local: `bin/Debug/gta_reversed.asi` y `.pdb`. El proceso documenta la ruta del CMake disponible en la caché local cuando no está en `PATH`. No mover esa caché sin revisar las rutas de compilación.

Se necesita una copia propia del juego y sus recursos, además de un ASI loader compatible. Las instrucciones generales heredadas de instalación están en [contrib/install.py](contrib/install.py); revisar sus rutas y enlaces antes de aplicarlas. El flujo local de validación usa respaldos y la copia de pruebas descrita arriba.

## Flujo para continuar

1. Leer [MEJORAS_DEL_PROCESO.md](reversing/MEJORAS_DEL_PROCESO.md), incluida la última continuidad, y comprobar rama y cambios de Git.
2. Revisar funciones pendientes, contrato original, llamadas y código comparable. La guía de prioridades es una estimación, no una orden para habilitar código.
3. Reconstruir y comparar original/candidato dentro del motor con entradas reproducibles, contadores y restauración de estado.
4. Solo después de validar, actualizar `reversed/locked` en `hooks.csv`, documentar resultados y regenerar el informe:

   ```powershell
   python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
   ```

5. Restaurar las fuentes instrumentadas, compilar normalmente e instalar solo en la copia de pruebas con respaldo. Conservar límites, hashes y evidencia en `reversing/<clase>/`.

Los artefactos voluminosos, logs y cachés locales no se incluyen en Git. No hacer commit ni push sin solicitud expresa. Mantener los cambios previos y reutilizar la infraestructura existente.

## Proyecto original y contribuciones

Este trabajo continúa [gta-reversed](https://github.com/gta-reversed/gta-reversed). Se conservan los [criterios de código](docs/CodingGuidelines.MD) y el reconocimiento a sus autores. Comunidad original: [GTA Groupies](https://discord.gg/FG8XJ5Npqe). No hay una fecha comprometida para completar todo el proyecto.

## Créditos
- All contributors of the [plugin-sdk](https://github.com/DK22Pac/plugin-sdk) project.
- All contributors of the [original project](https://github.com/codenulls/gta-reversed).
- Hundreds of great people on [gtaforums.com](https://gtaforums.com/topic/194199-documenting-gta-sa-memory-addresses).
- [re3 team](https://github.com/GTAmodding/re3).
- And everyone who contributed to GTA:SA reversing.
