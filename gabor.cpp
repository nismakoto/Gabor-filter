#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <sstream>
using namespace std;

// 帯域幅
#define Sigma 10.0
#define Sigma_2D 5.0
// 波長のコサイン成分
#define Lambda 20
#define Lambda_2D 20
// 空間アスペクト比
#define Gamma 0.5
// 方位選択
#define Direction 8
// ガボール関数に使う画像の座標幅
#define K1 3
#define K2 3
#define K1_1D 25
#define K1_2D 25
#define K2_2D 25

// split関数
// string str 分割したい文字列
// string delim デリミタ
// list<string> 分割された文字列
list<string> split(string str, string delim)
{
        list<string> result;
        int cutAt;
        while((cutAt = str.find_first_of(delim)) != str.npos)
        {
                if(cutAt > 0)
                {
                        result.push_back(str.substr(0, cutAt));
                }
                str = str.substr(cutAt+1);
        }
        if(str.length() > 0)
        {
                result.push_back(str);
        }
        return result;
}

// ガボール関数
double gabor(int x)
{
        double pos_psi = 45;
        // 位相オフセット
        double psi = 2*M_PI*pos_psi/180;

        return (exp(-(pow(x,2)/pow(Sigma,2)))/sqrt(2.0*M_PI*pow(Sigma,2))*sin(2.0*M_PI/Lambda*x+psi))/20;
}

// 2次元ガボール関数
double gabor_2d(int x, int y, int Deg)
{
        double X, Y;
        double Rad = Deg*M_PI/180;
        double pos_psi = 0;
        // 位相オフセット
        double psi = 2*M_PI*pos_psi/180;
        X = (x*cos(Rad))+(y*sin(Rad));
        Y = (-(x*sin(Rad))+(y*cos(Rad)));

        return (exp(-(pow(X,2)+pow(Gamma*Y,2))/(2.0*pow(Sigma_2D,2)))*sin((2.0*M_PI/Lambda_2D*X)+psi))/20;
}

int main()
{
        // ファイルを開く
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

        // PGMパーサ
        while(ifs1 && getline(ifs1, buf))
        {
                if(i >= 0 && i < 3)
                {
                        // splitを実行
                        list<string> strList = split(buf, " ");

                        // イテレータを取得
                        list<string>::iterator iter1 = strList.begin();

                        j = 0;
                        // パースする
                        // 最後まで
                        while(iter1 != strList.end())
                        {
                                if(i == 1 && j == 0)
                                {
                                        // PGM画像の横幅を取得
                                        width = atoi(string(*iter1).c_str());
                                }
                                else if(i == 1 && j == 1)
                                {
                                        // PGM画像の縦幅を取得
                                        height = atoi(string(*iter1).c_str());
                                }
                                else if(i == 2)
                                {
                                        // 1画素の最大値を取得
                                        pixel_max = atoi(string(*iter1).c_str());
                                }
                                j++;
                                // １つ後方に進む
                                ++iter1; 
                        }
                }
                i++;
        }

        // ファイルを閉じる
        ifs1.close();

        // input画像を格納する領域の動的確保
        // 行を作る
        int **input = new int*[height];
        // 列を作る
        for(i=0; i < height; i++)
        {
                input[i] = new int[width];
        }

        // output画像を格納する領域の動的確保
        // 行を作る
        double **output = new double*[height-K1*2];
        // 列を作る
        for(i=0; i < height-K1*2; i++)
        {
                output[i] = new double[width-K2*2];
        }

        // ファイルを開く
        ifstream ifs2("test.pgm");
        i = 0;
        j = 0;
        k = 0;
        l = 0;

        // PGMパーサ
        while(ifs2 && getline(ifs2, buf))
        {
                if(i > 2)
                {
                        // splitを実行
                        list<string> strList = split(buf, " ");

                        // イテレータを取得
                        list<string>::iterator iter2 = strList.begin(); 

                        k = 0;
                        // パースする
                        // 最後まで
                        while(iter2 != strList.end())
                        {
                                if(j < height && k < width)
                                {
                                        // 1画素辺りの数値を入力用配列に格納する
                                        input[j][k] = atoi(string(*iter2).c_str());
                                }
                                k++;
                                // １つ後方に進む
                                ++iter2;
                        }
                        j++;
                }
                i++;
        }

        // ファイルを閉じる
        ifs2.close();

        // 指定した方位の数だけ処理をする
        for(int deg=1; deg <= Direction; deg++)
        {
                // 法線方向の計算
                int theta =  deg*(360/Direction);
                cout << theta << endl;
                // 文字列以外の変数を文字列にまとめる
                // 指定した方位の数だけ画像ファイルを開く
                stringstream imagestr;
                imagestr.str("");
                imagestr << "gabor_2d0" << deg << ".pgm" << flush;

                // ファイルを開く
                ofstream ofs1(imagestr.str().c_str());
                ofs1 << "P2" << endl;
                // 出力画像は,縦横ともにK1*2,K2*2ずつ小さくなる
                // そのために横幅と縦幅を調整する
                ofs1 << width-K2*2 << " " << height-K1*2 << endl;
                ofs1 << pixel_max << endl;

                // 2次元の単純型細胞の受容野に入力を与えた時の出力を計算する
                for(i=K1; i < height-K1; i++)
                {
                        for(j=K2; j < width-K2; j++)
                        {
                                for(k=-K1; k <= K1; k++)
                                {
                                        for(l=-K2; l <= K2; l++)
                                        {
                                                output[i-K1][j-K2] += input[i-k][j-l]*gabor_2d(k, l, theta);
                                        }
                                }
                        }
                }

                // 小さくなった画像を考慮しつつ,ファイルに出力する
                for(i=0; i < height-K1*2; i++)
                {
                        for(j=0; j < width-K2*2; j++)
                        {
                                ofs1 << abs((int)output[i][j]);
                                if(j < width-K2*2){
                                        ofs1 << " ";
                                }
                        }
                        ofs1 << endl;
                }

                // ファイルを閉じる
                ofs1.close();

        }

        // ファイルを開く
        ofstream ofs2("gabor.dat");
        ofstream ofs3("gabor_2d.dat");

        // 1次元の単純型細胞の受容野を計算する
        for(k=-K1_1D; k <= K1_1D; k++)
        {
                ofs2 << k << " " << gabor(k) << endl;
        }

        // 2次元の単純型細胞の受容野を指定した方位の数だけ計算する
        for(k=-K1_2D; k <= K1_2D; k++)
        {
                for(l=-K2_2D; l <= K2_2D; l++)
                {
                        ofs3 << k << " " << l;
                        for(int deg=1; deg <= Direction; deg++)
                        {
                                // 法線方向の計算
                                int theta = deg*(360/Direction);
                                ofs3 << " " << gabor_2d(k, l, theta);
                        }
                        ofs3 << endl; 
                }
                ofs3 << endl;
        }

        // ファイルを閉じる
        ofs2.close();
        ofs3.close();

        // 領域の解放
        // 列を解放
        for(i=0; i < height; i++)
        {
                delete [] input[i];
        }
        // 行を解放
        delete [] input;

        // 領域の解放
        // 列を解放
        for(i=0; i < height-K1*2; i++)
        {
                delete [] output[i];
        }
        // 行を解放
        delete [] output;

        return 0;
}
