#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <math.h>
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <fstream>
# include <string>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <vector>
using namespace std;

bool isWinning(int**, int, int, int, int);
bool find_move(int**, int, int&, int&);

int main(int argc, char **argv)
{
	struct {
		long mestype;
		int state;
		int x;
        int y;

	} msgame;

	struct //first type of msg
	{
		long mestype; // type
		long mes; // pid
		int n; // size of field [20, 30]
	} msgfrom;

	key_t key;

	int mesid;

	key = ftok("qexampe", 't');

	mesid = msgget (key, 0666 | IPC_CREAT);
    cout << "begin queue\n";
	while(1)
	{
		if (msgrcv(mesid, &msgfrom, sizeof(msgfrom), 1, 0) <= 0) continue;
        int cpid = msgfrom.mes;
        cout << "enter client: " << cpid << endl; 
        int pid = fork();
        int n = msgfrom.n;
        if (pid == 0)
        {
            int** field;
            int x = 0, y = 0;
            int ii = 0, jj = 0;

            cout << "size: " << n << endl;
            field = new int*[n];
            for (int i = 0; i < n; i++)
            {
                field[i] = new int[n];
            }
            for (int i= 0; i< n; i++)
                for (int j= 0; j < n; j++)
                    field[i][j] = 0;


            msgame.x = n;
            msgame.mestype = cpid;
            msgame.state = 1;
            msgsnd (mesid, &msgame, sizeof(msgame), 0);
            while(1)
            {
                while (msgrcv(mesid, &msgame, sizeof(msgame), cpid, 0) <= 0);
                if (msgame.state == -1)
                    break;
                x = msgame.x;
                y = msgame.y;
                field[x][y] = -1;

                if (isWinning(field, n, x, y, -1))
                {
                    msgame.state = 7;
                    msgame.mestype = cpid;
                    msgsnd (mesid, &msgame, sizeof(msgame), 0);
                    continue;
                }

                cout << "try find move\n";
                find_move(field, n, ii, jj);
                cout << "find move:" << ii << " - " << jj << endl;

                msgame.x = ii;
                msgame.y = jj;
                field[ii][jj] = 1;

                msgame.state = 2;
                if (isWinning(field, n, ii, jj, 1))
                    msgame.state = 8;
                msgame.mestype = cpid;
                cout << "send rcv\n";
                msgsnd (mesid, &msgame, sizeof(msgame), 0);
            }

            cout << "client pid: " << cpid << "left game\n";

            for (int i = 0; i < n; i++)
            {
                delete[] field[i];
            }
            delete[] field;
		    exit(0);
        }
		
	}

	msgctl (mesid, IPC_RMID, 0);

	return 0;
}


bool isWinning(int** field, int n, int x, int y, int flg)
{
    // horizontal
    int count = 0;
    int k = 1;
    while(k < 5 and y - k >= 0 and field[x][y-k] == flg) // left pass
    {
        k++;
    }
    k--;
    count += k;
    k = 1;
    while(k < 5 and y + k < n and field[x][y+k] == flg) //right pass
    {
        k++;
    }
    k--;
    count += k;
    if (count > 3)
        return true;

    // vertical
    count = 0;
    k = 1;
    while(k < 5 and x - k >= 0 and field[x - k][y] == flg) // down pass
    {
        k++;
    }
    k--;
    count += k;
    k = 1;
    while(k < 5 and x + k < n and field[x+k][y] == flg) //up pass
    {
        k++;
    }
    k--;
    count += k;
    if (count > 3)
        return true;

    // lup rdn diag
    count = 0;
    k = 1;
    while(k < 5 and x - k >= 0 and y - k >= 0 and field[x - k][y-k] == flg) // lup pass
    {
        k++;
    }
    k--;
    count += k;
    k = 1;
    while(k < 5 and x + k < n and y + k < n and field[x+k][y+k] == flg) //rdn pass
    {
        k++;
    }
    k--;
    count += k;
    if (count > 3)
        return true;

    
    // rup ldn diag
    count = 0;
    k = 1;
    while(k < 5 and x - k >= 0 and y + k < n and field[x - k][y+k] == flg) // lup pass
    {
        k++;
    }
    k--;
    count += k;
    k = 1;
    while(k < 5 and x + k < n and y - k >= 0 and field[x+k][y-k] == flg) //rdn pass
    {
        k++;
    }
    k--;
    count += k;
    if (count > 3)
        return true;

    return false;
}


/*
bool isWinning(int** field, int n)
{
    for(int i = 0; i < n; i++)
    {
        for(int j =0; j < n; j++)
        {
            int count = 0;
            // horizontal
            for (int k = 0; k < 5; k++)
            {
                if (j + k > n - 1)
                {
                    break;
                }
                count += field[i][j + k];
            }
            if (count == -5)
                return true;
            // vertical
            count = 0;
            for (int k = 0; k < 5; k++)
            {
                if (i + k > n - 1)
                {
                    break;
                }
                count += field[i + k][j];
            }
            if (count == -5)
                return true;
            // low r diag
            count = 0;
            for (int k = 0; k < 5; k++)
            {
                if (i + k > n - 1 or j + k > n - 1)
                {
                    break;
                }
                count += field[i + k][j + k];
            }
            if (count == -5)
                return true;

            // up r diag
            count = 0;
            for (int k = 0; k < 5; k++)
            {
                if (i - k < 0 or j + k > n - 1)
                {
                    break;
                }
                count += field[i - k][j + k];
            }
            if (count == -5)
                return true;
            
        }
    }
    return false;
}

*/

bool sendn(int** field, int n, int& a, int& b, int pos, int flg)
{
    for(int i = 0; i < n; i++)
    {
        for(int j =0; j < n; j++)
        {
            int count = 0;
            // horizontal
            for (int k = 0; k < pos; k++)
            {
                if (j + k > n - 1)
                {
                    break;
                }
                count += field[i][j + k];
            }
            if (count == flg * pos)
            {
                if (j + pos < n and field[i][j + pos] == 0)
                {
                    a = i;
                    b = j + pos;
                    return true;
                }
                if (j - 1 >= 0 and field[i][j - 1] == 0)
                {
                    a = i;
                    b = j - 1;
                    return true;
                }
            }
            // vertical
            count = 0;
            for (int k = 0; k < pos; k++)
            {
                if (i + k > n - 1)
                {
                    break;
                }
                count += field[i + k][j];
            }
            if (count == flg * pos)
            {
                if (i + pos < n - 1 and field[i + pos][j] == 0)
                {
                    a = i + pos;
                    b = j;
                    return true;
                }
                if (i - 1 >= 0 and field[i - 1][j] == 0)
                {
                    a = i - 1;
                    b = j;
                    return true;
                }
            }


            // low r diag
            count = 0;
            for (int k = 0; k < pos; k++)
            {
                if (i + k > n - 1 or j + k > n - 1)
                {
                    break;
                }
                count += field[i + k][j + k];
            }

            if (count == flg * pos)
            {
                if (i + pos < n and j + pos < n and field[i + pos][j + pos] == 0)
                {
                    a = i + pos;
                    b = j + pos;
                    return true;
                }
                if (i - 1 >= 0 and j - 1 >= 0 and field[i - 1][j - 1] == 0)
                {
                    a = i - 1;
                    b = j - 1;
                    return true;
                }
            }

            // up r diag
            count = 0;
            for (int k = 0; k < pos; k++)
            {
                if (i - k < 0 or j + k > n - 1)
                {
                    break;
                }
                count += field[i - k][j + k];
            }
            if (count == flg * pos)
            {
                if (i - pos >= 0 and j + pos < n and field[i - pos][j + pos] == 0)
                {
                    a = i - pos;
                    b = j + pos;
                    return true;
                }
                if (i + 1 > n and j - 1 >= 0 and field[i + 1][j - 1] == 0)
                {
                    a = i + 1;
                    b = j - 1;
                    return true;
                }
            }
            
        }
    }
    return false;
}


bool find_move(int** field, int n, int& a, int& b)
{
    if (sendn(field, n, a, b, 4, 1))
        return true;
    if (sendn(field, n, a, b, 4, -1))
        return true;
    if (sendn(field, n, a, b, 3, 1))
        return true;
    if (sendn(field, n, a, b, 3, -1))
        return true;
    if (sendn(field, n, a, b, 2, 1))
        return true;
    
    for (int i = 0; i < n/2; i++)
        for (int j = 0; j < n/2; j++)
        {
            if (field[n/2 + i][n/2 + j] == 0)
            {
                a = n/2 + i;
                b = n/2 + j;
                return true;
            }
            if (field[n/2 - i][n/2 - j] == 0)
            {
                a = n/2 - i;
                b = n/2 - j;
                return true;
            }
            if (field[n/2][n/2 + j] == 0)
            {
                a = n/2;
                b = n/2 + j;
                return true;
            }
            if (field[n/2 + i][n/2] == 0)
            {
                a = n/2 + i;
                b = n/2;
                return true;
            }
        }

    return false;
}