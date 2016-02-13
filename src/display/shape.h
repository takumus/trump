#ifndef JP_TACM_SPRITE_PLUS
#define JP_TACM_SPRITE_PLUS

#include "sprite.h"
#include <string>

using namespace std;

//--------------------------------------------------------//
//描画機能内蔵版のスプライト(Spriteを継承)
//--------------------------------------------------------//
class Shape : public Sprite {
public:
	Shape();
	void fill(char c){fillc = c;}//塗りの文字をセット
	void noFill(){fillc = ' ';}//塗りの文字をなしにセット
	void clear();
	void rect(int x, int y, int w, int h);
	void line(int x1, int y1, int x2, int y2);
private:
	char fillc;
};

#endif
