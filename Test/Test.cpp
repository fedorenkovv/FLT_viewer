#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"


#include "..\flow-code\flowIO.h"
//#include "..\flow-code\imageLib\Image.h"

// используем пространстав имён cv и std
using namespace cv;
using namespace std;

// объявление функции справки
void readme();

/** @function main - главная функция */
int main(int argc, char** argv)
{

	string exrFile;
	exrFile = "..\\test5.exr";

	// загружаем изображения
	Mat image = imread(exrFile.c_str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	int depth = image.depth();
	int c = image.channels();
	CFloatImage img;
	int nBands = 2;
	Vec3f val = image.at<Vec3f>(image.rows / 2, image.cols / 2);
	
	CShape sh(image.cols, image.rows, nBands);
	img.ReAllocate(sh);

	for (int i = 0; i < image.rows; i++)
	for (int j = 0; j < image.cols; j++)
	{
	val = image.at<Vec3f>(i, j);
	img.Pixel(j, i, 0) = val.val[0];
	img.Pixel(j, i, 1) = val.val[1];
	}

	WriteFlowFile(img, "out1.flo");


	return 0;
	///////////////////////////////////////////////////////
	string path = "D:\\Features2D\\relief\\";
	string ext = ".bmp";
	string name = "im_ora0";
	int pointNum = 0;


	// путь к заданным файлам с изобаржениями
	for (int ff = 1; ff < 8; ff++)
	for (int sf = ff + 1; sf < 9; sf++)
	{
		string sceneFile, objectFile;
		sceneFile = path + name + to_string(ff) + ext;
		objectFile = path + name + to_string(sf) + ext;

		cout << "File1: " << sceneFile << endl;
		cout << "File2: " << objectFile << endl;

		// загружаем изображения
		Mat img_object = imread(objectFile.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		Mat img_scene = imread(sceneFile.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		//CvMat* img_object1 = cvLoadImageM(objectFile.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		//CvMat* img_scene1 = cvLoadImageM(sceneFile.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

		//Mat img_object = cv::Mat(img_object1);
		//Mat img_scene = cv::Mat(img_scene1);
		// если не уадлось загрузить изображения, выходим
		if (!img_object.data || !img_scene.data)
		{
			std::cout << " --(!) Error reading images " << std::endl; return -1;
		}

		//-- Шаг 1: Обнаружить характерные точки с помощью SURF
		int minHessian = 400;

		// создаём детектор
		SurfFeatureDetector detector(minHessian);

		// создаём массивы характерных точек
		std::vector<KeyPoint> keypoints_object, keypoints_scene;

		// ищем характерные точки
		detector.detect(img_object, keypoints_object);
		detector.detect(img_scene, keypoints_scene);

		//-- Шаг 2: Рассчёт дескрипторов (векоторов характерных точек)
		SurfDescriptorExtractor extractor;

		// создаём массивы дексрипторов
		Mat descriptors_object, descriptors_scene;

		// вычисяляем дескрипторы
		extractor.compute(img_object, keypoints_object, descriptors_object);
		extractor.compute(img_scene, keypoints_scene, descriptors_scene);

		//-- Шаг 3: Ищем соответствие с помощь алгоритма FLANN (поиск ближайшего соседа)
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;

		// производим поиск соответствий
		matcher.match(descriptors_object, descriptors_scene, matches);

		double max_dist = 0; double min_dist = 100;

		//-- Вычисялем минимальное и максимальное расстояние между сооствествующими точками
		for (int i = 0; i < descriptors_object.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		// сообщаем найденные расстояния пользователю
		printf("-- Max dist : %f \n", max_dist);
		printf("-- Min dist : %f \n", min_dist);

		//-- Сооздаём массив "хороших" соответсвующих точек (тех, для которых расстояние меньше 3*min_dist )
		std::vector< DMatch > good_matches;

		for (int i = 0; i < descriptors_object.rows; i++)
		{
			if (matches[i].distance < 3 * min_dist)
			{
				good_matches.push_back(matches[i]);
			}
		}

		Mat img_matches;
		drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		//-- Localize the object
		std::vector<Point2f> obj;
		std::vector<Point2f> scene;

		for (int i = 0; i < good_matches.size(); i++)
		{
			for (int j = 0; j < obj.size(); j++)
			//-- Get the keypoints from the good matches
			if (keypoints_object[good_matches[i].queryIdx].pt.x == obj[j].x && keypoints_object[good_matches[i].queryIdx].pt.y == obj[j].y)
				int k = 23;

			obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
			scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
		}
		ofstream sc;
		string outputFile = path + "points" + to_string(ff) + ".pnt";
		sc.open(outputFile, ios::app);
		//sc << "Marker" << endl;
		//sc << obj.size() << endl;
		for (int i = 0; i < obj.size(); i++)
			sc << pointNum + i + 1 << " " << (int)obj[i].x << " " << (int)obj[i].y << endl;
		sc.close();

		outputFile = path + "points" + to_string(sf) + ".pnt";
		sc.open(outputFile, ios::app);
		//ob << "Marker" << endl;
		//ob << scene.size() << endl;
		for (int i = 0; i < scene.size(); i++)
			sc << pointNum + i + 1 << " " << (int)scene[i].x << " " << (int)scene[i].y << endl;
		sc.close();
		pointNum += scene.size();

	}
	//-- Show detected matches
	//imshow("Good Matches & Object detection", img_matches);
	//for (int i = 0; i < (int)good_matches.size(); i++)
	//{
	//	printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	//}


	waitKey(0);
	return 0;
}

/** @function readme */
void readme()
{
	std::cout << " Usage: ./SURF_descriptor <img1> <img2>" << std::endl;
}