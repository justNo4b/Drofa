#include "defs.h"
#include "eval.h"
#include "tuning.h"
#include "board.h"
#include "outposts.h"
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
    std::cout << "Terje Kirstihagen (Weiss author) tuning implementation \n\n" << std::endl;
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

    std::cout << "\n Finishing. Final Parameters: \n" << std::endl;
    PrintTunedParams(currTerms, diffTerms);

}

void EvalTermPrint(std::string name, double opV, double egV, double opDiff, double egDiff){
    std::string op = std::to_string( (int)(opV + opDiff));
    std::string eg = std::to_string( (int)(egV + egDiff));
    std::cout << name + " = gS(" + op + "," + eg + ");"<< std::endl;
}

void EvalArrayPrint(std::string name, tValueHolder current, tValueHolder diff, int head, int length, int per){
    std::cout << name + "[" + std::to_string(length) + "] = {\n          ";
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

    // c. Array terms
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

    for (int j = 0; j < 9; j++){
        cTerms[c][OPENING] = opS(Eval::KING_PASSER_DISTANCE_FRIENDLY[j]);
        cTerms[c][ENDGAME] = egS(Eval::KING_PASSER_DISTANCE_FRIENDLY[j]);
        c++;
    }

    for (int j = 0; j < 9; j++){
        cTerms[c][OPENING] = opS(Eval::KING_PASSER_DISTANCE_ENEMY[j]);
        cTerms[c][ENDGAME] = egS(Eval::KING_PASSER_DISTANCE_ENEMY[j]);
        c++;
    }

    for (int j = 0; j < 2; j++){
        cTerms[c][OPENING] = opS(Eval::ROOK_OPEN_FILE_BONUS[j]);
        cTerms[c][ENDGAME] = egS(Eval::ROOK_OPEN_FILE_BONUS[j]);
        c++;
    }

    for (int j = 0; j < 2; j++){
        cTerms[c][OPENING] = opS(Eval::ROOK_SEMI_FILE_BONUS[j]);
        cTerms[c][ENDGAME] = egS(Eval::ROOK_SEMI_FILE_BONUS[j]);
        c++;
    }

    for (int j = 0; j < 5; j++){
        cTerms[c][OPENING] = opS(Eval::HANGING_PIECE[j]);
        cTerms[c][ENDGAME] = egS(Eval::HANGING_PIECE[j]);
        c++;
    }

    for (int j = 0; j < 14; j++){
        cTerms[c][OPENING] = opS(Eval::BISHOP_MOBILITY[j]);
        cTerms[c][ENDGAME] = egS(Eval::BISHOP_MOBILITY[j]);
        c++;
    }
    
    for (int j = 0; j < 9; j++){
        cTerms[c][OPENING] = opS(Eval::KNIGHT_MOBILITY[j]);
        cTerms[c][ENDGAME] = egS(Eval::KNIGHT_MOBILITY[j]);
        c++;
    }

    for (int j = 0; j < 9; j++){
        cTerms[c][OPENING] = opS(Eval::KING_MOBILITY[j]);
        cTerms[c][ENDGAME] = egS(Eval::KING_MOBILITY[j]);
        c++;
    }

    for (int j = 0; j < 15; j++){
        cTerms[c][OPENING] = opS(Eval::ROOK_MOBILITY[j]);
        cTerms[c][ENDGAME] = egS(Eval::ROOK_MOBILITY[j]);
        c++;
    }

    for (int j = 0; j < 28; j++){
        cTerms[c][OPENING] = opS(Eval::QUEEN_MOBILITY[j]);
        cTerms[c][ENDGAME] = egS(Eval::QUEEN_MOBILITY[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(Eval::KING_PSQT_BLACK[j]);
        cTerms[c][ENDGAME] = egS(Eval::KING_PSQT_BLACK[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(Eval::PAWN_PSQT_BLACK[j]);
        cTerms[c][ENDGAME] = egS(Eval::PAWN_PSQT_BLACK[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(Eval::ROOK_PSQT_BLACK[j]);
        cTerms[c][ENDGAME] = egS(Eval::ROOK_PSQT_BLACK[j]);
        c++;
    }
    
    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(Eval::BISHOP_PSQT_BLACK[j]);
        cTerms[c][ENDGAME] = egS(Eval::BISHOP_PSQT_BLACK[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(Eval::KNIGHT_PSQT_BLACK[j]);
        cTerms[c][ENDGAME] = egS(Eval::KNIGHT_PSQT_BLACK[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(Eval::QUEEN_PSQT_BLACK[j]);
        cTerms[c][ENDGAME] = egS(Eval::QUEEN_PSQT_BLACK[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(KNIGHT_PROT_OUTPOST_BLACK[j]);
        cTerms[c][ENDGAME] = egS(KNIGHT_PROT_OUTPOST_BLACK[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(BISHOP_PROT_OUTPOST_BLACK[j]);
        cTerms[c][ENDGAME] = egS(BISHOP_PROT_OUTPOST_BLACK[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(KNIGHT_OUTPOST_BLACK[j]);
        cTerms[c][ENDGAME] = egS(KNIGHT_OUTPOST_BLACK[j]);
        c++;
    }

    for (int j = 0; j < 64; j++){
        cTerms[c][OPENING] = opS(BISHOP_OUTPOST_BLACK[j]);
        cTerms[c][ENDGAME] = egS(BISHOP_OUTPOST_BLACK[j]);
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
    Board b = Board(myFen);

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
    InitCoefficients(newCoeffs);
    InitETraces(newCoeffs, &positionList[pCount]);

    // 5. Save Final evaluation for easier gradient recalculation
    // As we called evaluate() from stm perspective
    // we need to adjust it here to be from WHITE POW
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
    // and to take less space to hold it in general
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

    for (int j = 0; j < 9; j++){
        coeff[i++] = ft.KingFriendlyPasser[j][WHITE] - ft.KingFriendlyPasser[j][BLACK];
    }

    for (int j = 0; j < 9; j++){
        coeff[i++] = ft.KingEnemyPasser[j][WHITE] - ft.KingEnemyPasser[j][BLACK];
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

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.KnightOutProtBlack[j][WHITE] - ft.KnightOutProtBlack[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.BishopOutProtBlack[j][WHITE] - ft.BishopOutProtBlack[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.KnightOutBlack[j][WHITE] - ft.KnightOutBlack[j][BLACK];
    }

    for (int j = 0; j < 64; j++){
        coeff[i++] = ft.BishopOutBlack[j][WHITE] - ft.BishopOutBlack[j][BLACK];
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
    EvalTermPrint("\nconst int  BISHOP_PAIR_BONUS", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\nconst int  KING_HIGH_DANGER ", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\nconst int  KING_MED_DANGER  ", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\nconst int  KING_LOW_DANGER  ", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\nconst int  KING_SAFE        ", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\nconst int  PAWN_SUPPORTED", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\nconst int  DOUBLED_PAWN_PENALTY", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\nconst int  ISOLATED_PAWN_PENALTY", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalTermPrint("\nconst int  BISHOP_RAMMED_PENALTY", currTerms[i][OPENING], currTerms[i][ENDGAME], diffTerms[i][OPENING], diffTerms[i][ENDGAME]);
    i++;
    EvalArrayPrint("\nconst int  PASSED_PAWN_RANKS", currTerms, diffTerms, i, 8, 4);
    i = i + 8;
    EvalArrayPrint("\nconst int  PASSED_PAWN_FILES", currTerms, diffTerms, i, 8, 4);
    i = i + 8;
    EvalArrayPrint("\nconst int  KING_PASSER_DISTANCE_FRIENDLY", currTerms, diffTerms, i, 9, 4);
	i = i + 9;
    EvalArrayPrint("\nconst int  KING_PASSER_DISTANCE_ENEMY", currTerms, diffTerms, i, 9, 4);
    i = i + 9;
    EvalArrayPrint("\nconst int  ROOK_OPEN_FILE_BONUS", currTerms, diffTerms, i, 2, 10);
    i = i + 2;
    EvalArrayPrint("\nconst int  ROOK_SEMI_FILE_BONUS", currTerms, diffTerms, i, 2, 10);
    i = i + 2;
    EvalArrayPrint("\nconst int  HANGING_PIECE", currTerms, diffTerms, i, 5, 10);
    i = i + 5;
    EvalArrayPrint("\nconst int  BISHOP_MOBILITY", currTerms, diffTerms, i, 14, 7);
    i = i + 14;
    EvalArrayPrint("\nconst int  KNIGHT_MOBILITY", currTerms, diffTerms, i, 9, 5);
    i = i + 9;
    EvalArrayPrint("\nconst int  KING_MOBILITY", currTerms, diffTerms, i, 9, 5);
    i = i + 9;
    EvalArrayPrint("\nconst int  ROOK_MOBILITY", currTerms, diffTerms, i, 15, 7);
    i = i + 15;
    EvalArrayPrint("\nconst int  QUEEN_MOBILITY", currTerms, diffTerms, i, 28, 7);
    i = i + 28;
    EvalArrayPrint("\nconst int  KING_PSQT_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int  PAWN_PSQT_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int  ROOK_PSQT_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int  BISHOP_PSQT_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int  KNIGHT_PSQT_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int  QUEEN_PSQT_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int KNIGHT_PROT_OUTPOST_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int BISHOP_PROT_OUTPOST_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int KNIGHT_OUTPOST_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
    EvalArrayPrint("\nconst int BISHOP_OUTPOST_BLACK", currTerms, diffTerms, i, 64, 8);
    i = i + 64;
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