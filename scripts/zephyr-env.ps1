# Activa el entorno Zephyr 4.4.2 usado en el Máster IoT UPM

& "C:\ZephyrUPM\zephyrproject-4.4\.venv\Scripts\Activate.ps1"

$env:ZEPHYR_BASE = "C:\ZephyrUPM\zephyrproject-4.4\zephyr"
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = "C:\Users\davma\zephyr-sdk-1.0.1"

Write-Host ""
Write-Host "Zephyr environment ready:"
Write-Host "  ZEPHYR_BASE=$env:ZEPHYR_BASE"
Write-Host "  ZEPHYR_TOOLCHAIN_VARIANT=$env:ZEPHYR_TOOLCHAIN_VARIANT"
Write-Host "  ZEPHYR_SDK_INSTALL_DIR=$env:ZEPHYR_SDK_INSTALL_DIR"
Write-Host ""
west --version
python --version
