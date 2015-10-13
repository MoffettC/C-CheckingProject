#include <stdio.h>

void outputHeaders(void);

void initialBalance(double amount, double *balance, double *service, double *openBalance, double *previousBalance,
	double *adjustment, int month, int *adjustmentSign);

void deposit(double amount, double *balance, double *service, int *numDeposit, double *amtDeposit);

void check(double amount, double *balance, double *service, int *numCheck, double *amtCheck);

void outputSummary(int numDeposit, double amtDeposit, int numCheck, double amtCheck, double openBalance, double service, double closeBalance,
																	int month, double previousBalance, double adjustment, int adjustmentSign);

int main(void) {
	char code;
	double amount, nextAmount, service, balance, previousBalance, adjustment;
	double amtCheck, amtDeposit, openBalance, closeBalance;
	int numCheck, numDeposit;
	int month, adjustmentSign, isNextMonth;

	FILE *fp;

	if (!(fp = fopen("account.txt", "r"))) {
		printf("account.txt could not be opened for input.");
		getchar(); //debug added so it actually gives user time to read in MVS
		exit(1);
	}

	month = 1;
	previousBalance = 0.0;
	isNextMonth = 0;
	adjustment = 0.0;
	amount = 0.0;
	nextAmount = 0.0;

	while (!feof(fp)) { //monthly loop


	adjustmentSign = 0;
	service = 0.0;
	balance = 0.0;
	amtCheck = 0.0;
	amtDeposit = 0.0;
	openBalance = 0.0;
	closeBalance = 0.0;
	numCheck = 0;
	numDeposit = 0;


	outputHeaders();
	if (isNextMonth == 1) {
		amount = nextAmount;
		initialBalance(amount, &balance, &service, &openBalance, &previousBalance, &adjustment, month, &adjustmentSign);
	}

	while (!feof(fp)) { //within month loop
			fscanf(fp, "%c %lf\n", &code, &amount);
			if (code == 'I') {
				if (isNextMonth == 0) {
					initialBalance(amount, &balance, &service, &openBalance, &previousBalance, &adjustment, month, &adjustmentSign);
					isNextMonth = 1;
				}
				else {
					nextAmount = amount;
					break;
				}
			}
		

			if (code == 'D') {
				deposit(amount, &balance, &service, &numDeposit, &amtDeposit);
			}
			if (code == 'C') {
				check(amount, &balance, &service, &numCheck, &amtCheck);
			}

		}

		closeBalance = balance - service;
		outputSummary(numDeposit, amtDeposit, numCheck, amtCheck, openBalance, service, closeBalance, month, 
																previousBalance, adjustment, adjustmentSign);

		previousBalance = closeBalance;
		month += 1;
		adjustmentSign = 0; //RESET SIGN INDICATOR
	}

	getchar(); //debug added so it actually gives user time to read in MVS
	fclose(fp);
	return 0;
}

void outputHeaders(void) {
	printf("Transaction %7c Deposit %7c Check %7c Balance\n", ' ', ' ', ' ');
	printf("----------- %7c ------- %7c ----- %7c -------\n", ' ', ' ', ' ');
}

void initialBalance(double amount, double *balance, double *service, double *openBalance, double *previousBalance, 
																double *adjustment, int month, int *adjustmentSign) {

	*openBalance = amount;
	*balance = amount;

	if ((*previousBalance != *openBalance) && (month != 1)) {
		if (*openBalance > *previousBalance) {
			*adjustment = *openBalance - *previousBalance;
			printf("Adjustment %7c %8.2lf %20c %8.2lf\n", ' ', *adjustment, ' ', *balance);
		}
		if (*previousBalance > *openBalance) { //NEGATIVE ADJUSTMENT
			*adjustment = *previousBalance - *openBalance;
			*adjustmentSign += 1;
			printf("Adjustment %20c %8.2lf- %6c %8.2lf\n", ' ', *adjustment, ' ', *balance);
		}
	}
	*service += 3.00;
	printf("Initial Balance %32c %8.2lf\n", ' ', *balance);
}

void deposit(double amount, double *balance, double *service, int *numDeposit, double *amtDeposit) {
	*service += 0.03;
	*amtDeposit += amount;
	*balance += amount;
	*numDeposit += 1;
	printf("Deposit %10c %8.2lf %20c %8.2lf\n", ' ', amount, ' ', *balance);

}

void check(double amount, double *balance, double *service, int *numCheck, double *amtCheck){
	*service += 0.06;
	*amtCheck += amount;
	*balance -= amount;
	*numCheck += 1;
	printf("Check %26c %8.2lf %6c %8.2lf\n", ' ', amount, ' ', *balance);
	if (*balance < 0.00){
		*service += 5.00;
	}

}

void outputSummary(int numDeposit, double amtDeposit, int numCheck, double amtCheck, double openBalance, double service,
								double closeBalance, int month, double previousBalance, double adjustment, int adjustmentSign) {

	printf("\n\n\n %17c Month: %11d\n\n", ' ', month);

	printf("Total number of deposits: %8c %2d\n", ' ', numDeposit);
	printf("Total amount of deposits: %2c %8.2lf\n\n", ' ', amtDeposit);

	printf("%1c Total number of checks: %8c %2d\n", ' ', ' ', numCheck);
	printf("%1c Total amount of checks: %2c %8.2lf\n\n", ' ', ' ', amtCheck);

	printf("%3c Total service charge: %2c %8.2lf\n\n", ' ', ' ', service);

	if (month > 1) {
		printf("Previous Closing Balance: %2c %8.2lf\n", ' ', previousBalance);
		if (adjustmentSign == 0) {
			
			printf("%2c Accounting adjustment: %11.2lf\n\n", ' ', adjustment);
		} 
		else {
			printf("%2c Accounting adjustment: %11.2lf-\n\n", ' ', adjustment);
		}
	}

	printf("%8c Opening Balance: %2c %8.2lf\n", ' ', ' ', openBalance);
	printf("%8c Closing Balance: %2c %8.2lf\n\n\n\n", ' ', ' ', closeBalance);
}

