<h1 align="center">Drofa</h1>

A UCI chess engine written in C++11.

## Origins
Drofa started as fork of the <a href="https://github.com/GunshipPenguin/shallow-blue">Shallow Blue</a> chess engine.
My initial intention was to take weak, but stable and working chess engine and try to improve it,
learning c++ along the way.

During my Drofa experiments huge chunk of knowlenge were received from:

<a href="https://github.com/peterwankman/vice">VICE</a> chess engine and tutorials.
<a href="https://www.chessprogramming.org">Chessprogramming WIKI</a> great place for all chessprogramming-related knowledge.

Special thanks to the Terje Kirstihagen and his <a href="https://www.chessprogramming.org">Weiss</a> chess engine, from where
I took LMP and LMR base reduction formulas, and idead of using IMPROVING factor in pruning.
## Chages from Shallow Blue
Strength:
Drofa 2.0.0 will be probably a little less than 2500 CCRL elo.

## Chages from Shallow Blue
With Drofa 2.0 many features was added on top of the Shallow Blue, especially in the search section.

Almost-full changelog with elo-gains measured for some of the features can be found:
 - ShallowBlue -> Drofa 1.0 in the `Drofa_changelog` file.
 - Drofa 1.0 -> Drofa 2.0 in the `Drofa_changelog_2` file
## Features

  - Board representation
    - [Bitboards](https://en.wikipedia.org/wiki/Bitboard)
  - Move generation
    - [Magic bitboard hashing](https://www.chessprogramming.org/Magic_Bitboards)
  - Search
    - [Principal variation search](https://www.chessprogramming.org/Principal_Variation_Search)
    - [Iterative deepening](https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search)
    - [Quiescence search](https://en.wikipedia.org/wiki/Quiescence_search)
    - [Check extensions](https://www.chessprogramming.org/Check_Extensions)
    - [Transposition table](https://en.wikipedia.org/wiki/Transposition_table)
  - Search Pruning and Reductions
    - [Null move pruning](https://www.chessprogramming.org/Null_Move_Pruning)
    - [Delta pruning](https://www.chessprogramming.org/Delta_Pruning)
    - [Razoring](https://www.chessprogramming.org/Razoring) - Dropping in the QSearch variation
    - [Reverse Futility Pruning](https://www.chessprogramming.org/Reverse_Futility_Pruning)
    - [Un-Hashed Reduction](http://talkchess.com/forum3/viewtopic.php?f=7&t=74769) - Rebels idea
    - [Late Move Pruning] - based on the Weiss formula
    - [Extended Futility Pruning](https://www.chessprogramming.org/Futility_Pruning)
    - [Late Move Reduction](https://www.chessprogramming.org/Late_Move_Reductions) - based on the Weiss formula
  - Evaluation
    - [Piece square tables](https://www.chessprogramming.org/Piece-Square_Tables)
    - [Pawn structure](https://www.chessprogramming.org/Pawn_Structure)
    - [King safety](https://www.chessprogramming.org/King_Safety)
    - [Bishop pairs](https://www.chessprogramming.org/Bishop_Pair)
    - [Rooks on open/halfopen files](https://www.chessprogramming.org/Rook_on_Open_File)
    - [Mobility](https://www.chessprogramming.org/Mobility)
    - [Evaluation tapering](https://www.chessprogramming.org/Tapered_Eval)
    - [Pawn HashTable]
  - Move ordering
    - [Hash move](https://www.chessprogramming.org/Hash_Move)
    - [MVV/LVA](https://www.chessprogramming.org/MVV-LVA)
    - [Killer heuristic](https://www.chessprogramming.org/Killer_Heuristic)
    - [History heuristic](https://www.chessprogramming.org/History_Heuristic)	
    - [CounterMove heuristic](https://www.chessprogramming.org/Countermove_Heuristic) - only for Quiet moves
  - Other
    - [Opening book support](https://www.chessprogramming.org/Opening_Book) (PolyGlot format)

## Building

To build on *nix:

```
make
``

If you have Mingw-w64 installed, you can cross compile for Windows on Linux with:
WARNING - migw-w64 compiles are ~50% slower than native windows compiles, for best performance,
use native windows g++ compiler.

```
./build_windows.sh
```

You can build with debugging symbols and no optimizations using:

```
make debug
```

You can build tuning-ready version of the Drofa Engine using:
Drofa-tune version can be tuned with [Optuna-Game-Parameter-Tuner](https://github.com/fsmosca/Optuna-Game-Parameter-Tuner)

```
make tune
```



## Documentation

Shallow Blue's code was extensively documented with Doxygen.
I tried to follow this rule in the Drofa, and mostly sucseeded.

To generate HTML documentation use:

```
doxygen
```

## UCI commands

Drofa, as Shallow Blue, supports PolyGlot formatted (`.bin`) opening books. To use an opening book, the `OwnBook`
and `BookPath` UCI options must be set to `true` and the path to the opening book file respectively.

These options can be set from your chess GUI or the UCI interface as follows:

```
setoption name OwnBook value true
setoption name BookPath value /path/to/book.bin
```

Drofa tuning version allows additional UCI options for the piece values:
vPawnEG
vPawnOP
vKnightOP
vKnightEG
vBishopOP
vBishopEG
vRookOP
vRookEG
vQueenOP
vQueenEG

## Implemented non UCI Commands

These commands can be useful for debugging.

- `perft <depth>`
  - Prints the perft value for each move on the current board to the specified depth
- `printboard`
    - Pretty prints the current state of the game board
- `printmoves`
    - Prints all legal moves for the currently active player

## License

I dont know shit about licensing and Drofa is too weak to be plagiarised.
So it is under the same MIT license as Shallow Blue.

2017-2019 © Rhys Rustad-Elliott (original Shallow Blue creator)
2020 © Litov Alexander 

