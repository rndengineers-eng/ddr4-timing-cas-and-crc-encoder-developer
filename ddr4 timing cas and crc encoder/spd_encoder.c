#include "spd_encoder.h"

#include <string.h> 

static int range_is_valid(size_t offset, size_t count)
{
    if (offset > SPD_IMAGE_SIZE)
    {
        return 0; 
    }

    if (count > (SPD_IMAGE_SIZE - offset))
    {
        return 0;
    }

    return 1;
}

SpdEncoderStatus spd_image_init(
    SpdImage *image)
{
    if (image == NULL)
    {
        return SPD_ENCODER_INVALID_ARGUMENT;
    }

    memset(image->bytes, 0, sizeof image->bytes);
    image->initialized = true;

    return SPD_ENCODER_OK;
}

SpdEncoderStatus spd_image_read(
    const SpdImage *image,
    size_t offset,
    uint8_t *out,
    size_t count)
{
    if ((image == NULL) || (out == NULL))
    {
        return SPD_ENCODER_INVALID_ARGUMENT;
    }

    if (!image->initialized)
    {
        return SPD_ENCODER_NOT_INITIALIZED;
    }

    if (!range_is_valid(offset, count))
    {
        return SPD_ENCODER_OUT_OF_BOUNDS;
    }

    if (count == 0U)
    {
        return SPD_ENCODER_OK;
    }

    memcpy(out, &image->bytes[offset], count); 
    return SPD_ENCODER_OK;
} 

SpdEncoderStatus spd_image_write(SpdImage *image, size_t offset, const uint8_t *data,
    size_t count)  
{ 
    if ((image == NULL) || (data == NULL))
    {
        return SPD_ENCODER_INVALID_ARGUMENT;
    }

    if (!image->initialized)
    {
        return SPD_ENCODER_NOT_INITIALIZED;
    }

    if (!range_is_valid(offset, count))
    {
        return SPD_ENCODER_OUT_OF_BOUNDS;
    }

    if (count == 0U)
    {
        return SPD_ENCODER_OK;
    }

    memcpy(&image->bytes[offset], data, count); 
    return SPD_ENCODER_OK;
} 

SpdEncoderStatus spd_image_update_bits(SpdImage *image, size_t offset,
    uint8_t mask, uint8_t encoded_bits)
{
    uint8_t current;

    if (image == NULL)
    {
        return SPD_ENCODER_INVALID_ARGUMENT;
    }

    if (!image->initialized)
    {
        return SPD_ENCODER_NOT_INITIALIZED;
    }

    if (offset >= SPD_IMAGE_SIZE)
    {
        return SPD_ENCODER_OUT_OF_BOUNDS;
    }

    if ((encoded_bits & (uint8_t)(~mask)) != 0U)
    {
        return SPD_ENCODER_INVALID_VALUE;
    }

    current = image->bytes[offset];

    current = (uint8_t)((current & (uint8_t)(~mask)) | encoded_bits); 

    image->bytes[offset] = current;

    return SPD_ENCODER_OK;
}

SpdEncoderStatus spd_bits_pack_u32(uint32_t value, unsigned shift, unsigned width,
    uint32_t *out) 
{
    uint32_t mask;
    uint32_t max_value;

    if (out == NULL)
    {
        return SPD_ENCODER_INVALID_ARGUMENT;
    }

    if ((width == 0U) || (width > 32U) ||
        (shift >= 32U) || (width > (32U - shift)))
    {
        return SPD_ENCODER_INVALID_VALUE;
    } 

    if (width == 32U)
    {
        max_value = UINT32_MAX;
        mask = UINT32_MAX;
    }
    else
    {
        max_value = (UINT32_C(1) << width) - UINT32_C(1);
        mask = max_value << shift;
    }

    if (value > max_value)
    {
        return SPD_ENCODER_INVALID_VALUE;
    }

    *out = (value << shift) & mask;
    return SPD_ENCODER_OK; 
}

SpdEncoderStatus spd_bits_unpack_u32(uint32_t word, unsigned shift,
    unsigned width, uint32_t *out)
{
    uint32_t mask;

    if (out == NULL)
    {
        return SPD_ENCODER_INVALID_ARGUMENT;
    }

    if ((width == 0U) || (width > 32U) ||
        (shift >= 32U) || (width > (32U - shift)))
    {
        return SPD_ENCODER_INVALID_VALUE;
    }

    if (width == 32U)
    {
        mask = UINT32_MAX;
    }
    else
    {
        mask = (UINT32_C(1) << width) - UINT32_C(1); 
    }

    *out = (word >> shift) & mask;
    return SPD_ENCODER_OK;
} 

SpdEncoderStatus spd_encode(const SpdModuleConfig *config, SpdImage *image)
{
    SpdConfigStatus config_status;

    if ((config == NULL) || (image == NULL))
    {
        return SPD_ENCODER_INVALID_ARGUMENT;
    }

    if (!image->initialized)
    {
        return SPD_ENCODER_NOT_INITIALIZED;
    }

    config_status = spd_config_check(config);

    switch (config_status)
    {
        case SPD_CONFIG_INVALID_ARGUMENT:
            return SPD_ENCODER_INVALID_ARGUMENT;

        case SPD_CONFIG_INCOMPLETE:
            return SPD_ENCODER_INCOMPLETE_CONFIG;

        case SPD_CONFIG_INVALID_VALUE:
            return SPD_ENCODER_INVALID_CONFIG;

        case SPD_CONFIG_UNVERIFIED_SPECIFICATION:
            return SPD_ENCODER_MISSING_SPECIFICATION;

        case SPD_CONFIG_FIELDS_PRESENT:
            break;

        default:
            return SPD_ENCODER_INVALID_CONFIG;
    }
    return SPD_ENCODER_MISSING_SPECIFICATION;
}

SpdEncoderStatus spd_encoder_final_check(const SpdModuleConfig *config,
    const SpdImage *image)
{
    SpdConfigStatus config_status; 

    if ((config == NULL) || (image == NULL))
    {
        return SPD_ENCODER_INVALID_ARGUMENT;
    }

    if (!image->initialized)
    {
        return SPD_ENCODER_NOT_INITIALIZED;
    }

    config_status = spd_config_check(config);

    switch (config_status)
    {
        case SPD_CONFIG_INVALID_ARGUMENT:
            return SPD_ENCODER_INVALID_ARGUMENT;

        case SPD_CONFIG_INCOMPLETE:
            return SPD_ENCODER_INCOMPLETE_CONFIG;

        case SPD_CONFIG_INVALID_VALUE:
            return SPD_ENCODER_INVALID_CONFIG;

        case SPD_CONFIG_UNVERIFIED_SPECIFICATION:
            return SPD_ENCODER_MISSING_SPECIFICATION;

        case SPD_CONFIG_FIELDS_PRESENT:
            break;

        default:
            return SPD_ENCODER_INVALID_CONFIG;
    }
    return SPD_ENCODER_MISSING_SPECIFICATION;
}

const char *spd_encoder_status_string(SpdEncoderStatus status) 
{
    switch (status)
    {
        case SPD_ENCODER_OK:
            return "OK";

        case SPD_ENCODER_INVALID_ARGUMENT:
            return "INVALID_ARGUMENT";

        case SPD_ENCODER_NOT_INITIALIZED:
            return "NOT_INITIALIZED";

        case SPD_ENCODER_OUT_OF_BOUNDS:
            return "OUT_OF_BOUNDS";

        case SPD_ENCODER_INVALID_VALUE:
            return "INVALID_VALUE";

        case SPD_ENCODER_INCOMPLETE_CONFIG:
            return "INCOMPLETE_CONFIG";

        case SPD_ENCODER_INVALID_CONFIG:
            return "INVALID_CONFIG";

        case SPD_ENCODER_MISSING_SPECIFICATION:
            return "MISSING_SPECIFICATION";

        default:
            return "UNKNOWN_STATUS";
    }
}      