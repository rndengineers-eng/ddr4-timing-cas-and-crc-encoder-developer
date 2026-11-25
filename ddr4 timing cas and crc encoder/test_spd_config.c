#include "spd_config.h"

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

static SpdModuleConfig make_complete_config(void) 
{
    SpdModuleConfig config;

    memset(&config, 0, sizeof config);

    config.module_capacity_gb = 16U;
    config.data_rate_mtps = 3200U;

    config.module_type = SPD_MODULE_SO_DIMM;

    config.dram_part_number = "MT40A2G8SA-062E";
    config.jedec_spd_revision = "controlled-test-reference";

    config.dram_density_mbit = 16384U;
    config.dram_device_width_bits = 8U;
    config.rank_count = 1U;
    config.module_bus_width_bits = 64U;

    config.package_type = SPD_PACKAGE_MONOLITHIC;
    config.dies_per_package = 1U;

    config.ecc_extension_known = true;
    config.ecc_extension_width_bits = 0U;

    config.timings.tck_min = (SpdTimingInput){ true, UINT64_C(625) };

    config.timings.taa_min = (SpdTimingInput){ true, UINT64_C(13750) }; 

    config.timings.trcd_min = (SpdTimingInput){ true, UINT64_C(13750) }; 

    config.timings.trp_min = (SpdTimingInput){ true, UINT64_C(13750) }; 

    config.timings.tras_min = (SpdTimingInput){ true, UINT64_C(32000) }; 

    config.timings.trc_min = (SpdTimingInput){ true, UINT64_C(45750) };

    config.timings.trfc1_min = (SpdTimingInput){ true, UINT64_C(1) }; 

    config.timings.trfc2_min = (SpdTimingInput){ true, UINT64_C(1) }; 

    config.timings.trfc4_min = (SpdTimingInput){ true, UINT64_C(1) };

    config.timings.tfaw_min = (SpdTimingInput){ true, UINT64_C(1) }; 

    config.timings.trrd_s_min = (SpdTimingInput){ true, UINT64_C(1) };

    config.timings.trrd_l_min = (SpdTimingInput){ true, UINT64_C(1) };

    config.timings.tccd_l_min = (SpdTimingInput){ true, UINT64_C(1) };

    config.timings.supported_cas_latencies = "synthetic CAS source text";

    config.module_manufacturer = "Q Technologies";
    config.dram_manufacturer = "Micron";
    config.module_part_number = "QTECH-DDR4-16GB";

    config.manufacturer_ids_known = true;

    config.module_manufacturer_id = 1U;
    config.dram_manufacturer_id = 1U;

    config.jedec_document.reference = "controlled-jedec-reference";

    config.jedec_document.approval = SPD_SOURCE_APPROVED;

    config.dram_document.reference = "controlled-dram-reference";

    config.dram_document.approval = SPD_SOURCE_APPROVED;

    config.module_bom.reference = "controlled-module-bom";

    config.module_bom.approval = SPD_SOURCE_APPROVED;

    config.eeprom_document.reference = "controlled-eeprom-reference";

    config.eeprom_document.approval = SPD_SOURCE_APPROVED;

    config.physical_dram_device_count = 8U;

    return config;
} 

int main(void)
{
    SpdModuleConfig config;
    SpdConfigReport report;
    SpdConfigStatus status;

    CHECK(
        spd_config_inspect(NULL, &report) == SPD_CONFIG_INVALID_ARGUMENT); 

    config = make_complete_config();

    CHECK(spd_config_inspect(&config, NULL) == SPD_CONFIG_INVALID_ARGUMENT); 

    CHECK(spd_config_check(NULL) == SPD_CONFIG_INVALID_ARGUMENT); 

    printf(
        "NULL config/report                  : PASS\n"); 

    config = make_complete_config();

    config.dram_part_number = NULL;

    status = spd_config_inspect(&config, &report); 

    CHECK(status == SPD_CONFIG_INCOMPLETE); 

    CHECK(
        report.fields_present == false); 

    printf(
        "Missing required field               : PASS\n"); 

    config = make_complete_config();

    config.timings.trfc1_min.known = false;

    status = spd_config_inspect(&config, &report); 

    CHECK(
        status == SPD_CONFIG_INCOMPLETE); 

    CHECK(
        report.fields_present == false);  

    printf(
        "Unknown timing field                 : PASS\n"); 

    config = make_complete_config();

    config.timings.taa_min.known = true;
    config.timings.taa_min.ps = UINT64_C(0);

    status = spd_config_inspect(&config, &report); 

    CHECK(
        status == SPD_CONFIG_INVALID_VALUE); 

    CHECK(
        report.basic_values_valid == false); 

    printf(
        "Known-zero timing rejection          : PASS\n"); 

    config = make_complete_config();

    config.physical_dram_device_count = 7U;

    status = spd_config_inspect(&config, &report); 

    CHECK(
        status == SPD_CONFIG_INVALID_VALUE); 

    printf(
        "Physical DRAM count validation       : PASS\n"); 

    config = make_complete_config();

    config.module_capacity_gb = 8U;

    status = spd_config_inspect(&config, &report); 

    CHECK(
        status == SPD_CONFIG_INVALID_VALUE); 

    printf(
        "Module capacity validation           : PASS\n"); 

    config = make_complete_config();

    config.jedec_document.approval = SPD_SOURCE_PENDING;

    status = spd_config_inspect(&config, &report);  

    CHECK(
        status == SPD_CONFIG_UNVERIFIED_SPECIFICATION); 

    CHECK(
        report.sources_approved == false); 

    printf(
        "Unapproved source handling           : PASS\n"); 

    config = make_complete_config();

    status = spd_config_inspect(&config, &report); 

    CHECK(
        status == SPD_CONFIG_UNVERIFIED_SPECIFICATION); 

    CHECK(
        report.fields_present == true); 

    CHECK(
        report.basic_values_valid == true);  

    CHECK(
        report.sources_approved == true); 

    CHECK(
        report.semantic_validation_complete == false); 

    printf(
        "Fail-closed JEDEC specification      : PASS\n");

    status = spd_config_check(&config); 

    CHECK(
        status == SPD_CONFIG_UNVERIFIED_SPECIFICATION); 

    printf(
        "spd_config_check() consistency       : PASS\n"); 

    CHECK(strcmp(spd_config_status_string(SPD_CONFIG_INVALID_ARGUMENT),
            "INVALID_ARGUMENT") == 0); 

    CHECK(
        strcmp(spd_config_status_string(SPD_CONFIG_INCOMPLETE), 
            "INCOMPLETE") == 0); 

    CHECK(strcmp(spd_config_status_string(SPD_CONFIG_FIELDS_PRESENT),
            "FIELDS_PRESENT") == 0); 

    CHECK(
        strcmp(spd_config_status_string(SPD_CONFIG_INVALID_VALUE), 
            "INVALID_VALUE") == 0); 

    CHECK(
        strcmp(spd_config_status_string(SPD_CONFIG_UNVERIFIED_SPECIFICATION),
            "UNVERIFIED_SPECIFICATION") == 0); 

    printf(
        "Status string validation             : PASS\n");
 
    printf("\n");
    printf(
        "SPD configuration unit tests         : PASS\n"); 

    printf(
        "Production SPD generation            : BLOCKED\n");

    return 0;
}      