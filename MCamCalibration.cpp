
#include"MCamCalibration.hpp"

MCalibCam::MCalibCam()
{
	std::string ImgDir = "./CaliCamIn/*.jpg";

	//�������̸����� �����ڽǵ����������ڽǵ��������̸�����mm)
	SetBoardSize(11, 8, 5);

	//����ͼ������̸�����
	LoadChessBoardData(ImgDir);

	//������Ԫ�ߴ磨��ͷ�������ݣ�
	this->m_PixelSize = 0.003; //0.003mm - 3΢��

	// �ҵ����̸�ǵ�1
	std::vector<std::vector<cv::Point2f>> imagePoints;

	// �����ڲξ���2���������3
	cv::Mat intrinxMatrix, coeffsMatrix;

	// �궨��123
	CalibrationOfCamera(intrinxMatrix, coeffsMatrix);

	std::vector<cv::Mat> Rvecs, Tvecs; //ÿ��ͼ�����ת����4 ƽ�ƾ���5

	calibrateCamera(this->m_objectPoints, this->m_imagePoints, m_ImgSize, intrinxMatrix, coeffsMatrix, Rvecs, Tvecs);


	// ��ӡ�궨���
	std::cout << "Camera Matrix:\n" << intrinxMatrix << "\n\n";
	std::cout << "Distortion Coefficients:\n" << coeffsMatrix << "\n\n";

	m_IntrinxMatrix = intrinxMatrix;
	m_CoeffsMatrix = coeffsMatrix;


	/*
	for (int i = 0; i < m_ImgList.size(); i++)
	{
		// ÿһ��ͼ�������нǵ�ľ���
		//CalculateEvConerDistance(m_imagePoints[i], i,Rvecs[i],Tvecs[i]);

	}
	*/

}

MCalibCam::~MCalibCam()
{
}
 // �����ڲξ���K�ͻ���ϵ��distCoeffs��ͶӰͼ���ϵĵ�
cv::Point2f undistortPoint(const cv::Point2f& distorted_point, const cv::Mat& K, const cv::Mat& distCoeffs) {
	cv::Mat distorted = (cv::Mat_<float>(1, 1) << distorted_point.x, distorted_point.y);
	cv::Mat undistorted;
	cv::undistortPoints(distorted, undistorted, K, distCoeffs);
	return cv::Point2f(undistorted.at<float>(0, 0), undistorted.at<float>(0, 1));
}

void MCalibCam::SetBoardSize(int _nWidthLatticeNum, int _nHeightLatticeNum, float _fSquarSize)
{
	m_HeightLatticeNum = _nHeightLatticeNum;
	m_WidthLattinceNum = _nWidthLatticeNum;
	m_SquarSize = _fSquarSize;
}

void MCalibCam::LoadChessBoardData(std::string& _ImgDir)
{
	std::vector<cv::String> ImgList;
	cv::glob(_ImgDir, ImgList);
	m_ImgList = ImgList;


	std::vector<std::vector<cv::Point3f>> objectPoints;
	std::vector<cv::Point3f> realChessboardData;

	for (int i = 0; i < m_HeightLatticeNum; ++i)
	{
		for (int j = 0; j < m_WidthLattinceNum; ++j)
		{
			realChessboardData.push_back(cv::Point3f(j * m_SquarSize, i * m_SquarSize, 0.0f));
		}
	}

	for (size_t i = 0; i < ImgList.size(); ++i)
	{
		objectPoints.push_back(realChessboardData);
	}
	m_objectPoints = objectPoints;
}

void MCalibCam::CalibrationOfCamera(cv::Mat& _intrinxMatrix, cv::Mat& _coeffsMatrix)
{
	std::vector<cv::Point2f> corners;

	for (int i = 0; i < m_ImgList.size(); ++i)
	{
		cv::Mat image = cv::imread(m_ImgList[i]);
		cvtColor(image, image, cv::COLOR_BGR2GRAY);

		//�����̸�ǵ�
		bool found = findChessboardCorners(image, cv::Size(m_WidthLattinceNum, m_HeightLatticeNum), corners);

		if (found)
		{
			// �����ض�λ��ȷ��Ѱ�ǵ� 
			cornerSubPix(image, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
			// ����ͼ���ҵ��Ľǵ㼯�Ż�vector��
			m_imagePoints.push_back(corners);
			// �����ǵ㣨���ӻ�������
			drawChessboardCorners(image, cv::Size(m_WidthLattinceNum, m_HeightLatticeNum), corners, found);
		}

		m_ImgSize = image.size();
		//m_imagePoints.push_back(corners);
	}

}

void MCalibCam::CalEConerCoordinationInCamera(std::vector<cv::Point2f>_corner,int imgindex,cv::Mat Rvecs, cv::Mat Tvecs)
{
	double fx = m_IntrinxMatrix.at<double>(0, 0);  // ������x���ϵķ���
	double fy = m_IntrinxMatrix.at<double>(1, 1);  // ������y���ϵķ���
	double cx = m_IntrinxMatrix.at<double>(0, 2);  // ͼ�����ĵ�x����
	double cy = m_IntrinxMatrix.at<double>(1, 2);  // ͼ�����ĵ�y����
	double PixelSize = 0.003;


	cv::Mat rotationMatrix;
	cv::Mat imagePoint;
	for (int i = 0; i < _corner.size(); i++)
	{
		imagePoint = (cv::Mat_<double>(3, 1) << double(_corner[i].x), double(_corner[i].y), 1);
		//�����������S

		double s;
		cv::Mat tempMat, tempMat2;
		cv::Mat rotationMatrix;
		Rodrigues(Rvecs, rotationMatrix);

		tempMat = rotationMatrix.inv() * m_IntrinxMatrix.inv() * imagePoint;
		tempMat2 = rotationMatrix.inv() * Tvecs;
		s = tempMat2.at<double>(2, 0);
		s /= tempMat.at<double>(2, 0);

		// ������������
		cv::Mat matWorldPoints = rotationMatrix.inv() * (s * m_IntrinxMatrix.inv() * imagePoint - Tvecs);

		// �����������
		cv::Mat matCameraPoints = rotationMatrix * matWorldPoints + Tvecs;


		cv::Point3f ptCameraPoints(matCameraPoints.at<double>(0, 0), matCameraPoints.at<double>(1, 0), matCameraPoints.at<double>(2, 0));


	}
	
}

cv::Point3f MCalibCam::CalPlaneLineIntersectPoint(const cv::Vec3d& planeNormal, const cv::Point3f& planePoint, const cv::Vec3f& lineDirection, const cv::Point3f& linePoint)
{
	double denom = planeNormal.dot(lineDirection);
	if (std::abs(denom) < 1e-6) 
{
		// ƽ�л��غϣ�û�н���
		return cv::Point3f(NAN, NAN, NAN); // ������Ч�ĵ�
	}
	else {
		cv::Vec3f diff = cv::Vec3f(planePoint.x - linePoint.x, planePoint.y - linePoint.y, planePoint.z - linePoint.z);
		double t = diff.dot(planeNormal) / denom;
		cv::Point3f intersectionPoint(linePoint.x + t * lineDirection[0],
			linePoint.y + t * lineDirection[1],
			linePoint.z + t * lineDirection[2]);
		return intersectionPoint;
	}
}


