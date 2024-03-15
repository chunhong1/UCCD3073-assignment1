#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>

using namespace std;

struct Item
{
    string name;
    int weight;
    int value;
    bool found;
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
bool ReadFile(string filename, int& containerSize, vector<Item>& item, vector<Rule>& rule, vector<Item>& item2)
{
    ifstream in;
    in.open(filename);

    //return false if the file is not found
    if (!in)
        return false;

    char text[256];
    string name;
    int weight, value, extraWeight, bonusValue;
    string combination;

    while (!in.eof())
    {
        in >> text; //ignore "ContainerSize:"
        in >> containerSize;
        in >> text; //ignore "Items:"

        //push the items into a dynamic array
        while (in >> name >> weight >> value) 
        {
            item.push_back({ name, weight, value });
            item2.push_back({ name, weight, value });
        }

        //clear buffer so that the program continue read next line
        in.clear();

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
vector<Item> Knapsack(int containerSize,  vector<Item>& items, const vector<Rule>& rules)
{
    int itemSize = items.size();

    //2d matrix, size of (n + 1) x (containerSize + 1) and initialise to 0
    vector<vector<int>> dp(itemSize + 1, vector<int>(containerSize + 1, 0)); 

    //knapsack algorithm
    for (int i = 1; i <= itemSize; ++i) 
    {
        for (int w = 0; w <= containerSize; ++w) 
        {
            //assign weight and value of the current item
            int itemWeight = items[i - 1].weight;
            int itemValue = items[i - 1].value;

            //check to include or exclude the item into the knapsack
            dp[i][w] = dp[i - 1][w];

            if (w >= itemWeight) 
            {
                dp[i][w] = max(dp[i][w], dp[i - 1][w - itemWeight] + itemValue);
            }
        }
    }

    vector<Item> selectedItems;
    int capacity = containerSize;

    //select item
    while (itemSize > 0 && capacity > 0) 
    {
        if (dp[itemSize][capacity] != dp[itemSize - 1][capacity])
        {
            //if selectedItems is empty, then straight select the item
            if (selectedItems.empty())
            {
                selectedItems.push_back(items[itemSize - 1]);
                capacity -= items[itemSize - 1].weight;
                items[itemSize - 1].found = true; //found is also act as selected
            }
            else 
            {
                //check the following selected item have common letter with the items in selectedItems
                //if found, then mark the item found as true
                for (int i = 0; i < selectedItems.size(); i++) 
                {
                    string selectName = selectedItems[i].name;

                    for (char c : items[itemSize - 1].name) 
                    {
                        if (selectName.find(c) != std::string::npos) 
                        {
                            items[itemSize - 1].found = true;
                            break;
                        }
                    }
                }

                //if the item does not have common letter or selected, then push into selectedItems
                if (!items[itemSize - 1].found) 
                {
                    selectedItems.push_back(items[itemSize - 1]);
                    capacity -= items[itemSize - 1].weight;
                    items[itemSize - 1].found = true;
                }
            }
        }
        itemSize--;
    }

    //perform second time knapsack for some special case where some of the items should be selected but are not selected
    vector<Item> leftItem;
    for (int i = 0; i < items.size(); i++) 
    {
        if (items[i].found == false) 
        {
            leftItem.push_back({ items[i].name, items[i].weight, items[i].value, items[i].found });
        }
    }

    int k = leftItem.size();

    if (k > 0) 
    {
        vector<vector<int>> dp2(leftItem.size() + 1, vector<int>(capacity + 1, 0)); //2d matrix, size of (n + 1) x (containerSize + 1) and initialise to 0

        //knapsack algorithm
        for (int i = 1; i <= k; ++i) 
        {
            for (int w = 0; w <= capacity; ++w) 
            {
                //assign weight and value of the current item
                int itemWeight = leftItem[i - 1].weight;
                int itemValue = leftItem[i - 1].value;

                //update the value by choosing the maximum value of not including the item or including the item 
                //if there is enough capacity
                dp2[i][w] = dp2[i - 1][w];

                if (w >= itemWeight) 
                {
                    dp2[i][w] = max(dp2[i][w], dp2[i - 1][w - itemWeight] + itemValue);
                }
            }
        }

        while (k > 0 && capacity > 0) 
        {
            if (dp2[k][capacity] != dp2[k - 1][capacity])
            {

                if (selectedItems.size() == 0)
                {
                    selectedItems.push_back(leftItem[k - 1]);
                    capacity -= leftItem[k - 1].weight;
                    leftItem[k - 1].found = true;
                }

                else 
                {
                    bool found = false;
                    for (int i = 0; i < selectedItems.size(); i++) 
                    {
                        string selectName = selectedItems[i].name;

                        for (char c : leftItem[k - 1].name) {
                            if (selectName.find(c) != std::string::npos) 
                            {
                                leftItem[k - 1].found = true;
                                found = true;
                                break;
                            }
                        }
                    }

                    if (!found) 
                    {
                        selectedItems.push_back(leftItem[k - 1]);
                        capacity -= leftItem[k - 1].weight;
                        leftItem[k - 1].found = 1;

                    }
                }
            }
            k--;
        }
    }

    return selectedItems;
}

/* This function is a normal 01 knapsack algorithm */
vector<Item> Knapsack2(int containerSize, vector<Item>& items, const vector<Rule>& rules)
{
    int itemSize = items.size();
    vector<vector<int>> dp(itemSize + 1, vector<int>(containerSize + 1, 0)); //2d matrix, size of (n + 1) x (containerSize + 1) and initialise to 0

    //knapsack algorithm
    for (int i = 1; i <= itemSize; ++i) 
    {
        for (int w = 0; w <= containerSize; ++w) 
        {
            //assign weight and value of the current item
            int itemWeight = items[i - 1].weight;
            int itemValue = items[i - 1].value;

            //update the value by choosing the maximum value of not including the item or including the item 
            //if there is enough capacity
            dp[i][w] = dp[i - 1][w];

            if (w >= itemWeight) 
            {
                dp[i][w] = max(dp[i][w], dp[i - 1][w - itemWeight] + itemValue);
            }
        }
    }

    vector<Item> selectedItems;
    int capacity = containerSize;

    //select item
    while (itemSize > 0 && capacity > 0) 
    {
        if (dp[itemSize][capacity] != dp[itemSize - 1][capacity])
        {           
            selectedItems.push_back(items[itemSize - 1]);
            capacity -= items[itemSize - 1].weight;

        }
        itemSize--;
    }
   
    return selectedItems;

}

/* This function write the selected item into output.txt */
void WriteFile(vector<Item>& selectedItems)
{
    ofstream myfile;
    myfile.open("output.txt");
    for (int i = 0; i < selectedItems.size(); i++)
    {
        myfile << selectedItems[i].name << "\n";
    }

    myfile.close();
}

/* This function calculate the total value of selected items and return it */
int CalculateTotalValue(vector<Item>& item)
{
    int value = 0;

    for (auto& it : item)
    {
        value += it.value;
    }

    return value;
}

int main()
{
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    int containerSize = 0;
    //items consist of items and rules while items2 consist of items only
    vector<Item> items,items2;
    vector<Rule> rules;
    int value1 = 0, value2 = 0;

    //return -1 if file is not found
    if (!ReadFile(FILENAME, containerSize, items, rules,items2))
        return -1;

    //push the rule as an object into items
    for (int i = 0; i < rules.size(); i++) 
    {
        int sumWeight = 0;
        int sumValue = 0;

        for (int j = 0; j < items.size(); j++) 
        {
            if (rules[i].combinedItems.find(items[j].name) != std::string::npos)
            {
                sumWeight += items[j].weight;
                sumValue += items[j].value;
            }

        }
        string name = rules[i].combinedItems;
        sumWeight += rules[i].extraWeight;
        sumValue += rules[i].bonusValue;

        items.push_back({ name, sumWeight, sumValue });
    }
    
    vector<Item> selectedItems = Knapsack(containerSize, items, rules);
    vector<Item> selectedItems2 = Knapsack2(containerSize, items2, rules);
    
    
    value1 = CalculateTotalValue(selectedItems);
    value2 = CalculateTotalValue(selectedItems2);

    //compare items or items2 has a larger profit and write into output.txt
    if (value2 >= value1)
    {
        WriteFile(selectedItems2);
    }
    else 
    {
        WriteFile(selectedItems);
    }
    

    return 0;
}