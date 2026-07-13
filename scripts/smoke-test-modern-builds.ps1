[CmdletBinding()]
param(
    [string]$PackageDirectory,
    [ValidateRange(1, 60)]
    [int]$RunSeconds = 8
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if (-not $PackageDirectory) {
    $PackageDirectory = Join-Path $repoRoot 'out\modern-builds'
}
elseif (-not [IO.Path]::IsPathRooted($PackageDirectory)) {
    $PackageDirectory = Join-Path $repoRoot $PackageDirectory
}
$PackageDirectory = [IO.Path]::GetFullPath($PackageDirectory)

$packages = @(
    [pscustomobject]@{
        Name = 'ComicChat-1.0-pre-unofficial-modern'
        Executable = 'chat.exe'
        ArtPatterns = @('ComicArt\Avatars\*.avb', 'ComicArt\Backdrop\*')
    },
    [pscustomobject]@{
        Name = 'ComicChat-2.5-beta-1-unofficial-modern'
        Executable = 'CChat.exe'
        ArtPatterns = @('ComicArt\*.avb', 'ComicArt\*.bgb')
    }
)

$testRoot = Join-Path ([IO.Path]::GetTempPath()) "Comic Chat smoke $([guid]::NewGuid())"
$workingDirectory = Join-Path ([IO.Path]::GetTempPath()) "Unrelated working directory $([guid]::NewGuid())"
New-Item -ItemType Directory -Path $testRoot, $workingDirectory -Force | Out-Null

try {
    foreach ($package in $packages) {
        $zipPath = Join-Path $PackageDirectory "$($package.Name).zip"
        if (-not (Test-Path -LiteralPath $zipPath)) {
            throw "Package is missing: $zipPath"
        }

        $extractDirectory = Join-Path $testRoot $package.Name
        Expand-Archive -LiteralPath $zipPath -DestinationPath $extractDirectory
        $packageRoot = Join-Path $extractDirectory $package.Name
        $executable = Join-Path $packageRoot $package.Executable
        if (-not (Test-Path -LiteralPath $executable)) {
            throw "Packaged executable is missing: $executable"
        }

        foreach ($pattern in $package.ArtPatterns) {
            if (-not (Get-ChildItem -Path (Join-Path $packageRoot $pattern) -File -ErrorAction SilentlyContinue)) {
                throw "Package contains no files matching $pattern"
            }
        }

        $bytes = [IO.File]::ReadAllBytes($executable)
        $peOffset = [BitConverter]::ToInt32($bytes, 0x3c)
        $machine = [BitConverter]::ToUInt16($bytes, $peOffset + 4)
        if ($machine -ne 0x014c) {
            throw ('Expected an x86 PE executable, but {0} has machine type 0x{1:x4}.' -f $executable, $machine)
        }

        $process = $null
        try {
            $process = Start-Process -FilePath $executable -WorkingDirectory $workingDirectory -PassThru
            $deadline = [DateTime]::UtcNow.AddSeconds($RunSeconds)
            $mainWindow = [IntPtr]::Zero
            while ([DateTime]::UtcNow -lt $deadline) {
                Start-Sleep -Milliseconds 500
                $process.Refresh()
                if ($process.HasExited) {
                    throw "$($package.Executable) exited during the random-folder smoke test with code $($process.ExitCode)."
                }
                if ($process.MainWindowHandle -ne [IntPtr]::Zero) {
                    $mainWindow = $process.MainWindowHandle
                }
            }
            if ($mainWindow -eq [IntPtr]::Zero) {
                throw "$($package.Executable) did not create a top-level window during the random-folder smoke test."
            }
            Write-Host "$($package.Name) displayed a window and remained running for $RunSeconds seconds from $packageRoot"
        }
        finally {
            if ($null -ne $process) {
                $process.Refresh()
                if (-not $process.HasExited) {
                    Stop-Process -Id $process.Id
                    $process.WaitForExit(5000) | Out-Null
                }
                $process.Dispose()
            }
        }
    }
}
finally {
    Remove-Item -LiteralPath $testRoot, $workingDirectory -Recurse -Force -ErrorAction SilentlyContinue
}
