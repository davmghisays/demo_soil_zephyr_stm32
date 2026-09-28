/* Versión breve para exposición: temporizador -> I2C -> BLE -> STOP2. */

#include <errno.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>

#define SAMPLE_PERIOD K_MINUTES(60) // Definición de tiempo de espera entre mediciones
#define BLE_ADVERTISING_TIME K_SECONDS(2) //Tiempo máximo de broadcast del BLE

// Direcciones del sensor en I2C. No tiene controlador
#define CHIRP_I2C_ADDR 0x20
#define CHIRP_REG_CAPACITANCE 0x00
#define CHIRP_REG_BUSY 0x09
#define CHIRP_BUSY_POLL_TIME K_MSEC(10)
#define CHIRP_BUSY_MAX_POLLS 100

// Obtención de los dispositivos del Devicetree declarados en el overlay
static const struct device *const i2c_bus = DEVICE_DT_GET(DT_NODELABEL(i2c1));
static const struct device *const soil_power = DEVICE_DT_GET(DT_NODELABEL(soil_power));

//hourly timer es un temporizador y K_SEM_DEFINE es el semáforo que se bloquea
static struct k_timer hourly_timer;
static K_SEM_DEFINE(sample_due, 0, 1);

//Cuando el tiempo se vence,  hace esto. Desbloquear el semáforo
static void hourly_timer_expired(struct k_timer *timer)
{
	ARG_UNUSED(timer);
	k_sem_give(&sample_due);
}

//Activa la conversación con el sensor. Aquí devuelve ret, que dice si lo logró o no (la comunicación)
static int chirp_read(uint8_t reg, uint8_t *data, size_t length)
{
	return i2c_write_read(i2c_bus, CHIRP_I2C_ADDR,
			      &reg, sizeof(reg), data, length);
}

// Aquí mira como está el sensor. Si es 0, ya terminó y todo bien, si es menor que 0, está en fallo. repite y reintenta.
static int chirp_wait_until_ready(void)
{
	uint8_t busy;
	int ret;

	for (int poll = 0; poll < CHIRP_BUSY_MAX_POLLS; poll++) {
		ret = chirp_read(CHIRP_REG_BUSY, &busy, sizeof(busy));
		if (ret < 0) {
			return ret;
		}

		if (busy == 0) {
			return 0;
		}

		k_sleep(CHIRP_BUSY_POLL_TIME);
	}

	return -ETIMEDOUT;
}

// Esta es la función de lectura. El get manda energía al sensor, luego envía energía al bus.
//		Si ambos están, entonces lee la capacitancia que le da el sensor
//		La primera medida se desecha. Se toma la segunda.
//		Finalmente apaga bus y sensor con put
static int read_moisture_raw(uint16_t *moisture)
{
	uint8_t response[2];
	int ret;

	/* Enciende el sensor y espera el segundo configurado en Devicetree. */
	ret = pm_device_runtime_get(soil_power);
	if (ret < 0) {
		return ret;
	}

	ret = pm_device_runtime_get(i2c_bus);
	if (ret < 0) {
		(void)pm_device_runtime_put(soil_power);
		return ret;
	}

	/* La primera lectura inicia una medida nueva; su valor anterior se descarta. */
	ret = chirp_read(CHIRP_REG_CAPACITANCE, response, sizeof(response));
	if (ret == 0) {
		ret = chirp_wait_until_ready();
	}

	/* Cuando GET_BUSY vale cero, la segunda lectura contiene la medida actual. */
	if (ret == 0) {
		ret = chirp_read(CHIRP_REG_CAPACITANCE, response, sizeof(response));
	}
	if (ret == 0) {
		*moisture = sys_get_be16(response);
	}

	(void)pm_device_runtime_put(i2c_bus);
	(void)pm_device_runtime_put(soil_power);
	return ret;
}


// Aqui activa el BLE para enviar. Los primeros dos términos son el fabricante. Los segundos dos son los dos bytes de la lectura
//		En orden Alto y Bajo tal como aparecen
static int transmit_ble(uint16_t moisture)
{
	uint8_t manufacturer_data[] = { 0xff, 0xff, moisture >> 8, moisture & 0xff };
	const struct bt_data ad[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
		BT_DATA(BT_DATA_MANUFACTURER_DATA, manufacturer_data, sizeof(manufacturer_data)),
	};
	int ret;

	ret = bt_enable(NULL);
	if (ret < 0) {
		return ret;
	}

	ret = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), NULL, 0);
	if (ret == 0) {
		k_sleep(BLE_ADVERTISING_TIME);
		(void)bt_le_adv_stop();
	}

	(void)bt_disable();
	return ret;
}


//El main hace varias cosas.
//		1. Verifica que el bus y el sensor no estén en error.
//		2. Arranca el temporizador de 1 hora
//		3. Arranca el ciclo que espera el semáforo, que es liberado por el temporizador
//		4. Realiza la lectura del sensor
//		5. Transmite la lectura obtenida por BLE haciendo un advertising
int main(void)
{
	uint16_t moisture = 0;
	int ret;

	if (!device_is_ready(i2c_bus) || !device_is_ready(soil_power)) {
		printk("ERROR: I2C o dominio de alimentación no está listo\n");
		return 0;
	}

	k_timer_init(&hourly_timer, hourly_timer_expired, NULL);
	k_timer_start(&hourly_timer, SAMPLE_PERIOD, SAMPLE_PERIOD);

	while (true) {
		/* Mientras espera, el hilo idle permite a Zephyr entrar en STOP2. */
		k_sem_take(&sample_due, K_FOREVER);

		ret = read_moisture_raw(&moisture);
		if (ret < 0) {
			printk("ERROR: lectura del Chirp (%d)\n", ret);
			continue;
		}

		ret = transmit_ble(moisture);
		if (ret < 0) {
			printk("ERROR: BLE (%d)\n", ret);
		}
	}
}
