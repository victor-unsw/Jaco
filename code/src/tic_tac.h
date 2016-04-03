//
//  other.h
//  tic_tac
//
//  Created by Demon on 03/11/15.
//  Copyright © 2015 Victor_Choudhary. All rights reserved.
//

#ifndef other_h
#define other_h


#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

char matrix[3][3];  //intitial matrix declaration

/*Functions and function prototypes below
 ==============================================================================*/

void init_matrix(void) //matrix intitialisation
{
    int i, j;
    
    for(i=0; i<3; i++)
        for(j=0; j<3; j++) matrix[i][j] =  ' ';
}


void get_player_move(int cell) //call function for player input
{
	int c = cell-1;
    int x, y;
    
    //cout<<"Enter X,Y coordinates for your move: ";
    //scanf("%d%*c%d", &x, &y);
    x = (c/3)+1;
	y = (c%3)+1;

    x--; y--;
    
    if(matrix[x][y]!= ' '){
        cout<<"Invalid move, try again.\n";
        //get_player_move();
    }
    else matrix[x][y] = 'X';

}

int get_cell_2(int r, int c){
    switch (r) {
            
        case 0:{
            if (c == 0) {
                return 1;
            }else if(c == 1)
                return 2;
            else return 3;
        }break;
            
        case 1:{
            if(c == 0)
                return 4;
            else if(c==1)
                return 5;
            else return 6;
        }break;
            
        case 2:{
            if (c == 0)
                return 7;
            else if (c == 1)
                return 8;
            else return 9;
        }break;
        
        default:
            break;
    }
    return 0;
}

int get_computer_move(void) //AI move input
{
    int i, j;
    for(i=0; i<3; i++){
        for(j=0; j<3; j++)
            if(matrix[i][j]==' ') break;
        if(matrix[i][j]==' ') break;
    }
    
    if(i*j==9)  {
        cout<<"draw\n";
        exit(0);
    }
    else
        matrix[i][j] = 'O';
    cout << "i : " << i << " , j : " << j << endl;
    return get_cell_2(i, j);
}

void disp_matrix(void) //matrix display
{
    int t;
    
    for(t=0; t<3; t++)
    {
        printf(" %c | %c | %c ",matrix[t][0],
               matrix[t][1], matrix [t][2]);
        if(t!=2)
            printf("\n---|---|---\n");
    }
    printf("\n");
}


char check(void) //used for identifying winner
{
    int i;
    
    for(i=0; i<3; i++)  /* check rows */
        if(matrix[i][0]==matrix[i][1] &&
           matrix[i][0]==matrix[i][2]) return matrix[i][0];
    
    for(i=0; i<3; i++)  /* check columns */
        if(matrix[0][i]==matrix[1][i] &&
           matrix[0][i]==matrix[2][i]) return matrix[0][i];
    
    /* test diagonals */
    if(matrix[0][0]==matrix[1][1] &&
       matrix[1][1]==matrix[2][2])
        return matrix[0][0];
    
    if(matrix[0][2]==matrix[1][1] &&
       matrix[1][1]==matrix[2][0])
        return matrix[0][2];
    
    return ' ';
}




#endif /* other_h */
