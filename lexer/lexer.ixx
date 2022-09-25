export module lexer;

import core;
import log;

import <vector>;
import <string>;
import <unordered_map>;

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

module :private;

static std::unordered_map<std::string_view, TokenType> keywords
{
	{"and", TokenType::AND},
	{"class", TokenType::CLASS},
	{"else", TokenType::ELSE},
	{"false", TokenType::FALSE},
	{"for", TokenType::FOR},
	{"fun", TokenType::FUN},
	{"if", TokenType::IF},
	{"nil", TokenType::NIL},
	{"or", TokenType::OR},
	{"print", TokenType::PRINT},
	{"return", TokenType::RETURN},
	{"super", TokenType::SUPER},
	{"this", TokenType::THIS},
	{"true", TokenType::TRUE},
	{"var", TokenType::VAR},
	{"while", TokenType::WHILE},
};

static bool IsAlphanumeric(char ch)
{
	return std::isalnum(ch) || ch == '_';
}


Lexer::Lexer(std::string text)
	: m_text(std::move(text))
{
	//m_text.erase(std::remove_if(m_text.begin(), m_text.end(), isspace), m_text.end());
}

void Lexer::AddNextToken()
{
	const char c = Advance();
	switch (c)
	{
	case '(': AddToken(TokenType::LEFT_PAREN); break;
	case ')': AddToken(TokenType::RIGHT_PAREN); break;
	case '{': AddToken(TokenType::LEFT_BRACE); break;
	case '}': AddToken(TokenType::RIGHT_BRACE); break;
	case ',': AddToken(TokenType::COMMA); break;
	case '.': AddToken(TokenType::DOT); break;
	case '-': AddToken(TokenType::MINUS); break;
	case '+': AddToken(TokenType::PLUS); break;
	case ';': AddToken(TokenType::SEMICOLON); break;
	case '*': AddToken(TokenType::STAR); break;
	case '!':
		AddToken(CheckMatchNext('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
		break;
	case '=':
		AddToken(CheckMatchNext('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
		break;
	case '<':
		AddToken(CheckMatchNext('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
		break;
	case '>':
		AddToken(CheckMatchNext('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
		break;
	case '/':
		// Only one-lined comments
		if (CheckMatchNext('/'))
		{
			while (Peek() != '\n' && !IsAtEnd())
				Advance();
		}
		else
			AddToken(TokenType::SLASH);
		break;
	case ' ':
	case '\r':
	case '\t':
		// Ignore whitespace.
		break;
	case '\n':
		++m_line;
		break;
	case '"':
		ConsumeStringLiteral();
		break;
	default:
		if (std::isdigit(c))
			ConsumeNumericLiteral();
		else if (IsAlphanumeric(c))
			ConsumeIdentifier();
		else
			Error(m_line, "Unexpected character.");  break;
	}
}

std::vector<Token> Lexer::GetTokens()
{
	while (!IsAtEnd())
	{
		m_start_pos = m_curr_pos;
		AddNextToken();
	}
	m_tokens.emplace_back(TokenType::END_OF_FILE, "", LiteralT{}, m_line);
	return std::move(m_tokens);
}

bool Lexer::IsAtEnd() const
{
	return m_curr_pos >= m_text.size();
}

char Lexer::Advance()
{
	return m_text[m_curr_pos++];
}

bool Lexer::CheckMatchNext(char expected)
{
	if (IsAtEnd())
		return false;
	if (m_text[m_curr_pos] != expected)
		return false;
	++m_curr_pos;
	return true;
}

char Lexer::Peek() const
{
	if (IsAtEnd())
		return '\0';
	return m_text[m_curr_pos];
}

char Lexer::PeekNext() const
{
	if (m_curr_pos + 1 >= m_text.size())
		return '\0';
	return m_text[m_curr_pos + 1];
}

int Lexer::GetLexemeSize() const
{
	return m_curr_pos - m_start_pos;
}

void Lexer::ConsumeStringLiteral()
{
	while (Peek() != '"' && !IsAtEnd())
	{
		if (Peek() == '\n')
			++m_line;
		Advance();
	}
	if (IsAtEnd())
	{
		Error(m_line, "Unterminated string.");
		return;
	}
	// Closing "
	Advance();
	auto val =
		m_text.substr(m_start_pos + 1, GetLexemeSize() - 2);
	AddToken(TokenType::STRING, std::move(val));

}

void Lexer::ConsumeNumericLiteral()
{
	while (std::isdigit(Peek()))
		Advance();
	if (Peek() == '.' && std::isdigit(PeekNext()))
	{
		//Skip the "."
		Advance();
		while (std::isdigit(Peek()))
			Advance();
	}
	AddToken(TokenType::NUMBER,
		strtod(m_text.substr(m_start_pos, GetLexemeSize()).c_str(), nullptr));
}

void Lexer::ConsumeIdentifier()
{
	while (IsAlphanumeric(Peek()))
		Advance();

	auto lexeme = m_text.substr(m_start_pos, GetLexemeSize());
	const auto it = keywords.find(lexeme);
	if (it != std::end(keywords))
		AddToken(it->second, std::move(lexeme));
	else
		AddToken(TokenType::IDENTIFIER, std::move(lexeme));
}

void Lexer::AddToken(TokenType type)
{
	AddToken(type, {});
}

void Lexer::AddToken(TokenType type, LiteralT literal)
{
	auto lexeme = m_text.substr(m_start_pos, GetLexemeSize());
	m_tokens.emplace_back(type, std::move(lexeme), std::move(literal), m_line);

}
