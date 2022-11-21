#include "defs.h"
#include "rays.h"
#include "movegen.h"
#include "attacks.h"
#include "outposts.h"
#include "eval.h"
#include "transptable.h"
#include "tuning.h"

extern HASH * myHASH;
extern posFeatured ft;
extern egEvalEntry myEvalHash[];

int MATERIAL_VALUES_TUNABLE[6] = {
        [PAWN] = 0,
        [ROOK] = 0,
        [KNIGHT] = 0,
        [BISHOP] = 0,
        [QUEEN] = 0,
        [KING] = 0
};

U64 Eval::detail::FILES[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};
U64 Eval::detail::DISTANCE[64][64];
U64 Eval::detail::NEIGHBOR_FILES[8] = {
    FILE_B,
    FILE_A | FILE_C,
    FILE_B | FILE_D,
    FILE_C | FILE_E,
    FILE_D | FILE_F,
    FILE_E | FILE_G,
    FILE_F | FILE_H,
    FILE_G
};
U64 Eval::detail::TWO_PLUS_FILES[8] = {
    FILE_A | FILE_B | FILE_C,
    FILE_A | FILE_B | FILE_C | FILE_D,
    FILE_A | FILE_B | FILE_C | FILE_D | FILE_E,
    FILE_B | FILE_C | FILE_D | FILE_E | FILE_F,
    FILE_C | FILE_D | FILE_E | FILE_F | FILE_G,
    FILE_D | FILE_E | FILE_F | FILE_G | FILE_H,
    FILE_E | FILE_F | FILE_G | FILE_H,
    FILE_F | FILE_G | FILE_H,
};

U64 Eval::detail::PASSED_PAWN_MASKS[2][64];
U64 Eval::detail::OUTPOST_MASK[2][64];
U64 Eval::detail::CONNECTED_MASK[64];
U64 Eval::detail::OUTPOST_PROTECTION[2][64];
U64 Eval::detail::KINGZONE[2][64];
U64 Eval::detail::FORWARD_BITS[2][64];
U64 Eval::detail::KING_PAWN_MASKS[2][2][8] = {
        [WHITE] = {
          [0] = {
            (ONE << f2) | (ONE << g2) | (ONE << h2),
            (ONE << f2) | (ONE << g2) | (ONE << h3),
            (ONE << f2) | (ONE << g3) | (ONE << h2),
            (ONE << f2) | (ONE << g3) | (ONE << h4),
            (ONE << f2) | (ONE << g3) | (ONE << h3),
            (ONE << g2) | (ONE << h2),
            (ONE << g2) | (ONE << h3),
            (ONE << g2) | (ONE << g3) | (ONE << f2),
          },
          [1] = {
            (ONE << a2) | (ONE << b2) | (ONE << c2),
            (ONE << a3) | (ONE << b2) | (ONE << c2),
            (ONE << a2) | (ONE << b3) | (ONE << c2),
            (ONE << a4) | (ONE << b3) | (ONE << c2),
            (ONE << a3) | (ONE << b3) | (ONE << c2),
            (ONE << a2) | (ONE << b2),
            (ONE << a3) | (ONE << b2),
            (ONE << b2) | (ONE << b3) | (ONE << c2)
          }
        },
        [BLACK] = {
          [0] = {
            (ONE << f7) | (ONE << g7) | (ONE << h7),
            (ONE << f7) | (ONE << g7) | (ONE << h6),
            (ONE << f7) | (ONE << g6) | (ONE << h7),
            (ONE << f7) | (ONE << g6) | (ONE << h5),
            (ONE << f7) | (ONE << g6) | (ONE << h6),
            (ONE << g7) | (ONE << h7),
            (ONE << g7) | (ONE << h6),
            (ONE << g7) | (ONE << g6) | (ONE << f7),
          },
          [1] = {
            (ONE << a7) | (ONE << b7) | (ONE << c7),
            (ONE << a6) | (ONE << b7) | (ONE << c7),
            (ONE << a7) | (ONE << b6) | (ONE << c7),
            (ONE << a5) | (ONE << b6) | (ONE << c7),
            (ONE << a6) | (ONE << b6) | (ONE << c7),
            (ONE << a7) | (ONE << b7),
            (ONE << a6) | (ONE << b7),
            (ONE << b7) | (ONE << b6) | (ONE << c7)
          }
        }
    };


void Eval::init() {
  initEG();
  // Initialize passed pawn masks
  for (int square = 0; square < 64; square++) {

    detail::CONNECTED_MASK[square] = ((ONE << (square + 1)) & ~FILE_A) | ((ONE << (square - 1)) & ~FILE_H);

    for (int i =0; i < 64; i++){
      detail::DISTANCE[square][i] = std::max(abs(_col(square) - _col(i)), abs(_row(square) - _row(i)));
    }

    for (auto color : { WHITE, BLACK }) {

      U64 forwardRay = Rays::getRay(color == WHITE ? Rays::NORTH : Rays::SOUTH, square);

      detail::FORWARD_BITS[color][square] = forwardRay;

      detail::PASSED_PAWN_MASKS[color][square] = forwardRay | _eastN(forwardRay, 1) | _westN(forwardRay, 1);

      detail::OUTPOST_MASK[color][square] = detail::PASSED_PAWN_MASKS[color][square] & detail::NEIGHBOR_FILES[_col(square)];

      U64 sqv = ONE << square;
      detail::OUTPOST_PROTECTION[color][square] = color == WHITE ? ((sqv >> 9) & ~FILE_H) | ((sqv >> 7) & ~FILE_A)
                                                                 : ((sqv << 9) & ~FILE_A) | ((sqv << 7) & ~FILE_H);

      U64 kingAttack = Attacks::getNonSlidingAttacks(KING, square, WHITE);
      detail::KINGZONE[color][square] = color == WHITE ? sqv | kingAttack | (kingAttack << 8)
                                                       : sqv | kingAttack | (kingAttack >> 8);
      // When king is on the side files (A and H) include
      // squares on the C and F files to the kingzone
      if (_col(square) == 0){
        detail::KINGZONE[color][square] = detail::KINGZONE[color][square] | detail::KINGZONE[color][square] << 1;
      }else if (_col(square) == 7){
        detail::KINGZONE[color][square] = detail::KINGZONE[color][square] | detail::KINGZONE[color][square] >> 1;
      }

    }
  }

  // Init KPK bitbase
  // It should be initialized after rest of the Eval as some const are used in the
  // bitbase creation
  Bitbase::init_kpk();
}



inline int Eval::evaluateMain(const Board &board, Color color) {

  int score = 0;


  return score;
}

int Eval::evaluate(const Board &board, Color color){

    // Probe eval hash
    U64 index = board.getpCountKey().getValue() & (EG_HASH_SIZE - 1);
    egEvalFunction spEval   = myEvalHash[index].eFunction;
    egEntryType    spevType = myEvalHash[index].evalType;
    int            egResult = 1;

    if (myEvalHash[index].key == board.getpCountKey().getValue() && spEval != nullptr){
        egResult = spEval(board, color);
        if (spevType == RETURN_SCORE) return egResult;
    }


    return evaluateMain(board, color) / egResult;
}
