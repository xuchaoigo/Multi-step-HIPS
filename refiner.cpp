
#include "refiner.h"


std::vector<std::string> split(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
    str+=pattern;
    unsigned int size=str.size();
    for(unsigned int i=0; i<size; i++)
    {
		pos=str.find(pattern,i);
		if(pos<size)
         {
             std::string s=str.substr(i,pos-i);
             result.push_back(s);
             i=pos+pattern.size()-1;
         }
     }
     return result;
}

//test
void print_param(const std::vector<std::string> vec)
{
	//std::cout<<"---------------"<<std::endl;
	//std::cout<<"len="<<vec.size()<<std::endl;	
	for(unsigned int i=0; i<vec.size(); i++)
	{
	//	std::cout<<vec[i]<<std::endl;		
	}
	return;
}

void string_replace(std::string&s1,const std::string&s2,const std::string&s3)
{
	std::string::size_type pos=0;
	std::string::size_type a=s2.size();
	std::string::size_type b=s3.size();
	while((pos=s1.find(s2,pos))!=std::string::npos)
	{
		s1.replace(pos,a,s3);
		pos+=b;
	}
}

void refine(const char * pInputFile,const char * pOutputFile)
{
	if(!pInputFile ||!pOutputFile)
		return;

	//char buffer[LINE_LEN];  
	
	std::ifstream input_file(pInputFile); 
	std::ofstream output_file(pOutputFile);
	if (!input_file.is_open())  
	{ 
		return;
	}  

	std::set<std::string> DsdtSet;
	std::string last_line, last_output_line, sample_path,sample_name;
	std::size_t pos1, pos2;
	while (!input_file.eof())  
	{  
		std::string line;
		std::string output_line;
		std::getline (input_file,line);  
		if(line == "")
			continue;
		if(line == last_line)
			continue;
		else
			last_line = line;

		//std::cout<<"\n*******newline********\n"<<std::endl;
		//std::cout<<line<<std::endl;

		std::vector<std::string> ParamList=split(line,"|#|");
		/*
		for(int i=ParamList.size()-1; i>=0; i--)
		{			
			if(ParamList[i]=="")
				ParamList.pop_back();			
		}
		*/

		print_param(ParamList);

		if(ParamList.size()<3)
			continue;

		if(ParamList[0]=="SECTION")
		{
			
			if(ParamList.size()<5)
				continue;
			string_replace(ParamList[3],"\\Device\\HarddiskVolume1","C:");
			string_replace(ParamList[4],"\\Device\\HarddiskVolume1","C:");
			if(ParamList[4]=="" || ParamList[3]==ParamList[4])
				continue;
			std::vector<std::string>::iterator it3 = ParamList.begin()+3;
			ParamList.erase(it3);
		}	
		else
		{			
			ParamList.pop_back();
		}

		//here we have all 4 params.
		if(ParamList.size()!=4)
			continue;

		if(sample_path=="")
		{
			pos1 = ParamList[3].rfind("\\");
			if(pos1 != std::string::npos)			
				sample_name = ParamList[3].substr(pos1+1,ParamList[3].size());
			else
				continue;
			if(sample_name.size()==36)
				sample_path = ParamList[3];
			else
				continue;
		}
		if(ParamList[2] == sample_path)
			ParamList[2]="$self";
		if(ParamList[3] == sample_path)
			ParamList[3]="$self";		
		if(ParamList[0] == "PROCESS" && ParamList[2] == ParamList[3]) 
			continue;
		if(ParamList[0] == "PROCESS" && ParamList[1] == "0") 
			DsdtSet.insert(ParamList[2]);
		if(ParamList[0] == "FILE" && ParamList[1] == "0") 
			DsdtSet.insert(ParamList[2]);

		std::set<std::string>::iterator it;
		for(it=DsdtSet.begin();it!=DsdtSet.end();it++)
		{
			if(*it==ParamList[2])
			{
				pos1 = ParamList[2].rfind("\\");
				if(pos1 != std::string::npos)
					ParamList[2].replace(pos1+1, ParamList[2].size()-(pos1+1), "$dsdt");
			}
		}

		pos1=ParamList[2].find(":\\");
		pos2=ParamList[2].find("\\Device\\HarddiskVolume");
		if(pos1 != std::string::npos || pos2 != std::string::npos)
		{
			if(ParamList[0] != "REGISTRY")//not REGISTRY:replace the whole path with $other_path
			{
				if(ParamList[2].find("Program Files")== std::string::npos \
				&& ParamList[2].find("WINDOWS")== std::string::npos \
				&& ParamList[2].find("Documents and Settings")== std::string::npos )
					ParamList[2]= "$other_path";
			}
			else//REGISTRY:replace child_path with $other_path.
			{
				if(pos1 != std::string::npos)
				{
					std::string child_path = ParamList[2].substr(pos1-1,ParamList[2].size());
					if(child_path == sample_path)
						ParamList[2].replace(pos1-1, ParamList[2].size()-(pos1-1), "$self");
					else
						ParamList[2].replace(pos1-1, ParamList[2].size()-(pos1-1), child_path.substr(strlen("C:\\"),child_path.size()));
				}
				else
				{
					//ingore \\Device\\HarddiskVolume ,can't decide disk number and it's rare
				}
			}
		}
		
		if(ParamList[0] != "SECTION")
		{
			ParamList.pop_back();
		}
		print_param(ParamList);
		//here, SECTION have 4 params, others have 3 params.
		
		//global replace, from param2 to the end		
		for(int i=2; i< ParamList.size(); i++)
		{
			pos1 = ParamList[i].find("\\S-");
			if(pos1 != std::string::npos)
			{
				pos2 = ParamList[i].find("\\",pos1+1);
				if(pos2 != std::string::npos)
				{
					ParamList[2].replace(pos1+1, pos2-pos1-1, "S-*");
				}
				else
				{
					ParamList[2].replace(pos1+1,ParamList[2].size(), "S-*");
				}
			}

			// TODO:find and clean more garbages here
			pos1 = ParamList[i].find("\\Control\\SafeBoot");
			if(pos1 != std::string::npos)
			{
				ParamList[i] = ParamList[i].substr(0,pos1+strlen("\\Control\\SafeBoot"));
			}
			
			pos1 = ParamList[i].find(":\\");
			if(pos1 != std::string::npos)
			{
				ParamList[i] = ParamList[i].substr(strlen("X:\\"),ParamList[i].size()-strlen("X:\\"));
			}
			pos1 = ParamList[i].find("\\Device\\HarddiskVolume");
			if(pos1 != std::string::npos)
			{
				ParamList[i] = ParamList[i].substr(strlen("\\Device\\HarddiskVolume")+2,ParamList[i].size()-strlen("\\Device\\HarddiskVolume"));
			}
			string_replace(ParamList[i],sample_name,"$self");
			string_replace(ParamList[i],"DOCUME~1","Documents and Settings");
			string_replace(ParamList[i],"LOCALS~1","Local Settings");

			pos1 = ParamList[i].find("Documents and Settings");
			if(pos1 != std::string::npos)
			{
				pos1 += strlen("Documents and Settings\\");
				pos2 = ParamList[i].find("\\",pos1);
				if(pos2 != std::string::npos)
				{
					ParamList[2].replace(pos1, pos2-pos1+1, "");
				}
			}
			print_param(ParamList);

		}
		int i;
		for(i=0; i< ParamList.size()-1; i++)
		{
			output_line += ParamList[i];
			output_line += "|#|";
		}
		output_line += ParamList[i];
		//std::cout<<"----->\n"<<output_line<<std::endl;
		if(output_line == last_output_line)
			continue;
		else
			last_output_line = output_line;

		output_file<<output_line<<std::endl;
	} 
	input_file.close();
	output_file.close();
	return;
}