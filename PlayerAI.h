#ifndef PLAYERAI_H
#define PLAYERAI_H
#include "main.h"

struct ScoreMovePair
{
	uint32_t move;
	float score;
};

struct VNode
{
	uint32_t item;
	struct VNode *next_node;
};

/*struct MoveCapturePair
{
	uint32_t key;
	uint32_t value;
	struct MoveCapturePair *next_node;
}

struct MoveCaptureHeader
{
	size_t size;
	struct MoveCapturePair *first_node;
}*/

void playerAI_init(uint32_t o_board, uint32_t x_board, uint32_t kings);

void ai_next_move(bool player, uint32_t o_board, uint32_t x_board, uint32_t kings);

uint32_t playerAI_o_board();
uint32_t playerAI_x_board();
uint32_t playerAI_kings();

#endif
