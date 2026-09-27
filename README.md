# UPM Zephyr Project Template

Plantilla reutilizable para proyectos de **Embedded Platforms and Communications for IoT**.

## Entorno esperado

- Zephyr 4.4.2
- Zephyr SDK 1.0.1
- Python 3.12
- Board `nucleo_wl55jc`
- Visual Studio Code
- STM32CubeProgrammer
- PuTTY/plink (opcional para monitor serie)

## Crear un proyecto nuevo

Copia esta carpeta a:

```text
C:\ZephyrUPM\workspace\NOMBRE_PROYECTO
```

Abre **la raíz del proyecto** en VS Code:

```powershell
code C:\ZephyrUPM\workspace\NOMBRE_PROYECTO
```

`${workspaceFolder}` apuntará automáticamente a esa carpeta.

## Build

```text
Ctrl + Shift + B
```

Ejecuta `West Build (auto)`.

Para una compilación completamente limpia:

```text
Tasks: Run Task → West Build (pristine)
```

## Flash

```text
Tasks: Run Task → Flash (CubeProgrammer, UR+HWrst)
```

## Puerto serie

```text
Tasks: Run Task → List COM Ports
```

Después:

```text
Tasks: Run Task → Serial Monitor
```

## Archivos clave

```text
src/main.c                         lógica de aplicación
boards/nucleo_wl55jc.overlay      descripción/adaptación de hardware
confs/prj_nucleo_wl55jc.conf      configuración Kconfig de Zephyr
CMakeLists.txt                     fuentes de la aplicación
.vscode/settings.json              entorno VS Code
.vscode/tasks.json                 build / flash / serial / debug
```

Consulta `docs/Zephyr_UPM_Playbook.md` para el procedimiento completo.
