#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <bit>

using namespace std;

mutex mtx;

int zeroesCounter = 0;
int onesCounter = 0;

list <unsigned char> listToCheck;
list <unsigned char> bufferList;

/// <summary>
///     Prints list values to the console
/// </summary>
/// <param name="listToPrint"> List of integers to print </param>
void PrintList(list<unsigned char> listToPrint)
{
    for (auto it = listToPrint.begin(); it != listToPrint.end(); it++)
    {
        cout << (int)(*it) << " ";
    }

    cout << endl;
}

/// <summary>
///     Counts passed items and zeroes or ones in given list 
/// </summary>
/// <param name="ivertedList"> Iterating the list from the beginning </param>
/// <param name="itemsReachedCounter"> Count of processed items </param>
void ListChecker(bool invertedList, int &itemsPassedCounter)
{
    if (invertedList)
    {
        //  While there are any items is list -> lock mutex -> count item -> 
        //  -> remove item -> unlock mutex -> increase passed items counter
        while (true)
        {
            unique_lock<mutex> uniqueMtx(mtx);

            if (bufferList.empty())
            {
                return;
            }

            auto currentItem = bufferList.front();
            
            //  This also counts zeros in insignificant digits
            zeroesCounter += 8 * sizeof(unsigned char) - __popcnt(bufferList.front());

            bufferList.pop_front();
            
            uniqueMtx.unlock();

            itemsPassedCounter++;
        }
    }
    else
    {
        while (true)
        {
            unique_lock<mutex> uniqueMtx(mtx);

            if (bufferList.empty())
            {
                return;
            }

            onesCounter += __popcnt(bufferList.back());

            bufferList.pop_back();

            uniqueMtx.unlock();

            itemsPassedCounter++;
        }
    }
}

/// <summary>
///     Initializes threads to process list
/// </summary>
/// <param name="firstCount"> Count of passed items for first thread </param>
/// <param name="secondCount"> Count of passed items for second thread </param>
void StartThreads(int& firstCount, int& secondCount)
{
    thread firstAdder(ListChecker, true, ref(firstCount));
    thread secondAdder(ListChecker, false, ref(secondCount));

    firstAdder.join();
    secondAdder.join();
}

/// <summary>
///     Generate list of random items
/// </summary>
/// <param name="itemsCount"> Count of items </param>
void GenerateList(int itemsCount)
{
    for (int i = 0; i < itemsCount; i++)
    {
        unsigned char item = 0;

        for (int j = 0; j < 8; j++)
        {
            item |= (rand() % 2) << j;
        }

        listToCheck.push_back(item);
    }
}

/// <summary>
///     Print results of work into the console
/// </summary>
/// <param name="firstCount"> First thread passed items count </param>
/// <param name="secondCount"> Second thread passed items count </param>
void PrintResults(int firstCount, int secondCount)
{
    cout << endl << "First thread reached items count = " << firstCount << endl;
    cout << "Second thread reached items count = " << secondCount << endl;

    cout << endl << "Zeroes found = " << zeroesCounter << endl;
    cout << "Ones found = " << onesCounter << endl;
}

int main()
{
    int firstThreadPassedItemsCount = 0;
    int secondThreadPassedItemsCount = 0;
    int allItemsCount;

    cout << "Set count of items to check: ";
    cin >> allItemsCount;
    cout << endl;

    GenerateList(allItemsCount);

    bufferList = listToCheck;

    cout << "Generated list is: ";
    PrintList(listToCheck);

    StartThreads(firstThreadPassedItemsCount, secondThreadPassedItemsCount);

    PrintResults(firstThreadPassedItemsCount, secondThreadPassedItemsCount);
}