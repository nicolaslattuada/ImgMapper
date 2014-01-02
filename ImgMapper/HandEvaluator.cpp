//
//  HandEvaluator.cpp
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#include "HandEvaluator.h"

StdDeck_CardMask HandEvaluator::textToPokerEval(const char* strHand)
{
    StdDeck_CardMask theHand, theCard;
    StdDeck_CardMask_RESET(theHand);
    
    if (strHand && strlen(strHand))
    {
        int cardIndex = -1;
        char* curCard = const_cast<char*>(strHand);
        
        while (*curCard)
        {
            printf("curCard:%s \n", curCard);
            // Take the card text and convert it to an index (0..51)
            StdDeck_stringToCard(curCard, &cardIndex);
            printf("cardIdx: %d \n", cardIndex);
            // Convert the card index to a mask
            theCard = StdDeck_MASK(cardIndex);
            // Add the card (mask) to the hand
            StdDeck_CardMask_OR(theHand, theHand, theCard);
            // Advance to the next card (if any)
            curCard += 2;
        }
    }
    
    //char *handStr = StdDeck_maskString(theHand);
    
    //printf("hand:%s", handStr);
    
    return theHand;
}

int HandEvaluator::evalHand(const char* player, const char* board)
{
    StdDeck_CardMask player1 = HandEvaluator::textToPokerEval("AhAd");
    StdDeck_CardMask player2 = HandEvaluator::textToPokerEval("KcKs");
    StdDeck_CardMask theBoard = HandEvaluator::textToPokerEval("2d4c4sKh9h");
    
    // Get each player’s full 7-card hand into his mask
    StdDeck_CardMask_OR(player1, player1, theBoard);
    StdDeck_CardMask_OR(player2, player2, theBoard);
    
    // Evaluate each player’s hand
    int player1Val = StdDeck_StdRules_EVAL_N(player1, 7);
    int player2Val = StdDeck_StdRules_EVAL_N(player2, 7);
    
     player1Val = StdDeck_StdRules_EVAL_TYPE(player1, 7);
     player2Val = StdDeck_StdRules_EVAL_TYPE(player2, 7);
    
    printf("player1: %d\n", player1Val);
    printf("player2: %d\n", player2Val);

    return player1Val;
}



