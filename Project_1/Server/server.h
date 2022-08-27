/* server.h */
#ifndef SERVER_H
#define SERVER_H

#include "../Card/card.h"
#include "../Terminal/terminal.h"


typedef enum EN_transState_t
{
	APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, INTERNAL_SERVER_ERROR
}EN_transStat_t;


typedef unsigned int uint32_t;



typedef struct ST_transaction_t
{
	ST_cardData_t cardHolderData;
	ST_terminalData_t terminalData;
	EN_transStat_t transState;
	uint32_t transactionSequenceNumber;
}ST_transaction_t;


typedef struct ST_accountsDB_t
{
	float balance;
	uint8_t primaryAccountNumber[20];
}ST_accountsDB_t;


typedef enum EN_serverError_t
{
	SERVER_OK, SAVING_FAILED, TRANSACTION_NOT_FOUND, ACCOUNT_NOT_FOUND, LOW_BALANCE, FRAUD_CARD
}EN_serverError_t;


EN_transStat_t recieveTransactionData(ST_transaction_t* transData);
EN_serverError_t isValidAccount(ST_cardData_t* cardData);
EN_serverError_t isBlockedAccount(void);
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData);
EN_serverError_t saveTransaction(ST_transaction_t* transData);
EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t* transData);

#endif 
/* End of server.h*/
