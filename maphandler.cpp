#include "maphandler.h"
#include <opencv2/imgproc/types_c.h>
#include "imagehandler.h"
#include "mainwindow.h"

MapHandler maphandler;

void MapHandler::Mat2Map(cv::Mat mat) {
    if (imgSize == _188_180) {
        for (uint8_t j = 0; j < ROW; ++j) {
            uint8_t* ptr = mat.ptr(j);  // 第row行的起点
            for (uint8_t i = 0; i < COL; ++i) {
                imagehandler.imgGray[j][i] = ptr[i];
            }
        }
    } else if (imgSize == _47_60) {
        for (uint8_t j = 0; j < YM; ++j) {
            uint8_t* ptr = mat.ptr(j);
            for (uint8_t i = 0; i < XM; ++i) {
                imagehandler.allmap[YY - j][i] = !(ptr[i] >> 7);
            }
        }
    }
}
/* 二维图像数组--->Mat类(RGB) */
cv::Mat MapHandler::Map2Mat(uint8_t map[YM][XM], QPoint point) {
    cv::Mat mat(YM * MULTIPLE, XM * MULTIPLE, CV_8UC3);  // RGB3通道

    for (int j = 0; j < mat.rows; ++j) {
        cv::Vec3b* ptr = mat.ptr<cv::Vec3b>(j);  // vector<unchar,3>

        /* 图像数组--->真实颜色 */
        for (int i = 0; i < mat.cols; ++i) {  // BGR通道
            uint8_t nowPixel = map[YY - (j / MULTIPLE)][i / MULTIPLE];
            if (nowPixel == 0)  // White
            {
                ptr[i][0] = 255;       // b
                ptr[i][1] = 255;       // g
                ptr[i][2] = 255;       // r
            } else if (nowPixel == 1)  // Black
            {
                ptr[i][0] = 0;
                ptr[i][1] = 0;
                ptr[i][2] = 0;
            } else if (nowPixel == 2)  // Red
            {
                ptr[i][0] = 0;
                ptr[i][1] = 0;
                ptr[i][2] = 255;
            } else if (nowPixel == 3)  // Magenta(类似 Purple)
            {
                ptr[i][0] = 255;
                ptr[i][1] = 0;
                ptr[i][2] = 255;
            } else if (nowPixel == 4)  // DeepSkyBlue
            {
                ptr[i][0] = 255;
                ptr[i][1] = 191;
                ptr[i][2] = 0;
            } else if (nowPixel == 5)  // Green
            {
                ptr[i][0] = 0;
                ptr[i][1] = 255;
                ptr[i][2] = 0;
            } else if (nowPixel == 6) {
                ptr[i][0] = 181;
                ptr[i][1] = 228;
                ptr[i][2] = 255;
            } else if (nowPixel == 7) {
                ptr[i][0] = 181;
                ptr[i][1] = 228;
                ptr[i][2] = 255;
            } else if (nowPixel == 8)  // Moccasin 褐色
            {
                ptr[i][0] = 181;
                ptr[i][1] = 228;
                ptr[i][2] = 255;
            } else if (nowPixel == 250) {
                ptr[i][0] = 0;
                ptr[i][1] = 255;
                ptr[i][2] = 255;
            } else if (nowPixel == 251) {
                ptr[i][0] = 0;
                ptr[i][1] = 255;
                ptr[i][2] = 0;
            } else if (nowPixel == 253)  // DarkOrange
            {
                ptr[i][0] = 0;
                ptr[i][1] = 140;
                ptr[i][2] = 255;
            } else if (nowPixel == 254)  // Chartreuse(黄绿)
            {
                ptr[i][0] = 0;
                ptr[i][1] = 255;
                ptr[i][2] = 127;
            } else if (nowPixel == 255)  // Grey
            {
                ptr[i][0] = 190;
                ptr[i][1] = 190;
                ptr[i][2] = 190;
            }
        }
    }

    /* 选中像素点高亮 */
    if (point.x() >= 0 && point.y() >= 0) {
        int x = point.x();
        int y = point.y();
        QPoint highlight;

        for (int i = 0; i < MULTIPLE; ++i) {
            for (int j = 0; j < MULTIPLE; ++j) {
                highlight.rx() = x * MULTIPLE + i;
                highlight.ry() = y * MULTIPLE + j;

                if (highlight.y() >= 0 && highlight.y() < mat.rows &&
                    highlight.x() >= 0 && highlight.x() < mat.cols) {
                    /* LimeGreen */
                    mat.at<cv::Vec3b>(highlight.y(), highlight.x())[0] = 50;
                    mat.at<cv::Vec3b>(highlight.y(), highlight.x())[1] = 205;
                    mat.at<cv::Vec3b>(highlight.y(), highlight.x())[2] = 50;
                }
            }
        }
    }

    return mat;
}
void MapHandler::showMat(cv::Mat bgrMat, QLabel* label) {
    cv::Mat rgbMat;
    cvtColor(bgrMat, rgbMat, CV_BGR2RGB);
    QImage img(static_cast<const uint8_t*>(rgbMat.data), rgbMat.cols,
               rgbMat.rows, rgbMat.cols * rgbMat.channels(),
               QImage::Format_RGB888);

    label->setPixmap(QPixmap::fromImage(img));
    label->resize(label->pixmap()->size());
    label->show();
}

void MapHandler::showMap(uint8_t map[YM][XM], QLabel* label, QPoint point) {
    cv::Mat mat = Map2Mat(map, point);
    showMat(mat, label);
}

void MapHandler::showRealMap(QLabel* label, QPoint) {
    cv::Mat realmat;

    if (imgSize == _188_180) {
        cv::Point2f mapPoint[4] = {cv::Point2f(0, 84), cv::Point2f(187, 84),
                                   cv::Point2f(76, 0), cv::Point2f(111, 0)};
        cv::Point2f realPoint[4] = {cv::Point2f(100, 192),
                                    cv::Point2f(140, 192), cv::Point2f(100, 0),
                                    cv::Point2f(140, 0)};

        /* 逆透视变换 */
        cv::Mat M =
            getPerspectiveTransform(mapPoint, realPoint);  // 获取透视变换矩阵
        warpPerspective(imgMat, realmat, M, cv::Size(400, 200),
                        cv::INTER_AREA);  // 对图像进行透视变换

        cv::resize(realmat, realmat, cv::Size(0, 0), 3, 3);
    } else if (imgSize == _47_60) {
        cv::Point2f mapPoints[4] = {cv::Point2f(0, 41), cv::Point2f(46, 41),
                                    cv::Point2f(20, 0), cv::Point2f(26, 0)};
        cv::Point2f realPoints[4] = {cv::Point2f(100, 192),
                                     cv::Point2f(140, 192), cv::Point2f(100, 0),
                                     cv::Point2f(140, 0)};
        uint8_t basemapTemp[YM][XM] = {{0}};

        for (uint8_t j = 0; j < YM; ++j) {
            for (uint8_t i = 0; i < XM; ++i) {
                if (!imagehandler.basemap[j][i]) {
                    basemapTemp[YY - j][i] = 255;
                }
            }
        }
        cv::Mat M = getPerspectiveTransform(mapPoints, realPoints);
        warpPerspective(cv::Mat(YM, XM, CV_8UC1, basemapTemp), realmat, M,
                        cv::Size(240, 200), cv::INTER_AREA);
    }

    QImage img(static_cast<const uint8_t*>(realmat.data), realmat.cols,
               realmat.rows, realmat.cols * realmat.channels(),
               QImage::Format_Grayscale8);
    label->setPixmap(QPixmap::fromImage(img));
    label->resize(label->pixmap()->size());
    label->show();
}

void MapHandler::showImgGray(uint8_t map[ROW][COL],
                             QLabel* label,
                             QPoint point) {
    uint8_t multiple = 2;
    cv::Mat grayMat(ROW, COL, CV_8UC1, map);
    cv::Mat rgbMat;

    cvtColor(grayMat, rgbMat, CV_GRAY2RGB);                  // GRAY--->RGB
    cv::resize(rgbMat, rgbMat, {0, 0}, multiple, multiple);  // 压缩

    /* 高亮选中像素 */
    if (point.x() >= 0 && point.y() >= 0) {
        int x = point.x();
        int y = point.y();
        QPoint highlight;

        for (int i = 0; i < 4 * multiple; ++i) {
            for (int j = 0; j < 2 * multiple; ++j) {
                highlight.rx() = x * multiple * 4 + i;
                highlight.ry() = y * multiple * 2 + j;
                if (highlight.y() >= 0 && highlight.y() < rgbMat.rows &&
                    highlight.x() >= 0 && highlight.x() < rgbMat.cols) {
                    rgbMat.at<cv::Vec3b>(highlight.y(), highlight.x())[0] = 50;
                    rgbMat.at<cv::Vec3b>(highlight.y(), highlight.x())[1] = 205;
                    rgbMat.at<cv::Vec3b>(highlight.y(), highlight.x())[2] = 50;
                }
            }
        }
    }

    QImage img(static_cast<const uint8_t*>(rgbMat.data), rgbMat.cols,
               rgbMat.rows, rgbMat.cols * rgbMat.channels(),
               QImage::Format_RGB888);
    label->setPixmap(QPixmap::fromImage(img));  // convert the image to a pixmap
    label->resize(label->pixmap()->size());
    label->show();
}
