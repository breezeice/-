#pragma once
#include <iostream>
#include <vector>

#include <ctemplate/template.h>

#include "oj_model.hpp"

class OjView
{
	public:
		static void DrawAllQuestions(std::vector<Question>& questions, std::string* html)
		{
			ctemplate::TemplateDictionary dict("all_questions");
			for(const auto& ques:questions)
			{
				ctemplate::TemplateDictionary* sub_dict = dict.AddSectionDictionary("question");
				sub_dict->SetValue("id", ques.id_);
                sub_dict->SetValue("id", ques.id_);
                sub_dict->SetValue("title", ques.title_);
                sub_dict->SetValue("star", ques.star_);
				
			}	
			ctemplate::Template* tl=ctemplate::Template::GetTemplate("./template_pretty/all_questions.html", ctemplate::DO_NOT_STRIP);
			tl->Expand(html, &dict);
		}
		static void DrawOneQuestion(const Question& ques, std::string* html)
        {
            ctemplate::TemplateDictionary dict("question");
            dict.SetValue("id", ques.id_);
            dict.SetValue("title", ques.title_);
            dict.SetValue("star", ques.star_);
            dict.SetValue("desc", ques.desc_);
            dict.SetValue("id", ques.id_);
            dict.SetValue("code", ques.header_cpp_);
            ctemplate::Template* tl = ctemplate::Template::GetTemplate("./template_pretty/question.html", ctemplate::DO_NOT_STRIP);
            //渲染
            tl->Expand(html, &dict);
       }
		
		static void DrawCaseResult(const std::string& err_no, const std::string& q_result, const std::string& reason, std::string* html)
        {
            ctemplate::TemplateDictionary dict("question");
            dict.SetValue("errno", err_no);
            dict.SetValue("compile_result", reason);
            dict.SetValue("case_result", q_result);

            ctemplate::Template* tl = ctemplate::Template::GetTemplate("./template_pretty/case_result.html", ctemplate::DO_NOT_STRIP);
            //渲染
            tl->Expand(html, &dict);
          }
};
