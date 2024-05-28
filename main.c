#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  char *id;
  double gpa;
  int creditHours;
} Student;

typedef struct StudentNode {
  Student *pStudent;
  struct StudentNode *pNext;
} StudentNode;

typedef struct {
  StudentNode *pIDList;        // stores pointers to all students
  StudentNode *pHonorRollList; // points to students on honor roll (gpa > 3.5)
  StudentNode *pAcademicProbationList; // gpa < 2.00
  StudentNode *pFreshmanList;          // 0-29 credits
  StudentNode *pSophomoreList;         // 30-59 credits
  StudentNode *pJuniorList;            // 60-89 credits
  StudentNode *pSeniorList;            // 90 hours min
} Database;

void printStudent(Database *db) {
  char studentID[20];
  printf("Enter the id of the student to find: ");
  scanf("%s", studentID);

  StudentNode *curr = db->pIDList;
  while (curr != NULL) {
    if (strcmp(curr->pStudent->id, studentID) == 0) {
      printf("%s: \n", curr->pStudent->name);
      printf("\tID - %s\n", curr->pStudent->id);
      printf("\tGPA - %.2f\n", curr->pStudent->gpa);
      printf("\tCredit Hours - %d\n", curr->pStudent->creditHours);
      return;
    }
    curr = curr->pNext;
  }

  printf("Sorry, there is no student in the database with the id %s.\n",
         studentID);
}

void sortInfo(Database *db, Student *s) {
  StudentNode *pID = malloc(sizeof(StudentNode));
  pID->pStudent = s;
  pID->pNext = NULL;

  // add the student to the ID list
  if (db->pIDList == NULL) {
    db->pIDList = pID;
  } else {
    StudentNode *curr = db->pIDList;
    while (curr != NULL && curr->pNext != NULL) {
      curr = curr->pNext;
    }
    curr->pNext = pID;
  }

  // add the student to the appropriate year list
  StudentNode *p = malloc(sizeof(StudentNode));
  p->pStudent = s;
  p->pNext = NULL;

  int credits = s->creditHours;
  if (credits < 30) {
    if (db->pFreshmanList == NULL) {
      db->pFreshmanList = p;
    } else {
      StudentNode *curr = db->pFreshmanList;
      while (curr != NULL && curr->pNext != NULL) {
        curr = curr->pNext;
      }
      curr->pNext = p;
    }
  } else if (credits < 60) {
    if (db->pSophomoreList == NULL) {
      db->pSophomoreList = p;
    } else {
      StudentNode *curr = db->pSophomoreList;
      while (curr != NULL && curr->pNext != NULL) {
        curr = curr->pNext;
      }
      curr->pNext = p;
    }
  } else if (credits < 90) {
    if (db->pJuniorList == NULL) {
      db->pJuniorList = p;
    } else {
      StudentNode *curr = db->pJuniorList;
      while (curr != NULL && curr->pNext != NULL) {
        curr = curr->pNext;
      }
      curr->pNext = p;
    }
  } else {
    if (db->pSeniorList == NULL) {
      db->pSeniorList = p;
    } else {
      StudentNode *curr = db->pSeniorList;
      while (curr != NULL && curr->pNext != NULL) {
        curr = curr->pNext;
      }
      curr->pNext = p;
    }
  }

  // update the honor roll and academic probation lists
  if (s->gpa >= 3.5) {
    StudentNode *q = malloc(sizeof(StudentNode));
    q->pStudent = s;
    q->pNext = db->pHonorRollList;
    db->pHonorRollList = q;
  } else if (s->gpa < 2.0) {
    StudentNode *q = malloc(sizeof(StudentNode));
    q->pStudent = s;
    q->pNext = db->pAcademicProbationList;
    db->pAcademicProbationList = q;
  }
}

void addStudent(Database *db) {
  Student *s = malloc(sizeof(Student));

  printf("Enter the name of the new student: ");
  char name1[50];
  char name2[50];
  scanf("%s %s", name1, name2);

  char *fullName = malloc(strlen(name1) + strlen(name2) + 2);
  strcpy(fullName, name1);
  strcat(fullName, " ");
  strcat(fullName, name2);
  s->name = fullName;

  printf("Enter the ID of the new student: ");
  char id[20];
  scanf("%s", id);
  s->id = strdup(id);

  printf("Enter the GPA of the new student: ");
  scanf("%lf", &s->gpa);

  printf("Enter the credit hours of the new student: ");
  scanf("%d", &s->creditHours);

  sortInfo(db, s);

  printf("Successfully added the following student to the database!\n");
  printf("%s: \n", s->name);
  printf("\tID - %s\n", s->id);
  printf("\tGPA - %.2f\n", s->gpa);
  printf("\tCredit Hours - %d\n", s->creditHours);
}

void addStudentsfromfile(Database *db) {
  char filename[50];
  printf("Enter the name of the file you would like to use: \n");
  scanf("%s", filename);

  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("Error opening file.\n");
    return;
  }

  char line[1000];
  fgets(line, 1000, file);
  while (fgets(line, 1000, file)) {
    Student *s = malloc(sizeof(Student));

    char *name = strtok(line, ",");
    char *id = strtok(NULL, ",");
    double gpa = atof(strtok(NULL, ","));
    int creditHours = atoi(strtok(NULL, ","));

    s->name = strdup(name);
    s->id = strdup(id);
    s->gpa = gpa;
    s->creditHours = creditHours;
    sortInfo(db, s);
  }
  fclose(file);
  // printf("Successfully added students from file!\n");
}

StudentNode *insertSortedByID(StudentNode *head, StudentNode *newNode) {
  if (head == NULL || strcmp(head->pStudent->id, newNode->pStudent->id) > 0) {
    newNode->pNext = head;
    return newNode;
  }
  StudentNode *curr = head;
  while (curr->pNext != NULL &&
         strcmp(curr->pNext->pStudent->id, newNode->pStudent->id) < 0) {
    curr = curr->pNext;
  }
  newNode->pNext = curr->pNext;
  curr->pNext = newNode;
  return head;
}

// helper function to free memory allocated for a linked list of students
void freeStudentList(StudentNode *head) {
  while (head != NULL) {
    StudentNode *next = head->pNext;
    free(head->pStudent);
    free(head);
    head = next;
  }
}

void print10Database(Database *db) {
  int count = 0;
  StudentNode *curr = db->pIDList;
  StudentNode *sortedList = NULL;
  while (curr != NULL) {
    StudentNode *next = curr->pNext;
    sortedList = insertSortedByID(sortedList, curr);
    curr = next;
  }
  curr = sortedList;
  while (curr != NULL && count < 10) {
    printf("%s: \n", curr->pStudent->name);
    printf("\tID - %s\n", curr->pStudent->id);
    printf("\tGPA - %.2f\n", curr->pStudent->gpa);
    printf("\tCredit Hours - %d\n", curr->pStudent->creditHours);
    curr = curr->pNext;
    count++;
  }
}

// helper function to insert a student node into a linked list sorted by ID

void printHonorRoll(Database *db) {

  if (db == NULL || db->pHonorRollList == NULL) {
    printf("There are no students matching that criteria.\n\n");
    return;
  }
  StudentNode *curr = db->pHonorRollList;

  // First, count the number of students on the honor roll.
  int numStudents = 0;
  while (curr != NULL) {
    numStudents++;
    curr = curr->pNext;
  }
  if (numStudents == 0) {
    printf("There are no students matching that criteria.\n\n");
    return;
  }

  // Then, create an array to store pointers to the students on the honor roll.
  Student **students = malloc(numStudents * sizeof(Student *));

  curr = db->pHonorRollList;
  for (int i = 0; i < numStudents; i++) {
    students[i] = curr->pStudent;
    curr = curr->pNext;
  }

  // Sort the array in ascending order by GPA.
  for (int i = 0; i < numStudents - 1; i++) {
    for (int j = i + 1; j < numStudents; j++) {
      if (students[i]->gpa > students[j]->gpa) {
        Student *tmp = students[i];
        students[i] = students[j];
        students[j] = tmp;
      }
    }
  }

  // Finally, print out the students on the honor roll in ascending order by
  // GPA.
  for (int i = 0; i < numStudents; i++) {
    printf("%s: \n", students[i]->name);
    printf("\tID - %s\n", students[i]->id);
    printf("\tGPA - %.2f\n", students[i]->gpa);
    printf("\tCredit Hours - %d\n", students[i]->creditHours);
  }

  free(students);
}

void printProbation(Database *db) {
  if (db == NULL || db->pAcademicProbationList == NULL) {
    printf("There are no students matching that criteria.\n\n");
    return;
  }

  StudentNode *curr = db->pAcademicProbationList;

  // First, count the number of students on probation.
  int numStudents = 0;
  while (curr != NULL) {
    if (curr->pStudent->gpa < 2.0) {
      numStudents++;
    }
    curr = curr->pNext;
  }

  if (numStudents == 0) {
    printf("There are no students matching that criteria.\n\n");
    return;
  }
  // Then, create an array to store pointers to the students on probation.
  Student **students = malloc(numStudents * sizeof(Student *));

  curr = db->pAcademicProbationList;
  int index = 0;
  while (curr != NULL) {
    if (curr->pStudent->gpa < 2.0) {
      students[index] = curr->pStudent;
      index++;
    }
    curr = curr->pNext;
  }

  // Sort the array in ascending order by GPA.
  for (int i = 0; i < numStudents - 1; i++) {
    for (int j = i + 1; j < numStudents; j++) {
      if (students[i]->gpa > students[j]->gpa) {
        Student *tmp = students[i];
        students[i] = students[j];
        students[j] = tmp;
      }
    }
  }

  // Finally, print out the students on probation in ascending order by
  // GPA.
  for (int i = 0; i < numStudents; i++) {
    printf("%s: \n", students[i]->name);
    printf("\tID - %s\n", students[i]->id);
    printf("\tGPA - %.2f\n", students[i]->gpa);
    printf("\tCredit Hours - %d\n", students[i]->creditHours);
  }

  free(students);
}

void printYear(Database *db, char year) {
  char *yearName;
  StudentNode *yearList;

  switch (year) {
  case '1':
    yearName = "Freshman";
    yearList = db->pFreshmanList;
    break;
  case '2':
    yearName = "Sophomore";
    yearList = db->pSophomoreList;
    break;
  case '3':
    yearName = "Junior";
    yearList = db->pJuniorList;
    break;
  case '4':
    yearName = "Senior";
    yearList = db->pSeniorList;
    break;
  default:
    printf("Invalid year\n");
    return;
  }

  // count number of students in the list
  int numStudents = 0;
  StudentNode *curr = yearList;
  while (curr != NULL) {
    numStudents++;
    curr = curr->pNext;
  }

  // create an array of pointers to students
  Student **students = malloc(numStudents * sizeof(Student *));
  curr = yearList;
  int i = 0;
  while (curr != NULL) {
    students[i] = curr->pStudent;
    curr = curr->pNext;
    i++;
  }

  if (numStudents == 0) {
    printf("There are no students matching that criteria.\n\n");
    free(students);
    return;
  }

  // sort the array of pointers using a bubble sort algorithm
  for (int i = 0; i < numStudents - 1; i++) {
    for (int j = 0; j < numStudents - i - 1; j++) {
      if (strcmp(students[j]->name, students[j + 1]->name) > 0) {
        Student *temp = students[j];
        students[j] = students[j + 1];
        students[j + 1] = temp;
      }
    }
  }

  // print the information of each student in order of their name
  for (int i = 0; i < numStudents; i++) {
    printf("%s: \n", students[i]->name);
    printf("\tID - %s\n", students[i]->id);
    printf("\tGPA - %.2f\n", students[i]->gpa);
    printf("\tCredit Hours - %d\n", students[i]->creditHours);
  }

  free(students);
}

void deleteStudent(Database *db) {
  char studentID[20];
  printf("Enter the id of the student to be removed: ");
  scanf("%s", studentID);

  // check if the student ID is valid
  StudentNode *prev = NULL;
  StudentNode *curr = db->pIDList;
  while (curr != NULL) {
    if (strcmp(curr->pStudent->id, studentID) == 0) {
      break;
    }
    prev = curr;
    curr = curr->pNext;
  }
  if (curr == NULL) {
    printf("Sorry, there is no student in the database with the id %s.",
           studentID);
    return;
  }

  // remove the student from the ID list
  if (prev == NULL) {
    db->pIDList = curr->pNext;
  } else {
    prev->pNext = curr->pNext;
  }

  // remove the student from the year list
  int credits = curr->pStudent->creditHours;
if (credits < 30) {
  if (db->pFreshmanList->pStudent == curr->pStudent) {
    db->pFreshmanList = db->pFreshmanList->pNext;
  } else {
    StudentNode *yearcurr = db->pFreshmanList;
    while (yearcurr != NULL && yearcurr->pNext != NULL &&
           yearcurr->pNext->pStudent != curr->pStudent) {
      yearcurr = yearcurr->pNext;
    }
    if (yearcurr != NULL && yearcurr->pNext != NULL) {
      yearcurr->pNext = yearcurr->pNext->pNext;
    }
  }
} else if (credits < 60) {
  if (db->pSophomoreList->pStudent == curr->pStudent) {
    db->pSophomoreList = db->pSophomoreList->pNext;
  } else {
    StudentNode *yearcurr = db->pSophomoreList;
    while (yearcurr != NULL && yearcurr->pNext != NULL &&
           yearcurr->pNext->pStudent != curr->pStudent) {
      yearcurr = yearcurr->pNext;
    }
    if (yearcurr != NULL && yearcurr->pNext != NULL) {
      yearcurr->pNext = yearcurr->pNext->pNext;
    }
  }
} else if (credits < 90) {
  if (db->pJuniorList->pStudent == curr->pStudent) {
    db->pJuniorList = db->pJuniorList->pNext;
  } else {
    StudentNode *yearcurr = db->pJuniorList;
    while (yearcurr != NULL && yearcurr->pNext != NULL &&
           yearcurr->pNext->pStudent != curr->pStudent) {
      yearcurr = yearcurr->pNext;
    }
    if (yearcurr != NULL && yearcurr->pNext != NULL) {
      yearcurr->pNext = yearcurr->pNext->pNext;
    }
  }
} else {
  if (db->pSeniorList->pStudent == curr->pStudent) {
    db->pSeniorList = db->pSeniorList->pNext;
  } else {
    StudentNode *yearcurr = db->pSeniorList;
    while (yearcurr != NULL && yearcurr->pNext != NULL &&
           yearcurr->pNext->pStudent != curr->pStudent) {
      yearcurr = yearcurr->pNext;
    }
    if (yearcurr != NULL && yearcurr->pNext != NULL) {
      yearcurr->pNext = yearcurr->pNext->pNext;
    }
  }
}

  // remove the student from the honor roll and academic probation lists
StudentNode *q = db->pHonorRollList;
if (curr == db->pHonorRollList) {
  db->pHonorRollList = db->pHonorRollList->pNext;
} else {
  while (q != NULL && q->pNext != NULL && q->pNext->pStudent != curr->pStudent) {
    q = q->pNext;
  }
  if (q != NULL && q->pNext != NULL) {
    StudentNode *temp = q->pNext;
    q->pNext = temp->pNext;
    free(temp);
  }
}
StudentNode *k = db->pAcademicProbationList;
if (curr == db->pAcademicProbationList) {
  db->pAcademicProbationList = db->pAcademicProbationList->pNext;
} else {
  while (k != NULL && k->pNext != NULL && k->pNext->pStudent != curr->pStudent) {
    k = k->pNext;
  }
  if (k != NULL && k->pNext != NULL) {
    StudentNode *temp = k->pNext;
    k->pNext = temp->pNext;
    free(temp);
  }
}

  // free memory used by the deleted student
  free(curr->pStudent->name);
  free(curr->pStudent->id);
  free(curr->pStudent);
  free(curr);
}

void readDatabase(Database *db) {
  char filename[100];
  printf("Enter E to start with an empty database, \n");
  printf("or F to start with a database that has information on students from "
         "a file.\n");
  printf("Your choice --> ");
  char choice = ' ';
  scanf(" %c", &choice);

  db->pIDList = NULL;
  db->pHonorRollList = NULL;
  db->pAcademicProbationList = NULL;
  db->pFreshmanList = NULL;
  db->pSophomoreList = NULL;
  db->pJuniorList = NULL;
  db->pSeniorList = NULL;

  while (1) {
    if (choice == 'E') {
      return;
    } else if (choice == 'F') {
      addStudentsfromfile(db);
      return;
    } else {
      printf("Sorry, that input was invalid. Please try again.\n");
      printf("Your choice --> ");
      scanf(" %c", &choice);
    }
  }
}

int main() {
  Database db;
  Student *s = malloc(sizeof(Student));
  printf("CS 211, Spring 2023\n");
  printf("Program 4: Database of Students\n\n");
  readDatabase(&db);

  char choice = ' ';
  while (1) {
    printf(
        "\nEnter: \tC to create a new student and add them to the database,\n");
    printf("\tR to read from the database,\n");
    printf("\tD to delete a student from the database, or\n");
    printf("\tX to exit the program.\n");
    printf("Your choice --> ");
    scanf(" %c", &choice);

    switch (choice) {
    case 'C':
      if (db.pIDList == NULL) {
        addStudent(&db);
      } else {
        addStudent(&db);
      }
      break;
    case 'R':
      printf("Select one of the following: \n");
      printf("\t1) Display the head (first 10 rows) of the database\n");
      printf(
          "\t2) Display students on the honor roll, in order of their GPA\n");
      printf("\t3) Display students on academic probation, in order of their "
             "GPA\n");
      printf("\t4) Display freshmen students, in order of their name\n");
      printf("\t5) Display sophomore students, in order of their name\n");
      printf("\t6) Display junior students, in order of their name\n");
      printf("\t7) Display senior students, in order of their name\n");
      printf("\t8) Display the information of a particular student\n");
      while (1) {
        int option;
        printf("Your choice --> ");
        scanf("%d", &option);
        if (option >= 1 && option <= 8) {
          switch (option) {
          case 1:
            print10Database(&db);
            break;
          case 2:
            printHonorRoll(&db);
            break;
          case 3:
            printProbation(&db);
            break;
          case 4:
            printYear(&db, '1');
            break;
          case 5:
            printYear(&db, '2');
            break;
          case 6:
            printYear(&db, '3');
            break;
          case 7:
            printYear(&db, '4');
            break;
          case 8:
            printStudent(&db);
            break;
          }
          break; // break out of the while loop
        } else {
          printf("Sorry, that input was invalid. Please try again.\n");
        }
      }
      continue; // go back to the main menu

    case 'D':
      deleteStudent(&db);
      break;
    case 'X':
      printf("\nThanks for playing!\n");
      printf("Exiting...\n");
      return 0;
    }
  }

  return 0;
}