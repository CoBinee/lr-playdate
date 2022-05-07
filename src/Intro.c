// Intro.c - 導入
//

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Game.h"
#include "Intro.h"

// 内部関数
//
static void IntroUnload(struct Intro *intro);
static void IntroDraw(struct Intro *intro);
static void IntroDemo(struct Intro *intro);

// 内部変数
//


// 導入を初期化する
//
void IntroInitialize(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの確認
    if (sizeof (struct Intro) > kActorBlockSize) {
        playdate->system->error("%s: %d: intro actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct Intro));
    }
}

// 導入を読み込む
//
void IntroLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Intro *intro = (struct Intro *)ActorLoad((ActorFunction)IntroDemo, kGamePriorityDemo);
    if (intro == NULL) {
        playdate->system->error("%s: %d: intro actor is not loaded.", __FILE__, __LINE__);
    }

    // 導入の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&intro->actor, (ActorFunction)IntroUnload);

        // タグの設定
        ActorSetTag(&intro->actor, kGameTagDemo);

        // ヘリコプターの設定
        intro->helicopterX = 344;
        intro->helicopterY = 80;

        // 降り立つの設定
        intro->getoffX = 200;
        intro->getoffY = 240;

        // 完了の設定
        intro->done = false;
    }
}

// 導入を解放する
//
static void IntroUnload(struct Intro *intro)
{
    ;
}

// 導入を描画する
//
static void IntroDraw(struct Intro *intro)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // スプライトの描画
    AsepriteDrawRotatedSpriteAnimation(&intro->animations[kIntroSpriteAnimationHelicopter], intro->helicopterX, intro->helicopterY, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f, kDrawModeFillWhite);
    AsepriteDrawRotatedSpriteAnimation(&intro->animations[kIntroSpriteAnimationGetoff], intro->getoffX, intro->getoffY, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f, kDrawModeFillWhite);
    AsepriteDrawSpriteAnimation(&intro->animations[kIntroSpriteAnimationMask], 0, 0, kDrawModeCopy, kBitmapUnflipped);
}

// 導入をデモする
//
static void IntroDemo(struct Intro *intro)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (intro->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&intro->animations[kIntroSpriteAnimationHelicopter], GameGetSpriteName(kGameSpriteNameHelicopter), "Loop", true);
        AsepriteStartSpriteAnimation(&intro->animations[kIntroSpriteAnimationGetoff], GameGetSpriteName(kGameSpriteNameGetoff), "Null", true);
        AsepriteStartSpriteAnimation(&intro->animations[kIntroSpriteAnimationMask], GameGetSpriteName(kGameSpriteNameBack), "Mask", true);

        // オーディオの再生
        intro->audio = IocsPlayAudioEffect(kGameAudioSampleHelicopterHigh, 0);

        // 初期化の完了
        ++intro->actor.state;
    }

    // 1: ヘリが入る
    if (intro->actor.state == 1) {
        intro->helicopterX -= 2;
        if (intro->helicopterX <= 208) {
            AsepriteStartSpriteAnimation(&intro->animations[kIntroSpriteAnimationGetoff], GameGetSpriteName(kGameSpriteNameGetoff), "Getoff", false);
            ++intro->actor.state;
        }
    }

    // 2: 降り立つ
    if (intro->actor.state == 2) {
        if (AsepriteIsSpriteAnimationDone(&intro->animations[kIntroSpriteAnimationGetoff])) {
            IocsStopAudioEffect(intro->audio);
            intro->audio = IocsPlayAudioEffect(kGameAudioSampleHelicopterLow, 0);
            ++intro->actor.state;
        }
    }

    // 3: ヘリが出る
    if (intro->actor.state == 3) {
        intro->helicopterX -= 2;
        if (intro->helicopterX <= 56) {
            IocsStopAudioEffect(intro->audio);
            intro->done = true;
        }
    }

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&intro->animations[kIntroSpriteAnimationHelicopter]);
    AsepriteUpdateSpriteAnimation(&intro->animations[kIntroSpriteAnimationGetoff]);
    AsepriteUpdateSpriteAnimation(&intro->animations[kIntroSpriteAnimationMask]);

    // 描画処理の設定
    ActorSetDraw(&intro->actor, (ActorFunction)IntroDraw, kGameOrderDemo);
}

// 導入が完了したかどうかを判定する
//
bool IntroIsDone(void)
{
    struct Intro *intro = (struct Intro *)ActorFindWithTag(kGameTagDemo);
    return intro != NULL ? intro->done : false;
}

