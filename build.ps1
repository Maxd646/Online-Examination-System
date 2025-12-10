# Online Examination System Build Script
# PowerShell build script for Windows

Write-Host "Building Online Examination System..." -ForegroundColor Green

# Create build directories
$buildDirs = @("build", "build\structure", "build\authentication", "build\database", "database")
foreach ($dir in $buildDirs) {
    if (!(Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "Created directory: $dir" -ForegroundColor Yellow
    }
}

# Check for g++ compiler
try {
    $null = Get-Command g++ -ErrorAction Stop
    Write-Host "g++ compiler found" -ForegroundColor Green
} catch {
    Write-Host "Error: g++ compiler not found!" -ForegroundColor Red
    Write-Host "Please install MinGW-w64 or add g++ to your PATH" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Compiling source files..." -ForegroundColor Cyan

# Compilation settings
$cppFlags = "-std=c++17", "-Wall", "-Wextra", "-O2", "-Isrc"

# Compile utils
Write-Host "Compiling utils..." -ForegroundColor Yellow
& g++ @cppFlags -c src/structure/utils.cpp -o build/structure/utils.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling utils.cpp" -ForegroundColor Red
    exit 1
}

# Compile user
Write-Host "Compiling user..." -ForegroundColor Yellow
& g++ @cppFlags -c src/authentication/user.cpp -o build/authentication/user.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling user.cpp" -ForegroundColor Red
    exit 1
}

# Compile simple_auth
Write-Host "Compiling simple_auth..." -ForegroundColor Yellow
& g++ @cppFlags -c src/authentication/simple_auth.cpp -o build/authentication/simple_auth.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling simple_auth.cpp" -ForegroundColor Red
    exit 1
}

# Compile database
Write-Host "Compiling database..." -ForegroundColor Yellow
& g++ @cppFlags -c src/database/database.cpp -o build/database/database.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling database.cpp" -ForegroundColor Red
    exit 1
}

# Compile main
Write-Host "Compiling main..." -ForegroundColor Yellow
& g++ @cppFlags -c src/main.cpp -o build/main.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling main.cpp" -ForegroundColor Red
    exit 1
}

# Link everything
Write-Host "Linking..." -ForegroundColor Yellow
& g++ build/main.o build/structure/utils.o build/authentication/user.o build/authentication/simple_auth.o build/database/database.o -o build/exam_system.exe -lsqlite3
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error linking executable" -ForegroundColor Red
    Write-Host "Make sure SQLite3 development libraries are installed" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "To run the system:" -ForegroundColor Cyan
Write-Host "  .\build\exam_system.exe" -ForegroundColor White
Write-Host ""
Write-Host "Default admin login:" -ForegroundColor Cyan
Write-Host "  Username: admin" -ForegroundColor White
Write-Host "  Password: admin123" -ForegroundColor White