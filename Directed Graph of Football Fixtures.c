#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct graphVertex Vertex;
struct graphVertex
{
    Vertex* next;
    char team[81];
    int inDegree;
    int outDegree;
    int isVisited;
    struct graphEdge* firstArc;
};

typedef struct graphEdge Edge;
struct graphEdge
{
    Vertex* destination;
    int weight;
    Edge* nextArc;
};

typedef struct graphHead graphHead;
struct graphHead
{
    int count;
    Vertex* first;
};

graphHead* readTeams(FILE*);
void readMatches(FILE*, graphHead*);
graphHead* createGraph(void);
void createVertex(graphHead*, char[]);
void createEdge(graphHead*, char[], char[], int);
void printGraph(graphHead*);
void getMostWins(graphHead*);
void getMostLosses(graphHead*);
void getMaxGoals(graphHead*);
void getMinGoals(graphHead*);
int checkwinChain(graphHead*, char[], char[]);
int checkPath(graphHead*, char[], char[]);

void dft(Vertex *);

int main(int argc, char *argv[]){
    FILE *inFile;
    graphHead *graph;
    inFile = fopen("teams.txt", "r");
    if (inFile == NULL){
        printf("File could not be opened.\n");
        exit(1);
    }
    printf("Teams File has been opened successfully; the graph with no edges can be seen below:\n");
    graph = readTeams(inFile);
    printGraph(graph);
    printf("***************************************************************************************************************\n");

    fclose(inFile);
    inFile = fopen("matches.txt", "r");
    if (inFile == NULL){
        printf("File could not be opened.\n");
        exit(1);
    }
    printf("Matches File has been opened successfully; the graph with edges can be seen below:\n");
    readMatches(inFile, graph);
    printGraph(graph);
    printf("***************************************************************************************************************\n");

    getMostWins(graph);
    getMostLosses(graph);
    getMaxGoals(graph);
    getMinGoals(graph);
    printf("***************************************************************************************************************\n");
    if (argc < 3){
        printf("Two teams are not given in the command line arguments!");
    }
    else{
        char str1[81], str2[81];
        int i = 0;
        while (argv[1][i] != '\0'){
            if (argv[1][i] == '_')
                str1[i] = ' ';
            else
                str1[i] = argv[1][i];
            i++;
        }
        str1[i] = '\0';
        i = 0;
        while (argv[2][i] != '\0'){
            if (argv[2][i] == '_')
                str2[i] = ' ';
            else
                str2[i] = argv[2][i];
            i++;
        }
        str2[i] = '\0';

        int result = checkwinChain(graph, str1, str2);
        if (result)
            printf("%s have beaten a team that beat %s.\n", str1, str2);

        else
            printf("%s have NOT beaten a team that beat %s.\n", str1, str2);

        int result2 = checkPath(graph, str1, str2);
        if (result2)
            printf("There is a path from %s to %s", str1, str2);
        else
            printf("There is no path from %s to %s", str1, str2);
    }
    return 0;
}

graphHead* createGraph(void){
    graphHead *graph;
    graph = (graphHead*)malloc(sizeof(graphHead));
    if(graph == NULL)
    {
        printf("Memory allocation is failed.");
        exit(1);
    }
    graph->count = 0;
    graph->first = NULL;
    return graph;
}

void createVertex(graphHead *graph, char teamName[]){
    Vertex *vertex = (struct graphVertex *) malloc(sizeof(struct graphVertex));
    vertex->next = NULL;
    strcpy(vertex->team, teamName);
    vertex->inDegree = 0;
    vertex->outDegree = 0;
    vertex->firstArc = NULL;
    vertex->isVisited = 0;

    graph->count++;

    if(graph->first == NULL){
        graph->first = vertex;
    }
    else {
        struct graphVertex *temp = graph->first;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = vertex;
    }
}

graphHead* readTeams(FILE* inFile){
    
    // WRITE YOUR CODE HERE
    //Calling the CreateGraph Function to create a graph;
    graphHead *graph;
    graph = createGraph();
    //Temp variable to store the team name;
    char temp_team[100];
    //Reading the team file line by line;
    while(fscanf(inFile, "%[^\n]\n", temp_team) != EOF){
      	//sending the team name and graph to add the vertex;
    	createVertex(graph, temp_team);
	}
    //returning the graph back to the main function;
    return graph;
    
}

void readMatches(FILE* inFile, graphHead *graph){
    
    // WRITE YOUR CODE HERE
    //Temp Variable to store the data from matches.txt file;
    int temp_year,temp_wk,temp_homegoals,temp_awaygoals;
    char temp_home_team[100],temp_away_team[100],temp_date[50];
    char temp_ftr,c;
    //printf("Start of readMatches Functions\n");
    //Skipping the first line;
	while((c=fgetc(inFile))!=EOF){
		//printf("Skipping first line\n");
		if(c=='\n'){
			//GETTING OUT OF THE WHILE LOOP USING THE BREAK STATEMENT WHEN FILE IS ON SECOND LINE.
			break;
		}
	}
    
    while(fscanf(inFile,"%d;%d;%[^;];%[^;];%d;%d;%[^;];%c\n", &temp_year, &temp_wk, temp_date, temp_home_team,&temp_homegoals,&temp_awaygoals,temp_away_team,&temp_ftr ) != EOF){
    	//When home_team is the winner;
    	if(temp_homegoals > temp_awaygoals){
				//sending the data to createedge to create the edge when home team is the winner;
    		createEdge(graph,temp_home_team,temp_away_team,temp_homegoals-temp_awaygoals);
		}
		//When away_team is the winner;
		else if(temp_homegoals < temp_awaygoals){
			//sending the data to createedge to create the edge when away team is the winner;
				createEdge(graph,temp_away_team,temp_home_team,temp_awaygoals-temp_homegoals);
		}
	
	}
}

void createEdge(graphHead* graph, char winningTeam[], char losingTeam[], int goalDiff){
    
    // WRITE YOUR CODE HERE
    Vertex *temp,*temp2,*temp3;
    Edge *edge_temp;
    
    //Creating the edge and allocating the memory for it;
    Edge *new_edge= (struct graphEdge *) malloc(sizeof(struct graphEdge));
    if(new_edge == NULL){
    	printf("Failed to allocate memory for new egde\n");
    	exit(1);
	}
	new_edge->nextArc=NULL;
	new_edge->weight = goalDiff;
    
    temp = graph->first;
    //traversing the graph;
    while(temp!=NULL){
    	edge_temp = temp->firstArc;
    	temp2 = graph->first;
    	temp3 = graph->first;

        //Finding the vertex of winning team;
    	if(strcmp(temp->team, winningTeam) == 0){
            //if there are zero edges;
    		if(temp->firstArc == NULL){
    			temp->firstArc = new_edge;
    			temp->outDegree++;
    			while(temp2!=NULL){
    				if(strcmp(temp2->team, losingTeam) == 0){
    					temp->firstArc->destination = temp2;
    					temp2->inDegree++;
					}
					
					temp2 = temp2->next;
				} 
			}
            //else we will find the last postion to store the loosing team;
			else{
			
			while(edge_temp->nextArc != NULL){
				edge_temp = edge_temp->nextArc;
				}
				
					edge_temp->nextArc =new_edge;
    				temp->outDegree++;
    			while(temp3!=NULL){
    				if(strcmp(temp3->team, losingTeam) == 0){
    					new_edge->destination = temp3;
    					temp3->inDegree++;
					}
    				temp3= temp3->next;
    				
				}
			}
		}// End of If Statement
	temp = temp->next;
	} // End of Outter While loop;
}


void printGraph(graphHead* graph){
	Vertex *temp;
	Edge *edge_temp;
	
	char team_name[100], team_name2[100];
	temp = graph->first;
	while(temp!=NULL){
		strcpy(team_name,temp->team);
		printf("%s -> ",team_name);
		edge_temp = temp->firstArc;
		while(edge_temp != NULL){
			strcpy(team_name2,edge_temp->destination->team);

			if(edge_temp->nextArc == NULL){
				printf("%s | %d",team_name2,edge_temp->weight);
				
			}
			else{
			printf("%s | %d -> ",team_name2,edge_temp->weight);	
			}
			
			edge_temp=edge_temp->nextArc;
		}
		printf("\n-------------------------------------------------------\n");
		temp = temp->next;
	}
}

void getMostWins(graphHead *graph){

	printf("The following stats have been generated:\n");
	printf("Team with the most victories:\n");
    Vertex *temp;
    int max=-99,count=0;
    char team_name[100];
	Edge *edge_temp;
	//Simply comparing the outdegrees in this function;
	temp = graph->first;
	temp = graph->first;
	while(temp!=NULL){
		if(temp->outDegree > max){
			max = temp->outDegree;
			strcpy(team_name,temp->team);
		}
		
		temp = temp->next;
	}
	
	printf("%s have won %d matches\n\n",team_name,max);

}

void getMostLosses(graphHead *graph){

    printf("Team with the most losses:\n");
	
    Vertex *temp;
    int max=-99;
    char team_name[100];
	Edge *edge_temp;
	//Simply comparing the outdegrees;
	temp = graph->first;
	while(temp!=NULL){
		
		if(temp->inDegree >max){
			max=temp->inDegree;
			strcpy(team_name,temp->team);

		}
		temp = temp->next;
	}
	printf("%s have lost %d matches\n\n",team_name,max);

}

void getMaxGoals(graphHead *graph){

   printf("Team with the highest goal difference:\n");
	
    Vertex *temp,*temp2;
    int max=-9999,count=0,count1=0;
    char team_name[100], team_name2[100];
	Edge *edge_temp,*edge_temp2;
	
	temp = graph->first;
    //First counting the goals when the team won the match;
	while(temp!=NULL){
		
		strcpy(team_name2,temp->team);
		edge_temp = temp->firstArc;
		while(edge_temp != NULL){
			
		    count = count + edge_temp->weight;
			edge_temp=edge_temp->nextArc;
		}
		
		temp2 = graph->first;
        //counting the goals when they lost the match;
		while(temp2!=NULL){
			
			if(strcmp(team_name2,temp2->team) != 0 ){
				edge_temp2 = temp2->firstArc;
				while(edge_temp2 != NULL){
					
					if(strcmp(edge_temp2->destination->team,team_name2) == 0){
						count1 = count1 + edge_temp2->weight;
					}
				edge_temp2=edge_temp2->nextArc;	
				}
			}
			temp2 =temp2->next;
		}
		if( (count - count1) > max){
			max = count - count1;
			strcpy(team_name,team_name2);
		}
		count=0;
		count1=0;
		temp = temp->next;
	}
	
	printf("%s have a goal difference of %d\n\n",team_name,max);
}

void getMinGoals(graphHead *graph){

   printf("Team with the lowest goal difference:\n");
	//doing the same thing as previous function;
    Vertex *temp,*temp2;
    int min=9999,count=0,count1=0;
    char team_name[100], team_name2[100];
	Edge *edge_temp,*edge_temp2;
	
	temp = graph->first;
	while(temp!=NULL){
		
		strcpy(team_name2,temp->team);
		edge_temp = temp->firstArc;
		while(edge_temp != NULL){
			
		    count = count + edge_temp->weight;
			edge_temp=edge_temp->nextArc;
		}
		
		temp2 = graph->first;
		while(temp2!=NULL){
			if(strcmp(team_name2,temp2->team) != 0 ){
				edge_temp2 = temp2->firstArc;
				while(edge_temp2 != NULL){
					
					if(strcmp(edge_temp2->destination->team,team_name2) == 0){
						count1 = count1 + edge_temp2->weight;
					}
				edge_temp2=edge_temp2->nextArc;	
				}
			}
			temp2 =temp2->next;
		}
		if( (count - count1) < min){
			min = count - count1;
			strcpy(team_name,team_name2);
		}
		count=0;
		count1=0;
		temp = temp->next;
	}
	printf("%s have a goal difference of %d\n\n",team_name,min);
}

int checkwinChain(graphHead *graph, char team1[], char team2[]){

	Vertex *vertex_temp, *vertex_temp2;
	Edge *edge_temp, *edge_temp2;
	vertex_temp= graph->first;
	vertex_temp2= graph->first;
	edge_temp= vertex_temp->firstArc;
	
	//In this function we are finding the vertex of team1;
    // then tranersing the edging of team 1;
    //before we move to the next edge we check check the arc of the edges present in team 1;
    // and we look for team 2;
	while(vertex_temp != NULL){
		if(strcmp(team1, vertex_temp->team) == 0){
			while(edge_temp != NULL){
				while(vertex_temp2 != NULL){
					if(strcmp(vertex_temp2->team, edge_temp->destination->team) == 0){
						edge_temp2 = vertex_temp2->firstArc;
						while(edge_temp2 != NULL){

							if(strcmp(edge_temp2->destination->team, team2) == 0){
								return 1;	
							}

							edge_temp2 = edge_temp2->nextArc;
						}//end of while loop;
					}//end of if statement;
					vertex_temp2 = vertex_temp2->next;
				}//End
				//Reseting the pointer for vertex_temp2;
				vertex_temp2 = graph->first;
				
				edge_temp = edge_temp->nextArc;
			} // end of inside while loop;
			
		}// End of if statement;
	
	
	
		vertex_temp = vertex_temp->next;	
	} // End of while loop;
	return 0;
}
void dft(Vertex *v){
    //traversing  the arc making there isvisted equal to 1;
    Edge *edge_temp=NULL;
    Vertex *u=NULL;
    v->isVisited=1;
    for(edge_temp = v->firstArc; edge_temp!=NULL; edge_temp=edge_temp->nextArc){
        u=edge_temp->destination;
        if(u->isVisited != 1){
            //calling recursively;
            dft(u);
        }
    }
}

int checkPath(graphHead *graph, char team1[], char team2[]) {

    Vertex *temp = graph->first;
    Vertex *temp2 = graph->first;
    //Finding the vertex for team1;
    while(temp!=NULL){
        if (strcmp(temp->team, team1) == 0){
            break;}
        temp=temp->next;
    }//sending the vertex of team to dft function;
    dft(temp);
        //looking for the team2 vertex;
    while (temp2 != NULL) {
        if (strcmp(temp2->team, team2) == 0)
            //checking dft function have visited team2
            //if yes , it return 1
            //else when this while loop will end and return 0 to main function;
            if (temp2->isVisited == 1) {
                return 1;
            }
        temp2 = temp2->next;
    }
    return 0;
	
}
