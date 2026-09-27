#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/*
 * El alias led0 se resuelve desde el Devicetree / overlay.
 * Cambia únicamente el overlay si quieres usar otro LED físico.
 */
static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void)
{
    int ret;

    printk("UPM Zephyr project template\n");
    printk("Board: NUCLEO-WL55JC\n");

    if (!gpio_is_ready_dt(&led)) {
        printk("ERROR: LED GPIO device is not ready\n");
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("ERROR: could not configure LED (%d)\n", ret);
        return 0;
    }

    while (1) {
        gpio_pin_toggle_dt(&led);
        printk("Toggle!\n");
        k_msleep(500);
    }

    return 0;
}
