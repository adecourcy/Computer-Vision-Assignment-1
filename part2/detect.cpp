//
// detect.cpp : Detect integrated circuits in printed circuit board (PCB) images.
//
// Based on skeleton code by D. Crandall, Spring 2018
//
// Katherine Spoon
// Alex DeCourcy

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
// Feel free to modify.
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
void  write_detection_image(const string &filename,
                            const vector<DetectedBox> &ics,
                            const SDoublePlane &input)
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

// Convolve an image with a separable convolution kernel
SDoublePlane convolve_separable(const SDoublePlane &input,
                                const SDoublePlane &row_filter,
                                const SDoublePlane &col_filter)
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
          if ((j + filter.cols() <= input.cols()) &&
              (i + filter.rows() <= input.rows()) &&
              i_bound >= 0 &&
              j_bound >= 0) {
            output[i][j] += input[i_bound][j_bound] * filter[k][m];
          } else {
            output[i][j] = input[i][j];
          }
        }
      }
    }
  }
  
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

      /*
        The difference between the various colors should be within a limited amount
        and the colors must be at most a certain amount to not be "white"
      */ 
      if ((max - min < 25.0) &&
          (red_plane[row][col] < 110.0) &&
          (green_plane[row][col] < 110.0) &&
          (blue_plane[row][col] < 110.0)) {

        filtered_plane[row][col] = gray_plane[row][col];
      } else {
        filtered_plane[row][col] = 200.0;
      }
      
    }
  }

  return filtered_plane;

}

// check if a point is an core point. Refer to cluster_filter
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

// check if a point is an edge point. Refer to cluster_filter
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

// Find "clusters" of points using a simplified version of DBScan
SDoublePlane cluster_filter(const SDoublePlane &gray_plane)
{
  SDoublePlane core_points(gray_plane.rows(), gray_plane.cols());

  for (int row = 0; row < gray_plane.rows(); row++) {
    for (int col = 0; col < gray_plane.cols(); col++) {
      core_points[row][col] = 200.0;
    }
  }

  // If a point is surrounded by non-white points, it is a "core" point
  for (int row = 1; row < gray_plane.rows() - 2; row++) {
    for (int col = 1; col < gray_plane.cols() - 2; col++) {
      if ((gray_plane[row][col] != 200.0) &&
          (is_core_point(gray_plane, row, col))) {
        core_points[row][col] = gray_plane[row][col];
      }

    }
  }

  SDoublePlane edge_points(gray_plane.rows(), gray_plane.cols());
  // If a point is next to at least 1 core point, it is an edge point
  for (int row = 1; row < gray_plane.rows() - 1; row++) {
    for (int col = 1; col < gray_plane.cols() - 1; col++) {
      if ((gray_plane[row][col] != 200.0) && 
          (is_edge_point(core_points, row, col))) {
        edge_points[row][col] = gray_plane[row][col];
      }

    }
  }

  // combine core and edge points and return
  for (int row = 1; row < gray_plane.rows() - 1; row++) {
    for (int col = 1; col < gray_plane.cols() - 1; col++) {
      if (edge_points[row][col] != 200.0) {
        core_points[row][col] = gray_plane[row][col];
      }

    }
  }

  return core_points;
}

// The two functions below expand the line by a certain amount in both directions
int get_adjusted_line_start(int start,
                            int line_length,
                            float extend_by)
{
  int new_start = (int) round((float) start - 
                              ((float) line_length * extend_by));

  if (new_start < 0) {
    return 0;
  } else {
    return new_start;
  }
}

int get_adjusted_line_end(int end,
                          int line_length,
                          float extend_by,
                          int image_end)
{
  int new_end = (int) round((float) end + 
                            ((float) line_length * extend_by));

  if (new_end > image_end) {
    return image_end;
  } else {
    return new_end;
  }
}

// Refer to check_if_horz_line for comments
int check_if_vert_line(const SDoublePlane &vert_edges,
                       int start_row,
                       int start_col,
                       int max_consecutive_blanks,
                       int min_line_length,
                       float min_coverage)
{
  int edge_pixels = 0;
  int total_pixels = 0;
  int on_blank = 0;
  float current_pixel = 0.0;

  for (int row = start_row; row < vert_edges.rows(); row++) {
    current_pixel = vert_edges[row][start_col];

    if (current_pixel == 0.0) {
      if (on_blank >= max_consecutive_blanks) {
        break;
      } else {
        on_blank++;
      }
    } else if (on_blank == 0 && current_pixel != 0.0) {
      edge_pixels++;
      total_pixels++;
    } else if (on_blank == 1 && current_pixel != 0.0) {
      on_blank = 0;
      edge_pixels++;
      total_pixels += 2;
    }
  }

  if (total_pixels < min_line_length) {
    return -1;
  }

  float coverage = (float) edge_pixels / (float) total_pixels;

  if (coverage < min_coverage) {
    return -1;
  }

  return total_pixels + start_row;

}

// Refer to the find_horz_lines function for comments
SDoublePlane find_vert_lines(const SDoublePlane &vert_edges,
                             int max_consecutive_blanks,
                             int min_line_length,
                             float extend_by,
                             float min_coverage)
{

  int line_start;
  int line_end;
  int line_length;

  SDoublePlane vert_lines(vert_edges.rows(), vert_edges.cols());

  for (int row = 0; row < vert_lines.rows(); row++) {
    for (int col = 0; col < vert_lines.cols(); col++) {
      vert_lines[row][col] = 0.0;
    }
  }

  for (int col = 0; col < vert_edges.cols(); col++) {
    for (int row = 0; row < vert_edges.rows(); row++) {
      if (vert_edges[row][col] != 0.0) {
        line_end = check_if_vert_line(vert_edges,
                                      row,
                                      col,
                                      max_consecutive_blanks,
                                      min_line_length,
                                      min_coverage);

        if (line_end != -1) {
          line_length = line_end - row;
          line_start = get_adjusted_line_start(row,
                                               line_length,
                                               extend_by);

          line_end = get_adjusted_line_end(line_end,
                                           line_length,
                                           extend_by,
                                           vert_edges.rows() - 1);

          for (int line_row = line_start; line_row < line_end + 1; line_row++) {
            vert_lines[line_row][col] = 200.0;
          }

        }
      }
    }
  }

  return vert_lines;
}

// Given an edge pixel, check if that pixel is the start of a valid line
int check_if_horz_line(const SDoublePlane &horz_edges,
                       int start_row,
                       int start_col,
                       int max_consecutive_blanks,
                       int min_line_length,
                       float min_coverage)
{
  int edge_pixels = 0;
  int total_pixels = 0;
  int on_blank = 0;
  float current_pixel = 0.0;

  /*
    Most of this code was used for checking conditions that were
    later eliminated. This code basically checks for at least 30
    consecutive edge pixels
  */
  for (int col = start_col; col < horz_edges.cols(); col++) {
    current_pixel = horz_edges[start_row][col];

    if (current_pixel == 0.0) {
      if (on_blank >= max_consecutive_blanks) {
        break;
      } else {
        on_blank++;
      }
    } else if (on_blank == 0 && current_pixel != 0.0) {
      edge_pixels++;
      total_pixels++;
    } else if (on_blank == 1 && current_pixel != 0.0) {
      on_blank = 0;
      edge_pixels++;
      total_pixels += 2;
    }
  }

  if (total_pixels < min_line_length) {
    return -1;
  }

  float coverage = (float) edge_pixels / (float) total_pixels;

  if (coverage < min_coverage) {
    return -1;
  }

  return total_pixels + start_col;

}

// Check for horizontal lines
SDoublePlane find_horz_lines(const SDoublePlane &horz_edges,
                             int max_consecutive_blanks,
                             int min_line_length,
                             float extend_by,
                             float min_coverage)
{

  int line_start;
  int line_end;
  int line_length;

  // Make a blank SDoublePlane for our lines
  SDoublePlane horz_lines(horz_edges.rows(), horz_edges.cols());
  for (int row = 0; row < horz_lines.rows(); row++) {
    for (int col = 0; col < horz_lines.cols(); col++) {
      horz_lines[row][col] = 0.0;
    }
  }

  for (int row = 0; row < horz_edges.rows(); row++) {
    for (int col = 0; col < horz_edges.cols(); col++) {
      if (horz_edges[row][col] != 0.0) {
          /*
            If we have found an edge pixel, check if it is a part of
            a valid line. If it is, return the end of that line
          */
        line_end = check_if_horz_line(horz_edges,
                                      row,
                                      col,
                                      max_consecutive_blanks,
                                      min_line_length,
                                      min_coverage);

        // If line_end is -1, this is not a valid line
        if (line_end != -1) {
          line_length = line_end - col;
          // Get an adjusted start and end to the line
          line_start = get_adjusted_line_start(col,
                                               line_length,
                                               extend_by);

          line_end = get_adjusted_line_end(line_end,
                                            line_length,
                                            extend_by,
                                            horz_edges.cols() - 1);

          // write the line to the horizontal line matrix
          for (int line_col = line_start; line_col < line_end + 1; line_col++) {
            horz_lines[row][line_col] = 200.0;
          }

        }
      }
    }
  }

  return horz_lines;
}

/*
  Find lines in our edge filtered image. Lines are defined by
  consecutive edge points on a minimum length
*/
SDoublePlane find_lines(const SDoublePlane &filtered_vert,
                        const SDoublePlane &filtered_horz)
{

  // A line must be 30 consective edge pixels
  int max_consecutive_blanks = 0;
  int min_line_length = 30;

  /*
    We will extend the lines we found by a small amount (10%) in order to 
    find proper intections, as compensation for noise in our image
  */
  float extend_by = 0.10;
  float min_coverage = 0.90;

  // Find the vertical and horizontal lines
  SDoublePlane vert_lines = 
      find_vert_lines(filtered_vert,
                      max_consecutive_blanks,
                      min_line_length,
                      extend_by,
                      min_coverage);

  SDoublePlane horz_lines = 
      find_horz_lines(filtered_horz,
                      max_consecutive_blanks,
                      min_line_length,
                      extend_by,
                      min_coverage);

  // 200 for vert lines, 201 for horz line, 202 for intersection
  SDoublePlane all_lines(vert_lines.rows(),
                         vert_lines.cols());

  for (int row = 0; row < all_lines.rows(); row++) {
    for (int col = 0; col < all_lines.cols(); col++) {
      if (vert_lines[row][col] != 0.0 &&
          horz_lines[row][col] != 0.0) {
        all_lines[row][col] = 202.0;
      } else if (vert_lines[row][col] != 0.0) {
        all_lines[row][col] = 200.0;
      } else if (horz_lines[row][col] != 0.0) {
        all_lines[row][col] = 201.0;
      } else {
        all_lines[row][col] = 0.0;
      }
    }
  }

  return all_lines;
}

class Point
{
    public:
        int row, col;
};


/*
  check if a cluster is a box
  3 conditions:
  1) cluster is > min% filled
  2) cluster size is small enough relative to image size,
     and large enough in general
  3) proportion of height to width is lower than a threshold
*/
vector<DetectedBox> check_if_box(SDoublePlane &filled_mat,
                                 vector<DetectedBox> detected_boxes,
                                 int top_row_index,
                                 int bot_row_index,
                                 int left_col_index, 
                                 int right_col_index) {

  float min_percent_filled = 0.8;
  int max_proportion = 4; // max proportion (to 1) of height:width or width:height

  int filled = 0; // number of filled pixels in the cluster

  // The height and width of our clustered area
  int shape_height = abs(top_row_index - bot_row_index) + 1;
  int shape_width = abs(left_col_index - right_col_index) + 1;

  // check how "filled" the filled matrix is (enough to be a square?)
  for(int i = top_row_index; i < bot_row_index + 1; i++) {
    for(int j = left_col_index; j < right_col_index + 1; j++) {
      if(filled_mat[i][j] == 1.0) {
        filled += 1;
      }
    }
  }

  // The total area of the shape
  int shape_area = shape_height * shape_width;
  float percent_filled = (float) filled / (float) shape_area;

  // If the clustered area is not filled enough, it's not a good enough box
  if (percent_filled < min_percent_filled) {
    return detected_boxes;
  }
  
  // check size of box and dimensions (proportion of height to width)
  int image_area = filled_mat.rows() * filled_mat.cols();
  int min_size = 450;
  int max_size = image_area / 4;
  if (shape_area > max_size ||
      shape_area < min_size ||
      shape_height > max_proportion * shape_width ||
      shape_width > max_proportion * shape_height) {

    return detected_boxes;
  }

  DetectedBox new_box;
  new_box.row = top_row_index;
  new_box.col = left_col_index;
  new_box.width = shape_width;
  new_box.height = shape_height;

  detected_boxes.push_back(new_box);
  return detected_boxes;

} 

/*
  A basic flood fill algorithm
*/
void flood_fill(SDoublePlane &line_boundaries,
                SDoublePlane &cluster,
                int row,
                int col,
                int &top_row,
                int &bottom_row,
                int &left_col,
                int &right_col)
{

  // A matrix of points we have and have not checked
  SDoublePlane checked(cluster.rows(),
                       cluster.cols());
  
  Point current_point;
  current_point.row = row;
  current_point.col = col;

  Point new_point;

  // A psuedo-stack to keep track of points to be filled
  vector<Point> add_points;
  add_points.push_back(current_point);

  top_row = bottom_row = row;
  left_col = right_col = col;

  while (add_points.size() != 0) {
    row = add_points.back().row;
    col = add_points.back().col;
    add_points.pop_back();

    if (line_boundaries[row][col] == 0.0) {

      if (row < top_row) {
        top_row = row;
      } else if (row > bottom_row) {
        bottom_row = row;
      }

      if (col < left_col) {
        left_col = col;
      } else if (col > right_col) {
        right_col = col;
      }

      line_boundaries[row][col] = 1.0;
      cluster[row][col] = 1.0;
      checked[row][col] = 1.0;

      if (row != 0 && checked[row-1][col] != 1.0) {
        checked[row-1][col] = 1.0;
        new_point.row = row - 1;
        new_point.col = col;
        add_points.push_back(new_point);
      }

      if (col != 0 && checked[row][col-1] != 1.0) {
        checked[row][col-1] = 1.0;
        new_point.row = row;
        new_point.col = col-1;
        add_points.push_back(new_point);
      }

      if (row != (cluster.rows() - 1) &&
          checked[row+1][col] != 1.0) {
        checked[row+1][col] = 1.0;
        new_point.row = row+1;
        new_point.col = col;
        add_points.push_back(new_point);
      }

      if (col != (cluster.cols() - 1) &&
          checked[row][col+1] != 1.0) {
        checked[row][col+1] = 1.0;
        new_point.row = row;
        new_point.col = col+1;
        add_points.push_back(new_point);
      }
    }
  }
}

/*
  Use a flood fill algorithm to cluster empty spaces in the boundaries
  in the lines. Send each cluster to a function to check if the box
  meets some acceptable standards to be considered an IC
*/
vector<DetectedBox> flood_fill_cluster(SDoublePlane &line_boundaries)
{
  vector<DetectedBox> detected_boxes;
  int top_row;
  int bottom_row;
  int left_col;
  int right_col;

  for (int row = 0; row < line_boundaries.rows(); row++) {
    for (int col = 0; col < line_boundaries.cols(); col++) {
      if (line_boundaries[row][col] == 0.0) {
        SDoublePlane cluster(line_boundaries.rows(),
                             line_boundaries.cols());

        flood_fill(line_boundaries,
                   cluster,
                   row,
                   col,
                   top_row,
                   bottom_row,
                   left_col,
                   right_col);

        detected_boxes = check_if_box(cluster,
                                      detected_boxes,
                                      top_row,
                                      bottom_row,
                                      left_col,
                                      right_col);

      }
    }
  }

  return detected_boxes;
}

int main(int argc, char *argv[])
{
  if(!(argc == 3))
    {
      cerr << "usage: " << argv[0] << " input_image output_image" << endl;
      return 1;
    }

  string input_filename(argv[1]);
  string output_filename(argv[2]);

  SDoublePlane red_plane;
  SDoublePlane green_plane;
  SDoublePlane blue_plane;

  SDoublePlane gray_plane = SImageIO::read_png_file(input_filename.c_str());
  SImageIO::read_png_file(input_filename.c_str(), red_plane, green_plane, blue_plane);
  
  // We'll use a very aggressive mean filter to blur at first
  SDoublePlane mean_7_7(7, 7);
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 7; col++) {
      mean_7_7[row][col] = 1.0 / 49.0;
    }
  }

  // And a less aggressive gaussian to blur later
  SDoublePlane gaussian_7_7(7, 7);
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 7; col++) {
      int x = row - 3;
      int y = col - 3;

      gaussian_7_7[row][col] =
        (1 / (M_PI * 18)) * exp(0 - (((x * x) + (y * y)) / 18));
    }
  }

  // Blur all colors using the mean filter
  red_plane = convolve_general(red_plane, mean_7_7);
  green_plane = convolve_general(green_plane, mean_7_7);
  blue_plane = convolve_general(blue_plane, mean_7_7);

  /*
    And filter out anything that isn't sufficiently gray, returning a grayscale
    image of the remaining data. This elimates a massive amount of noise.
  */
  SDoublePlane filtered_plane = color_filter(red_plane,
                                             green_plane,
                                             blue_plane,
                                             gray_plane);

  /*
    We'll use a simplified version of DBScan to find clusters of points,
    removing isolated "islands" of points
  */
  filtered_plane = cluster_filter(filtered_plane);

  // Transform the remaining points into a black and white image  
  for (int row = 0; row < filtered_plane.rows(); row++) {
    for (int col = 0; col < filtered_plane.cols(); col++) {
      if (filtered_plane[row][col] != 200.0) {
        filtered_plane[row][col] = 0.0;
      }
    }
  }
  
  SDoublePlane short_vert_edge_filter(3, 3);

  short_vert_edge_filter[0][0] = 1.0;
  short_vert_edge_filter[1][0] = 2.0;
  short_vert_edge_filter[2][0] = 1.0;

  short_vert_edge_filter[0][2] = -1.0;
  short_vert_edge_filter[1][2] = -2.0;
  short_vert_edge_filter[2][2] = -1.0;
  

  SDoublePlane short_horz_edge_filter(3, 3);

  short_horz_edge_filter[0][0] = 1.0;
  short_horz_edge_filter[0][1] = 2.0;
  short_horz_edge_filter[0][2] = 1.0;

  short_horz_edge_filter[2][0] = -1.0;
  short_horz_edge_filter[2][1] = -2.0;
  short_horz_edge_filter[2][2] = -1.0;


  /*
    Apply a vertical and horizontal edge filter to the image independently,
    the combine the resulting edges
  */
  SDoublePlane filtered_vert =
      convolve_general(filtered_plane, short_vert_edge_filter);
  SDoublePlane filtered_horz =
      convolve_general(filtered_plane, short_horz_edge_filter);


  SDoublePlane combined_edges(filtered_plane.rows(),
                              filtered_plane.cols());

  for (int row = 0; row < filtered_plane.rows(); row++) {
    for (int col = 0; col < filtered_plane.cols(); col++) {
      combined_edges[row][col] = 
          filtered_horz[row][col] + filtered_vert[row][col];
    }
  }

  // Apply a gaussing blur to our found edges
  filtered_vert = convolve_general(filtered_vert, gaussian_7_7);
  filtered_horz = convolve_general(filtered_horz, gaussian_7_7);

  // Find lines in our edges
  SDoublePlane all_lines = find_lines(filtered_vert,
                                      filtered_horz);

  // Use a flood fill algorithm to find boxes from our edges
  vector<DetectedBox> detected_boxes =
      flood_fill_cluster(all_lines);

  write_detection_image(output_filename.c_str(),
                        detected_boxes,
                        gray_plane);
  
  
}
