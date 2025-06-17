#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <limits>
#include <ctime>
#define max_q 10 // Represents max questions a player can choose to play in a session, not total in bank

using namespace std;

// Forward declaration
class q_bank;

// Function to get current question count from q.bin
// Returns:
// >= 0: Actual count of questions
// -1: File could not be opened or a critical read error occurred
// -2: File format is invalid (e.g., too small, count negative, count doesn't match size)
int get_q_count(fstream& file_stream) {
    if (!file_stream.is_open()) {
        cerr << "Error in get_q_count: File stream not open." << endl;
        return -1;
    }

    file_stream.clear();
    file_stream.seekg(0, ios::beg);
    if (file_stream.fail()) { cerr << "Error seeking to beginning in get_q_count (1)." << endl; file_stream.clear(); return -1;}


    if (file_stream.peek() == EOF) {
        file_stream.clear();
        return 0;
    }

    long file_size;
    file_stream.seekg(0, ios::end);
    if (file_stream.fail()) { cerr << "Error seeking to end in get_q_count." << endl; file_stream.clear(); return -1;}
    file_size = file_stream.tellg();
    if (file_stream.fail()) { cerr << "Error getting file size in get_q_count." << endl; file_stream.clear(); return -1;}
    file_stream.seekg(0, ios::beg);
    if (file_stream.fail()) { cerr << "Error seeking to beginning in get_q_count (2)." << endl; file_stream.clear(); return -1;}


    if (file_size < sizeof(int)) {
        return -2;
    }

    int count;
    file_stream.read(reinterpret_cast<char*>(&count), sizeof(int));

    if (file_stream.fail() || file_stream.gcount() != sizeof(int)) {
        file_stream.clear();
        return -2;
    }

    if (count < 0) {
        return -2;
    }

    long single_question_size = sizeof(q_bank);
    if (single_question_size == 0) { // Avoid division by zero if q_bank is empty struct (not possible here)
        cerr << "Error: sizeof(q_bank) is zero." << endl;
        return -1;
    }
    long expected_file_size = static_cast<long>(sizeof(int)) + (static_cast<long>(count) * single_question_size);
    if (expected_file_size != file_size) {
        return -2;
    }
    return count;
}

// Function to write question count to q.bin
bool write_q_count(fstream& file_stream, int count) {
    if (!file_stream.is_open()) {
        cerr << "Error in write_q_count: File stream not open." << endl;
        return false;
    }
    if (count < 0) {
        cerr << "Error in write_q_count: Attempting to write negative count." << endl;
        return false;
    }
    file_stream.clear();
    file_stream.seekp(0, ios::beg);
    if (file_stream.fail()) { cerr << "Error seeking to beginning in write_q_count." << endl; file_stream.clear(); return false;}
    file_stream.write(reinterpret_cast<const char*>(&count), sizeof(int));
    if (file_stream.fail()) { cerr << "Error writing count in write_q_count." << endl; file_stream.clear(); return false;}
    file_stream.flush();
    if (file_stream.fail()) { cerr << "Error flushing stream in write_q_count." << endl; file_stream.clear(); return false;}
    return true;
}


void clrscr() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

void getch_pause() {
    cout << "\nPress any key to continue...";
    cin.clear();
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
	void disp_admin(int display_q_num);
	void disp_play();
	void disp_play50();
};

void q_bank::input()
{
	clrscr();
	cout<<"Enter question text (max 239 chars): ";
	cin.getline(ques, sizeof(ques));
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Warning: Input too long. Question may be truncated." << endl;
    }

    cout<<"\nEnter the four options (max 39 chars each):\n";
    for(int j=0;j<4;j++) {
        cout<<"Option "<<static_cast<char>('A'+j)<<": ";
        cin.getline(opt[j], sizeof(opt[j]));
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Warning: Input for option " << static_cast<char>('A'+j) << " too long. May be truncated." << endl;
        }
    }
	cout<<"\nEnter correct option (A, B, C, or D): \n";
	cin>>c_opt;
    c_opt = toupper(c_opt);
    while(cin.fail() || (c_opt < 'A' || c_opt > 'D')){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid option. Please enter A, B, C, or D: ";
        cin >> c_opt;
        c_opt = toupper(c_opt);
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
	status=1;
}

void q_bank::disp_play()
{
	puts(ques);
	cout<<"\n\n";
	for(int i=0;i<4;i++) {
        cout<<static_cast<char>('A'+i)<<". ";
		puts(opt[i]);
    }
}

void q_bank::disp_play50()
{
	char correct_char_upper = toupper(c_opt);
	int correct_opt_idx = correct_char_upper - 'A';

	puts(ques);
	cout<<"\n\n";
    cout<<static_cast<char>('A'+correct_opt_idx)<<". ";
	puts(opt[correct_opt_idx]);

	int random_incorrect_idx;
	do {
		random_incorrect_idx = rand() % 4;
	} while (random_incorrect_idx == correct_opt_idx);
    cout<<static_cast<char>('A'+random_incorrect_idx)<<". ";
	puts(opt[random_incorrect_idx]);
}

void adm_menu()
{
	int opt=0, display_counter=0;
	q_bank q_item;
	char paswrd[20]="kbc?",ent[20],ch='y';

    cout<<"\n\n Enter password: ";
    if(cin.peek() == '\n') cin.ignore();
    cin.getline(ent, sizeof(ent));

    if(strcmpi(paswrd,ent)!=0) {
        cout << "Wrong password." << endl;
        getch_pause();
        return;
    }

    fstream file;
    file.open("q.bin", ios::in | ios::out | ios::binary);

    if (!file.is_open()) {
        cout << "Info: q.bin not found or cannot be opened in r/w mode. Attempting to create it." << endl;
        file.clear();
        file.open("q.bin", ios::out | ios::binary);
        if (!file.is_open()) {
            cerr << "FATAL ERROR: Cannot open or create q.bin!" << endl;
            getch_pause();
            return;
        }
        cout << "Info: q.bin created successfully." << endl;
        int initial_count = 0;
        if(!write_q_count(file, initial_count)){ // write_q_count now handles seekp to beginning
            cerr << "FATAL ERROR: Failed to write initial count to new q.bin!" << endl;
            file.close(); getch_pause(); return;
        }
        // After creating and writing initial count, close and reopen in r/w mode if necessary,
        // or ensure fstream is in a good state for subsequent r/w.
        // For fstream opened with ios::out, it might need to be closed and reopened
        // or seek operations might be needed to switch between read/write.
        // Simplest is to close and reopen.
        file.close();
        file.open("q.bin", ios::in | ios::out | ios::binary);
        if(!file.is_open()){
            cerr << "FATAL ERROR: Created q.bin but cannot reopen in read/write mode!" << endl;
            getch_pause();
            return;
        }
    }

	do
	{
		clrscr();
		cout<<"ADMINSTRATOR MENU FOR K.B.C.\n";
		cout<<"\n1.Add questions to q.bin\n2.Display questions from q.bin\n3.Total questions\n4.Back to MAIN MENU\n";
		cout<<"Enter your choice: ";
        cin>>opt;

        if(cin.fail()){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number (1-4)." << endl;
            getch_pause();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch(opt)
		{
			case 1:
                {
                    if(!file.is_open()) {
                        cerr << "Error: q.bin is not open for adding questions." << endl; getch_pause(); break;
                    }

                    int current_q_count = get_q_count(file);
                    if (current_q_count == -1) {
                        cerr << "Error reading question count. Cannot add questions." << endl; getch_pause(); break;
                    }
                    if (current_q_count == -2) {
                        cout << "Info: q.bin is empty or count is invalid. Initializing count to 0." << endl;
                        current_q_count = 0;
                        if (!write_q_count(file, current_q_count)) {
                             cerr << "Error initializing question count. Cannot add questions." << endl; getch_pause(); break;
                        }
                    }

                    file.clear();
                    file.seekp(sizeof(int) + (static_cast<long>(current_q_count) * sizeof(q_bank)), ios::beg);
                    if(file.fail()){ cerr << "Error seeking to write position for new question." << endl; file.clear(); getch_pause(); break;}

                    clrscr();
                    cout<<"Add questions now? (y/n): ";
                    cin>>ch;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    while(ch=='y'||ch=='Y')
                    {
                        q_item.input();
                        q_item.q_no = current_q_count + 1;
                        file.write(reinterpret_cast<const char*>(&q_item),sizeof(q_item));
                        if(file.fail()){
                            cerr << "Error writing question data to file!" << endl; file.clear(); getch_pause(); break;
                        }
                        current_q_count++;
                        if (!write_q_count(file, current_q_count)) {
                             cerr << "Error updating question count in file after adding question!" << endl; getch_pause(); break;
                        }
                        file.seekp(sizeof(int) + (static_cast<long>(current_q_count) * sizeof(q_bank)), ios::beg);
                        if(file.fail()){ cerr << "Error seeking for next potential question add." << endl; file.clear(); getch_pause(); break;}

                        cout<<"\n\nAdd another question? (y/n): ";
                        cin>>ch;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                }
				break;

			case 2:
                {
                    if(!file.is_open()) { cerr << "Error: q.bin is not open for display." << endl; getch_pause(); break;}
                    int total_qs = get_q_count(file);

                    if (total_qs == -1) {cerr << "Error accessing file to get question count." << endl; getch_pause(); break;}
                    if (total_qs == -2) {
                        cout << "Info: q.bin format is invalid or it's empty. No questions to display." << endl;
                        total_qs = 0;
                    }
                    if (total_qs == 0) {
                        cout << "Question bank is empty." << endl; getch_pause(); break;
                    }

                    file.clear();
                    file.seekg(sizeof(int), ios::beg);
                    if(file.fail()){ cerr << "Error seeking past header to display questions." << endl; file.clear(); getch_pause(); break;}

                    clrscr();
                    cout<<"--- All Questions in Bank (" << total_qs << ") ---\n\n";
                    display_counter = 0;
                    for (int k=0; k < total_qs; ++k) {
                        file.read(reinterpret_cast<char*>(&q_item),sizeof(q_item));
                        if(file.fail() || file.gcount() != sizeof(q_item)){
                            cerr << "Error reading question " << k+1 << " from file or read incomplete." << endl;
                            file.clear();
                            getch_pause();
                            break;
                        }
                        q_item.disp_admin(k+1);
                        cout<<endl<<endl;
                        display_counter++;
                    }
                    if (display_counter < total_qs && total_qs > 0) {
                        cout << "Note: Only " << display_counter << " of " << total_qs << " questions could be displayed due to earlier errors." << endl;
                    }
                    getch_pause();
                }
				break;

			case 3:
                {
                    if(!file.is_open()) { cerr << "Error: q.bin is not open to count questions." << endl; getch_pause(); break;}
                    int count = get_q_count(file);
                    if (count == -1) {
                        cout << "Error accessing q.bin. Cannot retrieve total questions." << endl;
                    } else if (count == -2) {
                        cout << "q.bin is empty or has an invalid format. Total questions: 0" << endl;
                    } else {
                        cout<<"\n\nTotal number of questions in the bank: "<<count;
                    }
                    getch_pause();
                }
				break;

			case 4:
				clrscr();
				break;

			default: cout<<"Wrong option entered!"; getch_pause();
		}
	}while(opt!=4);

	if(file.is_open()){
		file.close();
	}
}

void q_bank::disp_admin(int display_q_num)
{
	cout<<"\nDisplaying Question No: "<< display_q_num << " (Original file q_no: "<<this->q_no<<")\n";
	puts(this->ques);
	cout<<endl;
	for(int j=0;j<4;j++) {
		cout<<"Opt "<<static_cast<char>('A'+j)<<": ";
		puts(this->opt[j]);
	}
	cout<<"\nCorrect option is: ";
	cout<<this->c_opt;
}

int main()
{
	srand(static_cast<unsigned>(time(nullptr)));
	int opt;
	do
	{
		clrscr();
		cout<<"@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ \n\n";
		cout<<"\n                          Let's play KAUN BANEGA CROREPATI!!!";
		cout<<"\n\n\n\n\n MENU";
		cout<<"\n 1.Rules\t\t2.Play\n 3.Administrator menu\t4.Exit\n";
		cout<<"\n\n\n\n\n\n\n\n\n\n\n@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ ";
		cout << "\n\nEnter your choice (1-4): ";
        cin>>opt;
        if(cin.fail()){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl;
            getch_pause();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch(opt)
		{
			case 1:
				clrscr();
				cout<<"********************************************************************************";
				cout<<" ****************************************************************************** ";
				cout<<"\n\n\n\n\n *You can choose number of questions to be asked (max "<<max_q<<").\n";
				cout<<" *Each right answer will make you rich by 5,000 rupees.\n";
				cout<<" *A wrong answer will end the game and your amount won will be given to you.\n *You may quit the game at any moment (by not continuing).";
				cout<<"\n *You will be provided with three life lines: '50-50', 'Drop the Question', and 'Double Dip'.\n  Each can be used once per game.\n\n\n\n\n\n\n\n\n";
				cout<<" ****************************************************************************** ";
				cout<<"********************************************************************************";
				getch_pause();
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
				cout<<"Thank you for playing KBC! Goodbye!";
                getch_pause();
				break;

			default: cout<<"Wrong option. Please choose between 1 and 4."; getch_pause();
		}
        }while(opt!=4);
        return 0;
}

void player::input()
{
	cout<<"###############################################################################";
	cout<<"\n\nEnter your name: ";
    if(cin.peek() == '\n') cin.ignore();
    cin.getline(name, sizeof(name));
    if(cin.fail()){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); strcpy(name, "Player"); }

	win_amt=0;
	life50=1;
	lifedd=1;
	lifedip=1;
	cout<<"\nHow many questions would you like to answer in this session? ";

    // Refactored goto f; with a do-while loop
    bool valid_q_ask_input = false;
    do {
        cout<<"(Enter a number between 1 and "<<max_q<<")\n";
	    cin>>q_ask;
        if(cin.fail() || q_ask <= 0 || q_ask > max_q){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid number. Please try again." << endl;
            // q_ask remains invalid, loop continues
        } else {
            valid_q_ask_input = true;
        }
    } while (!valid_q_ask_input);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void player::output()
{
	cout<<"                             !!!CONGRATULATIONS!!!";
	cout<<"\nPLAYER NAME: ";
	puts(name);
	cout<<"\nTOTAL AMOUNT WON: Rs. "<<win_amt;
}

void play()
{
	int t_rand_idx;
	q_bank current_question;

	clrscr();
	p.input();

    int game_continues_flag=1;
	char player_answer_char, continue_option='y';
    int questions_played_count = 0;
	int question_display_number=1;
	char uppercase_player_answer;

    int total_questions_from_file = 0;
    fstream q_file_play_init("q.bin", ios::in | ios::binary);
    if (!q_file_play_init.is_open()) {
        cerr << "CRITICAL ERROR: Question bank file 'q.bin' not found or cannot be opened!" << endl;
        cerr << "Please contact an administrator." << endl;
		getch_pause();
		return;
    }
    total_questions_from_file = get_q_count(q_file_play_init);
    q_file_play_init.close();

    if (total_questions_from_file <= 0) {
        cerr << "CRITICAL ERROR: Question bank 'q.bin' is empty, corrupt, or unreadable!" << endl;
        cerr << "Total questions reported: " << total_questions_from_file << ". Please contact an administrator." << endl;
		getch_pause();
		return;
    }

    if (q_ask > total_questions_from_file) {
        cout << "\nNote: You've requested " << q_ask << " questions, but only " << total_questions_from_file << " are available." << endl;
        cout << "The game will proceed with a maximum of " << total_questions_from_file << " questions." << endl;
        q_ask = total_questions_from_file;
        cout << "Press any key to continue..." << endl;
        getch_pause();
    }
    if (q_ask == 0) {
        cout << "No questions to play. Returning to main menu." << endl;
        getch_pause();
        return;
    }

	do
	{
        bool question_dropped_this_turn = false; // Flag for 'Drop the Question' lifeline
		clrscr();

		ifstream question_file_stream("q.bin", ios::binary | ios::in);
		if(!question_file_stream.is_open())
		{
			cerr<<"CRITICAL ERROR: Could not open question bank 'q.bin' during gameplay!"<<endl;
			getch_pause();
			return;
		}

		t_rand_idx = rand() % total_questions_from_file;

		question_file_stream.seekg(sizeof(int) + (static_cast<long>(t_rand_idx) * sizeof(q_bank)), ios::beg);
        if(question_file_stream.fail()){
            cerr << "Error seeking to question " << t_rand_idx + 1 << " in file." << endl; // t_rand_idx is 0-based
            question_file_stream.close(); getch_pause();
            questions_played_count++; // Count as an attempt
            if(questions_played_count >= q_ask) game_continues_flag = 0; // End if max attempts reached
            continue; // Try next question
        }

		question_file_stream.read(reinterpret_cast<char*>(&current_question), sizeof(q_bank));

		if(question_file_stream.fail() || question_file_stream.gcount() != sizeof(q_bank))
		{
			cerr<<"Error reading question " << t_rand_idx + 1 << " from file, or read incomplete."<<endl;
            cerr << "Gcount: " << question_file_stream.gcount() << endl;
			question_file_stream.close();
			getch_pause();
            questions_played_count++;
            if (questions_played_count >= q_ask) {continue_option = 'n'; game_continues_flag = 0;}
			continue;
		}
		question_file_stream.close();

		cout<<"Question No. "<<question_display_number<<" for Rs. "<< (p.win_amt + 5000) <<": \n\n";
		current_question.disp_play();

		if(p.life50!=0)cout<<"\n(Press 1 for 50-50 lifeline)\n";
		if(p.lifedd!=0)cout<<"\n(Press 2 to Drop the Question - this will skip the question)\n";
		if(p.lifedip!=0)cout<<"\n(Press 3 for Double Dip lifeline)\n";
		cout<<"\n\nYour answer (A, B, C, D, or lifeline number): ";
		cin>>player_answer_char;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');


		switch(player_answer_char)
		{
			case '1':
			    if(p.life50!=0) {
					clrscr();
                    cout << "--- 50-50 Lifeline Used ---" << endl;
					cout<<"Question No. "<<question_display_number<<": \n\n";
					current_question.disp_play50();
					p.life50=0;
					cout<<"\nYour answer (A, B, C, or D from the remaining options): ";
					cin>>player_answer_char;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
			    } else {
			        cout << "You have already used the 50-50 lifeline!" << endl; getch_pause();
                    cout << "Please answer the original question again: ";
                    cin >> player_answer_char;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
			    }
				break;

			case '2':
			    if(p.lifedd!=0) {
					p.lifedd=0;
					questions_played_count++;
                    question_display_number++;
					cout << "Question dropped. Moving to the next question." << endl; getch_pause();
					if (questions_played_count >= q_ask) {
                        continue_option = 'n';
                        game_continues_flag = 0;
                    } else {
                        continue_option = 'y';
                    }
					question_dropped_this_turn = true; // Set flag to skip answer processing
			    } else {
			         cout << "You have already used the Drop the Question lifeline!" << endl; getch_pause();
                     cout << "Please answer the original question again: ";
                     cin >> player_answer_char;
                     cin.ignore(numeric_limits<streamsize>::max(), '\n');
			    }
				break;

			case '3':
			    if(p.lifedip!=0) {
					p.lifedip=0;
					cout<<"\n--- Double Dip Lifeline Activated! ---";
                    cout <<"\nYou get two chances to answer this question." << endl;
                    cout << "Enter your first answer (A, B, C, or D): ";
					cin>>player_answer_char;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
					uppercase_player_answer = toupper(player_answer_char);
					if(uppercase_player_answer >= 'A' && uppercase_player_answer <= 'D' && uppercase_player_answer == current_question.c_opt) {
						// Correct on first try
					} else {
                        if(uppercase_player_answer >= 'A' && uppercase_player_answer <= 'D'){ // Check if it was a valid but wrong option
						    cout<<"\nOOPS! That was incorrect. Try your second answer for Double Dip: ";
                        } else { // Invalid char on first DD attempt
                            cout <<"\nInvalid first attempt. Try your second answer for Double Dip: ";
                        }
						cin>>player_answer_char;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
					}
			    } else {
			         cout << "You have already used the Double Dip lifeline!" << endl; getch_pause();
                     cout << "Please answer the original question again: ";
                     cin >> player_answer_char;
                     cin.ignore(numeric_limits<streamsize>::max(), '\n');
			    }
				break;
		}

        if(question_dropped_this_turn){
            if(game_continues_flag) continue; // If game still on, skip to next iteration of outer loop
            else break; // if game ended (e.g. max qns reached), break from switch and let outer loop terminate
        }

		uppercase_player_answer = toupper(player_answer_char);

		if(uppercase_player_answer >= 'A' && uppercase_player_answer <= 'D' && uppercase_player_answer == current_question.c_opt)
		{
			cout<<"\n\t\t\tAbsolutely Correct! \n\t\t\tCongratulations, ";
			puts(p.name);
			p.win_amt+=5000;
			cout<<"\nYou have won Rs. "<<p.win_amt<<" so far!\n";
			questions_played_count++;
            question_display_number++;
			if (questions_played_count >= q_ask) {
			    cout << "\nYou have successfully answered all " << q_ask << " questions for this session!" << endl;
			    continue_option = 'n';
                game_continues_flag = 0; // End game
			} else {
			    cout << "\n\nDo you want to continue to the next question? (y/n): ";
			    cin >> continue_option;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if(tolower(continue_option) != 'y') game_continues_flag = 0; // End game if player doesn't want to continue
			}
		}
		else
		{
            if (uppercase_player_answer >= 'A' && uppercase_player_answer <= 'D') {
			cout<<"\n\nOh no! That was the incorrect answer.";
            } else {
                cout << "\n\nInvalid input. That was not a valid option or lifeline." << endl;
            }
			cout<<"\nThe correct option was "<<current_question.c_opt<<": ";
			int correct_idx = toupper(current_question.c_opt) - 'A';
			if (correct_idx >=0 && correct_idx < 4) {
                puts(current_question.opt[correct_idx]);
            } else {
                cout << "(Correct option text not available)" << endl;
            }

			game_continues_flag=0;
			continue_option = 'n';
			getch_pause();
		}
	}while(game_continues_flag==1 && questions_played_count < q_ask); // Loop condition simplified

	clrscr();
	cout<<"--------- GAME OVER ---------\n";
	p.output();
	getch_pause();
}

```
