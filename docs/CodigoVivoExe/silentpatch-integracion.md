# SilentPatch: rama de integración (`silentpatch-integracion`)

Fecha: 25/11/2026. Base: `integracion-fase7b` (6a308600cd). Fusiona, en este orden: `silentpatch-rtanim` (hasta 4c391fb3b0), `silentpatch-entidades`, `silentpatch-graficos` y `silentpatch-scm`.
Las licencias están en [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md); la tabla de desviaciones es la sección 15 de [DesviacionesConocidas.md](../DesviacionesConocidas.md).

## Documentos por área

* RtAnim / streaming memory bug / `CBike::ProcessRiderAnims`: sección 15.1 de DesviacionesConocidas.md (fila 1 y párrafo de verificación) y `reversing/FnSkin/LEEME_skin.md` (vigilancia de skin, apagada por defecto).
* Vehículos, peds, jugador, fuego y pools: [silentpatch-entidades.md](silentpatch-entidades.md) (15.1).
* Gráficos, tiempo y streaming: [silentpatch-graficos.md](silentpatch-graficos.md) (15.2).
* Scripts, misiones, entrada, HUD, trucos, guardado y textos: [silentpatch-scm.md](silentpatch-scm.md) (15.3).

## Esquema único de nombres

`GTASA_SILENTPATCH=0|1` o `SilentPatch=0|1` en `gta-reversed.ini` (por defecto 1). Por opción: `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0|1` o `SilentPatch_<Nombre>=0|1`;
las numéricas (`Units`) igual con un entero. Definido y documentado en la cabecera de `source/extensions/SilentPatchConfig.h`. Las consultas se guardan en caché por nombre (antes se releía el ini en cada llamada).
El log del juego (`logs/log.log`) muestra `SilentPatch: interruptor general = N` y `SilentPatch: opcion X = N` (función `SilentPatchLogState`, llamada desde `Idle`).
La vigilancia de skin (`GTASA_SKIN_VIGILANCIA=1`) y el diagnóstico `GTASA_SILENTPATCH_DIAG=1` están APAGADOS por defecto (sin `SkinVigilanciaActivaPorDefecto.h`, que no se versiona).

## Conflictos de la fusión y resolución

| Conflicto | Resolución |
| --- | --- |
| `SilentPatchConfig.h/.cpp` (sp1 = sp2 = sp4 idénticos; sp3 distinto: `GTASA_SP_<NOMBRE>`, sección `[SilentPatch]`, `SilentPatchIntOption`) | Se conserva el de sp1 y se le añade `SilentPatchIntOption`; los usos de sp3 no necesitaron reescritura salvo nombres de opción (ver abajo); esquema documentado solo en la cabecera |
| `THIRD_PARTY_NOTICES.md` | Una sola entrada SilentPatch con todas las áreas y el texto completo de la licencia MIT (una vez) |
| `DesviacionesConocidas.md` sección 15 / 15b / 15 (sp3) | Una sola sección 15 con subsecciones 15.1, 15.2, 15.3; ninguna fila perdida (se anotan las duplicadas) |
| `Streaming.cpp`: `StreamedEntityList` (sp2) y `BiggerStreamedEntityList` (sp4), mismo arreglo | Queda la de sp2 (`StreamedEntityList`) |
| Arreglos portados dos veces por sp2 y sp3: SEAROADER, melee de trucos (katana/paracaídas), Beagle de Fort Carson, azar de 16 bits, biker cop, reinicio de variables de New Game | Una sola implementación: sp2 para SEAROADER, Beagle, azar y biker cop; el reinicio de New Game es el de sp3 (`SilentPatchNewGame`, superconjunto: añade `TimeNextMadDriverChaseCreated` y la recarga de IPL); el melee de sp3 (conjuntos de armas 1-3) queda con la opción `MeleeCheatAmmo` de sp2. Se eliminaron los ayudantes de sp3 que quedaron sin uso (`FixBikerCop`, `Random16`) |
| `Cheat.cpp`, `FileLoader.cpp`, `Game.cpp`, `MathCommands.cpp`, `RunningScript.cpp`, `RunningScriptCommands100To199.cpp` | Resueltos según la fila anterior |

## Hallazgo: opciones de compilación por archivo (aserción de X87Math.cpp)

La primera ASI de pruebas se compiló con `compilar_over.cmd` y X87Math.cpp salió con las opciones genéricas (`/Od`), no con las del CMake (`/O2 /Oi /arch:IA32 /fp:fast`, sin PCH). Al cargarse, la guarda
`ResidueHelpersAreTrivial()` (X87Math.cpp:491) falló con un diálogo `Assertion failed!` y la prueba `sp_off_rop` terminó con `ERROR el juego se cerro`. No es un defecto del código fusionado (el cambio del ratón de sp3 en X87Math.cpp solo sustituye
la dirección de `vB6EC18` por una llamada a `SilentPatch_MouseAccelVerticalPtr()`): con X87Math.cpp compilado con sus opciones (`reversing/FnFase8/over/cl.x87.rsp`: las de `cl.efectivo.rsp` con `/Od` cambiado por `/O2 /Oi /arch:IA32 /fp:fast`) la aserción pasa y no se tocó. Los otros archivos con opciones especiales (`CarCtrl.cpp`, `Stats.cpp`) solo piden `/Zi` sin comprobaciones de ejecución, que coincide con `cl.efectivo.rsp` (sin `/RTC`).
Esto probablemente explica también el "no llego al interior" que sp3 vio con SilentPatch encendido (misma causa, no atribuible a ReinitIplObjectsOnNewGame); no se pudo comprobar con la ASI de sp3.

## Compilación y enlace

50 archivos .cpp nuevos o modificados respecto a la base compilados con `compilar_over.cmd` (EXIT=0), X87Math.cpp con `cl.x87.rsp`; ASI enlazada con los .obj de `wv-int\build` (1192 objetos, 44+ de `over`) y standalone con `enlazar.cmd`; 0 símbolos sin resolver.
`python contrib/audit_hook_catalog.py --strict`: OK.

Fusión de prueba con `auditoria-tamanos` (worktree temporal esparso, descartado): sin conflictos de código (solo los tres archivos `reversing/FnColor/*` en add/add, que chocan igual con la base sin SilentPatch); los 9 .cpp del cruce y `VerifTamanos.cpp` compilan con los `static_assert` (EXIT=0).

`compilar_over.cmd` ahora compila X87Math.cpp y Core/Vector.cpp con las opciones de CMake (probado con X87Math: mismo tamaño de .obj que el compilado a mano).

## Resultados de las pruebas (25/11/2026)

Humo de la ASI en la instalación de pruebas (`prueba_ropero.py` y `prueba_marcadores.py`, `cal_mal.args`, lanzados con `humo_cola` esperando a `gta_lock` sin crearlo; ASI = copia de la ASI de la rama; control = `wv-int\bin\Debug\gta_reversed.asi`). `GTASA_SILENTPATCH=0` llegó al juego: el log de la ejecución apagada dice `interruptor general = 0` y la encendida `= 1` con la lista de opciones.

| Etiqueta | Hora | ASI | SilentPatch | Prueba | Veredicto |
| --- | --- | --- | --- | --- | --- |
| sp_ctl_rop | 01:59-02:00 | control wv-int | (sin código SP) | ropero | BIEN |
| sp_ctl_mar | 02:00-02:00 | control wv-int | (sin código SP) | marcadores | BIEN |
| sp_off_rop | 02:00-02:02 | rama v1 (X87Math con /Od) | apagado | ropero | `ERROR el juego se cerro` (diálogo Assertion failed de X87Math.cpp:491; causa: opciones de compilación, ver arriba) |
| sp_off_rop2 / sp_off_mar2 | 02:05-02:06 | rama v2 (X87Math corregido) | apagado | ropero / marcadores | BIEN / BIEN |
| sp_on_rop2 / sp_on_mar2 | 02:06-02:07 | rama v2 | encendido (por defecto) | ropero / marcadores | BIEN / BIEN |
| sp_off_rop3 / sp_off_mar3 | 02:24-02:25 | rama v3 (final: con RtQuatRotate de la moto y caché de opciones) | apagado | ropero / marcadores | BIEN / BIEN |
| sp_on_rop3 / sp_on_mar3 | 02:25-02:26 | rama v3 | encendido | ropero / marcadores | BIEN / BIEN |

No hizo falta bisecar por arreglo: con SilentPatch encendido, ropero y marcadores dan BIEN (incluidos `ReinitIplObjectsOnNewGame` y `ResetVariablesOnNewGame`, activados por defecto). **Ningún arreglo queda apagado por defecto** salvo `TrueInvincibility` (apagado por defecto como en SilentPatchSA.ini). Reservas: una sola tanda por combinación; el humo del ropero no ejercita partida nueva ni misiones.

Pruebas dirigidas con el standalone final (copia `reversing/FnFase8/salida/snap/spint_v3.exe`, SilentPatch por defecto = encendido):

* `ejecutar_skin.ps1 -Modo std -Vigilancia` (pie, bici 481, moto 463, coche 411; 420 s límite, 407 s de vigilancia): `skin.log` con 37 resúmenes y `anomalias=0` en todos (maxAbs 6,2 hasta el último resumen leído), `excepciones=0` en 210 latidos, sin NaN. Los 11 «EVENTO imagen_ABA» son avisos del comparador de imágenes de la sonda, no de huesos. Limitación: en la moto 463 el piloto montó pero la sonda lo reporta fuera del vehículo (`en_veh=False`) casi toda la fase, así que la moto quedó poco ejercitada; no hay escenario de peds en moto (modelos 284 y 60) en las sondas actuales: NO probado.
* `ejecutar_juego.ps1 -Escenario conduccion,armas,interiores,gimnasio,trucos` (330 s hasta la guardia de tiempo, vivo todo el rato): conducción sin NaN, armas (uzi con ratón, teclado y mando), interiores CARLS/GYM1/CLOTHGP/AMMUN1/BARBERS con entrada y salida, gimnasio de Ganton 30 s dentro sin caída, 165 latidos con `excepciones=0`. Esa misma ejecución cubre los 120 s pedidos. (El escenario `trucos` no llegó a ejecutarse: la guardia mató el proceso durante el gimnasio.)
* `FnTraf/ejecutar_traf.ps1` no se pudo usar: falta `D:\Original\_trafico_run` (borrada). Tráfico y spawns solo se ejercitaron por el truco de la sonda de conducción.
* Con SilentPatch apagado el standalone no se ejecutó; solo la ASI (tabla anterior).
