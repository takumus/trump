#ifndef _JP_TACM_SPRITE
#define _JP_TACM_SPRITE

#include <string>
#include <vector>
using namespace std;

//--------------------------------------------------------//
//描画オブジェクトの原型
//Spriteの名前の由来はActionScriptの真似です。
//--------------------------------------------------------//
class Sprite {
public:
	Sprite();
	void addChild(Sprite* s);
	void removeChild(Sprite* s);
	void setSize(int _width, int _height);
	void dot(int x, int y, char c);
	//ゲッターセッター
	string getData(){return data;}
	void setData(string _data){data = _data;}
	double getX(){return x;}
	double getY(){return y;}
	void setX(double _x){x = _x;}
	void setY(double _y){y = _y;}
	int getWidth(){return width;}
	int getHeight(){return height;}
	int getScaledWidth(){return scale * (double)width;}
	int getScaledHeight(){return scale * (double)height;}
	bool getTransparent(){return transparent;}
	void setTransparent(bool _transparent){transparent = _transparent;}
	double getScale(){return scale>1?1:scale<0?0:scale;}
	void setScale(double value){scale = value;}
	vector<Sprite*> getChild(){return child;}
protected:
	string data;
	double x, y;
	double scale;
	int width, height;
	bool transparent;
	vector<Sprite*> child;
	int id;
};

#endif
