// For every assignment, unless an alternative is required, I will be using Visual Studio 2019 and C++17.

#include <iostream>
#include <string>
#include <fstream>
#include <set>

int main()
{
	std::cout << "Enter the filepath: ";
	
	// Get the filepath
	std::string filepath;
	std::getline(std::cin, filepath);

	// Try to open the file
	std::ifstream file(filepath);
	if (file.is_open())
	{
		std::string currentToken;
		std::set<std::string> tokens;

		while (file)
		{
			// Get the token
			file >> currentToken;
			tokens.insert(currentToken);
		}

		file.close();


		std::cout << "\nTokens in the file:\n";

		// Print the tokens
		for (const auto& token : tokens)
			std::cout << token << '\n';
	}
	else
		std::cerr << "Unable to open file.\n";

	//std::cin.get();
	return 0;
}
