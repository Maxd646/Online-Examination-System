
# PowerShell script

Write-Host "Online Examination System" -ForegroundColor Green
Write-Host "=========================" -ForegroundColor Green

# Check if executable exists
if (!(Test-Path "build\exam_system.exe")) {
    Write-Host "Executable not found!" -ForegroundColor Red
    Write-Host "Please run .\build.ps1 first to build the project." -ForegroundColor Yellow
    exit 1
}

# Check if database exists
if (!(Test-Path "database\exam.db")) {
    Write-Host "Database not found. Creating..." -ForegroundColor Yellow
    if (!(Test-Path "database")) {
        New-Item -ItemType Directory -Path "database" -Force | Out-Null
    }
}

Write-Host "Starting examination system..." -ForegroundColor Green
Write-Host ""
Write-Host "Default admin login:" -ForegroundColor Cyan
Write-Host "  Username: admin" -ForegroundColor White
Write-Host "  Password: admin123" -ForegroundColor White
Write-Host ""
Write-Host "Press Ctrl+C to exit the system" -ForegroundColor Yellow
Write-Host "================================" -ForegroundColor Green
Write-Host ""

# Run the executable
& ".\build\exam_system.exe"