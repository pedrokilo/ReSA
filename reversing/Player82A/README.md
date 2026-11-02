<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 02/11/2026 (Ciudad de México):** 820 grupos / 9010 funciones reconstruidas; 9010 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Player82A (lote 82A)

`CPlayerPed::DrawTriangleForMouseRecruitPed` (EXE 0x60BA80, thiscall, ECX = jugador, sin argumentos, `ret`, void; 0x638 bytes). Hook `S`.

Dibuja el triangulo de reclutamiento sobre el peaton apuntado con el raton (`m_p3rdPersonMouseTarget`, +0x79C): sin objetivo no hace nada. Con objetivo fija siete estados de render,
calcula el color (verde a rojo segun vida/vida maxima; alfa 255 solo en el primer vertice), el tamano (distancia al jugador menos 10, por 0,02, limitado a 1; V = v * 0,825 + 0,175), tres vertices a partir de la posicion del
objetivo y el eje derecho de la camara (matriz, o cos/sin del rumbo si no hay matriz; los pandilleros, tipo 7, usan otra disposicion), acerca cada vertice a la camara una unidad (vector normalizado
con `CVector::Normalise` 0x59C910), escribe vertices e indices en el bufer temporal (0xC4D958 / 0xC4B958), llama a `RwIm3DTransform` y, si devuelve algo distinto de cero (EAX completo), a
`RwIm3DRenderIndexedPrimitive` y `RwIm3DEnd`, y restablece los siete estados.

## Reconstruccion
Fuente: `PlayerPed.cpp` (secuencia de llamadas y escritura en el bufer) y `X87Math.cpp` (`Player82ATriangleCpp`: aritmetica con la x87 a 24 bits). Los valores que el EXE deja en la pila x87 sin guardar
(cociente de vidas, longitud al cuadrado, raiz y su inversa) se calculan con exponente ampliado (`Player82ARound24`: mantisa de 24 bits con exponente de double); una vida desnormal o coordenadas enormes
dan resultados distintos si se estrecha a float en medio. La normalizacion reproduce la del EXE (longitud <= 0: solo x = 1; con NaN sigue por la raiz).

## Banco
- 16.384 pares (16 familias de 1.024 casos): pandilleros y no pandilleros, matrices/posiciones de reserva del jugador, objetivo y camara, camara sin matriz (rumbos hasta 1e10), barrido de distancia en 10 y 60, cocientes de vida
  (cero, negativos, mayores que 1, NaN, vida maxima cero), vertices coincidentes con la camara (normalizacion de longitud cero), camaras muy lejanas, no finitos, desnormales, sin objetivo, retorno de `RwIm3DTransform` (cero, uno, basura con AL = 0) y tipos de peaton alrededor de 7.
- Falsos identicos en ambos lados: `RwIm3DTransform` 0x7EF450, `RwIm3DRenderIndexedPrimitive` 0x7EF550, `RwIm3DEnd` 0x7EF520 (funciones de la DLL sin hook que llaman al EXE: parche de entrada del EXE) y
  `fpRenderStateSet` (RwEngineInstance + 0x20). Se registran sus argumentos en orden, los tres vertices vistos por `RwIm3DTransform` y los cambios del bufer temporal (solo posicion, color e indices).
- Oraculo independiente `fixture_oracle.py` (modelo desde el desensamblado, con el mismo redondeo): cada lado de cada par debe coincidir con el.

## Fuera de los fixtures
- NaN: la x87 y SSE propagan cargas utiles distintas; todo float observado se canonicaliza (cualquier NaN = 0x7FC00000).
- Rumbos de camara con |valor| mayor que ~1e10 sin matriz: `fsin`/`fcos` de la x87 reducen con un pi de 66 bits y difieren de `sin`/`cos` de la CRT (1e18 difiere en el ultimo bit). Rumbo infinito o NaN tampoco se prueba.
- Punteros nulos de la matriz del jugador/objetivo no existen (el EXE usa la posicion de reserva); jugador nulo imposible (ECX).
- `CVector::Normalise` (0x59C910) corre de verdad en el lado del EXE; la DLL usa su propia copia exacta. EAX/ECX/EDX de salida no se comparan (nadie los lee).

Repetir: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Player82A`. Evidencia: [validation-summary.json](validation-summary.json).
