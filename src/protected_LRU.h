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

void init_protected_LRU(cache_t*, uint32_t, uint32_t);
void update_protected_LRU(cache_set_t*, cache_blk_t*);
cache_blk_t* get_protected_LRU_victim(cache_set_t*, int);

#endif

