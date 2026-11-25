#include "spd_timing_encode.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition)                                      \
    do                                                        \
    {                                                         \
        if (!(condition))                                     \
        {                                                     \
            fprintf(stderr,                                   \
                    "FAIL: %s:%d: %s\n",                      \
                    __FILE__,                                 \
                    __LINE__,                                 \
                    #condition);                              \
            return 1;                                         \
        }                                                     \
    } while (0)

static SpdTimingInputs make_valid_timings(void)
{
    SpdTimingInputs timing;

    memset(&timing, 0, sizeof timing);

    timing.tck_min =
        (SpdTimingInput){ true, UINT64_C(625) };

    timing.taa_min =
        (SpdTimingInput){ true, UINT64_C(13750) };

    timing.trcd_min =
        (SpdTimingInput){ true, UINT64_C(13750) };

    timing.trp_min =
        (SpdTimingInput){ true, UINT64_C(13750) };

    timing.tras_min =
        (SpdTimingInput){ true, UINT64_C(32000) };

    timing.trc_min =
        (SpdTimingInput){ true, UINT64_C(45750) };

    
    timing.trfc1_min =
        (SpdTimingInput){ true, UINT64_C(1) };

    timing.trfc2_min =
        (SpdTimingInput){ true, UINT64_C(1) };

    timing.trfc4_min =
        (SpdTimingInput){ true, UINT64_C(1) };

    timing.tfaw_min =
        (SpdTimingInput){ true, UINT64_C(1) };

    timing.trrd_s_min =
        (SpdTimingInput){ true, UINT64_C(1) };

    timing.trrd_l_min =
        (SpdTimingInput){ true, UINT64_C(1) };

    timing.tccd_l_min =
        (SpdTimingInput){ true, UINT64_C(1) };

    timing.supported_cas_latencies =
        "synthetic CAS source text";

    return timing;
}

int main(void)
{
    SpdTimingInputs timing;
    SpdImage image = SPD_IMAGE_INITIALIZER;
    uint8_t before[SPD_IMAGE_SIZE];

    CHECK(
        spd_timing_inputs_check(NULL) ==
        SPD_TIMING_ENCODE_INVALID_ARGUMENT); 

    timing = make_valid_timings();

    CHECK(
        spd_timing_inputs_check(&timing) ==
        SPD_TIMING_ENCODE_OK
    );

    timing.trfc1_min.known = false;

    CHECK(
        spd_timing_inputs_check(&timing) ==
        SPD_TIMING_ENCODE_INCOMPLETE); 

    timing = make_valid_timings();

    timing.taa_min.ps = UINT64_C(0);

    CHECK(
        spd_timing_inputs_check(&timing) ==
        SPD_TIMING_ENCODE_INVALID_VALUE);

    timing.trfc1_min.known = false;

    CHECK(
        spd_timing_inputs_check(&timing) ==
        SPD_TIMING_ENCODE_INVALID_VALUE);

    timing = make_valid_timings();

    CHECK(
        spd_timing_encode(&timing, NULL) ==
        SPD_TIMING_ENCODE_INVALID_ARGUMENT); 

    
    CHECK(
        spd_timing_encode(&timing, &image) ==
        SPD_TIMING_ENCODE_NOT_INITIALIZED);

    CHECK(
        spd_image_init(&image) ==
        SPD_ENCODER_OK); 

    memset(image.bytes, 0xA5,
        sizeof image.bytes); 

    memcpy(before,
        image.bytes, sizeof before); 

    CHECK(spd_timing_encode(&timing, &image) ==
        SPD_TIMING_ENCODE_MISSING_SPECIFICATION); 

    CHECK(
        memcmp(before, image.bytes, sizeof before) == 0);

    CHECK(spd_cas_encode("CL20 CL22", &image) ==
        SPD_TIMING_ENCODE_MISSING_SPECIFICATION); 

    CHECK(
        memcmp(before, image.bytes,
            sizeof before) == 0); 

    CHECK(
        spd_cas_encode("", &image) ==
        SPD_TIMING_ENCODE_INCOMPLETE); 

    CHECK(
        spd_cas_encode("   ", &image) ==
        SPD_TIMING_ENCODE_INCOMPLETE); 

    CHECK(
        spd_cas_encode(NULL, &image) ==
        SPD_TIMING_ENCODE_INVALID_ARGUMENT); 

    CHECK(
        memcmp(before, image.bytes,sizeof before) == 0);  

    printf("Timing/CAS encoder scaffolding tests : PASS\n");
    printf("Timing input validation              : PASS\n");
    printf("Unknown timing handling              : PASS\n");
    printf("Known-zero timing rejection          : PASS\n");
    printf("Invalid precedence                   : PASS\n");
    printf("Output preservation                  : PASS\n");
    printf("JEDEC timing byte encoding           : BLOCKED\n");
    printf("CAS bit encoding                     : BLOCKED\n");

    return 0;
}    