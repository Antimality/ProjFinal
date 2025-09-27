# TODO: Run this before hand in

# This finds all .c and .h files and counts lines per function (simple heuristic)
Get-ChildItem -Recurse -Include *.c, *.h | 
Select-String -Pattern '^[a-zA-Z_][a-zA-Z0-9_ \*]*\([^\)]*\)[ \n]*\{' | 
ForEach-Object {
  $file = $_.Path
  $start = $_.LineNumber
  $lines = Get-Content $file
  $brace = 0
  $count = 0
  for ($i = $start - 1; $i -lt $lines.Count; $i++) {
    $count++
    $brace += ($lines[$i] -split '{').Count - 1
    $brace -= ($lines[$i] -split '}').Count - 1
    if ($brace -eq 0) { break }
  }
  if ($count -gt 40) {
    Write-Host "${file}:${start} function exceeds 40 lines ($count lines)"
  }
}