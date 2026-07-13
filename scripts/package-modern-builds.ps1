[CmdletBinding()]
param(
    [string]$OutputDirectory,
    [string]$SourceRevision,
    [string]$SourceRepositoryUrl
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if (-not $OutputDirectory) {
    $OutputDirectory = Join-Path $repoRoot 'out\modern-builds'
}
elseif (-not [IO.Path]::IsPathRooted($OutputDirectory)) {
    $OutputDirectory = Join-Path $repoRoot $OutputDirectory
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)

if (-not $SourceRevision) {
    $SourceRevision = (& git -C $repoRoot rev-parse HEAD).Trim()
    if ($LASTEXITCODE -ne 0) {
        throw 'Could not determine the source revision.'
    }
}
if (-not $SourceRepositoryUrl) {
    $SourceRepositoryUrl = (& git -C $repoRoot remote get-url origin).Trim()
    if ($LASTEXITCODE -ne 0) {
        throw 'Could not determine the source repository URL.'
    }
    if ($SourceRepositoryUrl -match '^git@github\.com:(.+?)(?:\.git)?$') {
        $SourceRepositoryUrl = "https://github.com/$($Matches[1])"
    }
    elseif ($SourceRepositoryUrl.EndsWith('.git')) {
        $SourceRepositoryUrl = $SourceRepositoryUrl.Substring(0, $SourceRepositoryUrl.Length - 4)
    }
}
$SourceRepositoryUrl = $SourceRepositoryUrl.TrimEnd('/')

$stagingDirectory = Join-Path $OutputDirectory 'staging'
if (Test-Path -LiteralPath $stagingDirectory) {
    Remove-Item -LiteralPath $stagingDirectory -Recurse -Force
}
New-Item -ItemType Directory -Path $stagingDirectory -Force | Out-Null

$packages = @(
    [pscustomobject]@{
        Name = 'ComicChat-1.0-pre-unofficial-modern'
        Version = 'Comic Chat 1.0 prerelease (Beta 2), modernized'
        Executable = 'v1.0-pre-modern\chat.exe'
        Art = 'v1.0-pre-modern\comicart'
        Help = 'v1.0-pre-modern\cchat.hlp'
        Readme = 'docs\MODERNIZATION.md'
        RuntimeFiles = @(
            'v1.0-pre-modern\cchat.cnt'
            'v1.0-pre-modern\log.ept'
            'v1.0-pre-modern\profile.txt'
            'v1.0-pre-modern\readme.gif'
            'v1.0-pre-modern\readme.htm'
            'v1.0-pre-modern\readme.txt'
            'v1.0-pre-modern\titles.txt'
        )
    },
    [pscustomobject]@{
        Name = 'ComicChat-2.5-beta-1-unofficial-modern'
        Version = 'Comic Chat 2.5 beta 1, modernized'
        Executable = 'v2.5-beta-1-modern\Release\CChat.exe'
        Art = 'v2.5-beta-1-modern\comicart'
        Help = 'v2.5-beta-1-modern\cchat.hlp'
        Readme = 'v2.5-beta-1-modern\README.md'
        RuntimeFiles = @(
            'v2.5-beta-1-modern\Release\CChat.exe.manifest'
        )
    }
)

foreach ($package in $packages) {
    $packageDirectory = Join-Path $stagingDirectory $package.Name
    $artDirectory = Join-Path $packageDirectory 'ComicArt'
    New-Item -ItemType Directory -Path $artDirectory -Force | Out-Null

    $executable = Join-Path $repoRoot $package.Executable
    $artSource = Join-Path $repoRoot $package.Art
    $helpFile = Join-Path $repoRoot $package.Help
    $readmeFile = Join-Path $repoRoot $package.Readme
    foreach ($requiredPath in @($executable, $artSource, $helpFile, $readmeFile)) {
        if (-not (Test-Path -LiteralPath $requiredPath)) {
            throw "Required package input is missing: $requiredPath"
        }
    }

    Copy-Item -LiteralPath $executable -Destination $packageDirectory
    Copy-Item -Path (Join-Path $artSource '*') -Destination $artDirectory -Recurse
    Copy-Item -LiteralPath $helpFile -Destination $packageDirectory
    Copy-Item -LiteralPath $readmeFile -Destination (Join-Path $packageDirectory 'README.md')
    Copy-Item -LiteralPath (Join-Path $repoRoot 'LICENSE') -Destination (Join-Path $packageDirectory 'LICENSE.txt')
    foreach ($runtimeFile in $package.RuntimeFiles) {
        $runtimePath = Join-Path $repoRoot $runtimeFile
        if (-not (Test-Path -LiteralPath $runtimePath)) {
            throw "Required runtime file is missing: $runtimePath"
        }
        Copy-Item -LiteralPath $runtimePath -Destination $packageDirectory
    }

    @(
        'UNOFFICIAL ARCHIVAL BUILD'
        ''
        $package.Version
        ''
        'This is an unsigned, unsupported build of the modernized historical source.'
        'It is not an official Microsoft product release.'
        ''
        "Source: $SourceRepositoryUrl/tree/$SourceRevision"
        "Revision: $SourceRevision"
        "Built (UTC): $([DateTime]::UtcNow.ToString('yyyy-MM-ddTHH:mm:ssZ'))"
        ''
        'There is no installer. Extract the entire directory and run the executable'
        'in place so that the bundled ComicArt directory remains beside it.'
    ) | Set-Content -LiteralPath (Join-Path $packageDirectory 'UNOFFICIAL-BUILD.txt') -Encoding ascii

    $zipPath = Join-Path $OutputDirectory "$($package.Name).zip"
    if (Test-Path -LiteralPath $zipPath) {
        Remove-Item -LiteralPath $zipPath -Force
    }
    Compress-Archive -LiteralPath $packageDirectory -DestinationPath $zipPath -CompressionLevel Optimal
}

$hashLines = Get-ChildItem -LiteralPath $OutputDirectory -Filter '*.zip' |
    Sort-Object Name |
    ForEach-Object {
        $hash = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
        "$hash  $($_.Name)"
    }
$hashLines | Set-Content -LiteralPath (Join-Path $OutputDirectory 'SHA256SUMS.txt') -Encoding ascii

Remove-Item -LiteralPath $stagingDirectory -Recurse -Force
Write-Host "Created archival packages in $OutputDirectory"
