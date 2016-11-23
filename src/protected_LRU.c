//*****************************************************************************
//
// Filename: protected_LRU.c
// Author: Sean Koppenhafer
// Project: ECE587
//
//*****************************************************************************

#include <stdbool.h>
#include <stdio.h>
#include "protected_LRU.h"
#include "misc.h"

//#define DEBUG

static uint32_t max_counter_value = 0;
static uint32_t ways_to_save_on_eviction = 0;

typedef struct {
    struct cache_blk_t* cache_line;
    uint32_t access_number;
    bool ignore;    // Is one of the ways to save
} validWay;

static FILE* DEBUG_FILE = NULL;

static void open_debug() {
    DEBUG_FILE = fopen("pLRU_debug.log", "w");
    if(DEBUG_FILE == NULL) {
        fatal("Failed to open pLRU debug FILE");
    }
}

static void write_debug(const char* string) {
    fprintf(DEBUG_FILE, "%s", string);
    fflush(DEBUG_FILE);
}

//*****************************************************************************
// Functions
//*****************************************************************************
void init_protected_LRU(struct cache_t* cache, unsigned int counter_value_max, unsigned int ways_to_save) {
    if(ways_to_save > (unsigned int)cache->assoc) {
        fatal("ERROR: Ways to save: %u is greater than the assoc: %u of the cache\n", ways_to_save, (unsigned int)cache->assoc);
    }

    max_counter_value = (uint32_t)counter_value_max;
    ways_to_save_on_eviction = (uint32_t)ways_to_save;
    uint32_t i;

    for(i = 0; i < cache->nsets; i++) {
        struct cache_set_t* current_set = &cache->sets[i];
        struct cache_blk_t* current_way = current_set->way_head;
        // Traverse all of the ways and inititalize the counter to 0
        while(current_way != NULL) {
            current_way->access_counter = 0;
            current_way = current_way->way_next;
        }
    }
    #ifdef DEBUG
    open_debug();
    #endif
}

void update_protected_LRU(struct cache_set_t* hit_set, struct cache_blk_t* hit_block) {
    hit_block->access_counter++;

    // Divide all counters in the set by 2 to avoid having lines that get accessed
    // a lot from getting stuck in the cache forever
    if(hit_block->access_counter >= max_counter_value) {
        struct cache_blk_t* current_way = hit_set->way_head;
        while(current_way != NULL) {
            current_way->access_counter >>= 1;
            current_way = current_way->way_next;
        }
    }
}

struct cache_blk_t* get_protected_LRU_victim(struct cache_set_t* miss_set, int assoc) {
    #ifdef DEBUG
    char text[200];
    #endif

    // Build up an array of valid ways
    validWay valid_ways[assoc];
    struct cache_blk_t* current_way = miss_set->way_head;
    uint32_t way_number = 0;
    while(current_way != NULL) {
        valid_ways[way_number].cache_line = current_way;
        valid_ways[way_number].access_number = current_way->access_counter;
        valid_ways[way_number].ignore = false;
        current_way = current_way->way_next;
        way_number++;
    }

    // Save the cache lines with the N largest access counter values.
    // Where N = ways_to_save_on_eviction
    uint32_t i;
    uint32_t way = 0;
    for(i = 0; i < ways_to_save_on_eviction; i++) {
        uint32_t largest_counter = 0;
        validWay* largest_counter_way = NULL;
        for(way = 0; way < assoc; way++) {
            #ifdef DEBUG
            sprintf(text, "Current access count = %u, index = %u\n", valid_ways[way].access_number, way);
            write_debug(text);
            #endif

            if(valid_ways[way].ignore) continue;
            if( (largest_counter_way == NULL) || (valid_ways[way].access_number > largest_counter) ) {
                largest_counter = valid_ways[way].access_number;
                largest_counter_way = &valid_ways[way];
            }
        }
        #ifdef DEBUG
        sprintf(text, "Counter to ignore = %u\n", largest_counter_way->cache_line->access_counter);
        write_debug(text);
        #endif
        largest_counter_way->ignore = true;
    }

    // Find the LRU of the remaining ways, clear the counter and return a pointer to that line
    uint32_t LRU_stack_position = 0;
    for(way = 0; way < assoc; way++) {
        if(valid_ways[way].ignore) continue;
        if( way > LRU_stack_position ) LRU_stack_position = way;
    }
    #ifdef DEBUG
    sprintf(text, "Remove way with access count = %u, index = %u\n", valid_ways[LRU_stack_position].cache_line->access_counter, LRU_stack_position);
    write_debug(text);
    #endif
    valid_ways[LRU_stack_position].cache_line->access_counter = 0;
    return valid_ways[LRU_stack_position].cache_line;
}

