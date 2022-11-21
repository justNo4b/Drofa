#ifndef EVAL_H
#define EVAL_H

#include "defs.h"
#include "movegen.h"
#include "bitutils.h"
#include "endgame.h"

#define gS(opS, egS) (int)((unsigned int)(opS) << 16) + (egS)
#define opS(gS) (int16_t)((uint16_t)((unsigned)((gS) + 0x8000) >> 16))
#define egS(gS) (int16_t)((uint16_t)((unsigned)((gS))))


// define stuff for NNUE

#define INPUT_SIZE   (64 * 6 * 2)
#define HIDDEN_SIZE  (128)
#define OUTPUT_SIZE  (1)

enum CastleSide {
    KingSide,
    QueenSide,
    NoCastle
};

/**
 * @brief Namespace containing board evaluation functions
 */
namespace Eval {
namespace detail {
/**
 * @brief Array of files A-H as bitboards
 */
extern U64 FILES[8];

/**
 * @brief Array of the files neighboring each file as bitboards
 */
extern U64 CONNECTED_MASK[64];
extern U64 NEIGHBOR_FILES[8];
extern U64 TWO_PLUS_FILES[8];
extern U64 OUTPOST_MASK[2][64];
extern U64 OUTPOST_PROTECTION[2][64];
extern U64 KINGZONE[2][64];
extern U64 PAWN_DUOS [64];
extern U64 DISTANCE[64][64];
extern U64 FORWARD_BITS[2][64];

/**
 * @brief Array of masks indexed by [Color][square] containing all squares that
 * must be free of enemy pawns for a pawn of the given color on the given
 * square to be considered passed
 */
extern U64 PASSED_PAWN_MASKS[2][64];

/**
 * @brief Array of masks indexed by [Color][sideTo_OO][mask_NUM]
 * 0 - kindgside castle masks
 * 1 - queenside castle masks
 *
 * maskNUMs - just all masks
 */
extern U64 KING_PAWN_MASKS[2][2][8];
};

/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(82,110), gS(484,685), gS(339,373), gS(366,385), gS(980,1254),
};

/**
 * @brief Initializes all inner constants used by functions in the Eval namespace
 */
void init();

/**
 * @brief Returns the evaluated advantage of the given color in centipawns
 *
 * @param board Board to evaluate
 * @param color Color to evaluate advantage of
 * @return Advantage of the given color in centipawns
 */
inline int evaluateMain(const Board &, Color);

int evaluate(const Board &, Color);


void initEG();

inline void egHashAdd(std::string , egEvalFunction, egEntryType);

int evaluateDraw(const Board &, Color);
int evaluateRookMinor_Rook(const Board &, Color);
int evaluateQueen_vs_X(const Board &, Color);
int evaluateQueen_vs_Pawn(const Board &, Color);
int evaluateRook_vs_Bishop(const Board &, Color);
int evaluateRook_vs_Knight(const Board &, Color);
int evaluateRook_vs_Pawn(const Board &, Color);
int evaluateBishopPawn_vs_Bishop(const Board &, Color);
int evaluateBishopPawn_vs_Knight(const Board &, Color);
int evaluateQueen_vs_RookPawn(const Board &, Color);
int evaluateRookPawn_vs_Bishop(const Board &, Color);
int evaluateHugeAdvantage(const Board &, Color);
int evaluateBN_Mating(const Board &, Color);
int evaluateKnights_vs_Pawn(const Board &, Color);
int evaluateRookPawn_vs_Rook(const Board &, Color);
int evaluateKingPawn_vs_King(const Board &, Color);
int evaluateBishopPawn_vs_KP(const Board &, Color);

/**
 * @brief Returns the value of the given PieceType used for evaluation
 * purposes in centipawns
 *
 * This method returns the material value assuming the game is in the opening
 * and can be used to quickly determine relative capture values for move
 * ordering purposes.
 *
 * @param pieceType PieceType to get value of
 * @return The value of the given PieceType used for evaluation purposes
 */
int getMaterialValue(int, PieceType);

/**
 * @brief Set value for a MATERIAL_VALUES_TUNABLE array
 * which is used for optuna tuning
 */
void SetupTuning(PieceType piece, int value);

};

#endif
