//#include <cstdlib>
#include <stdio.h>
#include "score.h"
#include "misc.h"
// *************************************************
//  global for now will change to a struct 
//*************************************************
  	int initialScore;
    bool initialScoreUp;
    
    int accessCounter;
    int prevMisses;
    int currMisses;

    //--------------------
    // Statistics
    //--------------------
    // Total scores of all lines in a set
    unsigned long long int totalScores;
    // Update counter, counts the number of updates
    unsigned long long int updateCounter;
    
    // Total score range of a set
    unsigned long long int totalScoreRanges;
    // Total lines under thredshold
    unsigned long long int totalNumLines;
    // Total min scores
    unsigned long long int totalMinScores;
    // Total max scores
    unsigned long long int totalMaxScores;
    // Score range counter
    unsigned long long int scoreRangeCounter;
    // Number of times no lines under threshold
    unsigned long long int noLinesUnderThreshold;
    // Total number of select victim
    unsigned long long int numLinesEvicted;
    // Total initial scores
    unsigned long long int totalInitScores;
    // Number of intervals
    unsigned long long int numIntervals;


// ***************************************************************************
    // Init score
void initScore(struct cache_blk_t*  self) { self->score = 0; }

    // Increase the score
void incrScore(struct cache_blk_t * self) {
	self->score += 40;
	if (self->score > MAX_LINE_SCORE) self->score = MAX_LINE_SCORE;
    }

    // Decrease the score
void decrScore(struct cache_blk_t*  self) {
     self->score -= 1;
     if (self->score < 0) self->score = 0;
    }

void setInitScore(struct cache_blk_t*  self,int _score) {
 	self->score = _score; }

    // Return the score

int getScore(struct cache_blk_t*  self) { return self->score; }


/* mod_begin 1107 - Functions */
void score_init(struct cache_t * cache)
{
    int numsets = cache->nsets;

    // Lines
    for(unsigned int setIndex=0; setIndex<numsets; setIndex++) {
        struct cache_set_t * current_set = &cache->sets[setIndex];
        struct cache_blk_t * current_way = current_set->way_head;
        while (current_way != NULL ){
			initScore(current_way);
            current_way= current_way->way_next;
        }
	}
    // Statistics
    totalScores = 0;
    updateCounter = 0;
    totalScoreRanges = 0;
    totalNumLines = 0;
    totalMinScores = 0;
    totalMaxScores = 0;
    scoreRangeCounter = 0;
    noLinesUnderThreshold = 0;
    numLinesEvicted = 0;
    numIntervals = 0;
    totalInitScores = 0;
}



void score_update_state( struct cache_set_t * access_set , struct cache_blk_t *access_blk, bool cache_hit, int assoc)
{
  	struct cache_blk_t * replSet = access_set->way_head;


    //----------------------------------------------
    // Update scores
    //----------------------------------------------
    for (unsigned int i = 0; i < assoc; i++) {				// updates the set
        if (&(replSet[i]) == access_blk) {							// access block 
            if (cache_hit) {								// if a hit 
                incrScore(&(replSet[i]));						// increase the scrore
            } else {						
                setInitScore(&(replSet[i]),initialScore);		// on miss set initial score  
            }
        
        } else {
            decrScore(&(replSet[i]));							// other members of the set not hit score decreased
        }
    }
    

    //----------------------------------------------
    // Dynamically change initial score
    //----------------------------------------------
    accessCounter++;
    if (!cache_hit) currMisses++;
    
    if (accessCounter == ACCESS_INTERVAL) {
        initialScoreUp ^= (currMisses > prevMisses);
        
        initialScore += initialScoreUp ? INIT_SCORE_STEP : -INIT_SCORE_STEP;
        
        if (initialScore > MAX_LINE_SCORE) initialScore = MAX_LINE_SCORE;
        if (initialScore < 0) initialScore = 0;
    
        prevMisses = currMisses;
        currMisses = 0;

        accessCounter = 0;
        
        // Stats
        numIntervals++;
        totalInitScores += initialScore;
    }
}




struct cache_blk_t * score_select_victim( struct cache_set_t * miss_set, int assoc)
{
    struct cache_blk_t * way = NULL;

    //----------------------------------------------
    // Selection policy: Randomly select lines with scores under threshold
    //----------------------------------------------
    int threshold_score = THRESHOLD_SCORE;
    
    unsigned int num_lines = 0;
    int maxScore = 0;
    int minScore = MAX_LINE_SCORE;
    bool found_lines = true;
    
    // Get the number of lines with scores under threshold
    int minScoreAll = MAX_LINE_SCORE;
    struct cache_blk_t * miss_blk = miss_set->way_head; 

    for (unsigned int i = 0; i < assoc; i++) {
        int score = getScore(&(miss_blk[i]));
        if (score < threshold_score) {
            num_lines++;
            
            if (maxScore < score) maxScore = score;
            if (minScore > score) minScore = score;
        }
        
        if (minScoreAll > score) {
            way =&(miss_blk[i]) ;
            minScoreAll = score;
        }
    }
   // int time t;
    // Select the line if there is at least one line available
    if (num_lines > 0) {
        srand((unsigned) time(NULL));
        unsigned int line_number = rand() % num_lines;
        unsigned int line_cnt = 0;
        
        for (unsigned int i = 0; i < assoc; i++) {
            if (getScore(&(miss_blk[i])) < threshold_score) {
                if (line_cnt == line_number) {
                    way =&(miss_blk[i]) ;
                    break;
                }
            
                line_cnt++;
            }
        }
    
    } else {
        
        found_lines = false;
    }

    //----------------------------------------------
    // Update stats
    //----------------------------------------------
    if (found_lines) {
        totalScoreRanges += maxScore - minScore;
        totalNumLines += num_lines;
        scoreRangeCounter++;
        
        totalMinScores += minScore;
        totalMaxScores += maxScore;
    
    } else {
        noLinesUnderThreshold++;
    }
    
    numLinesEvicted++;
    
    //----------------------------------------------
    // Return
    //----------------------------------------------
    return way;
}

