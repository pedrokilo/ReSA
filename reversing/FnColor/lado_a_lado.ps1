# Une N capturas en una tira horizontal con etiquetas: pwsh -File lado_a_lado.ps1 salida.png "Etiqueta1=ruta1" "Etiqueta2=ruta2" ...
param([string]$Salida, [Parameter(ValueFromRemainingArguments)][string[]]$Items)
Add-Type -AssemblyName System.Drawing
$imgs = @(); foreach ($it in $Items) { $e = $it.IndexOf('='); $imgs += [pscustomobject]@{ L = $it.Substring(0, $e); B = [System.Drawing.Bitmap]::new($it.Substring($e + 1)) } }
$h = ($imgs | % { $_.B.Height } | Measure-Object -Maximum).Maximum; $w = ($imgs | % { $_.B.Width } | Measure-Object -Sum).Sum + 8 * ($imgs.Count - 1)
$bm = [System.Drawing.Bitmap]::new([int]$w, [int]($h + 26)); $g = [System.Drawing.Graphics]::FromImage($bm); $g.Clear([System.Drawing.Color]::Black)
$f = [System.Drawing.Font]::new('Consolas', 12, [System.Drawing.FontStyle]::Bold); $x = 0
foreach ($i in $imgs) { $g.DrawImage($i.B, $x, 26); $g.DrawString($i.L, $f, [System.Drawing.Brushes]::White, $x + 4, 3); $x += $i.B.Width + 8 }
$bm.Save($Salida, [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bm.Dispose()
