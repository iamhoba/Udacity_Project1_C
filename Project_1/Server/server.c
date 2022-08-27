#include "server.h"
#include <stdio.h>
#include <string.h>

uint8_t m = 0;								//database line counter to know which line has the account data in the accounts database 

EN_serverError_t isValidAccount(ST_cardData_t* cardData) {
	FILE* fp;
	uint32_t x;
	fp = fopen("Accounts_DB.txt", "r");
	uint8_t pan[20] = {0};
	m = 0;
	if (fp == NULL) {
		printf("Accounts data base is not found\n");
		return INTERNAL_SERVER_ERROR;
	}
	while (!feof(fp)) {
		x = fscanf(fp, "%*s\t%*s\t%s", pan);
		if (!strcmp(pan, cardData->primaryAccountNumber))
		{
			//printf("%s\n", pan);
			//printf("Account Found\n");
			fclose(fp);
			return SERVER_OK;
		}
		m = m + 1;
	}
	fclose(fp);
	printf("Account not found\n");
	return ACCOUNT_NOT_FOUND;
}



EN_serverError_t isAmountAvailable(ST_terminalData_t* termData) {
	float a = 0;
	uint8_t i = 0;
	uint32_t x;
	FILE* fp;
	fp = fopen("Accounts_DB.txt", "r");
	if (fp == NULL) {
		printf("Accounts data base is not found\n");
		return INTERNAL_SERVER_ERROR;
	}
	while (!feof(fp)) {
		x = fscanf(fp, "%f\t%*s\t%*s", &a);
		if (m == i)
		{
			if (termData->transAmount <= a)
			{
				printf("Transaction value is ok, Balance = %f\n", a);
				fclose(fp);
				return SERVER_OK;
			}
			else {
				printf("Transaction declined, not enough balance = %f\n", a);
				fclose(fp);
				return LOW_BALANCE;
			}
		}
		i = i + 1;

	}
	fclose(fp);
	printf("Low Balance");
	return LOW_BALANCE;
}


EN_serverError_t isBlockedAccount(void) {
	uint8_t i = 0;
	uint32_t x;
	uint8_t b[10] = { 0 };
	FILE* fp;
	fp = fopen("Accounts_DB.txt", "r");
	if (fp == NULL) {
		printf("Accounts data base is not found\n");
		return INTERNAL_SERVER_ERROR;
	}
	while (!feof(fp)) {
		x = fscanf(fp, "%*s\t%s\t%*s\n", &b);
		if (m == i)
		{

			if (!strcmp(b, "BLOCKED"))
			{
				//printf("Account is blocked\n");
				fclose(fp);
				return DECLINED_STOLEN_CARD;
			}
			else {
				printf("Account running\n");
				fclose(fp);
				return SERVER_OK;
			}
		}
		i = i + 1;
	}
	fclose(fp);
}



EN_transStat_t recieveTransactionData(ST_transaction_t* transData) {
	uint8_t i;
	if (isValidAccount(&(transData->cardHolderData)) == ACCOUNT_NOT_FOUND) {
		printf("Fraud Card\n");
		transData->transState = FRAUD_CARD;
		saveTransaction(transData);
		return  DECLINED_STOLEN_CARD;
	}
	else if (isBlockedAccount() == DECLINED_STOLEN_CARD) {
		printf("Declined Blocked Account");
		transData->transState = DECLINED_STOLEN_CARD;
		saveTransaction(transData);
		return  DECLINED_STOLEN_CARD;
	}
	else if (isAmountAvailable(&(transData->terminalData)) == LOW_BALANCE) {
		printf("Declined insuffecient fund\n");
		transData->transState = DECLINED_INSUFFECIENT_FUND;
		saveTransaction(transData);
		return DECLINED_INSUFFECIENT_FUND;
	}
	else {
		transData->transState = APPROVED;
		i = saveTransaction(transData);
		if (i == SAVING_FAILED) {
			printf("Error saving transaction\n");
			return INTERNAL_SERVER_ERROR;
		}
		else {
			printf("Transaction approved\n");
			return APPROVED;
		}
	}
}


EN_serverError_t saveTransaction(ST_transaction_t* transData) {
	static uint32_t x;
	static uint32_t n;
	uint8_t s;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t c;
	float y = 0;
	float b = 0;
	FILE* fp,*fa,*temp;
	uint8_t trans[30];
	uint8_t rb[10] = { 0 };
	fp = fopen("Transactions_DB.txt", "a");
	if (fp == NULL) {
		printf("Transactions data base is not found\n");
		return INTERNAL_SERVER_ERROR;
	}
	if (n >= 254) {
		fclose(fp);
		return SAVING_FAILED;
	}
	transData->transactionSequenceNumber = n;
	if (transData->transState == DECLINED_STOLEN_CARD)
	{
		strcpy(trans, "Declined Stolen Card");
	}
	else if (transData->transState == DECLINED_INSUFFECIENT_FUND) {
		strcpy(trans, "Declined insuffecient fund");
	}
	else if (transData->transState == FRAUD_CARD) {
		strcpy(trans, "Declined fraud card\n");
	}
	else {
		strcpy(trans, "Approved");
	}
	fprintf(fp, "%d\t%s\t%s\t%s\n", transData->transactionSequenceNumber,transData->cardHolderData.cardHolderName,transData->cardHolderData.primaryAccountNumber,trans);
	fclose(fp);
	n = n + 1;
	if (transData->transState == APPROVED) {
		fa = fopen("Accounts_DB.txt", "r");
		if (fa == NULL) {
			printf("Accounts data base is not found\n");
			return INTERNAL_SERVER_ERROR;
		}
		while (!feof(fa)) {
			x = fscanf(fa, "%f\t%s\t%*s\n", &b, &rb);
			if (m == i)
			{
				y = b - transData->terminalData.transAmount;
				break;
			}
			i = i + 1;
		}
		rewind(fa);
		temp = fopen("replica.txt", "w");
		c = getc(fa);
		while (!feof(fa)) {
			if (c == '\n') {
				j++;
			}
			if (j != m) {
				putc(c, temp);
			}
			else {
				while ((c = getc(fa)) != '\n') {}
				putc('\n', temp);
				fprintf(temp, "%.2f\t%s\t%s", y, rb, transData->cardHolderData.primaryAccountNumber);
				fputs("\n", temp);
				j++;
			}
			c = getc(fa);
		}
		fclose(fa);
		fclose(temp);
		remove("Accounts_DB.txt");
		s = rename("replica.txt", "Accounts_DB.txt");
		return APPROVED;
	}
	return SAVING_FAILED;
}