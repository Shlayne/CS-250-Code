// Bruce Cosgrove

// In this lab, I'm trying a different approach to text output.
// While it takes up memory, I think it's easier to understand.
// I'm simply creating a buffer that I write to, then the entire
// buffer is written to std::cout.

#include <iostream>
#include <string>

class Buffer
{
public:
	Buffer(int width, int height)
		: width(width), height(height)
	{
		buffer = new char[(width + 1) * height];

		Clear(' ');
		for (int i = 0; i < height - 1; i++)
			buffer[(i + 1) * (width + 1) - 1] = '\n';
		buffer[(width + 1) * height - 1] = 0;
	}
	~Buffer()
	{
		delete[] buffer;
	}
public:
	void Clear(char c)
	{
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				buffer[i * (width + 1) + j] = ' ';
	}
	void PutChar(int x, int y, char c)
	{
		// Not doing bounds checks to keep it simple
		buffer[y * (width + 1) + x] = c;
	}
	friend std::ostream& operator<<(std::ostream& ostr, const Buffer& buffer);
private:
	int width;
	int height;
	char* buffer;
};

std::ostream& operator<<(std::ostream& ostr, const Buffer& buffer)
{
	return ostr << buffer.buffer;
}

void MoveDisk(int* towerDisks[3], int disks, int from, int to)
{
	for (int i = 0; i < disks; i++)
	{
		if (int& disk = towerDisks[from][i]; disk)
		{
			for (int j = disks - 1; j >= 0; j--)
			{
				if (!towerDisks[to][j])
				{
					towerDisks[to][j] = disk;
					disk = 0;
					return;
				}
			}
		}
	}
}

void Draw(Buffer& buffer, int* towerDisks[3], int towerPositions[3], int disks, char* separator)
{
	buffer.Clear(' ');
	for (int i = 0; i < disks; i++)
		for (int j = 0; j < 3; j++)
			buffer.PutChar(towerPositions[j], i, '|');

	for (int i = 0; i < 3; i++)
		for (int j = disks - 1; j >= 0 && towerDisks[i][j]; j--)
			for (int k = 1 - towerDisks[i][j]; k < towerDisks[i][j]; k++)
				buffer.PutChar(towerPositions[i] + k, j, '-');

	std::cout << buffer << separator;
};

int GetOtherTower(int tower1, int tower2, int left)
{
	// This is for moving disk 0
	if (tower1 == tower2)
		return (tower1 + 1 + left) % 3;

	// For moving any other disk, move
	// it to the only available spot
	switch (tower1)
	{
		case 0: return tower2 == 1 ? 2 : 1;
		case 1: return tower2 == 2 ? 0 : 2;
		case 2: return tower2 == 0 ? 1 : 0;
	}

	return 0; // Shouldn't happen
}

// Here are the actual algorithms.
void IterativeHanoi(Buffer& buffer, int* towerDisks[3], int towerPositions[3], int disks, char* separator)
{
	int* diskPositions = new int[disks]();

	// This is to make the disks always end in the right-most tower
	int left = disks & 1;

	int moves = (1 << disks) - 1;
	for (int i = 0; i < moves; i++)
	{
		int next = i + 1;
		int move = i ^ next;

		long disk = 0;
		while (move != 1)
		{
			move >>= 1;
			disk++;
		}

		int& disk0Tower = diskPositions[0];
		int& diskTower = diskPositions[disk];
		int nextTower = GetOtherTower(disk0Tower, diskTower, left);
		MoveDisk(towerDisks, disks, diskTower, nextTower);
		diskTower = nextTower;

		Draw(buffer, towerDisks, towerPositions, disks, separator);
	}

	delete[] diskPositions;
}

void RecursiveHanoi(Buffer& buffer, int* towerDisks[3], int towerPositions[3], int disks, char* separator, int currentDisks, int from, int to)
{
	if (currentDisks)
	{
		int otherTower = GetOtherTower(from, to, disks & 1);
		RecursiveHanoi(buffer, towerDisks, towerPositions, disks, separator, currentDisks - 1, from, otherTower);
		MoveDisk(towerDisks, disks, from, to);
		Draw(buffer, towerDisks, towerPositions, disks, separator);
		RecursiveHanoi(buffer, towerDisks, towerPositions, disks, separator, currentDisks - 1, otherTower, to);
	}
}

int main()
{
	int disks = 1;

	std::cout << "Enter the number of disks: ";
	std::cin >> disks;
	std::cin.get();

	// The 32 is completely dependent on me using an int.
	// The max would be 63 if using long longs. Regardless,
	// even without displaying the result, it would still
	// take a couple of minutes for a 31-disk TOH puzzle to
	// be solved, so I think that's plenty.
	if (disks > 0 && disks < 32)
	{
		// Initialize the towers
		int towerPositions[3] {disks - 1, 3 * disks - 1, 5 * disks - 1};
		int* towerDisks[3];
		// I learned today that adding the "()" to the end of a new[]
		// operation for primitive types initializes each them to zero.
		for (int i = 0; i < 3; i++)
			towerDisks[i] = new int[disks]();
		for (int i = 0; i < disks; i++)
			towerDisks[0][i] = i + 1;

		// Initialize drawing capabilities
		Buffer buffer(6 * disks - 1, disks);
		char* separator = new char[6 * disks + 4];
		for (int i = 0; i < 6 * disks; i++)
			separator[i + 2] = '-';
		separator[0] = '\n';
		separator[1] = '\n';
		separator[6 * disks + 1] = '\n';
		separator[6 * disks + 2] = '\n';
		separator[6 * disks + 3] = 0;

		// Get solving method
		std::string iterative;
		std::cout << "Type 0 for an recursive solution or anything else for a iterative solution: ";
		std::getline(std::cin, iterative);
		std::cout << '\n';

		// Draw the initial board
		Draw(buffer, towerDisks, towerPositions, disks, separator);

		// Actually solve the problem
		if (iterative.size() == 1 && iterative[0] == '0')
			IterativeHanoi(buffer, towerDisks, towerPositions, disks, separator);
		else
			RecursiveHanoi(buffer, towerDisks, towerPositions, disks, separator, disks, 0, 2);

		// No memory leaks, please and thank you.
		for (int i = 0; i < 3; i++)
			delete[] towerDisks[i];
		delete[] separator;
	}
	else
		std::cout << "Please use at least one and at most 31 disks.\n";

#if _WIN64
	std::cin.get();
#endif
	return 0;
}
