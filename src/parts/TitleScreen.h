/*
 * Gameover.h
 *
 *  Created on: Jan 11, 2015
 *      Author: tacm
 */

#ifndef PARTS_TITLESCREEN_H_
#define PARTS_TITLESCREEN_H_

#include "..\display\display.h"
#include "..\display\sprite.h"
#include "..\display\texture.h"
#include "..\display\tween.h"
#include "..\display\textfield.h"

#include <string>
#include <algorithm>
#include <vector>
using namespace std;
class TitleScreen {
public:
	void setup(Display* display, TextureData titleTextureData){
		_display = display;
		_titleTexture = new Texture;
		_titleTexture->setTextureData(titleTextureData, true);
		_titleTexture->setX(display->getWidth()/2);
		_titleTexture->setY(display->getHeight());
		_titleTexture->setScale(0);
		_titleX = (_display->getWidth() - _titleTexture->getWidth())/2;
		_titleY = 2;
		_playerText = new TextField;
		_nameText = new TextField;
		_cursor = new TextField;
		_cursor->setText("-> ");
		_nameText->setText("BY TAKUMUS");
		_nameText->setX(_display->getWidth());
		_nameText->setY(_titleY + _titleTexture->getHeight() + 1);
		_phase = "title";
		_playerNumList.push_back(2);
		_playerNumList.push_back(3);
		_playerNumList.push_back(4);
		_playerNum = _playerNumList[0];
	}
	bool render(){
		if      (_phase == "title"){
			if(SpriteTween::moveTo(_titleTexture, _titleX, _titleY, 1, 0.08)){
				_phase = "name";
			}
			update();
		}else if(_phase == "name"){
			if(SpriteTween::moveTo(_nameText, _titleX + _titleTexture->getWidth() - _nameText->getWidth(), _titleY + _titleTexture->getHeight() + 1, 1, 0.1)){
				_phase = "selectplayer";
			}
			update();
		}else if(_phase == "selectplayer"){
			int ty = 16;
			for(int i = 0; i < _playerNumList.size(); i++){
				_playerText->setText(_to_string(_playerNumList[i]) + " player");
				_playerText->setX((_display->getWidth() - _playerText->getWidth())/2);
				_playerText->setY(ty);
				ty+= 2;
				_display->draw(_playerText);
			}
			_cursor->setX(_playerText->getX() - _cursor->getWidth());
			ty = 16 + _playerNumId * 2;
			_cursor->setY(ty);

			//äÁÇÃï`âÊ
			if(!renderFace(_playerText->getX() + _playerText->getWidth(), ty)){
				update();
				return false;
			}


			update();
			char c = getchar();
			if(c == 'a'){
				return true;
			}else if(c == '\n'){
				_playerNumId++;
				if(_playerNumId >= _playerNumList.size()){
					_playerNumId = 0;
				}
				resetFace();
				_playerNum = _playerNumList[_playerNumId];
			}else if(c == 'c'){
				printf("\nplayer : ");
				scanf("%d", &_playerNum);
				getchar();//ÉGÉìÉ^Å[Ç™ç¨ì¸Ç∑ÇÈÇÃÇñhé~
				return true;
			}
		}
		return false;
	}
	bool renderFace(int x, int y){
		updateFace(_playerNum);
		int status = 1;
		for(int i = 0; i < _playerNum; i ++){
			_face[i]->setX(i*5 + x);
			_face[i]->setY(y);//, 1, 0.2);
			_display->draw(_face[i]);
		}
		return 0 < status;
	}
	void resetFace(){
		for(int i = 0; i < _face.size(); i ++){
			_face[i]->setX(_display->getWidth());
			_face[i]->setY(_display->getHeight());
		}
	}
	void updateFace(int n){
		while(_face.size() <= n){
			TextField* face = new TextField;
			face->setText("('o')");
			_face.push_back(face);
		}
	}
	int getPlayerNum(){
		return _playerNum;
	}
	void update(){
		_display->draw(_cursor);
		_display->draw(_titleTexture);
		_display->draw(_nameText);
		_display->render();
	}
private:
	Display* _display;
	Texture* _titleTexture;
	TextField* _nameText;
	TextField* _playerText;
	TextField* _cursor;
	string _phase = "";
	double _titleX;
	double _titleY;
	vector<int> _playerNumList;
	vector<TextField*> _face;
	int _playerNumId = 0;
	int _playerNum;
	string _to_string(int number)
	{
		stringstream s;
		s << number;
		return s.str();
	}
};

#endif /* ANIMATION_GAMEOVER_H_ */
