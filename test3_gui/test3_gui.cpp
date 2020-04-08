/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Program test3:gui                                                                    */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


/*------------------------------------- Includes ---------------------------------------*/
#include <stdio.h>
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2\objdetect.hpp>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 
#include "utils.h"
#include "GUI_ring.h"


using namespace std;


/*------------------------------------- Function headers -------------------------------*/

void opencv_algorithm(void);
void drawOverlays(Texture2D texture, RenderTexture2D target,
	Shader shader_BLUR,
	std::vector<Rectangle> vector_of_rectangles_azules,
	std::vector<Rectangle> vector_of_rectangles_rojos,
	double total_time_in_video,
	double CPU_load);



/*------------------------------------- Global variables -------------------------------*/

cv::Mat frame_in;
cv::Mat frame2_inrange_azules;
cv::Mat frame3_dilate_azules;
std::vector<vector<cv::Point>> array_contours_azules;
std::vector <Rectangle> vector_of_rectangles_azules;
std::vector <cv::Rect> vector_of_opencv_rectangles_azules;

cv::Mat frame2_inrange_rojos;
cv::Mat frame3_dilate_rojos;
std::vector<vector<cv::Point>> array_contours_rojos;
std::vector <Rectangle> vector_of_rectangles_rojos;
std::vector <cv::Rect> vector_of_opencv_rectangles_rojos;

Image frame_raylib;
cv::Mat framergb;
Texture2D texture;


int selector = 0;
cv::Rect aux;
Rectangle aux2;
int screenWidth;
int screenHeight;


// OpenCV Alogrithms parameters

Color azules_min = { 19,  95,  59 };
Color azules_max = { 57, 150, 178 };
Color rojos_min = { 120,  30,  15 };
Color rojos_max = { 200,  50,  50 };


int dilation_size = 7;
int dilation_type = cv::MORPH_RECT;
cv::Mat dilatation_element = cv::getStructuringElement(dilation_type, cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1), cv::Point(dilation_size, dilation_size));
int groupingRectangles_th = 1;
int groupingRectangles_eps = 0.5;
double total_frames_in_video;
double total_time_in_video;
int framerate = 25;
double CPU_load = 0;

//Shader parameter
float aperture=178.0;

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: main()                                                                     */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
	
	// Open input video
	cv::VideoCapture cap("Secuencia 01.avi");
	if (!cap.isOpened())
	{
		printf("ERROR Cannot open video\n");
	}
	cap.read(frame_in);

	// Get properties of the video
	total_frames_in_video=cap.get(cv::CAP_PROP_FRAME_COUNT);
	total_time_in_video = (total_frames_in_video / framerate);
	screenWidth = frame_in.cols;
	screenHeight = frame_in.rows;

	// Init Raylib window
	SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)
	InitWindow(screenWidth, screenHeight, "raylib opencv test3: GUI");
	SetTargetFPS(framerate);

	
	// Convert frames from OpenCV to Raylib

	cv::cvtColor(frame_in, framergb, cv::COLOR_BGR2RGB);
	frame_raylib.width = framergb.cols;
	frame_raylib.height = framergb.rows;
	frame_raylib.format = UNCOMPRESSED_R8G8B8;
	frame_raylib.mipmaps = 1;
	frame_raylib.data = (void*)(framergb.data);
	// Image converted to texture, uploaded to GPU memory (VRAM)
	texture = LoadTextureFromImage(frame_raylib); 


	// Prepare shader
	Shader shader_BLUR = LoadShader(0, FormatText("../external_libs/glsl330/fisheye2.fs"));
	RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
	
	// Remove texture log traces from Raylib
	SetTraceLogLevel(LOG_NONE);

	// Main loop
	while (!WindowShouldClose()) 
	{

		opencv_algorithm();

		// Choose OpenCV frame to render
		switch (selector)
		{
		case 0:
			cv::cvtColor(frame_in, framergb, cv::COLOR_BGR2RGB);
			break;
		case 1:
			cv::cvtColor(frame2_inrange_azules, framergb, cv::COLOR_BGR2RGB);
			break;
		case 2:
			cv::cvtColor(frame3_dilate_azules, framergb, cv::COLOR_BGR2RGB);
			break;
		case 3:
			cv::cvtColor(frame2_inrange_rojos, framergb, cv::COLOR_BGR2RGB);
			break;
		case 4:
			cv::cvtColor(frame3_dilate_rojos, framergb, cv::COLOR_BGR2RGB);
			break;
		default:
			cv::cvtColor(frame_in, framergb, cv::COLOR_BGR2RGB);
			break;
		}

		// Render in Raylib selected OpenCV frame
		frame_raylib.data = (void*)(framergb.data);
		UnloadTexture(texture);
		texture = LoadTextureFromImage(frame_raylib);

		CPU_load = getCPULoad();


		drawOverlays(texture, target, shader_BLUR, vector_of_rectangles_azules, vector_of_rectangles_rojos, total_time_in_video, CPU_load);

		// Release buffers
		frame_in.release();
		frame2_inrange_azules.release();
		frame3_dilate_azules.release();
		frame2_inrange_rojos.release();
		frame3_dilate_rojos.release();
		framergb.release();

		// Caputre new frame
		cap.read(frame_in);
		if (frame_in.empty())
		{
			break;
		}
		
	}

	// De-Initialization
	UnloadTexture(texture);
	CloseWindow();
	cap.release();

	return 0;
}


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: opencv_algorithm()                                                         */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void opencv_algorithm(void)
{

	// DETECCION DE AZULES
	cv::inRange(frame_in, cv::Scalar(azules_min.b, azules_min.g, azules_min.r), cv::Scalar(azules_max.b, azules_max.g, azules_min.r), frame2_inrange_azules);
	cv::dilate(frame2_inrange_azules, frame3_dilate_azules, dilatation_element);
	cv::findContours(frame3_dilate_azules, array_contours_azules, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	vector_of_opencv_rectangles_azules.clear();

	for (int i = 0; i < array_contours_azules.size(); i++)
	{
		aux = boundingRect(array_contours_azules[i]);
		vector_of_opencv_rectangles_azules.push_back(aux);
		vector_of_opencv_rectangles_azules.push_back(aux);
	}
	cv::groupRectangles(vector_of_opencv_rectangles_azules, groupingRectangles_th, groupingRectangles_eps);

	// Generate bounding rectangles in raylib format
	vector_of_rectangles_azules.clear();
	for (int i = 0; i < vector_of_opencv_rectangles_azules.size(); i++)
	{
		aux = vector_of_opencv_rectangles_azules[i];
		aux2 = { (float)aux.x,(float)aux.y,(float)aux.width,(float)aux.height };
		vector_of_rectangles_azules.push_back(aux2);
	}

	// DETECCION DE ROJOS
	cv::inRange(frame_in, cv::Scalar(rojos_min.b, rojos_min.g, rojos_min.r), cv::Scalar(rojos_max.b, rojos_max.g, rojos_max.r), frame2_inrange_rojos);
	cv::dilate(frame2_inrange_rojos, frame3_dilate_rojos, dilatation_element);
	cv::findContours(frame3_dilate_rojos, array_contours_rojos, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	vector_of_opencv_rectangles_rojos.clear();

	for (int i = 0; i < array_contours_rojos.size(); i++)
	{
		aux = boundingRect(array_contours_rojos[i]);
		vector_of_opencv_rectangles_rojos.push_back(aux);
		vector_of_opencv_rectangles_rojos.push_back(aux);
	}
	cv::groupRectangles(vector_of_opencv_rectangles_rojos, groupingRectangles_th, groupingRectangles_eps);

	// Generate bounding rectangles in raylib format
	vector_of_rectangles_rojos.clear();
	for (int i = 0; i < vector_of_opencv_rectangles_rojos.size(); i++)
	{
		aux = vector_of_opencv_rectangles_rojos[i];
		aux2 = { (float)aux.x,(float)aux.y,(float)aux.width,(float)aux.height };
		vector_of_rectangles_rojos.push_back(aux2);
	}
	return;
}



/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: drawOverlays()                                                             */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void drawOverlays(Texture2D texture, RenderTexture2D target,
	Shader shader_BLUR,
	std::vector<Rectangle> vector_of_rectangles_azules,
	std::vector<Rectangle> vector_of_rectangles_rojos,
	double total_time_in_video,
	double CPU_load)
{

	
	Rectangle zone1 = {0,(int)(screenHeight*3/4), screenWidth, screenHeight/4};
	Rectangle zone2 = { (screenWidth * 3 / 4), (screenHeight * 3 / 4), screenWidth/4, screenHeight / 4 };

	Rectangle selector_pos = {zone2.x,zone2.y, 120, 20 };
	Rectangle aperture_pos = {zone2.x,zone2.y + 40, 120, 20 };

	Vector3 text1_pos = { zone2.x, zone2.y + 80, 10 };
	Vector3 text2_pos = { zone2.x, zone2.y +100, 10 };
	Vector3 text3_pos = { screenWidth/2, screenHeight/4, 10 };


	GUI_ring Ring_cpu;

	Ring_cpu.center= { zone1.width / 4, zone1.y + zone1.height / 2 };
	Ring_cpu.innerRadius = zone1.height / 2 - 50.0f;
	Ring_cpu.outerRadius = zone1.height / 2 - 20.0f;
	Ring_cpu.max_value = 100;
	Ring_cpu.color_back = LIGHTGRAY;
	Ring_cpu.color_fore = ORANGE;
	Ring_cpu.title = "CPU LOAD";
	Ring_cpu.suffix="%";
	Ring_cpu.set_value(CPU_load);


	GUI_ring Ring_time;

	Ring_time.center = { zone1.width*2/ 4, zone1.y + zone1.height / 2 };
	Ring_time.innerRadius = zone1.height / 2 - 50.0f;
	Ring_time.outerRadius = zone1.height / 2 - 20.0f;
	Ring_time.max_value = total_time_in_video;
	Ring_time.color_back = LIGHTGRAY;
	Ring_time.color_fore = MAROON;
	Ring_time.title = "TIME";
	Ring_time.suffix = "s";
	Ring_time.set_value(GetTime());


	int apertureLoc = GetShaderLocation(shader_BLUR, "aperture");
	SetShaderValue(shader_BLUR, apertureLoc, &aperture, UNIFORM_FLOAT);

	BeginDrawing();


		ClearBackground(RAYWHITE);

		BeginTextureMode(target);

			DrawTexture(texture, 0, 0, WHITE);

			for (int i = 0; i < vector_of_rectangles_azules.size(); i++)
			{
				DrawRectangleLines(vector_of_rectangles_azules[i].x, vector_of_rectangles_azules[i].y, vector_of_rectangles_azules[i].width, vector_of_rectangles_azules[i].height, SKYBLUE);
			}
			for (int i = 0; i < vector_of_rectangles_rojos.size(); i++)
			{
				DrawRectangleLines(vector_of_rectangles_rojos[i].x, vector_of_rectangles_rojos[i].y, vector_of_rectangles_rojos[i].width, vector_of_rectangles_rojos[i].height, RED);
			}

			DrawText(FormatText("Time: %2.1f /%2.1f [s]", GetTime(), total_time_in_video), text3_pos.x, text3_pos.y, text3_pos.z, RED);


		EndTextureMode();
		
		BeginShaderMode(shader_BLUR);

			Rectangle texture_area = { 0, 0, target.texture.width, -target.texture.height };
			Vector2 texture_pos = { 0, 0 };
			DrawTextureRec(target.texture, texture_area, texture_pos, WHITE);

		EndShaderMode();


		GuiSpinner(selector_pos, &selector, 0, 5, false);
		aperture = GuiSliderBar(aperture_pos, "Fish aperture", aperture, 0, 180, true);

		DrawText(FormatText("Azules = %i", vector_of_rectangles_azules.size()), text1_pos.x, text1_pos.y, text1_pos.z, RAYWHITE);
		DrawText(FormatText("Rojos = %i", vector_of_rectangles_rojos.size()), text2_pos.x, text2_pos.y, text2_pos.z, RAYWHITE);




		Ring_cpu.draw();
		Ring_time.draw();

	EndDrawing();

	return;
}



