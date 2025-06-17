#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <limits>
#include <chrono> // For std::chrono::system_clock
#include <random> // For std::mt19937 and std::uniform_int_distribution
#include <string>   // For std::string
#include <algorithm> // For std::equal, std::tolower (though cctype can also work for tolower)

const int kMaxQ = 10; // Represents max questions a player can choose to play in a session, not total in bank

// Global random number generator
std::mt19937 rng;

// Forward declaration (player is used by q_bank methods, q_bank is used by global functions)
class player;
class q_bank;

// Moved q_bank class definition earlier to resolve sizeof issue
class q_bank
{
	char ques[240];
	char opt[4][40]; // Will be made public for now to fix access error
	public: // Made opt public temporarily
	int status;
	char c_opt;
	int q_no;
	void input();
	void disp_admin(int display_q_num);
	void disp_play();
	void disp_play50();
    // Temporary method to get a specific option for play() function
    const char* get_option(int index) const {
        if (index >= 0 && index < 4) {
            return opt[index];
        }
        return "Error: Invalid Option Index";
    }
    // Temporary method to get the correct option string
    const char* get_correct_option_string() const {
        int correct_idx = std::toupper(c_opt) - 'A';
        if (correct_idx >= 0 && correct_idx < 4) {
            return opt[correct_idx];
        }
        return "Error: Correct Option Invalid";
    }
};


// Function to get current question count from q.bin
// Returns:
// >= 0: Actual count of questions
// -1: File could not be opened or a critical read error occurred
// -2: File format is invalid (e.g., too small, count negative, count doesn't match size)
int get_q_count(std::fstream& file_stream) {
    if (!file_stream.is_open()) {
        std::cerr << "Error in get_q_count: File stream not open." << std::endl;
        return -1;
    }

    file_stream.clear();
    file_stream.seekg(0, std::ios::beg);
    if (file_stream.fail()) { std::cerr << "Error seeking to beginning in get_q_count (1)." << std::endl; file_stream.clear(); return -1;}


    if (file_stream.peek() == std::ifstream::traits_type::eof()) {
        file_stream.clear();
        return 0;
    }

    long file_size;
    file_stream.seekg(0, std::ios::end);
    if (file_stream.fail()) { std::cerr << "Error seeking to end in get_q_count." << std::endl; file_stream.clear(); return -1;}
    file_size = file_stream.tellg();
    if (file_stream.fail()) { std::cerr << "Error getting file size in get_q_count." << std::endl; file_stream.clear(); return -1;}
    file_stream.seekg(0, std::ios::beg);
    if (file_stream.fail()) { std::cerr << "Error seeking to beginning in get_q_count (2)." << std::endl; file_stream.clear(); return -1;}


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
        std::cerr << "Error: sizeof(q_bank) is zero." << std::endl;
        return -1;
    }
    long expected_file_size = static_cast<long>(sizeof(int)) + (static_cast<long>(count) * single_question_size);
    if (expected_file_size != file_size) {
        return -2;
    }
    return count;
}

// Function to write question count to q.bin
bool write_q_count(std::fstream& file_stream, int count) {
    if (!file_stream.is_open()) {
        std::cerr << "Error in write_q_count: File stream not open." << std::endl;
        return false;
    }
    if (count < 0) {
        std::cerr << "Error in write_q_count: Attempting to write negative count." << std::endl;
        return false;
    }
    file_stream.clear();
    file_stream.seekp(0, std::ios::beg);
    if (file_stream.fail()) { std::cerr << "Error seeking to beginning in write_q_count." << std::endl; file_stream.clear(); return false;}
    file_stream.write(reinterpret_cast<const char*>(&count), sizeof(int));
    if (file_stream.fail()) { std::cerr << "Error writing count in write_q_count." << std::endl; file_stream.clear(); return false;}
    file_stream.flush();
    if (file_stream.fail()) { std::cerr << "Error flushing stream in write_q_count." << std::endl; file_stream.clear(); return false;}
    return true;
}


void clrscr() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

void getch_pause() {
    std::cout << "\nPress any key to continue...";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// New case-insensitive string comparison function
bool case_insensitive_compare(const std::string& s1, const std::string& s2) {
    if (s1.length() != s2.length()) {
        return false;
    }
    return std::equal(s1.begin(), s1.end(), s2.begin(), [](char c1, char c2) {
        return std::tolower(static_cast<unsigned char>(c1)) == std::tolower(static_cast<unsigned char>(c2));
    });
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

// q_bank definition moved up

void q_bank::input()
{
	clrscr();
	std::cout<<"Enter question text (max 239 chars): ";
	std::cin.getline(ques, sizeof(ques));
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Warning: Input too long. Question may be truncated." << std::endl;
    }

    std::cout<<"\nEnter the four options (max 39 chars each):\n";
    for(int j=0;j<4;j++) {
        std::cout<<"Option "<<static_cast<char>('A'+j)<<": ";
        std::cin.getline(opt[j], sizeof(opt[j]));
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Warning: Input for option " << static_cast<char>('A'+j) << " too long. May be truncated." << std::endl;
        }
    }
	std::cout<<"\nEnter correct option (A, B, C, or D): \n";
	std::cin>>c_opt;
    c_opt = std::toupper(c_opt);
    while(std::cin.fail() || (c_opt < 'A' || c_opt > 'D')){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid option. Please enter A, B, C, or D: ";
        std::cin >> c_opt;
        c_opt = std::toupper(c_opt);
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	status=1;
}

void q_bank::disp_play()
{
	puts(ques); // Keep puts for now, will replace later
	std::cout<<"\n\n";
	for(int i=0;i<4;i++) {
        std::cout<<static_cast<char>('A'+i)<<". ";
		puts(opt[i]); // Keep puts for now
    }
}

void q_bank::disp_play50()
{
	char correct_char_upper = std::toupper(c_opt);
	int correct_opt_idx = correct_char_upper - 'A';

	puts(ques); // Keep puts for now
	std::cout<<"\n\n";
    std::cout<<static_cast<char>('A'+correct_opt_idx)<<". ";
	puts(opt[correct_opt_idx]); // Keep puts for now

	int random_incorrect_idx;
    std::uniform_int_distribution<int> distrib(0, 3);
	do {
		random_incorrect_idx = distrib(rng);
	} while (random_incorrect_idx == correct_opt_idx);
    std::cout<<static_cast<char>('A'+random_incorrect_idx)<<". ";
	puts(opt[random_incorrect_idx]); // Keep puts for now
}

void adm_menu()
{
	int opt=0, display_counter=0;
	q_bank q_item;
	char paswrd[20]="kbc?",ent[20]; // ch is initialized later
    char ch='y';


    std::cout<<"\n\n Enter password: ";
    if(std::cin.peek() == '\n') std::cin.ignore();
    std::cin.getline(ent, sizeof(ent));

    if(!case_insensitive_compare(std::string(paswrd), std::string(ent))) { // Replaced strcmpi
        std::cout << "Wrong password." << std::endl;
        getch_pause();
        return;
    }

    std::fstream file;
    file.open("q.bin", std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Info: q.bin not found or cannot be opened in r/w mode. Attempting to create it." << std::endl;
        file.clear();
        file.open("q.bin", std::ios::out | std::ios::binary); // Create new file
        if (!file.is_open()) {
            std::cerr << "FATAL ERROR: Cannot open or create q.bin!" << std::endl;
            getch_pause();
            return;
        }
        std::cout << "Info: q.bin created successfully." << std::endl;
        int initial_count = 0;
        if(!write_q_count(file, initial_count)){
            std::cerr << "FATAL ERROR: Failed to write initial count to new q.bin!" << std::endl;
            file.close(); getch_pause(); return;
        }
        file.close(); // Close and reopen in r/w mode
        file.open("q.bin", std::ios::in | std::ios::out | std::ios::binary);
        if(!file.is_open()){
            std::cerr << "FATAL ERROR: Created q.bin but cannot reopen in read/write mode!" << std::endl;
            getch_pause();
            return;
        }
    }

	do
	{
		clrscr();
		std::cout<<"ADMINSTRATOR MENU FOR K.B.C.\n";
		std::cout<<"\n1.Add questions to q.bin\n2.Display questions from q.bin\n3.Total questions\n4.Back to MAIN MENU\n";
		std::cout<<"Enter your choice: ";
        std::cin>>opt;

        if(std::cin.fail()){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number (1-4)." << std::endl;
            getch_pause();
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch(opt)
		{
			case 1:
                {
                    if(!file.is_open()) {
                        std::cerr << "Error: q.bin is not open for adding questions." << std::endl; getch_pause(); break;
                    }

                    int current_q_count = get_q_count(file);
                    if (current_q_count == -1) {
                        std::cerr << "Error reading question count. Cannot add questions." << std::endl; getch_pause(); break;
                    }
                    if (current_q_count == -2) {
                        std::cout << "Info: q.bin is empty or count is invalid. Initializing count to 0." << std::endl;
                        current_q_count = 0;
                        if (!write_q_count(file, current_q_count)) {
                             std::cerr << "Error initializing question count. Cannot add questions." << std::endl; getch_pause(); break;
                        }
                    }

                    file.clear();
                    file.seekp(sizeof(int) + (static_cast<long>(current_q_count) * sizeof(q_bank)), std::ios::beg);
                    if(file.fail()){ std::cerr << "Error seeking to write position for new question." << std::endl; file.clear(); getch_pause(); break;}

                    clrscr();
                    std::cout<<"Add questions now? (y/n): ";
                    std::cin>>ch;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    while(ch=='y'||ch=='Y')
                    {
                        q_item.input();
                        q_item.q_no = current_q_count + 1;
                        file.write(reinterpret_cast<const char*>(&q_item),sizeof(q_item));
                        if(file.fail()){
                            std::cerr << "Error writing question data to file!" << std::endl; file.clear(); getch_pause(); break;
                        }
                        current_q_count++;
                        if (!write_q_count(file, current_q_count)) {
                             std::cerr << "Error updating question count in file after adding question!" << std::endl; getch_pause(); break;
                        }
                        file.seekp(sizeof(int) + (static_cast<long>(current_q_count) * sizeof(q_bank)), std::ios::beg);
                        if(file.fail()){ std::cerr << "Error seeking for next potential question add." << std::endl; file.clear(); getch_pause(); break;}

                        std::cout<<"\n\nAdd another question? (y/n): ";
                        std::cin>>ch;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                }
				break;

			case 2:
                {
                    if(!file.is_open()) { std::cerr << "Error: q.bin is not open for display." << std::endl; getch_pause(); break;}
                    int total_qs = get_q_count(file);

                    if (total_qs == -1) {std::cerr << "Error accessing file to get question count." << std::endl; getch_pause(); break;}
                    if (total_qs == -2) {
                        std::cout << "Info: q.bin format is invalid or it's empty. No questions to display." << std::endl;
                        total_qs = 0;
                    }
                    if (total_qs == 0) {
                        std::cout << "Question bank is empty." << std::endl; getch_pause(); break;
                    }

                    file.clear();
                    file.seekg(sizeof(int), std::ios::beg);
                    if(file.fail()){ std::cerr << "Error seeking past header to display questions." << std::endl; file.clear(); getch_pause(); break;}

                    clrscr();
                    std::cout<<"--- All Questions in Bank (" << total_qs << ") ---\n\n";
                    display_counter = 0;
                    for (int k=0; k < total_qs; ++k) {
                        file.read(reinterpret_cast<char*>(&q_item),sizeof(q_item));
                        if(file.fail() || file.gcount() != sizeof(q_item)){
                            std::cerr << "Error reading question " << k+1 << " from file or read incomplete." << std::endl;
                            file.clear();
                            getch_pause();
                            break;
                        }
                        q_item.disp_admin(k+1);
                        std::cout<<std::endl<<std::endl;
                        display_counter++;
                    }
                    if (display_counter < total_qs && total_qs > 0) {
                        std::cout << "Note: Only " << display_counter << " of " << total_qs << " questions could be displayed due to earlier errors." << std::endl;
                    }
                    getch_pause();
                }
				break;

			case 3:
                {
                    if(!file.is_open()) { std::cerr << "Error: q.bin is not open to count questions." << std::endl; getch_pause(); break;}
                    int count = get_q_count(file);
                    if (count == -1) {
                        std::cout << "Error accessing q.bin. Cannot retrieve total questions." << std::endl;
                    } else if (count == -2) {
                        std::cout << "q.bin is empty or has an invalid format. Total questions: 0" << std::endl;
                    } else {
                        std::cout<<"\n\nTotal number of questions in the bank: "<<count;
                    }
                    getch_pause();
                }
				break;

			case 4:
				clrscr();
				break;

			default: std::cout<<"Wrong option entered!"; getch_pause();
		}
	}while(opt!=4);

	if(file.is_open()){
		file.close();
	}
}

void q_bank::disp_admin(int display_q_num)
{
	std::cout<<"\nDisplaying Question No: "<< display_q_num << " (Original file q_no: "<<this->q_no<<")\n";
	puts(this->ques); // Keep puts for now
	std::cout<<std::endl;
	for(int j=0;j<4;j++) {
		std::cout<<"Opt "<<static_cast<char>('A'+j)<<": ";
		puts(this->opt[j]); // Keep puts for now
	}
	std::cout<<"\nCorrect option is: ";
	std::cout<<this->c_opt;
}

// Forward declaration for play()
void play();

int main()
{
    // Initialize random number generator
    std::random_device rd;
    rng.seed(rd() ? rd() : std::chrono::system_clock::now().time_since_epoch().count());

	int opt;
	do
	{
		clrscr();
		std::cout<<"@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ \n\n";
		std::cout<<"\n                          Let's play KAUN BANEGA CROREPATI!!!";
		std::cout<<"\n\n\n\n\n MENU";
		std::cout<<"\n 1.Rules\t\t2.Play\n 3.Administrator menu\t4.Exit\n";
		std::cout<<"\n\n\n\n\n\n\n\n\n\n\n@ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ ";
		std::cout << "\n\nEnter your choice (1-4): ";
        std::cin>>opt;
        if(std::cin.fail()){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            getch_pause();
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch(opt)
		{
			case 1:
				clrscr();
				std::cout<<"********************************************************************************";
				std::cout<<" ****************************************************************************** ";
				std::cout<<"\n\n\n\n\n *You can choose number of questions to be asked (max "<<kMaxQ<<").\n"; // Replaced max_q
				std::cout<<" *Each right answer will make you rich by 5,000 rupees.\n";
				std::cout<<" *A wrong answer will end the game and your amount won will be given to you.\n *You may quit the game at any moment (by not continuing).";
				std::cout<<"\n *You will be provided with three life lines: '50-50', 'Drop the Question', and 'Double Dip'.\n  Each can be used once per game.\n\n\n\n\n\n\n\n\n";
				std::cout<<" ****************************************************************************** ";
				std::cout<<"********************************************************************************";
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
				std::cout<<"Thank you for playing KBC! Goodbye!";
                getch_pause();
				break;

			default: std::cout<<"Wrong option. Please choose between 1 and 4."; getch_pause();
		}
        }while(opt!=4);
        return 0;
}

void player::input()
{
	std::cout<<"###############################################################################";
	std::cout<<"\n\nEnter your name: ";
    if(std::cin.peek() == '\n') std::cin.ignore();
    std::cin.getline(name, sizeof(name));
    if(std::cin.fail()){ std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); strcpy(name, "Player"); }

	win_amt=0;
	life50=1;
	lifedd=1;
	lifedip=1;
	std::cout<<"\nHow many questions would you like to answer in this session? ";

    // Refactored goto f; with a do-while loop
    bool valid_q_ask_input = false;
    do {
        std::cout<<"(Enter a number between 1 and "<<kMaxQ<<")\n"; // Replaced max_q
	    std::cin>>q_ask;
        if(std::cin.fail() || q_ask <= 0 || q_ask > kMaxQ){ // Replaced max_q
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid number. Please try again." << std::endl;
            // q_ask remains invalid, loop continues
        } else {
            valid_q_ask_input = true;
        }
    } while (!valid_q_ask_input);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void player::output()
{
	std::cout<<"                             !!!CONGRATULATIONS!!!";
	std::cout<<"\nPLAYER NAME: ";
	puts(name); // Keep puts for now
	std::cout<<"\nTOTAL AMOUNT WON: Rs. "<<win_amt;
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
    std::fstream q_file_play_init("q.bin", std::ios::in | std::ios::binary);
    if (!q_file_play_init.is_open()) {
        std::cerr << "CRITICAL ERROR: Question bank file 'q.bin' not found or cannot be opened!" << std::endl;
        std::cerr << "Please contact an administrator." << std::endl;
		getch_pause();
		return;
    }
    total_questions_from_file = get_q_count(q_file_play_init);
    q_file_play_init.close();

    if (total_questions_from_file <= 0) {
        std::cerr << "CRITICAL ERROR: Question bank 'q.bin' is empty, corrupt, or unreadable!" << std::endl;
        std::cerr << "Total questions reported: " << total_questions_from_file << ". Please contact an administrator." << std::endl;
		getch_pause();
		return;
    }

    if (q_ask > total_questions_from_file) {
        std::cout << "\nNote: You've requested " << q_ask << " questions, but only " << total_questions_from_file << " are available." << std::endl;
        std::cout << "The game will proceed with a maximum of " << total_questions_from_file << " questions." << std::endl;
        q_ask = total_questions_from_file;
        std::cout << "Press any key to continue..." << std::endl;
        getch_pause();
    }
    if (q_ask == 0) {
        std::cout << "No questions to play. Returning to main menu." << std::endl;
        getch_pause();
        return;
    }

	do
	{
        bool question_dropped_this_turn = false; // Flag for 'Drop the Question' lifeline
		clrscr();

		std::ifstream question_file_stream("q.bin", std::ios::binary | std::ios::in);
		if(!question_file_stream.is_open())
		{
			std::cerr<<"CRITICAL ERROR: Could not open question bank 'q.bin' during gameplay!"<<std::endl;
			getch_pause();
			return;
		}

        if (total_questions_from_file > 0) { // Ensure total_questions_from_file is positive before creating distribution
            std::uniform_int_distribution<int> distrib(0, total_questions_from_file - 1);
            t_rand_idx = distrib(rng);
        } else {
            // This case should ideally be caught by earlier checks, but as a fallback:
            std::cerr << "Error: No questions available to select randomly." << std::endl;
            getch_pause();
            return;
        }


		question_file_stream.seekg(sizeof(int) + (static_cast<long>(t_rand_idx) * sizeof(q_bank)), std::ios::beg);
        if(question_file_stream.fail()){
            std::cerr << "Error seeking to question " << t_rand_idx + 1 << " in file." << std::endl; // t_rand_idx is 0-based
            question_file_stream.close(); getch_pause();
            questions_played_count++; // Count as an attempt
            if(questions_played_count >= q_ask) game_continues_flag = 0; // End if max attempts reached
            continue; // Try next question
        }

		question_file_stream.read(reinterpret_cast<char*>(&current_question), sizeof(q_bank));

		if(question_file_stream.fail() || question_file_stream.gcount() != sizeof(q_bank))
		{
			std::cerr<<"Error reading question " << t_rand_idx + 1 << " from file, or read incomplete."<<std::endl;
            std::cerr << "Gcount: " << question_file_stream.gcount() << std::endl;
			question_file_stream.close();
			getch_pause();
            questions_played_count++;
            if (questions_played_count >= q_ask) {continue_option = 'n'; game_continues_flag = 0;}
			continue;
		}
		question_file_stream.close();

		std::cout<<"Question No. "<<question_display_number<<" for Rs. "<< (p.win_amt + 5000) <<": \n\n";
		current_question.disp_play();

		if(p.life50!=0)std::cout<<"\n(Press 1 for 50-50 lifeline)\n";
		if(p.lifedd!=0)std::cout<<"\n(Press 2 to Drop the Question - this will skip the question)\n";
		if(p.lifedip!=0)std::cout<<"\n(Press 3 for Double Dip lifeline)\n";
		std::cout<<"\n\nYour answer (A, B, C, D, or lifeline number): ";
		std::cin>>player_answer_char;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


		switch(player_answer_char)
		{
			case '1':
			    if(p.life50!=0) {
					clrscr();
                    std::cout << "--- 50-50 Lifeline Used ---" << std::endl;
					std::cout<<"Question No. "<<question_display_number<<": \n\n";
					current_question.disp_play50();
					p.life50=0;
					std::cout<<"\nYour answer (A, B, C, or D from the remaining options): ";
					std::cin>>player_answer_char;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			    } else {
			        std::cout << "You have already used the 50-50 lifeline!" << std::endl; getch_pause();
                    std::cout << "Please answer the original question again: ";
                    std::cin >> player_answer_char;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			    }
				break;

			case '2':
			    if(p.lifedd!=0) {
					p.lifedd=0;
					questions_played_count++;
                    question_display_number++;
					std::cout << "Question dropped. Moving to the next question." << std::endl; getch_pause();
					if (questions_played_count >= q_ask) {
                        continue_option = 'n';
                        game_continues_flag = 0;
                    } else {
                        continue_option = 'y';
                    }
					question_dropped_this_turn = true; // Set flag to skip answer processing
			    } else {
			         std::cout << "You have already used the Drop the Question lifeline!" << std::endl; getch_pause();
                     std::cout << "Please answer the original question again: ";
                     std::cin >> player_answer_char;
                     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			    }
				break;

			case '3':
			    if(p.lifedip!=0) {
					p.lifedip=0;
					std::cout<<"\n--- Double Dip Lifeline Activated! ---";
                    std::cout <<"\nYou get two chances to answer this question." << std::endl;
                    std::cout << "Enter your first answer (A, B, C, or D): ";
					std::cin>>player_answer_char;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					uppercase_player_answer = std::toupper(player_answer_char);
					if(uppercase_player_answer >= 'A' && uppercase_player_answer <= 'D' && uppercase_player_answer == current_question.c_opt) {
						// Correct on first try
					} else {
                        if(uppercase_player_answer >= 'A' && uppercase_player_answer <= 'D'){ // Check if it was a valid but wrong option
						    std::cout<<"\nOOPS! That was incorrect. Try your second answer for Double Dip: ";
                        } else { // Invalid char on first DD attempt
                            std::cout <<"\nInvalid first attempt. Try your second answer for Double Dip: ";
                        }
						std::cin>>player_answer_char;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
			    } else {
			         std::cout << "You have already used the Double Dip lifeline!" << std::endl; getch_pause();
                     std::cout << "Please answer the original question again: ";
                     std::cin >> player_answer_char;
                     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			    }
				break;
		}

        if(question_dropped_this_turn){
            if(game_continues_flag) continue; // If game still on, skip to next iteration of outer loop
            else break; // if game ended (e.g. max qns reached), break from switch and let outer loop terminate
        }

		uppercase_player_answer = std::toupper(player_answer_char);

		if(uppercase_player_answer >= 'A' && uppercase_player_answer <= 'D' && uppercase_player_answer == current_question.c_opt)
		{
			std::cout<<"\n\t\t\tAbsolutely Correct! \n\t\t\tCongratulations, ";
			puts(p.name); // Keep puts for now
			p.win_amt+=5000;
			std::cout<<"\nYou have won Rs. "<<p.win_amt<<" so far!\n";
			questions_played_count++;
            question_display_number++;
			if (questions_played_count >= q_ask) {
			    std::cout << "\nYou have successfully answered all " << q_ask << " questions for this session!" << std::endl;
			    continue_option = 'n';
                game_continues_flag = 0; // End game
			} else {
			    std::cout << "\n\nDo you want to continue to the next question? (y/n): ";
			    std::cin >> continue_option;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if(std::tolower(continue_option) != 'y') game_continues_flag = 0; // End game if player doesn't want to continue
			}
		}
		else
		{
            if (uppercase_player_answer >= 'A' && uppercase_player_answer <= 'D') {
			std::cout<<"\n\nOh no! That was the incorrect answer.";
            } else {
                std::cout << "\n\nInvalid input. That was not a valid option or lifeline." << std::endl;
            }
			std::cout<<"\nThe correct option was "<<current_question.c_opt<<": ";
            // Use the new public method to get the correct option string
            std::cout << current_question.get_correct_option_string();
            std::cout << std::endl; // Add newline as puts did

			game_continues_flag=0;
			continue_option = 'n';
			getch_pause();
		}
	}while(game_continues_flag==1 && questions_played_count < q_ask); // Loop condition simplified

	clrscr();
	std::cout<<"--------- GAME OVER ---------\n";
	p.output();
	getch_pause();
}
