"""Tipos de parametros de script por comando (fuente unica: gen_gen.py los pasa a Gen.hpp). Letras separadas por comas:
 P/V/O  manejador valido de ped/vehiculo/objeto; Pn/Vn/On valido o invalido (ranura libre o byte de estado distinto); Pm/Vm valido o -1; i entero pequeno; I entero cualquiera; f float; b booleano (0, 1 u otro);
 z indice de pandilla 0..9; w ranura de arma 0..13; m indice de modelo 0..7; t indice de jugador 0..1; c indice de ropa 0..9; k pulso de ritmo -9..10; Tr/Cb remolque / cabina (al menos uno valido)
"""
CMDTYPES = {
    1200: 'i', 1201: 'i', 1202: 'i', 1203: 'i', 1204: 'i', 1205: 'i', 1206: 'i', 1207: 'i',
    1208: 'P,w', 1209: 'f,f,f,f,f', 1210: 'V,f', 1211: 'i', 1213: 'V,b', 1216: 'f,f,f,f,f,f,b', 1217: '',
    1220: 'P,f,f,f', 1221: 'Pn', 1224: 'P', 1230: 'f,f,f,i', 1232: 'V,f', 1233: 'V', 1234: 'V,f,f,f,f,f', 1235: 'f,f,f,i',
    1237: 'f,f,f,f,I,I,I,I,I', 1238: 'b', 1239: 'P,b', 1240: 'P,b', 1241: 'O,b', 1242: 'O', 1243: '', 1245: 'P', 1247: 'V,b', 1248: 'V,I', 1249: 'V', 1250: 'I,I', 1251: 'I,I,I', 1252: 'f,f',
    1253: '', 1254: '', 1255: 'On', 1257: '', 1258: '', 1259: 'P,i', 1261: '', 1262: '', 1263: '', 1264: 'P', 1265: 'V', 1268: 'P,O,f,f,f,I,f,i', 1271: 'I,i', 1272: 'I,f,f,f,f,f,f,f,f,f,f,f,f',
    1273: 'i,b', 1274: 'b', 1276: 't', 1278: 'V,i', 1280: 't,c', 1281: 't,b', 1283: 'i,i,f,f,f', 1286: 'I,I,I', 1288: 'V', 1289: 'f,f,f,f', 1290: 'f,f,f,f,f,f', 1294: 'O,V', 1295: '', 1298: '', 1299: 'i',
    1212: '', 1214: '', 1215: '', 1223: '', 1269: '', 1287: '', 1291: '',
    1900: 'z,I', 1901: 'z', 1903: '', 1904: 'P,b', 1905: 'm,I', 1906: 'P,V,i,f', 1910: '', 1911: '', 1912: '', 1914: 'P,i,i', 1915: 'P,i,i', 1916: 'P,i', 1917: 'V', 1918: '',
    1920: 'V,Pm,Vm,f,f', 1921: 'i', 1922: 'i', 1923: 'I,i', 1924: 't,I,I,i', 1926: 'f,f,f,f,f,f', 1928: 'V,i', 1929: 'V', 1930: 'V,I', 1931: 'V', 1932: 'I', 1935: 'P,b', 1936: 'I', 1938: 'P', 1939: '', 1940: '',
    1942: 'O', 1943: 'O,f', 1944: 'O', 1945: 'O', 1949: '', 1950: '', 1951: '', 1952: 'P,i,i,i', 1953: 'i', 1955: 'P,P,f,f', 1956: 'P', 1957: 'P,P,i', 1958: 'f,f,f', 1959: 'P', 1960: 'b', 1961: 'P',
    1963: 'Tr,Cb', 1964: 'Tr,Cb', 1967: 't', 1968: '', 1969: 'k', 1971: 'i,i', 1972: 't,b', 1979: 'V,I', 1980: 'Pm,i', 1981: 'Vn', 1982: 'Vn', 1983: 'I,I', 1984: 'I', 1985: 'I',
    1987: 'O', 1988: 'O,f,f,f,f', 1989: 'V', 1990: 'V,f,f,f,f', 1991: 'V,f,f,f', 1993: 'P,O', 1995: 'P,b', 1996: 't,b', 1997: 'P,f,f,f,f,f',
    1902: '', 1907: '', 1908: '', 1909: '', 1913: '', 1919: '', 1925: '', 1927: '', 1933: '', 1934: '', 1937: '', 1941: '', 1946: '', 1947: '', 1948: '', 1954: '', 1962: '', 1965: '', 1966: '', 1970: '',
    1973: '', 1974: '', 1975: '', 1976: '', 1977: '', 1978: '', 1986: '', 1992: '', 1994: '', 1998: '', 1999: '',
}
# comandos fuera de los dos rangos que tambien se prueban (el EXE devuelve -1)
OUT_OF_RANGE = {0: (1199, 1300, 1000, 1499, 0, 1899, 2000, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF), 1: (1899, 2000, 1199, 1300, 0, 1800, 1999 + 100, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF)}
# comandos de cada rango que el banco recorre (los 100 de la banda)
def band(fn): return [1200 + k if fn == 0 else 1900 + k for k in range(100)]
for k in range(1200, 1300): CMDTYPES.setdefault(k, '')
for k in range(1900, 2000): CMDTYPES.setdefault(k, '')
# reserva nula de CTask::operator new solo es segura donde el EXE comprueba el puntero antes de usarlo
NULL_ALLOC_OK = {1259, 1906, 1955, 1957, 1959, 1993, 1980, 1283}
