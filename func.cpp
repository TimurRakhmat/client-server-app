#include "header.h"

using namespace std;

void show(int** f, int n)
{
    system("clear");
    cout << "  ";
    for(int j = 0; j < n; j++)
        {
            if (j < 9)
                cout << " ";
            cout << j + 1;
        }
    cout << endl;
    for (int i = 0; i < n; i++)
    {
        if (i < 9)
                cout << " ";
        cout << i + 1;
        for(int j = 0; j < n; j++)
        {
            if (f[i][j] == 0)
                cout << "|_";
            else if (f[i][j] == -1)
                cout << "|x";
            else if (f[i][j] == 1)
                cout << "|o";
        }
        cout << "|\n";

    }
}

void remove_field(int** f, int n)
{
	for (int i = 0; i < n; i++)
		delete[] f[i];
	delete[] f; 
}

int cmp(ent ent1, ent ent2)
{
	if (ent1.type != ent2.type)
	return (ent1.type > ent2.type);
	else if (ent1.number != ent2.number)
	return (ent1.number > ent2.number);
	return (strcmp(ent1.filename.c_str(), ent2.filename.c_str()) < 0);
}

int read_file_strs(string filepath)
{
	string tmp;
	ifstream fin;
	int ans;
	ans = 0;
	fin.open(filepath, ios::in);
	if (!fin.is_open())
		return (-1);
	while (getline(fin, tmp))
		ans++;
	fin.close();
	return (ans);
}
int read_dir_files(string filepath)
{
	DIR *dir;
	struct dirent *dir_ent;
	int ans;
	ans = 0;
	dir = opendir(filepath.c_str());
	if (!dir)
		return (-1);
	while ((dir_ent = readdir(dir)))
	{
		if (dir_ent->d_type == DT_REG)
			ans++;
	}
	closedir(dir);
	return (ans);
}

vector <ent> get_list(string dirpath)
{
	string tmpstr;
	vector <ent> ans;
	DIR *dir;
	struct dirent *dir_ent;
	ent tmp;
	errno = 0;
	dir = opendir(dirpath.c_str());
	if (!dir)
	{
		cerr << "Dir doesn't open" << endl;
		return (ans);
	}
	while ((dir_ent = readdir(dir)))
	{
		if (dir_ent->d_type == DT_REG)
		{
			tmp.type = 1;
			if (dir_ent->d_name != NULL)
				tmp.filename = string(dir_ent->d_name);
			else
				tmp.filename = string("");
			tmp.number = read_file_strs(dirpath + string("/") + tmp.filename);
			tmp.errn = errno;
			errno = 0;
			ans.push_back(tmp);
		}
		else if (dir_ent->d_type == DT_DIR)
		{
			tmp.type = 2;
			if (dir_ent->d_name != NULL)
				tmp.filename = string(dir_ent->d_name);
			else
				tmp.filename = string("");
			tmp.number = read_dir_files(dirpath + string("/") + tmp.filename);
			tmp.errn = errno;
			errno = 0;
			ans.push_back(tmp);
		}
	}
	closedir(dir);
	sort(ans.begin(), ans.end(), cmp);
	return (ans);
}