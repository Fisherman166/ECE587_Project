//*****************************************************************************
//
// Filename: protected_LRU.h
// Author: Sean Koppenhafer
// Project: ECE587
//
//*****************************************************************************

#ifndef PROTECTED_LRU_H
#define PROTECTED_LRU_H

#include <stdint.h>
#include "cache.h"

void init_protected_LRU(struct cache_t*, unsigned int, unsigned int);
void update_protected_LRU(struct cache_set_t*, struct cache_blk_t*);
struct cache_blk_t* get_protected_LRU_victim(struct cache_set_t*, int);

#endif

