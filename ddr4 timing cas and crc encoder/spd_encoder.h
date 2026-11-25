#ifndef SPD_ENCODER_H
#define SPD_ENCODER_H

#include "spd_config.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct 
{
    uint8_t bytes[SPD_IMAGE_SIZE];
    bool initialized;
} SpdImage;

#define SPD_IMAGE_INITIALIZER { {0}, false }

typedef enum
{
    SPD_ENCODER_OK = 0,
    SPD_ENCODER_INVALID_ARGUMENT,
    SPD_ENCODER_NOT_INITIALIZED,
    SPD_ENCODER_OUT_OF_BOUNDS,
    SPD_ENCODER_INVALID_VALUE,
    SPD_ENCODER_INCOMPLETE_CONFIG,
    SPD_ENCODER_INVALID_CONFIG,
    SPD_ENCODER_MISSING_SPECIFICATION
} SpdEncoderStatus;

SpdEncoderStatus spd_image_init(SpdImage *image); 

SpdEncoderStatus spd_image_read(const SpdImage *image, size_t offset, uint8_t *out,
    size_t count); 

SpdEncoderStatus spd_image_write
(
    SpdImage *image,
    size_t offset,
    const uint8_t *data,
    size_t count
);

SpdEncoderStatus spd_image_update_bits
(
    SpdImage *image,
    size_t offset,
    uint8_t mask,
    uint8_t encoded_bits
);

SpdEncoderStatus spd_bits_pack_u32
(
    uint32_t value,
    unsigned shift,
    unsigned width,
    uint32_t *out
); 

SpdEncoderStatus spd_bits_unpack_u32
(
    uint32_t word,
    unsigned shift,
    unsigned width,
    uint32_t *out
); 

SpdEncoderStatus spd_encode(const SpdModuleConfig *config, SpdImage *image); 

SpdEncoderStatus spd_encoder_final_check
(
    const SpdModuleConfig *config,
    const SpdImage *image
); 

const char *spd_encoder_status_string(SpdEncoderStatus status); 
#endif    