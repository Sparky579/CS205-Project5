# CS205-Project3: A Matrix library in C++
___

### 项目简介：
本项目使用C++实现了一个矩阵库，功能类似之前的$Project3$，但利用C++的特性做出了一些扩展。同时，本项目实现了不同矩阵间高效又安全的拷贝（利用软拷贝），不同变量类型的支持，以及高效ROI（感兴趣区域）的支持。

###Part 1: Analysis
需求：
1. 实现一个矩阵的Class，需要包含矩阵的信息。如：矩阵的行数，列数，channel（频道）数。
2. 需要支持不同的数据类型，如unsigned char, short, int, float, double。
3. 不能用硬拷贝实现。在实现软拷贝的同时做好内存管理。
4. 重载常用的运算符，如=, ==, +, -, *。
5. 为了实现矩阵部分运算的优化，实现ROI（感兴趣区域）。

###Part 2: Code
关键部分代码：
1. 存储矩阵
```
Matrix(int r, int c, int ch): rows(r), columns(c), channels(ch) {
    shared_ptr<T> d(new T[r * c * ch], [](T* p) {delete[] p;});
    datas = d;
    roi_start_col = roi_start_row = 0;
    roi_num_rows = rows;
    roi_num_cols = columns;
}
```
对于该类，我们设置了三种构造方法：无参（不建议，指向为null），三个参数，以及拷贝构造。其中拷贝构造采用的是软拷贝。
为了实现安全的软拷贝，在这里我们使用了$shared_ptr$ ，为了支持较低的C++版本，使用了自定义的删除方法。如果是在C++17及以上的版本中，不需要自定义删除方法，可以直接支持动态数组。

对于ROI的支持，我们在类中存储了4个private size_t变量：
```
size_t roi_start_row;
size_t roi_start_col;
size_t roi_num_rows;
size_t roi_num_cols;
```
分别存储roi区域的开始和行列数。
为了方便我们调试，重载了<<以及>>运算符便于输入和输出。
### Part 3: Result& Verification
#### Case 1: 建立矩阵
建立一个空的矩阵，大小为$3*3$， 频道数为$2$，类型为int。（分别使用无参和有参）
注意：重载了=（T变量）运算符，方便用户使用。
可以使用：```mat = 3```这种代码将全部元素设为3。
代码：
```
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
```
输出：
```
null pointer
(3 3) (3 3) (3 3)
(3 3) (3 3) (3 3)
(3 3) (3 3) (3 3)
```
#### Case 2: 基本运算：加减乘
代码：
```
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
```
输出：
```
(1 10) (2 11) (3 12)
(4 13) (5 14) (6 15)
(7 16) (8 17) (9 18)
(-1 8) (0 9) (1 10)
(2 11) (3 12) (4 13)
(5 14) (6 15) (7 16)
(3 30) (3 30) (3 30)
(12 39) (12 39) (12 39)
(21 48) (21 48) (21 48)
```
#### Case 3: 软拷贝测试
代码：（为了节省篇幅，省略了try-catch部分）
注意：我们再一次重载=，为了给矩阵赋值为一个数组。但是，用户需要让这个数组大小和矩阵大小匹配，否则会发生错误。（因为难以检测到用户传入的数组大小）
```
cout << "Case 3:\n";
double a[9] = {5, 2, 10, 7, 2, 9, 7, 4, 4};
mt21 = a;
mt22 = mt21;
Matrix<double> mt23(mt22);
mt23 -= 2;
cout << mt21;
```
输出：
```
3 0 8 5 0 7 5 2 2
```
#### Case 4: ROI测试
我们在这里用copy和ROIcopy方法分别将整个矩阵和ROI区域硬拷贝出去，再进行比较。
代码：
```
mt21.setROI(0, 3, 1, 3);
mt21.showROI();
Matrix<double> mt31 = mt21.ROIclone();
Matrix<double> mt41 = mt21.clone();
mt31.setPosition(0, 100);
mt41.setPosition(0, 100);
cout << mt31;
cout << mt41;
cout << mt21;
```
输出：
```
ROI from 0 3 to 0 5
100 0 7
100 0 8 5 0 7 5 2 2
3 0 8 5 0 7 5 2 2
```
可以观察到提取出了ROI。
![a](./images/c.png)
Output it
![a](./images/d.png)
#### Case 5: 测试ROI下的比较
代码：
```
double a[10] = {2, 1, 4, 7, 4, 8, 3, 6, 4, 8};
double b[5] = {4, 7, 4, 8, 3};
Matrix<double> mt51(1, 10, 1);
Matrix<double> mt52(1, 5, 1);
mt51 = a;
mt52 = b;
mt51.setROI(0, 2, 1, 5);
cout << (mt51 == mt52);
```
输出：
```
1
```
（代表相等）
### Part 4: Difficulties & Solutions

#### 如何安全高效地存储矩阵？
在学会指针之后，我认识到像前两次Project那样在结构体中直接存储数组是十分低效且浪费内存的。所以，这次的Project作为一个应当实用的库，采用了结构体中只存储一个$float$指针用于表示整个矩阵，再存储$rows$和$columns$变量分别表示矩阵的行和列信息。在使用矩阵中，只将一个结构体的指针传入，避免每次函数调用都赋值一遍整个结构体。当然，带有指针的结构体需要考虑内存安全的问题，库中不能不销毁局部指针，用户在局部使用完后也必须要记得使用$deleteMatrix$。

#### 如何用一个指针模拟整个二维数组？
将二维数组展开变成$rows * columns$个$float$变量，矩阵的$r$行$c$列通过计算可得就是第$r * columns + c$个变量。函数内$pos$方法就是基于此，$pos(mat, r, c)$在不出现异常的情况下返回$datas + (r * columns + c)$的地址。

#### 如何实现安全的删除操作
一开始时，该库使用的删除函数如下：
```
bool deleteMatrix(Matrix *mat)
{   
    if (mat == NULL) return 0;
	//delete a matrix
	//this version requires to set mat=NULL after using
	if (mat == NULL) return;
	if (mat->datas != NULL) free(mat->datas);
	free(mat);
    mat == NULL //*
    return 1;
}
```
但是，$*$的一行被发现实际上是无用的，对一个矩阵调用该方法时会导致它指向的内容被释放，也就是成为了野指针。出现的问题就是再次误调用该指针会出现错误。原因是传入的$mat$是原$mat$的一个复制，原$mat$指针并未因为打$*$的一行而被置为$NULL$。所以将传入的类型改为矩阵指针的指针。想要删除一个矩阵指针的时候传入该指针的地址。即：```deleteMatrix(&mat)```
