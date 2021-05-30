/********************************************************************************
 * File: Pgm2Base.cpp
 * Author: Adwaith Moothezhath Rajesh
 * Modification History: Adwaith Moothezhath - 3/14/2021
 * Procedures:
 * shuffle  -Function which shuffles the card randomly using rand() function
 * main		  -test program which runs BlackJack 1000 times and implements
 *            different strategies for each player and the dealer 
*******************************************************************************/
#include<iostream>                                  //Header file iostream to read/write to the standard input/output streams 
#include <unistd.h>                                 //Header for pipe and fork

using namespace std;                                //namespace std is for cout

/********************************************************************************
 * void shuffleDeck(int card[], int n) 
 * Author: Adwaith Moothezhath Rajesh
 * Date: 3/14/2021
 * Modification History: Adwaith Moothezhath - 3/14/2021
 * Description: This function is used to shuffle cards. It goes through array
 *              and swaps elements with randomly chosen element in the deck.
 *              This could swap same element with itself.   
*******************************************************************************/
void shuffleDeck(int card[], int n) 
{ 
  srand(time(0));                                   //Initialize seed randomly                    
  
  for (int i=0; i<n; i++)                           //goes thorugh cardDeck       
  {  
    int r = i + (rand() % (52 -i));                 //Random for remaining positions 
    swap(card[i], card[r]);                         //swap elements 
  } 
} 
/********************************************************************************
 * int main( int argc, char *argv[] )
 * Author: Adwaith Moothezhath Rajesh
 * Date: 3/14/2021
 * Modification History: Adwaith Moothezhath - 3/14/2021
 * Description: Implements 21 game and has dealer,2 players strategies using multiple 
 * 				      process using fork() and each process communicates using pipes
 * Parameters: 
 *	argc 	I/P 		int 		The number of arguments on the command line
 * 	argv 	I/P 		char *[] 	The arguments on the command line
 * 	main	O/P		int		Status code (not currently used)  
*******************************************************************************/
int main( int argc, char *argv[] )
{
  //Initialize suits clubs,diamonds,hearts,spades
  int clubs[13]    = {11,2,3,4,5,6,7,8,9,10,10,10,10};  //array of cards of suit clubs
  int diamonds[13] = {11,2,3,4,5,6,7,8,9,10,10,10,10};  //array of cards of suit diamonds
  int hearts[13]   = {11,2,3,4,5,6,7,8,9,10,10,10,10};  //array of cards of suit hearts
  int spades[13]   = {11,2,3,4,5,6,7,8,9,10,10,10,10};  //array of cards of suit spades

  int cardDeck[52];                                     //declare array of 52 integers

  //Initialzing cardDeck with cards from
  //all suits
  for(int i = 0; i < 13; i++)                           //loop to Initialize first 13 elements in cardDeck array with clubs array 
  {
    cardDeck[i] = clubs[i];                             //initializing cardDeck array with integers in clubs array
  }
  int j =0;                                             //Counter varaible to help with initializing cardDeck with different suits
  for(int i = 13; i < 26; i++)                          //loop to Initialize next 13 elements in cardDeck array with diamonds array 
  {
    if(j > 13)                                          //if the next 13 elements are initialized break from loop 
    {
      break;
    }
    cardDeck[i] = diamonds[j];                          //initializing cardDeck array with integers in diamonds array
    j++;                                                //incrementing counter so it can print all elements in diamonds array
  }
  j =0;                                                 //reinitializing counter to 0 so it can print all elements of next suit from begining
  for(int i = 26; i < 39; i++)                          //loop to Initialize next 13 elements in cardDeck array with hearts array
  {
    if(j > 13)                                          //if the next 13 elements are initialized break from loop
    {
      break;
    }
    cardDeck[i] = hearts[j];                            //initializing cardDeck array with integers in hearts array
    j++;                                                //incrementing counter so it can print all elements in hearts array
  } 
  j =0;                                                 //reinitializing counter to 0 so it can print all elements of next suit
  for(int i = 39; i < 52; i++)                          //loop to Initialize next 13 elements in cardDeck array with spades array
  {
    if(j > 13)                                          //if the next 13 elements are initialized break from loop
    {
      break;
    }
    cardDeck[i] = spades[j];                            //initializing cardDeck array with integers in spades array
    j++;                                                //incrementing counter so it can print all elements in spades array
  }


  int bust[3] = {0,0,0};                               //array of bust to keep track of when dealer,player1, player 2 go over 21
  int winningResults[] = {0,0,0};                      //array to store number of times dealer,player1,player2 won

  //Create 3 pipes and error check if pipe is created
  //Pipe 1 to dealer, Pipe 2 to player1,
  //Pipe 3 to player2
  int fdDealer[2];                                    //creates two files descriptors one fdDealer[0] for read end one fdDealer[1] for write end                    
  if(pipe(fdDealer) == -1)                            //if pipe call returns -1 there was error creating pipe
  {
    cout << "error creating pipe" << endl;
    return 1;                                         //return value of 1 to show program has error in pipe in main code
  }
  
  int fdP1[2];                                        //creates two files descriptors one fdP1[0] for read end one fdP1[1] for write end
  if(pipe(fdP1) == -1)                                //if pipe call returns -1 there was error creating pipe
  {
    cout << "error creating pipe" << endl;
    return 1;                                         //return value of 1 to show program has error in pipe in main code
  }
  
  
  int fdP2[2];                                        //creates two files descriptors one fdP2[0] for read end one fdP2[1] for write end
  if(pipe(fdP2) == -1)                                //if pipe call returns -1 there was error creating pipe
  {
    cout << "error creating pipe" << endl;
    return 1;                                         //return value of 1 to show program has error in pipe in main code
  }
  
  
  int child = 0;                                      //Initialize child to 0
  int pnum = 0;                                       //Initialize process number to 0
  int pid = 0;                                        //Initialize process id to 0
  
  while(child < 2)                                    //repeat fork until there is 2 child
  {
    pid = fork();                                     //call fork and initalize the return value to process id
    if(pid == -1)                                     //if fork returned -1 then fork failed and error is set properly
    {
      cout << "fork failed" << endl;
      return 2;                                       //return value of 2 to show program has error in fork in main code 
    }
    if(pid == 0)                                      //if it is the child process 
    {
      pnum = child + 1;                               //set process number to child + 1
      break;                                          //break out of while loop
    }
    else
    {
      child++;                                        //if its main process increment child
      
    }
  }

  int trials = 1000;                                        //Initialize trials to 1000
  while(trials --> 0)                                       //loop to  run 1000 trials 
  {
    shuffleDeck(cardDeck, 52);                              //shuffle the deck each time new trial begins
   
    if(pnum == 0)                                           //If process is dealer
    {
      int dealerTotal = 0;                                  //initalize dealer total to 0 
      int currentCard = 6;                                  //next card on top of the deck
      int p1Total = 0;                                      //initalize player1 total to 0 
      int p2Total = 0;                                      //initalize player2 total to 0 

      dealerTotal = dealerTotal + cardDeck[0];              //add the first card to dealer total
      dealerTotal = dealerTotal + cardDeck[3];              //add the fourth card to dealer because second and third go to player 1 and player 2 respectively
      
      write(fdP1[1],&currentCard,sizeof(currentCard));      //write thorugh pipe of player1 the current card on the top of deck 
      read(fdDealer[0],&currentCard,sizeof(currentCard));   //read through pipe of dealer the current card on top of the deck
      read(fdDealer[0],&p1Total,sizeof(p1Total));           //read through pipe of dealer player1's total
      
      write(fdP2[1],&currentCard,sizeof(currentCard));      //write thorugh pipe of player2 the current card on the top of deck
      read(fdDealer[0],&currentCard,sizeof(currentCard));   //read through pipe of dealer the current card on top of the deck
      read(fdDealer[0],&p2Total,sizeof(p2Total));           //read through pipe of dealer player2's total
      
      while(dealerTotal <= 17)                              //Dealers strategy: if total is less than or equal to 17 
      {
        dealerTotal = dealerTotal + cardDeck[currentCard];  //Dealer takes current card on top of the deck and add to total
        currentCard++;                                      //set current card on top of deck to next card of the deck
      }
    
      if(dealerTotal > 21)                                  //if dealer's total goes more than 21 
      {
        bust[0] = 1;                                        //set dealer to bust because total went over 21
      }
      else
      {
        bust[0] = 0;                                        //set dealer to safe because total is under 21
      }

      if(p1Total > 21)                                       //if player1's total goes more than 21 
      {
        bust[1] = 1;                                         //set player1 to bust because total went over 21
      }
      else
      {
        bust[1] = 0;                                         //set player1 to safe because total is under 21
      }

      if(p2Total > 21)                                       //if player2's total goes more than 21 
      {
        bust[2] = 1;                                         //set player2 to bust because total went over 21
      }
      else
      {
        bust[2] = 0;                                         //set player2 to safe because total is under 21
      }
  
      if((p1Total > p2Total && p1Total > dealerTotal && bust[1] == 0) || (bust[0] == 1 && bust[1] == 0 && bust[2] == 1))          //checks of player 1's total is higher than dealer and player 2 and not over 21 or if dealer and player2 is bust and player 1 is not
      {
        winningResults[1]++;                                                                                                      //increment number of wins for player1 by 1
      }
      else if((p2Total > p1Total && p2Total > dealerTotal && bust[2] == 0) || (bust[0] == 1 && bust[1] == 1 && bust[2] == 0))     //checks of player 2's total is higher than dealer and player 1 and not over 21 or if dealer and player1 is bust and player 2 is not 
      {
        winningResults[2]++;                                                                                                      //increment number of wins for player2 by 1
      }
      else if((dealerTotal > p1Total && dealerTotal > p2Total && bust[0] == 0) || (bust[0] == 0 && bust[1] == 1 && bust[2] == 1)) //checks of dealers's total is higher than player1 and player 2 and not over 21 or if player1 and player2 is bust and dealer is not 
      {
        winningResults[0]++;                                                                                                      //increment number of wins for dealer by 1
      }
      else
      {
        winningResults[0]++;                                                                                                      //increment number of wins for dealer by 1 because dealer automatically win if both players go over 21
      }    
    }

    if(pnum == 1)                                                                                                                 //if it is player1
    {
      int p1Total = 0;                                                                                                            //initalize player1's total to 0
      p1Total = p1Total + cardDeck[1];                                                                                            //add 2nd card to player1's total because dealer takes 1st card   
      p1Total = p1Total + cardDeck[4];                                                                                            //add 5th card to player1's total because player2 takes 3rd card and dealer takes 4th card
      int currentCard = 0;                                                                                                        //initalize current card on top to 0
      int ace = 0;                                                                                                                //initalize ace to 0
      read(fdP1[0],&currentCard,sizeof(currentCard));                                                                             //read through pipe of player1 the current card on top of the deck

      while(p1Total <= 14)                                                                                                        //Player1's strategy: if player1's total is less than or equal to 14
      {
        if(cardDeck[currentCard] == 11)                                                                                           // if card is ace default value of ace is 11
        {
          if(p1Total >= 12)                                                                                                       // if player1's total is 12 or greater set ace to 1
          {
            ace = 1;                                                                                                              
          }
        }
        p1Total = p1Total + cardDeck[currentCard];                                                                                //Player1 takes card on top of deck and adds that to total
        currentCard++;                                                                                                            //set current card on top of deck to next card of the deck
        if(p1Total > 21 && ace == 1)                                                                                              //if ace is set to 1 and player1's total is more than 21
        {
          p1Total = p1Total - 10;                                                                                                 //subtract 10 from total because player1 uses ace as 1 not 11
        }
      }
      write(fdDealer[1],&currentCard,sizeof(currentCard));                                                                        //write thorugh pipe of dealer the current card on the top of deck 
      write(fdDealer[1],&p1Total,sizeof(p1Total));                                                                                //write thorugh pipe of dealer player1's total 
    }

    if(pnum == 2)                                                                                                                 //if it is player2
    {
      int p2Total = 0;                                                                                                            //initalize player2's total to 0
      p2Total = p2Total + cardDeck[2];                                                                                            //add 3rd card to player2's total because dealer takes 1st card and player1 takes 2nd card
      p2Total = p2Total + cardDeck[5];                                                                                            //add 6th card to player2's total because dealer takes 4th card and player1 takes 5th card 
      int currentCard = 0;                                                                                                        //initalize current card on top to 0                                                        
      read(fdP2[0],&currentCard,sizeof(currentCard));                                                                             //read through pipe of player2 the current card on top of the deck
      while(p2Total <= 16)                                                                                                        //Player2's strategy: if player2's total is less than or equal to 16
      {
        p2Total = p2Total + cardDeck[currentCard];                                                                                //Player2 takes card on top of deck and adds that to total                                        
        currentCard++;                                                                                                            //set current card on top of deck to next card of the deck
      }
      write(fdDealer[1],&currentCard,sizeof(currentCard));                                                                        //write thorugh pipe of dealer the current card on the top of deck
      write(fdDealer[1],&p2Total,sizeof(p2Total));                                                                                //write thorugh pipe of dealer player1's total 
    }
  }
  if(pnum == 0)                                                                                                                   //if it is dealer process
  {
    cout << "In 1000 trails the number of wins for Dealer, Player1, Player2 are:" << endl;                                         
    cout << "Dealer Won  : " << winningResults[0] << " times" << endl;                                                            //print Dealer results of 1000 trials
    cout << "Player 1 Won: " << winningResults[1] << " times" << endl;                                                            //print Player1 results of 1000 trials
    cout << "Player 2 Won: " << winningResults[2] << " times" << endl;                                                            //print Player2 results of 1000 trials
  }
  
}

