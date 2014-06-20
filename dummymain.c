int main()
{
	
}

int evaluate(int* board)
{
	int i,total=0,lpiece;
	for(i=0;i<64;i++)
	{
		lpiece=color[i];
		if(lpiece)
		{
			total+=board[i];
		}
	}
	return total;
}


int maxi( int depth, bool maximize ) {
	int max;
    if ( depth == 0 ) return evaluate();
	if ( maximize )
	{
        max = -50000;
		
		history_board[depth]=board;		
		for ( all pawn) {
			for ( all moves) {	
				move(pawn,moves);
				score = maxi( depth - 1, false );        
				if( score > max ) max = score;
			}
		}
		board=history_board[depth];
		return max;
	}else
	{
		max = 50000;
		history_board[depth]=board;	
		
		for ( all pawn) {
			for( all moves) {
				move(pawn,moves);
				score = maxi( depth - 1, true );
				if( score < max ) max = score;
			}
		}
		
		board=history_board[depth];
		return max;
	}
}
 


/*
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