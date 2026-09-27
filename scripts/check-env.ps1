Write-Host "=== Zephyr UPM Environment Check ==="

$checks = @(
    @{ Name = "Zephyr 4.4.2 source"; Path = "C:\ZephyrUPM\zephyrproject-4.4\zephyr" },
    @{ Name = "Zephyr venv"; Path = "C:\ZephyrUPM\zephyrproject-4.4\.venv\Scripts\python.exe" },
    @{ Name = "west"; Path = "C:\ZephyrUPM\zephyrproject-4.4\.venv\Scripts\west.exe" },
    @{ Name = "SDK 1.0.1"; Path = "C:\Users\davma\zephyr-sdk-1.0.1" },
    @{ Name = "ARM GCC"; Path = "C:\Users\davma\zephyr-sdk-1.0.1\gnu\arm-zephyr-eabi\arm-zephyr-eabi\bin\arm-zephyr-eabi-gcc.exe" },
    @{ Name = "STM32CubeProgrammer"; Path = "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" },
    @{ Name = "PuTTY/plink"; Path = "C:\Program Files\PuTTY\plink.exe" }
)

foreach ($check in $checks) {
    $ok = Test-Path $check.Path
    $mark = if ($ok) { "[OK]" } else { "[MISSING]" }
    Write-Host "$mark $($check.Name): $($check.Path)"
}

Write-Host ""
Write-Host "Environment variables:"
Get-ChildItem Env:ZEPHYR* | Format-Table -AutoSize
