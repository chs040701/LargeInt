/*
Author:chs040701
Number:1000010237
ver 1.03
Time:2011.3.30
Description:This is a head file,which to make large integer's calculate possible.
*/
//ver 1.01 ���ӶԳ˷���֧�� �Ż��˷����� ����ȡ�෴�������Ĵ���
//ver 1.02 ���Ӻ�׺�Լ��Լ�����
//ver 1.03 �Ż��Ӽ����㷨 �����˷�����BUG
//ver 1.04 ����ֱ�Ӵ��������������
#ifndef LARGENUM_H
#define LARGENUM_H
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#define NDEBUG


using namespace std;

const int sizeint=sizeof(int);

class LargeNum//����10000����
{

private:
    class stackdata
    {
    public:
        int size,n,lock;//size:�����С n:ʹ�õĴ�С lock���ýṹ�����õĴ���,lock==0ʱ�ű�release
        int *pData;//���ڴ�Ŵ���ÿλ������
        stackdata()//��ʼ��������
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
        bool copy(stackdata& src,int newsize);//����src���ݣ�����pData��С������¯�
        void update(const char* StrNum);//���ַ�����������
        bool reserve(int newsize);//����pData���ʱ(�޸�pData��С)
        void normalize();//������pData�����ݣ��������λ�ȣ�
        int operator [] (int pos)//�������ȡ��pData������
        {
            if(pos<n)return(pData[pos]);
            else return 0;
        }
    };
    stackdata *pnum;//������ʱ �����ɵ�LargeNum��*pnum���ݻ�ı�
    //����ʱ��*pnum����Ϊ���� LargeNum��С�ı�ʱ�ı�pnumָ�룬����*pnum����
public:
    void delete_s(stackdata *p)//��ȫ�ͷ�stackdata *p�����ݣ����û�ñ�������ɾ�����������ô���-1
    {
        if(p)
            if(p->lock==1)
                delete p;
            else
                p->lock--;
    }
    LargeNum()//Ĭ�Ϲ��� 0
    {
        pnum=new stackdata;
        pnum->lock=1;
        pnum->n=1;
        pnum->size=1;
        pnum->pData=new int[1];
        pnum->pData[0]=0;
    }
    LargeNum(int src)//��������LargeNumת��
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
    LargeNum(const LargeNum &src)//��src��������
    {
        pnum=src.pnum;
        pnum->lock++;
    }
    LargeNum(string StrNum)//���ַ�������LargeNum �����Ϸ���
    {
        pnum=new stackdata;
        pnum->update(StrNum.c_str());
        pnum->lock=1;
    }
    LargeNum(const char* StrNum)//���ַ�������LargeNum �����Ϸ���
    {
        pnum=new stackdata;
        pnum->update(StrNum);
        pnum->lock=1;
    }
    ~LargeNum()//����
    {
        delete_s(pnum);
    }
    char *ToStr();//ת��Ϊ�ַ���

    //���������������Ĭ������������LargeNum ����Ϊ������������
    //�ڲ����������ڷ�������LargeNum ��Ӧ����ȷ�ϲ�����������
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
    LargeNum div(const LargeNum &src,LargeNum *prmd=NULL)  const;//src:����,rmd:���� ���������ĳ�����
    bool ifpositive() const;//�Ƿ�Ϊ��
    bool ifnagative() const;//�Ƿ�Ϊ��
    bool operator == (const LargeNum &src) const;
    bool operator != (const LargeNum &src) const
    {
        return !(*this==src);
    }
    bool operator > (const LargeNum &src) const;
    bool operator < (const LargeNum &src) const;
    bool operator >= (const LargeNum &src) const
    {
        return !(*this<src);    //���ڵ��ھ��ǲ�С��
    }
    bool operator <= (const LargeNum &src) const
    {
        return !(*this>src);    //С�ڵ��ھ��ǲ�����
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
    LargeNum operator << (int n);//����1��int��*=10000�Ŀ��ٷ�����
    LargeNum operator >> (int n);//����1��int��/=10000�Ŀ��ٷ�����
    LargeNum opposite();//ȡ�෴��


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

LargeNum abs(LargeNum &src);//ȡ����ֵ


#endif
