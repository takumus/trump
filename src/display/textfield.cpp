#include "textfield.h"
#include <string>
//--------------------------------------------------------//
//テキスト表示クラス
//--------------------------------------------------------//
TextField::TextField(){
	width = 0;
	height = 1;
}
//テキストをセット
void TextField::setText(string _text){
	//テキストを保存
	text = _text;
	//テクスチャに変換
	data = _text;
	width = text.length();
	height = 1;
}
