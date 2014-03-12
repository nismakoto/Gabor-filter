#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <fstream>
#include <sstream>
using namespace std;

// Band width
#define Sigma 10.0
#define Sigma_2D 5.0
// Cosin component
#define Lambda 20
#define Lambda_2D 20
// Aspect ratio
#define Gamma 0.5
// Direction
#define Direction 8
// Coordinate width for gabor
#define K1 3
#define K2 3
#define K1_1D 25
#define K1_2D 25
#define K2_2D 25

list<string> split(string str, string delim)
{
  list<string> result;
  int cut_at;
  while ((cut_at = str.find_first_of(delim)) != str.npos) {
    if (cut_at > 0) {
      result.push_back(str.substr(0, cut_at));
    }
    str = str.substr(cut_at + 1);
  }
  if (str.length() > 0) {
    result.push_back(str);
  }
  return result;
}

// Gabor function
double gabor(int x)
{
  double pos_psi = 45;
  // Phase offset
  double psi = 2 * M_PI * pos_psi / 180;

  return (exp(-(pow(x, 2) / pow(Sigma, 2))) / sqrt(2.0 * M_PI * pow(Sigma, 2)) *
          sin(2.0 * M_PI / Lambda * x + psi)) /
         20;
}

// 2D gabor function
double gabor_2d(int x, int y, int Deg)
{
  double X, Y;
  double Rad = Deg * M_PI / 180;
  double pos_psi = 0;
  // Phase offset
  double psi = 2 * M_PI * pos_psi / 180;
  X = (x * cos(Rad)) + (y * sin(Rad));
  Y = (-(x * sin(Rad)) + (y * cos(Rad)));

  return (exp(-(pow(X, 2) + pow(Gamma * Y, 2)) / (2.0 * pow(Sigma_2D, 2))) *
          sin((2.0 * M_PI / Lambda_2D * X) + psi)) /
         20;
}

int main()
{
  ifstream ifs1("test.pgm");
  string buf;
  int width = 0;
  int height = 0;
  int pixel_max = 0;
  int k1, k2, n1, n2;
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;

  while (ifs1 && getline(ifs1, buf)) {
    if (i >= 0 && i < 3) {
      // Split
      list<string> strList = split(buf, " ");
      // Iterator
      list<string>::iterator iter1 = strList.begin();

      j = 0;
      while (iter1 != strList.end()) {
        if (i == 1 && j == 0) {
          // PGM width
          width = atoi(string(*iter1).c_str());
        } else if (i == 1 && j == 1) {
          // PGM height
          height = atoi(string(*iter1).c_str());
        } else if (i == 2) {
          // Max value
          pixel_max = atoi(string(*iter1).c_str());
        }
        ++j;
        ++iter1;
      }
    }
    ++i;
  }

  ifs1.close();

  int **input = new int *[height];
  for (i = 0; i < height; ++i) {
    input[i] = new int[width];
  }

  double **output = new double *[height - K1 * 2];
  for (i = 0; i < height - K1 * 2; ++i) {
    output[i] = new double[width - K2 * 2];
  }

  ifstream ifs2("test.pgm");
  i = 0;
  j = 0;
  k = 0;
  l = 0;

  while (ifs2 && getline(ifs2, buf)) {
    if (i > 2) {
      // Split
      list<string> strList = split(buf, " ");
      // Iterator
      list<string>::iterator iter2 = strList.begin();

      k = 0;
      while (iter2 != strList.end()) {
        if (j < height && k < width) {
          input[j][k] = atoi(string(*iter2).c_str());
        }
        ++k;
        ++iter2;
      }
      ++j;
    }
    ++i;
  }

  ifs2.close();

  for (int deg = 1; deg <= Direction; ++deg) {
    // Normal direction
    int theta = deg * (360 / Direction);
    cout << theta << endl;
    stringstream imagestr;
    imagestr.str("");
    imagestr << "gabor_2d0" << deg << ".pgm" << flush;

    ofstream ofs1(imagestr.str().c_str());
    ofs1 << "P2" << endl;
    ofs1 << width - K2 * 2 << " " << height - K1 * 2 << endl;
    ofs1 << pixel_max << endl;

    // 2D receptive field of simple cells
    for (i = K1; i < height - K1; ++i) {
      for (j = K2; j < width - K2; ++j) {
        for (k = -K1; k <= K1; ++k) {
          for (l = -K2; l <= K2; ++l) {
            output[i - K1][j - K2] +=
                input[i - k][j - l] * gabor_2d(k, l, theta);
          }
        }
      }
    }

    for (i = 0; i < height - K1 * 2; ++i) {
      for (j = 0; j < width - K2 * 2; ++j) {
        ofs1 << abs((int)output[i][j]);
        if (j < width - K2 * 2) {
          ofs1 << " ";
        }
      }
      ofs1 << endl;
    }
    ofs1.close();
  }

  ofstream ofs2("gabor.dat");
  ofstream ofs3("gabor_2d.dat");

  // 1D receptive field of simple cells
  for (k = -K1_1D; k <= K1_1D; k++) {
    ofs2 << k << " " << gabor(k) << endl;
  }

  // 2D receptive field of simple cells
  for (k = -K1_2D; k <= K1_2D; ++k) {
    for (l = -K2_2D; l <= K2_2D; ++l) {
      ofs3 << k << " " << l;
      for (int deg = 1; deg <= Direction; ++deg) {
        //  Normal direction
        int theta = deg * (360 / Direction);
        ofs3 << " " << gabor_2d(k, l, theta);
      }
      ofs3 << endl;
    }
    ofs3 << endl;
  }

  ofs2.close();
  ofs3.close();

  for (i = 0; i < height; ++i) {
    delete[] input[i];
  }
  delete[] input;

  for (i = 0; i < height - K1 * 2; ++i) {
    delete[] output[i];
  }
  delete[] output;

  return 0;
}
