#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>

#include <cassert>

struct GridElement
{
	int xCoord;
	int yCoord;
	char symbol;
	int value;
	int size;
};

void ParseLine(const std::string& input, int yCoordinate, std::vector<GridElement>& lineElements)
{
	lineElements.clear();

	GridElement gridElement;
	gridElement.yCoord = yCoordinate;
	bool inNumber = false;

	auto endIfInNumber = [&]()
		{
			if (inNumber)
			{
				inNumber = false;
				lineElements.push_back(gridElement);
			}
		};

	for (int i = 0; i < input.length(); ++i)
	{
		char character = input[i];

		switch (character)
		{
		case '.':
		{
			endIfInNumber();
			break;
		}
		case '*':
		case '#':
		case '&':
		case '+':
		case '/':
		case '%':
		case '$':
		case '=':
		case '@':
		case '-':
		{
			endIfInNumber();
			gridElement.symbol = character;
			gridElement.xCoord = i;
			gridElement.value = -1;
			gridElement.size = 1;
			lineElements.push_back(gridElement);
			break;
		}
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			if (!inNumber)
			{
				gridElement.xCoord = i;
				gridElement.value = 0;
				gridElement.symbol = -1;
				gridElement.size = 0;
				inNumber = true;
			}
			
			gridElement.size += 1;
			gridElement.value = gridElement.value * 10 + character - '0';
			break;
		}
		default:
			assert(false);
		}
	}
	endIfInNumber();
}

struct CoordHasher
{
	std::size_t operator()(const std::pair<int, int>& coord) const
	{
		return std::hash<int>()(coord.first) ^ std::hash<int>()(coord.second);
	}
};

struct Grid
{
	int xDimension;
	int yDimension;

	std::vector<GridElement> gridElements;
	std::unordered_map<std::pair<int, int>, size_t, CoordHasher> gridCoordToIndex;

	bool InsertElement(const GridElement& element)
	{
		assert(element.xCoord + element.size < xDimension);
		assert(element.yCoord < yDimension);

		size_t index = gridElements.size();
		gridElements.push_back(element);

		for (int i = 0; i < element.size; ++i)
		{
			auto pair = std::pair<int, int>(element.xCoord + i, element.yCoord);
			
			assert(gridCoordToIndex.find(pair) == gridCoordToIndex.end());			
			gridCoordToIndex[pair] = index;
		}

		return true;
	}

	bool GetElementAtCoordinate(int xCoord, int yCoord, GridElement& element)
	{
		auto it = gridCoordToIndex.find({ xCoord, yCoord });
		if (it != gridCoordToIndex.end())
		{
			element = gridElements[it->second];
			return true;
		}
		return false;
	}

	int Task1()
	{
		int partNumSum = 0;

		auto hasNeighbourSymbol = [&](const GridElement& inputElement) 
			{
				for (int i = 0; i < inputElement.size; ++i)
				{
					for (int x = -1; x < 2; ++x)
					{
						for (int y = -1; y < 2; ++y)
						{
							GridElement neighbourElement;
							int xCoord = inputElement.xCoord + i + x;
							int yCoord = inputElement.yCoord + y;

							bool elementFound = GetElementAtCoordinate(xCoord, yCoord, neighbourElement);

							if (elementFound && !(inputElement.xCoord == neighbourElement.xCoord && inputElement.yCoord == neighbourElement.yCoord))
							{
								if (neighbourElement.symbol != -1)
								{
									return true;
								}
							}
						}
					}
				}
				return false;
			};

		for (const GridElement& element : gridElements)
		{
			if (element.value != -1)
			{
				if (hasNeighbourSymbol(element))
				{					
					partNumSum += element.value;
				}
			}
		}

		return partNumSum;
	}

	int Task2()
	{
		int gearRatioSum = 0;

		auto gearRatio = [&](const GridElement& inputElement) 
			{
				GridElement gearVal[2];
				int gearValsFound = 0;

				for (int x = -1; x < 2; ++x)
				{
					for (int y = -1; y < 2; ++y)
					{
						GridElement neighbourElement;
						int xCoord = inputElement.xCoord + x;
						int yCoord = inputElement.yCoord + y;

						bool elementFound = GetElementAtCoordinate(xCoord, yCoord, neighbourElement);
						if (elementFound && neighbourElement.symbol == -1)
						{
							if (gearValsFound == 0)
							{
								gearVal[0] = neighbourElement;
								gearValsFound++;
							}
							else if (gearValsFound == 1)
							{
								if (!(gearVal[0].xCoord == neighbourElement.xCoord && gearVal[0].yCoord == neighbourElement.yCoord))
								{
									gearVal[1] = neighbourElement;
									gearValsFound++;
								}
							}
							else
							{
								if (!(gearVal[0].xCoord == neighbourElement.xCoord && gearVal[0].yCoord == neighbourElement.yCoord) &&
									!(gearVal[1].xCoord == neighbourElement.xCoord && gearVal[1].yCoord == neighbourElement.yCoord))
								{
									return 0;
								}
							}
						}
					}
				}

				if (gearValsFound == 2)
				{
					return gearVal[0].value * gearVal[1].value;
				}
				return 0;
			};

		for (const GridElement& element : gridElements)
		{
			if (element.symbol == '*')
			{
				gearRatioSum += gearRatio(element);
			}
		}

		return gearRatioSum;
	}
};

int main()
{
	std::ifstream inputStream("input.txt");

	if (!inputStream.is_open())
	{
		std::cout << "Error opening file.\n";
		return 1;
	}

	Grid grid;

	int lineNum = 0;
	std::string line;
	std::vector<GridElement> lineElements;
	while (std::getline(inputStream, line))
	{
		if (lineNum == 0)
		{
			grid.xDimension = static_cast<int>(line.length()) + 1;
		}

		ParseLine(line, lineNum++, lineElements);		
		grid.yDimension = lineNum;		

		for (auto& element : lineElements)
		{
			grid.InsertElement(element);
		}
	}
	inputStream.close();

	std::cout << "Task 1: " << grid.Task1() << std::endl;
	std::cout << "Task 2: " << grid.Task2() << std::endl;
	return 0;
}