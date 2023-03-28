#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum {
  EMPTY = 0,
  PAWN,
  ROOK,
  KNIGHT,
  BISHOP,
  QUEEN,
  KING,
} Type;

typedef enum {
  NONE = 0,
  WHITE,
  BLACK,
} Color;

typedef struct {
  Type type;
  Color color;
  int hasMoved;
} Piece;

typedef struct {
    Piece board[8][8];
    int whiteToMove;
} GameState;

void initializeBoard(GameState *state) {
  // Set all board positions to EMPTY and color to NONE
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
        state->board[i][j].color = NONE;
        state->board[i][j].type = EMPTY;
        state->board[i][j].hasMoved = 0;
    }
  }

  // Set up the white pieces
  state->board[0][0].type = ROOK;
  state->board[0][1].type = KNIGHT;
  state->board[0][2].type = BISHOP;
  state->board[0][3].type = QUEEN;
  state->board[0][4].type = KING;
  state->board[0][5].type = BISHOP;
  state->board[0][6].type = KNIGHT;
  state->board[0][7].type = ROOK;
  for (int i = 0; i < 8; i++) {
    state->board[0][i].color = BLACK;
    state->board[1][i].color = BLACK;
    state->board[1][i].type = PAWN;
  }

  // Set up the black pieces
  state->board[7][0].type = ROOK;
  state->board[7][1].type = KNIGHT;
  state->board[7][2].type = BISHOP;
  state->board[7][3].type = QUEEN;
  state->board[7][4].type = KING;
  state->board[7][5].type = BISHOP;
  state->board[7][6].type = KNIGHT;
  state->board[7][7].type = ROOK;
  for (int i = 0; i < 8; i++) {
    state->board[6][i].color = WHITE;
    state->board[7][i].color = WHITE;
    state->board[6][i].type = PAWN;
  }

  // Set other state information
  state->whiteToMove = 1;
}

int parseMove(const char *move, int *fromFile, int *fromRank, int *toFile, int *toRank) {
  if ((tolower(move[0]) != 'a' && tolower(move[0]) != 'b' && tolower(move[0]) != 'c' && tolower(move[0]) != 'd' && 
      tolower(move[0]) != 'e' && tolower(move[0]) != 'f' && tolower(move[0]) != 'g' && tolower(move[0]) != 'h') || 
      (tolower(move[2]) != 'a' && tolower(move[2]) != 'b' && tolower(move[2]) != 'c' && tolower(move[2]) != 'd' && 
      tolower(move[2]) != 'e' && tolower(move[2]) != 'f' && tolower(move[2]) != 'g' && tolower(move[2]) != 'h') ||
      !move || !(move + 1) || !(move + 2) || !(move + 3)) {
        // TODO: more complex verification
        return 0;
  }

  *fromFile = tolower(move[0]) - 'a';
  *fromRank = 8 - (move[1] - '0');
  *toFile = tolower(move[2]) - 'a';
  *toRank = 8 - (move[3] - '0');

  return 1;
}

int isLegalRookMove(GameState *state, int fromFile, int fromRank, int toFile, int toRank) {
  if (fromFile != toFile &&  fromRank != toRank) {
    return 0;
  }

  int offset = 1, vertical = fromFile - toFile == 0 ? 1 : 0;

  if (vertical) {
    if (fromRank - toRank > 0) {
      offset = -1;
    }

    for (int i = fromRank + offset; i != toRank; i + offset) {
      if (state->board[i][fromFile].type != EMPTY ||
          state->board[i][fromFile].color != NONE) {
        return 0;
      }
      printf("%i", offset);
    }
  } else {
    if (fromFile - toFile > 0) {
      offset = -1;
    }

    for (int i = fromFile + offset; i != toFile; i + offset) {
      if (state->board[fromRank][i].type != EMPTY ||
          state->board[fromRank][i].color != NONE) {
        return 0;
      }
    }
  }
}

int isLegalBishopMove(GameState *state, int fromFile, int fromRank, int toFile, int toRank) {
  if (abs(fromFile - toFile) != abs(fromRank - toRank)) {
    return 0;
  }

  // vertical if a8 h1 diagonal; horizontal is a1 h8 diagonal
  int rankOffset = 1, fileOffset = 1, vertical = (fromRank - toRank ^ fromFile - toFile) >= 0 ? 1 : 0;

  if (vertical) {
    if (fromRank - toRank > 0) {
      rankOffset = -1;
      fileOffset = -1;
    }
  } else {
    if (fromRank - toRank > 0) {
      rankOffset = -1;
    } else {
      fileOffset = -1;
    }
  }

  for (int i = fromRank + rankOffset; i != toRank; i + rankOffset) {
    for (int j = fromFile + fileOffset;;) {
      if (state->board[i][j].type != EMPTY || state->board[i][j].color != NONE) {
        return 0;
      }
      j += fileOffset;
    }
  }

  return 1;
}

int isLegalMove(GameState *state, int fromFile, int fromRank, int toFile, int toRank) {
  int legal;

  switch (state->board[fromRank][fromFile].type) {
    case ROOK:
      legal = isLegalRookMove(state, fromFile, fromRank, toFile, toRank);
      break;
    case BISHOP:
      legal = isLegalBishopMove(state, fromFile, fromRank, toFile, toRank);
      break;
    case QUEEN:
      break;
    case PAWN:
      break;
    case KNIGHT:
      break;
    case KING:
      break;
  }

  return legal;
}

int makeMove(GameState *state, const char *move) {
  int fromFile, fromRank, toFile, toRank;

  if (!parseMove(move, &fromFile, &fromRank, &toFile, &toRank)) {
    printf("Invalid move: %s (wrong input)\n", move);
    return 0;
  }

  if (fromFile == toFile && fromRank == toRank) {
    printf("Invalid move: %s (piece didn't move)\n", move);
    return 0;
  }

  Piece piece = state->board[fromRank][fromFile];

  // Check for empty tile
  if (piece.color == NONE || piece.type == EMPTY) {
    printf("Invalid move: %s (no piece at %c%d)\n", move, 'a' + fromFile, fromRank + 1);
    return 0;
  }

  // Check for the right color move
  if ((piece.color == WHITE && !state->whiteToMove) ||
      (piece.color == BLACK && state->whiteToMove)) {
    printf("Invalid move: %s (wrong color to move)\n", move);
    return 0;
  }

  if (!isLegalMove(state, fromFile, fromRank, toFile, toRank)) {
    printf("Invalid move: %s (illegal move)\n", move);
    return 0;
  }

  if (state->board[fromRank][fromFile].color == state->board[toRank][toFile].color) {
    printf("Invalid move: %s (illegal move)\n", move);
    return 0;
  }

  piece.hasMoved = 1;
  state->board[toRank][toFile] = piece;
  state->board[fromRank][fromFile].type = EMPTY;
  state->board[fromRank][fromFile].color = NONE;

  state->whiteToMove = !state->whiteToMove;
  return 1;
}

void displayBoard(GameState *state) {
  printf("\e[1;1H\e[2J"); // clear || cls
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      char charToPrint;

      switch (state->board[i][j].type) {
        case PAWN:
          charToPrint = 'P';
          break;
        case ROOK:
          charToPrint = 'R';
          break;
        case KNIGHT:
          charToPrint = 'N';
          break;
        case BISHOP:
          charToPrint = 'B';
          break;
        case KING:
          charToPrint = 'K';
          break;
        case QUEEN:
          charToPrint = 'Q';
          break;
        case EMPTY:
          charToPrint = 'o';
          break;
      }

      if (state->board[i][j].color == BLACK) {
        charToPrint = tolower(charToPrint);
      }

      printf("%-2c", charToPrint);
    }
    printf("\n");
  }
  printf("\n");
}

int main(void) {
  GameState gameState;
  initializeBoard(&gameState);
  displayBoard(&gameState);

  char move[4];
  while (scanf("%s", move) == 1) {
    if (makeMove(&gameState, &move)) {
      displayBoard(&gameState);
    }
    fflush(stdin);
  }
  
  return 0;
};