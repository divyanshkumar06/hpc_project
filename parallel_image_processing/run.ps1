$ErrorActionPreference = "Stop"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host " Starting Real-Time Parallel Image Processing Automation  " -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

# 1. Compilation
Write-Host "[1/5] Compiling the C++ Application..." -ForegroundColor Yellow
mingw32-make clean
mingw32-make
Write-Host "Compilation Successful.`n" -ForegroundColor Green

# 2. Download Image
$imageUrl = "https://raw.githubusercontent.com/ultralytics/yolov5/master/data/images/bus.jpg"
$imageFile = "test_traffic.jpg"

if (-Not (Test-Path $imageFile)) {
    Write-Host "[2/5] Downloading Test Image for ITS application..." -ForegroundColor Yellow
    Invoke-WebRequest -Uri $imageUrl -OutFile $imageFile
    Write-Host "Download Complete.`n" -ForegroundColor Green
} else {
    Write-Host "[2/5] Test image already exists. Skipping download.`n" -ForegroundColor Yellow
}

# 3. Execution / Benchmarking
Write-Host "[3/5] Running OpenMP HPC Benchmarks..." -ForegroundColor Yellow
if (Test-Path "benchmark_results.csv") { Remove-Item "benchmark_results.csv" }
.\image_filter.exe $imageFile
Write-Host "Benchmarking Complete.`n" -ForegroundColor Green

# 4. Generate Dashboard Data
Write-Host "[4/5] Generating Dashboard Data..." -ForegroundColor Yellow
python .\generate_dashboard_data.py
Write-Host "Dashboard Payload JSON injected.`n" -ForegroundColor Green

# 5. Launch Dashboard
Write-Host "[5/5] Launching Interactive Dashboard..." -ForegroundColor Yellow
Start-Process -FilePath "dashboard\index.html"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host " Automation Complete! Check the output images and browser." -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan
