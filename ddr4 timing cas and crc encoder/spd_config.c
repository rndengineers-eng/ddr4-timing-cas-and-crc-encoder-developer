#include "spd_config.h"

#include <string.h> 

static int has_text(const char *text)
{
    if (text == NULL)  
    {
        return 0;
    } 

    return text[0] != '\0';  
}

static void add_issue(SpdConfigReport *report, const char *field, SpdIssueKind kind,
    const char *message) 
{
    if (report == NULL) 
    {
        return;
    } 

    if (kind == SPD_ISSUE_MISSING) 
    {
        report->fields_present = false;
    }

    if (kind == SPD_ISSUE_INVALID) 
    {
        report->basic_values_valid = false;
    } 

    if (report->issue_count >= SPD_CONFIG_MAX_ISSUES) 
    {
        report->diagnostics_truncated = true;
        return;
    } 

    report->issues[report->issue_count].field = field;
    report->issues[report->issue_count].kind = kind;
    report->issues[report->issue_count].message = message;

    ++report->issue_count;
} 

static void require_text(SpdConfigReport *report, const char *field, const char *value) 
{ 
    if (!has_text(value)) 
    {
        add_issue(report, field, SPD_ISSUE_MISSING, "Required text value is missing."); 
    }
} 

static void require_number(SpdConfigReport *report, const char *field,
    uint64_t value)
{
    if (value == 0U) 
    {
        add_issue(report, field, SPD_ISSUE_MISSING,"Required numeric value is missing."); 
    }
}

static void check_timing(SpdConfigReport *report, const char *field,
    const SpdTimingInput *timing)
{
    if (timing == NULL) 
    {
        add_issue(report, field, SPD_ISSUE_MISSING, "Timing input is missing."); 
        return;
    }

    if (!timing->known) 
    {
        add_issue(report, field, SPD_ISSUE_MISSING, "Timing value is not known."); 
        return;
    }

    if (timing->ps == UINT64_C(0)) 
    {
        add_issue(report, field, SPD_ISSUE_INVALID,
            "Known timing value must be greater than zero."); 
    }
} 

static void check_document(SpdConfigReport *report, const char *field,
    const SpdDocumentReference *document) 
{
    if (document == NULL) 
    {
        add_issue(report, field, SPD_ISSUE_MISSING, "Document reference is missing."); 
        report->sources_approved = false;
        return;
    } 

    if (!has_text(document->reference)) 
    {
        add_issue(report, field, SPD_ISSUE_MISSING,"Controlled document reference is missing."); 
        report->sources_approved = false;
    } 

    switch (document->approval) 
    {
        case SPD_SOURCE_APPROVED:
            break;

        case SPD_SOURCE_UNKNOWN:
        case SPD_SOURCE_PENDING:
            report->sources_approved = false;

            add_issue( report, field, SPD_ISSUE_UNVERIFIED,"Document source has not been approved."); 
            break;

        default:
            report->sources_approved = false;

            add_issue(report, field, SPD_ISSUE_INVALID, "Invalid source approval value."); 
            break;
    }
} 

static void check_geometry(const SpdModuleConfig *config, SpdConfigReport *report)
{
    uint32_t devices_per_rank;
    uint32_t expected_device_count;
    uint64_t calculated_capacity_mbit;
    uint64_t expected_capacity_mbit;  

    if ((config == NULL) || (report == NULL)) 
    {
        return;
    } 

    if (config->package_type != SPD_PACKAGE_MONOLITHIC) 
    {
        return;
    } 

    if (config->dies_per_package != 1U) 
    {
        return;
    } 

    if (!config->ecc_extension_known) 
    {
        return;
    } 

    if (config->ecc_extension_width_bits != 0U) 
    {
        return;
    } 

    if ((config->module_bus_width_bits == 0U) ||
        (config->dram_device_width_bits == 0U) ||
        (config->rank_count == 0U) ||
        (config->dram_density_mbit == 0U) ||
        (config->module_capacity_gb == 0U)){
        return;
    } 

    if ((config->module_bus_width_bits %
         config->dram_device_width_bits) != 0U)
         {
            add_issue(report, "module_bus_width_bits", SPD_ISSUE_INVALID,
            "Module bus width is not divisible by DRAM device width.");
            return;
         }
         
         devices_per_rank = config->module_bus_width_bits /
         config->dram_device_width_bits;
         
         expected_device_count = devices_per_rank * config->rank_count;
         
         if ((config->physical_dram_device_count != 0U) &&
        (config->physical_dram_device_count != expected_device_count))
        {
            add_issue(report, "physical_dram_device_count", SPD_ISSUE_INVALID,
            "Physical DRAM device count is inconsistent with bus width, device width and rank count.");
        }
        
        calculated_capacity_mbit = (uint64_t)expected_device_count *
        (uint64_t)config->dram_density_mbit;
        
        expected_capacity_mbit = (uint64_t)config->module_capacity_gb *
        UINT64_C(8192);  
        
        if (calculated_capacity_mbit != expected_capacity_mbit){
        add_issue(report, "module_capacity_gb", SPD_ISSUE_INVALID,
            "Module capacity is inconsistent with DRAM density, width and rank count."); 
    }
}  

SpdConfigStatus spd_config_inspect(const SpdModuleConfig *config,
    SpdConfigReport *report)
{
    if ((config == NULL) || (report == NULL)) 
    {
        return SPD_CONFIG_INVALID_ARGUMENT;
    } 

    memset(report, 0, sizeof *report);

    report->fields_present = true;
    report->basic_values_valid = true;
    report->sources_approved = true;
    report->semantic_validation_complete = false;

    require_number(report, "module_capacity_gb", config->module_capacity_gb); 

    require_number(report, "data_rate_mtps", config->data_rate_mtps); 

    if (config->module_type == SPD_MODULE_UNKNOWN) 
    {
        add_issue(report, "module_type", SPD_ISSUE_MISSING,
            "Module type is not specified."); 
    } 
    else if (config->module_type != SPD_MODULE_SO_DIMM) 
    {
        add_issue(report, "module_type", SPD_ISSUE_INVALID, "Unsupported module type."); 
    }

    require_text(report, "dram_part_number", config->dram_part_number); 

    require_text(report, "jedec_spd_revision", config->jedec_spd_revision); 

    require_number(report, "dram_density_mbit", config->dram_density_mbit); 

    require_number(report, "dram_device_width_bits",config->dram_device_width_bits); 

    require_number(report, "rank_count", config->rank_count); 

    require_number(report, "module_bus_width_bits", config->module_bus_width_bits); 

    if (config->package_type == SPD_PACKAGE_UNKNOWN) 
    {
        add_issue(report, "package_type", SPD_ISSUE_MISSING,
            "DRAM package type is not specified.");  
    } 
    else if ((config->package_type != SPD_PACKAGE_MONOLITHIC) &&
    (config->package_type != SPD_PACKAGE_STACKED)) 
    {
        add_issue(report, "package_type", SPD_ISSUE_INVALID, "Invalid DRAM package type."); 
    }

    require_number(report, "dies_per_package", config->dies_per_package); 

    if (!config->ecc_extension_known) 
    {
        add_issue(report, "ecc_extension_known", SPD_ISSUE_MISSING,
            "ECC extension status is unknown.");  
    }

    check_timing(report, "tck_min", &config->timings.tck_min); 

    check_timing(report, "taa_min", &config->timings.taa_min); 

    check_timing(report, "trcd_min", &config->timings.trcd_min); 

    check_timing(report, "trp_min", &config->timings.trp_min); 

    check_timing(report, "tras_min", &config->timings.tras_min); 

    check_timing(report, "trc_min",&config->timings.trc_min); 

    check_timing(report, "trfc1_min", &config->timings.trfc1_min); 

    check_timing(report, "trfc2_min", &config->timings.trfc2_min); 

    check_timing(report, "trfc4_min", &config->timings.trfc4_min); 

    check_timing(report, "tfaw_min", &config->timings.tfaw_min); 

    check_timing(report, "trrd_s_min", &config->timings.trrd_s_min); 

    check_timing(report, "trrd_l_min", &config->timings.trrd_l_min); 

    check_timing(report, "tccd_l_min", &config->timings.tccd_l_min); 

    require_text(report, "supported_cas_latencies",config->timings.supported_cas_latencies); 

    require_text(report, "module_manufacturer", config->module_manufacturer); 

    require_text(report, "dram_manufacturer", config->dram_manufacturer); 

    require_text(report, "module_part_number", config->module_part_number); 

    if (!config->manufacturer_ids_known) 
    {
        add_issue(report, "manufacturer_ids_known", SPD_ISSUE_MISSING,
            "Manufacturer IDs are not known."); 
    } 
    else 
    {
        if (config->module_manufacturer_id == 0U) 
        {
            add_issue(report, "module_manufacturer_id", SPD_ISSUE_INVALID,
                "Known module manufacturer ID must be non-zero."); 
        } 

        if (config->dram_manufacturer_id == 0U) 
        {
            add_issue(report, "dram_manufacturer_id", SPD_ISSUE_INVALID,
                "Known DRAM manufacturer ID must be non-zero."); 
        }
    } 

    check_document(report, "jedec_document", &config->jedec_document); 

    check_document(report, "dram_document", &config->dram_document); 

    check_document(report, "module_bom", &config->module_bom); 

    check_document(report, "eeprom_document", &config->eeprom_document); 

    check_geometry(config, report);

    add_issue(report, "semantic_validation", SPD_ISSUE_UNVERIFIED,
        "Full DDR4 SPD semantic validation is not yet verified against the controlled specification."); 

    if (!report->basic_values_valid) 
    {
        return SPD_CONFIG_INVALID_VALUE;
    } 

    if (!report->fields_present) 
    {
        return SPD_CONFIG_INCOMPLETE;
    } 
 
    if ((!report->sources_approved) ||
        (!report->semantic_validation_complete)) {
        return SPD_CONFIG_UNVERIFIED_SPECIFICATION;
    }

    return SPD_CONFIG_FIELDS_PRESENT;
}  

SpdConfigStatus spd_config_check(const SpdModuleConfig *config)
{
    SpdConfigReport report;

    if (config == NULL) 
    {
        return SPD_CONFIG_INVALID_ARGUMENT;
    }

    return spd_config_inspect(config, &report); 
}

const char *spd_config_status_string(SpdConfigStatus status)
{
    switch (status) 
    {
        case SPD_CONFIG_INVALID_ARGUMENT:
            return "INVALID_ARGUMENT";

        case SPD_CONFIG_INCOMPLETE:
            return "INCOMPLETE";

        case SPD_CONFIG_FIELDS_PRESENT:
            return "FIELDS_PRESENT";

        case SPD_CONFIG_INVALID_VALUE:
            return "INVALID_VALUE";

        case SPD_CONFIG_UNVERIFIED_SPECIFICATION:
            return "UNVERIFIED_SPECIFICATION";

        default:
            return "UNKNOWN_STATUS";
    }
}       