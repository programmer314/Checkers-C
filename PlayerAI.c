#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include "PlayerAI.h"
#include "hash_table.h"

#define USE_ALPHABETA true
#define DEPTH 9

#define max(x,y) (((x) >= (y)) ? (x) : (y))
#define min(x,y) (((x) <= (y)) ? (x) : (y))

// board is {uint32_t x_board, uint32_t o_board, uint32_t kings}

uint32_t board[3];

void update(uint32_t o_board, uint32_t x_board, uint32_t kings)
{
	board[0] = x_board;
	board[1] = o_board;
	board[2] = kings;
}

void playerAI_init(uint32_t o_board, uint32_t x_board, uint32_t kings)
{
	update(o_board, x_board, kings);
}

uint32_t playerAI_o_board() { return board[1]; }
uint32_t playerAI_x_board() { return board[0]; }
uint32_t playerAI_kings() { return board[2]; }

int bitsize(uint32_t num)
{
    int count = num & 1;
    while(num >>= 1)
        count += num & 1;

    return count;
}

struct VNode* new_vnode()
{
	struct VNode vnode;
	vnode.next_node = NULL;
	
	struct VNode* result = malloc(sizeof(struct VNode));
	(*result) = vnode;
	
	return result;
}

float evaluate(uint32_t board[])
{
	int scores[4];
	
	scores[0] = bitsize(board[0] & (~board[2])); // X pieces
	scores[1] = bitsize(board[1] & (~board[2])); // O pieces
	scores[2] = bitsize(board[0] & board[2]);    // X kings
	scores[3] = bitsize(board[1] & board[2]);    // O kings
	
	return (scores[0] - scores[1]) * 3 + (scores[2] - scores[3]) * 5;
}

// return 0 if move is out of bounds
uint32_t ai_move(uint32_t pos, bool left, bool forward, bool o_turn)
{
    bool shift_left = o_turn == forward;
    int shift_value = 5;

    if (pos & EVEN_MASK)
        shift_value--;
    if (left)
        shift_value--;
    if (!shift_left)
        shift_value = 8 - shift_value;

    if ((left && (pos & L_COL_MASK)) || (!left && (pos & R_COL_MASK)))
        return 0;

    if (shift_left)
        pos <<= shift_value;
    else
        pos >>= shift_value;

    return pos;
}

void simulate_move(uint32_t board[], uint32_t new_board[], uint32_t move, bool is_o, bool promote)
{
	uint32_t captures = move & board[!is_o];
	move &= ~captures;
	
	new_board[is_o] = board[is_o];
	if (move & ~board[is_o])
		new_board[is_o] ^= move;
	
	new_board[!is_o] = board[!is_o] & ~captures;
	
	new_board[2] = board[2] & ~captures;
	if ((new_board[2] & move) && (move & ~new_board[2]))
		new_board[2] ^= move;
	
	if (promote)
		new_board[2] |= (new_board[0] & O_BACK_RANK) | (new_board[1] & X_BACK_RANK);
}

struct HashTable* get_piece_moves_inner(uint32_t board[], uint32_t pos, uint32_t origin, uint32_t captures)
{
	struct HashTable* moves = malloc(sizeof(struct HashTable));
	(*moves) = newHashTable();
	
	const uint32_t x_board = board[0], o_board = board[1], kings = board[2];
	bool o_turn = pos & o_board;
	const uint32_t player = board[o_turn], opponent = board[!o_turn];
	
	if (captures)
		hashTable_update(moves, pos | origin, captures);
	
	bool forward = true, left = true;
	struct HashTable* innerResult;
	do
	{
		uint32_t new_pos = ai_move(pos, left, forward, o_turn);
		
		if (new_pos)
		{
			if (!captures && !(new_pos & (o_board | x_board)))
				hashTable_update(moves, pos | new_pos, 0);
			else
			{
				uint32_t capture_pos = new_pos;
				uint32_t new_pos = ai_move(capture_pos, left, forward, o_turn);
				
				if (new_pos && !(captures & capture_pos) && (capture_pos & opponent) &&
					((new_pos & origin) || !(new_pos & (o_board | x_board))))
				{
					uint32_t new_board[3];
					simulate_move(board, new_board, pos | new_pos | capture_pos, o_turn, false);
					
					innerResult = get_piece_moves_inner(new_board, new_pos, origin, captures | capture_pos);
					for (size_t i = 0; i < innerResult->key_count; i++)
					{
						uint32_t key = innerResult->keys[i];
						uint32_t cur_captures = map_get(innerResult->map, key);
						uint32_t prev_captures = map_get(moves->map, key);
						if (!prev_captures || bitsize(prev_captures) < bitsize(cur_captures))
							hashTable_update(moves, key, cur_captures);
						
						map_free(innerResult->map, key);
					}
					free(innerResult);
				}
			}
		}
		
		left = !left;
		if (left && (kings & pos))
			forward = !forward;
		
	} while (!left || !forward);
	
	return moves;
}

struct HashTable* get_piece_moves(uint32_t board[], uint32_t pos)
{
	return get_piece_moves_inner(board, pos, pos, 0);
}

struct VNode* get_all_moves(uint32_t board[], bool is_o)
{
	struct VNode* moves = new_vnode();
	struct VNode* cur_move = moves;
	
	struct HashTable* innerResult;
	for (uint32_t pos = 1; pos > 0; pos <<= 1)
	{
		if (board[is_o] & pos)
		{
			innerResult = get_piece_moves(board, pos);
			for (size_t i = 0; i < innerResult->key_count; i++)
			{
				uint32_t move = innerResult->keys[i];
				uint32_t captures = map_get(innerResult->map, move);
				cur_move->item = move | captures;
				
				cur_move->next_node = new_vnode();
				cur_move = cur_move->next_node;
				
				map_free(innerResult->map, move);
			}
			free(innerResult);
		}
	}
		
	return moves;
}

void free_vnodes(struct VNode* vnode)
{
	if (vnode == NULL)
		return;
	
	struct VNode* next_node;
	do
	{
		next_node = vnode->next_node;
		free(vnode);
		vnode = next_node;
	} while (next_node != NULL);
}

struct ScoreMovePair minimax_inner(uint32_t board[], int depth, bool max_player)
{
	struct ScoreMovePair result;
	
	if (depth == 0) 
	{
		result.score = evaluate(board);
		return result;
	}
	
	float evalLimit;
	uint32_t best_move;
	
	if (max_player)
	{
		evalLimit = -FLT_MAX;
		struct VNode *moves = get_all_moves(board, false);
		struct VNode *move = moves;
		if (move != NULL) do
		{
			uint32_t temp_board[3];
			simulate_move(board, temp_board, move->item, false, true);
			
			float evaluation = minimax_inner(temp_board, depth - 1, false).score;
			if (evalLimit < evaluation)
				best_move = move->item;
			
			evalLimit = max(evalLimit, evaluation);
			
			move = move->next_node;
			
		} while (move != NULL);
		free_vnodes(moves);
	}
	else
	{
		evalLimit = FLT_MAX;
		struct VNode *moves = get_all_moves(board, true);
		struct VNode *move = moves;
		if (move != NULL) do
		{
			uint32_t temp_board[3];
			simulate_move(board, temp_board, move->item, true, true);
			
			float evaluation = minimax_inner(temp_board, depth - 1, true).score;
			if (evalLimit > evaluation)
				best_move = move->item;
			
			evalLimit = min(evalLimit, evaluation);
			
			move = move->next_node;
			
		} while (move != NULL);
		free_vnodes(moves);
	}
	
	result.move = best_move;
	if (best_move)
		result.score = evalLimit;
	else
		result.score = evaluate(board);
	
	return result;
}

void minimax(int depth, bool max_player)
{
	uint32_t best_move = minimax_inner(board, depth, max_player).move;
	simulate_move(board, board, best_move, !max_player, true);
}

struct ScoreMovePair alphabeta_inner(uint32_t board[], int depth, bool max_player, float alpha, float beta)
{
	struct ScoreMovePair result;
	
	if (depth == 0)
	{
		result.score = evaluate(board);
		return result;
	}
	
	float evalLimit;
	uint32_t best_move;
	
	if (max_player)
	{	
		evalLimit = -FLT_MAX;
		struct VNode *moves = get_all_moves(board, false);
		struct VNode *move = moves;
		if (move != NULL) do
		{
			uint32_t temp_board[3];
			simulate_move(board, temp_board, move->item, false, true);
			
			float evaluation = alphabeta_inner(temp_board, depth - 1, false, alpha, beta).score;
			if (evalLimit < evaluation)
				best_move = move->item;
			
			evalLimit = max(evalLimit, evaluation);
			
			alpha = max(alpha, evalLimit);
			if (beta <= alpha)
				break;
			
			move = move->next_node;
			
		} while (move != NULL);
		free_vnodes(moves);
	}
	else
	{	
		evalLimit = FLT_MAX;
		struct VNode *moves = get_all_moves(board, true);
		struct VNode *move = moves;
		if (move != NULL) do
		{
			uint32_t temp_board[3];
			simulate_move(board, temp_board, move->item, true, true);
			
			float evaluation = alphabeta_inner(temp_board, depth - 1, true, alpha, beta).score;
			if (evalLimit > evaluation)
				best_move = move->item;
			
			evalLimit = min(evalLimit, evaluation);
			
			beta = min(beta, evalLimit);
			if (beta <= alpha)
				break;
			
			move = move->next_node;
			
		} while (move != NULL);
		free_vnodes(moves);
	}
	
	result.move = best_move;
	if (best_move)
		result.score = evalLimit;
	else
		result.score = evaluate(board);
	
	return result;
}

void alphabeta(int depth, bool max_player)
{
	uint32_t best_move = alphabeta_inner(board, depth, max_player, -FLT_MAX, FLT_MAX).move;
	simulate_move(board, board, best_move, !max_player, true);
}

void ai_next_move(bool player, uint32_t o_board, uint32_t x_board, uint32_t kings)
{
	update(o_board, x_board, kings);
	
	if (USE_ALPHABETA)
		alphabeta(DEPTH, player);
	else
		minimax(DEPTH, player);
}