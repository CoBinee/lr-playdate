// Enemy.c - エネミー
//

// 外部参照
//
#include <stdbool.h>
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"

// 内部関数
//
static void EnemyControllerUnload(struct EnemyController *controller);
static void EnemyControllerDraw(struct EnemyController *controller);
static void EnemyControllerUpdate(struct EnemyController *controller);
static void EnemyLoad(void);
static void EnemyUnload(struct Enemy *enemy);
static void EnemyDraw(struct Enemy *enemy);
static void EnemyStay(struct Enemy *enemy);
static void EnemyTaken(struct Enemy *enemy);

// 内部変数
//


// エネミーを初期化する
//
void EnemyInitialize(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの確認
    if (sizeof (struct EnemyController) > kActorBlockSize) {
        playdate->system->error("%s: %d: enemy controller actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct EnemyController));
    }
    if (sizeof (struct Enemy) > kActorBlockSize) {
        playdate->system->error("%s: %d: enemy actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct Enemy));
    }
}

// エネミーコントローラを読み込む
//
void EnemyControllerLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct EnemyController *controller = (struct EnemyController *)ActorLoad((ActorFunction)EnemyControllerUpdate, kGamePriorityEnemyController);
    if (controller == NULL) {
        playdate->system->error("%s: %d: enemy controller actor is not loaded.", __FILE__, __LINE__);
    }

    // コントローラの初期化
    {
        // 解放処理の設定
        ActorSetUnload(&controller->actor, (ActorFunction)EnemyControllerUnload);

        // タグの設定
        ActorSetTag(&controller->actor, kGameTagEnemyController);

        // 警官の設定
        controller->officerCount = 0;
        controller->officerMillisecond = kEnemyMillisecondOfficerStart;

        // 警部の設定
        controller->inspectorCount = 0;
        controller->inspectorMillisecond = kEnemyMillisecondInspectorStart;

        // 犬の設定
        controller->dogCount = 0;
        controller->dogMillisecond = kEnemyMillisecondDogStart;
    }
}

// エネミーコントローラを解放する
//
static void EnemyControllerUnload(struct EnemyController *controller)
{
    ;
}

// エネミーコントローラを描画する
//
static void EnemyControllerDraw(struct EnemyController *controller)
{
    ;
}

// エネミーコントローラを更新する
//
static void EnemyControllerUpdate(struct EnemyController *controller)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (controller->actor.state == 0) {

        // 初期化の完了
        ++controller->actor.state;
    }

    // ゲームをプレイ中
    if (GameIsPlay()) {

        // 警官の更新
        if (controller->officerCount < kEnemyCountOfficer) {
            controller->officerMillisecond -= IocsGetFrameMillisecond();
            if (controller->officerMillisecond <= 0) {
                EnemyOfficerLoad();
                ++controller->officerCount;
                controller->officerMillisecond = kEnemyMillisecondOfficerInterval;
            }
        }

        // 警部の更新
        if (controller->inspectorCount < kEnemyCountInspector) {
            controller->inspectorMillisecond -= IocsGetFrameMillisecond();
            if (controller->inspectorMillisecond <= 0) {
                EnemyInspectorLoad();
                ++controller->inspectorCount;
                controller->inspectorMillisecond = kEnemyMillisecondInspectorInterval;
            }
        }

        // 犬の更新
        if (controller->dogCount < kEnemyCountDog) {
            controller->dogMillisecond -= IocsGetFrameMillisecond();
            if (controller->dogMillisecond <= 0) {
                EnemyDogLoad();
                ++controller->dogCount;
                controller->dogMillisecond = kEnemyMillisecondDogInterval;
            }
        }
    }
}

// 警官のカウントを減らす
//
void EnemyControllerDecOfficerCount(void)
{
    struct EnemyController *controller = (struct EnemyController *)ActorFindWithTag(kGameTagEnemyController);
    if (controller != NULL) {
        if (controller->officerCount > 0) {
            --controller->officerCount;
        }
    }
}

// 警部のカウントを減らす
//
void EnemyControllerDecInspectorCount(void)
{
    struct EnemyController *controller = (struct EnemyController *)ActorFindWithTag(kGameTagEnemyController);
    if (controller != NULL) {
        if (controller->inspectorCount > 0) {
            --controller->inspectorCount;
        }
    }
}

// 犬のカウントを減らす
//
void EnemyControllerDecDogCount(void)
{
    struct EnemyController *controller = (struct EnemyController *)ActorFindWithTag(kGameTagEnemyController);
    if (controller != NULL) {
        if (controller->dogCount > 0) {
            --controller->dogCount;
        }
    }
}

// エネミーが移動する
//
void EnemyMove(struct Enemy *enemy)
{
    if (enemy->direction < 0.0f) {
        enemy->speed -= enemy->accel;
        if (enemy->speed < -enemy->speedMaximum) {
            enemy->speed = -enemy->speedMaximum;
        }
    } else if (enemy->direction > 0.0f) {
        enemy->speed += enemy->accel;
        if (enemy->speed > enemy->speedMaximum) {
            enemy->speed = enemy->speedMaximum;
        }
    }
    enemy->position.a = GameAdjustPositionA(enemy->position.a + enemy->speed);
}

// エネミーが停止する
//
bool EnemyStop(struct Enemy *enemy)
{
    if (enemy->speed < 0.0f) {
        enemy->speed += enemy->accel;
        if (enemy->speed > 0.0f) {
            enemy->speed = 0.0f;
        }
    } else if (enemy->speed > 0.0f) {
        enemy->speed -= enemy->accel;
        if (enemy->speed < 0.0f) {
            enemy->speed = 0.0f;
        }
    }
    enemy->position.a = GameAdjustPositionA(enemy->position.a + enemy->speed);
    return enemy->speed == 0.0f ? true : false;
}

// プレイヤを目視したかどうかを判定する
//
bool EnemyIsLookPlayer(struct Enemy *enemy)
{
    float d = PlayerGetDistanceTo(enemy->position.a);
    return (
        (enemy->direction < 0.0f && d < 0.0f && d > -enemy->look) || 
        (enemy->direction > 0.0f && d > 0.0f && d < enemy->look)
    ) ? true : false;
}


