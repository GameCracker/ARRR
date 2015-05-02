
#include <asm/mach/arch.h>

#include <linux/clk.h>
#include <linux/edm.h>
#include <linux/i2c.h>
#include <linux/i2c/lis331dlh.h>
#include <linux/platform_device.h>
#include <linux/regulator/fixed.h>
#include <linux/regulator/machine.h>

#include <mach/common.h>
#include <mach/devices-common.h>
#include <mach/gpio.h>
#include <mach/iomux-mx6dl.h>
#include <mach/iomux-v3.h>
#include <mach/mx6.h>

/****************************************************************************
 *                                                                          
 * SGTL5000 Audio Codec
 *                                                                          
 ****************************************************************************/

static struct regulator_consumer_supply fairy_sgtl5000_consumer_vdda = {
	.supply = "VDDA",
	.dev_name = "0-000a", /* Modified load time */
};

/* ------------------------------------------------------------------------ */

static struct regulator_consumer_supply fairy_sgtl5000_consumer_vddio = {
	.supply = "VDDIO",
	.dev_name = "0-000a", /* Modified load time */
};

/* ------------------------------------------------------------------------ */

static struct regulator_init_data fairy_sgtl5000_vdda_reg_initdata = {
	.num_consumer_supplies = 1,
	.consumer_supplies = &fairy_sgtl5000_consumer_vdda,
};

/* ------------------------------------------------------------------------ */

static struct regulator_init_data fairy_sgtl5000_vddio_reg_initdata = {
	.num_consumer_supplies = 1,
	.consumer_supplies = &fairy_sgtl5000_consumer_vddio,
};

/* ------------------------------------------------------------------------ */

static struct fixed_voltage_config fairy_sgtl5000_vdda_reg_config = {
	.supply_name		= "VDDA",
	.microvolts		= 2500000,
	.gpio			= -1,
	.init_data		= &fairy_sgtl5000_vdda_reg_initdata,
};

/* ------------------------------------------------------------------------ */

static struct fixed_voltage_config fairy_sgtl5000_vddio_reg_config = {
	.supply_name		= "VDDIO",
	.microvolts		= 3300000,
	.gpio			= -1,
	.init_data		= &fairy_sgtl5000_vddio_reg_initdata,
};

/* ------------------------------------------------------------------------ */

static struct platform_device fairy_sgtl5000_vdda_reg_devices = {
	.name	= "reg-fixed-voltage",
	.id	= 0,
	.dev	= {
		.platform_data = &fairy_sgtl5000_vdda_reg_config,
	},
};

/* ------------------------------------------------------------------------ */

static struct platform_device fairy_sgtl5000_vddio_reg_devices = {
	.name	= "reg-fixed-voltage",
	.id	= 1,
	.dev	= {
		.platform_data = &fairy_sgtl5000_vddio_reg_config,
	},
};

/* ------------------------------------------------------------------------ */

static struct platform_device fairy_audio_device = {
	.name = "imx-sgtl5000",
};

/* ------------------------------------------------------------------------ */

static const struct i2c_board_info fairy_sgtl5000_i2c_data __initdata = {
	I2C_BOARD_INFO("sgtl5000", 0x0a)
};

/* ------------------------------------------------------------------------ */

static char fairy_sgtl5000_dev_name[8] = "0-000a";

static __init int fairy_init_sgtl5000(void) {
	int ret = -ENODEV, i2c_bus = 1;

	if (edm_audio_data[0].enabled && edm_i2c[i2c_bus] >= 0) {
		edm_audio_data[0].bus_type = EDM_BUS_I2C;
		edm_audio_data[0].bus_number = edm_i2c[i2c_bus];
		edm_audio_data[0].bus_address = 0x000a;

		fairy_sgtl5000_dev_name[0] = '0' + edm_i2c[i2c_bus];
		fairy_sgtl5000_consumer_vdda.dev_name = fairy_sgtl5000_dev_name;
		fairy_sgtl5000_consumer_vddio.dev_name = fairy_sgtl5000_dev_name;
        
		fairy_audio_device.dev.platform_data = edm_audio_data[0].platform_data;
		platform_device_register(&fairy_audio_device);
        
		i2c_register_board_info(edm_i2c[i2c_bus], &fairy_sgtl5000_i2c_data, 1);
		platform_device_register(&fairy_sgtl5000_vdda_reg_devices);
		platform_device_register(&fairy_sgtl5000_vddio_reg_devices);
	}
	return 0;
}


/****************************************************************************
 *                                                                          
 * ADS7846 Touchscreen
 *                                                                          
 ****************************************************************************/

#include <linux/spi/ads7846.h>
#include <linux/spi/spi.h>

static int fairy_tsc2046_gpio_index = 8;

int fairy_get_tsc2046_pendown_state(void) {
	return !gpio_get_value(edm_external_gpio[fairy_tsc2046_gpio_index]);
}

static struct ads7846_platform_data fairy_tsc2046_config = {
	.x_max              	= 0x0fff,
	.y_max                  = 0x0fff,
	.pressure_max           = 1024,
	.get_pendown_state      = fairy_get_tsc2046_pendown_state,
	.keep_vref_on           = 1,
	.wakeup			= true,
	.model                  = 7846,

	.x_plate_ohms  = 90,
	.y_plate_ohms  = 90,
	.debounce_max  = 70,
	.debounce_tol  = 3,
	.debounce_rep  = 2,
	.settle_delay_usecs = 150
};

static struct spi_board_info fairy_tsc2046_spi_data  = {
	.modalias		= "ads7846",
	.bus_num		= 0,
	.chip_select		= 0,
	.max_speed_hz		= 1500000,
	.irq			= -EINVAL, /* Set programmatically */
	.platform_data		= &fairy_tsc2046_config,
};

static struct spi_board_info fairy_spidev_data  = {
	.modalias		= "spidev",
	.bus_num		= 0,
	.chip_select		= 0,
	.max_speed_hz		= 300000,
};

/* ------------------------------------------------------------------------ */

void __init fairy_init_ts(void) {
	gpio_request(edm_external_gpio[fairy_tsc2046_gpio_index], "tsc2046 irq");
	fairy_tsc2046_spi_data.irq = gpio_to_irq(edm_external_gpio[fairy_tsc2046_gpio_index]);
	spi_register_board_info(&fairy_tsc2046_spi_data, 1);
}


/****************************************************************************
 *                                                                          
 * ISL20923 light sensor
 *                                                                          
 ****************************************************************************/

static struct i2c_board_info fairy_isl29023_binfo = {
	I2C_BOARD_INFO("isl29018", 0x44),	
	.platform_data = NULL,
	.irq = -EINVAL,
};

void __init fairy_init_lightsensor(void) {
	fairy_isl29023_binfo.irq  = gpio_to_irq(edm_external_gpio[6]),
	i2c_register_board_info(edm_i2c[2], &fairy_isl29023_binfo, 1);
}


/****************************************************************************
 *                                                                          
 * MAG3110 compass
 *                                                                          
 ****************************************************************************/

static const int mag3110_position = 1;

static struct i2c_board_info fairy_mag3110_binfo = {
	I2C_BOARD_INFO("mag3110", 0x0e),
	
	.platform_data = (void *)&mag3110_position,
};

void __init fairy_init_compass(void) {
	fairy_mag3110_binfo.irq = gpio_to_irq(edm_external_gpio[5]),
	i2c_register_board_info(edm_i2c[2], &fairy_mag3110_binfo, 1);
}


/****************************************************************************
 *                                                                          
 * LIS331D accelerometer
 *                                                                          
 ****************************************************************************/

static struct lis331dlh_platform_data fairy_lis331dlh_data = {
	.min_interval = 1,
	.poll_interval = 200,
	.g_range = LIS331DLH_G_8G,
	.axis_map_x = 0,
	.axis_map_y = 1,
	.axis_map_z = 2,
	.negate_x = 0,  
	.negate_y = 0,  
	.negate_z = 0,  
};

static const struct i2c_board_info fairy_lis331dlh_binfo = {
	I2C_BOARD_INFO("lis331dlh", 0x18),
	.platform_data = &fairy_lis331dlh_data,
};

void __init fairy_init_gsensor(void) {
	i2c_register_board_info(edm_i2c[2], &fairy_lis331dlh_binfo, 1);
}


/****************************************************************************
 *                                                                          
 * DS1337 RTC
 *                                                                          
 ****************************************************************************/

static const struct i2c_board_info fairy_ds1337_binfo = {
	I2C_BOARD_INFO("ds1337", 0x68),
};

void __init fairy_init_rtc() {
	i2c_register_board_info(edm_i2c[2], &fairy_ds1337_binfo, 1);
}


/****************************************************************************
 *                                                                          
 * main-function for fairyboard
 *                                                                          
 ****************************************************************************/

static __init int fairy_init(void) {
	fairy_init_sgtl5000();
	fairy_init_ts();
	fairy_init_lightsensor();
	fairy_init_compass();
	fairy_init_gsensor();
	fairy_init_rtc();
        
	return 0;
}
arch_initcall_sync(fairy_init);

static __exit void fairy_exit(void) {
	/* Actually, this cannot be unloaded. Or loaded as a module..? */
} 
module_exit(fairy_exit);

MODULE_DESCRIPTION("Fairyboard");
MODULE_AUTHOR("Tapani Utriainen <tapani@technexion.com>");
MODULE_LICENSE("GPL");

