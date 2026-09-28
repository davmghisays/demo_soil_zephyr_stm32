# UPM Zephyr Project Template

Ejemplo mínimo de un sensor de humedad de suelo de muy bajo consumo para
**Embedded Platforms and Communications for IoT**.

## Qué hace

1. Programa el temporizador del kernel a 60 minutos. En la NUCLEO-WB55RG el
   contador de bajo consumo (LPTIM) puede despertar al MCU.
2. Mientras `main` espera el semáforo, Zephyr entra en STOP2, que conserva SRAM
   y se describe como `suspend-to-idle`.
3. Al vencer el temporizador, `pm_device_runtime_get()` activa mediante PC7 el
   dominio `power-domain-gpio` del sensor y despierta el bus I2C.
4. El Chirp inicia una medida leyendo `GET_CAPACITANCE`, se consulta `GET_BUSY`
   hasta que termina y una segunda lectura obtiene la capacitancia actual.
5. Los dos `pm_device_runtime_put()` vuelven a dejarlos inactivos.
6. El BLE integrado se inicializa, anuncia la medida dos segundos y se detiene
   con `bt_disable()` antes de volver a STOP2.

## Hardware mínimo adicional

- Una NUCLEO-WB55RG, con el firmware HCI Only en su coprocesador Cortex-M0+.
- Un Chirp I²C Capacitive Soil Moisture Sensor en `I2C1`: SCL PB8, SDA PB9.
- PC7 como habilitación de alimentación. Puede alimentar el sensor directamente
  si sus límites eléctricos lo permiten o controlar un MOSFET externo.

El Chirp usa la dirección I2C predeterminada `0x20`. La humedad se obtiene del
registro `GET_CAPACITANCE` (`0x00`) y los valores de 16 bits llegan primero con
el byte más significativo. El dominio espera un segundo después de conectar
VCC y la lectura usa `GET_BUSY` (`0x09`) para esperar la medida sin asumir una
duración fija. No se envía `SLEEP` (`0x08`) porque al terminar se corta VCC.

El coprocesador inalámbrico del STM32WB55 necesita una imagen **HCI Only** de
STM32CubeWB. Sin ella, la aplicación compila pero `bt_enable()` fallará al
ejecutarse. Zephyr expone el inicio/parada de esta radio integrada mediante
`bt_enable()` y `bt_disable()`; no hace falta un GPIO o dispositivo PM BLE
adicional en la aplicación.

## Entorno esperado

- Zephyr 4.4.2
- Zephyr SDK 1.0.1
- Python 3.12
- Board `nucleo_wb55rg`
- Visual Studio Code
- STM32CubeProgrammer
- PuTTY/plink (opcional para monitor serie)

## Archivos clave

```text
src/main.c                         lógica de aplicación
src/main_comentado.c              copia pedagógica que no se compila
boards/nucleo_wb55rg.overlay       descripción/adaptación de hardware
confs/prj_nucleo_wb55rg.conf       configuración Kconfig de Zephyr
CMakeLists.txt                     fuentes de la aplicación
.vscode/settings.json              entorno VS Code
.vscode/tasks.json                 build / flash / serial / debug
```
