#ifndef _JP_TACM_TWEEN
#define _JP_TACM_TWEEN

#include <math.h>
#include "texture.h"
//--------------------------------------------------------//
//スプライトをイージング効果によって移動する
//--------------------------------------------------------//
class SpriteTween {
public:
	//スプライトを徐々に指定した座標へ移動させる
	static bool moveTo(Sprite *s, int targetX, int targetY, double targetScale, double speed);
	static bool moveTo(Sprite *s, int targetX, int targetY);
};

#endif
