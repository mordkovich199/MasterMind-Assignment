#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <map>

void set_random_seed();
int randn(int n);

//function that searches a number in a vector is part of the algorithm library => not allowed => here's my own.
int FindNumInVector(std::vector<int> &b, int &n);
bool FindVectorInVector(std::vector<std::vector<int>> &B, std::vector<int> &N);

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
					if (FindNumInVector(avoid_j, j) == -1 && FindNumInVector(avoid_i, i) == -1) {    //if 'j' is not in avoid.
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

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Functions for the short algorithm!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
	void create_attempt(std::vector<int>& attempt, std::vector<std::vector<int>> &S) {
			int chooseFromS = randn(S.size()) - 1;
			attempt = S[chooseFromS];
	}

	void generate(std::vector<std::vector<int>> &S) {
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

	void learn(std::vector<int>& attempt, int black_hits, int white_hits, std::vector<std::vector<int>> &S) {
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


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Functions for the genetic algorithm (higher order)!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
	void EliminateUnderperformant(std::vector <std::vector<int>> &population, std::map <std::vector<int>, float > &SurvivalScores) {
		float threshold = SurvivalThreshold(SurvivalScores);
		for (int i = 0; i < population.size(); i++) {
			if (SurvivalScores[population[i]] < threshold) {
				population.erase(std::find(population.begin(), population.end(), population[i]));
				/*SurvivalScores.erase(population[i]);*/
				i--;
			}
		}
	}

	void crossOver(std::vector<int> &Parent1, std::vector<int> &Parent2, std::vector<int> &child1, std::vector<int> &child2) {
		int crossoverPoint;
	

		for (int i = 0; i < randn(Parent1.size()); i++) {
			//position index at which the one-point cossover will be applied.
			crossoverPoint = randn(Parent1.size() - 1);

			//Before the crossover point
			for (int k = 0; k < crossoverPoint; k++) {
				child1.push_back(Parent1[k]);
				child2.push_back(Parent2[k]);
			}

			//After the crossover point
			for (int j = crossoverPoint; j < Parent1.size(); j++) {
				child1.push_back(Parent2[j]);
				child2.push_back(Parent1[j]);
			}

			Parent1 = child1;
			Parent2 = child2;
			child1.clear();
			child2.clear();
		}
		child1 = Parent1;
		child2 = Parent2;
	}

	void mutation(std::vector<int> &child) {
		int chooseRandomPos = randn(length) - 1;
		child[chooseRandomPos] = randn(num);
	}

	void permutation(std::vector<int> &child) {
		int a;
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

	void inversion(std::vector<int> &child) {
		int a;
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

	void createPopulation(std::vector <std::vector<int>> &population) {
		std::vector<int> Individual;
		
		int x;	

		for (int i = 0; i < populationSize; i++) {

			for (int k = 0; k < length; k++) {
				x = randn(num);
				Individual.push_back(x);
			}
			//Can't have same people in the initial population.
			while (FindVectorInVector(population, Individual)) {
				Individual.clear();
				for (int k = 0; k < length; k++) {
					x = randn(num);
					Individual.push_back(x);
				}

			}
			population.push_back(Individual);
			Individual.clear();
		}
	}

	float fitness(std::vector<int> &c, std::vector<std::vector<int>> &AllAttempts, std::vector<int> &Black, std::vector<int> &White) {
		float LeftSum = 0;
		float RightSum = 0;
		float result;
		mm_code_maker demo;
		int b = 0;
		int w = 0;

		//Can play around with these constants for better optimization.
		
		
		demo.init(length, num);

		//Right and the left part in one loop
		for (int i = 1; i <= AllAttempts.size(); i++) {

			
			demo.sequence = AllAttempts[i - 1];
			demo.give_feedback(c, b, w);

			LeftSum += const_a * abs(b - Black[i - 1]);
			RightSum += abs(w - White[i - 1]) + (const_b * num * (i - 1));
			/*RightSum += const_b * abs(w - White[i - 1]);*/ //Modification from Jaffar.
			b = 0;
			w = 0;
		}

		result = LeftSum + RightSum;
		return result;
	}

	void collectData(std::vector<int>& attempt, int black_hits, int white_hits) {
		allAttempts.push_back(attempt);
		allBlacks.push_back(black_hits);
		allWhites.push_back(white_hits);
	}

	void computeFintesses(std::vector<std::vector<int>> &population, std::map <std::vector<int>, float > &SurvivalScores) {
		int individualFitness;
		for (int k = 0; k < population.size(); k++) {
			individualFitness = fitness(population[k], allAttempts, allBlacks, allWhites);
			SurvivalScores.insert(std::pair<std::vector<int>, int>(population[k], individualFitness));
		}
	}

	//Note: crossover should always happen, whereas other changes with a certain probability.
	//I am going to the set equal probability of mutation, permutation or inversion occuring.
	void NewGeneration(std::vector<std::vector<int>> &population, std::map <std::vector<int>, float > &SurvivalScores) {
		std::vector<std::vector<int>> NewGeneration;
		std::vector<int> Parent1, Parent2, Child1, Child2;
		int chance1;
		int chance2;

		// 1. Kill weak
		EliminateUnderperformant(population, SurvivalScores);
		NewGeneration = population;
		std::cout << NewGeneration.size() << std::endl;

		// 2. Add new up to the size of the population to keep it fixed.
		while (NewGeneration.size() != populationSize) {
			Parent1 = population[randn(population.size()) - 1];
			Parent2 = population[randn(population.size()) - 1];

			//Need to make sure, parents are not the same individual, otherwise impossible to produce children.
			while (Parent2 == Parent1) {

				Parent2 = population[randn(population.size()) - 1];
			}

			crossOver(Parent1, Parent2, Child1, Child2);
			//Either of three is chosen with equal chance (equal so far - can play around later)
			chance1 = randn(2);
			if (chance1 == 1) {
				inversion(Child1);
				inversion(Child2);
			}
			/*if (chance1 == 2) {
				mutation(Child1);
				mutation(Child2);
			}*/
			if (chance1 == 3) {
				permutation(Child1);
				permutation(Child2);
			}

			//In case elimination happened in such a way that the number of new individuals needed is odd.
			if (populationSize - NewGeneration.size() == 1) {
				chance2 = randn(2);
				if (chance2 == 1) {
					NewGeneration.push_back(Child1);
				}

				else {
					NewGeneration.push_back(Child2);
				}
			}

			else {
				NewGeneration.push_back(Child1);
				NewGeneration.push_back(Child2);
			}

			Child1.clear();
			Child2.clear();
		}
		population = NewGeneration;
	}

	//Calculating mean value of fitness.
	float SurvivalThreshold(std::map <std::vector<int>, float > &SurvivalScores) {
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
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		return result;
	}

	//This function without variation will produce some guess, thus it makes no point to break it down into smaller tasks.
	//Note: our population vector here is already a newly created generation.
	void NextGuess(std::map <std::vector<int>, float > &SurvivalScores, std::vector<std::vector<int>> &population, std::vector<int> &Guess) {

		//This is the set of the strongest codes, based on fitness, in the new generation, from which our next guess will be chosen.
		//It might be any size, though at least one code to produce the guess.
		std::vector<std::vector<int>> SetOfEligible;

		//////finding the maximum score//////
		std::vector<int> ScoreList;

		//fill in the scorelist with all scores
		for (auto x : SurvivalScores) {
			ScoreList.push_back(x.second);
		}

		//find the biggest score (don't care about duplicates now);
		int max = ScoreList[0];
		int next;
		for (int k = 0; k < ScoreList.size() - 1; k++) {
			next = ScoreList[k + 1];
			if (next > max) {
				max = next;
			}
		}
		/////////////////////////////////////

		//Adding all eligible codes to the eligible set
		for (auto x : SurvivalScores) {
			if (x.second == max) {
				SetOfEligible.push_back(x.first);
			}
		}


		//Choosing at random one of the eligible ones
		int EligiblePosition = randn(SetOfEligible.size()) - 1;
		Guess = SetOfEligible[EligiblePosition];
	}

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

	int length;
	int num;
	int const_a = 2, const_b = 1;
	int populationSize = 350;
	std::vector<std::vector<int>> allAttempts;
	std::vector<int> allBlacks;
	std::vector<int> allWhites;
	

};


//Main decides whether to go into the long algorithm mode or short one. Based on this decission, all necessary variables specific to
//short or long algorithm are declared and used. Think of the solver as sort of your cards on hand and main as the player that plays
//these.
int main() {
	//!!!!!!!!!!!!!!!!!!!!!!!!FOR THE GENETIC!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
	set_random_seed();
	mm_code_maker maker;
	mm_solver solver;
	
	std::map <std::vector<int>, float > SurvivalScores;
	std::vector<std::vector<int>> population, NewGeneration;
	std::vector<int> attempt;
	
	int const maxGenerations = 10;
	int genCount = 0;
	int attempts = 0;
	int black_hits = 0, white_hits = 0;

	int length, num;
	std::cout << "enter length of sequence and number of possible values:" << std::endl;
	std::cin >> length >> num;
	solver.init(length, num);
	maker.init(length, num);
	maker.generate_sequence();
	std::cout << "The actual code: ";
	for (int y = 0; y < maker.sequence.size(); y++) {
		std::cout << maker.sequence[y] << " ";
	}
	std::cout << std::endl;
	

	while (black_hits < length) {
		
			solver.createPopulation(population);
			std::cout << "Checked" << std::endl;
			while ((genCount != maxGenerations) && (black_hits < length)) {
				black_hits = 0; 
				white_hits = 0;

				if (genCount == 0) {
					int randIndex = randn(population.size()) - 1;
					attempt = population[randIndex];
				}

				else {
					solver.NextGuess(SurvivalScores, population, attempt);
				}

				maker.give_feedback(attempt, black_hits, white_hits);

				std::cout << "attempt: " << std::endl;
				for (int i = 0; i < attempt.size(); i++) {
					std::cout << attempt[i] << " ";
				}
				std::cout << std::endl;
				std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;

				std::cout << population.size() << std::endl;

				solver.collectData(attempt, black_hits, white_hits);
				solver.computeFintesses(population, SurvivalScores);
				solver.NewGeneration(population, SurvivalScores);
				SurvivalScores.clear();
				solver.computeFintesses(population, SurvivalScores);
				

				genCount++;
				attempts++;
				attempt.clear();
				
			}
			std::cout << "Starting a new population " << std::endl;
			population.clear();
			solver.allAttempts.clear();
			solver.allBlacks.clear();
			solver.allWhites.clear();
			
			genCount = 0;
		
	}

	
		std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
	
	
		std::cout << "the sequence generated by the code maker was:" << std::endl;

	for (int i = 0; i < maker.sequence.size(); i++) {
			std::cout << maker.sequence[i] << " ";
	}
		std::cout << std::endl;

	
	




	//!!!!!!!!!!!!!!!!!!!!FOR THE KNUTH!!!!!!!!!!!!!!!!///
	//std::vector<int> attempt;
	//std::vector<std::vector<int>> S;

	//set_random_seed();

	//int length, num;
	//std::cout << "enter length of sequence and number of possible values:" << std::endl;
	//std::cin >> length >> num;
	//
	//mm_solver solver;
	//solver.init(length, num);
	//solver.generate(S);
	//std::cout << "S: " << std::endl;
	//for (int i = 0; i < S.size(); i++) {
	//	for (int k = 0; k < solver.length; k++) {
	//		std::cout << S[i][k];
	//	}
	//	std::cout << std::endl;
	//}

	//mm_code_maker maker;
	//maker.init(length, num);
	//maker.generate_sequence();

	//std::cout << "the code is: ";

	//for (int i = 0; i < maker.sequence.size(); i++) {
	//	std::cout << maker.sequence[i] << " ";
	//}

	//std::cout << std::endl;

	//int black_hits = 0, white_hits = 0;
	//int attempts_limit = 5000;
	//int attempts = 0;
	//while ((black_hits < length) && (attempts < attempts_limit)) {
	//	black_hits = 0;
	//	white_hits = 0;

	//	//Initial guess according to Knuth.
	//	if (attempts == 0) {
	//		attempt = { 1, 1, 2, 2 };
	//	}
	//	else {
	//		solver.create_attempt(attempt, S);
	//	}

	//	maker.give_feedback(attempt, black_hits, white_hits);
	//	std::cout << "attempt: " << std::endl;
	//	for (int i = 0; i < attempt.size(); i++) {
	//		std::cout << attempt[i] << " ";
	//	}
	//	std::cout << std::endl;
	//	std::cout << "black pegs: " << black_hits << " " << " white pegs: " << white_hits << std::endl;

	//	solver.learn(attempt, black_hits, white_hits, S);
	//	std::cout << "S: " << std::endl;
	//	for (int i = 0; i < S.size(); i++) {
	//		for (int k = 0; k < solver.length; k++) {
	//			std::cout << S[i][k];
	//		}
	//		std::cout << std::endl;

	//	}
	//	attempt.clear();
	//	attempts++;
	//}

	//if (black_hits == length) {
	//	std::cout << "the solver has found the sequence in " << attempts << " attempts" << std::endl;
	//}
	//else {
	//	std::cout << "after " << attempts << " attempts still no solution" << std::endl;
	//}
	//std::cout << "the sequence generated by the code maker was:" << std::endl;
	//for (int i = 0; i < maker.sequence.size(); i++) {
	//	std::cout << maker.sequence[i] << " ";
	//}
	//std::cout << std::endl;

	return 0;
}



void set_random_seed() {    //seeding random fnction.
	std::srand(std::time(0));
}

int randn(int n) {     //produces a random number bewteen 1 and n;
	return (std::rand() % n) + 1;
}

////If an int is contained, return index, otherwise -1.
int FindNumInVector(std::vector<int> &B, int &n) {

	for (int i = 0; i < B.size(); i++) {
		if (n == B[i]) {
			return i;
		}
	}
	return -1;
}

bool FindVectorInVector(std::vector<std::vector<int>> &B, std::vector<int> &N) {
	bool equal = false;
	for (int i = 0; i < B.size(); i++) {
		if (B[i] == N) {
			equal = true;
		}
	}
	return equal;
}