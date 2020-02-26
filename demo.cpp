#include "MySpider"


int main ()
{
   string url ; 
   cout <<"请输入你要爬取的url" <<endl;
   cin >> url;
   
   MySpider test;
   test.CatchHtml(url);

}
