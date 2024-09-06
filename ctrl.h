#ifndef _CONTROL_H
#define _CONTROL_H

#include <datahandler.h>
#include <headfile.h>
#include <imagehandler.h>
#include <mainwindow.h>
#include <QString>

#define ZERO_COEFF 10  // 零点系数

typedef struct {
    uint32_t P;
    uint32_t I;
    uint32_t D;
    uint32_t T;
    uint32_t Coeff;

    float sumError;
    int32_t preError;
    int32_t prePreError;

    int32_t feedBack;
} PIDParam;

typedef enum ControlLine {
    BOTHLINE,
    LEFTLINE,
    RIGHTLINE,
    MIDLINE,
    NONELINE,
} ControlLine;

typedef struct DeviationParam {
    uint16_t k;
    uint16_t b;
    uint16_t annulusK[5];
    uint16_t rampK;
    uint16_t rampB;
    uint16_t parkingK;
    uint16_t parkingB;

    uint16_t forkK;
    uint16_t forkB;
    uint16_t goout_parkK;
    uint16_t goout_parkB;

} DeviationParam;

typedef struct LeastSquareMethodInfo {
    float K;
    float B;
    float DK;
    float lastK;
    float lastB;
    float nowDeviation;
    float lastDeviation;
    float aveDeviation;
    float aveDeviationLeft;
    float aveDeviationRight;
    uint32_t absDeviation[5];
    int32_t diffDeviation[5];
} LeastSquareMethodInfo;

typedef struct DirecctionParam {
    int32_t curvature;  // 曲率
    uint32_t curvatureLimit;
    uint32_t curvatureMin;
    uint32_t curvatureParam;

    int32_t targetYawRate;
    int32_t targetYawRateLimit;

    int32_t dirPWM;
    int32_t speedPWM;

    uint32_t devGain;
} DirectionParam;

class Controller {
   public:
    int16_t annulus_yaw = 0;
    int16_t annulus_yaw_limit = 200;

    int16_t Yaw_Rate_variance = 0;

    EulerAngleTypedef targetAngle;
    EulerAngleTypedef targetAngularVelocity;
    uint8_t angularVelocity_flag;
    uint8_t angle_flag;
    uint8_t velocity_flag;
    PIDParam angularVelocity_PIDParam;
    PIDParam angle_PIDParam;
    PIDParam velocity_PIDParam;

    ControlLine controlLine = BOTHLINE;
    LeastSquareMethodInfo leastSquareMethodInfo;
    DeviationParam deviationParam;
    uint16_t yaw_ramp_k = 10;

    uint8_t mid = 23;

    LCDState lcdState = ADJUST_PARAM;
    RunMode runMode = TIMING_RUN;
    RunSpeedMode runSpeedMode = VARIABLE_SPEED;
    RunState runState = CAR_STOP;
    uint16_t runInfo = (VARIABLE_SPEED << 8) | NORMAL_RUN;
    RunningInfo runningInfo;

    StopReason stopReason;
    uint8_t debugFlag = 0;

    uint16_t parkingDelayTime;
    uint8_t parkingForceStop;

    float kbleft[2] = {0};
    float kbright[2] = {0};
    float kbmid[2] = {0};

    PIDParam directionInternalPIDParam;  // 转向内环
    PIDParam directionOuterPIDParam;     // 转向外环
    DirectionParam directionParam;
    DirectionParam lastDirectionParam;

    int16_t act_dirPWM = 0;
    int16_t Last_YawRate = 0;
    int16_t act_dYRate = 0;
    int32_t act_curvature = 0;
    int32_t act_TarYawrate = 0;
    uint8_t running_brake_flag = 0;
    uint8_t act_speedType = 1;
    uint8_t act_brakeType = 0;

    int32_t act_aimSpeed = 0;

    /* 坡道延迟 */
    int8_t ramp_delay = 0;
    uint8_t ramp_new = 0, ramp_old = 0;
    uint8_t ramp_flag = 0;

    int32_t MPID_AC_P = 0;
    int32_t MPID_UD = 0;
    int32_t MPID_oncei = 0;
    int32_t MPID_ki = 0;

    int32_t act_nowSpeedM = 0;
    int32_t act_nowSpeedL = 0;
    int32_t act_nowSpeedR = 0;

    uint8_t angle_protect_flag = 0;

    uint8_t force_brake_flag = 0;

    void go(uint8_t);
    uint8_t fork_outPark_Detect();
    void PIDParam_init();
    void balanceControl();
    void directionControl();
    int16_t range_protect(int16_t, int16_t, int16_t);
    int16_t cal_IncrementalPID(PIDParam* PIDparam,
                               int16_t reference,
                               int16_t feedBack);
    int16_t cal_RealizePID(PIDParam* PIDparam,
                           int16_t reference,
                           int16_t feedBack);
    int16_t PlacePID_Control(PIDParam* PIDparam,
                             int16_t reference,
                             int16_t feedBack);  // 计算曲率用
    int32_t PID_Realize_Dir(PIDParam* PIDparam,
                            int32_t reference,
                            int32_t feedBack);  // 转向内环
    uint8_t getline(float kb[2], ControlLine cl);
    uint8_t getrampline(float kb[2], ControlLine cl);
    float getAveDeviation(ControlLine cl);
    void motorControl(void);
    void controlParam_init();
};

extern Controller controller;
#endif