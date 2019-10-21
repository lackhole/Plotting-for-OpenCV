
#include <fstream>
#include <iostream>
#include <io.h>
#include <direct.h>

#include "plot.h"

#define POINT_INT(x,y) Point((int)(x),(int)(y))

#define WRITE_VAR(v)			outFile.write(	(char *)(&(v)),	sizeof(v)	)
#define WRITE_ARR(arr, size)	outFile.write(	(char *)(arr),	(size)		)
#define WRITE_SCALAR3(s)		WRITE_ARR(((s).val), 4*sizeof(double))

#define READ_VAR(v)				inFile.read(	(char *)(&(v)),	sizeof(v)	)
#define READ_ARR(arr, size)		inFile.read(	(char *)(arr),	(size)		)
#define READ_SCALAR3(s)			READ_ARR(((s).val), 4*sizeof(double))


using namespace cv;
using std::to_string;
using std::_Floating_to_string;
using std::cout;
using std::endl;
using std::cin;
using std::fstream;
using std::ofstream;
using std::ifstream;
using std::ios;

void btnCallBackFunc(int event, int x, int y, int flags, void* userdata);


void btnCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	//Canvas *cptr = &example1;
	Canvas *cptr = (Canvas *)(userdata);

	if (event == EVENT_MOUSEMOVE) {
		int baseline = 0;
		Point2d position = cptr->mouseToPosition(x, y);

		static String maxmousepos = "(" + to_string(cptr->getMaxRange().x) + "," + to_string(cptr->getMaxRange().y) + ")";
		static Size maxputpos = getTextSize(maxmousepos, 1, 1, 1, &baseline);
		static Rect cover = Rect(cptr->image.cols - maxputpos.width, cptr->image.rows - 2.2 * maxputpos.height, maxputpos.width, maxputpos.height*1.5);


		String mousepos = "(--,--)";
		if (cptr->button_zoom.contains(Point(x, y)))
			mousepos = "(" + to_string(position.x) + "," + to_string(position.y) + ")";
		Size putpos = getTextSize(mousepos, 1, 1, 1, &baseline);


		rectangle(cptr->image, cover, WHITE, CV_FILLED);
		putText(cptr->image, mousepos, Point(cptr->image.cols - putpos.width, cptr->image.rows - putpos.height), 1, 1, BLACK);
	}

	if (event == EVENT_LBUTTONDOWN)
	{
		if (cptr->button_save.contains(Point(x, y))) {
			cout << "Save button clicked" << endl;
			cptr->button_save.pressed();
			//cptr->write();
			cptr->save("C:\\users\\cosgenio\\Desktop\\plot\\", "PLOT.plot", OVERWRITE);
		}
		else if (cptr->button_home.contains(Point(x, y))) {
			cout << "Home button clicked" << endl;
			cptr->button_home.pressed();
			cptr->draw_home();
		}
		else if (cptr->button_add.contains(Point(x, y))) {
			double A, power;
			cout << "Add button clicked" << endl;
			cptr->button_add.pressed();

			cout << "y=Ax^-r" << endl;
			cout << "A, r: ";
			cin >> A >> power;
			cout << endl;

			const int w = 500, h = 500;
			double *linex = new double[w*h];
			double *func = new double[w*h];



			for (int i = 0; i < w*h; i++) {
				linex[i] = (double)i;
				func[i] = A * pow(i, -power);
			}

			//cptr->insert_arr(w*h - 1, linex + 1, func, WHITE);

			cptr->draw(w*h - 1, linex + 1, func, WHITE, _Floating_to_string("%.0f", A) + "x^-" + _Floating_to_string("%.2f", power));


		}
		else if (cptr->button_range.contains(Point(x, y))) {

			Pos2D min(0, 0), max(0, 0);
			cout << "Range button clicked" << endl;

			
			cptr->button_range.pressed();
			cout << "X[a,b], Y[a,b] :";
			cin >> min.x >> max.x >> min.y >> max.y;

			cptr->clear();
			cptr->range(min.x, max.x, min.y, max.y);
			cptr->redraw();

		}
		else if (cptr->button_grid.contains(Point(x, y))) {
			static bool toggle = false;
			toggle = !toggle;
			cout << "Grid butto clicked" << endl;
			cptr->clear();
			cptr->grid(toggle);
			cptr->redraw();
		}
	}

	else if (event == EVENT_LBUTTONDBLCLK && cptr->button_zoom.contains(Point(x, y))) {
		cout << "Zoom button clicked" << endl;
		cptr->zoomclk = true;
		cptr->draw_zoom(x, y);
		cptr->zoomclk = false;
	}
	if (event == EVENT_LBUTTONUP)
	{
		cptr->setButtons();
	}

	imshow(cptr->title, cptr->image);
	cv::waitKey(1);
}

Pos2D::Pos2D(double def) : x(def), y(def) {}
Pos2D::Pos2D(double _x, double _y) {
	x = _x; y = _y;
}
void Pos2D::set(double _x, double _y) {
	x = _x; y = _y;
}
void Pos2D::operator=(Pos2D p) {
	x = p.x;
	y = p.y;
}
void Pos2D::operator*=(double n) {
	this->x *= n;
	this->y *= n;
}
void Pos2D::operator+=(double n) {
	this->x += n;
	this->y += n;
}
void Pos2D::operator+=(Pos2D p) {
	x += p.x;
	y += p.y;
}




Canvas::~Canvas() {
	if (is_loaded()) {
		for (int i = 0; i < count_plot; i++) {
			delete[] arr_x[i];
			delete[] arr_y[i];
		}
	}

	delete[] arr_x;
	delete[] arr_y;
	delete[] dots;
	delete[] arr_color;

}
Canvas::Canvas() {
	initializer();
}
Canvas::Canvas(String _title) { 
	title = _title;
	initializer();
}
Canvas::Canvas(int _rows, int _cols) : rows(_rows), cols(_cols) {
	image = Mat3b(rows, cols, Vec3b(255, 255, 255));
	/*
	image.create(_rows, _cols, CV_8UC3);
	image = WHITE;
	*/
	ptr = image.data;
	initializer();
}
Canvas::Canvas(int _rows, int _cols, String _title) : rows(_rows), cols(_cols) {
	image = Mat3b(rows, cols, Vec3b(255, 255, 255));
	/*
	image.create(_rows, _cols, CV_8UC3);
	image = WHITE;
	*/
	ptr = image.data;
	title = _title;
	initializer();
}
void Canvas::initializer() {
	initArrays();
}
void Canvas::initArrays() {
	arr_x = new double*[1000],
	arr_y = new double*[1000];
	dots = new int[1000];
	arr_color = new Scalar[1000];
}
int Canvas::__digit(double num) {
	int temp = TO_INT(num);
	int x = temp > 0 ? 0 : 1;
	while (temp != 0) {
		temp /= 10;
		x++;
	}
	return x;
}
void Canvas::create(int _rows, int _cols) {
	rows = _rows;
	cols = _cols;
	/*
	image.create(_rows, _cols, CV_8UC3);
	image = WHITE;
	*/
	image = Mat3b(rows, cols, Vec3b(255, 255, 255));
	ptr = image.data;
}
void Canvas::create(int _rows, int _cols, String _title) {
	/*
	image.create(_rows, _cols, CV_8UC3);
	image = WHITE;
	*/
	image = Mat3b(_rows, _cols, Vec3b(255, 255, 255));
	rows = _rows;
	cols = _cols;
	ptr = image.data;
	title = _title;
}
void Canvas::clear() {
	image = Vec3b(255, 255, 255);
	isEmpty = true;
	count_legend = 0;
	max_default = 0.01; min_default = 0.01;
	ongrid = false; expandRange = true; rangeSetExternal = false;
}
void Canvas::insert_arr(int ndot, double x[], double y[], Scalar Color) {
	arr_x[count_plot] = x;
	arr_y[count_plot] = y;
	dots[count_plot] = ndot;
	arr_color[count_plot] = Color;
	//if(!is_loaded())
		count_plot++;

	if (count_plot % 1000 == 0) {
		arr_x = (double **)realloc(arr_x, (count_plot + 1000) * sizeof(double *));
		arr_y = (double **)realloc(arr_y, (count_plot + 1000) * sizeof(double *));
		dots = (int *)realloc(dots, (count_plot + 1000) * sizeof(int));
		arr_color = (Scalar *)realloc(dots, (count_plot + 1000) * sizeof(Scalar));
	}
}
void Canvas::draw(int ndot, double x[], double y[], String name) {
	draw(ndot, x, y, Scalar(255, 255, 255), name);
}
void Canvas::draw(int ndot, double x[], double y[], Scalar Color, String name) {
	if (rows == 0 || cols == 0) {
		std::cout << "You must initialize a image first!";
		return;
	}

	if (Color == WHITE) {
		Color = colors20[count_plot];
	}

	if (zoomclk == false) {
		insert_arr(ndot, x, y, Color);
	}


	if (isEmpty) {

		putText(image, title, POINT_INT(cols / 2 - (title.length() - 1) * 5 * font_title_size, pad_up / 2 * rows + 5 * font_title_size), 1, font_title_size, BLACK, 1, 1);

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

		len.x = max.x - min.x;
		len.y = max.y - min.y;

		if (zoomclk == false) {
			min_ori = min;
			max_ori = max;
			len_ori = len;
		}

		// Axis unit text format 
		char formatx[5] = "%.0f", formaty[5] = "%.0f";

		// Set format by ranges
		if (len.x < 10) {
			if (len.x < 0.00001) { formatx[3] = 'e'; }
			else { formatx[2] += __digit(10 / len.x); }
		}
		else if (len.x >= 10000) {
			formatx[2] = '1';
			formatx[3] = 'e';
		}

		if (len.y < 10) {
			if (len.y < 0.00001) { formaty[3] = 'e'; }
			else { formaty[2] += __digit(10 / len.y); }
		}
		else if (len.y >= 10000) {
			formaty[2] = '1';
			formaty[3] = 'e';
		}

		// Draw axis and markings
		
		
		const int __num = 11;
		for (int i = 0; i < __num+1; i++) {

			// Draw grid 
			if (ongrid) {
				line(image, POINT_INT(pad_left*cols, pad_up*rows + (1 - pad_up - pad_down)*i*rows / __num), POINT_INT((1 - pad_right)*cols, pad_up*rows + (1 - pad_up - pad_down)*i*rows / __num), color_grid, 1);
				line(image, POINT_INT(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / __num, (1 - pad_down)*rows), POINT_INT(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / __num, pad_up*rows), color_grid, 1);
			}

			// x-axis markings 
			putText(image, _Floating_to_string(formatx, min.x + len.x / __num * i), POINT_INT(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / __num - 5 * font_mark_size * __digit(min.x + len.x / __num * i), pad_text + (1 - pad_down)*rows), 1, font_mark_size, BLACK, 1, FILLED);
			line(image, POINT_INT(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / __num, (1 - pad_down)*rows + mark_out), POINT_INT(pad_left*cols + ((1 - pad_left - pad_right) * i * cols) / __num, (1 - pad_down)*rows - mark_in), BLACK, 1);

			// y-axis markings 
			putText(image, _Floating_to_string(formaty, max.y - len.y * i / __num), POINT_INT(pad_left*cols - __num * font_mark_size * (font_mark_size + (formaty[2] - '0') + 2), pad_up*rows + (1 - pad_up - pad_down)*i*rows / __num + 5 * font_mark_size), 1, font_mark_size, BLACK, 1, FILLED);
			line(image, POINT_INT(pad_left*cols - mark_out, pad_up*rows + (1 - pad_up - pad_down)*i*rows / __num), POINT_INT(pad_left*cols + mark_in, pad_up*rows + (1 - pad_up - pad_down)*i*rows / __num), BLACK, 1);
		}

		// Draw plot box 
		rectangle(image, POINT_INT(pad_left*cols, pad_up*rows), POINT_INT((1 - pad_right)*cols, (1 - pad_down)*rows), BLACK, 1);

		isEmpty = false;
	}

	// Draw plot 
	for (int i = 0; i < ndot - 1; i++) {
		line(image, POINT_INT((pad_left*cols + (1 - pad_left - pad_right)*cols*(x[i] - min.x) / len.x), (pad_up*rows + (1 - pad_up - pad_down)*rows*(max.y - y[i]) / len.y)), POINT_INT((pad_left*cols + (1 - pad_left - pad_right)*cols*(x[i + 1] - min.x) / len.x), ((pad_up)*rows + (1 - pad_up - pad_down)*rows*(max.y - y[i + 1]) / len.y)), Color, 1, LINE_AA);
	}

	// Write legend 
	if (name.compare("__NO__NAME__") != 0) {
		line(image, POINT_INT((1 - pad_right)*cols + pad_names_rows, pad_up*rows + count_legend * pad_names_cols * 2), POINT_INT(cols - pad_names_rows, pad_up*rows + count_legend * pad_names_cols * 2), Color, 1, LINE_AA);
		putText(image, name, POINT_INT((1 - pad_right)*cols + pad_names_rows, pad_up*rows + count_legend * pad_names_cols * 2 + pad_names_cols), font_name, font_name_size, BLACK, 1, 1);
		count_legend++;
	}



	//waitKey(0); 
}
void Canvas::redraw() {
	setButtons();
	zoomclk = true;
	for (int i = 0; i < count_plot; i++) {
		draw(dots[i], arr_x[i], arr_y[i], arr_color[i]);
	}
	zoomclk = false;
}
void Canvas::draw_zoom(int x, int y) {
	double xL = cols * (1 - pad_left - pad_right), yL = rows * (1 - pad_up - pad_down);
	double xr = x - cols * pad_left, yr = y - rows * pad_up;
	double xp = min.x + (xr / xL)*len.x, yp = max.y - (yr / yL)*len.y;

	//cout << min.x + (xr / xL)*len.x << " : " << max.y - (yr / yL)*len.y << endl;

	clear();
	range(xp - len.x / 2.5, xp + len.x / 2.5, yp - len.y / 2.5, yp + len.y / 2.5);
	//range(0, xp + len.x / 2.5, 0, yp + len.y / 2.5);
	setButtons();

	for (int i = 0; i < count_plot; i++) {
		draw(dots[i], arr_x[i], arr_y[i], arr_color[i]);
	}
}
void Canvas::draw_home() {
	clear();
	range(min_ori.x, max_ori.x, min_ori.y, max_ori.y);
	//range(0, max_ori.x, 0, max_ori.y);
	redraw();
}

// Show axis units 
void Canvas::legend(String type, String unitx, String unity) {
	if (type == "unit") {
		putText(image, unitx, POINT_INT((1 - pad_right)*cols + pad_unit, (1 - pad_down)*rows + 5 * font_legend_size), font_legend, font_legend_size, BLACK);
		putText(image, unity, POINT_INT(pad_left*cols - unity.length() * 5 * font_legend_size, pad_up*rows - 5 * font_legend_size - 5), font_legend, font_legend_size, BLACK);
	}
}

// Set grid setting
void Canvas::grid(bool __true_or_false) {
	ongrid = __true_or_false;
}

// Set range expand setting
void Canvas::expand(bool __true_or_false) {
	expandRange = __true_or_false;
}

// Set range at outside 
void Canvas::range(double xmin, double xmax, double ymin, double ymax) {
	rangeSetExternal = true;
	max.set(xmax, ymax);
	min.set(xmin, ymin);
	len.x = max.x - min.x;
	len.y = max.y - min.y;
}

String Canvas::checkDupName(String path, String name, String _type) {
	struct _finddata_t fd;
	intptr_t handle;
	String type = "", filename = "";

	int i;
	for (i = 0; i < name.length() && name[i] != '.'; i++)
		filename += name[i];
	if (i == name.length() && name[i-1] != '.') {
		name += _type;
	}

	while (i < name.length())
		type += name[i++];



	if ((handle = _findfirst((path + filename + type).c_str(), &fd)) == -1L) {
		_findclose(handle);
		return name;
	}

	int idx = 1;
	String zeros = "000";
	String idx2str = to_string(idx);
	const int digit = zeros.length();

	String fullname = filename + "(" + zeros.substr(0, digit - idx2str.length()) + idx2str + ")" + type;

	handle = _findfirst((path + filename + "*" + type).c_str(), &fd);
	while (_findnext(handle, &fd) == 0) {
		idx++;
		idx2str = to_string(idx);
		fullname = filename + "(" + zeros.substr(0, digit - idx2str.length()) + idx2str + ")" + type;
		if (fullname.compare(fd.name) != 0)
			break;
	}

	_findclose(handle);

	return fullname;
}

// Save image 
void Canvas::write(String path, String name) {

	if (name.compare("Simple_Plot") == 0) {
		name = title;
	}

	String newname = checkDupName(path, name, ".png");
	imwrite(path + newname, image);
	char* s;

	s = _getcwd(NULL, 0);
	cout << (path.length() == 0 ? (s + String("\\")) : path) + newname << endl;

	free(s);
	return;
}

// Save plot data
void Canvas::save(String path, String name, bool overwrite) {

	if (isEmpty) {
		cout << "Undrawn plot cannot be saved!" << endl;
		return;
	}
	
	String newname = overwrite ? name : checkDupName(path, name);

	ofstream outFile((path + newname).c_str(), ios::binary);	

	WRITE_VAR(rows);
	WRITE_VAR(cols);
	WRITE_VAR(count_plot);
	WRITE_VAR(min_ori.x);
	WRITE_VAR(min_ori.y);
	WRITE_VAR(max_ori.x);
	WRITE_VAR(max_ori.y);
	WRITE_VAR(len_ori.x);
	WRITE_VAR(len_ori.y);

	for (int i = 0; i < count_plot; i++) {
		WRITE_VAR(dots[i]);
		WRITE_ARR(arr_x[i], dots[i] * sizeof(double));
		WRITE_ARR(arr_y[i], dots[i] * sizeof(double));
		WRITE_SCALAR3(arr_color[i]);

		cout << "\r(" << i + 1 << " / " << count_plot << ")";
	}

	cout << "\r" << path << newname << endl;

	outFile.close();
}


// Load plot data
void Canvas::load(String path, String name) {
	//name += ".plot";

	ifstream inFile((path +name).c_str(), ios::binary);

	if (!inFile.is_open()) {
		cout << "Load Failed!" << endl;
		return;
	}

	if (!isEmpty) {
		cout << "Unempty canvas cannot load plot files!" << endl;
		return;
	}

	//isEmpty = false;

	READ_VAR(rows);
	READ_VAR(cols);
	READ_VAR(count_plot);
	READ_VAR(min_ori.x);
	READ_VAR(min_ori.y);
	READ_VAR(max_ori.x);
	READ_VAR(max_ori.y);
	READ_VAR(len_ori.x);
	READ_VAR(len_ori.y);

	arr_x = (double **)		realloc(arr_x,		((count_plot / 10) * 10 + 10) * sizeof(double *));
	arr_y = (double **)		realloc(arr_y,		((count_plot / 10) * 10 + 10) * sizeof(double *));
	dots = (int *)			realloc(dots,		((count_plot / 10) * 10 + 10) * sizeof(int)		);
	arr_color = (Scalar *)	realloc(arr_color,	((count_plot / 10) * 10 + 10) * sizeof(Scalar)	);

	for (int i = 0; i < count_plot; i++) {
		READ_VAR(dots[i]);
		arr_x[i] = new double[dots[i]];
		arr_y[i] = new double[dots[i]];
		READ_ARR(arr_x[i], dots[i] * sizeof(double));
		READ_ARR(arr_y[i], dots[i] * sizeof(double));
		READ_SCALAR3(arr_color[i]);
		cout << "\r(" << i + 1 << " / " << count_plot << ")";
	}
	inFile.close();

	this->create(rows, cols);
	for (int i = 0; i < count_plot; i++) {
		redraw();
		//draw(dots[i], arr_x[i], arr_y[i], BLACK);
	}

	loaded = true;

	show();
}

bool Canvas::is_loaded() {
	return loaded;
}

// Show plot
void Canvas::show(cv::MouseCallback onMouse, void *userdata) {
	namedWindow(title);
	if (onMouse == NULL) {
		setMouseCallback(title, btnCallBackFunc, this);
		setButtons();
	}

	imshow(title, image);
	cv::waitKey(0);
}

void Canvas::showOnly() {
	imshow(title, image);
	cv::waitKey(0);
}

// Load image 
void Canvas::getImage(String path, String name, int _count_legend) {
	image = imread(path + name);
	cols = image.cols;
	rows = image.rows;
	ptr = image.data;
	isEmpty = false;
	count_legend = _count_legend;
}

void Canvas::setButtons() {
	if (btnsetup) {
		int shorter = image.cols > image.rows ? image.rows : image.cols;


		button_save = ButtonRect(&image, 2, (int)(shorter*(1 - pad_down / 2)), 50, 50);
		button_save.setText("Save", POINT_INT(button_save.width*0.15, button_save.height*0.6));

		button_add = ButtonRect(&image, 54, (int)(shorter*(1 - pad_down / 2)), 50, 50);
		button_add.setText("Add", POINT_INT(button_save.width*0.2, button_save.height*0.6));

		button_home = ButtonRect(&image, 106, (int)(shorter*(1 - pad_down / 2)), 50, 50);
		button_home.setText("Home", POINT_INT(button_save.width*0.06, button_save.height*0.6));

		button_range = ButtonRect(&image, 158, (int)(shorter*(1 - pad_down / 2)), 50, 50);
		button_range.setText("Range", POINT_INT(button_range.width*0.0, button_range.height*0.6));

		button_grid = ButtonRect(&image, 210, (int)(shorter*(1 - pad_down / 2)), 50, 50);
		button_grid.setText("Grid", POINT_INT(button_save.width*0.15, button_save.height*0.6));

		button_zoom = ButtonRect(&image, (int)(cols*pad_left), (int)(rows*pad_up), (int)(cols*(1 - pad_left - pad_right)), (int)(rows*(1 - pad_up - pad_down)));

		btnsetup = false;
	}

	button_save.unpressed();
	button_home.unpressed();
	button_add.unpressed();
	button_range.unpressed();
	button_grid.unpressed();
}

void Canvas::btnclk(Rect &button) {


}

void Canvas::setPaddings(double up, double down, double left, double right) {

	pad_up = up;
	pad_down = down;
	pad_left = left;
	pad_right = right;
}

Pos2D Canvas::getMaxRange() {
	return max;
}

Pos2D Canvas::getMinRange() {
	return min;
}

Point2d Canvas::mouseToPosition(int x, int y) {
	double xL = cols * (1 - pad_left - pad_right), yL = rows * (1 - pad_up - pad_down);
	double xr = x - cols * pad_left, yr = y - rows * pad_up;
	double xp = min.x + (xr / xL)*len.x, yp = max.y - (yr / yL)*len.y;


	return Point2d(min.x + (xr / xL)*len.x, max.y - (yr / yL)*len.y);
}
