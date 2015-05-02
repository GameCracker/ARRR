
/* Support file for FWBADAPT-7WVGA from Future Electronics
   This file is GPL.
 */

#include <linux/delay.h>
#include <linux/edm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>

#include <mach/common.h>
#include <mach/devices-common.h>
#include <mach/gpio.h>

#define FWBADAPT_PRISM_IRQ edm_external_gpio[8]
#define FWBADAPT_PRISM_RESET edm_external_gpio[9]

static struct i2c_board_info fwbadapt_prism_i2c_data[] __initdata = {
	{
		I2C_BOARD_INFO("prism", 0x10),
		.flags = I2C_CLIENT_WAKE,
	},
};

static __init int fwbadapt_init(void) {
	gpio_direction_output(FWBADAPT_PRISM_RESET, 0);
	gpio_set_value(FWBADAPT_PRISM_RESET, 0);
	mdelay(50);
	gpio_set_value(FWBADAPT_PRISM_RESET, 1);
	gpio_direction_input(FWBADAPT_PRISM_IRQ);
	fwbadapt_prism_i2c_data[0].irq = gpio_to_irq(FWBADAPT_PRISM_IRQ);

	i2c_register_board_info(edm_i2c[1], &fwbadapt_prism_i2c_data[0], 1);

	gpio_direction_input(edm_external_gpio[3]);
	gpio_direction_input(edm_external_gpio[4]);
	gpio_direction_input(edm_external_gpio[5]);
	gpio_direction_input(edm_external_gpio[7]);
	return 0;
}
arch_initcall_sync(fwbadapt_init);

static __exit void fwbadapt_exit(void) {
	/* Actually, this cannot be unloaded. Or loaded as a module..? */
} 
module_exit(fwbadapt_exit);

MODULE_DESCRIPTION("FWBADAPT expansion board driver");
MODULE_LICENSE("GPL");
