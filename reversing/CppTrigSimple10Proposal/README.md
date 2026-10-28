<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Diez helpers trigonométricos simples: propuesta offline

Pendiente: no aplicada, compilada, ejecutada ni aprobada. Diez helpers se sustituyen por cuatro primitivas C++ propias de un TU separado; se retirarían diez naked y diez bloques asm (20 apariciones). Los helpers no tienen entradas propias catalogadas del EXE: se valida a través de sus consumidores y el código emitido, no inventando direcciones.

| Helpers actuales | Secuencia | Consumidor EXE | Banco existente |
|---|---|---|---|
| BirdsSinMul / BirdsCosMul | FLD float; FSIN/FCOS; FMUL float; RET | CBirds::Render 0x712810 | Misc6, 512 pares Render |
| BrightSinMulX87 / BrightCosMulX87 | misma | CBrightLights::Render 0x7241C0 | Render23, 512 pares Render |
| PlaneTrailsSinMulX87 / PlaneTrailsCosMulX87 | misma | CPlaneTrails::Update 0x7174F0 | Render23, 1024 pares Update |
| JpMulSinX87 / JpMulCosX87 | misma | CTaskSimpleJetPack::ProcessPed 0x6801F0 (VMT slot7) | CTasks14b, 1024 pares ProcessPed |
| SinX87 / CosX87 de TaskSimpleSwim | FLD float; FSIN/FCOS; RET | CTaskSimpleSwim::ProcessControlInput 0x688A90 | Mix11, 2048 pares ProcessControlInput |

Los pares señalados son cobertura histórica disponible, no resultados de la migración. Requiere comprobar que los escenarios ejercitan todas las ramas consumidoras. Cada banco completo conserva sus acompañantes; no se atribuye conversión a ellos.

## Fuente propuesta

TrigSimpleCpp.cpp declara sin/cos double como intrínsecos y usa las expresiones double del patrón probado por el coordinador en CloudsMathCpp. Mantiene retorno float en ST0, cdecl y parámetros float. No hay std::sin(float), sqrt, atan2, sinMulAdd, ABI de registros especiales, delegación GAME ni funciones vacías.

El parche añade las declaraciones externas vía TrigSimpleCpp.h, retira solo los diez cuerpos y sustituye sus llamadas por las primitivas propias. Mantiene opciones de cada TU consumidor; únicamente TrigSimpleCpp.cpp usa /O2 /Oi /arch:IA32 /fp:fast, /Zi en Debug, sin PCH ni Unity ni runtime checks, igual que CloudsMathCpp.

## Condiciones previas a medir

Inspeccionar objeto real y confirmar: FLD DWORD parámetro; FSIN/FCOS; en variantes multiplicadas FMUL DWORD segundo parámetro; RET; sin redondeo/store intermedio, CRT calls, SSE, registros ECX/EDX modificados, ni prólogo /ZI/RTC que altere flags. Si el compilador emite conversiones double en memoria u otro orden, la semejanza textual no demuestra fidelidad: corregir/descartar antes de admitir.

También confirmar retorno ST0 y consumo en callers, preservación de control/status x87 relevante y comportamiento fuera de rango de FSIN/FCOS (|angle|>=2^63), NaN/infinito, cero firmado y PC/RC reales. El patrón intrínseco depende de instrucciones emitidas; no constituye promesa universal del estándar C++.

Los bancos históricos comparan snapshots/trazas/resultados pero pueden omitir FP status/FIP/raw ABI; no concederles esas garantías adicionales. Derivar bancos nuevos si se requiere ampliar evidencia, sin modificar ni debilitar los antiguos. No se proporciona aprobación ni resultados dinámicos en esta carpeta.

## Verificación offline

Diez cuerpos encontrados una sola vez y retirados de las propuestas generadas en memoria. No se escribe producción. El parche incluye cinco consumidores, CMake y dos archivos nuevos. Bancos existentes localizados y conteos consultados en sus tablas Fn. No se compila ni corre GTA ni se modifica Git.

Evidencia puntual de las instrucciones nativas en exe-consumer-evidence.txt, obtenida con dis.sh. No son direcciones de entradas independientes de los helpers.
