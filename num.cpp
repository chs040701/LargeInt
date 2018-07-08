#include "num.h"
#include <cstring>
#include <cmath>
#ifndef NDEBUG
extern stringstream strerr;
#endif
bool LargeNum::stackdata::copy(stackdata& src,int newsize)
{
#ifndef NDEBUG
    if(newsize<n)//�´�С����������ԭ���� ����
    {
        strerr<<"stackdata:��ͼreserve�������\n";
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
    if(StrNum[0]=='-')//�������췽����ÿλ����Ϊ��ֵ
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
    else//�������췽����ÿλ����Ϊ��ֵ
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
        strerr<<"stackdata:��ͼreserve�������\n";
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
    while(pData[n-1]==0&&n>1)n--;//�ҵ���һ����Ϊ��ĸ�λ�����ߵ�ֻ��һλ��
    if(n>=size)reserve(n+1);//Ϊ��λ׼��
    //����δ�����������ֿ�����1234*10000^2+(-239999)*10000^1+23141֮�����ʽ
    if(pData[n-1]<0)//<0ʱ�Ľ�λ����
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
    else//>0�Ľ�λ����
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
    while(pData[n-1]==0&&n>1)n--;//����λ��
}

char *LargeNum::ToStr()
{
    char* dst=new char[pnum->n*4+2];
    stringstream itos;//�����ݴ����
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
        strerr<<"��ͼ���ڿգ�"
#endif
        throw (-1L);
    }
    return *this;
}

LargeNum LargeNum::operator + (const LargeNum &src) const
{
    //��λ��� ������λ
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
    //˼��ͬ�ӷ�
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
    //ȡһλ��λ��� Ȼ��ȥ�ۼӣ���������ʽ��
    LargeNum dst,tmp;
    const LargeNum *pLNa,*pLNb;//pLNa Ϊ������ָ�룬pLNbΪ����ָ��
    dst.pnum->reserve(pnum->n+src.pnum->n);
    tmp.pnum->reserve(pnum->n+src.pnum->n);
    if(pnum->n>src.pnum->n)//����λ���٣���������normalize()������ʱ�临�Ӷ�
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
            throw 1L;//0�ķ��������ݷǷ�
        else
            return LargeNum(0);
    if(*this==1)
        return LargeNum(1);//1���������Ϊ1
    if(src.ifnagative())
        return LargeNum(0);//>1���������ĸ���������<1��������0
    LargeNum dst(1);
    LargeNum ex(src);
    while(ex.ifpositive())//�������˷�
    {
        dst*=(*this);
        --ex;
    }
    return dst;
}

bool LargeNum::operator == (const LargeNum &src) const
{
    if(pnum->n!=src.pnum->n)//λ������ ��Ȼ�������
        return false;
    for(int i=0; i<pnum->n; i++)//��λ�Ƚ�
        if(pnum->pData[i]!=src.pnum->pData[i])
            return false;
    return true;
}
bool LargeNum::ifpositive() const
{
    return pnum->pData[pnum->n-1]>0;//ֻ���ж����λ����
}

bool LargeNum::ifnagative() const
{
    return pnum->pData[pnum->n-1]<0;//ֻ���ж����λ����
}
bool LargeNum::operator > (const LargeNum &src)  const
{
    if(src.pnum->n==pnum->n)//λ�����ʱ ��λ�жϣ����Ų�ͬ�ж���λ���ý����
    {
        for(int i=pnum->n-1; i>0; i--)
            if(pnum->pData[i]!=src.pnum->pData[i])
                return pnum->pData[i]>src.pnum->pData[i];
        return pnum->pData[0]>src.pnum->pData[0];//���һλ ��ʹ��������� Ҳ��false
    }
    else//λ������
        if(pnum->pData[pnum->n-1]<0)
            if(src.pnum->pData[src.pnum->n-1]>0)
                return false;//����<����
            else
                return src.pnum->n>pnum->n;//λ�����С
        else if(src.pnum->pData[src.pnum->n-1]<0)
            return true;//����>����
        else
            return src.pnum->n<pnum->n;//λ����Ĵ�
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
    if(src==0)throw int(-1);//��������Ϊ0���׳��쳣
    //�����������
    if(pnum->n<src.pnum->n)//����λ��С�ڱ����� ֱ�ӷ���
    {
        if(prmd)
            *prmd=(*this);
        return LargeNum(0);
    }
    if(pnum->n<2&&src.pnum->n<2)//1λ��1λ ֱ�ӷ���
    {
        if(prmd)
            *prmd=pnum->pData[0]%src.pnum->pData[0];
        return LargeNum(pnum->pData[0]/src.pnum->pData[0]);
    }
    LargeNum dst;//��
    LargeNum rmd;
    rmd.pnum->copy(*pnum,pnum->n);//����
    dst.pnum->reserve(rmd.pnum->n-src.pnum->n+1);
    dst.pnum->n=dst.pnum->size;
    //nλ����1λ�ĸ߾���
    if(src.pnum->n<2)
    {
        int rmdn=rmd.pnum->n-1;
        while(rmdn>0)
        {
            //ֱ����λ������
            rmd.pnum->pData[rmdn-1]
            +=(*rmd.pnum)[rmdn]*10000;
            rmd.pnum->pData[rmdn]=0;
            rmdn--;
            dst.pnum->pData[rmdn]
            =(*rmd.pnum)[rmdn]/src.pnum->pData[0];
            //��ȥ��λ����
            rmd.pnum->pData[rmdn]-=dst.pnum->pData[rmdn]*src.pnum->pData[0];
        }
    }
    //nλ����nλ�ĸ߾���
    //˼·����ͬnλ����1λ�ĸ߾��� �������̵�ʱ����ý�������ǰ3λ���Գ���ǰ2λ���Գ�
    //Ϊ��ֹint��� ����float�������м�����
    //����������1/10000������������ֵ��С�ڳ�������ֵ
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
    //�÷����õ����̺�������λ�����и� ��Ҫ������
    rmd.pnum->normalize();
    dst.pnum->normalize();
    //������������Ӧ��ͬ��
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
    //�򵥵�����
    if(n<0)return (*this)<<(-n);
    if(n)
    {
        LargeNum dst;
        dst.pnum->reserve(pnum->n+n);//����pnum->pDataΪ��λΪ0�ĳ�Ϊpnum->n+n������
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
    //�򵥵�����
    if(n<0)return (*this)<<(-n);
    if(n)
        if(n<pnum->n)
        {
            LargeNum dst;
            dst.pnum->reserve(pnum->n-n);
            memcpy(dst.pnum->pData,pnum->pData+n,pnum->n-n);
            return dst;
        }
        else//����>=����λ�� �������0
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
    LargeNum dst;//��ʱ����ֱ�ӹ��죬��Ϊ����Ϊ���ã�����������޸�ԭ����
    dst.pnum->copy(*pnum,pnum->size);
    for(int i=0; i<dst.pnum->n; i++)
    {
        dst.pnum->pData[i]=-dst.pnum->pData[i];
    }
    return dst;
}
