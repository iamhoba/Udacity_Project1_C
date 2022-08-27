#include "terminal.h"
#include <stdio.h>
#include <string.h>
#include <time.h>




EN_terminalError_t getTransactionDate(ST_terminalData_t* termData) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	termData->transactionDate[2] = '/';													//next lines are to change the date to string and store it in the terminal data with the required format MM/DD/YYYY
	termData->transactionDate[5] = '/';
	termData->transactionDate[1] = (tm.tm_mon + 1) % 10 + ZERO_ASCII;
	termData->transactionDate[0] = ((tm.tm_mon + 1) / 10) % 10 + ZERO_ASCII;
	termData->transactionDate[4] = tm.tm_mday % 10 + ZERO_ASCII;
	termData->transactionDate[3] = (tm.tm_mday / 10) % 10 + ZERO_ASCII;
	termData->transactionDate[9] = (tm.tm_year + 1900) % 10 + ZERO_ASCII;
	termData->transactionDate[8] = ((tm.tm_year + 1900) / 10) % 10 + ZERO_ASCII;
	termData->transactionDate[7] = ((tm.tm_year + 1900) / 100) % 10 + ZERO_ASCII;
	termData->transactionDate[6] = ((tm.tm_year + 1900) / 1000) % 10 + ZERO_ASCII;
	termData->transactionDate[10] = '\0';

	//printf("%s\n", termData->transactionDate);											//for testing
	return TERMINAL_OK;
}


EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData) {
	uint8_t exp_date_y;
	uint8_t curr_date_y;
	uint8_t exp_date_m;
	uint8_t curr_date_m;
	exp_date_y = ((cardData.cardExpirationDate[3] - ZERO_ASCII) * 10)+ (cardData.cardExpirationDate[4] - ZERO_ASCII);					//convert expiry date year to integer
	curr_date_y = ((termData.transactionDate[8] - ZERO_ASCII) * 10) + (termData.transactionDate[9] - ZERO_ASCII);						//convert current date year to integer
	exp_date_m = ((cardData.cardExpirationDate[0] - ZERO_ASCII) * 10) + (cardData.cardExpirationDate[1] - ZERO_ASCII);					//convert expiry date month to integer
	curr_date_m = ((termData.transactionDate[0] - ZERO_ASCII) * 10) + (termData.transactionDate[1] - ZERO_ASCII);						//convert current date month to integer
	if (exp_date_y > curr_date_y) {
		//printf("ok\n");																													//for testing
		return TERMINAL_OK;
	}
	else if (exp_date_y == curr_date_y) {
		if (exp_date_m >= curr_date_m) {
			//printf("ok\n");																												//for testing
			return TERMINAL_OK;
		}
		else {
			printf("Expired card\n");
			return EXPIRED_CARD;
		}
	}
	else {
		printf("Expired card\n");
		return EXPIRED_CARD;
	}
}


EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData) {
	int luhn = 0;
	int x = 0;
	uint8_t i;																			//counter
	for (i = 0; i < strlen(cardData->primaryAccountNumber); i = i + 2) {				//apply the luhn algorithm
		if (((cardData->primaryAccountNumber[i] - ZERO_ASCII) * 2) > 9) {
			x = (cardData->primaryAccountNumber[i] - ZERO_ASCII) * 2;
			luhn = luhn + (x % 10) + ((x / 10) % 10);
			//printf("%d * 2 = ", cardData->primaryAccountNumber[i] - ZERO_ASCII);
			//printf("%d\n", (x % 10) + ((x / 10) % 10));
		}
		else {
			luhn = luhn + (cardData->primaryAccountNumber[i] - ZERO_ASCII) * 2;			//multiply numbers with odd order by 2 and add to luhn
			//printf("%d * 2 = ", cardData->primaryAccountNumber[i] - ZERO_ASCII);
			//printf("%d\n", (cardData->primaryAccountNumber[i] - ZERO_ASCII) * 2);
		}
	}
	for (i = 1; i < strlen(cardData->primaryAccountNumber); i=i+2) {
		luhn = luhn + (cardData->primaryAccountNumber[i] - ZERO_ASCII);				//multiply numbers with even order by 1 and add to luhn
		//printf("%d * 1 = ", cardData->primaryAccountNumber[i] - ZERO_ASCII);
		//printf("%d\n", (cardData->primaryAccountNumber[i] - ZERO_ASCII));
	}
	//printf("%d\n", luhn);															//for testing
	if (luhn % 10 == 0) {
		//printf("ok\n");															//for testing
		return CARD_OK;
	}
	else
	{
		printf("Invalid card\n");														
		return INVALID_CARD;
	}
}


EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData) {
	float a;
	uint8_t x;
	printf("Enter the transaction amount\n");
	scanf_s("%f", &a);														//scan input from user
	x = getchar();
	if (a <= 0) {
		printf("Invalid Entry, Amount should be greater zero\n");
		return INVALID_AMOUNT;
	}
	else {
		termData->transAmount = a;											//copy value to terminal data
		//printf("Ok\n");														//for testing
		return TERMINAL_OK;
	}

}


EN_terminalError_t setMaxAmount(ST_terminalData_t* termData) {
	float n;
	uint8_t x;
	printf("Enter the max allowed transaction amount\n");
	scanf_s("%f", &n);													//scan input from user
	x = getchar();
	if (n <= 0) {
		printf("Invalid Entry, Amount should be greater zero\n");
		return INVALID_MAX_AMOUNT;
	}
	else {
		termData->maxTransAmount = n;										//copy value to terminal data
		//printf("ok\n");													//for testing
		return TERMINAL_OK;
	}
}


EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData) {
	if (termData->transAmount > termData->maxTransAmount) {
		printf("Max amount (%f) exceeded\n", termData->maxTransAmount);
		return EXCEED_MAX_AMOUNT;
	}
	else {
		//printf("ok\n");													//for testing
		return TERMINAL_OK;
	}
}