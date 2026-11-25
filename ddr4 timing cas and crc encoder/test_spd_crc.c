#include "spd_crc.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    const char *text = "123456789";
    uint16_t crc = 0U;

    if (spd_crc16_ccitt_zero((const uint8_t *)text, strlen(text),
            &crc) != SPD_CRC_STATUS_OK) 
    {
        printf("CRC calculation failed\n");
        return 1;
    }

    printf("Input : %s\n", text);
    printf("CRC   : 0x%04X\n", crc);  

    if (crc == 0x31C3U) 
    {
        printf("CRC known-answer test : PASS\n");  
        return 0;
    }

    printf("CRC known-answer test : FAIL\n");
    return 1;
}          
