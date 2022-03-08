#pragma once
#include <sys/time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

class FileUtil
{
	public:
	//file_name: 文件名称 content: 读到的内容, 输出参数， 返还调用者
		static bool ReadFile(const std::string& file_name, std::string* content)
	{
		content->clear();
		std::ifstream file(file_name.c_str());
		if(!file.is_open())
        {
            return false;
    	}
		std::string line;
        while(std::getline(file, line))
        {
			(*content) += line + "\n";
		}
		file.close();
		return true;
	}   
	static bool WriteFile(const std::string& filename, const std::string data)
    {
        std::ofstream file(filename.c_str());
        if(!file.is_open())
        {
           return false;
        }

        file.write(data.c_str(), data.size());
        file.close();
        return true;
     }     
	
};

class StringUtil
{
    public:
        static void Split(const std::string& input, const std::string& split_char, std::vector<std::string>* output)
        {
            boost::split(*output, input, boost::is_any_of(split_char), boost::token_compress_off);
        }
};

class UrlUtil
{
	public:
		static void PraseBody(const std::string& body, std::unordered_map<std::string, std::string>* body_kv)
        {
            std::vector<std::string> kv_vec;
            StringUtil::Split(body, "&", &kv_vec);

            for(const auto& t : kv_vec)
            {
                std::vector<std::string> sig_kv;
                StringUtil::Split(t, "=", &sig_kv);

                if(sig_kv.size() != 2)
                {
                    continue;
                }
                (*body_kv)[sig_kv[0]] = UrlDecode(sig_kv[1]);
            }
        }
		
		static unsigned char ToHex(unsigned char x)   
		{   
    		return  x > 9 ? x + 55 : x + 48;   
		}  
  
		static unsigned char FromHex(unsigned char x)   
		{   
    		unsigned char y;  
    		if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;  
    		else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;  
   			else if (x >= '0' && x <= '9') y = x - '0';  
    		else assert(0);  
    		return y;  
		}
		 
		static std::string UrlDecode(const std::string& str)  
		{  
			std::string strTemp = "";
			size_t length = str.length();  
   			for(size_t i = 0; i < length; i++)  
    		{  
       			if (str[i] == '+') strTemp += ' ';  
     			else if (str[i] == '%')  
        		{  
            		assert(i + 2 < length);  
            		unsigned char high = FromHex((unsigned char)str[++i]);  
            		unsigned char low = FromHex((unsigned char)str[++i]);  
            		strTemp += high*16 + low;  
        		}  
        		else strTemp += str[i];  
    		}  
    		return strTemp;  
		}
};

class TimeUtil
{
    public:
        static int64_t GetTimeStampMs()
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec + tv.tv_usec / 1000;
        }

        static void GetTimeStamp(std::string* TimeStamp)
        {
            time_t st;
            time(&st);

            struct tm* t = localtime(&st);
            
            char buf[30] = { 0 };
            snprintf(buf, sizeof(buf) - 1, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
            TimeStamp->assign(buf, strlen(buf));
        }

}; 

enum LogLevel
{
    INFO = 0,
    WARNING,
    ERROR,
    FATAL,
    DEBUG
};


const char* Level[] = 
{
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL",
    "DEBUG"
};

inline std::ostream& Log(LogLevel lev, const char* file, int line, const std::string& logmsg)
{
    std::cout << "begin log" << std::endl;
    std::string level_info = Level[lev];
    std::cout << level_info << std::endl;
    std::string TimeStamp;
    TimeUtil::GetTimeStamp(&TimeStamp);
    std::cout << TimeStamp << std::endl;

    std::cout << "[" << TimeStamp << " " << level_info << " " << file << ":" << line << "]" << " " << logmsg;
    return std::cout;
}


#define LOG(lev, msg) Log(lev, __FILE__, __LINE__, msg)		
