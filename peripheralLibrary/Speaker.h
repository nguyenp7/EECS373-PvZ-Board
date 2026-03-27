#ifndef __AUDIO_H
#define __AUDIO_H

#include "stm32l4xx_hal.h"
#include <stdint.h>

/* === Audio Configuration === */
#define AUDIO_BUFFER_SIZE 1024
#define AUDIO_HEADER_SIZE  44   // WAV header offset

/* === Handles === */
extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim6;

/* === Audio Data (optional default sound) === */
extern const uint8_t PvZloseSound_wav[];
extern const uint32_t PvZloseSound_wav_len;

extern const uint8_t PvZBackground_wav[];

/* === API === */
void AUDIO_Init(void);
void AUDIO_Play(void);
void AUDIO_Stop(void);

/* === NEW API === */
void AUDIO_PlayMusic(const uint8_t *data, uint32_t length);
void AUDIO_PlaySFX(const uint8_t *data, uint32_t length);

#endif
