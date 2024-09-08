#ifndef MAPHANDLER
#define MAPHANDLER
#include "imagehandler.h"

class MapHandler {
   public:
    cv::Mat imgMat;
    ImgSize imgSize;

    void Mat2Map(cv::Mat);
    cv::Mat Map2Mat(uint8_t map[YM][XM], QPoint);
    void showAllMaps(QPoint mypoint = QPoint(-1, -1));
    void showImgGray(uint8_t map[ROW][COL], QLabel*, QPoint);
    void showMap(uint8_t map[YM][XM], QLabel*, QPoint);
    void showMat(cv::Mat bgrMat, QLabel*);
    void showRealMap(QLabel* label, QPoint);
};

extern MapHandler maphandler;
#endif