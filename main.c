#include<windows.h>
#include<string.h>
#include<stdio.h>
#include <stdbool.h>

#define MAX_BOARDS 10
#define TIMESTEP 100
//                       CTRL, C 
const int COPY_KEYS[] = {0x11, 0x43};
const int PRINT_KEY = 0x50; // P key 
const int CLEAR_ALL_KEYS[] = {VK_CONTROL,VK_SHIFT,0x44}; // D key

typedef struct Board {
  char* data;
}Board;

typedef struct Boards {
  Board boards[MAX_BOARDS];      
}Boards;

void printBoards(Boards* cbs) {
  for(int i =0;i<MAX_BOARDS;i++) {
    char* strToPrint = cbs->boards[i].data;
    if(strToPrint=="") {
      strToPrint = "EMPTY";
    }
    printf("%d: %s\n",i,strToPrint);
  }
  printf("\n\n");
}

Boards initBoardsEmpty() {
  Boards cbs = {};
  for(int i =0;i<MAX_BOARDS;i++) {
    Board board = {
      data:""
    };
    cbs.boards[i] = board;
  }
  return cbs;
}

void setBoard(Boards* cbs,size_t i,char d[]) {

  if(i>=MAX_BOARDS) {
    printf("Error: index is more than maximum boards or is less than 0. Supplied Index:%d\n",i);
    return;
  }
  cbs->boards[i].data = d;
}

void clearBoard(Boards* cbs,size_t i) {
  setBoard(cbs,i,"");
}

void clearAll(Boards* cbs) {
  for(int i =0;i<MAX_BOARDS;i++) {
    clearBoard(cbs,i);
  }
}


// -1 is no key pressed
int getNumberKeyPressed() {
  for (int key = 0x30; key <= 0x39; ++key) { // VK_0 to VK_9
    if (GetAsyncKeyState(key) & 0x8000) {
      return key-48;
    }
  }
  return -1;
}

char* getClipboardData() {
  char* result = NULL;

  if (OpenClipboard(NULL)) {
    // Get handle to clipboard data
    HANDLE hClipboardData = GetClipboardData(CF_TEXT);

    if (hClipboardData != NULL) {
      // Lock the handle to get a pointer to the data
      char* pszData = (char*)GlobalLock(hClipboardData);

      if (pszData != NULL) {
        size_t length = strlen(pszData) + 1;
        result = (char*)malloc(length);

        if (result != NULL) {
          strcpy(result, pszData);
        } else {
          printf("Failed to allocate memory for clipboard data.\n");
        }

        // Unlock the handle
        GlobalUnlock(hClipboardData);
      } else {
        printf("Failed to lock clipboard data.\n");
      }
    } else {
      printf("Failed to get clipboard data.\n");
    }

    // Close the clipboard
    CloseClipboard();
  } else {
    printf("Failed to open the clipboard.\n");
  }

  return result;
}

void printMenu() {
  printf("********************\n");
  printf("\tMultiple Clipboards v1.0\n");
  printf("Usage:\n");
  printf("\tCTRL+C+<board-number>: Copies the given content into the clipboard and also to the given board number.\n\t\tNote: <board-number> must be less than %d and more than or equal to 0\n",MAX_BOARDS);
  printf("\tP: Prints all the boards and their contents.\n");
  printf("\tM: Prints this menu.\n");
  printf("\tCTRL+SHIFT+D: Clears all boards.\n");
  printf("*******************\n");
}

int main(int argc, char *argv[]) {
  Boards b = initBoardsEmpty();
  printMenu();
  while(true) {
    if ((GetKeyState(COPY_KEYS[0]) & 0x8000)&& (GetAsyncKeyState(COPY_KEYS[1]) & 0x8000)) {
      int num = getNumberKeyPressed();
      if(num == -1) {
        continue;
      }
      char* data = getClipboardData();
      setBoard(&b,num,data);
      printBoards(&b);  
    }else if(GetAsyncKeyState(PRINT_KEY)& 0x8000) {
      printBoards(&b);
    }else if(GetAsyncKeyState('M')& 0x8000) {
      printMenu();
    }else if (GetAsyncKeyState(CLEAR_ALL_KEYS[0]) & 0x8000 && GetAsyncKeyState(CLEAR_ALL_KEYS[1]) & 0x8000 && GetAsyncKeyState(CLEAR_ALL_KEYS[2]) & 0x8000) {
      clearAll(&b);
      printBoards(&b);
    }
    Sleep(TIMESTEP);
  } 
  return 0;
}
