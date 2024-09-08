#ifndef SPEED_H
#define SPEED_H

#include <headfile.h>
#include <imagehandler.h>
#include <mainwindow.h>
// 速度信息PID结构体
typedef struct SpeedInfo {
    int16_t varL[3];  // 记录左边编码器的速度值
    int16_t varR[3];  // 记录右边编码器的速度值

    int16_t nowSpeedL;  // 左编码器的当前速度
    int16_t nowSpeedR;  // 右编码器的当前速度
    int16_t nowSpeedM;  // 中间的当前速度
    int16_t aimSpeed;   // 目标速度

    float speedTrueL;  // 左侧真实速度
    float speedTrueR;  // 右侧真实速度
    float speedTrueM;  // 中间真实速度
    uint16_t speedLimitThreshold;
    int16_t speedMin;  // 左右最小速度

    int16_t aimSpeedL;  // 目标速度
    int16_t aimSpeedR;  // 目标速度

    int16_t motorPWML;  // 电机输出PWM值
    int16_t motorPWMR;  // 电机输出PWM值

    uint16_t differential;
    uint16_t encoderThreshold;
} SpeedInfo;

typedef enum SpeedType {
    NORMAL_SHIFT,  // 使用二次函数变速
    FULL_ACCELE,   // 全速
    HALF_ACCELE,
    BRAKE,              // 刹车
    EXIT_CURVE_ACCELE,  // 出弯加速
} SpeedType;

typedef enum BrakeType {
    STRIGHT_BRAKE,
    SHORT_STRIGHT_BRAKE,
    EXIT_ENTER_BRAKE,
} BrakeType;

typedef struct SpeedParam {
    uint16_t addSpeed;  // 加速

    uint16_t maxSpeed;  // 最大速度
    uint16_t max2Speed;
    uint16_t minSpeed;  // 最小速度

    uint16_t curveSpeed;  // 二次公式弯道速度
    uint16_t exitSpeed;
    uint16_t maxExitSpeed;  // 出弯加速的最大速度
    uint16_t exitEnterSpeed;
    uint16_t speedK;
    uint16_t speedK2;
    uint16_t annulusSpeedK2;

    uint16_t constantSpeed;
    uint16_t annulusSpeed;
    uint16_t annulusMinSpeed;
    uint16_t rampUpSpeed;  // 过坡道速度
    uint16_t rampOnSpeed;
    uint16_t rampDownSpeed;
    uint16_t adcSpeed;

    uint16_t strightSpeedCut;
    uint16_t shortStrightSpeedCut;
    uint16_t exitEnterSpeedCut;

    uint16_t parkingSpeed;
} SpeedParam;

typedef struct TrackDectionParam {
    uint16_t strightAD;     // 入直道的判断
    uint16_t enterCurveAD;  // 入弯减速的判断
    uint16_t exitCurveAD;   // 出弯加速的判断
    uint16_t halfAD;

    uint16_t brakeTime;
    uint16_t limitTime;

    uint16_t brakeTest;
    uint16_t brakeTop_1;
    uint16_t brakeTop_2;
    uint16_t brakeTop_3;
    uint16_t brakeTop_4;
    uint16_t brakeTop_0;

    uint16_t brakeSpeedTop_1;
    uint16_t brakeSpeedTop_2;
    uint16_t brakeSpeedTop_3;
    uint16_t brakeSpeedTop_4;
    uint16_t brakeSpeedTop_0;

    uint16_t brakeSpeed_1;
    uint16_t brakeSpeed_2;
    uint16_t brakeSpeed_3;
    uint16_t brakeSpeed_4;

    uint16_t exitEnterTop;
    uint16_t exitEnterSpeedTop;
    uint16_t halfTop;
    uint16_t halfSpeedTop;
} TrackDectionParam;

class SpeedHandler {
   public:
    volatile SpeedInfo speedInfo;
    SpeedParam speedParam;
    SpeedType speedType = FULL_ACCELE;
    BrakeType brakeType = STRIGHT_BRAKE;
    uint8_t brakeTime = 0;
    uint16_t speedUpTime = 0;
    uint16_t halfSpeedUpTime = 0;
    TrackDectionParam trackDectionParam;

    uint8_t limitFlag = 0;
    uint8_t limitTime = 0;

    void speed_init();
    void speedParam_init();
    void SpeedInfo_Init();
    void trackDectionParam_init();
    SpeedType getSpeedType();
    inline uint8_t isBrakeFinished(int16_t aimSpeed);
    int16_t getBrakeSpeed();
    int16_t getAimSpeed();
};

extern SpeedHandler speedhandler;
#endif