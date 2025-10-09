/*
 * Main file for naming convention checker
 *
 *   =========================================================
 *  | Entity               | Convention   | Example           |
 *	| -------------------- | ------------ | ----------------- |
 *	| Variables            | `camelCase`  | `myVariable`      |
 *	| Functions            | `camelCase`  | `doComputation()` |
 *	| Classes / Structs    | `PascalCase` | `MatrixSolver`    |
 *	| Constants (optional) | `UPPER_CASE` | `MAX_SIZE`        |
 *	 =========================================================
 */ 

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/AST/AST.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/CommandLine.h>

#include <regex>
#include <string>

static llvm::cl::OptionCategory toolCategory("naming convention checker");

class RegexExpressions {
	public:
		bool isCamelCase(const std::string name) {
			return std::regex_match(name, std::regex("^[a-z][a-zA-Z0-9]*$"));
		}
		bool isPascalCase(const std::string name) {
			return std::regex_match(name, std::regex("^[A-Z][a-zA-Z0-0]*$"));
		}
		bool isUpperCase(const std::string name) {
			return std::regex_match(name, std::regex("^[A-Z0-9_]*$"));
		}
};

class NamingConventionCheckerCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
	private:
		void report(std::string type, const std::string name, const clang::SourceManager &SM, clang::SourceLocation location) {
			llvm::outs() << type << " named \"" << name << "\" type at: " \
			   			 <<	SM.getSpellingLineNumber(location) << "\n";
		}

	public:
		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
			const clang::SourceManager &SM = *result.SourceManager;
			RegexExpressions RE_obj;
			// Search for VarDecls 
			if (const clang::VarDecl *VD = result.Nodes.getNodeAs<clang::VarDecl>("vardecl")) {
				if (!RE_obj.isCamelCase(VD->getNameAsString()))
					report("Variable", VD->getNameAsString() , SM, VD->getLocation());
			}

			// For Functions 
			if (const clang::FunctionDecl *FD = result.Nodes.getNodeAs<clang::FunctionDecl>("functions")) {
				if ( (FD->getNameAsString() != "main") && !RE_obj.isPascalCase(FD->getNameAsString()) )
					report("Function", FD->getNameAsString(), SM, FD->getLocation());
			}

			// For Constants
			if (const clang::ConstantExpr *CE = result.Nodes.getNodeAs<clang::ConstantExpr>("constant")) {

			}
		}
};



int main(int argc, const char **argv) {
	auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, toolCategory);
	if (!ExpectedParser) {
		llvm::errs() << ExpectedParser.takeError();
		return 1;
	}

	clang::tooling::CommonOptionsParser &optionparser = ExpectedParser.get();
	clang::tooling::ClangTool tool(optionparser.getCompilations(), optionparser.getSourcePathList());	

	NamingConventionCheckerCallback callback;
	clang::ast_matchers::MatchFinder finder;

	finder.addMatcher(clang::ast_matchers::varDecl(clang::ast_matchers::isExpansionInMainFile()).bind("vardecl"), &callback);
	finder.addMatcher(clang::ast_matchers::functionDecl(clang::ast_matchers::isExpansionInMainFile()).bind("functions"), &callback);
	finder.addMatcher(clang::ast_matchers::constantExpr(clang::ast_matchers::isExpansionInMainFile()).bind("constant"), &callback);
	return tool.run(clang::tooling::newFrontendActionFactory(&finder).get());
}

