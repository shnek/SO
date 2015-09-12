#include<stdio.h>
#include<stdlib.h>
#include<string.h>


struct Node{
	char* firstName;
	char* lastName;
	char* email;
	char* address;
	int bDate;
	int phone;
	struct Node* next;
	struct Node* previous;
};
struct List{
	struct Node* first;
	struct Node* last;
};

void addPerson(struct List *list, char* varFirstName, char* varLastName, char* varemail, char* varaddress, int varbDate, int varphone){
	struct Node *node = (struct Node*)malloc(sizeof(struct Node));
	struct Node *temp = list -> last;
	if(list -> last != NULL){
		temp -> next = node;
		list -> last = node;
		node -> previous = temp;
		node -> next = NULL;
	}else{
		node -> next = NULL;
		node -> previous = NULL;
		list -> first = list -> last = node;
	}
	node -> firstName = malloc(strlen(varFirstName)+1);
	if(varFirstName!= NULL){
		node -> firstName = malloc(strlen(varFirstName+1));
		strcpy(node -> firstName,varFirstName);
	}
	if(varLastName!= NULL){
		node -> lastName = malloc(strlen(varLastName+1));
		strcpy(node -> lastName, varLastName);
	}
	if(varemail!= NULL){
		node -> email = malloc(strlen(varemail+1));
		strcpy(node -> email,varemail);
	}
	if(varemail!=NULL){
		node -> address = malloc(strlen(varaddress+1));
		strcpy(node -> address,varaddress);
	}
	if(varbDate!= 0){
		node -> bDate = varbDate;
	}
	if(varphone!=0){
		node -> phone = varphone;
	}	
}

void viewEveryone(struct List *list){
	printf("View all:\n");
	if(list -> first == NULL) return;
	struct Node *temp = list -> first;
	while(temp != NULL){
		printf("FN: %s, LN: %s, Birth: %i\n", temp -> firstName, temp->lastName, temp->bDate);
		temp = temp -> next;
	}
}
void viewFromBack(struct List *list){
	printf("View from back: \n");
	if(list -> last == NULL) return;
	struct Node *temp = list -> last;
	while(temp != NULL){
		printf("FN: %s, LN: %s, Birth: %i\n", temp -> firstName, temp -> lastName, temp->bDate);
		temp = temp -> previous;
	}
}

void deleteList(struct List *list){
	struct Node *temp = list -> first;
	while(list -> first != NULL){
		list -> first = temp -> next;
		free(temp -> firstName);
		free(temp -> lastName);
		free(temp -> email);
		free(temp -> address);
		free(temp);
		temp = list -> first;
	}
	free(list);
}
void deletePerson(struct List *list, char* varFirstName, char* varLastName){
	struct Node *temp = list -> first;
	while(temp != NULL){
		if(strcmp(temp->firstName, varFirstName) == 0){
			if(strcmp(temp-> lastName, varLastName) == 0){
				temp -> previous -> next = temp -> next;
				temp -> next -> previous = temp -> previous;
				free(temp -> firstName);
				free(temp -> lastName);
				free(temp -> email);
				free(temp -> address);
				free(temp);
			}
		}
		temp = temp -> next;
	}
}

void searchForPerson(struct List *list, char* varFirstName, char* varLastName){
	struct Node *temp = list -> first;
	while(temp != NULL){
		if(strcmp(temp->firstName, varFirstName) == 0){
			if(strcmp(temp-> lastName, varLastName) == 0){
				printf("Happy chosen is: %s %s %i\n", temp -> firstName, temp -> lastName, temp -> bDate);
			}
		}
		temp = temp -> next;
	}
}

void sortPersonList(struct List *list){
	if(list -> first == NULL || list -> first -> next == NULL) {
		printf("im there");
		return;
	}
	else{
		struct Node *temp;
		struct Node *new_first = NULL;
		struct Node *new_last = NULL;
		struct Node *chosen;
		while(list -> first != NULL){
			chosen = list -> first;
			temp = list -> first;
			while(temp != NULL){
				if(temp -> bDate > chosen -> bDate){
					chosen = temp;
				}
				temp = temp -> next;
			}
			if(chosen == list -> first){
				list -> first = chosen -> next;
			}
			if(new_first == NULL){
				new_first = new_last = chosen;
				if(chosen -> previous != NULL){
					chosen -> previous -> next = chosen -> next;
				}
				if(chosen -> next != NULL){
					chosen -> next -> previous = chosen -> previous;	
				}
				chosen -> next = NULL;
				chosen -> previous = NULL;
			}else{
				new_last -> next = chosen;
				if(chosen -> previous != NULL){
					chosen -> previous -> next = chosen -> next;
				}
				if(chosen -> next != NULL){
					chosen -> next -> previous = chosen -> previous;	
				}
				chosen -> previous = new_last;
				chosen -> next = NULL;
				new_last = new_last -> next;
			}
		}
		list -> first = new_first;
		list -> last  = new_last;		
	}
}
struct List *createList(){
	struct List *list = (struct List*)malloc(sizeof(struct List));
	return list;
}
