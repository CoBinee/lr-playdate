// EnemOfficery.c - 警官
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
    kEnemyOfficerStayMillisecond = 500, 
};

// 内部関数
//
static void EnemyOfficerUnload(struct Enemy *enemy);
static void EnemyOfficerDraw(struct Enemy *enemy);
static void EnemyOfficerStart(struct Enemy *enemy);
static void EnemyOfficerStay(struct Enemy *enemy);
static void EnemyOfficerWalk(struct Enemy *enemy);
static void EnemyOfficerStop(struct Enemy *enemy);

// 内部変数
//


// 警官を読み込む
//
void EnemyOfficerLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Enemy *enemy = (struct Enemy *)ActorLoad((ActorFunction)EnemyOfficerStart, kGamePriorityEnemy);
    if (enemy == NULL) {
        playdate->system->error("%s: %d: enemy officer actor is not loaded.", __FILE__, __LINE__);
    }

    // 警官の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&enemy->actor, (ActorFunction)EnemyOfficerUnload);

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
        enemy->speedMaximum = (1.0f * (1000 / 30)) / IocsGetFrameMillisecond();
        enemy->accel = enemy->speedMaximum / 10.0f;
    }
}

// 警官を解放する
//
static void EnemyOfficerUnload(struct Enemy *enemy)
{
    EnemyControllerDecOfficerCount();
}

// 警官を描画する
//
static void EnemyOfficerDraw(struct Enemy *enemy)
{
    // スプライトの描画
    AsepriteDrawRotatedSpriteAnimation(&enemy->animation, enemy->position.x, enemy->position.y, enemy->position.a - 180.0f, 0.5f, 1.0f, enemy->direction, 1.0f, kDrawModeCopy);
}

// 警官が出現する
//
static void EnemyOfficerStart(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameOfficer), "Blink", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyOfficerDraw, kGameOrderEnemy);

    // アニメーションの完了
    if (AsepriteIsSpriteAnimationDone(&enemy->animation)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyOfficerStay);
    }

}

// 警官が待機する
//
static void EnemyOfficerStay(struct Enemy *enemy)
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
        enemy->millisecond = kEnemyOfficerStayMillisecond;

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameOfficer), "Stay", false);

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
            enemy->millisecond = kEnemyOfficerStayMillisecond;

        // 移動開始
        } else {
            ActorTransition(&enemy->actor, (ActorFunction)EnemyOfficerWalk);
        }
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyOfficerDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyOfficerStop);
    }
}

// 警官が歩く
//
static void EnemyOfficerWalk(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // ミリ秒の設定
        enemy->millisecond = ((IocsGetRandom() % (int)(360.0f / enemy->speedMaximum)) + (int)(180.0f / enemy->speedMaximum)) * IocsGetFrameMillisecond();

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameOfficer), "Walk", true);

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
            ActorTransition(&enemy->actor, (ActorFunction)EnemyOfficerStay);
        }
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyOfficerDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyOfficerStop);
    }
}

// 警官が止まる
//
static void EnemyOfficerStop(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameOfficer), "Stay", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyOfficerDraw, kGameOrderEnemy);
}

