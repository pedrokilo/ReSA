#!/bin/bash
# espera a que gta_lock quede libre y lanza la prueba de marcadores con el ASI de FnD3DX9C (reintenta si otro la toma entre medias)
cd /d/Original/wv-8/reversing/FnBIS1 || exit 1
for i in 1 2 3 4 5 6; do
  while [ -d /d/Original/gta_lock ]; do sleep 5; done
  AGUA_ASI='D:\Original\wv-d3dx\reversing\FnD3DX9C\out\asi\gta_reversed.asi' python prueba_marcadores.py cal_mal.args d3dx_mar > /d/Original/wv-d3dx/reversing/FnD3DX9C/out/humo_marcadores.txt 2>&1
  grep -q "VEREDICTO" /d/Original/wv-d3dx/reversing/FnD3DX9C/out/humo_marcadores.txt && break
  sleep 20
done
echo FIN >> /d/Original/wv-d3dx/reversing/FnD3DX9C/out/humo_marcadores.txt
