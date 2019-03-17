#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

void set_random_seed();
int randn(int n);
//function that searches a number in a vector is part of the algorithm library => not allowed.
int Find(std::vector<int> &B, int &n);
//void generate(std::vector<std::vector<int>> &S, int length, int num);
//void eliminate(std::vector<std::vector<int>> &S, std::vector<int> &attempt, int &black_hits, int &white_hits);

struct mm_code_maker {

	void init(int i_length, int i_num) {
		length = i_length;
		num = i_num;
	}

	void generate_sequence() {
		for (int i = 0; i < length; i++) {
			sequence.push_back(randn(num));
		}
	}

	void give_feedback(const std::vector<int>& attempt, int& black_hits, int& white_hits) {
		//Storing indexes of the sequence that has to be avoided when caluclating next hits.
		std::vector <int> avoid_i, avoid_j;

		//Get Black
		for (int i = 0; i < length; i++) {

			if (attempt[i] == sequence[i]) {
				black_hits++;
				avoid_i.push_back(i);
				avoid_j.push_back(i);
			}
		}

		//Get White
		for (int i = 0; i < length; i++) {
			
				for (int j = 0; j < length; j++) {
					if (Find(avoid_j, j) == -1 && Find(avoid_i, i) == -1) {    //if 'j' is not in avoid.
						if (attempt[i] == sequence[j]) {
							white_hits++;
							avoid_j.push_back(j);
							avoid_i.push_back(i);

						}
					}
				}
			
		}

	}

	std::vector<int> sequence;

	int length;
	int num;
};

struct mm_solver {

	void init(int i_length, int i_num) {
		length = i_length;
		num = i_num;
	}

	/// this member function creates an attempt to find the right code
	/// this version generates a random attempt, then checks whether
	/// it has already been tried before and was not the right solution
	/// (if that's the case it generates another attempt etc)
	/// this is clearly still far from being a good implementation!
	void create_attempt(std::vector<int>& attempt) {
		/*bool ready = false;*/

		/*while (!ready) {*/

			int chooseFromS = randn(S.size()) - 1;
			attempt = S[chooseFromS];

			/*if (S.empty()) {
				attempt.clear();
			}
			else {
				ready = true;
			}*/
		/*}*/
	}

	void generate() {
		std::vector<int> single;
		int q;
		//This is only necessary to initialize a vector so we can use single[i] down near the buttom.
		for (int j = 0; j < length; j++) {
			single.push_back(0);
		}

		for (int x = 0; x < pow(num, length); x++) {
			for (int i = 0; i < length; i++) {
				q = pow(num, i);
				single[i] = ((x / q) % num) + 1;

			}
			S.push_back(single);
		}
	}

	/// this version just learns whether an attempt is correct or not
	/// if it is not, it adds it adds it to the list of unsuccessful attempts
	/// this is clearly still far from being a good implementation
	/// the information from black hits and white hits is not really used
	void learn(std::vector<int>& attempt, int black_hits, int white_hits) {
		//elliminating impossible options from our set S.
		mm_code_maker demo;
		demo.init(length, num);
		demo.sequence = attempt;
		int b = 0;
		int w = 0;

		for (int i = 0; i < S.size(); i++) {
			demo.give_feedback(S[i], b, w);
			if (b != black_hits || w != white_hits) {
				S.erase(S.begin() + i);
				i--;
			}
			b = 0;
			w = 0;
		}
	}

	std::vector<std::vector<int> > not_correct;  //vector that stores all tested & incorrect guesses. vector of vectors.
	std::vector<std::vector<int>> S; //Set of all possible answers.
	int length;
	int num;

};

/// the main is the same as in the previous example
//void generate(std::vector<std::vector<int>> &S, int length, int num);

int main() {
	/*std::vector<std::vector<int>> S;*/
	std::vector<int> attempt;
	


	

	set_random_seed();

	int length, num;
	std::cout << "enter length of sequence and number of possible values:" << std::endl;
	std::cin >> length >> num;
	/*generate(S, length, num);*/
	//////printing original set S/////
	//std::cout << "S before: " << std::endl;

	//for (int i = 0; i < S.size(); i++) {
	//	for (int k = 0; k < length; k++) {
	//		std::cout << S[i][k];
	//	}
	//	std::cout << std::endl;
	//}
	///////////////////
	mm_solver solver;
	solver.init(length, num);
	solver.generate();
	std::cout << "S: " << std::endl;
	for (int i = 0; i < solver.S.size(); i++) {
		for (int k = 0; k < solver.length; k++) {
			std::cout << solver.S[i][k];
		}
		std::cout << std::endl;
	}

	mm_code_maker maker;
	maker.init(length, num);
	maker.generate_sequence();

	std::cout << "the code is: ";

	for (int i = 0; i < maker.sequence.size(); i++) {
		std::cout << maker.sequence[i] << " ";
	}

	std::cout << std::endl;

	int black_hits = 0, white_hits = 0;
	int attempts_limit = 5000;
	int attempts = 0;
	while ((black_hits < length) && (attempts < attempts_limit)) {
		black_hits = 0;
		white_hits = 0;
		
		//Initial guess according to Knuth.
		if (attempts == 0) {
			attempt = {1, 1, 2, 2};
		}
		else {
			solver.create_attempt(attempt);
		}
		
		maker.give_feedback(attempt, black_hits, white_hits);
		std::cout << "attempt: " << std::endl;
		for (int i = 0; i < attempt.size(); i++) {
			std::cout << attempt[i] << " ";
		}
		std::cout << std::endl;
		std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;
		
		solver.learn(attempt, black_hits, white_hits);
		std::cout << "S: " << std::endl;
		for (int i = 0; i < solver.S.size(); i++) {
			for (int k = 0; k < solver.length; k++) {
				std::cout << solver.S[i][k];
			}
			std::cout << std::endl;

		}
		attempt.clear();
		attempts++;
	}

	if (black_hits == length) {
		std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
	}
	else {
		std::cout << "after " << attempts << " attempts still no solution" << std::endl;
	}
	std::cout << "the sequence generated by the code maker was:" << std::endl;
	for (int i = 0; i < maker.sequence.size(); i++) { 
		std::cout << maker.sequence[i] << " ";
	}
	std::cout << std::endl;
	return 0;
}



void set_random_seed() {    //seeding random fnction.
	std::srand(std::time(0));
}

int randn(int n) {     //produces a random number bewteen 1 and n;
	return (std::rand() % n) + 1;
}

//If an int is contained, return index, otherwise -1.
int Find(std::vector<int> &B, int &n) {

	for (int i = 0; i < B.size(); i++) {
		if (n == B[i]) {
			return i;
		}
	}
	return -1;

}

//void eliminate(std::vector<std::vector<int>> &S, std::vector<int> &attempt, int &black_hits, int &white_hits) {
//	mm_code_maker demo;
//	demo.init(4, 6);
//	demo.sequence = attempt;
//	int b = 0;
//	int w = 0;
//
//	for (int i = 0; i < S.size(); i++) {
//		demo.give_feedback(S[i], b, w);
//		if (b != black_hits || w != white_hits) {
//				S.erase(S.begin() + i);
//				i--;
//		}
//		b = 0;
//		w = 0;
//	}
//
	/*for (int k = 0; k < S.size(); k++) {
		if (S[k][0] == -1) {
			S.erase(S.begin() + k);
		}

	}*/
//}


