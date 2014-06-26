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
int depthply;
int init_board[64]={ 
	4, 2, 3, 5, 6, 3, 2, 4,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    9, 9, 9, 9, 9, 9, 9, 9,
   12,10,11,13,14,11,10,12
   };
   
int  board[64];
int tboard[64];
int history_board[64][64];

bool slide[7] = {
	FALSE,FALSE, FALSE, TRUE, TRUE, TRUE, FALSE
};

int number_of_move[7] = {
	0, 0, 8, 4, 4, 8, 8
};

int pieces_value[7] = {
	V_NULL, 
	V_PAWN, 
	V_KNIGHT, 
	V_BISHOP, 
	V_ROOK, 
	V_QUEEN, 
	V_KING
	};
	
int movement[7][8] = {
	{   0,   0,   0,  0, 0,  0,  0,  0 },	//dummy
	{   10,  9,  11,  0, 0,  0,  0,  0 },	//creep
	{ -21, -19, -12, -8, 8, 12, 19, 21 },	//dullahan knight
	{ -11,  -9,   9, 11, 0,  0,  0,  0 },	//bishop
	{ -10,  -1,   1, 10, 0,  0,  0,  0 }, 	//barathum rook
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

/*
	this is our monster!!
*/
int maxi( int depth, bool maximize ) {
	int max;		//max or min
	int i;			//scan board
	int n_move;		//number of move
	int score;		//score for minimax
	int lpiece;
	int lcolor;
	int des_move; 	//destination move
    if ( depth <= 0 ) return evaluate();
	if ( maximize )
	{
        max = -V_MAX;
		
		history_board[depth]=tboard;		//record history (of three kingdom)
		for ( i=0; i<64; i++ ) {			//scan board
			lpiece = tboard[i] & MASK_PIECE;
			lcolor = tboard[i] & MASK_COLOR;
			if(( lpiece != 0) && ( lcolor == COLOR_WHITE))
			{
				if(lpiece==PAWN)
				{
					for ( n_move = 0; n_move < 3; n_move++ )
					{
						des_move = movement[lpiece][n_move];	/* des_move = -movement[lpiece][n_move]; for BLACK */
						if(check_pawn_move(i,des_move)
						{
							move_piece(i,des_move);
							score = maxi( depth - 1, false );        
							if( score > max ) max = score;
							tboard=history_board[depth];
						}
					}
					
				}else
				{
					for ( n_move = 0; n_move < number_of_move[lpiece]; n_move++ ) {	
					
						des_move=0;
						do
						{
							des_move=des_move+movement[lpiece][n_move];
							if(!check_move(i,des_move))
							{
								break;
							}
						
							if(lcolor!=check_target_color(i,des_move))	//if capture
							{
								move_piece(i,des_move);
								score = maxi( depth - 1, false );        
								if( score > max ) max = score;
								tboard=history_board[depth];			//return board
								break;									//stop
							}
						
							move(i,des_move);
							score = maxi( depth - 1, false );        
							if( score > max ) max = score;
							tboard=history_board[depth];				//return board
						
						}while(slide[lpiece]);					
					}
				}
			}
		}
		
		return max;
	}else
	{
		/*
		max = V_MAX;
		history_board[depth]=board;	
		
		for ( i=0; i<64; i++ ) {	//scan beard
			for( all moves) {
				move(pawn,moves);
				score = maxi( depth - 1, true );
				if( score < max ) max = score;
				board=history_board[depth];
			}
		}*/		
		
		return max;
	}
}
 /**
	check_move mechanism:
	1. check if out of bound
	2. check if empty
	3. check friendly piece
 
 */
bool check_move(int piece_pos, int des_move)
{
	int lpiece=board[piece_pos] & MASK_PIECE;
	int lcolor=board[piece_pos] & MASK_COLOR;
	int lcolor_target;
	int lpiece_target;
	
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index
	
	//1. check boundary
	if(destination==-1)
	{
		//out of bound
		return false;
	}else{
		lcolor_target = board[destination] & MASK_COLOR;
		lpiece_target = board[destination] & MASK_PIECE;
		if(lpiece_target) == 0)	//2. check if empty
		{
			return true;
		}else										//3. if not empty, check their color
		{
			
			if(lcolor==lcolor_target)				//you cannot attack same color
			{
				//hold your attack, its our ally
				return false;
			}else
			{
				//yes, kill him!!
				return true;
			}
		}
	}	
}

int check_target_color(int piece_pos, int des_move)
{
	int lpiece=board[piece_pos] & MASK_PIECE;
	int lcolor=board[piece_pos] & MASK_COLOR;
	int lcolor_target;
	
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index
	
	if(destination==-1)
	{
		//out of bound,return -1
		return -1;
	}else
	{
		return ( tboard[destination] & MASK_COLOR );
	}
}

bool check_pawn_move(int piece_pos, int des_move)
{
	int lpiece=board[piece_pos] & MASK_PIECE;
	int lcolor=board[piece_pos] & MASK_COLOR;
	int lcolor_target;
	int lpiece_target;
	
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index
	
	//1. check boundary
	if(destination==-1)
	{
		//out of bound
		return false;
	}else{
		lcolor_target=tboard[destination] & MASK_COLOR;
		lpiece_target=tboard[destination] & MASK_PIECE;
		if((des_move==10)||(des_move==-10))
		{
			if(lpiece_target == 0)	//2. check if empty
			{
				return true;
			}else
			{
				return false;
			}
		}
		if((des_move==9)||(des_move==-9)||(des_move==11)||(des_move==-11))
		{
			if( (lpiece_target != 0) && (lcolor_target!=lcolor))	//2. check if empty
			{
				return true;
			}else
			{
				return false;
			}
		}
	}
}

bool check_move_player(int piece_pos, int des_move)
{
	
}

int move_piece(int piece_pos, int des_move)
{
	int tpiece=tboard[piece_pos];
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index
	
	if(destination==-1)
	{
		//out of bound
		return -1;
	}else{
		tboard[destination]=tpiece;
		tboard[piece_pos]=EMPTY;
		return 1;
	}
}

void copy_board(int board_des[64], int board_src[64])
{
	int i;
	for(i=0;i<64;i++)
	{
		board_des[i]=board_src[i];
	}
}

//its just dummy main function
int not_main()
{
	
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