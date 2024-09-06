#ifndef _MOTOR_H
#define _MOTOR_H

#include "ctrl.h"
#include "headfile.h"
#include "imagehandler.h"
#include "speed.h"

#define MOTOR_HZ 15 * 1000

#define MOTOR_EN P22_3
#define MOTOR_PWM_CH0 ATOM1_CH1_P21_3  // youf ATOM1_CH1_P21_3
#define MOTOR_PWM_CH1 ATOM1_CH3_P21_5  // zuoz ATOM1_CH3_P21_5
#define MOTOR_PWM_CH2 ATOM1_CH2_P21_4  // ATOM1_CH2_P21_4//zuof ATOM1_CH2_P21_4
#define MOTOR_PWM_CH3 ATOM1_CH0_P21_2  // ATOM1_CH3_P21_5//youz ATOM1_CH0_P21_2
#define ENABLE_MOTOR gpio_set(MOTOR_EN, 1);
#define DISABLE_MOTOR gpio_set(MOTOR_EN, 0);

#define MOTOR_PWM_MAX 1000
#define MOTOR_PWM_MIN -1000

#define QUAD_MODULD0 GPT12_T5
#define QUAD_MODULD1 GPT12_T2
#define QUAD_MODULD0_PHASEA GPT12_T5INB_P10_3
#define QUAD_MODULD0_PHASEB GPT12_T5EUDB_P10_1
#define QUAD_MODULD1_PHASEA GPT12_T2INB_P33_7
#define QUAD_MODULD1_PHASEB GPT12_T2EUDB_P33_6

#define MPIDL motorPIDLeft
#define MPIDR motorPIDRight
#define MPIDM motorPIDMiddle

// 电机控制PID结构体
typedef struct IncrementalPID {
    int16_t REF;
    int16_t feedBack;

    int16_t preError;
    int16_t prePreError;

    int16_t PID_Out;
    float U_D;
    float AC_P;
    uint8_t turnInRem;
} IncrementalPID;

class MotorHandler {
   public:
    IncrementalPID motorPIDLeft;   // 电机控制PID结构体
    IncrementalPID motorPIDRight;  // 电机控制PID结构体
    IncrementalPID motorPIDMiddle;

    // 速度环参数
    uint16_t kp = 70;         // 180;
    uint16_t ki = 10;         // 9;
    uint16_t kd = 40;         // 150;
    uint16_t change_ki = 15;  // 10;
    uint16_t change_kib = 4;
    uint16_t turnILimitSub = 40;

    uint8_t fork_get = 0;
    int16_t sum = 0;

    int32_t amplitudeLimit(int32_t duty, int32_t min, int32_t max);
    void motorPID_init(void);
    void motor_init(void);
    void setMotorPWM(int16_t MOTORLPWM, int16_t MOTORRPWM);
    void stopMotor(void);
    void getEncoder(void);
    void getTrueSpeed(void);
    int16_t calc_vPID(volatile IncrementalPID* pp);
    void fork_control();
    void tag_control();
    void stop_car(int16_t small_brake, int16_t big_brake, int8_t detect_speed);
    void pwm_move(int16_t move_pwm);
};

MotorHandler motorhandler;

#endif
