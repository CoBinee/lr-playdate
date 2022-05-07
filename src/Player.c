// Player.c - プレイヤ
//

// 外部参照
//
#include <stdbool.h>
#include <math.h>
#include "pd_api.h"
#include "Iocs.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"

// 内部関数
//
static void PlayerUnload(struct Player *player);
static void PlayerDraw(struct Player *player);
static void PlayerWalk(struct Player *player);
static void PlayerJump(struct Player *player);
static void PlayerTurn(struct Player *player);
static void PlayerHop(struct Player *player);
static void PlayerMiss(struct Player *player);
static void PlayerDone(struct Player *player);
static void PlayerMoveToTarget(struct Player *player);

// 内部変数
//


// プレイヤを初期化する
//
void PlayerInitialize(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの確認
    if (sizeof (struct Player) > kActorBlockSize) {
        playdate->system->error("%s: %d: player actor size is over: %d bytes.", __FILE__, __LINE__, sizeof (struct Player));
    }
}

// プレイヤを読み込む
//
void PlayerLoad(void)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // アクタの登録
    struct Player *player = (struct Player *)ActorLoad((ActorFunction)PlayerWalk, kGamePriorityPlayer);
    if (player == NULL) {
        playdate->system->error("%s: %d: player actor is not loaded.", __FILE__, __LINE__);
    }

    // プレイヤの初期化
    {
        // 解放処理の設定
        ActorSetUnload(&player->actor, (ActorFunction)PlayerUnload);

        // タグの設定
        ActorSetTag(&player->actor, kGameTagPlayer);

        // 位置の設定
        player->position.a = 180.0f;
        player->position.r = GameGetR();
        player->position.w = 16.0f;
        player->position.h = 24.0f;

        // 目標の設定
        player->target = player->position;

        // 向きの設定
        player->direction = 1.0f;

        // 回転の設定
        player->rotation = player->position.a;

        // 速度の設定
        player->speed = 0.0f;
        player->speedMaximum = (1.5f * (1000 / 30)) / IocsGetFrameMillisecond();
        player->accel = player->speedMaximum / 10.0f;

        // ジャンプの設定
        player->jump = 0.0f;
        player->jumpMaximum = (8.0f * (1000 / 30)) / IocsGetFrameMillisecond();
        player->gravity = player->jumpMaximum / (12.0f * (1000 / 30)) * IocsGetFrameMillisecond();

        // 一周の設定
        player->roundDistance = 0.0f;
        player->roundCount = 0;

        // プレイ中の設定
        player->play = true;
    }
}

// プレイヤを解放する
//
static void PlayerUnload(struct Player *player)
{
    ;
}

// プレイヤを描画する
//
static void PlayerDraw(struct Player *player)
{
    // スプライトの描画
    if (player->play) {
        AsepriteDrawRotatedSpriteAnimation(&player->animations[kPlayerSpriteTarget], player->target.x, player->target.y, player->target.a - 180.0f, 0.5f, 1.0f, 1.0f, 1.0f, kDrawModeCopy);
    }
    AsepriteDrawRotatedSpriteAnimation(&player->animations[kPlayerSpriteThief], player->position.x, player->position.y, player->rotation - 180.0f, 0.5f, 1.0f, player->direction, 1.0f, kDrawModeCopy);
}

// プレイヤが歩く
//
static void PlayerWalk(struct Player *player)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (player->actor.state == 0) {

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteThief], GameGetSpriteName(kGameSpriteNameThief), "Walk", true);
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteTarget], GameGetSpriteName(kGameSpriteNameThief), "Target", true);

        // 初期化の完了
        ++player->actor.state;
    }

    // 移動
    PlayerMoveToTarget(player);

    // 回転の更新
    player->rotation = player->position.a;

    // 位置の計算
    GameCalcPosition(&player->position);

    // 時間切れ
    if (GameIsTimeout()) {
        player->play = false;
        ActorTransition(&player->actor, (ActorFunction)PlayerHop);

    // ジャンプ
    } else if (IocsIsButtonEdge(kButtonUp)) {
        ActorTransition(&player->actor, (ActorFunction)PlayerJump);

    // ターン
    } else if (IocsIsButtonEdge(kButtonDown)) {
        ActorTransition(&player->actor, (ActorFunction)PlayerTurn);
    }

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteThief]);
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteTarget]);

    // 描画処理の設定
    ActorSetDraw(&player->actor, (ActorFunction)PlayerDraw, kGameOrderPlayer);
}

// プレイヤがジャンプする
//
static void PlayerJump(struct Player *player)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (player->actor.state == 0) {

        // ジャンプの設定
        player->jump = -player->jumpMaximum;

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteThief], GameGetSpriteName(kGameSpriteNameThief), "Jump", true);
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteTarget], GameGetSpriteName(kGameSpriteNameThief), "Target", true);

        // オーディオの再生
        IocsPlayAudioEffect(kGameAudioSampleJump, 1);

        // 初期化の完了
        ++player->actor.state;
    }

    // 移動
    PlayerMoveToTarget(player);

    // ジャンプ
    {
        player->jump = player->jump + player->gravity;
        player->position.r = player->position.r + player->jump;
        float r = GameGetR();
        if (player->position.r >= r) {
            player->position.r = r;
            ActorTransition(&player->actor, (ActorFunction)PlayerWalk);
        }
    }

    // 回転の更新
    player->rotation = player->position.a;

    // 位置の計算
    GameCalcPosition(&player->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteThief]);
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteTarget]);

    // 描画処理の設定
    ActorSetDraw(&player->actor, (ActorFunction)PlayerDraw, kGameOrderPlayer);
}

// プレイヤが反対側にターンする
//
static void PlayerTurn(struct Player *player)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (player->actor.state == 0) {

        // ジャンプの設定
        player->jump = -player->jumpMaximum;

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteThief], GameGetSpriteName(kGameSpriteNameThief), "Jump", true);
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteTarget], GameGetSpriteName(kGameSpriteNameThief), "Target", true);

        // オーディオの再生
        IocsPlayAudioEffect(kGameAudioSampleJump, 1);

        // 初期化の完了
        ++player->actor.state;
    }

    // ジャンプ
    if (player->jump < 0.0f) {
        player->jump = player->jump + player->gravity;
        player->position.r = player->position.r + player->jump;
        if (player->jump >= 0.0f) {
            player->position.r = -player->position.r;
            player->position.a = GameAdjustPositionA(player->position.a + 180.0f);
        }

    // ターン
    } else {
        player->jump = player->jump + player->gravity * 3.0f;
        player->position.r = player->position.r + player->jump;
        float r = GameGetR();
        if (player->position.r >= r) {
            player->position.r = r;
            ActorTransition(&player->actor, (ActorFunction)PlayerWalk);
        }
    }

    // 回転
    {
        float d = player->rotation - player->position.a;
        float t = 30.0f;
        if (d < -t) {
            d = -t;
        } else if (d > t) {
            d = t;
        }
        player->rotation = GameAdjustPositionA(player->rotation - d);
    }

    // 位置の計算
    GameCalcPosition(&player->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteThief]);
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteTarget]);

    // 描画処理の設定
    ActorSetDraw(&player->actor, (ActorFunction)PlayerDraw, kGameOrderPlayer);
}

// プレイヤが飛び跳ねる
//
static void PlayerHop(struct Player *player)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (player->actor.state == 0) {

        // ジャンプの設定
        player->jump = -player->jumpMaximum;

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteThief], GameGetSpriteName(kGameSpriteNameThief), "Jump", true);
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteTarget], GameGetSpriteName(kGameSpriteNameThief), "Target", true);

        // オーディオの再生
        IocsPlayAudioEffect(kGameAudioSampleJump, 1);

        // 初期化の完了
        ++player->actor.state;
    }

    // ジャンプ
    {
        player->jump = player->jump + player->gravity;
        player->position.r = player->position.r + player->jump;
        float r = GameGetR();
        if (player->position.r >= r) {
            player->position.r = r;
            player->jump = -player->jumpMaximum;
            IocsPlayAudioEffect(kGameAudioSampleJump, 1);
        }
    }

    // 回転の更新
    player->rotation = player->position.a;

    // 位置の計算
    GameCalcPosition(&player->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteThief]);
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteTarget]);

    // 描画処理の設定
    ActorSetDraw(&player->actor, (ActorFunction)PlayerDraw, kGameOrderPlayer);
}

// プレイヤがミスする
//
static void PlayerMiss(struct Player *player)
{
    // Playdate の取得
    PlaydateAPI *playdate = IocsGetPlaydate();
    if (playdate == NULL) {
        return;
    }

    // 初期化
    if (player->actor.state == 0) {

        // ジャンプの設定
        player->jump = -player->jumpMaximum;

        // アニメーションの開始
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteThief], GameGetSpriteName(kGameSpriteNameThief), "Jump", true);
        AsepriteStartSpriteAnimation(&player->animations[kPlayerSpriteTarget], GameGetSpriteName(kGameSpriteNameThief), "Target", true);

        // オーディオの再生
        IocsPlayAudioSystem(kIocsAudioSystemSamplePo, 1);

        // 初期化の完了
        ++player->actor.state;
    }

    // ジャンプ
    {
        player->jump = player->jump + player->gravity;
        player->position.r = player->position.r + player->jump;
        float r = GameGetR();
        if (player->position.r > r + player->position.h) {
            ActorTransition(&player->actor, (ActorFunction)PlayerDone);
        }
    }

    // 回転の更新
    player->rotation = player->position.a;

    // 位置の計算
    GameCalcPosition(&player->position);

    // スプライトの更新
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteThief]);
    AsepriteUpdateSpriteAnimation(&player->animations[kPlayerSpriteTarget]);

    // 描画処理の設定
    ActorSetDraw(&player->actor, (ActorFunction)PlayerDraw, kGameOrderPlayer);
}

// プレイヤが完了する
//
static void PlayerDone(struct Player *player)
{
    ActorUnload(&player->actor);
}

// プレイヤが目標に向かって移動する
//
static void PlayerMoveToTarget(struct Player *player)
{
    // 目標の更新
    player->target.a = IocsGetCrankAngle();
    // player->target.a = GameAdjustPositionA(player->position.a + 90.0f);
    GameCalcPosition(&player->target);

    // 位置の更新
    {
        float d = GameGetDistance(player->target.a, player->position.a);
        if (d < 0.0f) {
            player->speed = player->speed + player->accel;
            if (player->speed > player->speedMaximum) {
                player->speed = player->speedMaximum;
            }
            if (player->direction <= 0.0f) {
                player->direction = 1.0f;
            }
        } else if (d > 0.0f) {
            player->speed = player->speed - player->accel;
            if (player->speed < -player->speedMaximum) {
                player->speed = -player->speedMaximum;
            }
            if (player->direction >= 0.0f) {
                player->direction = -1.0f;
            }
        }
        player->position.a = GameAdjustPositionA(player->position.a + player->speed);
    }

    // 一周の更新
    {
        player->roundDistance += player->speed;
        if (player->roundDistance <= -360.0f) {
            ++player->roundCount;
            player->roundDistance += 360.0f;
        } else if (player->roundDistance >= 360.0f) {
            ++player->roundCount;
            player->roundDistance -= 360.0f;
        }
    }
}

// プレイヤが存在しているかどうかを判定する
//
bool PlayerIsLive(void)
{
    struct Player *player = (struct Player *)ActorFindWithTag(kGameTagPlayer);
    return player != NULL ? true : false;
}

// プレイヤの位置を取得する
//
float PlayerGetPositionA(void)
{
    struct Player *player = (struct Player *)ActorFindWithTag(kGameTagPlayer);
    return player != NULL ? player->position.a : -1.0f;
}

// プレイヤとの距離を測る
//
float PlayerGetDistanceTo(float a)
{
    struct Player *player = (struct Player *)ActorFindWithTag(kGameTagPlayer);
    return player != NULL ? GameGetDistance(a, player->position.a) : 360.0f;
}

// プレイヤとヒットしたかどうかを判定する
//
bool PlayerIsHit(struct GamePosition *position)
{
    // プレイヤの取得
    struct Player *player = (struct Player *)ActorFindWithTag(kGameTagPlayer);
    if (player == NULL) {
        return false;
    }

    // 距離の計測
    bool result = false;
    if (
        player->play && 
        GameIsPlay() && 
        fabsf(GameGetDistance(player->position.a, position->a)) <= player->position.w / 2.0f && 
        player->position.r - player->position.h <= position->r && 
        player->position.r >= position->r - position->h
    ) {
        result = true;
    }

    // 終了
    return result;
}
bool PlayerIsHitThenMiss(struct GamePosition *position)
{
    // プレイヤの取得
    struct Player *player = (struct Player *)ActorFindWithTag(kGameTagPlayer);
    if (player == NULL) {
        return false;
    }

    // ヒットの判定
    bool result = PlayerIsHit(position);
    if (result) {
        player->play = false;
        ActorTransition(&player->actor, (ActorFunction)PlayerMiss);
    }

    // 終了
    return result;
}

// 一周のカウントをクリアする
//
void PlayerClearRoundCount(void)
{
    struct Player *player = (struct Player *)ActorFindWithTag(kGameTagPlayer);
    if (player != NULL) {
        player->roundCount = 0;
    }
}

// 何周したかを取得する
//
int PlayerGetRoundCount(void)
{
    struct Player *player = (struct Player *)ActorFindWithTag(kGameTagPlayer);
    return player != NULL ? player->roundCount : 0;
}
