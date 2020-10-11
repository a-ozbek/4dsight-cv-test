#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
using namespace cv;

Mat rotate_im(Mat im, double angle)
{
    Mat im_rot;     
    Point2f pt(im.cols/2., im.rows/2.);             
    Mat r = getRotationMatrix2D(pt, angle * -1, 1.0);

    double cos = abs(r.at<double>(0, 0));
    double sin = abs(r.at<double>(0, 1));

    int nW = int((im.rows * sin) + (im.cols * cos));
    int nH = int((im.rows * cos) + (im.cols * sin));

    r.at<double>(0, 2) += (nW / 2) - int(im.cols / 2);
    r.at<double>(1, 2) += (nH / 2) - int(im.rows / 2);

    warpAffine(im, im_rot, r, Size(nW, nH));
    return im_rot;
}


int main(){
    // User input
    string f_im;
    string f_im_small;
    int user_wants_visual_output;
    string f_visual_output;
    //
    cout << "Please enter the file path of small image: ";
    cin >> f_im_small;
    cout << "Please enter the file path of big image (Star Map): ";
    cin >> f_im;
    cout << "Do you want to create a visual output image (bounding box detection)? 1 for yes, 0 for no: ";
    cin >> user_wants_visual_output;
    if (user_wants_visual_output == 1){
        cout << "Please enter the filename to create visual output image file: ";
        cin >> f_visual_output;
    }

    // Read images
    Mat im = imread(f_im, CV_LOAD_IMAGE_GRAYSCALE);
    Mat im_small = imread(f_im_small, CV_LOAD_IMAGE_GRAYSCALE);
    
    // Process
    Mat im_query = im_small;
    Mat im_rot;
    double min_val; double max_val;  Point min_loc; Point max_loc;
    int max_match_angle=-1;
    double max_match_value_so_far = -1;
    Point max_loc_so_far;
    int query_rows_when_max = -1;
    int query_cols_when_max = -1;
    for (int angle=0; angle<360; angle++){  
        if (angle % 30 == 0){
            cout << "Progress: " << angle << "/360" << endl;
        }      
        
        // rotate
        im_rot = rotate_im(im_query, angle);  
        // cout << im_rot.rows << "," << im_rot.cols << endl;

        // do template matching
        Mat match_result;
        matchTemplate(im, im_rot, match_result, CV_TM_CCOEFF_NORMED);
    
        // check if greater
        minMaxLoc(match_result, &min_val, &max_val, &min_loc, &max_loc);
        if (max_val > max_match_value_so_far){
            max_match_value_so_far = max_val;
            max_loc_so_far = max_loc;
            max_match_angle = angle;
            query_rows_when_max = im_rot.rows;
            query_cols_when_max = im_rot.cols;
        }
    }

    // do rotation for the rectangle
    RotatedRect rotated_rectangle = RotatedRect(
        Point(max_loc_so_far.x + int(query_cols_when_max / 2), max_loc_so_far.y + int(query_rows_when_max / 2)), 
        Size2f(im_query.cols, im_query.rows), 
        max_match_angle);
    Point2f vertices2f[4];
    rotated_rectangle.points(vertices2f);

    // Convert vertices2f to vertices
    Point vertices[4];    
    for(int i = 0; i < 4; ++i){
        vertices[i] = vertices2f[i];
    }

    // output    
    cout << "Corner Points (x, y) (Origin is top-left):" << endl;
    for (int i=0; i<4; i++){
        cout << vertices[i] << endl;
    }

    // visualized output
    if (user_wants_visual_output == 1){
        Mat im_result = imread(f_im); 
        for (int i=0; i<4; i++){
            line(im_result, vertices[i], vertices[(i+1) % 4], Scalar(0,0,255), 2);
        }
    imwrite(f_visual_output, im_result);
    }    
}
