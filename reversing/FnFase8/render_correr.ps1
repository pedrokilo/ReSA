param([string]$Etiqueta='x',[int]$Limite=120)
$dest='D:\Original\_ren_run'; $sal="D:\Original\sim_out\rn_$Etiqueta"; New-Item -ItemType Directory -Force $sal|Out-Null
while($true){try{New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop|Out-Null;break}catch{Start-Sleep 5}}
'render-agente'|Set-Content 'D:\Original\gta_lock\owner_render'
try{
 while(Get-Process gta_sa,gta_sa_standalone,gta_reversed_exe -ErrorAction SilentlyContinue){Start-Sleep 10}
 Remove-Item "$dest\logs\*" -Force -ErrorAction SilentlyContinue
 $a="--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite"
 $p=Start-Process "$dest\gta_sa_standalone.exe" -ArgumentList $a -WorkingDirectory $dest -PassThru
 if(-not $p.WaitForExit(($Limite+40)*1000)){$p.Kill()}
}finally{Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue}
