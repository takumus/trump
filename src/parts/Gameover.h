/*
 * Gameover.h
 *
 *  Created on: Jan 11, 2015
 *      Author: tacm
 */

#ifndef PARTS_GAMEOVER_H_
#define PARTS_GAMEOVER_H_

#include "..\display\display.h"
#include "..\display\sprite.h"
#include "..\display\texture.h"
#include "..\display\shape.h"
#include<string>
using namespace std;
class Gameover {
public:
	void setup(Display* display, TextureData wonTextureData, TextureData loseTextureData) {
		// TODO Auto-generated constructor stub
		_display = display;
		_wonTexture.setTextureData(wonTextureData, true);
		_loseTexture.setTextureData(loseTextureData, true);
		_wonTexture.setX(display->getWidth());//初期ｘ位置を画面の右端へ
		_loseTexture.setX(display->getWidth());//初期ｘ位置を画面の右端へ
		_wonTexture.setY((display->getHeight() - _wonTexture.getHeight())/2);//初期ｙ位置を画面の中央へ
		_loseTexture.setY((display->getHeight() - _loseTexture.getHeight())/2);//初期ｙ位置を画面の中央へ

	}
	bool render() {
		static Shape* s = new Shape;
		static double w = _display->getWidth();
		static double h = _display->getHeight();
		s->clear();
		s->fill('@');
		s->rect(0,0,w,h);
		s->setX((_display->getWidth() - w)/2);
		s->setY((_display->getHeight() - h)/2);
		w-=0.2;
		//h-=0.06;
		_display->mask(s);

		if(_result == "win"){
			_wonTexture.setX(_wonTexture.getX() - 1);
			if(_wonTexture.getX() + _wonTexture.getWidth() < 0){
				_wonTexture.setX(_display->getWidth());
				endMessageCount++;
			}
			_display->draw(&_wonTexture);
		}else{
			_loseTexture.setX(_loseTexture.getX() - 1);
			if(_loseTexture.getX() + _loseTexture.getWidth() < 0){
				_loseTexture.setX(_display->getWidth());
				endMessageCount++;
			}
			_display->draw(&_loseTexture);
		}
		if(endMessageCount > 1){
			return true;
		}
		return false;
	}
	void setResult(string result){_result = result;}
private:
	Display* _display;
	Texture _wonTexture, _loseTexture;
	//終了メッセージのループ回数
	int endMessageCount = 0;
	string _result = "";
};

#endif /* PARTS_GAMEOVER_H_ */
