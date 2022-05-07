// Title.c - タイトル
//

// 外部参照
//
#include <string.h>
#include "pd_api.h"
#include "Iocs.h"
#include "Scene.h"
#include "Actor.h"
#include "Aseprite.h"
#include "Application.h"
#include "Title.h"
#include "Logo.h"
#include "Car.h"
#include "Starter.h"

// 内部関数
//
static void TitleUnload(struct Title *title);
static void TitleTransition(struct Title *title, TitleFunction function);
static void TitlePlay(struct Title *title);
static void TitleDone(struct Title *title);

// 内部変数
//
static const char *titleSpriteNames[] = {
    "null", 
    "logo", 
    "car", 
};


// タイトルを更新する
//
void TitleUpdate(struct Title *title)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (title == NULL) {

        // タイトルの作成
        title = playdate->system->realloc(NULL, sizeof (struct Title));
        if (title == NULL) {
            playdate->system->error("%s: %d: title instance is not created.", __FILE__, __LINE__);
            return;
        }
        memset(title, 0, sizeof (struct Title));

        // タイトルの初期化
        {
            // ユーザデータの設定
            SceneSetUserdata(title);

            // 解放の設定
            SceneSetUnload((SceneFunction)TitleUnload);
        }

        // スプライトの読み込み
        AsepriteLoadSpriteList(titleSpriteNames, kTitleSpriteNameSize);

        // ロゴの初期化
        LogoInitialize();

        // 車の初期化
        CarInitialize();

        // スターターの初期化
        StarterInitialize();

        // 処理の設定
        TitleTransition(title, (TitleFunction)TitlePlay);
    }

    // 処理の更新
    if (title->function != NULL) {
        (*title->function)(title);
    }
}

// タイトルを解放する
//
static void TitleUnload(struct Title *title)
{
    // アクタの解放
    ActorUnloadAll();

    // スプライトの解放
    AsepriteUnloadAllSprites();
}

// 処理を遷移する
//
static void TitleTransition(struct Title *title, TitleFunction function)
{
    title->function = function;
    title->state = 0;
}

// タイトルをプレイする
//
static void TitlePlay(struct Title *title)
{
    // 初期化
    if (title->state == 0) {

        // ロゴの読み込み
        LogoLoad();

        // 車の読み込み
        CarLoad();

        // スターターの読み込み
        StarterLoad();

        // 初期化の完了
        ++title->state;
    }

    // スターターの監視
    if (StarterIsDone()) {
        TitleTransition(title, (TitleFunction)TitleDone);
    }
}

// タイトルを完了する
//
static void TitleDone(struct Title *title)
{
    // 初期化
    if (title->state == 0) {

        // 初期化の完了
        ++title->state;
    }

    // シーンの遷移
    ApplicationTransition(kApplicationSceneGame);
}

// スプライトの名前を取得する
//
const char *TitleGetSpriteName(TitleSpriteName name)
{
    return titleSpriteNames[name];
}

