#ifndef SPD_TIMING_ENCODE_H
#define SPD_TIMING_ENCODE_H

#include "spd_config.h"
#include "spd_encoder.h"  

typedef enum
{
    SPD_TIMING_ENCODE_OK = 0,
    SPD_TIMING_ENCODE_INVALID_ARGUMENT,
    SPD_TIMING_ENCODE_NOT_INITIALIZED,
    SPD_TIMING_ENCODE_INCOMPLETE,
    SPD_TIMING_ENCODE_INVALID_VALUE,
    SPD_TIMING_ENCODE_MISSING_SPECIFICATION
} SpdTimingEncodeStatus;

SpdTimingEncodeStatus spd_timing_inputs_check(
    const SpdTimingInputs *timings);

SpdTimingEncodeStatus spd_timing_encode(const SpdTimingInputs *timings,
    SpdImage *image); 

SpdTimingEncodeStatus spd_cas_encode(const char *supported_cas_latencies,
    SpdImage *image); 

const char *spd_timing_encode_status_string(SpdTimingEncodeStatus status); 
#endif   