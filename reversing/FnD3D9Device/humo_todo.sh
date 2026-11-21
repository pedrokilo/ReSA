#!/bin/sh
# Humo completo con la DLL propia: ropero, agua y modo ventana (en serie; cada script espera a que no haya otro gta_sa.exe / candado).
export AGUA_ASI='D:\Original\wv-d3d9\reversing\FnD3D9Device\out\gta_reversed_d3d9dev.asi'
cd /d/Original/wv-8/reversing/FnBIS1
PY=/c/Users/pedro/AppData/Local/Programs/Python/Python314/python.exe
OUT=/d/Original/wv-d3d9/reversing/FnD3D9Device
$PY prueba_ropero.py cal_mal.args d3d9dev_rop > $OUT/humo_ropero.log 2>&1; echo "exit=$?" >> $OUT/humo_ropero.log
$PY prueba_agua.py cal_mal.args d3d9dev_agua > $OUT/humo_agua.log 2>&1; echo "exit=$?" >> $OUT/humo_agua.log
$PY $OUT/humo_ventana.py cal_mal.args d3d9dev_vent > $OUT/humo_ventana.log 2>&1; echo "exit=$?" >> $OUT/humo_ventana.log
