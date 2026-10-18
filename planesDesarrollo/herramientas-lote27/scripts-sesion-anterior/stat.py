import csv,collections,sys
f=list(csv.DictReader(open('docs/implementaciones/funciones.csv',encoding='utf-8')))
linked={}
for x in f:
    if x['reversed']=='1' and x['catalog']=='hooks.csv':
        k=(x['catalog_identity'],x['catalog_address']);linked[k]=linked.get(k,True) and x['language']=='C++'
try:
    for x in csv.DictReader(open('docs/implementaciones/catalogo-cuerpo-compartido.csv',encoding='utf-8')):
        if x['catalog']=='hooks.csv':
            k=(x['catalog_identity'],x['catalog_address']);linked[k]=linked.get(k,True) and x['language']=='C++'
except FileNotFoundError: pass
print('cpp',sum(linked.values()),len(linked))
r=[x for x in csv.DictReader(open('docs/implementaciones/catalogo-sin-coincidencia.csv',encoding='utf-8')) if x['catalog']=='hooks.csv']
print(len(r))
if len(sys.argv)>1:
    for x in r: print(x['class'],x['fn_name'],x['address'])
