#if not define _MAP_HPP_
#define _MAP_HPP_

#define WIDTH 10
#define HEIGHT 20

#include "Block.hpp"

class Map{
    public:
	Map(){
	    for(int i=0;i<WIDTH+2;i++){
		for(int j=0;j<HEIGHT+2;j++){
		    if(i==0 || i==WIDTH+1){
			map_[i][j]=1;
			continue;
		    }
		    if(j==0 || j==HEIGHT+1){
			map_[i][j]=1;
			continue;
		    }
		    map_[i][j]=0;
		}
	    }
	}
	Map(Map &other){}
	~Map(){}

	bool Update(Block &item);
	bool Legality(Block &item);

    public:
	int map_[WIDTH+2][HEIGHT+2];
};

//update the map
Map::Update(Block &item){
    return true;
}

//judge legality of current statu
Map::Legality(Block &item){
    return true;
}

#endif
