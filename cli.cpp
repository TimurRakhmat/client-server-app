#include "header.h"

using namespace std;

void five_in_row_cli(int);

int main(int argc, char** argv)
{
    int port_id = 10001;
    if (argc == 2)
        port_id = atoi(argv[1]);
    cout << port_id << endl;
	int client;
	string filepath;
	struct sockaddr_in adr;
	struct sockaddr_in adr_bind;
	char answer_buffer[512];
	int size;
	char buffer[1024];
	adr.sin_family = AF_INET;
	adr.sin_port = htons(port_id);
	adr.sin_addr.s_addr = 0;
	adr_bind.sin_family = AF_INET;
	adr_bind.sin_port = 0;
	adr_bind.sin_addr.s_addr = INADDR_ANY;
	client = socket(AF_INET, SOCK_STREAM, 0);
	connect(client, (struct sockaddr*)&adr,sizeof(adr));
	int i;

	char chos = '-';
	while(1)
	{
		cout << "wait serv\n";
		chos = '-';
		if (recv(client, answer_buffer, 128, 0) == -1)
		{
				cout << "client rcv error";
				chos = 0;
				break;
		}
		cout << "serv: " << answer_buffer << endl;
		char ansch[128];
		cin >> ansch;
		chos = *ansch;
		if (send(client, ansch, 128, 0) == -1)
		{
			cout << "client send error\n";
			*ansch = 0;
			chos = '0';
			break;
		}

		if (chos == '0')
			break;

		switch(chos)
		{
		case '1':
			while(1)
			{
				i = 0;
				cout << "Enter filepath: ";
				cin >> buffer;
				i = strlen(buffer);
				cout << "send this dir: " << buffer << endl;
				
				write(client, buffer, 128);
				if (*buffer == '\0' or *buffer == '#')
					break;
				cout << "sorted file and dir list: \n";
				while (true)
				{
					recv(client, answer_buffer, 128, 0);
					if (*answer_buffer == '#')
						break;
					cout << answer_buffer << endl;
				}
				cout << "----rpl_end-----\n";
			}
			break;
		case '2':
			five_in_row_cli(client);
			break;
		default:
			size = recv(client, answer_buffer, 128, 0);
			cout << answer_buffer << endl;
		}
		continue;

		/*
		if (*ansch == '0')
		{
			chos = '0';
			break;
		}
		if (*ansch == '1')
		{
			chos = '1';
			break;
		}

		if (*ansch == '2')
		{
			chos = '2';
			break;
		}
		size = recv(client, answer_buffer, 1024, 0);
		cout << "serve: resp:" << answer_buffer << endl;
	}
	if (chos == '0')
	{
		break;
	}
	if (chos == '1')
	{
		while(1)
		{
			i = 0;
			cout << "Enter filepath: ";
			cin.getline(buffer, 128);
			i = strlen(buffer);
			if (i == 0)
				break;
			write(client, buffer, strlen(buffer) + 1);
			cout << "sorted file and dir list: \n";
			while (true)
			{
				size = recv(client, answer_buffer, 1024, 0);
				if (*answer_buffer == '#')
					break;
				cout << answer_buffer << endl;
			}
			cout << "----rpl_end-----\n";
		}
	}
	if (chos == '2')
	{
		size = recv(client, answer_buffer, 1024, 0);
		cout << answer_buffer << endl;
	}
	*/
	}
	cout << "exit servak\n";
	close(client);
}

void five_in_row_cli(int client)
{
	int** field;
	char buf[256];
	const char* rcv;
	recv(client, buf, 128, 0);
	int n =0;
	while(1)
	{
		cout << buf << endl;
		cin >> n;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(32767, '\n');
			continue;
		}
		if (n < 20 or n > 30)
		{
			cout << "enter field size in range 20 - 30\n";
			continue;
		}
		cin.ignore(32767, '\n');
		break;
	}
	rcv = to_string(n).c_str();
	send(client, rcv, 128, 0);
	field = new int*[n];
	for (int i = 0; i < n; i++)
	{
		field[i] = new int[30];
	}
	for (int i= 0; i< n; i++)
		for (int j= 0; j < n; j++)
			field[i][j] = 0;
	int x = 0, y = 0;
	recv(client, buf, 128, 0);
	if (*buf == '#')
		return;
	show(field, n);
	while(1)
	{
		cout << buf << endl;
		cin >> x;
		if (cin.fail())
		{
			cout << "cin error\n";
			cin.clear();
			cin.ignore(32767, '\n');
			continue;
		}
		cin >> y;
		if (cin.fail())
		{
			cout << "cin error\n";
			cin.clear();
			cin.ignore(32767, '\n');
			continue;
		}
		if (x > n or x < 0 or y > n or y < 0)
		{
			cout << "enter x and y in range 1.." << n  << ", or 0 0 to exit: \n";
			continue;
		}
		cin.ignore(32767, '\n');
		break;
	}
	if (x == 0 and y == 0)
	{
		send(client, "0", 128, 0);
		send(client, "0", 128, 0);
		remove_field(field, n);
		return;
	}

	field[x-1][y-1] = -1;
	show(field, n);
	sleep(1);
	rcv = to_string(x).c_str();
	send(client, rcv, 128, 0);
	rcv = to_string(y).c_str();
	send(client, rcv, 128, 0);

	while (true)
	{
		recv(client, buf, 128, 0);
		if (*buf == '$')
		{
			cout << "you are winner\n";
			break;
		}
		if (*buf == '#')
			break;
		x = atoi(buf);
		recv(client, buf, 128, 0);
		y = atoi(buf);
		field[x][y] = 1;
		show(field, n);
		cout << "wait... recv..\n";
		recv(client, buf, 128, 0);
		if (*buf == '$')
		{
			cout << "you are loser\n";
			break;
		}
		if (*buf == '#')
			break;
		while(1)
		{
			cout << buf << endl;
			cin >> x;
			if (cin.fail())
			{
				cout << "cin error\n";
				cin.clear();
				cin.ignore(32767, '\n');
				continue;
			}
			cin >> y;
			if (cin.fail())
			{
				cout << "cin error\n";
				cin.clear();
				cin.ignore(32767, '\n');
				continue;
			}
			if (x > n or x < 0 or y > n or y < 0)
			{
				cout << "enter x and y in range 1.." << n  << ", or 0 0 to exit: \n";
				continue;
			}
			if (x != 0 and y != 0)
			if (field[x-1][y-1] != 0)
			{
				cout << "ceil " << x << ", " << y << "is not empty, pls retry\n";
				continue;
			}
			cin.ignore(32767, '\n');
			break;
		}

		if (x == 0 and y == 0)
		{
			send(client, "0", 128, 0);
			send(client, "0", 128, 0);
			remove_field(field, n);
			return;
		}
		

		field[x-1][y-1] = -1;
		show(field, n);
		sleep(1);
		rcv = to_string(x).c_str();
		send(client, rcv, 128, 0);
		rcv = to_string(y).c_str();
		send(client, rcv, 128, 0);
	}
	remove_field(field, n);
}