$exe='D:\Original\wv-sp1\reversing\FnFase8\salida\gta_sa_standalone.exe'
$env:GTASA_SILENTPATCH='0'; $env:GTASA_SILENTPATCH_DIAG='1'
& pwsh -NoProfile -File D:\Original\wv-sp1\reversing\FnSkin\ejecutar_skin.ps1 -Modo std -Etiqueta sp1_bikefix -Limite 330 -Vigilancia -Exe $exe -SondaArgs "--escenarios moto463 --minutos 4"
'fin' | Set-Content D:\Original\wv-sp1\reversing\FnSkin\_diag_fin.txt
