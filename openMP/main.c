
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
        col=0;
        for(int i =0; i<len; i++){
            if(line[i]=='|' || line[i]=='\n') continue;
            if(line[i]==' ') arr[row][col]=' ';
            else if(line[i]=='*') arr[row][col]='*';
            col++;
        }
        row++;
    }

        
    for(int gen=0; gen<numGenerations; gen++){
        // printf("----------------------\n");
        #pragma omp parallel for shared(arr , tmp)
            for(int i=0; i<numRow; i++){
                for(int j=0; j<numCol; j++){
                    int numOfAliveNeighboars = cnt_of_alive_neighboars(i , j , arr);
                    // printf("%d\n" , numOfAliveNeighboars);
                    if(arr[i][j]=='*' && (numOfAliveNeighboars < 2)) {
                        tmp[i][j]='0';
                    }
                    else if(arr[i][j]=='*' && (numOfAliveNeighboars > 3)){
                        tmp[i][j]='1';
                    }
                    else if(arr[i][j]=='*' && ( (numOfAliveNeighboars == 3) || (numOfAliveNeighboars == 2) ) ) {
                        continue;
                    }
                    else if(arr[i][j]==' ' && (numOfAliveNeighboars == 3)){
                        tmp[i][j]='3';        
                    }
                }
            }
        #pragma omp parallel for shared(arr , tmp)
            for(int i=0; i<numRow; i++){
                for(int j=0; j<numCol; j++){
                // printf("%d\n" , omp_get_thread_num());
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

    // PRINT THE ARRAY
    // for(int i=0; i<numRow; i++){
    //     for(int j=0; j<numCol; j++){
    //         printf("%c" , arr[i][j]);
    //     }
    //     printf("\n");
    // }

    return 0;
}