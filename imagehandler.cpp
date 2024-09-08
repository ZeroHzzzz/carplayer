#include "imagehandler.h"
#include "ctrl.h"
#include "datahandler.h"
#include "speed.h"
#include "stack.h"

ImageHandler imagehandler;

bool ImageHandler::standard() {
    uint8_t speedX;

    for (uint8_t j = 0; j < YM; ++j) {
        if (j < car_stop) {
            car_rightline[j] = XX - car_leftline[j];
        }

        rightline[j] = XX - leftline[j];
        k1[j] = XM / (rightline[j] - leftline[j] + 1);  // 逆透视 放大倍数
        speedX = 0;
        speedlineLeft[j] = 23 - speedX;
        speedlineRight[j] = 23 + speedX;
        if (!leftlineZero && leftline[j] >= 0)
            leftlineZero = j;
        if (!rightlineZero && rightline[j] <= XX)
            rightlineZero = j;
    }

    // 初始化工作
    imgInfoInit.num_lm = 0;
    imgInfoInit.num_rm = 0;
    imgInfoInit._num_lm = 0;
    imgInfoInit._num_rm = 0;

    memset(imgInfoInit.start_lm, 0, sizeof(imgInfoInit.start_lm));
    memset(imgInfoInit.start_rm, 0, sizeof(imgInfoInit.start_rm));

    for (uint8_t i = 0; i < 5; ++i) {
        imgInfoInit.left_bottom[i].x = XX;
        imgInfoInit.right_bottom[i].x = 0;
        imgInfoInit.left_top[i].x = XX;
        imgInfoInit.right_top[i].x = 0;

        imgInfoInit.bottom_left[i].x = XX;
        imgInfoInit.bottom_right[i].x = 0;
        imgInfoInit.top_left[i].x = XX;
        imgInfoInit.top_right[i].x = 0;

        imgInfoInit.left_bottom[i].y = YY;
        imgInfoInit.right_bottom[i].y = YY;
        imgInfoInit.left_top[i].y = 0;
        imgInfoInit.right_top[i].y = 0;

        imgInfoInit.bottom_left[i].y = YY;
        imgInfoInit.bottom_right[i].y = YY;
        imgInfoInit.top_left[i].y = 0;
        imgInfoInit.top_right[i].y = 0;
    }

    imgInfoInit.startMap_leftTop.x = XX;
    imgInfoInit.startMap_leftTop.y = 0;
    imgInfoInit.startMap_leftBottom.x = XX;
    imgInfoInit.startMap_leftBottom.y = YY;
    imgInfoInit.startMap_rightTop.x = 0;
    imgInfoInit.startMap_rightTop.y = 0;
    imgInfoInit.startMap_rightBottom.x = 0;
    imgInfoInit.startMap_rightBottom.y = YY;

    imgInfoInit.left_x = XM;
    imgInfoInit.right_x = 0;
    imgInfoInit.left_y = YM;
    imgInfoInit.right_y = YM;

    imgInfoInit._top = 0;
    imgInfoInit.top = 0;
    imgInfoInit.bottom = YY;
    imgInfoInit.speedTop = YM;
    imgInfoInit.annulusTop = YM;

    imgInfoInit.stop_line = STOP_LINE;  // 图像处理截止线 (顶部多为无效信息）
    imgInfoInit.limit_line = YM;
    imgInfoInit.step = STEP;  // 扫描终止行
    imgInfoInit.line_forbid = NONELINE;
    imgInfoInit.repeatNum = 0;
    imgInfoInit.dnum_top = 0;
    imgInfoInit.bnum_all = 0;
    imgInfoInit.lnum_all = 0;
    imgInfoInit.rnum_all = 0;
    imgInfoInit.dnum_all = 0;

    memset(II.lnum, 0, sizeof(II.lnum));
    memset(II.rnum, 0, sizeof(II.rnum));

    imgInfoInit.lmin = 0;
    imgInfoInit.rmin = 0;
    imgInfoInit.leftnum = 0;
    imgInfoInit.rightnum = 0;
    imgInfoInit.midnum = 0;

    imgFlagLock.imgFlag = 0;
    imgFlagLock.lockTime = 0;
    imgFlagLock.isLocked = 0;
}

void ImageHandler::map_init() {
    memset(histogram, 0, sizeof(histogram));
    memset(basemap, 1, sizeof(basemap));
    memset(leftmap, 0, sizeof(leftmap));
    memset(rightmap, 0, sizeof(rightmap));
    memset(insidemap, 1, sizeof(insidemap));
    memset(deletemap, 0, sizeof(deletemap));

    testBox.test0 = 0;
    testBox.test1 = 0;
    testBox.test2 = 0;
    testBox.test3 = 0;
    testBox.test4 = 0;
    testBox.test5 = 0;
    testBox.test6 = 0;
    testBox.test7 = 0;
    testBox.test8 = 0;
    testBox.test9 = 0;

    noDown.num = 0;
    II = imgInfoInit;
}

void ImageHandler::getGrayscaleHistogram() {
    uint8_t* ptr = &imgGray[0][0];
    uint8_t* ptrEnd = &imgGray[_ROW][_COL] + 1;
    while (ptr != ptrEnd)
        ++histogram[*ptr++];  // histogram[i]储存灰度为i的像素个数
}

uint8_t ImageHandler::getOSTUThreshold() {
    uint32_t sum = 0, valueSum = 0;
    uint64_t sigma = 0, maxSigma = 0;
    float w1 = 0, w2 = 0;
    int32_t u1 = 0, u2 = 0;
    uint8_t min = 0, max = 255;

    /*** 获取灰度区间 ***/
    for (min = 0; histogram[min] == 0 && min < 255; ++min)
        ;
    for (max = 255; histogram[max] == 0 && max > 0; --max)
        ;
    if (min < minThreshold)
        min = minThreshold;  // 出现的最小灰度
    if (max > maxThreshold)
        max = maxThreshold;  // 出现的最大灰度

    uint32_t lowSum[256] = {0};
    uint32_t lowValueSum[256] = {0};
    for (uint16_t i = min; i <= max; ++i) {
        sum += histogram[i];
        valueSum += histogram[i] * i;
        lowSum[i] = sum;            // 灰度<i的总像素数
        lowValueSum[i] = valueSum;  // 灰度<=i的总灰度值
    }
    for (uint16_t i = min; i < max + 1; ++i)  // 遍历求取最大阈值
    {
        w1 = (float)lowSum[i] / sum;
        w2 = 1 - w1;
        u1 = (int32_t)(lowValueSum[i] / w1);
        u2 = (int32_t)((valueSum - lowValueSum[i]) / w2);
        sigma = (uint64_t)(w1 * w2 * (u1 - u2) * (u1 - u2));  // 大津法公式
        if (sigma >= maxSigma) {
            maxSigma = sigma;
            nowThreshold = i;
        } else {
            break;
        }
    }
    previousThreshold = nowThreshold;
    //    qout<<"nowthreshold"<<nowThreshold;
    return nowThreshold;
}

void ImageHandler::OSTU_ROBERT() {
    uint8_t _x;
    uint8_t _y;
    uint8_t num;
    uint8_t sum;

    /*** 阈值判断 不运行 ***/
    for (uint8_t j = 0; j < startRow; ++j) {
        _y = (YY - j) * 3;  // 高度上 3倍压缩
        for (uint8_t i = 0; i < XM; ++i) {
            _x = i << 2;        // 位运算 左移2 即i*4 宽度上 4倍压缩
            ostuJudge(_x, _y);  // 判断压缩点灰度
        }
    }
    for (uint8_t j = endRow; j < YM; ++j) {
        _y = (YY - j) * 3;
        for (uint8_t i = 0; i < XM; ++i) {
            _x = i << 2;
            ostuJudge(_x, _y);
        }
    }

    /*** Roberts算子判断 ***/
    for (uint8_t j = startRow; j < endRow; ++j) {
        _y = (YY - j) * 3;
        if (j == 0)  // 第一行 专注连续白点的判断
        {
            for (uint8_t i = 0; i < XX; ++i) {
                _x = i << 2;
                robertJudge_Y(_x, _y);
            }
            for (uint8_t i = XX; i < XM; ++i) {
                _x = i << 2;
                robertJudge_XY(_x, _y);
            }
        } else {
            for (uint8_t i = 0; i < XX; ++i) {
                _x = i << 2;
                robertJudge(_x, _y);
            }
            for (uint8_t i = XX; i < XM; ++i) {
                _x = i << 2;
                robertJudge_X(_x, _y);
            }
        }
    }
}

void ImageHandler::searchimg(uint8_t x, uint8_t y) {
    // Point a;
    // a.x = x; a.y = y;
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    stackhandler.PushStack(&s, x, y);
    while (!stackhandler.EmptyStack(&s)) {
        Point a;
        a = s.data[s.top];
        if (!allmap[a.y][a.x])  // 白色进入判断
        {
            if (basemap[a.y][a.x] == 1)  // 0白前景 1黑背景 2红线 254绿线
            {
                ++II.bnum_all;
                insidemap[a.y][a.x] = 0;
                basemap[a.y][a.x] = 0;  // 白色记为0
                if (II.top < a.y)
                    II.top = a.y;
            }
            if (a.x > 0 && basemap[a.y][a.x - 1] == 1)  // 先dfs左侧
                stackhandler.PushStack(
                    &s, a.x - 1,
                    a.y);  // 栈 模拟递归 等价于 执行 searching(x-1,y)
            else if (a.x < XX && basemap[a.y][a.x + 1] == 1)  // 再右
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y > 0 && basemap[a.y - 1][a.x] == 1)  // 再上
                stackhandler.PushStack(&s, a.x, a.y - 1);
            else if (a.y < YY && basemap[a.y + 1][a.x] == 1)  // 再下
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else
                stackhandler.PopStack(&s);
        } else {
            basemap[a.y][a.x] = 2;  // 描边界
            stackhandler.PopStack(&s);
        }
    }
}

/******* 参与环岛标志位判定 ******/
uint8_t ImageHandler::getDown(uint8_t dir) {
    if (dir == AR2)  // 适用右环岛
    {
        uint8_t min = getMapYMin_Col2(XX, 0, basemap);  // 最右侧
        if (min > 15)
            for (uint8_t i = XX; i > XX - 20; --i) {
                // 只有右环岛赛道 与顶部和最右侧相连 后者存在即为右环岛
                if (basemap[min - 1][i] == 2) {
                    return min - 3;
                }
            }
    } else if (dir == AL2)  // 检索图的左侧 适用左环岛
    {
        uint8_t min = getMapYMin_Col2(0, 0, basemap);  // 首列
        if (min > 15)
            for (uint8_t i = 0; i < 20; ++i) {
                // 只有左环岛赛道 与顶部和最左侧相连 后者存在即为左环岛
                if (basemap[min - 1][i] == 2) {
                    return min - 3;
                }
            }
    } else if (dir != AR1 && dir != AL1 && dir != AR3 && dir != AL3) {
        return 19;  // 22;
    }
    return YM;
}

void ImageHandler::getallmap() {
    uint8_t _x;
    uint8_t _y;
    uint8_t num;
    for (uint8_t j = 0; j < YM; ++j) {
        for (uint8_t i = 0; i < XM; ++i) {
            _x = i << 2;
            _y = (YY - j) * 3;
            ostuJudge(_x, _y);
        }
    }
}

void ImageHandler::getlinemap() {
    memset(linemap, 0, sizeof(linemap));

    for (uint8_t j = 0; j < YM; ++j) {
        if (ceil(leftline[j]) >= 0)
            linemap[j][(uint8_t)ceil(leftline[j])] = 6;
        if (floor(rightline[j]) <= XX)
            linemap[j][(uint8_t)floor(rightline[j])] = 6;

        linemap[j][speedlineLeft[j]] = 8;
        linemap[j][speedlineRight[j]] = 8;
    }

    for (uint8_t j = 0; j < YM; ++j) {
        for (uint8_t i = 0; i < XM; ++i) {
            if (IF.ramp && j < YM / 2 && i == midline[j] / 2)
                linemap[j][i] = 5;

            if (leftmap[j][i] == 2 || rightmap[j][i] == 2)
                linemap[j][i] = 2;
            else if (leftmap[j][i] == 3 || rightmap[j][i] == 3)
                linemap[j][i] = 3;
            else if (leftmap[j][i] == 4 || rightmap[j][i] == 4)
                linemap[j][i] = 4;
        }
    }
}

/****** 连通域内计数 等同于求面积 ******/
void ImageHandler::searchCountmap(uint8_t x, uint8_t y, uint8_t src[][XM]) {
    // Point a;
    // a.x = x; a.y = y;
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    stackhandler.PushStack(&s, x, y);
    while (!stackhandler.EmptyStack(&s)) {
        Point a;
        a = s.data[s.top];
        if (basemap[a.y][a.x]) {
            if (src[a.y][a.x] == 0) {
                ++numCnt;
                src[a.y][a.x] = 1;
            }
            if (a.x > 0 && !src[a.y][a.x - 1])
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < XX && !src[a.y][a.x + 1])
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y > 0 && !src[a.y - 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y - 1);
            else if (a.y < YY && !src[a.y + 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else
                stackhandler.PopStack(&s);
        } else {
            src[a.y][a.x] = 2;
            stackhandler.PopStack(&s);
        }
    }
}

void ImageHandler::searchCoumtmap_Set_X(uint8_t x,
                                        uint8_t y,
                                        uint8_t start_x,
                                        uint8_t end_x,
                                        uint8_t src[][XM]) {
    Point a;
    stack s;
    Point data[Stack_Size];
    stackhandler.InitStack(&s, Stack_Size, data);
    stackhandler.PushStack(&s, x, y);

    while (!stackhandler.EmptyStack(&s)) {
        a = s.data[s.top];

        if (basemap[a.y][a.x]) {
            if (src[a.y][a.x] == 0) {
                ++numCnt;
                src[a.y][a.x] = 1;
            }

            if (a.x > start_x && !src[a.y][a.x - 1])
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < end_x && !src[a.y][a.x + 1])
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y > 0 && !src[a.y - 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y - 1);
            else if (a.y < YY && !src[a.y + 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else
                stackhandler.PopStack(&s);
        } else {
            src[a.y][a.x] = 2;
            stackhandler.PopStack(&s);
        }
    }
}

/****** 连通域内计数（面积） ******/
uint16_t ImageHandler::cntMap(uint8_t x, uint8_t y) {
    numCnt = 0;
    uint8_t map[YM][XM] = {
        {0}};  // 新建一幅图并置0，用于防止重复数点(具体见下一行)
    searchCountmap(x, y, map);
    return numCnt;  // 返回basemap中非0点数
}

uint16_t ImageHandler::cntMap_Set_X(uint8_t x,
                                    uint8_t y,
                                    uint8_t start_x,
                                    uint8_t end_y) {
    numCnt = 0;
    uint8_t map[YM][XM] = {{0}};
    searchCoumtmap_Set_X(x, y, start_x, end_y, map);
    return numCnt;
}

/****** 与searching 最后的置色不同 deletemap使用较多******/
void ImageHandler::searchmap(uint8_t x, uint8_t y, uint8_t src[][XM]) {
    // Point a;
    // a.x = x; a.y = y;
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    stackhandler.PushStack(&s, x, y);
    while (!stackhandler.EmptyStack(&s)) {
        Point a;
        a = s.data[s.top];
        if (basemap[a.y][a.x]) {
            if (src[a.y][a.x] == 0) {
                src[a.y][a.x] = 1;
            }
            if (a.x > 0 && !src[a.y][a.x - 1])
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < XX && !src[a.y][a.x + 1])
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y > 0 && !src[a.y - 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y - 1);
            else if (a.y < YY && !src[a.y + 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else
                stackhandler.PopStack(&s);
        } else {
            src[a.y][a.x] = 2;
            stackhandler.PopStack(&s);
        }
    }
}

/****** 找寻左边界的特殊点 ******/
#define searchleftmapPoint(x, y)                                                                         \
    {                                                                                                    \
        /*** RT RB 为leftlin的最右侧的坐标 竖直边界时 RB为起点 RT为终点 ***/          \
        if (x > RB_X(II.num_lm)) {                                                                       \
            RB_X(II.num_lm) = x;                                                                         \
            RB_Y(II.num_lm) = y;                                                                         \
            RT_X(II.num_lm) = x;                                                                         \
            RT_Y(II.num_lm) = y;                                                                         \
        } else if (x == RB_X(II.num_lm)) {                                                               \
            if (y < RB_Y(II.num_lm)) {                                                                   \
                RB_Y(II.num_lm) = y;                                                                     \
            } else if (y > RT_Y(II.num_lm)) {                                                            \
                RT_Y(II.num_lm) = y;                                                                     \
            }                                                                                            \
        }                                                                                                \
                                                                                                         \
        if (y < BR_Y(II.num_lm)) {                                                                       \
            BR_X(II.num_lm) = x;                                                                         \
            BR_Y(II.num_lm) = y;                                                                         \
        } else if (y == BR_Y(II.num_lm) &&                                                               \
                   x > BR_X(II.num_lm)) /***取一行中的最右侧的点***/                           \
        {                                                                                                \
            BR_X(II.num_lm) = x;                                                                         \
        }                                                                                                \
        /*** 从QT上来看 大部分情况：TR为leftmap中 左边界的最远延伸点（右弯时为 \
         * 59 46）***/                                                                                  \
        /****leftmap全白时:TR为 0 0 ***/                                                             \
        /*** 左弯时：从BR开始连续的到TR ***/                                                  \
        if (y > TR_Y(II.num_lm)) {                                                                       \
            TR_X(II.num_lm) = x;                                                                         \
            TR_Y(II.num_lm) = y;                                                                         \
        } else if (y == TR_Y(II.num_lm) && x > TR_X(II.num_lm)) {                                        \
            TR_X(II.num_lm) = x;                                                                         \
        }                                                                                                \
    }

// 功能与实现方式与 searchleftmapPoint()相同 确定RegionInfoLeft
#define searchleftPoint(x, y, r)                                      \
    {                                                                 \
        if (x > r->right_bottom.x) {                                  \
            r->right_bottom.x = x;                                    \
            r->right_bottom.y = y;                                    \
            r->right_top.x = x;                                       \
            r->right_top.y = y;                                       \
        } else if (x == r->right_bottom.x) {                          \
            if (y < r->right_bottom.y) {                              \
                r->right_bottom.y = y;                                \
            } else if (y > r->right_top.y) {                          \
                r->right_top.y = y;                                   \
            }                                                         \
        }                                                             \
        if (y < r->bottom_right.y) {                                  \
            r->bottom_right.x = x;                                    \
            r->bottom_right.y = y;                                    \
        } else if (y == r->bottom_right.y && x > r->bottom_right.x) { \
            r->bottom_right.x = x;                                    \
        }                                                             \
        if (y > r->top_right.y) {                                     \
            r->top_right.x = x;                                       \
            r->top_right.y = y;                                       \
        } else if (y == r->top_right.y && x > r->top_right.x) {       \
            r->top_right.x = x;                                       \
        }                                                             \
    }

#define searchrightPoint(x, y, r)                                   \
    {                                                               \
        if (x < r->left_bottom.x) {                                 \
            r->left_bottom.x = x;                                   \
            r->left_bottom.y = y;                                   \
            r->left_top.x = x;                                      \
            r->left_top.y = y;                                      \
        } else if (x == r->left_bottom.x) {                         \
            if (y < r->left_bottom.y) {                             \
                r->left_bottom.y = y;                               \
            } else if (y > r->left_top.y) {                         \
                r->left_top.y = y;                                  \
            }                                                       \
        }                                                           \
        if (y < r->bottom_left.y) {                                 \
            r->bottom_left.x = x;                                   \
            r->bottom_left.y = y;                                   \
        } else if (y == r->bottom_left.y && x < r->bottom_left.x) { \
            r->bottom_left.x = x;                                   \
        }                                                           \
        if (y > r->top_left.y) {                                    \
            r->top_left.x = x;                                      \
            r->top_left.y = y;                                      \
        } else if (y == r->top_left.y && x < r->top_left.x) {       \
            r->top_left.x = x;                                      \
        }                                                           \
    }

// 与 searchleftmapPoint(x,y) 类似 找寻 右边界的各种特殊点
#define searchrightmapPoint(x, y)                                 \
    {                                                             \
        if (x < LB_X(II.num_rm)) {                                \
            LB_X(II.num_rm) = x;                                  \
            LB_Y(II.num_rm) = y;                                  \
            LT_X(II.num_rm) = x;                                  \
            LT_Y(II.num_rm) = y;                                  \
        } else if (x == LB_X(II.num_rm)) {                        \
            if (y < LB_Y(II.num_rm)) {                            \
                LB_Y(II.num_rm) = y;                              \
            } else if (y > LT_Y(II.num_rm)) {                     \
                LT_Y(II.num_rm) = y;                              \
            }                                                     \
        }                                                         \
        if (y < BL_Y(II.num_rm)) {                                \
            BL_X(II.num_rm) = x;                                  \
            BL_Y(II.num_rm) = y;                                  \
        } else if (y == BL_Y(II.num_rm) && x < BL_X(II.num_rm)) { \
            BL_X(II.num_rm) = x;                                  \
        }                                                         \
        if (y > TL_Y(II.num_rm)) {                                \
            TL_X(II.num_rm) = x;                                  \
            TL_Y(II.num_rm) = y;                                  \
        } else if (y == TL_Y(II.num_rm) && x < TL_X(II.num_rm)) { \
            TL_X(II.num_rm) = x;                                  \
        }                                                         \
    }

// 从图像src提取信息 映射到到图像dst中
void ImageHandler::getRegionInfoLeft(uint8_t x,
                                     uint8_t y,
                                     uint8_t src[][XM],
                                     uint8_t dst[][XM],
                                     RegionInfoLeft* r) {
    Point a;
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    stackhandler.PushStack(&s, x, y);

    while (!stackhandler.EmptyStack(&s)) {
        a = s.data[s.top];
        x = a.x;
        y = a.y;

        if (src[a.y][a.x]) {
            if (dst[a.y][a.x] == 0) {
                ++r->numCnt;
                searchleftPoint(x, y, r);
                dst[a.y][a.x] = 1;
            }

            if (a.x > 0 && !dst[a.y][a.x - 1])
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < XX && !dst[a.y][a.x + 1])
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y > 0 && !dst[a.y - 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y - 1);
            else if (a.y < YY && !dst[a.y + 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else
                stackhandler.PopStack(&s);
        } else {
            dst[a.y][a.x] = 2;
            stackhandler.PopStack(&s);
        }
    }
}

// 功能同 同getRegionInfoLeft
void ImageHandler::getRegionInfoRight(uint8_t x,
                                      uint8_t y,
                                      uint8_t src[][XM],
                                      uint8_t dst[][XM],
                                      RegionInfoRight* r) {
    Point a;
    a.x = x;
    a.y = y;
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    stackhandler.PushStack(&s, x, y);
    while (!stackhandler.EmptyStack(&s)) {
        Point a;
        a = s.data[s.top];
        x = a.x;
        y = a.y;
        if (src[a.y][a.x]) {
            if (dst[a.y][a.x] == 0) {
                ++r->numCnt;
                searchrightPoint(x, y, r);
                dst[a.y][a.x] = 1;
            }
            if (a.x > 0 && !dst[a.y][a.x - 1])
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < XX && !dst[a.y][a.x + 1])
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y > 0 && !dst[a.y - 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y - 1);
            else if (a.y < YY && !dst[a.y + 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y + 1);

            else
                stackhandler.PopStack(&s);
        } else {
            dst[a.y][a.x] = 2;
            stackhandler.PopStack(&s);
        }
    }
}

/****** leftmap basemap ******
 ******  赋值 leftmap   ******/
void ImageHandler::searchleftmap(uint8_t x, uint8_t y) {
    Point a;
    a.x = x;
    a.y = y;
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    if (basemap[a.y][a.x])
        stackhandler.PushStack(&s, x, y);
    else  // 在黑色区域里搜，遇白为边界
        leftmap[y][x] =
            2;  // 0白前景 1黑背景 2红线 后加：3粉线 4蓝线 254绿线 255灰线

    while (!stackhandler.EmptyStack(&s)) {
        Point a;
        a = s.data[s.top];
        if (basemap[a.y][a.x]) {
            if (leftmap[a.y][a.x] == 0) {
                searchleftmapPoint(a.x, a.y);  // 查找各种特殊点
                leftmap[a.y][a.x] = 1;         // 将黑背景设为1
                insidemap[a.y][a.x] = 0;
                if (II._top < a.y)
                    II._top = a.y;

                if (rightmap[a.y][a.x])
                    ++II.repeatNum;

                ++II.lnum[II.num_lm];
            }

            /* leftmap 未曾赋值 或为非白 */
            if (a.x > 0 && !leftmap[a.y][a.x - 1])
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < XX && !leftmap[a.y][a.x + 1])
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y < YY && !leftmap[a.y + 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else if (
                a.y > 0 &&
                !leftmap
                    [a.y - 1]
                    [a.x])  // 向下限制。你从左下开始搜左图，怎么可能有向下的部分。除非搜到右边去了
            {
                if (a.y <= II.top)
                    stackhandler.PushStack(&s, a.x, a.y - 1);
                // 检验两个map边界的重合点：basemap 的边界在 left/rightmap 之外
                // 所以重合的部分即为 left/rightmap 中的 noDown 点
                // 禁止向下搜索，不然可能因为赛道左右侧图像在顶部(因近大远小)有一小部分联通，导致在搜左图时连着搜掉了右图的黑色区域，那样右图到时候会全白
                else if (basemap[a.y - 1][a.x] && noDown.num < XM) {
                    noDown.point[noDown.num].x = a.x;
                    noDown.point[noDown.num].y = a.y - 1;
                    ++noDown.num;
                    stackhandler.PopStack(&s);
                } else if (!basemap[a.y - 1][a.x])  // 白色
                    leftmap[a.y - 1][a.x] = 2;
                else
                    stackhandler.PopStack(&s);

            } else
                stackhandler.PopStack(&s);
        } else {
            leftmap[a.y][a.x] = 2;
            stackhandler.PopStack(&s);
        }
    }
}

/****** 赋值 rightmap ******/
void ImageHandler::searchrightmap(uint8_t x, uint8_t y) {
    Point a;
    a.x = x;
    a.y = y;
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    if (basemap[a.y][a.x])
        stackhandler.PushStack(&s, x, y);
    else
        rightmap[y][x] = 2;  // 白

    while (!stackhandler.EmptyStack(&s)) {
        Point a;
        a = s.data[s.top];
        if (basemap[a.y][a.x]) {
            if (rightmap[a.y][a.x] == 0)  // 初期 初始化为0
            {
                searchrightmapPoint(a.x, a.y);
                rightmap[a.y][a.x] = 1;  // 将黑背景设为1
                insidemap[a.y][a.x] = 0;
                if (II._top < a.y)
                    II._top = a.y;
                if (leftmap[a.y][a.x])
                    ++II.repeatNum;
                ++II.rnum[II.num_rm];
            }

            // 在蓝白 即边界内搜图
            if (a.x > 0 && !rightmap[a.y][a.x - 1])
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < XX && !rightmap[a.y][a.x + 1])
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y < YY && !rightmap[a.y + 1][a.x])
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else if (a.y > 0 && !rightmap[a.y - 1][a.x]) {
                if (a.y <= II.top)
                    stackhandler.PushStack(&s, a.x, a.y - 1);
                else if (basemap[a.y - 1][a.x] && noDown.num < XM) {
                    noDown.point[noDown.num].x = a.x;
                    noDown.point[noDown.num].y = a.y - 1;
                    ++noDown.num;
                    stackhandler.PopStack(&s);
                } else if (!basemap[a.y - 1][a.x])
                    rightmap[a.y - 1][a.x] = 2;
                else
                    stackhandler.PopStack(&s);
            } else
                stackhandler.PopStack(&s);
        } else {
            rightmap[a.y][a.x] = 2;
            stackhandler.PopStack(&s);
        }
    }
}

/****** 为 deletemap赋值 ******/
void ImageHandler::searchdeletemap(uint8_t x, uint8_t y) {
    /*  Point a;
          a.x = x; a.y = y;*/
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    stackhandler.PushStack(&s, x, y);
    while (!stackhandler.EmptyStack(&s)) {
        Point a;
        a = s.data[s.top];
        if (basemap[a.y][a.x])  // 白外
        {
            if (deletemap[a.y][a.x] == 0) {
                ++II.dnum_all;
                deletemap[a.y][a.x] = 1;  // 将黑背景设为1
                insidemap[a.y][a.x] =
                    0;  // 置insidemap 为0白 噪点在insidemap 仍为1黑
            }
            // deletemap 中的红线在 basemap 内
            if (a.x > 0 &&
                deletemap[a.y][a.x - 1] == 0)  // 白内的 deletemap仍为0
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < XX && deletemap[a.y][a.x + 1] == 0)
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y > 0 && deletemap[a.y - 1][a.x] == 0)
                stackhandler.PushStack(&s, a.x, a.y - 1);
            else if (a.y < YY && deletemap[a.y + 1][a.x] == 0)
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else
                stackhandler.PopStack(&s);
        } else {
            deletemap[a.y][a.x] = 2;
            stackhandler.PopStack(&s);
        }
    }
}

void ImageHandler::searchdeletemap2(uint8_t x, uint8_t y) {
    stack s;
    Point data[3000];
    stackhandler.InitStack(&s, 3000, data);
    stackhandler.PushStack(&s, x, y);

    while (!stackhandler.EmptyStack(&s)) {
        Point a;
        a = s.data[s.top];
        if (basemap[a.y][a.x]) {
            if (deletemap[a.y][a.x] == 0) {
                ++II.dnum_all;
                ++II.dnum_top;
                deletemap[a.y][a.x] = 254;  // 绿色区域
                insidemap[a.y][a.x] = 0;
            }
            /*** 遇到leftmap和rightmap的黑点停止  ***/
            if (a.x > 0 && !deletemap[a.y][a.x - 1] &&
                leftmap[a.y][a.x - 1] != 1 && rightmap[a.y][a.x - 1] != 1)
                stackhandler.PushStack(&s, a.x - 1, a.y);
            else if (a.x < XX && !deletemap[a.y][a.x + 1] &&
                     leftmap[a.y][a.x + 1] != 1 && rightmap[a.y][a.x + 1] != 1)
                stackhandler.PushStack(&s, a.x + 1, a.y);
            else if (a.y < YY && !deletemap[a.y + 1][a.x] &&
                     leftmap[a.y + 1][a.x] != 1 && rightmap[a.y + 1][a.x] != 1)
                stackhandler.PushStack(&s, a.x, a.y + 1);
            else if (a.y > 0 && !deletemap[a.y - 1][a.x] &&
                     leftmap[a.y - 1][a.x] != 1 && rightmap[a.y - 1][a.x] != 1)
                stackhandler.PushStack(&s, a.x, a.y - 1);

            else
                stackhandler.PopStack(&s);
        } else {
            deletemap[a.y][a.x] = 253;
            stackhandler.PopStack(&s);
        }
    }
}

/*** 判定起跑线 一行之内黑白跳变频繁 ***/
uint8_t ImageHandler::cnt_WBW_num(uint8_t x1,
                                  uint8_t x2,
                                  uint8_t y,
                                  uint8_t map[][XM]) {
    if (x1 > XX || x2 > XX || y > YY) {
        return 255;
    }
    uint8_t _cnt = 0;
    uint8_t color = 0;
    uint8_t j = y;
    if (x1 < x2) {
        uint8_t i = x1;

        do {
            if (map[j][i] != color) {
                ++_cnt;
                color = !color;
            }
            i++;
        } while (i <= x2);
        return _cnt / 2;
    } else if (x1 > x2) {
        int8_t i = x2;
        do {
            if (map[j][i] != color) {
                ++_cnt;
                color = !color;
            }
            i--;
        } while (i <= x1);
        return _cnt / 2;
    }
}

/* 在一列之内判定黑白跳变 */
uint8_t ImageHandler::y_cnt_WBW_num(uint8_t y1,
                                    uint8_t y2,
                                    uint8_t x,
                                    uint8_t map[][XM]) {
    uint8_t _cnt = 0;
    uint8_t color = 0;

    for (uint8_t j = y1; j < y2; j++) {
        if (map[j][x] != color) {
            ++_cnt;
            color = !color;
        }
    }

    return _cnt / 2;
}

/****** 判定是否还在搜图 ******/
uint8_t ImageHandler::isSearch(uint8_t x,
                               uint8_t y,
                               uint8_t map[][XM])  // 判断此区域是否已被Xmap搜过
{
    for (uint8_t k = y + 1; k < YM; ++k) {
        if (!basemap[k][x])  // 大边界或背景 表明left/rightmap 仍在搜图 跳出循环
                             // return 1
        {
            break;
        }
        if (map[k][x])  // 前景或小边界 搜图结束
        {
            return 0;
        }
    }
    return 1;
}

/******  从底部开始向上 找寻此列的第一个特定像素点（返回行数） ******/
uint8_t ImageHandler::getMapYMin_Col(uint8_t x,
                                     uint8_t map[][XM],
                                     uint8_t value) {
    for (uint8_t i = 0; i < YM; ++i) {
        if (map[i][x] == value) {
            return i;
        }
    }
    return YM;
}

/****** 从顶部开始向下 找寻此列的第一个特定像素点（返回行数） ******/
uint8_t ImageHandler::getMapYMax_Col(uint8_t x,
                                     uint8_t map[][XM],
                                     uint8_t value) {
    for (uint8_t i = 0; i < YM; ++i) {
        if (map[YY - i][x] == value)  // max 从顶部开始检索
        {
            return YY - i;
        }
    }
    return YM;
}

/****** basemap 从给定行向上 找寻此列的第一个边界 未找到则返回 YM ******/
uint8_t ImageHandler::getMapYMin_Col2(uint8_t x, uint8_t y, uint8_t map[][XM]) {
    for (uint8_t i = y; i < YM; ++i) {
        if (map[i][x] == 1)  // basemap 黑背景
            break;
        if (map[i][x] == 2)  // basemap 红边界
            return i;
    }
    return YM;
}

/* 在指定列 从起始行到终止行 计算连续点的个数 */
uint8_t ImageHandler::getMapY_Samevalue_Count(uint8_t x,
                                              uint8_t start_y,
                                              uint8_t end_y,
                                              uint8_t map[][XM],
                                              uint8_t value) {
    uint8_t count = 0, max_count = 0;
    for (uint8_t i = start_y; i <= end_y; ++i) {
        if (map[i][x] != value) {
            count = 0;
            continue;
        } else {
            ++count;
            max_count = count;
        }
    }

    return max_count;
}

// 在指定行 从起始列到终止列 计算点的个数
uint8_t ImageHandler::getMapX_Samevalue_Count(uint8_t y,
                                              uint8_t start_x,
                                              uint8_t end_x,
                                              uint8_t map[][XM],
                                              uint8_t value) {
    uint8_t count = 0, max_count = 0, i;

    for (i = start_x; i <= end_x; ++i) {
        if (map[y][i] != value) {
            count = 0;
            continue;
        } else {
            ++count;
            max_count = count;
        }
    }

    return max_count;
}

/* 从计算机向左 计算左侧赛道宽度 */
uint8_t ImageHandler::get_RoadWidth_left(uint8_t y,
                                         uint8_t start_x,
                                         uint8_t map[][XM]) {
    uint8_t count = 0;

    for (int8_t i = start_x; i > 0; i -= 2) {
        if (map[y][i] != 1) {
            count += 2;
            continue;
        } else if (map[y][i] == 1 && count) {
            break;
        }
    }

    if (!count)
        return XM;
    else
        return count;
}

/* 从计算机往右 计算右侧赛道宽度 */
uint8_t ImageHandler::get_RoadWidth_right(uint8_t y,
                                          uint8_t start_x,
                                          uint8_t map[][XM]) {
    uint8_t count = 0;

    for (uint8_t i = start_x; i < XM; i += 2) {
        if (map[y][i] != 1) {
            count += 2;
            continue;
        } else if (map[y][i] == 1 && count) {
            break;
        }
    }

    if (!count)
        return XM;
    else
        return count;
}

/* 45度向右上 检测可通路径 */
uint8_t ImageHandler::get_45degree_RoadWidth(uint8_t b, uint8_t map[][XM]) {
    uint8_t count = 0, j = b, i;

    for (i = 0; i < XM; i += 2) {
        if (map[j][i] != 1) {
            j += 2;
            count += 2;
            continue;
        } else if (map[j][i] == 1 && count) {
            break;
        }
    }

    if (!count)
        return XM;
    else
        return count;
}

/* 计数此行 连续出现的点的个数 */
uint8_t ImageHandler::getMapX_Samevalue_Sum(uint8_t y,
                                            uint8_t start_x,
                                            uint8_t end_x,
                                            uint8_t map[][XM],
                                            uint8_t value) {
    uint8_t count = 0, max_count, sum_count = 0, i;

    for (i = start_x; i <= end_x; ++i) {
        if (map[y][i] != value) {
            if (max_count)
                sum_count += max_count;
            count = 0;
            max_count = 0;
            continue;
        } else {
            ++count;
            max_count = count;
        }
    }

    return sum_count;
}

/*** 此列上 有多个红色边界 返回从下至上 第一个边界的横坐标 未找到则返回 YM ***/
uint8_t ImageHandler::getMap_YFirst_Col(uint8_t x,
                                        uint8_t y,
                                        uint8_t map[][XM]) {
    for (uint8_t i = y; i < YM; ++i)
        if (map[i][x] == 2 && map[i + 1][x] != 2) {
            return i;
        }
    return YM;
}

/*** 找寻此列从特定行数往下的第一条红色边界 ***/
uint8_t ImageHandler::getMap_YMax_first_Col(uint8_t x,
                                            uint8_t y,
                                            uint8_t map[][XM]) {
    for (int8_t i = y; i >= 0; --i) {
        if (map[i][x] == 2 && map[i - 1][x] == 0) {
            return i;
        }
    }

    return YM;
}
/*** 此列上 有多个红色边界 返回从下至上 第二个边界的横坐标 未找到则返回 YM ***/
uint8_t ImageHandler::getMap_YSecond_Col(uint8_t x,
                                         uint8_t y,
                                         uint8_t map[][XM]) {
    uint8_t temp = 0;
    for (uint8_t i = y; i < YM; ++i)
        if (map[i][x] == 2 && map[i + 1][x] != 2) {
            temp = i;
            break;
        }

    for (uint8_t j = temp + 2; j < YM; ++j)
        if (map[j][x] == 2)
            return j;

    return YM;
}

/*** 找寻此列从特定行数往上的第一条红色边界 ***/
uint8_t ImageHandler::getMap_Yset_Col(uint8_t x, uint8_t y, uint8_t map[][XM]) {
    for (uint8_t j = y; j < YM; ++j) {
        if (map[x][j] == 2 && map[x][j + 1] == 1) {
            return j;
        }
    }

    return YM;
}

/* 此列上 有多个红色边界 返回从底至顶 第三个边界的横坐标 未找到则返回 YM */
uint8_t ImageHandler::getMap_YThird_Col(uint8_t x,
                                        uint8_t y,
                                        uint8_t map[][XM]) {
    uint8_t first = 0, second = 0, i = 0;

    for (i = y; i < YM; ++i)
        if (map[i][x] == 2 && map[i + 1][x] != 2) {
            first = i;
            break;
        }

    for (i = first + 2; i < YM; ++i)
        if (map[i][x] == 2 && map[i + 1][x] != 2) {
            second = i;
            break;
        }

    for (i = second + 2; i < YM; ++i)
        if (map[i][x] == 2)
            return i;

    return YM;
}

/* 此行上 有多个红色边界 返回从右至左 第二个边界的横坐标 未找到则返回 XM */
uint8_t ImageHandler::getMap_XSecond_ROW(uint8_t y,
                                         uint8_t x,
                                         uint8_t map[][XM]) {
    int8_t temp = 0, i = 0;

    for (i = x; i > 0; --i)
        if (map[y][i] == 2) {
            temp = i;
            break;
        }

    for (i = temp - 2; i > 0; --i)
        if (map[y][i] == 2)
            return i;

    return XM;
}

/* 竖直路径 有则返回 1 无则返回 0 */
uint8_t ImageHandler::straight_road(uint8_t x,
                                    uint8_t end_y,
                                    uint8_t map[][XM]) {
    if (end_y >= YM)
        end_y = YY;
    uint8_t flag = 1;
    for (uint8_t i = 0; i < end_y; ++i)
        if (map[i][x] == 1) {
            flag = 0;
            break;
        }

    return flag;
}

/****** 从最右侧列开始 找寻此行的第一个特定像素点（返回横坐标） ******/
uint8_t ImageHandler::getMapXMax_Row(uint8_t y,
                                     uint8_t map[][XM],
                                     uint8_t value) {
    for (uint8_t i = 0; i < XM; ++i) {
        if (map[y][XX - i] == value) {
            return XX - i;
        }
    }
    return XM;
}

/*** 从左往右 找寻此行的左边界 满足右白 ***/
uint8_t ImageHandler::getMapX_Leftline(uint8_t y,
                                       uint8_t start_x,
                                       uint8_t map[][XM],
                                       uint8_t value) {
    for (uint8_t i = start_x; i < XX; ++i) {
        if (map[y][i] == value && map[y][i + 1] == 0) {
            return i;
        }
    }
    return XM;
}

/*** 从右往左 找寻此行的右边界 满足左白***/
uint8_t ImageHandler::getMapX_Rightline(uint8_t y,
                                        uint8_t start_x,
                                        uint8_t map[][XM],
                                        uint8_t value) {
    for (int8_t i = start_x; i >= 0; --i) {
        if (map[y][i] == value && map[y][i - 1] == 0) {
            return i;
        }
    }
    return XM;
}

/****** 从最左侧列开始  找寻此行的第一个特定像素点（返回横坐标） ******/
uint8_t ImageHandler::getMapXMin_Row(uint8_t y,
                                     uint8_t map[][XM],
                                     uint8_t value) {
    for (uint8_t i = 0; i < XM; ++i) {
        if (map[y][i] == value) {
            return i;
        }
    }
    return XM;
}

/*** 在指定行 从指定列往右找寻特定颜色的点 ***/
uint8_t ImageHandler::getMapX_Start_Row(uint8_t y,
                                        uint8_t start_x,
                                        uint8_t map[][XM],
                                        uint8_t value) {
    for (uint8_t i = start_x; i < XM; ++i) {
        if (map[y][i] == value)
            return i;
    }
    return XM;
}

/*** 在指定行 从指定列往左找寻特定颜色的点 ***/
uint8_t ImageHandler::getMapX_Start_Row_2(uint8_t y,
                                          uint8_t start_x,
                                          uint8_t map[][XM],
                                          uint8_t value) {
    for (int8_t i = start_x; i > 0; --i) {
        if (map[y][i] == value)
            return i;
    }
    return XM;
}

/*** 在指定列 从指定行开始找寻特定颜色的点 ***/
uint8_t ImageHandler::getMapY_Start_Line(uint8_t x,
                                         uint8_t start_y,
                                         uint8_t map[][XM],
                                         uint8_t value) {
    for (uint8_t i = start_y; i < YM; ++i) {
        if (map[i][x] == value)
            return i;
    }
    return YM;
}

/****** 横坐标方向上 非黑->黑->非黑 ******/
uint8_t ImageHandler::X_WBW_Detect(int8_t x1,
                                   int8_t x2,
                                   uint8_t y,
                                   uint8_t map[][XM],
                                   uint8_t flag) {
    if (flag == goRight)
        for (int8_t j = x1; j <= x2; ++j)  // 从左向右遍历
        {
            if (map[y][j] != 1) {
                for (int8_t k = j + 1; k <= x2 && k < XM; ++k) {
                    if (map[y][k] == 1) {
                        for (int8_t m = k + 1; m <= x2; ++m) {
                            // 非黑->黑->非黑
                            if (map[y][m] != 1)
                                return 1;
                        }
                        break;
                    }
                }
                break;
            }
        }
    if (flag == goLeft)
        for (int8_t j = x1; j >= x2 && j >= 0; --j)  // 从右向左遍历
        {
            if (map[y][j] != 1) {
                for (int8_t k = j - 1; k >= x2; --k) {
                    if (map[y][k] == 1) {
                        for (int8_t m = k - 1; m >= x2; --m) {
                            // 非黑<-黑<-非黑
                            if (map[y][m] != 1)
                                return 1;
                        }
                        break;
                    }
                }
                break;
            }
        }
    return 0;
}

/****** 横坐标方向上 黑->白->黑 ******/
uint8_t ImageHandler::X_BWB_Detect(int8_t x1,
                                   int8_t x2,
                                   uint8_t y,
                                   uint8_t map[][XM],
                                   uint8_t flag) {
    if (flag == goRight)
        for (int8_t j = x1; j <= x2; ++j)  // 从左向右遍历
        {
            if (map[y][j] == 1) {
                for (int8_t k = j + 1; k <= x2 && k < XM; ++k) {
                    if (map[y][k] == 0) {
                        for (int8_t m = k + 1; m <= x2; ++m) {
                            // 黑->白->黑
                            if (map[y][m] == 1)
                                return 1;
                        }
                        break;
                    }
                }
                break;
            }
        }
    if (flag == goLeft)
        for (int8_t j = x1; j >= x2 && j >= 0; --j)  // 从右向左遍历
        {
            if (map[y][j] == 1) {
                for (int8_t k = j - 1; k >= x2; --k) {
                    if (map[y][k] == 0) {
                        for (int8_t m = k - 1; m >= x2; --m) {
                            // 黑<-白<-黑
                            if (map[y][m] == 1)
                                return 1;
                        }
                        break;
                    }
                }
                break;
            }
        }
    return 0;
}

/****** basemap专用 纵坐标方向（从y1向上到y2) 白黑白判定 ******/
uint8_t ImageHandler::Y_WBW_Detect(uint8_t y1,
                                   uint8_t y2,
                                   uint8_t x,
                                   uint8_t map[][XM]) {
    for (uint8_t i = y1; i <= y2; ++i) {
        if (map[i][x] == 0)  // basemap 0为白
        {
            for (uint8_t k = i + 1; k <= y2; ++k) {
                if (map[k][x] == 1)  // basemap 1为黑
                {
                    for (uint8_t m = k + 1; m <= y2; ++m) {
                        if (map[m][x] == 0) {
                            return 1;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    return 0;
}

/****** basemap专用 纵坐标方向（从y1向上到y2) 红黑红判定 ******/
uint8_t ImageHandler::Y_RBR_Detect(uint8_t y1,
                                   uint8_t y2,
                                   uint8_t x,
                                   uint8_t map[][XM]) {
    for (uint8_t i = y1; i <= y2; ++i) {
        if (map[i][x] == 2)  // basemap 2为红边界
        {
            for (uint8_t k = i + 1; k <= y2; ++k) {
                if (map[k][x] == 1)  // basemap 1为黑背景
                {
                    for (uint8_t m = k + 1; m <= y2; ++m) {
                        if (map[m][x] == 2) {
                            return 1;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    return 0;
}

/****** straight_judge 判断边界为直线（可增加严格系数 num）还是曲线 直线为1
 * 曲线为0 ******/
uint8_t ImageHandler::strJudge(uint8_t x1,
                               uint8_t y1,
                               uint8_t x2,
                               uint8_t y2,
                               uint8_t map[][XM],
                               uint8_t sy1,
                               uint8_t sy2,
                               int8_t limit,
                               uint8_t errorNum) {
    int8_t x;
    uint8_t num = 0;
    if (y1 >= y2 || sy1 < y1 || sy2 > y2)  // y1<=sy1<sy2<=y2
    {
        return 0;
    }
    float k = (float)(x2 - x1) / (y2 - y1);  // 斜率

    for (uint8_t i = sy1; i < sy2; ++i) {
        x = (uint8_t)(x1 + k * (i - y1) + 0.5);  // 按照斜率k一行行往上补像素点
        if (map[i][x] == 2)  // 边界停止 进入下一循环 （往上一行 继续补像素点）
        {
            continue;
        } else {
            uint8_t flag = 0;
            for (int8_t j = -2; j <= 2; ++j) {
                if (x + j < 0 || x + j > XX)  // 图像外 进入下一循环（往上一行）
                    continue;
                if (map[i][x + j] == 2)  // 左右邻域为2内存在 边界
                {
                    flag = 1;
                    if (j < -limit || j > limit) {
                        ++num;
                    }
                    break;
                }
            }
            if (flag == 0) {
#ifdef T_REX_QT_DEBUG
                for (uint8_t i = y1; i < y2; ++i) {
                    // 入环岛前将左上角点和最低点用直线相连 灰线
                    map[i][(uint8_t)(x1 + k * (i - y1))] =
                        255;  // 判定为曲线 用灰线连接 退出
                }
#endif
                return 0;
            }
        }
    }

#ifdef T_REX_QT_DEBUG
    for (uint8_t i = y1; i < y2; ++i) {
        map[i][(uint8_t)(x1 + k * (i - y1))] = 254;  // 类直线则画 绿线
    }
#endif

    if (num > errorNum) {
        return 0;
    }
    return 1;
}

// 与strJudge功能实现相同 只是改为横坐标方向
uint8_t ImageHandler::strJudge_X(uint8_t x1,
                                 uint8_t y1,
                                 uint8_t x2,
                                 uint8_t y2,
                                 uint8_t map[][XM],
                                 uint8_t sx1,
                                 uint8_t sx2,
                                 int8_t limit,
                                 uint8_t errorNum) {
    int8_t y;
    uint8_t num = 0;
    if (x1 >= x2 || sx1 < x1 || sx2 > x2) {
        return 0;
    }
    float k = (float)(y2 - y1) / (x2 - x1);

    for (uint8_t i = sx1; i < sx2; ++i) {
        y = (uint8_t)(y1 + k * (i - x1) + 0.5);
        if (map[y][i] == 2) {
            continue;
        } else {
            uint8_t flag = 0;
            for (int8_t j = -2; j <= 2; ++j) {
                if (y + j < 0 || y + j > YY) {
                    continue;
                }
                if (map[y + j][i] == 2) {
                    flag = 1;
                    if (j < -limit || j > limit) {
                        ++num;
                    }
                    break;
                }
            }
            if (flag == 0) {
                // #ifdef BOOM4_QT_DEBUG
                //                 for (uint8_t i = x1; i < x2; ++i)
                //                 {
                //                     map[(uint8_t)(y1 + k * (i - x1) +
                //                     0.5)][i] = 255;
                //                 }
                // #endif
                return 0;
            }
        }
    }
    // #ifdef BOOM4_QT_DEBUG
    //     for (uint8_t i = x1; i < x2; ++i)
    //     {
    //         map[(uint8_t)(y1 + k * (i - x1) + 0.5)][i] = 254;
    //     }
    // #endif
    if (num > errorNum) {
        return 0;
    }
    return 1;
}

// 未被调用
float ImageHandler::getLineK(Point array[], uint8_t num) {
    float sumx = 0;
    float sumy = 0;
    float sumxy = 0;
    float sumxx = 0;
    float x;
    float y;

    for (uint8_t i = 0; i < num; ++i) {
        y = array[i].y;
        x = array[i].x;
        sumx += x;
        sumy += y;
        sumxy += x * y;
        sumxx += x * x;
    }

    if (abs(num * sumxx - sumx * sumx) < 1e-6) {
        return 10000;
    } else {
        return (num * sumxy - sumx * sumy) / (num * sumxx - sumx * sumx);
    }
}

/****** 补线（直线） (x2,y2)--->(x1,y1) y2<y1 ******/
// void drawline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,
//               uint8_t map[][XM])
//{
//     if (y1 == y2)   //功能函数 无返回
//         return;
//     float k = (float)(x1 - x2) / (y1 - y2); //斜率
//     for (uint8_t i = y2; i <= y1; ++i)
//     {
//         map[i][(uint8_t)(x2 + k * (i - y2))] = 2;
//     }
// }

/****** 补线（直线） (x2,y2)--->(x1,y1) y2<y1 ******/
void ImageHandler::draw_straight(uint8_t x1,
                                 uint8_t y1,
                                 uint8_t x2,
                                 uint8_t y2,
                                 uint8_t map[][XM]) {
    if (y1 == y2)  // 功能函数 无返回
        return;
    float k = (float)(x1 - x2) / (y1 - y2);  // 斜率
    for (uint8_t i = y2; i <= y1; ++i) {
        map[i][(uint8_t)(x2 + k * (i - y2))] = 2;
        map[i][(uint8_t)(x2 + k * (i - y2) + 1)] = 2;
    }
}

/*** 补直线  下(x2,y2)-->上(x1,y1) y2<y1 ***/
void ImageHandler::draw_straight_line(uint8_t x1,
                                      uint8_t y1,
                                      uint8_t x2,
                                      uint8_t y2,
                                      uint8_t map[][XM]) {
    uint8_t i;
    if (y1 == y2)
        return;

    float k = (float)(x1 - x2) / (y1 - y2);  // 斜率
    for (i = y2; i <= y1; i++)
        map[i][(uint8_t)(x2 + k * (i - y2))] = 2;
}

/****** 补线（曲线） x1>x2******/
void ImageHandler::drawline2(uint8_t x1,
                             uint8_t y1,
                             uint8_t x2,
                             uint8_t y2,
                             uint8_t map[][XM],
                             uint8_t n) {
    uint8_t lasty = YM;
    if (x1 == x2)
        return;
    float k = (float)(y1 - y2) / (x1 - x2);        // 斜率
    float mid = (float)(x1 + x2) / 2;              // 中点
    uint8_t climax = (uint8_t)n * (x1 - x2) / XM;  // n=12 放大系数

    for (uint8_t j = x2; j <= x1; ++j) {
        /* 特点 在起始点和终点 增高量为0 中点处最大 */
        uint8_t temp = (uint8_t)((j - x2) * k + y2 +  // 直线下的纵坐标
                                 climax -
                                 climax * (2 * fabs(j - mid) / (x1 - x2)) *
                                     (2 * fabs(j - mid) /
                                      (x1 - x2)));  // 二次曲线与直线的高度差
        if (temp > YY)
            continue;
        map[temp][j] = 3;                   // 补线
        int8_t gap = (int8_t)temp - lasty;  // 这次和上次的差

        if (gap > 1 && lasty != YM) {
            while (--gap) {
                map[temp - gap][j] = 3;
            }
        }
        if (gap < -1 && lasty != YM) {
            while (++gap) {
                map[temp - gap][j - 1] = 3;
            }
        }

        lasty = temp;
    }
}

/*        n 固定 n=12              *
 *    与drawline2 功能实现相同      *
 *      BOOM4那届 走断路用         */
void ImageHandler::drawline3(uint8_t x1,
                             uint8_t y1,
                             uint8_t x2,
                             uint8_t y2,
                             uint8_t map[][XM])  // x1>x2
{
    uint8_t lasty = YM;
    if (x1 == x2)
        return;
    float k = (float)(y1 - y2) / (x1 - x2);
    float mid = (float)(x1 + x2) / 2;
    uint8_t climax = (uint8_t)12 * (x1 - x2) / XM;  // 12.0
    for (uint8_t j = x2; j <= x1; ++j) {
        uint8_t temp = (uint8_t)((j - x2) * k + y2 - climax +
                                 climax * (2 * fabs(j - mid) / (x1 - x2)) *
                                     (2 * fabs(j - mid) / (x1 - x2)));
        if (temp > YY) {
            continue;
        }
        map[temp][j] = 3;
        int8_t gap = (int8_t)temp - lasty;
        if (gap > 1 && lasty != YM) {
            while (--gap) {
                map[temp - gap][j] = 3;
            }
        }
        if (gap < -1 && lasty != YM) {
            while (++gap) {
                map[temp - gap][j - 1] = 3;
            }
        }
        lasty = temp;
    }
}

/****** 与出右环情况相似 看 leftmap ******
 ******      十字删线              ******/
uint8_t ImageHandler::crossroadforleft() {
    if (!IF.parking && !IF.annulus && II._num_lm + II._num_rm == 0)  // fix
    {
        for (uint8_t j = II.step; j < 45; ++j)  // 出环时边界在上部
        {
            // 赛道边界为横向 与最左侧有交点
            if (basemap[j][0]) {
                if (!leftmap[j][0] && !rightmap[j][0] &&
                    isSearch(0, j, rightmap)) {
                    searchleftmap(0, j);
                    II.lnum_all +=
                        II.lnum[II.num_lm];  // leftmap 黑点数量 += leftmap
                                             // 黑色区域数组[左图黑色区域个数]
                    II.start_lm[II.num_lm++] = j;
                }
                break;
            }
        }
        if (!II.num_lm) {
            return 0;
        }
    }

    /************************ 找寻上折角点 ************************/
    uint8_t width = 3;
    uint8_t undelete_flag = 0;
    uint8_t rightX = 255;    // 角点横坐标
    uint8_t rightTop = 255;  // 角点纵坐标
    uint8_t rightBottom = 255;
    uint8_t leftX = 255;
    uint8_t leftTop = 255;     // 角点左移width 后的黑区最高点
    uint8_t leftBottom = 255;  // 角点左移width 后的黑区最低点
    for (uint8_t i = 0, flag = 0; i < XM; ++i) {
        for (uint8_t j = 0; j <= II.top; ++j) {
            if (leftmap[j][XX - i] == 1)  // 从右往左从下至上 找寻角点
            {
                rightBottom = j;
                for (uint8_t k = j + 1; k <= II.top; ++k)  // 从黑点向上一行
                {
                    if (leftmap[k][XX - i] != 1)  // leftmap 上边界 即角点上边界
                    {
                        rightX = XX - i;
                        rightTop = k - 1;
                        flag = 1;  // 找到角点
                        break;
                    }
                }
                break;
            }
        }
        if (flag == 1) {
            break;
        }
    }
    /************************ 找寻上角点 ************************/

    uint16_t pointnum = 0;
    uint16_t mynum = 0;
    if (rightX == 255 || rightTop == 255 || rightBottom == 255 ||
        rightTop > rightBottom + 15)  // 未重新赋值 即未找到角点
        goto DELETE;

    for (uint8_t i = rightTop; i < YM; ++i) {
        if (rightmap[i][rightX] ==
            1)  // 左图角点向上能遇到右图黑色区域，说明是斜向进入十字路口
            goto DELETE;

        if (i > rightTop + (YM - rightTop) / 1.2 ||
            undelete_flag)  // 最顶部的一块区域不搜，拍进去的东西太乱
            break;
    }
    if (IF.annulus == AL1 || IF.parking == PL1)
        return 0;

    do  // 这个do的存在只是为了被break。后跟while(0)
    {
        /****************** 确定找左移 width 后 黑点最低处、最高处
         * ******************/
        if (rightX < width) {
            undelete_flag = 1;
            break;
        }
        leftX = rightX - width;
        for (uint8_t j = 0; j <= II.top; ++j) {
            if (leftmap[j][leftX] == 1)  // 左移 width 后的黑区最低点
            {
                leftBottom = j;
                for (uint8_t k = j + 1; k <= II.top; ++k) {
                    if (leftmap[k][leftX] != 1)  // 左移 width 后的黑区最高点
                    {
                        leftTop = k - 1;
                        break;
                    }
                }
                break;
            }
        }
        if (leftBottom == 255 || leftTop == 255 || leftTop > leftBottom + 30)
            goto DELETE;
        /****************** 确定找左移 width 后 黑点最低处、最高处
         * ******************/

        /************ 计数黑区 黑点数量 并与 mynum 作比较 ************/
        for (uint8_t j = leftBottom; j <= leftTop; ++j) {
            for (uint8_t i = leftX; i <= rightX; ++i) {
                if (leftmap[j][i] == 1)
                    ++pointnum;
            }
        }
        mynum = (leftTop - leftBottom + 1) * (rightX - leftX + 1) /
                2;  // 计算黑区三角形面积
        mynum += (leftTop - leftBottom + 1) * 1.2 + 5;

        if (mynum <= pointnum)  // 玄学
            goto DELETE;

        II.right_x = rightX;
        II.right_y = rightTop;
    } while (0);

    /************ 角点右移三个 搜索赛道横区上部黑色区域 *********/
    if (!IF.parking && !IF.annulus && II._num_lm == 1 && II.start_lm[0] < 25 &&
        TOP_LM(0) < 40) {
        for (uint8_t j = 5; j < (YM - rightTop) * 3 / 4 + 3; ++j) {
            if (j + rightTop > YY || rightX + 3 > XX)
                break;

            if (basemap[j + rightTop][rightX + 3]) {
                if (leftmap[j + rightTop][rightX + 3] != 1 &&
                    rightmap[j + rightTop][rightX + 3] != 1 &&
                    !(II.start_rm[0] < 25 && TOP_RM(0) > j + rightTop &&
                      k2[TOP_RM(0)] >
                          k2[TOP_LM(0)] +
                              30) &&  // k2为y方向上的坐标变换，将图像视角的倾斜坐标系映射到俯视视角的坐标系上
                    isSearch(rightX + 3, j + rightTop,
                             rightmap))  // 判定 赛道横区上部 黑色区域
                {
                    searchleftmap(rightX + 3, j + rightTop);
                    II.lnum_all += II.lnum[II.num_lm];
                    II.start_lm[II.num_lm++] = j + rightTop;
                }
                break;
            }
        }
    }
    /************ 角点右移三个 搜索赛道横区上部黑色区域 *********/

    deleteforleft(undelete_flag);  // 角点左上区域删线

    /********* 角点左上区域删线（双重保险）*********/
    for (uint8_t j = rightTop + 1; j <= TOP_LM(0) + 1 && j <= II.top; ++j) {
        for (uint8_t i = 0; i <= rightX; ++i) {
            if (leftmap[j][i] == 2)
                leftmap[j][i] = 0;
        }
    }
    /********* 角点左上区域删线（双重保险）*********/

    /****************************** 删左下区域 线
     * ******************************/
    // 删左图所有区域下边界的线
DELETE:
    if (!IF.ramp &&
        !((IF.annulusDelay || IF.annulus == AL5 || IF.annulus == AR5 ||
           IF.parking == PL2 || IF.parking == PR2) &&
          II.num_lm + II.num_rm == 1)) {
        for (uint8_t n = 0; n < II.num_lm; ++n) {
            if (BOTTOM_LM(n) ==
                0)  // 左侧边界起始点y坐标，等于0则此区域无左侧边界
                continue;

            Point bottom_right;
            bottom_right.x = BR_X(n);      // 左侧边界的起始点 横坐标
            bottom_right.y = BR_Y(n) - 1;  // 左侧边界的起始点 纵坐标
            Point last[100];               // 被删 数组
            uint8_t num = 0;

            /* 删起始点 并入栈 */
            if (leftmap[bottom_right.y][0] == 2) {
                leftmap[bottom_right.y][0] = 0;
                last[num].x = 0;
                last[num].y = bottom_right.y;
                ++num;
            }

            for (uint8_t j = bottom_right.y; j < bottom_right.y + 8; ++j) {
                for (uint8_t i = 0, flagLeft = 0, flagRight = 0; i < XM; ++i) {
                    /* 起始点左边部分删线 高度限制 8 */
                    if (!flagLeft && bottom_right.x >= i) {
                        if (leftmap[j][bottom_right.x - i] == 2) {
                            leftmap[j][bottom_right.x - i] =
                                0;  // 效果：leftmap 左下区域无边线
                        }
                    }

                    /* 起始点右边部分删线 高度限制 8 */
                    if (!flagRight &&
                        bottom_right.x + i <=
                            XX &&  // 理论上是不用往右删的，但怕遇到奇怪如波浪线的图删不全，所以左右一起删
                        leftmap[j][bottom_right.x +
                                   i])  // 最右边的线会在后面补线时补回来
                    {
                        if (leftmap[j][bottom_right.x + i] == 2) {
                            leftmap[j][bottom_right.x + i] = 0;
                            if (leftmap[j][bottom_right.x + i + 1] == 0) {
                                last[num].x = bottom_right.x + i;
                                last[num].y = j;
                                ++num;
                            }
                        }
                    }
                }
            }

            for (int i = num - 1; i >= 0; --i)  // 补线
            {
                if ((last[i].y + 1 < YM &&
                     leftmap[last[i].y + 1][last[i].x] == 2) ||
                    (last[i].y + 1 < YM && last[i].x + 1 < XM &&
                     leftmap[last[i].y + 1][last[i].x + 1] == 2) ||
                    (last[i].x + 1 < XM &&
                     leftmap[last[i].y][last[i].x + 1] == 2) ||
                    (last[i].y + 1 < YM && last[i].x >= 1 &&
                     leftmap[last[i].y + 1][last[i].x - 1] == 2) ||
                    (last[i].x >= 1 &&
                     leftmap[last[i].y][last[i].x - 1] == 2) ||
                    (last[i].y >= 1 &&
                     leftmap[last[i].y - 1][last[i].x] == 2) ||
                    (last[i].y >= 1 && last[i].x + 1 < XM &&
                     leftmap[last[i].y - 1][last[i].x + 1] == 2) ||
                    (last[i].y >= 1 && last[i].x >= 1 &&
                     leftmap[last[i].y - 1][last[i].x - 1] ==
                         2))  // 附近 8点存在边界 重新赋为2
                {
                    leftmap[last[i].y][last[i].x] =
                        2;  // 效果 左下区域 尖角顶部有一红点
                }
            }
        }
    }
    return 0;
}

uint8_t ImageHandler::deleteforleft(uint8_t undelete_flag) {
    if (deleteforleftflag)
        return 0;

    deleteforleftflag = 1;
    uint8_t right = 0;
    for (uint8_t i = 0; i < II.num_lm; ++i) {
        if (RIGHT(i) > right)  // RIGHT(i) = II.right_top[i].x
            right = RIGHT(i);  // RT.X
    }
    for (uint8_t i = 0; i <= right; ++i) {
        for (uint8_t j = 0; j <= II.top; ++j) {
            if (leftmap[j][i] == 1) {
                for (uint8_t k = j + 1; k <= II.top; ++k)  // 竖直向上
                {
                    if (leftmap[k][i] == 2) {
                        if (!undelete_flag &&
                            II.right_y > k + 5)  //?undelete_flag=0 要删线
                            break;

                        for (uint8_t m = k; m <= II.top; ++m)  // 继续往上
                        {
                            if (leftmap[m][i] == 0) {
                                for (uint8_t n = m + 1; n <= II.top;
                                     ++n)  // 继续向上
                                {
                                    if (leftmap[n][i] ==
                                        1)  // 竖直方向 黑->红->白->黑
                                    {
                                        for (uint8_t a = n + 1; a <= II.top;
                                             ++a) {
                                            // 再往上 删线
                                            if (leftmap[a][i] == 2)
                                                leftmap[a][i] = 0;
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    return 1;
}

uint8_t ImageHandler::crossroadforright() {
    if (!IF.parking && !IF.annulus && II._num_lm + II._num_rm == 0) {
        for (uint8_t j = II.step; j < 45; ++j) {
            if (basemap[j][XX]) {
                if (!leftmap[j][XX] && !rightmap[j][XX] &&
                    isSearch(XX, j, leftmap)) {
                    searchrightmap(XX, j);
                    II.rnum_all += II.rnum[II.num_rm];
                    II.start_rm[II.num_rm++] = j;
                }
                break;
            }
        }
        if (!II.num_rm) {
            return 0;
        }
    }

    uint8_t width = 3;

    uint8_t undelete_flag = 0;
    uint8_t leftX = 255;
    uint8_t leftTop = 255;
    uint8_t leftBottom = 255;

    uint8_t rightX = 255;
    uint8_t rightTop = 255;
    uint8_t rightBottom = 255;
    for (uint8_t i = 0, flag = 0; i < XM; ++i) {
        for (uint8_t j = 0; j <= II.top; ++j) {
            if (rightmap[j][i] == 1) {
                leftBottom = j;
                for (uint8_t k = j + 1; k <= II.top; ++k) {
                    if (rightmap[k][i] != 1) {
                        leftX = i;
                        leftTop = k - 1;
                        flag = 1;
                        break;
                    }
                }
                break;
            }
        }
        if (flag == 1) {
            break;
        }
    }
    uint16_t pointnum = 0;
    uint16_t mynum = 0;
    if (leftX == 255 || leftTop == 255 || leftBottom == 255 ||
        leftTop > leftBottom + 15) {
        goto DELETE;
    }
    for (uint8_t i = leftTop; i < YM; ++i) {
        if (leftmap[i][leftX] == 1) {
            goto DELETE;
        }
        if (i > leftTop + (YM - leftTop) / 1.2 || undelete_flag) {
            break;
        }
    }
    if (IF.annulus == AR1 || IF.parking == PR1) {
        return 0;
    }

    do {
        if (leftX + width > XX) {
            undelete_flag = 1;
            break;
        }
        rightX = leftX + width;
        for (uint8_t j = 0; j <= II.top; ++j) {
            if (rightmap[j][rightX] == 1) {
                rightBottom = j;
                for (uint8_t k = j + 1; k <= II.top; ++k) {
                    if (rightmap[k][rightX] != 1) {
                        rightTop = k - 1;
                        break;
                    }
                }
                break;
            }
        }
        if (rightBottom == 255 || rightTop == 255 ||
            rightTop > rightBottom + 30) {
            goto DELETE;
        }
        for (uint8_t j = rightBottom; j <= rightTop; ++j) {
            for (uint8_t i = leftX; i <= rightX; ++i) {
                if (rightmap[j][i] == 1) {
                    ++pointnum;
                }
            }
        }
        mynum = (rightTop - rightBottom + 1) * (rightX - leftX + 1) / 2;
        mynum += (rightTop - rightBottom + 1) * 1.2 + 5;
        if (mynum <= pointnum) {
            goto DELETE;
        }
        II.left_x = leftX;
        II.left_y = leftTop;
    } while (0);

    if (!IF.parking && !IF.annulus && II._num_rm == 1 && II.start_rm[0] < 25 &&
        TOP_RM(0) < 40) {
        for (uint8_t j = 5; j < (YM - leftTop) * 3 / 4 + 3; ++j) {
            if (j + leftTop > YY || leftX < 3) {
                break;
            }
            if (basemap[j + leftTop][leftX - 3]) {
                if (leftmap[j + leftTop][leftX - 3] != 1 &&
                    rightmap[j + leftTop][leftX - 3] != 1 &&
                    !(II.start_lm[0] < 25 && TOP_LM(0) > j + leftTop &&
                      k2[TOP_LM(0)] > k2[TOP_RM(0)] + 30) &&
                    isSearch(leftX - 3, j + leftTop, leftmap)) {
                    searchrightmap(leftX - 3, j + leftTop);
                    II.rnum_all += II.rnum[II.num_rm];
                    II.start_rm[II.num_rm++] = j + leftTop;
                }
                break;
            }
        }
    }

    deleteforright(undelete_flag);
    for (uint8_t j = leftTop + 1; j <= TOP_RM(0) + 1 && j <= II.top; ++j) {
        for (int8_t i = XX; i >= leftX; --i) {
            if (rightmap[j][i] == 2) {
                rightmap[j][i] = 0;
            }
        }
    }

DELETE:
    if (!IF.ramp &&
        !((IF.annulusDelay || IF.annulus == AL5 || IF.annulus == AR5 ||
           IF.parking == PL2 || IF.parking == PR2) &&
          II.num_lm + II.num_rm == 1)) {
        for (uint8_t n = 0; n < II.num_rm; ++n) {
            if (BOTTOM_RM(n) == 0) {
                continue;
            }
            Point bottom_left;
            bottom_left.x = BL_X(n);
            bottom_left.y = BL_Y(n) - 1;
            Point last[100];
            uint8_t num = 0;
            if (rightmap[bottom_left.y][XX] == 2) {
                rightmap[bottom_left.y][XX] = 0;
                last[num].x = XX;
                last[num].y = bottom_left.y;
                ++num;
            }
            for (uint8_t j = bottom_left.y; j < bottom_left.y + 8; ++j) {
                for (uint8_t i = 0, flagRight = 0, flagLeft = 0; i < XM; ++i) {
                    if (!flagRight && bottom_left.x + i <= XX) {
                        if (rightmap[j][bottom_left.x + i] == 2) {
                            rightmap[j][bottom_left.x + i] = 0;
                        }
                    }
                    //                    else
                    //                    {
                    //                        flagRight = 1;
                    //                    }
                    if (!flagLeft && bottom_left.x >= i &&
                        rightmap[j][bottom_left.x - i]) {
                        if (rightmap[j][bottom_left.x - i] == 2) {
                            rightmap[j][bottom_left.x - i] = 0;
                            if (rightmap[j][bottom_left.x - i - 1] == 0) {
                                last[num].x = bottom_left.x - i;
                                last[num].y = j;
                                ++num;
                                //                                flagLeft = 1;
                            }
                        }
                    }
                    //                    else if (bottom_left.x + 1 == i)
                    //                    {
                    //                        flagLeft = 1;
                    //                    }
                    //                    if (flagRight && flagLeft)
                    //                    {
                    //                        break;
                    //                    }
                }
            }
            for (int i = num - 1; i >= 0; --i) {
                if ((last[i].y + 1 < YM &&
                     rightmap[last[i].y + 1][last[i].x] == 2) ||
                    (last[i].y + 1 < YM && last[i].x >= 1 &&
                     rightmap[last[i].y + 1][last[i].x - 1] == 2) ||
                    (last[i].x >= 1 &&
                     rightmap[last[i].y][last[i].x - 1] == 2) ||
                    (last[i].y + 1 < YM && last[i].x + 1 < XM &&
                     rightmap[last[i].y + 1][last[i].x + 1] == 2) ||
                    (last[i].x + 1 < XM &&
                     rightmap[last[i].y][last[i].x + 1] == 2) ||
                    (last[i].y >= 1 &&
                     rightmap[last[i].y - 1][last[i].x] == 2) ||
                    (last[i].y >= 1 && last[i].x + 1 < XM &&
                     rightmap[last[i].y - 1][last[i].x + 1] == 2) ||
                    (last[i].y >= 1 && last[i].x >= 1 &&
                     rightmap[last[i].y - 1][last[i].x - 1] == 2)) {
                    //                    if ((last[i].x == 0 && last[i].y == 0)
                    //                    ||
                    //                        (last[i].x == XX && last[i].y ==
                    //                        0))
                    //                    {
                    //                        break;
                    //                    }
                    rightmap[last[i].y][last[i].x] = 2;
                }
            }
        }
    }
    return 0;
}

uint8_t ImageHandler::deleteforright(uint8_t undelete_flag) {
    if (deleteforrightflag) {
        return 0;
    }
    deleteforrightflag = 1;
    uint8_t left = XX;
    for (uint8_t i = 0; i < II.num_rm; ++i) {
        if (left > LEFT(i)) {
            left = LEFT(i);
        }
    }
    for (int8_t i = XX; i >= left; --i) {
        for (uint8_t j = 0; j <= II.top; ++j) {
            if (rightmap[j][i] == 1) {
                for (uint8_t k = j + 1; k <= II.top; ++k) {
                    if (rightmap[k][i] == 2) {
                        if (!undelete_flag && II.left_y > k + 5) {
                            break;
                        }
                        for (uint8_t m = k; m <= II.top; ++m) {
                            if (rightmap[m][i] == 0) {
                                for (uint8_t n = m + 1; n <= II.top; ++n) {
                                    if (rightmap[n][i] == 1) {
                                        for (uint8_t a = n + 1; a <= II.top;
                                             ++a) {
                                            if (rightmap[a][i] == 2) {
                                                rightmap[a][i] = 0;
                                            }
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    return 1;
}

/************ 删除某种情况的线 ************/
void ImageHandler::deleteline() {
    if (IF.annulus == AL1 || IF.annulus == AR1 || IF.parking == PL1 ||
        IF.parking == PR1)
        return;

#define DELETE_NUM 0

    /************************ 删除回环左弯的 左上多余边界
     * ************************/

    /*          确定三个点 左下：down 中上:up 右下 minDown            *
     *  RT/minDown:左边界 最右侧的最上侧  TR/up ：回环左边界 最顶部    *
     *              down:回环最低行（边界延伸的终点）                 */

    uint8_t down = YM;
    for (uint8_t i = TOP_LM(0); i > 1; --i)  // TOP_LM为top_right.y
    {
        // 自顶向底 记录结束 非边界->边界->非边界 的最低行
        if (X_WBW_Detect(0, XX, i, leftmap, goRight))
            down = i;
        else
            break;
    }

    /****** 删除 down-minDown竖直方向上的边界 ******/
    uint8_t minDown;
    if (down > RT_Y(0) + 1)
        minDown = down;
    else {
        minDown = RT_Y(0) + 1;  // RT:左边界 最右侧的最上侧
        for (uint8_t j = down; j < RT_Y(0) + 1 && j < YM; ++j) {
            for (uint8_t i = 0; i < getMapXMax_Row(j, leftmap, 1) && i != XM;
                 ++i) {
                if (leftmap[j][i] == 2)
                    leftmap[j][i] = 0;
            }
        }
    }

    /****** 删除 minDown-up 竖直方向上的边界 ******/
    uint8_t up = TOP_LM(0) + 1 < YY
                     ? TOP_LM(0) + 1
                     : YY;  // 最高点   TR：回环左边界 最顶部的最右侧
    if (minDown != YM && up >= DELETE_NUM + minDown) {
        for (uint8_t j = minDown; j + DELETE_NUM <= up; ++j) {
            for (uint8_t i = 0; i <= getMapXMax_Row(j, leftmap, 2) && i < XM;
                 ++i) {
                if (leftmap[j][i] == 2)
                    leftmap[j][i] = 0;
            }
        }
    }

    /************************ 删除回环右弯的右上边界 ************************/
    down = YM;
    for (uint8_t i = TOP_RM(0); i > 1; --i) {
        if (X_WBW_Detect(XX, 0, i, rightmap, goLeft))
            down = i;
        else
            break;
    }

    if (down > LT_Y(0) + 1)
        minDown = down;
    else {
        minDown = LT_Y(0) + 1;
        for (uint8_t j = down; j < LT_Y(0) + 1 && j < YM; ++j) {
            for (int8_t i = XX; i > getMapXMin_Row(j, rightmap, 1) && i != XM;
                 --i) {
                if (rightmap[j][i] == 2)
                    rightmap[j][i] = 0;
            }
        }
    }

    up = TOP_RM(0) + 1 < YY ? TOP_RM(0) + 1 : YY;
    if (minDown != YM && up >= DELETE_NUM + minDown) {
        for (uint8_t j = minDown; j + DELETE_NUM <= up; ++j) {
            for (uint8_t i = getMapXMin_Row(j, rightmap, 2); i < XM; ++i) {
                if (rightmap[j][i] == 2)
                    rightmap[j][i] = 0;
            }
        }
    }

    if (II.num_lm == 0 && II.num_rm)
        deleteforright(1);
    else if (II.num_rm == 0 && II.num_lm)
        deleteforleft(1);  // 角点左上区域删线
}

void ImageHandler::Get_insideMap() {
    if (II.bnum_all + II.lnum_all + II.rnum_all == XM * YM + II.repeatNum) {
        return;
    }
    // noDown情况较少
    for (int8_t i = 0; i < noDown.num; ++i) {
        if (leftmap[noDown.point[i].y][noDown.point[i].x] != 1 &&
            rightmap[noDown.point[i].y][noDown.point[i].x] != 1 &&
            !deletemap[noDown.point[i].y][noDown.point[i].x])
            searchdeletemap2(noDown.point[i].x, noDown.point[i].y);
    }
    // 最左侧和最右侧进行条件判断 并由此开始搜图
    for (uint8_t i = 0; i < YM; ++i) {
        // 非白                白                白                 白
        if (basemap[i][0] && !leftmap[i][0] && !rightmap[i][0] &&
            !deletemap[i][0])
            searchdeletemap(0, i);
        if (basemap[i][XX] && !leftmap[i][XX] && !rightmap[i][XX] &&
            !deletemap[i][XX])
            searchdeletemap(XX, i);
    }
    // 顶部进行条件判断  并由此开始搜图
    for (uint8_t i = 0; i < XM; ++i) {
        if (basemap[YY][i] && !leftmap[YY][i] && !rightmap[YY][i] &&
            !deletemap[YY][i])
            searchdeletemap(i, YY);
    }
}

/****** 获取速度处理截止行 ******/
void ImageHandler::getSpeedTop() {
    for (uint8_t i = 0; i < YM; ++i) {
        // 非白&&白
        if (basemap[i][speedlineLeft[i]] && !insidemap[i][speedlineLeft[i]])
        // rightmap[i][speedlineLeft[i]]==1)
        {
            II.speedTop = i;
            break;
        }
        if (basemap[i][speedlineRight[i]] && !insidemap[i][speedlineRight[i]])
        // leftmap[i][speedlineRight[i]]==1)
        {
            II.speedTop = i;
            break;
        }
    }
}

uint8_t ImageHandler::get_Forkdown(uint8_t start_y,
                                   uint8_t mid,
                                   uint8_t down_dis,
                                   uint8_t test_dis,
                                   uint8_t symmetry_dis) {
    // mid=23,dis=5，test_dis=10,symmetry_dis=3;
    uint8_t i;
    uint8_t down_y = YY, down_x;
    uint8_t down_temp, down_temp_symmetry;

    /* 找寻三岔路口最低点 横纵坐标 */
    for (i = mid - down_dis; i <= mid + down_dis; ++i) {
        down_temp = getMapYMin_Col2(i, start_y, basemap);
        if (down_temp < down_y) {
            down_y = down_temp;
            down_x = i;
        }

        if (down_y == YY)
            return 0;
    }

    /* 最低点检测 1：最低点左右“对称” */
    for (i = down_x - test_dis; i <= down_x; ++i) {
        down_temp = getMapYMin_Col2(i, down_y - 5, basemap);
        down_temp_symmetry =
            getMapYMin_Col2(2 * down_x - i, down_y - 5, basemap);
        if (down_temp - down_temp_symmetry > symmetry_dis ||
            down_temp - down_temp_symmetry < -symmetry_dis)
            return 0;
    }
}

uint8_t ImageHandler::goAnnulus() {
    static uint8_t status = 0;  // 1:judge 2:isEnter
    static uint8_t dir;         // direction
    static uint16_t time = 0;
    static uint16_t in = 0;
    static uint8_t right_num = 0;

    qout << "status" << status;

    old_left_detect = new_left_detect;
    new_left_detect = first_leftAnnulus_Detect();

    old_right_detect = new_right_detect;
    new_right_detect = rightAnnulusDetect();  // first_rightAnnulus_Detect();
    qout << "nowAnnulusNum" << nowAnnulusNum;

    if (status == 0 && new_left_detect && old_left_detect)
    // if(status==0 && leftAnnulusDetect())
    {
#ifndef T_REX_QT_DEBUG
        BEEP(200);
#endif

        status = 1;
        dir = goLeft;
        time = 0;
        in = 0;
        AD = AD_INIT;
        ++nowAnnulusNum;
        // qout<<"left annulus detect success ! ! !";
        // draw_Annulusline();
    } else if (status == 0 && new_right_detect && old_right_detect)
    // else if(status==0 && rightAnnulusDetect())
    {
#ifndef T_REX_QT_DEBUG
        BEEP(200);
#endif
        right_num++;
        if (right_num >= 2) {
            status = 1;
            right_num = 0;
            dir = goRight;
            time = 0;
            in = 0;
            AD = AD_INIT;
            ++nowAnnulusNum;
            qout << "right annulus detect success ! ! !";
        }
    }

    if (status == 1) {
        qout << "right_delay" << right_delay[nowAnnulusNum - 1];
        if (right_delay[nowAnnulusNum - 1])
            --right_delay[nowAnnulusNum - 1];
    }

    if (status == 1 && !right_delay[nowAnnulusNum - 1]) {
        do {
            if (isEnter(dir)) {
                status = 2;
                break;
            }
            uint8_t ret = AnnulusDeal(dir, status);  // 返回1 补线成功
            // qout<<"ret"<<ret;
            if (ret)
                ++in;
            else if (in >= 2)
                II.line_forbid =
                    (dir == goLeft ? RIGHTLINE
                                   : LEFTLINE);  // 禁线 保证只有补线 没有其他线
                                                 // 右环禁左边线 左环禁右边线

        } while (0);
    }

    if (status == 2) {
        do {  // deletemap 应为空白 if 满足说明完全入环 即图像为普通弯道
            if ((dir == goLeft && getMapYMin_Col(0, deletemap, 1) == YM &&
                 LT_X(0) == 0 && II.top <= 55) ||
                (dir == goRight && getMapYMin_Col(XX, deletemap, 1) == YM &&
                 RT_X(0) == XX && II.top <= 55)) {
                status = 3;
                AD.flag = 1;
                break;
            }
            AnnulusDeal(dir, status);
        } while (0);
    }

    if (status == 3)  // 无需补线 进入环岛不需要再补线 控制中的差速固定
    {
        // 检测是否看到角点（仅能看到角点 另一边赛道边界还在视野外）
        if ((dir == goRight && II.right_y <= 50 && II.right_x < XX - 5 &&
             II.right_x != 0) ||  // && annulus_yaw > annulus_yaw_limit) ||
            (dir == goLeft && II.left_y <= 50 && II.left_x > 5 &&
             II.left_x != XM))  // && annulus_yaw > annulus_yaw_limit))
        {
            status = 4;
            AD.flag = 2;
        }
    }

    if (status == 4)  // 离开圆环 补线
    {
        do {
            // 进入if不需要再补线 left/rightmap 其中一个为全白
            if ((dir == goRight && II.num_lm == 0) ||
                (dir == goLeft && II.num_rm == 0)) {
                status = 5;
                AD.flag = 3;
                break;
            }
            AD.flag = 2;
            leave(dir);
        } while (0);
    }

    if (status == 5)  // 成功出圆环
    {
#ifndef T_REX_QT_DEBUG
        BEEP(200);
#endif
#define MIN_DOWN 20  // 最低行 当其小于某个值时说明已退出圆环 差不多进入直道
        if (dir == goRight) {
            uint8_t temp = getMapYMin_Col(23, basemap, 2);
            uint8_t min = getMapYMin_Col(0, basemap, 2);
            uint8_t max = getMapYMin_Col(XX, basemap, 2);

            if (abs(temp - (max + min) / 2) > 4)
                return 0;

            temp = getMapYMin_Col(26, basemap, 2);

            if (II.num_lm != 0) {
                for (uint8_t j = 0; j <= temp; ++j)
                    for (uint8_t i = 0; i <= 26; ++i)
                        if (leftmap[j][i] >= 2)
                            leftmap[j][i] = 0;
            } else {
                uint8_t temp_line;
                for (uint8_t i = 26; i <= XX; ++i) {
                    temp_line = getMapYMin_Col2(i, temp - 5, basemap);
                    leftmap[temp_line][i] = 2;
                }
            }

            draw_straight(26, temp, 0, temp / 5, leftmap);

            // drawline2(26,temp,0,temp/5,leftmap,12);
            uint8_t lmin =
                getMapYMin_Col(0, deletemap, 1);  // 直道时 deletemap 为全白
            if (lmin >= 55 && II.num_lm == 1 && II.start_lm[0] <= MIN_DOWN)
                status = 6;
        } else if (dir == goLeft) {
            uint8_t temp = getMapYMin_Col(23, basemap, 2);
            uint8_t min = getMapYMin_Col(XX, basemap, 2);
            uint8_t max = getMapYMin_Col(0, basemap, 2);

            if (abs(temp - (max + min) / 2) > 4)
                return 0;

            temp = getMapYMin_Col(20, basemap, 2);

            if (II.num_rm != 0) {
                for (uint8_t j = 0; j <= temp; ++j)
                    for (uint8_t i = 20; i < XX; ++i)
                        if (rightmap[j][i] >= 2)
                            rightmap[j][i] = 0;
            } else {
                uint8_t temp_line;
                for (uint8_t i = 0; i <= 20; ++i) {
                    temp_line = getMapYMin_Col2(i, temp - 5, basemap);
                    rightmap[temp_line][i] = 2;
                }
            }

            draw_straight(20, temp, XX, temp / 5, rightmap);
            // drawline2(20,temp,XX,temp/5,rightmap,12);

            qout << "good luck";

            uint8_t rmin = getMapYMin_Col(XX, deletemap, 1);
            if (rmin >= 55 && II.num_rm == 1 && II.start_rm[0] <= MIN_DOWN)
                status = 6;
        }
    }

    if (status == 6) {
        ++time;         // 多次判定为处在直道中
        if (time > 10)  // 类似延时
        {
            status = 0;  // 防止出环时进行入环识别 置零后重新开始圆环判断
            /* 最后复位 */
        }
    }

    if (status == 6)
        IF.annulusDelay = 1;
    else
        IF.annulusDelay = 0;

    // 返回 状态信息
    if (status && status != 6) {
        return (dir - 1) * 6 + status;  // 返回非0说明在圆环中
    } else
        return 0;
}

/* 圆环初判断 */
uint8_t ImageHandler::first_leftAnnulus_Detect() {}

uint8_t ImageHandler::first_rightAnnulus_Detect() {}
void ImageHandler::draw_Annulusline() {
    uint8_t top;
    uint8_t down;
    int8_t line[23] = {0};
    uint8_t i = 0;

    for (i = 0; i < RT_X(0) - 1; ++i) {
        top = getMap_YThird_Col(i, 0, basemap);
        down = getMap_YSecond_Col(i, 0, basemap);

        /*    if (top == down == YM)
                return 0;*/
        /*    if (top == YM || down == YM)
                                break;*/

        line[i] = (top + down) / 2;
        /*  if (line[i] > 55 || line[i] < 40)
            return 0;*/
    }

    uint8_t temp = RT_X(0) - 1;
    for (i = 0; i < 4; ++i) {
        if (i < 3)
            line[temp + i] = line[temp - 1];
        else
            line[temp + i] = line[temp - 1] - 1;
    }

    /* 画直线 (RT_X(0)-2,mid[RT_X(0)-2]) 45度 右下*/
    for (i = temp + 4; i <= XX / 2; ++i)
        line[i] = line[i - 1] - 1;

    for (i = 0; i <= XX / 2; ++i) {
        basemap[line[i] - 2][i] = 2;
        basemap[line[i] + 2][i] = 2;
    }
}

uint8_t ImageHandler::leftAnnulusDetect() {
    /******************************* 防止误判！！！
     * *******************************/
    if (II.num_lm != 1 || II.num_rm != 1 || II.lnum[0] + II.rnum[0] < 250 ||
        TOP_RM(0) < 50 || TOP_RM(0) < II.start_rm[0] + 25 || TOP_LM(0) < 20 ||
        TOP_LM(0) > 55 || II.start_rm[0] > 35 || II.start_lm[0] > 25 ||
        RIGHT(0) > 23 || BR_X(0) > 15 || k2[TOP_RM(0)] - k2[TOP_LM(0)] < 25 ||
        II.top < 55 || II.speedTop < 50)  // adjust II.speedTop
    {
        return 0;
    }
    // 圆环之后的区域会在deletmap 中 可能为 黑区域红线/橙区域绿线
    uint8_t deleteTop =
        getMapYMin_Col(0, deletemap, 254);  // 最左侧的绿点的最低点
    uint8_t min = getMapYMin_Col(0, deletemap, 1);  // 最左侧的黑点最低点
    uint8_t end = getMapYMax_Col(0, leftmap, 1);  // 最左侧的黑点最高点
    uint8_t yhTop;  // 第一列圆环外弧最高点

    /****** 防误判+确定圆环的最高点 ******/
    if (deleteTop != YM && min == YM && II.top >= 55) {
        if (II.dnum_top <= 50) {
            return 0;
        }
        // 入左环时 左环外弧边界左低右高 但高度差不大 即不会有3或4的高度差
        if (getMapYMin_Col(4, deletemap, 254) > deleteTop + 3) {
            return 0;
        }
        if (getMapYMin_Col(8, deletemap, 254) > deleteTop + 4) {
            return 0;
        }
        for (uint8_t i = 0; i < XM; ++i) {
            uint8_t j = getMapYMin_Col(i, deletemap, 253);
            if (j == YM) {
                return 0;
            }
            if (deletemap[j + 1][i] ==
                253)  // 最右侧竖直方向有多个点 此时橙线为竖直方向 退出循环
            {
                break;
            } else {
                continue;
            }
        }
        yhTop = deleteTop;
    } else {
        if (deleteTop <= min) {
            return 0;
        }
        if (min != YM && cntMap(0, min) <= 25)  // 圆环圆饼面积
        {
            return 0;
        }
        // 同上 左高右低
        if (getMapYMin_Col(4, deletemap, 1) > min + 3) {
            return 0;
        }
        if (getMapYMin_Col(8, deletemap, 1) > min + 4) {
            return 0;
        }
        yhTop = min;
    }

    if (end == YM || yhTop > end + 25 || yhTop > 55 || yhTop < 45 || end < 15 ||
        yhTop < TOP_LM(0) || end + 3 > yhTop) {
        return 0;
    }

    if (II.start_rm[0] > 0) {
        for (uint8_t j = XX - 1; j > 20; --j) {
            if (rightmap[II.start_rm[0]][j] == 1) {
                return 0;
            }
        }
    }

    for (int8_t i = RT_X(0); i >= 0; --i)  // RT左侧为环中心（黑色背景）
    {
        if (leftmap[RT_Y(0)][i] != 1) {
            return 0;
        }
    }

    uint8_t down = YY;  // 环中心的最低点
    int8_t stopX = getMapXMax_Row(0, leftmap, 1);
    // 找不到 return XM，leftmap 应该只有环中心（黑 在左侧中上位置）
    if (stopX == XM) {
        stopX = 0;
    }
    uint8_t n = 0;
    for (int8_t i = RIGHT(0); i >= stopX;
         --i)  // RT.X：环边界最右侧的顶部（圆环的 右上角点）
    {
        // 从环最右侧往左
        uint8_t temp =
            getMapYMin_Col(i, leftmap, 1);  // temp: 圆环下边界的横坐标
        if (down < temp) {
            return 0;
        } else if (down == temp) {
            ++n;
        } else {
            down = temp;
        }
    }
    if (n > 2) {
        return 0;
    }
    if (RB_Y(0) + 5 >
        TOP_LM(
            0))  // RT.Y TR.Y相差不会太大 前期小后期才慢慢变大一点（环中心更圆）
    {
        return 0;
    }
    if (TOP_LM(0) <
        RT_Y(0) +
            (uint8_t)((RIGHT(0) + 1) / 1.5 > 3 ? 3 : (RIGHT(0) + 1) / 1.5)) {
        return 0;
    }

    if (II.start_lm[0] == 0 || RIGHT(0) <= 3) {
        uint8_t up = 0;
        for (int8_t i = RIGHT(0); i >= 0; --i) {
            uint8_t temp = getMapYMax_Col(i, leftmap, 1);  // 环中心的最高点
            if (up > temp) {
                return 0;
            } else {
                up = temp;
            }
        }
    } else {
        uint8_t topLeft = getMapXMin_Row(TOP_LM(0), leftmap, 1);
        if (topLeft > ((RIGHT(0) + 1) / 4 < 3 ? (RIGHT(0) + 1) / 4 : 3)) {
            return 0;
        }
        uint8_t end = getMapYMax_Col(0, leftmap, 1);  // 圆环不会这么小
        if (TOP_LM(0) > end + 2) {
            return 0;
        }
    }

    // 圆环的高度差>20 && 圆环的最右侧横坐标>10 && 圆环边界起点横坐标<45 &&
    // 圆环最右侧横坐标 <45
    if (TOP_LM(0) > BOTTOM_LM(0) + 20 && RIGHT(0) > 10 && BR_X(0) <= XX - 1 &&
        RT_X(0) <= XX - 1) {
        uint8_t x1, y1, x2, y2;
        // BR：圆饼最低点 圆饼左上角点
        x1 = BR_X(0) + 1;
        y1 = BR_Y(0);
        x2 = RT_X(0) + 1;
        y2 = RT_Y(0);
        if (strJudge(x1, y1, x2, y2, leftmap, y1, y2, 1,
                     2))  // QT上表现为 leftmap 有灰直线段
        {
            return 0;
        }
    }

    uint8_t x1 = 255, y1 = 255, x2 = 255, y2 = 255;
    if (BL_X(0) <= 0) {
        return 0;
    }
    // 右边界的起点
    x1 = BL_X(0) - 1;
    y1 = BL_Y(0);
    for (uint8_t i = 0; i < XM; ++i) {
        // 找寻圆环外弧最高点所在行 右边界
        if (rightmap[yhTop][i] == 1) {
            //	if (rightmap[yhTop][j] == 1)
            //		x2 = j-1;
            // else
            x2 = i - 1;
            y2 = yhTop;
            break;
        }
        if (i == XX) {
            return 0;
        }
    }
    if (x1 == 255 || y1 == 255 || x2 == 255 || y2 == 255) {
        return 0;
    }

    // QT上表现为 rightmap 右边界有绿直线段
    return strJudge(x1, y1, x2, y2, rightmap, y1, y2, 1, (y2 - y1) / 10);
}

uint8_t ImageHandler::rightAnnulusDetect() {
    if (II.num_rm != 1 || II.num_lm != 1 || II.rnum[0] + II.lnum[0] < 250 ||
        TOP_LM(0) < 50 || TOP_LM(0) < II.start_lm[0] + 25 || TOP_RM(0) < 20 ||
        TOP_RM(0) > 55 || II.start_lm[0] > 35 || II.start_rm[0] > 25 ||
        LEFT(0) < 23 || BL_X(0) < XX - 15 ||
        k2[TOP_LM(0)] - k2[TOP_RM(0)] < 25 || II.top < 55 || II.speedTop < 50) {
        return 0;
    }
    uint8_t deleteTop =
        getMapYMin_Col(XX, deletemap, 254);  // 寻找dm XX列的最下侧的绿点
    uint8_t min = getMapYMin_Col(XX, deletemap, 1);  // 寻找dm
                                                     // XX列的最下侧的黑点
    uint8_t end = getMapYMax_Col(XX, rightmap, 1);  // 寻找rm XX列的最上侧的黑点
    uint8_t yhTop;
    if (deleteTop != YM && min == YM && II.top >= 55)  // dm有绿无黑，赛道顶部高
    {
        if (II.dnum_top <= 50)  // 绿点太少
        {
            return 0;
        }
        if (getMapYMin_Col(XX - 4, deletemap, 254) >
            deleteTop + 3)  // 绿色区域左不能高于右太多
        {
            return 0;
        }
        if (getMapYMin_Col(XX - 8, deletemap, 254) > deleteTop + 4) {
            return 0;
        }
        for (uint8_t i = 0; i < XM; ++i) {
            uint8_t j =
                getMapYMin_Col(XX - i, deletemap, 253);  // dm橙色边界线高度位置
            if (j == YM)                                 // 无橙线了
            {
                return 0;
            }
            if (deletemap[j + 1][XX - i] ==
                253)  // 橙线厚度2则跳出(这样就不会进入return
                      // 0，yhTop就能被赋值)    但为何要求厚度2?
            {
                break;
            } else {
                continue;
            }
        }
        yhTop = deleteTop;
    } else {
        if (deleteTop <=
            min)  // XX列 黑点(delete点)比绿点(nodown点)高    什么情况下出现?
        {
            return 0;
        }
        if (min != YM && cntMap(XX, min) <= 25)  // bm黑点太少
        {
            return 0;
        }
        if (getMapYMin_Col(XX - 4, deletemap, 1) > min + 3) {
            return 0;
        }
        if (getMapYMin_Col(XX - 8, deletemap, 1) > min + 4) {
            return 0;
        }
        yhTop = min;
    }

    /////MTgiant
    //    for(uint8_t i = 0; i < 5; ++i)
    //    {
    //        uint8_t _dTop = getMapYMax_Col(XX - i, deletemap, 1);
    //        if(_dTop < YY - 1)
    //        {
    //            return 0;
    //        }
    //    }
    //    if(cntMap(XX, YY) < 30 || II.rnum[0] < 70)
    //    {
    //        return 0;
    //    }
    /////
    if (end == YM || yhTop > end + 25 || yhTop > 55 || yhTop < 45 || end < 15 ||
        yhTop < TOP_RM(0) || end + 3 > yhTop)  // k2[yhTop] < k2[TOP_LM]
    {
        return 0;
    }
    if (II.start_lm[0] > 0) {
        for (uint8_t j = 1; j < XX - 20; ++j) {
            if (leftmap[II.start_lm[0]][j] == 1) {
                return 0;
            }
        }
    }
    for (uint8_t i = LT_X(0); i < XM; ++i) {
        if (rightmap[LT_Y(0)][i] != 1) {
            return 0;
        }
    }
    uint8_t down = YY;
    uint8_t stopX = getMapXMin_Row(0, rightmap, 1);
    if (stopX == XM) {
        stopX = XX;
    }
    uint8_t n = 0;
    for (uint8_t i = LEFT(0); i <= stopX; ++i) {
        uint8_t temp = getMapYMin_Col(i, rightmap, 1);
        if (down < temp) {
            return 0;
        } else if (down == temp) {
            ++n;
        } else {
            down = temp;
        }
    }
    if (n > 2) {
        return 0;
    }
    if (LB_Y(0) + 5 > TOP_RM(0)) {
        return 0;
    }
    if (TOP_RM(0) < LT_Y(0) + (uint8_t)((XX - LEFT(0) + 1) / 1.5 > 3
                                            ? 3
                                            : (XX - LEFT(0) + 1) / 1.5)) {
        return 0;
    }
    if (II.start_rm[0] == 0 || LEFT(0) >= XX - 3) {
        uint8_t up = 0;
        for (uint8_t i = LEFT(0); i < XM; ++i) {
            uint8_t temp = getMapYMax_Col(i, rightmap, 1);
            if (up > temp) {
                return 0;
            } else {
                up = temp;
            }
        }
    } else {
        uint8_t topRight = getMapXMax_Row(TOP_RM(0), rightmap, 1);
        if (topRight <
            XX - ((XX - LEFT(0) + 1) / 4 < 3 ? (XX - LEFT(0) + 1) / 4 : 3)) {
            return 0;
        }
        uint8_t end = getMapYMax_Col(XX, rightmap, 1);
        if (TOP_RM(0) > end + 2) {
            return 0;
        }
    }
    if (TOP_RM(0) > BOTTOM_RM(0) + 20 && LEFT(0) < XX - 10 && BL_X(0) >= 1 &&
        LT_X(0) >= 1) {
        uint8_t x1, y1, x2, y2;
        x1 = BL_X(0) - 1;
        y1 = BL_Y(0);
        x2 = LT_X(0) - 1;
        y2 = LT_Y(0);
        if (strJudge(x1, y1, x2, y2, rightmap, y1, y2, 1, 2)) {
            return 0;
        }
    }

    uint8_t x1 = 255, y1 = 255, x2 = 255, y2 = 255;
    if (BR_X(0) >= XX) {
        return 0;
    }
    x1 = BR_X(0) + 1;
    y1 = BR_Y(0);
    for (uint8_t i = 0; i < XM; ++i) {
        if (leftmap[yhTop][XX - i] == 1) {
            //	if (rightmap[yhTop][j] == 1)
            //		x2 = j-1;
            // else
            x2 = XX - i + 1;
            y2 = yhTop;
            break;
        }
        if (i == XX) {
            return 0;
        }
    }
    if (x1 == 255 || y1 == 255 || x2 == 255 || y2 == 255) {
        return 0;
    }
    return strJudge(x1, y1, x2, y2, leftmap, y1, y2, 1, (y2 - y1) / 10);
}

// 标志位为 V
uint8_t ImageHandler::isEnter(uint8_t dir) {
    if (dir == goLeft && II.num_rm > 0)
        return 0;
    if (dir == goRight && II.num_lm > 0)
        return 0;

    uint8_t num = 0;
    for (uint8_t i = 0; i < 40; ++i) {
        if (num >= 2) {
            return 1;
        }
        for (uint8_t j = 0; j < XM; ++j) {
            if (deletemap[i][j] == 0) {
                for (uint8_t k = j + 1; k < XM; ++k) {
                    if (deletemap[i][k]) {
                        for (uint8_t m = k + 1; m < XM; ++m) {
                            if (deletemap[i][m] == 0)  // 白->黑红->白
                            {
                                ++num;
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
}

uint8_t ImageHandler::leave(uint8_t dir) {
    // 补线:(min,0)<--(upd,left_x)
    if (dir == goLeft && II.left_x >= 5) {
        if (II.left_x == XM && TOP_RM(0) == YY && LEFT(0) < XM / 2) {
            return 0;
        } else if (II.left_x >= XX - 5) {
            II.line_forbid = BOTHLINE;

            //            left_leave++;
            //            if(left_leave<5)
            //            {
            //                uint8_t temp=getMapYMin_Col(23,basemap,2);
            //                uint8_t min=getMapYMin_Col(XX,basemap,2);
            //                uint8_t max=getMapYMin_Col(0,basemap,2);
            //
            //                if(temp ==YM || min==YM || max==YM)
            //                    return 0;
            //
            //                if(abs(temp-(max+min)/2)>4)
            //                    return 0;
            //
            //                if(II.num_rm!=0)
            //                    for(uint8_t j=0;j<=temp;++j)
            //                        for(uint8_t i=23;i<XX;++i)
            //                            if(rightmap[j][i]>=2)
            //                                rightmap[j][i]=0;
            //
            //                draw_straight(XX,0,23,temp,rightmap);
            //            }
            //            else if(left_leave==5)
            //            {
            //                left_leave=0;
            //                return 0;
            //            }
            return 0;

        } else {
            // 出环时背景（黑）在 deletemap
            uint8_t min = getMapYMin_Col(
                0, deletemap, 1);  // min：上边界与最左侧的交点 即补线终点
            if (min == YM) {
                min = getMapYMin_Col(0, deletemap, 254);  // 未找到 搜索绿色
            }
            if (min < YM && min > II.left_y) {
                uint8_t upd = getMapYMin_Col(II.left_x, rightmap,
                                             1);  // 出环时的右上角点 补线起点
                drawline2(II.left_x, upd, 0, min, rightmap, 12);
                // draw_straight(0, min,II.left_x,upd, rightmap);
                AD.flag = 4;
            } else {
                II.line_forbid = BOTHLINE;
                qout << "left_if_3";
                return 0;
            }
        }
    } else if (dir == goRight && II.right_x <= XX - 5) {
        if (II.right_x == XM && TOP_LM(0) == YY && RIGHT(0) > XM / 2) {
            return 0;
        } else if (II.right_x <= 5) {
            II.line_forbid = BOTHLINE;
            return 0;
        } else {
            uint8_t min = getMapYMin_Col(XX, deletemap, 1);
            if (min == YM) {
                min = getMapYMin_Col(XX, deletemap, 254);
            }
            if (min < YM && min > II.right_y) {
                uint8_t upd = getMapYMin_Col(II.right_x, leftmap, 1);
                drawline2(XX, min, II.right_x, upd, leftmap, 12);
                // draw_straight(XX, min,II.right_x,upd, leftmap);
                AD.flag = 4;
            } else {
                II.line_forbid = BOTHLINE;
                return 0;
            }
        }
    }
    return 1;
}

// ret 返回1 说明补线成功
uint8_t ImageHandler::AnnulusDeal(uint8_t ADir, uint8_t status) {
#define yhmap deletemap
    if (ADir == 0 || status > 2)
        return 0;

    uint8_t downY = YM, downX = XM;  // 圆环外弧 deletemap 右角点
    uint8_t ret = 0;

    if (ADir == goLeft) {
        uint8_t value_1 = 1;  // 区域颜色
        uint8_t value_2 = 2;  // 边界颜色
        uint8_t min = getMapYMin_Col(0, deletemap,
                                     1);  // 圆环外弧与最左侧的交点 （补线终点）
        if (min == YM) {
            min = getMapYMin_Col(0, deletemap, 254);
            value_1 = 254;
            value_2 = 253;
            if (min == YM)
                return 0;
        }

        if (II.top > min - 1)
            II.top = min - 1;
        if (cntMap(0, min) <= 30)  // 最外侧区域区域面积过小
            return 0;

        uint8_t myflag = 0;
        if (value_1 == 1) {
            memset(deletemap, 0, sizeof(deletemap));
            searchmap(0, min, yhmap);
        }

        /****************** 补deletemap里的线 ******************/
        for (uint8_t i = 0; i < XM - 1; ++i) {
            for (uint8_t j = 0; j < YY; ++j) {
                /* deletemap 从下往上遍历 边界->非边界 则补线 */
                if (yhmap[j][i] == value_2 &&
                    (yhmap[j + 1][i] != value_2 || i <= 3)) {
                    rightmap[j][i] = 3;  // 先赋值为3
                    downY = j;
                    downX = i;
                    break;
                } else if (yhmap[j][i] == value_2 &&
                           yhmap[j + 1][i] == value_2) {
                    if (i < XX && getMapYMin_Col(i + 1, yhmap, value_1) > j &&
                        myflag == 0) {
                        myflag = 1;
                        break;
                    } else {
                        rightmap[j][i] = 3;
                        rightmap[j + 1][i] = 3;
                        downY = j;
                        downX = i;
                        break;
                    }
                }
            }
            if (myflag) {
                break;
            }
        }

        if (downY < II.speedTop)
            II.speedTop = downY;
        /*** 极端情况 删除补线 ***/
        if (downY > 55 || downX < 5) {
            for (uint8_t i = min - 10; i < downY + 5 && i < YM; ++i) {
                for (uint8_t j = 0; j <= downX; ++j) {
                    if (rightmap[i][j] == 3)
                        rightmap[i][j] = 0;
                }
            }
            return 0;
        }

        /*** 确定 kk 下降速率 ***/
        uint8_t flag2 = 0;
        int x, y;
        float kk = 1.2;  // 下降速率
        if (II.num_rm && II.start_rm[0] < 25) {
            for (uint8_t i = 1; i < XM; ++i) {
                /*** 向右下延伸 ***/
                x = downX + i;
                y = (int)(downY - i * kk);
                if (x > XX || y < 0)
                    break;
                if (rightmap[y][x])  // 离边线距离较近 减小速率
                    kk = 0.9;
            }
        } else
            kk = 1.5;

        for (uint8_t i = 1; i < XM; ++i) {
            /*** x,y 向右下延伸 ***/
            x = downX + i;  // 从边界/底部/最右侧开始补线
            y = (int)(downY - i * kk);
            if (x > XX || y < 0)
                break;
            if (rightmap[y][x] || x == XX ||
                y == 0)  // 碰到边界||x==XX||y==0：从边界/底部/最右侧开始补线
            {
                flag2 = 1;
                uint8_t bo = y;
                if (rightmap[y][x] == 2)
                    bo = y + 1;
                for (uint8_t k = bo; k < YM; ++k) {
                    for (uint8_t m = 0; m < XM; ++m) {
                        if (rightmap[k][m] == 2)  // 删掉 rightmap
                                                  // 原有的直道边界
                            rightmap[k][m] = 0;
                    }
                }
                break;
            }
        }

        if (flag2) {
            if (II.num_rm)
                drawline2(x, y, downX, downY, rightmap,
                          12);  // 补线 (downX,downY)--->(x,y)
            else
                drawline2(x, y, downX, downY, rightmap, 12);
            ret = 1;
        }

    } else if (ADir == goRight) {
        uint8_t value_1 = 1;
        uint8_t value_2 = 2;
        uint8_t min = getMapYMin_Col(XX, deletemap, 1);
        if (min == YM) {
            min = getMapYMin_Col(XX, deletemap, 254);
            value_1 = 254;
            value_2 = 253;
            if (min == YM) {
                return 0;
            }
        }
        if (II.top > min - 1) {
            II.top = min - 1;
        }
        if (cntMap(XX, min) <= 30) {
            return 0;
        }
        uint8_t myflag = 0;
        if (value_1 == 1) {
            memset(deletemap, 0, sizeof(deletemap));
            searchmap(XX, min, yhmap);
        }
        for (uint8_t i = 0; i < XM - 1; ++i) {
            for (uint8_t j = 0; j < YY; ++j) {
                if (yhmap[j][XX - i] == value_2 &&
                    (yhmap[j + 1][XX - i] != value_2 || i <= 3)) {
                    leftmap[j][XX - i] = 3;
                    downY = j;
                    downX = XX - i;
                    break;
                } else if (yhmap[j][XX - i] == value_2 &&
                           yhmap[j + 1][XX - i] == value_2) {
                    if (i < XX &&
                        getMapYMin_Col(XX - i - 1, yhmap, value_1) > j &&
                        myflag == 0) {
                        myflag = 1;
                        break;
                    } else {
                        leftmap[j][XX - i] = 3;
                        leftmap[j + 1][XX - i] = 3;
                        downY = j;
                        downX = XX - i;
                        break;
                    }
                }
            }
            if (myflag) {
                break;
            }
        }
        if (downY < II.speedTop) {
            II.speedTop = downY;
        }
        if (downY > 55 || downX > XX - 5) {
            for (uint8_t i = min - 10; i < downY + 5 && i < YM; ++i) {
                for (int8_t j = XX; j >= downX; --j) {
                    if (leftmap[i][j] == 3) {
                        leftmap[i][j] = 0;
                    }
                }
            }
            return 0;
        }
        uint8_t flag2 = 0;
        int x, y;
        float kk = 1.2;
        if (II.num_lm && II.start_lm[0] < 25) {
            for (uint8_t i = 1; i < XM; ++i) {
                x = downX - i;
                y = (int)(downY - i * kk);
                if (x < 0 || y < 0) {
                    break;
                }
                if (leftmap[y][x]) {
                    kk = 0.9;
                }
            }
        } else {
            kk = 1.5;
        }
        for (uint8_t i = 1; i < XM; ++i) {
            x = downX - i;
            y = (int)(downY - i * kk);
            if (x < 0 || y < 0) {
                break;
            }
            if (leftmap[y][x] || x == 0 || y == 0) {
                flag2 = 1;
                uint8_t bo = y;
                if (leftmap[y][x] == 2) {
                    bo = y + 1;
                }
                for (uint8_t k = bo; k < YM; ++k) {
                    for (uint8_t m = 0; m < XM; ++m) {
                        if (leftmap[k][m] == 2) {
                            leftmap[k][m] = 0;
                        }
                    }
                }
                break;
            }
        }
        if (flag2) {
            //            qout<<"draw_line";
            drawline2(downX, downY, x, y, leftmap, 12);
            ret = 1;
        }
    }
    return ret;
}

uint8_t ImageHandler::getAnnulusTop() {
    static uint8_t isAnnulus = 0;
    static uint8_t time = 0;

    if (isAnnulus)
        goto CONTINUE;

    /********* 左进圆环 *********/
    do {
        /*** 看到圆环入口或者圆饼就开始判定了 看到圆环的角点才返回值 ***/
        if ((II.num_lm == 1 &&
             !(II.right_y < 45 || (II.right_y == YM && RIGHT(0) < 3))) ||
            II.left_y != YM || II.num_rm != 1 || TOP_RM(0) != YY ||
            TOP_LM(0) > 40 || II.lnum[0] < 5)  // || leftmap[TOP_LM(0)][0] != 0)
            break;
        if (II.num_lm != 1 && II.num_lm != 2)
            break;

        do {
            /****** 初始化 ******/
            uint8_t up = XM, upu = YM, upd = YM, downd = YM, downu = YM;
            RegionInfoLeft info;
            info.right_bottom.x = 0;
            info.right_bottom.y = YY;
            info.right_top.x = 0;
            info.right_top.y = 0;
            info.bottom_right.x = 0;
            info.bottom_right.y = YY;
            info.top_right.x = 0;
            info.top_right.y = 0;
            uint8_t yhmap[YM][XM] = {0};
            uint8_t end;          // 圆饼最右侧横坐标
            uint8_t(*map)[XM];    // 圆环外侧 左上区域
            uint8_t stop = 255;   // 入圆环赛道上边界
            uint8_t start = 255;  // 入圆环赛道下边界
            uint8_t endY;         // 圆环最右侧纵坐标

            /****************** 从basemap/leftmap 获取圆饼图 ******************/
            if (II.num_lm == 1) {
                for (uint8_t j = TOP_LM(0) + 1; j < YM; ++j) {
                    if (basemap[j][RIGHT(0)] && leftmap[j][RIGHT(0)] == 0 &&
                        rightmap[j][RIGHT(0)] == 0) {
                        if (j <= TOP_LM(0) + 5)
                            return YM;
                        getRegionInfoLeft(RIGHT(0), j, basemap, yhmap,
                                          &info);  // 将圆环外侧 basemap
                                                   // 左上角的部分 映射到 yhmap
                        break;
                    }
                    if (j > 55)
                        return YM;
                }
                end = info.right_bottom.x;  // RT 即为圆饼最右侧
                endY = info.right_bottom.y;
                map = yhmap;
                if (info.bottom_right.y < II.start_rm[0] + 15)
                    return YM;
            } else {
                end = RIGHT(1);
                endY = RB_Y(1);
                map = leftmap;
                if (BR_Y(1) < II.start_rm[0] + 15)
                    return YM;
            }
            if (end <= 5)
                return YM;
            /****************** 从basemap/leftmap 获取圆饼图 ******************/

            /****** 判圆饼 并找到圆饼角点 最后返回其高度 ******/
            for (uint8_t j = II.start_lm[0]; j < YY; ++j) {
                if (leftmap[j][0] == 1 &&
                    leftmap[j + 1][0] != 1)  // 黑->白 圆饼上边界
                {
                    for (uint8_t k = j + 1; k < YY; ++k)  // 再往上
                    {
                        if (map[k][0] != 1 &&
                            map[k + 1][0] == 1)  // 白->黑 左上区域下边界
                        {
                            start = k + 1;  // 入圆环赛道上边界
                            stop = j;       // 入圆环赛道下边界
                            break;
                        }
                    }
                    break;
                }
            }

            if (start == 255 || stop == 255)
                return YM;
            if (start < 30)
                return YM;
            if (stop + 5 > start)
                return YM;

            if (RIGHT(0) < 3) {
                if (start < stop + 15)
                    return YM;
                if (II.start_lm[0] + (RIGHT(0) + 1) * 6 < TOP_LM(0))
                    return YM;
            } else {
                if (leftmap[RT_Y(0) + 1][0] == 1)
                    return YM;
            }

            uint8_t down = 0;
            for (uint8_t i = 0, flag = 0; i <= end; ++i) {
                for (uint8_t j = start; j <= endY; ++j) {
                    if (map[j][i] == 1) {
                        if (down > j)
                            return YM;

                        down = j;
                        if (flag == 0) {
                            for (uint8_t k = j; k < YY; ++k) {
                                if (basemap[k][i] != 2)
                                    break;
                                else if (map[k][i] == 1 &&
                                         basemap[k + 1][i] == 0 &&
                                         map[k + 1][i] != 1)  // 圆饼上界
                                {
                                    up = i;
                                    upu = k;
                                    upd = j;
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    if (j == endY)
                        return YM;
                }
            }
            if (up == XM || upu == YM || upd == YM)
                return YM;

            if (up <= 5)
                return YM;
            if (upd < 40)
                return YM;

            for (uint8_t j = start; j < YM; ++j) {
                if (map[j][up - 3] == 1) {
                    for (uint8_t k = j + 1; k < YM; ++k) {
                        if (map[k][up - 3] != 1) {
                            downu = k - 1;  // 入圆环赛道上界
                            downd = j;
                            break;
                        } else if (k == YY) {
                            downu = k;
                            downd = j;
                            break;
                        }
                    }
                    break;
                }
            }
            if (downu == YM || downd == YM)
                return YM;
            if (downu < upu || downd + 1 >= upd ||
                downu - downd <= upu - upd + 1)
                return YM;

            uint8_t downX;
            uint8_t downY;  // 右边界起点纵坐标

            /*** 确定downY ***/
            if (II.start_rm[0] == 0)
                downY = 0;
            else {
                downY = getMapYMin_Col(XX, rightmap, 1);
                if (downY == YM)
                    return YM;
                downY = downY > 0 ? downY - 1 : 0;
            }
            downY = downY > 15 ? downY : 15;

            downX = getMapXMax_Row(downY, rightmap, 2);
            if (downX == XM)
                return YM;
            if (!strJudge(downX, downY, (TL_X(0) > 0 ? TL_X(0) - 1 : 0),
                          TOP_RM(0), rightmap, downY, YY, 1, 3))
                return YM;

            isAnnulus = goLeft;
            time = 0;
            return upd;
        } while (0);
    } while (0);

    do {
        if ((II.num_rm == 1 &&
             !(II.left_y < 45 || (II.left_y == YM && LEFT(0) > XX - 3))) ||
            II.right_y != YM || II.num_lm != 1 || TOP_LM(0) != YY ||
            TOP_RM(0) > 40 || II.rnum[0] < 5)
        //                          || rightmap[TOP_RM(0)][XX] != 0)
        {
            break;
        }
        if (II.num_rm != 1 && II.num_rm != 2) {
            break;
        }

        do {
            uint8_t up = XM, upu = YM, upd = YM, downd = YM, downu = YM;
            RegionInfoRight info;
            info.left_bottom.x = XX;
            info.left_bottom.y = YY;
            info.left_top.x = XX;
            info.left_top.y = 0;
            info.bottom_left.x = XX;
            info.bottom_left.y = YY;
            info.top_left.x = XX;
            info.top_left.y = 0;
            uint8_t yhmap[YM][XM] = {0};
            uint8_t end;
            uint8_t(*map)[XM];
            uint8_t stop = 255;
            uint8_t start = 255;
            uint8_t endY;
            if (II.num_rm == 1) {
                for (uint8_t j = TOP_RM(0) + 1; j < YM; ++j) {
                    if (basemap[j][LEFT(0)] && leftmap[j][LEFT(0)] == 0 &&
                        rightmap[j][LEFT(0)] == 0) {
                        if (j <= TOP_RM(0) + 5) {
                            return YM;
                        }
                        getRegionInfoRight(LEFT(0), j, basemap, yhmap, &info);
                        break;
                    }
                    if (j > 55) {
                        return YM;
                    }
                }
                end = info.left_bottom.x;
                endY = info.left_bottom.y;
                map = yhmap;
                if (info.bottom_left.y < II.start_lm[0] + 15) {
                    return YM;
                }
            } else {
                end = LEFT(1);
                endY = LB_Y(1);
                map = rightmap;
                if (BL_Y(1) < II.start_lm[0] + 15) {
                    return YM;
                }
            }
            if (end >= XX - 5) {
                return YM;
            }
            for (uint8_t j = II.start_rm[0]; j < YY; ++j) {
                if (rightmap[j][XX] == 1 && rightmap[j + 1][XX] != 1) {
                    for (uint8_t k = j + 1; k < YY; ++k) {
                        if (map[k][XX] != 1 && map[k + 1][XX] == 1) {
                            start = k + 1;
                            stop = j;
                            break;
                        }
                    }
                    break;
                }
            }
            if (start == 255 || stop == 255) {
                return YM;
            }
            if (start < 30) {
                return YM;
            }
            if (stop + 5 > start)  // extra
            {
                return YM;
            }
            if (LEFT(0) > XX - 3) {
                if (start < stop + 15) {
                    return YM;
                }
                if (II.start_rm[0] + (XX - LEFT(0) + 1) * 6 < TOP_RM(0)) {
                    return YM;
                }
            } else {
                if (rightmap[LT_Y(0) + 1][XX] == 1) {
                    return YM;
                }
            }

            uint8_t down = 0;
            for (int8_t i = XX, flag = 0; i >= end; --i) {
                for (uint8_t j = start; j <= endY; ++j) {
                    if (map[j][i] == 1) {
                        if (down > j) {
                            return YM;
                        }
                        down = j;
                        if (flag == 0) {
                            for (uint8_t k = j; k < YY; ++k) {
                                if (basemap[k][i] != 2) {
                                    break;
                                } else if (map[k][i] == 1 &&
                                           basemap[k + 1][i] == 0 &&
                                           map[k + 1][i] != 1) {
                                    up = i;
                                    upu = k;
                                    upd = j;
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    if (j == endY) {
                        return YM;
                    }
                }
            }
            if (up == XM || upu == YM || upd == YM) {
                return YM;
            }
            if (up >= XX - 5) {
                return YM;
            }
            if (upd < 40) {
                return YM;
            }
            for (uint8_t j = start; j < YM; ++j) {
                if (map[j][up + 3] == 1) {
                    for (uint8_t k = j + 1; k < YM; ++k) {
                        if (map[k][up + 3] != 1) {
                            downu = k - 1;
                            downd = j;
                            break;
                        } else if (k == YY) {
                            downu = k;
                            downd = j;
                            break;
                        }
                    }
                    break;
                }
            }
            if (downu == YM || downd == YM) {
                return YM;
            }
            if (downu < upu || downd + 1 >= upd ||
                downu - downd <= upu - upd + 1) {
                return YM;
            }
            uint8_t downX;
            uint8_t downY;
            if (II.start_lm[0] == 0) {
                downY = 0;
            } else {
                downY = getMapYMin_Col(0, leftmap, 1);
                if (downY == YM) {
                    return YM;
                }
                downY = downY > 0 ? downY - 1 : 0;
            }
            downY = downY > 15 ? downY : 15;
            downX = getMapXMin_Row(downY, leftmap, 2);
            if (downX == XM) {
                return YM;
            }
            if (!strJudge(downX, downY, (TR_X(0) < XX ? TR_X(0) + 1 : XX),
                          TOP_LM(0), leftmap, downY, YY, 1, 3)) {
                return YM;
            }
            isAnnulus = goRight;
            time = 0;
            return upd;
        } while (0);
    } while (0);
    return YM;

CONTINUE:
    ++time;
    if (time > 50 || IF.annulus) {
        time = 0;
        isAnnulus = 0;
    }
    if (isAnnulus == goLeft) {
        if (II.num_lm == 2)
            return RB_Y(1);

        else if (II.num_lm == 1)
            return RB_Y(0);
    } else if (isAnnulus == goRight) {
        if (II.num_rm == 2)
            return LB_Y(1);
        else if (II.num_rm == 1)
            return LB_Y(0);
    }

    return YM;
}

uint8_t ImageHandler::my_go_Ramp() {
    static int8_t delay = 0;

    /* 上坡 */
    if ((ramp() && !simplify_ramp_flag && angle.Pitch > 2.5) ||
        (angle.Pitch > 3.0 && !simplify_ramp_flag))

    {
        simplify_ramp_flag = 1;
        delay = 15;
    }

    /* 接近坡顶 */
    if (delay && angle.Pitch < 10.0 && simplify_ramp_flag) {
        simplify_ramp_flag = 2;
        delay--;
    }

    /* 下坡 */
    if (delay && angle.Pitch < -5.0 && simplify_ramp_flag) {
        simplify_ramp_flag = 3;
        delay--;
    }

    if (delay <= 0) {
        if (angle.Pitch < 1.0 && angle.Pitch > -1.0) {
            simplify_ramp_flag = 0;
            delay = 0;
        }
    }

    qout << "ramp delay" << delay;
    return simplify_ramp_flag;
}

uint8_t ImageHandler::goRamp() {
    static uint8_t myRampFlag = 0;
    static uint8_t debounce = 0;  // 消抖
    if (ramp() && myRampFlag == 0) {
        if (angle.Pitch > 3) {
            myRampFlag = 1;
            debounce = 0;
        }
    }

    if (angle.Pitch > 3.0 && myRampFlag == 0) {
        myRampFlag = 1;
        debounce = 0;
    }

    if (myRampFlag == 1) {
        ++debounce;
        if (angle.Pitch > 7.0) {
            myRampFlag = 2;
            debounce = 0;
        } else if (debounce > 30) {
            myRampFlag = 0;
            IF.sramp = 0;
            debounce = 0;
        }
    }
    if (angle.Pitch > 10.0 && myRampFlag == 0) {
        myRampFlag = 2;
        IF.sramp = 1;
        debounce = 0;
    }
    if (myRampFlag == 2) {
        ++debounce;
        // if (angle.Pitch < 0 && ramp())
        if (angle.Pitch < 0) {
            myRampFlag = 3;
            debounce = 0;
        } else if (debounce > 50) {
            myRampFlag = 0;
            IF.sramp = 0;
            debounce = 0;
        }
    }
    if (myRampFlag == 3 || myRampFlag == 4) {
        ++debounce;
        // if (debounce > 10 && myRampFlag == 3 && !ramp())
        if (debounce > 10 && myRampFlag == 3) {
            myRampFlag = 4;
            IF.sramp = 0;
        }
        if (debounce > 10) {
            myRampFlag = 0;
            IF.sramp = 0;
            debounce = 0;
        }
    }

    qout << "myRanpFlag" << myRampFlag;
    if (myRampFlag == 1 || myRampFlag == 2 || myRampFlag == 3) {
        deleterampline();
        return myRampFlag;
    } else

        return 0;
}

uint8_t ImageHandler::ramp() {
    uint8_t high = 15;
    uint8_t rampTop = 255;
    uint8_t max = 20;

    /****** 做好误判排除 ******/
    if (II.top != YY)
        return 0;
    //    if(angle.Pitch < -2)
    //    {
    //    	return 0;
    //    }
    for (uint8_t i = 0; i < 5; ++i) {
        if (Y_WBW_Detect(0, YY, i, basemap))
            return 0;
    }
    for (uint8_t i = XX; i > XX - 5; --i) {
        if (Y_WBW_Detect(0, YY, i, basemap))
            return 0;
    }
    if (RT_X(0) < XX - 4) {
        for (uint8_t i = (RT_X(0) < 4 ? RT_X(0) : RT_X(0) - 4); i < RT_X(0) + 4;
             ++i) {
            if (Y_WBW_Detect(0, YY, i, basemap))
                return 0;
        }
    }
    if (LT_X(0) > 4) {
        for (int8_t i = (LT_X(0) > XX - 4 ? LT_X(0) : LT_X(0) + 4);
             i > LT_X(0) - 4; --i) {
            if (Y_WBW_Detect(0, YY, i, basemap))
                return 0;
        }
    }

    /************ 最后的宿命都是 goto BOTO_SIDES ************/
    if (II.num_lm == 1 && II.num_rm == 0)  // 仅有左线
    {
        // if (TFMINI_PLUS_DISTANCE > 80 || TFMINI_PLUS_DISTANCE < 10)
        //{
        //     goto BOTH_SIDES;
        // }

        /****** 及早排除误判 斜对坡道时将双线错误判定为单线 ******/

        if (TOP_LM(0) != YY)  // TR.X==YY 双线情况
            goto BOTH_SIDES;
        // 车头未正对坡道时 即使实际上为双线 其中一个赛道边线却可能在 deletemap
        // 中
        if (YM != getMapYMin_Col(XX, deletemap, 1))
            goto BOTH_SIDES;

        for (uint8_t j = 0; j < YM; ++j) {
            // 基本同上 车头未正对坡道
            uint8_t r = getMapXMin_Row(j, deletemap, 2);
            // 跳转到 basemap 里判断
            if (Y_RBR_Detect(0, YY, r + 1, basemap))
                goto BOTH_SIDES;
        }

        uint8_t num_large = 0;
        for (uint8_t j = 0; j < YM; ++j) {
            uint8_t l = getMapXMax_Row(j, leftmap, 1);
            if (l == XM)  // 此时底部为全白 返回XM
                goto BOTH_SIDES;

            uint8_t r = getMapXMin_Row(j, deletemap, 1);
            if (r != XM) {
                if (r <= l + rightline[YY] - leftline[YY] + 2)
                    goto BOTH_SIDES;
            }
            if (r == XM)
                r = XM;

            for (uint8_t i = 0; i < 4; ++i) {
                // 上坡时斜对坡 num_lm或num_rm可能为0
                // 此时远景赛道与坡顶可能形成内凹 出现 白->黑->白（竖直方向）
                if (Y_WBW_Detect(0, YY, l + i, basemap) ||
                    Y_WBW_Detect(0, YY, r - i, basemap))
                    goto BOTH_SIDES;
            }
            if (j >= YY - high) {
                if (r >= l + rightline[j] - leftline[j] +
                             (float)(j - (YY - high)) / high * 4 + 4)
                    num_large++;
            }
        }
        if (num_large >= 8)
            return 1;
        goto BOTH_SIDES;
    } else if (II.num_lm == 0 && II.num_rm == 1)  // 仅有右线
    {
        // if (TFMINI_PLUS_DISTANCE > 80 || TFMINI_PLUS_DISTANCE < 10)
        //{
        //     goto BOTH_SIDES;
        // }
        if (TOP_RM(0) != YY)
            goto BOTH_SIDES;
        if (YM != getMapYMin_Col(0, deletemap, 1))
            goto BOTH_SIDES;

        for (uint8_t j = 0; j < YM; ++j) {
            uint8_t l = getMapXMax_Row(j, deletemap, 2);
            if (Y_RBR_Detect(0, YY, l - 1, basemap))
                goto BOTH_SIDES;
        }

        uint8_t num_large = 0;
        for (uint8_t j = 0; j < YM; ++j) {
            uint8_t r = getMapXMin_Row(j, rightmap, 1);
            if (r == XM)
                goto BOTH_SIDES;
            uint8_t l = getMapXMax_Row(j, deletemap, 1);
            if (l != XM) {
                if (r <= l + rightline[YY] - leftline[YY] + 2)
                    goto BOTH_SIDES;
            }
            if (l == XM)
                l = 0;
            for (uint8_t i = 0; i < 4; ++i) {
                if (Y_WBW_Detect(0, YY, r + i, basemap) ||
                    Y_WBW_Detect(0, YY, l - i, basemap))
                    goto BOTH_SIDES;
            }
            if (j >= YY - high) {
                if (r >= l + rightline[j] - leftline[j] +
                             (float)(j - (YY - high)) / high * 4 + 2)
                    num_large++;
            }
        }
        if (num_large >= 8)
            return 1;
        goto BOTH_SIDES;
    }

BOTH_SIDES:
    if (II.num_lm <= 1 && II.num_rm <= 1) {
        // if (!GET_SWITCH5() && (TFMINI_PLUS_DISTANCE > 100 ||
        // TFMINI_PLUS_DISTANCE < 10))
        //{
        //     return 0;
        // }

        // 为1时 TR.Y（59）一般都不会小于55
        if ((II.num_lm == 1 && TOP_LM(0) < 55) ||
            (II.num_rm == 1 && TOP_RM(0) < 55))
            return 0;

        uint8_t(*mapLeft)[XM] = leftmap;
        uint8_t(*mapRight)[XM] = rightmap;

        if (II.num_lm == 0) {
            uint8_t topL = getMapYMax_Col(
                0, deletemap, 1);  // 若找到 值较大 实际在判定是否存在
            if (topL == YM || topL < 55)
                return 0;
            if (getMapYMin_Col(XX, rightmap, 1) >=
                20)  // 对应上坡时 斜对坡 一般起始行为17左右
                return 0;
            for (uint8_t j = 0; j < YY; ++j) {
                uint8_t l = getMapXMax_Row(j, deletemap, 1);
                if (Y_WBW_Detect(0, YY, l - 1, basemap))
                    return 0;
            }
            mapLeft = deletemap;  // num_lm为0 跳转到deletemap
        }
        if (II.num_rm == 0) {
            uint8_t topR = getMapYMax_Col(XX, deletemap, 1);
            if (topR == YM || topR < 55)
                return 0;
            if (getMapYMin_Col(0, leftmap, 1) >= 20)
                return 0;
            for (uint8_t j = 0; j < YY; ++j) {
                uint8_t r = getMapXMin_Row(j, deletemap, 1);
                if (Y_WBW_Detect(0, YY, r + 1, basemap))
                    return 0;
            }
            mapRight = deletemap;
        }
        if (II.num_lm == 0 && II.num_rm == 0)
            return 0;

        uint8_t n = 0;
        for (uint8_t i = 0; i < max; ++i)  // max=20
        {
            for (uint8_t j = 0; j < XM; ++j) {
                if (mapLeft[YY - i][XX - j] == 1)  // 自顶向底 自右向左
                {
                    for (uint8_t k = XX - j + 1; k < XM; ++k) {
                        if (mapRight[YY - i][k] ==
                            1)  // 在此行向右 找右边界 k 右边界横坐标
                        {
                            if (rampTop == 255)  // 未曾赋值
                                rampTop = YY - i;
                            if (k - (XX - j) <=
                                    rightline[YY - i] - leftline[YY - i] +
                                        (float)(high - (rampTop - (YY - i))) *
                                            4 / high +
                                        2 ||
                                k < 20 || XX - j > 26)  // 赛道宽度<=
                                return 0;
                            ++n;  // 能清楚的找到左右边界
                            break;
                        }
                        if (k == XX &&
                            i > max - high)  // 黑点 在最右侧 在顶部 退出
                            return 0;
                    }
                    break;
                }
                if (j == XX && i > max - high)
                    return 0;
            }
            if (n >= 15)
                break;
        }
        if (n < 15)
            return 0;

        uint8_t recX = XM, lastRecX = XM;
        for (uint8_t j = YY; j > YM - 20; --j) {
            for (uint8_t i = 0; i < XM; ++i) {
                if (leftmap[j][XX - i])  // 自顶向底 自右向左
                {
                    recX = XX - i;
                    break;
                }
            }
            if (recX > lastRecX)
                return 0;
            lastRecX = recX;
        }
        for (uint8_t j = YY; j > YM - 20; --j) {
            for (uint8_t i = 0; i < XM; ++i) {
                if (rightmap[j][i]) {
                    recX = j;
                    break;
                }
            }
            if (recX < lastRecX)
                return 0;
        }
        return 1;
    }
    return 0;
}

void ImageHandler::deleterampline() {
    // 避免坡道上往左往右
    deleteforleft(1);
    deleteforright(1);
}

//
uint8_t ImageHandler::goParking() {
    static uint8_t parkingStatus = 0;
    static uint8_t in = 0;
    static uint8_t park_detect = 0;
    static int8_t park_delay = 10;

    if (parkingStatus == 0 && parkingDetect()) {
#ifndef T_REX_QT_DEBUG
        BEEP(200);
#endif
        parkingStatus = 1;
        in = 0;
    }
    //    qout<<"parkingStatus"<<parkingStatus;
    //    qout<<"park_delay"<<park_delay;
    //    qout<<"park_detect"<<park_detect;

    // 第一圈直接屏蔽
    if (parkingStatus == 1 && parkingDir == goStraight) {
        park_detect++;
        parkingStatus = 0;

        return 0;
    }

    // 确认判定到车库后 延时屏蔽
    if (park_detect > 2 && parkingDir == goStraight) {
        if (park_delay <= 0) {
            parkingDir = goRight;
        }
        park_delay--;

        return 0;
    }

    if (parkingStatus == 1) {
        do {
            uint16_t inum_all = XM * YM + II.repeatNum - II.bnum_all -
                                II.lnum_all - II.rnum_all - II.dnum_all;
            testBox.test1 = inum_all;
            if (inum_all == 0) {
                if (parkingEnter(parkingDir) || noWayDetect()) {
#ifndef T_REX_QT_DEBUG
                    BEEP(500);
#endif
                    parkingStatus = 2;
                    break;
                }
            }
            uint8_t ret = parkingDeal(parkingStatus, parkingDir);
        } while (0);
    }
    if (parkingStatus == 0) {
        return 0;
    }
    return (parkingDir - 1) * 3 + parkingStatus;
}

uint8_t ImageHandler::my_goout_park() {
    static uint8_t outpark_status = 0;
    static uint8_t outpark_flag = 0;

    uint8_t dead_x = 25;

    if (outpark_status == 0 && outpark_detect()) {
        outpark_status = 1;
    }
    if (outpark_status && outpark_detect()) {
        if (outpark_dir == goRight) {
            Point up_point;
            //            if(RT_X(0)<18 && RT_Y(0)<20)
            //            {
            //                down_point.x=RT_X(0);
            //                down_point.y=RT_Y(0);
            //                qout<<"down_point.x"<<down_point.x;
            //                qout<<"down_point.y"<<down_point.y;
            //            }
            //            else if(II.num_lm==0)
            //            {
            //                outpark_delay--;
            //                if(outpark_delay<=0)
            //                {
            //                    outpark_delay=3;
            //                    return 0;
            //                }
            //                down_point.x=0;
            //                down_point.y=0;
            //            }
            //            else
            //            {
            //                qout<<"down_point fail";
            //                return 0;
            //            }

            uint8_t temp = getMapYMax_Col(dead_x, basemap, 2);
            if (temp != YM && temp < 55 && temp > 25) {
                up_point.x = XX;
                up_point.y = temp;
                qout << "up_point.x" << up_point.x;
                qout << "up_point.y" << up_point.y;
            } else {
                qout << "up_point fail";

                return 0;
            }

            int8_t j, i;
            for (j = up_point.y; j >= 0; --j) {
                for (i = up_point.x; i >= 0; --i) {
                    if (leftmap[j][i] >= 2)
                        leftmap[j][i] = 0;
                }
            }
            draw_straight(dead_x, up_point.y, 0, up_point.y / 2, leftmap);

            outpark_status = 2;
        }
    }
    if (outpark_status == 2 && !outpark_detect()) {
        outpark_flag++;
        if (outpark_flag > 3) {
            outpark_flag = 0;
            outpark_status = 0;
        }
    }

    return outpark_status;
}

uint8_t ImageHandler::outpark_detect() {
    if (II.top > 50)
        return 0;

    uint8_t wbwCnt = 0;

    for (uint8_t i = 20; i < 40; ++i) {
        if (y_cnt_WBW_num(0, YY, i, insidemap) > 3)  // 列内白黑白跳变多次
            ++wbwCnt;
        if (wbwCnt >= 3)  // 多列出现跳变
            return 1;
    }

    return 0;
}

uint8_t ImageHandler::parkingDetect() {
    if (II.top < 55 || (II.top_right[II.num_lm - 1].y < 55 &&
                        II.top_left[II.num_rm - 1].y < 55)) {
        //        testBox.test3 = 1;
        return 0;
    }
    if (detectStartline_roughly())
        return 1;

    return 0;
}

uint8_t ImageHandler::parkingDeal(uint8_t PStatus, uint8_t PDir) {
    if (PDir == goLeft) {
        uint8_t x1 = XX, y1 = 0, x2 = XX, y2 = YY;
        uint8_t value1 = 1, value2 = 2;  // value1:区域色 value2:边界色

        if (II.num_rm >= 2) {
            for (uint8_t k = 0; k < II.num_rm - 1; ++k) {
                for (uint8_t j = BL_Y(k) - 2; j <= TL_Y(k) + 2;
                     ++j)  // BL.Y-2~TL.Y+2
                {
                    for (uint8_t i = LT_X(0) - 2; i < XM; ++i)  // LT.X-2~XM
                    {
                        if (rightmap[j][i] == 2)  // 将右边界投影内的起跑线删除
                            rightmap[j][i] = 0;
                    }
                }
            }
        }

        // 补线第二个点
        // 左上的区域可能在deletemap，也可能是左图把左边都搜进去了，两种情况
        uint8_t flag = 0;  // 0:左上区域未在 deletemap 中
                           // 1:左上区域在deletemap中
        uint8_t min = getMapYMin_Col(0, deletemap, 1);
        if (min != YM && cntMap(0, min) < 30)
            min = YM;

        flag = 1;
        if (min == YM) {
            min = getMapYMin_Col(0, deletemap, 254);
            if (min != YM) {
                flag = 1;
                value1 = 254;
                value2 = 253;
            } else if (min == YM) {
                flag = 0;
                if (II.num_lm >= 2)  // 左图多个区域，左上角搜入deletemap
                {
                    flag = 1;
                    memset(deletemap, 0,
                           sizeof(deletemap));  // 清除数组图像 重新搜图
                    searchmap(0, YY, deletemap);
                    min = getMapYMin_Col(0, deletemap, 1);
                } else if (II.num_lm == 1) {
                    flag = 1;
                    for (uint8_t i = 0; i < XM / 2; ++i) {
                        // 右转接左车库，可能出新WBWB的情况
                        for (uint8_t j = 0; j < YM; ++j) {
                            if (leftmap[j][i] == 0) {
                                for (uint8_t k = j + 1; k < YM; ++k) {
                                    if (leftmap[k][i] == 1) {
                                        for (uint8_t l = k + 1; l < YM; ++l) {
                                            if (leftmap[l][i] == 0) {
                                                for (uint8_t m = l + 1; m < YM;
                                                     ++m) {
                                                    if (leftmap[m][i] ==
                                                        1)  // 白->黑->白->黑
                                                    {
                                                        if (BR_Y(0) < 10 &&
                                                            BR_X(0) < 5) {
                                                            flag = 0;
                                                            break;
                                                        }
                                                    }
                                                }
                                                break;
                                            }
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        if (flag == 0) {
                            break;
                        }
                    }
                    if (flag)  // 左转接左车库，只有左图上半部?
                    {
                        memset(deletemap, 0, sizeof(deletemap));
                        if (II.start_lm[0] == 0)
                            searchmap(0, YY, deletemap);
                        else if (II.start_lm[0] >= YM / 3)
                            searchmap(0, II.start_lm[0], deletemap);
                        min = getMapYMin_Col(0, deletemap, 1);
                    } else  // 右转接左车库，左图一个区域将左边线信息都搜进去了
                    {
                        for (uint8_t i = 0; i < XM / 2; ++i) {
                            min = getMapYMax_Col(i, leftmap, 2);
                            if (min > 40 && min != YM)
                                break;
                        }
                    }
                }
            }
            // else
            //{
            //     flag = 1;
            //     min = getMapYMin_Col(0, deletemap, 254);
            //     value1 = 254;
            //     value2 = 253;
            //     if(min == YM)
            //     {
            //         return 0;
            //     }
            // }
        }
        if (II.top > min - 1)
            II.top = min - 1;

        if (flag == 1) {
            uint8_t myFlag = 0;
            for (uint8_t i = 3; i < XX; ++i) {
                for (uint8_t j = 0; j < YY; ++j) {
                    if (deletemap[j][i] == value2 &&
                        (deletemap[j + 1][i] != value2 || i < 3)) {
                        rightmap[j][i] = 3;
                        x2 = i;  // 左上区域右下角点
                        y2 = j;
                        break;
                    } else if (deletemap[j][i] == value2 &&
                               deletemap[j + 1][i] == value2) {
                        if (i < XX && getMapYMin_Col(i + 1, deletemap, 1) > j &&
                            myFlag == 0) {
                            myFlag = 1;
                            break;
                        } else {
                            rightmap[j][i] = 3;
                            rightmap[j + 1][i] = 3;
                            x2 = i;
                            y2 = j;
                            break;
                        }
                    }
                }
                if (myFlag)
                    break;
            }
        } else if (flag == 0)  // 右转接左车库，信息都在左图的情况
        {
            uint8_t recTopX = 0, recTopY = YM, recBottomX = XM, recBottomY = 0;

            for (uint8_t j = YY; j >= YM / 2; --j) {
                if (X_WBW_Detect(XX, 0, j, leftmap, goLeft)) {
                    recTopY = j;  // 起跑线高点
                    break;
                }
            }
            if (recTopY == YM)
                return 0;

            for (uint8_t i = 0; i < XM / 2; ++i) {
                if (leftmap[recTopY][i] == 2) {
                    recTopX = i;  // 起跑线左高点
                    break;
                }
            }
            for (uint8_t j = recTopY; j >= YM / 2; --j) {
                if (!X_WBW_Detect(XX, 0, j, leftmap, goLeft))  // 起跑线低点
                {
                    recBottomY = j;  // 确定补线起点纵坐标
                    break;
                }
            }
            if (recBottomY == 0)
                return 0;

            for (int8_t i = XX; i >= 0; --i) {
                if (leftmap[recBottomY][i] == 2)  // 在recBottomY行 从右往左
                {
                    recBottomX = i;  // 确定补线起点纵坐标
                    break;
                }
            }
            x2 = recBottomX;
            y2 = recBottomY;
            for (uint8_t j = min; j <= recTopY; ++j) {
                for (uint8_t i = 0; i <= recTopX; ++i) {
                    if (leftmap[j][i] == 2)  // 将起跑线左下部分删除
                        leftmap[j][i] = 0;
                }
            }
        }

        for (uint8_t j = y2 + 1; j < YM; ++j)  // 删除右边界向上延伸的边线
        {
            for (int8_t i = XX; i >= x2; --i) {
                if (leftmap[j][i] == 2)
                    leftmap[j][i] = 0;
            }
        }

        /****** 和进圆环类似的补线 ******/
        uint8_t flag2 = 0;
        float kk = 1.2;
        if (II.num_rm && II.start_rm[0] < 25) {
            for (uint8_t i = 1; i < XM; ++i) {
                x1 = x2 + i;
                y1 = (int)(y2 - i * kk);
                if (x1 > XX || y1 < 0)
                    break;
                if (rightmap[y1][x1])
                    kk = 0.9;
            }
        } else
            kk = 1.5;

        for (uint8_t i = 1; i < XM; ++i) {
            x1 = x2 + i;
            y1 = (int)(y2 - i * kk);
            if (x1 > XX || y1 < 0)
                break;

            if (rightmap[y1][x1] || x1 == XX || y1 == 0) {
                flag2 = 1;
                uint8_t bo = y1;
                if (rightmap[y1][x1] == 2)
                    bo = y1 + 1;
                for (uint8_t k = bo; k < YM; ++k) {
                    for (uint8_t m = 0; m < XM; ++m) {
                        if (rightmap[k][m] == 2)  // 删去原有的直道右边线
                            rightmap[k][m] = 0;
                    }
                }
                break;
            }
        }

        if (PStatus == 1 && II.num_lm) {
            uint8_t _num = 0;
            if (getMapYMin_Col(0, deletemap, 1) == YM &&
                getMapYMin_Col(0, deletemap, 254) == YM)
                _num = 1;  // 判定 deletemap 里是否有信息

            for (uint8_t k = 0; k < II.num_lm - _num; ++k) {
                uint8_t down = 0, up = YY;
                if (BR_Y(k) > 2)
                    down = BR_Y(k) - 2;  // BT.Y-2
                if (TR_Y(k) < YY - 2)
                    up = TR_Y(k) + 2;  // TR.Y+2
                for (uint8_t j = down; j < up; ++j) {
                    for (uint8_t i = 0; i < 20; ++i) {
                        if (leftmap[j][i] == 2)  // 删去搜进左图的部分起跑线
                            leftmap[j][i] = 0;
                    }
                }
            }
        }

        if (y2 < II.speedTop)
            II.speedTop = y2;

        drawline2(x1, y1, x2, y2, rightmap, 12);
    } else if (PDir == goRight) {
        uint8_t x1 = 0, y1 = 0, x2 = 0, y2 = YY;
        uint8_t value1 = 1, value2 = 2;

        if (II.num_lm >= 2) {
            for (uint8_t k = 0; k < II.num_lm - 1; ++k) {
                for (uint8_t j = BR_Y(k) - 2; j <= TR_Y(k) + 2; ++j) {
                    for (int8_t i = RT_X(k) + 2; i >= 0; --i) {
                        if (leftmap[j][i] == 2)
                            leftmap[j][i] = 0;
                    }
                }
            }
        }

        uint8_t flag = 0;
        uint8_t min = getMapYMin_Col(XX, deletemap, 1);
        if (min != YM && cntMap(XX, min) < 30) {
            min = YM;
        }
        flag = 1;
        if (min == YM)  // 没有dmap或����dmap为截至下寻的内容
        {
            min = getMapYMin_Col(XX, deletemap, 254);
            if (min != YM) {
                flag = 1;
                value1 = 254;
                value2 = 253;
            } else if (min == YM) {
                flag = 0;
                if (II.num_rm >= 2) {
                    flag = 1;
                    memset(deletemap, 0, sizeof(deletemap));
                    searchmap(XX, YY, deletemap);
                    min = getMapYMin_Col(XX, deletemap, 1);
                } else if (II.num_rm == 1) {
                    flag = 1;
                    for (uint8_t i = 0; i < XM / 2; ++i) {
                        for (uint8_t j = 0; j < YM; ++j) {
                            if (rightmap[j][XX - i] == 0) {
                                for (uint8_t k = j + 1; k < YM; ++k) {
                                    if (rightmap[k][XX - i] == 1) {
                                        for (uint8_t l = k + 1; l < YM; ++l) {
                                            if (rightmap[l][XX - i] == 0) {
                                                for (uint8_t m = l + 1; m < YM;
                                                     ++m) {
                                                    if (rightmap[m][XX - i] ==
                                                        1) {
                                                        if (BL_Y(0) < 10 &&
                                                            BL_X(0) > XX - 5) {
                                                            flag = 0;
                                                            break;
                                                        }
                                                    }
                                                }
                                                break;
                                            }
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        if (flag == 0) {
                            break;
                        }
                    }
                    if (flag)  // 右转接右车库可能会遇��?
                    {
                        memset(deletemap, 0, sizeof(deletemap));
                        if (II.start_rm[0] == 0) {
                            searchmap(XX, YY, deletemap);
                        } else if (II.start_rm[0] >= YM / 3) {
                            searchmap(XX, II.start_rm[0], deletemap);
                        }
                        min = getMapYMin_Col(XX, deletemap, 1);
                    } else  // 左转接右车库，信息都搜入右图的情��?
                    {
                        for (uint8_t i = 0; i < XM / 2; ++i) {
                            min = getMapYMax_Col(XX - i, rightmap, 2);
                            if (min > 40 && min != YM) {
                                break;
                            }
                        }
                    }
                }
            }
            // else
            //{
            //     flag = 1;
            //     min = getMapYMin_Col(XX, deletemap, 254);
            //     value1 = 254;
            //     value2 = 253;
            //     if(min == YM)
            //     {
            //         return 0;
            //     }
            // }
        }
        if (II.top > min - 1) {
            II.top = min - 1;
        }

        if (flag == 1) {
            uint8_t myFlag = 0;  // 右边5列不提取
            for (uint8_t i = 3; i < XX; ++i) {
                for (uint8_t j = 0; j < YY; ++j) {
                    if (deletemap[j][XX - i] == value2 &&
                        (deletemap[j + 1][XX - i] != value2 || i <= 3)) {
                        leftmap[j][XX - i] = 3;
                        y2 = j;
                        x2 = XX - i;
                        break;
                    } else if (deletemap[j][XX - i] == value2 &&
                               deletemap[j + 1][XX - i] == value2) {
                        if (i < XX &&
                            getMapYMin_Col(XX - i - 1, deletemap, 1) > j &&
                            myFlag == 0) {
                            myFlag = 1;
                            break;
                        } else {
                            leftmap[j][XX - i] = 3;
                            leftmap[j + 1][XX - i] = 3;
                            y2 = j;
                            x2 = XX - i;
                            break;
                        }
                    }
                }
                if (myFlag) {
                    break;
                }
            }
        } else if (flag == 0) {
            uint8_t recTopX = XX, recTopY = YM, recBottomX = 0, recBottomY = YM;
            for (uint8_t j = YY; j >= YM / 2; --j) {
                if (X_WBW_Detect(0, XX, j, rightmap, goRight)) {
                    recTopY = j;
                    break;
                }
            }
            if (recTopY == YM) {
                return 0;
            }
            for (uint8_t i = 0; i < XM / 2; ++i) {
                if (rightmap[recTopY][XX - i] == 2) {
                    recTopX = XX - i;
                    break;
                }
            }
            for (uint8_t j = recTopY; j >= YM / 2; --j) {
                if (!X_WBW_Detect(0, XX, j, rightmap, goRight)) {
                    recBottomY = j;
                    break;
                }
                for (uint8_t i = 0; i < XM; ++i) {
                    if (rightmap[recBottomY][i] == 2) {
                        recBottomX = i;
                        break;
                    }
                }
                x2 = recBottomX;
                y2 = recBottomY;
                for (uint8_t j = min; j <= recTopY; ++j) {
                    for (int8_t i = XX; i >= recTopX; --i) {
                        if (rightmap[j][i] == 2) {
                            rightmap[j][i] = 0;
                        }
                    }
                }
            }
        }
        for (uint8_t j = y2 + 1; j < YM; ++j)  // 右图补线的点向上延伸的点
        {
            for (uint8_t i = 0; i < XM; ++i) {
                if (rightmap[j][i] == 2) {
                    rightmap[j][i] = 0;
                }
            }
        }

        uint8_t flag2 = 0;
        float kk = 1.2;
        if (II.num_lm && II.start_lm[0] < 25) {
            for (uint8_t i = 1; i < XM; ++i) {
                x1 = x2 - i;
                y1 = (int)(y2 - i * kk);
                if (x1 < 0 || y1 < 0) {
                    break;
                }
                if (leftmap[y1][x1]) {
                    kk = 0.9;
                }
            }
        } else {
            kk = 1.5;
        }
        for (uint8_t i = 1; i < XM; ++i) {
            x1 = x2 - i;
            y1 = (int)(y2 - i * kk);
            if (x1 < 0 || y1 < 0) {
                break;
            }
            if (leftmap[y1][x1] || x1 == 0 || y1 == 0) {
                flag2 = 1;
                uint8_t bo = y1;
                if (leftmap[y1][x1] == 2) {
                    bo = y1 + 1;
                }
                for (uint8_t k = bo; k < YM; ++k) {
                    for (uint8_t m = 0; m < XM; ++m) {
                        if (leftmap[k][m] == 2) {
                            leftmap[k][m] = 0;
                        }
                    }
                }
                break;
            }
        }

        if (PStatus == 1 && II.num_rm)  // 入库时起跑线搜入左右图的情况
        {
            uint8_t _num = 0;
            if (getMapYMin_Col(XX, deletemap, 1) == YM &&
                getMapYMin_Col(XX, deletemap, 254) == YM) {
                _num = 1;
            }
            for (uint8_t k = 0; k < II.num_rm - _num; ++k) {
                uint8_t down = 0, up = YY;
                if (BL_Y(k) > 2) {
                    down = BL_Y(k) - 2;
                }
                if (TL_Y(k) < YY - 2) {
                    up = TL_Y(k) + 2;
                }
                for (uint8_t j = down; j < up; ++j) {
                    for (uint8_t i = 0; i < 20; ++i) {
                        if (rightmap[j][XX - i] == 2) {
                            rightmap[j][XX - i] = 0;
                        }
                    }
                }
            }
        }
        if (y2 < II.speedTop) {
            II.speedTop = y2;
        }
        // drawline2(x2, y2, x1, y1, leftmap, 12);
        qout << "x2" << x2;
        qout << "y2" << y2;
        draw_straight(x2, y2, 0, y2 / 3, leftmap);
    }
}

/****** 开始进入后 出现V区域 v内为黑 ******/
uint8_t ImageHandler::parkingEnter(uint8_t PDir) {
    uint8_t _cnt = 0;
    if (PDir == goLeft) {
        uint8_t end = getMapYMin_Col(0, basemap, 1);
        if (end != YM) {
            for (uint8_t j = 0; j < end; ++j) {
                // 用横坐标方向上的 非黑->黑->非黑来识别V区域
                testBox.test2 = X_WBW_Detect(0, XX, j, basemap, goRight);
                if (X_WBW_Detect(0, XX, j, basemap, goRight)) {
                    ++_cnt;
                    testBox.test0 = _cnt;
                }
                if (_cnt > 15)
                    return 1;
            }
        }
    } else if (PDir == goRight) {
        uint8_t end = getMapYMin_Col(XX, basemap, 1);
        testBox.test3 = end;
        if (end != YM) {
            for (uint8_t j = 0; j < end; ++j) {
                testBox.test2 = X_WBW_Detect(0, XX, j, basemap, goRight);
                if (X_WBW_Detect(XX, 0, j, basemap, goLeft)) {
                    ++_cnt;
                    testBox.test0 = _cnt;
                }
                if (_cnt > 15)
                    return 1;
            }
        }
    }
    return 0;
}

uint8_t ImageHandler::detectStartline() {
    uint16_t inum_all = XM * YM + II.repeatNum - II.bnum_all - II.lnum_all -
                        II.rnum_all - II.dnum_all;

    if (inum_all < 80 || inum_all > 500) {
        return 0;
    }
    uint8_t startMap[YM][XM] = {{0}};
    uint8_t num = 0;
    uint16_t pixelNum = 0;
    for (uint8_t j = 0; j < YM / 2; ++j) {
        for (uint8_t i = 0; i < XX; ++i) {
            if (insidemap[j][i]) {
                for (uint8_t k = i; k < XX; ++k) {
                    if (insidemap[j][k] && !startMap[j][k]) {
                        numCnt = 0;
                        searchCountmap(k, j, startMap);
                        if (numCnt > 5) {
                            ++num;
                            pixelNum += numCnt;
                        }
                    }
                }
                break;
            }
        }
    }
    if (num >= 5 && pixelNum > 80) {
        return 1;
    }
    return 0;
}

uint8_t ImageHandler::detectStartline_roughly() {
    uint16_t inum_all = XM * YM + II.repeatNum - II.bnum_all - II.lnum_all -
                        II.rnum_all - II.dnum_all;
    if (inum_all < 10 || inum_all > 500)
        return 0;

    uint8_t startMap[YM][XM] = {{0}};
    uint8_t num = 0;
    uint8_t wbwCnt = 0;
    uint16_t pixelNum = 0;
    for (uint8_t j = 0; j < YM; ++j) {
        for (uint8_t i = 0; i < XX; ++i) {
            if (insidemap[j][i])  // 斑马线 在insidemap中
            {
                for (uint8_t k = i; k < XX; ++k) {
                    if (insidemap[j][k] && !startMap[j][k]) {
                        numCnt = 0;
                        searchCountmap(k, j, startMap);
                        if (numCnt >= 5) {
                            ++num;
                            pixelNum += numCnt;
                        }
                    }
                }
                break;
            }
        }
    }

    //    testBox.test0 = num;
    //    testBox.test1 = pixelNum;

    if (num >= 3 && pixelNum > 20) {
        for (uint8_t j = 20; j < 40; ++j) {
            if (cnt_WBW_num(0, XX, j, insidemap) > 3)  // 一行内白黑白跳变多次
                ++wbwCnt;
            if (wbwCnt >= 3)  // 多行出现跳变
                return 1;
        }
    }

    return 0;
}

uint8_t ImageHandler::noWayDetect() {
    uint8_t startLY = 255;
    uint8_t startRY = 255;
    do {
        if ((II.num_lm == 1 && II.num_rm == 1) ||
            (II.num_lm == 1 && II.num_rm == 0) ||
            (II.num_lm == 0 && II.num_rm == 1)) {
            if (II.top > 50)
                break;
            if (!II.repeatNum) {
                if (II.num_lm == 0) {
                    if (getMapYMax_Col(0, deletemap, 254) == II.top) {
                        uint8_t min = getMapYMin_Col(0, deletemap, 254);
                        if (min == YM)
                            break;
                        searchleftmap(0, min);
                    } else
                        break;
                } else if (II.num_rm == 0) {
                    if (getMapYMax_Col(XX, deletemap, 254) == II.top) {
                        uint8_t min = getMapYMax_Col(XX, deletemap, 254);
                        if (min == YM)
                            break;
                        searchrightmap(XX, II.top);
                    } else
                        break;
                } else
                    break;
            }

            if (TOP_LM(0) != YY || TOP_RM(0) != YY)  // 出界时 TR.Y=YY TL.Y=YY
                break;

            if (II.repeatNum != (YY - II.top) * XM)
                break;

            startLY = 0;
            startRY = 0;
            for (uint8_t i = 0; i < II.num_lm; ++i) {
                if (II.start_lm[i] > startLY)
                    startLY = II.start_lm[i];
            }
            for (uint8_t i = 0; i < II.num_rm; ++i) {
                if (II.start_rm[i] > startRY)
                    startRY = II.start_rm[i];
            }
            uint8_t max = startLY > startRY ? startLY : startRY;
            if (!(II.top >= max + 5 || (II.lnum_all > 20 + II.repeatNum &&
                                        II.rnum_all > 20 + II.repeatNum)))
                break;

            uint8_t leftDown = getMapYMin_Col(0, leftmap, 1);
            uint8_t rightDown = getMapYMin_Col(XX, rightmap, 1);
            if (leftDown + 5 > II.top || rightDown + 5 > II.top)
                break;
            return 1;
        }
    } while (0);
}

/* 三岔判定 并返回三岔最低点坐标 */
Point ImageHandler::fork_detect() {}

/* 获得三岔最低点后 进行三岔补线 */
void ImageHandler::fork_deal(Point down_point, uint8_t dir) {}

/* 通过三岔判定获得三岔底部顶点并进行三岔补线 */
uint8_t ImageHandler::go_fork() {}

/* 判定三岔最低点 */
// uint8_t ImageHandler::get_Forkdown(uint8_t dir) {
//     return 0;
// }

/*** 对应十字时 丢线导致 linemap缺中间段
    通过对 leftmap的检索 找到 上下点 并补线 ***/
uint8_t ImageHandler::draw_crossroad_leftline() {
    uint8_t temp = 0, temp_2 = 0;
    uint8_t j = 0;
    Point down_point = {0, 0};  // 低点
    Point up_point = {0, 0};    // 高点

    /* 确定搜索行起点 */
    temp = getMapXMax_Row(0, leftmap, 2);
    if (temp == XM) {
        temp = getMapYMin_Col(0, leftmap, 2);
        if (temp == YM) {
            //            qout<<" start_point";
            return 0;
        } else
            temp_2 = temp;
    } else
        temp_2 = 0;

    /* 开始遍历 找寻"丢线"处 */
    for (j = temp_2; j < YM; ++j) {
        temp = getMapXMax_Row(j, leftmap, 2);
        if (temp == XM) {
            down_point.x = getMapXMax_Row(j - 1, leftmap, 2);
            down_point.y = j - 1;

            if (down_point.x == XM) {
                qout << "down_point";
                return 0;
            }
            break;
        }
    }

    if (down_point.x == 0 || down_point.y == 0) {
        qout << "down_point .x .y";
        return 0;
    }

    /* 成功确定低点后 找寻"非丢线" 处 */
    for (j = down_point.y + 8; j < YY; ++j) {
        temp = getMapX_Leftline(j, down_point.x, leftmap, 2);
        if (temp < down_point.x)
            temp = getMapXMax_Row(j, leftmap, 2);
        if (temp != XM) {
            up_point.x = temp;
            up_point.y = j;
            break;
        }
    }

    if (up_point.x == 0 || up_point.y == 0) {
        //        qout<<"up_point .x .y";
        return 0;
    }

    temp = XM;
    temp =
        getMapX_Rightline(up_point.y - 1, (up_point.x + XX) / 2, rightmap, 2);
    if (temp != XM) {
        qout << "Rightline";
        return 0;
    }

    qout << up_point.y << up_point.x;
    qout << down_point.y << down_point.x;

    /* 排除十字误判(三岔) */
    if (down_point.x - up_point.x > 10 || up_point.x - down_point.x > 20) {
        qout << "fork";
        return 0;
    }

    /* 基本满足后补线 */
    draw_straight_line(up_point.x, up_point.y, down_point.x, down_point.y,
                       leftmap);

    return 1;
}

uint8_t ImageHandler::draw_crossroad_rightline() {
    uint8_t temp = 0, temp_2 = 0;
    uint8_t j = 0;
    Point down_point = {0, 46}, up_point = {0, 0};

    temp = getMapXMin_Row(0, rightmap, 2);
    if (temp == XM) {
        temp = getMapYMin_Col(XX, rightmap, 2);
        if (temp == YM)
            return 0;
        else
            temp_2 = temp;
    } else
        temp_2 = 0;

    for (j = temp_2; j < YM; ++j) {
        temp = getMapXMin_Row(j, rightmap, 2);
        if (temp == XM) {
            down_point.x = getMapXMin_Row(j - 1, rightmap, 2);
            down_point.y = j - 1;

            if (down_point.x == XM)
                return 0;
            break;
        }
    }

    if (down_point.x == 0 || down_point.y == 0)
        return 0;

    for (j = down_point.y + 8; j < YY; ++j) {
        temp = getMapX_Rightline(j, down_point.x, rightmap, 2);
        if (temp > down_point.x)
            temp = getMapXMin_Row(j, rightmap, 2);
        if (temp != XM) {
            up_point.x = temp;
            up_point.y = j;
            break;
        }
    }

    if (up_point.x == 0 || up_point.y == 0)
        return 0;

    temp = XM;
    temp = getMapX_Leftline(up_point.y - 1, up_point.x / 2, leftmap, 2);
    if (temp != XM)
        return 0;

    qout << up_point.y << up_point.x;
    qout << down_point.y << down_point.x;

    if (up_point.x - down_point.x > 10 || down_point.x - up_point.x > 25 ||
        up_point.y - down_point.y > 30)
        return 0;

    draw_straight_line(up_point.x, up_point.y, down_point.x, down_point.y,
                       rightmap);
    qout << "good luck";
    return 1;
}

/* 应对斜入十字 难以补线 */
uint8_t ImageHandler::draw_corssroad_45_degree_rightline() {
    // qout<<crossroad_45;
    uint8_t temp = 0, temp_2 = 0;
    uint8_t j = 0;
    Point down_point = {0, 46}, up_point = {0, 0};

    /* 获取起始行 */
    temp = getMapXMin_Row(0, rightmap, 2);
    if (temp == XM) {
        temp = getMapYMin_Col(XX, rightmap, 2);
        if (temp == YM)
            return 0;
        else
            temp_2 = temp;
    } else
        temp_2 = 0;

    /* 确定低点坐标 */
    if (temp_2 == 0) {
        down_point.x = getMapXMin_Row(0, rightmap, 2);
        down_point.y = 0;
    } else {
        down_point.x = 46;
        down_point.y = temp_2;
    }

    /* 偷懒 直接利用特征点 */
    if (LT_Y(0) - LT_X(0)) {
        up_point.x = LT_X(0);
        up_point.y = LT_Y(0);
    }

    //    qout<<"down_point.x"<<down_point.x;
    //    qout<<"down_point.y"<<down_point.y;
    //    qout<<"up_point.x"<<up_point.x;
    //    qout<<"up_point.y"<<up_point.y;

    if (draw_crossroad_45_degree_rightup(up_point.x, up_point.y)) {
        /* 严格判定 具有一定持续性 */
        // crossroad_45=3;

        /* 补线前先删线 */
        for (int8_t i = up_point.y; i >= 0; i--) {
            for (j = up_point.x; j < XM; j++) {
                if (rightmap[j][i] >= 2)
                    rightmap[j][i] = 0;
            }
        }

        draw_straight_line(0, down_point.x, down_point.x, down_point.y,
                           rightmap);
        qout << "good luck";
        return 1;
    }

    return 0;
}

uint8_t ImageHandler::draw_crossroad_45_degree_rightup(uint8_t start_x,
                                                       uint8_t start_y) {
    if (start_x > start_y)
        return 0;

    uint8_t temp, temp1, i;
    Point down_point = {0, 0};  // 左侧丢线最低点

    int8_t b = start_y - start_x + 1;  // 结局起始处

    temp = 59;
    for (i = 0; i < start_x / 2; i++) {
        /* 获取左侧丢线最低点 */
        temp1 = getMapY_Start_Line(i, start_y, basemap, 2);
        if (temp1 == YM) {
            //            qout<<"fisrt_start";
            return 0;
        }
        if (temp1 < temp) {
            /* 记录点坐标位置 */
            down_point.x = i;
            down_point.y = temp1;
        }
    }

    if (down_point.x > down_point.y || down_point.y - down_point.x < b) {
        //        qout<<"temp_error";
        return 0;
    }

    uint8_t count = 0;
    temp1 = down_point.y - down_point.x;
    for (i = b; b < temp1 - 1; b += 2) {
        temp = get_45degree_RoadWidth(i, basemap);
        if (temp > 40 && temp != XM)
            count++;
        if (count > (temp1 - b) / 2) {
            //            qout<<"corssroad_45!!! good_luck";
            return 1;
        }
    }

    return 0;
}
/*** 十字后期 linemap只有上部 即下部缺失 向下延长补线 ***/
uint8_t ImageHandler::draw_crossroad_leftline_button() {
    uint8_t j, temp = XM;
    uint8_t distance = 15;

    Point down_point = {0, 0};
    Point up_point = {0, 0};

    /* 第一行 */
    //    temp=getMapYMin_Col(0,leftmap,2);
    //    if(temp!=YM)
    //    {
    //        qout<<"y=0 exit";
    //        return;
    //    }

    temp = getMapXMax_Row(0, leftmap, 2);
    if (temp != XM) {
        //        qout<<"x=0 exit";
        return 0;
    }

    /* 找到"非丢线"处 */
    for (j = 0; j < YM; ++j) {
        temp = getMapX_Leftline(j, 0, leftmap, 2);
        if (temp != XM) {
            down_point.x = temp;
            down_point.y = j;
            break;
        }
    }

    /* 往上distance 先确定是否存在 再通过遍历确定 中间部分无丢线 */
    temp = getMapX_Leftline(down_point.y + distance, down_point.x, leftmap, 2);
    if (temp == XM)
        return 0;

    up_point.x = temp;
    up_point.y = down_point.y + distance;

    for (j = down_point.y + 1; j < down_point.y + distance; ++j) {
        temp = getMapX_Leftline(j, down_point.x, leftmap, 2);
        if (temp == XM)
            return 0;
    }

    //    qout<<down_point.y<<down_point.x;
    //    qout<<up_point.y<<up_point.x;

    /* 获得两点之后延长补线 先确定点再补线 因畸需调整斜率 */
    Point point = extend_button_leftline_straight(up_point, down_point, 1.6);
    //    qout<<point.y<<point.x;

    draw_straight_line(down_point.x, down_point.y, point.x, point.y, leftmap);

    return 1;
}

uint8_t ImageHandler::draw_crossroad_rightline_button() {
    uint8_t j, temp = XM;
    uint8_t distance = 15;

    Point down_point = {0, 0};
    Point up_point = {0, 0};

    /* 第一行  */
    temp = getMapXMin_Row(0, rightmap, 2);
    if (temp != XM) {
        //        qout<<"x=0 exit";
        return 0;
    }

    /* 找到"非丢线"处 */
    for (j = 0; j < YM; ++j) {
        temp = getMapX_Rightline(j, XX, rightmap, 2);
        if (temp != XM) {
            down_point.x = temp;
            down_point.y = j;
            break;
        }
    }

    /* 往上distance 先确定是否存在 再通过遍历确定 中间部分无丢线 */
    temp =
        getMapX_Rightline(down_point.y + distance, down_point.x, rightmap, 2);
    if (temp == XM)
        return 0;

    up_point.x = temp;
    up_point.y = down_point.y + distance;

    for (j = down_point.y + 1; j < down_point.y + distance; ++j) {
        temp = getMapX_Rightline(j, down_point.x, rightmap, 2);
        if (temp == XM)
            return 0;
    }

    //    qout<<down_point.y<<down_point.x;
    //    qout<<up_point.y<<up_point.x;

    /* 获得两点之后延长补线 先确定点再补线 因畸需调整斜率 */
    Point point = extend_button_rightline_straight(up_point, down_point, 1.6);
    //    qout<<point.y<<point.x;

    draw_straight_line(down_point.x, down_point.y, point.x, point.y, rightmap);
    return 1;
}

void ImageHandler::draw_crossroad_leftlint_up() {
    uint8_t temp = YM, temp_2 = XM, temp_3 = YM;
    uint8_t j = 0;
    Point down_point = {0, 0};  // 低点

    temp = getMapYMin_Col(0, leftmap, 2);
    temp_2 = getMapXMax_Row(0, leftmap, 2);
    temp_3 = getMapYMin_Col(XX, leftmap, 2);

    if ((temp == YM && temp_2 == XM) || (temp_2 == XM && temp_3 == YM)) {
        return;
    } else if ((temp != YM && temp_2 != XM) || (temp_2 != XM && temp_3 != YM)) {
        return;
    }
    /* 确定搜索行起点 */
    temp = getMapXMax_Row(0, leftmap, 2);
    if (temp == XM) {
        temp = getMapYMin_Col(0, leftmap, 2);
        if (temp == YM)
            return;
        else
            temp_2 = temp;
    } else
        temp_2 = 0;

    /* 开始遍历 找寻"丢线"处 */
    for (j = temp_2; j < 40; ++j) {
        temp = getMapXMax_Row(j, leftmap, 2);
        if (temp == XM) {
            down_point.x = getMapXMax_Row(j - 1, leftmap, 2);
            down_point.y = j - 1;

            if (down_point.x == XM)
                return;
            break;
        }
    }

    if (down_point.x == 0 || down_point.y == 0)
        return;

    uint8_t count = 0;
    for (j = down_point.y + 3; j < 50; ++j) {
        temp = getMapX_Leftline(j, down_point.x, leftmap, 2);
        if (temp != XM)
            count++;
    }

    if (count > (50 - down_point.y + 3) / 3)
        return;

    //    qout<<down_point.y<<down_point.x;
}

Point ImageHandler::extend_button_leftline_straight(Point up_point,
                                                    Point down_point,
                                                    float param) {
    Point point = {0, 0};
    float k = param * (up_point.y - down_point.y) / (up_point.x - down_point.x);

    float y_axis = down_point.y - k * down_point.x;

    if (y_axis >= 0) {
        point.y = (uint8_t)y_axis;
        point.x = 0;

        return point;
    }

    float x_axis = down_point.x - down_point.y / k;

    if (x_axis >= 0) {
        point.x = (uint8_t)x_axis;
        point.y = 0;

        return point;
    }
}

Point ImageHandler::extend_button_rightline_straight(Point up_point,
                                                     Point down_point,
                                                     float param) {
    Point point = {0, 0};
    float k = param * (up_point.y - down_point.y) / (up_point.x - down_point.x);

    float y_axis = down_point.y - k * (down_point.x - 46);

    if (y_axis >= 0) {
        point.y = (uint8_t)y_axis;
        point.x = 46;

        return point;
    }

    float x_axis = down_point.x - down_point.y / k;
    if (x_axis <= 46) {
        point.x = (uint8_t)x_axis;
        point.y = 0;

        return point;
    }
}

uint8_t ImageHandler::tag_detect() {
    uint8_t i, j;
    uint8_t b_num = 0;

    for (j = 20; j < 50; ++j) {
        if (X_BWB_Detect(0, XM, j, insidemap, 2))
            b_num++;
    }

    if (b_num > 2)
        return b_num;
    else
        return 0;
}

uint8_t ImageHandler::go_tag() {
    uint8_t static tag_num = 0;

    if (tag_num != 2 && tag_detect())
        tag_num++;

    if (tag_num >= 2)
        return 1;

    return 0;
}

float ImageHandler::get_nowYaw() {
    old_yaw = new_yaw;
    new_yaw = angle.Yaw;

    if (new_yaw > old_yaw)
        return new_yaw - old_yaw;
    else
        return old_yaw - new_yaw;
}
