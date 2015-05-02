/*
    EDM-CF-iMX6 board file. 
    Copyright (C) 2012,2013 TechNexion Ltd.

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
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/setup.h>

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

/* ------------------------------------------------------------------------ */

#define EDM_CF_IMX6_BT_ON		IMX_GPIO_NR(5, 21)
#define EDM_CF_IMX6_BT_WAKE		IMX_GPIO_NR(5, 30)
#define EDM_CF_IMX6_BT_HOST_WAKE	IMX_GPIO_NR(5, 20)

#define EDM_CF_IMX6_CAN1_STBY		IMX_GPIO_NR(7, 13)
#define EDM_CF_IMX6_CAN2_STBY		IMX_GPIO_NR(1, 7)

#define EDM_CF_IMX6_EDM_VERSION		IMX_GPIO_NR(2, 28)

#define EDM_CF_IMX6_PCIE_NRST		IMX_GPIO_NR(3, 31)

#define EDM_CF_IMX6_RGMII_INT		IMX_GPIO_NR(1, 28)
#define EDM_CF_IMX6_RGMII_RST		IMX_GPIO_NR(3, 29)

#define EDM_CF_IMX6_SD1_CD		IMX_GPIO_NR(1, 2)
#define EDM_CF_IMX6_SD3_WP		IMX_GPIO_NR(1, 10)

#define EDM_CF_IMX6_USB_OTG_OC		IMX_GPIO_NR(1, 9)
#define EDM_CF_IMX6_USB_OTG_PWR		IMX_GPIO_NR(3, 22)
#define EDM_CF_IMX6_USB_H1_OC		IMX_GPIO_NR(3, 30)

#define EDM_CF_IMX6_WL_REF_ON		IMX_GPIO_NR(5, 31)
#define EDM_CF_IMX6_WL_RST_N		IMX_GPIO_NR(5, 2)
#define EDM_CF_IMX6_WL_REG_ON		IMX_GPIO_NR(1, 26)
#define EDM_CF_IMX6_WL_HOST_WAKE	IMX_GPIO_NR(1, 29)
#define EDM_CF_IMX6_WL_WAKE		IMX_GPIO_NR(1, 30)

/* Syntactic sugar for pad configuration */
#define EDM_IMX6_SET_PAD(p) \
        if (cpu_is_mx6q()) \
                mxc_iomux_v3_setup_pad(MX6Q_##p);\
        else \
                mxc_iomux_v3_setup_pad(MX6DL_##p)

/* See arch/arm/plat-mxc/include/mach/iomux-mx6q.h for definitions */

/****************************************************************************
 *                                                                          
 * DMA controller init
 *                                                                          
 ****************************************************************************/

static __init void edm_cf_imx6_init_dma(void) {
        imx6q_add_dma();        
}


/****************************************************************************
 *                                                                          
 * SD init
 *
 * SD1 is routed to EDM connector (external SD on edm_cf_imx6 baseboard)
 * SD2 is WiFi
 * SD3 is boot SD on the module
 *                                                                          
 ****************************************************************************/

static const struct esdhc_platform_data edm_cf_imx6_sd_data[3] = {
	{
		.cd_gpio		= EDM_CF_IMX6_SD1_CD,
		.wp_gpio		=-EINVAL,
		.keep_power_at_suspend	= 1,
	        .support_8bit		= 0,
	}, {
		.cd_gpio		=-EINVAL,
		.wp_gpio		=-EINVAL,
		.keep_power_at_suspend	= 1,
                .always_present 	= 1,
	}, {
		.cd_gpio		=-EINVAL,
		.wp_gpio		= EDM_CF_IMX6_SD3_WP,
		.keep_power_at_suspend	= 1,
                .always_present 	= 1,
                .support_18v		= 1,
		.support_8bit		= 1,
		.delay_line		= 0,
	}
};

/* ------------------------------------------------------------------------ */

static void edm_cf_imx6_init_sd(void) {
        int i;
        
	/* SD1 Card Detect for + SD1 pads */
        EDM_IMX6_SET_PAD( PAD_GPIO_2__GPIO_1_2 );
        EDM_IMX6_SET_PAD( PAD_SD1_CLK__USDHC1_CLK );
        EDM_IMX6_SET_PAD( PAD_SD1_CMD__USDHC1_CMD );
        EDM_IMX6_SET_PAD( PAD_SD1_DAT0__USDHC1_DAT0 );
        EDM_IMX6_SET_PAD( PAD_SD1_DAT1__USDHC1_DAT1 );
        EDM_IMX6_SET_PAD( PAD_SD1_DAT2__USDHC1_DAT2 );
        EDM_IMX6_SET_PAD( PAD_SD1_DAT3__USDHC1_DAT3 );
        
        /* SD2 */
        EDM_IMX6_SET_PAD( PAD_SD2_CLK__USDHC2_CLK );
        EDM_IMX6_SET_PAD( PAD_SD2_CMD__USDHC2_CMD );
        EDM_IMX6_SET_PAD( PAD_SD2_DAT0__USDHC2_DAT0 );
        EDM_IMX6_SET_PAD( PAD_SD2_DAT1__USDHC2_DAT1 );
        EDM_IMX6_SET_PAD( PAD_SD2_DAT2__USDHC2_DAT2 );
        EDM_IMX6_SET_PAD( PAD_SD2_DAT3__USDHC2_DAT3 );
        
        /* SD3 Card Detect + SD3 pads (iNAND uses 8 data signals) */
        EDM_IMX6_SET_PAD( PAD_SD3_CLK__USDHC3_CLK_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_CMD__USDHC3_CMD_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_DAT0__USDHC3_DAT0_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_DAT1__USDHC3_DAT1_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_DAT2__USDHC3_DAT2_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_DAT3__USDHC3_DAT3_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_DAT4__USDHC3_DAT4_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_DAT5__USDHC3_DAT5_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_DAT6__USDHC3_DAT6_200MHZ );
        EDM_IMX6_SET_PAD( PAD_SD3_DAT7__USDHC3_DAT7_200MHZ );
        
	/* Add mmc devices in reverse order, so mmc0 always is boot sd (SD3) */
	for (i=2; i>=0; i--) {
                imx6q_add_sdhci_usdhc_imx(i, &edm_cf_imx6_sd_data[i]);
	}
}


/****************************************************************************
 *                                                                          
 * I2C
 *                                                                          
 ****************************************************************************/

static struct imxi2c_platform_data edm_cf_imx6_i2c_data[] = {
	{ .bitrate	= 400000, },
	{ .bitrate	= 400000, },
	{ .bitrate	= 400000, },
};

/* ------------------------------------------------------------------------ */

static void __init edm_cf_imx6_init_i2c(void) {
        int i;
        
        EDM_IMX6_SET_PAD( PAD_EIM_D21__I2C1_SCL);
	EDM_IMX6_SET_PAD( PAD_EIM_D28__I2C1_SDA );

	EDM_IMX6_SET_PAD( PAD_KEY_COL3__I2C2_SCL );
	EDM_IMX6_SET_PAD( PAD_KEY_ROW3__I2C2_SDA );

	EDM_IMX6_SET_PAD( PAD_GPIO_5__I2C3_SCL );
	EDM_IMX6_SET_PAD( PAD_GPIO_16__I2C3_SDA );
        
	for (i=0; i<3; i++) {
		imx6q_add_imx_i2c(i, &edm_cf_imx6_i2c_data[i]);
        }
}


/****************************************************************************
 *                                                                          
 * Initialize UARTs
 *                                                                          
 ****************************************************************************/

static const struct imxuart_platform_data edm_cf_imx6_external_uart_data = {
	.flags = IMXUART_HAVE_RTSCTS,
	.dma_req_tx = MX6Q_DMA_REQ_UART2_TX,
	.dma_req_rx = MX6Q_DMA_REQ_UART2_RX,
};

/* ------------------------------------------------------------------------ */
 
static __init void edm_cf_imx6_init_uart(void) {
        /* UART 1 */
        EDM_IMX6_SET_PAD( PAD_CSI0_DAT10__UART1_TXD );
        EDM_IMX6_SET_PAD( PAD_CSI0_DAT11__UART1_RXD );
        EDM_IMX6_SET_PAD( PAD_EIM_D19__UART1_CTS );
        EDM_IMX6_SET_PAD( PAD_EIM_D20__UART1_RTS );

        /* UART 2 */
        EDM_IMX6_SET_PAD( PAD_SD4_DAT4__UART2_RXD );
        EDM_IMX6_SET_PAD( PAD_SD4_DAT5__UART2_RTS );
        EDM_IMX6_SET_PAD( PAD_SD4_DAT6__UART2_CTS );
        EDM_IMX6_SET_PAD( PAD_SD4_DAT7__UART2_TXD );

	imx6q_add_imx_uart(0, NULL);
        imx6q_add_imx_uart(1, &edm_cf_imx6_external_uart_data);
}


/****************************************************************************
 *                                                                          
 * Initialize sound (SSI, ASRC, AUD3 channel and S/PDIF)
 *                                                                          
 ****************************************************************************/

extern struct mxc_audio_platform_data edm_cf_imx6_audio_channel_data;

/* This function is called as a callback from the audio channel data struct */
static int edm_cf_imx6_audio_clock_enable(void) {
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
		pr_err("SGTL5000: mclk freq %d out of range!\n", rate);
		clk_put(clko);
		return -1;
	}

        edm_cf_imx6_audio_channel_data.sysclk = rate;
	clk_set_rate(clko, rate);
	clk_enable(clko);
        
	return 0;        
}

/* ------------------------------------------------------------------------ */

/* This struct is added by the baseboard when initializing the codec */
struct mxc_audio_platform_data edm_cf_imx6_audio_channel_data = {
	.ssi_num = 1,
	.src_port = 2,
	.ext_port = 3, /* audio channel: 3=AUD3. TODO: EDM */
	.init = edm_cf_imx6_audio_clock_enable,
	.hp_gpio = -1,
};
EXPORT_SYMBOL_GPL(edm_cf_imx6_audio_channel_data); /* TODO: edm naming? */

/* ------------------------------------------------------------------------ */

static int edm_cf_imx6_set_spdif_clk_rate(struct clk *clk, unsigned long rate) {
	unsigned long rate_actual;
	rate_actual = clk_round_rate(clk, rate);
	clk_set_rate(clk, rate_actual);
	return 0;
}

/* ------------------------------------------------------------------------ */

static struct mxc_spdif_platform_data edm_cf_imx6_spdif = {
	.spdif_tx		= 1,	/* enable tx */
	.spdif_rx		= 1,	/* enable rx */
	.spdif_clk_44100	= 1,    /* tx clk from spdif0_clk_root */
	.spdif_clk_48000	= 1,    /* tx clk from spdif0_clk_root */
	.spdif_div_44100	= 23,
	.spdif_div_48000	= 37,
	.spdif_div_32000	= 37,
	.spdif_rx_clk		= 0,    /* rx clk from spdif stream */
	.spdif_clk_set_rate	= edm_cf_imx6_set_spdif_clk_rate,
	.spdif_clk		= NULL, /* spdif bus clk */
};

/* ------------------------------------------------------------------------ */

static struct imx_ssi_platform_data edm_cf_imx6_ssi_pdata = {
	.flags = IMX_SSI_DMA | IMX_SSI_SYN,
};

/* ------------------------------------------------------------------------ */

static struct imx_asrc_platform_data edm_cf_imx6_asrc_data = {
	.channel_bits	= 4,
	.clk_map_ver	= 2,
};

/* ------------------------------------------------------------------------ */

void __init edm_cf_imx6_init_audio(void) {
        EDM_IMX6_SET_PAD( PAD_CSI0_DAT4__AUDMUX_AUD3_TXC );        
        EDM_IMX6_SET_PAD( PAD_CSI0_DAT5__AUDMUX_AUD3_TXD );
        EDM_IMX6_SET_PAD( PAD_CSI0_DAT6__AUDMUX_AUD3_TXFS );
        EDM_IMX6_SET_PAD( PAD_CSI0_DAT7__AUDMUX_AUD3_RXD );
        EDM_IMX6_SET_PAD( PAD_GPIO_0__CCM_CLKO );
        
        /* Sample rate converter is added together with audio */
        edm_cf_imx6_asrc_data.asrc_core_clk = clk_get(NULL, "asrc_clk");
        edm_cf_imx6_asrc_data.asrc_audio_clk = clk_get(NULL, "asrc_serial_clk");
	imx6q_add_asrc(&edm_cf_imx6_asrc_data);

	imx6q_add_imx_ssi(1, &edm_cf_imx6_ssi_pdata);

	/* Enable SPDIF */
        EDM_IMX6_SET_PAD( PAD_ENET_RXD0__SPDIF_OUT1 );

	edm_cf_imx6_spdif.spdif_core_clk = clk_get_sys("mxc_spdif.0", NULL);
	clk_put(edm_cf_imx6_spdif.spdif_core_clk);
	imx6q_add_spdif(&edm_cf_imx6_spdif);                
	imx6q_add_spdif_dai();
	imx6q_add_spdif_audio_device();
}


/*****************************************************************************
 *                                                                           
 * Init FEC and AR8031 PHY
 *                                                                            
 *****************************************************************************/

static int edm_cf_imx6_fec_phy_init(struct phy_device *phydev) {
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

static int edm_cf_imx6_fec_power_hibernate(struct phy_device *phydev) { return 0; }

/* ------------------------------------------------------------------------ */

static struct fec_platform_data edm_cf_imx6_fec_data = {
	.init			= edm_cf_imx6_fec_phy_init,
	.power_hibernate	= edm_cf_imx6_fec_power_hibernate,
	.phy			= PHY_INTERFACE_MODE_RGMII,
	.phy_noscan_mask	= ~2, /* phy is on adress 1 */
};

/* ------------------------------------------------------------------------ */

static __init void edm_cf_imx6_init_ethernet(void) {
        EDM_IMX6_SET_PAD( PAD_ENET_MDIO__ENET_MDIO );
        EDM_IMX6_SET_PAD( PAD_ENET_MDC__ENET_MDC );
        EDM_IMX6_SET_PAD( PAD_ENET_REF_CLK__ENET_TX_CLK );
	EDM_IMX6_SET_PAD( PAD_RGMII_TXC__ENET_RGMII_TXC );
	EDM_IMX6_SET_PAD( PAD_RGMII_TD0__ENET_RGMII_TD0 );
	EDM_IMX6_SET_PAD( PAD_RGMII_TD1__ENET_RGMII_TD1 );
	EDM_IMX6_SET_PAD( PAD_RGMII_TD2__ENET_RGMII_TD2 );
	EDM_IMX6_SET_PAD( PAD_RGMII_TD3__ENET_RGMII_TD3 );
	EDM_IMX6_SET_PAD( PAD_RGMII_TX_CTL__ENET_RGMII_TX_CTL );
	EDM_IMX6_SET_PAD( PAD_RGMII_RXC__ENET_RGMII_RXC );
	EDM_IMX6_SET_PAD( PAD_RGMII_RD0__ENET_RGMII_RD0 );
	EDM_IMX6_SET_PAD( PAD_RGMII_RD1__ENET_RGMII_RD1 );
	EDM_IMX6_SET_PAD( PAD_RGMII_RD2__ENET_RGMII_RD2 );
	EDM_IMX6_SET_PAD( PAD_RGMII_RD3__ENET_RGMII_RD3 );
	EDM_IMX6_SET_PAD( PAD_RGMII_RX_CTL__ENET_RGMII_RX_CTL );
        EDM_IMX6_SET_PAD( PAD_ENET_TX_EN__GPIO_1_28 );
        EDM_IMX6_SET_PAD( PAD_EIM_D29__GPIO_3_29 );
        
	gpio_request(EDM_CF_IMX6_RGMII_RST, "rgmii reset");
	gpio_direction_output(EDM_CF_IMX6_RGMII_RST, 0);
#ifdef CONFIG_FEC_1588
	mxc_iomux_set_gpr_register(1, 21, 1, 1);
#endif
	msleep(10);
	gpio_set_value(EDM_CF_IMX6_RGMII_RST, 1);
	imx6_init_fec(edm_cf_imx6_fec_data);
}


/****************************************************************************
 *                                                                          
 * USB
 *                                                                          
 ****************************************************************************/

static void edm_cf_imx6_usbotg_vbus(bool on) {
        gpio_set_value_cansleep(EDM_CF_IMX6_USB_OTG_PWR, !on);
}

/* ------------------------------------------------------------------------ */

static __init void edm_cf_imx6_init_usb(void) {
        EDM_IMX6_SET_PAD( PAD_GPIO_9__GPIO_1_9 );
        EDM_IMX6_SET_PAD( PAD_GPIO_1__USBOTG_ID );
        EDM_IMX6_SET_PAD( PAD_EIM_D22__GPIO_3_22 );
        EDM_IMX6_SET_PAD( PAD_EIM_D30__GPIO_3_30 );
        
        gpio_request(EDM_CF_IMX6_USB_OTG_OC, "otg oc");
	gpio_direction_input(EDM_CF_IMX6_USB_OTG_OC);

        gpio_request(EDM_CF_IMX6_USB_OTG_PWR, "otg pwr");
        gpio_direction_output(EDM_CF_IMX6_USB_OTG_PWR, 0);

	imx_otg_base = MX6_IO_ADDRESS(MX6Q_USB_OTG_BASE_ADDR);
	mxc_iomux_set_gpr_register(1, 13, 1, 1);

	mx6_set_otghost_vbus_func(edm_cf_imx6_usbotg_vbus);

        gpio_request(EDM_CF_IMX6_USB_H1_OC, "usbh1 oc");
	gpio_direction_input(EDM_CF_IMX6_USB_H1_OC);
}


/****************************************************************************
 *                                                                          
 * IPU
 *                                                                          
 ****************************************************************************/

static struct imx_ipuv3_platform_data edm_cf_imx6_ipu_data[] = {
	{
		.rev		= 4,
		.csi_clk[0]	= "ccm_clk0",
	}, {
		.rev		= 4,
		.csi_clk[0]	= "ccm_clk0",
	},
};

/* ------------------------------------------------------------------------ */

static __init void edm_cf_imx6_init_ipu(void) {
	imx6q_add_ipuv3(0, &edm_cf_imx6_ipu_data[0]);

        if (cpu_is_mx6q())
                imx6q_add_ipuv3(1, &edm_cf_imx6_ipu_data[1]);
}


/****************************************************************************
 *                                                                          
 * MX6 DISPLAY CONTROL FRAMEWORK
 *                                                                          
 ****************************************************************************/

#include "mx6_display.h"

static void edm_cf_imx6_display0_control(int onoff)
{
	if (onoff) {
		gpio_direction_output(IMX_GPIO_NR(2, 8), 1);
		gpio_direction_output(IMX_GPIO_NR(2, 9), 1);
	} else {
		gpio_direction_output(IMX_GPIO_NR(2, 8), 0);
		gpio_direction_output(IMX_GPIO_NR(2, 9), 0);
	}

}
static void edm_cf_imx6_display1_control(int onoff)
{
	if (onoff) {
		gpio_direction_output(IMX_GPIO_NR(2, 10), 1);
		gpio_direction_output(IMX_GPIO_NR(2, 11), 1);
	} else {
		gpio_direction_output(IMX_GPIO_NR(2, 10), 0);
		gpio_direction_output(IMX_GPIO_NR(2, 11), 0);
	}
}

static void __init edm_cf_imx6_pads_lvds(void)
{
	/* LVDS 0 */
	EDM_IMX6_SET_PAD( PAD_LVDS0_CLK_P__LDB_LVDS0_CLK );
	EDM_IMX6_SET_PAD( PAD_LVDS0_TX0_P__LDB_LVDS0_TX0 );
	EDM_IMX6_SET_PAD( PAD_LVDS0_TX1_P__LDB_LVDS0_TX1 );
	EDM_IMX6_SET_PAD( PAD_LVDS0_TX2_P__LDB_LVDS0_TX2 );
	EDM_IMX6_SET_PAD( PAD_LVDS0_TX3_P__LDB_LVDS0_TX3 );
	/* LVDS 1 */
	EDM_IMX6_SET_PAD( PAD_LVDS1_CLK_P__LDB_LVDS1_CLK );
	EDM_IMX6_SET_PAD( PAD_LVDS1_TX0_P__LDB_LVDS1_TX0 );
	EDM_IMX6_SET_PAD( PAD_LVDS1_TX1_P__LDB_LVDS1_TX1 );
	EDM_IMX6_SET_PAD( PAD_LVDS1_TX2_P__LDB_LVDS1_TX2 );
	EDM_IMX6_SET_PAD( PAD_LVDS1_TX3_P__LDB_LVDS1_TX3 );
}

static void __init edm_cf_imx6_pads_ipu1_lcd(void)
{
	/* LCD0 on IPU1 */
	EDM_IMX6_SET_PAD( PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK );
	EDM_IMX6_SET_PAD( PAD_DI0_PIN2__IPU1_DI0_PIN2 );		/* HSync */
	EDM_IMX6_SET_PAD( PAD_DI0_PIN3__IPU1_DI0_PIN3 );		/* VSync */
	EDM_IMX6_SET_PAD( PAD_DI0_PIN4__IPU1_DI0_PIN4 );		/* Contrast */
	EDM_IMX6_SET_PAD( PAD_DI0_PIN15__IPU1_DI0_PIN15);		/* DISP0_DRDY */
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT0__IPU1_DISP0_DAT_0 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT1__IPU1_DISP0_DAT_1 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT2__IPU1_DISP0_DAT_2 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT3__IPU1_DISP0_DAT_3 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT4__IPU1_DISP0_DAT_4 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT5__IPU1_DISP0_DAT_5 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT6__IPU1_DISP0_DAT_6 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT7__IPU1_DISP0_DAT_7 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT8__IPU1_DISP0_DAT_8 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT9__IPU1_DISP0_DAT_9 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT10__IPU1_DISP0_DAT_10 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT11__IPU1_DISP0_DAT_11 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT12__IPU1_DISP0_DAT_12 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT13__IPU1_DISP0_DAT_13 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT14__IPU1_DISP0_DAT_14 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT15__IPU1_DISP0_DAT_15 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT16__IPU1_DISP0_DAT_16 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT17__IPU1_DISP0_DAT_17 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT18__IPU1_DISP0_DAT_18 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT19__IPU1_DISP0_DAT_19 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT20__IPU1_DISP0_DAT_20 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT21__IPU1_DISP0_DAT_21 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT22__IPU1_DISP0_DAT_22 );
	EDM_IMX6_SET_PAD( PAD_DISP0_DAT23__IPU1_DISP0_DAT_23 );
}

static void __init edm_cf_imx6_pads_ipu2_lcd(void)
{
	if (!cpu_is_mx6q())
		return;
	/* LCD0 on IPU2 */
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DI0_DISP_CLK__IPU2_DI0_DISP_CLK);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DI0_PIN2__IPU2_DI0_PIN2);		// HSync 
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DI0_PIN3__IPU2_DI0_PIN3);		// VSync 
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DI0_PIN4__IPU2_DI0_PIN4);		// Contrast 
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DI0_PIN15__IPU2_DI0_PIN15);		// DISP0_DRDY 
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT0__IPU2_DISP0_DAT_0);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT1__IPU2_DISP0_DAT_1);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT2__IPU2_DISP0_DAT_2);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT3__IPU2_DISP0_DAT_3);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT4__IPU2_DISP0_DAT_4);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT5__IPU2_DISP0_DAT_5);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT6__IPU2_DISP0_DAT_6);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT7__IPU2_DISP0_DAT_7);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT8__IPU2_DISP0_DAT_8);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT9__IPU2_DISP0_DAT_9);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT10__IPU2_DISP0_DAT_10);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT11__IPU2_DISP0_DAT_11);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT12__IPU2_DISP0_DAT_12);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT13__IPU2_DISP0_DAT_13);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT14__IPU2_DISP0_DAT_14);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT15__IPU2_DISP0_DAT_15);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT16__IPU2_DISP0_DAT_16);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT17__IPU2_DISP0_DAT_17);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT18__IPU2_DISP0_DAT_18);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT19__IPU2_DISP0_DAT_19);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT20__IPU2_DISP0_DAT_20);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT21__IPU2_DISP0_DAT_21);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT22__IPU2_DISP0_DAT_22);
	mxc_iomux_v3_setup_pad(MX6Q_PAD_DISP0_DAT23__IPU2_DISP0_DAT_23);
}

static void edm_cf_imx6_init_display(void)
{
	EDM_IMX6_SET_PAD( PAD_SD4_DAT0__GPIO_2_8 );
	EDM_IMX6_SET_PAD( PAD_SD4_DAT1__GPIO_2_9 );
	EDM_IMX6_SET_PAD( PAD_SD4_DAT2__GPIO_2_10 );
	EDM_IMX6_SET_PAD( PAD_SD4_DAT3__GPIO_2_11 );

	gpio_request(IMX_GPIO_NR(2, 8), "lvds0_en");
	gpio_request(IMX_GPIO_NR(2, 9), "lvds0_blt_ctrl");

	gpio_request(IMX_GPIO_NR(2, 10), "disp0_bklen");
	gpio_request(IMX_GPIO_NR(2, 11), "disp0_vdden");

	mx6_disp_ctrls = kmalloc(sizeof(struct mx6_display_controls), GFP_KERNEL);
	if (mx6_disp_ctrls != NULL) {
		mx6_disp_ctrls->hdmi_enable 	= NULL;
		mx6_disp_ctrls->lvds0_enable 	= edm_cf_imx6_display0_control;
		mx6_disp_ctrls->lvds1_enable 	= edm_cf_imx6_display1_control;
		mx6_disp_ctrls->lcd0_enable 	= edm_cf_imx6_display1_control;
		mx6_disp_ctrls->lcd1_enable 	= NULL;
		mx6_disp_ctrls->dsi_enable 	= NULL;
		mx6_disp_ctrls->hdmi_pads 	= NULL;
		mx6_disp_ctrls->lvds0_pads 	= edm_cf_imx6_pads_lvds;
		mx6_disp_ctrls->lvds1_pads 	= edm_cf_imx6_pads_lvds;
		mx6_disp_ctrls->lcd0_ipu1_pads 	= edm_cf_imx6_pads_ipu1_lcd;
		mx6_disp_ctrls->lcd0_ipu2_pads 	= edm_cf_imx6_pads_ipu2_lcd;
		mx6_disp_ctrls->lcd1_ipu1_pads 	= NULL;
		mx6_disp_ctrls->lcd1_ipu2_pads 	= NULL;
		mx6_disp_ctrls->dsi_pads 	= NULL;
		mx6_disp_ctrls->hdmi_ddc_pads_enable 	= NULL;
		mx6_disp_ctrls->hdmi_ddc_pads_disable 	= NULL;
		mx6_disp_ctrls->hdmi_i2c	= edm_ddc;
		mx6_disp_ctrls->hdcp_enable	= 0;
		mx6_disp_ctrls->lvds0_i2c	= -EINVAL;
		mx6_disp_ctrls->lvds1_i2c	= -EINVAL;
		mx6_disp_ctrls->lcd0_i2c	= -EINVAL;
		mx6_disp_ctrls->lcd1_i2c	= -EINVAL;
		mx6_disp_ctrls->dsi_i2c		= -EINVAL;
	}

/* For EDM2 , the following are supported: hdmi0, lvds0, lvds1, lvdsd, dsi */
	if (edm_version == 2)
		mx6_display_ch_capability_setup(0, 0, 1, 1, 1, 1, 1);
	else
/* For EDM1 , the following are supported: lcd0, hdmi0, lvds0, dsi */
		mx6_display_ch_capability_setup(1, 0, 1, 1, 0, 0, 1);

	mx6_init_display();
}


/****************************************************************************
 *                                                                          
 * WiFi
 *                                                                          
 ****************************************************************************/

/* assumes SD/MMC pins are set; call after edm_cf_imx6_init_sd() */
static __init void edm_cf_imx6_init_wifi(void) {
        /* wifi on*/
        EDM_IMX6_SET_PAD( PAD_EIM_A25__GPIO_5_2 );
        /* reg on, signal to FDC6331L */
        EDM_IMX6_SET_PAD( PAD_ENET_RXD1__GPIO_1_26 );        
        /* ref_on, enable 32k clock */
        EDM_IMX6_SET_PAD( PAD_CSI0_DAT13__GPIO_5_31 );
        /* host wake */
        EDM_IMX6_SET_PAD( PAD_ENET_TXD1__GPIO_1_29 );
        /* wl wake - nc */
        EDM_IMX6_SET_PAD( PAD_ENET_TXD0__GPIO_1_30 );

        
	gpio_request(EDM_CF_IMX6_WL_RST_N, "wl_rst_n");
	gpio_direction_output(EDM_CF_IMX6_WL_RST_N, 0);
	msleep(11);
	gpio_set_value(EDM_CF_IMX6_WL_RST_N, 1);

	gpio_request(EDM_CF_IMX6_WL_REF_ON, "wl_ref_on");
	gpio_direction_output(EDM_CF_IMX6_WL_REF_ON, 1);

	gpio_request(EDM_CF_IMX6_WL_REG_ON, "wl_reg_on");
	gpio_direction_output(EDM_CF_IMX6_WL_REG_ON, 1);
        
	gpio_request(EDM_CF_IMX6_WL_WAKE, "wl_wake");
	gpio_direction_output(EDM_CF_IMX6_WL_WAKE, 1);

	gpio_request(EDM_CF_IMX6_WL_HOST_WAKE, "wl_host_wake");
	gpio_direction_input(EDM_CF_IMX6_WL_HOST_WAKE);
}


/****************************************************************************
 *                                                                          
 * Bluetooth
 *                                                                          
 ****************************************************************************/

static const struct imxuart_platform_data edm_cf_imx6_bt_uart_data = {
	.flags = IMXUART_HAVE_RTSCTS,
	.dma_req_tx = MX6Q_DMA_REQ_UART3_TX,
	.dma_req_rx = MX6Q_DMA_REQ_UART3_RX,
};

/* ------------------------------------------------------------------------ */

/* This assumes wifi is initialized (chip has power) */
static __init void edm_cf_imx6_init_bluetooth(void) {
        /* AUD5 channel goes to BT */
        EDM_IMX6_SET_PAD( PAD_KEY_COL0__AUDMUX_AUD5_TXC );
        EDM_IMX6_SET_PAD( PAD_KEY_ROW0__AUDMUX_AUD5_TXD );
        EDM_IMX6_SET_PAD( PAD_KEY_COL1__AUDMUX_AUD5_TXFS );
        EDM_IMX6_SET_PAD( PAD_KEY_ROW1__AUDMUX_AUD5_RXD );
        
        /* Bluetooth is on UART3*/
        EDM_IMX6_SET_PAD( PAD_EIM_D23__UART3_CTS );
        EDM_IMX6_SET_PAD( PAD_EIM_D24__UART3_TXD );
        EDM_IMX6_SET_PAD( PAD_EIM_D25__UART3_RXD );
        EDM_IMX6_SET_PAD( PAD_EIM_EB3__UART3_RTS );

        /* BT_ON, BT_WAKE and BT_HOST_WAKE */
        EDM_IMX6_SET_PAD( PAD_CSI0_DAT12__GPIO_5_30 );
        EDM_IMX6_SET_PAD( PAD_CSI0_DATA_EN__GPIO_5_20 );
        EDM_IMX6_SET_PAD( PAD_CSI0_VSYNC__GPIO_5_21 );
	imx6q_add_imx_uart(2, &edm_cf_imx6_bt_uart_data);

	gpio_request(EDM_CF_IMX6_BT_ON, "bt_on");
	gpio_direction_output(EDM_CF_IMX6_BT_ON, 0);
	msleep(11);
	gpio_set_value(EDM_CF_IMX6_BT_ON, 1);
        gpio_export(EDM_CF_IMX6_BT_ON, true);

	gpio_request(EDM_CF_IMX6_BT_WAKE, "bt_wake");
	gpio_direction_output(EDM_CF_IMX6_BT_WAKE, 1);
        gpio_export(EDM_CF_IMX6_BT_WAKE, true);

	gpio_request(EDM_CF_IMX6_BT_HOST_WAKE, "bt_host_wake");
	gpio_direction_input(EDM_CF_IMX6_BT_WAKE);
}


/****************************************************************************
 *                                                                          
 * Power and thermal management
 *                                                                          
 ****************************************************************************/

extern bool enable_wait_mode;

static const struct anatop_thermal_platform_data edm_cf_imx6_thermal = {
	.name = "anatop_thermal",
};

/* ------------------------------------------------------------------------ */

static void edm_cf_imx6_suspend_enter(void) {
	gpio_set_value(EDM_CF_IMX6_WL_WAKE, 0);
	gpio_set_value(EDM_CF_IMX6_BT_WAKE, 0);
}

/* ------------------------------------------------------------------------ */

static void edm_cf_imx6_suspend_exit(void) {
	gpio_set_value(EDM_CF_IMX6_WL_WAKE, 1);
	gpio_set_value(EDM_CF_IMX6_BT_WAKE, 1);
}

/* ------------------------------------------------------------------------ */

static const struct pm_platform_data edm_cf_imx6_pm_data = {
	.name		= "imx_pm",
	.suspend_enter	= edm_cf_imx6_suspend_enter,
	.suspend_exit	= edm_cf_imx6_suspend_exit,
};

/* ------------------------------------------------------------------------ */

static const struct mxc_dvfs_platform_data edm_cf_imx6_dvfscore_data = {
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

static void edm_cf_imx6_poweroff(void) {
	void __iomem *snvs_lpcr = MX6_IO_ADDRESS(MX6Q_SNVS_BASE_ADDR) + 0x38;
	u32 old_lpcr = readl(snvs_lpcr);
	writel(old_lpcr | 0x60, snvs_lpcr);
}

/* ------------------------------------------------------------------------ */

static __init void edm_cf_imx6_init_pm(void) {
	int value;
	enable_wait_mode = false;
	imx6q_add_anatop_thermal_imx(1, &edm_cf_imx6_thermal);
	imx6q_add_pm_imx(0, &edm_cf_imx6_pm_data);
	imx6q_add_dvfs_core(&edm_cf_imx6_dvfscore_data);
	imx6q_add_busfreq();
        pm_power_off = edm_cf_imx6_poweroff;
}


/****************************************************************************
 *                                                                          
 * Expansion pin header GPIOs
 *                                                                          
 ****************************************************************************/

static __init void edm_cf_imx6_init_external_gpios(void) {
        int i;
	char *name = "external_gpio_0";
        
        /* pins 255 ... 264 in order */
        EDM_IMX6_SET_PAD(PAD_CSI0_DAT19__GPIO_6_5 | PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_CSI0_DAT16__GPIO_6_2 | PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_CSI0_DAT18__GPIO_6_4 | PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_EIM_D27__GPIO_3_27 | PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_EIM_D26__GPIO_3_26 | PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_EIM_BCLK__GPIO_6_31 | PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_CSI0_DAT17__GPIO_6_3 | PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_ENET_RX_ER__GPIO_1_24| PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_GPIO_19__GPIO_4_5 | PAD_CTL_PUS_100K_DOWN );
        EDM_IMX6_SET_PAD(PAD_SD3_RST__GPIO_7_8| PAD_CTL_PUS_100K_DOWN );
        
        for (i=0; i<EDM_N_EXTERNAL_GPIO; i++) {
                name[14] = '0' + i;                
                gpio_request(edm_external_gpio[i], name);
                gpio_export(edm_external_gpio[i], true);
        }
}


/****************************************************************************
 *                                                                          
 * SPI
 *                                                                          
 ****************************************************************************/

static const int edm_cf_imx6_spi1_chipselect[] = { IMX_GPIO_NR(2, 30) };

/* platform device */
static const struct spi_imx_master edm_cf_imx6_spi1_data = {
	.chipselect     = (int *)edm_cf_imx6_spi1_chipselect,
	.num_chipselect = ARRAY_SIZE(edm_cf_imx6_spi1_chipselect),
};

/* ------------------------------------------------------------------------ */

static const int edm_cf_imx6_spi2_chipselect[] = { IMX_GPIO_NR(2, 26), IMX_GPIO_NR(2, 27) };

static const struct spi_imx_master edm_cf_imx6_spi2_data = {
	.chipselect     = (int *)edm_cf_imx6_spi2_chipselect,
	.num_chipselect = ARRAY_SIZE(edm_cf_imx6_spi2_chipselect),
};

/* ------------------------------------------------------------------------ */

/* The choice of using gpios for chipselect is deliberate,
   there can be issues using the dedicated mux modes for cs.
*/

static void __init edm_cf_imx6_init_spi(void) {
	EDM_IMX6_SET_PAD( PAD_EIM_D16__ECSPI1_SCLK );
	EDM_IMX6_SET_PAD( PAD_EIM_D17__ECSPI1_MISO );
	EDM_IMX6_SET_PAD( PAD_EIM_D18__ECSPI1_MOSI );
	EDM_IMX6_SET_PAD( PAD_EIM_EB2__GPIO_2_30 );

	EDM_IMX6_SET_PAD( PAD_EIM_CS0__ECSPI2_SCLK );
	EDM_IMX6_SET_PAD( PAD_EIM_CS1__ECSPI2_MOSI );
	EDM_IMX6_SET_PAD( PAD_EIM_OE__ECSPI2_MISO );
	EDM_IMX6_SET_PAD( PAD_EIM_RW__GPIO_2_26 );
	EDM_IMX6_SET_PAD( PAD_EIM_LBA__GPIO_2_27 );
        
	imx6q_add_ecspi(0, &edm_cf_imx6_spi1_data);
	imx6q_add_ecspi(1, &edm_cf_imx6_spi2_data);
}

/****************************************************************************
 *                                                                          
 * Vivante GPU/VPU
 *                                                                          
 ****************************************************************************/

static const __initconst struct imx_viv_gpu_data edm_cf_imx6_gpu_data = {
	.phys_baseaddr = 0,
	.iobase_3d = GPU_3D_ARB_BASE_ADDR,
	.irq_3d = MXC_INT_GPU3D_IRQ,
	.iobase_2d = GPU_2D_ARB_BASE_ADDR,
	.irq_2d = MXC_INT_GPU2D_IRQ,
	.iobase_vg = OPENVG_ARB_BASE_ADDR,
	.irq_vg = MXC_INT_OPENVG_XAQ2,
};

/* ------------------------------------------------------------------------ */

static struct viv_gpu_platform_data edm_cf_imx6_gpu_pdata = {
	.reserved_mem_size = SZ_64M,
};

/* ------------------------------------------------------------------------ */

struct edm_cf_imx6_vout_mem_data {
	resource_size_t res_mbase;
	resource_size_t res_msize;
};

/* ------------------------------------------------------------------------ */

static struct edm_cf_imx6_vout_mem_data edm_cf_imx6_vout_mem __initdata = {
	.res_msize = 96 * SZ_1M,
};

/* ------------------------------------------------------------------------ */

static __init void edm_cf_imx6_init_gpu(void) {
	imx_add_viv_gpu(&edm_cf_imx6_gpu_data, &edm_cf_imx6_gpu_pdata);
        imx6q_add_vpu();
        imx6q_add_v4l2_output(0);
}


/*****************************************************************************
 *                                                                           
 * CAN bus
 *                                                                            
 *****************************************************************************/

static void edm_cf_imx6_flexcan0_switch(int enable) {
        gpio_set_value(EDM_CF_IMX6_CAN1_STBY, !enable);
}

/* ------------------------------------------------------------------------ */

static void edm_cf_imx6_flexcan1_switch(int enable) {
        gpio_set_value(EDM_CF_IMX6_CAN2_STBY, !enable);
}

/* ------------------------------------------------------------------------ */

static const struct flexcan_platform_data edm_cf_imx6_flexcan0_pdata = {
	.transceiver_switch = edm_cf_imx6_flexcan0_switch,        
};

/* ------------------------------------------------------------------------ */

static const struct flexcan_platform_data edm_cf_imx6_flexcan1_pdata = {
	.transceiver_switch = edm_cf_imx6_flexcan1_switch,
};

/* ------------------------------------------------------------------------ */

void __init edm_cf_imx6_init_can(void) {

        EDM_IMX6_SET_PAD( PAD_GPIO_18__GPIO_7_13 ); /* CAN1_STBY */
        EDM_IMX6_SET_PAD( PAD_KEY_COL2__CAN1_TXCAN );
        EDM_IMX6_SET_PAD( PAD_KEY_ROW2__CAN1_RXCAN );

        EDM_IMX6_SET_PAD( PAD_GPIO_7__GPIO_1_7 ); /* CAN2_STBY */
        EDM_IMX6_SET_PAD( PAD_KEY_COL4__CAN2_TXCAN );
        EDM_IMX6_SET_PAD( PAD_KEY_ROW4__CAN2_RXCAN );
        
        gpio_request(EDM_CF_IMX6_CAN1_STBY, "can1_stby");
        gpio_direction_output(EDM_CF_IMX6_CAN1_STBY, 0);

        gpio_request(EDM_CF_IMX6_CAN2_STBY, "can2_stby");
        gpio_direction_output(EDM_CF_IMX6_CAN2_STBY, 0);
        
        imx6q_add_flexcan0(&edm_cf_imx6_flexcan0_pdata);
        imx6q_add_flexcan1(&edm_cf_imx6_flexcan1_pdata);
}


/*****************************************************************************
 *                                                                           
 * PCIe
 *                                                                            
 *****************************************************************************/

static const struct imx_pcie_platform_data edm_cf_imx6_pcie_data = {
	.pcie_pwr_en	= -EINVAL,
	.pcie_rst	= EDM_CF_IMX6_PCIE_NRST,
	.pcie_wake_up	= -EINVAL,
	.pcie_dis	= -EINVAL,
};

/* ------------------------------------------------------------------------ */

static void __init edm_cf_imx6_init_pcie(void) {
        if (cpu_is_mx6q()) mxc_iomux_v3_setup_pad(MX6Q_PAD_EIM_D31__GPIO_3_31);
        else mxc_iomux_v3_setup_pad(MX6DL_PAD_EIM_D31__GPIO_3_31);
        imx6q_add_pcie(&edm_cf_imx6_pcie_data);
}


/****************************************************************************
 *                                                                          
 * AHCI - SATA
 *                                                                          
 ****************************************************************************/

static struct clk *edm_cf_imx6_sata_clk;

/* HW Initialization, if return 0, initialization is successful. */
static int edm_cf_imx6_sata_init(struct device *dev, void __iomem *addr) {
	u32 tmpdata;
	int ret = 0;
	struct clk *clk;

        edm_cf_imx6_sata_clk = clk_get(dev, "imx_sata_clk");
	if (IS_ERR(edm_cf_imx6_sata_clk)) {
		dev_err(dev, "no sata clock.\n");
		return PTR_ERR(edm_cf_imx6_sata_clk);
	}
	ret = clk_enable(edm_cf_imx6_sata_clk);
	if (ret) {
		dev_err(dev, "can't enable sata clock.\n");
		goto put_sata_clk;
	}

	tmpdata = readl(IOMUXC_GPR13);
	writel(((tmpdata & ~0x07FFFFFD) | 0x0593A044), IOMUXC_GPR13);

	/* enable SATA_PHY PLL */
	tmpdata = readl(IOMUXC_GPR13);
	writel((tmpdata | 0x2), IOMUXC_GPR13);

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

	/* SATA init failed */
release_sata_clk:
	writel((readl(IOMUXC_GPR13) & ~0x2), IOMUXC_GPR13);
	clk_disable(edm_cf_imx6_sata_clk);
put_sata_clk:
	clk_put(edm_cf_imx6_sata_clk);

	return ret;
}

/* ------------------------------------------------------------------------ */

static void edm_cf_imx6_sata_exit(struct device *dev) {
	clk_disable(edm_cf_imx6_sata_clk);
	clk_put(edm_cf_imx6_sata_clk);
}

/* ------------------------------------------------------------------------ */

static struct ahci_platform_data edm_cf_imx6_sata_data = {
	.init = edm_cf_imx6_sata_init,
	.exit = edm_cf_imx6_sata_exit,
};


/* ------------------------------------------------------------------------ */

static __init void edm_cf_imx6_init_sata(void) {
        imx6q_add_ahci(0, &edm_cf_imx6_sata_data);
}


/*****************************************************************************
 *                                                                           
 * Init EDM framework                                      
 *                                                                            
 *****************************************************************************/

int edm_cf_imx6_detect_edm_version(void) {
	int ret;
	EDM_IMX6_SET_PAD( PAD_EIM_EB0__GPIO_2_28 );

	ret = gpio_request(EDM_CF_IMX6_EDM_VERSION, "edm_version" );
	if ( !ret ) {
		gpio_direction_input(EDM_CF_IMX6_EDM_VERSION);
                ret = gpio_get_value(EDM_CF_IMX6_EDM_VERSION);
		gpio_free(EDM_CF_IMX6_EDM_VERSION);
	}
	return ret;
}

/* ------------------------------------------------------------------------ */

static void __init edm_cf_imx6_init_edm(void) {
	edm_version = 1 + edm_cf_imx6_detect_edm_version();

	edm_external_gpio[0] = IMX_GPIO_NR(6, 5); /* P255 */
	edm_external_gpio[1] = IMX_GPIO_NR(6, 2); /* P256 */
	edm_external_gpio[2] = IMX_GPIO_NR(6, 4); /* P257 */
	edm_external_gpio[3] = IMX_GPIO_NR(3, 27); /* P258 */
	edm_external_gpio[4] = IMX_GPIO_NR(3, 26); /* P259 */
	edm_external_gpio[5] = IMX_GPIO_NR(6, 31); /* P260 */
	edm_external_gpio[6] = IMX_GPIO_NR(6, 3);  /* P261 */
	edm_external_gpio[7] = IMX_GPIO_NR(1, 24); /* P262 */
	edm_external_gpio[8] = IMX_GPIO_NR(4, 5);  /* P263 */
	edm_external_gpio[9] = IMX_GPIO_NR(7, 8);  /* P264 */

        edm_i2c[0] = 0;
        edm_i2c[1] = 1;
        edm_i2c[2] = 2;
        edm_ddc = 0;

	edm_audio_data[0].enabled = true;
	edm_audio_data[0].platform_data = &edm_cf_imx6_audio_channel_data;
}



/*****************************************************************************
 *                                                                           
 * Init clocks and early boot console                                      
 *                                                                            
 *****************************************************************************/

extern void __iomem *twd_base;

static void __init edm_cf_imx6_init_timer(void) {
	struct clk *uart_clk;
#ifdef CONFIG_LOCAL_TIMERS
	twd_base = ioremap(LOCAL_TWD_ADDR, SZ_256);
#endif
	mx6_clocks_init(32768, 24000000, 0, 0);

	uart_clk = clk_get_sys("imx-uart.0", NULL);
	early_console_setup(UART1_BASE_ADDR, uart_clk);
}

/* ------------------------------------------------------------------------ */

static struct sys_timer edm_cf_imx6_timer = {
	.init = edm_cf_imx6_init_timer,
};

/* ------------------------------------------------------------------------ */

static void __init edm_cf_imx6_reserve(void) {
	phys_addr_t phys;
	phys_addr_t total_mem = 0;
	struct meminfo *mi = &meminfo;
	int i;

	for (i=0; i<mi->nr_banks; i++)
		total_mem += mi->bank[i].size;

	if (edm_cf_imx6_gpu_pdata.reserved_mem_size) {
		phys = memblock_alloc_base(edm_cf_imx6_gpu_pdata.reserved_mem_size, SZ_4K, total_mem);
		memblock_remove(phys, edm_cf_imx6_gpu_pdata.reserved_mem_size);
		edm_cf_imx6_gpu_pdata.reserved_mem_base = phys;
	}

	if (edm_cf_imx6_vout_mem.res_msize) {
		phys = memblock_alloc_base(edm_cf_imx6_vout_mem.res_msize, SZ_4K, total_mem);
		memblock_remove(phys, edm_cf_imx6_vout_mem.res_msize);
                edm_cf_imx6_vout_mem.res_mbase = phys;
	}
};


/*****************************************************************************
 *                                                                           
 * BOARD INIT                                                                
 *                                                                            
 *****************************************************************************/

static void __init edm_cf_imx6_board_init(void) {
	edm_cf_imx6_init_edm();
	edm_cf_imx6_init_dma();
	edm_cf_imx6_init_uart();
	edm_cf_imx6_init_sd();
        edm_cf_imx6_init_usb();
	edm_cf_imx6_init_pm();
        
	edm_cf_imx6_init_i2c();
	edm_cf_imx6_init_audio();
	edm_cf_imx6_init_ethernet();

	edm_cf_imx6_init_ipu();
	edm_cf_imx6_init_display();

	edm_cf_imx6_init_wifi();
	edm_cf_imx6_init_bluetooth();
	edm_cf_imx6_init_external_gpios();

	edm_cf_imx6_init_spi();
	edm_cf_imx6_init_gpu();

        if (cpu_is_mx6q())
                edm_cf_imx6_init_sata();

        edm_cf_imx6_init_can();
        edm_cf_imx6_init_pcie();
}

/* ------------------------------------------------------------------------ */
        
MACHINE_START(EDM_CF_IMX6, "edm_cf_imx6")
	.boot_params	= MX6_PHYS_OFFSET + 0x100,
	.map_io		= mx6_map_io,
	.init_irq	= mx6_init_irq,
	.init_machine	= edm_cf_imx6_board_init,
	.timer		= &edm_cf_imx6_timer,
	.reserve        = edm_cf_imx6_reserve,
MACHINE_END

