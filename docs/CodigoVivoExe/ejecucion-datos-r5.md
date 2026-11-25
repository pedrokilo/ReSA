<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ronda 5 de DatosExe (22/11/2026, rama `datos-r5`, desde `745cee8725`)

Objetivo: definir en `DatosExe` los datos que el codigo propio cita por direccion fija y reconducir las referencias. Herramienta nueva: `contrib/datos_exe_r5.py` (lotes A = `.rdata`, B = `.data`, C = `.bss`; definiciones en `source/game_sa/DatosExe/DataR5{A,B,C}.h/.cpp`, namespace `DatosExe::R5`, listas en `docs/DatosExe/lotes/R5*.csv`) y `contrib/datos_exe_r5_medir.py` (cuenta direcciones de datos en los `.obj` con la salida de `scan_obj.py`).

## 1. Que hace la herramienta

A diferencia de DE04/DE05 (solo `StaticRef<T>(0x..)` con un unico tipo), reescribe cada aparicion del literal segun contexto: `StaticRef<T>` -> la variable (o `reinterpret_cast<add_lvalue_reference_t<T>>`), `reinterpret_cast<T*>`/`(T*)` -> `&variable`, ayudantes y otros usos -> `reinterpret_cast<uintptr_t>(&variable)`; admite cabeceras y objetos de tipo clase (almacen de bytes) y `X87Math.cpp`. El valor inicial sale de los bytes del EXE canonico (`PE`), nunca del emulador.

## 2. Lecciones (fallos reales de la primera version)

La primera tanda (258 objetos) dejo el ropero en MAL (entidades 0, marcadores 10/12, un cierre del juego). Causas identificadas y corregidas en la herramienta:

* **Punteros a tablas**: `(uint8*)0x866D9C` (`CMatrix::EulerIndices`) se tomaba como objeto de 1 byte. Ahora un cast a puntero solo cuenta como extension conocida si se desreferencia al momento (`*reinterpret_cast<T*>(..)`, `->`) o T es clase de tamano conocido.
* **Direcciones pasadas como cadena o tabla** a ayudantes (`name(0x8D0A5C)` de `ClothesBuilder`): solo se admiten ayudantes escalares (`put`, `Raw<T>`, `f`, `ExeByte`, `StoreWord`, `CAM_LF`, `ScopedStaticRef`...).
* **Interior de una tabla indexada** (`0xB6ECA0 + tipo*0x94`, tabla de temblor de camara cuyos campos se rellenaban con `put`): el escaner solo vio la base como indexada. Ahora se busca el uso `L + i`/`L[i]` en TODO `source/` y se descarta una ventana de 0x1000 bytes tras cada base indexada por el C++ (hasta la siguiente direccion que cita el EXE si la indexa el EXE) y 0x40 tras objetos de extension solo estimada.
* En `.rdata` (constantes inmutables) no se aplican las ventanas ni los filtros de «lector vivo»/«direccion tomada» (dos copias iguales son seguras), pero si los de escritura.

## 3. Resultado

La parte segura que queda es pequena: la mayor parte de lo pendiente son (a) direcciones de archivos de otros agentes o del CRT (`CrtStaticInitTable.inc` solo aporta ~850 direcciones de `.rdata`), (b) objetos leidos por codigo vivo del EXE (1.876 de `.bss`), (c) objetos con direccion guardada en tablas, (d) tablas/estructuras de extension desconocida, (e) instancias de clases construidas por `StaticInit.cpp` (143), (f) campos de `TheCamera` y otras estructuras.

| | Antes (`745cee8725`) | Despues |
| --- | ---: | ---: |
| Direcciones de datos citadas por fija en los `.obj` | 3.192 | 3.131 |
| `.bss` / `.data` / `.rdata` | 1.841 / 620 / 691 | 1.806 / 619 / 666 |
| Objetos nuevos en `DataR5` | 0 | 61 (A 25, B 1, C 35), 940 B |

Bytes copiados del EXE por el cargador: **641.024 B, sin cambio** (`.rdata` 308.224, `.data` 262.144, `_TEXT_HA` 68.608, `_rwdseg` 512, `.rsrc` 1.536). Ninguna seccion queda cubierta por completo: vtables (469 direcciones, DE09), tablas de punteros, cadenas y el `.data` inicializado se leen ademas por registro base, asi que no se puede quitar la copia.

## 4. Verificacion

* Build completo `EXIT=0` (Debug x86, candado compartido).
* Humo con la DLL final (sin agua, por indicacion del coordinador): `prueba_ropero.py` BIEN (15 entidades, 12 marcadores, puerta y marcador; captura revisada: CJ de espaldas en el cuarto, espejo, puerta y tabla de surf correctos) y `prueba_marcadores.py` BIEN (12 marcadores, 12 cambian en 1 s). Una primera pasada coincidio con otro juego en marcha y dio «el juego se cerro»; repetida sola, BIEN.
* Standalone enlazado con los `.obj` nuevos: `gGameState=9` a los ~6 s, 40 s de ejecucion, `excepciones=0`, `codigo-original-distinto=0`, `fase8_codigo_original.csv` vacio (`reversing/FnFase8/ejecuciones/dat5r5f-*`).
* `contrib/audit_hook_catalog.py --strict`: ver commit.

## 5. Actualización 23/11/2026

Los bytes que copiaba el cargador desde el `gta-sa.exe` (sec. 3) ahora salen de arreglos C++ incrustados en el exe independiente (`docs/CodigoVivoExe/ejecucion-imagen-datos.md`); `--ruta-original` solo se usa con `--imagen-externa`.
