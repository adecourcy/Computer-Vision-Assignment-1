//
// Watermark.cpp : Add watermark to an image, or inspect if a watermark is present.
//
// Based on skeleton code by D. Crandall, Spring 2018
//
// PUT YOUR NAMES HERE
//
//

//Link to the header file
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <SImage.h>
#include <SImageIO.h>
#include <fft.h>
#include <math.h>

using namespace std;

// This code requires that input be a *square* image, and that each dimension
//  is a power of 2; i.e. that input.width() == input.height() == 2^k, where k
//  is an integer. You'll need to pad out your image (with 0's) first if it's
//  not a square image to begin with. (Padding with 0's has no effect on the FT!)
//
// Forward FFT transform: take input image, and return real and imaginary parts.
//
void fft(const SDoublePlane &input, SDoublePlane &fft_real, SDoublePlane &fft_imag)
{
  fft_real = input;
  fft_imag = SDoublePlane(input.rows(), input.cols());

  FFT_2D(1, fft_real, fft_imag);
}



// Inverse FFT transform: take real and imaginary parts of fourier transform, and return
//  real-valued image.
//
void ifft(const SDoublePlane &input_real, const SDoublePlane &input_imag, SDoublePlane &output_real)
{
  output_real = input_real;
  SDoublePlane output_imag = input_imag;

  FFT_2D(0, output_real, output_imag);
}



SDoublePlane create_padded_image(const SDoublePlane &image)
{
  int numRows = image.rows();
  int numCols = image.cols();
  
  // check if rows and cols are powers of 2 and if not, add extra rows/cols filled with 0s
  bool row_power_of_two = ceil(log2(numRows)) == floor(log2(numRows));
  bool col_power_of_two = ceil(log2(numCols)) == floor(log2(numCols));
  
  SDoublePlane paddedImage(numRows, numCols);

  if (!row_power_of_two) {
    int more_rows = pow(ceil(log2(numRows)), 2) - numRows;
    for (int row = image.rows(); row < more_rows; row++) {
      for (int col = 0; col < numCols; col++) {
        paddedImage[row][col] = 0;
      }
    }
  }

  if (!col_power_of_two) {
    int more_cols = pow(ceil(log2(numCols)), 2) - numCols;
    for (int row = 0; row < numRows; row++) {
      for (int col = image.cols(); col < more_cols; col++) {
        paddedImage[row][col] = 0;
      }
    }
  }

  // check if numRows = numCols and if not, add extra row/cols filled with 0s  
  if (numRows > numCols) {
    numCols = numRows;
  } else if (numCols > numRows) {
    numRows = numCols;
  }

  for (int row = 0; row < image.rows(); row++) {
    for (int col = 0; col < image.cols(); col++) {
      paddedImage[row][col] = image[row][col];
    }
  }

  for (int row = image.rows(); row < numRows; row++) {
    for (int col = image.cols(); col < numCols; col++) {
      paddedImage[row][col] = 0;
    }
  }

  return paddedImage;

}



// Write this in Part 1.1
SDoublePlane fft_magnitude(const SDoublePlane &fft_real, const SDoublePlane &fft_imag)
{
  SDoublePlane magnitude_matrix(fft_real.rows(), fft_real.cols());

  for (int row = 0; row < fft_real.rows(); row++) {
    for (int col = 0; col < fft_real.cols(); col++) {
      magnitude_matrix[row][col] =
          log(sqrt(pow(fft_real[row][col], 2) +
                   pow(fft_imag[row][col], 2)));
    }
  }

  return magnitude_matrix;
}



// Write this in Part 1.2
SDoublePlane remove_interference(const SDoublePlane &input)
{
  SDoublePlane fft_real(input.rows(), input.cols());
  SDoublePlane fft_imag(input.rows(), input.cols());

  fft(input, fft_real, fft_imag);

  fft_real[156][156] = fft_real[156][155];
  fft_real[157][156] = fft_real[157][155];
  fft_real[158][156] = fft_real[158][155];
  fft_real[159][156] = fft_real[159][155];
  fft_real[160][156] = fft_real[160][155];

  fft_real[156][159] = fft_real[156][160];
  fft_real[157][159] = fft_real[157][160];
  fft_real[158][159] = fft_real[158][160];
  fft_real[159][159] = fft_real[159][160];
  fft_real[160][159] = fft_real[160][160];

  fft_real[156][162] = fft_real[156][163];
  fft_real[157][162] = fft_real[157][163];
  fft_real[158][162] = fft_real[158][163];
  fft_real[159][162] = fft_real[159][163];
  fft_real[160][162] = fft_real[160][163];

  fft_real[158][157] = fft_real[159][157];
  fft_real[158][158] = fft_real[159][158];

  fft_real[352][350] = fft_real[352][351];
  fft_real[353][350] = fft_real[353][351];
  fft_real[354][350] = fft_real[354][351];
  fft_real[355][350] = fft_real[355][351];
  fft_real[356][350] = fft_real[356][351];

  fft_real[352][353] = fft_real[352][352];
  fft_real[353][353] = fft_real[353][352];
  fft_real[354][353] = fft_real[354][352];
  fft_real[355][353] = fft_real[355][352];
  fft_real[356][353] = fft_real[356][352];

  fft_real[352][356] = fft_real[352][357];
  fft_real[353][356] = fft_real[353][357];
  fft_real[354][356] = fft_real[354][357];
  fft_real[355][356] = fft_real[355][357];
  fft_real[356][356] = fft_real[356][357];

  fft_real[354][354] = fft_real[355][354];
  fft_real[354][355] = fft_real[355][355];



  fft_imag[156][156] = fft_imag[156][155];
  fft_imag[157][156] = fft_imag[157][155];
  fft_imag[158][156] = fft_imag[158][155];
  fft_imag[159][156] = fft_imag[159][155];
  fft_imag[160][156] = fft_imag[160][155];

  fft_imag[156][159] = fft_imag[156][160];
  fft_imag[157][159] = fft_imag[157][160];
  fft_imag[158][159] = fft_imag[158][160];
  fft_imag[159][159] = fft_imag[159][160];
  fft_imag[160][159] = fft_imag[160][160];

  fft_imag[156][162] = fft_imag[156][163];
  fft_imag[157][162] = fft_imag[157][163];
  fft_imag[158][162] = fft_imag[158][163];
  fft_imag[159][162] = fft_imag[159][163];
  fft_imag[160][162] = fft_imag[160][163];

  fft_imag[158][157] = fft_imag[159][157];
  fft_imag[158][158] = fft_imag[159][158];

  fft_imag[352][350] = fft_imag[352][351];
  fft_imag[353][350] = fft_imag[353][351];
  fft_imag[354][350] = fft_imag[354][351];
  fft_imag[355][350] = fft_imag[355][351];
  fft_imag[356][350] = fft_imag[356][351];

  fft_imag[352][353] = fft_imag[352][352];
  fft_imag[353][353] = fft_imag[353][352];
  fft_imag[354][353] = fft_imag[354][352];
  fft_imag[355][353] = fft_imag[355][352];
  fft_imag[356][353] = fft_imag[356][352];

  fft_imag[352][356] = fft_imag[352][357];
  fft_imag[353][356] = fft_imag[353][357];
  fft_imag[354][356] = fft_imag[354][357];
  fft_imag[355][356] = fft_imag[355][357];
  fft_imag[356][356] = fft_imag[356][357];

  fft_imag[354][354] = fft_imag[355][354];
  fft_imag[354][355] = fft_imag[355][355];


  SDoublePlane cleaned_image(input.rows(), input.cols());
  ifft(fft_real, fft_imag, cleaned_image);

  return cleaned_image;
}



int** get_modified_coordinates(const SDoublePlane &input,
                               int vector_length,
                               int radius)
{
  if ((radius > input.rows()) || (radius > input.cols())) {
    printf("Radius is greater than image size\n");
    exit(0);
  }

  int center_row = input.rows() / 2;
  if (input.rows() % 2 == 1) {
    center_row += 1;
  }

  int center_col = input.cols() / 2;
  if (input.cols() % 2 == 1) {
    center_col += 1;
  }

  // create a matrix to store the coordinates we will modify
  int** modified_coordinates = (int**) malloc(sizeof(int*) * (2 * vector_length));
  for (int i = 0; i < 2 * vector_length; i++) {
    modified_coordinates[i] = (int*) malloc(sizeof(int) * 2);
  }

  
  // We'll just truncate the values
  for (int coord = 0; coord < vector_length; coord++) {

    modified_coordinates[coord][0] = 
        (int) (center_row + radius * 
               cos(M_PI * ((float) (coord + 1) /
                           (float) vector_length)));
    modified_coordinates[coord][1] =
        (int) (center_col + radius *
               sin(M_PI * ((float) (coord + 1) /
                           (float) vector_length)));
                           

    modified_coordinates[coord + vector_length][0] = 
        (int) (center_row + radius *
               cos(M_PI + (M_PI * ((float) (coord + 1) /
                                   (float) vector_length))));
    modified_coordinates[coord + vector_length][1] =
        (int) (center_col + radius *
               sin(M_PI + (M_PI * ((float) (coord + 1) /
                                   (float) vector_length))));

                                   
  }

  return modified_coordinates;

}



// Write this in Part 1.3 -- add watermark N to image
SDoublePlane mark_image(const SDoublePlane &input,
                        int N,
                        int vector_length,
                        int radius,
                        int multiplication_constant)
{
  srand(N);

  SDoublePlane fft_real(input.rows(), input.cols());
  SDoublePlane fft_imag(input.rows(), input.cols());
  SDoublePlane wm_image(input.rows(), input.cols());

  fft(input, fft_real, fft_imag);

  // Generate our binary vector
  int binary_vector[vector_length];
  for (int vec_entry; vec_entry < vector_length; vec_entry++) {
    binary_vector[vec_entry] = rand() % 2;
  }

  // Get the coordinates to modify
  int** modified_coordinates = get_modified_coordinates(input,
                                                        vector_length,
                                                        radius);

  // Modify the coordinates
  int mod_x;
  int mod_y;
  for (int coord = 0; coord < vector_length; coord++) {
    mod_x = modified_coordinates[coord][0];
    mod_y = modified_coordinates[coord][1];
    fft_real[mod_x][mod_y] *= (multiplication_constant * binary_vector[coord]);

    mod_x = modified_coordinates[coord + vector_length][0];
    mod_y = modified_coordinates[coord + vector_length][1];
    fft_real[mod_x][mod_y] *= (multiplication_constant * binary_vector[coord]);
  }


  // Transform back to normal coordinates
  ifft(fft_real, fft_imag, wm_image);

  return wm_image;

}


// Write this in Part 1.3 -- check if watermark N is in image
void check_image(const SDoublePlane &input,
                 int N,
                 int vector_length,
                 int radius,
                 float r_check)
{

  srand(N);

  SDoublePlane fft_real(input.rows(), input.cols());
  SDoublePlane fft_imag(input.rows(), input.cols());
  SDoublePlane wm_image(input.rows(), input.cols());

  fft(input, fft_real, fft_imag);

  // Generate our binary vector
  int binary_vector[vector_length];
  for (int vec_entry; vec_entry < vector_length; vec_entry++) {
    binary_vector[vec_entry] = rand() % 2;
  }

  // Get the coordinates to modify
  int** modified_coordinates = get_modified_coordinates(input,
                                                        vector_length,
                                                        radius);

  float modified_values[vector_length];

  // Get the modified the coordinates
  int mod_x;
  int mod_y;
  for (int coord = 0; coord < vector_length; coord++) {
    mod_x = modified_coordinates[coord][0];
    mod_y = modified_coordinates[coord][1];
    modified_values[coord] = fft_real[mod_x][mod_y];
  }

  // Create a float version of the binary vector to get the mean
  float binary_vector_as_float[vector_length];
  for (int i = 0; i < vector_length; i++) {
    binary_vector_as_float[i] = (float) binary_vector[i];
  }

  // Get means
  float sum = 0;
  for (int i = 0; i < vector_length; i++) {
    sum += modified_values[i];
  }

  float mean_mod_values = (sum / (float) vector_length);

  sum = 0;
  for (int i = 0; i < vector_length; i++) {
    sum += binary_vector_as_float[i];
  }

  float mean_binary_values = (sum / (float) vector_length);


  // Calculate Pearson numerator and denominators
  float numerator = 0;
  float denom_mod_values = 0;
  float denom_binary_values = 0;

  for (int i = 0; i < vector_length; i++) {
    numerator += ((modified_values[i] - mean_mod_values) *
                  (binary_vector_as_float[i] - mean_binary_values));

    // Intermediate denominator values
    denom_mod_values += pow(modified_values[i], 2);
    denom_binary_values += pow(binary_vector_as_float[i], 2);
  }

  // Calculate actual denominator
  float denominator = sqrt(denom_mod_values) * sqrt(denom_binary_values);

  // Calculate R value
  float r_value = numerator / denominator;

  //printf("r_value: %f\n", r_value);

  if (r_value >= r_check) {
    printf("Image has watermark\n");
  } else {
    printf("Image does not have watermark\n");
  }

}



int main(int argc, char **argv)
{
  try {

    if(argc < 4) {
	    cout << "Insufficent number of arguments; correct usage:" << endl;
	    cout << "    p2 problemID inputfile outputfile" << endl;
	    return -1;
    }
    
    string part = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];
    cout << "In: " << inputFile <<"  Out: " << outputFile << endl;
    
    SDoublePlane input_image = SImageIO::read_png_file(inputFile.c_str());
    SDoublePlane padded_image = create_padded_image(input_image);
 

    //const char * outString = outputFile.c_str();
    
    if(part == "1.1") {
      SDoublePlane fft_real(input_image.rows(), input_image.cols());
      SDoublePlane fft_imag(input_image.rows(), input_image.cols());

      fft(padded_image, fft_real, fft_imag);

	    SDoublePlane magnitude_matrix = fft_magnitude(fft_real, fft_imag);
      SImageIO::write_png_file(outputFile.c_str(),
                               magnitude_matrix,
                               magnitude_matrix,
                               magnitude_matrix);
    } else if(part == "1.2") {
      SDoublePlane noise_removed = remove_interference(padded_image);

      SImageIO::write_png_file(outputFile.c_str(),
                               noise_removed,
                               noise_removed,
                               noise_removed);
    } else if(part == "1.3") {
      int vector_length = 5;
      int radius = 100;
      int multiplication_constant = 1; // alpha constant in PDF
      float r_check = 0.3; // R value cutoff for Pearson correlation, "t" in PDF
	    
      if(argc < 6) {
	      cout << "Need 6 parameters for watermark part:" << endl;
	      cout << "    p2 1.3 inputfile outputfile operation N" << endl;
	      return -1;
	    }

      int seed_number = strtol(argv[5], NULL, 10);
	   
      string op = argv[4];
	    if(op == "add") {
	      SDoublePlane transformed_image = mark_image(padded_image,
                                                    seed_number,
                                                    vector_length,
                                                    radius,
                                                    multiplication_constant);
              SImageIO::write_png_file(outputFile.c_str(),
                                 transformed_image,
                                 transformed_image,
                                 transformed_image);
	    } else if(op == "check") {
	      check_image(padded_image,
                    seed_number,
                    vector_length,
                    radius,
                    r_check);
	    } else {
	      throw string("Bad operation!");
      }
       
	     int N = atoi(argv[5]);
    } else {
      throw string("Bad part!");
    }

  } 
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}








