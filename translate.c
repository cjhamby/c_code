/*
 * translate.c
 *
 * Created on: Jun 29, 2020
 * Author: Chris Hamby
 *
 * Demonstrates basic input processing with strings
 * Converts human language to guinea pig speak, and vice-versa
 *
 */


#include <stdio.h>
#include <stdlib.h>

#define NEXT	  (1)
#define PREVIOUS  (-1)
#define EMPTY     (0)
#define BEGINNING (0)
#define PROG_RUN  (1)
#define PROG_END  (0)
#define BUF_SIZE  (50)

int g_translate();
int h_translate();
void word_replace(char*, char*, int, char*, char);




//--------------------------------------------------------------------------------------
int main(void)
{
	int prog_running = PROG_RUN;	// state of the program
	char in_char;					// an input holder

	/*
	 * remove buffering on stdout
	 * anything written to stdout is immediately sent to the console
	 * this removes the weird delay that printf functions have
	*/
	setvbuf(stdout, NULL, _IONBF, EMPTY);

	// print the start message
	printf("--- Guinea Pig <-> Human Translator ---\nPress (q) to quit\n\n");


	/*
	 * this is the program that runs
	*/
    while(prog_running)
    {
    	// get an input character from the user
    	// hit "enter" to submit the character
    	// note: only the first char is considered
    	printf("pick a starting language (g,h): ");
    	in_char = getchar();
    	while(getchar() != '\n'); // clear the rest of the feed

    	// decide what to do based on the input character
    	switch(in_char)
    	{
    	case 'g':		// translate guinea pig -> human
    		printf("\n---Guinea Pig to Human Translator---\n");
    		g_translate();
    		break;
    	case 'h':		// translate human -> guinea pig
    		printf("\n---Human to Guinea Pig Translator---\n");
    		h_translate();
    		break;
    	case 'q':		// exit the program
    		prog_running = PROG_END;
    		break;
    	default:		// take another input character
    		printf("invalid char, try again: ");
    		break;
    	}
    }

	return EXIT_SUCCESS;
}



/*--------------------------------------------------------------------------------------
 * Do you have something important to tell your Guinea Pig?
 * But you just don't know how?
 * Don't let this happen to you!
 * Use this handy translator to get your message across
 *
 * demonstrates FGETS method of obtaining string
*/
int h_translate() {
	// input string container/buffer
	char buf[BUF_SIZE];

	// this is what each word will be replaced with-- variable-length wheeking
	char new_word[BUF_SIZE] = "weeEeeeEeeEeEeeeeeEeeeeEeeEeeeEeeeeeeEeeeeeEeEEee";
	char end_char = 'k';

	// input : get a phrase
	printf("Enter your message here: ");
	fgets(buf, BUF_SIZE, stdin);

	/*
	 * processing:
	 * send data to the WORD_REPLACE function
	 * BUF is used as both the PHRASE and the DEST (see WORD_REPLACE for details)
	 * this just means that we re-use the BUF container, but the old phrase isn't stored
	 *
	*/
	word_replace(buf, buf, BUF_SIZE, new_word, end_char);

	// output : return the translated phrase
	printf("\nTranslated Message:      %s\n", buf);

	// wait for input to continue the program
	printf("press enter to continue...");
	while( getchar() != '\n' );
	return 1;
}





/*--------------------------------------------------------------------------------------
 * Guinea Pigs are notoriously hungry all the time
 * this function demonstrates that
 *
 * it accepts a phrase that the guinea pig said (i.e. wheeeeek)
 * and returns the human-understandable translation
 *
 * demonstrates GETCHAR method of obtaining string
 *
*/
int g_translate() {
	char c;						// the character read from input
	char buf[BUF_SIZE];         // input string container/buffer
	int loc = BEGINNING;	    // index to write at in BUF


	// this is what each word will be replaced with - "food" with a variable amount of 'o's
	char new_word[BUF_SIZE] = "FOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";
	char end_char = 'D';

	/*
	 * input using getchar
	 * each char is added to the BUF container
	 *
	 */
	printf("What did your Guinea Pig say? ");

	do{
		c = getchar();	// get the input char by char
		buf[loc++] = c; // add the char to the buffer
	}
	while( loc < (BUF_SIZE + PREVIOUS) && (c != '\n') );	// until buffer is full or no more input

	buf[loc] = '\0';  // add an "end of string" char (needed for later printf call)


	/*
	 * processing:
	 * send data to the WORD_REPLACE function
	 * BUF is used as both the PHRASE and the DEST (see WORD_REPLACE for details)
	 * this just means that we re-use the BUF container, but the old phrase isn't stored
	 *
	*/
	word_replace(buf, buf, BUF_SIZE, new_word, end_char);

	// output : return the translated phrase
	printf("\nTranslated Message:           %s\n", buf);

	// wait for input to continue the program
	printf("press enter to continue...");
	while( getchar() != '\n' );
	return 1;
}




/*--------------------------------------------------------------------------------------
 * replace all words in the phrase with the new word, with LAST_LETTER at the end
 * this is pretty specific to this program's needs
 *
 * the original phrase is not modified by this function
 * the destination is provided by whatever calls this function
 * this is done to avoid returning a local var address
 *
*/
void word_replace(char* dest, char* phrase, int p_length, char* new_word, char last_letter){
	int i, j = BEGINNING;		// index vars
	int word_begin = BEGINNING; // indicates the beginning of a word in the phrase

	/*
	 * go through the buffer character-by-character
	 * if a space is encountered, that is interpreted as the end of a word
	 * the preceding word is then "translated"
	*/
	for(i=BEGINNING; i<BUF_SIZE; i++){
		char c = phrase[i];							// c: the char being looked at
		// look for spaces
		if(c == ' ' || c == '\n' || c == '\0') {    // i: index of the space or newline
			int new_word_i = BEGINNING;		        // used when copying NEW_WORD
			int word_length = i-word_begin;         // how long the word we're replacing is

			// replace the phrase words with the new word
			for(j=word_begin; j<i; j++)				// j: index of character to replace (in word)
				dest[j]=new_word[new_word_i++];     // copy letter from new_word to dest

			if(word_length>1)                       // single-letter words don't get the last letter
				dest[i+PREVIOUS] = last_letter;     // put the last letter on the end, before the space

			word_begin = i + NEXT;			        // the next word starts after the space
		}

		// end of phrase reached
		if(phrase[i] == '\n' || phrase[i] == '\0') break;
	}
}

