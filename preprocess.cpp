#include "preprocess.h"

MSHIPSClassifier::MSHIPSClassifier()
{
	Ngram = 2;
	operation_id.insert(std::make_pair("PROCESS","0"));
	operation_id.insert(std::make_pair("REGISTRY","1"));
	operation_id.insert(std::make_pair("FILE","2"));
	operation_id.insert(std::make_pair("INJECT","3"));
	operation_id.insert(std::make_pair("SERVICE","4"));	
	operation_id.insert(std::make_pair("DRIVER","5"));
	operation_id.insert(std::make_pair("MEMORY","6"));
	operation_id.insert(std::make_pair("THREAD","7"));
	operation_id.insert(std::make_pair("SECTION","8"));
	operation_id.insert(std::make_pair("OTHER","9"));
	operation_id.insert(std::make_pair("PORT","10"));
	operation_id.insert(std::make_pair("WINDOW","11"));
	operation_id.insert(std::make_pair("TOKEN","12"));
	operation_id.insert(std::make_pair("PIPE","13"));
	operation_id.insert(std::make_pair("EVENT","14"));
	operation_id.insert(std::make_pair("MUTUNT","15"));
	operation_id.insert(std::make_pair("SEMAPHORE","16"));

	Threshold[0] = 0.6;
	Threshold[1] = 0.7;
	Threshold[2] = 0.6;
	Threshold[3] = 0.6;
	Threshold[4] = 0.6;
	Threshold[5] = 0.6;
	Threshold[6] = 0.6;
	Threshold[7] = 0.6;
	Threshold[8] = 0.6;
	Threshold[9] = 0.6;
	Threshold[10] = 0.6;
	Threshold[11] = 0.6;
	Threshold[12] = 0.6;
	Threshold[13] = 0.6;
	Threshold[14] = 0.6;
	Threshold[15] = 0.6;
	Threshold[16] = 0.6;

	delimiter_arg="|";                                                                       
	delimiter_behavior="<BH>";                                                                   
	delimiter_count="<CN>";                                                                       
	delimiter_target="#";                                                                         
	delimiter_table="<TB>";                                                                       
	delimiter_feature="<FE>";                                                                     
	delimiter_item="<IT>";                                                                        
	delimiter_table_name="$";   
	delimiter_fet_quality="<FET>";
	return;

}
MSHIPSClassifier::~MSHIPSClassifier()
{
	return;
}

std::vector<std::string> MSHIPSClassifier::split(std::string str,std::string pattern)
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
void MSHIPSClassifier::print_param(const std::vector<std::string> vec)
{
	//std::cout<<"---------------"<<std::endl;
	//std::cout<<"len="<<vec.size()<<std::endl;	
	for(unsigned int i=0; i<vec.size(); i++)
	{
		//	std::cout<<vec[i]<<std::endl;		
	}
	return;
}

void MSHIPSClassifier::string_replace(std::string&s1,const std::string&s2,const std::string&s3)
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

std::string MSHIPSClassifier::get_table_name(std::string &line)
{
	//1#8#REGISTRY|USER|S-*|Keyboard Layout|Toggle|Language Hotkey<BH>1#2#REGISTRY|USER|S-*|Software|Microsoft|Windows|CurrentVersion|Policies|Explorer|NoRun<CN>1
	std::string table_name = "";
	std::size_t pos1,pos2;
	std::vector<std::string> OneBehaviorList=split(line,delimiter_behavior);
	for(unsigned int i=0; i<OneBehaviorList.size(); i++)
	{
		pos1=OneBehaviorList[i].find(delimiter_target.c_str(),0);
		pos2=OneBehaviorList[i].find(delimiter_target.c_str(),pos1+1);
		if(pos1 == std::string::npos ||pos2 == std::string::npos )
		{
			std::cout<<"invalid Behavior"<<std::endl;
			continue;
		}
		std::string major,minor;
		major = OneBehaviorList[i].substr(0,pos1);
		minor = OneBehaviorList[i].substr(pos1+1,pos2-(pos1+1));
		table_name +=major;
		table_name +="_";
		table_name +=minor;		

		//table_name_extend is used after itr G
		std::string table_name_extend = "A";
		std::vector<std::string> ArgList=split(OneBehaviorList[i].substr(pos2+1,OneBehaviorList[i].size()),delimiter_arg);
		if(ArgList.size() >= 5)
		{
			if(major == "1")
			{
				if(ArgList[0] == "REGISTRY")
				{
					if(ArgList[1] == "MACHINE")
					{
						if(ArgList[2] == "SOFTWARE")
						{
							if(ArgList[3] == "Classes")
							{
								if(ArgList[4] == "CLSID")
									table_name_extend = "MSCC";
								else
									table_name_extend = "MSCO";
							}
							else if(ArgList[3] == "Microsoft")
								table_name_extend = "MSM";
							else
								table_name_extend = "MSO";
						}
						else if(ArgList[2] == "SYSTEM")
							table_name_extend = "MY";
						else
							table_name_extend = "MO";
					}
					else if(ArgList[1] == "USER")
						table_name_extend = "U";
					else
						table_name_extend = "O";
				}
				else
					table_name_extend = "X";
			}
			else if(major == "2")
			{
				if(ArgList[0] == "Documents and Settings")
				{
					if(ArgList[1] == "Local Settings")
						table_name_extend = "DL";
					else
						table_name_extend = "DO";
				}
				else if(ArgList[0] == "WINDOWS")
					table_name_extend = "W";
				else if(ArgList[0] == "Program Files")
					table_name_extend = "P";
				else
					table_name_extend = "O";
			}
		}
		table_name +="_";
		table_name +=table_name_extend;	

		if(i!=OneBehaviorList.size()-1)
			table_name += delimiter_table_name;
	}


	return table_name;
}
void MSHIPSClassifier::MshipsRefine(const char * pInputFile,const char * pOutputFile)
{
	if(!pInputFile ||!pOutputFile)
		return;
	std::ifstream input_file(pInputFile); 
	std::ofstream output_file(pOutputFile);
	if (!input_file.is_open())  
		return;	

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
		for(unsigned int i=2; i< ParamList.size(); i++)
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
		unsigned int i;
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

bool less_mships_index(const std::string& str1, const std::string& str2)
{
	int num1,num2;
	std::stringstream ss;
	ss << str1.substr(0,str1.find(":"));
	ss >> num1;
	ss.clear();
	ss.str("");
	ss << str2.substr(0,str2.find(":"));
	ss >> num2;
	return num1 < num2;
}


std::map<std::string,std::vector<std::pair<std::string,std::string>> >g_mships_global_feature;
std::string g_mships_current_table_name;
int feature_num = 0;

int MSHIPSClassifier::callback_get_feature(void *NotUsed, int argc, char **argv, char **azColName)
{   
	//i=2,argv[0]:id, argv[1]:Target	
	if(g_mships_current_table_name=="")
		return 1;
	std::pair<std::string,std::string> feature;
	feature = std::make_pair(argv[0],argv[1]);

	g_mships_global_feature[g_mships_current_table_name].push_back(feature);
	//std::cout<<"table:"<<g_mships_current_table_name<<std::endl;
	//std::cout<<"id:"<<argv[0]<<std::endl;
	//std::cout<<"fet:"<<argv[1]<<std::endl;
	feature_num++;
	return 0;  
}  


int MSHIPSClassifier::callback_get_tables(void *NotUsed, int argc, char **argv, char **azColName)
{  
	g_mships_global_feature[argv[0]];
	return 0;  
}  

double MSHIPSClassifier::SimilarSingleArg(const std::string& line1,const std::string& line2)
{

	std::vector<std::string> ArgList1 = split(line1, delimiter_arg);
	std::vector<std::string> ArgList2 = split(line2, delimiter_arg);
	std::size_t arg_len1 = ArgList1.size();
	std::size_t arg_len2 = ArgList2.size();

	if(abs(static_cast<int>(arg_len1 - arg_len2)) >= 3)
		return 0;
	std::size_t arg_len_min = arg_len1>arg_len2?arg_len2:arg_len1;
	if(arg_len_min ==0)
		return 0;
	float similarity_sum=0;
	for(unsigned int i=0;i<arg_len_min;i++)
	{
		if(ArgList1[i]==ArgList2[i])
		{
			similarity_sum += (1/static_cast<float>(arg_len1)+1/static_cast<float>(arg_len2))/2;
			//std::cout<<"similarity_sum: "<<similarity_sum<<std::endl;
		}
		else
			break;
	}	
	return similarity_sum;
}
bool MSHIPSClassifier::SimilarOneBehavior(std::string& line1,std::string& line2)
{
	//cut off table name
	std::size_t pos1,pos2;

	pos1 = line1.find(delimiter_target,0);
	pos2 = line1.find(delimiter_target,pos1+1);
	std::string major_str;
	int major_num;
	std::stringstream ss;
	major_str = line1.substr(0,pos1);
	ss << major_str;
	ss >> major_num;
	line1 = line1.substr(pos2+1,line1.size());

	pos1 = line2.find(delimiter_target,0);
	pos2 = line2.find(delimiter_target,pos1+1);
	line2 = line2.substr(pos2+1,line2.size());

	//split targets(SECTION).
	std::vector<std::string> TargetList1 = split(line1, delimiter_target);
	std::vector<std::string> TargetList2 = split(line2, delimiter_target);
	if(TargetList1.size()!=TargetList2.size())
		return false;
	std::size_t target_num = TargetList1.size();
	double weight,similarity_value;
	weight = 1.0/target_num;
	similarity_value = 0;

	for(unsigned int i=0;i<target_num;i++)
	{
		similarity_value += SimilarSingleArg(TargetList1[i],TargetList2[i]);
		if(similarity_value >=Threshold[major_num])
		{
			return true;
		}
	}
	return false;
}

bool MSHIPSClassifier::SimilarNgram(std::string line1,std::string line2)
{
	//1_8$3_3<TB>1#8#REGISTRY|MACHINE|SOFTWARE|Policies|Microsoft|Windows|Safer|CodeIdentifiers|TransparentEnabled<BH>3#3#WINDOWS|system32|MSCTF.dll
	std::size_t pos1,pos2;
	pos1=line1.find(delimiter_table.c_str(),0);
	pos2=line2.find(delimiter_table.c_str(),0);
	if(pos1 == std::string::npos ||pos2 == std::string::npos )
	{
		std::cout<<"Similar_N_gram:invalid Behavior"<<std::endl;
		return false;
	}
	if(line1.substr(0,pos1)!=line2.substr(0,pos2))
		return false;

	line1 = line1.substr(pos1+delimiter_table.size(),line1.size());
	line2 = line2.substr(pos2+delimiter_table.size(),line2.size());
	
	std::vector<std::string> OneBehaviorList1=split(line1,delimiter_behavior);
	std::vector<std::string> OneBehaviorList2=split(line2,delimiter_behavior);
	if(OneBehaviorList1.size()!=OneBehaviorList2.size())
		return false;

	for(unsigned int i=0; i<OneBehaviorList1.size(); i++)
	{
		if (!SimilarOneBehavior(OneBehaviorList1[i],OneBehaviorList2[i]))
			return false;
	}
	return true;
}

bool MSHIPSClassifier::SimilarNgram_legacy(std::string line1,std::string line2)
{
	//1#8#REGISTRY|MACHINE|SOFTWARE|Policies|Microsoft|Windows|Safer|CodeIdentifiers|TransparentEnabled<BH>3#3#WINDOWS|system32|MSCTF.dll
	std::vector<std::string> OneBehaviorList1=split(line1,delimiter_behavior);
	std::vector<std::string> OneBehaviorList2=split(line2,delimiter_behavior);
	if(OneBehaviorList1.size()!=OneBehaviorList2.size())
		return false;

	for(unsigned int i=0; i<OneBehaviorList1.size(); i++)
	{
		if (!SimilarOneBehavior(OneBehaviorList1[i],OneBehaviorList2[i]))
			return false;
	}
	return true;
}

void MSHIPSClassifier::MshipsLog2mist(const char * pInputFile)
{
	std::cout<<"***MshipsLog2mist in"<<std::endl;
	if(!pInputFile)
		return;
	
	std::ifstream input_file(pInputFile); 
	if (!input_file.is_open())  
		return;

	std::string last_line, last_output_line, sample_path;	
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
		//std::cout<<line<<std::endl;
		
		std::vector<std::string> ParamList=split(line,"|#|");
		//print_param(ParamList);
		//param3 may be empty
		if(ParamList.size()<3)
			continue;

		//param1,2
		if(operation_id.find(ParamList[0])==operation_id.end())
		{
			std::cout<<"ERROR!wrong operation_id"<<std::endl;
			continue;
		}
		output_line = operation_id[ParamList[0]];
		output_line += delimiter_target;
		output_line += ParamList[1];
		//std::cout<<"out="<<output_line<<std::endl;

		//param3,4
		for(unsigned int i=2; i<ParamList.size(); i++)
		{
			if(ParamList[i]=="")
			{
				//something like: INJECT|#|3|#|
				output_line += delimiter_target;
				continue;
			}
			if(ParamList[i][0] == '\\')
			{
				ParamList[i]=ParamList[i].substr(1,ParamList[i].size());
			}
			if(ParamList[i][ParamList[i].size()-1] == '\\')
			{
				ParamList[i]=ParamList[i].substr(0,ParamList[i].size()-1);
			}			

			string_replace(ParamList[i],"\\",delimiter_arg);
			output_line += delimiter_target;
			output_line += ParamList[i];
		}
		//std::cout<<"out="<<output_line<<std::endl;
		mist_lines.push_back(output_line);
	} 
	input_file.close();
	std::cout<<"***MshipsLog2mist out"<<std::endl;
/*
	std::ofstream output_file(pOutputFile);
	for(std::vector<std::string>::iterator line =mist_lines.begin();line != mist_lines.end(); line++)
	{
		output_file<<*line<<std::endl;
		//std::cout<<*line<<"\n"<<std::endl;
	}
	mist_lines.clear();
	output_file.close();
	*/
	
	return;
}
void MSHIPSClassifier::MshipsMist2join()
{
	std::cout<<"***MshipsMist2join in"<<std::endl;
	std::string output_line;
	//print_param(mist_lines);
	if(mist_lines.size()<Ngram)
		return;

	for(unsigned int i=0; i<mist_lines.size()-Ngram+1; i++)
	{
		bool flag_exist_not_just_registry_query_1_8 = false;
		output_line = "";
		unsigned int j;
		for(j=i; j<=i+Ngram-1; j++)
		{
			output_line += mist_lines[j];
			if(j!=i+Ngram-1)
				output_line += delimiter_behavior;
			//NOTE!
			//只保留不全是1#8的join_lines
			if(mist_lines[j].substr(0,4) != "1#8#")
				flag_exist_not_just_registry_query_1_8 = true;
		}
		if(flag_exist_not_just_registry_query_1_8)
			join_lines.push_back(output_line);
	}
	mist_lines.clear();
	std::cout<<"***MshipsMist2join out"<<std::endl;
/*
	std::ofstream output_file(pOutputFile);
	for(std::list<std::string>::iterator line =join_lines.begin();line != join_lines.end(); line++)
	{
		output_file<<*line<<std::endl;
		//std::cout<<*line<<"\n"<<std::endl;
	}
	join_lines.clear();
*/
	return;
}


void MSHIPSClassifier::MshipsJoin2fet(const char * pOutputFile)
{
	std::cout<<"***MshipsJoin2fet in"<<std::endl;
	if(!pOutputFile)
		return;
	
	std::string output_line;
	std::list<std::string>::iterator line;
	std::vector<std::string> TableList;
	std::size_t pos1, pos2;

	if(join_lines.size()==0)
		return;
	//add table_name
	
	for(line =join_lines.begin();line != join_lines.end();)
	{
		std::vector<std::string> OneBehaviorList=split(*line,delimiter_behavior);
		std::string table_name;
		table_name = get_table_name(*line);
		//bad!!!
		//if(table_name=="1_8$1_8")
		if(0)
		{
			line = join_lines.erase(line);
			continue;
		}	
		else
		{
			table_name += delimiter_table;
			*line = table_name + *line;
			line++;
		}
	}

	if(join_lines.size()==0)
		return;

	//remove similar
	//std::cout<<"--remove in"<<std::endl;
	std::list<std::string>::iterator startline,endline,tmpline;
	for(startline = join_lines.begin();;)
	{		
		//std::cout<<"\n%start="<<*startline<<std::endl;
		if(startline == --join_lines.end())
		{
			std::stringstream ss;
			std::string s;
			ss <<delimiter_count<<1;
			ss >> s;
			*startline += s;
			//std::cout<<"%last2 start="<<*startline<<"\n"<<std::endl;
			break;
		}
		tmpline = startline;
		unsigned int count=1;			
		endline = ++tmpline;
		for(;;)
		{
			tmpline = join_lines.end();
			if(endline == tmpline)
			{
				std::stringstream ss;
				std::string s;
				ss <<delimiter_count<<count;
				ss >> s;
				*startline += s;
				//std::cout<<"%last start="<<*startline<<"\n"<<std::endl;
				break;
			}

			//std::cout<<"--->to="<<*endline<<std::endl;
			if(SimilarNgram(*startline,*endline))
			{
				count +=1;
				endline = join_lines.erase(endline);
				//std::cout<<"#similar"<<std::endl;
			}
			else
			{				
				//std::cout<<"#not similar"<<std::endl;
				endline++;
			}	
		}
		//if now startline is the last line, should not use startline++

		if(startline == --join_lines.end())
		{
			if(startline->rfind(delimiter_count)== std::string::npos)
			{
				std::stringstream ss;
				std::string s;
				ss <<delimiter_count<<1;
				ss >> s;
				*startline += s;
			//	std::cout<<"%last2 start="<<*startline<<"\n"<<std::endl;
			}
			break;
		}
		else
		{
			startline++;
		}

		//std::cout<<"@start="<<*startline<<std::endl;
	}
	//std::cout<<"--remove out"<<std::endl;
	std::ofstream output_file(pOutputFile);
	if (!output_file.is_open())  
		return;
	
	for(line =join_lines.begin();line != join_lines.end(); line++)
	{
		//此处可不截掉table name，fet2vector时更快。
		//仅为和linux版本生成一致的fet
		*line = line->substr(line->find(delimiter_table)+delimiter_table.size(),line->size());
		output_file<<*line<<std::endl;
		//std::cout<<*line<<"\n"<<std::endl;
	}

	join_lines.clear();
	output_file.close();
	std::cout<<"***MshipsJoin2fet out"<<std::endl;
}


void MSHIPSClassifier::MshipsFetchFeature(const char *sqliteFilename)
{
//1#8#REGISTRY|MACHINE|SOFTWARE|Microsoft|CTF|EnableAnchorContext<BH>3#3#WINDOWS|system32|MSCTF.dll<CN>1
//3#3#WINDOWS|system32|MSCTF.dll<BH>1#8#REGISTRY|MACHINE|SOFTWARE|Microsoft|Windows NT|CurrentVersion|GRE_Initialize|DisableMetaFiles<CN>1
	sqlite3 *db = NULL;  
	char *zErrMsg = 0;  
	if(!sqliteFilename)
		return;

	int result=0;

	result = sqlite3_open(sqliteFilename, &db);  
	if( result!= SQLITE_OK )
	{  
		std::cout<<"Can't open database"<<std::endl;
		sqlite3_close(db);  
		return;  
	}  
	g_mships_global_feature.clear();
	//char * sqlstatement = "select * from t1_8$3_3;";  
	char * sqlstatement = "select name from sqlite_master where type='table';";  

	result = sqlite3_exec(db, sqlstatement, &MSHIPSClassifier::callback_get_tables, 0, &zErrMsg); 
	if( result!=SQLITE_OK )
	{   
		sqlite3_free(zErrMsg);  
	}  

	std::map<std::string,std::vector<std::pair<std::string,std::string>> >::iterator it;
	std::cout<<"table_name="<<std::endl;
	int size_of_gf =0;
	for(it=g_mships_global_feature.begin();it!=g_mships_global_feature.end();++it)
	{
		//std::cout<<it->first<<" ";
		g_mships_current_table_name = it->first;
		
		std::string sqlstatement = "select * from ";		
		sqlstatement += g_mships_current_table_name;		
		sqlstatement += ";";
		result = sqlite3_exec(db, sqlstatement.c_str(), &MSHIPSClassifier::callback_get_feature, 0, &zErrMsg); 
		if( result!=SQLITE_OK )
		{   
			sqlite3_free(zErrMsg);  
		}
	/*
		std::vector<std::string>::iterator it2;
		for(it2=it->second.begin();it2!=it->second.end();++it2)
			std::cout<<"vec str="<<*it2<<std::endl;
	*/
		size_of_gf += it->second.size();
	}
	std::cout<<std::endl;
	std::cout<<"*****************feature_num="<<feature_num<<std::endl;
	std::cout<<"*****************size_of_gf="<<size_of_gf<<std::endl;
	sqlite3_close(db);  
	return ;
}


void MSHIPSClassifier::MshipsFet2Vector(const char * pInputFile, const char * pOutputFile)
{
	std::cout<<"***MshipsFet2Vector in"<<std::endl;
	if(!pInputFile||!pOutputFile)
		return;

	std::ifstream input_file(pInputFile); 
	if (!input_file.is_open())  
		return;

    //MshipsFetchFeature();
	std::cout<<"start MshipsFet2Vector"<<std::endl;

	std::vector<std::string> vector_of_index;
	std::string output_line("-1 ");
	int count_feature,count_found,count_miss;
	count_feature = 0;
	count_found = 0;
	count_miss = 0;
	while (!input_file.eof())  
	{  
		std::string line;
		std::getline (input_file,line);  
		if(line == "")
			continue;

		//sqlite table_name cannot start with a number.
		std::size_t pos,pos1,pos2;
		std::string table_name="t";
		std::string	freq;
		/*
		pos = line.find(delimiter_table);
		if(pos!=std::string::npos)
		{
			table_name += line.substr(0,pos);
			line = line.substr(pos+delimiter_table.size(),line.size());
		}
		else
		{
			continue;
		}*/
		table_name += get_table_name(line);

		pos = line.rfind(delimiter_count);
		if(pos!=std::string::npos)
		{
			freq = line.substr(pos+delimiter_count.size(),line.size());
			line = line.substr(0,pos);
		}
		else
		{
			continue;
		}

		/*
		std::cout<<"\nin table:"<<table_name<<std::endl;
		std::cout<<"to find:"<<line<<std::endl;
		*/
		bool found_flag = false;
		count_feature++;
		if(g_mships_global_feature.count(table_name)!=0)
		{
			std::vector<std::pair<std::string,std::string>>::iterator it;
			for(it=g_mships_global_feature[table_name].begin();it!=g_mships_global_feature[table_name].end();++it)
			{
				//std::cout<<"compare:\n"<<it->second<<"\n"<<line<<std::endl;
				if(SimilarNgram_legacy(it->second,line))
				{
					//std::cout<<"found!index="<<it->first<<"\n"<<std::endl;
					count_found++;
					found_flag = true;
					std::stringstream ss;
					std::string s;
					ss <<it->first<<":"<<freq;
					ss >> s;
					vector_of_index.push_back(s);
					//if(it->first=="19708")
					//	std::cout<<"found!index="<<it->first<<"fet="<<it->second<<std::endl;

					break;
				}			
			}
			if(!found_flag)
			{
				//std::cout<<"not found!fet="<<line<<std::endl;
				count_miss++;
			}
		}
		else
		{
			count_miss++;
			std::cout<<"table not exists:"<<table_name<<std::endl;
		}	
	}
	
	if(vector_of_index.size()==0)
		return;

	std::sort(vector_of_index.begin(),vector_of_index.end(),less_mships_index);

	std::string last_index="";
	for(unsigned int i=0;i<vector_of_index.size();i++)
	{
		std::string current_index = vector_of_index[i].substr(0,vector_of_index[i].find(":"));
		if(last_index == current_index)
			continue;
		else
		{
			output_line += vector_of_index[i];
			output_line += " ";	
			last_index = current_index;
		}
	}
	//fet quality
	std::stringstream ss;
	std::string s;
	ss <<delimiter_fet_quality<<count_found<<"/"<<count_feature;
	ss >> s;
	output_line += s;

	std::cout<<"MshipsFet2Vector output="<<output_line<<std::endl;
	std::cout<<"feature num="<<count_feature<<",found num="<<count_found<<",miss num="<<count_miss<<std::endl;

	std::ofstream output_file(pOutputFile);
	if (!output_file.is_open())  
		return;
	
	output_file<<output_line;
	input_file.close();
	output_file.close();
	std::cout<<"***MshipsFet2Vector out"<<std::endl;
	return;
}


void MSHIPSClassifier::LogtoVectors(const char* pfile,const char *sqliteFilename)
{
	char file[100];
	intptr_t handle;
	struct _finddata_t fileinfo;
	std::string full_path_orig, full_path_log, full_path_fet, full_path_vec;
	if(!pfile ||!sqliteFilename)
		return;
	strcpy(file,pfile);
	strcat(file,"\\*.*");
	handle = _findfirst(file,&fileinfo);
	if(-1 == handle)
		return;

	printf("%s\n",fileinfo.name);
	MshipsFetchFeature(sqliteFilename);

	wchar_t *str;
	str = L"C:\\behavior_tool_v2\\behavior_out_log\\";
	RemoveDirectory(str);
	CreateDirectory(str, NULL);
	str = L"C:\\behavior_tool_v2\\behavior_out_fet\\";
	RemoveDirectory(str);
	CreateDirectory(str, NULL);
	str = L"C:\\behavior_tool_v2\\behavior_out_vec\\";
	RemoveDirectory(str);
	CreateDirectory(str, NULL);


	while(!_findnext(handle,&fileinfo))
	{
		if(strcmp(fileinfo.name,".")==0||strcmp(fileinfo.name,"..")==0)
		{
			continue;                                                       
		}
		printf("%s\tsize:%d\n",fileinfo.name,fileinfo.size);  
		std::string str_name(fileinfo.name);
		full_path_orig="C:\\behavior_tool_v2\\behavior_out\\";
		full_path_log="C:\\behavior_tool_v2\\behavior_out_log\\";
		full_path_fet="C:\\behavior_tool_v2\\behavior_out_fet\\";
		full_path_vec="C:\\behavior_tool_v2\\behavior_out_vec\\";
		full_path_orig+=str_name;
		full_path_log+=str_name;
		full_path_fet+=str_name;
		full_path_vec+=str_name;

		MshipsRefine(full_path_orig.c_str(),full_path_log.c_str());
		MshipsLog2mist(full_path_log.c_str());
		MshipsMist2join();
		MshipsJoin2fet(full_path_fet.c_str());
		MshipsFet2Vector(full_path_fet.c_str(),full_path_vec.c_str());
		vec_file_to_predict.push_back(full_path_vec);
	}
	_findclose(handle);

	std::cout<<"\nLog2Vec: generate vec num="<<vec_file_to_predict.size()<<std::endl;
	return;
}

void MSHIPSClassifier::PredictVectors(const char * pModelFile)
{
	std::cout<<"\nLIBLINEAR: load_model"<<std::endl;
	wchar_t* ret_path = L"C:\\behavior_tool_v2\\behavior_out_ret\\";
	RemoveDirectory(ret_path);
	CreateDirectory(ret_path, NULL);

	model * pLibLinearModel=load_model(pModelFile);
	if(!pLibLinearModel)
	{
		std::cout<<"load_model failed."<<std::endl; 
		return;
	}

	struct feature_node * feature=(struct feature_node*)malloc((pLibLinearModel->nr_feature+1)*sizeof(feature_node));
	std::cout<<"nr_class="<<pLibLinearModel->nr_class<<std::endl; 
	std::cout<<"nr_feature="<<pLibLinearModel->nr_feature<<std::endl; 
	std::cout<<"label0="<<pLibLinearModel->label[0]<<std::endl; 
	std::cout<<"label1="<<pLibLinearModel->label[1]<<std::endl; 


	for(unsigned int i=0;i<vec_file_to_predict.size();i++)
	{
		std::cout<<"\npredict file="<<vec_file_to_predict[i]<<std::endl; 
		std::ifstream input_file(vec_file_to_predict[i].c_str()); 
		if (!input_file.is_open())  
			continue;
		
		std::string string_fet_quality="";
		while (!input_file.eof())  
		{  
			std::string line;
			std::size_t pos1 = 0;
			std::size_t pos2 = 0;
			bool exit_flag= false;
			int i=0;

			std::getline (input_file,line); 
			pos1 = line.find(" ");
			if(pos1 == std::string::npos)
				continue;
			
			std::cout<<"liblinear feature ="; 
			while(1)
			{
				pos2 = line.find(" ",pos1+1);
				if(pos2 == std::string::npos)
					pos2 = line.size();
				std::string string_vec = line.substr(pos1+1,pos2-pos1-1);
				std::size_t pos_fet = string_vec.find(delimiter_fet_quality);
				if(pos_fet != std::string::npos)//last item
				{
					string_fet_quality = string_vec.substr(pos_fet+delimiter_fet_quality.size(),string_vec.size());
					break;
				}				
				std::size_t pos_mid = string_vec.find(":");
				if(pos_mid == std::string::npos)
					break;
				std::string string_index = string_vec.substr(0, pos_mid);
				std::string string_value = string_vec.substr(pos_mid+1, string_vec.size());

				int num_index,num_value;
				std::stringstream ss;
				ss << string_index;
				ss >> num_index;
				ss.clear();
				ss.str("");
				ss << string_value;
				ss >> num_value;
				
				(feature+i)->index = num_index;
				(feature+i)->value = num_value;
				i++;	

				std::cout<<num_index<<":"<<num_value<<" "; 
				pos1 = pos2;
			}
			//end with -1:0
			(feature+i)->index = -1;
			(feature+i)->value = 0;	
		}	
		input_file.close();
		/*
		int arr_index[157]={
			4500, 13246, 13257, 13259, 21836, 21863, 22824, 22825, 27808, 39273, \
			48699, 76647, 76649, 76674, 116940, 117235, 129643, 129644, 129645, 138009,\
			138013, 138014, 138015, 138016, 138017, 138505, 159193, 159195, 166701, 166703, \
			166704, 166705, 166706, 166707, 166708, 166709, 179352, 179353, 179354, 179355,\
			179356, 179357, 179358, 179359, 179360, 179361, 179362, 179364, 179365, 179366, 179367,\
			179368, 179369, 179370, 179371, 179372, 179373, 179374, 179375, 179376, 179377, 179378,\
			179379, 179380, 179381, 179382, 179383, 179386, 179387, 179396, 179410, 179411, 179412,\
			179413, 179415, 179416, 179417, 179419, 179420, 179421, 179422, 179423, 179424, 179425,\
			179426, 179427, 179428, 179429, 179430, 179431, 179432, 179433, 179436, 179437, 179438,\
			179439, 179440, 179441, 179442, 179443, 179444, 179445, 179446, 179447, 179448, 179449,\
			179450, 179451, 179452, 179453, 179455, 179456, 179457, 179458, 179461, 179462, 179463, 179464,\
			179465, 179468, 179469, 179470, 179476, 179477, 179478, 179479, 179480, 179481, 179482, 179490,\
			179491, 179492, 179498, 179565, 179578, 179583, 179815, 179834, 179835, 179836, 179837, 179838, 179839,\
			180115, 180117, 180143, 180294, 180343, 180675, 180815, 183459, 183484, 219006, 226361, 226647};
		int arr_value[157]={
			1 ,12 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 \
			,1 ,13 ,1 ,1 ,1 ,1 ,1 ,9 ,4 ,1\
			,3 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,1 ,6 \
			,1 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,1 ,134\
			,1 ,1 ,1 ,1 ,1 ,1 ,4 ,1 ,1 ,1 ,1\
			,1 ,82 ,1 ,1 ,1 ,1 ,1 ,1 ,21 ,1 ,1\
			,10 ,1 ,1 ,1 ,3 ,1 ,4 ,1 ,8 ,11 ,23\
			,1 ,14 ,12 ,1 ,1 ,1 ,2 ,2 ,3 ,4 ,10\
			,1 ,22 ,1 ,1 ,1 ,14 ,1 ,12 ,1 ,1 ,1\
			,11 ,11 ,12 ,1 ,1 ,1 ,12 ,12 ,19 ,1 ,1\
			,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,3 ,1 ,1\
			,1 ,1 ,3 ,1 ,13 ,1 ,1 ,14 ,7 ,9 ,10 ,3\
			,10 ,1 ,1 ,1 ,1 ,1 ,1 ,9 ,1 ,1 ,8 ,1 ,3\
			,1 ,5 ,1 ,1 ,3 ,1 ,1 ,1 ,1 ,1 ,2 ,1\
		};
		

		for(i=0;i<157;i++)
		{
			(feature+i)->index = arr_index[i];
			(feature+i)->value = arr_value[i];		
		}
		
		(feature+i)->index = -1;
		(feature+i)->value = 0;	
	*/
		double dec_distance = 0;
		std::string count_found,count_feature;
		if(string_fet_quality=="" || string_fet_quality.find("/")== std::string::npos)
		{
			if(feature)
				free(feature);
			feature=NULL;
			free_and_destroy_model(&pLibLinearModel);
			return;
		}

		//double ret = predict(pLibLinearModel, feature, &dec_distance);
		double ret = predict_values(pLibLinearModel, feature, &dec_distance);
		std::cout<<"\nPREDICT RESULT = "<<ret<<",distance = "<<dec_distance<<std::endl;

		std::string filename="C:\\behavior_tool_v2\\behavior_out_ret";
		filename += vec_file_to_predict[i].substr(vec_file_to_predict[i].rfind("\\"),vec_file_to_predict[i].size());
		std::ofstream output_file(filename.c_str());
		if (!output_file.is_open())  
			continue;
		output_file<<ret<<" "<<dec_distance<<" "<<string_fet_quality<<std::endl;
		output_file.close();
	}

	if(feature)
		free(feature);
	feature=NULL;
	free_and_destroy_model(&pLibLinearModel);
	return;
}