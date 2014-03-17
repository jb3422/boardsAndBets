/*
 * program.c
 *
 *  Created on: Jan 10, 2014
 *      Author: Hillel Chaitoff
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <json/json.h>
#include "cJSON.h"
#include <math.h>


#define BUFFSIZE	2048

void print(const char * a){
	int i;
	for(i = 0; i < strlen(a); i++){
		printf("%c", a[i]);
	}
}


char* file2str(char *fName){
	int i, c, size;
	char x;
	char* json;
	FILE* file;
	
	file = fopen(fName, "r");
	
	c = 0;
	// Get number of chars
	while((x = fgetc(file)) != EOF){
		c++;
		// printf("%d \n", c);
	}
	// Allocate space for the json
	size = c + 1;
	json = malloc(size);
	rewind(file);	// Back to top for reading
	//Read the file and store the json
	for(i = 0; i < size; i++){
		fscanf(file, "%c ", &json[i]);
	}
	fclose(file);
	json[size - 1] = '\0';
	return json;
}

/**
* This function compares a Team's average goals versus
* a Goalie's saves.
*
* @param teamGoals		This is the average goals of a team
* @param goalie			This is the averages saves of the goalie
* @return score			returns the score for the team being compared to the
* goalie
*/
double scoreFactor(double teamGoals, double goalie){
	double score = 0;
	double factor = 0;
	/*determine the score factor based on the goalies GAA.  A lower GAA should be a higher value.
	The idea behind this is explained below after the factor is determined.
	*/
	if (goalie >= 2 && goalie < 2.20)
	{
		factor = 2.5;
	}
	if (goalie >= 2.21 && goalie < 2.40)
	{
		factor = 2.0;
	}
	if (goalie >= 2.41 && goalie < 2.60)
	{
		factor = 1.5;
	}
	if (goalie >=2.61)
	{
		factor = 1.0;
	}
	//printf("\nfactor is %f", factor);
	score = teamGoals +  goalie;
	/*Add the teamGoals to the goalies GAA.  If a goalie has a lower GAA
	this will result in a lower number
	example : teamA goals (2.547) + teamB goalie (2.26) will result 
			in a value (4.807) the lower the value the lower the score 
			should be because it should be harder to score on 
			a goalie with a lower GAA.  Then divide this by the score factor
			obtained above.  A goalie with a low GAA will have a high number
			and the quotient will yeild a smaller result of goals for teamA
			in the case above it would yeild 1.92 which will give a score of 
			2 goals.
	*/
	score = score/factor;
	//printf("\nafter math score is %f", score);
	score = round(score);
	//printf("\nafter math score is %f", score);
	return score;
}

/**
* This function is used if scoreFactor() returns two even scores.
* And the game has to be decided in overtime or by shootout.
* 
* @param otA		average overtime goals for team A
* @param otB		average overtime goals for team B
* @param goalieA	avaerage saves for goalie A
* @param goalieB	average saves for goalie B
* @param nameA		three letter abbreviation for team name A
* @param nameB		three letter abbreviation for team name B
*/
void otFactor(double otA, double otB, double goalieA, double goalieB, 
	char *nameA, char *nameB){

	if(round(otA) > round(otB)){
		printf("%s wins in overtime! \n", nameA);
	}
	else if(round(otA) < round(otB)){
		printf("%s wins in overtime! \n", nameB);
	}
	else{
		if(round(goalieA) > round(goalieB)){
			printf("%s wins by shootout! \n", nameA);
		}
		else if(round(goalieA) < round(goalieB)){
			printf("%s wins by shootout! \n", nameB);
		}
		else{
			printf("Not enough information to determine winner.\n");
		}
	}
}

/**
* This function uses both scoreFactor() and otFactor() to determine
* the outcome of the desired game.
*
* @param teamGoalsA		average # of goals for team A
* @param teamGoalsB		average # of goals for team B
* @param goalieA		average # of saves for goalie A
* @param goalieB		average # of saves for goalie B
* @param scoreA			rounded score for team A
* @param scoreB			rounded score for team B
*	@param otA				average overtime stats for team A
* @param otB				average overtime stats for team B
* @param nameA			three letter abbreviation for team name A
* @param nameB			three letter abbreviation for team name B
*/
void gameDecision(double teamGoalsA, double teamGoalsB, double goalieA,double goalieB,
	double otA, double otB, char *nameA, char *nameB){
	//printf("before scoreFactor goals A = %f and goals B = %f\n", teamGoalsA, teamGoalsB);
	double scoreA = scoreFactor(teamGoalsA, goalieB);
	double scoreB = scoreFactor(teamGoalsB, goalieA);
	//printf("\nafter score factor scoreA is %f and scoreB is %f\n", scoreA, scoreB);
 	int finalA = scoreA;
	int finalB = scoreB;	
	if(scoreA == scoreB){
		otFactor(otA, otB, goalieA, goalieB, nameA, nameB);
	}
	else if(scoreA > scoreB){
		printf("\n%s: %d %1s: %d\n", nameA, finalA, nameB, finalB);
	}
	else{
		printf("\n%s: %d %1s: %d\n", nameB, finalB, nameA, finalA);
	}
}

/*
	This function reads in the JSON char array and parses it for player data
	The values we are interested in are totalgames played, and goals
	this function returns an average goals per game of all skaters (NOT GOALIES) 
*/

double getPlayerValues(char *skaterData)
{
	
	cJSON* request_json = NULL;
	cJSON* items = cJSON_CreateArray();
	cJSON* root = cJSON_Parse(skaterData);
	int count;
	int i = 0;
	int gamesPlayed = 0;
	int goals = 0;
	int sumGames = 0;
	int sumGoals = 0;
	int maxGames = 0;
	int total = 0;
	cJSON *item = cJSON_GetObjectItem(root, "skaterData");//get all data in skaterData array in JSON
	for (i = 0; i < cJSON_GetArraySize(item); ++i)//scan every element in the JSON array
	{	
		int gamesPlayed = 0;	
		int goals = 0;
		count = 0;
		//printf("next player");
		cJSON *data = cJSON_GetArrayItem(item,i);//get all JSON objects
		//printf("\n %d", i);
		char *test = cJSON_Print(data);//send the array items to string
		//printf("\n %s", test);

		//parse the data and get values
		char tokchar[6];
		char *cptr;
		
		
		strcpy(tokchar, ",");
		cptr = strtok(test, tokchar);
		//printf("\n %s", cptr);
		count++;
		while(cptr!= NULL)
		{
			cptr= strtok(NULL, tokchar);
			if (count == 4)//items number 4 is games played
			{
				gamesPlayed = atoi(cptr);
				/*The tricky thing is not everybody has played the same amount of games
				so you must determine the total games played.  Find the first value and set 
				it to max.  Check it against all other values, if a higher value is found
				then set it to max.  
				*/
				if (gamesPlayed > maxGames)
				{
					maxGames = gamesPlayed;
				}
				//printf("games played is %d ", gamesPlayed);
			}
			if (count == 5)//obtain the goals for each player
			{
				goals = atoi(cptr);
				//printf("\n goals scored is %d ", goals);
			}
			//printf("\n %s index is %d ", cptr,count);
			count++;
		}//end while loop to parse strings
		sumGames = sumGames + gamesPlayed;
		sumGoals = sumGoals + goals;
		total++;
	}//end for loop to get all player data from json
	//printf("max games = %d\n", maxGames);
	//printf("total goals is = %d\n", sumGoals);
	double average =(double)sumGoals/maxGames;
	//printf("\naerage goals per game is %f", average); 
	return average;
}

/*
	This function reads in the JSON char array and parses it for player data
	The values we are interested in are goals against, which is an average 
	amount of goals a goalie lets in per game based on total games played.  
	This function returns an average GAA of all goalies.
*/

double getGoalieValues(char *goalieData)
{
	
	cJSON* request_json = NULL;
	cJSON* items = cJSON_CreateArray();
	cJSON* root = cJSON_Parse(goalieData);
	
	int i;
	int count = 0;
	int total = 0;
	//int gamesPlayed = 0;
	double goalsAgainst = 0;
	double sumAgainst = 0;
	//int sumGames = 0;
	cJSON *item = cJSON_GetObjectItem(root, "goalieData");//get all data in goalieData array
	for (i = 0; i < cJSON_GetArraySize(item); ++i)
	{
		cJSON *data = cJSON_GetArrayItem(item,i);//get all items in the JSON object
		//printf("\n %d", i);
		char *test = cJSON_Print(data);
		//printf("\n %s", test);
		char tokchar[6];
		char *cptr;
		//parse the information	
		strcpy(tokchar, ",");
		cptr = strtok(test, tokchar);
		//printf("\n %s", cptr);
		count++;
		while(cptr!= NULL)
		{
			cptr= strtok(NULL, tokchar);
			/*
			if (count == 4)//get total games played for this goalie
			{
				gamesPlayed = atoi(cptr);
				//printf("games played is %d ", gamesPlayed);
			}*/
			if (count == 12)//get GAA against for this goalie
			{
				goalsAgainst = atof(cptr);
				//printf("\n goals against is %f ", goalsAgainst);
			}
			//printf("\n %s index is %d ", cptr,count);
			count++;
		}//end while loop to parse strings
		//sumGames = sumGames + gamesPlayed;
		sumAgainst = sumAgainst + goalsAgainst;
		total++;
	}//end for loop to get all player data from json
	//printf("total players is = %d\n", total);
	//printf("total goals against is = %f\n", sumAgainst);
	double average =(double)sumAgainst/total;
	//printf("\naverage goals per game is %f", average); 
	
	return average;
}

/*
	This function reads in the JSON char array and parses it for player data
	The values we are interested in are OT wins against, which is the average 
	amount of OT wins between all goalies.  THis number will help decide which 
	team has the edge if the game goes into overtime.
	This function returns an average OT of all goalies.
*/

double getGoalieOt(char *goalieData)
{
	
	cJSON* request_json = NULL;
	cJSON* items = cJSON_CreateArray();
	cJSON* root = cJSON_Parse(goalieData);
	
	int i;
	int count = 0;
	int total = 0;
	double ot = 0;
	double sum = 0;
	cJSON *item = cJSON_GetObjectItem(root, "goalieData");
	for (i = 0; i < cJSON_GetArraySize(item); ++i)
	{
		cJSON *data = cJSON_GetArrayItem(item,i);
		//printf("\n %d", i);
		char *test = cJSON_Print(data);
		//printf("\n %s", test);
		char tokchar[6];
		char *cptr;
		//parse the information	
		strcpy(tokchar, ",");
		cptr = strtok(test, tokchar);
		//printf("\n %s", cptr);
		count++;
		while(cptr!= NULL)
		{
			cptr= strtok(NULL, tokchar);
			if (count == 7)//get OT number of wins
			{
				ot = atoi(cptr);
				//printf("games played is %d ", gamesPlayed);
			}
			
			//printf("\n %s index is %d ", cptr,count);
			count++;
		}//end while loop to parse strings
		sum = sum + ot;
		total++;
	}//end for loop to get all player data from json
	sum = ot/total;	
	//printf("\n overtime points %f", sum); 
	
	return ot;
}

int main( int argc, char * argv[]) {
	int size1;
	int size2;
	char* json1;
	char* json2;
	
	double teamA = 0;
	double goalieA = 0;
	char* nameA = argv[1];//set team A
	char* nameB = argv[2];//set team B
	double otA = 0;
	double teamB = 0;
	double goalieB = 0;
	double otB = 0;
	// Files to Strings
	json1 = file2str("T1.json");
	json2 = file2str("T2.json");

	//parse team A skater data, then team A goalie data, then teamA goalie OT 
	teamA = getPlayerValues(json1);
	goalieA = getGoalieValues(json1);
	otA = getGoalieOt(json1);
	//parse team B skater data, then team B goalie data, then teamB goalie OT
	teamB = getPlayerValues(json2);
	goalieB = getGoalieValues(json2);
	otB = getGoalieOt(json2);
	
	printf("\nteam A average goals is = %f", teamA);
	printf("\nteam A goalie is          %f", goalieA);
	printf("\nteam B average goals is = %f", teamB);
	printf("\nteam B goalie is          %f", goalieB);
	printf("\n");
	
	//default for now
	/*
	double otA = 1;
	double otB = 3;
	*/
	//hard coded
	//char* nameA = "PHI";
	//char* nameB = "ANA";
	
	
	//determine which team wins!!
	gameDecision(teamA, teamB, goalieA, goalieB, otA, otB, nameA, nameB);

}

