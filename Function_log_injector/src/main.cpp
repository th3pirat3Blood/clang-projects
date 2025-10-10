/*
 * Main file for FunctionLogInjector
 */ 

#include <clang/AST/AST.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>

#include <llvm/Support/CommandLine.h>

static llvm::cl::OptionCategory ToolCategory("FunctionLogInjector options");

class LoginInjectorCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
	public:
		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
			llvm::outs() << "Wroks" << "\n";
		}
};


/*class LoginInjectorFrontendAction : public clang::FrontendAction {

};*/

int main(int argc, const char **argv) {
	auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, ToolCategory); 
	if (!ExpectedParser) {
		llvm::errs() << ExpectedParser.takeError();
		return 1;
	}

	clang::tooling::CommonOptionsParser &OptionsParser = ExpectedParser.get();
	clang::tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

	return Tool.run(clang::tooling::newFrontendActionFactory<LoginInjectorFrontendAction>().get());
}
