#include "spd_crc.h"

SpdCrcStatus spd_crc16_ccitt_zero(const uint8_t *data,
    size_t length, uint16_t *out_crc) 
{
    uint16_t crc = UINT16_C(0);
    size_t i;
    unsigned bit;

    if (out_crc == NULL)
    {
        return SPD_CRC_STATUS_INVALID_ARGUMENT;
    }

    if ((data == NULL) && (length != 0U))
    {
        return SPD_CRC_STATUS_INVALID_ARGUMENT;
    }

    for (i = 0U; i < length; ++i)
    {
        crc ^= (uint16_t)((uint16_t)data[i] << 8);

        for (bit = 0U; bit < 8U; ++bit)
        {
            if ((crc & UINT16_C(0x8000)) != 0U)
            {
                crc = (uint16_t)
                (
                    (uint16_t)(crc << 1) ^
                    UINT16_C(0x1021)
                );
            }
            else
            {
                crc = (uint16_t)(crc << 1);
            }
        }
    }

    *out_crc = crc;

    return SPD_CRC_STATUS_OK;
}   