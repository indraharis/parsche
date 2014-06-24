#define V_NULL		0
#define V_PAWN		100
#define V_KNIGHT	300
#define V_BISHOP	325
#define V_ROOK		500
#define V_QUEEN		900
#define V_KING		30000
#define V_MAX		900000

// PIECE_CODE
#define EMPTY			0
#define PAWN			1
#define KNIGHT			2
#define BISHOP			3
#define ROOK			4
#define QUEEN			5
#define KING			6

#define MASK_COLOR		8
#define MASK_PIECE		7

#define COLOR_WHITE		0
#define COLOR_BLACK		8

/*
	piece = COLOR_X or PIECE_CODE
	1  = white pawn
	9  = black pawn
	10 = black knight
	dst	
*/

//array 0 for dummy
int ply;
int gameply;
int board[64]={ 
	4, 2, 3, 5, 6, 3, 2, 4,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    9, 9, 9, 9, 9, 9, 9, 9,
   12,10,11,13,14,11,10,12
   };
int history_board[64][64];

bool slide[7] = {
	FALSE,FALSE, FALSE, TRUE, TRUE, TRUE, FALSE
};

int point[7] = {
	0, 	//EMPTY
	100, 	//PAWN
	300, 	//KNIGHT
	325, 	//BISHOP
	500, 	//ROOK
	900, 	//QUEEN
	30000	//KING
};

int number_of_move[7] = {
	0, 0, 8, 4, 4, 8, 8
};

int pieces_value[7] = {
	V_NULL, V_PAWN, V_KNIGHT, V_BISHOP, V_ROOK, V_QUEEN, V_KING
	};
	
int movement[7][8] = {
	{   0,   0,   0,  0, 0,  0,  0,  0 },				//dummy
	{   0,   0,   0,  0, 0,  0,  0,  0 },				//creep
	{ -21, -19, -12, -8, 8, 12, 19, 21 },	//dullahan knight
	{ -11,  -9,   9, 11, 0,  0,  0,  0 },		 	//bishop
	{ -10,  -1,   1, 10, 0,  0,  0,  0 }, 	 	//barathum rook
	{ -11, -10,  -9, -1, 1,  9, 10, 11 },	//queen of fire, candidate for stepmother
	{ -11, -10,  -9, -1, 1,  9, 10, 11 }	//useless king
};

int mailbox[120] = {
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
	 -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
	 -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
	 -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
	 -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
	 -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
	 -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
	 -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

int mailbox64[64] = {
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98
};

int evaluate()
{
	int i;
	int total=0;
	int lpiece;
	int lcolor;
	
	for(i=0;i<64;i++)
	{
	
		lpiece = board[i] & MASK_PIECE;
		lcolor = board[i] & MASK_COLOR;
		if(lcolor == 0)
		{
			total = total + pieces_value[lpiece];
		}else
		{
			total = total - pieces_value[lpiece];
		}
	}
	return total;
}


int maxi( int depth, bool maximize ) {
	int max;	//max or min
	int i;		//scan board
	int n_move;	//number of move
	int score;	//score for minimax
	int lpiece;
	int lcolor;
    if ( depth <= 0 ) return evaluate();
	if ( maximize )
	{
        max = -V_MAX;
		
		history_board[depth]=board;		//record history (of three kingdom)
		for ( i=0; i<64; i++ ) {		//scan board
			lpiece = board[i] & MASK_PIECE;
			lcolor = board[i] & MASK_COLOR;
			if(( lpiece != 0) && ( lcolor == COLOR_WHITE))
			{
				for ( n_move = 0; n_move < number_of_move[lpiece]; n_move++ ) {	
					
					move(pawn,moves);
					score = maxi( depth - 1, false );        
					if( score > max ) max = score;
					board=history_board[depth];		//return board
					
				}
			}
		}
		
		return max;
	}else
	{
		max = V_MAX;
		history_board[depth]=board;	
		
		for ( i=0; i<64; i++ ) {	//scan beard
			for( /*all moves*/) {
				move(pawn,moves);
				score = maxi( depth - 1, true );
				if( score < max ) max = score;
				board=history_board[depth];
			}
		}		
		
		return max;
	}
}
 
int check_move(int piece_pos, int piece_des)
{
	
}

int move_piece(int piece_pos, int n_move)
{
	int lpiece=board[piece_pos] & MASK_PIECE;
	int lcolor=board[piece_pos] & MASK_COLOR;
	int lcolor_target;
	
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int get_move_value=movement[lpiece][n_move];	//convert direction into movement value
	int destination = mailbox[mb_pos+move_value];	
	
	if(destination==-1)
	{
		return -1;
	}else{
		return board[destination];
	}	
}

/* //from wiki-wikipedia
function minimax(node, depth, maximizingPlayer)
    if depth = 0 or node is a terminal node
        return the heuristic value of node
    if maximizingPlayer
        bestValue := -∞
        for each child of node
            val := minimax(child, depth - 1, FALSE)
            bestValue := max(bestValue, val)
        return bestValue
    else
        bestValue := +∞
        for each child of node
            val := minimax(child, depth - 1, TRUE)
            bestValue := min(bestValue, val)
        return bestValue
*/