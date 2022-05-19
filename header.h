#ifndef HEADER_H
# define HEADER_H
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <fstream>
# include <string>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>
# include <dirent.h>
# include <vector>
# include <utility>
# include <algorithm>
#include "message.h"

using namespace std;
struct ent
{
	string filename;
	int number;
	int type; //1 - file, 2 - dir
	int errn;
};
vector <ent> get_list(string filepath);
int cmp(ent ent1, ent ent2);
int read_file_strs(string filepath);
int read_dir_files(string filepath);
void show(int** f, int n);
void remove_field(int** f, int n);
#endif