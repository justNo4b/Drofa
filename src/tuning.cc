#include "defs.h"
#include "eval.h"
#include "tuning.h"
#include "board.h"
#include <limits>
#include <fstream>
#include <cstring>
#include <math.h>
#include <iomanip>



eTrace *eTraceStack;
int eTraceStackSize;

posFeatured ft, zero;

void TunerStart(){
    // Startup messages
    std::cout << "This is special TUNING build \n\n" << std::endl;
    std::cout << "Implementation is heavily based on:" << std::endl;
    std::cout << "A. Grant (Ethereal author) tuning paper" << std::endl;
    std::cout << "Terje Kirien (Weiss author) tuning implementation \n\n" << std::endl;
    std::cout << "Starting TUNING \n\n" << std::endl;

    //Declare stuff
    zero = {0};
    tEntry *entries = (tEntry*) calloc(TUNING_POS_COUNT, sizeof(tEntry));
    eTraceStack      = (eTrace*) calloc(TUNING_STACK_SIZE,  sizeof(eTrace));

    tValueHolder diffTerms = {{0}}, gradTerms = {{0}};
    double rate  = TUNING_L_RATE;
    double error = 0;

    //Initiate our terms
    tValueHolder currTerms = {{0}};
    EvalTermInitiate(currTerms);

    // Initialize training data
    // Program will exit if there is smth wrong with our data
    if (!InitTuningPositions(entries))
        return;

    // Perform tuninig
    // For now we do only with all data at once, no partial chunks.

    std::cout << std::setprecision(16);

    for (int epoch = 0; epoch < TUNIGN_MAX_ITER; epoch++){

        // Calculate gradient
        tValueHolder gradient = {{0}};
        CalculateGradient(entries, gradient, diffTerms);

        //adjust stuff
        for (int i = 0; i < TUNING_TERMS_COUNT; i++) {
                gradTerms[i][OPENING] += pow((TUNING_K / 200.0) * gradient[i][OPENING] / TUNING_POS_COUNT, 2.0);
                gradTerms[i][ENDGAME] += pow((TUNING_K / 200.0) * gradient[i][ENDGAME] / TUNING_POS_COUNT, 2.0);
                diffTerms[i][OPENING] += (TUNING_K / 200.0) * (gradient[i][OPENING] / TUNING_POS_COUNT) * (rate / sqrt(1e-8 + gradTerms[i][OPENING]));
                diffTerms[i][ENDGAME] += (TUNING_K / 200.0) * (gradient[i][ENDGAME] / TUNING_POS_COUNT) * (rate / sqrt(1e-8 + gradTerms[i][ENDGAME]));
        }
        
        
        // Learning drop rate
        if (epoch % TUNING_L_STEP == 0){
            rate = rate / TUNING_L_DROP;
        }
        // Print new terms
        if (epoch % TUNIGN_PRINT == 0){
            error = TunedError(entries, diffTerms);
            std::cout << " IterationNum = " + std::to_string(epoch) + " Error: " <<  error;
            EvalTermPrint("\n BISHOP_PAIR_BONUS", currTerms[0][OPENING], currTerms[0][ENDGAME], diffTerms[0][OPENING], diffTerms[0][ENDGAME]);
        } 
    }

}



void EvalTermPrint(std::string name, double opV, double egV, double opDiff, double egDiff){
    std::string op = std::to_string( (opV + opDiff));
    std::string eg = std::to_string( (egV + egDiff));
    std::cout << name + " = S(" + op + "," + eg + ");"<< std::endl;
}

void EvalTermInitiate(tValueHolder cTerms){
    int c = 0;
    // Setup terms for tuning
    // 1. Simple terms (op, eg) only
    cTerms[c][0] = opS(Eval::BISHOP_PAIR_BONUS);
    cTerms[c][1] = egS(Eval::BISHOP_PAIR_BONUS);


}

bool InitTuningPositions(tEntry * positionList){
    std::string myFen;
    std::ifstream file(TUNING_DATA);

    // Exit the program if file is not found or cannot be opened
    if (!file){
        std::cout << "Failed to open file. Exit"<< std::endl;
        return false;
    }
    else
    {
        std::cout << "File opened. Processing positions"<< std::endl;
    }

    // Initialize our positions in cycle
    for (int pCount = 0; pCount < TUNING_POS_COUNT; pCount++){
        std::getline(file, myFen);

        if (myFen.find("[1.0]") !=  strFail) {
            positionList[pCount].result = 1.0;
        }else if (myFen.find("[0.5]") !=  strFail) {
            positionList[pCount].result = 0.5;
        }else if (myFen.find("[0.0]") !=  strFail) {
            positionList[pCount].result = 0.0;
        }else {
            std::cout << "Bad position format. Exit"<< std::endl;
            return false;
        }

        // our position has a result, procceed with initialization
        InitSinglePosition(pCount, myFen, positionList);

    }


    file.close();
    std::cout << "Done with positions \n\n"<< std::endl;
    return true;
}

void InitSinglePosition(int pCount, std::string myFen, tEntry * positionList){
    // 1. Construct position for us.
    Board b = Board(myFen);

    // 2. Calculate phase-related stuff
    double phase = (double) Eval::getPhase(b);
    positionList[pCount].phase = phase;
    // Basically we transformed and divided in two expression below
    // final_eval = ((opS(score) * (MAX_PHASE - phase)) + (egS(score) * phase)) / MAX_PHASE;
    positionList[pCount].pFactors[OPENING] = 1 - phase / 256.0;
    positionList[pCount].pFactors[ENDGAME] = 0 + phase / 256.0;

    // 3. Prepare for evaluatin and save evaluation - stuff
    ft  = zero; 
    featureCoeff newCoeffs; 
    positionList[pCount].stm = b.getActivePlayer();
    positionList[pCount].statEval = b.getActivePlayer() == WHITE ? Eval::evaluate(b, b.getActivePlayer()) : -Eval::evaluate(b, b.getActivePlayer());
    // after we did evaluate out ft containg fevaulated featutres
    // so now we can save it
    InitCoefficients(newCoeffs);
    InitETraces(newCoeffs, &positionList[pCount]);

    positionList[pCount].FinalEval = ft.FinalEval;

}

void InitCoefficients(featureCoeff coeff){
    int i = 0;

    // Insert features here
    // Basicall we calculate if feature difference, so we dont hold it if it is 0
    coeff[i++] = ft.BishopPair[WHITE] - ft.BishopPair[BLACK];


    // end

    if (i != TUNING_TERMS_COUNT){
        std::cout << "Term count error. \n\n"<< std::endl;
        exit(1);
    }
}

void InitETraces(featureCoeff coeffs, tEntry* entry){
    int length = 0;

    // 1. Calculate lenght by counting non-null featureCoeffs
    for (int j = 0; j < TUNING_TERMS_COUNT; j++){
        if (coeffs[j] != 0){
            length++;
        }
    }

    // in case we have less mermory for coefficients than is allocated already
    // we need to allocate some more

    if (length > eTraceStackSize){
        eTraceStackSize = TUNING_STACK_SIZE;
        eTraceStack = (eTrace*) calloc(TUNING_STACK_SIZE,  sizeof(eTrace));
        std::cout << "Allocating additional RAM for the tuner \n"<< std::endl;
    }

    // Claim part of the stack
    entry->traces = eTraceStack;
    entry->tracesCount = length;
    eTraceStack += length;
    eTraceStackSize -= length;

    //setup each eTrace for this tEntry
    int indexNum = 0;
    for (int j = 0; j < TUNING_TERMS_COUNT; j++){
        if (coeffs[j] != 0){
            entry->traces[indexNum] = eTrace(j, coeffs[j]);
            indexNum++;
        }
    }
}

void CalculateGradient(tEntry* entries, tValueHolder grad, tValueHolder diff){

    #pragma omp parallel shared(grad)
    {
         tValueHolder local = {{0}};

            for (int i = 0; i < TUNING_POS_COUNT; i++){
                UpdateSingleGrad( &entries[i], local, diff);
            }

            for (int i = 0; i < TUNING_TERMS_COUNT; i++){
                grad[i][OPENING] += local[i][OPENING];
                grad[i][ENDGAME] += local[i][ENDGAME];
            }
    }
}

void UpdateSingleGrad(tEntry* entry, tValueHolder local, tValueHolder diff){
    double eval = TuningEval(entry, diff);
    double sigm = Sigmoid(eval);
    double X = (entry->result - sigm) * sigm * (1.0 - sigm);

    double opBase = X * entry->pFactors[OPENING];
    double egBase = X * entry->pFactors[ENDGAME];

    for (int i = 0; i < entry->tracesCount; i++){
        int index = entry->traces[i].index;
        int count = entry->traces[i].count;

        local[index][OPENING] += (double) opBase * count;
        local[index][ENDGAME] += (double) egBase * count;

    }
}

double Sigmoid(double eval){
    return 1.0 / (1.0 + exp(( -TUNING_K * eval) / 400.0));
}

double TuningEval(tEntry* entry, tValueHolder diff){
    double opScore = opS(entry->FinalEval);
    double egScore = egS(entry->FinalEval);

    // Save any modifications for MG or EG for each evaluation type
    for (int i = 0; i < entry->tracesCount; i++) {
        opScore += (double) entry->traces[i].count * diff[entry->traces[i].index][OPENING];
        egScore += (double) entry->traces[i].count * diff[entry->traces[i].index][ENDGAME];
    }

    double final_eval = ((opScore * (256.0 - entry->phase)) + (egScore * entry->phase)) / 256.0;              

    return final_eval + (entry->stm == WHITE ? 5 : -5);
}

double TunedError(tEntry* entries, tValueHolder diff) {

    double total = 0.0;

    #pragma omp parallel shared(total)
    {
        #pragma omp for schedule(static, TUNING_POS_COUNT / TUNING_THREADS) reduction(+:total)
        for (int i = 0; i < TUNING_POS_COUNT; i++)
            total += pow(entries[i].result - Sigmoid(TuningEval(&entries[i], diff)), 2);
    }

    return total / (double) TUNING_POS_COUNT;
}