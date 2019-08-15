#include "threadpool.h"
#include <iostream>
//#include <windows.h>


void fun1(int slp)
{
	printf("  hello, fun1 !  %d\n" ,std::this_thread::get_id());
	if (slp>0) {
		printf(" ======= fun1 sleep %d  =========  %d\n",slp, std::this_thread::get_id());
		std::this_thread::sleep_for(std::chrono::milliseconds(slp));
		//Sleep(slp );
	}
}

class A {    //函数必须是 static 的才能使用线程池
public:
	static int Afun(int n = 0) {
		std::cout << n << "  hello, Afun !  " << std::this_thread::get_id() << std::endl;
		return n;
	}

	static std::string Bfun(int n, std::string str, char c) {
		std::cout << n << "  hello, Bfun !  "<< str.c_str() <<"  " << (int)c <<"  " << std::this_thread::get_id() << std::endl;
		return str;
	}
};

int main()
	try {
		int flag=0;
		std::threadpool executor{ 20 };
		A a;
		std::future<void> ff = executor.commit(fun1,0);
		std::future<int> gg = executor.commit(A::Afun, 999); //IDE提示错误,但可以编译运行
		std::future<std::string> gh = executor.commit(A::Bfun, 9998,"mult args", 123);
		std::future<std::string> fh = executor.commit([]()->std::string { std::cout << "hello, fh !  " << std::this_thread::get_id() << std::endl; return "hello,fh ret !"; });

		std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::microseconds(900));
		while(1)
		{
		    flag++;
		    if(flag>5000)
		 {
		   std::cout << "=======  buffer full  =====" << "PID:" << std::this_thread::get_id() << std::endl;
		   for (int i = 0; i < 20; i++) {
			executor.commit(fun1,i*100 );
		    }
		    flag=0;
		 }
		   
		   std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
		std::cout << " =======  commit all ========= " << std::this_thread::get_id()<< " idlsize="<<executor.idlCount() << std::endl;

		ff.get(); //调用.get()获取返回值会等待线程执行完,获取返回值
		std::cout << fh.get().c_str()<< "  " << std::this_thread::get_id() << std::endl;

		std::cout << " =======  fun1,55 ========= " << std::this_thread::get_id() << std::endl;
		executor.commit(fun1,55).get();    //调用.get()获取返回值会等待线程执行完

		std::cout << "end... " << std::this_thread::get_id() << std::endl;

		return 0;
	}
catch (std::exception& e) {
	std::cout << "some unhappy happened...  " << std::this_thread::get_id() << e.what() << std::endl;
}
