#define V_NULL		0
#define V_PAWN		100
#define V_KNIGHT	300
#define V_BISHOP	325
#define V_ROOK		500
#define V_QUEEN		900
#define V_KING		30000
#define V_MAX		900000

#define EMPTY			0
#define PAWN			1
#define KNIGHT			2
#define BISHOP			3
#define ROOK			4
#define QUEEN			5
#define KING			6

//array 0 for dummy

BOOL slide[7] = {
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

int movement[7][8] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 },		//dummy
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -21, -19, -12, -8, 8, 12, 19, 21 },
	{ -11, -9, 9, 11, 0, 0, 0, 0 },
	{ -10, -1, 1, 10, 0, 0, 0, 0 },
	{ -11, -10,  -9, -1, 1,  9, 10, 11 },
	{ -11, -10,  -9, -1, 1,  9, 10, 11 }
};

int evaluate(int* board)
{
	int i;
	int total=0;
	int lpiece;
	int addpoint=0;
	
	for(i=0;i<64;i++)
	{
		addpoint=point[board[i]];		
		lpiece=color[i];
		if(lpiece)
		{
			total+=addpoint;
		}else
		{
			total-=addpoint;
		}
	}
	return total;
}


int maxi( int depth, bool maximize ) {
	int max;
	int i;
	int score;
    if ( depth == 0 ) return evaluate();
	if ( maximize )
	{
        max = -V_MAX;
		
		history_board[depth]=board;		//record history (of three kingdom)
		for ( i=0; i<64; i++ ) {	//scan board
			for ( /*all moves*/ ) {	
				move(pawn,moves);
				score = maxi( depth - 1, false );        
				if( score > max ) max = score;
			}
		}
		board=history_board[depth];		//return board
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
			}
		}
		
		board=history_board[depth];
		return max;
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