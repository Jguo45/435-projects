#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <math.h>
using namespace std;

const double PI = 3.141592653589793;
const vector<string> KEYWORDS
{
	"background",
	"eyep",
	"lookp",
	"up",
	"fov",
	"screen",
	"surface",
	"sphere"
};

const vector<string> SURFACE_KEYWORDS
{
	"diffuse"
};


class Vector {
public:
	Vector() {
		_x = 0;
		_y = 0;
		_z = 0;
	}

	Vector(double x, double y, double z) {
		_x = x;
		_y = y;
		_z = z;
	}

	double magnitude() {
		return sqrt((_x * _x) + (_y * _y) + (_z * _z));
	}

	Vector normalize() {
		return Vector(_x / magnitude(), _y / magnitude(), _z / magnitude());
	}

	Vector operator- () {
		return Vector(-_x, -_y, -_z);
	}

	Vector operator+ (const Vector& rhs) const {
		return Vector(_x + rhs._x, _y + rhs._y, _z + rhs._z);
	}

	Vector operator- (const Vector& rhs) const {
		return Vector(_x - rhs._x, _y - rhs._y, _z - rhs._z);
	}

	Vector operator* (const double scalar) {
		return Vector(_x * scalar, _y * scalar, _z * scalar);
	}

	double dot(const Vector& rhs) const {
		return _x * rhs._x + _y * rhs._y + _z * rhs._z;
	}

	Vector cross(const Vector& rhs) const {
		return Vector(_y * rhs._z - _z * rhs._y,
			_z * rhs._x - _x * rhs._z,
			_x * rhs._y - _y * rhs._x);
	}

	void printVector() {
		cout << "(" << _x << ", " << _y << ", " << _z << ")" << endl;
		//cout << "X: " << _x << endl
		//	<< "Y: " << _y << endl
		//	<< "Z: " << _z << endl;
	}

private:
	double _x;
	double _y;
	double _z;
};


class RGB {
public:
	RGB() {
		_r = 0;
		_g = 0;
		_b = 0;
	}

	RGB(double r, double g, double b) {
		_r = r;
		_g = g;
		_b = b;
	}

	double getR() {
		return _r;
	}

	double getG() {
		return _g;
	}

	double getB() {
		return _b;
	}

	void print() {
		cout << _r << " " << _g << " " << _b << endl;
	}

private:
	double _r;
	double _g;
	double _b;
};


class Surface {
public:
	Surface() {
		_name = "default";
		_diffuse = RGB();
	}

	Surface(string name) {
		_name = name;
		_diffuse = RGB();
	}

	RGB getDiffuse() {
		return _diffuse;
	}

	void setDiffuse(RGB diffuse) {
		_diffuse = diffuse;
	}

	string getName() {
		return _name;
	}

private:
	string _name;
	RGB _diffuse;
};


class Sphere {
public:
	Sphere() {
		_center = Vector();
		_radius = 1;
	}

	Sphere(Surface surface, Vector center, double r) {
		_surface = surface;
		_center = center;
		_radius = r;
	}

	// Calculates intersection points between ray and sphere
	bool intersect(const Vector e, Vector d, double& t0) {
		Vector g = e - _center;
		double discriminant = (d.dot(g) * d.dot(g)) - (d.dot(d) * (g.dot(g) - (_radius * _radius)));

		// Ray misses the sphere
		if (discriminant < 0) {
			return false;
		}

		// Ray intersects with sphere
		else {
			// Intersects at edge
			if (discriminant == 0) {
				t0 = -d.dot(g) / d.dot(d);
				return true;
			}
			// Intersects through sphere
			else {
				double t1 = (-d.dot(g) + sqrt(discriminant)) / d.dot(d);	// Quadratic formula (add)
				t0 = (-d.dot(g) - sqrt(discriminant)) / d.dot(d);			// Quadratic formula (subtract)

				// Stores smallest positive t value in t0
				if (t1 < t0) {
					double tmp = t0;
					t0 = t1;
					t1 = tmp;
				}
				if (t0 < 0) {
					t0 = t1;
				}

				return true;
			}
		}
	}

	Surface getSurface() {
		return _surface;
	}

	void printSphere() {
		cout << "Surface: " << _surface.getName() << endl;
		cout << "Center: " << endl;
		_center.printVector();
	}

private:
	Vector _center;
	double _radius;

	Surface _surface;
};



class Trace {
public:
	Trace(string file) {
		// Parses .ray file
		readFile(file);

		// Initializees pixel array
		pixels = new unsigned char[width * height * 3];

		// Sets up basis vectors
		setUpBasis();

		// Sets up screen dimensions
		setUpScreen();

		// Determine intersections with rays and objects
		computeRays();

		// Writes to .ppm file
		writePPM();
	}
	void readFile(string file) {
		fstream ppmFile(file);

		string line;
		string surface;
		while (getline(ppmFile, line)) {
			string keyword;
			int index = -1;
			istringstream stream(line);

			if (!isspace(line.front())) {

				stream >> keyword;

				for (int i = 0; i < KEYWORDS.size(); ++i) {
					if (KEYWORDS[i] == keyword) {
						index = i;
						break;
					}
				}
				bool cont = true;
				string input1;
				string input2;
				string input3;

				switch (index)
				{
				case -1:
					break;
					// Set background color
				case 0:
					stream >> input1;
					stream >> input2;
					stream >> input3;

					background = RGB(stod(input1), stod(input2), stod(input3));

					break;
					// Set eyep coords
				case 1:
					stream >> input1;
					stream >> input2;
					stream >> input3;

					eyep = Vector(stod(input1), stod(input2), stod(input3));
					break;
					// Set lookp coords
				case 2:
					stream >> input1;
					stream >> input2;
					stream >> input3;

					lookp = Vector(stod(input1), stod(input2), stod(input3));
					break;
					// Set up vector
				case 3:
					stream >> input1;
					stream >> input2;
					stream >> input3;

					up = Vector(stod(input1), stod(input2), stod(input3));
					break;
					// Set horizontal and vertical fov
				case 4:
					stream >> fovH;
					stream >> fovV;

					fovH = fovH * (PI / 180);
					fovV = fovV * (PI / 180);
					break;
					// Set width and height of screen
				case 5:
					stream >> width;
					stream >> height;
					break;
					// Set surface properties
				case 6:
					stream >> input1;
					surface = input1;

					surfaces[surface] = Surface(surface);

					break;
					// Create sphere object
				case 7:
					string surfaceTitle;
					string radius;

					stream >> surfaceTitle;
					stream >> radius;
					stream >> input1;
					stream >> input2;
					stream >> input3;

					// IMPLEMENT SURFACE
					spheres.push_back(Sphere(surfaces[surfaceTitle], Vector(stod(input1), stod(input2), stod(input3)), stod(radius)));
					break;
				}
			}
			else {
				stream >> keyword;

				for (int i = 0; i < SURFACE_KEYWORDS.size(); ++i) {
					if (SURFACE_KEYWORDS[i] == keyword) {
						index = i;
						break;
					}
				}

				string input1;
				string input2;
				string input3;

				switch (index)
				{
				case 0:
					stream >> input1;
					stream >> input2;
					stream >> input3;

					surfaces[surface].setDiffuse(RGB(stod(input1), stod(input2), stod(input3)));
					break;
				}
			}
		}
	}

	RGB getBackground() {
		return background;
	}

	void setUpBasis() {
		_w = -(eyep - lookp);
		distance = _w.magnitude();
		_w = _w.normalize();

		_u = up.cross(_w).normalize();
		_v = _w.cross(_u);
	}

	void setUpScreen() {
		top = -tan(fovH / 2) * distance * 2;
		bottom = tan(fovH / 2) * distance * 2;

		right = tan(fovV / 2) * distance * 2;
		left = -tan(fovV / 2) * distance * 2;
	}

	void computeRays() {
		int counter = 0;

		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				Vector dir;
				Vector ray;
				double rValue = background.getR();
				double gValue = background.getG();
				double bValue = background.getB();

				double uScalar = left + (right - left) * (j + 0.5) / width;
				double vScalar = top + (bottom - top) * (i + 0.5) / height;

				// Compute direction of viewing ray
				dir = eyep + (_u * uScalar) + (_v * vScalar) - (_w * distance);

				// Checks if there are sphere objects
				if (spheres.size() > 0) {
					double t = 0;
					double tMin = 0;
					int indexMin = -1;

					// Checks viewing ray with each sphere object
					for (int k = 0; k < spheres.size(); ++k) {
						if (spheres[k].intersect(eyep, dir, t)) {
							counter++;

							// Determines which sphere is closest (not working)
							if (k == 0) {
								tMin = t;
								indexMin = 0;
							}
							else {
								if (t < tMin) {
									tMin = t;
									indexMin = k;
								}
							}
						}
					}

					// Gets the color from closest spheres surface
					if (indexMin >= 0) {
						rValue = spheres[indexMin].getSurface().getDiffuse().getR();
						gValue = spheres[indexMin].getSurface().getDiffuse().getG();
						bValue = spheres[indexMin].getSurface().getDiffuse().getB();
					}
				}

				// Writes color data to array
				pixels[i * width * 3 + j * 3 + 0] = 255 * rValue;
				pixels[i * width * 3 + j * 3 + 1] = 255 * gValue;
				pixels[i * width * 3 + j * 3 + 2] = 255 * bValue;
			}
		}
	}

	void writePPM() {
		FILE* ppmfile = fopen("trace.ppm", "wb");
		fprintf(ppmfile, "P6\n");
		fprintf(ppmfile, "%d %d\n", width, height);
		fprintf(ppmfile, "255\n");
		fwrite(pixels, 1, width * height * 3, ppmfile);
		fclose(ppmfile);
	}

	void print() {
		cout << "_u" << endl;
		_u.printVector();
		cout << "_v" << endl;
		_v.printVector();
		cout << "_w" << endl;
		_w.printVector();
		cout << "FOVH: " << fovH << endl;
		cout << "FOVV: " << fovV << endl;
		cout << "Distance: " << distance << endl;
	}
private:
	RGB background;
	Vector eyep;
	Vector lookp;
	Vector up;
	float fovH, fovV;
	int width, height;
	map<string, Surface> surfaces;
	vector<Sphere> spheres;

	Vector _u, _v, _w;
	double right, left, top, bottom;
	double distance;
	unsigned char* pixels;
};



int main() {

	Trace trace("balls-3.ray");

	return 0;
}