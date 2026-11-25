#ifndef SPD_CRC_H
#define SPD_CRC_H

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    SPD_CRC_STATUS_OK = 0,
    SPD_CRC_STATUS_INVALID_ARGUMENT = 1
} SpdCrcStatus;

SpdCrcStatus spd_crc16_ccitt_zero
(
    const uint8_t *data,
    size_t length,
    uint16_t *out_crc
); 

#endif  