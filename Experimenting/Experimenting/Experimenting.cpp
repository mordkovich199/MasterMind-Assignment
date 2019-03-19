#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <map>

void set_random_seed();
int randn(int n);
//function that searches a number in a vector is part of the algorithm library => not allowed.
int Find(std::vector<int> &b, int &n);
//void generate(std::vector<std::vector<int>> &S, int length, int num);
//void eliminate(std::vector<std::vector<int>> &S, std::vector<int> &attempt, int &black_hits, int &white_hits);
void mutation(std::vector<int> &parent, std::vector<int> &child);
void permutation(std::vector <int> &parent, std::vector<int> &child);
void inversion(std::vector <int> &parent, std::vector<int> &child);
void createPopulation(std::vector <std::vector<int>> &population, int &size);
int fitness(std::vector<int> &c, std::vector<std::vector<int>> &AllAttempts, std::vector<int> &Black, std::vector<int> &White);
void crossOver(std::vector<int> &Parent1, std::vector<int> &Parent2, std::vector<int> &child1, std::vector<int> &child2);
double SurvivalThreshold(std::map <std::vector<int>, int > &SurvivalScores);
void NewGeneration(std::vector<std::vector<int>> &population, std::map <std::vector<int>, int > &SurvivalScores, float &threshold);

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
			int chooseFromS = randn(S.size()) - 1;
			attempt = S[chooseFromS];
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

	
	std::vector<std::vector<int>> S; //Set of all possible answers.
	int length;
	int num;

};


int main() {
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Testing Inversion function!!!!!!!!!!!!!!!!!!!!!!!!!///////////////
	/*set_random_seed();
	std::vector<std::vector<int>> population;
	int PopulationSize = 150;
	std::vector<int> attempt;
	createPopulation(population, PopulationSize);
	std::vector<int> Parent1, Parent2, Child1, Child2;
	 Parent1 = population[randn(PopulationSize) - 1];
	 Parent2 = population[randn(PopulationSize) - 1];

	 while (Parent1 == Parent2) {
		 Parent2 = population[randn(PopulationSize) - 1];
	 }

	 std::cout << "Parent one: ";
	 for (int k = 0; k < Parent1.size(); k++) {
		 std::cout << Parent1[k];
	 }
	 std::cout << std::endl;
	 std::cout << "Parent two: ";
	 for (int k = 0; k < Parent2.size(); k++) {
		 std::cout << Parent2[k];
	 }
	 std::cout << std::endl;
	 crossOver(Parent1, Parent2, Child1, Child2);
	 std::cout << "Child one: ";
	for (int k = 0; k < Child1.size(); k++) {
			 std::cout << Child1[k];
	}

	 std::cout << std::endl;
	 std::cout << "Child two: ";
	 for (int k = 0; k < Child2.size(); k++) {
		 std::cout << Child2[k];
	 }
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////////////////


	//!!!!!!!!!!!!!!!!!!!!!Testing the fitness function!!!!!!!!!!!!!!///////////
	int length = 4;
	int num = 6;
	set_random_seed();
	int PopulationSize = 150;
	std::vector<std::vector<int>> population;
	std::vector<std::vector<int>> AllAttempts;
	std::map <std::vector<int>, int > SurvivalScores;
	std::vector<int> Black;
	std::vector<int> White;


	std::vector<int> attempt; 
	int black_hits = 0;
	int white_hits = 0;
	
	//Fill in the population with random indivisduals
	createPopulation(population, PopulationSize);

	std::cout << "Random Population: " << std::endl;
	for (int x = 0; x < population.size(); x++) {
		for (int y = 0; y < length; y++) {
			std::cout << population[x][y];
		}
		std::cout << std::endl;
	}

	mm_code_maker maker;
	maker.init(length, num);
	//Pretend this is our code word
	maker.sequence = {4, 5, 6, 6};

	
	attempt = population[randn(PopulationSize) - 1];
	maker.give_feedback(attempt, black_hits, white_hits);
	AllAttempts.push_back(attempt);
	Black.push_back(black_hits);
	White.push_back(white_hits);
	black_hits = 0;
	white_hits = 0;
	//What's the point, it will receive a reasonable survival score number - no problem
	/*population.erase(std::find(population.begin(), population.end(), attempt));*/

	std::cout << "Attempt: " << std::endl;

	for (int k = 0; k < attempt.size(); k++) {
		std::cout << attempt[k];
	}
	std::cout << std::endl;

	attempt.clear();
	
	std::vector<int> member;
	int memeberScore;

	for (int i = 0; i < population.size(); i++) {
		member = population[i];
		memeberScore = fitness(member, AllAttempts, Black, White);
		SurvivalScores.insert(std::pair<std::vector<int>, int>(member, memeberScore));
	}
	std::cout << "Member: " << "Score:" << std::endl;
	
	for (auto x : SurvivalScores) {
		/*for (int k = 0; k < x.first.size(); k++) {
			std::cout << x.first[k];
		}*/
		std::cout << " " << x.second << std::endl;
	}

	std::cout << "Surival Threshold would be: " << SurvivalThreshold(SurvivalScores) << std::endl;

	////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///////////

	

	
	


	

	//////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Testing the transformation functions!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///////
	/*std::ofstream myfile;
	int output = 0;
	myfile.open("Data.csv", std::ofstream::out | std::ofstream::app);

	std::vector<int> child;
	std::vector<int> parent = { 1, 3, 4, 5 };
	std::cout << "Parent: " << std::endl;

	for (int i = 0; i < parent.size(); i++) {
		std::cout << parent[i];

	}

	std::cout << std::endl;

	
	std::cout << "Children: " << std::endl;
	for (int count = 1; count <= 200; count++) {
		inversion(parent, child);
		for (int i = 0; i < child.size(); i++) {
			std::cout << child[i];
		}
		int k = 0;
		int j = child.size() - 1;
		while (k < child.size()) {

			output += child[k] * pow(10, j);
				k++;
				j--;
		}

		myfile << count << "," << output << "\n";
		output = 0;
	}
	
	
	myfile.close();
*/
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///////


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Testing the learn function!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//////
	//std::ofstream myfile;
	//std::vector<int> attempt;

	//////////
	//double duration;
	//std::clock_t start;
	///////////


	//
	//

	//set_random_seed();

	//int length, num;
	//std::cout << "enter length of sequence and number of possible values:" << std::endl;
	//std::cin >> length >> num;
	//
	//mm_solver solver;
	//solver.init(length, num);

	//mm_code_maker maker;
	//maker.init(length, num);
	//for (int cases = 0; cases < 1000; cases++) {
	//	myfile.open("Data.csv", std::ofstream::out | std::ofstream::app);
	//	start = std::clock();

	//	solver.generate();
	//	std::cout << "S: " << std::endl;
	//	for (int i = 0; i < solver.S.size(); i++) {
	//		for (int k = 0; k < solver.length; k++) {
	//			std::cout << solver.S[i][k];
	//		}
	//		std::cout << std::endl;
	//	}


	//	maker.generate_sequence();

	//	std::cout << "the code is: ";

	//	for (int i = 0; i < maker.sequence.size(); i++) {
	//		std::cout << maker.sequence[i] << " ";
	//	}

	//	std::cout << std::endl;

	//	int black_hits = 0, white_hits = 0;
	//	int attempts_limit = 5000;
	//	int attempts = 0;
	//	while ((black_hits < length) && (attempts < attempts_limit)) {
	//		black_hits = 0;
	//		white_hits = 0;

	//		//Initial guess according to Knuth.
	//		if (attempts == 0) {
	//			attempt = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	//		}
	//		else {
	//			solver.create_attempt(attempt);
	//		}

	//		maker.give_feedback(attempt, black_hits, white_hits);
	//		std::cout << "attempt: " << std::endl;
	//		for (int i = 0; i < attempt.size(); i++) {
	//			std::cout << attempt[i] << " ";
	//		}
	//		std::cout << std::endl;
	//		std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;

	//		solver.learn(attempt, black_hits, white_hits);
	//		std::cout << "S: " << std::endl;
	//		for (int i = 0; i < solver.S.size(); i++) {
	//			for (int k = 0; k < solver.length; k++) {
	//				std::cout << solver.S[i][k];
	//			}
	//			std::cout << std::endl;

	//		}
	//		attempt.clear();
	//		attempts++;
	//	}

	//	if (black_hits == length) {
	//		std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
	//	}
	//	else {
	//		std::cout << "after " << attempts << " attempts still no solution" << std::endl;
	//	}
	//	std::cout << "the sequence generated by the code maker was:" << std::endl;
	//	for (int i = 0; i < maker.sequence.size(); i++) {
	//		std::cout << maker.sequence[i] << " ";
	//	}
	//	std::cout << std::endl;
	//	/////////
	//	maker.sequence.clear();
	//	solver.S.clear();
	//	duration = (std::clock() - start) / 1000.000;
	//	//I already have attemtps
	//	//I already have the case number.
	//	myfile << cases + 1 << "," << attempts << "," << duration << "\n";
	//	myfile.close();
	//}
///////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!////////////////////////////////
	return 0;
}



void set_random_seed() {    //seeding random fnction.
	std::srand(std::time(0));
}

int randn(int n) {     //produces a random number bewteen 1 and n;
	return (std::rand() % n) + 1;
}

////If an int is contained, return index, otherwise -1.
int Find(std::vector<int> &B, int &n) {

	for (int i = 0; i < B.size(); i++) {
		if (n == B[i]) {
			return i;
		}
	}
	return -1;

}

void mutation(std::vector<int> &parent, std::vector<int> &child) {
	int length = 4;
	int num = 6;
	child = parent;
	int chooseRandomPos = randn(length) - 1;
	child[chooseRandomPos] = randn(num);
}

void permutation(std::vector <int> &parent, std::vector<int> &child) {
	int a;
	int length = 4;
	int num = 6;
	child = parent; 
	int chooseRandomPos1 = randn(length) - 1;
	int	chooseRandomPos2 = randn(length) - 1;


	//Just in case, radn produce the same number.
	while (chooseRandomPos1 == chooseRandomPos2) {
		chooseRandomPos1 = randn(length) - 1;
	}
	
	a = child[chooseRandomPos1];
	child[chooseRandomPos1] = child[chooseRandomPos2];
	child[chooseRandomPos2] = a;
}

void inversion(std::vector <int> &parent, std::vector<int> &child) {
	int a;
	child = parent;
	int length = 4;
	int num = 6;
	int chooseRandomPos1 = randn(length) - 1;
	int	chooseRandomPos2 = randn(length) - 1;
	


	//Just in case, radn produce the same number.
	while (chooseRandomPos1 == chooseRandomPos2) {
		chooseRandomPos1 = randn(length) - 1;
	}

	
	//extracting our inversion region 
	if (chooseRandomPos1 < chooseRandomPos2) {
		while (chooseRandomPos2 - chooseRandomPos1 >= 0) {
			a = child[chooseRandomPos1];
			child[chooseRandomPos1] = child[chooseRandomPos2];
			child[chooseRandomPos2] = a;

			chooseRandomPos1++;
			chooseRandomPos2--;
		}
	}

	else {
		while (chooseRandomPos1 - chooseRandomPos2 >= 0) {
			a = child[chooseRandomPos2];
			child[chooseRandomPos2] = child[chooseRandomPos1];
			child[chooseRandomPos1] = a;

			chooseRandomPos1--;
			chooseRandomPos2++;
		}
	}
}

void createPopulation(std::vector <std::vector<int>> &population, int &size) {
	int length = 4;
	int num = 6;
	std::vector<int> single;

	int q;
	int x;

	//This is only necessary to initialize a vector so we can use single[i] down near the buttom.
	for (int j = 0; j < length; j++) {
		single.push_back(0);
	}

	for (int i = 0; i < size; i++) {

		for (int k = 0; k < length; k++) {
			x = randn(pow(num, length)) - 1;
			q = pow(num, k);
			single[k] = ((x / q) % num) + 1;
		}
		population.push_back(single);
		


	}
}
int fitness(std::vector<int> &c, std::vector<std::vector<int>> &AllAttempts, std::vector<int> &Black, std::vector<int> &White) {
	int LeftSum = 0;
	int RightSum = 0;
	int result;
	mm_code_maker demo;
	int b = 0;
	int w = 0;
	int const_a = 1, const_b = 2;
	//temporary variables - not go in the final version, because in the class already
	int length = 4;
	int num = 6;
	//
	demo.init(length, num);

	//Right and the left part in one loop
	for (int i = 1; i <= AllAttempts.size(); i++) {

		//calculating X' 
		demo.sequence = AllAttempts[i - 1];
		demo.give_feedback(c, b, w);

		LeftSum += const_a * abs(b - Black[i - 1]);
		/*RightSum += abs(w - White[i - 1]) + (const_b * num * (i - 1));*/
		RightSum += const_b * abs(w - White[i - 1]); //Modification from Jaffar.
		b = 0;
		w = 0;
	}

	result = LeftSum + RightSum;
	return result;
}

//Should make use of all modification functions. Note: crossover should always happen, whereas other changes with a certain probability.
//I am going to the set roughly equal probability of mutation, permutation or inversion occuring.
void NewGeneration(std::vector<std::vector<int>> &population, std::map <std::vector<int>, int > &SurvivalScores, float &threshold) {








}
//There areseveral options here: (a) cross strong ones with weak ones OR (b) Strong with strong - weak with weak
//OR (c) choose two parents randomly. I am trying (c) so far. 
//Also I decided to do one-point crossover only for now - two-point could be done later as a modification.
void crossOver(std::vector<int> &Parent1, std::vector<int> &Parent2, std::vector<int> &child1, std::vector<int> &child2) {
	int crossoverPoint;

	//position index at which the one-point cossover will be applied.
	crossoverPoint = randn(Parent1.size() - 1);

	//Before the crossover point
	for (int i = 0; i < crossoverPoint; i++) {
		child1.push_back(Parent1[i]);
		child2.push_back(Parent2[i]);
	}

	//After the crossover point
	for (int j = crossoverPoint; j < Parent1.size(); j++) {
		child1.push_back(Parent2[j]);
		child2.push_back(Parent1[j]);
	}
}

//Calculating mean value of fitness.
double SurvivalThreshold(std::map <std::vector<int>, int > &SurvivalScores) {
	float ScoreSum = 0;
	float result;
	for (auto x : SurvivalScores) {
		ScoreSum += x.second;
	}

	//This bit is to properly round up the result to 1d.p. Imagine you do Int's and you get 1, whereas the actual value might be 1.7
	//All 1's are going to survive, while the threshold is almost nearing 2!!, doesn't make sense, right?
	result = ScoreSum / SurvivalScores.size();

	char str[40];
	sprintf_s(str, "%.1f", result);
	sscanf_s(str, "%f", &result);

	return result;	
}

void EliminateUnderperformant(std::vector<std::vector<int>> &population, std::map <std::vector<int>, int > &SurvivalScores, float &threshold) {
	for (int i = 0; i < population.size(); i++) {
		if (SurvivalScores[population[i]] < threshold) {
			population.erase(std::find(population.begin(), population.end(), population[i]));
		}
	}
}

//This function without variation will produce some guess, thus it makes no point to break it down into smaller tasks.
//Note: our population vector here is already a newly created generation.
void NextGuess(std::map <std::vector<int>, int > &SurvivalScores, std::vector<std::vector<int>> &population, std::vector<int> &Guess) {
	
	//This is the set of the strongest codes, based on fitness, in the new generation, from which our next guess will be chosen.
	//It might be any size, though at least one code to produce the guess.
	std::vector<std::vector<int>> SetOfEligible;

	//finding the maximum score

	std::vector<int> ScoreList;

	//fill in the scorelist with all scores
	for (auto x : SurvivalScores) {
	ScoreList.push_back(x.second);
	}

	//find the biggest score (don't care about duplicates now);
	int max = ScoreList[0];
	int next;
	for (int k = 0; k < ScoreList.size(); k++) {
		next = ScoreList[k + 1];
		if (next > max) {
			max = next;
		}
	}

	//Adding all eligible codes to the eligible set
	for (auto x : SurvivalScores) {
		if (x.second = max) {
			SetOfEligible.push_back(x.first);
		}
	}
 

	//Choosing at random one of the eligible ones
	int EligiblePosition = randn(SetOfEligible.size()) - 1;
	Guess = SetOfEligible[EligiblePosition];
}




