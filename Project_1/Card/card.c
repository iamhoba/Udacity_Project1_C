#include "card.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



EN_cardError_t getCardHolderName(ST_cardData_t* cardData) {
	uint8_t name[50] = { 0 };								//character more than the required name to check overflow
	printf("Enter your name\n");							//ask the user to enter the name
	gets(name);												//scan the pan input from user
	//printf("Length is %zu\n", strlen(name));				//for testing
	//printf("Name is %s", name);							//for testing
	if (strlen(name) >= 20 && strlen(name) < 25) {			//check if name length is within limit	
		strcpy(cardData->cardHolderName, name);				//copy name to data base
		//printf("ok\n");									//for testing
		return CARD_OK;
	}
	else {
		printf("Wrong Entry, Name shall be between 20 & 24 character including spaces\n");
		return WRONG_NAME;
	}
}


EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData) {

	uint8_t date[50] = { 0 };												//character more than the limit to check overflow
	uint8_t i = 0;															//counter
	printf("Enter card expiry date in the format MM/YY\n");					//ask the user for the expiry date
	gets(date);																//scan the expiry date input from user
	if (strlen(date) == 5) {												//check if date is only 5 characters
		for (i = 0; i <= 4; i++) {
			if (i == 2) {
				if (date[i] != '/') {										//check if the third array is / to match the required format
					printf("wrong date format\n");
					return WRONG_EXP_DATE;
				}
			}
			else {
				if (date[i] < ZERO_ASCII || date[i] > (ZERO_ASCII + 9)) {	//check if the month and day digits are between 0 and 9
					printf("wrong date format\n");
					return WRONG_EXP_DATE;
				}
			}
		}
	}
	else {
		printf("Wrong date format\n");
		return WRONG_EXP_DATE;
	}
	strcpy(cardData->cardExpirationDate, date);								//copy date to the data base
	//printf("OK\n");														//for testing
	return CARD_OK;
}


EN_cardError_t getCardPAN(ST_cardData_t* cardData) {
	uint8_t pan[50] = { 0 };													//char more than the pan limit to check overflow
	uint8_t i;																	//counter
	printf("Enter the Primary Account Number\n");								//ask the user for the pan
	gets(pan);																	//scan the pan input from user
	//printf("%zu\n", strlen(pan));												//for testing
	if (strlen(pan) >= 16 && strlen(pan) < 20) {								//check if pan length is correct
		for (i = 0; i < strlen(pan); i++) {								
			if (pan[i] < ZERO_ASCII || pan[i] > (ZERO_ASCII + 9)) {				//check if pan is only numbers
				printf("wrong pan\n");
				return WRONG_PAN;
			}
		}
		strcpy(cardData->primaryAccountNumber, pan);							//copy PAN to the data base
		//printf("ok\n");														//for testing
		return CARD_OK;
	}
	else {
		printf("Wrong Account number, PAN should be 16 to 19 digits\n");																
		return WRONG_PAN;
	}
}