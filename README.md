# A-class-for-matrices
SUSTech CS205

use C++ to implement a class.

[Tip] Class cv::Mat is a good example for this project.
https://docs.opencv.org/master/d3/d63/classcv_1_1Mat.html

## Requirements

1. Design a class for matrices, and the class should contain the data of a matrix and related
information such the number of rows, the number of columns, the number of channels, etc.

2. The class should support different data types. It means that the matrix elements can be
unsigned char, short, int, float, double, etc.

3. Do not use memory hard copy if a matrix object is assigned to another. Please carefully handle
the memory management to avoid memory leaks and to release memory multiple times.

4. Implement some frequently used operators including but not limited to =, ==, +, -, *, etc.
Surely the matrix multiplication in Project 4 can be included.

5. Implement region of interest (ROI) to avoid memory hard copy.

6. Test your program on X86 and ARM platforms, and describe the differences.
