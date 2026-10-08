$ErrorActionPreference = 'Stop'
$workspaceRoot = (Resolve-Path -LiteralPath 'D:\Original\gta-reversed-master').Path
$buildRoot = (Resolve-Path -LiteralPath (Join-Path $workspaceRoot 'build')).Path
$buildPrefix = $buildRoot.TrimEnd('\') + '\'
if (-not $buildRoot.StartsWith($workspaceRoot + '\', [StringComparison]::OrdinalIgnoreCase)) { throw 'Build root outside workspace' }
if ((Get-Item -LiteralPath $buildRoot).Attributes -band [IO.FileAttributes]::ReparsePoint) { throw 'Build root is reparse point' }
$tracked = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
git -C $workspaceRoot ls-files | ForEach-Object { [void]$tracked.Add($_) }
if ($LASTEXITCODE -ne 0) { throw 'git ls-files failed' }
$allowed = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
'.obj','.pch','.ilk','.idb','.tlog' | ForEach-Object { [void]$allowed.Add($_) }
$pendingDirectories = [Collections.Generic.Stack[string]]::new()
$pendingDirectories.Push($buildRoot)
$candidates = [Collections.Generic.List[object]]::new()
while ($pendingDirectories.Count -gt 0) {
    $directory = $pendingDirectories.Pop()
    foreach ($item in Get-ChildItem -LiteralPath $directory -Force) {
        if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) { continue }
        if ($item.PSIsContainer) { $pendingDirectories.Push($item.FullName); continue }
        if (-not $allowed.Contains($item.Extension)) { continue }
        $resolved = (Resolve-Path -LiteralPath $item.FullName).Path
        if (-not $resolved.StartsWith($buildPrefix,[StringComparison]::OrdinalIgnoreCase)) { throw "Candidate escaped build root: $resolved" }
        $relative = $resolved.Substring($workspaceRoot.Length + 1).Replace('\','/')
        if ($tracked.Contains($relative)) { continue }
        $candidates.Add([pscustomobject]@{ path=$resolved; bytes=$item.Length; extension=$item.Extension })
    }
}
$before = @{}
foreach ($name in 'C','D') { $before[$name] = (Get-PSDrive -Name $name).Free }
$deleted = [Collections.Generic.List[object]]::new()
foreach ($candidate in $candidates) {
    $item = Get-Item -LiteralPath $candidate.path
    if ($item.PSIsContainer -or ($item.Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw 'Candidate changed type' }
    $resolved = (Resolve-Path -LiteralPath $candidate.path).Path
    if (-not $resolved.StartsWith($buildPrefix,[StringComparison]::OrdinalIgnoreCase)) { throw 'Candidate escaped root' }
    if (-not $allowed.Contains($item.Extension) -or $item.Length -ne $candidate.bytes) { throw 'Candidate changed extension/length' }
    $relative = $resolved.Substring($workspaceRoot.Length + 1).Replace('\','/')
    if ($tracked.Contains($relative)) { throw 'Tracked candidate' }
    Remove-Item -LiteralPath $resolved -Force
    if (Test-Path -LiteralPath $resolved) { throw 'Removal did not complete' }
    $deleted.Add($candidate)
}
$after = @{}
foreach ($name in 'C','D') { $after[$name] = (Get-PSDrive -Name $name).Free }
$bytes = ($deleted | Measure-Object -Property bytes -Sum).Sum
$report = [ordered]@{ timestamp_utc=[DateTime]::UtcNow.ToString('o'); build_root=$buildRoot; allowed_extensions=@('.obj','.pch','.ilk','.idb','.tlog'); files_deleted=$deleted.Count; bytes_deleted=$bytes; free_before=$before; free_after=$after; directories_deleted=0; tracked_files_deleted=0; reparse_points_followed=0; paths=@($deleted.ToArray()) }
$reportPath = Join-Path $workspaceRoot 'reversing\RequestedValidation20261008\Diagnostics\regenerable-build-cleanup-20261008.json'
$report | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $reportPath -Encoding utf8
[pscustomobject]@{ files_deleted=$deleted.Count; bytes_deleted=$bytes; free_D_after=$after.D; report=$reportPath } | ConvertTo-Json
