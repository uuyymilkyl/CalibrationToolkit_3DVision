#ifndef  _LASER_CALIBRATION_H
#define  _LASER_CALIBRATION_H

#include<opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include "MCamCalibration.hpp"

class MCalibLaser
{
public:
	MCalibLaser();
	~MCalibLaser();
	// ���ع�ƽ��궨ͼ��
	void LoadLaserImage(std::string &_LaserImgDir ,std::vector<cv::Mat>& _CaliCamImg, std::vector <cv::Mat>& _CaliLasImg, cv::Mat& _intrinsicMatrix, cv::Mat& _coeffsMatrix);
	
	// �궨����ȡ�޼���ͼ�Ķ�Ӧ���
	void CalibraCamForLaserImg(std::vector<cv::Mat>& _CaliCamImg, std::vector< cv::Mat> &_Rves, std::vector<cv::Mat> &_Tves);
	
	//Steger�㷨��ȡ��������
	static std::vector<cv::Point2f> GetLaserPoints_Steger(cv::Mat& src, int gray_Thed, int Min, int Max, int Type);

	//Sobel�㷨��ȡ��������
	static std::vector<cv::Point2f> GetLaserPoints_Sobel(const cv::Mat& inputImage);

	// ��ȡͨ���궨���ȡ����㼯�ķ�Χ
	void GetCaliRangeAndFitLine(cv::Mat& _inputImg, int &xmin, int &ymin, int &xmax, int &ymax);

	//ͨ���궨��ǵ㷶Χɸѡ����������
	std::vector<cv::Point2f> FilterLaserPointsByRange(std::vector<cv::Point2f> _LaserPoints,int xmin,int ymin,int xmax,int ymax);

	//������ϱ궨��Ľǵ�
	std::vector<cv::Vec4f> FitChessBoardLines(const std::vector<cv::Point2f>& corners, cv::Mat& image,std::vector<std::vector<cv::Point2f>> &_Line_Points);

	// ��ÿ���㣩ͼ������ת�������
	void PointToCameraPoint(cv::Point2f _vptImg, cv::Point3f& _vptCam, const cv::Mat _matRvecs, const cv::Mat _matTvecs, const cv::Mat _matIntrinsics);

	cv::Point2f getCrossPoint(const cv::Vec4f& line1, const cv::Vec4f& line2);

	//��Ϲ�ƽ��
	void FitLaserPlane(std::vector<cv::Point3f> _vecPoints3ds, cv::Mat& _mat_plane);

	//���ڹ�ƽ��ϵ�� ��ͼ������ϵתΪ�������ϵ��2d -> 3d)
	static void ImgPtsToCamFrame(const cv::Mat _LaserPlane_Coeffs, const cv::Mat _Intrinsic_Matrix, cv::Mat _Dist_Coeff,
		std::vector<cv::Point2f> _vec_ptSrc, std::vector<cv::Point3f>& _vec_ptDst);
	//�������
	static void DistortPoints(std::vector<cv::Point2f> _ptSrc, std::vector<cv::Point2f>& _ptDst, const cv::Mat _matIntrinsics, const cv::Mat _matDistCoeff);

	static void Writedown(std::vector<cv::Point3f>& _vec_ptDst,std::string _name);


	
private:

	cv::Mat BaseLayExtrinxMatrix;
	std::vector<std::vector<cv::Point2f>> m_imagePoints;
	
};





#endif // ! _LASER_CALIBRATION_H
