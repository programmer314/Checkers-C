#ifndef MAIN_H
#define MAIN_H

#define O_PIECE 'O'
#define X_PIECE 'X'
#define O_KING '@'
#define X_KING '#'
#define SPACE '*'

/*const uint32_t EVEN_MASK = 252645135;
const uint32_t ROW_END_MASK = 4026531855;
const uint32_t O_BACK_RANK = 15;
const uint32_t X_BACK_RANK = 4026531840;
const uint32_t L_COL_MASK = 16843009;
const uint32_t R_COL_MASK = 2155905152;

uint32_t x_board = 4293918720;
uint32_t o_board = 4095;
uint32_t kings = 0;*/

#define EVEN_MASK 252645135
#define ROW_END_MASK 4026531855
#define O_BACK_RANK 15
#define X_BACK_RANK 4026531840
#define L_COL_MASK 16843009
#define R_COL_MASK 2155905152

/*bool o_turn = true;
bool o_wins = false, x_wins = false;
uint32_t piece_pos;*/

// PlayerAI playerAI(o_board, x_board, kings);

/*bool is_space(uint32_t pos);

char getKingSide(char king);

bool same_side(uint32_t pos, char piece);

void add_piece(bool is_o, uint32_t pos, bool is_king);

void remove_piece(bool is_o, uint32_t pos);

uint32_t move(uint32_t pos, bool left, bool forward, bool o_turn);
uint32_t move(uint32_t pos, bool left, bool forward);

char getPiece(uint32_t pos);

void available_captures(bool* captures, uint32_t pos);

bool has_move(uint32_t pos);

bool is_winner(bool o_turn);

void change_turn();

char getTurnChar();

int main();*/

#endif