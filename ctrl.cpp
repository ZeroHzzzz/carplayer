#include <ctrl.h>

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
        fork_control();
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

    SI.aimSpeed = getAimSpeed();
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