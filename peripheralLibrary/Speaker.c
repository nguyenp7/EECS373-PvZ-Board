#include "Speaker.h"

/* === Internal Buffer === */
static uint16_t dac_buffer[AUDIO_BUFFER_SIZE];
static uint32_t audio_index = AUDIO_HEADER_SIZE;

/* === Internal Functions === */
static void AUDIO_FillBuffer(uint16_t *buf, uint32_t len);

/* ===================================================== */

void AUDIO_Init(void)
{
    // Nothing required here for basic setup
}

/* ===================================================== */

void AUDIO_Play(void)
{
    /* Fill initial buffer */
    AUDIO_FillBuffer(dac_buffer, AUDIO_BUFFER_SIZE);

    /* Start DAC with DMA */
    HAL_DAC_Start_DMA(
        &hdac1,
        DAC_CHANNEL_1,
        (uint32_t*)dac_buffer,
        AUDIO_BUFFER_SIZE,
        DAC_ALIGN_12B_R
    );

    /* Start timer (drives sample rate) */
    HAL_TIM_Base_Start(&htim6);
}

/* ===================================================== */

void AUDIO_Stop(void)
{
    HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
    HAL_TIM_Base_Stop(&htim6);
}

/* ===================================================== */

static void AUDIO_FillBuffer(uint16_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        if (audio_index + 1 >= PvZloseSound_wav_len)
        {
            audio_index = AUDIO_HEADER_SIZE; // loop
        }

        /* Read 16-bit signed sample (little endian) */
        int16_t sample = (int16_t)(
            PvZloseSound_wav[audio_index] |
            (PvZloseSound_wav[audio_index + 1] << 8)
        );

        audio_index += 2;

        /* Convert signed PCM → 12-bit DAC */
        uint16_t dac_val = (sample + 32768) >> 4;

        buf[i] = dac_val;
    }
}

/* ===================================================== */
/* DMA CALLBACKS */
/* These must be visible to HAL (not static)            */
/* ===================================================== */

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    AUDIO_FillBuffer(dac_buffer, AUDIO_BUFFER_SIZE / 2);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    AUDIO_FillBuffer(&dac_buffer[AUDIO_BUFFER_SIZE / 2], AUDIO_BUFFER_SIZE / 2);
}
