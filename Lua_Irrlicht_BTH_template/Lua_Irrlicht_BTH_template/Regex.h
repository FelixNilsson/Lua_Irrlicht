#pragma once

#include <string>
#include <vector>

class Regex
{
public:
	virtual int match(char const *) = 0;
};

class CharClass : public Regex
{
public:
	std::string m_contents;
public:
	CharClass(std::string c) : m_contents(c) {}

	int match(char const * text) override;
};

class Star : public Regex
{
private:
	Regex * m_operand = nullptr;
public:
	Star(Regex* p) : m_operand(p) {}

	int match(char const * text);
};

class Seq : public Regex
{
private:
	std::vector<Regex*> m_cells;
public:
	Seq(std::vector<Regex*> v) : m_cells(v) {}

	int match(char const * text);
};