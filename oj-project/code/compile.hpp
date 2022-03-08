#pragma once
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <iostream>
#include <string>
#include <atomic>

#include <json/json.h>
#include "tools.hpp"

enum ErrorNo
{
    OK = 0,
    PRAM_ERROR,
    INTERNAL_ERROR,
    COMPILE_ERROR,
    RUN_ERROR
};


class Compiler
{
	public:
		//编译和运行方法
		static void CompileAndRun(Json::Value Req, Json::Value* Resp)
		{
			//从网页获取的code本身为空导致的错误
			if(Req["code"].empty())
            {
                (*Resp)["errorno"] = PRAM_ERROR;
                (*Resp)["reason"] = "Pram error";

                return;
            }
			//不为空将code里的内容写到文件里，方便编译
			std::string code = Req["code"].asString();
			std::string file_nameheader = WriteTmpFile(code);
            if(file_nameheader == "")
            {
                (*Resp)["errorno"] = INTERNAL_ERROR;
                (*Resp)["reason"] = "write file failed";

                return;
            }
			//编译
			if(!Compile(file_nameheader))
            {
                (*Resp)["errorno"] = COMPILE_ERROR;
                std::string reason;
                FileUtil::ReadFile(CompileErrorPath(file_nameheader), &reason);
                (*Resp)["reason"] = reason;
                return;
            }
			//运行
			int ret = Run(file_nameheader);
			if(ret != 0)
            {
                (*Resp)["errorno"] = RUN_ERROR;
                (*Resp)["reason"] = "program exit by sig: " + std::to_string(ret);
                return;
            }
			//
			(*Resp)["errorno"] = OK;
            (*Resp)["reason"] = "Compile and Run ok";

            std::string stdout_str;
            FileUtil::ReadFile(StdoutPath(file_nameheader), &stdout_str);
            (*Resp)["stdout"] = stdout_str;

            std::string stderr_str;
            FileUtil::ReadFile(StderrPath(file_nameheader), &stderr_str);
            (*Resp)["stderr"] = stderr_str;
			
			//删除产生的各种临时文件	
			//Clean(file_nameheader);
		}
	
	private:
		static int Run(const std::string& file_name)
        {
 			 //1.创建子进程
 			 //2.子进程进行进程程序替换
            int pid = fork();
            if(pid < 0)
            {
                return -1;
            }
            else if(pid > 0)
            {
				//status 来存放进程状态，正常状态后2个字节，低八位全为0，异常状态后7位为信号
                
                int status = 0;
                waitpid(pid, &status, 0);
				
            	//取出低7位 
                return status & 0x7f;
            }
            else
            {
               //程序运行时间为1秒，一秒运行不完进程退出 
                alarm(1);
         		//内存限制
                struct rlimit rlim;
				//soft limit 和 hard limit。
                rlim.rlim_cur = 30000 * 1024;   
                rlim.rlim_max = RLIM_INFINITY;
                setrlimit(RLIMIT_AS, &rlim);
				//将标准输出存进这个文件
                int stdout_fd = open(StdoutPath(file_name).c_str(), O_CREAT | O_WRONLY, 0666);
                if(stdout_fd < 0)
                {
                    return -2;
                }
				//重定向标准输出
                dup2(stdout_fd, 1);

                int stderr_fd = open(StderrPath(file_name).c_str(), O_CREAT | O_WRONLY, 0666);
                if(stderr_fd < 0)
                {
                    return -2;
                }
                dup2(stderr_fd, 2);
                
          		//程序替换为执行文件      
                execl(ExePath(file_name).c_str(), ExePath(file_name).c_str(), NULL);
                exit(0);
            }

            return 0;
        }
		static bool Compile(const std::string& file_name)
		{
			//1.创建子进程
			// 1.1 父进程进行进程等待
			// 1.2 子进程进行进程程序替换	
			int pid = fork();
            if(pid > 0)
			{
				waitpid(pid,NULL,0);
			}
			else if(pid==0)
			{
				int fd=open(CompileErrorPath(file_name).c_str(), O_CREAT | O_WRONLY, 0666);
				if(fd < 0)
                {
                    return false;
                }
				//将标准错误重定向到fd文件
				dup2(fd,2);
				//进程程序替换，换成编译的代码
				execlp("g++", "g++", SrcPath(file_name).c_str(), "-o", ExePath(file_name).c_str(), "-std=c++11", "-D", "CompileOnline", NULL);
                close(fd);
				exit(0);
			}
			else
            {
                return false;
            }
			//stat用来判断是否有这个文件，这个结构体里存着文件信息
			struct stat st;
            int ret = stat(ExePath(file_name).c_str(), &st);
            if(ret < 0)
            {
                return false;
            }
			return true;
		}
		//编译的文件存放位置
		static std::string SrcPath(const std::string& filename)
        {
            
            return "./tmp_file/" + filename + ".cpp";
        }
		//输出信息存放位置
		static std::string StdoutPath(const std::string& filename)
        {
            return "./tmp_file/" + filename + ".stdout";
        }
		//运行错误信息存放位置
        static std::string StderrPath(const std::string& filename)
        {
            return "./tmp_file/" + filename + ".stderr";
        }
		//编译出错存放的路径
        static std::string CompileErrorPath(const std::string& filename)
        {
            return "./tmp_file/" + filename + ".Compilerr";
        }
		//执行文件存放位置
        static std::string ExePath(const std::string& filename)
        {
            return "./tmp_file/" + filename + ".executable";
        }
		//将编译文件按照tmp+时间戳+原子操作的数字标记命名
		static std::string WriteTmpFile(const std::string& code)
		{
			static std::atomic_uint id(0);
			std::string tmp_filename = "tmp_" + std::to_string(TimeUtil::GetTimeStampMs()) + "." + std::to_string(id);
			id++;
			FileUtil::WriteFile(SrcPath(tmp_filename), code);
            return tmp_filename;
		}	
		
};
