#include <iostream>
#include <iomanip>

// Basic Operators
// Most of these are overkill and some aren't even used, but what the heck.
bool Not(bool a) { return !a; }
bool And(bool a, bool b) { return a && b; }
bool Nand(bool a, bool b) { return !And(a, b); }
bool Or(bool a, bool b) { return a || b; }
bool Nor(bool a, bool b) { return !Or(a, b); }
bool Xor(bool a, bool b) { return (a && !b) || (!a && b); }
bool Xnor(bool a, bool b) { return !Xor(a, b); }
bool IfThen(bool a, bool b) { return !a || b; }
bool IfAndOnlyIf(bool a, bool b) { return Xnor(a, b); }

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

bool GetPremise(bool p, bool q, bool r)
{
	// ((P v Q) ^ (Q -> R) XOR (P ^ R)) <-> (R ^ Q)
	return IfAndOnlyIf(Xor(And(Or(p, q), IfThen(q, r)), And(p, r)), And(r, q));
}

bool GetConclusion(bool p, bool q, bool r)
{
	// (P v R)
	return Or(p, r);
}

int main()
{
	// Calculate truth table
	unsigned char pBits = 0;
	unsigned char qBits = 0;
	unsigned char rBits = 0;
	unsigned char premiseBits = 0;
	unsigned char conclusionBits = 0;

	for (unsigned char i = 0, bit = 1; i < 8; i++, bit <<= 1)
	{
		bool p = !static_cast<bool>(i & 0b100);
		bool q = !static_cast<bool>(i & 0b10);
		bool r = !static_cast<bool>(i & 0b1);
		bool premise = GetPremise(p, q, r);
		bool conclusion = GetConclusion(p, q, r);

		if (p) pBits |= bit;
		if (q) qBits |= bit;
		if (r) rBits |= bit;
		if (premise) premiseBits |= bit;
		if (conclusion) conclusionBits |= bit;
	}

	// Display truth table
	static constexpr size_t pWidth = 3;
	static constexpr size_t qWidth = 3;
	static constexpr size_t rWidth = 3;
	static constexpr size_t premiseWidth = 46;
	static constexpr size_t conclusionWidth = 9;
	static constexpr size_t validityWidth = 10;

	static constexpr const char* separator = "+---+---+---+----------------------------------------------+---------+----------+\n";

	std::cout << separator
			  << "| P | Q | R | ((P v Q) ^ (Q -> R) XOR (P ^ R)) <-> (R ^ Q) | (P v R) | Validity |\n"
			  << separator;

	bool invalidArgument = false;
	for (unsigned char bit = 1; bit != 0; bit <<= 1)
	{
		bool p = static_cast<bool>(pBits & bit);
		bool q = static_cast<bool>(qBits & bit);
		bool r = static_cast<bool>(rBits & bit);
		bool premise = static_cast<bool>(premiseBits & bit);
		bool conclusion = static_cast<bool>(conclusionBits & bit);

		bool invalidRow = premise && !conclusion;
		invalidArgument |= invalidRow;

		std::cout << '|' << Center(pWidth, p) << '|'
				  << Center(qWidth, q) << '|'
				  << Center(rWidth, r) << '|'
				  << Center(premiseWidth, premise) << '|'
				  << Center(conclusionWidth, conclusion) << '|'
				  << std::setw(validityWidth) << std::left << (invalidRow ? " Invalid" : " Valid") << "|\n";
	}

	std::cout << separator;
	
	if (invalidArgument)
		std::cout << "The argument is invalid!\n";

	return 0;
}
