//
//  main.cpp
//  BitMApFilter
//
//  Created by Guillaume Luccisano on 10/17/15.
//  Copyright (c) 2015 Atul Kumar. All rights reserved.
//

#include <iostream>
#include "Matrix.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    Mat m("/Users/guillaume/atul/landscape_small.bmp");
    m.applyShapenningFilter();
    m.outputFile("/Users/guillaume/atul/landscape_small_out.bmp");
    return 0;
}
