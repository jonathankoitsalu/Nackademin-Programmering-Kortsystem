#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "safeinput.h"
#include <stdbool.h>
#include <time.h>
#include "serial.h"
#include <Windows.h>



typedef struct
{
	int cardnumber;
	int access;
	time_t CreationDate;
} CARD;


typedef struct
{
	CARD* AllCards;
	int AmountOfCards;
}SYSTEM_STATE;

void ListCards(SYSTEM_STATE* state)
{
	printf("Card List\n");
	for (int i = 0; i < state->AmountOfCards; i++)
	{
		CARD c = state->AllCards[i];
		if (c.access == 0) {
			printf("Cardnumber: %d\t\taccess: denied\t\t", c.cardnumber, c.access);
			struct tm* datum = localtime(&state->AllCards[i].CreationDate);
			printf("Date of creation: %4d-%02d-%02d %02d:%02d:%02d%\n", datum->tm_year + 1900, datum->tm_mon + 1, datum->tm_mday, datum->tm_hour, datum->tm_min, datum->tm_sec);
		}
		if (c.access == 1) {
			printf("Cardnumber: %d\t\taccess: granted\t\t", c.cardnumber, c.access);
			struct tm* datum = localtime(&state->AllCards[i].CreationDate);
			printf("Date of creation: %4d-%02d-%02d %02d:%02d:%02d%\n", datum->tm_year + 1900, datum->tm_mon + 1, datum->tm_mday, datum->tm_hour, datum->tm_min, datum->tm_sec);
		}
	}
}

void ChangeAccess(SYSTEM_STATE* state)
{
	int cardnumber;
	GetInputInt("6-digit cardnumber: ", &cardnumber);
	int i = 0;
	while (cardnumber != state->AllCards[i].cardnumber) 
	{
		i++;
	}
	int access;
	GetInputInt("0 to deny access and 1 to grant acccess: ", &access);
	state->AllCards[i].access = access;
	printf("card %d acccessibility changed to %d\n", state->AllCards[i].cardnumber, state->AllCards[i].access);
}



void NewCard(SYSTEM_STATE* state)
{
	int NewCardIndex;
	if (state->AmountOfCards == 0)
	{
		state->AllCards = malloc(sizeof(CARD));
		state->AmountOfCards = 1;
		NewCardIndex = 0;
		//state.AntalSpelare[0] är den vi ska lägga in nya data på
	}
	else
	{
		state->AllCards = realloc(state->AllCards,
			sizeof(CARD) * (state->AmountOfCards + 1));
		NewCardIndex = state->AmountOfCards;
		state->AmountOfCards++;
	}

	int cardnumber;
	GetInputInt("Cardnumber: ", &cardnumber);
	state->AllCards[NewCardIndex].cardnumber = cardnumber;

	int access;
	GetInputInt("Access: ", &access);
	state->AllCards[NewCardIndex].access = access;
	state->AllCards[NewCardIndex].CreationDate = time(NULL);
}


void opendoor(SERIALPORT port) 
{
	printf("Sending command to open door.\n");
	SerialWritePort(port, "<OPENDOOR>", strlen("<OPENDOOR>"));
	char buf[512];
	SerialReadPort(port, buf, 512);
}


void ClearCards(SERIALPORT port)
{
	printf("Clearing cards in arduino.\n");
	SerialWritePort(port, "<CLEARCARDS>", strlen("<CLEARCARDS>"));
	char buf[512];
	SerialReadPort(port, buf, 512);
}


void SendCards(SYSTEM_STATE* state, SERIALPORT port)
{
	printf("Sending valid cards through serial.\n");
	for (int i = 0;i < state->AmountOfCards;i++)
	{
		if (state->AllCards[i].access == 1)
		{	
			char command[64] = "<ADDCARD";
			char cardnumber[64];
			sprintf(cardnumber, "%d>", state->AllCards[i].cardnumber);
			strcat(command, cardnumber);

			SerialWritePort(port, command, strlen(command));
			char buf[512];
			SerialReadPort(port, buf, 512);
		}
	}
}

void faketest(SERIALPORT port) {
	char faketest[64] = "<FAKETEST";
	char cardnumber[64];
	GetInput("Card to open with: ", cardnumber, sizeof(cardnumber));
	strcat(faketest, cardnumber);
	strcat(faketest, ">");
	printf("faketest with card %s\n", cardnumber);
	SerialWritePort(port, faketest, strlen(faketest));

	char buf[512];
	SerialReadPort(port, buf, 512);

}

void Huvudmeny(SYSTEM_STATE* state, SERIALPORT port)
{
	int exit = 0;
	while (exit == 0)
	{
		printf("***Main menu***\n");
		printf("1. Remote open door\n2. List all cards in system\n3. Add new card\n4. Add/Remove access\n5. Send valid cards to Arduino\n6. Exit\n9. Fake test scan card\n");
		int selection;
		if (GetInputInt("Selection:>", &selection) == false)
			continue;
		switch (selection)
		{
		case 1:
			opendoor(port);
			break;
		case 2:
			ListCards(state);
			break;
		case 3:
			NewCard(state);
			break;
		case 4:
			ChangeAccess(state);
			break;
		case 5:
			ClearCards(port);
			Sleep(200);
			SendCards(state, port);
			break;
		case 6:
			exit = 1;
			break;
		case 9:
			faketest(port);
			break;
		}

	}

}

int main()
{
	SERIALPORT port = SerialInit("\\\\.\\COM3");
	if (!SerialIsConnected(port))
	{
		return;
	}

	SYSTEM_STATE state;
	state.AllCards = NULL;
	state.AmountOfCards = 0;
	Huvudmeny(&state, port);

	return 0;
}
