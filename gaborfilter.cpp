#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <sstream>
using namespace std;

#define Alfa 1.0
#define Beta 5.0
#define Sigma 10.0
#define Sigma_2D 5.0
#define Lambda 20
#define Lambda_2D 10
#define Gamma 0.5
#define Direction 8 //方向
//#define Deg 45
//#define Rad Deg*M_PI/180
#define Start 3
#define k1_min -3
#define k2_min -3
#define k1_max 4
#define k2_max 4
#define k1_min_1D -25
#define k1_max_1D 25
#define k1_min_2D -25
#define k1_max_2D 25
#define k2_min_2D -25
#define k2_max_2D 25

int k1, k2, n1, n2;

/**
 * split関数
 * @param string str 分割したい文字列
 * @param string delim デリミタ
 * @return list<string> 分割された文字列
 */
list<string> split(string str, string delim)
{
        list<string> result;
        int cutAt;
        while( (cutAt = str.find_first_of(delim)) != str.npos )
        {
                if(cutAt > 0)
                {
                        result.push_back(str.substr(0, cutAt));
                }
                str = str.substr(cutAt + 1);
        }
        if(str.length() > 0)
        {
                result.push_back(str);
        }
        return result;
}

double gabo(int x)
{
        //double X, Y = 0.0;
        double pos_psi = 45;
        double psi = 2*M_PI*pos_psi/180;
        //X = (x*cos(Rad))+(y*sin(Rad));
        //Y = (-(x*sin(Rad))+(y*cos(Rad)));
        //return -exp(-((pow(X,2.0)+(pow(Gamma,2.0)*pow(Y,2.0))))/(2.0*pow(Sigma,2.0)))*cos((2*M_PI*(X/Lambda))+psi)/3;
        //return exp( -(pow(X,2.0)+pow(Y,2.0))/(2*var))*cos(w*X*cos(phase)+w*Y*sin(phase)+psi);
        //return exp(-(pow(X,2.0)+0.025*pow(Y,2.0))/pow(Sigma,2.0))*cos(2*M_PI*pow(X,2.0)/Lambda+psi);

        //double N = exp( ( -pow(X,2) - pow(Gamma*Y,2) ) / (2*pow(Sigma,2)) );
        //double H = cos( 2*M_PI*X/Lambda + psi );

        //double G = N*H;
        //return G;
        return exp( - (pow(x,2) / pow(Sigma,2))) / sqrt(2.0 * M_PI * pow(Sigma,2)) * sin(2.0 * M_PI/Lambda * x + psi);
}

double gabo_2D(int x, int y, int Deg)
{
        double X, Y = 0.0;
        double Rad = Deg*M_PI/180;
        double pos_psi = 0;
        double psi = 2*M_PI*pos_psi/180;
        X = (x*cos(Rad))+(y*sin(Rad));
        Y = (-(x*sin(Rad))+(y*cos(Rad)));
        return exp( -( pow(X,2) + pow(Gamma*Y,2)) / (2.0*pow(Sigma_2D,2))) * sin((2.0 * M_PI/Lambda_2D * X) + psi);
}

void make_rbc()
{
        ofstream ofs("image03.pgm");
        ofs << "P2" << endl;
        ofs << "50" << " " << "50" << endl;
        ofs << "255" << endl;

        for(int i = 0; i < 50; i++){
                for(int j = 0; j < 50; j++)
                {
                        if(j < 25) ofs << "128";
                        if(j >= 25) ofs << "64";
                        if(j != 49){
                                ofs << " ";
                        }
                }
                ofs << endl;
        }

}

int main( )
{
        make_rbc();
        ifstream ifs1("image03.pgm");
        string buf;
        int bit_width = 0;
        int bit_height = 0;
        int bit_max = 0;
        int i = 0;
        int j = 0;
        int k = 0;
        int l = 0;

        while(ifs1 && getline(ifs1, buf))
        {
                if(i >= 0 && i < 3)
                {
                        // splitを実行します。
                        list<string> strList = split(buf, " ");

                        // イテレータを取得します。
                        list<string>::iterator iter1 = strList.begin();

                        j = 0;
                        // パースする
                        while( iter1 != strList.end() ) // 最後まで
                        {
                                if(i == 1 && j == 0)
                                {
                                        bit_width = atoi(string(*iter1).c_str());
                                        //cout << bit_width << endl;
                                }
                                else if(i == 1 && j == 1)
                                {
                                        bit_height = atoi(string(*iter1).c_str());
                                        //cout << bit_height << endl;
                                }
                                else if(i == 2)
                                {
                                        bit_max = atoi(string(*iter1).c_str());
                                        //cout << bit_max << endl;
                                }

                                //cout << *iter << endl;
                                j++;
                                // １つ後方に進む
                                ++iter1; 
                        }
                }
                i++;
        }

        i = 0;
        // 領域の動的確保
        int **input = new int*[bit_height+3];  // 行を作る      
        for(i = 0; i < bit_height+3; i++)    // 列を作る
                input[i] = new int[bit_width+3];
        i = 0;
        // 領域の動的確保
        double **output = new double*[bit_height+3];  // 行を作る      
        for(i = 0; i < bit_height+3; i++)    // 列を作る
                output[i] = new double[bit_width+3];

        int **input_ptr = input+3;

        double output2[bit_height][bit_width];

        // 領域の動的確保 malloc残骸
        //int **input = (int **)malloc(sizeof(int *)*(bit_height+5));  // 行を作る      
        //for(i = 0; i < bit_height+5; i++)    // 列を作る
        //        input[i] = (int *)malloc(sizeof(int *)*(bit_width+5));
        //
        //int **input = (int **)malloc(sizeof(int *)*(bit_height+5));  // 行を作る      
        //int *input2 = (int *)malloc(sizeof(int *)*(bit_width+5)*(bit_height+5));
        //for(i = 0; i < bit_height+5; i++)    // 列を作る
        //        input[i] = input2 + i * bit_width;

        ifstream ifs2("image03.pgm");
        i = 0;
        j = 0;
        k = 0;
        l = 0;

        while(ifs2 && getline(ifs2, buf))
        {
                if(i > 2)
                {

                        //cout << buf << endl;

                        // splitを実行します。
                        list<string> strList = split(buf, " ");

                        // イテレータ
                        // を取得します。
                        list<string>::iterator iter2 = strList.begin(); 

                        k = 0;
                        // パースする
                        while( iter2 != strList.end() ) // 最後まで
                        {
                                input[j][k] = atoi(string(*iter2).c_str());
                                //cout << input[j][k] << endl;
                                // １つ後方に進む
                                ++iter2;
                                k++;
                        }

                }
                i++;
                j++;
        }

        int output_bit_height = bit_height-k1_max*2+1;
        int output_bit_width = bit_width-k2_max*2+1;

        //string[] imagelist = new string[] {"gabo_2Dimage01.pgm", "gabo_2Dimage02.pgm", "gabo_2Dimage03.pgm", "gabo_2Dimage04.pgm", "gabo_2Dimage05.pgm", "gabo_2Dimage06.pgm", "gabo_2Dimage07.pgm", "gabo_2Dimage08.pgm"};

        //string[] datlist = new string[] {"gabo_2Ddata01.dat", "gabo_2Ddata02.dat", "gabo_2Ddata03.dat", "gabo_2Ddata04.dat", "gabo_2Ddata05.dat", "gabo_data_2D.dat", "gabo_data_2D.dat", "gabo_data_2D01.dat"};

        for(int deg=1; deg <= Direction; deg++)
        {
                int theta =  deg*(360/Direction);
                cout << theta << endl;
                stringstream imagestr;
                imagestr.str("");
                imagestr << "gabo_2Dimage0" << deg << ".pgm" << flush;

                ofstream ofs1(imagestr.str().c_str());
                ofs1 << "P2" << endl;
                ofs1 << output_bit_width << " " << output_bit_height << endl;
                ofs1 << bit_max << endl;

                for(i=Start; i<bit_height-(Start+1); i++)
                {
                        for(j=Start; j<bit_width-(Start+1); j++)
                        {
                                for(k=k1_min; k<k1_max; k++)
                                {
                                        for(l=k2_min; l<k2_max; l++)
                                        {
                                                output[i][j] += input_ptr[i-k][j-l]*gabo_2D(k, l, theta);
                                        }
                                }
                                //cout << (int)output[i][j] << " ";
                                //cout << input_ptr[i][j] << endl;
                        }
                        //cout << endl;
                }

                for(i=Start; i<bit_height-(Start+1); i++)
                {
                        for(j=Start; j<bit_height-(Start+1); j++)
                        {
                                ofs1 << abs((int)output[i][j]);
                                if(j != bit_width-5){
                                        ofs1 << " ";
                                }
                        }
                        ofs1 << endl;
                }
        }

        ofstream ofs2("gabo_data.dat");
        ofstream ofs3("gabo_data_2D.dat");

        for(k=k1_min_1D; k<=k1_max_1D; k++)
        {
                ofs2 << k;
                for(int deg=1; deg <= Direction; deg++)
                {
                        int theta =  deg*(360/Direction);
                        ofs2 << " " << gabo(k);
                }
                ofs2 << endl;
        }

        for(k=k1_min_2D; k<=k1_max_2D; k++)
        {
                for(l=k2_min_2D; l<=k2_max_2D; l++)
                {
                        ofs3 << k << " " << l;
                        for(int deg=1; deg <= Direction; deg++)
                        {
                                int theta = deg*(360/Direction);
                                ofs3 << " " << gabo_2D(k, l, theta);
                        }
                        ofs3 << endl; 
                }
                ofs3 << endl;
        }

        //領域の解放
        for(i = 0; i < bit_height+3; i++)
        {
                delete [] input[i];  // 列を解放
        }
        delete [] input;         // 行を解放

        //領域の解放
        for(i = 0; i < bit_height+3; i++)
        {
                delete [] output[i];  // 列を解放
        }
        delete [] output;         // 行を解放

        //for (i=0;i<bit_height+5;i++)
        //        free(input[i]);        
        //free(input);

        //free(*input);
        //free(input);

        return 0;
}
