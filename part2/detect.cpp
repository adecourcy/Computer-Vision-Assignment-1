//
// detect.cpp : Detect integrated circuits in printed circuit board (PCB) images.
//
// Based on skeleton code by D. Crandall, Spring 2018
//
// PUT YOUR NAMES HERE
//
//

#include <SImage.h>
#include <SImageIO.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// The simple image class is called SDoublePlane, with each pixel represented as
// a double (floating point) type. This means that an SDoublePlane can represent
// values outside the range 0-255, and thus can represent squared gradient magnitudes,
// harris corner scores, etc. 
//
// The SImageIO class supports reading and writing PNG files. It will read in
// a color PNG file, convert it to grayscale, and then return it to you in 
// an SDoublePlane. The values in this SDoublePlane will be in the range [0,255].
//
// To write out an image, call write_png_file(). It takes three separate planes,
// one for each primary color (red, green, blue). To write a grayscale image,
// just pass the same SDoublePlane for all 3 planes. In order to get sensible
// results, the values in the SDoublePlane should be in the range [0,255].
//

// Below is a helper functions that overlays rectangles
// on an image plane for visualization purpose. 

// Draws a rectangle on an image plane, using the specified gray level value and line width.
//
void overlay_rectangle(SDoublePlane &input, int _top, int _left, int _bottom, int _right, double graylevel, int width)
{
  for(int w=-width/2; w<=width/2; w++) {
    int top = _top+w, left = _left+w, right=_right+w, bottom=_bottom+w;

    // if any of the coordinates are out-of-bounds, truncate them 
    top = min( max( top, 0 ), input.rows()-1);
    bottom = min( max( bottom, 0 ), input.rows()-1);
    left = min( max( left, 0 ), input.cols()-1);
    right = min( max( right, 0 ), input.cols()-1);
      
    // draw top and bottom lines
    for(int j=left; j<=right; j++)
    input[top][j] = input[bottom][j] = graylevel;
    // draw left and right lines
    for(int i=top; i<=bottom; i++)
    input[i][left] = input[i][right] = graylevel;
  }
}

// DetectedBox class may be helpful!
//  Feel free to modify.
//
class DetectedBox {
public:
  int row, col, width, height;
  double confidence;
};

// Function that outputs the ascii detection output file
void  write_detection_txt(const string &filename, const vector<DetectedBox> &ics)
{
  ofstream ofs(filename.c_str());

  for(vector<DetectedBox>::const_iterator s=ics.begin(); s != ics.end(); ++s)
    ofs << s->row << " " << s->col << " " << s->width << " " << s->height << " " << s->confidence << endl;
}

// Function that outputs a visualization of detected boxes
void  write_detection_image(const string &filename, const vector<DetectedBox> &ics, const SDoublePlane &input)
{
  SDoublePlane output_planes[3];

  for(int p=0; p<3; p++)
    {
      output_planes[p] = input;
      for(vector<DetectedBox>::const_iterator s=ics.begin(); s != ics.end(); ++s)
  overlay_rectangle(output_planes[p], s->row, s->col, s->row+s->height-1, s->col+s->width-1, p==2?255:0, 2);
    }

  SImageIO::write_png_file(filename.c_str(), output_planes[0], output_planes[1], output_planes[2]);
}


// The rest of these functions are incomplete. These are just suggestions to 
// get you started -- feel free to add extra functions, change function
// parameters, etc.

// Convolve an image with a separable convolution kernel
//
SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
  SDoublePlane output(input.rows(), input.cols());
  SDoublePlane final_output(input.rows(), input.cols());

  for (int i = 0; i < input.rows(); i++) {
    for (int j = 0; j < input.cols(); j++) {
      for (int k = 0; k < row_filter.cols(); k++) {
        int i_bound = i + (k - 1/2);
        int j_bound = j + (k - row_filter.cols() / 2);
        // check if inside image boundary, and if so, do the multiplication
        if (i + row_filter.cols() <= input.rows() && i_bound >= 0 && j_bound >= 0) {
          output[i][j] += input[i_bound][j_bound] * row_filter[k][0];
        }
      }
    }
 }
     
  // now convolve output with the column filter
  for (int l = 0; l < output.rows(); l++) {
    for (int m = 0; m < output.cols(); m++) {
      for (int n = 0; n < col_filter.rows(); n++) {
        int l_bound = l + (n - 1/2);
        int m_bound = m + (n - col_filter.rows() / 2);
        // check if inside image boundary, and if so, do the multiplication
        if (l + col_filter.rows() <= output.rows() && l_bound >= 0 && m_bound >= 0) {
          final_output[l][m] += output[l_bound][m_bound] * col_filter[n][0];
        }
      }
    }
  }
  return final_output;
}

// Convolve an image with a  convolution kernel
//
SDoublePlane convolve_general(const SDoublePlane &input, const SDoublePlane &filter)
{
  SDoublePlane output(input.rows(), input.cols());

  for (int i = 0; i < input.rows(); i++) {
    for (int j = 0; j < input.cols(); j++) {
      for (int k = 0; k < filter.rows(); k++) {
        for (int m = 0; m < filter.cols(); m++) {
          int i_bound = i + (k - filter.rows() / 2);
          int j_bound = j + (m - filter.cols() / 2);

          // check if inside image boundary, and if so, do the multiplication
          if ((j + filter.cols() <= input.cols()) && (i + filter.rows() <= input.rows()) && i_bound >= 0 && j_bound >= 0) {
            output[i][j] += input[i_bound][j_bound] * filter[k][m];
          }
        }
      }
    }
  }
  
  return output;
}


// Apply a sobel operator to an image, returns the result
// 
SDoublePlane sobel_gradient_filter(const SDoublePlane &input, bool _gx)
{
  SDoublePlane output(input.rows(), input.cols());

  // Implement a sobel gradient estimation filter with 1-d filters
  

  return output;
}

// Apply an edge detector to an image, returns the binary edge map
// 
SDoublePlane find_edges(const SDoublePlane &input, double thresh=0)
{
  SDoublePlane output(input.rows(), input.cols());

  // Implement an edge detector of your choice, e.g.
  // use your sobel gradient operator to compute the gradient magnitude and threshold
  
  return output;
}



// Filter out everything that isn't "gray" enough
SDoublePlane color_filter(const SDoublePlane &red_plane,
                          const SDoublePlane &green_plane,
                          const SDoublePlane &blue_plane,
                          const SDoublePlane &gray_plane)
{
  SDoublePlane filtered_plane(gray_plane.rows(), gray_plane.cols());

  double min, max;

  for (int row = 0; row < red_plane.rows(); row++) {
    for (int col = 0; col < red_plane.cols(); col++) {

      if (red_plane[row][col] < green_plane[row][col]) {
        min = red_plane[row][col];
      } else {
        min = green_plane[row][col];
      }

      if (blue_plane[row][col] < min) {
        min = blue_plane[row][col];
      }

      if (red_plane[row][col] > green_plane[row][col]) {
        max = red_plane[row][col];
      } else {
        max = green_plane[row][col];
      }

      if (blue_plane[row][col] > max) {
        max = blue_plane[row][col];
      }

      if ((max - min < 25.0) &&
          (red_plane[row][col] < 130.0) &&
          (green_plane[row][col] < 130.0) &&
          (blue_plane[row][col] < 130.0)) {

        filtered_plane[row][col] = gray_plane[row][col];
      } else {
        filtered_plane[row][col] = 200.0;
      }
      
    }
  }

  return filtered_plane;

}



int is_core_point(const SDoublePlane &gray_plane, int ref_row, int ref_col)
{
  int surrounding_points = 0;

  for (int row = ref_row - 1; row < ref_row + 2; row++) {
    for (int col = ref_col - 1; col < ref_col + 2; col++) {
      if (row == col) {
        continue;
      } else if (gray_plane[row][col] != 200.0) {
        surrounding_points++;
      }
    }
  }

  if (surrounding_points >= 8) {
    return 1;
  } else {
    return 0;
  }
}



int is_semi_core_point(const SDoublePlane &core_points, int ref_row, int ref_col)
{
  int surrounding_points = 0;

  for (int row = ref_row - 1; row < ref_row + 2; row++) {
    for (int col = ref_col - 1; col < ref_col + 2; col++) {
      if (row == col) {
        continue;
      } else if (core_points[row][col] != 200.0) {
        surrounding_points++;
      }
    }
  }

  if (surrounding_points >= 5) {
    return 1;
  } else {
    return 0;
  }
}



int is_edge_point(const SDoublePlane &core_points, int ref_row, int ref_col)
{

  int surrounding_points = 0;

  for (int row = ref_row - 1; row < ref_row + 2; row++) {
    for (int col = ref_col - 1; col < ref_col + 2; col++) {
      if (core_points[row][col] != 200.0) {
        surrounding_points++;
      }
    }
  }

  if (surrounding_points >= 1) {
    return 1;
  } else {
    return 0;
  }

}



// Find "clusters" of points
SDoublePlane cluster_filter(const SDoublePlane &gray_plane)
{
  SDoublePlane core_points(gray_plane.rows(), gray_plane.cols());

  for (int row = 1; row < gray_plane.rows() - 1; row++) {
    for (int col = 1; col < gray_plane.cols() - 1; col++) {
      if ((gray_plane[row][col] != 200.0) &&
          (is_core_point(gray_plane, row, col))) {
        core_points[row][col] = 0.0;
      } else {
        core_points[row][col] = 200.0;
      }

    }
  }

  /*
  int continue_search = 0;

  while (1) {

    for (int row = 1; row < gray_plane.rows() - 1; row++) {
      for (int col = 1; col < gray_plane.cols() - 1; col++) {
        if (core_points[row][col] != 0.0 &&
            is_semi_core_point(core_points, row, col)) {
          continue_search = 1;
          core_points[row][col] = 0.0;
        }

      }
    }


    if (continue_search) {
      continue_search = 0;
    } else {
      break;
    }

  }

  SImageIO::write_png_file("semi_core_points.png",
                           core_points,
                           core_points,
                           core_points);
  exit(0);
  */


  SDoublePlane edge_points(gray_plane.rows(), gray_plane.cols());

  for (int row = 1; row < gray_plane.rows() - 1; row++) {
    for (int col = 1; col < gray_plane.cols() - 1; col++) {
      if ((gray_plane[row][col] != 200.0) && 
          (is_edge_point(core_points, row, col))) {
        edge_points[row][col] = 0.0;
      } else {
        edge_points[row][col] = 200.0;
      }

    }
  }


  for (int row = 1; row < gray_plane.rows() - 1; row++) {
    for (int col = 1; col < gray_plane.cols() - 1; col++) {
      if (edge_points[row][col] != 200.0) {
        core_points[row][col] = 0.0;
      }

    }
  }


  return core_points;

}


//
// This main file just outputs a few test images. You'll want to change it to do 
//  something more interesting!
//
int main(int argc, char *argv[])
{
  if(!(argc == 3))
    {
      cerr << "usage: " << argv[0] << " input_image output_image" << endl;
      return 1;
    }

  string input_filename(argv[1]);
  string output_filename(argv[2]);
  SDoublePlane input_image= SImageIO::read_png_file(input_filename.c_str());
  
  // test step 2 by applying mean filters to the input image
  SDoublePlane mean_filter(3,3);
  for(int i=0; i<3; i++)
    for(int j=0; j<3; j++)
      mean_filter[i][j] = 1/9.0;
  SDoublePlane output_image = convolve_general(input_image, mean_filter);
  //SImageIO::write_png_file(output_filename.c_str(), output_image, output_image, output_image);

  // test separable
  SDoublePlane new_row_filter(3, 3);
  new_row_filter[0][0] = 1;
  new_row_filter[0][1] = 2;
  new_row_filter[0][2] = 1;

  SDoublePlane new_col_filter(3, 3); 
  new_col_filter[0][0] = 1;
  new_col_filter[2][0] = -1; 

  SDoublePlane output_image_2 = convolve_separable(input_image, new_row_filter, new_col_filter);
  SImageIO::write_png_file(output_filename.c_str(), output_image_2, output_image_2, output_image_2);
  
  // randomly generate some detected ics -- you'll want to replace this
  //  with your ic detection code obviously!
  /*
  vector<DetectedBox> ics;
  for(int i=0; i<10; i++)
    {
      DetectedBox s;
      s.row = rand() % input_image.rows();
      s.col = rand() % input_image.cols();
      s.width = 20;
      s.height = 20;
      s.confidence = rand();
      ics.push_back(s);
    }

  write_detection_txt("detected.txt", ics);
  write_detection_image("detected.png", ics, input_image);
  */
}
