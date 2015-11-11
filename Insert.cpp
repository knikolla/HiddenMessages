#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#define bit_at(a,i) !!(a[i / CHAR_BIT] >> (CHAR_BIT - (i % CHAR_BIT) - 1) & 1)
#define set_bit_at(a, i, x) a ^= (-x ^ a) & (1 << i);

using namespace cv;
using namespace std;

void simple(Mat& source, char* text);

int main(int argc, char** argv )
{
	
    Mat image;
    image = imread(argv[1], IMREAD_COLOR);

    cv::cvtColor(image, image, CV_BGR2GRAY);

    if (image.empty())
    {
        cout << "Error loading image!";
        return 1;
    }

    simple(image, argv[3]);
    imwrite(argv[2], image);
}

void simple(Mat& source, char* text)
{
	const int characters = 6;
	const int arraysize = characters * 8;
	

    for (int i = 0; i < source.rows; i++)
    {
        for (int j = 0; j < source.cols; j++)
        {
        	if (i == 0 && j < arraysize) {
				//source.at<uchar>(i, j) = bit_at(text, j);
				uchar n = source.at<uchar>(i, j);
				cout << (int)source.at<uchar>(i, j) << " ";
				n = set_bit_at(n, 0, bit_at(text, j));
				source.at<uchar>(i, j) = n;
				cout << (int)source.at<uchar>(i, j) << endl;
        	}

        }
    }
}