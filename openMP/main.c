
#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

int numRow , numCol;


int cnt_of_alive_neighboars(int row , int col , char** arr){
    int cnt=0;
    for(int i=row-1; i<=row+1; i++){
        for(int j=col-1; j<=col+1; j++){
            if( (i<0 || i>=numRow) || (j<0 || j>=numCol) || (i==row && j==col) ) continue;
            if(arr[i][j] == '*') cnt++;
            // printf("1");
        }
    }
    return cnt;
}


int main(int argc , char** argv){
    char* fileName = argv[1];
    char *line;
    FILE* file = fopen(fileName, "r");
    size_t len = 0;
    size_t read;
    int numGenerations = atoi(argv[2]);

    if(!file){
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }

    read = getline(&line, &len, file);
    sscanf(line, "%d %d",&numRow,&numCol);
    printf("numRow = %d and numCol=%d \n" , numRow , numCol);
    char *arr[numRow];
    char *tmp[numRow];
    for(int i=0; i<numRow; i++){
        arr[i] = (char*)malloc(numCol * sizeof(char));
        tmp[i] = (char*)malloc(numCol * sizeof(char));
    }
    
    int row=0,col=0;
    while ((read = getline(&line, &len, file)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        // printf("%s", line);
        col=0;
        for(int i =0; i<len; i++){
            if(line[i]=='|' || line[i]=='\n') continue;
            if(line[i]==' ') arr[row][col]=' ';
            else if(line[i]=='*') arr[row][col]='*';
            col++;
        }
        row++;
    }

    // #pragma omp paraller for shared(arr)
    // {
        int numOfAliveNeighboars=0;
        for(int gen=0; gen<numGenerations; gen++){
            for(int i=0; i<numRow; i++){
                for(int j=0; j<numCol; j++){
                    numOfAliveNeighboars = cnt_of_alive_neighboars(i , j , arr);
                    // printf("%d\n" , numOfAliveNeighboars);
                    // printf("2");
                    if(arr[i][j]=='*' && (numOfAliveNeighboars < 2)) {
                        // arr[i][j]=' ';
                        tmp[i][j]='0';
                    }
                    else if(arr[i][j]=='*' && (numOfAliveNeighboars > 3)){
                        // arr[i][j]=' ';
                        tmp[i][j]='1';
                    }
                    else if(arr[i][j]=='*' && ( (numOfAliveNeighboars == 3) || (numOfAliveNeighboars == 2) ) ) {
                        // tmp[i][j]='2';
                        continue;
                    }
                    else if(arr[i][j]==' ' && (numOfAliveNeighboars == 3)){
                        // arr[i][j]='*';        
                        tmp[i][j]='3';        
                        // printf("4\n");
                    }
                }
            }
            for(int i=0; i<numRow; i++){
                for(int j=0; j<numCol; j++){
                    if(tmp[i][j]=='0' || tmp[i][j]=='1'){
                        arr[i][j]=' ';
                        tmp[i][j]=' ';
                    }
                    else if(tmp[i][j]=='3'){
                        arr[i][j]='*';
                        tmp[i][j]=' ';
                    }
                }
            }
        }
    // }


    // for(int i=0; i<numRow; i++){
    //     for(int j=0; j<numCol; j++){
    //         printf("%c" , arr[i][j]);
    //     }
    //     printf("\n");
    // }

    return 0;
}