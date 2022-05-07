// EnemInspectory.c - 警部
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
    kEnemyInspectorStayMillisecond = 500, 
};

// 内部関数
//
static void EnemyInspectorUnload(struct Enemy *enemy);
static void EnemyInspectorDraw(struct Enemy *enemy);
static void EnemyInspectorStart(struct Enemy *enemy);
static void EnemyInspectorStay(struct Enemy *enemy);
static void EnemyInspectorWalk(struct Enemy *enemy);
static void EnemyInspectorJump(struct Enemy *enemy);
static void EnemyInspectorRun(struct Enemy *enemy);
static void EnemyInspectorStop(struct Enemy *enemy);

// 内部変数
//


// 警部を読み込む
//
void EnemyInspectorLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Enemy *enemy = (struct Enemy *)ActorLoad((ActorFunction)EnemyInspectorStart, kGamePriorityEnemy);
    if (enemy == NULL) {
        playdate->system->error("%s: %d: enemy inspector actor is not loaded.", __FILE__, __LINE__);
    }

    // 警部の初期化
    {
        // 解放処理の設定
        ActorSetUnload(&enemy->actor, (ActorFunction)EnemyInspectorUnload);

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
        enemy->speedWalk = (0.8f * (1000 / 30)) / IocsGetFrameMillisecond();
        enemy->speedRun = enemy->speedWalk * 2.0f;
        enemy->speedMaximum = enemy->speedWalk;
        enemy->accel = enemy->speedMaximum / 10.0f;

        // ジャンプの設定
        enemy->jump = 0.0f;
        enemy->jumpMaximum = (6.0f * (1000 / 30)) / IocsGetFrameMillisecond();
        enemy->gravity = enemy->jumpMaximum / (12.0f * (1000 / 30)) * IocsGetFrameMillisecond();

        // 目視の設定
        enemy->look = 60.0f;
    }
}

// 警部を解放する
//
static void EnemyInspectorUnload(struct Enemy *enemy)
{
    EnemyControllerDecInspectorCount();
}

// 警部を描画する
//
static void EnemyInspectorDraw(struct Enemy *enemy)
{
    // スプライトの描画
    AsepriteDrawRotatedSpriteAnimation(&enemy->animation, enemy->position.x, enemy->position.y, enemy->position.a - 180.0f, 0.5f, 1.0f, enemy->direction, 1.0f, kDrawModeCopy);
}

// 警部が出現する
//
static void EnemyInspectorStart(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameInspector), "Blink", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyInspectorDraw, kGameOrderEnemy);

    // アニメーションの完了
    if (AsepriteIsSpriteAnimationDone(&enemy->animation)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorStay);
    }

}

// 警部が待機する
//
static void EnemyInspectorStay(struct Enemy *enemy)
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
        enemy->millisecond = kEnemyInspectorStayMillisecond;

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameInspector), "Stay", false);

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
            enemy->millisecond = kEnemyInspectorStayMillisecond;

        // 移動開始
        } else {
            ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorWalk);
        }
    }

    // プレイヤの目視
    if (EnemyIsLookPlayer(enemy)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorJump);
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyInspectorDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorStop);
    }
}

// 警部が歩く
//
static void EnemyInspectorWalk(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // 速度の設定
        enemy->speedMaximum = enemy->speedWalk;

        // ミリ秒の設定
        enemy->millisecond = ((IocsGetRandom() % (int)(270.0f / enemy->speedMaximum)) + (int)(90.0f / enemy->speedMaximum)) * IocsGetFrameMillisecond();

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameInspector), "Walk", true);

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
            ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorStay);
        }
    }

    // プレイヤの目視
    if (EnemyIsLookPlayer(enemy)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorJump);
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyInspectorDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorStop);
    }
}

// 警部がジャンプする
//
static void EnemyInspectorJump(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // 速度の設定
        enemy->speed = 0.0f;

        // ジャンプの設定
        enemy->jump = -enemy->jumpMaximum;

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameInspector), "Jump", true);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // ジャンプ
    {
        enemy->jump = enemy->jump + enemy->gravity;
        enemy->position.r = enemy->position.r + enemy->jump;
        float r = GameGetR();
        if (enemy->position.r >= r) {
            enemy->position.r = r;
            ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorRun);
        }
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyInspectorDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorStop);
    }
}

// 警部が走る
//
static void EnemyInspectorRun(struct Enemy *enemy)
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
        enemy->millisecond = ((int)(90.0f / enemy->speedMaximum)) * IocsGetFrameMillisecond();

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameInspector), "Run", true);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // ミリ秒の更新
    enemy->millisecond -= IocsGetFrameMillisecond();

    // 移動
    if (
        enemy->millisecond > 0 || 
        EnemyIsLookPlayer(enemy)
    ) {
        EnemyMove(enemy);

    // 停止
    } else {
        if (EnemyStop(enemy)) {
            ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorStay);
        }
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyInspectorDraw, kGameOrderEnemy);

    // プレイヤにヒット
    if (PlayerIsHitThenMiss(&enemy->position)) {
        ActorTransition(&enemy->actor, (ActorFunction)EnemyInspectorStop);
    }
}

// 警部が止まる
//
static void EnemyInspectorStop(struct Enemy *enemy)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (enemy->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&enemy->animation, GameGetSpriteName(kGameSpriteNameInspector), "Stay", false);

        // 初期化の完了
        ++enemy->actor.state;
    }

    // 位置の計算
    GameCalcPosition(&enemy->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&enemy->animation);

    // 描画処理の設定
    ActorSetDraw(&enemy->actor, (ActorFunction)EnemyInspectorDraw, kGameOrderEnemy);
}

