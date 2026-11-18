<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11771 con evidencia histórica y 670 pendientes de validación en 23 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Código del juego en el EXE: mapa de las 11,686 funciones

[Progreso del reversing](../ReversedClasses.md#cobertura-exe) · [Lista completa (CSV)](funciones.csv)

Región **0x401000–0x74FFFF**: 11,686 funciones, 2,979,290 bytes. Es el código propio del juego y la capa de plataforma; RenderWare, D3D y el CRT quedan fuera (se sustituyen enlazando bibliotecas equivalentes). Generado por `contrib/game_function_map.py`; cotas y método en `contrib/exe_coverage.py`.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| A · Falta: llamada desde código ya reemplazado | 0 | 0.0% | 0 | 0.0% |
| B · Falta: método virtual | 0 | 0.0% | 0 | 0.0% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 0 | 0.0% | 0 | 0.0% |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 0 | 0.0% | 0 | 0.0% |
| Envoltura pendiente (la DLL aún llama al EXE) | 0 | 0.0% | 0 | 0.0% |
| Reemplazada por C++ | 11,686 | 100.0% | 2,979,290 | 100.0% |

Las funciones de A–D son **lo que falta**. El grupo A es el piso para la independencia (la DLL ya depende de ellas); es una cota inferior porque hay 6,004 llamadas indirectas sin resolver. Las envolturas pendientes (que ya tienen hook pero llaman al EXE) se cuentan aparte, por eso A–D suman menos que en la tabla de cobertura de ReversedClasses. Listas: [A](pendientes-A.md) · [B](pendientes-B.md) · [C](pendientes-C.md) · [D](pendientes-D.md).

## Por bloque de direcciones (64 KB)

Ordenados de mayor a menor porcentaje reemplazado.

El código se enlaza por unidad de compilación, así que cada bloque suele corresponder a pocos módulos. Útil para ver dónde queda trabajo.

## Excluidas del objetivo

118 función(es) del rango del juego no se cuentan en las cifras de arriba porque no son viables en C++ puro (lista en [excluidas.csv](excluidas.csv)):

| Dirección | Bytes | Módulo estimado | Motivo |
| --- | ---: | ---: | ---: |
| 0x004027CD | 58 | ExeProtectionStub | bloque protector: pushad y lectura de AX/ECX/EDX como argumentos de registro; inviable en C++ puro; excluida el 15/11/2026 (lote 221A) |
| 0x00402A36 | 211 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00402DC5 | 9 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x004032C5 | 18 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x0040335F | 26 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403379 | 29 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x004033B2 | 11 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x004033F5 | 525 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x0040374C | 437 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403902 | 513 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403B03 | 120 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403B7B | 2 | ExeProtectionStub | bloque protector: pushad/popad o argumentos en EAX/EBX/ESI, retorno en EBX o en la bandera de acarreo, longjmp al marco de 0x403B03; inviable en C++ puro; excluida el 15/11/2026 (lote 222A) |
| 0x00403B7D | 151 | ExeProtectionStub | Bloque de proteccion y arranque del EXE: protocolo en registros, resultado en la bandera de acarreo y cambio de pila tipo longjmp (mov esp,[0xC9E0C0]); no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x00403C14 | 92 | ExeProtectionStub | Bloque de proteccion y arranque del EXE: entradas por registros y salida por acarreo; no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x00403C70 | 24 | ExeProtectionStub | Bloque de proteccion y arranque del EXE: entrada BH y salida por acarreo; no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x00403C88 | 127 | ExeProtectionStub | Bloque de proteccion y arranque del EXE: entrada BH y salida por acarreo; no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x00403D07 | 88 | ExeProtectionStub | Bloque de proteccion y arranque del EXE: reparte 10 argumentos de pila en seis registros; no hace falta en un ejecutable propio; excluida el 15/11/2026 (lote 222A) |
| 0x0048DE60 | 5 | CTaskSimpleAnim | no es una funcion con trabajo propio: un unico jmp a otra direccion; un hook de 5 bytes no se puede encadenar con seguridad (jmp 0x61A6F0); excluida el 15/11/2026 (lote 214A) |
| 0x0049B0E0 | 217 | CShopping | __usercall con registros de entrada que C++ no puede leer (EDI=identificador de la mejora) y jmp [tabla]; su unico llamador la tiene en linea; excluida el 15/11/2026 (lote 195A) |
| 0x0049EA70 | 5 | Fx_c | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x004A2BB0 | 5 | FxEmitter_c | no es una funcion con trabajo propio: un unico jmp a otra direccion; un hook de 5 bytes no se puede encadenar con seguridad (jmp 0x4A9F40); excluida el 15/11/2026 (lote 230A) |
| 0x004AFB40 | 5 | CEventAcquaintancePed | no es una funcion con trabajo propio: un unico jmp a CEventAcquaintancePed::AffectsPed (0x4AFA30), sin llamadores ni referencias de datos; excluida el 12/11/2026 (lote 178A) |
| 0x004B5C2A | 197 | CPedDamageResponseCalculator | no es una funcion: bloque interior de CPedDamageResponseCalculator/ComputeDamageResponse (0x4B5AC0), sin prologo; excluida el 12/11/2026 (lote 176A) |
| 0x004C4923 | 48 | CDamageAtomicModelInfo | no es una funcion: fragmento interno de CDamageAtomicModelInfo::CreateInstance (0x4C4910), que ya tiene hook (lote 173A); artefacto del mapa; excluida el 12/11/2026 |
| 0x004CA5A0 | 1,339 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (ESI=raster, EBX=modo) y 14 llamadas internas del controlador; excluida el 15/11/2026 (lote 223A) |
| 0x004CAAE0 | 673 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CAD90 | 104 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CAEA0 | 1,530 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CB560 | 78 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CB730 | 141 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CB7C0 | 512 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CB9C0 | 310 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CC0C0 | 129 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ECX/EDX/ESI/EDI segun la funcion); 0x4CB560 y 0x4CB730 reimplementadas como funciones locales sin hook; excluida el 15/11/2026 (lote 224A) |
| 0x004CC5C0 | 907 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI) o llama a funciones que las usan; excluida el 15/11/2026 (lote 225A) |
| 0x004CCD70 | 236 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI) o llama a funciones que las usan; excluida el 15/11/2026 (lote 225A) |
| 0x004CCE60 | 457 | RenderWare (D3D9 raster) | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI) o llama a funciones que las usan; excluida el 15/11/2026 (lote 225A) |
| 0x004D1680 | 971 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX=AnimBlendUpdateData); solo las llaman callbacks ya enganchados que usan la plantilla C++; excluida el 15/11/2026 (lote 214A) |
| 0x004D1A50 | 850 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX=AnimBlendUpdateData); solo las llaman callbacks ya enganchados que usan la plantilla C++; excluida el 15/11/2026 (lote 214A) |
| 0x004D1DB0 | 843 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX=AnimBlendUpdateData); solo las llaman callbacks ya enganchados que usan la plantilla C++; excluida el 15/11/2026 (lote 214A) |
| 0x004D2100 | 845 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D2450 | 910 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D27F0 | 910 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D6450 | 73 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D6510 | 256 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004D66A0 | 118 | RpAnimBlend | __usercall con registros de entrada que C++ no puede leer (EAX/ESI/EDI/EBX); cuerpos ya en C++ dentro de funciones enganchadas; excluida el 15/11/2026 (lote 215A) |
| 0x004F3A15 | 32 | STL (std::string) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de std::string::_Copy (0x4F39C0); excluida el 15/11/2026 (lote 208A) |
| 0x004F3A3E | 102 | STL (std::string) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de std::string::_Copy (0x4F39C0) y de 0x4F4320; excluida el 15/11/2026 (lote 209A) |
| 0x004F3AAD | 45 | STL (std::string) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de std::string::_Copy (0x4F39C0) y de 0x4F4320; excluida el 15/11/2026 (lote 209A) |
| 0x004F44B3 | 9 | STL (std::string) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de std::string::_Copy (0x4F39C0) y de 0x4F4320; excluida el 15/11/2026 (lote 209A) |
| 0x0059F000 | 5 | CHandObject | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x005A42B0 | 203 | CClothesBuilder | __usercall con registros de entrada que C++ no puede leer (EAX, ESI, EDI y EBX); el calculo ya esta en linea en CreateSkinnedClump; excluida el 15/11/2026 (lote 203A) |
| 0x005C4640 | 226 | libjpeg (jchuff/jcmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C4760 | 387 | libjpeg (jchuff/jcmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C48F0 | 200 | libjpeg (jchuff/jcmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C4B90 | 224 | libjpeg (jchuff/jcmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C54F0 | 104 | libjpeg (jchuff/jcmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C5560 | 102 | libjpeg (jchuff/jcmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EAX/EBX/BL/ECX); excluida el 15/11/2026 (lote 230A) |
| 0x005C7010 | 191 | libjpeg (jcparam/jdapistd) | __usercall con registros de entrada que C++ no puede leer (EAX/EBX/ESI); excluida el 15/11/2026 (lote 232A) |
| 0x005C70D0 | 82 | libjpeg (jcparam/jdapistd) | __usercall con registros de entrada que C++ no puede leer (EAX/EBX/ESI); excluida el 15/11/2026 (lote 232A) |
| 0x005C8620 | 100 | libjpeg (jcparam/jdapistd) | __usercall con registros de entrada que C++ no puede leer (EAX/EBX/ESI); excluida el 15/11/2026 (lote 232A) |
| 0x005C9C60 | 127 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CA2A0 | 493 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CA490 | 415 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CA630 | 170 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CA890 | 168 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CAF00 | 164 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CAFB0 | 777 | libjpeg (jdhuff/jdinput/jdmainct/jdmarker) | __usercall con registros de entrada que C++ no puede leer (ESI/EBX/AL/CL); excluida el 15/11/2026 (lote 233A) |
| 0x005CB2C0 | 751 | libjpeg (jdmarker/jdmaster) | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CB8C0 | 599 | libjpeg (jdmarker/jdmaster) | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CBC00 | 587 | libjpeg (jdmarker/jdmaster) | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CBE50 | 169 | libjpeg (jdmarker/jdmaster) | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CC950 | 119 | libjpeg (jdmarker/jdmaster) | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CC9D0 | 373 | libjpeg (jdmarker/jdmaster) | __usercall con registros de entrada que C++ no puede leer (ESI/EDI/EAX/ECX); excluida el 15/11/2026 (lote 234A) |
| 0x005CF1B0 | 148 | libjpeg (jmemmgr) | __usercall con registros de entrada que C++ no puede leer (ESI=matriz virtual); excluida el 15/11/2026 (lote 235A) |
| 0x005CF250 | 151 | libjpeg (jmemmgr) | __usercall con registros de entrada que C++ no puede leer (ESI=matriz virtual); excluida el 15/11/2026 (lote 235A) |
| 0x005CFA30 | 5 | libjpeg (jcomapi) | no es una funcion con trabajo propio: un unico jmp a otra direccion; un hook de 5 bytes no se puede encadenar con seguridad (jmp 0x5C6B90, jpeg_destroy_compress); excluida el 15/11/2026 (lote 235A) |
| 0x005CFE00 | 5 | libjpeg (jcomapi) | no es una funcion con trabajo propio: un unico jmp a otra direccion; un hook de 5 bytes no se puede encadenar con seguridad (jmp 0x5C6B90); excluida el 15/11/2026 (lote 236A) |
| 0x005CFE20 | 394 | libjpeg (jdapimin) | __usercall con registros de entrada que C++ no puede leer (ESI=cinfo); excluida el 15/11/2026 (lote 236A) |
| 0x005D84C0 | 207 | CustomBuildingPipeline | __usercall con registros de entrada que C++ no puede leer (ECX/EAX/ESI/EDI/EBX); sus UV ya estan en funciones locales sin hook de CustomPipeRenderCB; excluida el 15/11/2026 (lote 211A) |
| 0x005D8590 | 723 | CustomBuildingPipeline | __usercall con registros de entrada que C++ no puede leer (ECX/EAX/ESI/EDI/EBX); sus UV ya estan en funciones locales sin hook de CustomPipeRenderCB; excluida el 15/11/2026 (lote 211A) |
| 0x005DBE40 | 104 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005DD1E0 | 57 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005DD220 | 323 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005E3630 | 400 | CPed | __usercall con registros de entrada que C++ no puede leer (EBX=peaton, EDI=posicion, EAX=numero de particulas); excluida el 15/11/2026 (lote 199A) |
| 0x005E37C0 | 404 | CPed | __usercall con registros de entrada que C++ no puede leer (EBX=peaton, EDI=posicion, EAX=numero de particulas); excluida el 15/11/2026 (lote 199A) |
| 0x005ED0A3 | 9 | STL (std::vector) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los insert 0x5ECF10 y 0x5ED1B0; excluida el 15/11/2026 (lote 219A) |
| 0x005ED388 | 9 | STL (std::vector) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los insert 0x5ECF10 y 0x5ED1B0; excluida el 15/11/2026 (lote 219A) |
| 0x005ED643 | 9 | STL (std::vector) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los _Insert_n 0x5ED1B0, 0x5ED750 y 0x5EDA50; excluida el 15/11/2026 (lote 220A) |
| 0x005ED92A | 9 | STL (std::vector) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los _Insert_n 0x5ED1B0, 0x5ED750 y 0x5EDA50; excluida el 15/11/2026 (lote 220A) |
| 0x005EDBE3 | 9 | STL (std::vector) | no es una funcion: funclet de catch(...) del marco de otra funcion (lee [ebp-N] del padre, sin ret, solo lo invoca la tabla de excepciones del EXE); artefacto del mapa de los _Insert_n 0x5ED1B0, 0x5ED750 y 0x5EDA50; excluida el 15/11/2026 (lote 220A) |
| 0x00619910 | 410 | CCam | __usercall con registros de entrada que C++ no puede leer (EAX=camara, EDI y EBX=salidas); su unico llamador RsSavePresetView ya esta en C++; excluida el 15/11/2026 (lote 191A) |
| 0x00637320 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637350 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637380 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006373B0 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006373E0 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637410 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637440 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637470 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00655DD0 | 5 | CTaskComplexWalkRoundFire | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00655E00 | 5 | CTaskComplexWalkRoundFire | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006935B0 | 5 | CTaskSimpleHoldEntity | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006935E0 | 5 | CTaskSimpleHoldEntity | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006F55C0 | 5 | CTrain | no es una funcion con trabajo propio: un unico jmp a CVehicle::Render (0x6D0E60), ranura 18 de la vtable 0x872370 de CTrain; sin logica que reconstruir; excluida el 12/11/2026 (lote 174A) |
| 0x00709CF0 | 837 | CShadows | __usercall (EAX=normal, EBX=triangulo, EDI=parametros): C++ no puede engancharla sin __asm; ya reconstruida en source/game_sa/Shadows.cpp pero sin hook; excluida del objetivo el 12/11/2026 |
| 0x0070FA70 | 107 | CStencilShadows | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x00711160 | 136 | CStencilShadows | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x007111F0 | 136 | CStencilShadows | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x0072E080 | 180 | RenderWare (rwtexdict) | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x0072E140 | 217 | RenderWare (rwtexdict) | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x0072E220 | 79 | RenderWare (rwtexdict) | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x0072E270 | 236 | RenderWare (rwtexdict) | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x0072E360 | 60 | RenderWare (rwtexdict) | __usercall con registros de entrada que C++ no puede leer (convencion propia con EBX/EDI/ESI/EAX); reconstruidas como funciones privadas sin hook en el lote 95C; excluida el 15/11/2026 (lote 208A) |
| 0x00745920 | 327 | VideoModeSelectDialog | __usercall con registros de entrada que C++ no puede leer (EBX=combo); excluida el 15/11/2026 (lote 196A) |
| 0x00745CC0 | 252 | VideoMode | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00745DC0 | 130 | VideoMode | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00746480 | 300 | WinMain | __usercall con registros de entrada que C++ no puede leer (EAX=linea de comandos); excluida el 15/11/2026 (lote 196A) |
| 0x00746800 | 110 | VideoPlayer | __usercall anti-manipulacion: lee registros que C++ no puede leer; sin llamadores directos ni vtable; excluida del objetivo el 11/11/2026 (lote 161A, evidencia en reversing/Fn161A) |

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
| 0x4E0000 | 246 | 100% | 0 | 0 | — |
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
