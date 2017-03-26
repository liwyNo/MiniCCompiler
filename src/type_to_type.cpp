#include<bits/stdc++.h>
#include"symbol.h"
#define debug(x) cerr<<#x<<"="<<x<<endl

/*
typedef enum {
    idt_char, idt_short, idt_int, idt_long, 
    idt_uchar, idt_ushort, idt_uint, idt_ulong, 
    idt_float, idt_double, 
    idt_pointer, idt_fpointer, 
    idt_void, 
    idt_array, 
    idt_struct, idt_union, 
    idt_enum 
} IdType_t;
*/
/*
char + short = int
ushort + int = int
uchar + ushort = int
uint + ulong = ulong
ushort + uint = uint
char + uint = uint
int + uint = uint
double >> float >> ulong >> long >> uint >> int
pointer(array) >>  ulong >> long >> uint >> int
fpointer >>  ulong >> long >> uint >> int
*/
int prrt[10]={idt_double,idt_float,idt_ulong,idt_long,idt_uint,idt_int};//priority
int ptr[3]={idt_pointer,idt_fpointer,idt_array};
int rel[IDTYPE_NUM][IDTYPE_NUM];
int main()
{
    //不会有enum类型的变量，不用处理！
    freopen("type_to_type_arr.txt","w",stdout);
    memset(rel,-1,sizeof(rel));
    int i,j,k;
    //数字之间按优先级处理
    for(i=idt_char;i<=idt_double;i++)
        for(j=i;j<=idt_double;j++)
        {
            for(k=0;k<6;k++)
                if(i==prrt[k]||j==prrt[k])
                {
                    rel[i][j]=rel[j][i]=prrt[k];
                    //printf("[%d][%d]=%d,\n",i,j,prrt[k]);
                    //if(i!=j)
                        //printf("[%d][%d]=%d,\n",j,i,prrt[k]);
                    break;
                }
            if(k==6)
                if(i!=j)
                {
                    rel[i][j]=rel[j][i]=idt_int;
                    //printf("[%d][%d]=%d,\n",i,j,idt_int);
                    //printf("[%d][%d]=%d,\n",j,i,idt_int);
                }                    
                else
                {
                    rel[i][j]=rel[j][i]=i;
                    //printf("[%d][%d]=%d,\n",i,j,i);
                    //printf("[%d][%d]=%d,",j,i,i);
                }        
        }
    
    //整数和指针相运算，全部还是指针
    for(k=0;k<3;k++)
    {
        i=ptr[k];
        rel[i][i]=idt_int;
        //printf("[%d][%d]=%d,\n",i,i,idt_int);//同类自己的指针其实只能互相减法，得到int
        for(j=idt_char;j<=idt_ulong;j++)
        {
            rel[i][j]=rel[j][i]=i;
            //printf("[%d][%d]=%d,\n",i,j,i);
            //printf("[%d][%d]=%d,\n",j,i,i);
        }
    }
    printf("{");
    for(i=0;i<IDTYPE_NUM;i++)
    {
        printf("{");
        for(j=0;j<IDTYPE_NUM;j++)
        {
            if(j==0)
                printf("(Idtype_t)%d",rel[i][j]);
            else printf(",(Idtype_t)%d",rel[i][j]);
        }
        if(i==IDTYPE_NUM-1)
            printf("}\n");
        else printf("},\n");
        
    }
    printf("}");
}