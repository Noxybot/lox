export module lexer;

import core;
import <vector>;
import <string>;

export class Lexer
{
	std::string m_text;
	int m_curr_pos = 0;
	int m_start_pos = 0;
	int m_line = 1;
	std::vector<Token> m_tokens;
public:
	explicit Lexer(std::string text);
	std::vector<Token> GetTokens();
private:
	void AddNextToken();
	bool IsAtEnd() const;
	char Advance();
	bool CheckMatchNext(char expected);
	char Peek() const;
	char PeekNext() const;
	int GetLexemeSize() const;
	void ConsumeStringLiteral();
	void ConsumeNumericLiteral();
	void ConsumeIdentifier();
	void AddToken(TokenType type);
	void AddToken(TokenType type, LiteralT literal);
};