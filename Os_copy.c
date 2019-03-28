//time:20190328
//auther:lizhiyang
//function:Copy the files modified before the specified time in the original folder to the destination folder.


#include<iostream>
#include<string>
#include<fstream>
#include<direct.h> 
#include <windows.h>
using namespace std;

const int buffer_size = 10000;

int main()
{
	//参数含义：源文件路径，目的文件路径，用户输入的时间（转换为秒数）；
	bool copyfile(string source,string target,int mysecond);
	void findfile(string source, string target, int mysecond);
	int cmputsecond(int year, int month, int day, int hour, int minute, int second);
	
	int year, month, day, hour, minute, second;
	string sourcefilenamewithpath;
	string targetfilenamewithpath;
	//输入源目录和目标目录；
	cout << "原始目录:" << endl;
	cin >> sourcefilenamewithpath;
	cout << "指定目录:" << endl;
	cin >> targetfilenamewithpath;
	cout << "请输入日期,在此日期前修改的文件都将被拷贝到指定目录，依次输入年、月、日、时、分、秒：" << endl;
	cin >> year >> month >> day >> hour >> minute >> second;
	int mysecond=cmputsecond(year ,month, day, hour, minute, second);
	findfile(sourcefilenamewithpath, targetfilenamewithpath,mysecond);
	system("pause");
	return 0;
}

	//计算文件的国际标准时间时间：自1970年1月1日00:00:00（国际标准时间）以来所经过的秒数，其中世界时=北京时间-8小时
int cmputsecond(int year, int month, int day, int hour, int minute, int second)
{
	int rem, quo,days=0,seconds=0;//rem为余数，quo为商；
	quo = (year - 1970) / 4;
	rem = year - quo*4-1970;
	days = (365 * 3 + 366)*quo+ rem * 365;
	switch (month) {
	case 12:days += 30;
	case 11:days += 31;
	case 10:days += 30;
	case 9:days += 31;
	case 8:days += 31;
	case 7:days += 30;
	case 6:days += 31;
	case 5:days += 30;
	case 4:days += 31;
	case 3:days += 28;
	case 2:days += 31;
	default:days = days; break;
}
	//在余数年分判断闰月是否出现；
	if (rem > 2 || (rem == 2 && month > 2))
	{
		days++;
	}
	days= days + day-1;
	//世界时=北京时间-8小时
	seconds = ((days * 24+hour-8)*60+minute)*60+second;
	return seconds;
}

	//递归遍历文件夹，寻找文件
void findfile(string source, string target, int mysecond)
{
	bool copyfile(string source, string target, int mysecond);
	HANDLE hFind;
	WIN32_FIND_DATA findData;
	LARGE_INTEGER size;
	char dirNew[100];
	char dirNew_dst[100];

	// 向目录加通配符，用于搜索第一个文件 
	strcpy(dirNew, source.c_str());
	strcat(dirNew, "\\*.*");
	strcpy(dirNew_dst, target.c_str());


	//查找第一个文件or文件夹
	if (!(hFind = FindFirstFile(dirNew, &findData)))
	{
		cout << "一个文件/文件夹也没有" << endl;
		return;
	}
		
	do
	{
		// 是文件夹，并且名称不为"."或".." 
		if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
			&& strcmp(findData.cFileName, ".") != 0
			&& strcmp(findData.cFileName, "..") != 0
			)
		{
			// 将dirNew设置为搜索到的目录，并进行递归搜索 
			strcpy(dirNew, source.c_str());
			strcat(dirNew, "\\");
			strcat(dirNew, findData.cFileName);
			strcpy(dirNew_dst, target.c_str());
			strcat(dirNew_dst, "\\");
			strcat(dirNew_dst, findData.cFileName);
			_mkdir(dirNew_dst);
			findfile(dirNew, dirNew_dst,mysecond);
		}
		//是文件
		else if (strcmp(findData.cFileName, ".") != 0
			&& strcmp(findData.cFileName, "..") != 0)
		{
			strcpy(dirNew, source.c_str());
			strcat(dirNew, "\\");
			strcat(dirNew, findData.cFileName);
			strcpy(dirNew_dst, target.c_str());
			strcat(dirNew_dst, "\\");
			strcat(dirNew_dst, findData.cFileName);
			if (!(copyfile(dirNew, dirNew_dst, mysecond)))
			{
				cout << dirNew << "   to   " << dirNew_dst << " wrong " << endl;
				break;
			}
		
		}
	} while (FindNextFile(hFind, &findData));
}

//复制文件函数，通过fread和fwrite函数实现复制
bool copyfile(string source, string target, int mysecond)
{
	FILE *fpr;//指向source文件
	FILE *fpw;//指向target文件
	struct stat buf;
	char buffer[buffer_size];//缓冲区
	int lenr, lenw;//lenr为source文件读到缓冲区的字节数，lenr为从缓冲区写到target文件的字节数

	memset(buffer, 0, buffer_size);
	if ((fpr = fopen(source.c_str(), "rb")) == NULL)
	{
		cout << "The file:" << source << " can not be opened!" << endl;
		fclose(fpr);
		return false;
	}
	//buf为打开文件的状态信息
	int fd = _fileno(fpr);
	fstat(fd, &buf);
	//buf.st_mtime表示文件自1970年1月1日00:00:00（国际标准时间）以来时间所经过的秒数
	//比较文件时间和用户输入时间，如果文件最后修改时间在输入时间后，则不拷贝此文件
	if (mysecond < buf.st_mtime) {
		fclose(fpr);
		return true;
	}
	if ((fpw= fopen(target.c_str(), "wb")) == NULL)
	{
		cout << "The file:" << target << " can not be opened!" << endl;
		fclose(fpr);
		return false;
	}
	while ((lenr=fread(buffer,1,buffer_size,fpr))>0)
	{
		if ((lenw = fwrite(buffer, 1, lenr, fpw)) != lenr)
		{
			cout << lenw << endl;
			cout << "Write to file:" << target << "failed" << endl;
			fclose(fpr);
			fclose(fpw);
			return false;
		}
		memset(buffer, 0, buffer_size);
	}
	fclose(fpr);
	fclose(fpw);
	return true;
}
