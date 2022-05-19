#include <strstream>
#include <string.h>
#include <signal.h>
#include <iostream>
#include "header.h"

int main(int argc, char** argv)
{
	cout <<"server: start executing...\n" << flush;
	int key = ftok("exa/wmples",'g');
	if (argc == 2)
        key = atoi(argv[1]);
	char ans[1024];
	char buf[1024];
	message serv(key);
	if (!serv.good()) exit(1);

	double a, b, c;
	char op;
	
	while(1)
	{
		serv.rcv(buf, 0);
		cout << "get: " << buf << endl;
		vector <ent> ans = get_list(string(buf));
		for (auto j: ans)
		{
			serv.sendpart((j.filename).c_str(), j.filename.size() + 1);
		}
		serv.sendpart("#\0", 2);
		
		if (serv.send(buf, 0) == -1)
			cerr<<"Server: "<<getpid() << "send response fails\n";
	};
	serv.rmQ();
	return 0;
}
