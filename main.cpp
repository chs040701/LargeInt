#include "num.h"
//#include <Windows.h>
#include <stack>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

char *errdiv0="��ͼ����0��";
char *errmsg="���ʽ����";
char *err0pow0="0�ķ��������ݣ�";

struct int2//ȷ��ĳ�ַ������ִ� ���ڷֻ��沨�����ʽ
{
    int start,length;//��������ʼλ�� ����
    int2(int inta,int intb)
    {
        start=inta;
        length=intb;
    }
};


stringstream strerr;//���ڴ�ų�����ʾ

int judge(string &src)//�����׺���ʽ�Ϸ���
{
    int layer=0;//���ڼ���������
    bool symbol=true;//ǰ���Ƿ��������
	int srclen=src.size();
    for(int i=1; i<srclen-1; i++)
    {
        if(src[i]>='0'&&src[i]<='9')
            symbol=false;
        else if(src[i]=='+'||src[i]=='/'||src[i]=='*'||src[i]=='%'||src[i]=='^')
            if(symbol)
                return -1;//������Ų���ǰ�������
            else
                symbol=true;
        else if(src[i]=='-')
            if(symbol)
				src[i]='o';//��ʾ����
			else 
				symbol=true;
        else if(src[i]=='(')
            if(!symbol)
                return -1;//'('ǰ���Ȼ�������
            else
                layer++;
        else if(src[i]==')')
            if(symbol)
                return -1;//')'ǰΪ����
            else
            {
                if(!(layer--))
                    return -1;
            }
        else return -1;//���쳣����
    }
    if(symbol)//�������ֽ�β||����δ���
        return -1;
    else
        return layer;//��Ҫ��ӵ���������
}

extern "C" char* LNCalculate(char* src1)
//Ϊ�����ⲿ���õĵ��� ���ʽchar*��ַ��DWORD���ͽ��� 
//���ؽ��Ϊ�ַ���ָ�� �ɵ��ñ������ĺ��������ϼ������ͷ�
{
    string inputstr((char*)src1);
    inputstr="("+inputstr+")";//���ʽǰ������� �����жϺϷ��Ժ͹����沨�����ʽ
    //char *src=new char[inputstr.size()];
    //strcpy(src,inputstr.c_str());
	int addnum=judge(inputstr);
    if(addnum<0)
    {
		char *err=new char[20];
		strcpy(err,errmsg);
        return err;
    }
	else
		while(addnum--)//��������
			inputstr+=")";
	//�����沨�����ʽ
    stack<int2> poland;//�����沨�����ʽ��ջ
    queue<int2> polandq;//�洢�沨�����ʽ�Ķ���
    int srclength=inputstr.size();
    for(int i=0; i<srclength;)
    {
        if(inputstr[i]>='0'&&inputstr[i]<='9')//����������ջ
        {
            int l=1;
            while(inputstr[i+l]>='0'&&inputstr[i+l]<='9')
                l++;
            poland.push(int2(i,l));
            i+=l;
        }
		else if(inputstr[i]=='o')//����ֱ����ջ
		{
			poland.push(int2(i++,1));
		}
        else if(inputstr[i]=='+'||inputstr[i]=='-')//�Ӽ������ջ��������ǰ
        {
            while(inputstr[poland.top().start]!='(')
            {
                polandq.push(poland.top());
                poland.pop();
            }
            poland.push(int2(i++,1));
        }
        else if(inputstr[i]=='*'||inputstr[i]=='/'||inputstr[i]=='%')
        {
            while(1)
            {
				char t=inputstr[poland.top().start];
				if(t=='('||t=='+'||t=='-')
					break;
				polandq.push(poland.top());
                poland.pop();
            }
            poland.push(int2(i++,1));
        }
		else if(inputstr[i]=='^')
        {
            while(1)
            {
				char t=inputstr[poland.top().start];
				if(t=='('||t=='+'||t=='-'||t=='*'||t=='/'||t=='%')
					break;
                polandq.push(poland.top());
                poland.pop();
            }
            poland.push(int2(i++,1));
        }
        else if(inputstr[i]=='(')
        {
            poland.push(int2(i++,1));//������ֱ����ջ
        }
        else if(inputstr[i]==')')//�����ų�ջ����ƥ���������ǰ
        {
            while(inputstr[poland.top().start]!='(')
            {
                polandq.push(poland.top());
                poland.pop();
            }
            poland.pop();
            i++;
        }


    }

#ifndef NDEBUG //�����ж��沨�����ʽ�Ƿ񴴽���ȷ ��ʽʹ�ò�ִ��
	
    queue <int2>polandq2(polandq);//����һ���������
	while(!polandq2.empty())
    {
        int2 tmp=polandq2.front();
        for(int i=tmp.start; i<tmp.start+tmp.length; i++)
            cout<<src[i];
        cout<<' ';
        polandq2.pop();
    }
    cout<<endl;
#endif
	//�����沨�����ʽ
    stack<LargeNum> stackLN;//���ڴ洢���ֵ�ջ
    while(!polandq.empty())
    {
        char t=inputstr[polandq.front().start];
		if(t=='o')//�����򵯳�һ����ȡ�෴������ջ
		{
			LargeNum LNtmp=stackLN.top();
            stackLN.pop();
			stackLN.push(LNtmp.opposite());
		}
        else if((t=='+'||t=='-'||t=='*'||t=='/'||t=='%'||t=='^'))//�����������
        {
			//ȡ��������
            LargeNum LNtmp1=stackLN.top();
            stackLN.pop();
            LargeNum LNtmp2=stackLN.top();
            stackLN.pop();
			//ִ�ж�Ӧ������������ջ
            switch(t)
            {
            case '+':
                stackLN.push(LNtmp2+LNtmp1);
                break;
            case '-':
                stackLN.push(LNtmp2-LNtmp1);
                break;
            case '*':
                stackLN.push(LNtmp2*LNtmp1);
                break;
				//'/'��'%'��try...catch..���ڲ������0���쳣
            case '/':
                try
                {
                    stackLN.push(LNtmp2/LNtmp1);
                }
                catch(...)
                {
					char *err=new char[20];
					strcpy(err,errdiv0);
					return err;
                }
                break;
			case '^':
				try//��׽0^0�쳣
                {
                    stackLN.push(LNtmp2^LNtmp1);
                }
                catch(...)
                {
					char *err=new char[20];
					strcpy(err,err0pow0);
					return err;
                }
                break;
            default://'%'
                try
                {
                    stackLN.push(LNtmp2%LNtmp1);
                }
                catch(...)
                {
					char *err=new char[20];
					strcpy(err,errdiv0);
					return err;
                }
                break;
            }

        }
        else 
			stackLN.push(LargeNum(inputstr.substr(polandq.front().start,polandq.front().length).c_str()));
		//�������������ջ
        polandq.pop();//����
    }
    return stackLN.top().ToStr();//���ظ����ֵ��ַ�����ʽ
}

int main()
{
	//��ñ��ʽ
	cout<<"������Ҫ�����ʽ��"<<endl;
	string str;
	cin>>str;
	//����LNCalculate��ý��
	char *output=LNCalculate((char*)str.c_str());
	//���
	cout<<output<<endl;
	if(output[0]>0)delete output;
	system("pause");
	return 0;

}