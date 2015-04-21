#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>

using namespace std;

string mn;

void toLower(string &s)
{
	for(string::iterator iter=s.begin();iter!=s.end();++iter)
	{
		*iter = tolower(*iter);
	}
}

inline bool startswith(string &a,string &b)
{
	if(a.size()<b.size())
	{
		return false;
	}
	return a.substr(0,b.size())==b;
}

void doObj(string &fp,string &fn,string &nd)
{
	ifstream in(fp.c_str());
	ofstream out((nd+"\\"+fn+".obj").c_str());
	string usemtl("usemtl"),f("f"),mtllib("mtllib"),now,pre;
	int c = 0;

	while(getline(in,now))
	{
		now+="\n";
		if(startswith(now,usemtl)&&startswith(pre,f))
		{
			out<<"o "<<c<<"\n";
			out<<now;
		}
		else if(startswith(now,mtllib))
		{
			istringstream is(now);            
			is>>mn>>mn;
			out<<now;
		}
		else
		{
			out<<now;
		}
		++c;
		pre = now;
	}

	in.close();
	out.close();
}

void doMtl(string &cd,string &mn,string &nd)
{
	ifstream in((cd+"\\"+mn).c_str());
	ofstream out((nd+"\\"+mn).c_str());
	string map_Ka("map_Ka"),map_Kd("map_Kd"),line,word;

	while(getline(in,line))
	{
		line+="\n";
		if(line.find(map_Ka)!=string::npos)
		{
			line = string(line.begin()+line.rfind("\\")+1,line.begin()+line.rfind("."));
			line+=".bmp\n";
			out<<"\t"<<map_Ka<<" "<<line;
		}
		else if(line.find(map_Kd)!=string::npos)
		{
			line = string(line.begin()+line.rfind("\\")+1,line.begin()+line.rfind("."));
			line+=".bmp\n";
			out<<"\t"<<map_Kd<<" "<<line;
		}
		else
		{
			out<<line;
		}
	}

	in.close();
	out.close();
}

int main(int argc,char **argv)
{
	string cd,fp,fn,nd;
	size_t index;

	if(argc<2)
	{
		cout<<"使用方法: 拖动OBJ模型文件到此软件图标上！"<<endl;
		system("pause");
		return 0;
	}
	else if(GetFileAttributes(argv[1])&FILE_ATTRIBUTE_DIRECTORY)
	{
		cout<<"你拖动进来的是文件夹不是OBJ模型文件！"<<endl;
		system("pause");
		return 0;
	}
	else
	{
		string file(argv[1]);
		toLower(file);
		if(file.rfind(".obj")!=file.size()-4)
		{
			cout<<"请确保拖进来的是OBJ模型文件，并以.obj或.OBJ为后缀！"<<endl;
			system("pause");
			return 0;
		}
	}

	fp = argv[1];
	toLower(fp);
	index = fp.rfind("\\");
	cd = fp.substr(0,index);
	fn = fp.substr(index+1,fp.rfind(".obj")-index-1);
	nd = cd+"\\"+fn+"_new";

	system(("md "+nd).c_str());
	doObj(fp,fn,nd);
	if(!mn.empty())
	{
		doMtl(cd,mn,nd);
	}

	cout<<"模型处理成功！并已存储至："<<nd<<"目录下"<<endl;
	system("pause");
	return 0;
}
