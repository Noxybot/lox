#include <array>
#include <iostream>
#include <fstream>
#include <string_view>
#include <span>
#include <ranges>
#include <vector>
#include <filesystem>
#include <sstream>

constexpr std::string_view VERSION{ "1.0.0" };

void DefineAST(std::ofstream& file, std::string_view base_name,
	std::span<const std::string_view> members, bool add_expr_namespace = false);

void WriteProlog(std::ofstream& file)
{
	file << "//This file was generated by ast_builder.exe v" << VERSION << '\n';
	file << "export module ast;\n\n";
	file << "import <any>;\n";
	file << "import <memory>;\n";
	file << "import <vector>;\n\n";
	file << "import core;\n\n";
	file << "export namespace ast\n";
	file << "{\n";
}

void WriteEpilog(std::ofstream& file)
{
	file << "}//namespace ast\n";
}

int main(int argc, char** argv)
{
	const auto path = "C:/Users/Eduard/Desktop/my_int/ast/ast.ixx";
	std::ofstream file { path };
	if (!file.is_open())
	{
		std::cerr << "can`t open file: " << path << std::endl;
		return -1;
	}
	WriteProlog(file);
	file << "\nnamespace expr \n{\n\n";
	DefineAST(file, "Expr", {{
		"Assign   ^Token-name,Expr-value",
		"Binary   ^Expr-left,Token-op,Expr-right",
		"Grouping ^Expr-expression",
		"Logical  ^Expr-left,Token-op,Expr-right",
		"Literal  ^LiteralT-value",
		"Unary    ^Token-op,Expr-right",
		"Variable ^Token-name"
		} } );
	file << "\n} //namespace expr\n";
	file << "\nnamespace stmt \n{\n\n";
	DefineAST(file, "Stmt", { {
		"Block      ^std::vector<StmtPtr>-statements",
		"Expression ^Expr-expression",
		"If         ^Expr-condition,StmtPtr-then_branch,StmtPtr-else_branch",
		"Print      ^Expr-expression",
		"Var        ^Token-name,Expr-initializer",
		"While      ^Expr-condition,StmtPtr-body"
		} }, true);
	file << "\n} //namespace stmt\n";
	WriteEpilog(file);
	return 0;
}

void DefineType(std::ostream& file, std::string_view base_name,
	std::string_view struct_name, std::string_view fiends, bool add_expr_namespace = false);
void DefineVisitor(std::ofstream& file, std::span<std::string_view> types, std::string_view base_name);
void ForwardDeclareTypes(std::ofstream& file, std::span<std::string_view> types);

void DefineAST(std::ofstream& file, std::string_view base_name,
	std::span<const std::string_view> members, bool add_expr_namespace)
{
	std::vector<std::string_view> types;
	std::stringstream ss;
	constexpr std::string_view delim{ "^" };
	for (const auto& node : members)
	{
		std::vector<std::string_view> views;
		for (const auto& view : std::views::split(node, delim))
		{
			if (views.empty())
				types.emplace_back(view);
			views.emplace_back(view);
		}
			
		if (views.size() == 2)
			DefineType(ss, base_name, views[0], views[1], add_expr_namespace);
	}
	ForwardDeclareTypes(file, types);
	DefineVisitor(file, types, base_name);
	file << "struct " << base_name
		<< "\n{\n"
		<< "\tvirtual ~" << base_name << "() = default;\n"
		<< "\tvirtual std::any Accept(Visitor" << base_name << "& visitor) const = 0;\n"
		<< "};\n\n"
		<< "using " << base_name << "Ptr = std::unique_ptr<" << base_name << ">; \n\n";
	file << ss.str();
}

void ForwardDeclareTypes(std::ofstream& file, std::span<std::string_view> types)
{
	for (const auto& type : types)
		file << "struct " << type << ";\n";
	file << '\n';
}

void DefineVisitor(std::ofstream& file, std::span<std::string_view> types, std::string_view base_name)
{
	file << "struct Visitor" << base_name << "\n"
		<< "{\n"
		<< "\tvirtual ~Visitor" << base_name << "() = default;\n";
	for (const auto& type : types)
	{
		file << "\tvirtual std::any Visit(const " << type << "& val) = 0;\n";
	}
	file << "};\n";
}

void DefineType(std::ostream& file, std::string_view base_name,
	std::string_view struct_name, std::string_view fiends, bool add_expr_namespace)
{
	file << "struct " << struct_name << " : " << base_name << '\n'
		<< "{\n";
	std::stringstream constructor_params;
	constructor_params << "\texplicit " << struct_name << "(";
	const char* sep = "";
	std::stringstream constructor_init_list;
	constructor_init_list << "\t\t: ";
	const char* tabs2 = "";
	for (const auto& field :
		std::views::split(fiends, std::string_view(",")))
	{
		std::vector<std::string_view> type_then_name;
		for (const auto& val :
			std::views::split(
				std::string_view(field.begin(), field.end()), std::string_view("-")))
		{
			type_then_name.emplace_back(val);
		}
		if (type_then_name.size() != 2)
			continue;
		std::stringstream ss;
		if (type_then_name[0] == base_name || type_then_name[0] == "Expr")
		{
			ss << '\t' << "std::unique_ptr<" << (!add_expr_namespace ? type_then_name[0] : "expr::" + std::string(type_then_name[0]))
				<< "> " << type_then_name[1] << ";\n";
		}
		else
		{
			ss << '\t' << type_then_name[0] << " " << type_then_name[1] << ";\n";
		}
		file << ss.str();
		auto str = ss.str();
		str.erase(0, 1); // \t
		str.pop_back(); // \n
		str.pop_back(); // \;
		str.push_back('_');
		constructor_params << sep << str;
		constructor_init_list << tabs2 << sep << type_then_name[1] << "(std::move("
			<< std::string(type_then_name[1]) + "_))\n";
		tabs2 = "\t\t";
		sep = ", ";
	}
	constructor_params << ")\n";
	file << constructor_params.str();
	file << constructor_init_list.str()
		<< "\t\t{}\n";
	file << "\tstd::any Accept(Visitor" << base_name << "& visitor) const override\n"
		<< "\t{\n"
		<< "\t\treturn visitor.Visit(*this);\n"
		<< "\t}\n";
	file << "};\n";
}
