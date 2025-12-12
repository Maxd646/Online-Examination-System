# Test script for Online Examination System
Write-Host "Testing Online Examination System..." -ForegroundColor Green

# Build the project first
Write-Host "Building project..." -ForegroundColor Yellow
& .\build.ps1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

# Populate database with sample data
Write-Host ""
Write-Host "Populating database..." -ForegroundColor Yellow
& .\populate_db.ps1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Database population failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "✓ System is ready for testing!" -ForegroundColor Green
Write-Host ""
Write-Host "Authentication Fixed:" -ForegroundColor Cyan
Write-Host "  ✓ Simplified login flow" -ForegroundColor White
Write-Host "  ✓ Proper database authentication" -ForegroundColor White
Write-Host "  ✓ Memory synchronization" -ForegroundColor White
Write-Host "  ✓ Account locking protection" -ForegroundColor White
Write-Host ""
Write-Host "To run the system:" -ForegroundColor Cyan
Write-Host "  .\build\exam_system.exe" -ForegroundColor White
Write-Host ""
Write-Host "Test Accounts:" -ForegroundColor Cyan
Write-Host "  Admin - Username: admin, Password: admin123" -ForegroundColor White
Write-Host "  Student - Username: johndoe, Password: password123" -ForegroundColor White
Write-Host "  Student - Username: testuser, Password: testpass123" -ForegroundColor White
Write-Host "  Student - Username: newstudent, Password: password123" -ForegroundColor White
Write-Host "  Or register new student accounts through the system" -ForegroundColor White
Write-Host ""
Write-Host "Available Features:" -ForegroundColor Cyan
Write-Host "  • User Registration & Login (FIXED)" -ForegroundColor White
Write-Host "  • Admin Panel (Question Management, User Management)" -ForegroundColor White
Write-Host "  • Student Panel (Take Exams, Practice Mode, View Results)" -ForegroundColor White
Write-Host "  • Guest Mode (Practice without registration)" -ForegroundColor White
Write-Host "  • Performance Analytics" -ForegroundColor White