# CS205-Project5: A Matrix library in C++
___

### 项目简介：
本项目使用C++实现了一个矩阵库，功能类似之前的 $Project3$ ，但利用C++的特性做出了一些扩展。同时，本项目实现了不同矩阵间高效又安全的拷贝（利用软拷贝），不同变量类型的支持，以及高效ROI（感兴趣区域）的支持。

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
为了实现安全的软拷贝，在这里我们使用了 shared_ptr ，为了支持较低的C++版本，使用了自定义的删除方法。如果是在C++17及以上的版本中，不需要自定义删除方法，可以直接支持动态数组。

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
建立一个空的矩阵，大小为 $3*3$ ， 频道数为 $2$ ，类型为int。（分别使用无参和有参）
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
1代表两个区域相等。
#### Case 6: 异常处理
代码：
```
try{
    cout << mt21 * mt21;
}catch(exception &e) {
    cout << e.what() << '\n';
}
try{
    mt21.setROI(5, 5, 9, 9);
    cout << mt21 + 1;
}catch(exception &e) {
    cout << e.what() << '\n';
}
```
输出：
```
not match
roi not valid
```
### Part 4: Difficulties & Solutions

#### 如何存储矩阵？
在软拷贝的要求下，使用普通的指针完成内存管理是较为困难的。我们可以借助C++11自带的shared_ptr完成这个工作。shared_ptr可以记录多少个指针指向同一块区域，并且在这个值为0的时候自动回收内存。这能帮我们节省很多代码量。

#### 如何实现支持不同的数据类型？
在很多mat库中，使用了union，合并不同的数据类型。但是在C++标准下，我们有更简便的方法：类模板。我们使用类模板，然后在类中对T数据类型进行操作即可。

#### 如何在类模板下实现矩阵乘法？
是的，一个困难是矩阵乘法需要初始化一些变量为0再让它加若干个值。我采用的解决方法是：首先确保矩阵非空，这代表矩阵一定有第一个元素。然后采用```mat(0) - mat(0)```代表0元。

#### 如何实现安全的删除操作？
在使用智能指针的情况下，这变得较为简单。只需要调用智能指针的reset函数，shared_ptr会在合适的时机帮我们完成内存回收。

#### 如何让用户快捷地访问矩阵内的元素？
我采用了重载括号的方式。重载的括号有：3个元素（行，列，频道），1个元素（原始的位置），这两种中每种又分为用户可以在外部修改的引用重载和不可修改的直接重载，共4个重载括号运算符。

#### 如何实现cout << mt1 + mt2这种操作？
由于mt1 + mt2返回的是一个右值引用（临时量），正常的运算符重载不支持。所以，我在这里采用了两种不同的重载，分别将左值和右值引用输出到流里。
