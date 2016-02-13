#include "sprite.h"
#include <string>
using namespace std;
//--------------------------------------------------------//
//描画オブジェクトの原型
//Spriteの名前の由来はActionScriptの真似です。
//--------------------------------------------------------//
Sprite::Sprite() {
	data = "";
	x = y = 0;
	width = height = 0;
	transparent = false;
	//オブジェクト固有のidを保存する。
	static int _id = 0;
	_id ++;
	id = _id;
	scale = 1;
}
void Sprite::addChild(Sprite* s) {
	//子オブジェクトを追加する。
	//これもActionScriptの真似である。
	child.push_back(s);
}
void Sprite::removeChild(Sprite* s) {
	//childは子オブジェクトである。
	//子オブジェクトには重ね順があり、追加した順番に管理される必要がある。
	//よって親にaddChildした順番になる必要があり、
	//ソートしてはいけない。そのため、idが一致するものを削除する方法は
	//この方法しか浮かばなかった。
	//そもそも、この関数は頻繁に呼ばれることはないので、あまり使わない。
	//単にオブジェクト保非表示にする場合は、visibleをfalseにすれば良い。
	int l = child.size();
	for(int i = 0; i < l; i ++){
		if(child[i]->id == s->id){
			child.erase(child.begin() + i);
			return;
		}
	}
}
void Sprite::setSize(int _width, int _height){
	width = _width;
	height = _height;
	data = "";
	for(int i = 0; i < width * height; i ++){
		data += ' ';
	}
}
//ドットを打つ
void Sprite::dot(int x, int y, char c){
	//c = c==' '?'0':'1';
	if(x >= width || y >= height || x < 0 || y < 0){
		return;
	}
	data[width * y + x] = c;
}
