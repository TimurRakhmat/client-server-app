#include "header.h"
using namespace std;

void five_in_row(int);

int main(int argc, char** argv)
{
    int port_id = 10001;
    if (argc == 2)
        port_id = atoi(argv[1]);
		
	int key = ftok("exa/wmples",'g');
	if (argc==3)
	{
		port_id = atoi(argv[1]);
		key = atoi(argv[2]);
	}
    cout << port_id << endl;
	vector <ent> ans;
	string ansstr;
	int server;
	struct sockaddr_in adr;
	socklen_t adrlen;
	int fd;
	char buffer[1024];
	char ansbuf[1023];
	int pid;
	size_t yes = 1;
	adr.sin_family = AF_INET;
	adr.sin_addr.s_addr = 0;
	adr.sin_port = htons(port_id);
	server = socket(AF_INET, SOCK_STREAM, 0);
	bind(server, (struct sockaddr*)&adr, sizeof(adr));
	listen(server, 50);
	int i;
	while (true)
	{
		cout << "wait client...\n";
		adrlen = sizeof(adr);
		
		fd = accept(server, (struct sockaddr*) &adr, &adrlen);
		pid = fork();
		
		if (pid == 0)
		{
			
			while(1)
			{
				char chos = '0';
				char ch[128];
				cout << "send rqst\n";
				if(send(fd, "choose programm: 0 - exit, 1 - file list, 2 - five in row: ", 128, 0)==-1)
					{
						cout << "send error\n";
						*ch = '0';
						chos = '0';
						break;
					}
				cout << "wait answer...\n";
				if (recv(fd, ch, 128, 0) == -1)
				{
					cout << "rcv error\n";
					*ch = '0';
					chos = '0';
					break;
				}
				else
					chos = *ch;

				if (chos == '0')
					break;

				switch(chos)
				{
				case '1':
				{
					message serv(key);
					if (!serv.good())
						exit(1);

					while (true)
					{
						i = 0;
						
						read(fd, buffer, 128);
						cout << "get dirpath: " << buffer << endl;
						if (*buffer == '#')
						{
							break;
						}
						i = strlen(buffer);
						if (i >= BUFSIZ)
						{
							cerr << "Error" << endl;
							write(fd, "Error\n", 128);
							continue ;
						}
						if (*buffer == '\0' or *buffer == '#')
						{
							break;
						}
						buffer[i] = '\0';
						
						cout << "send: " << buffer << endl;
						serv.sendClient(buffer, strlen(buffer) + 1);

						while(1)
						{
							serv.rcvpart(ansbuf,0);
							//cout << "1)rcv: " << ansbuf << endl;
							if (*ansbuf == '#')
							{	
								send(fd, "#", 128, 0);
								//cout << "end of getting part\n";
								break;
							}
							send(fd, ansbuf, 128, 0);
						}
						serv.rcvClient(buffer, 0); // end semafor work
						cout << "end sem work\n";
					}
					break;
				}
				case '2':
				{
					five_in_row(fd);
					break;
				}
				default:
					send(fd, "retry: enter 0 or 1 or 2", 128, 0);
				}
				continue;
				
				/*
				{
				char chos = '-';
				while(1)
				{
					chos = '-';
					char ch[128];
					cout << "send rqst\n";
					if(send(fd, "choose programm: 0 - exit, 1 - file list, 2 - eshop: ", 128, 0)==-1)
					{
						cout << "send error\n";
						*ch = '0';
						chos = '0';
						break;
					}
					cout << "wait answer...\n";
					if (recv(fd, ch, 128, 0) == -1)
					{
						cout << "rcv error\n";
						*ch = '0';
						chos = '0';
						break;
					}
					cout << "clnt:" << ch << endl;
					if (*ch == '0')
					{
						chos = '0';
						break;
					}
					if (*ch == '1')
					{
						chos = '1';
						break;
					}
					if (*ch == '2')
					{
						chos = '2';
						break;
					}
					send(fd, "enter 0 or 1 or 2\n", 128, 0);
					cout << "send error enum\n";
				}


				if (chos == '0')
				{
					break;
				}
				if (chos == '1')
				{
					cout << "111we are there\n";
					message serv(key);
					if (!serv.good())
						exit(1);

					while (true)
					{
						i = 0;
						
						read(fd, buffer, 128);
						i = strlen(buffer);
						if (i >= BUFSIZ)
						{
							cerr << "Error" << endl;
							write(fd, "Error\n", 7);
							continue ;
						}
						if (i == 0 or buffer[0] == '#')
						{
							close(fd);
							fd = -1;
							break;
						}
						buffer[i] = '\0';
						
						cout << "send: " << buffer << endl;
						serv.sendClient(buffer, strlen(buffer) + 1);
						
						//vector<string> ans;
						while(1)
						{
							serv.rcvpart(ansbuf,0);
							//cout << "1)rcv: " << ansbuf << endl;
							send(fd, ansbuf, strlen(ansbuf) + 1, 0);
							if (*ansbuf == '#')
							{	
								send(fd, "#", 2, 0);
								//cout << "end of getting part\n";
								break;
							}
							//ans.push_back(string(ansbuf));
						}
						serv.rcvClient(buffer, 0); // end semafor work

						// cout << ans.size() << " - size of replye" << endl;
						// for(auto it = ans.begin(); it != ans.end(); it++)
						// {
						// 	write(fd, (*it).c_str(), (*it).length() + 1);
						// 	cout << (*it).c_str();
						// }
						// write(fd, "#", 2);
					}
				}
				if (chos == '2')
				{
					cout << "222we are there\n";
					send(fd, "you in 2nd programm\n", 128, 0);
				}
				}
				*/
			}
			cout << "client exit server\n";
			if (fd != -1)
				close(fd);
			exit(0);
		}
	}
	if (fd != -1)
		close(fd);
	close(server);
}


void five_in_row(int fd)
{
	struct {
		long mestype; 
		long mes;
		int n;
	} messageto;

	struct {
		long mestype; 
		int state;
		int x;
		int y;
	} msggame;

	key_t key;

	int mesid;
	long pid = getpid();
	//cout << "my pid: " << pid << endl;
	
	key = ftok("qexampe", 't');

	mesid = msgget(key, 0); 

	messageto.mestype = 1;
	messageto.mes = pid;
	send(fd, "enter size of field: ", 128, 0);
	int nn = 20;
	char buf[256];
	recv(fd, buf, 128, 0);
	nn = atoi(buf);
	messageto.n = nn;

	msgsnd (mesid, &messageto, sizeof(messageto), 0); 
	bool isstart = false;
	int n = 20;
	int x = 0, y = 0;
	while (1)
	{
		while (msgrcv (mesid, &msggame, sizeof(msggame), pid, 0) <= 0) ;
		if (!isstart)
		{
			cout << "stat: " << msggame.state << endl;
			if (msggame.state == 1)
			{
				isstart = true;
				if (msggame.x <= 30 or msggame.x >= 20)
				{   
					n = msggame.x;
				}
			}
			send(fd, "enter x y, or 0 0, to exit: ", 128, 0);
			recv(fd, buf, 128, 0);
			x = atoi(buf);
			recv(fd, buf, 128, 0);
			y = atoi(buf);
			if (x == 0 and y == 0)
			{
				msggame.state = -1;
				msggame.mestype = pid;
				msgsnd (mesid, &msggame, sizeof(msggame), 0);
				return;
			}
			--x; --y;
			msggame.x = x;
			msggame.y = y;
			msggame.state = 2;
			msggame.mestype = pid;
			cout << "i was there\n";
			msgsnd (mesid, &msggame, sizeof(msggame), 0);
			continue;
		}
		if (msggame.state == 7)
		{
			send(fd, "$", 128, 0);
			msggame.state = -1;
			msggame.mestype = pid;
			msgsnd (mesid, &msggame, sizeof(msggame), 0);
			return;
		}
		if (msggame.state == 2)
		{
			const char* rcv;
			rcv = to_string(msggame.x).c_str();
			send(fd, rcv, 128, 0);
			rcv = to_string(msggame.y).c_str();
			send(fd, rcv, 128, 0);
		}

		if (msggame.state == 8)
		{
			const char* rcv;
			rcv = to_string(msggame.x).c_str();
			send(fd, rcv, 128, 0);
			rcv = to_string(msggame.y).c_str();
			send(fd, rcv, 128, 0);
			send(fd, "$", 128, 0);
			msggame.state = -1;
			msggame.mestype = pid;
			msgsnd (mesid, &msggame, sizeof(msggame), 0);
			return;
		}

		send(fd, "enter x y, or 0 0, to exit: ", 128, 0);
		recv(fd, buf, 128, 0);
		x = atoi(buf);
		recv(fd, buf, 128, 0);
		y = atoi(buf);
		if (x == 0 and y == 0)
		{
			msggame.state = -1;
			msggame.mestype = pid;
			msgsnd (mesid, &msggame, sizeof(msggame), 0);
			return;
		}
		
		--x; --y;
		msggame.x = x;
		msggame.y = y;
		msggame.state = 2;
		msggame.mestype = pid;
		msgsnd (mesid, &msggame, sizeof(msggame), 0);
	}
	return;       
}