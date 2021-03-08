#include "eval.h"

#ifdef _TUNE_
    #define TRACK (1)
#else
    #define TRACK (0)
#endif

// This structure holds all features we want to tune
// that is traced as we Evalute position
struct posFeatured{

    //whole finalEval
    int FinalEval;

    // Simple features
    int BishopPair[2];

    //Array features

};

struct eTrace {
    int16_t index;
    int16_t count;

    eTrace(int16_t i, int16_t c) : index(i), count(c) {}
};

struct tEntry {
    int16_t statEval;
    int16_t phase;
    int16_t tracesCount;
    int FinalEval;
    double result;
    double pFactors[2];
    bool stm;
    eTrace *traces;
};


 /**
  * @brief Main tuning constants are defined here
  * @{
  */
  const std::string TUNING_DATA        = "BOOK.txt";
  const int         TUNING_POS_COUNT   = 9996883; //9996883
  const int         TUNING_THREADS     = 16;
  const int         TUNING_TERMS_COUNT = 1;
  const int         TUNING_BATCH_SIZE  = 0;
  const int         TUNIGN_MAX_ITER    = 10000;
  const int         TUNIGN_PRINT       = 50; 
  const double      TUNING_K           = 2.0;
  const int         TUNING_L_STEP      = 250;
  const double      TUNING_L_DROP      = 1.0;
  const double      TUNING_L_RATE      = 1.0;

  const int         TUNING_STACK_SIZE = ((int)((double) TUNING_POS_COUNT * TUNING_TERMS_COUNT / 64));
  /**@}*/

  // 
  
  typedef double tValueHolder[TUNING_TERMS_COUNT][2];
  typedef double featureCoeff[TUNING_TERMS_COUNT];


  #define strFail (std::string::npos)
 /** 
  * @brief run the tuner using data specified in the TUNING_DATA
  *        All other stuff is private because it is not needed anywhere else.
  */
 void TunerStart();

 /**
  * @brief print simple term (op, eg)
  * 
  * @param name  name of the term
  * @param value OPENING value of the term
  * @param value ENDGAME value of the term
  * 
  */ 
 void EvalTermPrint(std::string, double, double, double, double);

 void EvalTermInitiate(tValueHolder);

 bool InitTuningPositions(tEntry*);

 void InitSinglePosition(int, std::string, tEntry*);

 void InitCoefficients(featureCoeff);

 void InitETraces(featureCoeff, tEntry*);

 void CalculateGradient(tEntry*, tValueHolder, tValueHolder);

 void UpdateSingleGrad(tEntry*, tValueHolder, tValueHolder);

 double Sigmoid(double);

 double TuningEval(tEntry*, tValueHolder);

 double TunedError(tEntry*, tValueHolder);