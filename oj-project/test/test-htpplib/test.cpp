#include<stdio.h>
#include"httplib.h"


void func(const httplib::Request& req,httplib::Response& resp)
{
	resp.set_content("<html>111</html>",15,"text/html");
	printf("okkk\n");
}


int main()
{
	httplib::Server svr;
	svr.Get("/1",func);
	svr.listen("0.0.0.0",1899);
	return 0;
}
