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

#ifdef _TUNE_

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

    std::cout << "\n Calculating K... " << std::endl;
    double K = TUNING_K; //CalculateFactorK(entries);
    std::cout << "\n Optimal K = " << K << std::endl;

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
            std::cout << "\n\n IterationNum = " + std::to_string(epoch) + " Error: " <<  error;
            std::cout << "\n Printing Terms: \n";
            PrintTunedParams(currTerms, diffTerms);
        } 
    }

}

void EvalTermPrint(std::string name, double opV, double egV, double opDiff, double egDiff){
    std::string op = std::to_string( (int)(opV + opDiff));
    std::string eg = std::to_string( (int)(egV + egDiff));
    std::cout << name + " = gS(" + op + "," + eg + ");"<< std::endl;
}

void EvalArrayPrint(std::string name, tValueHolder current, tValueHolder diff, int head, int length){
    std::cout << name + "[" + std::to_string(length) + "] = {"<< std::endl;
    for (int i = 0; i < length; i++){
        std::string op = std::to_string( (int)(current[head + i][OPENING] + diff[head + i][OPENING]));
        std::string eg = std::to_string( (int)(current[head + i][ENDGAME] + diff[head + i][ENDGAME]));  
        std::cout << " gS(" + op + "," + eg + "),";
    }


    std::cout << "\n};"<< std::endl;
}

void EvalTermInitiate(tValueHolder cTerms){
    int c = 0;
    // Setup terms for tuning
    // 1. Simple terms (op, eg) only
    // a. PieceValues
    cTerms[c][OPENING] = opS(Eval::MATERIAL_VALUES[PAWN]);
    cTerms[c][ENDGAME] = egS(Eval::MATERIAL_VALUES[PAWN]);
    c++;
    cTerms[c][OPENING] = opS(Eval::MATERIAL_VALUES[ROOK]);
    cTerms[c][ENDGAME] = egS(Eval::MATERIAL_VALUES[ROOK]);
    c++;
    cTerms[c][OPENING] = opS(Eval::MATERIAL_VALUES[KNIGHT]);
    cTerms[c][ENDGAME] = egS(Eval::MATERIAL_VALUES[KNIGHT]);
    c++;
    cTerms[c][OPENING] = opS(Eval::MATERIAL_VALUES[BISHOP]);
    cTerms[c][ENDGAME] = egS(Eval::MATERIAL_VALUES[BISHOP]);
    c++;
    cTerms[c][OPENING] = opS(Eval::MATERIAL_VALUES[QUEEN]);
    cTerms[c][ENDGAME] = egS(Eval::MATERIAL_VALUES[QUEEN]);
    c++;
    // b. Other terms
    cTerms[c][OPENING] = opS(Eval::BISHOP_PAIR_BONUS);
    cTerms[c][ENDGAME] = egS(Eval::BISHOP_PAIR_BONUS);
    c++;
    cTerms[c][OPENING] = opS(Eval::KING_HIGH_DANGER);
    cTerms[c][ENDGAME] = egS(Eval::KING_HIGH_DANGER);
    c++;
    cTerms[c][OPENING] = opS(Eval::KING_MED_DANGER);
    cTerms[c][ENDGAME] = egS(Eval::KING_MED_DANGER);
    c++;
    cTerms[c][OPENING] = opS(Eval::KING_LOW_DANGER);
    cTerms[c][ENDGAME] = egS(Eval::KING_LOW_DANGER);
    c++;
    cTerms[c][OPENING] = opS(Eval::KING_SAFE);
    cTerms[c][ENDGAME] = egS(Eval::KING_SAFE);
    c++;
    cTerms[c][OPENING] = opS(Eval::PAWN_SUPPORTED);
    cTerms[c][ENDGAME] = egS(Eval::PAWN_SUPPORTED);
    c++;
    cTerms[c][OPENING] = opS(Eval::DOUBLED_PAWN_PENALTY);
    cTerms[c][ENDGAME] = egS(Eval::DOUBLED_PAWN_PENALTY);
    c++;
    cTerms[c][OPENING] = opS(Eval::ISOLATED_PAWN_PENALTY);
    cTerms[c][ENDGAME] = egS(Eval::ISOLATED_PAWN_PENALTY);
    c++;
    cTerms[c][OPENING] = opS(Eval::BISHOP_RAMMED_PENALTY);
    cTerms[c][ENDGAME] = egS(Eval::BISHOP_RAMMED_PENALTY);
    c++;

    for (int j = 0; j < 8; j++){
        cTerms[c][OPENING] = opS(Eval::PASSED_PAWN_RANKS[j]);
        cTerms[c][ENDGAME] = egS(Eval::PASSED_PAWN_RANKS[j]);
        c++;
    }

    for (int j = 0; j < 8; j++){
        cTerms[c][OPENING] = opS(Eval::PASSED_PAWN_FILES[j]);
        cTerms[c][ENDGAME] = egS(Eval::PASSED_PAWN_FILES[j]);
        c++;
    }


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
    double phase = simplifyPhaseCalculation(b);
    positionList[pCount].phase = (phase * 256 + 12 ) / 24;
    // Basically we transformed and divided in two expression below
    // final_eval = ((opS(score) * (MAX_PHASE - phase)) + (egS(score) * phase)) / MAX_PHASE;
    positionList[pCount].pFactors[OPENING] = 1 - phase / 24.0;
    positionList[pCount].pFactors[ENDGAME] = 0 + phase / 24.0;

    // 3. Prepare for evaluatin and save evaluation - stuff
    ft  = zero; 
    featureCoeff newCoeffs; 
    positionList[pCount].stm = b.getActivePlayer();
    positionList[pCount].statEval = b.getActivePlayer() == WHITE ? Eval::evaluate(b, b.getActivePlayer()) : -Eval::evaluate(b, b.getActivePlayer());

    // after we did evaluate out ft containg fevaulated featutres
    // so now we can save it
    InitCoefficients(newCoeffs);
    InitETraces(newCoeffs, &positionList[pCount]);

    positionList[pCount].FinalEval =  b.getActivePlayer() == WHITE ? ft.FinalEval : -ft.FinalEval;

}

int simplifyPhaseCalculation(const Board &board){

    int phase = Eval::detail::PHASE_WEIGHT_SUM;

  for (auto pieceType : {ROOK, KNIGHT, BISHOP, QUEEN}) {
    phase -= _popCount(board.getPieces(WHITE, pieceType)) * Eval::detail::PHASE_WEIGHTS[pieceType];
    phase -= _popCount(board.getPieces(BLACK, pieceType)) * Eval::detail::PHASE_WEIGHTS[pieceType];
  }

  return phase;

}

void InitCoefficients(featureCoeff coeff){
    int i = 0;

    // Insert features here
    // Basicall we calculate if feature difference, so we dont hold it if it is 0
    //
    coeff[i++] = ft.PawnValue[WHITE] - ft.PawnValue[BLACK];
    coeff[i++] = ft.RookValue[WHITE] - ft.RookValue[BLACK];
    coeff[i++] = ft.KnightValue[WHITE] - ft.KnightValue[BLACK];
    coeff[i++] = ft.BishopValue[WHITE] - ft.BishopValue[BLACK];
    coeff[i++] = ft.QueenValue[WHITE] - ft.QueenValue[BLACK];
	coeff[i++] = ft.BishopPair[WHITE] - ft.BishopPair[BLACK];
    coeff[i++] = ft.KingHighDanger[WHITE] - ft.KingHighDanger[BLACK];
    coeff[i++] = ft.KingMedDanger[WHITE] - ft.KingMedDanger[BLACK];
    coeff[i++] = ft.KingLowDanger[WHITE] - ft.KingLowDanger[BLACK];
    coeff[i++] = ft.KingSafe[WHITE] - ft.KingSafe[BLACK];
    coeff[i++] = ft.PawnSupported[WHITE] - ft.PawnSupported[BLACK];
    coeff[i++] = ft.PawnDoubled[WHITE] - ft.PawnDoubled[BLACK];
    coeff[i++] = ft.PawnIsolated[WHITE] - ft.PawnIsolated[BLACK];
    coeff[i++] = ft.BishopRammed[WHITE] - ft.BishopRammed[BLACK];

    for (int j = 0; j < 8; j++){
        coeff[i++] = ft.PassedPawnRank[j][WHITE] - ft.PassedPawnRank[j][BLACK];
    }

    for (int j = 0; j < 8; j++){
        coeff[i++] = ft.PassedPawnFile[j][WHITE] - ft.PassedPawnFile[j][BLACK];
    }




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

        local[index][OPENING] +=  opBase * count;
        local[index][ENDGAME] +=  egBase * count;

    }
}

double Sigmoid(double eval){
    return 1.0 / (1.0 + exp( -TUNING_K * eval / 400.0));
}

double SigmoidForK(double eval, double K){
    return 1.0 / (1.0 + exp(( -K * eval) / 400.0));
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

double StaticError(tEntry * entries, double K) {

    double total = 0.0;

    #pragma omp parallel shared(total)
    {
        #pragma omp for schedule(static, TUNING_POS_COUNT / TUNING_THREADS) reduction(+:total)
        for (int i = 0; i < TUNING_POS_COUNT; i++)
            total += pow(entries[i].result - SigmoidForK(entries[i].statEval, K), 2);
    }

    return total / (double) TUNING_POS_COUNT;
}

void PrintTunedParams(tValueHolder currTerms, tValueHolder diffTerms){
    int i = 0;

    EvalTermPrint("\n PawnValue", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n RookValue", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n KnightValue", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n BishopValue", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n QueenValue", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n BISHOP_PAIR_BONUS", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n KING_HIGH_DANGER ", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n KING_MED_DANGER  ", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n KING_LOW_DANGER  ", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n KING_SAFE        ", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n PAWN_SUPPORTED", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n DOUBLED_PAWN_PENALTY", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n ISOLATED_PAWN_PENALTY", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\n BISHOP_RAMMED_PENALTY", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalArrayPrint("\n PASSED_PAWN_RANKS", currTerms, diffTerms, i, 8);
    i = i + 8;
    EvalArrayPrint("\n PASSED_PAWN_FILES", currTerms, diffTerms, i, 8);
	

}

double CalculateFactorK(tEntry * entries){
    double start = 0.0, end = 10, step = 1.0;
    double curr = 0.0;
    double error = 0.0;
    double best = StaticError(entries, start);

    for (int i = 0; i < TUNING_K_PRECISION; i++) {

        // Find the minimum within [start, end] using the current step
        curr = start - step;
        while (curr < end) {
            curr = curr + step;
            error = StaticError(entries, curr);
            if (error <= best)
                best = error, start = curr;
        }

        // Adjust the search space
        end   = start + step;
        start = start - step;
        step  = step  / 10.0;
    }

    return start;
}
#endif