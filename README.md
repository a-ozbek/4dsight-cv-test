# 4dsight-cv-test

## Solution Explanation

I used template matching functionality of OpenCV. To handle rotation, I rotate the small image and look for the angle that gave the maximum response (match). After that I find the location of the maximum match and take that as the coordinates where the small image appears in the big image with that angle.

## Compile

To compile: `g++ find_stars.cpp`
You may need to add these flags to the end: ``pkg-config opencv --cflags --libs``

## Results

![png](./result.png)
![png](./result_rotated.png)