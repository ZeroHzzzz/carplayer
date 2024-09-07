#ifndef MAP
#define MAP
#include "imagehandler.h"

class MapHandler {
   public:
    cv::Mat imgMat;
    ImgSize imgSize;

    void Mat2Map(cv::Mat mat);
    cv::Mat Map2Mat(uint8_t map[YM][XM], QPoint point);
    void showAllMaps(QPoint point = QPoint(-1, -1));
    void showImgGray(uint8_t map[ROW][COL], QLabel* label, QPoint point);
    void showMap(uint8_t map[YM][XM], QLabel* label, QPoint point);
    void showMat(cv::Mat bgrMat, QLabel* label);
    void showRealMap(QLabel* label, QPoint);
};
#endif