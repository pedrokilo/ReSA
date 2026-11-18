"""Tipos de parametros de script por comando (fuente unica: gen_gen.py los pasa a GenTables.hpp). Letras separadas por comas:
 P/V/O  manejador valido de ped/vehiculo/objeto; Pn/Vn/On valido o invalido (ranura libre o byte de estado distinto); Pm/Vm valido o -1; i entero pequeno; I entero cualquiera; f float; b booleano (0, 1 u otro);
 t indice de jugador 0..1; M modelo de recogida (positivo, o negativo = indice de UsedObjectArray -1..-7); q modelo de vehiculo (o negativo = cualquiera); a angulo en grados (de ANGLES); s velocidad (de SPEEDS);
 g float pequeno (sobre todo 0 y 1; tambien -1 y 2); Pq ped valido el 80% de las veces; n booleano que casi siempre es 0
"""
import bank_spec as S

CMDTYPES = {
    801: 'P', 803: 'V,b', 805: 'V', 806: 'P', 807: 'f,f,f,f,q', 810: 'I', 811: 'M,I,I,f,f,f', 816: 't,b', 817: 't,b', 818: 'P,b', 821: 'b', 823: 'P,b', 824: 'V,b',
    825: 'f,f,f,f,f,f,b,b,b,b,b', 830: 'f,f', 831: 'f,f', 832: 'I,I,I,I', 833: 'b', 834: 'b', 835: 'f', 836: 'f', 837: 'b', 840: 'b', 841: 'i',
    845: 'O,a,s,b', 846: 'O,g,g,g,s,s,s,b', 847: 'Pq', 848: 'P,b', 854: 'i,f,f,f,f,f,f', 860: 'O,V,f,f,f', 861: 'O,n', 863: 'P,i', 864: '', 865: '',
    866: 'P,f,f,f', 867: 'f,f,f,f,M,n', 868: 'P,P', 870: 'O', 874: 'P,V', 877: 'i,i,I,i', 883: '', 886: 'f,f,f', 897: 'O,f,f,f', 898: 'O,b',
}
# sin manejador (devuelven -1 sin leer parametros) o sin parametros (809, 812, 894, 899: devuelven 0)
for k in range(801, 900): CMDTYPES.setdefault(k, '')
# comandos fuera de la banda que tambien se prueban (el EXE devuelve -1)
OUT_OF_RANGE = S.OOR
# reserva nula de CTask::operator new solo es segura donde el EXE comprueba el puntero antes de usarlo
NULL_ALLOC_OK = {886}
