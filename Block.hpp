#if not define _BLOCK_HPP_
#define _BLOCK_HPP_

//define each item's each statu

class Block{
	public:
		Block(int type){
			type_=type;
		}
		~Block(){}
	public:
		int coordinate_[5][5]={0};
		int orientation_=0;
		int type_;
};

#endif
