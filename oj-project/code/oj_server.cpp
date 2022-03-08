#include <iostream>
#include <cstdio>

#include <json/json.h>

#include "httplib.h"

#include "oj_model.hpp"
#include "oj_view.hpp"
#include "compile.hpp"
#include "tools.hpp"

int main()
{
	using namespace httplib;
	OjModel model;
	Server svr;
	//获取整个试题列表
	svr.Get("/all_questions",[&model](const Request& req,Response& resp){
		std::vector<Question> questions;
		model.GetAllQuestion(&questions);
		std::string html;
		OjView::DrawAllQuestions(questions, &html);
		resp.set_content(html,"text/html");
	});
	//获取单个试题
	svr.Get(R"(/question/(\d+))", [&model](const Request& req, Response& resp){
		Question ques;
    	model.GetOneQuestion(req.matches[1].str(), &ques);
		std::string html;
		OjView::DrawOneQuestion(ques, &html);
		resp.set_content(html, "text/html");
	});
	
	//编译运行
	svr.Post(R"(/compile/(\d+))", [&model](const Request& req, Response& resp){
		Question ques;
		//根据req里的match来获取试题编号，因为试题编号和url的最后一个数字为同一个东西
    	model.GetOneQuestion(req.matches[1].str(), &ques);
		//用哈希来保存body的内容，为key value键值对
		std::unordered_map<std::string, std::string> body_kv;
		//prasebody函数用来对body进行切割和解码
		UrlUtil::PraseBody(req.body, &body_kv);
		std::string user_code = body_kv["code"];
		Json::Value req_json;
		//用json数据来存储code里的内容和测试用例结合的内容
        req_json["code"] = user_code + ques.tail_cpp_;
		req_json["stdin"] = "";

            std::cout << req_json["code"].asString() << std::endl;
		Json::Value resp_json;
		//编译运行
		Compiler::CompileAndRun(req_json, &resp_json);
		std::string err_no = resp_json["errorno"].asString();
            std::string case_result = resp_json["stdout"].asString();
            std::string reason = resp_json["reason"].asString();

            std::string html;
            OjView::DrawCaseResult(err_no, case_result, reason, &html);

            resp.set_content(html, "text/html");	
	});
	LOG(INFO, "listn_port") << ": 17878" << std::endl;
    svr.set_base_dir("./template_pretty");
	svr.listen("0.0.0.0", 17878);
	return 0;
}
