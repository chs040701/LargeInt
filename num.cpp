#include "num.h"
#include <cstring>
#include <cmath>
#ifndef NDEBUG
extern stringstream strerr;
#endif
bool LargeNum::stackdata::copy(stackdata& src,int newsize)
{
#ifndef NDEBUG
    if(newsize<n)//新大小不足以容纳原内容 出错
    {
        strerr<<"stackdata:试图reserve到溢出！\n";
        return false;
    }
#endif
    if(pData)
        delete []pData;
    size=newsize;
    n=src.n;
    lock=1;
    pData=new int[size];
    memcpy(pData,src.pData,n*sizeint);
    memset(pData+n,0,(size-n)*sizeint);
    return true;
}

void LargeNum::stackdata::update(const char* StrNum)
{
    if(pData)
        delete []pData;
    int l=strlen(StrNum);
    if(StrNum[0]=='-')//负数构造方法：每位都设为负值
    {
        size=n=(l+2)/4;
        pData=new int[size];
        for(int i=0; i<size-1; i++)
            pData[i]=-int(StrNum[l-1-i*4]-'0')
                     -int(StrNum[l-2-i*4]-'0')*10
                     -int(StrNum[l-3-i*4]-'0')*100
                     -int(StrNum[l-4-i*4]-'0')*1000;
        pData[size-1]=0;
        for(int i=0; i<=(l-2)%4; i++)
            pData[size-1]=pData[size-1]*10-StrNum[i+1]+'0';
    }
    else//正数构造方法：每位都设为正值
    {
        size=n=(l+3)/4;
        pData=new int[size];
        for(int i=0; i<size-1; i++)
            pData[i]=int(StrNum[l-1-i*4]-'0')
                     +int(StrNum[l-2-i*4]-'0')*10
                     +int(StrNum[l-3-i*4]-'0')*100
                     +int(StrNum[l-4-i*4]-'0')*1000;
        pData[size-1]=0;
        for(int i=0; i<=(l-1)%4; i++)
            pData[size-1]=pData[size-1]*10+StrNum[i]-'0';
    }
    normalize();
}


bool LargeNum::stackdata::reserve(int newsize)
{
#ifndef NDEBUG
    if(newsize<n)
    {
        strerr<<"stackdata:试图reserve到溢出！\n";
        return false;
    }
#endif
    int *tp=pData;
    size=newsize;
    pData=new int[size];
    memcpy(pData,tp,n*sizeint);
    memset(pData+n,0,(size-n)*sizeint);
    delete []tp;
    return true;
}

void LargeNum::stackdata::normalize()
{
    while(pData[n-1]==0&&n>1)n--;//找到第一个不为零的高位（或者到只有一位）
    if(n>=size)reserve(n+1);//为进位准备
    //由于未正常化的数字可能有1234*10000^2+(-239999)*10000^1+23141之类的形式
    if(pData[n-1]<0)//<0时的进位方法
    {
        for(int i=0; i<n; i++)
        {
            pData[i+1]+=pData[i]/10000;
            pData[i]=pData[i]%10000;
            if(pData[i]>0)
            {
                pData[i+1]++;
                pData[i]-=10000;
            }
        }
        if(pData[n])n++;
    }
    else//>0的进位方法
    {
        for(int i=0; i<n; i++)
        {
            pData[i+1]+=pData[i]/10000;
            pData[i]=pData[i]%10000;
            if(pData[i]<0)
            {
                pData[i+1]--;
                pData[i]+=10000;
            }
        }
        if(pData[n])n++;
    }
    while(pData[n-1]==0&&n>1)n--;//重设位数
}

char *LargeNum::ToStr()
{
    char* dst=new char[pnum->n*4+2];
    stringstream itos;//用来暂存输出
    itos<<(pnum->pData[pnum->n-1]);
    for(int i=pnum->n-2; i>-1; i--)
        itos<<setfill('0')<<setw(4)<<abs(pnum->pData[i]);
    itos>>dst;
    return dst;

}

LargeNum LargeNum::operator = (const LargeNum &src)
{
    if(pnum)
        delete_s(pnum);
    if(src.pnum)
    {
        pnum=src.pnum;
        pnum->lock++;
    }
    else
    {

#ifndef NDEBUG
        strerr<<"试图等于空！"
#endif
        throw (-1L);
    }
    return *this;
}

LargeNum LargeNum::operator + (const LargeNum &src) const
{
    //逐位相加 最后处理进位
    LargeNum dst;
    int i;
    if(pnum->n>src.pnum->n)
    {
        dst.pnum->reserve(pnum->n+1);
        for(i=0; i<src.pnum->n; i++)
        {
            dst.pnum->pData[i]=pnum->pData[i]+src.pnum->pData[i];
        }
        for(; i<pnum->n; i++)
        {
            dst.pnum->pData[i]=pnum->pData[i];
        }
        dst.pnum->n=pnum->n;
    }
    else
    {
        dst.pnum->reserve(src.pnum->n+1);
        for(i=0; i<pnum->n; i++)
        {
            dst.pnum->pData[i]=pnum->pData[i]+src.pnum->pData[i];
        }
        for(; i<src.pnum->n; i++)
        {
            dst.pnum->pData[i]=src.pnum->pData[i];
        }
        dst.pnum->n=src.pnum->n;
    }
    dst.pnum->normalize();
    return dst;
}

LargeNum LargeNum::operator - (const LargeNum &src) const
{
    //思想同加法
    LargeNum dst;
    int i;
    if(pnum->n>src.pnum->n)
    {
        dst.pnum->reserve(pnum->n+1);
        for(i=0; i<src.pnum->n; i++)
        {
            dst.pnum->pData[i]=pnum->pData[i]-src.pnum->pData[i];
        }
        for(; i<pnum->n; i++)
        {
            dst.pnum->pData[i]=pnum->pData[i];
        }
        dst.pnum->n=pnum->n;
    }
    else
    {
        dst.pnum->reserve(src.pnum->n+1);
        for(i=0; i<pnum->n; i++)
        {
            dst.pnum->pData[i]=pnum->pData[i]-src.pnum->pData[i];
        }
        for(; i<pnum->n; i++)
        {
            dst.pnum->pData[i]=-src.pnum->pData[i];
        }
        dst.pnum->n=src.pnum->n;
    }
    dst.pnum->normalize();
    return dst;
}

LargeNum LargeNum::operator * (const LargeNum &src) const
{
    //取一位逐位相乘 然后去累加（类似列竖式）
    LargeNum dst,tmp;
    const LargeNum *pLNa,*pLNb;//pLNa 为被乘数指针，pLNb为乘数指针
    dst.pnum->reserve(pnum->n+src.pnum->n);
    tmp.pnum->reserve(pnum->n+src.pnum->n);
    if(pnum->n>src.pnum->n)//乘数位数少，可以少做normalize()，减少时间复杂度
    {
        pLNa=this;
        pLNb=&src;
    }
    else
    {
        pLNa=&src;
        pLNb=this;
    }
    for(int j=0; j<pLNa->pnum->n; j++)
    {
        dst.pnum->pData[j]=pLNb->pnum->pData[0]*pLNa->pnum->pData[j];
    }
    dst.pnum->n=pLNa->pnum->n;
    dst.pnum->normalize();
    for(int i=1; i<pLNb->pnum->n; i++)
    {
        memset(tmp.pnum->pData,0,sizeint*i);
        for(int j=0; j<pLNa->pnum->n; j++)
        {
            tmp.pnum->pData[i+j]=pLNb->pnum->pData[i]*pLNa->pnum->pData[j];
        }
        tmp.pnum->n=i+pLNa->pnum->n;
        dst+=tmp;
    }
    return dst;
}

LargeNum LargeNum::operator ^ (const LargeNum &src) const
{
    if(*this==0)
        if(src<=0)
            throw 1L;//0的非正数次幂非法
        else
            return LargeNum(0);
    if(*this==1)
        return LargeNum(1);//1的任意次幂为1
    if(src.ifnagative())
        return LargeNum(0);//>1的正整数的负整数次幂<1，所以是0
    LargeNum dst(1);
    LargeNum ex(src);
    while(ex.ifpositive())//作连续乘法
    {
        dst*=(*this);
        --ex;
    }
    return dst;
}

bool LargeNum::operator == (const LargeNum &src) const
{
    if(pnum->n!=src.pnum->n)//位数不等 自然不会相等
        return false;
    for(int i=0; i<pnum->n; i++)//逐位比较
        if(pnum->pData[i]!=src.pnum->pData[i])
            return false;
    return true;
}
bool LargeNum::ifpositive() const
{
    return pnum->pData[pnum->n-1]>0;//只需判断最高位即可
}

bool LargeNum::ifnagative() const
{
    return pnum->pData[pnum->n-1]<0;//只需判断最高位即可
}
bool LargeNum::operator > (const LargeNum &src)  const
{
    if(src.pnum->n==pnum->n)//位数相等时 逐位判断（符号不同判断首位即得结果）
    {
        for(int i=pnum->n-1; i>0; i--)
            if(pnum->pData[i]!=src.pnum->pData[i])
                return pnum->pData[i]>src.pnum->pData[i];
        return pnum->pData[0]>src.pnum->pData[0];//最后一位 即使两大数相等 也是false
    }
    else//位数不等
        if(pnum->pData[pnum->n-1]<0)
            if(src.pnum->pData[src.pnum->n-1]>0)
                return false;//负数<正数
            else
                return src.pnum->n>pnum->n;//位数多的小
        else if(src.pnum->pData[src.pnum->n-1]<0)
            return true;//正数>负数
        else
            return src.pnum->n<pnum->n;//位数多的大
}

bool LargeNum::operator < (const LargeNum &src)  const//operator >
{
    if(src.pnum->n==pnum->n)
    {
        for(int i=pnum->n-1; i>0; i--)
            if(pnum->pData[i]!=src.pnum->pData[i])
                return pnum->pData[i]<src.pnum->pData[i];
        return pnum->pData[0]<src.pnum->pData[0];
    }
    else if(pnum->pData[pnum->n-1]<0)
        if(src.pnum->pData[src.pnum->n-1]>0)
            return true;
        else return src.pnum->n<pnum->n;
    else if(src.pnum->pData[src.pnum->n-1]<0)
        return false;
    else return src.pnum->n>pnum->n;
}

LargeNum LargeNum::div(const LargeNum &src,LargeNum *prmd) const
{
    if(src==0)throw int(-1);//如若除数为0，抛出异常
    //特殊情况处理
    if(pnum->n<src.pnum->n)//除数位数小于被除数 直接返回
    {
        if(prmd)
            *prmd=(*this);
        return LargeNum(0);
    }
    if(pnum->n<2&&src.pnum->n<2)//1位除1位 直接返回
    {
        if(prmd)
            *prmd=pnum->pData[0]%src.pnum->pData[0];
        return LargeNum(pnum->pData[0]/src.pnum->pData[0]);
    }
    LargeNum dst;//商
    LargeNum rmd;
    rmd.pnum->copy(*pnum,pnum->n);//余数
    dst.pnum->reserve(rmd.pnum->n-src.pnum->n+1);
    dst.pnum->n=dst.pnum->size;
    //n位除以1位的高精度
    if(src.pnum->n<2)
    {
        int rmdn=rmd.pnum->n-1;
        while(rmdn>0)
        {
            //直接退位作除法
            rmd.pnum->pData[rmdn-1]
            +=(*rmd.pnum)[rmdn]*10000;
            rmd.pnum->pData[rmdn]=0;
            rmdn--;
            dst.pnum->pData[rmdn]
            =(*rmd.pnum)[rmdn]/src.pnum->pData[0];
            //减去该位的商
            rmd.pnum->pData[rmdn]-=dst.pnum->pData[rmdn]*src.pnum->pData[0];
        }
    }
    //n位除以n位的高精度
    //思路基本同n位除以1位的高精度 就是试商的时候采用将被除数前3位除以除数前2位的试除
    //为防止int溢出 采用float类型作中间运算
    //试商误差不超过1/10000，即余数绝对值会小于除数绝对值
    else
    {
        int rmdn=rmd.pnum->n-1;
        if(rmdn<src.pnum->n)rmdn++;
        int srcint2=(*src.pnum)[src.pnum->n-1]*10000+(*src.pnum)[src.pnum->n-2];
        while(rmdn>src.pnum->n)
        {
            dst.pnum->pData[rmdn-src.pnum->n]=
                (float((*rmd.pnum)[rmdn])*100000000+float((*rmd.pnum)[rmdn-1])*10000+(*rmd.pnum)[rmdn-2])/srcint2;
            LargeNum tmp=(src*(*dst.pnum)[rmdn-src.pnum->n])
                         <<(rmdn-src.pnum->n);
            rmd-=tmp;

            //rmd.pnum->pData[rmdn-1]=0;
            rmdn--;
        }
        {
            dst.pnum->pData[0]=(float((*rmd.pnum)[rmdn])*100000000+float((*rmd.pnum)[rmdn-1])*10000+(*rmd.pnum)[rmdn-2])/srcint2;
            LargeNum tmp=src*(*dst.pnum)[0];
            rmd-=tmp;
        }
    }
    //该方法得到的商和余数各位有正有负 需要正常化
    rmd.pnum->normalize();
    dst.pnum->normalize();
    //被除数和余数应该同号
    if(this->ifpositive())
    {
        if(rmd.ifnagative())
            if(src.ifpositive())
            {
                rmd+=src;
                dst-=1;
            }
            else
            {
                rmd-=src;
                dst+=1;
            }
    }
    else if(rmd.ifpositive())
        if(src.ifpositive())
        {
            rmd-=src;
            dst+=1;
        }
        else
        {
            rmd+=src;
            dst-=1;
        }

    if(prmd)
        (*prmd)=rmd;
    return dst;
}

LargeNum LargeNum::operator << (int n)
{
    //简单的左移
    if(n<0)return (*this)<<(-n);
    if(n)
    {
        LargeNum dst;
        dst.pnum->reserve(pnum->n+n);//至此pnum->pData为各位为0的长为pnum->n+n的数组
        dst.pnum->n=pnum->n+n;
        memcpy(dst.pnum->pData+n,pnum->pData,pnum->n*sizeint);
        memset(dst.pnum->pData,0,n*sizeint);
        return dst;
    }
    else
        return (*this);

}

LargeNum LargeNum::operator >> (int n)
{
    //简单的右移
    if(n<0)return (*this)<<(-n);
    if(n)
        if(n<pnum->n)
        {
            LargeNum dst;
            dst.pnum->reserve(pnum->n-n);
            memcpy(dst.pnum->pData,pnum->pData+n,pnum->n-n);
            return dst;
        }
        else//右移>=数字位数 结果就是0
            return LargeNum(0);
    else
        return (*this);

}


LargeNum abs(LargeNum &src)
{
    if(src.ifnagative())return src.opposite();
    return src;
}
LargeNum LargeNum::opposite()
{
    LargeNum dst;//此时不可直接构造，因为构造为引用，后续处理会修改原数据
    dst.pnum->copy(*pnum,pnum->size);
    for(int i=0; i<dst.pnum->n; i++)
    {
        dst.pnum->pData[i]=-dst.pnum->pData[i];
    }
    return dst;
}
