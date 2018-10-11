#include "opencv2\highgui\highgui.hpp" 
#include "opencv2\core\core.hpp" 
#include "opencv2\imgcodecs\imgcodecs.hpp" 
#include "opencv2\opencv.hpp"
#include <iostream> 
#include <string> 
#include <time.h> 

#ifndef __COLOR_NAME__
#define __COLOR_NAME__ 
#define WHITE Scalar(255,255,255) 
#define BLACK Scalar(0,0,0) 
#define BLUE Scalar(255,0,0) 
#define GREEN Scalar(0,255,0) 
#define RED Scalar(0,0,255) 
#define YELLOW Scalar(255,255,0) 
#define MAGENTA Scalar(255,0,255) 
#define CYAN Scalar(0,255,255) 
#define PURPLE Scalar(128,0,255) 
#define ORANGE Scalar(255,127,0) 
#define PINK Scalar(255,144,190) 
#define BROWN Scalar(150, 75, 0) 
#define GREY Scalar(128,128,128) 
#define GRASS Scalar(0,153,0) 
#endif 
#define END_DRAW 	 	cv::waitKey(0) 
#define TO_INT(n) 	 	(n > 0 ? (int)(n+0.5) : (int)(n-0.5)) 

using namespace cv;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::_Floating_to_string;
using std::ofstream;



class Canvas {
private:
	struct Pos2D {
		double x, y;
		Pos2D(double def) : x(def), y(def) {}
		Pos2D(double _x = 0, double _y = 0) { x = _x; y = _y; }
		void set(double _x, double _y) { x = _x; y = _y; }
		void operator*=(double n) {
			this->x *= n;
			this->y *= n;
		}
		void operator+=(double n) {
			this->x += n;
			this->y += n;
		}
	};

	int __digit(double num) {
		int temp = TO_INT(num);
		int x = temp > 0 ? 0 : 1;
		while (temp != 0) {
			temp /= 10;
			x++;
		}
		return x;
	}

	// Image properties 
	Mat 	image;
	int 	cols = 0, rows = 0;
	uchar	*ptr;
	bool	isEmpty = true; // plot drawn, not image 

	// plotting properties 
	int 	count = 0;
	double  max_default = 0.01, min_default = 0.01;
	Pos2D  	max, min, len;
	bool  	ongrid = false, expandRange = true, rangeSetExternal = false;

	// drawing properties 
	String title = "Simple plot";
	
	//paddings 
	double 		pad_up = 0.15, pad_left = 0.13, pad_right = 0.17,pad_down = 0.15,		// percentages (1 = 100%)
				pad_text = 20, pad_unit = 10,pad_names_rows = 30, pad_names_cols = 20;	// absolute pixels
	uint 		mark_in = 4, mark_out = 4;

	// font, color properties 
	int 		font_title = 1,			font_mark = 1,			font_legend = 1,		font_name = 1;
	double		font_title_size = 4,	font_mark_size = 0.9,	font_legend_size = 1,	font_name_size = 1;
	Scalar		color_title = BLACK,	color_mark = BLACK,		color_legend = BLACK,	color_grid = Scalar(200, 200, 200);
public:
	// Constructors
	Canvas() {}
	Canvas(String _title) {title = _title;}
	Canvas(int _rows, int _cols) : rows(_rows), cols(_cols) {
		image.create(_rows, _cols, CV_8UC3);
		image = WHITE;
		ptr = image.data;
	}
	Canvas(int _rows, int _cols, String _title) : rows(_rows), cols(_cols) {
		image.create(_rows, _cols, CV_8UC3);
		image = WHITE;
		ptr = image.data;
		title = _title;
	}
	void create(int _rows, int _cols) {
		image.create(_rows, _cols, CV_8UC3);
		rows = _rows;   cols = _cols;   image = WHITE;   ptr = image.data;
	}
	void create(int _rows, int _cols, String _title) {
		image.create(_rows, _cols, CV_8UC3);
		image = WHITE;
		rows = _rows;
		cols = _cols;
		ptr = image.data;
		title = _title;
	}

	// plot drawing function 
	void draw(int ndot, double x[], double y[], Scalar Color, const char* name = "__NO__NAME__") {
		if (rows == 0 || cols == 0) {
			cout << "You must initialize a image first!";
			return;
		}

		if (isEmpty) {

			putText(image, title, Point(cols / 2 - (title.length() - 1) * 5 * font_title_size, pad_up / 2 * rows + 5 * font_title_size), 1, font_title_size, BLACK, 1, 1);

			if (!rangeSetExternal) {

				max.set(x[0], y[0]); min.set(x[0], y[0]);
				for (int i = 1; i < ndot; i++) {
					if (x[i] > max.x) max.x = x[i];
					else if (x[i] < min.x) min.x = x[i];
					if (y[i] > max.y) max.y = y[i];
					else if (y[i] < min.y) min.y = y[i];
				}

				len.set(max.x - min.x, max.y - min.y);

				if (len.x == 0) {
					max.x += max_default;
					min.x -= min_default;
				}
				else if (expandRange) {
					if (max.x > 0 || max.x < -len.x / 10)
						max.x += len.x / 10;
					if (min.x < 0 || min.x  > len.x / 10)
						min.x -= len.x / 10;
				}

				if (len.y == 0) {
					max.y += max_default;
					min.y -= min_default;
				}
				else if (expandRange) {
					if (max.y > 0 || max.y < -len.y / 10)
						max.y += len.y / 10;
					if (min.y < 0 || min.y > len.y / 10)
						min.y -= len.y / 10;
				}
			}
			min.y = 0;
			len.x = max.x - min.x;
			len.y = max.y - min.y;

			// Axis unit text format 
			char formatx[5] = "%.0f", formaty[5] = "%.0f";

			// Set format by ranges
			if (len.x < 10) {
				if (len.x < 0.00001) {	formatx[3] = 'e';					}
				else				{	formatx[2] += __digit(10 / len.x);		}
			}
			else if (len.x >= 10000) {	formatx[2] = '1';
										formatx[3] = 'e';					}

			if (len.y < 10) {
				if (len.y < 0.00001)	{	formaty[3] = 'e';					}
				else				{	formaty[2] += __digit(10 / len.y);		}
			}
			else if (len.y >= 10000) {	formaty[2] = '1';
										formaty[3] = 'e';					}

			// Draw axis and markings
			for (int i = 0; i < 11; i++) { 

				// Draw grid 
				if (ongrid) {
					line(image, Point(pad_left*cols, pad_up*rows + (1 - pad_up - pad_down)*i*rows / 10), Point((1 - pad_right)*cols, pad_up*rows + (1 - pad_up - pad_down)*i*rows / 10), color_grid, 1);
					line(image, Point(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / 10, (1 - pad_down)*rows), Point(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / 10, pad_up*rows), color_grid, 1);
				}

				// x-axis markings 
				putText(image, _Floating_to_string(formatx, min.x + len.x /10 * i), Point(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / 10 - 5 * font_mark_size * __digit(min.x + len.x / 10 * i), pad_text + (1 - pad_down)*rows), 1, font_mark_size, BLACK, 1, FILLED);
				line(image, Point(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / 10, (1 - pad_down)*rows + mark_out), Point(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / 10, (1 - pad_down)*rows - mark_in), BLACK, 1);

				// y-axis markings 
				putText(image, _Floating_to_string(formaty, max.y - len.y * i / 10), Point(pad_left*cols - 10 * font_mark_size * (font_mark_size + (formaty[2] - '0') + 2), pad_up*rows + (1 - pad_up - pad_down)*i*rows / 10 + 5 * font_mark_size), 1, font_mark_size, BLACK, 1, FILLED); 
				line(image, Point(pad_left*cols - mark_out, pad_up*rows + (1 - pad_up - pad_down)*i*rows / 10), Point(pad_left*cols + mark_in, pad_up*rows + (1 - pad_up - pad_down)*i*rows / 10), BLACK, 1);
			}

			// Draw plot box 
			rectangle(image, Point(pad_left*cols, pad_up*rows), Point((1 - pad_right)*cols, (1 - pad_down)*rows), BLACK, 1);

			isEmpty = false;
		}

		// Draw plot 
		for (int i = 0; i < ndot - 1; i++) {
			line(image, Point((pad_left*cols + (1 - pad_left - pad_right)*cols*(x[i] - min.x) / len.x), (pad_up*rows + (1 - pad_up - pad_down)*rows*(max.y - y[i]) / len.y)), Point((pad_left*cols + (1 - pad_left - pad_right)*cols*(x[i + 1] - min.x) / len.x), ((pad_up)*rows + (1 - pad_up - pad_down)*rows*(max.y - y[i + 1]) / len.y)), Color, 1, LINE_AA);
		}

		// Write name 
		if (strcmp(name, "__NO__NAME__") != 0) {
			line(image, Point((1 - pad_right)*cols + pad_names_rows, pad_up*rows + count * pad_names_cols * 2), Point(cols - pad_names_rows, pad_up*rows + count * pad_names_cols * 2), Color, 1, LINE_AA);
			putText(image, name, Point((1 - pad_right)*cols + pad_names_rows, pad_up*rows + count * pad_names_cols * 2 + pad_names_cols), font_name, font_name_size, BLACK, 1, 1);
			count++;
		}
		//waitKey(0); 
	}

	// Show axis units 
	void legend(String type, String unitx, String unity) {
		if (type == "unit") {
			putText(image, unitx, Point((1 - pad_right)*cols + pad_unit, (1 - pad_down)*rows + 5 * font_legend_size), font_legend, font_legend_size, BLACK);
			putText(image, unity, Point(pad_left*cols - unity.length() * 5 * font_legend_size, pad_up*rows - 5 * font_legend_size - 5), font_legend, font_legend_size, BLACK);
		}
	}

	// Set grid setting
	void grid(bool __true_or_false)		{ ongrid = __true_or_false;			}

	// Set range expand setting
	void expand(bool __true_or_false)	{ expandRange = __true_or_false;	}

	// Set range at outside 
	void range(double xmin, double xmax, double ymin, double ymax) {
		rangeSetExternal = true;
		max.set(xmax, ymax);
		min.set(xmin, ymin);
		len.x = max.x - min.x;
		len.y = max.y - min.y;
	}

	// Save image 
	void write(String path, String name) {
		imwrite(path + name, image);
	}

	// Show plot
	void show() {
		imshow(title, image); 
		cv::waitKey(0);
	}

	// Load image 
	void getImage(String path, String name, int _count = 0) {
		image = imread(path + name);
		cols = image.cols;
		rows = image.rows;
		ptr = image.data;
		isEmpty = false;
		count = _count;
	}
};
