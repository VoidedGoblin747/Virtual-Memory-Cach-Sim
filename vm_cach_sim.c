// CS-3853-001 - Virtual Memory & Cache Sim
// Team #11

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRUCTS
typedef struct 
{
    int cache_kb;
    int block_size;
    int associativity;
    char replacement[8];
    int physical_memory_mb;
    double os_used_percentage;
    long long time_slice;
    char *trace_files[3];
    int trace_count;
}Config;

typedef struct 
{
    unsigned long long total_blocks;
    int tag_bits;
    int index_bits;
    unsigned long long rows;
    unsigned long long overhead_bytes;
    double implementation_kb;
    double cost;
    unsigned long long num_physical_pages;
    unsigned long long num_system_pages;
    int pte_bits;
    unsigned long long total_pt_ram_bytes;
}Calculations;

//FUNCTION PROTOTYPE DECLERATIONS

void parse_args(int argc, char **argv, Config *cfg);
void compute_values(Config *cfg, Calculations *calc);
void print_results(Config *cfg, Calculations *calc);

int main(int argc, char **argv){
    Config cfg = {0};
    Calculations calc = {0};

    parse_args(argc, argv, &cfg);
    compute_values(&cfg, &calc);
    print_results(&cfg, &calc);

    return 0;
}


// FUNCTION DEFS

//Stores into config struct then checks for errors
void parse_args(int argc, char **argv, Config *cfg){

    if (argc == 1){
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    cfg->cache_kb = 0;
    cfg->block_size = 0;
    cfg->associativity = 0;
    strcpy(cfg->replacement, "");
    cfg->physical_memory_mb = 0;
    cfg->os_used_percentage = -1.0;
    cfg->time_slice = -2;
    cfg->trace_count = 0;

    for(int i = 1; i < argc; ++i){

        // Shows the help screen :D
        if (strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            exit(EXIT_SUCCESS);
        } 
        //Cache Size
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            cfg->cache_kb = atoi(argv[++i]);
        }

        else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            cfg->cache_kb = atoi(argv[++i]);
        }

        else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            cfg -> associativity = atoi(argv[++i]);
        }

        // Replacement policy
        else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            strncpy(cfg->replacement, argv[++i], sizeof(cfg->replacement) - 1);
            cfg->replacement[sizeof(cfg->replacement) - 1] = '\0';
        }
        // Physical memory (MB)
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            cfg->physical_memory_mb = atoi(argv[++i]);
        }
        // OS memory percent
        else if (strcmp(argv[i], "-u") == 0 && i + 1 < argc) {
            cfg->os_used_percentage = atof(argv[++i]);
        }
        // Time slice
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            cfg->time_slice = atoll(argv[++i]);
        }
        // Trace file(s)
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            if (cfg->trace_count < 3)
                cfg->trace_files[cfg->trace_count++] = argv[++i];
            else
                fprintf(stderr, "Warning: Ignoring extra -f file beyond 3.\n");
        }
        // Invalid argument
        else {
            fprintf(stderr, "Error: Unknown or incomplete argument: %s\n", argv[i]);
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

// Computing values
// Computing values
void compute_values(Config *cfg, Calculations *calc) {
    unsigned long long cache_bytes = cfg->cache_kb * 1024;
    unsigned long long physical_bytes = cfg->physical_memory_mb * 1024 * 1024;
    unsigned long long page_size = 4096;

    // Cache calculations
    calc->total_blocks = cache_bytes / cfg->block_size;
    calc->rows = calc->total_blocks / cfg->associativity;
    calc->index_bits = (int)log2(calc->rows);
    int offset_bits = (int)log2(cfg->block_size);
    calc->tag_bits = 32 - calc->index_bits - offset_bits;

    calc->overhead_bytes = (calc->total_blocks * (1 + calc->tag_bits)) / 8;
    calc->implementation_kb = (cache_bytes + calc->overhead_bytes) / 1024.0;
    calc->cost = calc->implementation_kb * 0.07;

    // Physical memory calculations
    calc->num_physical_pages = physical_bytes / page_size;
    calc->num_system_pages = (unsigned long long)(calc->num_physical_pages * (cfg->os_used_percentage / 100.0));
    calc->pte_bits = 1 + (int)log2(calc->num_physical_pages);
    calc->total_pt_ram_bytes = (512 * 1024 * calc->pte_bits / 8) * cfg->trace_count;
}

// Printing per the requirments
void print_results(Config *cfg, Calculations *calc){
        printf("MILESTONE #1: Input Parameters and Calculated Values\n\n");
    printf("Cache Simulator - CS 3853 – Team #11\n\n");

    printf("Trace File(s):\n");
    for (int i = 0; i < cfg->trace_count; ++i) {
        printf("        %s\n", cfg->trace_files[i]);
    }

    printf("\n***** Cache Input Parameters *****\n\n");
    printf("Cache Size:                     %d KB\n", cfg->cache_kb);
    printf("Block Size:                     %d bytes\n", cfg->block_size);
    printf("Associativity:                  %d\n", cfg->associativity);
    printf("Replacement Policy:             %s\n", cfg->replacement);
    printf("Physical Memory:                %d MB\n", cfg->physical_memory_mb);
    printf("Percent Memory Used by System:  %.1f%%\n", cfg->os_used_percentage);
    printf("Instructions / Time Slice:      %lld\n", cfg->time_slice);

    printf("\n***** Cache Calculated Values *****\n\n");
    printf("Total # Blocks:                 %llu\n", calc->total_blocks);
    printf("Tag Size:                       %d bits\n", calc->tag_bits);
    printf("Index Size:                     %d bits\n", calc->index_bits);
    printf("Total # Rows:                   %llu\n", calc->rows);
    printf("Overhead Size:                  %llu bytes\n", calc->overhead_bytes);
    printf("Implementation Memory Size:     %.2f KB\n", calc->implementation_kb);
    printf("Cost:                           $%.2f\n", calc->cost);

    printf("\n***** Physical Memory Calculated Values *****\n\n");
    printf("Number of Physical Pages:       %llu\n", calc->num_physical_pages);
    printf("Number of Pages for System:     %llu\n", calc->num_system_pages);
    printf("Size of Page Table Entry:       %d bits\n", calc->pte_bits);
    printf("Total RAM for Page Table(s):    %llu bytes\n", calc->total_pt_ram_bytes);
}