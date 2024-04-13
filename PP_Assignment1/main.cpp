#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <omp.h>

using namespace std;

struct Item {
    string name;
    int weight;
    int value;
    bool found;
};

struct Rule {
    string combinedItems;
    int extraWeight;
    int bonusValue;
};

const string FILENAME = "problem.txt";

bool ReadFile(string filename, int& containerSize, vector<Item>& item, vector<Rule>& rule)
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

// Convert rules into items
void ConvertRulesToItems(const vector<Rule>& rules, vector<Item>& items) {
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

}

// Generate all possible combinations of items
void GenerateItemCombinations(vector<vector<Item>>& combinations, const vector<Item>& items, int start, vector<Item>& combination) {
    if (!combination.empty()) {
        combinations.push_back(combination);
    }


    for (int i = start; i < items.size(); ++i) {
        combination.push_back(items[i]);
        GenerateItemCombinations(combinations, items, i + 1, combination);
        combination.pop_back();
    }
}

void knapsack_algorithm(int containerSize, const std::vector<Item>& items, std::vector<std::vector<int>>& dp) {
    // Parallel loop for each item
    int itemSize = items.size();
    for (int i = 1; i <= itemSize; ++i)
    {

        #pragma omp parallel 
        {
            //int thread_id = omp_get_thread_num();
            #pragma omp for
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
    }
}

void itemSelection(int itemSize, int& capacity, vector<Item>& items, vector<vector<int>>& dp, vector<Item>& selectedItems) {
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
}

/* This function takes the input specifications and
 * return the combination of items that maximize the total value without exceeding the container capacity
 */
vector<Item> Knapsack(int containerSize, vector<Item>& items)
{
    int itemSize = items.size();

    //2d matrix, size of (n + 1) x (containerSize + 1) and initialise to 0
    vector<vector<int>> dp(itemSize + 1, vector<int>(containerSize + 1, 0));

    //knapsack algorithm
    knapsack_algorithm(containerSize, items, dp);

    vector<Item> selectedItems;
    int capacity = containerSize;

    //select item
    itemSelection(itemSize, capacity, items, dp, selectedItems);

    return selectedItems;
}


// Write selected items to output file
void WriteFile(const vector<Item>& selectedItems) {
    ofstream myfile("output.txt");
    if (myfile.is_open()) {
        for (const auto& item : selectedItems) {
            myfile << item.name << "\n";
        }
        myfile.close();
    }
    else {
        cout << "Unable to open output file";
    }
}

int main() {
    clock_t start = clock();
    int containerSize = 0;
    vector<Item> items;
    vector<Rule> rules;

    if (!ReadFile(FILENAME, containerSize, items, rules)) {
        cout << "Error reading file";
        return -1;
    }

    ConvertRulesToItems(rules, items);

    vector<vector<Item>> itemCombinations;
    vector<Item> combination;
    GenerateItemCombinations(itemCombinations, items, 0, combination);

    //vector<vector<Item>> itemCombinations = GenerateItemCombinations(items);
    int maxValue = 0;
    vector<Item> bestItems;

    //cout << itemCombinations.size() << endl;
#pragma omp parallel for shared(maxValue, bestItems)
    for (int i = 0; i < itemCombinations.size(); ++i) {
        auto& combination = itemCombinations[i];
        vector<Item> selectedItems = Knapsack(containerSize, combination);
        int totalValue = 0;
        for (const auto& item : selectedItems) {
            totalValue += item.value;
        }
#pragma omp critical
        {
            if (totalValue > maxValue) {
                maxValue = totalValue;
                bestItems = selectedItems;
            }
        }
    }


    for (auto& i : bestItems) {
        cout << i.name << "\t" << i.value << "\t" << i.weight << endl;
    }
    WriteFile(bestItems);
    clock_t stop = clock();
   // printf(" %f \n", ((double)stop - start) / CLOCKS_PER_SEC);
    return 0;
}
