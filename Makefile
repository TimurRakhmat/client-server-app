all:
	g++ cli.cpp func.cpp -o client.exe
	g++ serv.cpp func.cpp -o server.exe
	g++ semServ.cpp func.cpp -o fsemserver.exe
	g++ qserver.cpp func.cpp -o qserver.exe