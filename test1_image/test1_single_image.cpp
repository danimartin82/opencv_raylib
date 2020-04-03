#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "raylib.h"


using namespace std;

int main(int argc, char** argv)
{
    cv::Mat image_cv;
	cv::Mat image_cv_rgb;
	Image image_raylib;

	 

	//--------------------------------------------------------------------------------------
	// Load image
	//--------------------------------------------------------------------------------------
	image_cv = cv::imread("img1.bmp", cv::IMREAD_COLOR);

	// Image img1 = LoadImage("img1.png");

	if (image_cv.empty())
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}


	//--------------------------------------------------------------------------------------
	// Init Raylib window
	//--------------------------------------------------------------------------------------
	int screenWidth = 800;
	int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "raylib opencv test1: load image");
	SetTargetFPS(60);

	/*    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
		cv::imshow("Display window", image_cv);
		cv::waitKey(0);
		*/

	//--------------------------------------------------------------------------------------
	// Concertison from OpenCV to Raylib
	//--------------------------------------------------------------------------------------
	cv::cvtColor(image_cv, image_cv_rgb, cv::COLOR_BGR2RGB);


	image_raylib.width = image_cv_rgb.cols;
	image_raylib.height = image_cv_rgb.rows;
	image_raylib.format = UNCOMPRESSED_R8G8B8;
	image_raylib.mipmaps= 1 ;
	image_raylib.data= (void*)(image_cv_rgb.data);
	
	Texture2D texture = LoadTextureFromImage(image_raylib);      // Image converted to texture, uploaded to GPU memory (VRAM)




	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// Update

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);
        DrawTexture(texture, 0, 0, WHITE);
		

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------   
    //UnloadImage(img1);
    UnloadTexture(texture);
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

    return 0;
}