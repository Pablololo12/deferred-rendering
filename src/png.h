#pragma once

#include <vector>
#include <string>

using namespace std;

class PNG
{
private:
	int	w,h;
	vector<float> px;

public:
	PNG();
	PNG(const string& name);

	void load(const string& name);
	int width() const { return w; }
	int height() const { return h; }
	int size() const { return w*h; }
	const vector<float>& pixels() const { return px; };
};

void save_image_vector(int, int, const char *, float*);
void save_image_vector_2(int, int, const char *, unsigned char*);
void save_image_vector_3(int, int, const char *, unsigned char*);
