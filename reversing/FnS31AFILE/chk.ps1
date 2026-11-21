# Comprobacion de sintaxis rapida (cl /Zs) de uno o varios .cpp de source\; muestra solo errores y avisos del propio archivo.
param([Parameter(ValueFromRemainingArguments=$true)]$files)
foreach ($f in $files) {
  $name = Split-Path $f -Leaf
  cmd /c "D:\Original\wv-31\reversing\FnS31AFILE\chk.bat $f" 2>&1 | Where-Object { $_ -match "error|$([regex]::Escape($name))\(\d+\)" } | Select-Object -First 60
}
