# Online Examination System Test Script
# PowerShell script to test the system

Write-Host "Testing Online Examination System..." -ForegroundColor Green
Write-Host ""

# Check if executable exists
if (Test-Path "build\exam_system.exe") {
    Write-Host "Executable found" -ForegroundColor Green
} else {
    Write-Host "Executable not found" -ForegroundColor Red
    Write-Host "Run .\build.ps1 to build the project first." -ForegroundColor Yellow
    exit 1
}

# Check if database exists
if (Test-Path "database\exam.db") {
    Write-Host "Database found" -ForegroundColor Green
} else {
    Write-Host "Database not found" -ForegroundColor Red
    Write-Host "Database will be created automatically when you run the system." -ForegroundColor Yellow
}

# Check database content if sqlite3 is available
try {
    $null = Get-Command sqlite3 -ErrorAction Stop
    Write-Host ""
    Write-Host "Database Statistics:" -ForegroundColor Cyan
    
    $questionCount = & sqlite3 "database\exam.db" "SELECT COUNT(*) FROM questions;" 2>$null
    if ($questionCount) {
        Write-Host "  Questions: $questionCount" -ForegroundColor White
    } else {
        Write-Host "  Questions: 0 (will be created automatically)" -ForegroundColor Yellow
    }
    
    $userCount = & sqlite3 "database\exam.db" "SELECT COUNT(*) FROM users;" 2>$null
    if ($userCount) {
        Write-Host "  Users: $userCount" -ForegroundColor White
    } else {
        Write-Host "  Users: 0 (admin will be created automatically)" -ForegroundColor Yellow
    }
} catch {
    Write-Host "SQLite3 not found - cannot check database content" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "System is ready to use!" -ForegroundColor Green
Write-Host ""
Write-Host "To run the system:" -ForegroundColor Cyan
Write-Host "  .\run.ps1" -ForegroundColor White
Write-Host "  or" -ForegroundColor Gray
Write-Host "  .\build\exam_system.exe" -ForegroundColor White
Write-Host ""
Write-Host "Default admin login:" -ForegroundColor Cyan
Write-Host "  Username: admin" -ForegroundColor White
Write-Host "  Password: admin123" -ForegroundColor White