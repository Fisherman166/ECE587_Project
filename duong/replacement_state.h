#ifndef REPL_STATE_H
#define REPL_STATE_H

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This file is distributed as part of the Cache Replacement Championship     //
// workshop held in conjunction with ISCA'2010.                               //
//                                                                            //
//                                                                            //
// Everyone is granted permission to copy, modify, and/or re-distribute       //
// this software.                                                             //
//                                                                            //
// Please contact Aamer Jaleel <ajaleel@gmail.com> should you have any        //
// questions                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cassert>
#include "utils.h"
#include "crc_cache_defs.h"

/* mod_begin 1109 */
#include <vector>
#include <map>
#include <iomanip>

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
/* mod_end 1109 */

// Replacement Policies Supported
typedef enum 
{
    CRC_REPL_LRU        = 0,
    CRC_REPL_RANDOM     = 1,
    CRC_REPL_CONTESTANT = 2
} ReplacemntPolicy;

// Replacement State Per Cache Line
typedef struct
{
    UINT32  LRUstackposition;

    // CONTESTANTS: Add extra state per cache line here
/* mod_begin 1101 - line states */
    // This is the score of a line. 
    INT32 score;
    
    // Init score
    void initScore() { score = 0; }

    // Increase the score
    void incrScore() {
        score += 40;
        if (score > MAX_LINE_SCORE) score = MAX_LINE_SCORE;
    }
    
    // Decrease the score
    void decrScore() {
        score -= 1;
        if (score < 0) score = 0;
    }
    
    void setInitScore(INT32 _score) { score = _score; }
    
    // Return the score
    INT32 getScore() { return score; }
    
/* mod_end 1101 */
    
} LINE_REPLACEMENT_STATE;


// The implementation for the cache replacement policy
class CACHE_REPLACEMENT_STATE
{

  private:
    UINT32 numsets;
    UINT32 assoc;
    UINT32 replPolicy;
    
    LINE_REPLACEMENT_STATE   **repl;

    COUNTER mytimer;  // tracks # of references to the cache

    // CONTESTANTS:  Add extra state for cache here
/* mod_begin 1102 - common structure */
    INT32 initialScore;
    bool initialScoreUp;
    
    INT32 accessCounter;
    INT32 prevMisses;
    INT32 currMisses;

    //--------------------
    // Statistics
    //--------------------
    // Total scores of all lines in a set
    unsigned long long totalScores;
    // Update counter, counts the number of updates
    unsigned long long updateCounter;
    
    // Total score range of a set
    unsigned long long totalScoreRanges;
    // Total lines under thredshold
    unsigned long long totalNumLines;
    // Total min scores
    unsigned long long totalMinScores;
    // Total max scores
    unsigned long long totalMaxScores;
    // Score range counter
    unsigned long long scoreRangeCounter;

    // Number of times no lines under threshold
    unsigned long long noLinesUnderThreshold;
    // Total number of select victim
    unsigned long long numLinesEvicted;

    // Total initial scores
    unsigned long long totalInitScores;

    // Number of intervals
    unsigned long long numIntervals;
/* mod_end 1102 */
    

  public:

    // The constructor CAN NOT be changed
    CACHE_REPLACEMENT_STATE( UINT32 _sets, UINT32 _assoc, UINT32 _pol );

    INT32  GetVictimInSet( UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, UINT32 assoc, Addr_t PC, Addr_t paddr, UINT32 accessType );
    void   UpdateReplacementState( UINT32 setIndex, INT32 updateWayID );

    void   SetReplacementPolicy( UINT32 _pol ) { replPolicy = _pol; } 
    void   IncrementTimer() { mytimer++; } 

    void   UpdateReplacementState( UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
                                   UINT32 tid, Addr_t PC, UINT32 accessType, bool cacheHit );

    ostream&   PrintStats( ostream &out);

  private:
    
    void   InitReplacementState();
    INT32  Get_Random_Victim( UINT32 setIndex );

    INT32  Get_LRU_Victim( UINT32 setIndex );
    void   UpdateLRU( UINT32 setIndex, INT32 updateWayID );

/* mod_begin 1108 - Functions */
    // Initialization
    void   sysarchInit();

    // Select victim cache
    INT32  sysarchSelectVictim(
        UINT32 tid, UINT32 setIndex, const LINE_STATE *vicSet, 
        UINT32 assoc, Addr_t PC, Addr_t paddr, UINT32 accessType );

    // Update replacement state 
    void   sysarchUpdateState( 
        UINT32 setIndex, INT32 updateWayID, const LINE_STATE *currLine, 
        UINT32 tid, Addr_t PC, UINT32 accessType, bool cacheHit );

    // Print stats
    ostream&   sysarchPrintStats( ostream &out);
/* mod_end 1108 */
};


#endif
