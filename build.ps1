$ErrorActionPreference = "Stop"

$WorkspaceFolder = "d:\All About C++\Github-playground\Lumina"
$BuildDir = "$WorkspaceFolder\build"
$SourceDir = "$WorkspaceFolder"
$MSBuildExe = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"

Write-Host "Building Lumina (RelWithDebInfo)..." -ForegroundColor Cyan

if (!(Test-Path $BuildDir)) {
    Write-Host "Creating build directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
}

Write-Host "Using Visual Studio 2024 MSBuild..." -ForegroundColor Green

# Check if we need to generate the project with CMake
$slnFiles = @(Get-ChildItem -Path $BuildDir -Filter "*.sln" -ErrorAction SilentlyContinue)

if ($slnFiles.Count -eq 0) {
    Write-Host "No solution file found. CMake is required to generate the Visual Studio project." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "📥 Please install CMake from: https://cmake.org/download/" -ForegroundColor Cyan
    Write-Host "   Then add cmake.exe to your PATH environment variable" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "After installation, either:" -ForegroundColor Yellow
    Write-Host "  1. Re-run this script" -ForegroundColor White
    Write-Host "  2. Or run: cmake -B build -S . -G \"Visual Studio 18 2024\" -A x64" -ForegroundColor White
    exit 1
}

# Build with MSBuild
$slnFile = $slnFiles[0].FullName
Write-Host "Building solution: $($slnFiles[0].Name)" -ForegroundColor Cyan

& $MSBuildExe $slnFile /p:Configuration=RelWithDebInfo /p:Platform=x64 /m:4

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "✅ Build completed successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "📦 Output should be at:" -ForegroundColor Cyan
    Write-Host "   $BuildDir\RelWithDebInfo\Lumina.dll" -ForegroundColor White
} else {
    Write-Host ""
    Write-Host "❌ Build failed with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit 1
}
