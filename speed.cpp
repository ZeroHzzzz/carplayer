#include "speed.h"

SpeedHandler speedhandler;

void SpeedHandler::speed_init() {
    speedParam_init();
    SpeedInfo_Init();
    trackDectionParam_init();
}

void SpeedHandler::speedParam_init() {
    SP.addSpeed = 1;  // 加速

    SP.maxSpeed = 270;  // 直道最大速度
    SP.max2Speed = 230;
    SP.minSpeed = 200;

    SP.curveSpeed = 250;  // 二次公式弯道速度
    SP.exitSpeed = 240;
    SP.maxExitSpeed = 270;  // 出弯加速的最大速度
    SP.exitEnterSpeed = 220;

    SP.rampUpSpeed = 220;  // 过坡道速度
    SP.rampOnSpeed = 180;
    SP.rampDownSpeed = 180;
    SP.adcSpeed = 200;
    SP.annulusSpeed = 250;
    SP.annulusMinSpeed = 200;
    SP.constantSpeed = 200;

    SP.speedK = 110;  // 二次公式系数
    SP.speedK2 = 130;

    SP.annulusSpeedK2 = 125;
    SP.strightSpeedCut = 10;
    SP.shortStrightSpeedCut = 10;
    SP.exitEnterSpeedCut = 10;

    SP.parkingSpeed = 180;
}

void SpeedHandler::trackDectionParam_init() {
    TDP.strightAD = 20;     // 入直道的判断
    TDP.enterCurveAD = 12;  // 入弯减速的判断
    TDP.exitCurveAD = 25;   // 出弯加速的判断
    TDP.halfAD = 20;
    // average deviation

    TDP.brakeTime = 12;
    TDP.limitTime = 20;

    TDP.brakeTest = 0;
    TDP.brakeTop_1 = 56;
    TDP.brakeTop_2 = 55;
    TDP.brakeTop_3 = 53;
    TDP.brakeTop_4 = 52;
    TDP.brakeTop_0 = 50;  // 短直到刹车

    TDP.brakeSpeedTop_1 = 55;
    TDP.brakeSpeedTop_2 = 54;
    TDP.brakeSpeedTop_3 = 52;
    TDP.brakeSpeedTop_4 = 50;
    TDP.brakeSpeedTop_0 = 48;

    TDP.brakeSpeed_1 = 270;
    TDP.brakeSpeed_2 = 260;
    TDP.brakeSpeed_3 = 245;
    TDP.brakeSpeed_4 = 230;

    TDP.exitEnterTop = 55;
    TDP.exitEnterSpeedTop = 53;
    TDP.halfTop = 56;
    TDP.halfSpeedTop = 54;
}

void SpeedHandler::SpeedInfo_Init() {
    SI.varL[0] = 0;
    SI.varL[1] = 0;
    SI.varL[2] = 0;

    SI.varR[0] = 0;
    SI.varR[1] = 0;
    SI.varR[2] = 0;

    SI.nowSpeedL = 0;
    SI.nowSpeedR = 0;
    SI.nowSpeedM = 0;
    SI.aimSpeedL = 0;
    SI.aimSpeedR = 0;
    SI.aimSpeed = 0;

    SI.speedTrueL = 0.0f;
    SI.speedTrueR = 0.0f;
    SI.speedTrueM = 0.0f;
    SI.speedLimitThreshold = 300;
    SI.speedMin = 0;

    SI.motorPWML = 0;
    SI.motorPWMR = 0;
    SI.differential = 42;
    SI.encoderThreshold = 70;
}

int16_t SpeedHandler::getAimSpeed() {
    int16_t aimSpeed;
    SpeedType _speedType;

    if (controller.runSpeedMode == CONSTANT_SPEED) {
        if (IF.ramp) {
            if (IF.ramp == 1) {
                aimSpeed = SP.rampUpSpeed;
            } else if (IF.ramp == 2) {
                aimSpeed = SP.rampOnSpeed;
            } else if (IF.ramp == 3) {
                aimSpeed = SP.rampDownSpeed;
            }
            return aimSpeed;
        }
        return SP.constantSpeed;
    } else if (controller.runSpeedMode == NORMAL_SHIFT_SPEED) {
        _speedType = NORMAL_SHIFT;
    } else if (controller.runSpeedMode == VARIABLE_SPEED) {
        _speedType = getSpeedType();
    }

    _speedType = getSpeedType();

    switch (_speedType) {
        case NORMAL_SHIFT: {
            uint16_t referenceSpeed;
            uint16_t minSpeed;
            uint16_t speedK2;
            if (IF.annulus) {
                referenceSpeed = SP.annulusSpeed;
                minSpeed = SP.annulusMinSpeed;
                speedK2 = SP.annulusSpeedK2;
            } else {
                referenceSpeed = SP.curveSpeed;
                minSpeed = SP.minSpeed;
                speedK2 = SP.speedK2;
            }
            aimSpeed = (int16_t)(referenceSpeed - (referenceSpeed - minSpeed) *
                                                      LSMI.DK * LSMI.DK /
                                                      speedK2 / speedK2);
            // 二次公式速度
            if (IF.annulus == AL1 || IF.annulus == AR1) {
                aimSpeed = SP.annulusMinSpeed;
            }
            /*速度限幅*/
            if (aimSpeed < minSpeed) {
                aimSpeed = minSpeed;
            }
            if (aimSpeed > SP.max2Speed) {
                aimSpeed = SP.max2Speed;
            }
            break;
        }
        case FULL_ACCELE: {
            aimSpeed = SP.maxSpeed;
            break;
        }
        case BRAKE: {
            aimSpeed = getBrakeSpeed();
            if (isBrakeFinished(aimSpeed)) {
                ++brakeTime;
            }
            if (brakeTime >= TDP.brakeTime) {
                brakeTime = 0;
                speedType = NORMAL_SHIFT;
            }
            break;
        }
        case EXIT_CURVE_ACCELE: {
            aimSpeed = (SI.aimSpeed + SP.addSpeed) < SP.exitSpeed
                           ? (SI.aimSpeed + SP.addSpeed)
                           : SP.exitSpeed;
            break;
        }
        case HALF_ACCELE: {
            aimSpeed = SP.maxExitSpeed;
            break;
        }
        default:
            break;
    }

    if (IF.ramp) {
        if (IF.ramp == 1) {
            aimSpeed = SP.rampUpSpeed;
        } else if (IF.ramp == 2) {
            aimSpeed = SP.rampOnSpeed;
        } else if (IF.ramp == 3) {
            aimSpeed = SP.rampDownSpeed;
        }
    }
    if (IF.annulusDelay) {
        float DK = imagehandler.AD.sumDK / imagehandler.AD.cnt;
        int16_t annulusSpeed =
            (int16_t)(SP.annulusSpeed - (SP.annulusSpeed - SP.annulusMinSpeed) *
                                            DK * DK / SP.annulusSpeedK2 /
                                            SP.annulusSpeedK2);
        annulusSpeed = annulusSpeed > SP.annulusMinSpeed ? annulusSpeed
                                                         : SP.annulusMinSpeed;
        aimSpeed = aimSpeed < annulusSpeed ? aimSpeed : annulusSpeed;
    }

    int16_t maxSpeed = SP.maxSpeed;

    switch (TDP.brakeTest) {
        case 1:
            maxSpeed = TDP.brakeSpeed_1;
            break;
        case 2:
            maxSpeed = TDP.brakeSpeed_2;
            break;
        case 3:
            maxSpeed = TDP.brakeSpeed_3;
            break;
        case 4:
            maxSpeed = TDP.brakeSpeed_4;
            break;
        default:
            break;
    }

    if (aimSpeed > maxSpeed) {
        aimSpeed = maxSpeed;
    }
    return aimSpeed;
}

SpeedType SpeedHandler::getSpeedType() {
    /************************** 进行速度类型判断 ******************************/
    int16_t maxSpeed = SI.varL[0] > SI.varR[0] ? SI.varL[0] : SI.varR[0];
    if (IF.annulus) {
        speedType = NORMAL_SHIFT;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        if (IF.annulus && imagehandler.LAST_IF.annulus == 0) {
            limitFlag = 1;
        }
    } else if (IF.ramp) {
        if (1) {
            speedType = NORMAL_SHIFT;
        } else {
            speedType = EXIT_CURVE_ACCELE;
        }
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        if (IF.ramp && imagehandler.LAST_IF.ramp == 0) {
            limitFlag = 1;
        }
    } else if (II.annulusTop <= YY && speedType == NORMAL_SHIFT) {
        speedType = NORMAL_SHIFT;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
    }
    /***********************************直道加速*******************************/
    // 加速条件满足  //长直道加速
    else if (II.top >= YY && II.speedTop >= YM && II.annulusTop >= YM &&
             LSMI.absDeviation[0] <= TDP.strightAD && speedType != BRAKE) {
        ++speedUpTime;  // 长直道加速条件，满足一定次数才进行长直道加速
    }
    /***********************************直道加速*******************************/

    /***********************************直道刹车*******************************/
    // 长直道刹车条件满足
    // 将速度类型标志置为刹车档
    else if ((II.top <= TDP.brakeTop_1 || II.speedTop <= TDP.brakeSpeedTop_1 ||
              II.annulusTop <= YY || LSMI.absDeviation[0] >= TDP.strightAD) &&
             (speedType == FULL_ACCELE || speedType == HALF_ACCELE) &&
             maxSpeed >= TDP.brakeSpeed_1 && TDP.brakeTest <= 1) {
        speedType = BRAKE;
        brakeType = STRIGHT_BRAKE;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        limitFlag = 1;
    }
    // 将速度类型标志置为刹车档
    else if ((II.top <= TDP.brakeTop_2 || II.speedTop <= TDP.brakeSpeedTop_2 ||
              II.annulusTop <= YY || LSMI.absDeviation[0] >= TDP.strightAD) &&
             (speedType == FULL_ACCELE || speedType == HALF_ACCELE) &&
             maxSpeed >= TDP.brakeSpeed_2 && TDP.brakeTest <= 2) {
        speedType = BRAKE;
        brakeType = STRIGHT_BRAKE;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        limitFlag = 1;
    }
    // 将速度类型标志置为刹车档
    else if ((II.top <= TDP.brakeTop_3 || II.speedTop <= TDP.brakeSpeedTop_3 ||
              II.annulusTop <= YY || LSMI.absDeviation[0] >= TDP.strightAD) &&
             (speedType == FULL_ACCELE || speedType == HALF_ACCELE) &&
             maxSpeed >= TDP.brakeSpeed_3 && TDP.brakeTest <= 3) {
        speedType = BRAKE;
        brakeType = STRIGHT_BRAKE;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        limitFlag = 1;
    }
    // 将速度类型标志置为刹车档
    else if ((II.top <= TDP.brakeTop_4 || II.speedTop <= TDP.brakeSpeedTop_4 ||
              II.annulusTop <= YY || LSMI.absDeviation[0] >= TDP.strightAD) &&
             (speedType == FULL_ACCELE || speedType == HALF_ACCELE) &&
             maxSpeed >= TDP.brakeSpeed_4 && TDP.brakeTest <= 4) {
        speedType = BRAKE;
        brakeType = STRIGHT_BRAKE;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        limitFlag = 1;
    }
    // 遇到短直道减速
    // 将速度类型标志置为刹车档
    else if ((II.top <= TDP.brakeTop_0 || II.speedTop <= TDP.brakeSpeedTop_0 ||
              II.annulusTop <= YY || LSMI.absDeviation[0] >= TDP.strightAD) &&
             (speedType == FULL_ACCELE || speedType == HALF_ACCELE)) {
        speedType = BRAKE;
        brakeType = STRIGHT_BRAKE;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        limitFlag = 1;
    }
    /***********************************直道刹车*******************************/

    /***********************************出弯加速*******************************/
    // 将速度类型标志置为出弯加速
    else if (LSMI.absDeviation[0] <= TDP.exitCurveAD &&
             LSMI.diffDeviation[0] < 0 && LSMI.diffDeviation[1] < 0 &&
             LSMI.diffDeviation[2] < 0 && speedType == NORMAL_SHIFT) {
        speedType = EXIT_CURVE_ACCELE;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
    }
    // 将速度类型标志置为出弯遇到短直道
    else if (II.top >= TDP.halfTop && II.speedTop >= TDP.halfSpeedTop &&
             LSMI.absDeviation[0] <= TDP.halfAD && speedType != FULL_ACCELE &&
             speedType != BRAKE) {
        ++halfSpeedUpTime;
    }
    /***********************************出弯加速*******************************/

    /***********************************出弯刹车*******************************/
    // 出弯加速再减速
    // 将速度类型标志置为刹车档
    else if ((II.annulusTop <= YY ||
              ((II.top <= TDP.exitEnterTop ||
                II.speedTop <= TDP.exitEnterSpeedTop ||
                LSMI.absDeviation[0] >= TDP.enterCurveAD) &&
               ((LSMI.diffDeviation[0] > 0 && LSMI.diffDeviation[1] > 0 &&
                 LSMI.diffDeviation[2] > 0) ||
                (((LSMI.diffDeviation[0] > 0 && LSMI.diffDeviation[1] > 0) ||
                  (LSMI.diffDeviation[1] > 0 && LSMI.diffDeviation[2] > 0)) &&
                 LSMI.diffDeviation[0] + LSMI.diffDeviation[1] +
                         LSMI.diffDeviation[2] >
                     3)))) &&
             speedType == EXIT_CURVE_ACCELE && maxSpeed >= SP.exitEnterSpeed) {
        speedType = BRAKE;
        brakeType = EXIT_ENTER_BRAKE;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        limitFlag = 1;
    }
    //     将速度类型标志置为刹车档
    else if ((II.annulusTop <= YY ||
              (LSMI.absDeviation[0] >= TDP.enterCurveAD &&
               ((LSMI.diffDeviation[0] > 0 && LSMI.diffDeviation[1] > 0 &&
                 LSMI.diffDeviation[2] > 0) ||
                (((LSMI.diffDeviation[0] > 0 && LSMI.diffDeviation[1] > 0) ||
                  (LSMI.diffDeviation[1] > 0 && LSMI.diffDeviation[2] > 0)) &&
                 LSMI.diffDeviation[0] + LSMI.diffDeviation[1] +
                         LSMI.diffDeviation[2] >
                     3)))) &&
             speedType == EXIT_CURVE_ACCELE) {
        speedType = BRAKE;
        brakeType = EXIT_ENTER_BRAKE;
        speedUpTime = 0;
        halfSpeedUpTime = 0;
        limitFlag = 1;
    }
    /***********************************出弯刹车*******************************/

    if (speedUpTime >= 3) {
        //        BEEP(100);
        speedType = FULL_ACCELE;
    } else if (halfSpeedUpTime >= 1) {
        speedType = HALF_ACCELE;
    }
    /************************** 结束速度类型判断 ******************************/
    return speedType;
}

int16_t SpeedHandler::getBrakeSpeed() {
    int16_t brakeSpeed;
    int16_t speedCut;
    /***************************计算直道刹车的速度****************************/
    switch (brakeType) {
        case STRIGHT_BRAKE:  // 计算长直道刹车的速度
            speedCut = SP.strightSpeedCut;
            break;
        case SHORT_STRIGHT_BRAKE:  // 计算短直道入弯刹车的速度
            speedCut = SP.shortStrightSpeedCut;
            break;
        case EXIT_ENTER_BRAKE:  // 计算出弯再入弯刹车的速度
            speedCut = SP.exitEnterSpeedCut;
            break;
        default:
            break;
    }
    brakeSpeed = (int16_t)(SP.curveSpeed -
                           (SP.curveSpeed - SP.minSpeed) * LSMI.DK * LSMI.DK /
                               SP.speedK / SP.speedK -
                           speedCut);
    if (brakeSpeed + speedCut < SP.minSpeed) {
        brakeSpeed = SP.minSpeed - speedCut;
    }
    if (brakeSpeed + speedCut > SP.max2Speed) {
        brakeSpeed = SP.max2Speed - speedCut;
    }
    /********************************结束计算刹车速度**************************************/

    return brakeSpeed;
}

uint8_t SpeedHandler::isBrakeFinished(int16_t aimSpeed) {
    return ((SI.varL[0] > SI.varR[0] ? SI.varL[0] : SI.varR[0]) <= aimSpeed);
}
