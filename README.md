<h1 align="center">Drofa</h1>

A UCI chess engine written in C++11.

## Origins
Drofa started as fork of the <a href="https://github.com/GunshipPenguin/shallow-blue">Shallow Blue</a> chess engine.
My initial intention was to take weak, but stable and working chess engine and try to improve it,
learning c++ along the way.

The base-goal is to reach strenght of play comparable to the <a href="https://github.com/peterwankman/vice">VICE</a> chess engine.
The mid-goal - get to ~2500 rating on the CCRL
Far goal (probably will never be reached) - get into top 100 of the CCRL.

## Chages from Shallow Blue
For now Drofa can be considered as highly advanced fork of the Shallow Blue, with 3 features
being completely rewritten by me:

- Hashtable
- Pawn Hashtable
- King Safety Evaluation

Evaluation function in general was refactored with an elo-gain in self-play, although its still reqire much
work, because for now tapered eval in fact is used much less than in original Shallow Blue, which is a shame.

Some features were added during the work:
- QSearch move generator (althought i think current implementation is lazy and it can be done better)
- Null-move pruning
- Delta pruning (again, very lazy implementation)
- Various bugfixes

But most of the elo-gainers was some code restructuring and optimizations.
It allowed Drofa to be ~8x times faster than Shallow Blue.

Almost-full changelog with elo-gains measured for some of the features
can be found in the `Drofa_changelog` file.

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
  - Other
    - [Opening book support](https://www.chessprogramming.org/Opening_Book) (PolyGlot format)

## Building

To build on *nix:

```
make
```

You can build with debugging symbols and no optimizations using:

```
make debug
```

If you have Mingw-w64 installed, you can cross compile for Windows on Linux with:

```
./build_windows.sh
```

## Documentation

Shallow Blue's code was extensively documented with Doxygen.
I tried to follow this rule in the Drofa, and mostly sucseeded.

To generate HTML documentation use:

```
doxygen
```

## Opening Books

Drofa, as Shallow Blue, supports PolyGlot formatted (`.bin`) opening books. To use an opening book, the `OwnBook`
and `BookPath` UCI options must be set to `true` and the path to the opening book file respectively.

These options can be set from your chess GUI or the UCI interface as follows:

```
setoption name OwnBook value true
setoption name BookPath value /path/to/book.bin
```

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

