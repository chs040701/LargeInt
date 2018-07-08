#include "num.h"
//#include <Windows.h>
#include <stack>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

char *errdiv0="试图除以0！";
char *errmsg="表达式错误";
char *err0pow0="0的非正数次幂！";

struct int2//确立某字符串的字串 用于分划逆波兰表达式
{
    int start,length;//参量：起始位置 长度
    int2(int inta,int intb)
    {
        start=inta;
        length=intb;
    }
};


stringstream strerr;//用于存放出错提示

int judge(string &src)//检查中缀表达式合法性
{
    int layer=0;//用于检查括号配对
    bool symbol=true;//前面是否是运算符
	int srclen=src.size();
    for(int i=1; i<srclen-1; i++)
    {
        if(src[i]>='0'&&src[i]<='9')
            symbol=false;
        else if(src[i]=='+'||src[i]=='/'||src[i]=='*'||src[i]=='%'||src[i]=='^')
            if(symbol)
                return -1;//运算符号不能前置运算符
            else
                symbol=true;
        else if(src[i]=='-')
            if(symbol)
				src[i]='o';//表示负号
			else 
				symbol=true;
        else if(src[i]=='(')
            if(!symbol)
                return -1;//'('前面必然是运算符
            else
                layer++;
        else if(src[i]==')')
            if(symbol)
                return -1;//')'前为数字
            else
            {
                if(!(layer--))
                    return -1;
            }
        else return -1;//有异常符号
    }
    if(symbol)//不以数字结尾||括号未配对
        return -1;
    else
        return layer;//需要添加的右括号数
}

extern "C" char* LNCalculate(char* src1)
//为方便外部调用的调试 表达式char*地址以DWORD类型接收 
//返回结果为字符串指针 由调用本函数的函数或其上级负责释放
{
    string inputstr((char*)src1);
    inputstr="("+inputstr+")";//表达式前后加括号 利于判断合法性和构造逆波兰表达式
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
		while(addnum--)//补足括号
			inputstr+=")";
	//构造逆波兰表达式
    stack<int2> poland;//构造逆波兰表达式的栈
    queue<int2> polandq;//存储逆波兰表达式的队列
    int srclength=inputstr.size();
    for(int i=0; i<srclength;)
    {
        if(inputstr[i]>='0'&&inputstr[i]<='9')//是数字则入栈
        {
            int l=1;
            while(inputstr[i+l]>='0'&&inputstr[i+l]<='9')
                l++;
            poland.push(int2(i,l));
            i+=l;
        }
		else if(inputstr[i]=='o')//负号直接入栈
		{
			poland.push(int2(i++,1));
		}
        else if(inputstr[i]=='+'||inputstr[i]=='-')//加减法则出栈到左括号前
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
            poland.push(int2(i++,1));//左括号直接入栈
        }
        else if(inputstr[i]==')')//右括号出栈到相匹配的左括号前
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

#ifndef NDEBUG //用于判断逆波兰表达式是否创建正确 正式使用不执行
	
    queue <int2>polandq2(polandq);//复制一份用于输出
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
	//计算逆波兰表达式
    stack<LargeNum> stackLN;//用于存储数字的栈
    while(!polandq.empty())
    {
        char t=inputstr[polandq.front().start];
		if(t=='o')//负号则弹出一个数取相反数后入栈
		{
			LargeNum LNtmp=stackLN.top();
            stackLN.pop();
			stackLN.push(LNtmp.opposite());
		}
        else if((t=='+'||t=='-'||t=='*'||t=='/'||t=='%'||t=='^'))//如若是运算符
        {
			//取出两数字
            LargeNum LNtmp1=stackLN.top();
            stackLN.pop();
            LargeNum LNtmp2=stackLN.top();
            stackLN.pop();
			//执行对应操作并重新入栈
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
				//'/'和'%'中try...catch..用于捕获除以0的异常
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
				try//捕捉0^0异常
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
		//如果是数字则入栈
        polandq.pop();//出队
    }
    return stackLN.top().ToStr();//返回该数字的字符串形式
}

int main()
{
	//获得表达式
	cout<<"请输入要计算的式子"<<endl;
	string str;
	cin>>str;
	//调用LNCalculate获得结果
	char *output=LNCalculate((char*)str.c_str());
	//输出
	cout<<output<<endl;
	if(output[0]>0)delete output;
	system("pause");
	return 0;

}