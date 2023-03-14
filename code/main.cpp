#include <iostream> 
#include <string>
#include <string.h> 
#include <memory>
#include <cstdlib>
#include <time.h>
#include <cmath>
#include <float.h>
// #include "cblas.h"

//宏定义数据类型，可以按需修改
// #define dataType unsigned char
// #define dataType short
// #define dataType int
#define dataType float
// #define dataType double
// #define nSize 1000


using namespace std;


template<class T>
class Matrix
{
    private:
    shared_ptr<T> data;
    size_t row;
    size_t col;
    //这里偷懒了，我把矩阵的通道统一定义为1
    size_t channel;

    public:
    Matrix(){
        this->row = 1; 
        this->col = 1;
        this->channel=1;
        this->data=nullptr;
    }

    Matrix(size_t& row, size_t& col, std::shared_ptr<T>& data) {
        this->row = row;
        this->col = col;
        this->channel=1;
        this->data = data;
    }

    void operator=(Matrix mat){
        this->row = mat.row; 
        this->col = mat.col;
        //释放智能指针所申请的空间，并重新分配内容
        this->data.reset();
        this->data=mat.data;

        //想简化代码，但不知道是什么原因，下面这句话一直报错
        // this->data.reset(mat.data);
    }

    bool operator==(Matrix mat){
        //首先判断矩阵大小是否相等
        if(this->row==mat.row&&this->col==mat.col){ 
            for(size_t i = 0; i < mat.col*mat.row; i++){
                //对矩阵中每一个元素进行判断
                if(mat.data.get()[i]!=this->data.get()[i]){ 
                    return false;
                }
                return true;
            }
        }
        return false;
    }

    Matrix operator+(float num){
        shared_ptr<T> result = make_shared<T>(this->row*this->col);

        //这是我一开始写的，但后来优化时发现，只需要一层循环就可以完成
        // for(size_t c = 0; c < col; c++){
        //     for (size_t r = 0; r < row; r++){

        //         result.get()[c*this->row+r]=this->data.get()[c*this->row+r]+num;
        //     } 
        // }

        for(size_t each=0;each<row*col;each++){
            //矩阵数加，对矩阵中每一个元素做加法
            result.get()[each]=this->data.get()[each]+num;
        }

        return Matrix(this->row,this->col,result);
    }

    Matrix operator+(Matrix mat){
        if_legal(*this,mat);

        shared_ptr<T> result = make_shared<T>(this->row*this->col);

        for(size_t each=0;each<row*col;each++){
            result.get()[each]=this->data.get()[each]+mat.data.get ()[each];
        }

        return Matrix(this->row,this->col,result);
    }

    Matrix operator-(float num){
        shared_ptr<T> result = make_shared<T>(this->row*this->col); 
        
        for(size_t each=0;each<row*col;each++){
            result.get()[each]=this->data.get()[each]-num;
        }

        return Matrix(this->row,this->col,result);
    }

    Matrix operator-(Matrix mat){
        if_legal(*this,mat);

        shared_ptr<T> result = make_shared<T>(this->row*this->col);
        
        for(size_t each=0;each<row*col;each++){
            result.get()[each]=this->data.get()[each]-mat.data.get()[each];
        }

        return Matrix(this->row,this->col,result);
    }

    Matrix operator*(float num){
        shared_ptr<T> result = make_shared<T>(this->row*this->col); 
        
        for(size_t each=0;each<row*col;each++){
            result.get()[each]=this->data.get()[each]*num;
        }

        return Matrix(this->row,this->col,result);
    }

    Matrix operator*(Matrix<T> mat){
        if_legal(*this,mat);

        shared_ptr<T> result = make_shared<T>(mat.row*mat.col);

        //使用 openblas 计算，不清楚什么原因，一直说无法调用该函数
        // cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,
        // mat.col,mat.col,mat.col,1.0,this->data,this->row,mat.data,mat.row,0.0,result,mat.row);
        
        for(size_t each=0;each<row*col;each++){
            result.get()[each]=this->data.get()[each]*mat.data.get()[each];
        }

        return Matrix(this->row,this->col,result);
    }

    Matrix operator/(float num){
        //检查被除数是否为0，尤其注意不能直接用"num==0"，因为float并不精确
        if(fabs(num) < FLT_EPSILON){
            cerr<<"divided by zero"<<endl;
            abort();
        }

        shared_ptr<T> result = make_shared<T>(this->row*this->col);

        for(size_t each=0;each<row*col;each++){
            result.get()[each]=this->data.get()[each]/num;
        }

        return Matrix(this->row,this->col,result);
    }

    Matrix operator/(Matrix<T> mat){
        if_legal(*this,mat);

        //提前检查被除矩阵中每个元素是否为0，防止程序出错
        for(size_t each=0;each<row*col;each++){
            if(fabs(mat.data.get()[each]) < FLT_EPSILON){
                cerr<<"divided by zero"<<endl;
                abort();
            }
        }

        shared_ptr<T> result = make_shared<T>(mat.row*mat.col);

        for(size_t each=0;each<row*col;each++){
            result.get()[each]=this->data.get()[each]*mat.data.get()[each];
        }

        return Matrix(this->row,this->col,result);
    }

    //为了判断矩阵大小是否相等，另外写的一个方法
    void if_legal(Matrix<T> matA, Matrix<T> matB){    
        if(matA.row!=matB.col||matA.col!=matB.row){
            cerr<< "size not match" << endl;
            abort();
        }
    }
};


int main(){ 
    clock_t start; //计时开始 
    clock_t end; //计时结束 
    int duration; //总用时

    //一开始我是用宏定义规定矩阵大小，但我发现这样创建的数据类型是int，后来改成在main方法里直接申明
    size_t nSize=1000;
    size_t total=nSize*nSize;

    //这是我最开始采用的方法，静态指针，但显然不符合要求，容易产生内存冲突
    // static dataType * p1;
    // p1=(dataType*)malloc(total*sizeof(dataType));
    //之后我又试了智能指针，但是初始化的语法不对
    // shared_ptr<dataType> sp1=make_shared<dataType>(total);
    // sp1=(dataType*)malloc(total*sizeof(dataType));

    shared_ptr<dataType> ptr1(new dataType[total],[](dataType *p){delete [] p;});
    shared_ptr<dataType> ptr2(new dataType[total],[](dataType *p){delete [] p;});
    shared_ptr<dataType> ptr3(new dataType[total],[](dataType *p){delete [] p;});

    for(size_t each=0;each<total;each++){
        ptr1.get()[each]=(dataType)rand()/RAND_MAX;
        ptr2.get()[each]=(dataType)rand()/RAND_MAX;
        ptr3.get()[each]=0;
	}

    //这是最开始的用静态指针的方法创建矩阵
    // auto matC = Matrix<dataType>(nSize,nSize,p1);

    Matrix<dataType> matA = Matrix<dataType>(nSize,nSize,ptr1);
    Matrix<dataType> matB = Matrix<dataType>(nSize,nSize,ptr2);
    Matrix<dataType> matC = Matrix<dataType>(nSize,nSize,ptr3);
    
    // start = clock();
    // matC = matA;
    // end = clock();
    // duration = (end-start)/1000;
    // printf("assign finished, assign time: %d ms\n",duration);

    // start = clock();
    // bool result=(matC == matA);
    // end = clock();
    // duration = (end-start)/1000;
    // cout<<"compare result: "<<result<<", ";
    // printf("compare time: %d ms\n",duration);
    
    start = clock();
    matC = matA+1;
    end = clock();
    duration = (end-start)/1000; 
    printf("numerical add, add time: %d ms\n",duration);

    start = clock();
    matC = matA + matB;
    end = clock();
    duration = (end-start)/1000; 
    printf("matrix add, add time: %d ms\n",duration);

    start = clock();
    matC = matA*2;
    end = clock();
    duration = (end-start)/1000;
    printf("numerical product time: %d ms\n",duration);

    start = clock();
    matC = matA*matB;
    end = clock();
    duration = (end-start)/1000;
    printf("matrix product time: %d ms\n",duration);

    start = clock();
    matC = matA/2;
    end = clock();
    duration = (end-start)/1000;
    printf("numerical divide time: %d ms\n",duration);

    start = clock();
    matC = matA/matB;
    end = clock();
    duration = (end-start)/1000;
    printf("matrix divide time: %d ms\n",duration);

    return 0;
}