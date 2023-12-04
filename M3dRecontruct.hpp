#ifndef _M3DRecontruct_H_
#define _M3DRecontruct_H_

#include<opencv2/opencv.hpp>
#include<opencv2/dnn.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "MLasCalibration.hpp"
#include "MHandEyeCalibration.hpp"

#define CamCaliXmlPath
#define LaserCaliXmlPath
#define HandEyeCaliXmlPath

class M3DRecontruct
{
public:
	M3DRecontruct();
	~M3DRecontruct();
	//�������� 
	static void WritedownCloudPoints(std::vector<cv::Point3f>& _vec_ptDst, std::string _name);

	//���ر궨��Ϣ
	void LoadCalibraionParam(std::string& _CamCaliXmlDir, std::string& _LaserCaliXmlDir, std::string& _HandEyeCaliXmlDir);

	//ͨ����Ƶ�������
	void VideoRecontruct(std::string& _videopath);

	//ͨ��������Ƭ�����ԣ����ӵ��Ʋ��õ��任��̬
	void TestRecontructByFrame(std::string &_ImagePath,std::string &_ArmPosePath);

	//����ÿ�����Ƶ���������ϵ
	std::vector<cv::Point3f> AccumulateFrameRecontruct(cv::Mat &_frame, cv::Mat& _HandEyeMatrix_R , cv::Mat& _HandEyeMatrix_T, cv::Mat& _CurrenArmPose);


	// �ӹ��� 
	cv::Mat eulerToRotationMatrix(const cv::Mat& angles);

	cv::Mat buildPoseMatrix(const cv::Mat& t, const cv::Mat& angles);


	//�����ã���ʱ���滻������Ͳ�������ӿ�
	std::vector<cv::Mat> LoadArmPoseByTxt(std::string& _Dir);
private:
	cv::Mat m_IntrinxMatrix;     ///< ����ڲ�
	cv::Mat m_DistortCoeffs;     ///< ����ϵ��
	cv::Mat m_LaserPlaneCoeffs;  ///< ��ƽ��ϵ��
	cv::Mat m_HandEyeMatrix_R;   ///< ���۱궨��R
	cv::Mat m_HandEyeMatrix_T;   ///< ���۱궨��T
	cv::Mat m_LastArmPose;       ///< ��һ֡�Ļ�е����̬

	
	int m_frame_count;
};


#endif // !_M3D_Recontruct_H_
