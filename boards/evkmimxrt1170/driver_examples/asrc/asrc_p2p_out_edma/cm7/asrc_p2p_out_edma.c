/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_sai_edma.h"
#include "music.h"
#include "fsl_codec_common.h"
#include "fsl_asrc_edma.h"
#include "fsl_dmamux.h"
#include "fsl_wm8960.h"
#include "fsl_codec_adapter.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* SAI instance and clock */
#define DEMO_CODEC_WM8960
#define DEMO_SAI                    SAI1
#define DEMO_SAI_IRQ                SAI1_IRQn
#define SAI_TxIRQHandler            SAI1_IRQHandler
#define DEMO_SAI_TX_DMAMUX_SOURCE   kDmaRequestMuxSai1Tx
#define DEMO_ASRC_IN_DMAMUX_SOURCE  kDmaRequestMuxASRCRequest1
#define DEMO_ASRC_OUT_DMAMUX_SOURCE kDmaRequestMuxASRCRequest4
#define DEMO_SAI_CHANNEL            0
#define DEMO_ASRC_IN_CHANNEL        1
#define DEMO_ASRC_OUT_CHANNEL       4
/* Select Audio/Video PLL (786.48 MHz) as sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_SELECT (2U)
/* Clock pre divider for sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER (1U)
/* Clock divider for sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_DIVIDER (63U)
/* Get frequency of sai1 clock */
#define DEMO_SAI_CLK_FREQ                                                        \
    (CLOCK_GetFreq(kCLOCK_AudioPllClk) / (DEMO_SAI1_CLOCK_SOURCE_DIVIDER + 1U) / \
     (DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER + 1U))

#define DEMO_AUDIO_MASTER_CLOCK          24576000U
#define DEMO_ASRC_OUTPUT_SOURCE_CLOCK_HZ (16 * kSAI_SampleRate32KHz * 2)
#define DEMO_ASRC_PERIPHERAL_CLOCK       200000000U
#define DEMO_ASRC                        ASRC
#define DEMO_ASRC_CHANNEL_PAIR           kASRC_ChannelPairA

#define DEMO_AUDIO_SAMPLE_RATE_IN  (kSAI_SampleRate48KHz)
#define DEMO_AUDIO_SAMPLE_RATE_OUT (kSAI_SampleRate32KHz)

/* I2C instance and clock */
#define DEMO_I2C LPI2C1

/* Select USB1 PLL (480 MHz) as master lpi2c clock source */
#define DEMO_LPI2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for master lpi2c clock source */
#define DEMO_LPI2C_CLOCK_SOURCE_DIVIDER (5U)
/* Get frequency of lpi2c clock */
#define DEMO_I2C_CLK_FREQ ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (DEMO_LPI2C_CLOCK_SOURCE_DIVIDER + 1U))

/* DMA */
#define EXAMPLE_DMA     DMA0
#define EXAMPLE_CHANNEL (0U)

/* demo audio data channel */
#define DEMO_AUDIO_DATA_CHANNEL (2U)
/* demo audio bit width */
#define DEMO_AUDIO_BIT_WIDTH          kSAI_WordWidth16bits
#define DEMO_ASRC_CONVERT_BUFFER_SIZE (100 * 1000U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void startSai(bool start);
/*******************************************************************************
 * Variables
 ******************************************************************************/
edma_handle_t asrcInDmaHandle                               = {0};
edma_handle_t asrcOutDmaHandle                              = {0};
AT_NONCACHEABLE_SECTION_INIT(asrc_edma_handle_t asrcHandle) = {0};
static volatile bool isASRCFinished                         = false;
extern codec_config_t boardCodecConfig;
#if (defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)) || \
    (defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER))
sai_master_clock_t mclkConfig = {
#if defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)
    .mclkOutputEnable = true,
#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
    .mclkSource = kSAI_MclkSourceSysclk,
#endif
#endif
};
#endif
codec_handle_t codecHandle;
uint8_t asrcConvertBuffer[DEMO_ASRC_CONVERT_BUFFER_SIZE] = {0U};
asrc_channel_pair_config_t s_asrcChannelPairConfig       = {
    .audioDataChannels         = kASRC_ChannelsNumber2,
    .inClockSource             = kASRC_ClockSourceNotAvalible,
    .inSourceClock_Hz          = 0,
    .outClockSource            = kASRC_ClockSourceBitClock0,
    .outSourceClock_Hz         = DEMO_ASRC_OUTPUT_SOURCE_CLOCK_HZ,
    .sampleRateRatio           = kASRC_RatioUseIdealRatio,
    .inDataWidth               = kASRC_DataWidth16Bit,
    .inDataAlign               = kASRC_DataAlignLSB,
    .outDataWidth              = kASRC_DataWidth16Bit,
    .outDataAlign              = kASRC_DataAlignLSB,
    .outSignExtension          = kASRC_NoSignExtension,
    .outFifoThreshold          = FSL_ASRC_CHANNEL_PAIR_FIFO_DEPTH / 8,
    .inFifoThreshold           = FSL_ASRC_CHANNEL_PAIR_FIFO_DEPTH / 2,
    .bufStallWhenFifoEmptyFull = false,
};
static const asrc_p2p_edma_config_t s_asrcEDMAP2PConfig = {
    .watermark       = FSL_FEATURE_SAI_FIFO_COUNT / 2U,
    .channel         = 0,
    .startPeripheral = startSai,
};
/*******************************************************************************
 * Code
 ******************************************************************************/
wm8960_config_t wm8960Config = {
    .i2cConfig        = {.codecI2CInstance = BOARD_CODEC_I2C_INSTANCE, .codecI2CSourceClock = 24000000U},
    .route            = kWM8960_RoutePlaybackandRecord,
    .leftInputSource  = kWM8960_InputDifferentialMicInput3,
    .rightInputSource = kWM8960_InputDifferentialMicInput2,
    .playSource       = kWM8960_PlaySourceDAC,
    .slaveAddress     = WM8960_I2C_ADDR,
    .bus              = kWM8960_BusI2S,
    .format = {.mclk_HZ = 24576000, .sampleRate = kWM8960_AudioSampleRate16KHz, .bitWidth = kWM8960_AudioBitWidth16bit},
    .master_slave = false,
};
codec_config_t boardCodecConfig = {.codecDevType = kCODEC_WM8960, .codecDevConfig = &wm8960Config};
/*
 * AUDIO PLL setting: Frequency = Fref * (DIV_SELECT + NUM / DENOM) / (2^POST)
 *                              = 24 * (32 + 77/100)  / 2
 *                              = 393.24MHZ
 */
const clock_audio_pll_config_t audioPllConfig = {
    .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
    .postDivider = 1,   /* Divider after the PLL, should only be 0, 1, 2, 3, 4, 5 */
    .numerator   = 77,  /* 30 bit numerator of fractional loop divider. */
    .denominator = 100, /* 30 bit denominator of fractional loop divider */
};

void BOARD_EnableSaiMclkOutput(bool enable)
{
    if (enable)
    {
        IOMUXC_GPR->GPR0 |= 1 << 8U;
    }
    else
    {
        IOMUXC_GPR->GPR0 &= ~(1 << 8U);
    }
}

static void asrc_callback(ASRC_Type *base, asrc_edma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_ASRCOutQueueIdle)
    {
        isASRCFinished = true;
    }
}

static void startSai(bool start)
{
    if (start)
    {
        SAI_TxEnable(DEMO_SAI, true);
    }
    else
    {
        SAI_TxEnable(DEMO_SAI, false);
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    sai_transceiver_t config;
    uint8_t *audioData      = (uint8_t *)music;
    edma_config_t dmaConfig = {0};
    uint32_t outputBufSize  = 0U;
    asrc_transfer_t asrcConvert;

    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    CLOCK_InitAudioPll(&audioPllConfig);
    BOARD_InitDebugConsole();

    CLOCK_SetRootClockMux(kCLOCK_Root_Lpi2c5, 1);
    /* audio pll  */
    CLOCK_SetRootClockMux(kCLOCK_Root_Sai1, 4);
    CLOCK_SetRootClockDiv(kCLOCK_Root_Sai1, 16);
    /* 0SC400M */
    CLOCK_SetRootClockMux(kCLOCK_Root_Asrc, 2);
    /* divider 2 */
    CLOCK_SetRootClockDiv(kCLOCK_Root_Asrc, 2);
    /* audio pll */
    CLOCK_SetRootClockMux(kCLOCK_Root_Mic, 6);
    CLOCK_SetRootClockDiv(kCLOCK_Root_Mic, 16);

    BOARD_EnableSaiMclkOutput(true);

    DMAMUX_Init(DMAMUX0);
    DMAMUX_SetSource(DMAMUX0, DEMO_SAI_CHANNEL, DEMO_SAI_TX_DMAMUX_SOURCE);
    DMAMUX_EnableChannel(DMAMUX0, DEMO_SAI_CHANNEL);
    DMAMUX_SetSource(DMAMUX0, DEMO_ASRC_IN_CHANNEL, DEMO_ASRC_IN_DMAMUX_SOURCE);
    DMAMUX_EnableChannel(DMAMUX0, DEMO_ASRC_IN_CHANNEL);
    DMAMUX_SetSource(DMAMUX0, DEMO_ASRC_OUT_CHANNEL, DEMO_ASRC_OUT_DMAMUX_SOURCE);
    DMAMUX_EnableChannel(DMAMUX0, DEMO_ASRC_OUT_CHANNEL);

    PRINTF("ASRC p2p edma example\n\r");

    /* Create EDMA handle */
    /*
     * dmaConfig.enableRoundRobinArbitration = false;
     * dmaConfig.enableHaltOnError = true;
     * dmaConfig.enableContinuousLinkMode = false;
     * dmaConfig.enableDebugMode = false;
     */
    EDMA_GetDefaultConfig(&dmaConfig);
    EDMA_Init(EXAMPLE_DMA, &dmaConfig);
    EDMA_CreateHandle(&asrcInDmaHandle, EXAMPLE_DMA, DEMO_ASRC_IN_CHANNEL);
    EDMA_CreateHandle(&asrcOutDmaHandle, EXAMPLE_DMA, DEMO_ASRC_OUT_CHANNEL);

    /* SAI init */
    SAI_Init(DEMO_SAI);
    /* I2S mode configurations */
    SAI_GetClassicI2SConfig(&config, DEMO_AUDIO_BIT_WIDTH, kSAI_Stereo, kSAI_Channel0Mask);
    SAI_TxSetConfig(DEMO_SAI, &config);
    SAI_TxEnableDMA(DEMO_SAI, kSAI_FIFORequestDMAEnable, true);

    /* set bit clock divider */
    SAI_TxSetBitClockRate(DEMO_SAI, DEMO_AUDIO_MASTER_CLOCK, DEMO_AUDIO_SAMPLE_RATE_OUT, DEMO_AUDIO_BIT_WIDTH,
                          DEMO_AUDIO_DATA_CHANNEL);
    /* asrc init */
    ASRC_Init(DEMO_ASRC, DEMO_ASRC_PERIPHERAL_CLOCK);
    ASRC_TransferInCreateHandleEDMA(DEMO_ASRC, &asrcHandle, DEMO_ASRC_CHANNEL_PAIR, asrc_callback, &asrcInDmaHandle,
                                    NULL, NULL);
    ASRC_TransferOutCreateHandleEDMA(DEMO_ASRC, &asrcHandle, DEMO_ASRC_CHANNEL_PAIR, asrc_callback, &asrcOutDmaHandle,
                                     &s_asrcEDMAP2PConfig, NULL);
    ASRC_TransferSetChannelPairConfigEDMA(DEMO_ASRC, &asrcHandle, &s_asrcChannelPairConfig, DEMO_AUDIO_SAMPLE_RATE_IN,
                                          DEMO_AUDIO_SAMPLE_RATE_OUT);

    /* Use default setting to init codec */
    if (CODEC_Init(&codecHandle, &boardCodecConfig) != kStatus_Success)
    {
        assert(false);
    }

    PRINTF("Playback converted audio data\r\n");
    PRINTF("    sample rate : %d\r\n", DEMO_AUDIO_SAMPLE_RATE_OUT);
    PRINTF("    channel number: %d\r\n", DEMO_AUDIO_DATA_CHANNEL);
    PRINTF("    frequency: 215HZ.\r\n\r\n");

    outputBufSize = ASRC_GetOutSamplesSizeEDMA(DEMO_ASRC, &asrcHandle, DEMO_AUDIO_SAMPLE_RATE_IN,
                                               DEMO_AUDIO_SAMPLE_RATE_OUT, MUSIC_LEN);

    asrcConvert.inData      = audioData;
    asrcConvert.inDataSize  = MUSIC_LEN;
    asrcConvert.outData     = (void *)SAI_TxGetDataRegisterAddress(DEMO_SAI, DEMO_SAI_CHANNEL);
    asrcConvert.outDataSize = outputBufSize;
    ASRC_TransferEDMA(ASRC, &asrcHandle, &asrcConvert);

    while (!isASRCFinished)
    {
    }

    PRINTF("ASRC p2p edma example finished\n\r ");
    while (1)
    {
    }
}
