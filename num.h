/*
Author:chs040701
Number:1000010237
ver 1.03
Time:2011.3.30
Description:This is a head file,which to make large integer's calculate possible.
*/
//ver 1.01 增加对乘方的支持 优化乘法运算 修正取相反数操作的错误
//ver 1.02 增加后缀自加自减功能
//ver 1.03 优化加减法算法 修正乘法代码BUG
//ver 1.04 增加直接从流读入输出功能
#ifndef LARGENUM_H
#define LARGENUM_H
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#define NDEBUG


using namespace std;

const int sizeint=sizeof(int);

class LargeNum//采用10000进制
{

private:
    class stackdata
    {
    public:
        int size,n,lock;//size:数组大小 n:使用的大小 lock：该结构被引用的次数,lock==0时才被release
        int *pData;//用于存放大数每位的数据
        stackdata()//初始化空数据
        {
            size=n=0;
            lock=0;
            pData=NULL;
        }
        ~stackdata()
        {
            if(pData)
                delete []pData;
        }
        bool copy(stackdata& src,int newsize);//复制src内容，重设pData大小（另起炉灶）
        void update(const char* StrNum);//从字符串创建数据
        bool reserve(int newsize);//用于pData溢出时(修改pData大小)
        void normalize();//正常化pData的数据（负责进退位等）
        int operator [] (int pos)//防溢出的取出pData的内容
        {
            if(pos<n)return(pData[pos]);
            else return 0;
        }
    };
    stackdata *pnum;//运算中时 新生成的LargeNum的*pnum内容会改变
    //其他时候*pnum基本为常量 LargeNum大小改变时改变pnum指针，不变*pnum内容
public:
    void delete_s(stackdata *p)//安全释放stackdata *p的内容：如果没用被引用则删除，否则引用次数-1
    {
        if(p)
            if(p->lock==1)
                delete p;
            else
                p->lock--;
    }
    LargeNum()//默认构造 0
    {
        pnum=new stackdata;
        pnum->lock=1;
        pnum->n=1;
        pnum->size=1;
        pnum->pData=new int[1];
        pnum->pData[0]=0;
    }
    LargeNum(int src)//从整数向LargeNum转化
    {
        pnum=new stackdata;
        pnum->lock=1;
        pnum->n=3;
        pnum->size=3;
        pnum->pData=new int[3];
        pnum->pData[0]=src%10000;
        pnum->pData[1]=(src/10000)%10000;
        pnum->pData[2]=src/100000000;
        pnum->normalize();
    }
    LargeNum(const LargeNum &src)//从src复制内容
    {
        pnum=src.pnum;
        pnum->lock++;
    }
    LargeNum(string StrNum)//从字符串构造LargeNum 不检查合法性
    {
        pnum=new stackdata;
        pnum->update(StrNum.c_str());
        pnum->lock=1;
    }
    LargeNum(const char* StrNum)//从字符串构造LargeNum 不检查合法性
    {
        pnum=new stackdata;
        pnum->update(StrNum);
        pnum->lock=1;
    }
    ~LargeNum()//析构
    {
        delete_s(pnum);
    }
    char *ToStr();//转化为字符串

    //所有重载运算符都默认是正常化的LargeNum 返回为正常化的数字
    //内部调用若存在非正常化LargeNum 则应该是确认不必正常化的
    LargeNum operator = (const LargeNum &src);
    LargeNum operator + (const LargeNum &src) const;
    LargeNum operator - (const LargeNum &src) const;
    LargeNum operator * (const LargeNum &src) const;
    LargeNum operator / (const LargeNum &src) const
    {
        return (*this).div(src);
    }
    LargeNum operator % (const LargeNum &src) const
    {
        LargeNum dst;
        this->div(src,&dst);
        return dst;
    }
    LargeNum operator ^ (const LargeNum &src) const;
    LargeNum operator += (const LargeNum &src)
    {
        return (*this=*this+src);
    }
    LargeNum operator -= (const LargeNum &src)
    {
        return (*this=*this-src);
    }
    LargeNum operator *= (const LargeNum &src)
    {
        return (*this=*this*src);
    }
    LargeNum operator ^= (const LargeNum &src)
    {
        return (*this=*this^src);
    }
    LargeNum operator /= (const LargeNum &src)
    {
        return (*this=this->div(src));
    }
    LargeNum operator %= (const LargeNum &src)
    {
        LargeNum dst;
        this->div(src,&dst);
        return (*this)=dst;
    }
    LargeNum div(const LargeNum &src,LargeNum *prmd=NULL)  const;//src:除数,rmd:余数 （带余数的除法）
    bool ifpositive() const;//是否为正
    bool ifnagative() const;//是否为负
    bool operator == (const LargeNum &src) const;
    bool operator != (const LargeNum &src) const
    {
        return !(*this==src);
    }
    bool operator > (const LargeNum &src) const;
    bool operator < (const LargeNum &src) const;
    bool operator >= (const LargeNum &src) const
    {
        return !(*this<src);    //大于等于就是不小于
    }
    bool operator <= (const LargeNum &src) const
    {
        return !(*this>src);    //小于等于就是不大于
    }
    LargeNum operator = (int src)
    {
        return (*this)=LargeNum(src);
    }
    LargeNum operator + (int src)
    {
        return (*this)+LargeNum(src);
    }
    LargeNum operator - (int src)
    {
        return (*this)-LargeNum(src);
    }
    LargeNum operator * (int src) const
    {
        return (*this)*LargeNum(src);
    }
    LargeNum operator ^ (int src)
    {
        return (*this)^LargeNum(src);
    }
    LargeNum operator / (int src)
    {
        return (*this)/LargeNum(src);
    }
    LargeNum operator % (int src)
    {
        return (*this)%LargeNum(src);
    }
    LargeNum operator += (int src)
    {
        return (*this)+=LargeNum(src);
    }
    LargeNum operator -= (int src)
    {
        return (*this)-=LargeNum(src);
    }
    LargeNum operator *= (int src)
    {
        return (*this)*=LargeNum(src);
    }
    LargeNum operator ^= (int src)
    {
        return (*this)^=LargeNum(src);
    }
    LargeNum operator /= (int src)
    {
        return (*this)/=LargeNum(src);
    }
    LargeNum operator %= (int src)
    {
        return (*this)%=LargeNum(src);
    }
    LargeNum operator = (int &src)
    {
        return (*this)=LargeNum(src);
    }
    bool operator >= (int src)
    {
        return (*this)>=LargeNum(src);
    }
    bool operator <= (int src)
    {
        return (*this)<=LargeNum(src);
    }
    bool operator > (int src)
    {
        return (*this)>LargeNum(src);
    }
    bool operator < (int src)
    {
        return (*this)<LargeNum(src);
    }
    bool operator == (int src)
    {
        return (*this)==LargeNum(src);
    }
    LargeNum operator ++()
    {
        return (*this)=(*this)+1;
    }
    LargeNum operator --()
    {
        return (*this)=(*this)-1;
    }
    LargeNum operator ++(int)
    {
        LargeNum dst(*this);
        ++*this;
        return dst;
    }
    LargeNum operator --(int)
    {
        LargeNum dst(*this);
        --*this;
        return dst;
    }
    LargeNum operator << (int n);//左移1个int（*=10000的快速方法）
    LargeNum operator >> (int n);//右移1个int（/=10000的快速方法）
    LargeNum opposite();//取相反数


    friend istream &operator >>(istream &src,LargeNum &dst)
    {
        string tmp;
        if(!(src>>tmp))
            return src;
        if(tmp[0]!='-')
            if(tmp[0]>'9'||tmp[0]<'0')
            {
                src.clear(istream::failbit);
                return src;
            }
        for(int i=tmp.size()-1; i; i--)
            if(tmp[i]>'9'||tmp[i]<'0')
            {
                src.clear(istream::failbit);
                return src;
            }
        dst=LargeNum(tmp);
        return src;
    }
    friend ostream &operator <<(ostream &dst,LargeNum &src)
    {
        char* tmp=src.ToStr();
        dst<<tmp;
        delete tmp;
        return dst;
    }
};

LargeNum abs(LargeNum &src);//取绝对值


#endif
