#pragma warning (disable:4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INIT_CAPACITY 100
#define BUFFER_LENGTH 100
#define INIT_CAPACITY 100


typedef struct {
	char* name;
	char* number;
	char* email;
	char* group;
}Person;

Person** directory;
int capacity;
int n;

void load(char* fileName);
void add(char* name, char* number, char* email, char* group);
void reallocate();
void remove(char* name);
void release_person(Person* p);
void handle_add(char* name);
void save(char* fileName);
void status();
void find(char* name);
int search(char* name);
void print_person(Person* p);

void init()
{
	directory = (Person**)malloc(INIT_CAPACITY * sizeof(Person*));
	capacity = INIT_CAPACITY;
	n = 0;
}

int compose_name(char str[], int limit)
{
	char* ptr;
	int length = 0;

	ptr = strtok(NULL, " ");
	if (ptr == NULL)
		return 0;
	
	strcpy(str, ptr);
	length += strlen(ptr);

	while ((ptr = strtok(NULL, " ")) != NULL) {
		if (length + strlen(ptr) + 1 < limit) {
			str[length++] = ' ';
			str[length] = '\0';
			strcat(str, ptr);
			length += strlen(ptr);
		}
	}
	return length;
}

int read_line(FILE* fp, char str[], int limit)
{
	int ch, i = 0;
	while ((ch = fgetc(fp)) != '\n' && ch != EOF)
		if (i < limit)
			str[i++] = ch;

	str[i] = '\0';

	return i;
}

int main()
{
	init();
	char command_line[BUFFER_LENGTH];
	char* command, * argument;
	char name_str[BUFFER_LENGTH];

	while (1) {
		printf("$ ");
		if (read_line(stdin, command_line, BUFFER_LENGTH) <= 0)
			continue;

		command = strtok(command_line, " ");
		
		if (strcmp(command, "load") == 0) {
			argument = strtok(NULL, " ");
			if (argument == NULL) {
				printf("Invalid arguments.\n");
				continue;
			}
			load(argument);
		}
		else if (strcmp(command, "add") == 0) {
			if (compose_name(name_str, BUFFER_LENGTH) <= 0) {
				printf("Name required.\n");
				continue;
			}
			handle_add(name_str);
		}
		else if (strcmp(command, "find") == 0) {
			if (compose_name(name_str, BUFFER_LENGTH) <= 0) {
				printf("Name required.\n");
				continue;
			}
			find(name_str);
		}
		else if (strcmp(command, "status") == 0) {
			status();
		}
		else if (strcmp(command, "remove") == 0) {
			if (compose_name(name_str, BUFFER_LENGTH) <= 0) {
				printf("Invalid arguments.\n");
				continue;
			}
			remove(name_str);
		}
		else if (strcmp(command, "save") == 0) {
			argument = strtok(NULL, " ");
			if (strcmp(argument, "as") != 0) {
				printf("Invalid arguments.\n");
				continue;
			}
			argument = strtok(NULL, " ");
			if (argument == NULL) {
				printf("Invalid arguments.\n");
				continue;
			}
			save(argument);
		}
		else if (strcmp(command, "exit") == 0)
			break;
	}

	return 0;
}

void load(char* fileName)
{
	char buffer[BUFFER_LENGTH];
	char* name, * number, * email, * group;
	char* token;

	FILE* fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("Open failed.\n");
		return;
	}

	while (1) {
		if (read_line(fp, buffer, BUFFER_LENGTH) <= 0)
			break;
		name = strtok(buffer, "#");
		token = strtok(NULL, "#");
		if (strcmp(token, " ") == 0)
			number = NULL;
		else
			number = strdup(token);
		token = strtok(NULL, "#");
		if (strcmp(token, " ") == 0)
			email = NULL;
		else
			email = strdup(token);
		token = strtok(NULL, "#");
		if (strcmp(token, " ") == 0)
			group = NULL;
		else
			group = strdup(token);
		add(strdup(name), number, email, group);
	}
	fclose(fp);
}

void add(char* name, char* number, char* email, char* group)
{
	if (n >= capacity)
		reallocate();

	int i = n - 1;
	while (i >= 0 && strcmp(directory[i]->name, name) > 0) {
		directory[i + 1] = directory[i];
		i--;
	}
	
	directory[i + 1] = (Person*)malloc(sizeof(Person));

	directory[i + 1]->name = name;
	directory[i + 1]->number = number;
	directory[i + 1]->email = email;
	directory[i + 1]->group = group;
	n++;
}

void reallocate()
{
	capacity *= 2;
	Person** tmp = (Person**)malloc(capacity * sizeof(Person*));
	for (int i = 0; i < n; i++)
		tmp[i] = directory[i];
	free(directory);
	directory = tmp;
}

void remove(char* name)
{
	int i = search(name);
	if (i == -1) {
		printf("No person named '%s' exists.\n", name);
		return;
	}

	Person* p = directory[i];
	for (int j = i; j < n - 1; j++)
		directory[j] = directory[j + 1];
	n--;
	release_person(p);
	printf("'%s' was deleted successfully.\n", name);
}

void release_person(Person* p)
{
	free(p->name);
	if (p->number != NULL) free(p->number);
	if (p->email != NULL) free(p->email);
	if (p->group != NULL) free(p->group);
	free(p);
}

void handle_add(char* name)
{
	char number[BUFFER_LENGTH], email[BUFFER_LENGTH], group[BUFFER_LENGTH];
	char empty[] = " ";

	printf("	Phone: ");
	read_line(stdin, number, BUFFER_LENGTH);

	printf("	Email: ");
	read_line(stdin, email, BUFFER_LENGTH);

	printf("	Group: ");
	read_line(stdin, group, BUFFER_LENGTH);

	add(strdup(name), strdup((char*)(strlen(number) > 0 ? number : empty)), strdup((char*)(strlen(email) > 0 ? email : empty)), strdup((char*)(strlen(group) > 0 ? group : empty)));
}

void save(char* fileName)
{
	int i;

	FILE* fp = fopen(fileName, "w");
	if (fp == NULL) {
		printf("Open failed.\n");
		return;
	}

	for (i = 0; i < n; i++) {
		fprintf(fp, "%s#", directory[i]->name);
		fprintf(fp, "%s#", directory[i]->number);
		fprintf(fp, "%s#", directory[i]->email);
		fprintf(fp, "%s#\n", directory[i]->group);
	}
	fclose(fp);
}

void status()
{
	int i;
	for (i = 0; i < n; i++)
		print_person(directory[i]);
	printf("Total %d persons.\n", n);
}

void find(char* name)
{
	int index = search(name);
	if (index == -1)
		printf("No person named '%s' exists.\n", name);
	else
		print_person(directory[index]);
}

int search(char* name)
{
	int i;
	for (i = 0; i < n; i++) {
		if (strcmp(name, directory[i]->name) == 0) {
			return i;
		}
	}
	return -1;
}

void print_person(Person *p)
{
	printf("%s:\n", p->name);
	printf("	Phone: %s\n", p->number);
	printf("	Email: %s\n", p->email);
	printf("	Group: %s\n", p->group);
}