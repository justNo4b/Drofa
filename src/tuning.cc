#include "defs.h"
#include "eval.h"
#include "tuning.h"
#include "board.h"
#include "tuningFeatures.h"
#include <limits>
#include <fstream>
#include <cstring>
#include <math.h>
#include <iomanip>



eTrace *eTraceStack;
int eTraceStackSize;

posFeatured ft, zero;

//#ifdef _TUNE_

double tuneHIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {0};
double tuneHIDDEN_BIAS[N_HIDDEN] = {0};

double tuneOUTPUT_WEIGHTS1[N_HIDDEN] = {0};
double tuneOUTPUT_WEIGHTS2[N_HIDDEN] = {0};
double tuneOUTPUT_BIAS1 = 0;
double tuneOUTPUT_BIAS2 = 0;

double hidden_values[N_HIDDEN]= {0};

double wTweaksHIDDEN[3][N_INPUTS * N_HIDDEN] = {0};
double wTweaksOUTPUT1[3][N_HIDDEN] = {0};
double wTweaksOUTPUT2[3][N_HIDDEN] = {0};

double wTweakOBias1[3] = {0};
double wTweakOBias2[3] = {0};
double wTweakHBias[3][N_HIDDEN] = {0};

double E = 0.01;
double A = 0.0001;

TuningType FeatureTypeMap[TUNING_TERMS_COUNT];

void TunerStart(){
    // Startup messages
    std::cout << "This is special TUNING build \n\n" << std::endl;
    std::cout << "Implementation is heavily based on:" << std::endl;
    std::cout << "A. Grant (Ethereal author) tuning paper" << std::endl;
    std::cout << "Terje Kirstihagen (Weiss author) tuning implementation \n\n" << std::endl;
    std::cout << "Starting TUNING \n\n" << std::endl;

    //Declare stuff
    zero = {0};
    tEntry *entries = (tEntry*) calloc(TUNING_POS_COUNT, sizeof(tEntry));
    eTraceStack      = (eTrace*) calloc(TUNING_STACK_SIZE,  sizeof(eTrace));

    tValueHolder diffTerms = {{0}}, gradTerms = {{0}};
    double rate  = TUNING_L_RATE;
    double error = 0;
    double errorU = 0;

    // Check if our number of terms
    // is consistent with number of features
    //CheckFeaturesNumber();

    //Initiate our terms
    tValueHolder currTerms = {{0}};
    //EvalTermInitiate(currTerms);


    initializeWeights();
    // Initialize training data
    // Program will exit if there is smth wrong with our data
    if (!InitTuningPositions(entries))
        return;

    // Perform tuninig
    // For now we do only with all data at once, no partial chunks.

    std::cout << std::setprecision(16);

    //std::cout << "\n Calculating K... " << std::endl;
    //double K = CalculateFactorK(entries);
    //std::cout << "\n Optimal K = " << K << std::endl;
    for (int epoch = 0; epoch < TUNIGN_MAX_ITER; epoch++){

        // Calculate gradient
        tValueHolder gradient = {{0}};
        CalculateGradient(entries, gradient, diffTerms);

/*
        //adjust stuff
        for (int i = 0; i < TUNING_TERMS_COUNT; i++) {
                gradTerms[i][OPENING] += pow((TUNING_K / 200.0) * gradient[i][OPENING] / TUNING_POS_COUNT, 2.0);
                gradTerms[i][ENDGAME] += pow((TUNING_K / 200.0) * gradient[i][ENDGAME] / TUNING_POS_COUNT, 2.0);
                diffTerms[i][OPENING] += (TUNING_K / 200.0) * (gradient[i][OPENING] / TUNING_POS_COUNT) * (rate / sqrt(1e-8 + gradTerms[i][OPENING]));
                diffTerms[i][ENDGAME] += (TUNING_K / 200.0) * (gradient[i][ENDGAME] / TUNING_POS_COUNT) * (rate / sqrt(1e-8 + gradTerms[i][ENDGAME]));
        }

*/
        // Learning drop rate
        if (epoch % TUNING_L_STEP == 0){
            rate = rate / TUNING_L_DROP;
        }
        // Print new terms
        errorU = UnsharedTunedError(entries, diffTerms);
        std::cout << "\n IterationNum = " + std::to_string(epoch) << " unshared " << errorU;
        if (epoch % TUNIGN_PRINT == 0){
            error = TunedError(entries, diffTerms);
            errorU = UnsharedTunedError(entries, diffTerms);
            std::cout << "\n IterationNum = " + std::to_string(epoch) << " unshared " << errorU;
            std::cout << "\n Printing Terms: \n";
            //PrintTunedParams(currTerms, diffTerms);
            printWeights();

        }
    }

    std::cout << "\n Finishing. Final Parameters: \n" << std::endl;
    //PrintTunedParams(currTerms, diffTerms);

}

void EvalTermPrint(std::string name, double opV, double egV, double opDiff, double egDiff){
    std::string op = std::to_string( (int)(opV + opDiff));
    std::string eg = std::to_string( (int)(egV + egDiff));
    std::cout <<"\nconst int "<< name + " = gS(" + op + "," + eg + ");"<< std::endl;
}

void EvalArrayPrint(std::string name, tValueHolder current, tValueHolder diff, int head, int length, int per){
    std::cout << "\nconst int " << name + "[" + std::to_string(length) + "] = {\n          ";
    for (int i = 0; i < length; i++){

        if (i != 0 && i % per == 0){
            std::cout << "\n          ";
        }

        std::string op = std::to_string( (int)(current[head + i][OPENING] + diff[head + i][OPENING]));
        std::string eg = std::to_string( (int)(current[head + i][ENDGAME] + diff[head + i][ENDGAME]));
        std::cout << " gS(" + op + "," + eg + "),";
    }


    std::cout << "\n};"<< std::endl;
}

void EvalTermInitiate(tValueHolder cTerms){
    int c = 0;
    for (int i = 0; i < BIG_FEATURE_NUMBER; i++){
        if (myFeatures[i].isArray){
            for (int j = 0; j < myFeatures[i].valuesTotal; j++){
                cTerms[c][OPENING] = opS(myFeatures[i].startValue[j]);
                cTerms[c][ENDGAME] = egS(myFeatures[i].startValue[j]);
                FeatureTypeMap[c] = myFeatures[i].type;
                c++;
            }
        }else{
            cTerms[c][OPENING] = opS(*myFeatures[i].startValue);
            cTerms[c][ENDGAME] = egS(*myFeatures[i].startValue);
            FeatureTypeMap[c] = myFeatures[i].type;
            c++;
        }
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
        std::cout << "File opened. Processing positions...\n"<< std::endl;
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
    Board b = Board(myFen, false);

    // 2. Calculate phase-related stuff
    double phase = simplifyPhaseCalculation(b);
    positionList[pCount].phase = (phase * 256 + 12 ) / 24;
    // Basically we transformed and divided in two expression below
    // final_eval = ((opS(score) * (MAX_PHASE - phase)) + (egS(score) * phase)) / MAX_PHASE;
    positionList[pCount].pFactors[OPENING] = 1 - phase / 24.0;
    positionList[pCount].pFactors[ENDGAME] = 0 + phase / 24.0;

    // 3. Prepare for evaluatin and save evaluation - stuff
    // Evaluation should be from White POW, but we stiif call
    // evaluate() from stm perspective to get right tempo evaluation
    ft  = zero;
    featureCoeff newCoeffs;
    positionList[pCount].stm = b.getActivePlayer();
    positionList[pCount].statEval = b.getActivePlayer() == WHITE ? Eval::evaluate(b, b.getActivePlayer()) : -Eval::evaluate(b, b.getActivePlayer());

    // 4. After we did evaluate out ft containg fevaulated featutres
    // So now we can save it
    // InitCoefficients ensures the we do not store features
    // that are zeroed out (i.e bishop pair for both sides)
    //InitCoefficients(newCoeffs);
    //InitETraces(newCoeffs, &positionList[pCount]);

    //init net inputs
    std::memcpy(std::begin(positionList[pCount].net), std::begin(ft.kpInput), sizeof(ft.kpInput));

    // 5. Save Final evaluation for easier gradient recalculation
    // As we called evaluate() from stm perspective
    // we need to adjust it here to be from WHITE POW
    positionList[pCount].FinalEval =  b.getActivePlayer() == WHITE ? ft.FinalEval : -ft.FinalEval;

    // 6. Also save modifiers to know is it is
    // OCBEndgame
    positionList[pCount].FinalEvalScale = ft.Scale;

}

int simplifyPhaseCalculation(const Board &board){

    int phase = PHASE_WEIGHT_SUM;

  for (auto pieceType : {ROOK, KNIGHT, BISHOP, QUEEN}) {
    phase -= _popCount(board.getPieces(WHITE, pieceType)) * PHASE_WEIGHTS[pieceType];
    phase -= _popCount(board.getPieces(BLACK, pieceType)) * PHASE_WEIGHTS[pieceType];
  }

  // Make sure phase is not negative
  phase = std::max(0, phase);

  return phase;

}

void InitCoefficients(featureCoeff coeff){
    int i = 0;

    // Insert features here
    // Basicall we calculate if feature difference, so we dont hold it if it is 0
    // and to take less space to hold it in general
    coeff[i++] = ft.KingHighDanger[WHITE] - ft.KingHighDanger[BLACK];
    coeff[i++] = ft.KingMedDanger[WHITE] - ft.KingMedDanger[BLACK];
    coeff[i++] = ft.KingLowDanger[WHITE] - ft.KingLowDanger[BLACK];
    coeff[i++] = ft.BishopPair[WHITE] - ft.BishopPair[BLACK];
    coeff[i++] = ft.PawnDoubled[WHITE] - ft.PawnDoubled[BLACK];
    coeff[i++] = ft.PawnIsolated[WHITE] - ft.PawnIsolated[BLACK];
    coeff[i++] = ft.PawnBlocked[WHITE] - ft.PawnBlocked[BLACK];
    coeff[i++] = ft.PassersBlocked[WHITE] - ft.PassersBlocked[BLACK];
    coeff[i++] = ft.PawnPushThreat[WHITE] - ft.PawnPushThreat[BLACK];
    coeff[i++] = ft.BishopRammed[WHITE] - ft.BishopRammed[BLACK];
    coeff[i++] = ft.BishopCenterControl[WHITE] - ft.BishopCenterControl[BLACK];
    coeff[i++] = ft.BishopProtOutJump[WHITE] - ft.BishopProtOutJump[BLACK];
    coeff[i++] = ft.BishopGenOutJump[WHITE] - ft.BishopGenOutJump[BLACK];
    coeff[i++] = ft.KnightProtOutJump[WHITE] - ft.KnightProtOutJump[BLACK];
    coeff[i++] = ft.KnightGenOutJump[WHITE] - ft.KnightGenOutJump[BLACK];
    coeff[i++] = ft.MinorBehindPawn[WHITE] - ft.MinorBehindPawn[BLACK];
    coeff[i++] = ft.MinorBehindPasser[WHITE] - ft.MinorBehindPasser[BLACK];
    coeff[i++] = ft.MinorBlockOwn[WHITE] - ft.MinorBlockOwn[BLACK];
    coeff[i++] = ft.MinorBlockOwnPassed[WHITE] - ft.MinorBlockOwnPassed[BLACK];
    coeff[i++] = ft.RookTensionLine[WHITE] - ft.RookTensionLine[BLACK];
    coeff[i++] = ft.RookRammedLine[WHITE] - ft.RookRammedLine[BLACK];
    coeff[i++] = ft.KingAheadPasser[WHITE] - ft.KingAheadPasser[BLACK];
    coeff[i++] = ft.KingEqualPasser[WHITE] - ft.KingEqualPasser[BLACK];
    coeff[i++] = ft.KingBehindPasser[WHITE] - ft.KingBehindPasser[BLACK];
    coeff[i++] = ft.KingOpenFile[WHITE] - ft.KingOpenFile[BLACK];
    coeff[i++] = ft.KingSemiOwnFile[WHITE] - ft.KingSemiOwnFile[BLACK];
    coeff[i++] = ft.KingSemiEnemyFile[WHITE] - ft.KingSemiEnemyFile[BLACK];
    coeff[i++] = ft.KingAttackPawn[WHITE] - ft.KingAttackPawn[BLACK];
    coeff[i++] = ft.KingPawnless[WHITE] - ft.KingPawnless[BLACK];

    for (int j = 0; j < 7; j++){
        coeff[i++] = ft.PassedPawnRank[j][WHITE] - ft.PassedPawnRank[j][BLACK];
    }

    for (int j = 0; j < 4; j++){
        coeff[i++] = ft.PassedPawnFile[j][WHITE] - ft.PassedPawnFile[j][BLACK];
    }

    for (int j = 0; j < 7; j++){
        coeff[i++] = ft.PassedPawnFree[j][WHITE] - ft.PassedPawnFree[j][BLACK];
    }

    for (int j = 0; j < 7; j++){
        coeff[i++] = ft.PassedPawnPosAdvance[j][WHITE] - ft.PassedPawnPosAdvance[j][BLACK];
    }

    for (int j = 0; j < 8; j++){
        coeff[i++] = ft.PassedPassedDistance[j][WHITE] - ft.PassedPassedDistance[j][BLACK];
    }

    for (int j = 0; j < 8; j++){
        coeff[i++] = ft.KingFriendlyPasser[j][WHITE] - ft.KingFriendlyPasser[j][BLACK];
    }

    for (int j = 0; j < 8; j++){
        coeff[i++] = ft.KingEnemyPasser[j][WHITE] - ft.KingEnemyPasser[j][BLACK];
    }

    for (int j = 0; j < 4; j++){
        coeff[i++] = ft.KnightEnemyPasser[j][WHITE] - ft.KnightEnemyPasser[j][BLACK];
    }

    for (int j = 0; j < 7; j++){
        coeff[i++] = ft.CandidatePasser[j][WHITE] - ft.CandidatePasser[j][BLACK];
    }

    for (int j = 0; j < 4; j++){
        coeff[i++] = ft.CandidatePasserFile[j][WHITE] - ft.CandidatePasserFile[j][BLACK];
    }

    for (int j = 0; j < 7; j++){
        coeff[i++] = ft.BackwardPawn[j][WHITE] - ft.BackwardPawn[j][BLACK];
    }

    for (int j = 0; j < 7; j++){
        coeff[i++] = ft.BackwardOpenPawn[j][WHITE] - ft.BackwardOpenPawn[j][BLACK];
    }

    for (int j = 0; j < 2; j++){
        coeff[i++] = ft.RookOpenFile[j][WHITE] - ft.RookOpenFile[j][BLACK];
    }

    for (int j = 0; j < 2; j++){
        coeff[i++] = ft.RookHalfFile[j][WHITE] - ft.RookHalfFile[j][BLACK];
    }

    for (int j = 0; j < 5; j++){
        coeff[i++] = ft.HangingPiece[j][WHITE] - ft.HangingPiece[j][BLACK];
    }

    for (int j = 0; j < 4; j++){
        coeff[i++] = ft.MinorAttackedBy[j][WHITE] - ft.MinorAttackedBy[j][BLACK];
    }

    for (int j = 0; j < 4; j++){
        coeff[i++] = ft.RookAttackedBy[j][WHITE] - ft.RookAttackedBy[j][BLACK];
    }

    for (int j = 0; j < 4; j++){
        coeff[i++] = ft.QueenAttackedBy[j][WHITE] - ft.QueenAttackedBy[j][BLACK];
    }

    for (int j = 0; j < 8; j++){
        coeff[i++] = ft.KingShieldKS[j][WHITE] - ft.KingShieldKS[j][BLACK];
    }

    for (int j = 0; j < 8; j++){
        coeff[i++] = ft.KingShieldQS[j][WHITE] - ft.KingShieldQS[j][BLACK];
    }

    for (int j = 0; j < 14; j++){
        coeff[i++] = ft.BishopMobility[j][WHITE] - ft.BishopMobility[j][BLACK];
    }

    for (int j = 0; j < 9; j++){
        coeff[i++] = ft.KnigthMobility[j][WHITE] - ft.KnigthMobility[j][BLACK];
    }

    for (int j = 0; j < 9; j++){
        coeff[i++] = ft.KingMobility[j][WHITE] - ft.KingMobility[j][BLACK];
    }

    for (int j = 0; j < 15; j++){
        coeff[i++] = ft.RookMobility[j][WHITE] - ft.RookMobility[j][BLACK];
    }

    for (int j = 0; j < 28; j++){
        coeff[i++] = ft.QueenMobility[j][WHITE] - ft.QueenMobility[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.KingPsqtBlack[j][WHITE] - ft.KingPsqtBlack[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.PawnPsqtBlack[j][WHITE] - ft.PawnPsqtBlack[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.PawnPsqtBlackIsQ[j][WHITE] - ft.PawnPsqtBlackIsQ[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.PawnPsqtBlackIsOwn[j][WHITE] - ft.PawnPsqtBlackIsOwn[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.RookPsqtBlack[j][WHITE] - ft.RookPsqtBlack[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.BishopPsqtBlack[j][WHITE] - ft.BishopPsqtBlack[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.KnightPsqtBlack[j][WHITE] - ft.KnightPsqtBlack[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.QueenPsqtBlack[j][WHITE] - ft.QueenPsqtBlack[j][BLACK];
    }

    for (int j = 0; j < 32; j++){
        coeff[i++] = ft.KnightOutProtBlack[j][WHITE] - ft.KnightOutProtBlack[j][BLACK];
    }

    for (int j = 0; j < 32; j++){
        coeff[i++] = ft.BishopOutProtBlack[j][WHITE] - ft.BishopOutProtBlack[j][BLACK];
    }

    for (int j = 0; j < 32; j++){
        coeff[i++] = ft.KnightOutBlack[j][WHITE] - ft.KnightOutBlack[j][BLACK];
    }

    for (int j = 0; j < 32; j++){
        coeff[i++] = ft.BishopOutBlack[j][WHITE] - ft.BishopOutBlack[j][BLACK];
    }

    for (int j = 0; j < 32; j++){
        coeff[i++] = ft.PawnConnected[j][WHITE] - ft.PawnConnected[j][BLACK];
    }

    for (int j = 0; j < 32; j++){
        coeff[i++] = ft.PawnSupported[j][WHITE] - ft.PawnSupported[j][BLACK];
    }

    for (int j = 0; j < 5; j++){
        coeff[i++] = ft.MaterialValue[j][WHITE] - ft.MaterialValue[j][BLACK];
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
        std::cout << "Allocating additional RAM for the tuner..."<< std::endl;
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

    tValueHolder local = {{0}};

    for (int i = 0; i < TUNING_POS_COUNT; i++){
        UpdateSingleGrad( &entries[i], local, diff);
        if (i % NN_BATCH_SIZE == 1){
                mergeGradients();
        }
    }
    mergeGradients();
    shuffleData(entries);
/*
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
*/
}

void UpdateSingleGrad(tEntry* entry, tValueHolder local, tValueHolder diff){
    double eval = TuningEval(entry, diff);
    double sigm = Sigmoid(eval);
    double X = (entry->result - sigm) * sigm * (1.0 - sigm) * TUNING_K / 400;
    double scale = entry->FinalEvalScale / 4;

    double sigmaOut1 = X * entry->pFactors[OPENING] * scale;
    double sigmaOut2 = X * entry->pFactors[ENDGAME] * scale;


    propagateReverse(entry, sigmaOut1, sigmaOut2);

/*
    double opBase = X * entry->pFactors[OPENING];
    double egBase = X * entry->pFactors[ENDGAME];


    for (int i = 0; i < entry->tracesCount; i++){
        int index = entry->traces[i].index;
        int count = entry->traces[i].count;

        // Check if the gradient needs to be updated for the selected stage
        // and the actually update gradient

        if (FeatureTypeMap[index] == ALL || FeatureTypeMap[index] == OP_ONLY) local[index][OPENING] +=  opBase * count * scale;
        if (FeatureTypeMap[index] == ALL || FeatureTypeMap[index] == EG_ONLY) local[index][ENDGAME] +=  egBase * count * scale;

    }
*/
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

    netResult net = propagateForward(entry);
    opScore += net.out1;
    egScore += net.out2;

    //std::cout << egScore << " net: " << net << std::endl;

    double final_eval = ((opScore * (256.0 - entry->phase)) + (egScore * entry->phase)) / 256.0;

    final_eval = final_eval * entry->FinalEvalScale / 4;

    return final_eval + (entry->stm == WHITE ? 10 : -10);
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

double UnsharedTunedError(tEntry* entries, tValueHolder diff) {

    double total = 0.0;
        for (int i = 0; i < TUNING_POS_COUNT; i++)
            total += pow(entries[i].result - Sigmoid(TuningEval(&entries[i], diff)), 2);


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
    for (int c = 0; c < BIG_FEATURE_NUMBER; c++){
        if (myFeatures[c].isArray){
            EvalArrayPrint(myFeatures[c].name, currTerms, diffTerms, i, myFeatures[c].valuesTotal, myFeatures[c].padding);
            i = i +  myFeatures[c].valuesTotal;
        }else{
            EvalTermPrint(myFeatures[c].name ,currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
            i++;
        }
    }
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

void CheckFeaturesNumber(){
    int c = 0;
    for (int i = 0; i < BIG_FEATURE_NUMBER; i++){
        c += myFeatures[i].valuesTotal;
    }

    if (c != TUNING_TERMS_COUNT){
        std::cout << "Numbers of terms and features do not match" << std::endl;
        std::cout << "Features(terms): " << c << " Features: " << TUNING_TERMS_COUNT << std::endl;
        exit(1);
    }
}

void printWeights(){

    std::cout << std::endl << std::endl;

    std::cout << "int OUTPUT_BIAS1 = " << round(tuneOUTPUT_BIAS1) << ";" << std::endl;
    std::cout << "int OUTPUT_BIAS2 = " << round(tuneOUTPUT_BIAS2) << ";" << std::endl;

    std::cout << "int HIDDEN_BIAS[N_HIDDEN] = {";
    for (int i = 0; i < N_HIDDEN; i++){
        std::cout << round(tuneHIDDEN_BIAS[i]) << ", ";
    }
    std::cout << "};" << std::endl;

    std::cout << "int OUTPUT_WEIGHTS1[N_HIDDEN] = {";
    for (int i = 0; i < N_HIDDEN; i++){
        std::cout << round(tuneOUTPUT_WEIGHTS1[i]) << ", ";
    }
    std::cout << "};" << std::endl;

    std::cout << "int OUTPUT_WEIGHTS2[N_HIDDEN] = {";
    for (int i = 0; i < N_HIDDEN; i++){
        std::cout << round(tuneOUTPUT_WEIGHTS2[i]) << ", ";
    }
    std::cout << "};" << std::endl;


    std::cout << std::endl << std::endl;
    std::cout << "int HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {\n          ";

    int total = 0;
    for (int i = 0; i < N_HIDDEN; i++){
        for (int j = 0; j < N_INPUTS; j++){
            std::cout << round(tuneHIDDEN_WEIGHTS[total]) << ", ";
            if (total != 0 && total % 8 == 0){
                std::cout << "\n          ";
            }
            total++;
        }
    }

    std::cout << "};" << std::endl;
}


netResult propagateForward(tEntry* entry){
    netResult nr;
    nr.out1 = 0;
    nr.out2 = 0;

    int total = 0;
    for (int i = 0; i < N_HIDDEN; i++){
        for (int j = 0; j < N_INPUTS; j++){
            hidden_values[i] += entry->net[j] * tuneHIDDEN_WEIGHTS[total];
            total++;
        }
        // add bias
        hidden_values[i] += tuneHIDDEN_BIAS[i];

        // use sigmoid now
        hidden_values[i] = Eval::nnSigmoid(hidden_values[i]);
    }

    // Now calculate output
    for (int k = 0; k < N_HIDDEN; k++){
        nr.out1 += hidden_values[k] * tuneOUTPUT_WEIGHTS1[k];
        nr.out2 += hidden_values[k] * tuneOUTPUT_WEIGHTS2[k];
    }

    nr.out1 += tuneOUTPUT_BIAS1;
    nr.out2 += tuneOUTPUT_BIAS2;

    return nr;

}

void propagateReverse(tEntry* entry, double sigmOut1, double sigmOut2){

    double hidden_sigmas[N_HIDDEN] = {0};

    // For the output bias gradient is just sigmOut
    wTweakOBias1[gradient] += sigmOut1;
    wTweakOBias2[gradient] += sigmOut2;


    // for hidden - to output
    // Use sum of (weight * sigmaHigher) for all weights to higher
    // Only higher now is output
    for (int i = 0; i < N_HIDDEN; i++){
        hidden_sigmas[i] = (1 - hidden_values[i]) * hidden_values[i] * (sigmOut1 * tuneOUTPUT_WEIGHTS1[i] + sigmOut2 * tuneOUTPUT_WEIGHTS2[i]);
        // use Sigmas to calculate grad and upgrade gradient
        // Grad(AB) = sigmaB * outA => for hidden weights (hidden_output * sigma_result)
        // calculate weight tweak using gradients
        wTweaksOUTPUT1[gradient][i] +=  hidden_values[i] * sigmOut1;
        wTweaksOUTPUT2[gradient][i] +=  hidden_values[i] * sigmOut2;

        // For hidden biases Grad will be just hiddenSigma
        wTweakHBias[gradient][i] += hidden_sigmas[i];
    }

    // do the same for weights from input to the hidden
    int total = 0;
    for (int i = 0; i < N_HIDDEN; i++){
        for (int j = 0; j < N_INPUTS; j++){
            double grad = entry->net[j] * hidden_sigmas[i];
            wTweaksHIDDEN[gradient][total] += grad;
            total++;
        }
    }

}

void mergeGradients(){

    wTweakOBias1[gradient] = wTweakOBias1[gradient];
    wTweakOBias1[momentum] = 0.9 * wTweakOBias1[momentum] + 0.1 * wTweakOBias1[gradient];
    wTweakOBias1[velocity] = 0.999 * wTweakOBias1[velocity] + 0.001 * wTweakOBias1[gradient] * wTweakOBias1[gradient];
    tuneOUTPUT_BIAS1 +=  0.01 * wTweakOBias1[momentum] / (sqrtf(wTweakOBias1[velocity]) + 1e-8);
    wTweakOBias1[gradient] = 0;

    wTweakOBias2[gradient] = wTweakOBias2[gradient];
    wTweakOBias2[momentum] = 0.9 * wTweakOBias2[momentum] + 0.1 * wTweakOBias2[gradient];
    wTweakOBias2[velocity] = 0.999 * wTweakOBias2[velocity] + 0.001 * wTweakOBias2[gradient] * wTweakOBias2[gradient];
    tuneOUTPUT_BIAS2 +=  0.01 * wTweakOBias2[momentum] / (sqrtf(wTweakOBias2[velocity]) + 1e-8);
    wTweakOBias2[gradient] = 0;


    for (int i = 0; i < N_HIDDEN; i++){
        wTweaksOUTPUT1[momentum][i] = 0.9 * wTweaksOUTPUT1[momentum][i] + 0.1 * wTweaksOUTPUT1[gradient][i];
        wTweaksOUTPUT1[velocity][i] = 0.999 * wTweaksOUTPUT1[velocity][i] + 0.001 * wTweaksOUTPUT1[gradient][i] * wTweaksOUTPUT1[gradient][i];
        tuneOUTPUT_WEIGHTS1[i]+= 0.01 * wTweaksOUTPUT1[momentum][i] / (sqrtf(wTweaksOUTPUT1[velocity][i]) + 1e-8);
        wTweaksOUTPUT1[gradient][i] = 0;

        wTweaksOUTPUT2[momentum][i] = 0.9 * wTweaksOUTPUT2[momentum][i] + 0.1 * wTweaksOUTPUT2[gradient][i];
        wTweaksOUTPUT2[velocity][i] = 0.999 * wTweaksOUTPUT2[velocity][i] + 0.001 * wTweaksOUTPUT2[gradient][i] * wTweaksOUTPUT2[gradient][i];
        tuneOUTPUT_WEIGHTS2[i]+= 0.01 * wTweaksOUTPUT2[momentum][i] / (sqrtf(wTweaksOUTPUT2[velocity][i]) + 1e-8);
        wTweaksOUTPUT2[gradient][i] = 0;
        // hidden bias
        wTweakHBias[momentum][i] = 0.9 * wTweakHBias[momentum][i] + 0.1 * wTweakHBias[gradient][i];
        wTweakHBias[velocity][i] = 0.999 * wTweakHBias[velocity][i] + 0.001 * wTweakHBias[gradient][i] * wTweakHBias[gradient][i];
        tuneHIDDEN_BIAS[i]+= 0.01 * wTweakHBias[momentum][i] / (sqrtf(wTweakHBias[velocity][i]) + 1e-8);
        wTweakHBias[gradient][i] = 0;
    }

    int total = 0;
    for (int i = 0; i < N_HIDDEN; i++){
        for (int j = 0; j < N_INPUTS; j++){
            wTweaksHIDDEN[momentum][total] = 0.9 * wTweaksHIDDEN[momentum][total] + 0.1 * wTweaksHIDDEN[gradient][total];
            wTweaksHIDDEN[velocity][total] = 0.999 * wTweaksHIDDEN[velocity][total] + 0.001 * wTweaksHIDDEN[gradient][total] * wTweaksHIDDEN[gradient][total];
            tuneHIDDEN_WEIGHTS[total]+= 0.01 * wTweaksHIDDEN[momentum][total] / (sqrtf(wTweaksHIDDEN[velocity][total]) + 1e-8);
            wTweaksHIDDEN[gradient][total] = 0;
            total++;
        }
    }
}

inline double getRandomWeight(){
    return (BASIC_RANDOM_WEIGHT / 2) - (std::rand() % BASIC_RANDOM_WEIGHT);
}


void initializeWeights(){
    std::srand(std::time(NULL));

    tuneOUTPUT_BIAS1 = getRandomWeight();
    tuneOUTPUT_BIAS2 = getRandomWeight();

    for (int i = 0; i < N_HIDDEN; i++){
        tuneOUTPUT_WEIGHTS1[i] = getRandomWeight();
        tuneOUTPUT_WEIGHTS2[i] = getRandomWeight();
        tuneHIDDEN_BIAS[i] = getRandomWeight();
    }

    int total = 0;
    for (int i = 0; i < N_HIDDEN; i++){
        for (int j = 0; j < N_INPUTS; j++){
            tuneHIDDEN_WEIGHTS[total] += getRandomWeight();
            total++;
        }
    }
}


void shuffleData(tEntry* entries){

    tEntry tempEntry;
    std::srand(std::time(NULL));

    for (int i = 0; i < TUNING_POS_COUNT / NN_BATCH_SIZE; i++ ){
        int rand1 = std::rand() % TUNING_POS_COUNT;
        int rand2 = std::rand() % TUNING_POS_COUNT;
        tempEntry = entries[rand1];
        entries[rand1] = entries[rand2];
        entries[rand2] = tempEntry;
    }
}

//#endif