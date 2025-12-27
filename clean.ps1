# PowerShell script to clean build files

Write-Host "Cleaning build files..." -ForegroundColor Green

# Remove build directory
if (Test-Path "build") {
    Write-Host "Removing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "build"
    Write-Host "Build directory removed" -ForegroundColor Green
} else {
    Write-Host "Build directory not found" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Clean completed!" -ForegroundColor Green
Write-Host "Run .\build.ps1 to rebuild the project." -ForegroundColor Cyan