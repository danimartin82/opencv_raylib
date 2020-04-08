#pragma once
#include <stdio.h>


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Class: GUI_ring()                                                                    */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
class GUI_ring {

public:
	Vector2 center;
	float innerRadius;
	float outerRadius;
	int endAngle;
	Color color_back;
	Color color_fore;
	double value_a;
	double max_value;
	const char* title;
	const char* suffix;

	void GUIRing(void)
	{

		center = { 0,0 };
		innerRadius = 0;
		outerRadius = 0;
		color_back = WHITE;
		color_fore = WHITE;
		max_value = 360;

	}
	void set_value(double value_r)
	{
		
		endAngle = value_r * 360 / max_value;
		value_a = value_r;

	}

	void draw(void)
	{
		DrawRing(center, innerRadius, outerRadius, 0, 360, 0, color_back);
		DrawRing(center, innerRadius, outerRadius, 0, endAngle, 0, color_fore);
		DrawRingLines(center, innerRadius, outerRadius, 0, 360, 0, color_fore);
		
		DrawText(title, center.x - 25, center.y - outerRadius - 10, 10, color_fore);
		
		const char* a = TextFormat("%.1f", value_a);
		char* b = (char*)a;
		int pos= TextLength(a);
		TextAppend(b,suffix,&pos);
		DrawText(a, center.x - 10, center.y - 5, 10, color_fore);

	}
};

