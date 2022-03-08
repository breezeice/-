#pragma once


#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>

#include "tools.hpp"

struct Question
{
    std::string id_; //题目id
    std::string title_; //题目标题
    std::string star_; //题目的难易程度
    std::string path_; //题目路径

    std::string desc_; //题目的描述
    std::string header_cpp_; //题目预定义的头
    std::string tail_cpp_; //题目的尾， 包含测试用例以及调用逻辑
};

class OjModel
{
	public:
        OjModel()
        {
            Load("./oj_data/oj_config.cfg");
        }

        ~OjModel()
        {

        }
		bool Load(const std::string& filename)
        {
			std::ifstream file(filename.c_str());
            if(!file.is_open())
            {
                std::cout << "config file open failed" << std::endl;
                return false;
            }
			std::string line;
            while(std::getline(file, line))
            {
				std::vector<std::string> vec;
                StringUtil::Split(line, "\t", &vec);
				Question ques;
                ques.id_ = vec[0];
                ques.title_ = vec[1];
                ques.star_ = vec[2];
                ques.path_ = vec[3];

                std::string dir = vec[3];
                FileUtil::ReadFile(dir + "/desc.txt", &ques.desc_);
                FileUtil::ReadFile(dir + "/header.cpp", &ques.header_cpp_);
                FileUtil::ReadFile(dir + "/tail.cpp", &ques.tail_cpp_);

                ques_map_[ques.id_] = ques;
            }
			file.close();
            return true;
        }
		//提供给上层调用这一个获取所有试题的接口
		bool GetAllQuestion(std::vector<Question>* questions)
		{
			for(const auto& kv : ques_map_)
            {
                questions->push_back(kv.second);
            }
			std::sort(questions->begin(), questions->end(), [](const Question& l, const Question& r){return std::stoi(l.id_) < std::stoi(r.id_);});
			return true;
		}
		 bool GetOneQuestion(const std::string& id, Question* ques)
        {
            auto it = ques_map_.find(id);
            if(it == ques_map_.end())
            {
                return false;
            }
            *ques = it->second;
            return true;
        }
    private:
        std::unordered_map<std::string, Question> ques_map_;
};




