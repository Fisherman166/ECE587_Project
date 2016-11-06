#include <stdio.h>
#include <stdbool.h>
#include "cache.h"
#//include <cstdlib>
#//include <cassert>


//------------------------------
// Useful definitions
//------------------------------

#define LINE_SCORE_BITS         6
#define LINE_SCORE_MASK         ((1 << LINE_SCORE_BITS) - 1)
#define MAX_LINE_SCORE          ((1 << LINE_SCORE_BITS) - 1)
#define HALF_MAX_LINE_SCORE     (1 << (LINE_SCORE_BITS - 1))

// Threshold to select victim lines
#define THRESHOLD_SCORE         24

#define ACCESS_INTERVAL         100000
#define INIT_SCORE_STEP         4

void score_init(struct cache_t * cache);
void score_update_state( struct cache_set_t * access_set , struct cache_blk_t *access_blk, bool cache_hit, int assoc);


struct cache_blk_t * score_select_victim( struct cache_set_t * miss_set, int assoc);



  

