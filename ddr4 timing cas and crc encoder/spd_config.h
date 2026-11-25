#ifndef SPD_CONFIG_H
#define SPD_CONFIG_H 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h> 

#define SPD_IMAGE_SIZE 512U
#define SPD_CONFIG_MAX_ISSUES 64U

typedef enum 
{
    SPD_MODULE_UNKNOWN,
    SPD_MODULE_SO_DIMM
} SpdModuleType;

typedef enum 
{
    SPD_PACKAGE_UNKNOWN,
    SPD_PACKAGE_MONOLITHIC,
    SPD_PACKAGE_STACKED
} SpdPackageType; 

typedef enum 
{
    SPD_SOURCE_UNKNOWN,
    SPD_SOURCE_PENDING,
    SPD_SOURCE_APPROVED
} SpdSourceApproval;

typedef struct 
{
    const char *reference;
    SpdSourceApproval approval;
} SpdDocumentReference;

typedef struct 
{
    bool known;
    uint64_t ps;
} SpdTimingInput;

typedef struct 
{
    SpdTimingInput tck_min;
    SpdTimingInput taa_min;
    SpdTimingInput trcd_min;
    SpdTimingInput trp_min;
    SpdTimingInput tras_min;
    SpdTimingInput trc_min;

    SpdTimingInput trfc1_min;
    SpdTimingInput trfc2_min;
    SpdTimingInput trfc4_min;
    SpdTimingInput tfaw_min;

    SpdTimingInput trrd_s_min;
    SpdTimingInput trrd_l_min;
    SpdTimingInput tccd_l_min;

    const char *supported_cas_latencies;
} SpdTimingInputs;

typedef struct 
{
    uint32_t module_capacity_gb;
    uint32_t data_rate_mtps;

    SpdModuleType module_type;

    const char *dram_part_number;
    const char *jedec_spd_revision;

    uint32_t dram_density_mbit;
    uint8_t dram_device_width_bits;
    uint8_t rank_count;
    uint8_t module_bus_width_bits;

    SpdPackageType package_type;
    uint8_t dies_per_package;

    bool ecc_extension_known;
    uint8_t ecc_extension_width_bits;

    SpdTimingInputs timings;

    const char *module_manufacturer;
    const char *dram_manufacturer;
    const char *module_part_number;

    bool manufacturer_ids_known;
    uint32_t module_manufacturer_id;
    uint32_t dram_manufacturer_id;

    SpdDocumentReference jedec_document;
    SpdDocumentReference dram_document;
    SpdDocumentReference module_bom;
    SpdDocumentReference eeprom_document;

    uint16_t physical_dram_device_count;
} SpdModuleConfig;

typedef enum 
{
    SPD_CONFIG_INVALID_ARGUMENT,
    SPD_CONFIG_INCOMPLETE,
    SPD_CONFIG_FIELDS_PRESENT,
    SPD_CONFIG_INVALID_VALUE,
    SPD_CONFIG_UNVERIFIED_SPECIFICATION
} SpdConfigStatus;

typedef enum 
{
    SPD_ISSUE_MISSING,
    SPD_ISSUE_INVALID,
    SPD_ISSUE_UNVERIFIED
} SpdIssueKind;

typedef struct 
{
    const char *field;
    SpdIssueKind kind;
    const char *message;
} SpdConfigIssue;

typedef struct 
{
    bool fields_present;
    bool basic_values_valid;
    bool sources_approved;
    bool semantic_validation_complete;

    size_t issue_count;
    bool diagnostics_truncated;

    SpdConfigIssue issues[SPD_CONFIG_MAX_ISSUES];
} SpdConfigReport;

SpdConfigStatus spd_config_inspect(const SpdModuleConfig *config,
    SpdConfigReport *report); 

SpdConfigStatus spd_config_check(const SpdModuleConfig *config); 

const char *spd_config_status_string(SpdConfigStatus status); 

#endif     