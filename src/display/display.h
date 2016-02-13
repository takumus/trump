#ifndef _JP_TACM_DISPLAY
#define _JP_TACM_DISPLAY

#include "sprite.h"
#include <string>
using namespace std;

class Display{
public:
	void setup(int width, int height, char background);
	void clear();
	void render();
	void draw(Sprite* s);
	void mask(Sprite* s);
	void repeatBackground(Sprite* s);
	int getWidth();
	int getHeight();

private:
	string _data;
	string _maskData;
	int _width, _height;
	char _background;
	bool _maskMode = false;
	bool _masked = false;
	void _draw(Sprite* s, double px, double py);
	void _mask();
	void setDot(int x, int y, char c);
	void setDots(double x, double y, string data, int w, int h, bool transparent, double scale);
};

#endif
