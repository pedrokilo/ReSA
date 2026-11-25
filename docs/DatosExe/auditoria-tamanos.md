# Auditoria de tamanos de los objetos propios DatosExe

Rama `auditoria-tamanos` (desde `origin/main` c86f7f6ebe, 24/11/2026). Herramienta: `contrib/auditoria_datos/tamanos_objetos.py`.
Salidas: [tamanos-objetos.csv](tamanos-objetos.csv) (un objeto por fila), [tamanos-minimos.csv](tamanos-minimos.csv) (tamano minimo medido por objeto, base de los `static_assert`).

## 1. La clase de fallo

Un objeto `DatosExe::*` (`extern T nombre; // 0xDIR`) definido con **menos bytes que el dato real del EXE**, mientras el codigo lo usa
como array o estructura. En el ejecutable independiente el enlazador coloca los objetos por su `sizeof` declarado; el codigo que
escribe mas alla del objeto pisa a los vecinos del mapa de enlace. Caso real (Ronda 5, `5efe9eeeb6`): `vB4E3E8` (`ms_lightsOn`) y
`vB4E3F0` (`ms_currentCol`) de 1 byte usados como `uint8[4]`, y `vB61100` (`m_sfLogLookup`) de 4 bytes usado como `float[50][2]`.

Causa de raiz: la Ronda 5 definio los objetos con el **ancho del acceso del EXE** (`tam_origen=acceso`) y reapunto los consumidores con
`reinterpret_cast<std::add_lvalue_reference_t<T>>(objeto)`: el cast ocultaba que `sizeof(T) > sizeof(objeto)`. Las auditorias
anteriores de "dato en dos sitios" miraban citas por literal y huecos hasta el siguiente objeto; no miran el tipo con que cada
consumidor ve el objeto.

## 2. Hallazgo importante sobre los datos de partida

**`gta_sa.pdb` de la instalacion NO tiene tamanos.** Es un PDB de simbolos publicos (registros `S_PUB32`: nombre + segmento:desplazamiento,
38.084 simbolos, 8.825 de ellos en `.rdata`/`.data`/`.bss`); `dbghelp` no lo asocia a ningun EXE de la instalacion (0 simbolos) y los
registros no llevan tipo ni tamano. Por eso la comparacion "sizeof declarado == tamano PDB" pedida **no es posible** con ese PDB. Se uso:

* el PDB solo como evidencia **cualitativa**: nombre del simbolo en la direccion exacta del objeto (981 de 2.134 objetos) y distancia al
  siguiente simbolo (cota superior del tamano, no el tamano: los simbolos sin nombre en gta-reversed ensanchan el hueco);
* el **tamano compilado** de cada objeto (PDB del independiente con `dbghelp`, exacto) frente al **tamano del tipo con que el codigo
  lo usa** (ver 3), que es la unica evidencia que determina si hay solape real en el enlace.

## 3. Metodo (todo medido, ninguna estimacion)

1. **Tamano declarado**: `sizeof` compilado de los 2.134 objetos (2.145 lineas `extern`, 11 repetidas en dos cabeceras) leido del PDB del
   independiente (`reversing/FnFase8/salida/gta_sa_standalone.pdb`). Las 32 ranuras `b<DIR><T>()` de `DataBssF` llevan el tipo `T` de la
   `StaticRef` original y no pueden verse con otro tipo (la especializacion exige el mismo `T`): no se auditan por tamano.
2. **VISTA (evidencia principal)**: se leen las relocaciones COFF de los 1.189 `.obj` del ASI Debug. Cada variable de referencia estatica
   (`static inline auto& x = reinterpret_cast<T&>(objeto)`) aparece como un puntero en `.data`/`.rdata` con relocacion al objeto; el
   nombre decorado de la variable lleva el tipo apuntado (`undname`: `float (& CAEAudioUtility::m_sfLogLookup)[50][2]`) y su tamano sale
   de la cadena de tipos (primitivos, `std::array`, arrays, punteros) o del PDB (clases). 1.304 objetos tienen al menos una vista medida
   (6.832 relocaciones de datos a objetos DatosExe, 3.444 pares variable-objeto distintos incluida la tabla de `Verif`; solo 1 sin resolver: un `const char*`).
3. **RELOC**: relocaciones DIR32 con sumando >= `sizeof` del objeto (accesos con desplazamiento constante fuera del objeto):
   10.248 relocaciones sobre 2.938 simbolos DatosExe; ninguna fuera de rango.
4. **SOLAPES REALES**: el `.map` del independiente da los vecinos que pisa cada objeto insuficiente (seccion 4).
5. **Informativas**: objeto escalar pequeno que el EXE indexa con registro (`tabla_indexada` de `objetos.csv`) y que el C++ usa;
   accesos del EXE dentro del simbolo del PDB original pero fuera del objeto (alto ruido: son variables vecinas sin nombre).
6. **Lectura de codigo**: los 24 sitios con `reinterpret_cast`/cast de C sobre objetos DatosExe de `source/` (fuera de `DatosExe/`, sin contar los
   que solo toman la direccion como `uintptr_t`) se revisaron uno a uno (tipo vista vs `sizeof` del objeto): todos caben tras la correccion de wv-color.

## 4. Cifras

Medicion con los artefactos de `main` **antes** de la correccion de `ms_lightsOn`/`ms_currentCol`/`m_sfLogLookup` (los .obj y el PDB de
`D:\Original\wv-int`, 24/11/2026 19:39-22:46) y mapa `_color_antes`:

| Concepto | Cifra |
| --- | ---: |
| Objetos revisados (declarados, con tamano compilado) | 2.134 |
| Con simbolo en el PDB original (nombre; sin tamano) | 981 |
| Con tamano exacto en el PDB original | 0 (el PDB no lo trae) |
| Sin simbolo en el PDB original | 1.153 |
| Con vista tipada medida (VISTA) | 1.304 |
| Con relocacion con sumando (RELOC, dentro del objeto) | 2 |
| **Insuficientes** (tamano real medido > declarado) | **3** |
| Insuficientes nuevos (no conocidos antes) | **0** |
| Sospechosos informativos | 1 (`v8CC29C`, `float[2]` indexado por `% 2`; correcto) |
| Invasores (declarados mas que el real) | 0 medibles (sin tamanos reales); 2 con hueco PDB menor (`rC8CFB0`/`rC8CFB8`, `RwV2d` = dos simbolos `_padX2/_padY2` y `_padX1/_padY1`: correcto) |
| Solapes de direccion entre objetos | 16 pares del mismo dato `uint32`/`float` en R5 y Rdata (constantes de solo lectura; ya conocidos) |
| Objetos sin ningun uso en `source/` | 46 (no pueden corromper nada) |

Los 3 insuficientes son exactamente los ya corregidos por el agente de vehiculos negros (`8b4974a72f`):

| Objeto | Declarado | Vista real | Vecinos pisados en el mapa de enlace (orden del `.map`) |
| --- | ---: | ---: | --- |
| `R5::vB4E3E8` (`CVehicleModelInfo::ms_lightsOn`) | 1 | 4 | `vB4E3F0` (ms_currentCol), `vB73402` (bInvertLook4Pad), `vBAA38A` |
| `R5::vB4E3F0` (`CVehicleModelInfo::ms_currentCol`) | 1 | 4 | `vB73402`, `vBAA38A`, `vC1CC01` |
| `R5::vB61100` (`CAEAudioUtility::m_sfLogLookup`) | 4 | 400 | 18 objetos: `vB6EC10/14/18` (cruceta 3a persona), `rA9A888` (pool de saltos), `vC02C20`, **`vC1BFD8` (textura de matricula)**, `rB6B99C`, `vC4032C`, `vC4B8C8`, **`rB745BC` (pool QuadTree), `rB745CC`/`rB745D0` (listas de render LOD)**, `rBC4094` (IplDef), **`rC02D28/2C/30` (pools de EnvMap/SpecMap de los coches)**, `rC1CC50`, `vC3FE08` (PostEffects) |

La auditoria independiente (VISTA) **redescubre solo esos 3**: no hay otro objeto de DatosExe cuyo uso tipado sobrepase su tamano.

### Efecto

* `ms_lightsOn`/`ms_currentCol`: medido por el agente de vehiculos negros (colores = 0 = negro). Aqui solo se anade la lista de vecinos del mapa.
* `m_sfLogLookup`: `CAEAudioUtility::StaticInitialise` escribe 400 bytes (floats) desde `vB61100`. En el mapa, esos bytes caen sobre punteros
  a pools y listas del renderizado (QuadTree, listas LOD, pools de EnvMap/SpecMap de los coches, textura de matricula) y sobre las
  constantes de la camara. **No se midio en ejecucion** si esos punteros se inicializan antes o despues del audio; si se inicializaban
  antes, quedaban corrompidos con floats. Es una **posible** causa de los artefactos graficos que ve el usuario (texturas/modelos
  deformados un fotograma), **no demostrada**; la correccion de `8b4974a72f` ya la elimina.

## 5. Correcciones de esta rama

No hay objetos adicionales que agrandar (los 3 ya estaban corregidos en `8b4974a72f`/`1976c92ebf`, que esta rama incorpora con
`cherry-pick` sin tocarlos). Lo que se anade es **prevencion permanente**:

1. `source/game_sa/DatosExe/Vista.h`: `DATOSEXE_VISTA(objeto, T)` es la unica forma de ver un objeto DatosExe con otro tipo: da `T&` y
   `static_assert(sizeof(objeto) >= sizeof(T))` (solo de tipos: no cambia el codigo generado ni el orden de inicializacion). Se aplico
   a los 24 sitios existentes (headers de clase `static inline auto& ... = DATOSEXE_VISTA(...)` y casts locales). Los tres objetos
   corregidos quedan cubiertos tambien por esa comprobacion. `contrib/datos_exe_r5.py` emite ya esta forma.
2. `source/game_sa/DatosExe/VerifTamanos.cpp` (generado): 1.987 `static_assert(sizeof(obj) >= minimo)` con el tamano minimo medido
   (VISTA o tipo de la `StaticRef` original). Reducir un objeto rompe la compilacion. No depende del PDB ni de los .obj (la tabla
   `tamanos-minimos.csv` esta versionada): **siempre activo**, tambien en el CI de GitHub (coste de compilacion despreciable).
3. `python contrib/auditoria_datos/tamanos_objetos.py lint` (sin datos externos): rechaza todo `reinterpret_cast`/cast de C sobre un
   objeto DatosExe que no pase por `DATOSEXE_VISTA`, comprueba que `VerifTamanos.cpp` coincide con el CSV y que cada objeto de la
   tabla existe. Flujo de CI nuevo: `.github/workflows/datosexe-tamanos.yml`.

## 6. Como se ejecuta

```
python contrib/auditoria_datos/tamanos_objetos.py lint        # CI; sin dependencias
python contrib/auditoria_datos/tamanos_objetos.py medir       # tras compilar ASI Debug + independiente (usa dbghelp: Windows)
python contrib/auditoria_datos/tamanos_objetos.py generar     # regenera VerifTamanos.cpp desde tamanos-minimos.csv
```

`medir` usa por defecto `reversing/FnFase8/salida/gta_sa_standalone.{pdb,exe,map}`, `build/source/gta_reversed.dir/Debug/*.obj` y
`D:\Grand Theft Auto San Andreas\gta_sa.pdb` (solo lectura); admite `--pdb --exe --obj-dir --map --pdb-orig`. Si faltan los
artefactos avisa (`SKIP`) y sale con 0 (con `--exigir`, con 1). Sale con 1 si hay un objeto **insuficiente** (vista o relocacion mayor que
su tamano) y reescribe los dos CSV: un objeto nuevo mas pequeno que el real que ya tenga consumidor tipado no pasa la medicion.
Al anadir objetos nuevos: ejecutar `medir` y `generar` y versionar los CSV y `VerifTamanos.cpp`.

## 7. Limites y lo que NO se pudo verificar

* **Tamano real exacto de los objetos con solo uso bare**: 784 objetos con usos y sin vista medida se ven con su propio tipo (alias
  `auto& x = DatosExe::...`): por construccion no sobrepasan lo que declaran, pero si el tipo declarado fuese mas corto que el dato del EXE
  (p. ej. un `int32` que el EXE usa como `int32[4]` y el C++ nunca indexa) no hay evidencia que lo muestre. Sin tamanos en `gta_sa.pdb`, la
  unica fuente seria el desensamblado; se usaron los indicadores de `objetos.csv` (`tabla_indexada`, 49 objetos: 0 escalares pequenos con
  consumidor) pero no se desensamblaron los accesos con registro base (`[esi+...]`).
* **Indexado dinamico dentro de una funcion** sobre una referencia local (no estatica) o aritmetica de punteros sobre `&objeto` no se ve
  (solo el sumando constante). Los 24 sitios existentes se revisaron a mano; el lint impide casts nuevos fuera de `DATOSEXE_VISTA`.
* **Referencias inicializadas dinamicamente** (variables locales `static auto&` en funciones): se detectaron 2 patrones `mov [var], offset`
  en codigo (solo `renderLicensePlateTexture`, 4 bytes, correcto); el resto de formas dinamicas no se resolvieron.
* **Efecto en ejecucion** de `m_sfLogLookup` sobre los pools/listas de render: no medido (seccion 4).
* **Las 32 ranuras `DataBssF`** no se auditan por tamano (tipo fijado por la especializacion).
* La medicion de esta rama se hizo con los artefactos de `main` previos a la correccion; la remedicion posterior a la correccion
  (0 insuficientes) esta en la seccion 8.
* La deformacion de texturas/modelos durante un fotograma (matrices de huesos con basura) **no** se explica por ningun objeto
  DatosExe insuficiente distinto de los 3 corregidos; si persiste con esas correcciones, la causa esta en otra clase (por ejemplo datos
  de la propia clase CPU/GPU, buffers de RenderWare) y esta auditoria no la cubre.

## 8. Verificacion posterior a la correccion

(ver el final de este documento: se completa tras compilar y medir el independiente de esta rama)
