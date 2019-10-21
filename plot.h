#include "opencv2\highgui\highgui.hpp" 
#include "opencv2\core\core.hpp" 
#include "opencv2\imgcodecs\imgcodecs.hpp" 
#include "opencv2\opencv.hpp"


#ifndef PLOT_H
#define PLOT_H

using namespace cv;
using std::to_string;
using std::_Floating_to_string;

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
const Scalar colors20[20] = {
	Scalar(0xF2,0x00,0xFF),
	Scalar(0xF8,0x59,0x00),
	Scalar(0xF2,0x88,0x00),
	Scalar(0xF2,0xAB,0x00),
	Scalar(0xEF,0xCC,0x00),
	Scalar(0xF0,0xEA,0x00),
	Scalar(0xB1,0xD7,0x00),
	Scalar(0x00,0xCA,0x24),
	Scalar(0x00,0xA8,0x77),
	Scalar(0x00,0xA7,0x8A),
	Scalar(0x00,0xA5,0x9C),
	Scalar(0x00,0xA3,0xAC),
	Scalar(0x00,0x93,0xAF),
	Scalar(0x00,0x82,0xB2),
	Scalar(0x00,0x6E,0xBF),
	Scalar(0x7D,0x00,0xF8),
	Scalar(0x9F,0x00,0xC5),
	Scalar(0xB9,0x00,0xA6),
	Scalar(0xD0,0x00,0x81),
	Scalar(0xE2,0x00,0x64)
};
#endif

#define END_DRAW 	 	cv::waitKey(0) 
#define TO_INT(n) 	 	(n > 0 ? (int)(n+0.5) : (int)(n-0.5))
#define OVERWRITE		true

#define CALLBACK_NOTHING 1



struct Pos2D {
	double x, y;
	Pos2D(double def);
	Pos2D(double _x = 0, double _y = 0);
	void set(double _x, double _y);
	void operator=(Pos2D p);
	void operator*=(double n);
	void operator+=(double n);
	void operator+=(Pos2D p);
};

struct ButtonRect {
	Rect rect;
	String text;
	Vec3b color_normal, color_pressed;
	Scalar edge_normal, edge_pressed;
	Mat3b *canvas;
	Point textlocation;
	int height, width;

	ButtonRect() {}

	ButtonRect(Mat3b *canvas, int x, int y, int width, int height, Vec3b col_n = Vec3b(200, 200, 200), Vec3b col_p = Vec3b(230, 230, 230), Scalar edge_n = BLACK, Scalar edge_p = RED) :
		canvas(canvas), height(height), width(width) {
		rect = Rect(x, y, width, height);
		color_normal = col_n;
		color_pressed = col_p;
		edge_normal = edge_n;
		edge_pressed = edge_p;
	}

	void setRect(int x, int y, int width, int height) {
		rect = Rect(x, y, width, height);
	}

	void setColor(Vec3b col_n = Vec3b(200, 200, 200), Vec3b col_p = Vec3b(230, 230, 230),
		int thickness = 1, int lineType = LINE_8,
		bool bottomLeftOrigin = false) {
		color_normal = col_n;
		color_pressed = col_p;
	}

	void setEdge(Scalar edge_n = BLACK, Scalar edge_p = RED) {
		edge_normal = edge_n;
		edge_pressed = edge_p;
	}

	void setText(String _text, Point position) {
		text = _text;
		textlocation = position;
	}

	void unpressed() {
		(*canvas)(rect) = color_normal;
		rectangle(*canvas, rect, edge_normal, 1);
		putText((*canvas)(rect), text, textlocation, FONT_HERSHEY_PLAIN, 1, BLACK);
	}

	void pressed() {
		(*canvas)(rect) = color_pressed;
		rectangle(*canvas, rect, edge_pressed, 1);
		putText((*canvas)(rect), text, textlocation, FONT_HERSHEY_PLAIN, 1, BLACK);
	}

	bool contains(const Point &p) const {
		return rect.contains(p);
	}

};

//Rect button_save;


class Canvas {
private:
	int __digit(double num);

	// Image properties 
	int 	cols = 0, rows = 0;
	uchar	*ptr;
	bool	isEmpty = true;	// plot drawn, not image 
	bool	btnsetup = true;
	bool	loaded = false;

	// plotting properties 
	int 	count_legend = 0, count_plot = 0;
	double  max_default = 0.01, min_default = 0.01;
	Pos2D  	max, min, len, max_ori, min_ori, len_ori;
	bool  	ongrid = false, expandRange = true, rangeSetExternal = false;

	//paddings 
	double 		pad_up = 0.15, pad_left = 0.13, pad_right = 0.17, pad_down = 0.15,		// percentages (1 = 100%)
				pad_text = 20, pad_unit = 10, pad_names_rows = 30, pad_names_cols = 20;	// absolute pixels
	uint 		mark_in = 4, mark_out = 4;

	// font, color properties 
	int 		font_title = 1, font_mark = 1, font_legend = 1, font_name = 1;
	double		font_title_size = 4, font_mark_size = 0.9, font_legend_size = 1, font_name_size = 1;
	Scalar		color_title = BLACK, color_mark = BLACK, color_legend = BLACK, color_grid = Scalar(200, 200, 200);

	// dot array
	double	**arr_x, **arr_y;
	int		*dots;
	Scalar	*arr_color;

public:
	// drawing properties 
	String title = "Simple plot";

	Mat3b 	image;
	// Button
	ButtonRect button_save, button_zoom, button_home, button_add, button_range, button_grid;

	bool zoomclk = false;

	// Constructors
	~Canvas();
	Canvas();
	Canvas(String _title);
	Canvas(int _rows, int _cols);
	Canvas(int _rows, int _cols, String _title);
	void create(int _rows, int _cols);
	void create(int _rows, int _cols, String _title);
	void initializer();
	void initArrays();

	// plot drawing function 
	void draw(int ndot, double x[], double y[], String name);
	void draw(int ndot, double x[], double y[], Scalar Color = WHITE, String name = "__NO__NAME__");
	void insert_arr(int ndot, double x[], double y[], Scalar Color);
	void redraw();
	void draw_zoom(int x, int y);
	void draw_home();

	// Show axis units 
	void legend(String type, String unitx, String unity);

	// Set grid setting
	void grid(bool __true_or_false);

	// Set range expand setting
	void expand(bool __true_or_false);

	// Set range at outside 
	void range(double xmin, double xmax, double ymin, double ymax);

	// Save image 
	void write(String path = "", String name = "Simple_Plot");

	void save(String path = "", String name = "Simple_Plot", bool overwrite = false);

	// Show plot
	void show(cv::MouseCallback onMouse = NULL, void *userdata = (void *)0);

	void showOnly();

	// Clear image
	void clear();

	// Load image 
	void getImage(String path, String name, int _count = 0);

	void load(String path = "", String name = "Simple_Plot");

	bool is_loaded();

	void setButtons();

	void btnclk(Rect &button);

	void setPaddings(double, double, double, double);

	Point2d mouseToPosition(int x, int y);
	Pos2D getMaxRange();
	Pos2D getMinRange();

	String checkDupName(String path = "", String name = "Simple_Plot", String _type = "");
};

#endif
