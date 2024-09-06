#ifndef HEADFILES_H
#define HEADFILES_H

#define T_REX_QT_DEBUG 1
#define qout qDebug() << __FILE__ << __LINE__ << __FUNCTION__

#define MULTIPLE 5

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QRegExp>
#include <QStorageInfo>
#include <iostream>
#include <opencv2/opencv.hpp>

#define PI 3.1415926

#define BYTE0(x) (*((char*)(&x)))
#define BYTE1(x) (*((char*)(&x) + 1))
#define BYTE2(x) (*((char*)(&x) + 2))
#define BYTE3(x) (*((char*)(&x) + 3))

#define TRACK 0.178        // 轮距，米
#define SPD_CONVERT 57.8f  // 1m/s 5783个脉冲/s 57.83个脉冲/s

typedef struct Test {
    uint8_t test0 = 0;
    uint8_t test1 = 0;
    uint8_t test2 = 0;
    uint8_t test3 = 0;
    uint8_t test4 = 0;
    float test5 = 0;
    float test6 = 0;
    float test7 = 0;
    float test8 = 0;
    float test9 = 0;
} Test;

typedef struct {
    float Pitch;  // 俯仰角
    float Yaw;    // 偏航角
    float Roll;   // 翻滚角
} EulerAngleTypedef;

typedef enum LCDState {
    ADJUST_PARAM,
    DISPLAY_PARAM,
    DISPLAY_RUNNING_INFO,
    DO_NOT_DISPLAY,
    REVIEW,
} LCDState;

typedef enum RunState {
    CAR_STOP,
    CAR_READY,
    CAR_RUNNING,
    CAR_BRAKING,
} RunState;

/* 小车运行模式枚举类型 */
typedef enum RunMode {
    NORMAL_RUN,
    TIMING_RUN,
} RunMode;

typedef enum RunSpeedMode {
    VARIABLE_SPEED,
    CONSTANT_SPEED,
    NORMAL_SHIFT_SPEED,
    ADC_SPEED,
} RunSpeedMode;

typedef enum StopReason {
    HaventStarted = 0,
    TimeUp,
    RunOutLine,
    ParkingDetected,
    StallProtect,
} StopReason;

typedef struct ProgramRunTime {
    uint16_t whileTime;
    uint16_t _whileTime;
    uint16_t maxWhileTime;
    uint16_t aveWhileTime;
    uint16_t goTime;
    uint16_t maxGoTime;
    uint16_t aveGoTime;
} ProgramRunTime;

typedef struct RunningInfo {
    ProgramRunTime programRunTime;
    uint8_t annulusNum;
    uint8_t crossRoadNum;
    uint8_t rampNum;
    uint8_t srampNum;
} RunningInfo;

// base
typedef struct Point {
    uint8_t x;
    uint8_t y;
} Point;

typedef struct Points {
    uint8_t num;
    Point point[40];
} Points;

typedef struct stack {
    int32_t top;
    uint32_t MAX;
    Point* data;
} stack;

extern int16_t annulus_yaw;
extern int16_t annulus_yaw_limit;

#endif