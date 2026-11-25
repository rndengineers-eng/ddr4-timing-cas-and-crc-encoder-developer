#include "spd_crc.h"
#include "spd_crc_finalize.h"

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

static int test_crc_known_answer(void)
{
    static const uint8_t input[] = {
        '1', '2', '3', '4', '5',
        '6', '7', '8', '9'};   

    uint16_t crc = UINT16_C(0xFFFF);

    CHECK(
        spd_crc16_ccitt_zero(input, sizeof input, &crc) == 
        SPD_CRC_STATUS_OK); 

    CHECK(
        crc == UINT16_C(0x31C3));

    return 0;
}

static int test_empty_crc(void)
{
    uint16_t crc = UINT16_C(0xFFFF);

    CHECK(
        spd_crc16_ccitt_zero(NULL, 0U, &crc) == 
        SPD_CRC_STATUS_OK
    ); 
        
    CHECK(crc == UINT16_C(0x0000));

    return 0;
}

static int test_invalid_arguments(void)
{
    uint16_t crc = UINT16_C(0x1234); 

    CHECK(
        spd_crc16_ccitt_zero(NULL, 1U, &crc) == 
        SPD_CRC_STATUS_INVALID_ARGUMENT); 

    CHECK(
        crc == UINT16_C(0x1234));

    CHECK(
        spd_crc16_ccitt_zero(NULL, 0U, NULL) == 
        SPD_CRC_STATUS_INVALID_ARGUMENT);  

    return 0;
}

static int test_corruption_detection(void)
{
    uint8_t region_a[32];
    uint8_t region_b[32];

    uint16_t original_a;
    uint16_t corrupted_a;

    uint16_t original_b;
    uint16_t corrupted_b;

    size_t i;

    for (i = 0U; i < sizeof region_a; ++i) 
    {
        region_a[i] = (uint8_t)i;
        region_b[i] = (uint8_t)(0x80U + i);
    } 

    CHECK(
        spd_crc16_ccitt_zero(region_a, sizeof region_a,
              &original_a) == SPD_CRC_STATUS_OK);

    region_a[0] ^= UINT8_C(0x01);

    CHECK(
        spd_crc16_ccitt_zero(region_a, sizeof region_a,
              &corrupted_a) == SPD_CRC_STATUS_OK);

    CHECK(
        original_a != corrupted_a);

    CHECK(
        spd_crc16_ccitt_zero(region_b, sizeof region_b,
              &original_b) == SPD_CRC_STATUS_OK); 

    region_b[31] ^= UINT8_C(0x80);

    CHECK(
        spd_crc16_ccitt_zero(region_b, sizeof region_b,
              &corrupted_b) == SPD_CRC_STATUS_OK); 

    CHECK(
        original_b != corrupted_b);

    return 0;
}

static int test_finalize_fail_closed(void)
{
    SpdImage image = SPD_IMAGE_INITIALIZER;

    uint8_t before[SPD_IMAGE_SIZE];

    CHECK(
        spd_crc_finalize(NULL) == SPD_CRC_FINALIZE_INVALID_ARGUMENT); 

    CHECK(
        spd_crc_finalize(&image) == SPD_CRC_FINALIZE_NOT_INITIALIZED);

    CHECK(
        spd_image_init(&image) == SPD_ENCODER_OK); 

    memset(image.bytes, 0x5A, sizeof image.bytes);

    memcpy(before, image.bytes, sizeof before); 

    CHECK(
        spd_crc_finalize(&image) == SPD_CRC_FINALIZE_MISSING_SPECIFICATION); 

    CHECK(
        memcmp(before, image.bytes, sizeof before) == 0);

    return 0;
}

int main(void)
{
    CHECK(test_crc_known_answer() == 0);
    CHECK(test_empty_crc() == 0);
    CHECK(test_invalid_arguments() == 0);
    CHECK(test_corruption_detection() == 0);
    CHECK(test_finalize_fail_closed() == 0);

    printf("CRC-16/XMODEM known-answer test : PASS\n");
    printf("123456789                       : 0x31C3 PASS\n");
    printf("Empty input                     : 0x0000 PASS\n");
    printf("CRC corruption tests            : PASS\n");
    printf("SPD CRC placement               : BLOCKED\n");
    printf("Output preservation             : PASS\n");

    return 0;
}      