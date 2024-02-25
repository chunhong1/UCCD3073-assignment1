#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Item 
{
    char name;
    int weight;
    int value;
};

struct Rule 
{
    string combinedItems;
    int extraWeight;
    int bonusValue;
};

//constant
const string FILENAME = "problem.txt";

/* This function reads the file and store the relevant data into containerSize, items and rules */
bool ReadFile(string filename, int& containerSize, vector<Item>& item, vector<Rule> rule) 
{

    ifstream in;
    in.open(filename);

    //return false if the file is not found
    if (!in)
        return false;

    char text[256];
    char name;
    int weight, value, extraWeight, bonusValue;
    string combination;

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
               
        //clear buffer as the Rules keyword will cause weight become 'u'
        in.clear();

        in >> text;

        //push the rules into a dynamic array
        while (in >> combination >> extraWeight >> bonusValue) 
        {
            rule.push_back({ combination, extraWeight, bonusValue });
        }  

        break;
    }

    in.close();
    return true;
}

/* This function takes the input specifications and 
 * return the combination of items that maximize the total value without exceeding the container capacity
 */
vector<Item> Knapsack(int containerSize, const vector<Item>& items, const vector<Rule>& rules) 
{
    int n = items.size();
    vector<vector<int>> dp(n + 1, vector<int>(containerSize + 1,0)); //2d matrix, size of (n + 1) x (containerSize + 1) and initialise to 0

    //knapsack algorithm
    for (int i = 1; i <= n; ++i) {
        for (int w = 0; w <= containerSize; ++w) {
            //assign weight and value of the current item
            int itemWeight = items[i - 1].weight;
            int itemValue = items[i - 1].value;

            //update the value by choosing the maximum value of not including the item or including the item 
            //if there is enough capacity
            dp[i][w] = dp[i - 1][w];

            if (w >= itemWeight) {
                dp[i][w] = max(dp[i][w], dp[i - 1][w - itemWeight] + itemValue);
            }
        }
    }

    vector<Item> selectedItems;
    int i = n;
    int w = containerSize;

    //select item
    while (i > 0 && w > 0) {
        if (dp[i][w] != dp[i - 1][w])
        {
            selectedItems.push_back(items[i - 1]);
            w -= items[i - 1].weight;
        }
        i--;
    }

    //reverse the order
    reverse(selectedItems.begin(), selectedItems.end());

    return selectedItems;
}

/* This function write the selected item into output.txt */
void WriteFile(vector<Item>& selectedItems)
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

    //return -1 if file is not found
    if (!ReadFile(FILENAME,containerSize,items,rules))
        return -1;

    vector<Item> selectedItems = Knapsack(containerSize, items, rules);
    
    WriteFile(selectedItems);

	return 0;
}

