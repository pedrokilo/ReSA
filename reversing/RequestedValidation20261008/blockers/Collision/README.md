<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# SphereVsEntity, análisis estático canónico

Identidad: CCollision::SphereVsEntity, 0x0041A5A0, función completa [0x41A5A0,0x41A811), 625 bytes. EXE SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`; bytes SHA-256 `0a875b4a749021293a9b5956f58b406fd00d75a555c766bf1a2d51329d33f1ce`. `identity.json` conserva hashes y herramienta.

Evidencia: `41A5A0.combo.txt` obtenido mediante `reversing/_handoff/combo.sh`. dis.sh usa D:/Grand Theft Auto San Andreas/gta-sa.exe, cuyo hash se comprobó igual al canónico. El ensamblador independiente `41A5A0.complete.asm.txt` decodifica TODOS los bytes desde entrada hasta RET final (Capstone 5.0.7), sin truncamiento. `41A5A0.bytes.bin` conserva esos bytes.

## Pseudocódigo REA y límites

No existe pseudocódigo REA acreditado para esta función en el volcado histórico: el índice registra invalid_request. combo conserva ese error. La llamada REA viva open_binary confirmó hash e identidad x86; analyze_function(procedure="0x41a5a0") devolvió provider_unavailable: el fork local Ghidra sólo admite x64 y GTAIV x86 de hash específico; Hopper no admite Windows e IDA no está configurado. `rea-live-error.json` conserva la respuesta exacta. Este fallo no impide análisis del ensamblador; no prueba imposibilidad de equivalencia. No se modificó configuración ni se instaló software.

## Observaciones del ensamblador

Sea S el ESP al entrar, apuntando al retorno. La función es cdecl, sphere=[S+4], entity=[S+8], resultado bool en AL; los dos epílogos usan RET sin limpieza de argumentos. 0x41A5B9 lee entity. No hay lectura del puntero sphere ni de [S+4] en el cuerpo.

0x41A5C0 comprueba byte entity+0x1C bit 0. Si vale cero, 0x41A5D5 pone AL=0 y retorna 0x41A5E9. Si vale uno, llama a CCamera::IsExtraEntityToIgnore (0x50CE80) con this=0xB6F028 y entity. Si devuelve AL!=0, retorna también false. Estas rutas NO leen la esfera local y son comparables determinísticamente.

La ruta restante guarda EBX/EBP/EDI y dispone de ESP=S-0x90. La esfera que consumen las pruebas está en [ESP+0x28..0x37], es decir [S-0x68..S-0x59]; x/y/z/radio se leen en 0x41A665/6B/72/79 (S-0x68,-0x64,-0x60,-0x5C). No se escriben esas 16 posiciones antes de leerlas. Los stores 0x41A60C/610/61D/630 afectan campos de la matriz/estado SEH, no ese intervalo. Invert(0x59B920) recibe salida [ESP+0x3C]; luego se destruye esa matriz (0x59ACD0). No se usa para transformar una esfera.

AllocateMatrix(0x54F560) y CSimpleTransform::UpdateMatrix(0x54F1B0) se llaman si entity+0x14 es nulo. Luego GetColModel(0x535300) entrega el modelo y [modelo+0x2C] los datos. Si éstos son nulos, 0x41A650 salta a false 0x41A7F6, SIN leer esfera. Esta tercera ruta es también determinista aunque incluye colaboradores/efectos.

Esferas: contador int16 con signo [data+0], desde última a primera, radio sumado al local, distancia cuadrada en orden z,y,x. El test AH&0x41 decide la comparación x87; NaN no se acepta como contacto en ese test.

Cajas: contador int16 [data+2], recorrido inverso; TestSphereBox(0x4120C0) recibe puntero a [ESP+0x28]. Triángulos: CalculateTrianglePlanes(0x416330), contador [data+4], vértices+0x14, triángulos+0x18 y planos+0x1C; TestSphereTriangle(0x4165B0) recibe la misma esfera local. Con grupos (data+7 bit2) se recorre contador previo a triángulos y grupos de 0x1C desde atrás. Sin grupos recorre triángulos desde último a primero. Contacto retorna AL=1 por 0x41A72E, agotamiento AL=0 por 0x41A7F6; el epílogo final termina 0x41A810.

## Diferencia del candidato y decisión

El candidato actual (Collision.cpp:4176) contiene únicamente ZoneScoped y NOTSA_UNREACHABLE. No implementa ni siquiera los early exits deterministas de AL=0. En Debug el macro llama a notsa::unreachable; en otras configuraciones es intrinsic unreachable (Base.h:115/117). No puede acreditarse equivalencia a un cuerpo original que retorna false para entradas válidas. Esto es una diferencia estática concreta de implementación; no un fallo de arranque ni una comparación runtime observada.

La frase histórica "no hay comportamiento determinista que comparar" es demasiado amplia: existen al menos las tres rutas mencionadas. La ausencia histórica de llamadores no elimina la obligación de validar una función del alcance; no se renovó aquí ese análisis de llamadas.

## ¿Se puede recuperar comparabilidad sin arreglar el bug del EXE?

Sí, técnicamente. La máquina x86 es determinista cuando se incluye el estado inicial de pila relevante. No se debe inicializar la esfera desde sphere ni escoger cero arbitrariamente: cambiaría el comportamiento. Tampoco basta un CColSphere local C++ sin inicializar: el compilador cambia layout y puede explotar UB.

Una reconstrucción fiel puede usar un pequeño puente x86 antes del prólogo de C++ que capture los 16 bytes de [S-0x68..S-0x59] como entrada implícita del EXE, y un cuerpo reconstruido que conserve orden, firmas y efectos. Alternativamente se puede reconstruir instrucción por instrucción el cuerpo x86 (con relocaciones propias de llamadas), sin redirigir al original. Debe conservarse también el comportamiento de excepciones/SEH si el contrato incluye entradas inválidas. Capturar el intervalo antes de cualquier push/call/store que pueda solaparlo es requisito; no asumir que un prólogo C++ mantiene esos bytes.

El banco debe usar un wrapper de llamada con ESP controlado y sembrar el MISMO intervalo en original y candidato, variando bytes (esferas finitas dentro/fuera/tangencia, radio 0/negativo, NaN e infinito y patrones crudos), conservar el argumento sphere aparte para demostrar que es ignorado, comprobar AL, memoria de entidades/modelos, traces, x87 y restauración. Esto amplía explícitamente el estado inicial comparable, no modifica el EXE ni relaja aceptación. Debe incluir rutas sin collision, ignore, colData nulo, allocation matrix, esferas/cajas/triángulos/grupos y positivos/negativos/límites.

No se afirma que el puente ya esté implementado o validado: es una vía concreta pendiente de integración por el coordinador. No hay bloqueo externo demostrado por datos sin inicializar. El bloqueo inmediato es interno: el candidato carece de implementación y falta instrumentación de pila y cobertura. No se compiló ni ejecutó GTA; no se alteraron fuentes.
