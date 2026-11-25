#include "spd_crc_finalize.h"   

SpdCrcFinalizeStatus spd_crc_finalize(SpdImage *image) 
{
    if (image == NULL) 
    {
        return SPD_CRC_FINALIZE_INVALID_ARGUMENT;
    }

    if (!image->initialized) 
    {
        return SPD_CRC_FINALIZE_NOT_INITIALIZED;
    }

    return SPD_CRC_FINALIZE_MISSING_SPECIFICATION; 
} 

const char *spd_crc_finalize_status_string(SpdCrcFinalizeStatus status) 
{
    switch (status) 
    {
        case SPD_CRC_FINALIZE_OK:
            return "OK";

        case SPD_CRC_FINALIZE_INVALID_ARGUMENT:
            return "INVALID_ARGUMENT";

        case SPD_CRC_FINALIZE_NOT_INITIALIZED:
            return "NOT_INITIALIZED";

        case SPD_CRC_FINALIZE_MISSING_SPECIFICATION:
            return "MISSING_SPECIFICATION";

        default:
            return "UNKNOWN_STATUS";
    }
}    