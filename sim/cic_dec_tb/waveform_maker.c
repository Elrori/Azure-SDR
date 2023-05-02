#include <stdio.h>
#include <math.h>

#define PI 3.1415926
#define DEPTH 2000     /*数据深度，即存储单元的个数*/
#define WIDTH 10       /*存储单元的宽度*/
#define K 0.5          /*幅度缩放*/
int main(void)
{
    int i,temp;
    float s,s2;

    FILE *fp;
	FILE *fp2;
    fp = fopen("sine.txt","w");   
	fp2= fopen("sine_int.txt","w");   
    if(NULL==fp)
        printf("Can not creat file!\r\n");
    else
    {
        printf("File created successfully!\n");
        /*
        * 以十六进制输出
        */
        for(i=0;i<DEPTH;i++)
        {
            s = sin(2*PI*i/DEPTH);   //一个周期2000个采样点，采样时间 1/20MHz，正弦波f=10KHz
			s2= sin(20*2*PI*i/DEPTH);//叠加一个高频信号f=200KHz.CIC 截止频率在 20MHz/R/2以内，因此R必须大于50
			s=s+s2;
            temp = (int)(K*s*((float)((1<<WIDTH)-1)/2));
            fprintf(fp,"%x\n",temp&((1<<WIDTH)-1));
			fprintf(fp2,"%d\n",temp);
			printf("%d ",temp);
        }
        fclose(fp);
		fclose(fp2);
    }
}