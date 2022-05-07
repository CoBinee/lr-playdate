// EnemDogy.c - 犬
//

// 外部参照
//
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"


// 内部定数
//
enum {
    kEnemyDogStayMillisecond = 500, 
};

// 内部関数
//
static void EnemyDogUnload(struct Enemy *enemy);
static void EnemyDogDraw(struct Enemy *enemy);
static void EnemyDogStart(struct Enemy *enemy);
static void EnemyDogStay(struct Enemy *enemy);
static void EnemyDogWalk(struct Enemy *enemy);
static void EnemyDogReady(struct Enemy *enemy);
static void EnemyDogRun(struct Enemy *enemy);
static void EnemyDogDone(struct Enemy *enemy);
static void EnemyDogStop(struct Enemy *enemy);

// 内部変数
//


// 犬を読み込む
//
void EnemyDogLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Enemy *enemy = (struct Enemy *)ActorLoad((ActorFunction)EnemyDogStart, kGamePriorityEnemy);
    if (enemy == NULL) {
        playdate->system->error("%s: %d: enemy dog actor is not loaded.", __FILE__, __LINE__);
    }

    // 犬の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&enemy->actor, (ActorFunction)EnemyDogUnload);

        // タグの設定
        // ActorSetTag(&enemy->actor, kGameTagEnemy);

        // 位置の設定
        enemy->position.a = GameAdjustPositionA(PlayerGetPositionA() + (float)(IocsGetRandom() % 180) + 90.0f);
        enemy->position.r = GameGetR();
        enemy->position.w = 12.0f;
        enemy->position.h = 24.0f;

        // 向きの設定
        enemy->direction = (float)((IocsGetRandom() & 0x02) - 0x01);

        // 速度の設定
        enemy->speed = 0.0f;
        enemy->speedWalk = (0.5f * (1000 / 30)) / IocsGetFrameMillisecond();
        enemy->speedRun = enemy->speedWalk * 4.0f;
        enemy->speedMaximum = enemy->speedWalk;
        enemy->accel = enemy->speedMaximum / 10.0f;

        // 目視の設定
        enemy->look = 75.0f;
    }
}

// 犬を解放する
//
static void EnemyDogUnload(struct Enemy *enemy)
{
    EnemyControllerDecDogCount();
    IocsGetPlaydate()->system->logToConsole("dog.");
}

// 犬を描画する
//
static void EnemyDogDraw(struct Enemy *enemy)
{
    // スプライトの描画
    AsepriteDrawRotatedSpriteAnimation(&enemy->animation, enemy->position.x, enemy->position.y, enemy->position.a - 180.0f, 0.5f, 1.0f, enemy->direction, 1.0f, kDrawModeCopy);
}

// 犬が出現する
//
static void EnemyDogStart(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameDog), "Blink", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyDogDraw, kGameOrderEnemy);

    // アニメーションの完了
    if (AsepriteIsSpriteAnimationDone(&enemy->animation)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyDogStay);
    }

}

// 犬が待機する
//
static void EnemyDogStay(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // カウントの設定
        enemy->count = (IocsGetRandom() % 3) + 3;

        // ミリ秒の設定
        enemy->millisecond = kEnemyDogStayMillisecond;

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameDog), "Stay", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // ミリ秒の更新
    enemy->millisecond -= IocsGetFrameMillisecond();
    if (enemy->millisecond <= 0) {

        // カウントの更新
        --enemy->count;

        // 向きを変える
        if (enemy->count > 0) {
            enemy->direction = -enemy->direction;
            enemy->millisecond = kEnemyDogStayMillisecond;

        // 移動開始
        } else {
            ActorTransition(&enemy->actor, (ActorFunction)EnemyDogWalk);
        }
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyDogDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyDogStop);
    }
}

// 犬が歩く
//
static void EnemyDogWalk(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameDog), "Walk", true);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 移動
    EnemyMove(enemy);

    // プレイヤの目視
    if (EnemyIsLookPlayer(enemy)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyDogReady);
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyDogDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyDogStop);
    }
}

// 犬が構える
//
static void EnemyDogReady(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameDog), "Ready", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyDogDraw, kGameOrderEnemy);

    // アニメーションの完了
    if (AsepriteIsSpriteAnimationDone(&enemy->animation)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyDogRun);
    }

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyDogStop);
    }
}

// 犬が走る
//
static void EnemyDogRun(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // 速度の設定
        enemy->speedMaximum = enemy->speedRun;

        // ミリ秒の設定
        enemy->millisecond = ((IocsGetRandom() % (int)(360.0f / enemy->speedMaximum)) + (int)(180.0f / enemy->speedMaximum)) * IocsGetFrameMillisecond();

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameDog), "Run", true);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // ミリ秒の更新
    enemy->millisecond -= IocsGetFrameMillisecond();

    // 移動
    if (enemy->millisecond > 0) {
        EnemyMove(enemy);

    // 停止
    } else {
        if (EnemyStop(enemy)) {
            ActorTransition(&enemy->actor, (ActorFunction)EnemyDogDone);
        }
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyDogDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyDogStop);
    }
}

// 犬が去る
//
static void EnemyDogDone(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameDog), "Blink", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyDogDraw, kGameOrderEnemy);

    // アニメーションの完了
    if (AsepriteIsSpriteAnimationDone(&enemy->animation)) {
        ActorUnload(&enemy->actor);
    }
}

// 犬が止まる
//
static void EnemyDogStop(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameDog), "Stay", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyDogDraw, kGameOrderEnemy);
}

