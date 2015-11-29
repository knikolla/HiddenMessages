#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <fstream>
#include <iostream>

#define bit_at(a,i) !!(a[i / CHAR_BIT] >> (CHAR_BIT - (i % CHAR_BIT) - 1) & 1)
#define set_bit_at(a, i, x) a ^= (-x ^ a) & (1 << i);

using namespace cv;
using namespace std;

void simple(Mat& source, const char* file_location);
void simple_decode(Mat& source, int size);

int main(int argc, char** argv )
{
    char* image_source = argv[1];
    char* image_dest = argv[2];
    char* file_location = argv[3];
    
    // Insert
    Mat image;
    image = imread(image_source, IMREAD_COLOR);
    cv::cvtColor(image, image, CV_BGR2GRAY);
    
    std::ifstream in(file_location);
    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    const char* text = contents.c_str();

    if (image.empty())
    {
        cout << "Error loading image!";
        return 1;
    }
    
    simple(image, text);
    imwrite(image_dest, image);
    
    // Decode
    Mat hidden;
    hidden = imread(image_dest, IMREAD_UNCHANGED);
    simple_decode(hidden, strlen(text));
}

void simple(Mat& source, const char* text)
{
	const int characters = strlen(text);
	const int arraysize = characters * 8;
    
    for (int i = 0; i < source.rows; i++)
    {
        for (int j = 0; j < source.cols; j++)
        {
            int text_bit  = ((i * source.cols) + j);
            
        	if (text_bit < arraysize) {
				uchar old_pixel = source.at<uchar>(i, j);
				uchar new_pixel = set_bit_at(old_pixel, 0, bit_at(text, text_bit));
                
				source.at<uchar>(i, j) = new_pixel;
            } else { break; }
        }
    }
}

void simple_decode(Mat& source, int size) {
    int characters = size;
    int arraysize = characters * 8;
    
    uchar* decoded = new uchar[characters];
    
    for (int i = 0; i < source.rows; i++)
    {
        for (int j = 0; j < source.cols; j++)
        {
            int text_char = ((i * source.cols) + j) / 8;
            int text_bit  = ((i * source.cols) + j);
            int offset = ((i * source.cols) + j) % 8;
            
            if (text_bit < arraysize) {
                // This looks ugly, I'll find a clever way
                int fixed_offset = 0;
                switch (offset) {
                    case 0: fixed_offset = 7; break;
                    case 1: fixed_offset = 6; break;
                    case 2: fixed_offset = 5; break;
                    case 3: fixed_offset = 4; break;
                    case 4: fixed_offset = 3; break;
                    case 5: fixed_offset = 2; break;
                    case 6: fixed_offset = 1; break;
                    case 7: fixed_offset = 0; break;
                }
                
                uchar pixel = source.at<uchar>(i, j);
                int bit = pixel & 1;
                decoded[text_char] = set_bit_at(decoded[text_char], fixed_offset, bit);
            } else { break; }
        }
    }
    
    cout << decoded << endl;
}