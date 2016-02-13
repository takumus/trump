#include "texture.h"
#include "sprite.h"
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <time.h>
//#include "textfield.h"
using namespace std;
///--------------------------------------------------------//
//テクスチャデータクラス
//--------------------------------------------------------//
void TextureData::setup(string _data, int _width, int _height, bool _transparent = false){
	data = _data;
	width = _width;
	height = _height;
	transparent = _transparent;
}
//--------------------------------------------------------//
//テクスチャクラス(Spriteを継承)
//--------------------------------------------------------//
void Texture::setTextureData(TextureData _texture, bool _transparent){
	data = _texture.getData();
	texture = _texture;
	transparent = _transparent;
	width = texture.getWidth();
	height = texture.getHeight();
	/*TextField* t = new TextField;
	t->setText("test");
	t->x = 9;
	t->y = 7;
	addChild(t);*/
}
//--------------------------------------------------------//
//テクスチャローダー
//--------------------------------------------------------//
bool TextureDataLoader::load(const char* fileName){
	if((fp = fopen(fileName, "r,ccs=UTF-8")) == NULL) {
		return false;
	}
	list.clear();
	parse();
	fclose(fp);
	return true;
}
TextureData TextureDataLoader::get(string label){
	return list.at(label);
}
void TextureDataLoader::convertToTexture(int width, int height, string name, vector<string> lines){
	printf("-------------------------\n");
	printf("name : %s\n", name.c_str());
	printf("width,height : %d,%d\n", width, height);
	int length = lines.size() - 1;
	string data = "";
	for(int i = 0; i < length; i ++){
		while(lines[i].size() < width){
			lines[i] += " ";
		}
		data += lines[i];
	}
	printf("data : ");
	for(int i = 0; i < width*height; i ++){
		printf("%c",data.c_str()[i]);
	}
	printf("\n");

	list[name].setup(data, width, height, false);
	double interval = 0.05 * CLOCKS_PER_SEC;
	clock_t cc = clock();
	while(clock() - cc < interval);
}
//読み込んだデータのパース
void TextureDataLoader::parse() {
	char c;
	int id, twidth, width, height;
	string name="";
	vector<string> lines;
	string phase = "";
	while(1) {

		c = fgetwc(fp);
		//-------------------------------------------------------//
		//　新規テクスチャの準備
		//-------------------------------------------------------//
		if(c == '@') {
			//次のテクスチャ開始記号＠を発見したらテクスチャ登録
			if(phase == "data"){
				convertToTexture(width, height, name, lines);
			}
			//次のデータ登録に備えて初期化
			id = 0;
			name = "";
			lines.clear();
			lines.push_back("");
			height = twidth = width = 0;
			//フェーズを「テクスチャ名のパース」モードに
			phase = "name";
			continue;
		}
		//-------------------------------------------------------//
		//　フェーズ　：　テクスチャ名のパース
		//-------------------------------------------------------//
		if(phase == "name"){
			if(c == '\n'){
				//改行コードを検出したらパースを終了し、
				//フェーズを「テクスチャデータのパース」モードに
				phase = "data";
				continue;
			}
			name += c;
			continue;
		}
		//-------------------------------------------------------//
		//　フェーズ　：　テクスチャデータのパース
		//-------------------------------------------------------//
		if(phase == "data"){
			if(c == '\n' || c < 0) {
				height++;
				if(width < twidth){
					width = twidth;
				}
				twidth = 0;
				lines.push_back("");
				id++;
				//c < 0つまりEOF？なら最後のデータを登録し、終了。
				if(c < 0){
					//テクスチャ登録
					convertToTexture(width, height, name, lines);
					break;
				}
				continue;
			}
			twidth++;
			lines[id] += c;
			continue;
		}
	}
}
