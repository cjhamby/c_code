/*
 * Program 2
 * poly.c
 *
 * Christopher Hamby
 * 13-Oct-2017
 *
 * This was done for a class assignment
 * My code begins after the main loop
 *
 * This program does some fun operations involving polynomials
 * ideally the polynomials could be any degree
 * but in this program the max degree is 4
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* functions to be implemented */
int readPoly(int coeff[], int degree);
void printPoly(int coeff[], int degree);
double evalPoly(int coeff[], int degree, double x);
double evalPolyDouble(double coeff[], int degree, double x);
double integratePoly(int coeff[], int degree, double low, double high);
int multPoly(int c1[], int d1, int c2[], int d2, int c3[], int d3);
int genPoly(int coeff[], int degree, int roots[], int nroots);

#ifndef NOMAIN

int main() {
    int f4[5] = {0};  /* f(x) - degree = 4 or less */
    int g4[5] = {0};  /* g(x) - degree = 4 or less */
    int p8[9] = {0};  /* p(x) - degree = 8 or less */
    int roots[4];     /* roots for generating */
    int degree = 0;   /* degree of generated poly */
    int status = 1;
    double x = 0;     /* point for evaluation */
    double low = 0, high = 0;  /* limits for integration */
    char command;     /* user command character */
    int done = 0;     /* set to 1 to quit */
    int i;

    /*
	 * remove buffering on stdout
	 * anything written to stdout is immediately sent to the console
	 * this removes the weird delay that printf functions have
	 */
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("hello\n");

    while (!done) {
        printf("\nEnter a command (e,i,g,m,q): ");
        do {
            command = getchar();
        } while (command == '\n');
        while (getchar() != '\n');

        switch (command) {

            /* e: evaluate a polynomial */
            case 'e':
                printf("EVALUATE\n");
                printf("Enter a polynomial f(x):\n");
                status = readPoly(f4 ,4);
                if (!status) {
                    printf("Enter x: ");
                    scanf("%lf", &x);
                    printf("f(%g) = %g\n", x, evalPoly(f4, 4, x));
                }
                else {
                    printf("ERROR: Could not read polynomial.\n");
                }
                break;

            /* integrate a polynomial */
            case 'i':
                printf("INTEGRATE\n");
                printf("Enter a polynomial f(x):\n");
                status = readPoly(f4, 4);
                if (!status) {
                    printf("Enter lower limit: ");
                    scanf("%lg", &low);
                    printf("Enter upper limit: ");
                    scanf("%lg", &high);
                    printf("Integral of f(x) = %g\n", integratePoly(f4, 4, low, high));
                }
                else {
                    printf("ERROR: Could not read polynomial.\n");
                }
                break;

            /* generate a polynomial */
            case 'g':
                printf("GENERATE\n");
                printf("Enter the degree (<= 4): ");
                scanf("%d", &degree);
                for (i=0; i<=4; i++) roots[i] = 0;
                if ((degree <= 4) && (degree > 0)) {
                    for (i=0; i<degree; i++) {
                        printf("Integer Root %d = ", i+1);
                        scanf("%d", &roots[i]);
                    }
                    status = genPoly(f4, degree, roots, degree);
                    if (!status) {
                        printf("f(x) = ");
                        printPoly(f4, degree);
                    }
                    else {
                        printf("ERROR: could not generate polynomial");
                    }
                    printf("\n");
                }
                else {
                    printf("ERROR: Degree must be between 1 and 4.\n");
                }
                break;

            /* multiply two polynomials */
            case 'm':
                printf("MULTIPLY\n");
                printf("Enter first polynomial f(x):\n");
                status = readPoly(f4,4);
                if (!status) {
                    printf("Enter second polynomial g(x):\n");
                    status = readPoly(g4,4);
                    if (!status) {
                        printf("f(x) * g(x) = ");
                        status = multPoly(f4, 4, g4, 4, p8, 8);
                        if (status) printf("ERROR");
                        else printPoly(p8, 8);
                        printf("\n");
                    }
                    else {
                        printf("ERROR: Could not read polynomial.\n");
                    }
                }
                else {
                    printf("ERROR: Could not read polynomial.\n");
                }
                break;

            /* quit */
            case 'q':
                done = 1;
                break;

        }
    }
    return (EXIT_SUCCESS);
}

#endif  /*NOMAIN*/

/*---- DO NOT CHANGE ANYTHING ABOVE THIS LINE ----*/





/*---- BEGINNING OF MY CODE ------*/
/*
 * this function takes a line of input and parses for polynomial info
 * the input will be of the form:
 *   ax^2 + bx + c
 *
 * with one space before/after each operator
 * and no spaces after the last term
 *
 * COEFF   where the polynomial coefficients will go
 * DEGREE  max degree that the polynomial can have
 */
int readPoly(int coeff[], int degree) {
	int sign=1;
	int done=0;
	char temp;
	int i;

	/* remove leading spaces               */
	do     {  temp=getchar(); }
	while  (  temp == ' '     );

	/* put the first non-space char back   */
	ungetc(temp, stdin);

	/* initialize the array                */
	for(i=0; i<=degree; i++)	coeff[i]=0;


	/* ------------ term parser loop --------------------------------
	 *
	 * a term can be in one of the following forms:
	 * (-)AX^B
	 * (-)AX
	 * (-)A
	 * (-)X^B
	 * (-)X
	 *
	 * A is the coefficient
	 * B is the degree
	 *
	 * check for the necessary parts of a term, in order
	 * (negative sign) >> (A) >> "x" >> ^(B) >> ' ' >> (operator)
	 *
	 * -------------------------------------------------------------- */
	while(!done)
	{
		int myCoeff=0;
		int myDegree=0;
		int hasX=0;
		int hasCoeff=0;
		int hasDegree=0;

		/* NEGATIVE SIGN CHECK
		 * if the first term is not a negative sign, it's a number-- put it back */
		temp = getchar();
		if(temp=='-')   sign *= -1;
		else		    ungetc(temp, stdin);

        /* (A) CHECK */
		hasCoeff = scanf("%d", &myCoeff);

		/* 'x' CHECK */
		/* if the term isn't an 'x', it could be a space-- put it back */
		temp = getchar();
		if(temp =='x')  hasX =1;
		else 	   	    ungetc(temp, stdin);

		/* if the term has no coeff and no x, that's illegal  */
		if(!hasCoeff && !hasX)				  return 1;

		/* an "x" with no coeff is assigned a coeff of 1 */
		if(hasX && !hasCoeff)  myCoeff = 1;

		/* (B) CHECK */
		hasDegree = scanf("^%d",&myDegree);

		/* if the term has a degree and a coefficient, but no x, that's illegal */
		if(hasCoeff && hasDegree && !hasX)    return 1;

		/* indicates a degree too large for the coefficient array */
		if(myDegree > degree)	              return 1;

		/* terms with an 'x' and no degree are assumed to be degree=1 */
		if(hasX && !hasDegree)  myDegree=1;

		/* terms with no "x" might still be legal */
		if(!hasX && hasCoeff)	myDegree=0;

		/* if the degree already has a coefficient, that's illegal */
		if(coeff[myDegree] != 0)              return 1;

		/* store the coefficient in the provided COEFF array */
		/* multiply by sign to apply the operator            */
		coeff[myDegree] = sign*myCoeff;

		/* END OF PARSE CHECK */
		/* a newline char will break the loop */
		temp = getchar();
		if(temp =='\n')  		              break;


		/* OPERATOR CHECK
		 * the operator modifies the sign of the next term
		 * needs to be a space between term and operator */
		if(temp != ' ')				          return 1;

		temp=getchar();
		if     (temp=='+')   sign =  1;
		else if(temp=='-')   sign = -1;

		/* if something else is in the operator spot, that's illegal */
		else								 return 1;

	}

	return 0; /* successful read */
}





/*
 * 	given the coefficients and degree of a polynomial,
 * 	this function prints the properly formatted polynomial
 *
 * 	COEFF:  coefficients of the polynomial
 * 	DEGREE: max degree of the polynomial
 */
void printPoly(int coeff[], int degree) {
    int n = degree;

    /* print each term in the polynomial
     * the first iteration of the loop will print:
     * <sign> <term>
     * successive iterations of the loop will print:
     * <operator> <term>
     */
    while(n>=0) {
    	int c = 0;
    	int sign = 0;

    	/* find a term with a non-zero coefficient
    	 * n is only decremented if the first "while" condition is true */
    	do{
    		c = abs(coeff[n]);
    		sign = coeff[n];
    	}
    	while(c==0 && --n>=0);

    	/* if c==0 after the loop, that means there are no more coefficients */
    	if(c==0)             return;

    	/* the first term gets a negative sign instead of a " - " operator */
    	if(n==degree){
    		if(sign < 0)	 printf("-");
    	}
    	/* only print the operator after the first number term */
    	else {
    	    if(sign > 0)     printf(" + ");
    	    if(sign < 0)     printf(" - ");
    	}

    	if(c>1)              printf("%d", c);    /* print coeff, if >1             */
    	if(n>0)              printf("x");        /* print letter 'x' if n>0        */
    	else if(c==1)		 printf("%d", c);	 /* special case for term == 1     */
    	if(n>1)	             printf("^%d",n);    /* print the degree if >1         */

    	n--;
    }
}




/* 	this function plugs x into the polynomial
   	and returns the evaluated value */
double evalPoly(int *coeff, int degree, double x) {
    double sum =0;
    int i=degree;
    for(i=degree; i>=0; i--)
    	sum+=(coeff[i]*pow(x, (double)i));
    return sum;
}


/*	used in integratePoly, where the integrated coefficients are likely to be doubles
	saving myself from a lot of type casting :-)  */
double evalPolyDouble(double *coeff, int degree, double x) {
	double sum =0;
    int i=degree;
    for(i=degree; i>=0; i--)
    	sum+=(coeff[i]*pow(x, (double)i));
    return sum;
}




/*
 * this function calculates the indefinite integral of the polynomial
 * from the low bound to the high bound
 *
 * INPUTS
 * COEFF   array that holds the coefficients
 * DEGREE  degree of the polynomial
 * LOW	   integration bound
 * HIGH    integration bound
 *
 * OUTPUTS
 * <none>
 *
*/
double integratePoly(int *coeff, int degree, double low, double high) {
	/* stores the coefficients of the integrated polynomial */
	double * newC = (double *) malloc((degree+2) * sizeof(double));
	double result = 0;
	int i;

	/* in true integration fashion, we just shift some coefficients around
	 * calculus is easy                                                        */
	for(i=0; i<=degree; i++)
		newC[i+1] = (double)(coeff[i])/(i+1);

	/* evaluate the integrated polynomial at each bound, return the difference */
	result = evalPolyDouble(newC, degree+1, high) - evalPolyDouble(newC, degree+1, low);

	/* the allocated memory is no longer needed */
	free(newC);
    return result;
}




/*
 * multiply two polynomials c1 and c2 together
 * store the result into c3
 *
 * INPUTS
 * C1, C2	coefficient arrays
 * D1, D2   degree of each array
 *
 * OUTPUTS
 * C3		output coeff. array
 * D3		degree of array
 */
int multPoly(int c1[], int d1, int c2[], int d2, int c3[], int d3) {
    int i;	/*corresponds to c1*/
    int j;	/*corresponds to c2*/
    int k;	/*corresponds to c3, the resultant polynomial*/

    for(k=0; k<=d3; k++) 	c3[k]=0;	/*initialize the output array*/

    /* for each combination, add the degrees and multiply the coefficients
     * get the product >>     C1[i] * C2[j]
     * add the product >>     C3[x] += PRODUCT
     * where           >>     x = i + j
    */
    for(i=0; i<=d1; i++)
    	for(j=0; j<=d2; j++)
    		c3[i+j]+=(c1[i] *c2[j]);

    return 0; /* success */
}




/*
 *
 * OUTPUTS:
 * C : Generated Polynomial
 *
 * INPUTS:
 * D : Degree of polynomial
 * R : Roots of the polynomial
 * N : Number of roots (== D)
 *
 * given roots, i.e. (x-r1)(x-r2)...(x-rn), generate a polynomial
	multiplies myPoly by (x-r[i]) as long as i<=n
	note that calling this function twice in a row causes an error with the newC declaration */

int genPoly(int c[], int d, int r[], int n) {
	int i;
	int j;
	int t1[2] = {-r[0], 1};	    /* t1 represents a 1-degree polynomial formed by each root, i.e. <X - R> */

	/* create an array with the same number of elements as c
	 * this array will hold the generated coefficients          */
	int * myPoly = (int *) malloc(n * sizeof(int));

	/* initialize the arrays */
	for(i=0; i<=n; i++) {
		c[i]=0;
		myPoly[i]=0;
	}

	/* only one root >> poly = (x-root) */
	if(n==1) {
		c[1] = t1[1];
		c[0] = t1[0];
		return 0;
	}

	/* myPoly initially equals (x-r1)
	 * not r0, since that's in t1      */
	myPoly[1] = 1;
	myPoly[0] = -r[1];

	/* starts at 2, because the first 2 roots have been assigned already */
	for(i =2; i<=d; i++) {
		/* each time MULT_POLY is called:
		 * C = t1 * MY_POLY
		 * C goes up 1 degree
		 *
		 * why use MY_POLY as an intermediate?
		 * why not C = t1 * C ?
		 * - because MULT_POLY empties the output array before using it
		 */
		multPoly(myPoly, d, t1, 1, c, d);

		/* put the next root inside the 1d polynomial */
		t1[0] = -r[i];

		/* copy C into MY_POLY before the next multiplication begins */
		for(j=0; j<=d; j++) 	myPoly[j] = c[j];

	}

    return 0;
}
