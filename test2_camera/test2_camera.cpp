#include <stdio.h>
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/utility.hpp>
#include "raylib.h"


using namespace std;

int main(int argc, char** argv)
{
    cv::Mat frame1;
	cv::Mat frame2;
	cv::Mat frame_diff;
	cv::Mat frame_gray;
	cv::Mat frame_blurr;
	cv::Mat frame_thresh;
	cv::Mat frame_dilated;
	cv::Mat framergb;
	vector<vector<cv::Point>> array_contours;
	std::vector <Rectangle> vector_of_rectangles;

	Image image_raylib;

	int screenWidth;
	int screenHeight;
	
	//--------------------------------------------------------------------------------------
	// open the default camera
	//--------------------------------------------------------------------------------------
	cv::VideoCapture cap("C:/GitHub/opencv_raylib/test2_camera/video1.avi");
	if (!cap.isOpened())
	{
		printf("ERROR Cannot open video\n");
	}

	cap.read(frame1);
	cap.read(frame2);

	//--------------------------------------------------------------------------------------
	// Init Raylib window
	//--------------------------------------------------------------------------------------
	screenWidth = frame1.cols;
	screenHeight = frame1.rows;


	InitWindow(screenWidth, screenHeight, "raylib opencv test2: camera");
	SetTargetFPS(30);


	//--------------------------------------------------------------------------------------
	// Convert from OpenCV to Raylib
	//--------------------------------------------------------------------------------------
	 
	cv::cvtColor(frame2, framergb, cv::COLOR_BGR2RGB);


	image_raylib.width = framergb.cols;
	image_raylib.height = framergb.rows;
	image_raylib.format = UNCOMPRESSED_R8G8B8;
	image_raylib.mipmaps= 1 ;
	image_raylib.data= (void*)(framergb.data);
	
	Texture2D texture = LoadTextureFromImage(image_raylib);      // Image converted to texture, uploaded to GPU memory (VRAM)

	SetTraceLogLevel(LOG_NONE);
	

		
	// Main loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// OpenCV algorithm for movement detection. Output is a vector of vectors
		cv::absdiff(frame1, frame2, frame_diff);
		cv::cvtColor(frame_diff, frame_gray,cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(frame_gray, frame_blurr, cv::Size(5, 5), 10);
		double threshold= cv::threshold(frame_blurr, frame_thresh, 20, 255, cv::THRESH_BINARY);
		cv::dilate(frame_thresh, frame_dilated, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
		cv::findContours(frame_dilated, array_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
		
		// Generate bounding rectangles and store only the relevant ones in raylib format
		vector_of_rectangles.clear();
		for (int i = 0; i < array_contours.size(); i++)
		{
			cv::Rect aux= boundingRect(array_contours[i]);
			if ((aux.width*aux.height > 900) && (aux.x > 30))
			{
				Rectangle aux2 = { aux.x,aux.y,aux.width,aux.height };
				vector_of_rectangles.push_back(aux2);
			}
		}
		// Paint last caputre frame
		cv::cvtColor(frame2, framergb, cv::COLOR_BGR2RGB);
		image_raylib.data = (void*)(framergb.data);
		Texture2D texture = LoadTextureFromImage(image_raylib);


		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);
        DrawTexture(texture, 0, 0, WHITE);

		for (int i = 0; i < vector_of_rectangles.size(); i++)
		{
			DrawRectangleLines(vector_of_rectangles[i].x, vector_of_rectangles[i].y, vector_of_rectangles[i].width, vector_of_rectangles[i].height, VIOLET);
		}
		EndDrawing();
		//----------------------------------------------------------------------------------

		// Caputre new frame
		frame1.release();
		frame1 = frame2;
		frame2.release();
		cap.read(frame2);
		if (frame2.empty())
		{
			break;

		}
	
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------   
    //UnloadImage(img1);
    UnloadTexture(texture);
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

    return 0;
}