/*
 * MTF: Encode/decode using "move-to-front" transform and Fibonacci encoding.
 */

#include <stdlib.h>
#include <stdio.h>

#include "mtft.h"
#include "debug.h"

/**
 * @brief  Given a symbol value, determine its encoding (i.e. its current rank).
 * @details  Given a symbol value, determine its encoding (i.e. its current rank)
 * according to the "move-to-front" transform described in the assignment
 * handout, and update the state of the "move-to-front" data structures
 * appropriately.
 *
 * @param sym  An integer value, which is assumed to be in the range
 * [0, 255], [0, 65535], depending on whether the
 * value of the BYTES field in the global_options variable is 1 or 2.
 *
 * @return  An integer value in the range [0, 511] or [0, 131071]
 * (depending on the value of the BYTES field in the global_options variable),
 * which is the rank currently assigned to the argument symbol.
 * The first time a symbol is encountered, it is assigned
 * a default rank computed by adding 512 or 131072 to its value.
 * A symbol that has already been encountered is assigned a rank in the
 * range [0, 255], [0, 65535], according to how recently it has occurred
 * in the input compared to other symbols that have already been seen.
 * For example, if this function is called twice in succession
 * with the same value for the sym parameter, then the second time the
 * function is called the value returned will be 0 because the symbol will
 * have been "moved to the front" on the previous call.
 *
 * @modifies  The state of the "move-to-front" data structures.
 */

int bufferLength = 0;
long bufferValue = 0;
int prev_bit = -1;
int initialized = 0;


void initialize(){
    initialized = 1;
    mtf_map = NULL;
    current_offset = 0;
    first_unused_node_index = 0;
    recycled_node_list = NULL;

    SYMBOL* p = symbol_rank;
    int i = 0;
    while(i<SYMBOL_MAX){
        *(p+i) = -1;
        i++;
    }

    OFFSET* p1 = last_offset;
    i = 0;
    while(i<SYMBOL_MAX){
        *(p1+i) = -1;
        i++;
    }

}

int numFibbonaci(int rank){
    if(rank<=1){
        return rank;
    }
    else if(rank==2){
        return 2;
    }

    int fib1=1;
    int fib2=2;
    int fib = fib1+fib2;
    int count=2;

    while(fib<=rank){
        count++;
        fib1 = fib2;
        fib2 = fib;
        fib = fib1+fib2;
    }

    return count;
}

int maxFibbonaci(int rank){
    if(rank<=3){
        return rank;
    }

    int fib1=1;
    int fib2=2;
    int fib = fib1+fib2;

    while(fib<=rank){
        fib1 = fib2;
        fib2 = fib;
        fib = fib1+fib2;
    }

    return fib2;
}


int secondMaxFibbonaci(int rank){
    if(rank==1){
        return 0;
    }
    else if(rank==2){
        return 1;
    }

    int fib1=1;
    int fib2=2;
    int fib = fib1+fib2;

    while(fib<=rank){
        fib1 = fib2;
        fib2 = fib;
        fib = fib1+fib2;
    }

    return fib1;
}

long getValue(int rank, int numDigits){
    if(numDigits==0){
        return 0;
    }
    int fib = maxFibbonaci(rank);
    int fib2 = secondMaxFibbonaci(rank);
    int fib1 = fib - fib2;

    int bitpos = 1;
    long decimalNum = 1;

    while(bitpos < numDigits){
        if(fib <= rank){
            rank -= fib;
            decimalNum += 1<<bitpos;
        }
        bitpos++;
        fib = fib2;
        fib2 = fib1;
        fib1 = fib - fib2;
    }

    return decimalNum;
}

long long getTotalValue(int rank){
    int numDigits = numFibbonaci(rank)+1;
    long long decimalNum = getValue(rank, numDigits);
    int totalLength = numDigits + bufferLength;
    int numBytes = totalLength / 8;
    int finalLength = numBytes*8;
    if(finalLength<8) {
        bufferLength = totalLength;
        bufferValue = (bufferValue * (1<<numDigits)) + decimalNum;
        return -1;
    }
    int number = totalLength - finalLength;
    long long valueTaken = decimalNum - getValue(rank, number);
    valueTaken = valueTaken / (1<<number);
    int num1 = finalLength - bufferLength;
    long long ans = (bufferValue * (1<<num1)) + valueTaken;
    bufferLength = number;
    bufferValue = getValue(rank, number);
    return ans;
}

int onlyNumberBytes(int rank){
    int numDigits = numFibbonaci(rank)+1;
    int totalLength = numDigits + bufferLength;
    int numBytes = totalLength / 8;
    return numBytes;
}


int toHexadecimal(int num, int byteNumber){
    int bitpos = (8*byteNumber) - 1;
    int value = 0;
    int count = 0;
    int currentBit = 7;
    while(count<8){
        int bool = (num & (1<<bitpos));
        if(bool){
            value += 1<<(currentBit);
        }
        bitpos--;
        count++;
        currentBit--;
    }
    return value;
}

int bitValue(long bits, long index){
    int v = (bits & (1<<index));
    if(v){
        return 1;
    }
    return 0;
}

long getOneByte(int start_bit, int num_bits, long decimalNum, int num_digits, long value){
    int rem = start_bit+1;
    if(rem < num_bits){
        int bitpos = 7 - bufferLength;
        bufferLength += rem;
        while(rem){
            if(bitValue(decimalNum, start_bit)){
                bufferValue += 1<<bitpos;
            }
            start_bit--;
            bitpos--;
            rem--;
        }
        return -1;
    }
    int count = 0;
    int curbitpos = 7-bufferLength;
    while(count<num_bits){
        int bit = bitValue(decimalNum, start_bit);
        if(bit){
            value += 1<<curbitpos;
        }
        curbitpos--;
        start_bit--;
        count++;
    }
    bufferLength = 0;
    bufferValue = 0;
    return value;
}


long getOneByteTwo(int start_bit, int num_bits, long decimalNum, int num_digits, long value){
    int rem = start_bit+1;
    if(rem < num_bits){
        int bitpos = 7 - bufferLength;
        bufferLength += rem;
        while(rem){
            if(bitValue(decimalNum, start_bit)){
                bufferValue += 1<<bitpos;
            }
            start_bit--;
            bitpos--;
            rem--;
        }
        return -1;
    }
    int count = 0;
    int curbitpos = 7-bufferLength;
    while(count<num_bits){
        int bit = bitValue(decimalNum, start_bit);
        if(bit){
            value += 1<<curbitpos;
        }
        curbitpos--;
        start_bit--;
        count++;
    }
    bufferLength = 0;
    bufferValue = 0;
    return value;
}


struct mtf_node* search(struct mtf_node* cur, char* bits, int depth, int index){

    if(index==depth){
        return cur;
    }

    if(*(bits+index)=='0'){
        return search(cur->left_child, bits, depth, index+1);
    }
    else{
        return search(cur->right_child, bits, depth, index+1);
    }
}

struct mtf_node* newSearch(struct mtf_node* cur, OFFSET bits, long depth, long index){

    if(index==depth){
        return cur;
    }

    if(bitValue(bits, depth-index-1)==0){
        return newSearch(cur->left_child, bits, depth, index+1);
    }
    else{
        return newSearch(cur->right_child, bits, depth, index+1);
    }
}

void createRoot(){
    struct mtf_node* new_node;
    if(!recycled_node_list){
        new_node = &(*(node_pool+first_unused_node_index));
        first_unused_node_index++;
    }
    else{
        new_node = recycled_node_list;
        recycled_node_list = recycled_node_list->left_child;
    }

    mtf_map->parent = new_node;
    new_node->left_child = mtf_map;
    mtf_map = new_node;

}

void populateCount(struct mtf_node* cur){
    if(!cur->left_child){
        cur->left_count = 0;
    }
    else{
        cur->left_count = cur->left_child->left_count + cur->left_child->right_count;
        if((!cur->left_child->left_child) && (!cur->left_child->right_child)){
            cur->left_count++;
        }
    }

    if(!cur->right_child){
        cur->right_count = 0;
    }
    else{
        cur->right_count = cur->right_child->left_count + cur->right_child->right_count;
        if((!cur->right_child->left_child) && (!cur->right_child->right_child)){
            cur->right_count++;
        }
    }
}


int populateCounts(struct mtf_node* cur){

    if(!cur){
        return 0;
    }

    if((!cur->left_child) && (!cur->right_child)){
        cur->left_count = 0;
        cur->right_count = 0;
        return 1;
    }

    if(!cur->left_child){
        cur->left_count = 0;
    }
    else{
        cur->left_count = populateCounts(cur->left_child);
    }

    if(!cur->right_child){
        cur->right_count = 0;
    }
    else{
        cur->right_count = populateCounts(cur->right_child);
    }

    return cur->left_count + cur->right_count;
}


int newpopulateCounts(struct mtf_node* cur){

    if(!cur){
        return 0;
    }

    if(!cur->left_child){
        cur->left_count = 0;
    }
    else{
        cur->left_count = cur->left_child->left_count + cur->left_child->right_count;
        if((!cur->left_child->left_child) && (!cur->left_child->right_child)){
            cur->left_count++;
        }
    }

    if(!cur->right_child){
        cur->right_count = 0;
    }
    else{
        cur->right_count = cur->right_child->left_count + cur->right_child->right_count;
        if((!cur->right_child->left_child) && (!cur->right_child->right_child)){
            cur->right_count++;
        }
    }
    newpopulateCounts(cur->left_child);
    newpopulateCounts(cur->right_child);
    return 1;
}


void newPopulateCounts(struct mtf_node* cur){
    while(cur){
        populateCount(cur);
        cur = cur->parent;
    }
}


int getVal(struct mtf_node* cur){
    if(!cur->parent){
        return cur->right_count;
    }


    if(cur->parent->right_child == cur){
        int val = getVal(cur->parent) - cur->left_count;
        if((!cur->left_child) && (!cur->right_child)){
            val -= 1;
        }
        return val;
    }
    else{
        return getVal(cur->parent) + cur->right_count;
    }
}

int newgetVal(struct mtf_node* cur){
    if(!cur->parent){
        return cur->right_count;
    }

    int total = 0;
    while(cur->parent){
        if(cur->parent->right_child == cur){
            total += 0;
        }
        else{
            total += cur->parent->right_count;
        }
        cur = cur->parent;
    }
    return total;
}

struct mtf_node* getNewNode(){
    struct mtf_node* new_node;
    if(!recycled_node_list){
        new_node = &(*(node_pool+first_unused_node_index));
        first_unused_node_index++;
    }
    else{
        new_node = recycled_node_list;
        recycled_node_list = recycled_node_list->left_child;
    }
    new_node->left_child = NULL;
    new_node->right_child = NULL;
    new_node->parent = NULL;
    new_node->left_count = 0;
    new_node->right_count = 0;
    return new_node;
}


void freeNode(struct mtf_node* node1){
    node1->left_child = NULL;
    node1->right_child = NULL;
    node1->parent = NULL;
    node1->left_count = 0;
    node1->right_count = 0;
    node1->symbol = NO_SYMBOL;
    node1->left_child = recycled_node_list;
    recycled_node_list = node1;
}


void repopulate(struct mtf_node* cur, int direction){
    if(direction==0){
        cur->left_count++;
    }
    else{
        cur->right_count++;
    }

    if(!cur->parent){
        return;
    }

    if(cur->parent->left_child == cur){
        repopulate(cur->parent, 0);
    }
    else{
        repopulate(cur->parent, 1);
    }

}


void retrace(struct mtf_node* cur){
    struct mtf_node* p = cur->parent;
    int count = 0;
    if(p->left_child){
        count++;
    }
    if(p->right_child){
        count++;
    }

    int direction = 0;

    if(p->left_child == cur){
        direction = 0;
    }
    else{
        direction = 1;
    }

    if(count==1){
        if(p->left_child == cur){
            p->left_count++;
        }
        else{
            p->right_count++;
        }
        return;
    }
    else{
        repopulate(p, direction);
    }
}


void newhelperInsert(struct mtf_node* cur, struct mtf_node* new_node, long bits, int index, int depth){
    if(index==depth-1){
        if(bitValue(bits, 0) == 0){
            cur->left_count++;
            cur->left_child = new_node;
            new_node->parent = cur;
        }
        else{
            cur->right_count++;
            cur->right_child = new_node;
            new_node->parent = cur;
        }
        //retrace(new_node);
        return;
    }

    if(bitValue(bits, depth-index-1) == 0){
        cur->left_count++;
        if(!cur->left_child){
            cur->left_child = getNewNode();
            cur->left_child->parent = cur;
        }
        newhelperInsert(cur->left_child, new_node, bits, index+1, depth);
    }
    else{
        cur->right_count++;
        if(!cur->right_child){
            cur->right_child = getNewNode();
            cur->right_child->parent = cur;
        }
        newhelperInsert(cur->right_child, new_node, bits, index+1, depth);
    }
    return;
}


void newInsert(OFFSET bits, long depth, int sym){
    if(!mtf_map){
        mtf_map = getNewNode();
        current_offset++;
        return;
    }
    else{
        struct mtf_node* new_node = getNewNode();
        newhelperInsert(mtf_map, new_node, bits, 0, depth);
    }

    struct mtf_node* insertedNode = newSearch(mtf_map, current_offset, depth, 0);
    insertedNode->symbol = sym;
    current_offset++;
}


void deleteTrace(struct mtf_node* curr){

    if(!curr){
        return;
    }

    struct mtf_node* parent=NULL;

    if((!curr->left_child) && (!curr->right_child)){
        if(!curr->parent){
            //freeNode(curr);
            //mtf_map = NULL;
            return;
        }

        if(curr->parent->left_child == curr){
            //struct mtf_node* parent = curr->parent;
            parent = curr->parent;
            parent->left_child = NULL;
            freeNode(curr);
        }
        else{
            parent = curr->parent;
            //struct mtf_node* parent = curr->parent;
            parent->right_child = NULL;
            freeNode(curr);
        }
        deleteTrace(parent);
    }

    return;
}


struct mtf_node* newdeleteNode(struct mtf_node* cur, long bits, int index, int depth){
    if(index==depth-1){
        if(bitValue(bits, 0) == 0){
        //if(*(bits+depth-1)=='0'){
            struct mtf_node* tbd = cur->left_child;
            cur->left_child = NULL;
            freeNode(tbd);
            return cur;
        }
        else{
            struct mtf_node* tbd = cur->right_child;
            cur->right_child = NULL;
            freeNode(tbd);
            return cur;
        }
    }

    if(bitValue(bits, depth-index-1) == 0){
        return newdeleteNode(cur->left_child, bits, index+1, depth);
    }
    else{
        return newdeleteNode(cur->right_child, bits, index+1, depth);
    }
}


void newDelete(struct mtf_node* cur, OFFSET bits, long index, long depth){
    struct mtf_node* node = newdeleteNode(cur, bits, index, depth);
    deleteTrace(node);
    return;
}

void decodeDelete(struct mtf_node* cur){
    struct mtf_node* parent = cur->parent;
    if(parent->left_child == cur){
        parent->left_child = NULL;
    }
    else{
        parent->right_child = NULL;
    }
    freeNode(cur);
    deleteTrace(parent);
    return ;
}


void inorder(struct mtf_node* cur, int space){
    if(!cur){
        return;
    }
    space += 10;

    inorder(cur->right_child, space);

    printf("\n");
    for(int i=10; i<space; i++){
        printf(" ");
    }
    printf("%d %d %d\n", cur->left_count, cur->right_count, cur->symbol);
    inorder(cur->left_child, space);
    return;
}


long getDepth(OFFSET d){
    int count=0;
    while(d>0){
        d = d / 2;
        count++;
    }
    return count;
}

void reduceCounts(struct mtf_node* cur){
    while(cur->parent){
        if(cur->parent->left_child == cur){
            cur->parent->left_count--;
        }
        else{
            cur->parent->right_count--;
        }
        cur = cur->parent;
    }
}


struct mtf_node* decodeSearch(struct mtf_node* cur, int rank, int depth, int index){
    if(index==depth){
        return cur;
    }

    if(cur->right_count > rank){
        return decodeSearch(cur->right_child, rank, depth, index+1);
    }
    else{
        return decodeSearch(cur->left_child, rank-cur->right_count, depth, index+1);
    }

}


void decodeInsert(OFFSET bits, long depth, int sym){
    if(!mtf_map){
        mtf_map = getNewNode();
        current_offset++;
        return;
    }
    else{
        struct mtf_node* new_node = getNewNode();
        newhelperInsert(mtf_map, new_node, bits, 0, depth);
    }


    struct mtf_node* insertedNode = decodeSearch(mtf_map, current_offset, depth, 0);
    insertedNode->symbol = sym;
    current_offset++;
}


CODE mtf_map_encode(SYMBOL sym) {
    if(initialized == 0){
        initialize();
    }
    int v = (global_options & (1<<1));
    int bytes;
    if(v){
        bytes = 256*256;
    }
    else{
        bytes = 256;
    }

    if(!mtf_map){
        mtf_map = getNewNode(); // returns new node
        mtf_map->symbol = sym;
        *(last_offset+sym) = current_offset;
        current_offset++;
        return sym+bytes;
    }

    if(((current_offset) & (current_offset-1)) == 0){
        createRoot(); // new root since offset is 2^n
    }

    populateCount(mtf_map);

    CODE rank;
    long d = getDepth(current_offset); // current no. of bits required to represent offset or last offset of symbol

    if(*(last_offset+sym) == -1){ // symbol not seen yet
        rank = sym + bytes;
        newInsert(current_offset, d, sym); // insert in tree
        *(last_offset+sym) = current_offset-1; // set last offset to current offset (-1 since current offset is updated in insert function)
    }
    else{
        OFFSET curpos = *(last_offset+sym); // offset at which symbol was last seen
        struct mtf_node* t = newSearch(mtf_map, curpos, d, 0); // pointer to current node representing symbol in tree
        rank = getVal(t); // get no. of right leaf nodes to current node of the symbol
        reduceCounts(t);
        newDelete(mtf_map, curpos, 0, d); // delete current node
        newInsert(current_offset, d, sym); // insert node at new position
        *(last_offset+sym) = current_offset-1; // set last offset to current offset
    }
    //printf("%d\n", rank);
    return rank;
}


SYMBOL mtf_map_decode(CODE code) {
    if(initialized == 0){
        initialize();
    }
    int v = (global_options & (1<<1));
    int bytes;
    if(v){
        bytes = 256*256;
    }
    else{
        bytes = 256;
    }

    if((code<0) || (code>=2*bytes)){
        return -1;
    }

    if(!mtf_map){
        mtf_map = getNewNode(); // returns new node
        mtf_map->symbol = code-bytes;
        current_offset++;
        return code-bytes;
    }

    if(((current_offset) & (current_offset-1)) == 0){
        createRoot(); // new root since offset is 2^n
    }

    populateCount(mtf_map);

    CODE rank;
    long d = getDepth(current_offset); // current no. of bits required to represent offset or last offset of symbol

    if(code >= bytes){ // symbol not seen yet
        rank = code - bytes;
        newInsert(current_offset, d, rank); // insert in tree
    }
    else{
        struct mtf_node* t = decodeSearch(mtf_map, code, d, 0);
        rank = t->symbol;
        reduceCounts(t);
        decodeDelete(t);
        newInsert(current_offset, d, rank); // insert node at new position
    }

    return rank;
}

int consecutive_ones(int ch, int start_bit){
    //int prev_bit = -1;
    while(start_bit >= 0){
        if((bitValue(ch, start_bit)== 1) && (prev_bit==1)){
            return start_bit;
        }
        prev_bit = bitValue(ch, start_bit);
        start_bit--;
    }
    return -1;
}

long getDecodeValue(int ch, int start_bit, int end_bit){
    int curbit = start_bit;
    long value=0;
    while(curbit >= end_bit){
        if(bitValue(ch, curbit) == 1){
            value += 1<<curbit;
        }
        curbit--;
    }
    return value;
}

long getDecodeValueEnd(int ch, int start_bit, int end_bit){
    int curbit = end_bit;
    int curpos = 0;
    long value = 0;
    while(curbit <= start_bit){
        if(bitValue(ch, curbit) == 1){
            value += 1<<curpos;
        }
        curbit++;
        curpos++;
    }
    return value;
}

int getFinalChar(long total_value, int total_length){
    int curpos = total_length-1;
    int prevfib = 1;
    int curfib = 1;
    int value = 0;
    while(curpos>=0){
        if(bitValue(total_value, curpos)==1){
            value += curfib;
        }
        int tmp = prevfib;
        prevfib = curfib;
        curfib += tmp;
        curpos--;
    }
    return value;
}


int mtf_encode() {
    initialize(); // Function to initiliaze root pointer of tree and last offset array
    int ch;
    int v = (global_options & (1<<1));
    if(v){
        while ((ch = fgetc(stdin)) != EOF){
            int counter = 0;
            int ch1 = fgetc(stdin);
            if(ch1 == EOF){
                return -1;
            }
            //printf("Char1 Char2 %d %d\n", ch, ch1);
            int ch2 = (256*ch) + ch1;
            //printf("Char %d\n", ch2);
            CODE c = mtf_map_encode(ch2);
            int numDigits = numFibbonaci(c+1) + 1;
            //printf("Num digits %d\n", numDigits);
            long numValue = getValue(c+1, numDigits);
            int start_bit = numDigits - 1;
            while(1){
                int tbs = 8-bufferLength;
                if(start_bit<0){
                    break;
                }
                long val = getOneByte(start_bit, 8-bufferLength, numValue, numDigits, bufferValue);
                start_bit -= tbs;
                counter++;
                if(val != -1){
                    //printf("Ch %lu\n", val);
                    putchar(val);
                }
                else{
                    break;
                }
            }
            //printf("BV Bl %lu %d", bufferValue, bufferLength);
        }
        if(bufferLength>0){
            //printf("Ch %lu\n", bufferValue);
            putchar(bufferValue);
        }
    }
    else{
        while ((ch = fgetc(stdin)) != EOF){
            int counter = 0;
            CODE c = mtf_map_encode(ch);
            int numDigits = numFibbonaci(c+1) + 1;
            long numValue = getValue(c+1, numDigits);
            int start_bit = numDigits - 1;
            while(1){
                int tbs = 8-bufferLength;
                if(start_bit<0){
                    break;
                }
                long val = getOneByte(start_bit, 8-bufferLength, numValue, numDigits, bufferValue);
                start_bit -= tbs;
                counter++;
                if(val != -1){
                    putchar(val);
                }
                else{
                    break;
                }
            }
        }
        if(bufferLength>0){
            putchar(bufferValue);
        }
    }
    return 0;
}


int mtf_decode() {
    initialize();
    int ch;
    int v = (global_options & (1<<1));
    if(v){
        while ((ch = fgetc(stdin)) != EOF){
            int start_bit = 7;
            while(start_bit>=0){
                int pos = consecutive_ones(ch, start_bit) + 1;
                if(pos == 0){
                    bufferValue *= (1<<(start_bit+1));
                    bufferValue += getDecodeValue(ch, start_bit, 0);
                    bufferLength += start_bit+1;
                    //printf("Buffer value Buffer length %lu %d\n", bufferValue, bufferLength);
                    break;
                }
                else{
                    int num = start_bit-pos+1;
                    long total_value = bufferValue*(1<<num);
                    long decodeVal = getDecodeValueEnd(ch, start_bit, pos);
                    total_value += decodeVal;
                    int total_length = bufferLength + start_bit - pos + 1;
                    int sym = getFinalChar(total_value, total_length);
                    SYMBOL s = mtf_map_decode(sym-1);
                    if(s==-1){
                        return -1;
                    }
                    //printf("SYM %d\n", s);
                    int char1 = s / 256;
                    int char2 = s-(256*char1);
                    putchar(char1);
                    putchar(char2);
                    bufferValue = 0;
                    bufferLength = 0;
                    prev_bit = -1;
                    start_bit = pos-2;
                }
            }
        }
    }
    else{
        while ((ch = fgetc(stdin)) != EOF){
            int start_bit = 7;
            while(start_bit>=0){
                int pos = consecutive_ones(ch, start_bit)+1;
                if(pos == 0){
                    bufferValue *= (1<<(start_bit+1));
                    bufferValue += getDecodeValue(ch, start_bit, 0);
                    bufferLength += start_bit+1;
                    break;
                }
                else{
                    int num = start_bit-pos+1;
                    long total_value = bufferValue*(1<<num);
                    long decodeVal = getDecodeValueEnd(ch, start_bit, pos);
                    total_value += decodeVal;
                    int total_length = bufferLength + start_bit - pos + 1;
                    int sym = getFinalChar(total_value, total_length);
                    SYMBOL s = mtf_map_decode(sym-1);
                    if(s==-1){
                        return -1;
                    }
                    putchar(s);
                    bufferValue = 0;
                    bufferLength = 0;
                    prev_bit = -1;
                    start_bit = pos-2;
                }
            }
        }
    }
    return 0;
}


int check(char *s){
    char *p = s;
    int flag = 0;
    while(*p != '\0'){
        if(*p != '0'){
            flag = 1;
            break;
        }
        p++;
    }

    if((flag==0) && (*p == '\0')){
        return 0;
    }

    if((*p != 49) && (*p != 50)){
        return 0;
    }

    int byteval = *p;

    p++;
    if(*p != '\0'){
        return 0;
    }
    return byteval-48;
}

int checkEquality(char *s1, char *s2){
    char *p1 = s1;
    char *p2 = s2;
    while((*p1 != '\0') && (*p2 != '\0')){
        if(*p1 != *p2){
            return 0;
        }
        p1++;
        p2++;
    }
    if(*p1 != '\0'){
        return 0;
    }
    if(*p2 != '\0'){
        return 0;
    }
    return 1;
}

int validargs(int argc, char **argv) {
    int counth=0, counte=0, countd=0, countb=0;
    int i = 1;
    int byteval = 1;
    if(argc<2){
        return -1;
    }

    while(i<argc){
        char *s = *(argv+i);
        if(checkEquality(s,"-h")){
        //if(s =="-h"){
            counth++;
            i++;
        }
        else if(checkEquality(s,"-e")){
            if((i!=argc-1) && !(checkEquality(*(argv+i+1),"-b"))){
                return -1;
            }
            counte++;
            i++;
        }
        else if(checkEquality(s,"-d")){
            countd++;
            if((i!=argc-1) && !(checkEquality(*(argv+i+1),"-b"))){
                return -1;
            }
            i++;
        }
        else if(checkEquality(s,"-b")){
            countb++;
            if((i==argc-1) || (!check(*(argv+i+1)))){
                return -1;
            }
            i += 2;
        }
        else{
            i++;
        }
    }


    if(counth > 0){
        if(checkEquality(*(argv+1),"-h")){
            global_options = HELP_OPTION;
            //printf("%d\n", number);
            return 0;
        }
        else{
            return -1;
        }
    }

    if((counte>1) || (countd>1) || (countb>1)){
        return -1;
    }

    if((countd>0) && (counte>0)){
        return -1;
    }

    if((countd==0) && (counte==0)){
        return -1;
    }

    i = 1;
    counte=0, countd=0, countb=0;

    while(i<argc){
        char *s = *(argv+i);
        if(checkEquality(s,"-e")){
            if(countb>0){
                return -1;
            }
            if((i != argc-1) && !(checkEquality(*(argv+i+1),"-b"))){
                return -1;
            }
            counte++;
            i++;
        }
        else if(checkEquality(s,"-d")){
            countd++;
            if(countb>0){
                return -1;
            }
            if((i != argc-1) && !(checkEquality(*(argv+i+1),"-b"))){
                return -1;
            }
            i++;
        }
        else if(checkEquality(s,"-b")){
            countb++;
            if((i==argc-1) || (!check(*(argv+i+1)))){
                return -1;
            }
            byteval = check(*(argv+i+1));
            i += 2;
        }
        else{
            return -1;
        }
    }

    if(countd==1){
        global_options = DECODE_OPTION + byteval;
    }
    else if(counte==1){
        global_options = ENCODE_OPTION + byteval;
    }

    return 0;
}
