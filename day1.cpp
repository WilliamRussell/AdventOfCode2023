#include <iostream>
#include <algorithm>
#include <fstream>

#include <vector>

int AsciiToInt(const char c)
{
	return static_cast<int>(c) - static_cast<int>('0');
}

bool FindDigitStringAndPos(const std::string& line, int& first, int& last, size_t& firstPos, size_t& lastPos)
{
	first = last = 0;
	firstPos = std::string::npos;
	lastPos = 0;

	const std::vector<std::string> digitStrings = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
	std::string revLine = line;
	std::reverse(revLine.begin(), revLine.end());

	for (size_t i = 0; i < digitStrings.size(); ++i)
	{
		std::string s = digitStrings[i];

		auto found = line.find(s);
		if (found != std::string::npos)
		{
			if (found < firstPos)
			{
				firstPos = found;
				first = i + 1;
			}
		}

		std::string revS = s;		
		std::reverse(revS.begin(), revS.end());

		found = revLine.find(revS);
		if (found != std::string::npos)
		{
			found = line.length() - found - revS.length();
			if (found >= lastPos)
			{
				lastPos = found;
				last = i + 1;
			}
		}
	}

	return firstPos != std::string::npos;
}


int ReadValue(const std::string &input)
{
	size_t firstDigitPos = input.find_first_of("0123456789");
	size_t lastDigitPos = input.find_last_of("0123456789");
	int first = AsciiToInt(input[firstDigitPos]);
	int last = AsciiToInt(input[lastDigitPos]);

	// Part 2
	size_t firstSDigitPos, lastSDigitPos;
	int firstSDigit, lastSDigit;
	if (FindDigitStringAndPos(input, firstSDigit, lastSDigit, firstSDigitPos, lastSDigitPos))
	{
		first = firstDigitPos < firstSDigitPos ? first : firstSDigit;
		last = lastDigitPos > lastSDigitPos ? last : lastSDigit;
	}
	// End of part 2

	int value = first * 10 + last;

	return value;
}

int main()
{
	std::ifstream inputStream("input.txt");

	long totalValue = 0;
	if (inputStream.is_open())
	{
		std::string line;	

		while (inputStream >> line)
		{
			int value = ReadValue(line);
			totalValue += value;
		}
		inputStream.close();

		std::cout << "Final value is: " << totalValue << std::endl;
	}
	else
	{
		std::cout << "Error opening file.\n";		
	}

	return 0;
}