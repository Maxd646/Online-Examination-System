# Online Examination System Build Script (FINAL FIX)

Write-Host "Building Online Examination System with Qt GUI..." -ForegroundColor Green

# --- Qt detection ---
$qmake = Get-Command qmake -ErrorAction SilentlyContinue

if (-not $qmake) {
    Write-Error "Qt not found! Please install Qt5 or Qt6 with MinGW."
    exit 1
}

Write-Host "Qt found: $($qmake.Source)" -ForegroundColor Cyan

# --- Prepare build directory ---
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory build | Out-Null
}

Set-Location build

# --- Configure ---
Write-Host "Configuring project..." -ForegroundColor Yellow
cmake -G "MinGW Makefiles" ..

if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed."
    exit 1
}

# --- Build ---
Write-Host "Building project..." -ForegroundColor Yellow
mingw32-make

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed."
    exit 1
}

Write-Host "Build completed successfully!" -ForegroundColor Green
