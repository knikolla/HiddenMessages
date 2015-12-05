#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <fstream>
#include <iostream>

#define bit_at(a,i) !!(a[i / CHAR_BIT] >> (CHAR_BIT - (i % CHAR_BIT) - 1) & 1)
#define set_bit_at(a, i, x) a ^= (-x ^ a) & (1 << i);

using namespace cv;
using namespace std;

void simple(Mat& source, const char* text);
void simple_decode(Mat& source, int size);

void amplitude_modulation(Mat& source, const char* text);
void decode_amplitude(Mat& source, int size);

int main(int argc, char** argv )
{
    string method = argv[1];
    char* image_source = argv[2];
    char* image_dest = argv[3];
    char* file_location = argv[4];
    
    Mat image;
    image = imread(image_source, IMREAD_COLOR);
    //
    
    if (image.empty())
    {
        cout << "Error loading image!";
        return 1;
    }
    
    std::ifstream in(file_location);
    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    const char* text = contents.c_str();
    
    if (method == "lsb") {
        // Insert and Write
        simple(image, text);
        imwrite(image_dest, image);
        
        // Decode
        Mat hidden;
        hidden = imread(image_dest, IMREAD_UNCHANGED);
        simple_decode(hidden, strlen(text));
    }
    
    if (method == "amp") {
        // Amplitude Modulation
        amplitude_modulation(image, text);
        imwrite(image_dest, image);
        // Decode
        Mat hidden;
        hidden = imread(image_dest, IMREAD_UNCHANGED);
        decode_amplitude(hidden, strlen(text));
    }
    
    // DCT
    //discrete_cosine(image, "Hello World");
    //patchwork(image, "Hello");
    
}

void amplitude_modulation(Mat& source, const char* text)
{
    const int characters = strlen(text);
    const int arraysize = characters * 8;
    int text_bit = 0;
    
    for (int i = 10; i < source.rows - 10; i += 10) {
        for (int j = 10; j < source.cols - 10; j += 10) {
            if (text_bit < arraysize) {
                Vec3b pixel = source.at<Vec3b>(i, j);
                int b = pixel[0];
                int g = pixel[1];
                int r = pixel[2];
                
                int bit = bit_at(text, text_bit);
                float q = 0.23;
                
                float l = (0.299 * r) + (0.587 * g) + (0.114 * b);
                
                //cout << "Luminance: " << l << endl;
                
                int new_b = b + ((2 * bit) - 1)*l*q;
                
                if (new_b > 255) new_b = 255;
                if (new_b < 0) new_b = 0;
                
                //cout << "B: " << b << ", New B: " << new_b << endl;
                
                source.at<Vec3b>(i, j)[0] = new_b;
                
                text_bit++;
            } else { break; }
        }
    }
    
    //imshow("After", source);
    //waitKey();
    
    //decode_amplitude(source, strlen(text));
}

void decode_amplitude(Mat& source, int characters)
{
    const int arraysize = characters * 8;
    int text_bit = 0;
    int c = 4;
    
    uchar* decoded = new uchar[characters];
    
    for (int y = 10; y < source.rows - 10; y += 10) {
        for (int x = 10; x < source.cols - 10; x += 10) {
            if (text_bit < arraysize) {
                int count_x = 0;
                int sum_x = 0;
                for (int i = x - c; i <= x + c; i++) {
                    Vec3b pixel = source.at<Vec3b>(y, i);
                    sum_x += pixel[0];
                    count_x++;
                }
                
                int sum_y = 0;
                int count_y = 0;
                for (int i = y - c; i <= y + c; i++) {
                    Vec3b pixel = source.at<Vec3b>(i, x);
                    sum_y += pixel[0];
                    count_y++;
                }
                
                int b = source.at<Vec3b>(y, x)[0];
                float mult = (1.0 / (4.0 * c));
                float sum = (sum_x + sum_y - (2 * b));
                float prediction = mult * sum;
                
                //cout << "Mult: " << mult << ", Sum: " << sum << endl;
                //cout << "Count X: " << count_x << ", Count Y: " << count_y << endl;
                //cout << "Prediction: " << prediction << endl;
                //cout << "B: " << b << endl;
                
                int bit = 0;
                if (b >= prediction) {
                    bit = 1;
                } else {
                    bit = 0;
                }
                //cout << bit << endl;
                
                int text_char = text_bit / 8;
                int offset = text_bit % 8;
            
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
                    
                decoded[text_char] = set_bit_at(decoded[text_char], fixed_offset, bit);
                text_bit++;
            } else { break; }
        }
    }
    cout << decoded << endl;
}

void simple(Mat& source, const char* text)
{
	const int characters = strlen(text);
	const int arraysize = characters * 8;
    
    cv::cvtColor(source, source, CV_BGR2GRAY);
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