/* 
 * scig is a tool takes in source code as an input and gives the information about the source code like lines of code, name & number of functions,
 * number of variables used and their types etc.
 * Date: 14-Oct-2025
*/

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendAction.h"
#include <clang/AST/AST.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

#include <llvm-20/llvm/Support/Error.h>
#include <llvm-20/llvm/Support/raw_ostream.h>
#include <llvm/Support/CommandLine.h>
#include <memory>

llvm::cl::OptionCategory Tooling("SourceCodeInfoGetter options");


class FunctionCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
	public:
		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
			clang::SourceManager *SM = result.SourceManager;
			if (const clang::FunctionDecl *FD = result.Nodes.getNodeAs<clang::FunctionDecl>("function")) {
				clang::SourceLocation location = FD->getLocation();
				llvm::outs() << FD->getNameAsString() << "() :" << location.printToString(*SM)<< "\n";	
			}
		}
};

class VariableCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
	public:
		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
			clang::SourceManager *SM = result.SourceManager;
			if (const clang::VarDecl *VD = result.Nodes.getNodeAs<clang::VarDecl>("variable")) {
				// If variable is declared in the file being processed and not in any other included file, then print info on it
				clang::SourceLocation location = VD->getLocation();
				if (SM->isWrittenInMainFile(location))
					llvm::outs() << "Variable " << VD->getNameAsString() << " declared at " << location.printToString(*SM) << "\n";
			}
		}
};


class SCIG_Consumer : public clang::ASTConsumer {
	private: 
		clang::ast_matchers::MatchFinder matcher;
		FunctionCallback f_callback;
		VariableCallback v_callback;

	public:
		SCIG_Consumer() {
			matcher.addMatcher(clang::ast_matchers::functionDecl(clang::ast_matchers::isDefinition()).bind("function"), &f_callback);
			matcher.addMatcher(clang::ast_matchers::varDecl().bind("variable"), &v_callback);
		}

		void HandleTranslationUnit(clang::ASTContext &context) override {
			matcher.matchAST(context);
		}
};


class SCIG_FrontendAction : public clang::ASTFrontendAction {
	public:
		void EndSourceFileAction() override {
			// Maybe something here ?
		}

		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override {
			return std::make_unique<SCIG_Consumer>(); 
		}	
};


int main(int argc, const char **argv) {
	auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, Tooling);
	if (!ExpectedParser) {
		llvm::errs() << llvm::toString(ExpectedParser.takeError()) << "\n";
		return -1;
	}

	clang::tooling::CommonOptionsParser &Options = ExpectedParser.get();
	clang::tooling::ClangTool tool(Options.getCompilations(), Options.getSourcePathList());

	return tool.run(clang::tooling::newFrontendActionFactory<SCIG_FrontendAction>().get());
}
