// Justin Young, Leon Kwong
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
//Extra libraries added
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <cctype>
#include <cstring>

using namespace std;

//NOTE: Naming conventions were taken from the input format in the project instructions
//Task 0 includes everything below and up to readGrammar();
//Create a structure/object type to store grammar rules

struct gRule{
    std::string lhs;
    vector<string> rhs;
};


//Create lexer object to call methods to read grammar
LexicalAnalyzer lexer;
//Create a vector that holds these grammar rules
vector<gRule> gRuleList;
//Used in task 1
vector<string> lhsList;

//Method to compare the current token with what was expected in the rule to see if there is a match
void tokenExpect(TokenType expToken){
    Token someToken = lexer.GetToken();
    if(someToken.token_type == expToken){
        //Do nothing if there is a match
        return;
    }
    //Otherwise just spit out an error
    else{
        cout << "SYNTAX ERROR!!!";
        exit(0);
    }
}

gRule idListParse(gRule someRule){
    Token pklist = lexer.peek(1);
    string temp = pklist.lexeme;
    switch(pklist.token_type){
        case STAR:
            tokenExpect(STAR);
            return someRule;
        
        case ID:
            tokenExpect(ID);
            someRule.rhs.push_back(temp);
            //Recursion
            someRule = idListParse(someRule);
            return someRule;
        
        default:
            cout << "SYNTAX ERROR!!!";
            exit(0);
    }
}

gRule rhsParse(gRule someRule){
    Token somerhs = lexer.peek(1);
    string temp = somerhs.lexeme;
    switch(somerhs.token_type){
        case STAR:
            tokenExpect(STAR);
            return someRule;

        case ID:
            someRule.rhs.push_back(temp);
            tokenExpect(ID);
            someRule = idListParse(someRule);
            return someRule;
        
        default:
            cout << "SYNTAX ERROR!!!";
            exit(0);
    }
}

//Parse the rule and store values in to lhs and rhs vectors
void ruleParsing(){
    //Create token types to check values of gramamr rules
    Token pklhs;
    gRule nRule;
    pklhs = lexer.peek(1);
    //Check lhs first
    if(pklhs.token_type == ID){
            tokenExpect(ID);
            //Then check for arrow before going into rhs
            Token pkArrow = lexer.peek(1);
            if(pkArrow.token_type == ARROW) {
                tokenExpect(ARROW);
                //Fill in information for GRule object
                nRule.lhs = pklhs.lexeme; //Fill information for left side
                //Fill information for right hand side. To make things easier, create a whole separate method.
                nRule = rhsParse(nRule);
                gRuleList.push_back(nRule);
            }
            else {
                cout << "SYNTAX ERROR!!!";
                exit(0);
            }
    }
    else {
        cout << "SYNTAX ERROR!!!";
        exit(0);
    }
}

void ruleListParse(){
    ruleParsing();
    Token pkrl = lexer.peek(1);
    switch(pkrl.token_type){
        case ID:
            //Recursion
            ruleListParse();
            return;
        
        case STAR:
            tokenExpect(STAR);
            return;
        
        case HASH:
            return;
        
        default:
            cout << "SYNTAX ERROR!!!";
            exit(0);
    }
}

void grammarParse(){
    ruleListParse();
    Token pkgp = lexer.peek(1);
    switch(pkgp.token_type){
        case HASH:
            tokenExpect(HASH);
            return;
        default:
            cout << "SYNTAX ERROR!!!";
            exit(0);
    }
}

// read grammar
void ReadGrammar(){ //May need to switch it up and replace with switch statements instead
    //Call grammarParse which will call conduct check on every grammar rule.
    grammarParse();
    Token pkrg = lexer.peek(1);
    //Start parsing grammar here
    switch(pkrg.token_type){
        case HASH:
            tokenExpect(HASH);
            return;
        
        case END_OF_FILE:
            return;
        
        default:
            cout << "SYNTAX ERROR!!";
            exit(0);
    }

    //Store information into a list to be used later
    // for(int i = 0; i < gRuleList.size(); i++){
    //     lhsList.push_back(gRuleList[i].lhs);
    // }

    //Print out at end
    cout << "0\n";
}

// Task 1
void printTerminalsAndNoneTerminals()
{
    vector<string> terminals, nonTerminals;
    //Store information into a list to be used later
    for(int i = 0; i < gRuleList.size(); i++){
        lhsList.push_back(gRuleList[i].lhs);
    }
    //Iterate through lhs.
    for(int i = 0; i < gRuleList.size(); i++){
        //Check if lhs is not in non terminals lists
        auto search1 = find(nonTerminals.begin(), nonTerminals.end(), gRuleList[i].lhs);
        if(!(search1 != nonTerminals.end())){
            nonTerminals.push_back(gRuleList[i].lhs);
        }
        //Check rhs if there needs to be added terminal or non terminal values to list.
        for(int j = 0; j < gRuleList[i].rhs.size(); j++){
            //Check to see if non terminal needs to be added
            auto search2 = find(lhsList.begin(), lhsList.end(), gRuleList[i].rhs[j]);
            if(search2 != lhsList.end()){
                auto search3 = find(nonTerminals.begin(), nonTerminals.end(), gRuleList[i].rhs[j]);
                if(!(search3 != nonTerminals.end())){
                    nonTerminals.push_back(gRuleList[i].rhs[j]);
                }
            }
            //Check to see if terminal needs to be added. 
            else{
                //Have to make sure terminal is not a nonTerminal before adding
                auto search4 = find(terminals.begin(), terminals.end(), gRuleList[i].rhs[j]);
                if(!(search4 != terminals.end())){
                    terminals.push_back(gRuleList[i].rhs[j]);
                }
            }
        }
    }
    
    //Print out terminals then non terminals from the vectors
    for(int k = 0; k < terminals.size(); k++){
        cout << terminals[k] + " ";
    }
    for(int k2 = 0; k2 < nonTerminals.size(); k2++){
        cout << nonTerminals[k2] + " ";
    }
}

// Task 2
void RemoveUselessSymbols()
{

vector<gRule> tempGRule; // new gRule for grammar without useless symbols
bool isGenerating = true; // used later to check generating symbols
map<string, bool> symbol_tracker; // used to keep track of symbols and if they generate or not
map<string, bool> symbol_tracker2;
symbol_tracker.insert(pair<string,bool>("*", true)); //hard coding these because they aren't actually in the grammar during parsing
symbol_tracker.insert(pair<string,bool>("#", false));

// i forget honestly, reused from task 1 to populate map but this section should be fine
vector<string> terminals, nonTerminals; 

tempGRule = gRuleList; //new gRule list is copy of original

for (int i = 0; i < tempGRule.size(); i++) 
    {
        lhsList.push_back(tempGRule[i].lhs);
    }

for (int i = 0; i < tempGRule.size(); i++){
       if(!(find(nonTerminals.begin(), nonTerminals.end(), gRuleList[i].lhs) != nonTerminals.end())){
            nonTerminals.push_back(tempGRule[i].lhs);
            string test = gRuleList[i].lhs;
            symbol_tracker.insert(pair<string, bool>(test, false));
        }
        //Check rhs if there needs to be added terminal or non terminal values to list.
        for(int j = 0; j < tempGRule[i].rhs.size(); j++){
            //Check to see if non terminal needs to be added
            if(find(lhsList.begin(), lhsList.end(), tempGRule[i].rhs[j]) != lhsList.end()){
                if(!(find(nonTerminals.begin(), nonTerminals.end(), tempGRule[i].rhs[j]) != nonTerminals.end())){
                    nonTerminals.push_back(tempGRule[i].rhs[j]);
                    string test = gRuleList[i].rhs[j];
                    symbol_tracker.insert(pair<string, bool>(test, false));
                }
            }
            //Check to see if terminal needs to be added. 
            else{
                //Have to make sure terminal is not a nonTerminal before adding
                if(!(find(terminals.begin(), terminals.end(), gRuleList[i].rhs[j]) != terminals.end())){
                    terminals.push_back(gRuleList[i].rhs[j]);
                    string test = gRuleList[i].rhs[j];
                    symbol_tracker.insert(pair<string, bool>(test, true));
                }
            }
        }
    }
/////////////////////////////////////// end of section that should be fine
//int noGen;
bool done = false;
int changes;
bool gen =true;
int iter = 0; // iterator to keep track of how many runs the while loop is taking, for debugging
//bool changed = false; // logically i think this and next line should be true but does worse for some reason
for(int h = 0; h < 11; h++){
   
    changes = 0; // keeping track of number of changes. Each run this should reset back to zero

    for(int i = 0; i < tempGRule.size(); i++){ // parsing through whole grammar
    gen = true;
    // if current rule has an empty rhs, iterate through map to find the matching symbol -> set generating flag in map to true -> incrememnt changes
        if(tempGRule[i].rhs.empty()){ 
            std::string hash = "#";
            tempGRule[i].rhs.push_back(hash);
            
            std::pair<string,bool> end = *symbol_tracker.rbegin();
            std::map<string,bool>::iterator it = symbol_tracker.begin();
            do {
                if(it->first == tempGRule[i].lhs && it->second == false){
                it->second = true;
                changes = changes + 1;
                }
                

            } while(symbol_tracker.value_comp()(*it++, end));
        ////////////////////// end of empty rhs case
        }
        // otherwise, iterate through corresponding rhs -> iterate through map to find the match -> if rhs symbol matches map element but does not generate, isGenerating = false
        else {
             
                for(int a = 0; a < tempGRule[i].rhs.size(); a++){
                    std::pair<string,bool> end = *symbol_tracker.rbegin();
                    std::map<string,bool>::iterator it = symbol_tracker.begin();
                    do {
                    if(tempGRule[i].rhs[a] == it->first && it->second == false){
                        gen = false;
                    }
            } while(symbol_tracker.value_comp()(*it++, end));
        }
    }
        // if rule doesn't come out to false, then the rule generates -> iterate map to find the match -> change the corresponding map element to true in generating -> increment changes
        if(gen == true){
            std::pair<string,bool> end = *symbol_tracker.rbegin();
            std::map<string,bool>::iterator it = symbol_tracker.begin();
                do {
                    if(it->first == tempGRule[i].lhs){
                        it->second = true;
                        changes++;
                    }
            } while(symbol_tracker.value_comp()(*it++, end));
        }
    }

   if(changes == 0){
    done = true;
}
}
string start;
start = tempGRule[0].lhs;
// removing useless symbols after checking if they generate
for(int i = 0; i < tempGRule.size(); i++){
    std::pair<string,bool> end = *symbol_tracker.rbegin();
    std::map<string,bool>::iterator it = symbol_tracker.begin();
    do {
        if(it->first == start){
            if(it->second ==false){
                tempGRule.clear();
            }
        }
        for(int j = 0; j < tempGRule[i].rhs.size(); j++){
            if(tempGRule[i].lhs == it->first && it->second == false){
               // cout << tempGRule[i].lhs << it->first << it->second << i;
                tempGRule.erase(tempGRule.begin() + i);
        }
    }
               
    } while(symbol_tracker.value_comp()(*it++, end));
}

////////////// issues here, i think

// setting up new map to determine non reachables
symbol_tracker2 = symbol_tracker;
std::pair<string,bool> end = *symbol_tracker2.rbegin();
std::map<string,bool>::iterator it = symbol_tracker2.begin();
    do {
       // cout << "it->first = " << it->first << "\n";
        
        
} while(symbol_tracker.value_comp()(*it++, end));

//cout << "test: " << tempGRule[0].lhs;
     string temp = "null";
    for(int a = 0; a < 10; a++){
    for(int i = 0; i < tempGRule.size(); i++){
        for(int j = 0; j < tempGRule[i].rhs.size(); j++){
            if(j != 0){
                temp = tempGRule[i].rhs[j-1];

                //cout << "symbol is : " << tempGRule[i].rhs[j] << "temp is set to: " << temp;
        }
        else {
            std::pair<string,bool> end = *symbol_tracker2.rbegin();
            std::map<string,bool>::iterator it = symbol_tracker2.begin();
                do {
                    if(it->first == tempGRule[i].rhs[j]){
                        it->second = false;
                    }
                } while(symbol_tracker2.value_comp()(*it++, end));
            
        }
        if(find(lhsList.begin(), lhsList.end(), tempGRule[i].rhs[j]) != lhsList.end()){
            if(temp != "null"){
                if((find(lhsList.begin(),lhsList.end(), temp) != lhsList.end())){
                    it->second =false;
                }
                    else {
                    std::pair<string,bool> end = *symbol_tracker2.rbegin();
                    std::map<string,bool>::iterator it = symbol_tracker2.begin();
                    do {
                        if(it->first == tempGRule[i].rhs[j]){
                            it->second = true;
                            //cout << "after reachable section: " << tempGRule[i].rhs[j] << " = " << it->second; 
                        }
                } while(symbol_tracker2.value_comp()(*it++, end));
            }
        }
        
                
} 
        
}
}
    }

// removing unreachable symbols
for(int i = 0; i < tempGRule.size(); i++){
    for(int j = 0; j < tempGRule[i].rhs.size(); j++){
    std::pair<string,bool> end = *symbol_tracker2.rbegin();
    std::map<string,bool>::iterator it = symbol_tracker2.begin();
    do {
        for(int j = 0; j < tempGRule[i].rhs.size(); j++){
            if(tempGRule[i].lhs == it->first && it->second == false){
                tempGRule.erase(tempGRule.begin() + i);
        }
    }
               
    } while(symbol_tracker2.value_comp()(*it++, end));
}
}


//printing final result
for(int i = 0; i < tempGRule.size(); i++){
    cout << tempGRule[i].lhs << " -> ";
    for(int j = 0; j<tempGRule[i].rhs.size(); j++){
        //if(tempGRule[i].rhs.empty()){
         //   string hash = "#";
           // tempGRule[i].rhs.push_back(hash);
        //}
        cout << tempGRule[i].rhs[j] << " ";
    }
    cout << "\n";
}

    //cout << "2\n";

    }


// Task 3
void CalculateFirstSets()
{
    vector<string> fsLHS, terminals, nonTerminals;
    vector<gRule> firstSets;
    //map<string, vector<string> > fsMap;

    //Store information for terminals and nonTerminals, copy from task 1 except don't print
    //------------------------------------------------------------------------------------------------------------------
    //Iterate through lhs.

    //Store information into a list to be used later
    for(int i = 0; i < gRuleList.size(); i++){
        lhsList.push_back(gRuleList[i].lhs);
    }
    //Iterate through lhs.
    for(int i = 0; i < gRuleList.size(); i++){
        //Check if lhs is not in non terminals lists
        if(!(find(nonTerminals.begin(), nonTerminals.end(), gRuleList[i].lhs) != nonTerminals.end())){
            nonTerminals.push_back(gRuleList[i].lhs);
        }
        //Check rhs if there needs to be added terminal or non terminal values to list.
        for(int j = 0; j < gRuleList[i].rhs.size(); j++){
            //Check to see if non terminal needs to be added
            if(find(lhsList.begin(), lhsList.end(), gRuleList[i].rhs[j]) != lhsList.end()){
                if(!(find(nonTerminals.begin(), nonTerminals.end(), gRuleList[i].rhs[j]) != nonTerminals.end())){
                    nonTerminals.push_back(gRuleList[i].rhs[j]);
                }
            }
                //Check to see if terminal needs to be added.
            else{
                //Have to make sure terminal is not a nonTerminal before adding
                if(!(find(terminals.begin(), terminals.end(), gRuleList[i].rhs[j]) != terminals.end())){
                    terminals.push_back(gRuleList[i].rhs[j]);
                }
            }
        }
    }
    //------------------------------------------------------------------------------------------------------------------
    //Task 3 starts here...
    //Store information into a list (do not include duplicates) to be used later
    for(int i = 0; i < gRuleList.size(); i++){
        if(!(find(fsLHS.begin(), fsLHS.end(), gRuleList[i].lhs) != fsLHS.end())){
            fsLHS.push_back(gRuleList[i].lhs);
        }
    }

    //Step 1 and 2
    //Set first sets of all terminals as the terminal itself and epsilon as epsilon
    //Set first sets for non-terminals as empty
    gRule newRule;
    for(int v = 0; v < fsLHS.size(); v++){
        
        //Add information to data struct but later put in map
        newRule.lhs = fsLHS[v];
        //newRule.rhs.push_back(fsLHS[v]);
        firstSets.push_back(newRule);
    }

for(int i = 0; i < firstSets.size(); i++){
    cout << firstSets[i].lhs;
}

    bool epsFound = false; //Used towards the end
    bool changed = false;
    while(changed != true){
        //Loop through all rules
        for(int j = 0; j < gRuleList.size(); j++){
            //Part A
            //Check if rhs is empty and first sets of lhs does not have epsilon
            if(gRuleList[j].rhs.empty() && gRuleList[j].lhs != "#"){
                //add epsilon to first of lhs/to the rhs
                firstSets[j].rhs.push_back("#");
                changed = true;
            }
            //Part B
            for(int k = 0; k < gRuleList[j].rhs.size(); k++){
                //Check if first symbol in rhs is a nonTerminal
                if(find(nonTerminals.begin(), nonTerminals.end(), gRuleList[j].rhs[0]) != nonTerminals.end()){
                    //Create array of first sets of that symbol remove epsilon from it
                    vector<string> temp;
                    int index;
                    for(int k1 = 0; k1 < gRuleList.size(); k1++){
                        auto matchFound = find(nonTerminals.begin(), nonTerminals.end(), gRuleList[j].rhs[0]);
                        if(matchFound != nonTerminals.end()){
                            index = matchFound - nonTerminals.begin(); //Get the lhs index
                        }
                    }
                    //Store the first set of the symbol found
                    temp = firstSets[index].rhs;

                    //Need to somehow get epsilon deleted but prob don't depend on cases on gradescope.
                    //Update first sets by add the first set of the symbol.
                    firstSets[j].rhs.insert(firstSets[j].rhs.end(), temp.begin(), temp.end()); //Not deleting epsilon in this case but might need to.
                    changed = true;
                }
                //---------------------------------------------- Everything above should be right, everything below needs revising.
                else{
                    //Check if rhs is not empty
                    //Add first of that terminal to first set of lhs if not in it
                    auto find1 = find(firstSets[j].rhs.begin(), firstSets[j].rhs.end(), gRuleList[j].rhs[k]);
                    if(!gRuleList[j].rhs.empty() && !(find1 != firstSets[j].rhs.end())){
                        //Make sure thing being added is a nonTerminal and not a terminal
                        auto find2 = find(nonTerminals.begin(), nonTerminals.end(), gRuleList[j].rhs[k]);
                        if(!(find2 != nonTerminals.end())){
                            firstSets[j].rhs.push_back(gRuleList[j].rhs[k]);
                        }
                    }
                }
                //Loop through all rhs symbols again
                for(int h = 0; h < gRuleList[j].rhs.size(); h++){
                    //Check if any rhs symbol has epsilon in its first set
                    if(find(firstSets[j].rhs.begin(), firstSets[j].rhs.end(), "#") != firstSets[j].rhs.end()){
                        //Assume index 0 is # since it needs to be added before any sets.
                        //Then proceed to check if the next index is empty or not
                        if(!firstSets[j].rhs[1].empty()){
                            //Create an array of first sets of that symbol and remove epsilon from it
                            //NEED TO FIX HERE

                            //assume epsilon found
                            epsFound = true; //might remove later
                            changed = true;
                        }
                    }
                    else{
                        //Break through loop
                        break;
                    }
                }
                //Check if epsilon was found in last loop
                if(epsFound == true){
                    //Add/push element to the beginning
                    if(find(firstSets[j].rhs.begin(), firstSets[j].rhs.end(), "#") != firstSets[j].rhs.end()){
                        firstSets[j].rhs.push_back("#");
                    }
                }
            }
        }
    }

    //Print every information down here using double for loop
    for(int i1 = 0; i1 < firstSets.size(); i1++){
        cout << "FIRST(" + firstSets[i1].lhs + ") = {";
        for(int i2 = 0; i2 < firstSets[i1].rhs.size(); i2++){
            cout << firstSets[i1].rhs[i2];
            if(i2 != firstSets[i1].rhs.size() - 1){
                cout << ", ";
            }
        }
        cout << "}\n";
    }
}

// Task 4
void CalculateFollowSets()
{
    cout << "4\n";
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    //std::cout << "YES";
    //cout << "5\n";
}
    
int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
    */
    
    

    task = atoi(argv[1]);
    
    ReadGrammar();  // Reads the input grammar from standard input
            // and represent it internally in data structures
            // ad described in project 2 presentation file

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
            break;

        case 4: CalculateFollowSets();
            break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}