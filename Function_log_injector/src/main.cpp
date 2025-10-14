/*
 * Main file for FunctionLogInjector
 */ 

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/FileEntry.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include <clang/AST/AST.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <llvm-20/llvm/ADT/SmallVector.h>
#include <llvm-20/llvm/ADT/StringRef.h>
#include <llvm-20/llvm/Support/raw_ostream.h>
#include <llvm/Support/CommandLine.h>
#include <memory>

static llvm::cl::OptionCategory ToolCategory("FunctionLogInjector options");

class LoginInjectorCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
	private:
		clang::Rewriter &rewriter;
	public:
		LoginInjectorCallback(clang::Rewriter &R) : rewriter(R) {}

		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
			if (const clang::FunctionDecl *FD = result.Nodes.getNodeAs<clang::FunctionDecl>("funcDecl")) {
				// If function is a declaration or main dont insert log statement there
				if (!FD->hasBody() || FD->isMain())
					return;

				const clang::Stmt *body = FD->getBody();
				clang::SourceLocation startLocation = body->getBeginLoc().getLocWithOffset(1);
				std::string functionName = FD->getNameAsString();
				std::string logStatement_start = "\nLOG_START(" + functionName + ");";
				std::string logStatement_end = "\nLOG_END(" + functionName + ");\n";

				rewriter.InsertText(startLocation, logStatement_start, true, true);

				clang::SourceLocation endLocation = body->getEndLoc().getLocWithOffset(0);
				rewriter.InsertText(endLocation, logStatement_end, true, true);
			}
		}
};


class LogInjectorASTConsumer : public clang::ASTConsumer {
	private:
		clang::ast_matchers::MatchFinder matcher;
		LoginInjectorCallback handler;
	public:
		LogInjectorASTConsumer(clang::Rewriter &R) : handler(R) {
			matcher.addMatcher(clang::ast_matchers::functionDecl(clang::ast_matchers::isDefinition()).bind("funcDecl"), &handler);
		}
		void HandleTranslationUnit(clang::ASTContext &context) override {
			matcher.matchAST(context);
		}

};

class LoginInjectorFrontendAction : public clang::ASTFrontendAction {
	private:
		clang::Rewriter rewriter;

	public:
		// This funciton runs at the end of source file processing
		void EndSourceFileAction() override {
			clang::SourceManager &SM = rewriter.getSourceMgr();

			clang::FileID mainFileID = SM.getMainFileID();
			llvm::StringRef fileText = SM.getBufferData(mainFileID);
			
			// Check if custom_logger.h is there 
			if (!fileText.contains("#include \"custom_logger.h\"")) {
				// Divide the whole code in lines and check line by line the presence of comments or blank line or #include 
				llvm::SmallVector<llvm::StringRef, 32> lines;
			    fileText.split(lines, '\n');
				unsigned insertAfterLine = 0;
				for (unsigned i = 0; i < lines.size(); i++) {
					llvm::StringRef line = lines[i].trim();
					// Skip comments
					if ( line.starts_with("//") || line.starts_with("/*") || line.empty())
						continue;

					if (line.starts_with("#include"))
						insertAfterLine = i;
	
				}
				const clang::FileEntry *FileEntry = SM.getFileEntryForID(mainFileID);
				clang::SourceLocation insertLocation = (insertAfterLine > 0)? 
														SM.translateFileLineCol(FileEntry, insertAfterLine+1, 1) : 
														SM.getLocForStartOfFile(mainFileID);

				rewriter.InsertText(insertLocation, "#include \"custom_logger.h>\"\n", true, true);
			}

			llvm::outs() << " ====== Transformed File ======\n";
			rewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
		}

		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef infile) override {
			rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
			return std::make_unique<LogInjectorASTConsumer>(rewriter);
		}
};

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
