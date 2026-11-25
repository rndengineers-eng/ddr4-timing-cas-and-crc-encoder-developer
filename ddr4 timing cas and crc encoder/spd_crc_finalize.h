#ifndef SPD_CRC_FINALIZE_H
#define SPD_CRC_FINALIZE_H

#include "spd_encoder.h"  

typedef enum
{
    SPD_CRC_FINALIZE_OK = 0,
    SPD_CRC_FINALIZE_INVALID_ARGUMENT,
    SPD_CRC_FINALIZE_NOT_INITIALIZED,
    SPD_CRC_FINALIZE_MISSING_SPECIFICATION
} SpdCrcFinalizeStatus;

SpdCrcFinalizeStatus spd_crc_finalize(SpdImage *image); 

const char *spd_crc_finalize_status_string(SpdCrcFinalizeStatus status); 

#endif  