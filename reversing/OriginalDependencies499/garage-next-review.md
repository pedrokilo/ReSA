<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Próximo lote: consultas de garajes

Revisión estática de las cuatro entradas, 10/10/2026. No se modificó fuente ni banco, ni se compiló/ejecutó GTA. Disasm mediante reversing/_handoff/dis.sh sobre el EXE canónico de investigación; bytes de constantes y switch leídos directamente del PE.

| Identidad | Entrada | Firma fuente / ABI | Estado |
| --- | --- | --- | --- |
| CGarage::IsPointInsideGarage(CVector) | 0x448740 | thiscall; vector por valor 12 bytes; ret0Ch; bool AL | Garage.cpp138 wrapper original, instalación comentada37 |
| CGarage::IsPointInsideGarage(CVector,float) | 0x4487D0 | thiscall; vector12+radius4; ret10h; bool AL | Sobrecarga distinta, wrapper original |
| CGarages::FindSafeHouseIndexForGarageType | 0x4489F0 | cdecl enum eGarageType subyacente uint8, argumento stack4; EAXint32 | Garages.cpp225 wrapper, RH comentado37 |
| CGarages::FindGarageForObject | 0x44A240 | cdecl CObject*; EAX índice, fuente declara int16 | Garages.cpp253 wrapper, RH comentado38 |

## Layout y contrato geométrico

CGarage ocupa 0xD8. pos{x,y,z} en0/4/8; DirectionA en0C/10; DirectionB14/18; TopZ1C; Width20; Height24. Global aGarages en96C048, NumGarages96C024, máximo50 según cabecera.

448740 compara point.z con pos.z yTopZ; después dx=point.x-pos.x,dy=point.y-pos.y yproyecciones dx*A.x+dy*A.y, dx*B.x+dy*B.y contra0 yWidth/Height. Constante858B50 es0.0 (bytes00000000). Límites inclusivos. Los test ah,5/jnp rechazan sólo ordenado-menor y test ah,41/je sólo ordenado-mayor: unordered pasa estas pruebas. No escribir negación de >= o <= que rechace NaN. dx se almacena con fst ydy con fstp antes de operaciones posteriores; x87 PC24 yordenexpresiones deben mantenerse.

4487D0 amplía z inferior por-radius, superior por+radius, yproyecciones a[-radius,Width+radius]/[-radius,Height+radius]. Almacena dy,dx y-radius; ret10h. Radiusnegativo no se corrige automáticamente: el EXE conserva la aritmética. Fixtures deberán incluir interior/exterior/fronteraULP, rotaciones coherentes,0radius,radio7usadoporelcaller, ycasosNaNquietmoderados cuando no provocan excepciones x87; verificar controlFP ystackx87restaurados.

**Riesgo ABI real adicional:** FindGarageForObject guarda la matriz enEDX yelpuntero de garage enECX antes del call4487D0 (44A26A/44A26D). Después del call44A296 reutiliza ambos sinrecargarlos:44A2A3 testEDX y44A2A5 fld[ECX]. El original4487D0 NOescribeECXniEDX. Un hook C++que los clobberea viola ese caller aunque bool/ESP coincidan. Se requiere preservar explícitamenteECX yEDX, verificar entradasRHreal ycaller compuesto; stackArguments wrapperqueconserveECXsolo no basta paraEDX. 448740 también dejaECXintacto pero produceAXstatus; analizarcallers antes de decidir preservación adicional. No confundir registros caller-saved generales con este contrato EXE concreto.

## Tabla de safehouses

4489F4 resta16,4489F7 compara sin signo29 yswitch tabla448A78. Lectura directa de30DWORD:

16->0;17->1;18->2;24->3;25->4;26->5;27->6;28->7;29->8;30->9;31->10;32->11;33->12;34->13;35->14;39->15;40->16;41->17;42->18;45->19. Todoslosdemás->0. Ganton16 es0, noerror-1. Banco256valores del enumbyte×variantesguard/EAXcaller, salidaEAXcompleta. No dereferencia globals ni llama colaboradores.

## Selección del garage

44A240 devuelve -1 siNumGarages0. Iteración compara sin signo (44A398/44A3A0 jb), por loqueNumGaragesnegativoestáfuera del dominio de memoria. Requiere objeto válido sólo cuando contadorpositivo. Usa matrix=obj+14 si nonNULL, posiciónmatrix+30; si no usaobj+4. Para cada garage invoca IsPointInsideGarage(pos,7.0), sin consultar tipo ni inactiveflags. Entre coincidentes calcula elcentro pos+DirectionA*(Width*.5)+DirectionB*(Height*.5), incluidas componentesz producidaspor multiplicar por0. La constante858B8C=.5 bytes0000003F. fsqrt ydistancia x87; orden suma visible44A363..371: z²+y²+x². Candidata debe reproducir almacenamientos temporales ysqrt sin ampliar/reasociar.

Distanciamejorinicialbits47C34FF3 (~99999.9), índice-1. 44A381 testah5/jp descarta distancias greater/equal/unordered; sólo ordered-less actualiza. Empates mantienen primer índice. Objetos fuera de todoslosgarages mantienen -1. No assume seleccionar nearestgarage sincontainment previa.

## Banco seguro propuesto

1. SafeHouse:256tipos exhaustivos,guardsyEAX/ESP; sin mundo niresources.
2. Geometría: CGaragearena conpoison/guards, vector/radiusporvalor. OriginalentryRHoff/candidateRHon; gate nakedque compruebeECX,EDX,ESP,callee-saved,x87. Banco sólogeométricosno llamaaudio/tasks.
3. FindGarage: snapshotNumGarages ybytes completos50garages; fixtures0/1/2/50,solapamiento,empates,posiciónlejana,ningunocontiene,tipos/flagsirrelevantes, matriznullyrealfixture. CObjectarena cruda yCMatrixfixture sinconstructor/destructor: getter sólo lee punteros/pos; guards antesdespués. NumGarages0 aceptaobjNULLporqueEXEnolo lee; conpositive nuncaNULL. Sin spawningvehículos, streaming,actualizacióngarageoaudio.
4. Tras aprobar geométricos, ejecutarFindGarage con helperRHreal ypreservaciónECX/EDX. Compararretint16 contra int16, guardandotambiénvalorEAXoriginalen diagnóstico; noatribuirEAXalto a contrato int16 sincorregirfirma/fuentes previamente. Snapshotentryflags/globals yrestore antesPumpMessages paraevitarexponer mundo fakeentreframes.

No es aún código ejecutable ni validación. El principal impedimento a incorporar los dos geométricos inmediatamente es el contrato de registros que usa el caller original, no recursos del juego.
