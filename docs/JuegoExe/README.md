<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Código del juego en el EXE: mapa de las 11,684 funciones

[Progreso del reversing](../ReversedClasses.md#cobertura-exe) · [Lista completa (CSV)](funciones.csv)

Región **0x401000–0x74FFFF**: 11,684 funciones, 2,978,807 bytes. Es el código propio del juego y la capa de plataforma; RenderWare, D3D y el CRT quedan fuera (se sustituyen enlazando bibliotecas equivalentes). Generado por `contrib/game_function_map.py`; cotas y método en `contrib/exe_coverage.py`.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| A · Falta: llamada desde código ya reemplazado | 0 | 0.0% | 0 | 0.0% |
| B · Falta: método virtual | 0 | 0.0% | 0 | 0.0% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 0 | 0.0% | 0 | 0.0% |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 0 | 0.0% | 0 | 0.0% |
| Envoltura pendiente (la DLL aún llama al EXE) | 0 | 0.0% | 0 | 0.0% |
| Reemplazada por C++ | 11,684 | 100.0% | 2,978,807 | 100.0% |

Las funciones de A–D son **lo que falta**. El grupo A es el piso para la independencia (la DLL ya depende de ellas); es una cota inferior porque hay 6,004 llamadas indirectas sin resolver. Las envolturas pendientes (que ya tienen hook pero llaman al EXE) se cuentan aparte, por eso A–D suman menos que en la tabla de cobertura de ReversedClasses. Listas: [A](pendientes-A.md) · [B](pendientes-B.md) · [C](pendientes-C.md) · [D](pendientes-D.md).

## Por bloque de direcciones (64 KB)

Ordenados de mayor a menor porcentaje reemplazado.

El código se enlaza por unidad de compilación, así que cada bloque suele corresponder a pocos módulos. Útil para ver dónde queda trabajo.

## Excluidas del objetivo

150 función(es) del rango del juego no se cuentan en las cifras de arriba porque no son viables en C++ puro (lista en [excluidas.csv](excluidas.csv)):

Cada una está **cerrada con una categoría y una evidencia** por inspección del EXE ([reversing/FnG2A](../../reversing/FnG2A/README.md); evidencia en [excluidas-evidencia.json](../../reversing/FnG2A/excluidas-evidencia.json)):

| Categoría | Funciones |
| --- | ---: |
| `arranque-sin-gancho` | 27 |
| `no-es-funcion` | 11 |
| `proteccion-descartada` | 18 |
| `retirada-quicktime` | 2 |
| `salto-puro` | 20 |
| `sin-equivalente-cpp` | 1 |
| `sin-espacio-para-gancho` | 1 |
| `sustituida-por-crt` | 1 |
| `usercall-cubierta` | 52 |
| `usercall-renderware` | 17 |

| Dirección | Bytes | Módulo estimado | Categoría | Motivo |
| --- | ---: | ---: | ---: | ---: |
| 0x004027CD | 58 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad y lectura de AX/ECX/EDX como argumentos de registro; inviable en C++ puro; excluida el 15/11/2026 (lote 221A) |
| 0x00402A36 | 211 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00402DC5 | 9 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x004032C5 | 18 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x0040335F | 26 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403379 | 29 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x004033B2 | 11 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x004033F5 | 525 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x0040374C | 437 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403902 | 513 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403B03 | 120 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403B7B | 2 | ExeProtectionStub | proteccion-descartada | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403B7D | 151 | ExeProtectionStub | proteccion-descartada | Bloque de proteccion y arranque del EXE: protocolo en registros, resultado en la bandera de acarreo y cambio de pila tipo longjmp (mov esp,[0xC9E0C0]); no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x00403C14 | 92 | ExeProtectionStub | proteccion-descartada | Bloque de proteccion y arranque del EXE: entradas por registros y salida por acarreo; no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x00403C70 | 24 | ExeProtectionStub | proteccion-descartada | Bloque de proteccion y arranque del EXE: entrada BH y salida por acarreo; no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x00403C88 | 127 | ExeProtectionStub | proteccion-descartada | Bloque de proteccion y arranque del EXE: entrada BH y salida por acarreo; no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x00403D07 | 88 | ExeProtectionStub | proteccion-descartada | Bloque de proteccion y arranque del EXE: reparte 10 argumentos de pila en seis registros; no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x0048DE60 | 5 | CTaskSimpleAnim | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion; un hook de 5 bytes no se puede encadenar con seguridad (jmp 0x61A6F0); excluida el 15/11/2026 (lote 214A) |
| 0x0049B0E0 | 217 | CShopping | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EDI=identificador de la mejora) y jmp [tabla]; su unico llamador la tiene en linea; excluida el 15/11/2026 (lote 195A) |
| 0x0049EA70 | 5 | Fx_c | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x004A2BB0 | 5 | FxEmitter_c | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion; un hook de 5 bytes no se puede encadenar con seguridad (jmp 0x4A9F40); excluida el 15/11/2026 (lote 230A) |
| 0x004AFB40 | 5 | CEventAcquaintancePed | salto-puro | no es una funcion con trabajo propio: un unico jmp a CEventAcquaintancePed::AffectsPed (0x4AFA30), sin llamadores ni referencias de datos; excluida el 12/11/2026 (lote 178A) |
| 0x004B5C2A | 197 | CPedDamageResponseCalculator | no-es-funcion | no es una funcion: bloque interior de CPedDamageResponseCalculator/ComputeDamageResponse (0x4B5AC0), sin prologo; excluida el 12/11/2026 (lote 176A) |
| 0x004C4923 | 48 | CDamageAtomicModelInfo | no-es-funcion | no es una funcion: fragmento interno de CDamageAtomicModelInfo::CreateInstance (0x4C4910), que ya tiene hook (lote 173A); artefacto del mapa; excluida el 12/11/2026 |
| 0x004CA5A0 | 1,339 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (ESI=raster, EBX=modo) y 14 llamadas internas del controlador; excluida el 15/11/2026 (lote 223A) |
| 0x004CAAE0 | 673 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CAD90 | 104 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CAEA0 | 1,530 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CB560 | 78 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CB730 | 141 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CB7C0 | 512 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CB9C0 | 310 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CC0C0 | 129 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CC5C0 | 907 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI) o llama a funciones que las usan; excluida el 15/11/2026 (lote 225A) |
| 0x004CCD70 | 236 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI) o llama a funciones que las usan; excluida el 15/11/2026 (lote 225A) |
| 0x004CCE60 | 457 | RenderWare (D3D9 raster) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI) o llama a funciones que las usan; excluida el 15/11/2026 (lote 225A) |
| 0x004D1680 | 971 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX=AnimBlendUpdateData); solo las llaman callbacks ya enganchados que usan la plantilla C++; excluida el 15/11/2026 (lote 214A) |
| 0x004D1A50 | 850 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX=AnimBlendUpdateData); solo las llaman callbacks ya enganchados que usan la plantilla C++; excluida el 15/11/2026 (lote 214A) |
| 0x004D1DB0 | 843 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX=AnimBlendUpdateData); solo las llaman callbacks ya enganchados que usan la plantilla C++; excluida el 15/11/2026 (lote 214A) |
| 0x004D2100 | 845 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D2450 | 910 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D27F0 | 910 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D6450 | 73 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D6510 | 256 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D66A0 | 118 | RpAnimBlend | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004E7990 | 238 | CAEMFDecoder | retirada-quicktime | Ayudante de QuickTime (abre la pelicula de la pista de usuario): QuickTime se retira por decision del usuario el 19/11/2026 (lote EXT1A); su unico llamador, CAEMFDecoder::Initialise, devuelve false y ya no la llama; excluida del objetivo el 20/11/2026 |
| 0x004E7A80 | 245 | CAEMFDecoder | retirada-quicktime | Ayudante de QuickTime (lee el formato de sonido de la pelicula): QuickTime se retira por decision del usuario el 19/11/2026 (lote EXT1A); su unico llamador, CAEMFDecoder::Initialise, devuelve false y ya no la llama; excluida del objetivo el 20/11/2026 |
| 0x004F3A15 | 32 | STL (std::string) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de std::string::_Copy (0x4F39C0); excluida el 15/11/2026 (lote 208A) |
| 0x004F3A3E | 102 | STL (std::string) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de std::string::_Copy (0x4F39C0) y de 0x4F4320; excluida el 15/11/2026 (lote 209A) |
| 0x004F3AAD | 45 | STL (std::string) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de std::string::_Copy (0x4F39C0) y de 0x4F4320; excluida el 15/11/2026 (lote 209A) |
| 0x004F44B3 | 9 | STL (std::string) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de std::string::_Copy (0x4F39C0) y de 0x4F4320; excluida el 15/11/2026 (lote 209A) |
| 0x0059F000 | 5 | CHandObject | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x005A42B0 | 203 | CClothesBuilder | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX, ESI, EDI y EBX); el calculo ya esta en linea en CreateSkinnedClump; excluida el 15/11/2026 (lote 203A) |
| 0x005C4640 | 226 | libjpeg (jchuff/jcmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C4760 | 387 | libjpeg (jchuff/jcmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C48F0 | 200 | libjpeg (jchuff/jcmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C4B90 | 224 | libjpeg (jchuff/jcmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C54F0 | 104 | libjpeg (jchuff/jcmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C5560 | 102 | libjpeg (jchuff/jcmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C7010 | 191 | libjpeg (jcparam/jdapistd) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/EBX/ESI); excluida el 15/11/2026 (lote 232A) |
| 0x005C70D0 | 82 | libjpeg (jcparam/jdapistd) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/EBX/ESI); excluida el 15/11/2026 (lote 232A) |
| 0x005C8620 | 100 | libjpeg (jcparam/jdapistd) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX/EBX/ESI); excluida el 15/11/2026 (lote 232A) |
| 0x005C9C60 | 127 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CA2A0 | 493 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CA490 | 415 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CA630 | 170 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CA890 | 168 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CAF00 | 164 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CAFB0 | 777 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CB2C0 | 751 | libjpeg (jdmarker/jdmaster) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CB8C0 | 599 | libjpeg (jdmarker/jdmaster) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CBC00 | 587 | libjpeg (jdmarker/jdmaster) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CBE50 | 169 | libjpeg (jdmarker/jdmaster) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CC950 | 119 | libjpeg (jdmarker/jdmaster) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CC9D0 | 373 | libjpeg (jdmarker/jdmaster) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CF1B0 | 148 | libjpeg (jmemmgr) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI=matriz virtual); excluida el 15/11/2026 (lote 235A) |
| 0x005CF250 | 151 | libjpeg (jmemmgr) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI=matriz virtual); excluida el 15/11/2026 (lote 235A) |
| 0x005CFA30 | 5 | libjpeg (jcomapi) | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion; un hook de 5 bytes no se puede encadenar con seguridad (jmp 0x5C6B90, jpeg_destroy_compress); excluida el 15/11/2026 (lote 235A) |
| 0x005CFE00 | 5 | libjpeg (jcomapi) | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion; un hook de 5 bytes no se puede encadenar con seguridad (jmp 0x5C6B90); excluida el 15/11/2026 (lote 236A) |
| 0x005CFE20 | 394 | libjpeg (jdapimin) | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ESI=cinfo); excluida el 15/11/2026 (lote 236A) |
| 0x005D84C0 | 207 | CustomBuildingPipeline | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ECX/EAX/ESI/EDI/EBX); sus UV ya estan en funciones locales sin hook de CustomPipeRenderCB; excluida el 15/11/2026 (lote 211A) |
| 0x005D8590 | 723 | CustomBuildingPipeline | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (ECX/EAX/ESI/EDI/EBX); sus UV ya estan en funciones locales sin hook de CustomPipeRenderCB; excluida el 15/11/2026 (lote 211A) |
| 0x005DBE40 | 104 | CPlantMgr | usercall-cubierta | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005DD1E0 | 57 | CPlantMgr | usercall-cubierta | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005DD220 | 323 | CPlantMgr | usercall-cubierta | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005E3630 | 400 | CPed | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EBX=peaton, EDI=posicion, EAX=numero de particulas); excluida el 15/11/2026 (lote 199A) |
| 0x005E37C0 | 404 | CPed | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EBX=peaton, EDI=posicion, EAX=numero de particulas); excluida el 15/11/2026 (lote 199A) |
| 0x005ED0A3 | 9 | STL (std::vector) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los insert 0x5ECF10 y 0x5ED1B0; excluida el 15/11/2026 (lote 219A) |
| 0x005ED388 | 9 | STL (std::vector) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los insert 0x5ECF10 y 0x5ED1B0; excluida el 15/11/2026 (lote 219A) |
| 0x005ED643 | 9 | STL (std::vector) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los _Insert_n 0x5ED1B0, 0x5ED750 y 0x5EDA50; excluida el 15/11/2026 (lote 220A) |
| 0x005ED92A | 9 | STL (std::vector) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los _Insert_n 0x5ED1B0, 0x5ED750 y 0x5EDA50; excluida el 15/11/2026 (lote 220A) |
| 0x005EDBE3 | 9 | STL (std::vector) | no-es-funcion | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los _Insert_n 0x5ED1B0, 0x5ED750 y 0x5EDA50; excluida el 15/11/2026 (lote 220A) |
| 0x00619910 | 410 | CCam | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EAX=camara, EDI y EBX=salidas); su unico llamador RsSavePresetView ya esta en C++; excluida el 15/11/2026 (lote 191A) |
| 0x00637320 | 5 | CTaskSimpleHitFromFront | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637350 | 5 | CTaskSimpleHitFromFront | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637380 | 5 | CTaskSimpleHitFromFront | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006373B0 | 5 | CTaskSimpleHitFromFront | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006373E0 | 5 | CTaskSimpleHitFromFront | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637410 | 5 | CTaskSimpleHitFromFront | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637440 | 5 | CTaskSimpleHitFromFront | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637470 | 5 | CTaskSimpleHitFromFront | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00655DD0 | 5 | CTaskComplexWalkRoundFire | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00655E00 | 5 | CTaskComplexWalkRoundFire | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006935B0 | 5 | CTaskSimpleHoldEntity | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006935E0 | 5 | CTaskSimpleHoldEntity | salto-puro | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006F55C0 | 5 | CTrain | salto-puro | no es una funcion con trabajo propio: un unico jmp a CVehicle::Render (0x6D0E60), ranura 18 de la vtable 0x872370 de CTrain; sin logica que reconstruir; excluida el 12/11/2026 (lote 174A) |
| 0x00709CF0 | 837 | CShadows | usercall-cubierta | __usercall (EAX=normal, EBX=triangulo, EDI=parametros): C++ no puede engancharla sin __asm; ya reconstruida en source/game_sa/Shadows.cpp pero sin hook; excluida del objetivo el 12/11/2026 |
| 0x0070FA70 | 107 | CStencilShadows | usercall-cubierta | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x00711160 | 136 | CStencilShadows | usercall-cubierta | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x007111F0 | 136 | CStencilShadows | usercall-cubierta | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x0072E080 | 180 | RenderWare (rwtexdict) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x0072E140 | 217 | RenderWare (rwtexdict) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x0072E220 | 79 | RenderWare (rwtexdict) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x0072E270 | 236 | RenderWare (rwtexdict) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x0072E360 | 60 | RenderWare (rwtexdict) | usercall-renderware | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x00745920 | 327 | VideoModeSelectDialog | usercall-cubierta | __usercall con registros de entrada que C++ no puede leer (EBX=combo); excluida el 15/11/2026 (lote 196A) |
| 0x00745CC0 | 252 | VideoMode | usercall-cubierta | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00745DC0 | 130 | VideoMode | usercall-cubierta | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00746480 | 300 | WinMain | sustituida-por-crt | __usercall con registros de entrada que C++ no puede leer (EAX=linea de comandos); excluida el 15/11/2026 (lote 196A) |
| 0x00746800 | 110 | VideoPlayer | proteccion-descartada | __usercall anti-manipulacion: lee registros que C++ no puede leer; sin llamadores directos ni vtable; excluida del objetivo el 11/11/2026 (lote 161A, evidencia en reversing/Fn161A) |
| 0x0076E664 | 4 | D3DX9 (compilador de shaders) | sin-espacio-para-gancho | Funcion de 4 bytes (mov eax,[ecx+8]; ret) seguida de otra de 4 bytes (0x76E668): un gancho de 5 bytes pisaria a la vecina; ya reconstruida como D3dxErrorLog::GetErrorCount (D3dxErrorLog.h) y usada por D3dxShaderAssembler::Assemble; excluida del gancho el 21/11/2026 (lote NEWDEL) |
| 0x0082134F | 56 | CRT (despacho de intrinsecas) | arranque-sin-gancho | Arranque del CRT: seis punteros a rutinas del EXE en 0x8E30F0; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (FpInitDispatch, dentro de FpMath); excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00821397 | 23 | CRT (__fpmath, _FPinit) | arranque-sin-gancho | Arranque del CRT: la llama __cinit por el puntero _FPinit (0x8E2B74) antes de la tabla C: tabla de despacho de intrinsecas, _adjust_fdiv, precision de 53 bits y fnclex; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (FpMath) y comparada con el EXE fuera del juego; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00823AFE | 47 | CRT (__crtExitProcess) | arranque-sin-gancho | Cierre del CRT: CorExitProcess de mscoree.dll si esta cargada y ExitProcess; no vuelve; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (CrtExitProcess), sin hook ni banco (no se puede ejecutar fuera del juego sin terminar el proceso); excluida del objetivo el 21/11/2026 (parte CRT de la fase 8) |
| 0x00823B76 | 101 | CRT (__cinit) | arranque-sin-gancho | Arranque del CRT: gancho previo, tabla C, atexit(RTC) e inicializadores de C++; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (Cinit) con las 1.489 constantes x87, la cookie y la tabla C completa; los constructores y las asignaciones de datos del juego quedan para DatosExe; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00823BDB | 211 | CRT (_doexit) | arranque-sin-gancho | Cierre del CRT: ejecuta atexit/_onexit en orden inverso, los terminadores y sale por __crtExitProcess; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (DoExit) y comparada con el EXE fuera del juego (retcaller = 1), sin hook propio: la usa exit (0x823CAE, enganchada en CrtFile.cpp) y el cierre del EXE independiente; excluida del objetivo el 21/11/2026 (parte CRT de la fase 8) |
| 0x00826A72 | 45 | CRT (tabla C de __cinit) | arranque-sin-gancho | Arranque del CRT: ocho bytes a 0xFF en 0x8E37B4; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (TableC826A72) y comparada con el EXE fuera del juego; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00826EC4 | 64 | CRT (__ms_p5_test_fdiv) | arranque-sin-gancho | Arranque del CRT: prueba de la division del Pentium; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (FdivBugCheck, dentro de FpMath); excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00826F04 | 41 | CRT (__ms_p5_mp_test_fdiv) | arranque-sin-gancho | Arranque del CRT: IsProcessorFeaturePresent(0) por nombre o la prueba FDIV; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (DetectFdivBug, dentro de FpMath); excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00827B0C | 30 | CRT (__mtterm) | arranque-sin-gancho | Cierre del CRT: borra los bloqueos y libera el indice TLS; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (Mtterm); excluida del objetivo el 21/11/2026 (parte P5, lote F1) |
| 0x00827D25 | 111 | CRT (__mtinit) | arranque-sin-gancho | Arranque del CRT: bloqueos, indice TLS y datos del hilo principal; se ejecuta una sola vez antes de que exista ningun gancho; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (Mtinit); excluida del objetivo el 21/11/2026 (parte P5, lote F1) |
| 0x008289C0 | 46 | CRT (prueba de SSE2) | arranque-sin-gancho | Arranque del CRT: ejecuta una instruccion SSE2 bajo SEH; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (Sse2ExecutionProbe) y comparada con el EXE fuera del juego (dentro de 0x8289F5); excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x008289F5 | 87 | CRT (__sse2_available_init) | arranque-sin-gancho | Arranque del CRT: CPUID y prueba SSE2 que elige el nucleo de pow/floor/ceil (0xC9C400); ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (TableCSseAvailableInit) y comparada con el EXE fuera del juego; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x0082AC0E | 70 | CRT (filtro de excepciones no manejadas) | arranque-sin-gancho | Filtro registrado por 0x82AC54: terminate para excepciones de C++ sin capturar y encadenado al filtro anterior; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (CrtUnhandledExceptionFilter; terminate del CRT enlazado) y comparada con el EXE fuera del juego salvo la rama de terminate; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x0082AC74 | 73 | CRT (__mtinitlocks) | arranque-sin-gancho | Arranque del CRT: inicializa las 36 secciones criticas estaticas; se ejecuta una sola vez antes de que exista ningun gancho; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (Mtinitlocks); excluida del objetivo el 21/11/2026 (parte P5, lote F1) |
| 0x0082ACBD | 81 | CRT (__mtdeletelocks) | arranque-sin-gancho | Cierre del CRT: destruye las secciones criticas; se ejecuta al terminar el proceso o si falla __mtinit; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (Mtdeletelocks); excluida del objetivo el 21/11/2026 (parte P5, lote F1) |
| 0x0082C0B9 | 510 | CRT (_ioinit) | arranque-sin-gancho | Arranque del CRT: crea __pioinfo[0] y hereda los descriptores del proceso padre; se ejecuta una sola vez antes de que exista ningun gancho; ya reconstruida en source/game_sa/Crt/CrtLowIo.cpp (Ioinit) y comparada con el EXE fuera del juego, sin hook; excluida del objetivo el 21/11/2026 (lote CRT5A) |
| 0x0082C457 | 26 | CRT (__heap_select) | arranque-sin-gancho | Arranque del CRT: elige el montón del sistema (1) o el de bloques pequeños (3) segun la version del sistema; se ejecuta una sola vez antes de que exista ningun gancho; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (HeapSelect); excluida del objetivo el 21/11/2026 (parte P5, lote F1) |
| 0x0082C471 | 81 | CRT (__heap_init) | arranque-sin-gancho | Arranque del CRT: HeapCreate del montón del CRT y __heap_select; se ejecuta una sola vez antes de que exista ningun gancho; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (HeapInit; el camino del montón de bloques pequeños no se reproduce); excluida del objetivo el 21/11/2026 (parte P5, lote F1) |
| 0x0082D8F3 | 166 | CRT (__setenvp) | arranque-sin-gancho | Arranque del CRT: construye _environ desde el bloque de entorno; se ejecuta una sola vez antes de que exista ningun gancho; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (Setenvp) y comparada con el EXE fuera del juego, sin hook; excluida del objetivo el 21/11/2026 (parte CRT de la fase 8) |
| 0x0082D9BA | 364 | CRT (parse_cmdline de __setargv) | arranque-sin-gancho | Arranque del CRT: cuenta o copia los argumentos de la linea de comandos (usercall: EAX, ECX y ESI); ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (ParseCmdline, ayudante de Setargv) y comparada con el EXE fuera del juego, sin hook; excluida del objetivo el 21/11/2026 (parte CRT de la fase 8) |
| 0x0082DB26 | 162 | CRT (__setargv) | arranque-sin-gancho | Arranque del CRT: nombre del ejecutable (_pgmptr) y linea de comandos partida en __argc/__argv; se ejecuta una sola vez antes de que exista ningun gancho; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (Setargv, con el analizador parse_cmdline 0x82D9BA dentro) y comparada con el EXE fuera del juego, sin hook; excluida del objetivo el 21/11/2026 (parte CRT de la fase 8) |
| 0x0082DBC8 | 285 | CRT (__crtGetEnvironmentStringsA) | arranque-sin-gancho | Arranque del CRT: copia ANSI del bloque de entorno (API ancha o ANSI segun el sistema); se ejecuta una sola vez antes de que exista ningun gancho; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (CrtGetEnvironmentStrings) y comparada con el EXE fuera del juego, sin hook; excluida del objetivo el 21/11/2026 (parte CRT de la fase 8) |
| 0x00832ECF | 24 | CRT (_ValidateExecute) | arranque-sin-gancho | Valida un puntero a codigo con IsBadCodePtr; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (IsValidCodePointer) y comparada con el EXE fuera del juego (dentro de 0x82AC0E); excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00832F45 | 47 | CRT (LCID de la pagina multibyte) | arranque-sin-gancho | usercall (pagina en EAX): LCID de las cuatro paginas de Asia oriental; ya reconstruida como ayudante de source/game_sa/Crt/CrtMbcs.cpp (MbcpToLcid) y comparada con el EXE dentro de _setmbcp; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00833494 | 30 | CRT (__initmbctable) | arranque-sin-gancho | Arranque del CRT (tabla C de __cinit y __setargv): _setmbcp(-3), tabla _mbctype segun la pagina ANSI; ya reconstruida en source/game_sa/Crt/CrtMbcs.cpp (InitMbcTable) y comparada con el EXE fuera del juego; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x008339CA | 86 | CRT (__security_init_cookie) | arranque-sin-gancho | Arranque del CRT (primer inicializador de C++): mezcla hora, identificadores y contadores en la cookie 0x8E31BC; ya reconstruida en source/game_sa/Crt/CrtStartup.cpp (InitSecurityCookie) y comparada con el EXE fuera del juego con las llamadas del sistema fijadas; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x00833FF8 | 743 | CRT (_sopen_helper) | usercall-cubierta | __usercall: el argumento shflag viaja en ECX (C++ no puede engancharla sin __asm); ya reconstruida en source/game_sa/Crt/CrtLowIo.cpp (SopenHelper, funcion interna) pero sin hook; sus unicas llamadoras (_sopen 0x834334 y _open 0x8342DF) estan reconstruidas, enganchadas y comparadas con el EXE fuera del juego; excluida del objetivo el 21/11/2026 (lote CRT5A) |
| 0x00834B6E | 28 | CRT (_strncnt) | arranque-sin-gancho | Longitud acotada de una cadena (usercall: cadena en EAX, cuenta en la pila); ya reconstruida en source/game_sa/Crt/CrtMbcs.cpp (Strncnt) y comparada con el EXE fuera del juego con un adaptador; excluida del objetivo el 21/11/2026 (parte CRT-B de la fase 8) |
| 0x0085742B | 7 | CRT (D3D9RestoreFpuControlWord) | sin-equivalente-cpp | fldcw word ptr [0xCAF004]; ret: carga la palabra de control x87 guardada por el juego. _control87/_controlfp_s NO son equivalentes (banco FnCRT3A, experimento fldcw): traducen los bits (precision, redondeo) y reescriben tambien MXCSR (con redondeo distinto de «al mas cercano» o con excepciones sin enmascarar el MXCSR cambia de 0x1F80 a 0x7F80/0x3F80/0x5F80) y no pueden cargar la palabra tal cual (PC=01 reservado, bit 6, bits 13-15); excluida del objetivo el 21/11/2026 (lote CRT3A) |

| Bloque | Funciones | Reemplazadas | Faltan | Bytes que faltan | Módulos con más faltantes (estimado) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0x400000 | 264 | 100% | 0 | 0 | — |
| 0x410000 | 162 | 100% | 0 | 0 | — |
| 0x420000 | 135 | 100% | 0 | 0 | — |
| 0x430000 | 200 | 100% | 0 | 0 | — |
| 0x440000 | 194 | 100% | 0 | 0 | — |
| 0x450000 | 172 | 100% | 0 | 0 | — |
| 0x460000 | 164 | 100% | 0 | 0 | — |
| 0x470000 | 46 | 100% | 0 | 0 | — |
| 0x480000 | 110 | 100% | 0 | 0 | — |
| 0x490000 | 151 | 100% | 0 | 0 | — |
| 0x4A0000 | 468 | 100% | 0 | 0 | — |
| 0x4B0000 | 359 | 100% | 0 | 0 | — |
| 0x4C0000 | 358 | 100% | 0 | 0 | — |
| 0x4D0000 | 310 | 100% | 0 | 0 | — |
| 0x4E0000 | 244 | 100% | 0 | 0 | — |
| 0x4F0000 | 199 | 100% | 0 | 0 | — |
| 0x500000 | 305 | 100% | 0 | 0 | — |
| 0x510000 | 61 | 100% | 0 | 0 | — |
| 0x520000 | 62 | 100% | 0 | 0 | — |
| 0x530000 | 275 | 100% | 0 | 0 | — |
| 0x540000 | 176 | 100% | 0 | 0 | — |
| 0x550000 | 257 | 100% | 0 | 0 | — |
| 0x560000 | 212 | 100% | 0 | 0 | — |
| 0x570000 | 110 | 100% | 0 | 0 | — |
| 0x580000 | 129 | 100% | 0 | 0 | — |
| 0x590000 | 254 | 100% | 0 | 0 | — |
| 0x5A0000 | 115 | 100% | 0 | 0 | — |
| 0x5B0000 | 157 | 100% | 0 | 0 | — |
| 0x5C0000 | 112 | 100% | 0 | 0 | — |
| 0x5D0000 | 330 | 100% | 0 | 0 | — |
| 0x5E0000 | 247 | 100% | 0 | 0 | — |
| 0x5F0000 | 292 | 100% | 0 | 0 | — |
| 0x600000 | 237 | 100% | 0 | 0 | — |
| 0x610000 | 326 | 100% | 0 | 0 | — |
| 0x620000 | 282 | 100% | 0 | 0 | — |
| 0x630000 | 577 | 100% | 0 | 0 | — |
| 0x640000 | 425 | 100% | 0 | 0 | — |
| 0x650000 | 385 | 100% | 0 | 0 | — |
| 0x660000 | 300 | 100% | 0 | 0 | — |
| 0x670000 | 290 | 100% | 0 | 0 | — |
| 0x680000 | 184 | 100% | 0 | 0 | — |
| 0x690000 | 308 | 100% | 0 | 0 | — |
| 0x6A0000 | 112 | 100% | 0 | 0 | — |
| 0x6B0000 | 83 | 100% | 0 | 0 | — |
| 0x6C0000 | 135 | 100% | 0 | 0 | — |
| 0x6D0000 | 179 | 100% | 0 | 0 | — |
| 0x6E0000 | 92 | 100% | 0 | 0 | — |
| 0x6F0000 | 181 | 100% | 0 | 0 | — |
| 0x700000 | 136 | 100% | 0 | 0 | — |
| 0x710000 | 156 | 100% | 0 | 0 | — |
| 0x720000 | 198 | 100% | 0 | 0 | — |
| 0x730000 | 278 | 100% | 0 | 0 | — |
| 0x740000 | 190 | 100% | 0 | 0 | — |

## Por módulo (estimado)

Módulos con alguna función por reconstruir, de menos a más funciones que faltan. Para funciones sin nombre el módulo es la heurística de vecindad descrita arriba; la columna «Confianza alta» cuenta las faltantes cuyo módulo coincide en ambos vecinos.

| Módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan | Confianza alta |
| --- | ---: | ---: | ---: | ---: | ---: |

0 de 782 módulos tienen algo pendiente.

## Cómo usarlo

- Filtra `funciones.csv` por `estado` = `A` y ordena por `llamadas_desde_reemplazadas` para empezar por lo más usado por código ya reconstruido.
- Una función de 16 bytes o menos suele ser un stub o thunk; mira `thunk` antes de tratarla como trabajo.
- Los grupos C y D pueden contener código muerto: confirma con análisis dinámico antes de reconstruir.
