#ifndef TREADHOLDER_H
#define TREADHOLDER_H


#include "search.h"
#include "uci.h"


/**
 * 
 * @brief structure that holds info needed for multy-threading
 */ 
struct ThreadHolder{
    Search * cSearch[MAX_THREAD];
};


#endif