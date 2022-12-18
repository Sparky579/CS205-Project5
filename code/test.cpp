#include<cstdio>
#include"matrix.h"
#include<memory>
using namespace std;
int main()
{
    //case 1
    cout << "Case 1:\n";
    Matrix<int> mt00;
    try{
        cout << mt00;
    }
    catch(exception &e) {
        cout << e.what() << '\n';
    }
    Matrix<int> mt01(3, 3, 2);
    try{
        mt01 = 3;
        cout << mt01;
    }
    catch(exception &e) {
        cout << e.what();
    }
    //case 2
    try{
        cout << "Case 2:\n";
        Matrix<int> mt11(3, 3, 2);
        Matrix<int> mt22(3, 3, 2);
        for (int i = 0; i < 18; i++) {
            mt11(i) = i;
            mt22(i) = 1;
        }
        cout << mt11 + mt22;
        cout << mt11 - mt22;
        cout << mt11 * mt22;
    }
    catch(exception &e) {
        cout << e.what();
    }
    //case 3
    Matrix<double> mt21(1, 9, 1), mt22;
    try{
        cout << "Case 3:\n";
        double a[9] = {5, 2, 10, 7, 2, 9, 7, 4, 4};
        mt21 = a;
        mt22 = mt21;
        Matrix<double> mt23(mt22);
        mt23 -= 2;
        cout << mt21;
    }
    catch(exception &e) {
        cout << e.what();
    }
    Matrix<double> mt1(3, 3, 1);
    for (int i = 0; i < 9; i++) {
        mt1.setPosition(i, i);
        // printf("\n%f\n", mt1(3));
    }
    //case 4
    try{
        cout << "Case 4:\n";
        mt21.setROI(0, 3, 1, 3);
        mt21.showROI();
        Matrix<double> mt31 = mt21.ROIclone();
        Matrix<double> mt41 = mt21.clone();
        mt31.setPosition(0, 100);
        mt41.setPosition(0, 100);
        cout << mt31;
        cout << mt41;
        cout << mt21;
    }
    catch(exception &e) {
        cout << e.what();
    }
    //case 5
    try{
        cout << "Case 5:\n";
        double a[10] = {2, 1, 4, 7, 4, 8, 3, 6, 4, 8};
        double b[5] = {4, 7, 4, 8, 3};
        Matrix<double> mt51(1, 10, 1);
        Matrix<double> mt52(1, 5, 1);
        mt51 = a;
        mt52 = b;
        mt51.setROI(0, 2, 1, 5);
        cout << (mt51 == mt52);
    }catch(exception &e) {
        cout << e.what();
    }
    //case 6
    system("pause");
}
