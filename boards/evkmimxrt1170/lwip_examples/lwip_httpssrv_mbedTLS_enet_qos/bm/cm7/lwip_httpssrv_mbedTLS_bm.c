/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lwip/opt.h"

#if LWIP_TCP

#include "lwip/apps/httpd.h"
#include "lwip/timeouts.h"
#include "lwip/init.h"
#include "netif/ethernet.h"
#include "enet_ethernetif.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_phy.h"

#include "ksdk_mbedtls.h"
#include "httpd_mbedtls.h"

#include "mbedtls/entropy.h"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_cache.h"
#include "mbedtls/debug.h"

#include "fsl_phyrtl8211f.h"
#include "fsl_enet_qos_mdio.h"
#include "fsl_enet_qos.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* IP address configuration. */
#define configIP_ADDR0 192
#define configIP_ADDR1 168
#define configIP_ADDR2 0
#define configIP_ADDR3 102

/* Netmask configuration. */
#define configNET_MASK0 255
#define configNET_MASK1 255
#define configNET_MASK2 255
#define configNET_MASK3 0

/* Gateway address configuration. */
#define configGW_ADDR0 192
#define configGW_ADDR1 168
#define configGW_ADDR2 0
#define configGW_ADDR3 100

/* MAC address configuration. */
#define configMAC_ADDR                     \
    {                                      \
        0x02, 0x12, 0x13, 0x10, 0x15, 0x11 \
    }

/* Address of PHY interface. */
#define EXAMPLE_PHY_ADDRESS 0x01U

/* MDIO operations. */
#define EXAMPLE_MDIO_OPS enet_qos_ops

/* PHY operations. */
#define EXAMPLE_PHY_OPS phyrtl8211f_ops

/* ENET clock frequency. */
#define EXAMPLE_CLOCK_FREQ CLOCK_GetRootClockFreq(kCLOCK_Root_Bus)


#ifndef EXAMPLE_NETIF_INIT_FN
/*! @brief Network interface initialization function. */
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif /* EXAMPLE_NETIF_INIT_FN */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static mdio_handle_t mdioHandle = {.ops = &EXAMPLE_MDIO_OPS};
static phy_handle_t phyHandle   = {.phyAddr = EXAMPLE_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_PHY_OPS};

const char *pers = "ssl_server";
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_ssl_context ssl;
mbedtls_ssl_config conf;
mbedtls_x509_crt srvcert;
mbedtls_pk_context pkey;
#if defined(MBEDTLS_SSL_CACHE_C)
mbedtls_ssl_cache_context cache;
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_InitModuleClock(void)
{
    /* Select syspll2pfd3, 528*18/24 = 396M */
    CLOCK_InitPfd(kCLOCK_PllSys2, kCLOCK_Pfd3, 24);
    const clock_sys_pll1_config_t sysPll1Config = {
        .pllDiv2En = true,
    };
    CLOCK_InitSysPll1(&sysPll1Config);
    clock_root_config_t rootCfg = {.mux = 4, .div = 4}; /* Generate 125M root clock. */
    CLOCK_SetRootClock(kCLOCK_Root_Enet_Qos, &rootCfg);
    rootCfg.div = 10;
    CLOCK_SetRootClock(kCLOCK_Root_Enet_Timer3, &rootCfg); /* Generate 50M PTP REF clock. */

    rootCfg.mux = 7;
    rootCfg.div = 2;
    CLOCK_SetRootClock(kCLOCK_Root_Bus, &rootCfg); /* Generate 198M bus clock. */
}

void BOARD_UpdateENETModuleClock(enet_qos_mii_speed_t miiSpeed)
{
    /* ENET_QOS clock source: Select SysPll1Div2, 1G/2 = 500M */
    clock_root_config_t rootCfg = {.mux = 4};

    switch (miiSpeed)
    {
        case kENET_QOS_MiiSpeed1000M:
            /* Generate 125M root clock for 1000Mbps. */
            rootCfg.div = 4U;
            break;
        case kENET_QOS_MiiSpeed100M:
            /* Generate 25M root clock for 100Mbps. */
            rootCfg.div = 20U;
            break;
        case kENET_QOS_MiiSpeed10M:
            /* Generate 2.5M root clock for 10Mbps. */
            rootCfg.div = 200U;
            break;
        default:
            /* Generate 125M root clock. */
            rootCfg.div = 4U;
            break;
    }
    CLOCK_SetRootClock(kCLOCK_Root_Enet_Qos, &rootCfg);
}

void ENET_QOS_SetSYSControl(enet_qos_mii_mode_t miiMode)
{
    IOMUXC_GPR->GPR6 |= (miiMode << 3U);
    IOMUXC_GPR->GPR6 |= IOMUXC_GPR_GPR6_ENET_QOS_CLKGEN_EN_MASK; /* Set this bit to enable ENET_QOS clock generation. */
}


static void my_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void)level);

    PRINTF("\r\n%s, at line %d in file %s\n", str, line, file);
}

/*!
 * @brief Interrupt service for SysTick timer.
 */
void SysTick_Handler(void)
{
    time_isr();
}

/*!
 * @brief Main function
 */
int main(void)
{
    struct netif netif;
    int ret;
    ip4_addr_t netif_ipaddr, netif_netmask, netif_gw;
    ethernetif_config_t enet_config = {
        .phyHandle  = &phyHandle,
        .macAddress = configMAC_ADDR,
    };

    gpio_pin_config_t gpio_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

    /* Hardware Initialization. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitModuleClock();

    IOMUXC_GPR->GPR6 |= IOMUXC_GPR_GPR6_ENET_QOS_RGMII_EN_MASK; /* Set this bit to enable ENET_QOS RGMII TX clock output
                                                                   on TX_CLK pad. */

    GPIO_PinInit(GPIO11, 14, &gpio_config);
    /* For a complete PHY reset of RTL8211FDI-CG, this pin must be asserted low for at least 10ms. And
     * wait for a further 30ms(for internal circuits settling time) before accessing the PHY register */
    GPIO_WritePinOutput(GPIO11, 14, 0);
    SDK_DelayAtLeastUs(10000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    GPIO_WritePinOutput(GPIO11, 14, 1);
    SDK_DelayAtLeastUs(30000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    EnableIRQ(ENET_1G_MAC0_Tx_Rx_1_IRQn);
    EnableIRQ(ENET_1G_MAC0_Tx_Rx_2_IRQn);

    mdioHandle.resource.csrClock_Hz = EXAMPLE_CLOCK_FREQ;

    time_init();

    CRYPTO_InitHardware();

    IP4_ADDR(&netif_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
    IP4_ADDR(&netif_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
    IP4_ADDR(&netif_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

    lwip_init();

    netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN, ethernet_input);
    netif_set_default(&netif);
    netif_set_up(&netif);

    /*
     * mbedTLS - setup
     */

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_init(&cache);
#endif
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_pk_init(&pkey);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    /*
     * 1. Load the certificates and private RSA key
     */

    /*
     * This demonstration program uses embedded test certificates.
     * Instead, you may want to use mbedtls_x509_crt_parse_file() to read the
     * server and CA certificates, as well as mbedtls_pk_parse_keyfile().
     */
    ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *)mbedtls_test_srv_crt, mbedtls_test_srv_crt_len);
    if (ret != 0)
    {
        PRINTF(" failed\r\n  !  mbedtls_x509_crt_parse returned %d\r\n\r\n", ret);
        goto exit;
    }

#if (!defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED))
    ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *)mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
    if (ret != 0)
    {
        PRINTF(" failed\r\n  !  mbedtls_x509_crt_parse returned %d\r\n\r\n", ret);
        goto exit;
    }
#endif

    ret = mbedtls_pk_parse_key(&pkey, (const unsigned char *)mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0);
    if (ret != 0)
    {
        PRINTF(" failed\r\n  !  mbedtls_pk_parse_key returned %d\r\n\r\n", ret);
        goto exit;
    }

    /*
     * 2. Seeding the random number generator
     */
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers,
                                     strlen(pers))) != 0)
    {
        PRINTF(" failed\r\n  ! mbedtls_ctr_drbg_seed returned %d\r\n", ret);
        goto exit;
    }

    /*
     * 3. Setting up the SSL data.
     */
    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        PRINTF(" failed\r\n  ! mbedtls_ssl_config_defaults returned %d\r\n\r\n", ret);
        goto exit;
    }

    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_dbg(&conf, my_debug, NULL);
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_conf_session_cache(&conf, &cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
#endif
    mbedtls_ssl_conf_ca_chain(&conf, srvcert.next, NULL);
    if ((ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey)) != 0)
    {
        PRINTF(" failed\r\n  ! mbedtls_ssl_conf_own_cert returned %d\r\n\r\n", ret);
        goto exit;
    }

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        PRINTF(" failed\r\n  ! mbedtls_ssl_setup returned %d\r\n\r\n", ret);
        goto exit;
    }

    httpd_mbedtls_init(&ssl);

    PRINTF("\r\n************************************************\r\n");
    PRINTF(" mbedTLS HTTPS Server example\r\n");
    PRINTF("************************************************\r\n");
    PRINTF(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&netif_ipaddr)[0], ((u8_t *)&netif_ipaddr)[1],
           ((u8_t *)&netif_ipaddr)[2], ((u8_t *)&netif_ipaddr)[3]);
    PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&netif_netmask)[0], ((u8_t *)&netif_netmask)[1],
           ((u8_t *)&netif_netmask)[2], ((u8_t *)&netif_netmask)[3]);
    PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&netif_gw)[0], ((u8_t *)&netif_gw)[1],
           ((u8_t *)&netif_gw)[2], ((u8_t *)&netif_gw)[3]);
    PRINTF("************************************************\r\n");

    while (1)
    {
        /* Poll the driver, get any outstanding frames */
        ethernetif_input(&netif);

        sys_check_timeouts(); /* Handle all system timeouts for all core protocols */
    }

exit:
    return -1;
}
#endif // LWIP_TCP
