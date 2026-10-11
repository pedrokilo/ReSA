<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RevisiÃƒÂ³n preliminar de World15 (no validado)

Reserva GAME: CGarage::IsEntityTouching3D, 0x448EE0. Propuesta sin aplicar de128 instrucciones 0x448EE0..0x449041, ambos RET4 y orden x87 original, seis rechazos de caja amplia y recorrido de cada esfera. ComprobaciÃƒÂ³n git apply --check --ignore-space-change vÃƒÂ¡lida. ProducciÃƒÂ³n y juego intactos.

La caja amplia usa el radio del colmodel estÃƒÂ¡ndar obtenido desde modelinfo+0x14/colmodel+0x24; el bucle llama CEntity::GetColModel0x535300, que puede devolver colmodel especial del vehÃƒÂ­culo. Los fixtures deben preservar esa distinciÃƒÂ³n, usar spheres reales no vacÃƒÂ­as y modelos legales, y demostrar resultados true temprano/tardÃƒÂ­o y false tras recorrer todas las esferas. Los colaboradores serÃƒÂ¡n GetColModel y GetMatrix y geometrÃƒÂ­a radius OFF original/ON cuerpoDLL probado; transformador0x59C890 permanecerÃƒÂ¡ nativo con124bytes completos verificados y sin crÃƒÂ©dito transitivo.

La representaciÃƒÂ³n placement que supera los seis filtros entra en GetMatrix0x411990 y materializa una matriz por AllocateMatrix0x54F560 y UpdateMatrix0x54F1B0. No es legÃƒÂ­timo probar sÃƒÂ³lo rechazos o spheres0 y declararla completa. Se prepara snapshot completo del gestor y todas sus matrices reales, inventariadas desde tres listas y verificadas contiguas desde m_pObjects; se exige libres disponibles para evitar expulsar otro propietario. RemoveMatrix0x54F3B0 original establece placement legÃƒÂ­timo, sin inventar actores ni matrices. El estado observado debe incluir el puntero realmente asignado y nodos modificados antes de restaurar.

## Alerta de layout retirada antes de modificar producciÃ³n

La interpretaciÃ³n preliminar de B74428 como freeTail.prev era errÃ³nea. CMatrixLink tiene tamaÃ±o0x54: freeHead empieza en0x150 y next estÃ¡ en+0x50, por tanto0x1A0; freeTail empieza en0x1A4 y prev estÃ¡ en+0x4C, por tanto0x1F0. B74428 = B74288+0x1A0 = freeHead.next. B74478 serÃ­a freeTail.prev. Los cuerpos originales AddToList1 (0x54E9D0), AddToList2 (0x54EA10) y AllocateMatrix (0x54F56A) toman el mismo extremo que la fuente. La supuesta diferencia no existe bajo este layout; se notificÃ³ la correcciÃ³n inmediatamente a la raÃ­z y al agente449050. La propuesta errÃ³nea se retirÃ³ sin aplicarse. El banco independiente incluirÃ¡ static_assert de offsets y efectos reales, sin proponer correcciÃ³n injustificada.

El banco completo de448EE0 sigue en preparaciÃ³n. No hay compilaciÃ³n, prueba ni admisiÃ³n.

## Banco final preparado

Tras revisión se corrigió CALL del helper0x4487D0 a GarageTouchGeometryBody (no etiqueta sin definición), y NativeUpdateMatrix incluyó los dos bytes del operandoRET4:42bytes totales. No hay patch de colaboradores MatrixLinkList: el extremo libre de la fuente y EXE coincide, según layout corregido anterior.

Dominio final: seis rechazos de caja, primer hit, hitposterior, miss completo, count0 temporal real; dos representaciones. GetMatrix sourceON/EXEOFF realiza lazyallocation real. CMatrix no tiene RH en Main: cuatrofunciones tienen rutas directasDLL y nativas compartidas explícitas, sin activar familia. El oráculo Fraction independiente recibe inputs binarios completos y controlword y reproduce operacionesPC/RC/floatstores, incluido que primerdotX usa valor extendido conservado trasFST, mientras segundo dot usaDX ya redondeado. Los perfiles se rechazan si el oráculo contradice su clasificación. Count0 se restaura con el descriptor entero antesPump, con arrays prestados inalterados.

No se ejecutó GTA ni compilación. La fuente propuesta sigue sin aplicar; MatrixAllocation15 es una prueba independiente separada de funciones ya catalogadas, no un motivo de corrección ni crédito adicionalGAME.
