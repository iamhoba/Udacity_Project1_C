#include <stdio.h>
#include "app.h"



void appStart(void) {
	uint8_t x,y;
	uint8_t i = 0;
	ST_cardData_t* cardDataPtr, cardData;
	ST_terminalData_t* termDataPtr, termData;
	ST_transaction_t* transDataPtr, transData;
	cardDataPtr = &cardData;
	termDataPtr = &termData;
	transDataPtr = &transData;
	do {
		x = setMaxAmount(termDataPtr);
	} while (x == INVALID_MAX_AMOUNT);
	while (1) {
		while (1) {
			do {
				x = getCardHolderName(cardDataPtr);
			} while (x == WRONG_NAME);
			do {
				x = getCardExpiryDate(cardDataPtr);
			} while (x == WRONG_EXP_DATE);
			do {
				x = getCardPAN(cardDataPtr);
				if (x != WRONG_PAN) {
					y = isValidCardPAN(cardDataPtr);
				}
			} while (x == WRONG_PAN || y == INVALID_CARD);
			getTransactionDate(termDataPtr);
			if (isCardExpired(cardData, termData) == TERMINAL_OK) {
				do {
					x = getTransactionAmount(termDataPtr);
				} while (x == INVALID_AMOUNT);
				if (isBelowMaxAmount(termDataPtr) == TERMINAL_OK) {
					transDataPtr->cardHolderData = cardData;
					transDataPtr->terminalData = termData; 
					transDataPtr->terminalData.transAmount = termDataPtr->transAmount;
					recieveTransactionData(transDataPtr);
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
	}
}