#include<cstdio>
#include<cstdlib>
#include<memory>
#include<exception>
#include<iostream>
#include<cstring>
using std::shared_ptr;
using std::invalid_argument;
using std::make_shared;
using std::istream;
using std::ostream;
template <typename T>
class Matrix{
    private:
        shared_ptr<T> datas;
        size_t rows, columns, channels;
        size_t roi_start_row;
        size_t roi_start_col;
        size_t roi_num_rows;
        size_t roi_num_cols;
    public:
        Matrix(int r, int c, int ch): rows(r), columns(c), channels(ch) {
            shared_ptr<T> d(new T[r * c * ch], [](T* p) {delete[] p;});
            datas = d;
            roi_start_col = roi_start_row = 0;
            roi_num_rows = rows;
            roi_num_cols = columns;
        }
        ~Matrix() {
            datas.reset();
        }
        Matrix (const Matrix &x) {
            rows = x.rows;
            columns = x.columns;
            datas = x.datas;
            (*this).setROI(x.roi_start_row, roi_start_col, roi_num_rows, roi_num_cols);
            channels = x.channels;
        }
        void setPosition(int x, T num) {
            *(datas.get() + x) = num;
        }
        Matrix &operator = (const Matrix &x) {
            columns = x.columns;
            rows = x.rows;
            datas = x.datas;
            channels = x.channels;
            (*this).setROI(x.roi_start_row, roi_start_col, roi_num_rows, roi_num_cols);
            return *this;
        }
        Matrix operator * (const T &x) const {
            if (this->datas == NULL) throw invalid_argument("null pointer");
            Matrix mat(this->roi_num_rows, this->roi_num_cols, this->channels);
            for (int l = 0; l < this->channels; l++)
            for (int i = 0; i < this->roi_num_rows; i++) {
                for (int j = 0; j < this->roi_num_cols; j++) {
                    mat(l, i, j) = (*this).ROIpos(l, i, j) * x;
                }
            }
            return mat;
        }
        T& operator () (int channel, int row, int column) {
            return *(datas.get() + (rows * columns * channel + row * columns + column));
        }
        T operator () (int channel, int row, int column)const {
            return *(datas.get() + (rows * columns * channel + row * columns + column));
        }
        T& operator () (int x) {
            return *(datas.get() + x);
        }
        T operator () (int x) const {
            return *(datas.get() + x);
        }
        T ROIpos(int channel, int row, int column) const {
            return (*this)(channel, roi_start_col + row, roi_start_col + column);
        }
        T& ROIpos(int channel, int row, int column) {
            return (*this)(channel, roi_start_col + row, roi_start_col + column);
        }
        bool operator == (const Matrix &x) const {
            if (this->roi_num_rows != x.roi_num_rows || this->roi_num_cols != x.roi_num_cols) return 0;
            if (x.datas == datas && x.roi_start_col == roi_start_col && x.roi_start_row == roi_start_row) return 1;
            for (int k = 0; k < x.channels; k++) {
                for (int i = 0; i < x.roi_num_rows; i++) {
                    for (int j = 0; j < x.roi_num_cols; j++) {
                        if (this->ROIpos(k, i, j) != x.ROIpos(k, i, j)) return 0;
                    }
                }
            }
            return 1;
        }
        friend Matrix operator *(const T &x, Matrix &y){
            return y * x;
        }
        void getROI(size_t &roi_st_row, size_t &roi_st_col, size_t &roi_rows, size_t &roi_cols) {
            roi_st_col = roi_start_col;
            roi_st_row = roi_start_row;
            roi_rows = roi_num_rows;
            roi_cols = roi_num_cols;
        }
        void setROI(size_t roi_st_row, size_t roi_st_col, size_t roi_rows, size_t roi_cols) {
            roi_start_col = roi_st_col;
            roi_start_row = roi_st_row;
            roi_num_rows = roi_rows;
            roi_num_cols = roi_cols;
        }
        Matrix operator * (const Matrix &x) const {
            if (this->datas == NULL || x.datas == NULL) throw invalid_argument("null pointer");
            if (this->roi_num_cols != x.roi_num_rows || this->channels != x.channels) throw invalid_argument("not match");
            Matrix mat(this->roi_num_rows, x.roi_num_cols, this->channels);
            for (int l = 0; l < this->channels; l++)
            for (int i = 0; i < this->roi_num_rows; i++) {
                for (int k = 0; k < this->roi_num_cols; k++) {
                    for (int j = 0; j < x.roi_num_cols; j++) {
                        (mat(l, i, j))
                        += (this->ROIpos(l, i, k)) * x.ROIpos(l, k, j);
                    }
                }
            }
            return mat;
        }
        Matrix operator + (const Matrix &x) const {
            if (this->datas == NULL || x.datas == NULL) throw invalid_argument("null pointer");
            if (this->roi_num_rows != x.roi_num_rows || this->roi_num_cols != x.roi_num_cols || x.channels != channels) throw invalid_argument("not match");
            Matrix mat(this->roi_num_rows, this->roi_num_cols, this->channels);
            for (int k = 0; k < this->channels; k++) {
                for (int i = 0; i < this->roi_num_rows; i++) {
                    for (int j = 0; j < this->roi_num_cols; j++) {
                        mat(k, i, j) = (*this).ROIpos(k, i, j) + x.ROIpos(k, i, j);
                    }
                }
            }
            return mat;
        }
        Matrix operator + (const T &x) const {
            if (this->datas == NULL) throw invalid_argument("null pointer");
            Matrix mat(this->roi_num_rows, this->roi_num_cols, this->channels);
            for (int l = 0; l < this->channels; l++)
            for (int i = 0; i < this->roi_num_rows; i++) {
                for (int j = 0; j < this->roi_num_cols; j++) {
                    mat(l, i, j) = (*this).ROIpos(l, i, j) + x;
                }
            }
            return mat;
        }
        Matrix operator - (const Matrix &x) const {
            if (this->datas == NULL || x.datas == NULL) throw invalid_argument("null pointer");
            if (this->roi_num_rows != x.roi_num_rows || this->roi_num_cols != x.roi_num_cols || x.channels != channels) throw invalid_argument("not match");
            Matrix mat(this->roi_num_rows, this->roi_num_cols, this->channels);
            for (int k = 0; k < this->channels; k++) {
                for (int i = 0; i < this->roi_num_rows; i++) {
                    for (int j = 0; j < this->roi_num_cols; j++) {
                        mat(k, i, j) = (*this).ROIpos(k, i, j) - x.ROIpos(k, i, j);
                    }
                }
            }
            return mat;
        }
        Matrix operator - (const T &x) const {
            if (this->datas == NULL) throw invalid_argument("null pointer");
            Matrix mat(this->roi_num_rows, this->roi_num_cols, this->channels);
            for (int l = 0; l < this->channels; l++)
            for (int i = 0; i < this->roi_num_rows; i++) {
                for (int j = 0; j < this->roi_num_cols; j++) {
                    mat(l, i, j) = (*this).ROIpos(l, i, j) - x;
                }
            }
            return mat;
        }
        Matrix operator += (const T &x) {
            for (int k = 0; k < this->channels; k++) {
                for (int i = 0; i < this->roi_num_rows; i++) {
                    for (int j = 0; j < this->roi_num_cols; j++) {
                        (*this).ROIpos(k, i, j) = (*this).ROIpos(k, i, j) + x;
                }
            }
            }
            return *this;
        }
        Matrix operator -= (const T &x) {
            for (int k = 0; k < this->channels; k++) {
                for (int i = 0; i < this->roi_num_rows; i++) {
                    for (int j = 0; j < this->roi_num_cols; j++) {
                        (*this).ROIpos(k, i, j) = (*this).ROIpos(k, i, j) - x;
                }
            }
            }
            return *this;
        }
        Matrix clone () const {
            if (this->datas == NULL) throw invalid_argument("null pointer");
            Matrix mat(this->rows, this->columns, this->channels);
            for (int i = 0; i < this->rows * this->columns * this->channels; i++) {
                mat(i) = (*this)(i);
            }
            return mat;
        }
        Matrix ROIclone() const {
            if (this->datas == NULL) throw invalid_argument("null pointer");
            Matrix mat(this->roi_num_rows, this->roi_num_cols, this->channels);
            for (int k = 0; k < this->channels; k++) {
                for (int i = 0; i < this->roi_num_rows; i++) {
                    for (int j = 0; j < this->roi_num_cols; j++) {
                        mat(k, i, j) = (*this).ROIpos(k, i, j);
                    }
                }
            }
            return mat;
        }
        friend ostream & operator <<(ostream &out, Matrix &x) {
            for (int i = 0; i < x.rows; i++) {
                for (int j = 0; j < x.columns; j++) {
                    if (x.channels == 1) out << x(0, i, j);
                    else {
                        out << "(";
                        for (int l = 0; l < x.channels; l++) {
                            out << x(l, i, j);
                            if (l < x.channels - 1) printf(" ");
                        }
                        out << ')';
                    }
                    if (j != x.columns - 1) out << " ";
                    else out << '\n';
                }
            }
            return out;
        }
        friend ostream & operator <<(ostream &out, Matrix &&x) {
            for (int i = 0; i < x.rows; i++) {
                for (int j = 0; j < x.columns; j++) {
                    if (x.channels == 1) out << x(0, i, j);
                    else {
                        out << "(";
                        for (int l = 0; l < x.channels; l++) {
                            out << x(l, i, j);
                            if (l < x.channels - 1) printf(" ");
                        }
                        out << ')';
                    }
                    if (j != x.columns - 1) out << " ";
                    else out << '\n';
                }
            }
            return out;
        }
        friend istream & operator >>(istream &in, Matrix &x) {
            T f;
            for (int i = 0; i < x.rows; i++) {
                for (int j = 0; j < x.columns; j++) {
                    for (int k = 0; k < x.channels; k++) {
                        in >> x(k, i, j);
                        // printf("(%d %d %d)", k, i, j);
                    }
                }
            }
            return in;
        }
        T *pos(int r, int c) {
            return (datas.get() + (r * columns + c));
        }
        T *pos(int i) {
            return (datas.get() + i);
        }

};