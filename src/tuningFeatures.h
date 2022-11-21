#ifndef TUNINGFEATURES_H
#define TUNINGFEATURES_H

#include "defs.h"
#include "eval.h"
#include "outposts.h"

#define BIG_FEATURE_NUMBER (69)

enum TuningType{
    ALL,
    OP_ONLY,
    EG_ONLY
};

struct tFeature {
    std::string     name;
    bool            isArray;
    const int      *startValue;
    int             valuesTotal;
    int             padding;
    TuningType      type;

    tFeature () : name("0"), isArray(false), startValue(nullptr),  valuesTotal(1), padding(1), type(ALL) {};
    tFeature (std::string n, bool b, const int * s, int v, int p, TuningType t) :
                  name(n), isArray(b), startValue(s), valuesTotal(v), padding(p), type(t) {};

};

tFeature myFeatures [BIG_FEATURE_NUMBER] = {
};


#endif