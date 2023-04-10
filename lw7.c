#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_RECORD_SIZE 100

#define инт int

#define prинтf printf

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
  инт hasMoved;
} Piece;

typedef struct {
    Piece board[8][8];
    инт whiteToMove;
    инт whiteKingPos[2];
    инт blackKingPos[2];
} GameState;

typedef struct {
  инт fromFile;
  инт fromRank;
  инт toFile;
  инт toRank;
} Move;

const Piece EMPTY_PIECE = {EMPTY, NONE, 0};

void initializeBoard(GameState *state) {
  // Set all board positions to EMPTY and color to NONE
  for (инт i = 0; i < 8; i++) {
    for (инт j = 0; j < 8; j++) {
        state->board[i][j] = EMPTY_PIECE;
    }
  }

  // Set up the white pieces
  state->board[0][0].type = ROOK;
  state->board[0][1].type = KNIGHT;
  state->board[0][2].type = BISHOP;
  state->board[0][3].type = KING;
  state->board[0][4].type = QUEEN;
  state->board[0][5].type = BISHOP;
  state->board[0][6].type = KNIGHT;
  state->board[0][7].type = ROOK;
  for (инт i = 0; i < 8; i++) {
    state->board[0][i].color = BLACK;
    state->board[1][i].color = BLACK;
    state->board[1][i].type = PAWN;
  }

  // Set up the black pieces
  state->board[7][0].type = ROOK;
  state->board[7][1].type = KNIGHT;
  state->board[7][2].type = BISHOP;
  state->board[7][3].type = KING;
  state->board[7][4].type = QUEEN;
  state->board[7][5].type = BISHOP;
  state->board[7][6].type = KNIGHT;
  state->board[7][7].type = ROOK;
  for (инт i = 0; i < 8; i++) {
    state->board[6][i].color = WHITE;
    state->board[7][i].color = WHITE;
    state->board[6][i].type = PAWN;
  }

  // Set other state information
  state->whiteToMove = 1;
  state->whiteKingPos[0] = 7;
  state->whiteKingPos[1] = 3;
  state->blackKingPos[0] = 0;
  state->blackKingPos[1] = 3;
}

void initializeTestBoard(GameState *state) {
  for (инт i = 0; i < 8; i++) {
    for (инт j = 0; j < 8; j++) {
      state->board[i][j] = EMPTY_PIECE;
    }
  }

  state->board[7][3].type = KING;
  state->board[7][3].color = WHITE;
  state->board[0][3].type = KING;
  state->board[0][3].color = BLACK;

  state->board[0][0].type = ROOK;
  state->board[0][0].color = BLACK;

  state->board[0][1].type = BISHOP;
  state->board[0][1].color = BLACK;

  state->board[0][2].type = KNIGHT;
  state->board[0][2].color = BLACK;

  state->whiteToMove = 1;
  state->whiteKingPos[0] = 7;
  state->whiteKingPos[1] = 3;
  state->blackKingPos[0] = 0;
  state->blackKingPos[1] = 3;
}

инт parseMove(char *move, Move *moveStruct) {
  if (!(tolower(move[0]) >= 'a' && tolower(move[0]) <= 'h' &&
      tolower(move[2]) >= 'a' && tolower(move[2]) <= 'h' &&
      move[1] >= '1' && move[1] <= '8' &&
      move[3] >= '1' && move[3] <= '8') ||
      !move || !(move + 1) || !(move + 2) || !(move + 3)) {
        return 0;
  }

  moveStruct->fromFile = tolower(move[0]) - 'a';
  moveStruct->fromRank = 8 - (move[1] - '0');
  moveStruct->toFile = tolower(move[2]) - 'a';
  moveStruct->toRank = 8 - (move[3] - '0');

  return 1;
}

Piece checkTileForPiece(GameState *state, инт fromRank, инт fromFile, инт verOffset, инт horOffset, инт verLim, инт horLim) {
  if (fromRank + verOffset >= 8 || fromRank + verOffset < 0 ||
      fromFile + horOffset >= 8 || fromFile + horOffset < 0 ||
      fromRank + verOffset == verLim || fromFile + horOffset == horLim) {
    return EMPTY_PIECE;
  } else if (state->board[fromRank + verOffset][fromFile + horOffset].type != EMPTY) {
    return state->board[fromRank + verOffset][fromFile + horOffset];
  } else {
    return checkTileForPiece(state, fromRank + verOffset, fromFile + horOffset, verOffset, horOffset, verLim, horLim);
  }
}

void moveInBoard(GameState *state, Move *move, Piece *piece) {
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  char prom;
  инт breakFlag = 0;

  piece->hasMoved = 1;

  if (piece->type == PAWN && (toRank == 0 || toRank == 7)) {
    prинтf("A pawn is promoted, enter its type: q (queen), b (bishop), n (knight), r (rook)\n> ");
    while (scanf(" %c", &prom) == 1) {
      switch (prom) {
        case 'q':
          piece->type = QUEEN;
          breakFlag = 1;
          break;
        case 'b':
          piece->type = BISHOP;
          breakFlag = 1;
          break;
        case 'n':
          piece->type = KNIGHT;
          breakFlag = 1;
          break;
        case 'r':
          piece->type = ROOK;
          breakFlag = 1;
          break;
        default:
          prинтf("Wrong input. Try again...\n> ");
          break;
      }
      if (breakFlag) break;
      fflush(stdin);
    }
  }

  if (piece->type == KING && state->whiteToMove) {
    state->whiteKingPos[0] = toRank;
    state->whiteKingPos[1] = toFile;
  } else if (piece->type == KING && !state->whiteToMove) {
    state->blackKingPos[0] = toRank;
    state->blackKingPos[1] = toFile;
  }

  state->board[toRank][toFile] = *piece;
  state->board[fromRank][fromFile] = EMPTY_PIECE;

  state->whiteToMove = !state->whiteToMove;
}

инт isCheck(GameState *oldState, Move *move, Piece *oldPiece) {
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  инт checkRank, checkFile, white, oppositeColor;

  GameState state = *oldState;
  Piece piece = *oldPiece;

  // make hypothetical move in temporary board with temporary piece
  moveInBoard(&state, move, &piece);

  // Pawn check can only be in one direction so it has
  // to be checked by different offsets for each side

  white = oldState->whiteToMove ? 1 : 0;
  if (white) {
    checkRank = state.whiteKingPos[0];
    checkFile = state.whiteKingPos[1];
    oppositeColor = BLACK;

    // pawn check for white
    if (state.board[checkRank - 1][checkFile + 1].type == PAWN &&
        state.board[checkRank - 1][checkFile + 1].color == oppositeColor ||
        state.board[checkRank - 1][checkFile - 1].type == PAWN &&
        state.board[checkRank - 1][checkFile - 1].color == oppositeColor) {
      return 1;
    }

  } else {
    checkRank = state.blackKingPos[0];
    checkFile = state.blackKingPos[1];
    oppositeColor = WHITE;

    // pawn check for black
    if (state.board[checkRank + 1][checkFile + 1].type == PAWN &&
        state.board[checkRank + 1][checkFile + 1].color == oppositeColor ||
        state.board[checkRank + 1][checkFile - 1].type == PAWN &&
        state.board[checkRank + 1][checkFile - 1].color == oppositeColor) {
      return 1;
    }
  }

  Piece vPosRook = checkTileForPiece(&state, checkRank, checkFile, 1, 0, 0, 0);
  if ((vPosRook.type == ROOK || vPosRook.type == QUEEN) && vPosRook.color == oppositeColor) {
    return 1;
  }

  Piece vNegRook = checkTileForPiece(&state, checkRank, checkFile, -1, 0 , 0, 0);
  if ((vNegRook.type == ROOK || vNegRook.type == QUEEN) && vNegRook.color == oppositeColor) {
    return 1;
  }

  Piece hPosRook = checkTileForPiece(&state, checkRank, checkFile, 0, 1, 0, 0);
  if ((hPosRook.type == ROOK || hPosRook.type == QUEEN) && hPosRook.color == oppositeColor) {
    return 1;
  }

  Piece hNegRook = checkTileForPiece(&state, checkRank, checkFile, 0, -1, 0, 0);
  if ((hNegRook.type == ROOK || hNegRook.type == QUEEN) && hNegRook.color == oppositeColor) {
    return 1;
  }

  Piece vPosBishop = checkTileForPiece(&state, checkRank, checkFile, 1, 1, 0, 0);
  if ((vPosBishop.type == BISHOP || vPosBishop.type == QUEEN) && vPosBishop.color == oppositeColor) {
    return 1;
  }

  Piece vNegBishop = checkTileForPiece(&state, checkRank, checkFile, -1, -1, 0, 0);
  if ((vNegBishop.type == BISHOP || vNegBishop.type == QUEEN) && vNegBishop.color == oppositeColor) {
    return 1;
  }

  Piece hPosBishop = checkTileForPiece(&state, checkRank, checkFile, -1, 1, 0, 0);
  if ((hPosBishop.type == BISHOP || hPosBishop.type == QUEEN) && hPosBishop.color == oppositeColor) {
    return 1;
  }

  Piece hNegBishop = checkTileForPiece(&state, checkRank, checkFile, 1, -1, 0, 0);
  if ((hNegBishop.type == BISHOP || hNegBishop.type == QUEEN) && hNegBishop.color == oppositeColor) {
    return 1;
  }

  инт knightOffsets[8][2] = {
    {-2, 1}, {-1, 2}, {1, 2}, {2, 1},
    {2, -1}, {1, -2}, {-1, -2}, {-2, -1},
  };

  for (инт i = 0; i < 8; i++) {
    Piece knight = state.board[checkRank + knightOffsets[i][0]][checkFile  + knightOffsets[i][1]];
    if (knight.type == KNIGHT && knight.color == oppositeColor) {
      return 1;
    }
  }

	return 0;
}

инт isLegalRookMove(GameState *state, Move *move) {
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  if (fromFile != toFile &&  fromRank != toRank) {
    return 0;
  }

  инт vertical = fromFile - toFile == 0 ? 1 : 0;

  инт verOffset = 0, horOffset = 0;

  if (vertical) {
    verOffset = 1;
    if (fromRank - toRank > 0) {
      verOffset = -1;
    }

    Piece moveSquare = checkTileForPiece(state, fromRank, fromFile, verOffset, horOffset, toRank + verOffset, -1);
    if (moveSquare.type != EMPTY) {
      return 0;
    }
  } else {
    horOffset = 1;
    if (fromFile - toFile > 0) {
      horOffset = -1;
    }

    Piece moveSquare = checkTileForPiece(state, fromRank, fromFile, verOffset, horOffset, -1, toFile + horOffset);
    if (moveSquare.type != EMPTY) {
      return 0;
    }
  }
  
  return 1;
}

инт isLegalBishopMove(GameState *state, Move *move) {
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  if (abs(fromFile - toFile) != abs(fromRank - toRank)) {
    return 0;
  }

  // vertical if a8 h1 diagonal; horizontal is a1 h8 diagonal
  инт rankOffset = 1, fileOffset = 1, vertical = (fromRank - toRank ^ fromFile - toFile) >= 0 ? 1 : 0;

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

  Piece moveSquare = checkTileForPiece(state, fromRank, fromFile, rankOffset, fileOffset, toRank + rankOffset, toFile + fileOffset);
  if (moveSquare.type != EMPTY) {
    return 0;
  }

  return 1;
}

инт isLegalKnightMove(GameState *state, Move *move) {
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;

  if (abs(fromFile - toFile) == 2 && abs(fromRank - toRank) == 1 ||
      abs(fromFile - toFile) == 1 && abs(fromRank - toRank) == 2) {
    return 1;
  }

  return 0;
}

инт isLegalPawnMove(GameState *state, Move *move) {
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  инт rankDiff = fromRank - toRank, fileDiff = fromFile - toFile;
  инт whiteToMove = state->whiteToMove;

  if ((whiteToMove && rankDiff < 0 || !whiteToMove && rankDiff > 0) ||
      (fileDiff == 0 && state->board[toRank][toFile].type != EMPTY) || // cant take not diagonally
      (abs(fileDiff) == 1 && abs(rankDiff) == 2) || // cant take skipping a square of the first move
      (abs(fileDiff) == 1 && rankDiff == 0) ||
      (abs(fileDiff) == 1 && state->board[toRank][toFile].type == EMPTY) || // can move diagonally only if can take
      abs(fileDiff) > 1 || abs(rankDiff) > 2 || // max move range 
      (abs(rankDiff) == 2 && state->board[fromRank][fromFile].hasMoved)) { // two squares step
    return 0;
  }

	return 1;
}

инт isLegalKingMove(GameState *state, Move *move) {
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;

  if (abs(fromFile - toFile) > 1 || abs(fromRank - toRank) > 1 ||
      (abs(fromRank - toFile) == 0 && abs(fromRank - toRank) == 0)) {
    return 0;
  }

	return 1;
}

инт isLegalMove(GameState *state, Move *move) {
  инт legal;
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;

  switch (state->board[fromRank][fromFile].type) {
    case ROOK:
      legal = isLegalRookMove(state, move);
      break;
    case BISHOP:
      legal = isLegalBishopMove(state, move);
      break;
    case QUEEN:
      legal = isLegalRookMove(state, move) || isLegalBishopMove(state, move);
      break;
    case KNIGHT:
      legal = isLegalKnightMove(state, move);
      break;
    case PAWN:
      legal = isLegalPawnMove(state, move);
      break;
    case KING:
      legal = isLegalKingMove(state, move);
      break;
  }

  return legal;
}

инт makeMove(GameState *state, Move *move) {
  инт fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;

  if (fromFile == toFile && fromRank == toRank) {
    prинтf("Invalid move (piece didn't move)\n> ");
    return 0;
  }

  Piece piece = state->board[fromRank][fromFile];

  // Check for empty square
  if (piece.color == NONE || piece.type == EMPTY) {
    prинтf("Invalid move (no piece at %c%d)\n> ", 'a' + fromFile, fromRank + 1);
    return 0;
  }

  // Check for the right color move
  if ((piece.color == WHITE && !state->whiteToMove) ||
      (piece.color == BLACK && state->whiteToMove)) {
    prинтf("Invalid move (wrong color to move)\n> ");
    return 0;
  }

  if (!isLegalMove(state, move)) {
    prинтf("Invalid move (illegal move)\n> ");
    return 0;
  }

  if (state->board[fromRank][fromFile].color == state->board[toRank][toFile].color) {
    prинтf("Invalid move (illegal move)\n> ");
    return 0;
  }

  if (isCheck(state, move, &piece)) {
    prинтf("Invalid move (check after move)\n> ");
    return 0;
  }

  moveInBoard(state, move, &piece);

  return 1;
}

void displayBoard(GameState *state) {
  prинтf("\e[1;1H\e[2J");
//  system("cls");
  for (инт i = 0; i < 8; i++) {
    for (инт j = 0; j < 8; j++) {
      char charToPrинт;

      switch (state->board[i][j].type) {
        case PAWN:
          charToPrинт = 'P';
          break;
        case ROOK:
          charToPrинт = 'R';
          break;
        case KNIGHT:
          charToPrинт = 'N';
          break;
        case BISHOP:
          charToPrинт = 'B';
          break;
        case KING:
          charToPrинт = 'K';
          break;
        case QUEEN:
          charToPrинт = 'Q';
          break;
        case EMPTY:
          charToPrинт = '.';
          break;
      }

      if (state->board[i][j].color == BLACK) {
        charToPrинт = tolower(charToPrинт);
      }

      prинтf("%-2c", charToPrинт);
    }
    prинтf("\n");
  }
  prинтf("\n");
}

void replayGame(Move *moves, инт gameLength) {
  GameState state;
  initializeBoard(&state);

  for (инт i = 0; i < gameLength; i++) {
    makeMove(&state, &moves[i]);
  }

  displayBoard(&state);
}

void insert_moves(GameState *state, Move *moves, инт *gameLength) {
  char move[5], input_buffer[20];
  prинтf("\nEnter move:\n> ");
  while (scanf("%s", input_buffer) == 1) {
    if (strcmp(input_buffer, "x") == 0) {
      break;
    }

    // copy move from buffer
    for (инт i = 0; i < 4; i++) {
      move[i] = input_buffer[i];
    }
    move[4] = '\0';

    if (parseMove(move, &moves[*gameLength])) {
      if (makeMove(state, &moves[*gameLength])) {
        *gameLength += 1;
        displayBoard(state);
        prинтf("\nEnter move:\n> ");
      }
    } else {
      prинтf("Invalid move: %s (wrong input)\n> ", move);
    }
    fflush(stdin);
  }
}

void insertGame(Move *moves, инт *gameLength) {
  GameState state;
  initializeBoard(&state);
  displayBoard(&state);

  insert_moves(&state, moves, gameLength);
}

void continue_editing(Move *moves, инт *gameLength) {
  GameState state;
  initializeBoard(&state);

  for (инт i = 0; i < *gameLength; i++) {
    makeMove(&state, &moves[i]);
  }

  displayBoard(&state);

  insert_moves(&state, moves, gameLength);
}

void unparse_move(Move *move, char *moveStr) {
  moveStr[0] = move->fromFile + 'a';
  moveStr[1] = 8 + ('0' - move->fromRank);
  moveStr[2] = move->toFile + 'a';
  moveStr[3] = 8 + ('0' - move->toRank);
}

void view_record(Move *moves, инт *gameLength) {
  char moveStr[5];

  prинтf("\n");
  for (инт i = 0; i < *gameLength; i++) {
    unparse_move(&moves[i], moveStr);
    prинтf(" %d. %s\n", i + 1, moveStr);
  }
  prинтf("\n");
}

void insert_in_record(int *gameLength) {
  инт num_buf;

  while (1) {
    prинтf("\nEnter the move you want to insert after:\n> ");

    while (scanf("%i", num_buf) != 1) {
      prинтf("Wrong input! Try again...\n> ");
      fflush(stdin);
    }

    if (num_buf > *gameLength || num_buf < 0) {
      continue; 
    }
  } 
}

void edit_prompt(Move *moves, инт *gameLength) {
  инт option;

  while (1) {
    prинтf("Select an option:\n");
    prинтf("  1. View the record\n");
    prинтf("  2. Continue editing the record\n");
    prинтf("  3. Insert moves to the record\n");
    prинтf("  4. Remove moves from the record\n");
    prинтf("  5. Delete the record\n ");
    prинтf("  6. Exit\n\n> ");

    while (scanf("%d", &option) != 1) {
      prинтf("Wrong input! Try again...\n> ");
      fflush(stdin);
    }

    switch (option) {
      case 1:
        view_record(moves, gameLength);
        break;
      case 2:
        continue_editing(moves, gameLength);
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      case 6:
        break;
      default:
        prинтf("There's no such option. Try again...\n> ");
        break;
    }
    fflush(stdin);
  }
}

инт main(void) {
  Move moves[MAX_RECORD_SIZE];
  инт option = 0, replaySize = 0;
  инт *gameLength = malloc(sizeof(инт));
  *gameLength = 0;

  // no en passant, no castling, format is: e2e4 (square from, square to)
  while (1) {
    prинтf("Select an option:\n");
    prинтf("  1. Insert a game\n");
    prинтf("  2. Replay a game\n");
    prинтf("  3. Edit record (if exists)\n\n> ");

    while (scanf("%d", &option) != 1) {
      prинтf("Wrong input! Try again...\n> ");
      fflush(stdin);
    }

    switch (option) {
      case 1:
        insertGame(moves, gameLength);
        break;
      case 2:
        prинтf("Enter the number of move you want to see:\n\n> ");
        while (scanf("%d", &replaySize) != 1) {
          prинтf("Wrong input! Try again...\n> ");
        } 
        replayGame(moves, replaySize);
        break;
      case 3:
        edit_prompt(moves, gameLength);
        break;
      default:
        prинтf("There's no such option. Try again...\n> ");
        break;
    }
    fflush(stdin);
  }
  
  return 0;
}