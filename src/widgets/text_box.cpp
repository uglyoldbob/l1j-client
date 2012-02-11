#include "text_box.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_widget.h"

text_box::text_box(int x, int y, int w, int h, client *who)
	: sdl_widget(x, y, who)
{
	num_lines = 0;
	max_lines = h / 12;
	if ((h % 12) > 0)
	{
		max_lines++;
	}
	lines = 0;
	
	width_c = w / 6;
	//num lines tall
	//num characters wide
	
	//first pixel row to display
	//number of lines to display
	
	one = new sdl_graphic(x, y, w, h);
	visible = true;
}

text_box::~text_box()
{
	if (lines != 0)
	{
		for (int i = 0; i < num_lines; i++)
		{
			delete [] lines[i];
		}
		delete [] lines;
	}
}

void text_box::set_visible(int top, int height)
{
	first_visible = top;
	this->height = height;
}

void text_box::redraw()
{
	one->erase();
	for (int i = 0; i < num_lines; i++)
	{
		lineage_font.draw(one->get_surf(), 0, i*12, lines[i], 0xFFFE);
	}
}

void text_box::add_line(char *data)
{
	char **proper_width;
	int number_lines;
	
	int data_length = strlen(data);
	number_lines = (data_length / width_c);
	if ((data_length % width_c) > 0)
	{
		number_lines++;
	}
	
	proper_width = new char*[number_lines];
	for (int i = 0; i < number_lines; i++)
	{
		proper_width[i] = new char[width_c + 1];
		memset(proper_width[i], 0, width_c + 1);
	}
		
	for (int i = 0; i < number_lines; i++)
	{
		strncpy(proper_width[i], &data[i * width_c], width_c);
	}
	
	int extra_lines = (num_lines + number_lines) - max_lines;
	char **new_line_data;
	if (extra_lines > 0)
	{	//toss out the lines at the beginning
		new_line_data = new char*[max_lines];
		int i;
		for (i = 0; i < extra_lines; i++)
		{	//data no longer needed
			if (lines[i] != 0)
				delete [] lines[i];
		}
		for (i = 0; i < (max_lines - extra_lines); i++)
		{
			new_line_data[i] = lines[i + extra_lines];
		}
		for (; i < max_lines; i++)
		{
			new_line_data[i] = proper_width[i+extra_lines-max_lines];
		}
		delete [] lines;
		num_lines = max_lines;
		lines = new_line_data;
	}
	else if (lines != 0)
	{	//add lines to the end
		new_line_data = new char*[num_lines + number_lines];
		int i;
		for (i = 0; i < num_lines; i++)
		{
			new_line_data[i] = lines[i];
		}
		for (; i < (num_lines + number_lines); i++)
		{
			new_line_data[i] = proper_width[i - num_lines];
		}
		delete [] lines;
		num_lines += number_lines;
		lines = new_line_data;
	}
	else
	{
		new_line_data = new char*[num_lines + number_lines];
		int i;
		for (i = 0; i < number_lines; i++)
		{
			new_line_data[i] = proper_width[i];
		}
		num_lines = number_lines;
		lines = new_line_data;
	}
	
	delete [] proper_width;
	
	//put the broken up lines into the lines variable
	//redraw all the visible text
	redraw();
}