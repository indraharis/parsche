#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

int false_init_board[64]={
	0, 0, 0, 0, 6, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 2, 0, 0, 5,
	0, 0, 0, 0, 0, 0, 0, 0,
    9, 9, 9, 9, 9, 9, 9, 9,
	0, 0, 0,13,14,11, 0, 0
   };

int  board[64];
int tboard[64];
int history_board[64][64];


bool slide[7] = {
	false, false, false, true, true, true, false
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
	{   0,   0,   0,  0,  0,  0,  0,  0 },	//dummy
	{   10,  9,  11,  0,-10, -9,-11,  0 },	//creep
	{ -21, -19, -12, -8,  8, 12, 19, 21 },	//dullahan knight
	{ -11,  -9,   9, 11,  0,  0,  0,  0 },	//bishop
	{ -10,  -1,   1, 10,  0,  0,  0,  0 }, 	//barathum rook
	{ -11, -10,  -9, -1,  1,  9, 10, 11 },	//queen of fire, candidate for stepmother
	{ -11, -10,  -9, -1,  1,  9, 10, 11 }	//useless king
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

int castling_white_left[2]  = {  5,  6};
int castling_white_right[3] = {  1,  2, 3};
int castling_black_left[3]  = { 57, 58, 59};
int castling_black_right[2] = { 61, 62};

bool bWhiteAllowCastlingL=true;
bool bWhiteAllowCastlingR=true;
bool bBlackAllowCastlingL=true;
bool bBlackAllowCastlingR=true;

bool bWhiteCheck=false;
bool bBlackCheck=false;

int start_pos_move;
int des_pos_move;

FILE* fftest;

int evaluate();
int evaluatebd(int bd[64]);
int maxi( int depth, bool maximize );
bool check_move(int piece_pos, int des_move);
bool check_target_color(int piece_pos, int des_move);
bool check_pawn_move(int piece_pos, int des_move);
bool check_move_player(int piece_pos, int des_move);
int move_player_piece(int piece_pos, int des_move);
int move_pseudo_piece(int piece_pos, int des_move);
int move_pseudo_pawn(int piece_pos, int des_move);
int move_piece(int piece_pos, int des_move);
int move_pawn(int piece_pos, int des_move);

void copy_board(int board_des[64], int board_src[64]);
void display(int bd[64]);
void printboard(int bd[64],FILE* tfile);
int not_main();

int evaluate()
{
	int i;
	int total=0;
	int lpiece;
	int lcolor;

	for(i=0;i<64;i++)
	{

		lpiece = tboard[i] & MASK_PIECE;
		lcolor = tboard[i] & MASK_COLOR;
		if(lcolor == COLOR_WHITE)
		{
			total = total + pieces_value[lpiece];
		}else
		{
			total = total - pieces_value[lpiece];
		}
	}
	return total;
}

int evaluatebd(int bd[64])
{
	int i;
	int total=0;
	int lpiece;
	int lcolor;

	for(i=0;i<64;i++)
	{

		lpiece = bd[i] & MASK_PIECE;
		lcolor = bd[i] & MASK_COLOR;
		if(lcolor == COLOR_WHITE)
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
	int lbestpos;
	int lbestmove;
    if ( depth <= 0 ) return evaluate();
	if ( maximize )
	{
	    printf("depth %d\n ",depth);
        max = -V_MAX;

		copy_board( history_board[depth], tboard);		//record history (of three kingdom)
		for ( i=0; i<64; i++ ) {						//scan board
			lpiece = tboard[i] & MASK_PIECE;
			lcolor = tboard[i] & MASK_COLOR;
			if(( lpiece != EMPTY) && ( lcolor == COLOR_WHITE))
			{
				if(lpiece==PAWN)
				{
					for ( n_move = 0; n_move < 3; n_move++ )
					{
						des_move = movement[lpiece][n_move];	/* des_move = -movement[lpiece][n_move]; if its BLACK */
						if(check_pawn_move(i,des_move))
						{
							move_pseudo_pawn(i,des_move);
							score = maxi( depth - 1, false );
							if(score>max)
								{
									max=score;

								}
							copy_board(tboard, history_board[depth]);
						}
					}

				}else
				{
					for ( n_move = 0; n_move < number_of_move[lpiece]; n_move++ )
					{
						des_move=0;
						do
						{
							des_move=des_move+movement[lpiece][n_move];
							if(!check_move(i,des_move))
							{
								break;
							}

							if(check_target_color(i,des_move))	//if capture
							{
								move_pseudo_piece(i,des_move);
								score = maxi( depth - 1, false );
								if(score>max)
								{
									max=score;

								}
								copy_board(tboard, history_board[depth]);	//return board
								break;										//stop
							}

							move_pseudo_piece(i,des_move);
							score = maxi( depth - 1, false );
							if(score>max)
								{
									max=score;

								}
							copy_board(tboard, history_board[depth]);		//return board

						}while(slide[lpiece]);

					}
					//castling move white
					/*
					if((lpiece==KING)&&(bWhiteAllowCastlingL)&&(bWhiteAllowCastlingR))
					{
						for()
						{

						}
					}
					*/
				}
			}
		}

		printf("\nstart pos: %d destination: %d\n ",start_pos_move,des_pos_move);
        printf("\nvalue %d \n ",max);
		return max;
	}else
	{
	    printf("depth %d\n ",depth);
        max = V_MAX;

		copy_board( history_board[depth], tboard);		//record history (of three kingdom)
		for ( i=0; i<64; i++ ) {						//scan board
			lpiece = tboard[i] & MASK_PIECE;
			lcolor = tboard[i] & MASK_COLOR;
			if(( lpiece != EMPTY) && ( lcolor == COLOR_BLACK))
			{
				if(lpiece==PAWN)
				{
					for ( n_move = 0; n_move < 3; n_move++ )
					{
						des_move = -movement[lpiece][n_move];	/* des_move = -movement[lpiece][n_move]; if its BLACK */
						if(check_pawn_move(i,des_move))
						{
							move_pseudo_pawn(i,des_move);
							score = maxi( depth - 1, true );
							if(score<max)
								{
									max=score;
									lbestpos=i;
									lbestmove=des_move;
								}
							copy_board(tboard, history_board[depth]);
						}
					}

				}else
				{
					for ( n_move = 0; n_move < number_of_move[lpiece]; n_move++ )
					{
						des_move=0;
						do
						{
							des_move=des_move+movement[lpiece][n_move];
							if(!check_move(i,des_move))
							{
								break;
							}

							if(check_target_color(i,des_move))	//if capture
							{
								move_pseudo_piece(i,des_move);
								score = maxi( depth - 1, true );
								if(score<max)
								{
									max=score;
									lbestpos=i;
									lbestmove=des_move;
								}
								copy_board(tboard, history_board[depth]);	//return board
								break;										//stop
							}

							move_pseudo_piece(i,des_move);
							score = maxi( depth - 1, true );
							if(score<max)
								{
									max=score;
									lbestpos=i;
									lbestmove=des_move;
								}
							copy_board(tboard, history_board[depth]);		//return board


						}while(slide[lpiece]);

					}
					//castling move white
					/*
					if((lpiece==KING)&&(bWhiteAllowCastlingL)&&(bWhiteAllowCastlingR))
					{
						for()
						{

						}
					}
					*/
				}
			}
		}

        start_pos_move=lbestpos;
        des_pos_move=lbestmove;
		printf("\nstart pos: %d destination: %d\n ",start_pos_move,des_pos_move);
        printf("\nvalue %d \n ",max);
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
		lcolor_target = tboard[destination] & MASK_COLOR;
		lpiece_target = tboard[destination] & MASK_PIECE;
		if((lpiece_target) == EMPTY)	//2. check if empty
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

//check if its empty and have different color, return true if they have different color
bool check_target_color(int piece_pos, int des_move)
{
	int lpiece=tboard[piece_pos] & MASK_PIECE;
	int lcolor=tboard[piece_pos] & MASK_COLOR;
	int lpiece_target;
	int lcolor_target;

	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index

	if(destination==-1)
	{
		//out of bound,return -1
		return false;
	}else
	{
		lpiece_target=tboard[destination] & MASK_PIECE;
		lcolor_target=tboard[destination] & MASK_COLOR;
		if(lpiece_target==EMPTY)
		{
			return false;
		}else
		{
			if(lcolor==lcolor_target)
			{
				return false;
			}else
			{
				return true;
			}
		}
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

int move_player_piece(int piece_pos, int des_move)
{
    board[des_move]=board[piece_pos];
	board[piece_pos]=EMPTY;
    return 0;
}

bool check_move_player(int player_start, int player_des)
{
	int i,j,k;
	int mail_start, mail_des, mail_diff;
	bool m_match=false;
	int m_obstacle_count=0;		//match count
	int lpiece=board[player_start] & MASK_PIECE;
	int lcolor=board[player_start] & MASK_COLOR;
	int lpiece_target=board[player_des] & MASK_PIECE;
	int lcolor_target=board[player_des] & MASK_COLOR;
	int	lpiece_check;

	//check piece
	if(lpiece==EMPTY)
		{ return false;}
	if(lcolor!=COLOR_WHITE)
		{ return false;}

	//check boundary
	if((player_start<0) || (player_start>63))
		{ return false;}
	if((player_des<0) || (player_des>63))
		{ return false;}
	//check destination
	if((lpiece_target!=EMPTY)&&(lcolor_target==COLOR_WHITE))
		{ return false;}

	//check movement rule
	mail_start = mailbox64[player_start];
	mail_des = mailbox64[player_des];
	mail_diff = mail_des - mail_start;

    printf("\n%d %d %d\n",mail_start,mail_des,mail_diff);
	if(lpiece==PAWN)
	{
        if((mail_diff==9)||(mail_diff==11))
        {
            if(lpiece_target==EMPTY)
            { return false;}
            else
            {
                if(lcolor_target==COLOR_WHITE)
                { return false;}
                else
                {
                     return true;
                }
            }
        }
        if(mail_diff==10)
        {
            if(lpiece_target==EMPTY)
            {
                return true;
            }
        }
        return false;
	}

	for(i=0;i<number_of_move[lpiece];i++)
	{

		if((mail_diff%movement[lpiece][i])==0)
		{
			m_match=true;
			break;
		}
	}

	if(m_match==false)
		{ return false;}
	else
	{
		return true;
		/*
		if(slide[lpiece])
		{
			for(j=mail_start; j<mail_des; j=j+movement[lpiece][i])
			{
				if(j!=mail_start
			}
		}
		*/
	}
}

//return destination array
int move_pseudo_piece(int piece_pos, int des_move)
{
	int tpiece=tboard[piece_pos];
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index


    printf("\npos+move %d\ndestin %d\n ",mb_pos+des_move,destination);
	if(destination==-1)
	{
		//out of bound
		return -1;
	}else{
		tboard[destination]=tpiece;
		tboard[piece_pos]=EMPTY;

        display(tboard);
		printf("\nmove %d to %d\n ",piece_pos,des_move);
        printboard(tboard,fftest);

		return destination;
	}
}

//special move for pawn (including queen promotion)
int move_pseudo_pawn(int piece_pos, int des_move)
{
	int tpiece=tboard[piece_pos];
	int lcolor=board[piece_pos] & MASK_COLOR;
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index

	if(destination==-1)
	{
		//out of bound
		return -1;
	}else{
		tboard[piece_pos]=EMPTY;
		tboard[destination]=tpiece;
		if(lcolor==COLOR_BLACK)
		{
			if(destination<8)
			{
				tboard[destination]=QUEEN+COLOR_BLACK;//color white=0, so queen+color white=5+0 :P
			}
		}
		if(lcolor==COLOR_WHITE)
		{
			if(destination>55)
			{
				tboard[destination]=QUEEN+COLOR_WHITE;//color white=0, so queen+color white=5+0 :P
			}
		}
		display(tboard);
		printf("\nmove %d to %d\n ",piece_pos,des_move);
        printboard(tboard,fftest);
		return destination;
	}
}


//return destination array
int move_piece(int piece_pos, int des_move)
{
	int tpiece=board[piece_pos];
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index

	if(destination==-1)
	{
		//out of bound
		return -1;
	}else{
		board[destination]=tpiece;
		board[piece_pos]=EMPTY;
		return destination;
	}
}

//special move for pawn (including queen promotion)
int move_pawn(int piece_pos, int des_move)
{
	int tpiece=board[piece_pos];
	int lcolor=board[piece_pos] & MASK_COLOR;
	int mb_pos=mailbox64[piece_pos];				//convert board into mailbox
	int destination = mailbox[mb_pos+des_move];		//if destination = -1 its out of bound, otherwise board array index

	if(destination==-1)
	{
		//out of bound
		return -1;
	}else{
	    board[destination]=tpiece;
		board[piece_pos]=EMPTY;
		if(lcolor==COLOR_BLACK)
		{
			if(destination<8)
			{
				board[destination]=QUEEN+COLOR_BLACK;//color white=0, so queen+color white=5+0 :P
			}
		}
		if(lcolor==COLOR_WHITE)
		{
			if(destination>55)
			{
				board[destination]=QUEEN+COLOR_WHITE;//color white=0, so queen+color white=5+0 :P
			}
		}
		return destination;
	}
}

int find_max(int value_a, int value_b)
{
	if(value_a>value_b)
	{
		return value_a;
	}else
	{
		return value_b;
	}
}

int find_min(int value_a, int value_b)
{
	if(value_a<=value_b)
	{
		return value_a;
	}else
	{
		return value_b;
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
void display(int bd[64])
{
	int i;
	printf(" ");
	for(i=0;i<64;i++)
	{
	    if(bd[i]==0)
	    {
	        printf(" .");
	    }else
	    {
	        printf(" %c",bd[i]+'a');
	    }
		if ((i + 1) % 8 == 0 && i != 63)
			printf("\n ");
	}
	printf("\n");
}

void printboard(int bd[64],FILE* tfile)
{
	int i;
	fprintf(tfile," ");
	for(i=0;i<64;i++)
	{
	    if(bd[i]==0)
	    {
	        fprintf(tfile," .");
	    }else
	    {
	        fprintf(tfile," %c",bd[i]+'a');
	    }
		if ((i + 1) % 8 == 0 && i != 63)
			fprintf(tfile,"\n ");
	}
	fprintf(tfile,"\n");
	fprintf(tfile,"temporary value %d\n\n",evaluate());
}
//its just dummy main function
int not_main()
{
    int c1,c2;
    bool gameover=false;
    fftest=fopen("../filetest.txt","w");
    if(fftest==NULL) return -1;

	copy_board(board,init_board);
	while(!gameover)
	{
		if(ply%2==0)//white one
		{
			//input
			//check
			//if(check)
			//-execute
			//-ply++
			display(board);
			printf("\n input your move ");
			scanf("%d",&c1);
			if(c1>=0)
            {
                scanf("%d",&c2);
            }else
            {
                gameover=true;
            }

			if(check_move_player(c1,c2))
            {
                move_player_piece(c1,c2);
                ply++;
            }else
            {
                printf("\nOops!! illegal move, try again\n\n");
            }

		}else
		{
		    if(ply<3)
            {
                move_piece(52,-10);
            }else
            {
                copy_board(tboard,board);

                fprintf(fftest,"==================================================\n");
                fprintf(fftest,"this is start at ply %d\n",ply);
                printboard(board,fftest);
                fprintf(fftest,"--------------------------------------------------\n");

                maxi(2,false);
                move_piece(start_pos_move, des_pos_move);

                fprintf(fftest,"..................................................\n");
                fprintf(fftest,"end of ply %d\nthe AI decision is:\n",ply);
                printboard(board,fftest);
                fprintf("\nvalue %d\n",evaluatebd(board));

                display(board);
                printf("\nvalue %d\n",evaluate());
            }
			ply++;
		}
	}
	fclose(fftest);
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
int main()
{
    not_main();
    printf("Hello world!\n");
    return 0;
}
