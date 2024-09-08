#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include "headfile.h"
#include "stack.h"

#define LB(i) II.left_bottom[i]
#define RB(i) II.right_bottom[i]
#define LT(i) II.left_top[i]
#define RT(i) II.right_top[i]

#define BL(i) II.bottom_left[i]
#define BR(i) II.bottom_right[i]
#define TL(i) II.top_left[i]
#define TR(i) II.top_right[i]

#define LB_X(i) II.left_bottom[i].x
#define RB_X(i) II.right_bottom[i].x
#define LT_X(i) II.left_top[i].x
#define RT_X(i) II.right_top[i].x

#define BL_X(i) II.bottom_left[i].x
#define BR_X(i) \
    II.bottom_right[i].x  // BR为basemap 边缘和前景在内的 最低点坐标
                          // //正常矩形赛道的左侧起始点
#define TL_X(i) II.top_left[i].x
#define TR_X(i) II.top_right[i].x  // 左边界的最远延伸点

#define LB_Y(i) II.left_bottom[i].y
#define RB_Y(i) II.right_bottom[i].y
#define LT_Y(i) II.left_top[i].y
#define RT_Y(i) II.right_top[i].y

#define BL_Y(i) II.bottom_left[i].y
#define BR_Y(i) II.bottom_right[i].y
#define TL_Y(i) II.top_left[i].y
#define TR_Y(i) II.top_right[i].y

#define TOP_LM(i) TR_Y(i)
#define BOTTOM_LM(i) BR_Y(i)
#define TOP_RM(i) TL_Y(i)
#define BOTTOM_RM(i) BL_Y(i)
#define RIGHT(i) RT_X(i)
#define LEFT(i) LT_X(i)

#define XM 47     // 图像宽度
#define YM 60     // 图像高度
#define X_MID 23  // 图像中线
#define XX 46     // XM-1
#define YY 59     // YM-1
#define COL 188
#define ROW 120   // 180
#define _COL 187  // COL-1
#define _ROW 119  // ROW-1

#define IMG_SIZE (ROW * COL)
#define MAP_SIZE (YM * XM)

#define STEP 36  // 53    //55
#define STOP_LINE 55

#define _YES 1
#define _NO 0
#define goLeft 1
#define goRight 2
#define goStraight 3

#define NO_LINE 0
#define LEFT_LINE 1
#define RIGHT_LINE 2
#define _LEFT_LINE 3
#define _RIGHT_LINE 4

#define AL1 1
#define AL2 2
#define AL3 3
#define AL4 4
#define AL5 5

#define AR1 7
#define AR2 8
#define AR3 9
#define AR4 10
#define AR5 11

#define PL1 1
#define PL2 2

#define PR1 4
#define PR2 5

#define Fre 1

//  Roberts算子 原点在左下角 对角线相减
#define H(x, y)                                  \
    (imgGray[y][x] > imgGray[y + 1][x + 1]       \
         ? imgGray[y][x] - imgGray[y + 1][x + 1] \
         : imgGray[y + 1][x + 1] - imgGray[y][x])
#define V(x, y)                                  \
    (imgGray[y][x + 1] > imgGray[y + 1][x]       \
         ? imgGray[y][x + 1] - imgGray[y + 1][x] \
         : imgGray[y + 1][x] - imgGray[y][x + 1])

// 检验三行 通过计数和不同比重 来增强前景、背景判断的准确性

#define robertJudge(x, y)                                                \
    {                                                                    \
        num = 0;                                                         \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 4; ++i)                                  \
            if (imgGray[y][x + i] < nowThreshold)                        \
                ++sum;                                                   \
            else if (H(x + i, y) + V(x + i, y) >= edgeThreshold)         \
                sum += 2;                                                \
        /*** 直接判定为边界 +2 ***/                               \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 4; ++i)                                  \
            if (imgGray[y + 1][x + i] < nowThreshold)                    \
                ++sum;                                                   \
            else if (H(x + i, y + 1) + V(x + i, y + 1) >= edgeThreshold) \
                sum += 2;                                                \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        if (num == 0) {                                                  \
            allmap[j][i] = 0;                                            \
            continue;                                                    \
        } else if (num >= 2) {                                           \
            allmap[j][i] = 1;                                            \
            continue;                                                    \
        }                                                                \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 4; ++i)                                  \
            if (imgGray[y + 2][x + i] < nowThreshold)                    \
                ++sum;                                                   \
            else if (H(x + i, y + 2) + V(x + i, y + 2) >= edgeThreshold) \
                sum += 2;                                                \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        if (num >= 2) {                                                  \
            allmap[j][i] = 1;                                            \
            continue;                                                    \
        } else {                                                         \
            allmap[j][i] = 0;                                            \
            continue;                                                    \
        }                                                                \
    }

// 第一行 专注水平方向的连续白点 体现在后半段的连续判定
#define robertJudge_Y(x, y)                                              \
    {                                                                    \
        num = 0;                                                         \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 4; ++i)                                  \
            if (imgGray[y][x + i] < nowThreshold)                        \
                ++sum;                                                   \
            else if (H(x + i, y) + V(x + i, y) >= edgeThreshold)         \
                sum += 2;                                                \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 4; ++i)                                  \
            if (imgGray[y + 1][x + i] < nowThreshold)                    \
                ++sum;                                                   \
            else if (H(x + i, y + 1) + V(x + i, y + 1) >= edgeThreshold) \
                sum += 2;                                                \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        if (num == 0) {                                                  \
            allmap[j][i] = 0;                                            \
            continue;                                                    \
        } else if (num >= 2) {                                           \
            allmap[j][i] = 1;                                            \
            continue;                                                    \
        }                                                                \
                                                                         \
        if (imgGray[y + 2][x] < nowThreshold &&                          \
            imgGray[y + 2][x + 1] < nowThreshold &&                      \
            imgGray[y + 2][x + 2] < nowThreshold &&                      \
            imgGray[y + 2][x + 3] < nowThreshold) {                      \
            ++num;                                                       \
        }                                                                \
        if (num >= 2) {                                                  \
            allmap[j][i] = 1;                                            \
            continue;                                                    \
        } else {                                                         \
            allmap[j][i] = 0;                                            \
            continue;                                                    \
        }                                                                \
    }

// 对应最右侧的 “放大” 访问3个像素 Roberts原点在左下
#define robertJudge_X(x, y)                                              \
    {                                                                    \
        num = 0;                                                         \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 3; ++i)                                  \
            if (imgGray[y][x + i] < nowThreshold)                        \
                ++sum;                                                   \
            else if (H(x + i, y) + V(x + i, y) >= edgeThreshold)         \
                sum += 2;                                                \
        if (imgGray[y][x + 3] < nowThreshold)                            \
            ++sum;                                                       \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 3; ++i)                                  \
            if (imgGray[y + 1][x + i] < nowThreshold)                    \
                ++sum;                                                   \
            else if (H(x + i, y + 1) + V(x + i, y + 1) >= edgeThreshold) \
                sum += 2;                                                \
        if (imgGray[y + 1][x + 3] < nowThreshold)                        \
            ++sum;                                                       \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        if (num == 0) {                                                  \
            allmap[j][i] = 0;                                            \
            continue;                                                    \
        } else if (num >= 2) {                                           \
            allmap[j][i] = 1;                                            \
            continue;                                                    \
        }                                                                \
                                                                         \
        for (uint8_t i = 0; i < 3; ++i)                                  \
            if (imgGray[y + 2][x + i] < nowThreshold)                    \
                ++sum;                                                   \
            else if (H(x + i, y + 2) + V(x + i, y + 2) >= edgeThreshold) \
                sum += 2;                                                \
        if (imgGray[y + 2][x + 3] < nowThreshold)                        \
            ++sum;                                                       \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        if (num >= 2) {                                                  \
            allmap[j][i] = 1;                                            \
            continue;                                                    \
        } else {                                                         \
            allmap[j][i] = 0;                                            \
            continue;                                                    \
        }                                                                \
    }

// 第一行 专注水平方向的连续白点 体现在后半段的连续判定
// 对应最右侧的“放大” 访问三个点
#define robertJudge_XY(x, y)                                             \
    {                                                                    \
        num = 0;                                                         \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 3; ++i)                                  \
            if (imgGray[y][x + i] < nowThreshold)                        \
                ++sum;                                                   \
            else if (H(x + i, y) + V(x + i, y) >= edgeThreshold)         \
                sum += 2;                                                \
        if (imgGray[y][x + 3] < nowThreshold)                            \
            ++sum;                                                       \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        sum = 0;                                                         \
                                                                         \
        for (uint8_t i = 0; i < 3; ++i)                                  \
            if (imgGray[y + 1][x + i] < nowThreshold)                    \
                ++sum;                                                   \
            else if (H(x + i, y + 1) + V(x + i, y + 1) >= edgeThreshold) \
                sum += 2;                                                \
        if (imgGray[y + 1][x + 3] < nowThreshold)                        \
            ++sum;                                                       \
        if (sum >= 4)                                                    \
            ++num;                                                       \
        if (num == 0) {                                                  \
            allmap[j][i] = 0;                                            \
            continue;                                                    \
        } else if (num >= 2) {                                           \
            allmap[j][i] = 1;                                            \
            continue;                                                    \
        }                                                                \
                                                                         \
        if (imgGray[y + 2][x] < nowThreshold &&                          \
            imgGray[y + 2][x + 1] < nowThreshold &&                      \
            imgGray[y + 2][x + 2] < nowThreshold &&                      \
            imgGray[y + 2][x + 3] < nowThreshold) {                      \
            ++num;                                                       \
        }                                                                \
        if (num >= 2) {                                                  \
            allmap[j][i] = 1;                                            \
            continue;                                                    \
        } else {                                                         \
            allmap[j][i] = 0;                                            \
            continue;                                                    \
        }                                                                \
    }

// 长条区域（同一行四像素） 阈值判断 在摄像头的采集图像中
// 判断三行四列是因为在 OSTU_ROBERT()中高度和宽度上的压缩
#define ostuJudge(x, y)                             \
    {                                               \
        num = 0;                                    \
        if (imgGray[y][x] < nowThreshold &&         \
            imgGray[y][x + 1] < nowThreshold &&     \
            imgGray[y][x + 2] < nowThreshold &&     \
            imgGray[y][x + 3] < nowThreshold) {     \
            ++num;                                  \
        }                                           \
        if (imgGray[y + 1][x] < nowThreshold &&     \
            imgGray[y + 1][x + 1] < nowThreshold && \
            imgGray[y + 1][x + 2] < nowThreshold && \
            imgGray[y + 1][x + 3] < nowThreshold) { \
            ++num;                                  \
        }                                           \
        if (num == 0) {                             \
            allmap[j][i] = 0;                       \
            continue;                               \
        } else if (num >= 2) {                      \
            allmap[j][i] = 1;                       \
            continue;                               \
        }                                           \
                                                    \
        if (imgGray[y + 2][x] < nowThreshold &&     \
            imgGray[y + 2][x + 1] < nowThreshold && \
            imgGray[y + 2][x + 2] < nowThreshold && \
            imgGray[y + 2][x + 3] < nowThreshold) { \
            ++num;                                  \
        }                                           \
        if (num >= 2) {                             \
            allmap[j][i] = 1;                       \
            continue;                               \
        } else {                                    \
            allmap[j][i] = 0;                       \
            continue;                               \
        }                                           \
    }

// struct
typedef enum ImgSize {
    _47_60,
    _188_180,
} ImgSize;

typedef struct ImgInfo {
    uint8_t num_lm;  // 左图黑色区域个数
    uint8_t num_rm;  // 右图黑色区域个数

    uint8_t _num_lm;  // 左图区域内个数
    uint8_t _num_rm;  // 右图区域内个数

    uint8_t start_lm[5];  // leftmap  存储 黑色区域 搜索起点纵坐标 0
    uint8_t start_rm[5];  // rightmap 存储 黑色区域 搜索起点纵坐标 XX

    Point left_bottom[5];
    Point right_bottom[5];
    Point left_top[5];
    Point right_top[5];

    Point bottom_left[5];
    Point bottom_right[5];
    Point top_left[5];
    Point top_right[5];

    Point startMap_leftTop;
    Point startMap_leftBottom;
    Point startMap_rightTop;
    Point startMap_rightBottom;

    uint8_t left_x;   // 左侧十字检测到的角点
    uint8_t left_y;   // 十字检测到的角点
    uint8_t right_x;  // 右侧十字检测到的角点
    uint8_t right_y;  // 十字检测到的角点

    uint8_t _top;  // 有时搜图不止一次，进行区分
    uint8_t top;   // 赛道部分最高点
    uint8_t bottom;
    uint8_t speedTop;
    uint8_t annulusTop;

    uint8_t stop_line;  // 算偏差 截止线
    uint8_t limit_line;
    uint8_t step;         // 搜图区域限高
    uint8_t line_forbid;  // 禁线 禁止一些不该出现的线出现
    uint16_t repeatNum;   // basemap 和 left/rightmap 同为黑点
    uint16_t dnum_top;    // deletemap 绿点数量

    uint16_t bnum_all;  // basemap 白点数量
    uint16_t lnum_all;  // leftmap 黑点数量
    uint16_t rnum_all;  // rightmap 黑点数量
    uint16_t dnum_all;  // deletemap 黑点数量
    uint16_t lnum[5];   // leftmap 黑色区域数组
    uint16_t rnum[5];   // rightmap 黑色区域数组

    uint16_t lmin;  // 未搜到的区域最小Y坐标
    uint16_t rmin;
    uint16_t leftnum;   // 左边线像素数
    uint16_t rightnum;  // 右边线像素数
    uint16_t midnum;    // 中线像素数
} ImgInfo;

// 标志位
typedef struct ImgFlag {
    uint8_t sramp;    // 小坡
    uint8_t ramp;     // 坡道
    uint8_t annulus;  // 环岛
    uint8_t annulusDelay;
    uint8_t startline;  // 起跑
    uint8_t parking;    // 出库
    uint8_t in_park;    // 停车入库
    uint8_t fork;       // 三岔路口
    uint8_t wait_fork;
    uint8_t crossroad;  // 十字路口
    uint8_t tag;
} ImgFlag;

// 标志位锁定
typedef struct ImgFlagLock {
    uint8_t imgFlag;  // 1 sramp, 2 ramp, 3 annulus, 4 annulusDelay, 5 parking
    uint16_t lockTime;
    uint8_t isLocked;
} ImgFlagLock;

typedef struct {
    uint16_t cnt;
    float sumDev;
    float sumDK;
    uint8_t flag;
} AnnulusDEV;

typedef struct {
    uint16_t numCnt;
    Point right_bottom;
    Point right_top;
    Point bottom_right;  // BR为basemap 边缘和前景在内的 最低点坐标
                         // //正常矩形赛道的左侧起始点
    Point top_right;  // 左边界的最远延伸点 详情见 searchLeftmapPoint
} RegionInfoLeft;     // rightline

typedef struct {
    uint16_t numCnt;
    Point left_bottom;
    Point left_top;
    Point bottom_left;
    Point top_left;
} RegionInfoRight;  // leftline

typedef struct PointNode {
    uint8_t num;
    Point point[XX];
} PtStack;

typedef enum RampMethod {
    translationL,
    translationR,
    midLine,
} RampMethod;

typedef struct RampInfo {
    RampMethod rampMethod;
    uint8_t startY;
    uint8_t endY;
} RampInfo;

class ImageHandler {
   public:
    EulerAngleTypedef SystemAttitude = {0, 0, 0};
    EulerAngleTypedef SystemAttitudeRate = {0, 0, 0};  // 当前欧拉角速度
    EulerAngleTypedef angle = {0, 0, 0};
    float relativeYaw;
    uint16_t TFMINI_PLUS_DISTANCE;
    uint16_t TFMINI_PLUS_STRENGTH;
    Test testBox;
    ImgFlag IIF;

    ImgInfo imgInfoInit;
    ImgInfo imgInfo;
    ImgFlag imgFlag = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ImgFlag LAST_IF = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ImgFlagLock imgFlagLock;

    AnnulusDEV AD;
    AnnulusDEV AD_INIT = {0};

    uint8_t leftlineZero = 0;
    uint8_t rightlineZero = 0;
    uint8_t deleteforleftflag = 0;
    uint8_t deleteforrightflag = 0;

    // 左标准线（固定 褐色）
    float leftline[YM] = {0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  3,
                          3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,
                          7,  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10,
                          11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 15,
                          15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 19};
    float rightline[YM];

    uint8_t car_stop = 45;
    uint8_t car_leftline[45] = {0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,
                                5,  5,  6,  6,  7,  7,  8,  8,  9,  9,  10, 10,
                                11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16,
                                17, 17, 18, 18, 19, 19, 20, 20, 20};
    uint8_t car_rightline[45];

    float k1[YM];  // 逆透视中 每一行对应的放大倍数
    // k1为x方向上的坐标变换，将图像视角的倾斜坐标系映射到俯视视角的坐标系上
    // 由于近大远小，通过k1将摄像头视图的赛道宽度转化为其真实的赛道宽度
    float k2[YM] = {16,   16.5,  17,   17.5,  18,    18.5, 19.5, 20,   21,
                    22,   22.5,  23,   23,    24,    25,   26,   27,   27.5,
                    28.5, 29.5,  30.5, 31.5,  33,    34,   35,   36,   37,
                    38.5, 39,    41.5, 43.5,  45,    46,   47.5, 49.5, 52,
                    53.5, 55.5,  57,   59,    61.5,  63,   65.5, 68.5, 72,
                    77,   80,    84,   88,    92,    98.5, 104,  117,  125,
                    134,  145.5, 161,  165.5, 187.5, 200};
    // k2为y方向上的坐标变换，将图像视角的倾斜坐标系映射到俯视视角的坐标系上
    // 人工测量图像上每一行dy与实际纵向距离的比例，并进行曲线性拟合

    float outYaw = 0;
    uint16_t nowAnnulusNum = 0;  // 环岛
    uint16_t nowNoWayNum = 0;
    uint16_t nowObstacleNum = 0;  // 障碍物
    float yaw_leftline[20] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2,
                              2, 3, 3, 3, 4, 4, 4, 4, 5, 5};
    float yaw_rightline[20] = {46, 46, 46, 46, 45, 45, 45, 45, 44, 44,
                               44, 43, 43, 43, 42, 42, 42, 42, 41, 41};

    uint8_t previousThreshold = 0;
    uint8_t nowThreshold = 0;  // 0是黑，255白
    uint16_t edgeThreshold = 30;
    uint16_t minThreshold = 0;
    uint16_t maxThreshold = 255;
    uint16_t startRow = 0;
    uint16_t endRow = YM;

    uint8_t simplify_ramp_flag = 0;

    int8_t leftAnnulus_times = 2;
    uint8_t leftAnnulus_flag = 0;

    int8_t rightAnnulus_times = 2;
    uint8_t rightAnnulus_flag = 0;

    uint8_t left_leave = 0;

    uint8_t speedlineLeft[YM];   // 23
    uint8_t speedlineRight[YM];  // 23

    // map
    uint8_t linemap[YM][XM];
    uint8_t allmap[YM][XM];
    uint16_t histogram[256];  // 灰度直方图
    uint8_t imgGray[ROW][COL];
    uint8_t (*imgBinary)[COL] = imgGray;
    uint8_t basemap[YM][XM];  // 0白前景 1黑背景 2红线 254绿线
    uint8_t leftmap[YM][XM];  // 0白前景 1黑背景 2红线 后加：3粉线 4蓝线 254绿线
                              // 255灰线
    uint8_t rightmap[YM][XM];  // 0白前景 1黑背景 2红线 后加：3粉线 4蓝线
                               // 254绿线 255灰线
    uint8_t insidemap
        [YM]
        [XM];  // 0白 1黑 去噪点 （黑白交替起跑线）
               // insidemap用于记录basemap不为白且left.rightmap不为黑的部分，即赛道内部黑色的地方。通常判断赛道里黑色起跑线
    uint8_t deletemap[YM][XM];  // 0白 1黑 2红线  后加：254绿点 253橙线
                                // 较远时删图 入十字时删掉上部分
                                // 入环岛时判定闭合背景 出环时删掉特有赛道

    uint8_t midline[YM / 2];

    uint8_t LeftAnnulus[5] = {0};  // 圆环多帧判定
    uint8_t Annulus_width[5] = {0};
    uint8_t Annulus_height[5] = {0};
    uint8_t Annulus_dis[5] = {0};

    PtStack noDown;
    uint16_t numCnt;
    uint8_t top;
    uint8_t bottom;
    uint8_t _left;
    uint8_t _right;
    uint16_t parkingDir = goStraight;
    uint8_t outpark_dir = goRight;
    uint8_t park_forbid = 0;

    uint8_t new_left_detect = 0, old_left_detect = 0;
    uint8_t new_right_detect = 0, old_right_detect = 0;
    uint8_t left_detect_flag = 0, right_detect_flag = 0;

    uint8_t fork_flag = 0;
    uint8_t fork_delay = 0;
    uint8_t fork_roll = 0;
    uint8_t fork_flag_num = 0;
    uint8_t fork_dir = 0;

    uint8_t old_fork = 0;
    uint8_t new_fork = 0;

    int8_t fork_detect_num = 0;

    int8_t crossroad_45 = 0;

    int8_t right_delay[5] = {5};
    int8_t left_delay[5] = {5};

    uint16_t horizontal_white = 0;
    uint16_t horizontal_blcak = 0;

    uint8_t fork_num = 0;

    float new_yaw = 0;
    float old_yaw = 0;

    bool standard();
    void map_init();
    void getGrayscaleHistogram();
    uint8_t getOSTUThreshold();
    void OSTU_ROBERT();

    /*** 图像获取 ***/
    void searchimg(uint8_t x, uint8_t y);
    void searchleftmap(uint8_t x, uint8_t y);
    void searchrightmap(uint8_t x, uint8_t y);
    void searchdeletemap(uint8_t x, uint8_t y);
    void searchdeletemap2(uint8_t x, uint8_t y);
    void searchCountmap(uint8_t x, uint8_t y, uint8_t src[][XM]);
    void searchmap(uint8_t x, uint8_t y, uint8_t src[][XM]);
    uint16_t cntMap(uint8_t x, uint8_t y);
    void searchStartMap(uint8_t x, uint8_t y, uint8_t src[][XM]);
    void getRegionInfoLeft(uint8_t x,
                           uint8_t y,
                           uint8_t src[][XM],
                           uint8_t dst[][XM],
                           RegionInfoLeft* r);
    void getRegionInfoRight(uint8_t x,
                            uint8_t y,
                            uint8_t src[][XM],
                            uint8_t dst[][XM],
                            RegionInfoRight* r);

    uint8_t getDown(uint8_t dir);
    void getallmap();
    void getlinemap();
    uint8_t isSearch(uint8_t x, uint8_t y, uint8_t map[][XM]);
    uint8_t getMapYMin_Col(uint8_t x, uint8_t map[][XM], uint8_t value);
    uint8_t getMapYMax_Col(uint8_t x, uint8_t map[][XM], uint8_t value);
    uint8_t getMapYMin_Col2(uint8_t x, uint8_t y, uint8_t map[][XM]);
    uint8_t getMapXMax_Row(uint8_t y, uint8_t map[][XM], uint8_t value);
    uint8_t X_WBW_Detect(int8_t x1,
                         int8_t x2,
                         uint8_t y,
                         uint8_t map[][XM],
                         uint8_t flag);
    uint8_t X_BWB_Detect(int8_t x1,
                         int8_t x2,
                         uint8_t y,
                         uint8_t map[][XM],
                         uint8_t flag);
    uint8_t Y_WBW_Detect(uint8_t y1,
                         uint8_t y2,
                         uint8_t x,
                         uint8_t map[][XM]);  // 白黑白判定
    uint8_t Y_RBR_Detect(uint8_t y1, uint8_t y2, uint8_t x, uint8_t map[][XM]);
    uint8_t strJudge(uint8_t x1,
                     uint8_t y1,
                     uint8_t x2,
                     uint8_t y2,
                     uint8_t map[][XM],
                     uint8_t sy1,
                     uint8_t sy2,
                     int8_t limit,
                     uint8_t errorNum);
    uint8_t strJudge_X(uint8_t x1,
                       uint8_t y1,
                       uint8_t x2,
                       uint8_t y2,
                       uint8_t map[][XM],
                       uint8_t sx1,
                       uint8_t sx2,
                       int8_t limit,
                       uint8_t errorNum);

    uint8_t deleteforleft(uint8_t undelete_flag);
    uint8_t deleteforright(uint8_t undelete_flag);
    uint8_t leftAnnulusDetect();
    uint8_t rightAnnulusDetect();
    uint8_t isEnter(uint8_t dir);
    uint8_t leave(uint8_t dir);
    uint8_t AnnulusDeal(uint8_t ADir, uint8_t status);
    uint8_t ramp();
    void deleterampline();
    uint8_t parkingDetect();  // 车库检测，主要还是检测起跑线
    uint8_t leftParkingDetect();
    uint8_t rightParkingDetect();
    uint8_t parkingEnter(uint8_t PDir);
    uint8_t parkingDeal(uint8_t PStatus, uint8_t PDir);
    uint8_t noWayDetect();

    uint8_t crossroadforleft();
    uint8_t crossroadforright();
    void deleteline();
    void Get_insideMap();
    void getSpeedTop();
    uint8_t goAnnulus();
    uint8_t getAnnulusTop();
    uint8_t goRamp();
    uint8_t goParking();
    uint8_t detectStartline();
    uint8_t detectStartline_roughly();
    // uint8_t get_Forkdown(uint8_t dir);
    uint8_t get_Forkdown(uint8_t start_y,
                         uint8_t mid,
                         uint8_t down_dis,
                         uint8_t test_dis,
                         uint8_t symmetry_dis);
    uint8_t getMap_YSecond_Col(uint8_t x, uint8_t y, uint8_t map[][XM]);
    uint8_t getMap_YThird_Col(uint8_t x, uint8_t y, uint8_t map[][XM]);
    void draw_Annulusline();
    uint8_t first_leftAnnulus_Detect();
    uint8_t first_rightAnnulus_Detect();

    Point fork_detect();
    void fork_deal(Point down_point, uint8_t dir);
    uint8_t go_fork();
    uint8_t go_tag();
    float get_nowYaw();

    // my function
    uint16_t myTen_count(uint8_t part,
                         uint8_t start_y,
                         uint8_t end_y,
                         uint8_t* map);
    void myTen_deviation(uint8_t* map);
    uint8_t getMapX_Samevalue_Sum(uint8_t y,
                                  uint8_t start_x,
                                  uint8_t end_x,
                                  uint8_t map[][XM],
                                  uint8_t value);
    uint8_t getMapY_Samevalue_Count(uint8_t x,
                                    uint8_t start_y,
                                    uint8_t end_y,
                                    uint8_t map[][XM],
                                    uint8_t value);
    uint8_t getMapX_Samevalue_Count(uint8_t y,
                                    uint8_t start_x,
                                    uint8_t end_x,
                                    uint8_t map[][XM],
                                    uint8_t value);
    uint8_t getMapX_Start_Row(uint8_t y,
                              uint8_t start_x,
                              uint8_t map[][XM],
                              uint8_t value);
    uint8_t getMap_XSecond_ROW(uint8_t y, uint8_t x, uint8_t map[][XM]);
    uint8_t straight_road(uint8_t x, uint8_t end_y, uint8_t map[][XM]);
    uint8_t getMapX_Start_Row_2(uint8_t y,
                                uint8_t start_x,
                                uint8_t map[][XM],
                                uint8_t value);
    uint8_t getMapY_Start_Line(uint8_t x,
                               uint8_t start_y,
                               uint8_t map[][XM],
                               uint8_t value);
    uint8_t get_RoadWidth_left(uint8_t y, uint8_t start_x, uint8_t map[][XM]);
    uint8_t get_RoadWidth_right(uint8_t y, uint8_t start_x, uint8_t map[][XM]);
    uint8_t draw_corssroad_45_degree_rightline();
    uint8_t get_45degree_RoadWidth(uint8_t b, uint8_t map[][XM]);
    uint8_t draw_crossroad_45_degree_rightup(uint8_t start_x, uint8_t start_y);
    uint8_t my_go_Ramp();
    uint8_t cnt_WBW_num(uint8_t x1, uint8_t x2, uint8_t y, uint8_t map[][XM]);
    uint8_t y_cnt_WBW_num(uint8_t y1, uint8_t y2, uint8_t x, uint8_t map[][XM]);
    uint8_t outpark_detect();
    uint8_t my_goout_park();
    uint8_t getMap_Yset_Col(uint8_t x, uint8_t y, uint8_t map[][XM]);
    uint8_t tag_detect();

    uint8_t getMap_YFirst_Col(uint8_t x, uint8_t y, uint8_t map[][XM]);
    void draw_straight(uint8_t x1,
                       uint8_t y1,
                       uint8_t x2,
                       uint8_t y2,
                       uint8_t map[][XM]);
    uint8_t getMap_YMax_first_Col(uint8_t x, uint8_t y, uint8_t map[][XM]);
    void draw_straight_line(uint8_t x1,
                            uint8_t y1,
                            uint8_t x2,
                            uint8_t y2,
                            uint8_t map[][XM]);
    void CornerPoint_Left(uint8_t start_x, uint8_t start_y, uint8_t map[][XM]);
    uint8_t draw_crossroad_leftline();
    uint8_t draw_crossroad_rightline();
    void drawline2(uint8_t x1,
                   uint8_t y1,
                   uint8_t x2,
                   uint8_t y2,
                   uint8_t map[][XM],
                   uint8_t n);
    void drawline3(uint8_t x1,
                   uint8_t y1,
                   uint8_t x2,
                   uint8_t y2,
                   uint8_t map[][XM]);
    uint8_t getMapX_Rightline(uint8_t y,
                              uint8_t start_x,
                              uint8_t map[][XM],
                              uint8_t value);
    uint8_t getMapX_Leftline(uint8_t y,
                             uint8_t start_x,
                             uint8_t map[][XM],
                             uint8_t value);
    uint8_t draw_crossroad_leftline_button();
    uint8_t draw_crossroad_rightline_button();
    Point extend_button_leftline_straight(Point up_point,
                                          Point down_point,
                                          float param);
    void draw_crossroad_leftlint_up();
    Point extend_button_rightline_straight(Point up_point,
                                           Point down_point,
                                           float param);

    void searchCoumtmap_Set_X(uint8_t x,
                              uint8_t y,
                              uint8_t start_x,
                              uint8_t end_x,
                              uint8_t src[][XM]);
    uint16_t cntMap_Set_X(uint8_t x, uint8_t y, uint8_t start_x, uint8_t end_x);
    uint8_t getMapXMin_Row(uint8_t y, uint8_t map[][XM], uint8_t value);
    float getLineK(Point array[], uint8_t num);
    void scanline(uint8_t start_x, uint8_t start_y, uint8_t map[][XM]);
};

extern ImageHandler imagehandler;
#endif