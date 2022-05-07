// Car.c - 車
//

// 外部参照
//
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Title.h"
#include "Car.h"

// 内部関数
//
static void CarUnload(struct Car *car);
static void CarDraw(struct Car *car);
static void CarLoop(struct Car *car);

// 内部変数
//


// 車を初期化する
//
void CarInitialize(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの確認
    if (sizeof (struct Car) > kActorBlockSize) {
        playdate->system->error("%s: %d: car actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct Car));
    }
}

// 車を読み込む
//
void CarLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Car *car = (struct Car *)ActorLoad((ActorFunction)CarLoop, kTitlePriorityDemo);
    if (car == NULL) {
        playdate->system->error("%s: %d: car actor is not loaded.", __FILE__, __LINE__);
    }

    // 車の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&car->actor, (ActorFunction)CarUnload);

        // 位置の設定
        car->x = 400;
        car->y = 120;

        // ミリ秒の設定
        car->millisecond = 0;
    }
}

// 車を解放する
//
static void CarUnload(struct Car *car)
{
    ;
}

// 車を描画する
//
static void CarDraw(struct Car *car)
{
    // スプライトの描画
    AsepriteDrawSpriteAnimation(&car->animation, car->x, car->y, kDrawModeXOR, kBitmapUnflipped);
}

// 車が待機する
//
static void CarLoop(struct Car *car)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (car->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&car->animation, TitleGetSpriteName(kTitleSpriteNameCar), "Loop", true);

        // 初期化の完了
        ++car->actor.state;
    }

    // ミリ秒の更新
    car->millisecond += IocsGetFrameMillisecond();
    if (car->millisecond >= kCarMillisecondCycle) {
        car->millisecond -= kCarMillisecondCycle;
    }

    // 位置の更新
    car->x = 400 - (512 * car->millisecond / kCarMillisecondCycle);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&car->animation);

    // 描画処理の設定
    ActorSetDraw(&car->actor, (ActorFunction)CarDraw, kTitleOrderCar);
}


