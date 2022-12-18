#include<cstdio>
#include"matrix.h"
#include<memory>
using namespace std;
int main()
{
    unique_ptr<int[]> up = make_unique<int[]> (5);
    shared_ptr<float> d (new float[3], [](float* p) {printf("del0");delete[] p;});
    *(d.get()) = 2;
    *(d.get() + 1) = 2;
    printf("%f\n", *(d.get() + 1));
    Matrix<double> mt1(3, 3, 1);
    for (int i = 0; i < 9; i++) {
        mt1.setPosition(i, i);
        // printf("\n%f\n", mt1(3));
    }
    
    Matrix<double> mt2(3, 3, 1);
    cin >> mt2;
    cout << mt1;
    cout << mt2;
    printf("\n");
    mt1.setROI(1, 1, 2, 2);
    mt2.setROI(1, 1, 2, 2);
    cout << mt2.ROIclone();
    cout << (mt1 * mt2);
    Matrix<double> mt3 = mt2;
    mt3(0, 1, 1) = 7;
    mt3.setPosition(0, 10);
    mt2 = mt2 * 2;
    cout << mt2;
    printf("%d\n", mt1 == mt2);
    system("pause");
}
