// Background.c - 背景
//

// 外部参照
//
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Game.h"
#include "Background.h"

// 内部関数
//
static void BackgroundUnload(struct Background *background);
static void BackgroundDraw(struct Background *background);
static void BackgroundLoop(struct Background *background);

// 内部変数
//


// 背景を初期化する
//
void BackgroundInitialize(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの確認
    if (sizeof (struct Background) > kActorBlockSize) {
        playdate->system->error("%s: %d: background actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct Background));
    }
}

// 背景を読み込む
//
void BackgroundLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Background *background = (struct Background *)ActorLoad((ActorFunction)BackgroundLoop, kGamePriorityBackgroud);
    if (background == NULL) {
        playdate->system->error("%s: %d: background actor is not loaded.", __FILE__, __LINE__);
    }

    // 背景の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&background->actor, (ActorFunction)BackgroundUnload);
    }
}

// 背景を解放する
//
static void BackgroundUnload(struct Background *background)
{
    ;
}

// 背景を描画する
//
static void BackgroundDraw(struct Background *background)
{
    // スプライトの描画
    AsepriteDrawSpriteAnimation(&background->animation, 0, 0, kDrawModeCopy, kBitmapUnflipped);
}

// 背景が待機する
//
static void BackgroundLoop(struct Background *background)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (background->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&background->animation, GameGetSpriteName(kGameSpriteNameBack), "Loop", true);

        // 初期化の完了
        ++background->actor.state;
    }

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&background->animation);

    // 描画処理の設定
    ActorSetDraw(&background->actor, (ActorFunction)BackgroundDraw, kGameOrderBackground);
}


