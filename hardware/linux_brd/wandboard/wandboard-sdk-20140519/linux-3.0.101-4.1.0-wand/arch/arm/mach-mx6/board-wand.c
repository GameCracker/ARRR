/*
    Wandboard board file. Copyright (C) 2013 Tapani Utriainen
    Authors: Tapani Utriainen, Edward Lin

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <asm/mach-types.h>
#include <asm/setup.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/edm.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/memblock.h>
#include <linux/phy.h>

#include <mach/ahci_sata.h>
#include <mach/common.h>
#include <mach/devices-common.h>
#include <mach/gpio.h>
#include <mach/iomux-mx6dl.h>
#include <mach/iomux-mx6q.h>
#include <mach/iomux-v3.h>
#include <mach/mx6.h>

#include "crm_regs.h"
#include "devices-imx6q.h"
#include "usb.h"

#define WAND_BT_ON_AB		IMX_GPIO_NR(3, 13)
#define WAND_BT_ON_C1		IMX_GPIO_NR(5, 21)
#define WAND_BT_WAKE_AB		IMX_GPIO_NR(3, 14)
#define WAND_BT_WAKE_C1		IMX_GPIO_NR(5, 30)
#define WAND_BT_HOST_WAKE_AB	IMX_GPIO_NR(3, 15)
#define WAND_BT_HOST_WAKE_C1	IMX_GPIO_NR(5, 20)

#define WAND_PCIE_NRST		IMX_GPIO_NR(3, 31)

#define WAND_REV_DETECT_C1	IMX_GPIO_NR(2, 28)

#define WAND_RGMII_INT		IMX_GPIO_NR(1, 28)
#define WAND_RGMII_RST		IMX_GPIO_NR(3, 29)

#define WAND_SD1_CD		IMX_GPIO_NR(1, 2)
#define WAND_SD3_CD		IMX_GPIO_NR(3, 9)
#define WAND_SD3_WP		IMX_GPIO_NR(1, 10)

#define WAND_USB_OTG_OC		IMX_GPIO_NR(1, 9)
#define WAND_USB_HOST_PWR_EN	IMX_GPIO_NR(3, 22)
#define WAND_USB_HOST_OC	IMX_GPIO_NR(3, 30)

#define WAND_WL_REF_ON_AB	IMX_GPIO_NR(2, 29)
#define WAND_WL_REF_ON_C1	IMX_GPIO_NR(5, 31)
#define WAND_WL_RST_N		IMX_GPIO_NR(5, 2)
#define WAND_WL_REG_ON		IMX_GPIO_NR(1, 26)
#define WAND_WL_HOST_WAKE	IMX_GPIO_NR(1, 29)
#define WAND_WL_WAKE		IMX_GPIO_NR(1, 30)

/* Syntactic sugar for pad configuration */
#define IMX6_SETUP_PAD(p) \
	if (cpu_is_mx6q()) \
		mxc_iomux_v3_setup_pad(MX6Q_PAD_##p); \
	else \
		mxc_iomux_v3_setup_pad(MX6DL_PAD_##p)

/* See arch/arm/plat-mxc/include/mach/iomux-mx6dl.h for definitions */

enum {
	WAND_REV_UNKNOWN = 0, /* assume rev A/B */
	WAND_REV_AB = 0,
	WAND_REV_C1 /* changed WiFi chip */
};

static int caam_enabled;
static int wand_revision = WAND_REV_UNKNOWN; /**/

/****************************************************************************
 *                                                                          
 * DMA controller init
 *                                                                          
 ****************************************************************************/

static __init void wand_init_dma(void) {
        imx6q_add_dma();        
}

/****************************************************************************
 *                                                                          
 * SD init
 *
 * SD1 is routed to EDM connector (external SD on wand baseboard)
 * SD2 is WiFi
 * SD3 is boot SD on the module
 *                                                                          
 ****************************************************************************/

static const struct esdhc_platform_data wand_sd_data[3] = {
	{
		.cd_gpio		= WAND_SD1_CD,
		.wp_gpio		=-EINVAL,
		.keep_power_at_suspend	= 1,
	        .support_8bit		= 0,
		.delay_line		= 0,
                .cd_type                = ESDHC_CD_CONTROLLER,
	}, {
		.cd_gpio		=-EINVAL,
		.wp_gpio		=-EINVAL,
		.keep_power_at_suspend	= 1,
		.support_8bit		= 0,
		.delay_line		= 0,
                .always_present		= 1,
                .cd_type                = ESDHC_CD_PERMANENT,
	}, {
		.cd_gpio		= WAND_SD3_CD,
		.wp_gpio		= WAND_SD3_WP,
		.keep_power_at_suspend	= 1,
		.support_8bit		= 0,
		.delay_line		= 0,
                .cd_type                = ESDHC_CD_CONTROLLER,
	}
};

/* ------------------------------------------------------------------------ */

static void wand_init_sd(void) {
	int i;

	IMX6_SETUP_PAD( SD1_CLK__USDHC1_CLK_50MHZ_40OHM );
	IMX6_SETUP_PAD( SD1_CMD__USDHC1_CMD_50MHZ_40OHM );
	IMX6_SETUP_PAD( SD1_DAT0__USDHC1_DAT0_50MHZ_40OHM );
	IMX6_SETUP_PAD( SD1_DAT1__USDHC1_DAT1_50MHZ_40OHM );
	IMX6_SETUP_PAD( SD1_DAT2__USDHC1_DAT2_50MHZ_40OHM );
	IMX6_SETUP_PAD( SD1_DAT3__USDHC1_DAT3_50MHZ_40OHM );

	IMX6_SETUP_PAD( SD2_CLK__USDHC2_CLK );
	IMX6_SETUP_PAD( SD2_CMD__USDHC2_CMD );
	IMX6_SETUP_PAD( SD2_DAT0__USDHC2_DAT0 );
	IMX6_SETUP_PAD( SD2_DAT1__USDHC2_DAT1 );
	IMX6_SETUP_PAD( SD2_DAT2__USDHC2_DAT2 );
	IMX6_SETUP_PAD( SD2_DAT3__USDHC2_DAT3 );

	IMX6_SETUP_PAD( SD3_CLK__USDHC3_CLK_50MHZ );
	IMX6_SETUP_PAD( SD3_CMD__USDHC3_CMD_50MHZ );
	IMX6_SETUP_PAD( SD3_DAT0__USDHC3_DAT0_50MHZ );
	IMX6_SETUP_PAD( SD3_DAT1__USDHC3_DAT1_50MHZ );
	IMX6_SETUP_PAD( SD3_DAT2__USDHC3_DAT2_50MHZ );
	IMX6_SETUP_PAD( SD3_DAT3__USDHC3_DAT3_50MHZ );

	/* Card Detect for SD1 & SD3, respectively */
	IMX6_SETUP_PAD( GPIO_2__GPIO_1_2 ); 
	IMX6_SETUP_PAD( EIM_DA9__GPIO_3_9 );

	/* Add mmc devices in reverse order, so mmc0 always is boot sd (SD3) */
	for (i=2; i>=0; i--) {
                imx6q_add_sdhci_usdhc_imx(i, &wand_sd_data[i]);
	}
}


/****************************************************************************
 *                                                                          
 * I2C
 *                                                                          
 ****************************************************************************/

static struct imxi2c_platform_data wand_i2c_data[] = {
	{ .bitrate	= 400000, },
	{ .bitrate	= 100000, },
	{ .bitrate	= 400000, },
};

/* ------------------------------------------------------------------------ */

static void __init wand_init_i2c(void) {
        int i;
        
        IMX6_SETUP_PAD( EIM_D21__I2C1_SCL );
	IMX6_SETUP_PAD( EIM_D28__I2C1_SDA );

	IMX6_SETUP_PAD( KEY_COL3__I2C2_SCL );
	IMX6_SETUP_PAD( KEY_ROW3__I2C2_SDA );

	IMX6_SETUP_PAD( GPIO_5__I2C3_SCL );
	IMX6_SETUP_PAD( GPIO_16__I2C3_SDA );
        
	for (i=0; i<3; i++) {
		imx6q_add_imx_i2c(i, &wand_i2c_data[i]);
        }
}


/****************************************************************************
 *                                                                          
 * Initialize debug console (UART1)
 *                                                                          
 ****************************************************************************/
 
static __init void wand_init_uart(void) {
	IMX6_SETUP_PAD( CSI0_DAT10__UART1_TXD );
	IMX6_SETUP_PAD( CSI0_DAT11__UART1_RXD );
        IMX6_SETUP_PAD( EIM_D19__UART1_CTS );
        IMX6_SETUP_PAD( EIM_D20__UART1_RTS );

	imx6q_add_imx_uart(0, NULL);
}


/****************************************************************************
 *                                                                          
 * Initialize sound (SSI, ASRC, AUD3 channel and S/PDIF)
 *                                                                          
 ****************************************************************************/

extern struct mxc_audio_platform_data wand_audio_channel_data;

/* This function is called as a callback from the audio channel data struct */
static int wand_audio_clock_enable(void) {
	struct clk *clko;
	struct clk *new_parent;
	int rate;

	clko = clk_get(NULL, "clko_clk");
	if (IS_ERR(clko)) return PTR_ERR(clko);
	
        new_parent = clk_get(NULL, "ahb");
	if (!IS_ERR(new_parent)) {
		clk_set_parent(clko, new_parent);
		clk_put(new_parent);
	}
        
	rate = clk_round_rate(clko, 16000000);
	if (rate < 8000000 || rate > 27000000) {
		clk_put(clko);
		return -1;
	}

        wand_audio_channel_data.sysclk = rate;
	clk_set_rate(clko, rate);
	clk_enable(clko);
        
	return 0;        
}

/* ------------------------------------------------------------------------ */

/* This struct is added by the baseboard when initializing the codec */
struct mxc_audio_platform_data wand_audio_channel_data = {
	.ssi_num = 1,
	.src_port = 2,
	.ext_port = 3,
	.init = wand_audio_clock_enable,
	.hp_gpio = -1,
};

/* ------------------------------------------------------------------------ */

static int wand_set_spdif_clk_rate(struct clk *clk, unsigned long rate) {
	unsigned long rate_actual;
	rate_actual = clk_round_rate(clk, rate);
	clk_set_rate(clk, rate_actual);
	return 0;
}

/* ------------------------------------------------------------------------ */

static struct mxc_spdif_platform_data wand_spdif = {
	.spdif_tx		= 1,	/* enable tx */
	.spdif_rx		= 1,	/* enable rx */
	.spdif_clk_44100	= 1,    /* tx clk from spdif0_clk_root */
	.spdif_clk_48000	= 1,    /* tx clk from spdif0_clk_root */
	.spdif_div_44100	= 23,
	.spdif_div_48000	= 37,
	.spdif_div_32000	= 37,
	.spdif_rx_clk		= 0,    /* rx clk from spdif stream */
	.spdif_clk_set_rate	= wand_set_spdif_clk_rate,
	.spdif_clk		= NULL, /* spdif bus clk */
};

/* ------------------------------------------------------------------------ */

static struct imx_ssi_platform_data wand_ssi_pdata = {
	.flags = IMX_SSI_DMA | IMX_SSI_SYN,
};

/* ------------------------------------------------------------------------ */

static struct imx_asrc_platform_data wand_asrc_data = {
	.channel_bits	= 4,
	.clk_map_ver	= 2,
};

/* ------------------------------------------------------------------------ */

void __init wand_init_audio(void) {
	/* Enable AUD3 channel (routed to primary I2S) */
	IMX6_SETUP_PAD( CSI0_DAT4__AUDMUX_AUD3_TXC );
	IMX6_SETUP_PAD( CSI0_DAT5__AUDMUX_AUD3_TXD );
	IMX6_SETUP_PAD( CSI0_DAT6__AUDMUX_AUD3_TXFS );
	IMX6_SETUP_PAD( CSI0_DAT7__AUDMUX_AUD3_RXD );
	IMX6_SETUP_PAD( GPIO_0__CCM_CLKO );
        
        /* Sample rate converter is added together with audio */
	wand_asrc_data.asrc_core_clk = clk_get(NULL, "asrc_clk");
	wand_asrc_data.asrc_audio_clk = clk_get(NULL, "asrc_serial_clk");
	imx6q_add_asrc(&wand_asrc_data);

	imx6q_add_imx_ssi(1, &wand_ssi_pdata);

	/* Enable SPDIF */

        IMX6_SETUP_PAD( ENET_RXD0__SPDIF_OUT1);

	wand_spdif.spdif_core_clk = clk_get_sys("mxc_spdif.0", NULL);
	clk_put(wand_spdif.spdif_core_clk);
	imx6q_add_spdif(&wand_spdif);                
	imx6q_add_spdif_dai();
	imx6q_add_spdif_audio_device();
}


/*****************************************************************************
 *                                                                           
 * Init FEC and AR8031 PHY
 *                                                                            
 *****************************************************************************/

static int wand_fec_phy_init(struct phy_device *phydev) {
	unsigned short val;

	/* Enable AR8031 125MHz clk */
	phy_write(phydev, 0x0d, 0x0007); /* Set device address to 7*/
	phy_write(phydev, 0x00, 0x8000); /* Apply by soft reset */
	udelay(500); 
        
	phy_write(phydev, 0x0e, 0x8016); /* set mmd reg */
	phy_write(phydev, 0x0d, 0x4007); /* apply */

	val = phy_read(phydev, 0xe);
	val &= 0xffe3;
	val |= 0x18;
	phy_write(phydev, 0xe, val);
	phy_write(phydev, 0x0d, 0x4007); /* Post data */        

	/* Introduce random tx clock delay. Why is this needed? */
	phy_write(phydev, 0x1d, 0x5);
	val = phy_read(phydev, 0x1e);
	val |= 0x0100;
	phy_write(phydev, 0x1e, val);

	return 0;
}

/* ------------------------------------------------------------------------ */

static int wand_fec_power_hibernate(struct phy_device *phydev) { return 0; }

/* ------------------------------------------------------------------------ */

static struct fec_platform_data wand_fec_data = {
	.init			= wand_fec_phy_init,
	.power_hibernate	= wand_fec_power_hibernate,
	.phy			= PHY_INTERFACE_MODE_RGMII,
	.phy_noscan_mask	= ~2, /* phy is on adress 1 */
};

/* ------------------------------------------------------------------------ */

static __init void wand_init_ethernet(void) {
	IMX6_SETUP_PAD( ENET_MDIO__ENET_MDIO );
	IMX6_SETUP_PAD( ENET_MDC__ENET_MDC );
	IMX6_SETUP_PAD( ENET_REF_CLK__ENET_TX_CLK );
	IMX6_SETUP_PAD( RGMII_TXC__ENET_RGMII_TXC );
	IMX6_SETUP_PAD( RGMII_TD0__ENET_RGMII_TD0 );
	IMX6_SETUP_PAD( RGMII_TD1__ENET_RGMII_TD1 );
	IMX6_SETUP_PAD( RGMII_TD2__ENET_RGMII_TD2 );
	IMX6_SETUP_PAD( RGMII_TD3__ENET_RGMII_TD3 );
	IMX6_SETUP_PAD( RGMII_TX_CTL__ENET_RGMII_TX_CTL );
	IMX6_SETUP_PAD( RGMII_RXC__ENET_RGMII_RXC );
	IMX6_SETUP_PAD( RGMII_RD0__ENET_RGMII_RD0 );
	IMX6_SETUP_PAD( RGMII_RD1__ENET_RGMII_RD1 );
	IMX6_SETUP_PAD( RGMII_RD2__ENET_RGMII_RD2 );
	IMX6_SETUP_PAD( RGMII_RD3__ENET_RGMII_RD3 );
	IMX6_SETUP_PAD( RGMII_RX_CTL__ENET_RGMII_RX_CTL );                
	IMX6_SETUP_PAD( ENET_TX_EN__GPIO_1_28 );
	IMX6_SETUP_PAD( EIM_D29__GPIO_3_29 );
        
	gpio_request(WAND_RGMII_RST, "rgmii reset");
	gpio_direction_output(WAND_RGMII_RST, 0);
#ifdef CONFIG_FEC_1588
	mxc_iomux_set_gpr_register(1, 21, 1, 1);
#endif
	msleep(10);
	gpio_set_value(WAND_RGMII_RST, 1);
	imx6_init_fec(wand_fec_data);
}


/****************************************************************************
 *                                                                          
 * USB
 *                                                                          
 ****************************************************************************/

static void wand_usbotg_vbus(bool on) {
	gpio_set_value_cansleep(WAND_USB_HOST_PWR_EN, !on);
}

/* ------------------------------------------------------------------------ */

static __init void wand_init_usb(void) {
	IMX6_SETUP_PAD( GPIO_9__GPIO_1_9 );
	IMX6_SETUP_PAD( GPIO_1__USBOTG_ID );
	IMX6_SETUP_PAD( EIM_D22__GPIO_3_22 );
	IMX6_SETUP_PAD( EIM_D30__GPIO_3_30 );
        
	gpio_request(WAND_USB_OTG_OC, "otg oc");
	gpio_direction_input(WAND_USB_OTG_OC);

	gpio_request(WAND_USB_HOST_PWR_EN, "host pwr");
	gpio_direction_output(WAND_USB_HOST_PWR_EN, 0);

	imx_otg_base = MX6_IO_ADDRESS(MX6Q_USB_OTG_BASE_ADDR);
	/* GPR1: bit 13 == 1 means GPIO1 is OTG_ID pin */
	mxc_iomux_set_gpr_register(1, 13, 1, 1);

	mx6_set_otghost_vbus_func(wand_usbotg_vbus);

	gpio_request(WAND_USB_HOST_OC, "usbh1 oc");
	gpio_direction_input(WAND_USB_HOST_OC);
}


/****************************************************************************
 *                                                                          
 * IPU
 *                                                                          
 ****************************************************************************/

static struct imx_ipuv3_platform_data wand_ipu_data[] = {
	{
		.rev		= 4,
		.csi_clk[0]	= "ccm_clk0",
	}, {
		.rev		= 4,
		.csi_clk[0]	= "ccm_clk0",
	},
};

/* ------------------------------------------------------------------------ */

static __init void wand_init_ipu(void) {
	imx6q_add_ipuv3(0, &wand_ipu_data[0]);
	if (cpu_is_mx6q()) imx6q_add_ipuv3(1, &wand_ipu_data[1]);
}


/****************************************************************************
 *                                                                          
 * HDMI
 *                                                                          
 ****************************************************************************/

static struct ipuv3_fb_platform_data wand_hdmi_fb[] = {
	{ /* hdmi framebuffer */
		.disp_dev		= "hdmi",
		.interface_pix_fmt	= IPU_PIX_FMT_RGB32,
		.mode_str		= "1920x1080@60",
		.default_bpp		= 32,
		.int_clk		= false,
	}
};

/* ------------------------------------------------------------------------ */

static void wand_hdmi_init(int ipu_id, int disp_id) {
	if ((unsigned)ipu_id > 1) ipu_id = 0;
	if ((unsigned)disp_id > 1) disp_id = 0;

	mxc_iomux_set_gpr_register(3, 2, 2, 2*ipu_id + disp_id);
}

/* ------------------------------------------------------------------------ */

static struct fsl_mxc_hdmi_platform_data wand_hdmi_data = {
	.init = wand_hdmi_init,
};

/* ------------------------------------------------------------------------ */

static struct fsl_mxc_hdmi_core_platform_data wand_hdmi_core_data = {
	.ipu_id		= 0,
	.disp_id	= 1,
};

/* ------------------------------------------------------------------------ */

static const struct i2c_board_info wand_hdmi_i2c_info = {
	I2C_BOARD_INFO("mxc_hdmi_i2c", 0x50),
};

/* ------------------------------------------------------------------------ */

static void wand_init_hdmi(void) {
	i2c_register_board_info(edm_ddc, &wand_hdmi_i2c_info, 1);
	imx6q_add_mxc_hdmi_core(&wand_hdmi_core_data);
	imx6q_add_mxc_hdmi(&wand_hdmi_data);
	imx6q_add_ipuv3fb(0, wand_hdmi_fb);
        
        /* Enable HDMI audio */
	imx6q_add_hdmi_soc();
	imx6q_add_hdmi_soc_dai();        
	mxc_iomux_set_gpr_register(0, 0, 1, 1);
}


/****************************************************************************
 *                                                                          
 * LCD/LVDS/TTL
 *                                                                          
 ****************************************************************************/

static struct fsl_mxc_lcd_platform_data wand_lcdif_data = {
	.ipu_id = 0,
	.disp_id = 0,
	.default_ifmt = IPU_PIX_FMT_RGB666,
};

/* ------------------------------------------------------------------------ */

static struct fsl_mxc_ldb_platform_data wand_ldb_data = {
	.ipu_id = 0,
	.disp_id = 0,
	.ext_ref = 1,
	.mode = LDB_SIN0,
	.sec_ipu_id = 0,
	.sec_disp_id = 1,
};

/* ------------------------------------------------------------------------ */

static struct ipuv3_fb_platform_data wand_lvds_fb[] = {
	{
		.disp_dev = "ldb",
		.interface_pix_fmt = IPU_PIX_FMT_RGB24,
		.mode_str = "LDB-WVGA",
		.default_bpp = 24,
		.int_clk = false,
	},
};

/* ------------------------------------------------------------------------ */


static void __init wand_init_lcd(void) {
	/* TTL */
	IMX6_SETUP_PAD( DI0_DISP_CLK__IPU1_DI0_DISP_CLK );
	IMX6_SETUP_PAD( DI0_PIN2__IPU1_DI0_PIN2 );		/* HSync */
	IMX6_SETUP_PAD( DI0_PIN3__IPU1_DI0_PIN3 );		/* VSync */
	IMX6_SETUP_PAD( DI0_PIN4__IPU1_DI0_PIN4 );		/* Contrast */
	IMX6_SETUP_PAD( DI0_PIN15__IPU1_DI0_PIN15 );		/* DISP0_DRDY */
	IMX6_SETUP_PAD( DISP0_DAT0__IPU1_DISP0_DAT_0 );
	IMX6_SETUP_PAD( DISP0_DAT1__IPU1_DISP0_DAT_1 );
	IMX6_SETUP_PAD( DISP0_DAT2__IPU1_DISP0_DAT_2 );
	IMX6_SETUP_PAD( DISP0_DAT3__IPU1_DISP0_DAT_3 );
	IMX6_SETUP_PAD( DISP0_DAT4__IPU1_DISP0_DAT_4 );
	IMX6_SETUP_PAD( DISP0_DAT5__IPU1_DISP0_DAT_5 );
	IMX6_SETUP_PAD( DISP0_DAT6__IPU1_DISP0_DAT_6 );
	IMX6_SETUP_PAD( DISP0_DAT7__IPU1_DISP0_DAT_7 );
	IMX6_SETUP_PAD( DISP0_DAT8__IPU1_DISP0_DAT_8 );
	IMX6_SETUP_PAD( DISP0_DAT9__IPU1_DISP0_DAT_9 );
	IMX6_SETUP_PAD( DISP0_DAT10__IPU1_DISP0_DAT_10 );
	IMX6_SETUP_PAD( DISP0_DAT11__IPU1_DISP0_DAT_11 );
	IMX6_SETUP_PAD( DISP0_DAT12__IPU1_DISP0_DAT_12 );
	IMX6_SETUP_PAD( DISP0_DAT13__IPU1_DISP0_DAT_13 );
	IMX6_SETUP_PAD( DISP0_DAT14__IPU1_DISP0_DAT_14 );
	IMX6_SETUP_PAD( DISP0_DAT15__IPU1_DISP0_DAT_15 );
	IMX6_SETUP_PAD( DISP0_DAT16__IPU1_DISP0_DAT_16 );
	IMX6_SETUP_PAD( DISP0_DAT17__IPU1_DISP0_DAT_17 );
	IMX6_SETUP_PAD( DISP0_DAT18__IPU1_DISP0_DAT_18 );
	IMX6_SETUP_PAD( DISP0_DAT19__IPU1_DISP0_DAT_19 );
	IMX6_SETUP_PAD( DISP0_DAT20__IPU1_DISP0_DAT_20 );
	IMX6_SETUP_PAD( DISP0_DAT21__IPU1_DISP0_DAT_21 );
	IMX6_SETUP_PAD( DISP0_DAT22__IPU1_DISP0_DAT_22 );
	IMX6_SETUP_PAD( DISP0_DAT23__IPU1_DISP0_DAT_23 );

	/* LVDS */
	IMX6_SETUP_PAD( SD4_DAT0__GPIO_2_8 );
	IMX6_SETUP_PAD( SD4_DAT1__GPIO_2_9 );
	IMX6_SETUP_PAD( SD4_DAT2__GPIO_2_10 );
	IMX6_SETUP_PAD( SD4_DAT3__GPIO_2_11 );
        
	IMX6_SETUP_PAD( LVDS0_CLK_P__LDB_LVDS0_CLK );
	IMX6_SETUP_PAD( LVDS0_TX0_P__LDB_LVDS0_TX0 );
	IMX6_SETUP_PAD( LVDS0_TX1_P__LDB_LVDS0_TX1 );
	IMX6_SETUP_PAD( LVDS0_TX2_P__LDB_LVDS0_TX2 );
	IMX6_SETUP_PAD( LVDS0_TX3_P__LDB_LVDS0_TX3 );

	gpio_request(IMX_GPIO_NR(2, 8), "lvds0_en");
	gpio_direction_output(IMX_GPIO_NR(2, 8), 1);
        
	gpio_request(IMX_GPIO_NR(2, 9), "lvds0_blt_ctrl");
	gpio_direction_output(IMX_GPIO_NR(2, 9), 1);

	gpio_request(IMX_GPIO_NR(2, 10), "disp0_bklen");
	gpio_direction_output(IMX_GPIO_NR(2, 10), 1);

	gpio_request(IMX_GPIO_NR(2, 11), "disp0_vdden");
	gpio_direction_output(IMX_GPIO_NR(2, 11), 1);

	imx6q_add_vdoa();

	imx6q_add_ldb(&wand_ldb_data);
	imx6q_add_lcdif(&wand_lcdif_data);

	imx6q_add_ipuv3fb(1, &wand_lvds_fb[0]);
}


/****************************************************************************
 *                                                                          
 * WiFi
 *                                                                          
 ****************************************************************************/

/* assumes SD/MMC pins are set; call after wand_init_sd() */
static __init void wand_init_wifi(void) {
	/* Wifi reset (resets when low!) */
	IMX6_SETUP_PAD( EIM_A25__GPIO_5_2 );
	/* reg on, signal to FDC6331L */
	IMX6_SETUP_PAD( ENET_RXD1__GPIO_1_26 );
	/* host wake */
	IMX6_SETUP_PAD( ENET_TXD1__GPIO_1_29 );
	/* wl wake - nc */
	IMX6_SETUP_PAD( ENET_TXD0__GPIO_1_30 );

	if (wand_revision == WAND_REV_C1) {
		IMX6_SETUP_PAD( CSI0_DAT13__GPIO_5_31 );

		gpio_request(WAND_WL_REF_ON_C1, "wl_ref_on");
		gpio_direction_output(WAND_WL_REF_ON_C1, 1);
		gpio_export(WAND_WL_REF_ON_C1, 0);
	} else {
		/* ref_on, enable 32k clock */
		IMX6_SETUP_PAD( EIM_EB1__GPIO_2_29 );

		gpio_request(WAND_WL_REF_ON_AB, "wl_ref_on");
		gpio_direction_output(WAND_WL_REF_ON_AB, 1);
		gpio_export(WAND_WL_REF_ON_AB, 0);
        }

	gpio_request(WAND_WL_RST_N, "wl_rst_n");
	gpio_direction_output(WAND_WL_RST_N, 0);
	msleep(11);
	gpio_set_value(WAND_WL_RST_N, 1);
	gpio_export(WAND_WL_RST_N, 0);

	gpio_request(WAND_WL_REG_ON, "wl_reg_on");
	gpio_direction_output(WAND_WL_REG_ON, 1);
	gpio_export(WAND_WL_REG_ON, 0);

        gpio_request(WAND_WL_WAKE, "wl_wake");
	gpio_direction_output(WAND_WL_WAKE, 1);

	gpio_request(WAND_WL_HOST_WAKE, "wl_host_wake");
	gpio_direction_input(WAND_WL_HOST_WAKE);
}


/****************************************************************************
 *                                                                          
 * Bluetooth
 *                                                                          
 ****************************************************************************/

static const struct imxuart_platform_data wand_bt_uart_data = {
	.flags = IMXUART_HAVE_RTSCTS | IMXUART_SDMA,
	.dma_req_tx = MX6Q_DMA_REQ_UART3_TX,
	.dma_req_rx = MX6Q_DMA_REQ_UART3_RX,
};

/* ------------------------------------------------------------------------ */

/* This assumes wifi is initialized (chip has power) */
static __init void wand_init_bluetooth(void) {

	if (wand_revision == WAND_REV_C1) {
		/* BT_ON, BT_WAKE and BT_HOST_WAKE */
		IMX6_SETUP_PAD( CSI0_DATA_EN__GPIO_5_20 );
                IMX6_SETUP_PAD( CSI0_DAT12__GPIO_5_30 );
		IMX6_SETUP_PAD( CSI0_VSYNC__GPIO_5_21 );

		gpio_request(WAND_BT_ON_C1, "bt_on");
		gpio_direction_output(WAND_BT_ON_C1, 0);
		msleep(11);
		gpio_set_value(WAND_BT_ON_C1, 1);
		gpio_export(WAND_BT_ON_C1, 0);

		gpio_request(WAND_BT_WAKE_C1, "bt_wake");
		gpio_direction_output(WAND_BT_WAKE_C1, 1);

		gpio_request(WAND_BT_HOST_WAKE_C1, "bt_host_wake");
		gpio_direction_input(WAND_BT_HOST_WAKE_C1);

	} else {
		/* BT_ON, BT_WAKE and BT_HOST_WAKE */
		IMX6_SETUP_PAD( EIM_DA13__GPIO_3_13 );
		IMX6_SETUP_PAD( EIM_DA14__GPIO_3_14 );
		IMX6_SETUP_PAD( EIM_DA15__GPIO_3_15 );

		gpio_request(WAND_BT_ON_AB, "bt_on");
		gpio_direction_output(WAND_BT_ON_AB, 0);
		msleep(11);
		gpio_set_value(WAND_BT_ON_AB, 1);
		gpio_export(WAND_BT_ON_AB, 0);

		gpio_request(WAND_BT_WAKE_AB, "bt_wake");
		gpio_direction_output(WAND_BT_WAKE_AB, 1);

		gpio_request(WAND_BT_HOST_WAKE_AB, "bt_host_wake");
		gpio_direction_input(WAND_BT_HOST_WAKE_AB);
        }

        /* AUD5 channel goes to BT */
        IMX6_SETUP_PAD( KEY_COL0__AUDMUX_AUD5_TXC );
        IMX6_SETUP_PAD( KEY_ROW0__AUDMUX_AUD5_TXD );
        IMX6_SETUP_PAD( KEY_COL1__AUDMUX_AUD5_TXFS );
        IMX6_SETUP_PAD( KEY_ROW1__AUDMUX_AUD5_RXD );
        
        /* Bluetooth is on UART3*/
        IMX6_SETUP_PAD( EIM_D23__UART3_CTS );
        IMX6_SETUP_PAD( EIM_D24__UART3_TXD );
        IMX6_SETUP_PAD( EIM_D25__UART3_RXD );
        IMX6_SETUP_PAD( EIM_EB3__UART3_RTS );
        
	imx6q_add_imx_uart(2, &wand_bt_uart_data);
}


/****************************************************************************
 *                                                                          
 * Power and thermal management
 *                                                                          
 ****************************************************************************/

extern bool enable_wait_mode;

static const struct anatop_thermal_platform_data wand_thermal = {
	.name = "anatop_thermal",
};

/* ------------------------------------------------------------------------ */


static void wand_suspend_enter(void) {
	gpio_set_value(WAND_USB_HOST_PWR_EN, 0);

	if (wand_revision == WAND_REV_C1) {
		gpio_direction_output(WAND_WL_REF_ON_C1, 0);
		gpio_set_value(WAND_BT_WAKE_C1, 0);
        } else {
		gpio_direction_output(WAND_WL_REF_ON_AB, 0);
		gpio_set_value(WAND_BT_WAKE_AB, 0);
        }
        gpio_set_value(WAND_WL_WAKE, 0);

	gpio_direction_output(WAND_WL_RST_N, 0);
	gpio_direction_output(WAND_WL_REG_ON, 0);

	gpio_direction_output(IMX_GPIO_NR(2, 8), 0);
	gpio_direction_output(IMX_GPIO_NR(2, 9), 0);
	gpio_direction_output(IMX_GPIO_NR(2, 10), 0);
	gpio_direction_output(IMX_GPIO_NR(2, 11), 0);
}

/* ------------------------------------------------------------------------ */

static void wand_suspend_exit(void) {
	gpio_direction_output(IMX_GPIO_NR(2, 8), 1);
	gpio_direction_output(IMX_GPIO_NR(2, 9), 1);
	gpio_direction_output(IMX_GPIO_NR(2, 10), 1);
	gpio_direction_output(IMX_GPIO_NR(2, 11), 1);

	gpio_direction_output(WAND_WL_RST_N, 1);
	if (wand_revision == WAND_REV_C1) {
		gpio_set_value(WAND_BT_WAKE_C1, 1);
		gpio_direction_output(WAND_WL_REF_ON_C1, 1);
	} else {
		gpio_direction_output(WAND_WL_REF_ON_AB, 1);
		gpio_set_value(WAND_BT_WAKE_AB, 1);
        }
	gpio_direction_output(WAND_WL_REG_ON, 1);

	gpio_set_value(WAND_WL_WAKE, 1);

	gpio_set_value(WAND_USB_HOST_PWR_EN, 1);
}

/* ------------------------------------------------------------------------ */

static const struct pm_platform_data wand_pm_data = {
	.name		= "imx_pm",
	.suspend_enter	= wand_suspend_enter,
	.suspend_exit	= wand_suspend_exit,
};

/* ------------------------------------------------------------------------ */

static const struct mxc_dvfs_platform_data wand_dvfscore_data = {
	.reg_id			= "cpu_vddgp",
	.clk1_id		= "cpu_clk",
	.clk2_id 		= "gpc_dvfs_clk",
	.gpc_cntr_offset 	= MXC_GPC_CNTR_OFFSET,
	.ccm_cdcr_offset 	= MXC_CCM_CDCR_OFFSET,
	.ccm_cacrr_offset 	= MXC_CCM_CACRR_OFFSET,
	.ccm_cdhipr_offset 	= MXC_CCM_CDHIPR_OFFSET,
	.prediv_mask 		= 0x1F800,
	.prediv_offset 		= 11,
	.prediv_val 		= 3,
	.div3ck_mask 		= 0xE0000000,
	.div3ck_offset 		= 29,
	.div3ck_val 		= 2,
	.emac_val 		= 0x08,
	.upthr_val 		= 25,
	.dnthr_val 		= 9,
	.pncthr_val 		= 33,
	.upcnt_val 		= 10,
	.dncnt_val 		= 10,
	.delay_time 		= 80,
};

/* ------------------------------------------------------------------------ */

static __init void wand_init_pm(void) {
	enable_wait_mode = false;
	imx6q_add_anatop_thermal_imx(1, &wand_thermal);
        imx6_add_armpmu();
	imx6q_add_pm_imx(0, &wand_pm_data);
	imx6q_add_dvfs_core(&wand_dvfscore_data);
	imx6q_add_busfreq();
	imx6q_add_imx2_wdt(0, NULL);
}


/****************************************************************************
 *                                                                          
 * Expansion pin header GPIOs
 *                                                                          
 ****************************************************************************/

static __init void wand_init_external_gpios(void) {
	IMX6_SETUP_PAD( EIM_DA12__GPIO_3_12 ); /* p255 */
	IMX6_SETUP_PAD( EIM_DA11__GPIO_3_11 ); /* p256 */
	IMX6_SETUP_PAD( EIM_DA10__GPIO_3_10 ); /* p257 */
	IMX6_SETUP_PAD( EIM_D27__GPIO_3_27 ); /* p258 */
	IMX6_SETUP_PAD( EIM_D26__GPIO_3_26 ); /* p259 */
	IMX6_SETUP_PAD( EIM_BCLK__GPIO_6_31 ); /* p260 */
	IMX6_SETUP_PAD( EIM_DA8__GPIO_3_8 ); /* p261 */
	IMX6_SETUP_PAD( ENET_RX_ER__GPIO_1_24 ); /* p262 */
	IMX6_SETUP_PAD( GPIO_19__GPIO_4_5 ); /* p263 */
	IMX6_SETUP_PAD( SD3_RST__GPIO_7_8 ); /* p264 */
        
	/* GPIO export and setup is in EDM framework, see drivers/edm/edm.c */
}


/****************************************************************************
 *                                                                          
 * SPI - while not used on the Wandboard, the pins are routed to baseboard
 *                                                                          
 ****************************************************************************/

static const int wand_spi1_chipselect[] = { IMX_GPIO_NR(2, 30) };

/* platform device */
static const struct spi_imx_master wand_spi1_data = {
	.chipselect     = (int *)wand_spi1_chipselect,
	.num_chipselect = ARRAY_SIZE(wand_spi1_chipselect),
};

/* ------------------------------------------------------------------------ */

static const int wand_spi2_chipselect[] = { IMX_GPIO_NR(2, 26), IMX_GPIO_NR(2, 27) };

static const struct spi_imx_master wand_spi2_data = {
	.chipselect     = (int *)wand_spi2_chipselect,
	.num_chipselect = ARRAY_SIZE(wand_spi2_chipselect),
};

/* ------------------------------------------------------------------------ */

static void __init wand_init_spi(void) {
	IMX6_SETUP_PAD( EIM_D16__ECSPI1_SCLK );
	IMX6_SETUP_PAD( EIM_D17__ECSPI1_MISO );
	IMX6_SETUP_PAD( EIM_D18__ECSPI1_MOSI );
	IMX6_SETUP_PAD( EIM_EB2__GPIO_2_30 );

	IMX6_SETUP_PAD( EIM_CS0__ECSPI2_SCLK );
	IMX6_SETUP_PAD( EIM_CS1__ECSPI2_MOSI );
	IMX6_SETUP_PAD( EIM_OE__ECSPI2_MISO );
/* The choice of using gpios for chipselect is deliberate,
   there can be issues using the dedicated mux modes for cs.*/
	IMX6_SETUP_PAD( EIM_RW__GPIO_2_26 );
	IMX6_SETUP_PAD( EIM_LBA__GPIO_2_27 );
        
	imx6q_add_ecspi(0, &wand_spi1_data);
	imx6q_add_ecspi(1, &wand_spi2_data);
}


/****************************************************************************
 *                                                                          
 * Vivante GPU/VPU
 *                                                                          
 ****************************************************************************/

static const __initconst struct imx_viv_gpu_data wand_gpu_data = {
	.phys_baseaddr = 0,
	.iobase_3d = GPU_3D_ARB_BASE_ADDR,
	.irq_3d = MXC_INT_GPU3D_IRQ,
	.iobase_2d = GPU_2D_ARB_BASE_ADDR,
	.irq_2d = MXC_INT_GPU2D_IRQ,
	.iobase_vg = OPENVG_ARB_BASE_ADDR,
	.irq_vg = MXC_INT_OPENVG_XAQ2,
};

static struct viv_gpu_platform_data wand_gpu_pdata = {
	.reserved_mem_size = 64 * SZ_1M,
};

struct wand_vout_mem_data {
	resource_size_t res_mbase;
	resource_size_t res_msize;
};

static struct wand_vout_mem_data wand_vout_mem __initdata = {
	.res_msize = 48 * SZ_1M,
};

static __init void wand_init_gpu(void) {
	struct platform_device *voutdev;
	imx_add_viv_gpu(&wand_gpu_data, &wand_gpu_pdata);
	imx6q_add_vpu();
	voutdev = imx6q_add_v4l2_output(0);

	if (wand_vout_mem.res_msize && voutdev) {
		dma_declare_coherent_memory(&voutdev->dev,
					    wand_vout_mem.res_mbase,
					    wand_vout_mem.res_mbase,
					    wand_vout_mem.res_msize,
					    (DMA_MEMORY_MAP |
                                             DMA_MEMORY_EXCLUSIVE));
	}
}


/*****************************************************************************
 *                                                                            
 * PCI Express (not present on default baseboard, but is routed to connector)
 *                                                                            
 *****************************************************************************/

static const struct imx_pcie_platform_data wand_pcie_data = {
	.pcie_pwr_en	= -EINVAL,
	.pcie_rst	= WAND_PCIE_NRST,
	.pcie_wake_up	= -EINVAL,
	.pcie_dis	= -EINVAL,
};

/* ------------------------------------------------------------------------ */

static void __init wand_init_pcie(void) {
	IMX6_SETUP_PAD( EIM_D31__GPIO_3_31);
	imx6q_add_pcie(&wand_pcie_data);
}


/****************************************************************************
 *
 * CAAM - I.MX6 Cryptographic Acceleration
 *
 ****************************************************************************/

static int __init caam_setup(char *__unused)
{
        caam_enabled = 1;
        return 1;
}
early_param("caam", caam_setup);

static void __init wand_init_caam(void) {
	if (caam_enabled) {
		pr_info("CAAM loading\n");
		imx6q_add_imx_caam();
        }
}


/****************************************************************************
 *                                                                          
 * AHCI - SATA
 *                                                                          
 ****************************************************************************/

static struct clk *wand_sata_clk;

/* HW Initialization, if return 0, initialization is successful. */
static int wand_sata_init(struct device *dev, void __iomem *addr) {
	u32 tmpdata;
	int ret = 0;
	struct clk *clk;

        wand_sata_clk = clk_get(dev, "imx_sata_clk");
	if (IS_ERR(wand_sata_clk)) {
		dev_err(dev, "no sata clock.\n");
		return PTR_ERR(wand_sata_clk);
	}

	ret = clk_enable(wand_sata_clk);
	if (ret) {
		dev_err(dev, "can't enable sata clock.\n");
		goto put_sata_clk;
	}

	/* Set PHY Paremeters, two steps to configure the GPR13,
	 * one write for rest of parameters, mask of first write is 0x07FFFFFD,
	 * and the other one write for setting the mpll_clk_off_b
	 *.rx_eq_val_0(iomuxc_gpr13[26:24]),
	 *.los_lvl(iomuxc_gpr13[23:19]),
	 *.rx_dpll_mode_0(iomuxc_gpr13[18:16]),
	 *.sata_speed(iomuxc_gpr13[15]),
	 *.mpll_ss_en(iomuxc_gpr13[14]),
	 *.tx_atten_0(iomuxc_gpr13[13:11]),
	 *.tx_boost_0(iomuxc_gpr13[10:7]),
	 *.tx_lvl(iomuxc_gpr13[6:2]),
	 *.mpll_ck_off(iomuxc_gpr13[1]),
	 *.tx_edgerate_0(iomuxc_gpr13[0]),
	 */

	tmpdata = readl(IOMUXC_GPR13);
	writel(tmpdata & ~0x2, IOMUXC_GPR13);
	writel(((tmpdata & ~0x07FFFFFF) | 0x0593A046), IOMUXC_GPR13);

	sata_phy_cr_addr(0x7F3F, addr);
	sata_phy_cr_write(0x1, addr);
	sata_phy_cr_read(&tmpdata, addr);

	/* Get the AHB clock rate, and configure the TIMER1MS reg later */
	clk = clk_get(NULL, "ahb");
	if (IS_ERR(clk)) {
		dev_err(dev, "no ahb clock.\n");
		ret = PTR_ERR(clk);
		goto release_sata_clk;
	}
	tmpdata = clk_get_rate(clk) / 1000;
	clk_put(clk);

	ret = sata_init(addr, tmpdata);
	if (ret == 0)
		return ret;

release_sata_clk:
	clk_disable(wand_sata_clk);
put_sata_clk:
	clk_put(wand_sata_clk);

	return ret;
}

static void wand_sata_exit(struct device *dev) {
	clk_disable(wand_sata_clk);
	clk_put(wand_sata_clk);
}

static struct ahci_platform_data wand_sata_data = {
	.init = wand_sata_init,
	.exit = wand_sata_exit,
};

static __init void wand_init_sata(void) {
        imx6q_add_ahci(0, &wand_sata_data);
}


/*****************************************************************************
 *                                                                           
 * Init EDM framework                                      
 *                                                                            
 *****************************************************************************/

static void __init wand_init_edm(void) {
	edm_version = 1;
        
	edm_external_gpio[0] = IMX_GPIO_NR(3, 12);        
	edm_external_gpio[1] = IMX_GPIO_NR(3, 11);
	edm_external_gpio[2] = IMX_GPIO_NR(4, 10);
	edm_external_gpio[3] = IMX_GPIO_NR(3, 27);
	edm_external_gpio[4] = IMX_GPIO_NR(3, 26);
	edm_external_gpio[5] = IMX_GPIO_NR(6, 31);
	edm_external_gpio[6] = IMX_GPIO_NR(3, 8);
	edm_external_gpio[7] = IMX_GPIO_NR(1, 24);
	edm_external_gpio[8] = IMX_GPIO_NR(4, 5);
	edm_external_gpio[9] = IMX_GPIO_NR(7, 8);
        
	edm_i2c[0] = 0;
	edm_i2c[1] = 1;
	edm_i2c[2] = 2;
        
	edm_ddc = 0;

	edm_spi[0] = 0;
	edm_spi[1] = 1;

	edm_audio_data[0].enabled = true;
	edm_audio_data[0].platform_data = &wand_audio_channel_data;
}


/*****************************************************************************
 *                                                                           
 * Revision detection
 *
 *****************************************************************************/

static void __init wand_detect_revision(void) {
	IMX6_SETUP_PAD( EIM_EB0__GPIO_2_28 ); /* boot cfg 4_3 */

	gpio_request(WAND_REV_DETECT_C1, "wand_rev_check");
	gpio_direction_input(WAND_REV_DETECT_C1);
	gpio_export(WAND_REV_DETECT_C1, 0);

	wand_revision = gpio_get_value(WAND_REV_DETECT_C1);
}

/*****************************************************************************
 *
 * Init clocks and early boot console                                      
 *                                                                            
 *****************************************************************************/

extern void __iomem *twd_base;

static void __init wand_init_timer(void) {
	struct clk *uart_clk;
#ifdef CONFIG_LOCAL_TIMERS
	twd_base = ioremap(LOCAL_TWD_ADDR, SZ_256);
#endif
	mx6_clocks_init(32768, 24000000, 0, 0);

	uart_clk = clk_get_sys("imx-uart.0", NULL);
	early_console_setup(UART1_BASE_ADDR, uart_clk);
}

/* ------------------------------------------------------------------------ */

static struct sys_timer wand_timer = {
	.init = wand_init_timer,
};

/* ------------------------------------------------------------------------ */

static void __init wand_reserve(void) {
	phys_addr_t phys;
	phys_addr_t total_mem = 0;
	struct meminfo *mi = &meminfo;
	int i;

	for (i=0; i<mi->nr_banks; i++)
		total_mem += mi->bank[i].size;
        
	if (wand_gpu_pdata.reserved_mem_size) {
		phys = memblock_alloc_base(wand_gpu_pdata.reserved_mem_size, SZ_4K, total_mem);
		memblock_remove(phys, wand_gpu_pdata.reserved_mem_size);
		wand_gpu_pdata.reserved_mem_base = phys;
	}

	if (wand_vout_mem.res_msize) {
		phys = memblock_alloc_base(wand_vout_mem.res_msize, SZ_4K, total_mem);
		memblock_remove(phys, wand_vout_mem.res_msize);
		wand_vout_mem.res_mbase = phys;
	}
}


/*****************************************************************************
 *                                                                           
 * BOARD INIT                                                                
 *                                                                            
 *****************************************************************************/

static void __init wand_board_init(void) {
	wand_detect_revision();
	wand_init_edm();
	wand_init_dma();
	wand_init_uart();
	wand_init_sd();
	wand_init_i2c();
	wand_init_audio();
	wand_init_ethernet();
	wand_init_usb();
	wand_init_ipu();
	wand_init_hdmi();
	wand_init_lcd();
	wand_init_wifi();
	wand_init_bluetooth();
	wand_init_pm();
	wand_init_external_gpios();
	wand_init_spi();
	wand_init_gpu();
	wand_init_caam();
	wand_init_pcie();
        if (cpu_is_mx6q())
                wand_init_sata();
}

/* ------------------------------------------------------------------------ */
        
MACHINE_START(WANDBOARD, "Wandboard")
	.boot_params	= MX6_PHYS_OFFSET + 0x100,
	.map_io		= mx6_map_io,
	.init_irq	= mx6_init_irq,
	.init_machine	= wand_board_init,
	.timer		= &wand_timer,
	.reserve        = wand_reserve,
MACHINE_END

