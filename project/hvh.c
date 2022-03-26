/*
 *  five_card_hands.c: Enumerate and tabulate five-card hands
 *
 *  Copyright (C) 1993-99 Clifford T. Matthews, Brian Goetz
 *
 *  This package is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 *
 *  This package is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this package; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<signal.h>

#include	"poker_defs.h"
#if defined(_MSC_VER) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#include	"getopt_w32.h"
#else
#include        <getopt.h>
#endif
#include	"inlines/eval.h"
#define SIZE 1328

 /* #define VERBOSE */

uint32 totals[HandType_LAST + 1];
int num_dead;
CardMask dead_cards;

const char* opts = "d:";
CardMask gDeadCards, gPlayerCards[2];

/*
 * returns number of dead cards, -1 on error
 *
 * argc, argv are the standard arguments to main
 * num_dead is a return value of the number of dead cards
 * dead_cards is a return value of the dead cards
 */
static int
parse_args(int argc, char** argv, int* num_dead, CardMask* dead_cards) {
    int i, c, o, rc, len;

    if (num_dead == NULL) {
        return (-1);
    }

    if (dead_cards == NULL) {
        return (-1);
    }

    *num_dead = 0;
    CardMask_RESET(*dead_cards);

    /*
     * parse any options passed to us.
     * -d "c1 [...]" is dead cards
     */
    while ((o = getopt(argc, argv, opts)) != -1) {
        switch (o) {
        case 'd':
            len = strlen(optarg);
            for (i = 0; i < len;) {
                rc = StdDeck_stringToCard(optarg + i, &c);
                if (rc) {
                    StdDeck_CardMask_SET(*dead_cards, c);
                    (*num_dead)++;
                    i += 2;
                }
                else {
                    i++;
                }
            }
            break;
        }
    }

    return (*num_dead);
}

void dump_totals(void) {
    int i;

    for (i = HandType_FIRST; i <= HandType_LAST; i++)
        printf("%s:  %d\n", handTypeNamesPadded[i], totals[i]);
}

static void
parseArgsHvh(int argc, char** argv) {
    int i, count = 0, c;
    for (i = 0; i < argc; ++i) {
        if (StdDeck_stringToCard(argv[i], &c) == 0) {
            goto error;
        }
        if (count < 2) {
            //printf("%d", c);
            StdDeck_CardMask_SET(gPlayerCards[0], c);
        }
        else if (count < 4) {
            //printf("%d", c);
            StdDeck_CardMask_SET(gPlayerCards[1], c);
        }
        ++count;
    }

    if (count < 4) { goto error; }
    return;
error:
    fprintf(stderr, "Usage: hcmp2 [ -d dead-card ] p1-cards p2-cards [ common-cards ]\n");
    exit(0);
}


int hvhCalc(int argc, char* argv[], float *equity)
{
    CardMask cards, p0, p1, c0, c1;
    HandVal h0, h1;
    int h0_count = 0, h1_count = 0, tie_count = 0, count = 0;
    int iter = 1;

    CardMask_RESET(gPlayerCards[0]);
    CardMask_RESET(gPlayerCards[1]);
    CardMask_RESET(gDeadCards);
    parseArgsHvh(argc, argv);

    CardMask_OR(gDeadCards, gDeadCards, gPlayerCards[0]);
    CardMask_OR(gDeadCards, gDeadCards, gPlayerCards[1]);

    ENUMERATE_N_CARDS_D(cards, 5, gDeadCards,
        {
          ++count;
          CardMask_OR(c0, gPlayerCards[0], cards);
          h0 = Hand_EVAL_N(c0, 7);
          CardMask_OR(c1, gPlayerCards[1], cards);
          h1 = Hand_EVAL_N(c1, 7);
          if (h0 > h1)
++h0_count;
          else if (h1 > h0)
++h1_count;
          else
++tie_count;
        }
    );
    *equity = ((100.00 * h0_count)+(50.00*tie_count)) / count;
    printf("boards:%d h0_count:%d h1_count:%d tie_count:%d\n", count, h0_count, h1_count, tie_count);
    //iter++
/*
    printf("%d boards \n", count);


    printf("  cards      win  %%win       loss  %%lose       tie  %%tie      EV\n");
    printf("  %s  %7d %6.2f   %7d %6.2f   %7d %6.2f     %5.3f\n",
        Deck_maskString(gPlayerCards[0]),
        h0_count, 100.0 * h0_count / count,
        h1_count, 100.0 * h1_count / count,
        tie_count, 100.0 * tie_count / count,
        (1.0 * h0_count + (tie_count / 2.0)) / count);

    printf("  %s  %7d %6.2f   %7d %6.2f   %7d %6.2f     %5.3f\n",
        Deck_maskString(gPlayerCards[1]),
        h1_count, 100.0 * h1_count / count,
        h0_count, 100.0 * h0_count / count,
        tie_count, 100.0 * tie_count / count,
        (1.0 * h1_count + (tie_count / 2.0)) / count);
*/
    return 0;
}

/* Parse 2nd card of hand */
int parseCard(char* card2buf, char* card2, int size) {
    if (size == 2) {
        card2[0] = card2buf[0];
        if (strlen(card2buf) == 1) {
            card2[1] = '0';
        }
        else {
            card2[1] = card2buf[1];
        }
        card2[2] = '\0';
    }
    else if(size ==0){
        card2[0] = '0';
        card2[1] = '\0';
    }
    else{
        card2[0] = card2buf[0];
        card2[1] = '\0';
    }
    return 0;
    }

/* Parse float*/
int parseFloat(float hand1, float hand2, int size1, int size2, char **holeCards) {
    int h1c1, h1c2, h2c1, h2c2;
    char *ptr, * fh1c1str, * fh1c2strbuf, * fh2c1str, * fh2c2strbuf;
    char  fh1c2str[3], fh2c2str[3], fHand1str[5], fHand2str[5], h1c1str[3], h1c2str[3], h2c1str[3], h2c2str[3];
    gcvt(hand1, 4, fHand1str);
    gcvt(hand2, 4, fHand2str);
    fh1c1str = strtok(fHand1str, ".");
    fh1c2strbuf = strtok(NULL, "\0");
    fh2c1str = strtok(fHand2str, ".");
    fh2c2strbuf = strtok(NULL, "\0");
    parseCard(fh1c2strbuf, fh1c2str, size1);
    parseCard(fh2c2strbuf, fh2c2str, size2);
    //printf("%s", fh1c1str);
    h1c1 = atoi(fh1c1str);
    h1c2 = atoi(fh1c2str);
    h2c1 = atoi(fh2c1str);
    h2c2 = atoi(fh2c2str);
    
    StdDeck_cardToString(h1c1, h1c1str);
    StdDeck_cardToString(h1c2, h1c2str);
    StdDeck_cardToString(h2c1, h2c1str);
    StdDeck_cardToString(h2c2, h2c2str); 
    holeCards[0] = h1c1str;
    holeCards[1] = h1c2str;
    holeCards[2] = h2c1str;
    holeCards[3] = h2c2str;
    return 0;
   
}


/* Parse float*/
int parseFloatOne(float hand1, int size1, char** holeCards) {
    int h1c1, h1c2, h2c1, h2c2;
    char* ptr, * fh1c1str, * fh1c2strbuf;
    char  fh1c2str[3], fHand1str[5], h1c1str[3], h1c2str[3];
    gcvt(hand1, 4, fHand1str);
    fh1c1str = strtok(fHand1str, ".");
    fh1c2strbuf = strtok(NULL, "\0");
    parseCard(fh1c2strbuf, fh1c2str, size1);
    //printf("%s", fh1c1str);
    h1c1 = atoi(fh1c1str);
    h1c2 = atoi(fh1c2str);

    StdDeck_cardToString(h1c1, h1c1str);
    StdDeck_cardToString(h1c2, h1c2str);
    holeCards[0] = h1c1str;
    holeCards[1] = h1c2str;
    return 0;

}

/* Compute hand vs. hand equities for given matrix*/
int HandvHandMatrix(float hvh[SIZE][SIZE]) {
    float fHand1, fHand2, size1, size2, equity;
    char* holeCards[4];
    //const char *delim = '.';
    for (int i = 1300;  i < SIZE; i++) {
        for (int j =2; j <SIZE; j++) {
            //fHand1 = hvh[i][0];
            //fHand2 = hvh[0][j];
           // size1 = hvh[i][1];
           // size2 = hvh[1][j];
            parseFloat(hvh[i][0], hvh[0][j], hvh[i][1], hvh[1][j], holeCards);
            printf("cards: %s %s %s %s ", holeCards[0], holeCards[1], holeCards[2], holeCards[3]);
            hvhCalc(4, holeCards, &equity);
            hvh[i][j] = equity;
        }
    }
    return 0;
}


/* Populate hands for table */
int popHvh(int i, float holeCards, float hvh[SIZE][SIZE], int fSize) {
    hvh[i+2][0] = holeCards;
    hvh[0][i+2] = holeCards;

    // 2nd row stores number of digits for 2nd hole card
    hvh[1][i+2] = fSize;
    hvh[i+2][1] = fSize;
    return 0;
}

/* Print matrix */
int printHvh(float hvh[SIZE][SIZE]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%.2f  ", hvh[i][j]);
        }
        printf(" | \n");
    }
}

/* Write to csv*/
int writeCsv(float hvh[SIZE][SIZE]) {
    FILE* fpt;

    fpt = fopen("hvh14.csv", "w");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
                fprintf(fpt, "%.2f,", hvh[i][j]);
            }
        fprintf(fpt, "\n");
        }
    fclose(fpt);
    return 0;
}

int indexer(float hvh[SIZE][SIZE]) {
    char* holeCards[2];
    FILE* fpt;
    fpt = fopen("converter.txt", "w");
    for (int j = 2; j < SIZE; j++) {
        parseFloatOne(hvh[0][j], hvh[1][j], holeCards);
        printf("%d ", j);
        printf("%s \n", strcat(holeCards[0], holeCards[1]));
        fprintf(fpt, "%d ", j);
        fprintf(fpt, "%s \n", strcat(holeCards[0], holeCards[1]));
    }
    fclose(fpt);
    return 0;
}



#ifdef VERBOSE
#define DUMP_HAND do {                          \
  Deck_printMask(cards);                        \
  printf(": ");                                 \
  HandVal_print(handval);                       \
  printf("\n");                                 \
} while (0)
#else
#define DUMP_HAND do { } while (0)
#endif

int main(int argc, char* argv[])
{
    CardMask cards, dead_cards;
    int holeCards[2];
    HandVal  handval;
    int      num_dead;
    int     cardIndex;
    int     length;
    char   outString;
    int i = 0;
    float hvh[SIZE][SIZE] = {0};
    int card1, card2;
    char card1str[4], card2str[4];
    float holeCardsF;
    char holeCardsStr[10];
    char* endp;

    parse_args(argc, argv, &num_dead, &dead_cards);

    ENUMERATE_2_CARDS_D(cards, dead_cards,
        {
           StdDeck_maskToCards((void*)&cards, holeCards);
           card1 = holeCards[0];
           card2 = holeCards[1];
           snprintf(card1str, 4, "%d", card1);
           snprintf(card2str, 4, "%d", card2);
           strcpy(holeCardsStr, card1str);
           strcat(holeCardsStr, ".");
           strcat(holeCardsStr, card2str);
           strcat(holeCardsStr, ".");
           holeCardsF = strtof(holeCardsStr, &endp);
           StdDeck_printMask(cards);
           printf(" %i ", i);
           printf(" %s %s", card1str, card2str);
           //printf(" %s\n", holeCardsStr);
           printf(" %.*f\n", (int)strlen(card2str), holeCardsF);
           if (card2 == 0) {
               length = 0;
           }
           else {
               length = (int)strlen(card2str);
           }
           popHvh(i, holeCardsF, hvh, length);
           DUMP_HAND;
           i++;

        });
    printHvh(hvh);
    HandvHandMatrix(hvh);
    printHvh(hvh);
    //indexer(hvh);
    writeCsv(hvh);

    exit(0);
}
