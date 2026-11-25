#include "spd_timing_encode.h"

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>  

static int has_text(const char *text)
{
    if (text == NULL)
    {
        return 0;
    }  

    while (*text != '\0')
    {
        if (!isspace((unsigned char)*text))
        {
            return 1;
        }

        ++text;
    }

    return 0;
}

SpdTimingEncodeStatus spd_timing_inputs_check(
    const SpdTimingInputs *timings)
{
    const SpdTimingInput *fields[13U];
    size_t i; 
 
    if (timings == NULL)
    {
        return SPD_TIMING_ENCODE_INVALID_ARGUMENT;
    }

    fields[0]  = &timings->tck_min;
    fields[1]  = &timings->taa_min;
    fields[2]  = &timings->trcd_min;
    fields[3]  = &timings->trp_min;
    fields[4]  = &timings->tras_min;
    fields[5]  = &timings->trc_min;
    fields[6]  = &timings->trfc1_min;
    fields[7]  = &timings->trfc2_min;
    fields[8]  = &timings->trfc4_min;
    fields[9]  = &timings->tfaw_min;
    fields[10] = &timings->trrd_s_min;
    fields[11] = &timings->trrd_l_min;
    fields[12] = &timings->tccd_l_min;

    for (i = 0U; i < 13U; ++i)
    {
        if (fields[i]->known && fields[i]->ps == UINT64_C(0))
        {
            return SPD_TIMING_ENCODE_INVALID_VALUE;
        }
    } 

    for (i = 0U; i < 13U; ++i)
    {
        if (!fields[i]->known)
        {
            return SPD_TIMING_ENCODE_INCOMPLETE;
        }
    }

    return SPD_TIMING_ENCODE_OK;
}

SpdTimingEncodeStatus spd_timing_encode(const SpdTimingInputs *timings,
    SpdImage *image) 
{
    SpdTimingEncodeStatus status;

    if ((timings == NULL) || (image == NULL))
    {
        return SPD_TIMING_ENCODE_INVALID_ARGUMENT;
    }

    if (!image->initialized)
    {
        return SPD_TIMING_ENCODE_NOT_INITIALIZED;
    }

    status = spd_timing_inputs_check(timings);

    if (status != SPD_TIMING_ENCODE_OK)
    {
        return status;
    } 

    return SPD_TIMING_ENCODE_MISSING_SPECIFICATION;
}

SpdTimingEncodeStatus spd_cas_encode(const char *supported_cas_latencies,
    SpdImage *image) 
{
    if ((supported_cas_latencies == NULL) ||
        (image == NULL))
    {
        return SPD_TIMING_ENCODE_INVALID_ARGUMENT;
    }

    if (!image->initialized)
    {
        return SPD_TIMING_ENCODE_NOT_INITIALIZED;
    }

    if (!has_text(supported_cas_latencies))
    {
        return SPD_TIMING_ENCODE_INCOMPLETE;
    }

    return SPD_TIMING_ENCODE_MISSING_SPECIFICATION;
}

const char *spd_timing_encode_status_string(
    SpdTimingEncodeStatus status)
{
    switch (status)
    {
        case SPD_TIMING_ENCODE_OK:
            return "OK";

        case SPD_TIMING_ENCODE_INVALID_ARGUMENT:
            return "INVALID_ARGUMENT";

        case SPD_TIMING_ENCODE_NOT_INITIALIZED:
            return "NOT_INITIALIZED";

        case SPD_TIMING_ENCODE_INCOMPLETE:
            return "INCOMPLETE";

        case SPD_TIMING_ENCODE_INVALID_VALUE:
            return "INVALID_VALUE";

        case SPD_TIMING_ENCODE_MISSING_SPECIFICATION:
            return "MISSING_SPECIFICATION";

        default:
            return "UNKNOWN_STATUS";
    }
}      