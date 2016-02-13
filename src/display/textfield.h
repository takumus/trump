#ifndef JP_TACM_TEXTFIELD
#define JP_TACM_TEXTFIELD
#include <string>
#include "sprite.h"

//--------------------------------------------------------//
//テキスト表示クラス(Spriteを継承)
//--------------------------------------------------------//
class TextField : public Sprite {
public:
	TextField();
	void setText(string _text);
private:
	string text;
};

#endif
