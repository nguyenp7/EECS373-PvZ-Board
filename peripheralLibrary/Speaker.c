#include "Speaker.h"

/* === Internal Buffer === */
static uint16_t dac_buffer[AUDIO_BUFFER_SIZE];

/* === Audio State === */
typedef struct {
    const uint8_t *data;
    uint32_t length;
    uint32_t index;
    uint8_t looping;
} AudioTrack;

static AudioTrack current_track;
static AudioTrack bgm_track;
static AudioTrack sfx_track;

static uint8_t playing_sfx = 0;

/* === Internal Functions === */
static void AUDIO_FillBuffer(uint16_t *buf, uint32_t len);

/* ===================================================== */

void AUDIO_Init(void)
{
    // Nothing required
}

/* ===================================================== */
/* === NEW: Play Background Music ======================= */

void AUDIO_PlayMusic(const uint8_t *data, uint32_t length)
{
    bgm_track.data = data;
    bgm_track.length = length;
    bgm_track.index = AUDIO_HEADER_SIZE;
    bgm_track.looping = 1;

    if (!playing_sfx) {
        current_track = bgm_track;
    }
}

/* ===================================================== */
/* === NEW: Play Sound Effect =========================== */

void AUDIO_PlaySFX(const uint8_t *data, uint32_t length)
{
    sfx_track.data = data;
    sfx_track.length = length;
    sfx_track.index = AUDIO_HEADER_SIZE;
    sfx_track.looping = 0;

    current_track = sfx_track;
    playing_sfx = 1;
}

/* ===================================================== */

void AUDIO_Play(void)
{
    AUDIO_FillBuffer(dac_buffer, AUDIO_BUFFER_SIZE);

    HAL_DAC_Start_DMA(
        &hdac1,
        DAC_CHANNEL_1,
        (uint32_t*)dac_buffer,
        AUDIO_BUFFER_SIZE,
        DAC_ALIGN_12B_R
    );

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
        /* If track finished */
        if (current_track.index + 1 >= current_track.length)
        {
            if (playing_sfx)
            {
                /* SFX finished → return to BGM */
                playing_sfx = 0;
                current_track = bgm_track;
            }
            else if (current_track.looping)
            {
                current_track.index = AUDIO_HEADER_SIZE;
            }
            else
            {
                /* No loop → silence */
                buf[i] = 2048;
                continue;
            }
        }

        int16_t sample = (int16_t)(
            current_track.data[current_track.index] |
            (current_track.data[current_track.index + 1] << 8)
        );

        current_track.index += 2;

        uint16_t dac_val = (sample + 32768) >> 4;
        buf[i] = dac_val;
    }
}

/* ===================================================== */
/* DMA CALLBACKS */
/* ===================================================== */

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    AUDIO_FillBuffer(dac_buffer, AUDIO_BUFFER_SIZE / 2);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    AUDIO_FillBuffer(&dac_buffer[AUDIO_BUFFER_SIZE / 2], AUDIO_BUFFER_SIZE / 2);
}
