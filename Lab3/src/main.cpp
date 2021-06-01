#include <iostream>
#include <iomanip>
#include <string>
#include <regex>
#include <vector>
#include <set>
#include <cmath>

// =======================================================================================================
//                                        I copied this from my Lab 2
// =======================================================================================================
// Just to make table printing syntax understandable
template<typename T>
struct Center
{
	size_t width;
	T value;

	Center(size_t width, const T& value) : width(width), value(value) {}

	friend std::ostream& operator<<(std::ostream& ostr, const Center& center)
	{
		size_t leftWidth = (center.width + 1) / 2;
		size_t rightWidth = center.width - leftWidth;
		return ostr << std::setw(leftWidth) << std::right << center.value << std::setw(rightWidth) << "";
	}
};
// =======================================================================================================

struct Variable
{
	Variable() = default;
	Variable(int id, bool negated) : id(id), negated(negated) {}

	int id = 0;
	bool negated = false;
};

struct Clause
{
	// Yes, this is overkill for what it's doing, but it made accessing it easier.
	std::vector<Variable> variables;
};

void SplitString(const std::string& source, const std::string& delimeter, std::vector<std::string>& tokens)
{
	for (size_t splitIndex = 0, searchIndex = 0; splitIndex != std::string::npos; searchIndex = splitIndex + delimeter.size())
	{
		splitIndex = source.find(delimeter, searchIndex);
		tokens.emplace_back(source.substr(searchIndex, splitIndex - searchIndex));
	}
}

bool Input(std::vector<std::string>& clauses)
{
	// Get the formula from the user
	std::cout << "Enter the 3CNF formula:\n";
	std::string rawFormula;
	std::getline(std::cin, rawFormula);

	// Split the formula to get the clauses
	SplitString(rawFormula, " ^ ", clauses);

	// Check the validity of the clauses
	static const std::regex clauseRegex("-?\\d+v-?\\d+v-?\\d+"); // supports multi-digit numbers

	for (const std::string& clause : clauses)
		if (!std::regex_match(clause, clauseRegex))
			return false;

	return true;
}

Clause ParseClause(const std::string& clauseSource)
{
	Clause clause;

	std::vector<std::string> variables;
	SplitString(clauseSource, "v", variables);
	
	for (const std::string& variableSource : variables)
		clause.variables.emplace_back(abs(stoi(variableSource)), variableSource[0] == '-');

	return clause;
}

void DisplayClauses(const std::vector<std::string>& clauseSources, std::set<int>& variableIDs, std::vector<Clause>& clauses)
{
	for (const std::string& clauseSource : clauseSources)
	{
		Clause& clause = clauses.emplace_back(ParseClause(clauseSource));
		std::cout << "\nClause \"" << clauseSource << "\":\n";

		for (const Variable& variable : clause.variables)
		{
			std::cout << "Variable: " << (variable.negated ? "NOT " : "") << variable.id << '\n';
			variableIDs.insert(variable.id);
		}
	}
}

bool GetVariable(size_t variables, size_t variableIndex, size_t variableCount)
{
	return variables & (1ull << (variableCount - 1ull - variableIndex));
}

// Yes, this is unoptimized; I might optimize it later.
bool Satisfied(size_t variables, const std::set<int>& variableIDs, const std::vector<Clause>& clauses)
{
	for (const Clause& clause : clauses)
	{
		bool clauseSatisfied = false;

		for (const Variable& variable : clause.variables)
		{
			size_t index = 0;
			for (auto it = variableIDs.begin(); it != variableIDs.end(); ++it, index++)
			{
				if (*it == variable.id)
				{
					// != acts as logical XOR
					clauseSatisfied = variable.negated != GetVariable(variables, index, variableIDs.size());
					break;
				}
			}

			if (clauseSatisfied)
				break;
		}

		if (!clauseSatisfied)
			return false;
	}

	return true;
}

void DisplayTruthTable(const std::set<int>& variableIDs, const std::vector<Clause>& clauses)
{
	// Calculate truth table width
	size_t width = 5; // Left and right bounds account for 5 characters
	std::string horizontalSeparator;
	std::vector<int> columnWidths;
	columnWidths.reserve(variableIDs.size());

	for (int id : variableIDs)
	{
		int columnWidth = (int)ceil(log10(id + (id < 1) + (id < 2))) + 2;
		columnWidths.push_back(columnWidth);
		width += columnWidth;

		std::string temp(columnWidth + 1ull, '-');
		temp[0] = '+';
		horizontalSeparator += temp;
	}

	horizontalSeparator += "+\n";

	// Print the truth table
	std::cout << '\n' << horizontalSeparator << "| ";
	for (auto it = variableIDs.begin(); it != variableIDs.end(); ++it)
		std::cout << *it << " | ";
	std::cout << '\n' << horizontalSeparator;

	for (size_t i = 1ull << variableIDs.size(); i != 0;)
	{
		i--; // Quite intentional

		std::cout << '|';
		for (size_t j = 0; j < variableIDs.size(); j++)
			std::cout << Center(columnWidths[j], GetVariable(i, j, variableIDs.size()) ? 'T' : 'F') << '|';

		if (!Satisfied(i, variableIDs, clauses))
			std::cout << " Not";
		std::cout << " Satisfied\n";
	}

	std::cout << horizontalSeparator;
}

// I have tested this with the following examples:
// 1v-2v3 ^ -2v1v-1
// 1v2v-1 ^ 1v2v-2 ^ -2v1v-1
// 1v2v-4 ^ -4v2v3 ^ -3v1v2 ^ -1v4v5
int main()
{
	std::vector<std::string> clauseSources;
	std::set<int> variableIDs;
	std::vector<Clause> clauses;

	if (Input(clauseSources))
	{
		DisplayClauses(clauseSources, variableIDs, clauses);

		if (variableIDs.size() < 8 * sizeof(size_t))
			DisplayTruthTable(variableIDs, clauses);
		else
			std::cout << "Too many variables.\n";
	}
	else
		std::cout << "Invalid formula. Make sure there is no leading nor trailing whitespace.\n";

	// I think this should work.
#if _WIN64
		std::cin.get();
#endif
	return 0;
}
