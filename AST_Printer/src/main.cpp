/*
 * A tool to print AST of a given program
 */ 

#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include <clang/AST/AST.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/ASTConsumer.h>

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <llvm-20/llvm/ADT/StringRef.h>
#include <llvm-20/llvm/Support/raw_ostream.h>
#include <llvm/Support/CommandLine.h>
#include <memory>

static llvm::cl::OptionCategory optioncategory("tool for printing AST");

class ASTViewer_Consumer : public clang::ASTConsumer {
	public:
		void HandleTranslationUnit(clang::ASTContext &context) override {
			const clang::SourceManager &SM = context.getSourceManager();
			for (auto D: context.getTranslationUnitDecl()->decls()) {
				clang::FullSourceLoc fulloc(D->getBeginLoc(), SM);
				if (SM.isInMainFile(fulloc))
					D->dump();
			}
		}
};

class ASTViewer_FrontendAction : public clang::ASTFrontendAction {
	public:
		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef infile) override {
			return std::make_unique<ASTViewer_Consumer>();
		}

};


int main(int argc, const char **argv) {
	auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, optioncategory);
	if (!ExpectedParser) {
		llvm::outs() << ExpectedParser.takeError();
		return 1;
	}

	clang::tooling::CommonOptionsParser &OptionsParser = ExpectedParser.get();
	clang::tooling::ClangTool tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

	return tool.run(clang::tooling::newFrontendActionFactory<ASTViewer_FrontendAction>().get());
}
