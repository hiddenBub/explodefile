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
	if ((p = strstr(base, search)) != NULL)/*pָ��str2��str1�е�һ�γ��ֵ�λ��*/
	{
		while (base != p && base != NULL)/*��str1ָ���ƶ���p��λ��*/
		{
			base++;
		}
		base[0] = '\0';/*��str1ָ��ָ���ֵ���/0,�Դ����ض�str1,����str2���Ժ�����ݣ�ֻ����str2��ǰ������*/
		strcat(base, replace);/*��str1��ƴ����str3,�����str1*/
		strcat(base, strstr(str4, search) + strlen(search));/*strstr(str4,str2)��ָ��str2���Ժ������(����str2),strstr(str4,str2)+strlen(str2)���ǽ�ָ����ǰ�ƶ�strlen(str2)λ������str2*/
	}
	return str4;
}
time_t  Str2Time(char * szTime)
{
	tm tm_;
	time_t t_;
	strptime(szTime, "%Y-%m-%d %H:%M:%S", &tm_); //���ַ���ת��Ϊtmʱ��  
	t_ = mktime(&tm_);                         //��tmʱ��ת��Ϊ��ʱ��  
	return t_;

} //time_t �� can compare directly

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
	
	// ����û�û���������������ʾ������Ϣ���˳�
	if (argc < 3)
	{
		
		
		/**************************************************/
		//cout << "now is " << strtime << endl;
		cout << "No arguments, you MUST give an argument at least!" << endl;
		ShowUsage();

		return -1;
	}

	int nOptionIndex = 1;
	// ����Դ�洢�ļ���
	char *sourcePath;
	// �������·��
	char *outPath;
	// Ԥ��ǰһ���ʱ���ַ���
	char* dateNow = new char[15];
	// Ԥ��ʱ���ַ���
	char *forecast = new char[15];

	while (nOptionIndex < argc)
	{
		// ��ȡ�û�����
		if (strncmp(argv[nOptionIndex], "--sourcePath=", 13) == 0)
		{
			sourcePath = &argv[nOptionIndex][13];
		}
		// ��ȡ�û�ְҵ
		else if (strncmp(argv[nOptionIndex], "--outPath=", 10) == 0)
		{
			outPath = &argv[nOptionIndex][10];
		}
		// ��ȡ�û���Ӫ
		else if (strncmp(argv[nOptionIndex], "--date=", 7) == 0)
		{
			dateNow = &argv[nOptionIndex][7];
		}
		//��ʾ������Ϣ
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
				// д�ļ�������
				if (j != 0)
				{
					char * outFile = new char[100];
					sprintf(outFile, "%s/%s/FY_%s_%s_0530_Forecast_%s.csv", outPath, dateNow, replaceFirst(title[j], "\n", ""), created, front);
					char * outRoot = new char[100];
					sprintf(outRoot, "%s/%s/", outPath, dateNow);
					if (access(outRoot, 0) == -1)//access�����ǲ鿴�ļ��ǲ��Ǵ���
					{
						if (mkdir(outRoot, 0777))//��������ھ���mkdir����������
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






