#include "shape.h"
#include "sprite.h"
#include <string>
#include <vector>
using namespace std;

//--------------------------------------------------------//
//描画機能内蔵版のスプライト(Spriteを継承)
//--------------------------------------------------------//
Shape::Shape(){
	fillc = ' ';
}
void Shape::rect(int x, int y, int w, int h){
	Sprite* s = new Sprite;
	s->setSize(w, h);
	s->setX(x);
	s->setY(y);
	for(int ty = 0; ty < h; ty ++){
		for(int tx = 0; tx < w; tx ++){
			s->dot(tx, ty, fillc);
		}
	}
	addChild(s);
}
void Shape::clear(){
	int l = child.size();
	for(int i = 0; i < l; i ++){
		delete child[i];
	}
	child.clear();
}
