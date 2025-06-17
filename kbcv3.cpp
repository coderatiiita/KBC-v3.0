#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <limits>
#include <ctime>
#define max_q 10

using namespace std;

void clrscr() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

void getch() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int strcmpi(const char* a, const char* b) {
    while (*a && *b) {
        char ca = tolower(static_cast<unsigned char>(*a));
        char cb = tolower(static_cast<unsigned char>(*b));
        if (ca != cb) return ca - cb;
        ++a; ++b;
    }
    return tolower(static_cast<unsigned char>(*a)) - tolower(static_cast<unsigned char>(*b));
}

int q_ask;
class player
{
	public:
		int life50;
		int lifedip;
		int lifedd;
		char name[30];
		long unsigned int win_amt;
		void input();
		void output();
};

player p;

class q_bank
{
	char ques[240];
	char opt[4][40];
	public:
	int status;
	char c_opt;
	int q_no;
	void input();
	void disp_admin();
	void disp_play();
	void disp_play50();
};

void q_bank::input()
{
	clrscr();
	cout<<"Enter question number:"<<endl;
	cin>>q_no;
        cout<<"Enter question: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.getline(ques, sizeof(ques));
        cout<<"\nEnter options:\n";
        for(int j=0;j<4;j++)
                cin.getline(opt[j], sizeof(opt[j]));
	cout<<"\nEnter correct option: \n";
	cin>>c_opt;
	status=1;
}

void q_bank::disp_play()
{
	puts(ques);
	cout<<"\n\n";
	for(int i=0;i<4;i++)
		puts(opt[i]);
}

void q_bank::disp_play50()
{
	int i=0,j=2;
	puts(ques);
	cout<<"\n\n";
	switch(c_opt)
	{
		case 'A': for(;i<2;i++) puts(opt[i]);break;
		case 'B': for(;i<2;i++) puts(opt[i]);break;
		case 'C': for(;j<4;j++) puts(opt[j]);break;
		case 'D': for(;j<4;j++) puts(opt[j]);break;
	}
}
void adm_menu()
{
	int opt=0,i=0;
	q_bank q[max_q];
	char paswrd[20]="kbc?",ent[20],ch='y';
        cout<<"\n\n Enter password: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.getline(ent, sizeof(ent));
	ofstream o("q.bin",ios::binary|ios::app);
	ifstream io("q.bin",ios::binary);
	ifstream tot_q("q.bin",ios::binary);
	if(strcmpi(paswrd,ent)!=0)
		goto x;
	do
	{
		clrscr();
		cout<<"ADMINSTRATOR MENU FOR K.B.C.\n";
		cout<<"\n1.Add questions to q.bin\n2.Display questions from q.bin\n3.Total questions\n4.Back to MAIN MENU\n";
		cin>>opt;

		switch(opt)
		{
			case 1:
				if(!o)
				{cout<<"\nFile not found.\n";getch();}
				else
				{
					clrscr();
					cout<<"Continue(y/n)";
					cin>>ch;
					while(ch=='y'||ch=='Y')
					{
						q[0].input();
						o.write((char*)&q[0],sizeof(q[0]));
						cout<<"\n\nContinue(y/n)";
						cin>>ch;
					}
					o.seekp(0);
				}
				o.close();
				break;

			case 2:
				clrscr();
				if(!io)
					cout<<"File not found";
				else
				{
					for(i=0;i<max_q;i++)
					{
						io.read((char*)&q[i],sizeof(q[i]));
						q[i].disp_admin();
						cout<<endl<<endl;
					}
					getch();
				}
				io.seekg(0);
				io.close();
				break;

			case 3:
				if(!tot_q)
					cout<<"Question bank not found.";
				else
				{
					tot_q.seekg(0,ios::end);
					int l=tot_q.tellg()/sizeof(q[0]);
					cout<<"\n\nTotal questions are: "<<l;
					getch();
				}
				tot_q.seekg(0,ios::beg);
				tot_q.close();
				break;

			case 4:
				clrscr();
				break;

			default: cout<<"Wrong option!";
		}
	}while(opt!=4);
x:
	if(strcmpi(ent,paswrd)!=0)
		cout<<"Wrong password.";
	getch();
}

void q_bank::disp_admin()
{
	cout<<"\n"<<q_no<<".";
	puts(ques);
	cout<<endl;
	for(int j=0;j<4;j++)
		puts(opt[j]);
	cout<<"\nCorrect option is: ";
	cout<<c_opt;
}

int main()
{
	void play();
	int opt;
	do
	{
		clrscr();
		cout<<"@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ \n\n";
		cout<<"\n                          Let's play KAUN BANEGA CROREPATI!!!";
		cout<<"\n\n\n\n\n MENU";
		cout<<"\n 1.Rules\t\t2.Play\n 3.Administrator menu\t4.Exit\n";
		cout<<"\n\n\n\n\n\n\n\n\n\n\n@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ ";
		cin>>opt;
		switch(opt)
		{
			case 1:
				clrscr();
				cout<<"********************************************************************************";
				cout<<" ****************************************************************************** ";
				cout<<"\n\n\n\n\n *You can chose number of questions to be asked.(max "<<max_q<<")\n";
				cout<<" *Each right answer will make you rich by 5,000 rupees.\n";
				cout<<" *A wrong answer will quit the game and your amount won, will be given to you.\n *You may quit the game at any moment.";
				cout<<"\n *You will be provided with three life lines which are '50-50' and 'Drop the \n  question' and 'Double dip'\n\n\n\n\n\n\n\n\n\n";
				cout<<" ****************************************************************************** ";
				cout<<"********************************************************************************";
				getch();
				break;

			case 2:
				clrscr();
				play();
				break;

			case 3:
				clrscr();
				adm_menu();
				break;

			case 4:
				clrscr();
				cout<<"Bye";
				break;

			default: cout<<"Wrong option.";
		}
        }while(opt!=4);
        return 0;
}

void player::input()
{
	cout<<"###############################################################################";
	cout<<"\n\nEnter your name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.getline(name, sizeof(name));
	win_amt=0;
	life50=1;
	lifedd=1;
	lifedip=1;
	cout<<"\nHow many questions should be asked?";
f:
	cout<<"max("<<max_q<<")\n";
	cin>>q_ask;
	if(q_ask>max_q)
		goto f;

}

void player::output()
{
	cout<<"                             !!!CONGRATULATIONS!!!";
	cout<<"\nNAME: ";
	puts(name);
	cout<<"\nAMOUNT WON: "<<win_amt;
}

void play()
{
	int t;
	q_bank q[max_q];

	clrscr();
	p.input();
	int chk=1;
	char ans,opt='y';

	ifstream ply("q.bin",ios::binary);

	if(!ply)
		cout<<" No question bank available!";

	else
	{
		for(int i=0;i<max_q;i++)
			ply.read((char*)&q[i],sizeof(q[i]));
                srand(static_cast<unsigned>(time(nullptr)));
                int q_no=1;
		char u;
		do
		{
			clrscr();

wer:
                        t=rand()%max_q;
			if(q[t].status==0)
				goto wer;
			cout<<"Your question is: \n\n";
			cout<<q_no<<".";
			q[t].disp_play();
			if(p.life50!=0)cout<<"\n(to use 50-50 press 1)\n";
			if(p.lifedd!=0)cout<<"\n(to use 'Drop the question' press 2)\n";
			if(p.lifedip!=0)cout<<"\n(to use 'Double dip press 3)\n";
			cout<<"\n\nYour answer is: ";
			cin>>ans;
			switch(ans)
			{
				case '1':
					clrscr();
					cout<<"Your question is: \n\n";
					cout<<q_no<<".";
					q[t].disp_play50();
					p.life50=0;
					cout<<"\nYour answer is: ";
					cin>>ans;
					break;

				case '2':
					clrscr();
					p.lifedd=0;
					q[t].status=0;
					goto wer;

				case '3':
					p.lifedip=0;
					cout<<"\nEnter your first answer: ";
					cin>>ans;
					u=toupper(ans);
					if(u==q[t].c_opt)
						break;
					else
					{
						cout<<"\nOOPS! Try again...\n";
						cin>>ans;
						break;
					}
			}
			u=toupper(ans);
			if(u==q[t].c_opt)
			{
				cout<<"\n\t\t\tYou are correct! \n\t\t\tCongratulations ";
				puts(p.name);
				cout<<"\nYou have won 5,000 rupees!\n\n\ncontinue(y/n)";
				cin>>opt;
				p.win_amt+=5000;
				q[t].status=0;
				q_no++;
			}
			else
			{
				cout<<"\n\nYou are incorrect!";chk=0;
				cout<<"\nCorrect option is "<<q[t].c_opt;
				getch();
			}
		}while(chk==1&&opt=='y'&&p.win_amt<(q_ask*5000));
	}
	clrscr();
	cout<<"Game ends here.\n";
	p.output();
	getch();
}

