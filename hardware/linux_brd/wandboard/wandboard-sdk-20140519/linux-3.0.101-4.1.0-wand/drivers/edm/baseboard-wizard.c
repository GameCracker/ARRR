
#include <asm/mach/arch.h>

#include <linux/clk.h>
#include <linux/edm.h>
#include <linux/i2c.h>
#include <linux/i2c/tsc2004.h>
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

static struct regulator_consumer_supply wizard_sgtl5000_consumer_vdda = {
	.supply = "VDDA",
	.dev_name = "0-000a", /* Modified load time */
};

/* ------------------------------------------------------------------------ */

static struct regulator_consumer_supply wizard_sgtl5000_consumer_vddio = {
	.supply = "VDDIO",
	.dev_name = "0-000a", /* Modified load time */
};

/* ------------------------------------------------------------------------ */

static struct regulator_init_data wizard_sgtl5000_vdda_reg_initdata = {
	.num_consumer_supplies = 1,
	.consumer_supplies = &wizard_sgtl5000_consumer_vdda,
};

/* ------------------------------------------------------------------------ */

static struct regulator_init_data wizard_sgtl5000_vddio_reg_initdata = {
	.num_consumer_supplies = 1,
	.consumer_supplies = &wizard_sgtl5000_consumer_vddio,
};

/* ------------------------------------------------------------------------ */

static struct fixed_voltage_config wizard_sgtl5000_vdda_reg_config = {
	.supply_name		= "VDDA",
	.microvolts		= 2500000,
	.gpio			= -1,
	.init_data		= &wizard_sgtl5000_vdda_reg_initdata,
};

/* ------------------------------------------------------------------------ */

static struct fixed_voltage_config wizard_sgtl5000_vddio_reg_config = {
	.supply_name		= "VDDIO",
	.microvolts		= 3300000,
	.gpio			= -1,
	.init_data		= &wizard_sgtl5000_vddio_reg_initdata,
};

/* ------------------------------------------------------------------------ */

static struct platform_device wizard_sgtl5000_vdda_reg_devices = {
	.name	= "reg-fixed-voltage",
	.id	= 0,
	.dev	= {
		.platform_data = &wizard_sgtl5000_vdda_reg_config,
	},
};

/* ------------------------------------------------------------------------ */

static struct platform_device wizard_sgtl5000_vddio_reg_devices = {
	.name	= "reg-fixed-voltage",
	.id	= 1,
	.dev	= {
		.platform_data = &wizard_sgtl5000_vddio_reg_config,
	},
};

/* ------------------------------------------------------------------------ */

static struct platform_device wizard_audio_device = {
	.name = "imx-sgtl5000",
};

/* ------------------------------------------------------------------------ */

static const struct i2c_board_info wizard_sgtl5000_i2c_data = {
        I2C_BOARD_INFO("sgtl5000", 0x0a)
};

/* ------------------------------------------------------------------------ */

static char wizard_sgtl5000_dev_name[8] = "0-000a";

static __init int wizard_init_sgtl5000(void) {
	int i2c_bus = 1;

	if (edm_audio_data[0].enabled && edm_i2c[i2c_bus] >= 0) {
		edm_audio_data[0].bus_type = EDM_BUS_I2C;
		edm_audio_data[0].bus_number = edm_i2c[i2c_bus];
		edm_audio_data[0].bus_address = 0x000a;

		wizard_sgtl5000_dev_name[0] = '0' + edm_i2c[i2c_bus];
		wizard_sgtl5000_consumer_vdda.dev_name = wizard_sgtl5000_dev_name;
		wizard_sgtl5000_consumer_vddio.dev_name = wizard_sgtl5000_dev_name;
        
		wizard_audio_device.dev.platform_data = edm_audio_data[0].platform_data;
		platform_device_register(&wizard_audio_device);
        
		i2c_register_board_info(edm_i2c[i2c_bus], &wizard_sgtl5000_i2c_data, 1);
		platform_device_register(&wizard_sgtl5000_vdda_reg_devices);
		platform_device_register(&wizard_sgtl5000_vddio_reg_devices);
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

static int wizard_tsc2046_gpio_index = 8;
static int wizard_ads7845_gpio_index = 8;

int wizard_get_tsc2046_pendown_state(void) {
	return !gpio_get_value(edm_external_gpio[wizard_tsc2046_gpio_index]);
}

static struct ads7846_platform_data wizard_tsc2046_config = {
        .x_max              	= 0x0fff,
        .y_max                  = 0x0fff,
        .pressure_max           = 255,
        .get_pendown_state      = wizard_get_tsc2046_pendown_state,
        .keep_vref_on           = 1,
        .wakeup			= true,
};

static struct spi_board_info wizard_tsc2046_spi_data  = {
        .modalias		= "ads7846",
        .bus_num		= 0,
        .chip_select		= 0,
        .max_speed_hz		= 1500000,
        .irq			= -EINVAL, /* Set programmatically */
        .platform_data		= &wizard_tsc2046_config,
};

static struct spi_board_info wizard_spidev_data  = {
        .modalias		= "spidev",
        .bus_num		= 0,
        .chip_select		= 0,
        .max_speed_hz		= 300000,
};


/* ------------------------------------------------------------------------ */

int wizard_get_ads7845_pendown_state(void) {
	return !gpio_get_value(edm_external_gpio[wizard_ads7845_gpio_index]);
}

static struct ads7846_platform_data wizard_ads7845_config = {
        .x_max              	= 0x0fff,
        .y_max                  = 0x0fff,
        .pressure_max           = 255,
        .get_pendown_state      = wizard_get_ads7845_pendown_state,
        .keep_vref_on           = 1,
        .wakeup			= true,
};

static struct spi_board_info wizard_ads7845_spi_data  = {
        .modalias		= "ads7846",
        .bus_num		= 1,
        .chip_select		= 0,
        .max_speed_hz		= 1500000,
        .irq			= -EINVAL, /* Set programmatically */
        .platform_data		= &wizard_ads7845_config,
};

/*
 * TSC 2004 Support
 */

static int wizard_tsc2004_gpio_index = 9;

static int wizard_tsc2004_init_irq(void)
{
	int ret = 0;

	ret = gpio_request(edm_external_gpio[wizard_tsc2004_gpio_index], "tsc2004-irq");
	if (ret < 0) {
		return ret;
	}

	if (gpio_direction_input(edm_external_gpio[wizard_tsc2004_gpio_index])) {
		return -ENXIO;
	}

	gpio_set_debounce(edm_external_gpio[wizard_tsc2004_gpio_index], 0x13565);
	return ret;
}

static void wizard_tsc2004_exit_irq(void) {
	gpio_free(edm_external_gpio[wizard_tsc2004_gpio_index]);
}

static int wizard_tsc2004_get_irq_level(void) {
	return gpio_get_value(edm_external_gpio[wizard_tsc2004_gpio_index]) ? 0 : 1;
}


struct tsc2004_platform_data wizard_tsc2004data = {
	.model = 2004,
	.x_plate_ohms = 180,

	.get_pendown_state = wizard_tsc2004_get_irq_level,
	.init_platform_hw = wizard_tsc2004_init_irq,
	.exit_platform_hw = wizard_tsc2004_exit_irq,

};

static struct i2c_board_info __initdata wizard_tsc2004_i2c_data[] = {
	{
		I2C_BOARD_INFO("tsc2004", 0x4a),
		.type		= "tsc2004",
		.platform_data	= &wizard_tsc2004data,
                .irq = -EINVAL,
	},
};


/* ------------------------------------------------------------------------ */

void __init wizard_init_ts(void) {
        gpio_request(edm_external_gpio[wizard_tsc2046_gpio_index], "tsc2046 irq");
        gpio_request(edm_external_gpio[wizard_ads7845_gpio_index], "ads7845 irq");
        wizard_tsc2046_spi_data.irq = gpio_to_irq(edm_external_gpio[wizard_tsc2046_gpio_index]);
	spi_register_board_info(&wizard_tsc2046_spi_data, 1);
        wizard_ads7845_spi_data.irq = gpio_to_irq(edm_external_gpio[wizard_ads7845_gpio_index]);
	spi_register_board_info(&wizard_ads7845_spi_data, 2);

	wizard_tsc2004_i2c_data[0].irq = 
		gpio_to_irq(edm_external_gpio[wizard_tsc2004_gpio_index]);
        i2c_register_board_info(edm_i2c[1], &wizard_tsc2004_i2c_data[0], 1);
}


/****************************************************************************
 *                                                                          
 * DS1337 RTC
 *                                                                          
 ****************************************************************************/

static const struct i2c_board_info wizard_ds1337_binfo = {
	I2C_BOARD_INFO("ds1337", 0x68),
};

void __init wizard_init_rtc(void) {
	static const int rtc_i2c = 1;
	i2c_register_board_info(edm_i2c[rtc_i2c], &wizard_ds1337_binfo, 1);
}


/****************************************************************************
 *                                                                          
 * main-function for wizardboard
 *                                                                          
 ****************************************************************************/

static __init int wizard_init(void) {
	wizard_init_sgtl5000();
	wizard_init_ts();
	wizard_init_rtc();
        return 0;
}
arch_initcall_sync(wizard_init);

static __exit void wizard_exit(void) {
	/* Actually, this cannot be unloaded. Or loaded as a module..? */
} 
module_exit(wizard_exit);

MODULE_DESCRIPTION("Wizardboard driver");
MODULE_AUTHOR("Tapani <tapani@technexion.com>");
MODULE_LICENSE("GPL");
