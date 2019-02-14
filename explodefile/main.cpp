#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <strstream>
#include <string.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;


void ShowUsage()
{
	cout << "Usage   : explode --sourcePath=/home/upload -outPath=/var/files [Option]" << endl;
	cout << "Options :" << endl;
	cout << " --sourcePath=source data file path		path except file name, this option MUST be given" << endl;
	cout << " --outPath=formated file storage path		path except file name, this option MUST be given" << endl;
	cout << " --date=file created date		file created date ,formatter is YYYY-MM-DD" << endl;
	cout << " --help		Print this help." << endl;

	return;
}

int fun(char *str, char *substr) 
{ 
	int n = 0;
	char *p, *r;
	while (*str)
	{ 
		p = str;
		r = substr;
		while (*r)
			if (*r == *p)
			{ 
				r++;
				p++;
			} 
			else break;
		if (*r == '\0')
			n++;
		str++;
	} 
	return n;
}

char * replaceFirst(char *base, char *search, char *replace)
{
	char str4[strlen(base) + 1];
	char *p;
	strcpy(str4, base);
	if ((p = strstr(base, search)) != NULL)/*p指向str2在str1中第一次出现的位置*/
	{
		while (base != p && base != NULL)/*将str1指针移动到p的位置*/
		{
			base++;
		}
		base[0] = '\0';/*将str1指针指向的值变成/0,以此来截断str1,舍弃str2及以后的内容，只保留str2以前的内容*/
		strcat(base, replace);/*在str1后拼接上str3,组成新str1*/
		strcat(base, strstr(str4, search) + strlen(search));/*strstr(str4,str2)是指向str2及以后的内容(包括str2),strstr(str4,str2)+strlen(str2)就是将指针向前移动strlen(str2)位，跳过str2*/
	}
	return str4;
}
time_t  Str2Time(char * szTime)
{
	tm tm_;
	time_t t_;
	strptime(szTime, "%Y-%m-%d %H:%M:%S", &tm_); //将字符串转换为tm时间  
	t_ = mktime(&tm_);                         //将tm时间转换为秒时间  
	return t_;

} //time_t ： can compare directly

time_t time()
{
	time_t seconds = time(NULL);
	return seconds;
}

void date( char* res,time_t seconds,char * formatter)
{
	
	struct tm *p_tm;
	p_tm = localtime(&seconds);
	int size = sizeof(formatter) + 3;
	strftime(res, size, formatter, p_tm);
}

void substring(char *dest, char *src, int start, int end)
{
	int i = start;
	if (start > strlen(src))return;
	if (end > strlen(src))
		end = strlen(src);
	while (i < end)
	{
		dest[i - start] = src[i];
		i++;
	}
	dest[i - start] = '\0';
	return;
}




int main(int argc, char *argv[])
{
	
	// 如果用户没有输入参数，则提示错误信息并退出
	if (argc < 3)
	{
		
		
		/**************************************************/
		//cout << "now is " << strtime << endl;
		cout << "No arguments, you MUST give an argument at least!" << endl;
		ShowUsage();

		return -1;
	}

	int nOptionIndex = 1;
	// 数据源存储文件夹
	char *sourcePath;
	// 数据输出路径
	char *outPath;
	// 预报前一天的时间字符串
	char* dateNow = new char[15];
	// 预报时间字符串
	char *forecast = new char[15];

	while (nOptionIndex < argc)
	{
		// 获取用户姓名
		if (strncmp(argv[nOptionIndex], "--sourcePath=", 13) == 0)
		{
			sourcePath = &argv[nOptionIndex][13];
		}
		// 获取用户职业
		else if (strncmp(argv[nOptionIndex], "--outPath=", 10) == 0)
		{
			outPath = &argv[nOptionIndex][10];
		}
		// 获取用户阵营
		else if (strncmp(argv[nOptionIndex], "--date=", 7) == 0)
		{
			dateNow = &argv[nOptionIndex][7];
		}
		//显示帮助信息
		else if (strncmp(argv[nOptionIndex], "--h", 2) == 0)
		{
			ShowUsage();
			return 0;
		}
		else
		{
			cout << "Options '" << argv[nOptionIndex] << "' not valid. Run '" << argv[0] << "' for details." << endl;
			return -1;
		}
		nOptionIndex++;
	}
	int len = strlen(dateNow);
	if (strlen(sourcePath) == 0 || strlen(outPath) == 0)
	{
		ShowUsage();

		return -1;
	}
	time_t now;
	if (len == 0)
	{
		now = time();
		date(dateNow, now,"%Y-%m-%d");
	}
	else
	{
		char tm_str[sizeof(dateNow)+10];
		strcpy(tm_str, dateNow);
		strcat(tm_str, " 00:00:00");
		now = Str2Time(tm_str);
	}
	time_t forecastTime = now + (24 * 60 * 60);
	date(forecast, forecastTime, "%Y-%m-%d");

	char * filename = new char[100];
	sprintf(filename, "%s/meteomatics_forecast_%s.csv", sourcePath, forecast);

	int i = 1;

	int LEN = 1024;
	FILE *ofp;
	char *title[31];
	char ch, string[LEN], string1[LEN], stringTemp[LEN];
	FILE *ifp;
	if (!(ofp = fopen(filename, "r")))
	{
		printf("Can not open %s\n", filename);
	}
	else
	{
		char * created = new char[15], *front = new char[15];
		date(created, now, "%Y%m%d");
		

		date(front, forecastTime, "%Y%m%d");
		if ((fgets(string1, LEN, ofp)) != NULL)
		{
			char * cur = string1;
			int count = fun(string1, ",");
			char *ip_arr[count + 1];
			char * s = strsep(&cur, ",");
			int j = 0;
			printf("writing line %d now", i);
			while (s)
			{

				replaceFirst(s, "\n", "\0");

				title[j] = s;
				// 写文件标题栏
				if (j != 0)
				{
					char * outFile = new char[100];
					sprintf(outFile, "%s/%s/FY_%s_%s_0530_Forecast_%s.csv", outPath, dateNow, replaceFirst(title[j], "\n", ""), created, front);
					char * outRoot = new char[100];
					sprintf(outRoot, "%s/%s/", outPath, dateNow);
					if (access(outRoot, 0) == -1)//access函数是查看文件是不是存在
					{
						if (mkdir(outRoot, 0777))//如果不存在就用mkdir函数来创建
						{
							printf("create directory failed!!!");
						}
					}
					printf("Writing File %s.\n", outFile);
					char * line = new char[40];
					if ((ifp = fopen(outFile, "w")))
					{
						
						sprintf(line, "%s,%s\n", title[0], "Wind (m/s)");
						if (!fputs(line, ifp))
							printf("success\n");
						else
							printf("failed\n");
						fclose(ifp);
					}
				}

				j++;
				s = strsep(&cur, ",");
			}
		}
		i++;
		printf("The content of %s is:", filename);

		while ((fgets(stringTemp, LEN, ofp)) != NULL)
		{
			printf("%s\n", stringTemp);
			if (strlen(stringTemp) != 1)
			{
				char * cur = stringTemp;
				int count = fun(stringTemp, ",");
				char *ip_arr[count + 1];
				char * s = strsep(&cur, ",");
				int j = 0;
				printf("writing line %d now\n", i);
				while (s)
				{
					
					if (i != 1)
					{
						replaceFirst(s, "\n", "\0");
						ip_arr[j] = s;
						if (j != 0)
						{

							char * outFile = new char[100];
							//FY_EEE1048_20181112_0530_Forecast20181113.csv
							sprintf(outFile, "%s/%s/FY_%s_%s_0530_Forecast_%s.csv", outPath, dateNow, replaceFirst(title[j], "\n", ""), created, front);
							char * line = new char[40];
							if ((ifp = fopen(outFile, "a")))
							{
								
								sprintf(line, "%s,%s\n", ip_arr[0], s);
								if (!fputs(line, ifp))
									printf("success\n");
								else
									printf("failed\n");
								fclose(ifp);
							}

						}
					}
					j++;
					s = strsep(&cur, ",");
				}
				i++;
				printf("\n");
			}
		}
		fclose(ofp);
	}
	

	//cout << endl;
	//ifile.close();
	
	

	return 0;
	
}

char * str2charP(string str)
{
	const char * pstr = str.c_str();
	char *p = new char[strlen(pstr) + 1];
	int count = 0;
	strcpy(p, pstr);
	return p;
}






