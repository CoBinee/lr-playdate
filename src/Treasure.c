// Treasure.c - 宝
//

// 外部参照
//
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"
#include "Treasure.h"

// 内部関数
//
static void TreasureControllerUnload(struct TreasureController *controller);
static void TreasureControllerDraw(struct TreasureController *controller);
static void TreasureControllerUpdate(struct TreasureController *controller);
static void TreasureControllerDecCount(void);
static void TreasureLoad(TreasureType type);
static void TreasureUnload(struct Treasure *treasure);
static void TreasureDraw(struct Treasure *treasure);
static void TreasureStay(struct Treasure *treasure);
static void TreasureTaken(struct Treasure *treasure);

// 内部変数
//


// 宝を初期化する
//
void TreasureInitialize(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの確認
    if (sizeof (struct TreasureController) > kActorBlockSize) {
        playdate->system->error("%s: %d: treasure controller actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct TreasureController));
    }
    if (sizeof (struct Treasure) > kActorBlockSize) {
        playdate->system->error("%s: %d: treasure actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct Treasure));
    }
}

// 宝コントローラを読み込む
//
void TreasureControllerLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct TreasureController *controller = (struct TreasureController *)ActorLoad((ActorFunction)TreasureControllerUpdate, kGamePriorityTreasureController);
    if (controller == NULL) {
        playdate->system->error("%s: %d: treasure controller actor is not loaded.", __FILE__, __LINE__);
    }

    // コントローラの初期化
    {
        // 解放処理の設定
        ActorSetUnload(&controller->actor, (ActorFunction)TreasureControllerUnload);

        // タグの設定
        ActorSetTag(&controller->actor, kGameTagTreasureController);

        // カウントの設定
        controller->count = 0;

        // ミリ秒の設定
        controller->millisecond = 0;
    }
}

// 宝コントローラを解放する
//
static void TreasureControllerUnload(struct TreasureController *controller)
{
    ;
}

// 宝コントローラを描画する
//
static void TreasureControllerDraw(struct TreasureController *controller)
{
    ;
}

// 宝コントローラを更新する
//
static void TreasureControllerUpdate(struct TreasureController *controller)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (controller->actor.state == 0) {

        // ミリ秒の設定
        controller->millisecond = kTreasureMillisecondImmidiate;

        // 初期化の完了
        ++controller->actor.state;
    }

    // ゲームをプレイ中
    if (GameIsPlay()) {

        // 宝はまだ出現可能
        if (controller->count < kTreasureCountMaximum) {

            // ミリ秒の更新
            if (controller->millisecond > 0) {
                controller->millisecond -= IocsGetFrameMillisecond();
            }

            // 宝の発生
            if (
                controller->millisecond <= 0 && 
                GameIsPlay() && 
                GameGetTimer() > kGameTimerLimit
            ) {
                if (PlayerGetRoundCount() < 3) {
                    TreasureLoad(kTreasureTypeGold);
                } else {
                    TreasureLoad(kTreasureTypeJewel);
                    PlayerClearRoundCount();
                }
                ++controller->count;
                controller->millisecond = kTreasureMillisecondInterval;
            }
        }
    }
}

// カウントを減らす
//
static void TreasureControllerDecCount(void)
{
    struct TreasureController *controller = (struct TreasureController *)ActorFindWithTag(kGameTagTreasureController);
    if (controller != NULL) {
        if (controller->count > 0) {
            --controller->count;
        }
    }
}

// 宝を読み込む
//
static void TreasureLoad(TreasureType type)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Treasure *treasure = (struct Treasure *)ActorLoad((ActorFunction)TreasureStay, kGamePriorityTreasure);
    if (treasure == NULL) {
        playdate->system->error("%s: %d: treasure actor is not loaded.", __FILE__, __LINE__);
    }

    // 宝の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&treasure->actor, (ActorFunction)TreasureUnload);

        // タグの設定
        // ActorSetTag(&treasure->actor, kGameTagTreasure);

        // 位置の設定
        treasure->position.a = GameAdjustPositionA(PlayerGetPositionA() + (float)(IocsGetRandom() % 270) + 45.0f);
        treasure->position.r = GameGetR();
        treasure->position.w = 12.0f;
        treasure->position.h = 12.0f;

        // 種類の設定
        treasure->type = type;

        // スプライトの設定
        treasure->sprite = type == kTreasureTypeGold ? kGameSpriteNameGold : kGameSpriteNameJewel;
    }
}

// 宝を解放する
//
static void TreasureUnload(struct Treasure *treasure)
{
    // カウントを減らす
    TreasureControllerDecCount();
}

// 宝を描画する
//
static void TreasureDraw(struct Treasure *treasure)
{
    // スプライトの描画
    AsepriteDrawRotatedSpriteAnimation(&treasure->animation, treasure->position.x, treasure->position.y, treasure->position.a - 180.0f, 0.5f, 1.0f, 1.0f, 1.0f, kDrawModeCopy);
}

// 宝が待機する
//
static void TreasureStay(struct Treasure *treasure)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (treasure->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&treasure->animation, GameGetSpriteName(treasure->sprite), "Stay", false);

        // 初期化の完了
        ++treasure->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&treasure->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&treasure->animation);

    // 描画処理の設定
    ActorSetDraw(&treasure->actor, (ActorFunction)TreasureDraw, kGameOrderTreasure);

    // プレイヤとのヒット、あるいはゲームクリア
    if (
        PlayerIsHit(&treasure->position) || 
        GameIsComplete()
    ) {
        ActorTransition(&treasure->actor, (ActorFunction)TreasureTaken);
    }
}

// 宝が取られる
//
static void TreasureTaken(struct Treasure *treasure)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (treasure->actor.state == 0) {

        // スコアの加算
        GameAddScore(treasure->type == kTreasureTypeGold ? kGameScoreGold : kGameScoreJewel);

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&treasure->animation, GameGetSpriteName(treasure->sprite), "Taken", false);

        // オーディオの再生
        IocsPlayAudioEffect(kGameAudioSampleCoin, 1);

        // 初期化の完了
        ++treasure->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&treasure->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&treasure->animation);

    // 描画処理の設定
    ActorSetDraw(&treasure->actor, (ActorFunction)TreasureDraw, kGameOrderTreasure);

    // アニメーションの完了
    if (AsepriteIsSpriteAnimationDone(&treasure->animation)) {
        ActorUnload(&treasure->actor);
    }
}



