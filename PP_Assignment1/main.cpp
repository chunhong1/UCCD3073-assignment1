#include	<iostream>
#include	<cstdlib>
#include	<cstdio>
#include    <fstream>
#include    <string>
#include    <sstream>
#include    <iomanip>
//#include	"Item.h"
#include <vector>

using namespace std;

struct Item {
    char name;
    int weight;
    int value;
};

struct Rule {
    string combinedItems;
    int extraWeight;
    int bonusValue;
};
//constant
const string FILENAME = "problem.txt";

bool ReadFile(string filename, int& containerSize, vector<Item>& item, vector<Rule> rule) {

    ifstream in;

    char text[256];
    char name;
    int weight, value, extraWeight, bonusValue;
    string combination;
    //string line;

    in.open(filename);

    //return false if the file is not found
    if (!in)
        return false;

    
    while (!in.eof())
    {
        in >> text; //ignore "ContainerSize:"
        in >> containerSize;
        in >> text; //ignore "Items:"
        
        //push the items into a dynamic array
        while (in >> name >> weight >> value) {
            // Check for the "Rules" keyword
            if (name == 'R' && weight == 0 && value == 0)
                break;  

            item.push_back({ name, weight, value });
        }
        
        
        for (auto i : item) {
            cout << i.name << "\t" << i.weight << "\t" << i.value << endl; 
        }
        

        //clear buffer as the Rules keyword will cause weight become 'u'
        in.ignore(1000, '\n');
        in.clear();

        in >> text;

        while (in >> combination >> extraWeight >> bonusValue) 
        {
            rule.push_back({ combination, extraWeight, bonusValue });
        }

        
        for (auto i : rule) {
            cout << i.combinedItems << "\t" << i.extraWeight << "\t" << i.bonusValue << endl; 
        }
        

        break;
    }

    in.close();
    return true;
}

vector<Item> knapsack(int containerSize, const vector<Item>& items, const vector<Rule>& rules) {
    int n = items.size();
    vector<vector<int>> dp(n + 1, vector<int>(containerSize + 1,0)); //2d matrix, size of (n + 1) x (containerSize + 1)

    //knapsack algorithm
    for (int i = 1; i <= n; ++i) {
        for (int w = 0; w <= containerSize; ++w) {
            int itemWeight = items[i - 1].weight;
            int itemValue = items[i - 1].value;

            dp[i][w] = dp[i - 1][w];

            if (w >= itemWeight) {
                dp[i][w] = max(dp[i][w], dp[i - 1][w - itemWeight] + itemValue);
            }
        }
    }

    vector<Item> selectedItems;
    int i = n;
    int w = containerSize;

    while (i > 0 && w > 0) {
        if (dp[i][w] != dp[i - 1][w])
        {
            selectedItems.push_back(items[i - 1]);
            w -= items[i - 1].weight;
        }
        i--;
    }

    

    reverse(selectedItems.begin(), selectedItems.end());

    //check for same weight different value

    
    // Apply rules for combining items
    /*
    for (const Rule& rule : rules) {
        string combinedItemName = rule.combinedItems;
        auto it = find_if(selectedItems.begin(), selectedItems.end(),
            [combinedItemName](const Item& item) {
                return item.name == combinedItemName[0];
            });

        if (it != selectedItems.end()) {
            selectedItems.erase(it);
            selectedItems.push_back({ combinedItemName[0], rule.combinedWeight, rule.combinedValue });
            it = find_if(selectedItems.begin(), selectedItems.end(),
                [combinedItemName](const Item& item) {
                    return item.name == combinedItemName[1];
                });

            if (it != selectedItems.end()) {
                selectedItems.erase(it);
            }
        }
    }
    */
    return selectedItems;
}

void writeFile(vector<Item>& selectedItems)
{
    ofstream myfile;
    myfile.open("output.txt");
    for (int i = 0; i < selectedItems.size(); i++)
    {
        myfile << selectedItems[i].name <<"\n";
    }
    
    myfile.close();
}

int main() 
{
    int containerSize = 0;
    vector<Item> items;
    vector<Rule> rules;

    if (!ReadFile(FILENAME,containerSize,items,rules))
    {
        return -1;
    }


    vector<Item> selectedItems = knapsack(containerSize, items, rules);
    for (int i = 0; i < selectedItems.size(); i++) {
        cout << selectedItems[i].name << endl;
    }
    
    writeFile(selectedItems);
	return 0;
}

