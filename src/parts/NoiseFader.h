/*
 * Gameover.h
 *
 *  Created on: Jan 11, 2015
 *      Author: tacm
 */

#ifndef PARTS_NOISEFADER_H_
#define PARTS_NOISEFADER_H_

#include "..\display\display.h"
#include "..\display\sprite.h"
#include "..\display\texture.h"
#include <string>
#include <algorithm>
#include <vector>
using namespace std;
class NoiseFader {
public:
	void setup(Display* display){
		_display = display;
		_noise = new Sprite;
		_noise->setSize(_display->getWidth(), _display->getHeight());
		_dataLength = _display->getWidth() * _display->getHeight();
		for(int i = 0; i < _dataLength; i++){
			_data += ' ';
			_dataId.push_back(i);
		}
		random_shuffle(_dataId.begin(), _dataId.end());
	}
	void fadeIn(){
		_mode = "in";
		_dataIdCount = 0;
		for(int i = 0; i < _dataLength; i++){
			_data[i] = ' ';
		}
	}
	void fadeOut(){
		_mode = "out";
		_dataIdCount = 0;
		for(int i = 0; i < _dataLength; i++){
			_data[i] = '#';
		}
	}
	bool render(){
		char c = ' ';
		if(_mode == "in"){
			c = '#';
		}
		for(int i = 0; i < _speed; i ++){
			if(_dataLength <= _dataIdCount){
				return true;
			}
			_data[_dataId[_dataIdCount]] = c;
			_dataIdCount ++;
		}
		_noise->setData(_data);
		_display->mask(_noise);
		return false;
	}
private:
	const int _speed = 32;
	int _dataIdCount;
	int _dataLength;
	string _data;
	vector<int> _dataId;
	string _mode;
	Display* _display;
	Sprite* _noise;
};

#endif /* ANIMATION_GAMEOVER_H_ */
