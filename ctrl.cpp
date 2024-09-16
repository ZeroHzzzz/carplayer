#include <ctrl.h>
#include <motor.h>

Controller controller;

void Controller::go(uint8_t imgSize) {
    imagehandler.LAST_IF = IF;
    imagehandler.map_init();

    if (imgSize == 1)  // 灰度图
    {
        imagehandler.getGrayscaleHistogram();
        imagehandler.nowThreshold =
            imagehandler.getOSTUThreshold();  // 获取阈值
        imagehandler
            .OSTU_ROBERT();  // 用Robert算子通过OSTU阈值算出压缩图allmap灰度
    }

    for (uint8_t i = 0; i < XM; ++i)  // 通过allmap创建basemap
    {
        if (mid + i < XM && !imagehandler.allmap[0][mid + i])  // 0白 1黑
        {
            imagehandler.searchimg(mid + i,
                                   0);  // 创建basemap,创建insidemap白色区域
            break;
        }
        if (mid >= i && !imagehandler.allmap[0][mid - i]) {
            imagehandler.searchimg(mid - i, 0);
            break;
        }
    }

    if (IF.annulus)  // 环岛
    {
        uint8_t down = imagehandler.getDown(IF.annulus);
        II.step = down < 30 ? down : 30;
    }

    if (IF.ramp)  // 坡道
    {
        II.limit_line = 0;
    }

    if (II.bnum_all)  // basemap 白点数量
    {
        // 从首末列出发往中间 搜图
        for (uint8_t j = 0; j < II.step; ++j)  // step为首次搜图限高
        {
            if (imagehandler.basemap[j][0] && !imagehandler.leftmap[j][0] &&
                !imagehandler.rightmap[j][0] &&
                (II.num_lm == 0 ||
                 (II.num_lm > 0 && II.num_lm < 5 &&
                  imagehandler.isSearch(
                      0, j,
                      imagehandler
                          .rightmap)) /*判断basemap的非白区域是否已被rightmap搜过，搜过就不搜leftmap*/)) {
                imagehandler.searchleftmap(0, j);  // 创建leftmap
                II.lnum_all += II.lnum[II.num_lm];
                II.start_lm[II.num_lm] =
                    j;  // leftmap  存储 黑色区域 搜索第一列起点纵坐标
                ++II.num_lm;
                ++II._num_lm;
            }
            if (imagehandler.basemap[j][XX] && !imagehandler.leftmap[j][XX] &&
                !imagehandler.rightmap[j][XX] &&
                (II.num_rm == 0 ||
                 (II.num_rm > 0 && II.num_rm < 5 &&
                  imagehandler.isSearch(
                      XX, j,
                      imagehandler.leftmap) /*base和left双黑为0，其他1*/))) {
                imagehandler.searchrightmap(XX, j);  // 创建rightmap
                II.rnum_all += II.rnum[II.num_rm];
                II.start_rm[II.num_rm] = j;
                ++II.num_rm;
                ++II._num_rm;  // 右图限高区域内个数
            }
        }
    }

    uint8_t left = 0;
    uint8_t right = XX;
    for (int8_t i = mid; i >= 0; --i) {
        if (imagehandler.basemap[0][i]) {
            left = i;
            break;
        }
    }
    for (uint8_t i = mid; i < XM; ++i) {
        if (imagehandler.basemap[0][i]) {
            right = i;
            break;
        }
    }
    mid = (left + right) / 2;  // 以上为找第一行中点

    if (II.bnum_all)  // basemap 白点数量
    {
        if (!IF.ramp) {
            imagehandler.crossroadforleft();
            imagehandler.crossroadforright();
        }
        imagehandler.deleteline();
        imagehandler.Get_insideMap();
        imagehandler.getSpeedTop();  // 获取速度处理截止行
    }

    if (!(IF.fork || IF.ramp || IF.sramp)) {
        IF.annulus = imagehandler.goAnnulus();
        qout << "IF.annulus" << IF.annulus;
        II.annulusTop = imagehandler.getAnnulusTop();
    }

    if (!(IF.annulus || IF.ramp)) {
        IF.parking = imagehandler.my_goout_park();
    }

    if (!(IF.annulus || IF.parking || IF.annulusDelay || IF.crossroad)) {
        IF.fork = imagehandler.go_fork();
    }

    if (IF.wait_fork == 1 || IF.wait_fork == 2) {
        motorhandler.fork_control();
    }

    if (!(IF.fork || IF.annulus || IF.parking || IF.ramp || IF.wait_fork == 1 ||
          IF.wait_fork == 2)) {
        IF.crossroad = imagehandler.draw_crossroad_rightline();
        IF.crossroad = imagehandler.draw_crossroad_leftline();

        //        if(II.num_lm &&II.num_rm)
        //        {
        //            IF.crossroad=draw_crossroad_leftline_button();
        //            IF.crossroad=draw_crossroad_rightline_button();
        //        }
    }

    if (!(IF.annulus || IF.fork || IF.wait_fork == 1 || IF.wait_fork == 2)) {
        IF.ramp = imagehandler.goRamp();
        qout << "IF.ramp" << IF.ramp;
    }

    if (!(IF.annulus || IF.ramp || IF.fork)) {
        IF.parking = imagehandler.goParking();
    }

    directionControl();

    SI.aimSpeed = speedhandler.getAimSpeed();
}

uint8_t Controller::fork_outPark_Detect() {
    uint8_t i, j, temp_y;
    if (runState == CAR_RUNNING) {
        for (j = 35; j < XM; ++j) {
            if (imagehandler.leftmap[0][j] >= 2) {
                if (j > 40) {
                    for (i = XX; i > 23; --i) {
                        temp_y = imagehandler.getMapYMin_Col(
                            i, imagehandler.leftmap, 1);
                        if (temp_y < 10) {
                            // runState = CAR_BRAKING;
                            // stopReason = RunOutLine;
                            return 1;
                        }
                    }
                }
                break;
            }
        }
        if (runState == CAR_RUNNING) {
            for (int8_t j = 20; j >= 0; --j) {
                if (imagehandler.rightmap[0][j] >= 2) {
                    if (j < 7) {
                        for (i = 0; i < 23; ++i) {
                            temp_y = imagehandler.getMapYMin_Col(
                                i, imagehandler.rightmap, 1);
                            if (temp_y < 10) {
                                // runState = CAR_BRAKING;
                                // stopReason = RunOutLine;
                                return 1;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

void Controller::directionControl() {
    II.midnum = getrampline(kbmid, MIDLINE);
    II.leftnum = getline(kbleft, LEFTLINE);
    II.rightnum = getline(kbright, RIGHTLINE);

    /************ 确定 controlLine ************/
    uint16_t lnum = II.lnum_all;
    uint16_t rnum = II.rnum_all;

    if (II.leftnum < 10)
        lnum = 0;
    if (II.rightnum < 10)
        rnum = 0;
    if (II.leftnum < 10 && II.rightnum < 10)
        lnum = rnum = 0;

    if (II.line_forbid == LEFTLINE)
        lnum = 0;
    else if (II.line_forbid == RIGHTLINE)
        rnum = 0;
    else if (II.line_forbid == BOTHLINE)
        lnum = rnum = 0;

    if (lnum > rnum + 500)
        controlLine = LEFTLINE;
    else if (rnum > lnum + 500)
        controlLine = RIGHTLINE;
    else {
        if (lnum >= rnum && rnum <= 100 && lnum >= 100)
            controlLine = LEFTLINE;
        else if (rnum >= lnum && lnum <= 100 && rnum >= 100)
            controlLine = RIGHTLINE;
        else if (rnum >= 100 && lnum >= 100)
            controlLine = BOTHLINE;
        else
            controlLine = NONELINE;
    }

    if (II.line_forbid == NONELINE) {
        if (IF.annulus > 6)
            controlLine = LEFTLINE;
        else if (IF.annulus > 0)
            controlLine = RIGHTLINE;
    }

    if (IF.ramp) {
        controlLine = MIDLINE;
        if (II.midnum <= 10)
            controlLine = NONELINE;
    }

    if (IF.fork) {
        if (imagehandler.fork_dir == goRight)
            controlLine = LEFTLINE;
        else if (imagehandler.fork_dir == goLeft)
            controlLine = RIGHTLINE;
    }

    if (II.line_forbid == NONELINE) {
        if (IF.in_park > 3)
            controlLine = LEFTLINE;
        else if (IF.in_park > 0)
            controlLine = RIGHTLINE;
    }
    /************ 确定 controlLine ************/

    /************ 确定 不同元素下的直线拟合参数 ************/
    float kk = DP.k / 10.000, bb = DP.b / 100.000;

    if (IF.annulus)
        kk = DP.annulusK[imagehandler.nowAnnulusNum - 1] / 10.000;
    else if (IF.ramp) {
        kk = DP.rampK / 10.000;
        bb = DP.rampB / 100.000;
    } else if (IF.fork) {
        kk = DP.forkK / 10.0;
        bb = DP.forkB / 100.0;
    } else if (IF.parking) {
        kk = DP.goout_parkK / 10.000;
        bb = DP.goout_parkB / 100.000;
    } else if (IF.in_park) {
        kk = DP.parkingK / 10.000;
        bb = DP.parkingB / 100.000;
    }
    /************ 确定 不同元素下的直线拟合参数 ************/

    if (controlLine == BOTHLINE) {
        LSMI.K = (kbleft[0] + kbright[0]) / 2;
        LSMI.B = (kbleft[1] + kbright[1]) / 2;
        LSMI.nowDeviation = LSMI.K * kk + LSMI.B * bb;
        LSMI.DK = LSMI.K * 100;
    } else if (controlLine == LEFTLINE) {
        LSMI.K = kbleft[0];
        LSMI.B = kbleft[1];
        LSMI.nowDeviation = LSMI.K * kk + LSMI.B * bb;
        LSMI.DK = LSMI.K * 100;
    } else if (controlLine == RIGHTLINE) {
        LSMI.K = kbright[0];
        LSMI.B = kbright[1];
        LSMI.nowDeviation = LSMI.K * kk + LSMI.B * bb;
        LSMI.DK = LSMI.K * 100;
    } else if (controlLine == MIDLINE) {
        LSMI.K = kbmid[0];
        LSMI.B = kbmid[1];
        LSMI.nowDeviation =
            LSMI.K * kk + LSMI.B * bb -
            yaw_ramp_k * imagehandler.SystemAttitudeRate.Yaw / 180.0f;
        LSMI.DK = LSMI.K * 100;
    }

    if (!(LSMI.nowDeviation == LSMI.nowDeviation && LSMI.K == LSMI.K &&
          LSMI.B == LSMI.B)) {
        LSMI.nowDeviation = LSMI.lastDeviation;
        LSMI.K = LSMI.lastK;
        LSMI.B = LSMI.lastB;
    }

    /* 更新数据 */
    LSMI.lastDeviation = LSMI.nowDeviation;
    LSMI.lastK = LSMI.K;
    LSMI.lastB = LSMI.B;

    if (IF.annulus) {
        if (imagehandler.AD.flag == 3) {
            LSMI.nowDeviation = imagehandler.AD.sumDev / imagehandler.AD.cnt;
            LSMI.DK = imagehandler.AD.sumDK / imagehandler.AD.cnt;
        } else if (imagehandler.AD.flag == 4) {
            LSMI.DK = imagehandler.AD.sumDK / imagehandler.AD.cnt;
        }
    }

    if (imagehandler.AD.flag == 1) {
        imagehandler.AD.sumDev += LSMI.nowDeviation;
        imagehandler.AD.sumDK += LSMI.DK;
        ++imagehandler.AD.cnt;
    }

#ifndef T_REX_QT_DEBUG
    // servoPID.error = LSMI.nowDeviation;
    // Calc_ServoPID(&servoPID);
    // servoPWM = SERVO_PWM_MID - servoPID.PID_Out;
    // pwm_duty(S3010_PWM_CH, servoPWM); //舵机转向
#endif

    //    testBox.test6 = DIRP.curvature;
    //    testBox.test5 = DIRP.curvature;
    LSMI.aveDeviationLeft = getAveDeviation(LEFTLINE);
    LSMI.aveDeviationRight = getAveDeviation(RIGHTLINE);

    /* 返回 1000 表示left/rightmap 中无边线 */
    if (LSMI.aveDeviationLeft != 10000 && LSMI.aveDeviationRight != 10000) {
        LSMI.aveDeviation =
            (LSMI.aveDeviationLeft + LSMI.aveDeviationRight) / 2;
    } else if (LSMI.aveDeviationLeft == 10000 &&
               LSMI.aveDeviationRight != 10000) {
        LSMI.aveDeviation = LSMI.aveDeviationRight;
    } else if (LSMI.aveDeviationRight == 10000 &&
               LSMI.aveDeviationLeft != 10000) {
        LSMI.aveDeviation = LSMI.aveDeviationLeft;
    }

    /*** 更新数据 ***/
    LSMI.absDeviation[4] = LSMI.absDeviation[3];
    LSMI.absDeviation[3] = LSMI.absDeviation[2];
    LSMI.absDeviation[2] = LSMI.absDeviation[1];
    LSMI.absDeviation[1] = LSMI.absDeviation[0];
    LSMI.absDeviation[0] = abs((int)LSMI.aveDeviation);

    LSMI.diffDeviation[4] = LSMI.diffDeviation[3];
    LSMI.diffDeviation[3] = LSMI.diffDeviation[2];
    LSMI.diffDeviation[2] = LSMI.diffDeviation[1];
    LSMI.diffDeviation[1] = LSMI.diffDeviation[0];
    LSMI.diffDeviation[0] = LSMI.absDeviation[0] - LSMI.absDeviation[1];
}

float Controller::getAveDeviation(ControlLine cl)  // leftmap, leftline
{
    float sum = 0;
    float x;
    int n = 0;
    uint8_t d_line = 30;
    uint8_t stop = II.top - 1 < 55 ? II.top - 1 : 55;
    if (cl == LEFTLINE) {
        for (uint8_t j = d_line; j < stop; ++j) {
            for (uint8_t i = 0; i < XM; ++i) {
                if (imagehandler.leftmap[j][i] >= 2) {
                    ++n;
                    x = (i - imagehandler.leftline[j]) * imagehandler.k1[j];
                    sum += x;
                }
            }
        }
    }
    if (cl == RIGHTLINE) {
        for (uint8_t j = d_line; j < stop; ++j) {
            for (int8_t i = XX; i >= 0; --i) {
                if (imagehandler.rightmap[j][i] >= 2) {
                    ++n;
                    x = (i - imagehandler.rightline[j]) * imagehandler.k1[j];
                    sum += x;
                }
            }
        }
    }
    if (n == 0) {
        return 10000;
    } else {
        return (sum / n);
    }
}

uint8_t Controller::getline(float kb[2], ControlLine cl) {
    float sumx = 0;
    float sumy = 0;
    float sumxy = 0;
    float sumx2 = 0;
    uint8_t n = 0;
    float x;
    float y;
    uint8_t lastx = 0, llastx = 0;
    uint8_t lasty = 0;
    if (cl == LEFTLINE) {
        lastx = 0;
        lasty = 0;

        for (uint8_t i = 0; i < II.stop_line; ++i) {
            for (uint8_t j = 0; j < XX; ++j) {
                if ((imagehandler.leftmap[i][j] == 2 &&
                     (imagehandler.leftmap[i][j + 1] == 0 ||
                      imagehandler.leftmap[i][j + 1] == 2)) ||
                    imagehandler.leftmap[i][j] == 3) {
                    imagehandler.leftmap[i][j] = 4;
                    ++n;
                    y = (j - imagehandler.leftline[i]) * imagehandler.k1[i];

                    x = imagehandler.k2[i];
                    sumx += x;
                    sumy += y;
                    sumxy += x * y;
                    sumx2 += x * x;

                    lastx = j;
                    lasty = i;
                }
            }
            if ((imagehandler.leftmap[i][XX - 1] == 4 ||
                 (i < YY && imagehandler.leftmap[i + 1][XX - 1] == 4) ||
                 (i > 0 && imagehandler.leftmap[i - 1][XX - 1] == 4)) &&
                (imagehandler.leftmap[i][XX] == 2 ||
                 imagehandler.leftmap[i][XX] == 3)) {
                imagehandler.leftmap[i][XX] = 4;
                ++n;
            }
            llastx = lastx;
        }
    }

    else if (cl == RIGHTLINE) {
        lastx = XX;
        lasty = 0;
        for (uint8_t i = 0; i < II.stop_line; ++i) {
            for (uint8_t j = XX; j > 0; --j) {
                if ((imagehandler.rightmap[i][j] == 2 &&
                     (imagehandler.rightmap[i][j - 1] == 0 ||
                      imagehandler.rightmap[i][j - 1] == 2)) ||
                    imagehandler.rightmap[i][j] == 3) {
                    imagehandler.rightmap[i][j] = 4;
                    ++n;
                    y = (j - imagehandler.rightline[i]) * imagehandler.k1[i];
                    x = imagehandler.k2[i];
                    sumx += x;
                    sumy += y;
                    sumxy += x * y;
                    sumx2 += x * x;

                    lastx = j;
                    lasty = i;
                }
            }
            if ((imagehandler.rightmap[i][1] == 4 ||
                 (i < YY && imagehandler.rightmap[i + 1][1] == 4) ||
                 (i > 0 && imagehandler.rightmap[i - 1][1] == 4)) &&
                (imagehandler.rightmap[i][0] == 2 ||
                 imagehandler.rightmap[i][0] == 3)) {
                imagehandler.rightmap[i][0] = 4;
                ++n;
            }
            llastx = lastx;
        }
    }
    kb[0] = (n * sumxy - sumx * sumy) / (n * sumx2 - sumx * sumx);
    kb[1] = sumy / n - kb[0] * sumx / n;
    return n;
}

uint8_t Controller::getrampline(float kb[2], ControlLine cl) {
    for (uint8_t j = 0, flag = 0; j < YM / 2; ++j) {
        uint8_t l = 0;
        uint8_t r = XX;
        for (uint8_t i = 0; i < XM; ++i) {
            if (imagehandler.leftmap[j][XX - i] == 2) {
                l = XX - i;
                break;
            }
        }
        for (uint8_t i = 0; i < XM; ++i) {
            if (imagehandler.rightmap[j][i] == 2) {
                r = i;
                break;
            }
        }
        if (flag == 0 && l != 0 && r != XX) {
            flag = 1;
        } else if (flag == 1 && (l == 0 || r == XX)) {
            continue;
        }
        imagehandler.midline[j] = l + r;
    }

    float sumx = 0;
    float sumy = 0;
    float sumxy = 0;
    float sumx2 = 0;
    uint8_t n = 0;
    float x;
    float y;
    uint8_t lasty = 255;

    for (uint8_t i = 0; i < YM / 2; ++i) {
        if (imagehandler.midline[i] == 0) {
            continue;
        }
        if (lasty != 255) {
            if (imagehandler.midline[i] > imagehandler.midline[lasty] + 2 ||
                imagehandler.midline[i] + 2 < imagehandler.midline[lasty]) {
                continue;
            }
        }
        lasty = i;
        ++n;
        y = (imagehandler.midline[i] - XX) * imagehandler.k1[i];

        x = imagehandler.k2[i];
        sumx += x;
        sumy += y;
        sumxy += x * y;
        sumx2 += x * x;
    }

    kb[0] = (n * sumxy - sumx * sumy) /
            (n * sumx2 - sumx * sumx);  // 最小二乘拟合rampline
    kb[1] = sumy / n - kb[0] * sumx / n;
    return n;
}

void Controller::deviationParam_init() {
    DP.k = 680;            // 560;//675;//718
    DP.b = 155;            // 300;//125;//172
    DP.annulusK[0] = 698;  // 720;
    DP.annulusK[1] = 698;  // 720;
    DP.annulusK[2] = 698;  // 720;
    DP.annulusK[3] = 698;  // 720;
    DP.annulusK[4] = 698;  // 720;

    DP.forkK = 690;
    DP.forkB = 170;

    DP.parkingK = 700;
    DP.parkingB = 120;

    DP.goout_parkK = 650;
    DP.goout_parkB = 120;
}

void Controller::controlParam_init() {
    directionInternalPIDParam.P = 250;  // 转向内环
    directionInternalPIDParam.I = 0;
    directionInternalPIDParam.D = 180;  // 100;
    directionInternalPIDParam.T = 70;
    directionInternalPIDParam.Coeff = 10000;
    directionInternalPIDParam.sumError = 0;
    directionInternalPIDParam.preError = 0;
    directionInternalPIDParam.prePreError = 0;
    directionInternalPIDParam.feedBack = 0;

    directionOuterPIDParam.P = 300;  // 300//转向外环
    directionOuterPIDParam.I = 0;    // 105;//105;
    directionOuterPIDParam.D = 0;    // 90;
    directionOuterPIDParam.T = 400;
    directionOuterPIDParam.Coeff = 10000;
    directionOuterPIDParam.sumError = 0;
    directionOuterPIDParam.preError = 0;
    directionOuterPIDParam.prePreError = 0;
    directionOuterPIDParam.feedBack = 0;

    directionParam.curvature = 0;
    directionParam.curvatureLimit = 480;
    directionParam.curvatureMin = 335;
    directionParam.curvatureParam = 300;
    directionParam.targetYawRate = 0;
    directionParam.targetYawRateLimit = 26;
    directionParam.dirPWM = 0;
    directionParam.speedPWM = 0;
    directionParam.devGain = 5;

    angularVelocity_flag = 0;
    angle_flag = 0;

    lastDirectionParam = directionParam;

    deviationParam_init();
}

int16_t Controller::cal_IncrementalPID(PIDParam* PIDparam,
                                       int16_t reference,
                                       int16_t feedBack) {
    int16_t nowError, increase;
    float kp_t, ki_t, kd_t;

    nowError = reference - feedBack;
    kp_t = (float)(PIDparam->P) / PIDparam->Coeff;
    ki_t = (float)(PIDparam->I) / PIDparam->Coeff;
    kd_t = (float)(PIDparam->D) / PIDparam->Coeff;

    increase = (int16_t)((kp_t * (nowError - PIDparam->preError)) +
                         (ki_t * (nowError)) +
                         (kd_t * (nowError - 2 * PIDparam->preError +
                                  PIDparam->prePreError)));

    PIDparam->prePreError = PIDparam->preError;
    PIDparam->preError = nowError;
    PIDparam->feedBack = feedBack;

    return increase;
}

int16_t Controller::cal_RealizePID(PIDParam* PIDparam,
                                   int16_t reference,
                                   int16_t feedBack) {
    int16_t nowError, realize;
    float kp_t, ki_t, kd_t;

    nowError = reference - feedBack;
    kp_t = (float)(PIDparam->P) / PIDparam->Coeff;
    ki_t = (float)(PIDparam->I) / PIDparam->Coeff;
    kd_t = (float)(PIDparam->D) / PIDparam->Coeff;

    PIDparam->sumError += ki_t * nowError;
    PIDparam->sumError = motorhandler.amplitudeLimit(
        PIDparam->sumError, (-1 * (float)(PIDparam->T)),
        (float)(PIDparam->T));  // 积分限幅
    realize = (int16_t)((kp_t * nowError) + (PIDparam->sumError) +
                        (kd_t * (nowError - PIDparam->preError)));

    PIDparam->prePreError = PIDparam->preError;
    PIDparam->preError = nowError;
    PIDparam->feedBack = feedBack;

    return realize;
}

int16_t Controller::PlacePID_Control(PIDParam* PIDparam,
                                     int16_t reference,
                                     int16_t feedBack)  // 转向外环，计算曲率用
{
    int16_t nowError,  // 当前误差
        actualVal,     // 实际输出值
        protectRange;  // 动态限幅值
    float kp_t, kd_t;  // 动态pd

    nowError = reference - feedBack;
    kd_t = (float)(PIDparam->D) / PIDparam->Coeff;
    // P值与差值成二次函数关系，此处P和I不是PID参数，而是动态PID参数，要注意！！！
    //(1)偏差平方关联////iError*iError/((float)(PIDparam.P)) +
    kp_t = nowError * nowError / ((float)(PIDparam->P)) +
           (float)(PIDparam->I) / PIDparam->Coeff;

    //    testBox.test0 = kp_t;

    //    if(SI.speedTrueM < 80)//对中间较低速度进行一定的参数衰减
    //    {
    //        kp_t = (float)(PIDparam->I) / PIDparam->Coeff;
    //        kd_t = 0;
    //    }
    //    else if(SI.speedTrueM < 90)
    //    {
    //        kp_t = (float)(PIDparam->I - 18) / PIDparam->Coeff;
    //        kd_t = 0.3 * kd_t;
    //    }
    //    else if(SI.speedTrueM < 100)
    //    {
    //        kp_t = (float)(PIDparam->I - 15) / PIDparam->Coeff;
    //        kd_t = 0.3 * kd_t;
    //    }
    //    else if(SI.speedTrueM < 110)
    //    {
    //        kp_t = (float)(PIDparam->I - 13) / PIDparam->Coeff;
    //        kd_t = 0.5 * kd_t;
    //    }
    //    else if(SI.speedTrueM < 120)
    //    {
    //        kp_t = (float)(PIDparam->I - 8) / PIDparam->Coeff;
    //        kd_t = 0.7 * kd_t;
    //    }

    if (IF.ramp == 1 || IF.ramp == 2) {
        kd_t = 0;
    }
    actualVal =
        (int16_t)(kp_t * nowError + kd_t * ((0.8 * (float)nowError +
                                             0.2 * (float)PIDparam->preError) -
                                            PIDparam->preError));
    //    testBox.test1 = actualVal;
    PIDparam->preError = nowError;
    PIDparam->prePreError = PIDparam->preError;

    protectRange =
        (int16_t)(DIRP.curvatureLimit -
                  pow(SI.speedTrueM, 2) / (float)(DIRP.curvatureParam));
    if (protectRange < DIRP.curvatureMin) {
        protectRange = DIRP.curvatureMin;  // 300
    }

    actualVal =
        motorhandler.amplitudeLimit(actualVal, -protectRange, protectRange);

    return actualVal;
}

int32_t Controller::PID_Realize_Dir(PIDParam* PIDparam,
                                    int32_t reference,
                                    int32_t feedBack)  // 转向内环
{
    int32_t nowError,  // 当前误差
        realize;       // 最后得出的实际增量
    uint32_t kp_t, kd_t;
    float setD = ((float)(PIDparam->I) / PIDparam->Coeff);

    static int32_t sumError = 0;
    nowError = reference - feedBack;  // 计算当前误差
    sumError += nowError;

    if (SI.speedTrueM > 170) {
        kp_t = PIDparam->P + 20;
        kd_t = PIDparam->D + 50;
    } else {
        kp_t = PIDparam->P;
        kd_t = PIDparam->D;
    }
    if (nowError * feedBack < 0) {
        setD = 0;  // 低速不加D
    }

    float Integral =
        ((float)(sumError) * ((float)(PIDparam->I) / PIDparam->Coeff));

    if (Integral > 70.0)  // 积分限幅
    {
        Integral = 0;
        if ((sumError > 0 && nowError > 0) || (sumError < 0 && nowError < 0))
            sumError -= nowError;  // 遇限削弱积分
    }

    realize = (int32_t)((((float)kp_t) / PIDparam->Coeff) * nowError +
                        (((float)kd_t) / PIDparam->Coeff) *
                            (nowError - PIDparam->preError) +
                        Integral);
    //			- setD * (feedBack));//这项为- 则就是纯阻尼作用

    PIDparam->prePreError = PIDparam->preError;  // 更新前次误差
    PIDparam->preError = nowError;               // 更新上次误差
    PIDparam->feedBack = feedBack;               // 更新上次数据

    return realize;  // 返回实际值
}