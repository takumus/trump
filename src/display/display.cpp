#include "sprite.h"
#include "display.h"
#include <string>
#include <iostream>

using namespace std;
//幅、高さ、背景色（背景を塗りつぶす記号）を設定し初期化
void Display::setup(int width, int height, char background){
	_width = width + 1;
	_height = height;
	_background = background;
	_data = "";
	_maskMode = false;
	_maskData = "";
	//init
	for(int i = 0; i < _width * _height; i ++){
		_data += '@';
		_maskData += ' ';
	}
	clear();
}
//画面クリア
void Display::clear(){
	_masked = false;
	for(int y = 0; y < _height; y++){
		for(int x = 0; x < _width; x++){

			_data[_width * y + x] = _background;

			_maskData[_width * y + x] = ' ';
		}
	}
}
//描画
void Display::render(){
	//if(_masked) _mask();
	for(int y = 0; y < _height; y++){
		_data[_width * y + _width - 1] = '\n';
	}
	cout << "\033c";
	cout << _data;
	//printf("%s",_data);
}
//リピート画像
void Display::repeatBackground(Sprite* s){
	for(int x = 0; x < _width; x += s->getWidth()){
		for(int y = 0; y < _height; y += s->getHeight()){
			s->setX(x);
			s->setY(y);
			draw(s);
		}
	}
}
//スプライトを画面に追加
void Display::draw(Sprite *s){
	_draw(s, 0, 0);
}
void Display::mask(Sprite *s){
	_masked = true;
	_maskMode = true;
	_draw(s, 0, 0);
	_mask();
	_maskMode = false;
}
void Display::_draw(Sprite* s, double px, double py){
	//画面にドット軍をセット
	setDots(s->getX() + px, s->getY() + py, s->getData(), s->getWidth(), s->getHeight(), s->getTransparent(), s->getScale());
	int childSize = s->getChild().size();
	for(int i = 0; i < childSize; i ++){
		Sprite* cs = s->getChild()[i];
		//親であるsの座標を基準点とし、子の座標を決定
		_draw(cs, s->getX(), s->getY());
	}
}
//ドットを打つ
void Display::setDot(int x, int y, char c){
	//c = c==' '?'0':'1';
	if(x >= _width || y >= _height || x < 0 || y < 0){
		return;
	}
	if(_maskMode && c != ' '){
		_maskData[_width * y + x] = '@';
		return;
	}
	_data[_width * y + x] = c;
}
//ドット群を打つ
void Display::setDots(double x, double y, string data, int w, int h, bool transparent, double scale){
	int _x = (int)(x + 0.5);
	int _y = (int)(y + 0.5);
	//スケールの適用
	for(int ty = 0; ty < h; ty++){
		for(int tx = 0; tx < w; tx++){
			char c = data[w * ty + tx];
			//背景透明モードなら空白を無視する
			if(transparent && c == ' '){
				continue;
			}
			//ドットを打つ
			setDot(_x + scale * (double)tx, _y + scale * (double)ty, c);
		}
	}
}
//mask
void Display::_mask(){
	int size = _maskData.size();
	for(int i = 0; i < size; i ++){
		if(_maskData[i] != '@'){
			_data[i] = ' ';
		}
	}
}
//幅のゲッター
int Display::getWidth(){
	return _width - 1;
}
//高さのゲッター
int Display::getHeight(){
	return _height;
}
