/*
 * www.botzone.org

 * rules:
 	there will be show two windows,one is for you(maybe support your program latter),
	another is prepare to my program.I plan to realize it with neural netwok.

	unlike traditional Tetris,only the first item is randomized,the subsequent items is
	appointed by the other party.
	
	the map is 10*20,each item is consist of four squares.when you move,you can't cross
	the existing items.

	every item borns at [4,0]

 * struct:
	there are 7 kinds of item:
		* *       *       *             *     * *     * *     
		* ^     * ^ *     * ^ *     * ^ *   * ^         ^ *     * ^ * *
	
	every item has four orientations and one center(mark with the char '^').
	To keep consistency,an item is represented by 5*5 matrix,the center coordinate is [1,1].

	need a map class
	an item class
	an A* algithm to judge if it can be reached.
	need to judge edge

	the max d-value between two items need to be less than 2.

	item will fall one lattice each second.
	player can contral item to trun left or right,and you can mv it left and right with keyboard '<-' and '->'
	program need to give the final center coordinate and orientation,if the statu is illegal,the program is loser.
*/

#include <iostream>
#include <Astar.h>



int main(){
	//init the map
	Map player,program;
	while (true){
		//gain players input and the program's input
	}

	return 0;
}
