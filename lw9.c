#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_RECORD_SIZE 100

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
    int whiteKingPos[2];
    int blackKingPos[2];
} GameState;

typedef struct {
  int fromFile;
  int fromRank;
  int toFile;
  int toRank;
} Move;

const Piece EMPTY_PIECE = {EMPTY, NONE, 0};

void initializeBoard(GameState *state) {
  // Set all board positions to EMPTY and color to NONE
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
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
  for (int i = 0; i < 8; i++) {
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
  for (int i = 0; i < 8; i++) {
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
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
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

int parseMove(char *move, Move *moveStruct) {
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

Piece checkTileForPiece(GameState *state, int fromRank, int fromFile, int verOffset, int horOffset, int verLim, int horLim) {
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
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  char prom;
  int breakFlag = 0;

  piece->hasMoved = 1;

  if (piece->type == PAWN && (toRank == 0 || toRank == 7)) {
    printf("A pawn is promoted, enter its type: q (queen), b (bishop), n (knight), r (rook)\n> ");
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
          printf("Wrong input. Try again...\n> ");
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

int isCheck(GameState *oldState, Move *move, Piece *oldPiece) {
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  int checkRank, checkFile, white, oppositeColor;

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

  int knightOffsets[8][2] = {
    {-2, 1}, {-1, 2}, {1, 2}, {2, 1},
    {2, -1}, {1, -2}, {-1, -2}, {-2, -1},
  };

  for (int i = 0; i < 8; i++) {
    Piece knight = state.board[checkRank + knightOffsets[i][0]][checkFile  + knightOffsets[i][1]];
    if (knight.type == KNIGHT && knight.color == oppositeColor) {
      return 1;
    }
  }

	return 0;
}

int isLegalRookMove(GameState *state, Move *move) {
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  if (fromFile != toFile &&  fromRank != toRank) {
    return 0;
  }

  int vertical = fromFile - toFile == 0 ? 1 : 0;

  int verOffset = 0, horOffset = 0;

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

int isLegalBishopMove(GameState *state, Move *move) {
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
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

  Piece moveSquare = checkTileForPiece(state, fromRank, fromFile, rankOffset, fileOffset, toRank + rankOffset, toFile + fileOffset);
  if (moveSquare.type != EMPTY) {
    return 0;
  }

  return 1;
}

int isLegalKnightMove(GameState *state, Move *move) {
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;

  if (abs(fromFile - toFile) == 2 && abs(fromRank - toRank) == 1 ||
      abs(fromFile - toFile) == 1 && abs(fromRank - toRank) == 2) {
    return 1;
  }

  return 0;
}

int isLegalPawnMove(GameState *state, Move *move) {
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;
  int rankDiff = fromRank - toRank, fileDiff = fromFile - toFile;
  int whiteToMove = state->whiteToMove;

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

int isLegalKingMove(GameState *state, Move *move) {
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;

  if (abs(fromFile - toFile) > 1 || abs(fromRank - toRank) > 1 ||
      (abs(fromRank - toFile) == 0 && abs(fromRank - toRank) == 0)) {
    return 0;
  }

	return 1;
}

int isLegalMove(GameState *state, Move *move) {
  int legal;
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;

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

int makeMove(GameState *state, Move *move) {
  int fromFile = move->fromFile, fromRank = move->fromRank, toFile = move->toFile, toRank = move->toRank;

  if (fromFile == toFile && fromRank == toRank) {
    printf("Invalid move (piece didn't move)\n> ");
    return 0;
  }

  Piece piece = state->board[fromRank][fromFile];

  // Check for empty square
  if (piece.color == NONE || piece.type == EMPTY) {
    printf("Invalid move (no piece at %c%d)\n> ", 'a' + fromFile, fromRank + 1);
    return 0;
  }

  // Check for the right color move
  if ((piece.color == WHITE && !state->whiteToMove) ||
      (piece.color == BLACK && state->whiteToMove)) {
    printf("Invalid move (wrong color to move)\n> ");
    return 0;
  }

  if (!isLegalMove(state, move)) {
    printf("Invalid move (illegal move)\n> ");
    return 0;
  }

  if (state->board[fromRank][fromFile].color == state->board[toRank][toFile].color) {
    printf("Invalid move (illegal move)\n> ");
    return 0;
  }

  if (isCheck(state, move, &piece)) {
    printf("Invalid move (check after move)\n> ");
    return 0;
  }

  moveInBoard(state, move, &piece);

  return 1;
}

void displayBoard(GameState *state) {
  printf("\e[1;1H\e[2J");
//  system("cls");
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
          charToPrint = '.';
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

void replayGame(Move *moves, int gameLength) {
  GameState state;
  initializeBoard(&state);

  for (int i = 0; i < gameLength; i++) {
    makeMove(&state, &moves[i]);
  }

  displayBoard(&state);
}

void insert_moves(GameState *state, Move *moves, int *gameLength) {
  char move[5], input_buffer[20];
  printf("\nEnter move:\n> ");
  while (scanf("%s", input_buffer) == 1) {
    if (strcmp(input_buffer, "x") == 0) {
      break;
    }

    // copy move from buffer
    for (int i = 0; i < 4; i++) {
      move[i] = input_buffer[i];
    }
    move[4] = '\0';

    if (parseMove(move, &moves[*gameLength])) {
      if (makeMove(state, &moves[*gameLength])) {
        *gameLength += 1;
        displayBoard(state);
        printf("\nEnter move:\n> ");
      }
    } else {
      printf("Invalid move: %s (wrong input)\n> ", move);
    }
    fflush(stdin);
  }
}

void insertGame(Move *moves, int *gameLength) {
  GameState state;
  initializeBoard(&state);
  displayBoard(&state);
  *gameLength = 0;

  insert_moves(&state, moves, gameLength);
}

void continue_editing(Move *moves, int *gameLength) {
  GameState state;
  initializeBoard(&state);

  for (int i = 0; i < *gameLength; i++) {
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

void view_record(Move *moves, int *gameLength) {
  char moveStr[5];

  printf("\n");
  for (int i = 0; i < *gameLength; i++) {
    unparse_move(&moves[i], moveStr);
    printf(" %d. %s\n", i + 1, moveStr);
  }
  printf("\n");
}

void insert_in_record(Move *origMoves, int *gameLength) {
  int num_buf, exit_flag = 0;
  char move_str[5], move_buf[20];

  Move moves[MAX_RECORD_SIZE];
  int oldGamelength = *gameLength;

  for (int i = 0; i < *gameLength; i++) {
    moves[i] = origMoves[i];
  }

  while (!exit_flag) {
    printf("\nEnter the move you want to insert after:\n> ");

    while (scanf("%i", &num_buf) != 1) {
      printf("Wrong input! Try again...\n> ");
      fflush(stdin);
    }

    if (num_buf > *gameLength || num_buf < 0) {
      printf("Wrong input! Try again...\n");
      continue; 
    }

    while (1) {
      printf("Enter a move you want to add, x when finished or z to discard changes:\n> ");

      while (scanf("%s", move_buf) != 1) {
        printf("Wrong input! Try again...\n> ");
        fflush(stdin);
      }

      if (move_buf[0] == 'x') {
        GameState state;
        initializeBoard(&state);
        for (int i = 0; i < *gameLength; i++) {
          if (!makeMove(&state, &moves[i])) {
            printf("Unable to save changes since the record has mistakes!\n");
            *gameLength = oldGamelength;
            return;
          }
        }

        for (int i = 0; i < *gameLength; i++) {
          origMoves[i] = moves[i];
        }

        exit_flag = 1;
        break;
      }

      if (move_buf[0] == 'z') {
        printf("Discarding changes!\n");
        *gameLength = oldGamelength;

        exit_flag = 1;
        return;
      }

      for (int i = 0; i < 4; i++) {
        move_str[i] = move_buf[i];
      }
      move_str[4] = '\0';

      Move move;
      if (parseMove(move_str, &move)) {
        if (*gameLength >= 100) {
          printf("Possible buffer overflow, either stop inserting moves or discard all changes!\n");
          continue;
        }

        for (int i = *gameLength - 1; i >= num_buf; i--) {
          moves[i + 1] = moves[i];
        }

        moves[num_buf] = move;

        num_buf++;

        *gameLength += 1;
      } else {
        printf("Wrong input! Try again...\n");
        fflush(stdin);
      }
    }

  } 
}

void remove_from_record(Move* origMoves, int *gameLength) {
  int num_buf, exit_flag = 0;
  char option_buf[20];

  Move moves[MAX_RECORD_SIZE];
  int oldGamelength = *gameLength;

  for (int i = 0; i < *gameLength; i++) {
    moves[i] = origMoves[i];
  }
  
  while (!exit_flag) {
    printf("\nEnter the move you want to remove or 0 to finish:\n> ");

    while (scanf("%i", &num_buf) != 1) {
      printf("Wrong input! Try again...\n> ");
      fflush(stdin);
    }

    if (num_buf == 0) {
      for (int i = 0; i < *gameLength; i++) {
        origMoves[i] = moves[i];
      }
      return;
    }

    if (num_buf > *gameLength || num_buf <= 0) {
      printf("Wrong input! Try again...\n");
      continue; 
    }

    for (int i = num_buf - 1; i < *gameLength - 1; i++) {
      moves[i] = moves[i + 1];
    }

    *gameLength -= 1;

    while (1) {
      printf("Enter c to continue removing, x to finish removing, v to insert new moves or z to discard changes:\n> ");

      while (scanf("%s", option_buf) != 1) {
        printf("Wrong input! Try again...\n> ");
        fflush(stdin);
      }

      if (option_buf[0] == 'c') {
        break;
      }

      if (option_buf[0] == 'x') {
        GameState state;
        initializeBoard(&state);
        for (int i = 0; i < *gameLength; i++) {
          if (!makeMove(&state, &moves[i])) {
            printf("Unable to save changes since the record has mistakes!\n");
            *gameLength = oldGamelength;
            return;
          }
        }

        for (int i = 0; i < *gameLength; i++) {
          origMoves[i] = moves[i];
        }

        exit_flag = 1;
        break;
      }

      if (option_buf[0] == 'z') {
        printf("Discarding changes!\n");
        *gameLength = oldGamelength;

        exit_flag = 1;
        return;
      }

      if (option_buf[0] == 'v') {
        insert_in_record(moves, gameLength);
        break;
      }
    }

  } 
}

void edit_prompt(Move *moves, int *gameLength) {
  int option;

  while (1) {
    printf("Select an option:\n");
    printf("  1. View the record\n");
    printf("  2. Continue editing the record\n");
    printf("  3. Insert moves to the record\n");
    printf("  4. Remove moves from the record\n");
    printf("  5. Delete the record\n");
    printf("  6. Return to menu\n\n> ");

    while (scanf("%d", &option) != 1) {
      printf("Wrong input! Try again...\n> ");
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
        insert_in_record(moves, gameLength);
        break;
      case 4:
        remove_from_record(moves, gameLength);
        break;
      case 5:
        *gameLength = 0;
        break;
      case 6:
        return;
        break;
      default:
        printf("There's no such option. Try again...\n> ");
        break;
    }
    fflush(stdin);
  }
}

int main(void) {
  Move moves[MAX_RECORD_SIZE];
  int option = 0, replaySize = 0;
  int *gameLength = malloc(sizeof(int));
  *gameLength = 0;

  // no en passant, no castling, format is: e2e4 (square from, square to)
  while (1) {
    printf("Select an option:\n");
    printf("  1. Insert a game\n");
    printf("  2. Replay a game\n");
    printf("  3. Edit record\n\n> ");

    while (scanf("%d", &option) != 1) {
      printf("Wrong input! Try again...\n> ");
      fflush(stdin);
    }

    switch (option) {
      case 1:
        insertGame(moves, gameLength);
        break;
      case 2:
        printf("Enter the number of move you want to see:\n\n> ");
        while (scanf("%d", &replaySize) != 1) {
          printf("Wrong input! Try again...\n> ");
        }
        if (replaySize <= 0 || replaySize > *gameLength) {
          printf("The move number is out of range!\n");
          break;
        }
        replayGame(moves, replaySize);
        break;
      case 3:
        edit_prompt(moves, gameLength);
        break;
      default:
        printf("There's no such option. Try again...\n> ");
        break;
    }
    fflush(stdin);
  }
  
  return 0;
}