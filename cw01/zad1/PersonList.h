struct List{};
void addPerson(struct List *list, char* varFirstName, char* varLastName, char* varemail, char* varaddress, int varbDate, int varphone);
void viewEveryone(struct List *list);
void viewFromBack(struct List *list);
void deleteList(struct List *list);
void deletePerson(struct List *list, char* varFirstName, char* varLastName);
void searchForPerson(struct List *list, char* varFirstName, char* varLastName);
void sortPersonList(struct List *list);
struct List *createList();