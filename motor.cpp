#include "motor.h"
#include "ctrl.h"
#include "imagehandler.h"

extern ImageHandler imagehandler;
extern Controller controller;

void MotorHandler::fork_control() {
    /* 到达三岔区域 */
    if (fork_get != 3 && (SI.varL[0] < 5 || SI.varR[0] < 5)) {
        fork_get++;
    }

    if (fork_get == 3) {
        if (controller.fork_outPark_Detect()) {
            /* 小车位置距离三岔顶较近 通过出界进行判定 */

            //            // 定 pwm倒车
            //            SI.motorPWML = -200;
            //            SI.motorPWMR = -200;

            // 类imu功能 远离三岔顶
            if (SI.varL[0] < 0 && SI.varR[0] < 0)
                sum += (-SI.varL[0] - SI.varR[0]) / 2;

            // 达到设定阈值
            if (sum > 1200)
                IF.wait_fork = 2;
        } else {
            /* 小车位置距离三岔顶距离适中 无需倒车 */
            IF.wait_fork = 2;
        }
    } else {
        //// 未停车前 使用 定pwm刹车 较大可能产生滑动，较小刹停距离较远
        //        SI.motorPWML = -400;
        //        SI.motorPWMR = -400;
    }

    /* 以是否能判定到三岔为条件进行倒车 */
    if (IF.wait_fork == 2) {
        //        SI.motorPWML = -150;
        //        SI.motorPWMR = -150;

        Point fork_point = imagehandler.fork_detect();
        if (fork_point.x != 0 && fork_point.y != 0)
            IF.wait_fork = 3;

        imagehandler.fork_num = 0;
    }

    /* 电机输出 */
    //    if (SI.motorPWML >= 0) // 左电机正转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_LF, (SI.motorPWML * 10));
    //        pwm_duty(MOTOR_PWM_CH_LB, 0);
    //    }
    //    else if (SI.motorPWML < 0) // 左电机反转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_LB, (-SI.motorPWML * 10));
    //        pwm_duty(MOTOR_PWM_CH_LF, 0);
    //    }

    //    if (SI.motorPWMR >= 0) // 右电机正转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_RF, (SI.motorPWMR * 10));
    //        pwm_duty(MOTOR_PWM_CH_RB, 0);
    //    }
    //    else if (SI.motorPWMR < 0) // 右电机反转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_RB, (-SI.motorPWMR * 10));
    //        pwm_duty(MOTOR_PWM_CH_RF, 0);
    //    }
}

void MotorHandler::stop_car(int16_t small_brake,
                            int16_t big_brake,
                            int8_t detect_speed) {
    if (SI.speedTrueM > detect_speed) {
        SI.motorPWML = big_brake - 2 * (SI.nowSpeedL - detect_speed);
        SI.motorPWMR = big_brake - 2 * (SI.nowSpeedR - detect_speed);
    } else {
        SI.motorPWML = small_brake - 2 * (SI.nowSpeedL - detect_speed);
        SI.motorPWMR = small_brake - 2 * (SI.nowSpeedR - detect_speed);
    }

    //    /* 电机输出 */
    //    if (SI.motorPWML >= 0) // 左电机正转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_LF, (SI.motorPWML * 10));
    //        pwm_duty(MOTOR_PWM_CH_LB, 0);
    //    }
    //    else if (SI.motorPWML < 0) // 左电机反转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_LB, (-SI.motorPWML * 10));
    //        pwm_duty(MOTOR_PWM_CH_LF, 0);
    //    }

    //    if (SI.motorPWMR >= 0) // 右电机正转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_RF, (SI.motorPWMR * 10));
    //        pwm_duty(MOTOR_PWM_CH_RB, 0);
    //    }
    //    else if (SI.motorPWMR < 0) // 右电机反转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_RB, (-SI.motorPWMR * 10));
    //        pwm_duty(MOTOR_PWM_CH_RF, 0);
    //    }
}

/* 倒车请注意 */
void MotorHandler::pwm_move(int16_t move_pwm) {
    SI.motorPWML = move_pwm;
    SI.motorPWMR = move_pwm;

    //    /* 电机输出 */
    //    if (SI.motorPWML >= 0) // 左电机正转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_LF, (SI.motorPWML * 10));
    //        pwm_duty(MOTOR_PWM_CH_LB, 0);
    //    }
    //    else if (SI.motorPWML < 0) // 左电机反转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_LB, (-SI.motorPWML * 10));
    //        pwm_duty(MOTOR_PWM_CH_LF, 0);
    //    }

    //    if (SI.motorPWMR >= 0) // 右电机正转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_RF, (SI.motorPWMR * 10));
    //        pwm_duty(MOTOR_PWM_CH_RB, 0);
    //    }
    //    else if (SI.motorPWMR < 0) // 右电机反转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_RB, (-SI.motorPWMR * 10));
    //        pwm_duty(MOTOR_PWM_CH_RF, 0);
    //    }
}

void MotorHandler::tag_control() {
    int16_t static tag_sum = 0;

    if (IF.tag == 1) {
        //        SI.motorPWML = -400;
        //        SI.motorPWMR = -400;

        if (SI.varL[0] < 0 && SI.varR[0] < 0)
            tag_sum += (-SI.varL[0] - SI.varR[0]) / 2;

        if (tag_sum > 600)
            IF.tag = 2;
    } else if (IF.tag == 2) {
        //        SI.motorPWML = 150;
        //        SI.motorPWMR = 150;

        if (SI.varL[0] > 0 && SI.varR[0] > 0)
            tag_sum += (SI.varL[0] + SI.varR[0]) / 2;

        if (tag_sum > 3000)
            IF.tag = 3;
    }

    /* 电机输出 */
    //    if (SI.motorPWML >= 0) // 左电机正转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_LF, (SI.motorPWML * 10));
    //        pwm_duty(MOTOR_PWM_CH_LB, 0);
    //    }
    //    else if (SI.motorPWML < 0) // 左电机反转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_LB, (-SI.motorPWML * 10));
    //        pwm_duty(MOTOR_PWM_CH_LF, 0);
    //    }

    //    if (SI.motorPWMR >= 0) // 右电机正转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_RF, (SI.motorPWMR * 10));
    //        pwm_duty(MOTOR_PWM_CH_RB, 0);
    //    }
    //    else if (SI.motorPWMR < 0) // 右电机反转
    //    {
    //        pwm_duty(MOTOR_PWM_CH_RB, (-SI.motorPWMR * 10));
    //        pwm_duty(MOTOR_PWM_CH_RF, 0);
    //    }
}

int32_t MotorHandler::amplitudeLimit(int32_t duty, int32_t min, int32_t max) {
    if (duty >= max) {
        return max;
    }
    if (duty <= min) {
        return min;
    } else {
        return duty;
    }
}

/************************************电机停止**********************************/
void MotorHandler::stopMotor() {
#ifndef T_REX_QT_DEBUG
    DISABLE_MOTOR;
    pwm_duty(MOTOR_PWM_CH0, 0);
    pwm_duty(MOTOR_PWM_CH1, 0);
    pwm_duty(MOTOR_PWM_CH2, 0);
    pwm_duty(MOTOR_PWM_CH3, 0);
#endif
}

/*******************************************************************************
 *     函数功能：获取左右侧编码器值
 *     返回值：编码器在一个控制周期内的脉冲值
 *******************************************************************************/
void MotorHandler::getEncoder(void) {
#ifndef T_REX_QT_DEBUG
    SI.varL[2] = SI.varL[1];
    SI.varL[1] = SI.varL[0];
    SI.nowSpeedL = gpt12_get(QUAD_MODULD1);
    gpt12_clear(QUAD_MODULD1);

    SI.varR[2] = SI.varR[1];
    SI.varR[1] = SI.varR[0];
    SI.nowSpeedR = -gpt12_get(QUAD_MODULD0);
    gpt12_clear(QUAD_MODULD0);

    SI.nowSpeedM = (SI.nowSpeedL + SI.nowSpeedR) / 2;

    float speedLimit;
    speedLimit = SI.encoderThreshold / 10.0f;

    if (SI.varL[1] > SI.nowSpeedL + speedLimit && runState != CAR_BRAKING) {
        SI.varL[0] = SI.varL[1] - (int16_t)speedLimit;
    } else {
        SI.varL[0] = (SI.nowSpeedL + SI.varL[1] + SI.varL[2]) / 3;
    }

    if (SI.varR[1] > SI.nowSpeedR + speedLimit && runState != CAR_BRAKING) {
        SI.varR[0] = SI.varR[1] - (int16_t)speedLimit;
    } else {
        SI.varR[0] = (SI.nowSpeedR + SI.varR[1] + SI.varR[2]) / 3;
    }
#endif
}

/******************************************************************************
 * FunctionName   : getTrueSpeed()
 * Description    : 获取真实速度
 * EntryParameter : None
 * ReturnValue    : None
 *******************************************************************************/
void MotorHandler::getTrueSpeed() {
    float speed = 0;

    if (imagehandler.SystemAttitudeRate.Yaw > 30) {
        //    	speed = (SI.nowSpeedR + TRACK / 2 *
        //    imagehandler.SystemAttitudeRate.Yaw * PI / 180 * SPD_CONVERT );
        speed =
            (SI.nowSpeedL - TRACK / 2 * imagehandler.SystemAttitudeRate.Yaw *
                                PI / 180 * SPD_CONVERT);  // 右转，用左边
    } else if (imagehandler.SystemAttitudeRate.Yaw < -30) {
        //    	speed = (SI.nowSpeedL - TRACK / 2 *
        //    imagehandler.SystemAttitudeRate.Yaw * PI / 180 * SPD_CONVERT );
        speed =
            (SI.nowSpeedR + TRACK / 2 * imagehandler.SystemAttitudeRate.Yaw *
                                PI / 180 * SPD_CONVERT);  // 左转，用右边
    } else {
        speed = SI.nowSpeedM;  // 不怎么转，用两边
    }

    // 变速过大时，变速限幅
    if ((speed - SI.speedTrueM) > (SI.speedLimitThreshold / 100.0)) {
        SI.speedTrueM += (SI.speedLimitThreshold / 100.0);
    } else if ((speed - SI.speedTrueM) <
               ((-1.0) * (SI.speedLimitThreshold / 100.0))) {
        SI.speedTrueM -= (SI.speedLimitThreshold / 100.0);
    } else {
        SI.speedTrueM = speed;
    }

    if (SI.speedTrueM > 400) {
        SI.speedTrueM = 400;
    }

    SI.speedTrueL = SI.speedTrueM + TRACK / 2 *
                                        imagehandler.SystemAttitudeRate.Yaw *
                                        PI / 180 * SPD_CONVERT;
    SI.speedTrueR = SI.speedTrueM - TRACK / 2 *
                                        imagehandler.SystemAttitudeRate.Yaw *
                                        PI / 180 * SPD_CONVERT;
}

/*********************************增量式PID算法*********************************
 *
 *     函数功能：增量式PID计算电机输出PWM值
 *     参数：结构体 PIDM 指针
 *     返回值：电机PWM值
 *******************************************************************************/
int16_t MotorHandler::calc_vPID(volatile IncrementalPID* pp) {
    int16_t error;
    float duty = 0.0;
    float once_i;
    float kp_t, ki_index, kd_t;

    kp_t = (float)((kp) / 10.0);  // 取出P I D系数
    kd_t = (float)((kd) / 10.0);
    error = pp->REF - pp->feedBack;
    if (error + pp->preError >= 0) {
        ki_index = (change_ki / 10.0) -
                   (change_ki / 10.0) /
                       (1 + exp(change_kib - 0.2 * abs(error)));  // 变积分控制
    } else {
        ki_index =
            ki / 10.0;  // 0.9;//ki - ki / (1 + exp(kib - 0.2*abs(error)));
    }
    pp->U_D = kd_t * (error - pp->preError) * 0.5 + 0.5 * pp->U_D;
    once_i = 0.5 * ki_index * (error + pp->preError);
    duty = pp->AC_P + kp_t * error + pp->U_D;

    if (duty > MOTOR_PWM_MIN && duty < MOTOR_PWM_MAX) {
        duty += once_i;
        if (duty > MOTOR_PWM_MAX) {
            float temp;
            temp = duty - MOTOR_PWM_MAX;
            once_i -= temp;
            duty = MOTOR_PWM_MAX;
        } else if (duty < MOTOR_PWM_MIN) {
            float temp;
            temp = duty - MOTOR_PWM_MIN;
            once_i -= temp;
            duty = MOTOR_PWM_MIN;
        }
        pp->AC_P += once_i;
    } else if ((duty >= MOTOR_PWM_MAX && once_i < 0) ||
               (duty <= MOTOR_PWM_MIN && once_i > 0)) {
        pp->AC_P += once_i;
        duty += once_i;
    }

    pp->prePreError = pp->preError;
    pp->preError = error;

    if (duty > MOTOR_PWM_MAX) {
        duty = MOTOR_PWM_MAX;
    } else if (duty < MOTOR_PWM_MIN) {
        duty = MOTOR_PWM_MIN;
    }
#ifndef T_REX_QT_DEBUG
    if (debugFlag == 0) {
        if (runState == CAR_BRAKING) {
            if (duty < -500) {
                duty = -500;
            }
        }
    }
#endif
    pp->PID_Out = (int16_t)(duty);
    return (pp->PID_Out);
}

/***********************************电机控制************************************
 *
 *     函数功能：PWM控制电机转速
 *     参数：设定的目标速度（编码器值）
 *     返回值：void
 *******************************************************************************/
void MotorHandler::setMotorPWM(int16_t motorPWML, int16_t motorPWMR) {
#ifndef T_REX_QT_DEBUG
    // 正转
    if (motorPWML >= 0) {
        motorPWML = amplitudeLimit(motorPWML, 0, MOTOR_PWM_MAX);
        pwm_duty(MOTOR_PWM_CH0, (motorPWML));
        pwm_duty(MOTOR_PWM_CH1, 0);
    }
    // 反转
    else if (motorPWML < 0) {
        motorPWML = amplitudeLimit(motorPWML, MOTOR_PWM_MIN, 0);
        pwm_duty(MOTOR_PWM_CH1, (-motorPWML));
        pwm_duty(MOTOR_PWM_CH0, 0);
    }

    if (motorPWMR >= 0) {
        motorPWMR = amplitudeLimit(motorPWMR, 0, MOTOR_PWM_MAX);
        pwm_duty(MOTOR_PWM_CH2, (motorPWMR));
        pwm_duty(MOTOR_PWM_CH3, 0);
    }
    // 正转
    else if (motorPWMR < 0) {
        motorPWMR = amplitudeLimit(motorPWMR, MOTOR_PWM_MIN, 0);
        pwm_duty(MOTOR_PWM_CH3, (-motorPWMR));
        pwm_duty(MOTOR_PWM_CH2, 0);
    }
#endif
}

void MotorHandler::motor_init() {
    motorPID_init();

#ifndef T_REX_QT_DEBUG
    gpio_init(MOTOR_EN, GPO, 0, PUSHPULL);
    // 配置编码器
    gpt12_init(QUAD_MODULD0, QUAD_MODULD0_PHASEA, QUAD_MODULD0_PHASEB);
    gpt12_init(QUAD_MODULD1, QUAD_MODULD1_PHASEA, QUAD_MODULD1_PHASEB);

    gpt12_clear(QUAD_MODULD0);
    gpt12_clear(QUAD_MODULD1);
    // 电机PWM初始化
    gtm_pwm_init(MOTOR_PWM_CH0, MOTOR_HZ, 0);
    gtm_pwm_init(MOTOR_PWM_CH1, MOTOR_HZ, 0);
    gtm_pwm_init(MOTOR_PWM_CH2, MOTOR_HZ, 0);
    gtm_pwm_init(MOTOR_PWM_CH3, MOTOR_HZ, 0);

    ENABLE_MOTOR;
#endif
}

void MotorHandler::motorPID_init() {
    MPIDL.REF = 0;  // 目标速度
    MPIDL.feedBack = 0;
    MPIDL.preError = 0;
    MPIDL.prePreError = 0;
    MPIDL.PID_Out = 0;
    MPIDL.U_D = 0;
    MPIDL.AC_P = 0;
    MPIDL.turnInRem = 0;

    MPIDR.REF = 0;       // 目标速度
    MPIDR.feedBack = 0;  // 当前速度
    MPIDR.preError = 0;
    MPIDR.prePreError = 0;
    MPIDR.PID_Out = 0;
    MPIDR.U_D = 0;
    MPIDR.AC_P = 0;
    MPIDR.turnInRem = 0;
}
