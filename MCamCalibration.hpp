#ifndef _MCALIBRATION_H_
#define _MCAlIBRATION_H_


#include<opencv2/opencv.hpp>
#include<opencv2/dnn.hpp>
#include <iostream>
#include <string>

class MCalibCam
{
public:
	MCalibCam();
	~MCalibCam();

    // ���ñ궨���̸�ĳߴ�
	void SetBoardSize(int WidthLatticeNum, int HeightLatticeNum, float SquarSize); 

    // ����ͼ�����̸�����
	void LoadChessBoardData(std::string& _ImgDir); 

    // ����궨������ - �õ�����ڲ� - ������� - ÿ��ͼ���ƽ�ƾ��� - ��ת����
    void CalibrationOfCamera( cv::Mat &_intrinxMatrix, cv::Mat &_coeffsMatrix);

    // ������������ϵ������ 
    void CalEConerCoordinationInCamera(std::vector<cv::Point2f>_corner,int imageindex, cv::Mat Rvecs, cv::Mat Tvecs);  //����ÿ���ǵ㵽�����Z�����

    cv::Point3f CalPlaneLineIntersectPoint(const cv::Vec3d& planeNormal, const cv::Point3f& planePoint,
        const cv::Vec3f& lineDirection, const cv::Point3f& linePoint);



private:
    int m_WidthLattinceNum; //�궨�����ǵ����������-1��
    int m_HeightLatticeNum; //�궨������ǵ����������-1��
    float m_SquarSize;      //�궨���߳���mm)
    int m_PixelSize;        //��Ԫ�ߴ磨���sensor��������mm)

    cv::Size m_ImgSize;     //�����ͼ��ֱ���
    std::vector<cv::String> m_ImgList;

    cv::Mat m_IntrinxMatrix;  //������ڣ�����
    cv::Mat m_CoeffsMatrix;   //�������

    std::vector<std::vector<cv::Point2f>> m_imagePoints;
    std::vector<std::vector<cv::Point3f>> m_objectPoints;
};


static bool haveSameXCoordinate(const cv::Point2f& p1, const cv::Point2f& p2) {
    return p1.x == p2.x;
}

// Function to compare points based on y coordinate
static bool comparePointsByY(const cv::Point2f& p1, const cv::Point2f& p2) {
    return p1.y < p2.y;
}

// Function to find median of y coordinates of adjacent points with the same x coordinate
static std::vector<cv::Point2f> findMedianPoints(const std::vector<cv::Point2f>& points) {
    std::vector<cv::Point2f> resultPoints;

    for (size_t i = 0; i < points.size(); ++i) {
        if (i == 0 || !haveSameXCoordinate(points[i], points[i - 1])) {
            // If it's the first point or if current point has different x coordinate than the previous point
            resultPoints.push_back(points[i]);
        }
        else {
            // Find adjacent points with same x coordinate
            size_t startIndex = i;
            while (i < points.size() && haveSameXCoordinate(points[i], points[startIndex])) {
                ++i;
            }

            // Collect y coordinates of adjacent points
            std::vector<float> yCoordinates;
            for (size_t j = startIndex; j < i; ++j) {
                yCoordinates.push_back(points[j].y);
            }

            // Calculate median of y coordinates
            float medianY = 0.0f;
            if (!yCoordinates.empty()) {
                std::sort(yCoordinates.begin(), yCoordinates.end());
                size_t mid = yCoordinates.size() / 2;
                if (yCoordinates.size() % 2 == 0) {
                    medianY = (yCoordinates[mid - 1] + yCoordinates[mid]) / 2.0f;
                }
                else {
                    medianY = yCoordinates[mid];
                }
            }

            // Create a new point with same x coordinate and median y coordinate
            resultPoints.emplace_back(points[startIndex].x, medianY);
            --i; // Adjust the index as the loop will increment it again
        }
    }

    return resultPoints;
}


inline static std::vector<cv::Point2f> mergePointsWithEqualY(std::vector<cv::Point2f>& ContoursPoints) {
    // ����һ�� map �� x ������Ϊ�������㰴����ͬ�� x �������
    std::map<float, std::vector<cv::Point2f>> pointsByX;

    // ���㰴�� y �������
    for (const auto& point : ContoursPoints) {
        pointsByX[point.x].push_back(point);
    }

    std::vector<cv::Point2f> mergedPoints;
    std::vector<cv::Point2f> EveYPoints;
    // ��ÿ����ͬ y ����ĵ㼯�Ͻ��д���
    for (const auto& pair : pointsByX) 
    {
        const auto& points = pair.second;


        EveYPoints = findMedianPoints(points);
        mergedPoints.insert(mergedPoints.end(), EveYPoints.begin(), EveYPoints.end());

    }
    return mergedPoints;
}

inline static void ExtractRedLaserContour(const cv::Mat& inputImage, std::vector<cv::Point2f>& outputContourPoints) {
    // ת������ͼ��HSV��ɫ�ռ�
    cv::Mat hsvImage;
    cv::cvtColor(inputImage, hsvImage, cv::COLOR_BGR2HSV);

    // ����ɫ�����߲�����ȡ����
    cv::Mat redMask;
    cv::Mat whiteMask;
    //cv::inRange(inputImage, cv::Scalar(0, 0, 25), cv::Scalar(70, 70, 255), redMask);
    cv::inRange(inputImage, cv::Scalar(0, 0, 100), cv::Scalar(70, 70, 255), redMask);
    cv::inRange(inputImage, cv::Scalar(140, 110, 220), cv::Scalar(255, 255, 255), whiteMask);
    cv::Mat redContourImage = cv::Mat::zeros(inputImage.size(), CV_8UC1);



    // ����dx��dy����ֵѡ�����Ψһ��������
    /*
    for (size_t i = 0; i < contours.size(); ++i) {
        std::vector<cv::Point>& contour = contours[i];
        if (contour.size() < 2) {
            continue;
        }
        for (size_t j = 0; j < contour.size(); ++j) {
            int x = contour[j].x;
            int y = contour[j].y;

            if (x % dx == 0 && y % dy == 0) {
                cv::Vec3b pixel = hsvImage.at<cv::Vec3b>(y, x);
                if (pixel[0] > redThreshold) {
                    outputContourPoints.push_back(cv::Point2f(x, y));
                }
            }
        }
    }*/

    std::vector<cv::Point2f> ContoursPoints;
    for (int y = 0; y < redMask.rows; ++y)
    {
        for (int x = 0; x < redMask.cols; ++x)
        {
            if (redMask.at<uchar>(y, x) == 255)
            {
                ContoursPoints.push_back(cv::Point2f(x, y));
            }
            if (whiteMask.at<uchar>(y, x) == 255)
            {
                ContoursPoints.push_back(cv::Point2f(x, y));
            }
        }
    }
    

    std::vector<cv::Point2f> ContoursPoints_Merge;

    ContoursPoints_Merge= mergePointsWithEqualY(ContoursPoints);

    outputContourPoints = ContoursPoints_Merge;
    for (int i = 0; i < ContoursPoints_Merge.size(); i++)
    {
        cv::circle(inputImage, ContoursPoints_Merge[i], 1, cv::Scalar(255, 0, 0), 2, 6, 0);
    }
}

#endif