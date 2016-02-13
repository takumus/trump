//============================================================================
// Name        : Babanuki.cpp
// Author      : Tacm
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "display\shape.h"
#include "display\texture.h"
#include "display\sprite.h"
#include "display\shape.h"
#include "display\display.h"
#include "display\tween.h"
#include "display\textfield.h"

#include "parts/Gameover.h"
#include "parts/NoiseFader.h"
#include "parts/TitleScreen.h"

using namespace std;

//動作フレームレート
int FRAME_RATE = 70;

//プレイヤー人数
int PLAYER = 2;

//テクスチャファイル名
#define TEXTURE_PATH "assets/texture.txt"

//トランプの幅
#define TRUMP_WIDTH 9
//トランプの高さ
#define TRUMP_SPACE 3


string _to_string(int);

//---------------------------------------------------------------//
//トランプ
//---------------------------------------------------------------//
struct Trump {
	int number;
	int mark;
	bool _visible;
	Texture* texture = new Texture;
	//表のテクスチャ
	TextureData frontTextureData;
	//裏のテクスチャ
	TextureData backTextureData;
	void setup(int _number, int _mark, TextureDataLoader* tl){
		number = _number;
		mark = _mark;
		//カード番号からテクスチャを読み込む
		frontTextureData = tl->get("trump-" + _to_string(_number));
		//裏テクスチャは同一
		backTextureData = tl->get("trump-back");
		//表にする
		setVisible(true);
	}
	//表裏の設定用（true=表）
	void setVisible(bool value) {
		if(_visible == value) return;
		_visible = value;
		if(_visible) {
			//表なら表のテクスチャを再設定
			texture->setTextureData(frontTextureData);
		}else{
			//裏なら裏のテクスチャを再設定
			texture->setTextureData(backTextureData);
		}
	}
};
//--------------------------------------------------------//
//プレイヤーの顔
//--------------------------------------------------------//
struct Face {
	map<string, string> faceTextureData;
	TextField *texture = new TextField;
	void add(string key, string text) {
		faceTextureData[key] = text;
		set(key);
	}
	void set(string key) {
		texture->setText(faceTextureData[key]);
	}
};
//--------------------------------------------------------//
//プレイヤー（AIと人を含む）
//主に手札やプレイヤーの名前、顔を管理する
//手札に変更を加えるメソッドもある。（追加、削除）
//--------------------------------------------------------//
struct People {
	string name;
	bool select = false;
	int selectedId = 0;
	vector<Trump*> hand;
	Face face;
	void addTrump(Trump* trump) {
		hand.push_back(trump);
	}
	Trump* removeTrump(int id) {
		Trump* t = hand[id];
		hand.erase(hand.begin() + id);
		return t;
	}
};
//この関数群は下で解説している
char getcharf(void);
void initPlayer(int);
void initTrump();
bool renderScreen();
bool dealTrump();
bool firstDiscardTrump();
bool discardTrump(int);
bool selectTrumpMode();
bool selectTrump();
void drawTrump(int, int, int);
bool drawTrumpPlayer();
bool drawTrumpAI();
bool winCheck(int);

//---------------------------------------------------------------//
//---------------------------------------------------------------//
//ここから
//トランプゲーム処理
//---------------------------------------------------------------//
//---------------------------------------------------------------//

//レンダリングエンジン
Display display;
//バックグラウンド画像
Texture background;
//テクスチャーリスト
TextureDataLoader textureList;
//配るトランプ（ストック）
vector<Trump*> stock;
//捨てたトランプ
vector<Trump*> trush;
//プレイヤー
vector<People> people;
//カード枚数を表示するテキスト
vector<TextField> cardCountLabel;
//カード選択のカーソル
Texture cursorSprite;
//ゲームの進行状態を示すフェーズ
string phase;
//デバッグモードかどうか
bool debug = false;
//初期化
#include <unistd.h>

//ゲームオーバーアニメーション
TitleScreen titleScreen;
Gameover gameover;
//この関数はゲーム開始時に呼ばれる。
void init() {

	//変数初期化
	stock.clear();
	people.clear();
	trush.clear();
	cardCountLabel.clear();

	//ファイルからテクスチャをロード
	textureList.load(TEXTURE_PATH);
	//cursorの初期化
	cursorSprite.setTextureData(textureList.get("cursor"));
	background.setTextureData(textureList.get("background"));
	//ディスプレイのセットアップ
	display.setup(79, 24, ' ');
	//プレイヤー初期化
	//initPlayer(PLAYER);
	//トランプを初期化
	initTrump();

	//phaseを配るモード（deal）に
	//このphaseはゲームの進行状況を示す変数。
	phase = "title";


	//ゲームオーバーアニメーションのセットアップ
	titleScreen.setup(&display, textureList.get("title"));
	gameover.setup(&display, textureList.get("won_message"), textureList.get("lose_message"));
}
//ゲーム進行用関数
//この関数は一番上の定数で設定されたフレームレートを元に
//毎フレームごとに呼び出される。
//ゲームの進行をこれで行い、またアニメーションをするためでもある。
void enterFrame() {
	//画面のレンダリングが完了したことを示す
	bool render = false;
	//そのフェーズが完了したかどうかを示す
	bool phaseB = false;

	display.clear();
	display.repeatBackground(&background);
	if(phase == "title"){
		display.clear();
		if(titleScreen.render()){
			initPlayer(titleScreen.getPlayerNum());
			phase = "deal";
		}
		//phase = "deal";
	}else if(phase == "deal"){
		//カードを配る
		phaseB = dealTrump();
		render = renderScreen();
		if(phaseB && render){
			//配り終わる
			phase = "firstDiscard";
		}
	}else if(phase == "firstDiscard"){
		//最初にカードを捨てる
		phaseB = firstDiscardTrump();
		render = renderScreen();
		if(phaseB && render){
			//カードを捨て終わる
			phase = "selectMode";
		}
	}else if(phase == "selectMode"){
		//カード選択モードへ移行
		phaseB = selectTrumpMode();
		render = renderScreen();
		if(phaseB && render){
			//移行した
			phase = "select";
		}
	}else if(phase == "select"){
		//カードを選択する
		render = renderScreen();
		if(render){
			if(selectTrump()){
				//選択したらドローする
				phase = "draw";
				drawTrumpPlayer();
			}
		}
	}else if(phase == "draw"){
		//ドローした
		render = renderScreen();
		if(render){
			//ドローした
			phase = "discard";
		}
	}else if(phase == "discard"){
		//カードを捨てる
		phaseB = discardTrump(0);
		render = renderScreen();
		if(!phaseB && render){
			//捨て終わる
			phase = "drawAI";
		}
	}else if(phase == "drawAI"){
		//AIのカード処理
		phaseB = drawTrumpAI();
		render = renderScreen();
		if(phaseB){
			//カード処理が終わる
			phase = "selectMode";
		}
	}else if(phase == "win" || phase == "lose"){
		//ゲーム終了
		render = renderScreen();
		//printf("%f", w);
		gameover.setResult(phase);
		if(gameover.render()){
			exit(1);
		}
		//nf.render();
		//3回文字を送ったら終了する
	}
	display.render();
	//cout << "phase : \n" << phase;
	//getcharf();
}

//プレイヤー初期化
void initPlayer(int n){
	for(int i = 0; i < n; i ++){
		People p;
		p.name = "noname0" + _to_string(i);
		p.face.add("happy", "(*^-^*)< Yatta-!");
		p.face.add("sad", "( -_- )< Saiaku...");
		p.face.add("normal", "( '-' )");
		people.push_back(p);
		TextField t;
		cardCountLabel.push_back(t);
		//printf("added : %s\n", p.name.c_str());
	}
}
//トランプ初期化
void initTrump(){
	//１から１３までを４回の計５２枚
	int i = 0;
	for(int mark = 0; mark < 4; mark ++){
		int maxNum = mark==3 ? 14 : 13;
		for(int number = 0; number < maxNum; number ++){
			Trump* t = new Trump;
			//数字、マーク、テクスチャリスト
			t->setup(number + 1, mark, &textureList);
			//非表示に設定
			t->setVisible(false);
			//初期座標を設定
			t->texture->setX(display.getWidth() / 2);
			t->texture->setY(display.getHeight() / 2);
			stock.push_back(t);
			i++;
		}
	}
	//シャッフル
	srand(time(0));
	//printf("%d", time(0));
	random_shuffle(stock.begin(), stock.end());
}
//カードを配る
bool dealTrump() {
	//時間を遅らせる処理
	static int wait = 999;
	wait ++;
	if(wait < 8) {
		return false;
	}else{
		wait = 0;
	}
	//カードを人数分配る。
	static int peopleId = 0;
	if(0 < stock.size()){
		//ユーザーIDを上限内に収める。
		if(peopleId >= people.size()) peopleId = 0;
		people[peopleId].addTrump(stock.back());
		//配りながらストックから削除する。
		stock.pop_back();
		//ユーザーIDを増加
		peopleId++;
		return false;
	}
	return true;
}
//カードを捨てる
bool firstDiscardTrump() {
	//時間を遅らせる処理
	static int wait = 999;
	wait ++;
	if(wait < 30){
		return false;
	}else{
		wait = 0;
	}
	//終了ステータス
	int status = 1;
	int size = people.size();
	for(int i = 0; i < size; i ++){
		//一人ずつ１ペアずつカードを捨てる
		status -= discardTrump(i);
	}
	return 0 < status;
}
bool discardTrump(int id) {
	//手札のサイズを取得
	int size = people[id].hand.size();
	//（手札はソートしないので逐次探索）
	for(int k = 0; k < size; k ++){
		//数字を仮に保管
		int number = people[id].hand[k]->number;
		for(int j = 0; j < size; j ++){
			if(j != k){
				//数字が一致したら
				if(people[id].hand[j]->number == number){
					int tj;
					if(j < k){
						//kとjを入れ替える
						tj = j;
						j = k;
						k = tj;
					}
					//インデックスの大きい順に削除する。
					//そのためにkとjを入れ替える処理がある。
					trush.push_back(people[id].removeTrump(j));
					trush.push_back(people[id].removeTrump(k));
					winCheck(id);
					return true;
				}
			}
		}
	}
	return false;
}
//カード引モード
bool selectTrumpMode() {
	people[1].select = true;
	return true;
}
//カードを選択
bool selectTrump() {
	char c = getcharf();
	if(c == 'd') debug = !debug;
	if(c == 'a') return true;
	//増やす
	people[1].selectedId++;
	//行き過ぎた場合０にする
	if(people[1].selectedId >= people[1].hand.size()) people[1].selectedId = 0;
	return false;
}
//カードを引く
void drawTrump(int drawerId, int fromId, int id = -1){
	if(id < 0){
		//マイナスの場合ランダムにする
		id = rand()%people[fromId].hand.size();
	}
	//removeTrumpの戻り値をaddTrumpする。
	Trump* trump = people[fromId].removeTrump(id);
	people[drawerId].addTrump(trump);
	//AIならシャッフルする
	if(drawerId != 0){
		srand(time(0));
		random_shuffle(people[drawerId].hand.begin(), people[drawerId].hand.end());
	}

	if(trump->number == 14) {
		people[fromId].face.set("happy");
		people[drawerId].face.set("sad");
	}else{
		people[drawerId].face.set("normal");
	}
	//勝利チェックをする
	winCheck(fromId);
}
//プレイヤーがカードを引く
bool drawTrumpPlayer() {
	people[1].select = false;
	//引く
	drawTrump(0, 1, people[1].selectedId);
	//選択idをリセット
	people[1].selectedId = 0;
	return true;
}
//AIがカードを引く
bool drawTrumpAI() {
	static int wait = 0;
	static string phase = "draw";
	static int id = 1;
	//時間を遅らせる処理
	wait ++;
	if(wait < 20){
		return false;
	}else{
		wait = 0;
	}
	//IDが最大を超えたら１に戻す
	if(id >= people.size()){
		id = 1;
		return true;
	}
	int targetId = id + 1;
	//最初にカードを引く
	//その次にカードを捨てるphaseがdrawからdiscardになる。
	if(phase == "draw"){
		if(targetId >= people.size()) targetId = 0;
		//引く
		drawTrump(id, targetId, -1);
		phase = "discard";
	}else if(phase == "discard"){
		//カードを捨てる
		discardTrump(id);
		phase = "draw";
		id++;
	}
	return false;
}
bool winCheck(int id){
	//手札の数が１未満の場合勝利する。
	printf("%d\n",people[id].hand.size());
	if(people[id].hand.size() < 1){
		people[id].face.set("happy");
		//誰かが勝ったらゲームを終了
		if(id == 0){
			//プレイヤーだったら勝利
			phase = "win";
		}else{
			phase = "lose";
		}
		return true;
	}
	return false;
}
//--------------------------------------------------------//
//レンダリング
//--------------------------------------------------------//
bool renderStock();
bool renderTrush();
bool renderPlayer();
bool renderAI();
bool renderSelecting(int i);
//親
bool renderScreen() {
	int status = 1;
	//全てのステータスが完了したか調べる
	status -= !renderStock();
	status -= !renderTrush();
	status -= !renderPlayer();
	status -= !renderAI();
	return 0 < status;
}
//子：ストックの描画（配るとき）
bool renderStock() {
	int status = 1;
	int stockSize = stock.size();
	int x = 5;
	int y = display.getHeight() / 2 - 2;
	for(int i = 0; i < stockSize; i ++){
		stock[i]->setVisible(false);
		Texture* sprite = stock[i]->texture;
		//所定の位置へ
		sprite->setX(x);
		sprite->setY(y);
		//描画
		display.draw(sprite);
	}
	return 0 < status;
}
//子：捨てたトランプの描画
bool renderTrush() {
	int status = 1;
	int size = trush.size();
	int x = display.getWidth() - 5 - TRUMP_WIDTH;
	int y = display.getHeight() / 2 - 2;
	for(int i = 0; i < size; i ++){
		trush[i]->setVisible(true);
		Texture* sprite = trush[i]->texture;
		//所定の位置へ
		status -= !SpriteTween::moveTo(sprite, x, y);
		//描画
		display.draw(sprite);
	}
	return 0 < status;
}
//子：プレイヤーの描画
bool renderPlayer() {
	int status = 1;
	int handSize = people[0].hand.size();
	//位置調整処理
	int twidth = handSize * TRUMP_SPACE;
	int tx = (display.getWidth() - twidth) / 2 - TRUMP_WIDTH / 2 + TRUMP_SPACE;
	int x, y = display.getHeight() - 5;
	for(int j = 0; j < handSize; j++){
		x = tx + TRUMP_SPACE * j;
		//手札は常に表示
		people[0].hand[j]->setVisible(true);
		Texture* sprite = people[0].hand[j]->texture;
		//所定の位置へ
		status -= !SpriteTween::moveTo(sprite, x, y);
		//描画
		display.draw(sprite);
	}
	//手札の枚数表示
	cardCountLabel[0].setText("x"+_to_string(handSize));
	cardCountLabel[0].setX(tx + TRUMP_SPACE * handSize + TRUMP_WIDTH / 2 + TRUMP_SPACE - 1);
	cardCountLabel[0].setY(y + 4);
	display.draw(&cardCountLabel[0]);
	return 0 < status;
}
//子：AIの描画
bool renderAI() {
	int status = 1;
	int handSize;
	int peopleSize = people.size() - 1;
	int x, y = 2;
	for(int i = 0; i < peopleSize; i ++){
		int id = i + 1;
		if(people[id].select){
			status -= !renderSelecting(id);
			continue;
		}
		//手札の枚数
		handSize = people[id].hand.size();
		//ベース座標
		x = i * display.getWidth() / peopleSize + display.getWidth() / (peopleSize * 2) - TRUMP_WIDTH / 2;
		y = 2;
		status -= !SpriteTween::moveTo(people[id].face.texture, x, 1);
		display.draw(people[id].face.texture);
		//手札の枚数表示
		cardCountLabel[id].setText("x"+_to_string(handSize));
		cardCountLabel[id].setX(x + TRUMP_WIDTH);
		cardCountLabel[id].setY(y + 6);
		display.draw(&cardCountLabel[id]);
		//手札を選ぶとき用の座標
		int twidth = handSize * TRUMP_SPACE;
		int tx = (display.getWidth() - twidth) / 2 - TRUMP_WIDTH / 2 + TRUMP_SPACE;
		for(int j = 0; j < handSize; j ++){
			//AIの手札は非表示(デバッグモード時は表示)
			people[id].hand[j]->setVisible(debug);
			Texture* sprite = people[id].hand[j]->texture;
			//所定の位置へ
			status -= !SpriteTween::moveTo(sprite, x, y);
			//描画
			display.draw(sprite);
		}
	}
	//すべて描画
	return 0 < status;
}
//子：選択画面の描画
bool renderSelecting(int i) {
	int handSize;
	int x, y;
	int status = 1;
	handSize = people[i].hand.size();
	int twidth = handSize * TRUMP_SPACE;
	int tx = (display.getWidth() - twidth) / 2 - TRUMP_WIDTH / 2 + TRUMP_SPACE;
	status -= !SpriteTween::moveTo(people[i].face.texture, tx - 8, display.getHeight()/2 + 1);
	display.draw(people[i].face.texture);
	for(int j = 0; j < handSize; j ++){
		//もし選択中だったら、手札を選択モードの配置につかせる。
		x = tx + TRUMP_SPACE * j;
		y = display.getHeight()/2 - 2;
		if(people[i].selectedId == j){
			//選択中のカードは少し上にあげる
			y -= 1;
			//カーソルの描画
			cursorSprite.setX(x + 2);
			cursorSprite.setY(y + 8);
			display.draw(&cursorSprite);
		}
		cardCountLabel[i].setY(y + 6);
		//AIの手札は非表示(デバッグモード時は表示)
		people[i].hand[j]->setVisible(debug);
		Texture* sprite = people[i].hand[j]->texture;
		//所定の位置へ
		status -= !SpriteTween::moveTo(sprite, x, y);
		//描画
		display.draw(sprite);
	}
	return 0 < status;
}
//--------------------------------------------------------//
//１度に複数文字の入力が来ても
//最初の１つしか返さないgetcharの派生型
//--------------------------------------------------------//
char getcharf(void) {
	char tc;
	char c;
	bool f = true;
	//改行が来るまで繰り返し。
	while((tc = getchar())!='\n'){
		if(f) c = tc;
		//最初に来た文字のみを保管
		f = false;
	}
	//返す
	return c;
}
//--------------------------------------------------------//
//int型をstring型に変換する。
//（to_stringは環境やバージョンで使えたり使えなかったりしたので。）
//--------------------------------------------------------//
string _to_string(int number)
{
	stringstream s;
	s << number;
	return s.str();
}
//--------------------------------------------------------//
//メイン
//--------------------------------------------------------//
int main() {
	//printf("frameRate : ");
	//scanf("%d", &FRAME_RATE);
	init();
	clock_t c;
	//1秒をFPSで割る。
	double interval = (1.0/FRAME_RATE) * CLOCKS_PER_SEC;
	while(1) {
		//毎フレーム呼び出し
		enterFrame();
		c = clock();
		while(clock() - c < interval);
		//getchar();
	}
	return 0;
}
