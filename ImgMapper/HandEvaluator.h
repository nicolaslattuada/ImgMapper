//
//  HandEvaluator.h
//  ImgMapper
//
//  Created by nicolas lattuada on 26/12/13.
//  Copyright (c) 2013 nicolas lattuada. All rights reserved.
//

#ifndef __ImgMapper__HandEvaluator__
#define __ImgMapper__HandEvaluator__

#include <iostream>

#include <inlines/eval_type.h>
#include <poker_wrapper.h>
#include <deck_std.h>
#include <inlines/eval.h>
#include <enumerate.h>

class HandEvaluator
{
public:
    StdDeck_CardMask textToPokerEval(const char* strHand);
    int evalHand(const char* player, const char* board);
};

#endif /* defined(__ImgMapper__HandEvaluator__) */
