// Game.c - ゲーム
//

// 外部参照
//
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "pd_api.h"
#include "Iocs.h"
#include "Scene.h"
#include "Aseprite.h"
#include "Application.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Treasure.h"
#include "Background.h"
#include "Status.h"
#include "Intro.h"
#include "Result.h"

// 内部関数
//
static void GameUnload(struct Game *game);
static void GameTransition(struct Game *game, GameFunction function);
static void GameIntro(struct Game *game);
static void GameLoad(struct Game *game);
static void GamePlay(struct Game *game);
static void GameOver(struct Game *game);
static void GameComplete(struct Game *game);
static void GameDone(struct Game *game);

// 内部変数
//
static const char *gameSpriteNames[] = {
    "null", 
    "back", 
    "thief", 
    "officer", 
    "inspector", 
    "dog", 
    "gold", 
    "jewel", 
    "helicopter", 
    "getoff", 
    "result", 
};
static const char *gameAudioSamplePaths[] = {
    "sounds/helihigh", 
    "sounds/helilow", 
    "sounds/jump", 
    "sounds/coin", 
};
static const char *gameAudioMusicPath = "sounds/theme";


// ゲームを更新する
//
void GameUpdate(struct Game *game)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (game == NULL) {

        // ゲームの作成
        game = playdate->system->realloc(NULL, sizeof (struct Game));
        if (game == NULL) {
            playdate->system->error("%s: %d: game instance is not created.", __FILE__, __LINE__);
            return;
        }
        memset(game, 0, sizeof (struct Game));

        // ゲームの初期化
        {
            // ユーザデータの設定
            SceneSetUserdata(game);

            // 解放の設定
            SceneSetUnload((SceneFunction)GameUnload);

            // スコアの設定
            game->score = 0;

            // タイマの設定
            game->timer = kGameTimerMaximum;

            // プレイ中の設定
            game->play = false;
        }

        // スプライトの読み込み
        AsepriteLoadSpriteList(gameSpriteNames, kGameSpriteNameSize);

        // オーディオの読み込み
        IocsLoadAudioEffects(gameAudioSamplePaths, kGameAudioSampleSize);

        // プレイヤの初期化
        PlayerInitialize();

        // エネミーの初期化
        EnemyInitialize();

        // 宝の初期化
        TreasureInitialize();

        // 背景の初期化
        BackgroundInitialize();

        // ステータスの初期化
        StatusInitialize();

        // 導入の初期化
        IntroInitialize();

        // 結果の初期化
        ResultInitialize();

        // 処理の設定
        GameTransition(game, (GameFunction)GameIntro);
    }

    // 処理の更新
    if (game->function != NULL) {
        (*game->function)(game);
    }
}

// ゲームを解放する
//
static void GameUnload(struct Game *game)
{
    // アクタの解放
    ActorUnloadAll();

    // スプライトの解放
    AsepriteUnloadAllSprites();

    // オーディオの解放
    IocsUnloadAllAudioEffects();
}

// 処理を遷移する
//
static void GameTransition(struct Game *game, GameFunction function)
{
    game->function = function;
    game->state = 0;
}

// ゲームを導入する
//
static void GameIntro(struct Game *game)
{
    // 初期化
    if (game->state == 0) {

        // 背景の読み込み
        BackgroundLoad();

        // ステータスの読み込み
        StatusLoad();

        // 導入の読み込み
        IntroLoad();

        // 初期化の完了
        ++game->state;
    }

    // 処理の遷移
    if (IntroIsDone()) {
        GameTransition(game, (GameFunction)GameLoad);
    }
}

// ゲームを読み込むする
//
static void GameLoad(struct Game *game)
{
    // 初期化
    if (game->state == 0) {

        // 導入の解放
        ActorUnloadWithTag(kGameTagDemo);

        // プレイヤの読み込み
        PlayerLoad();
        
        // エネミーコントローラの読み込み
        EnemyControllerLoad();

        // 宝コントローラの読み込み
        TreasureControllerLoad();

        // 初期化の完了
        ++game->state;
    }

    // 処理の遷移
    GameTransition(game, (GameFunction)GamePlay);
}

// ゲームをプレイする
//
static void GamePlay(struct Game *game)
{
    // 初期化
    if (game->state == 0) {

        // プレイの設定
        game->play = true;

        // オーディオの再生
        IocsPlayMusicAudio(gameAudioMusicPath, 0);

        // 初期化の完了
        ++game->state;
    }

    // タイマの更新
    game->timer -= IocsGetFrameMillisecond();

    // ゲームクリア
    if (GameIsComplete()) {
        game->play = false;
        GameTransition(game, (GameFunction)GameComplete);

    // ゲームオーバー
    } else if (GameIsOver()) {
        game->play = false;
        GameTransition(game, (GameFunction)GameOver);
    }
}

// ゲームオーバーになる
//
static void GameOver(struct Game *game)
{
    // 初期化
    if (game->state == 0) {

        // 結果の読み込み
        ResultOverLoad();

        // オーディオの停止
        IocsStopMusicAudio();

        // 初期化の完了
        ++game->state;
    }

    // 結果の完了
    if (ResultIsDone()) {
        GameTransition(game, (GameFunction)GameDone);
    }
}

// ゲームをクリアする
//
static void GameComplete(struct Game *game)
{
    // 初期化
    if (game->state == 0) {

        // スコアの設定
        bool hiscore = ApplicationSetScore(game->score);

        // 結果の読み込み
        ResultCompleteLoad(hiscore);

        // オーディオの停止
        IocsStopMusicAudio();

        // 初期化の完了
        ++game->state;
    }

    // 結果の完了
    if (ResultIsDone()) {
        GameTransition(game, (GameFunction)GameDone);
    }    
}

// ゲームを完了する
//
static void GameDone(struct Game *game)
{
    // 初期化
    if (game->state == 0) {

        // 初期化の完了
        ++game->state;
    }

    // シーンの遷移
    ApplicationTransition(kApplicationSceneTitle);
}

// スプライトの名前を取得する
//
const char *GameGetSpriteName(GameSpriteName name)
{
    return gameSpriteNames[name];
}

// 半径を取得する
//
float GameGetR(void)
{
    return 120.0f;
}

// 位置を計算する
//
void GameCalcPosition(struct GamePosition *position)
{
    float rad = position->a * (float)M_PI / 180;
    position->x = (int)lrintf(position->r * sinf(rad)) + LCD_COLUMNS / 2;
    position->y = (int)lrintf(-position->r * cosf(rad)) + LCD_ROWS / 2;
}

// A 位置を 0.0 〜 360.0f の範囲で調整する
//
float GameAdjustPositionA(float a)
{
    while (a >= 360.0f) {
        a -= 360.0f;
    }
    while (a < 0.0f) {
        a += 360.0f;
    }
    return a;
}

// ２点間の距離を取得する
//
float GameGetDistance(float origin, float target)
{
    float d = target - origin;
    if (d >= 180.0f) {
        d -= 360.0f;
    } else if (d < -180.0f) {
        d += 360.0f;
    }
    return d;
}

// スコアを取得する
//
int GameGetScore(void)
{
    struct Game *game = (struct Game *)SceneGetUserdata();
    return game != NULL ? game->score : 0;
}

// スコアを加算する
//
void GameAddScore(int score)
{
    // ゲームの取得
    struct Game *game = (struct Game *)SceneGetUserdata();
    if (game == NULL) {
        return;
    }

    // スコアの加算
    game->score += score;
    if (game->score > kGameScoreMaximum) {
        game->score = kGameScoreMaximum;
    }
}

// タイマを取得する
//
int GameGetTimer(void)
{
    struct Game *game = (struct Game *)SceneGetUserdata();
    return game != NULL && game->timer > 0 ? game->timer : 0;
}

// ゲームをプレイ中かどうかを判定する
//
bool GameIsPlay(void)
{
    struct Game *game = (struct Game *)SceneGetUserdata();
    return game != NULL ? game->play : false;
}

// ゲームオーバーかどうかを判定する
//
bool GameIsOver(void)
{
    return !PlayerIsLive();
}

// ゲームが時間切れしたかどうかを判定する
//
bool GameIsTimeout(void)
{
    struct Game *game = (struct Game *)SceneGetUserdata();
    return game != NULL && game->timer <= 0 ? true : false;
}

// ゲームをクリアしたかどうかを判定する
//
bool GameIsComplete(void)
{
    struct Game *game = (struct Game *)SceneGetUserdata();
    return game != NULL && game->timer <= kGameTimerComplete ? true : false;
}

