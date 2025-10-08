/*
 * Main class for Function Metrics Analyzer
 */ 

#include "clang/AST/Decl.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include <clang/AST/AST.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <llvm/Support/CommandLine.h>

// This class is called at every successfull match found in the AST 
class FunctionMetricsCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
	public:
		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) {
			if (const clang::FunctionDecl *FD = result.Nodes.getNodeAs<clang::FunctionDecl>("func")) {
				// If the match found is just a function defintion then just return 
				if (!FD->hasBody())
					return;

				const clang::SourceManager &SM = *result.SourceManager;
				clang::SourceLocation startLoc = FD->getBody()->getBeginLoc();
				clang::SourceLocation endLoc = FD->getBody()->getEndLoc();
				unsigned start_line_number = SM.getSpellingLineNumber(startLoc);
				unsigned end_line_number = SM.getSpellingLineNumber(endLoc);
				
				llvm::outs() << "Function : " << FD->getNameAsString() << "\n";
				llvm::outs() << "Lines of code : " << (end_line_number - start_line_number) << "\n";
				llvm::outs() << "Parameter count : " << FD->getNumParams() << "\n";
				llvm::outs() << "Location : " << SM.getFilename(FD->getLocation()) << ":" \
			  				 << SM.getSpellingLineNumber(FD->getLocation()) << "\n\n";
			}
		}
};

static llvm::cl::OptionCategory ToolCategory("function-metrics option");

int main(int argc, const char **argv) {
	// Helps parse the command line options passed to tool 
	// clang::tooling::CommonOptionsParser optionParser = clang::tooling::CommonOptionsParser::create(argc, argv, ToolCategory);
	auto optionParser = clang::tooling::CommonOptionsParser::create(argc, argv, ToolCategory);
	
	// Helps run FrontendActions over a set of files
	clang::tooling::ClangTool Tool(optionParser->getCompilations(), optionParser->getSourcePathList());
	
	FunctionMetricsCallback callback;
	clang::ast_matchers::MatchFinder finder;

	finder.addMatcher(clang::ast_matchers::functionDecl(clang::ast_matchers::isDefinition()).bind("func"), &callback);

	return Tool.run(clang::tooling::newFrontendActionFactory(&finder).get());

}

