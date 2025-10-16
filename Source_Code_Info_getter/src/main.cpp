/* 
 * scig is a tool takes in source code as an input and gives the information about the source code like lines of code, name & number of functions,
 * number of variables used and their types etc.
 * Date: 14-Oct-2025
*/

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/DiagnosticCategories.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Preprocessor.h"
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
#include <vector>

// ============================== CommandLine Optinos ======================================

static llvm::cl::OptionCategory Tooling("SourceCodeInfoGetter options");

static llvm::cl::opt<bool> ShowFunctionsInfo("info-f", llvm::cl::desc("Display just the function information"), 
											llvm::cl::init(false), llvm::cl::cat(Tooling));

static llvm::cl::opt<bool> ShowVariablesInfo("info-v", llvm::cl::desc("Displays just the variables information"),
											llvm::cl::init(false), llvm::cl::cat(Tooling));

static llvm::cl::opt<bool> ShowCommentsInfo("info-c", llvm::cl::desc("Displays just the comments information"),
											llvm::cl::init(false), llvm::cl::cat(Tooling));


// ============================== Callbacks ======================================

class FunctionCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
	public:
		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
			clang::SourceManager *SM = result.SourceManager;
			if (const clang::FunctionDecl *FD = result.Nodes.getNodeAs<clang::FunctionDecl>("function")) {
				// If function is declared/defined in the current file and not any  header, then print info on it 
				clang::SourceLocation location = FD->getLocation();
				if (SM->isWrittenInMainFile(location))
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

class CommentHandler : public clang::CommentHandler {
	private: 
		unsigned total_comment_lines = 0;
		const clang::SourceManager *SM = nullptr;
		std::vector<std::string> comment_locations;

	public:
		void setSourceManager(const clang::SourceManager *sourcemanager) {
			SM = sourcemanager;
		}

		bool HandleComment(clang::Preprocessor &PP, clang::SourceRange range) override {
			if (!SM)
				SM = &PP.getSourceManager();
		
			clang::SourceLocation start = range.getBegin();
			clang::SourceLocation end = range.getEnd();

			// If comment is not in current file, then just omit it 
			if (!SM->isWrittenInMainFile(start))
				return false;

			unsigned start_line = SM->getSpellingLineNumber(start);
			unsigned end_line = SM->getSpellingLineNumber(end);
			
			total_comment_lines = total_comment_lines + end_line - start_line + 1;
			comment_locations.push_back(start.printToString(*SM) + " - " + end.printToString(*SM));

			return false;
		}

		unsigned getTotalComments() {
			return total_comment_lines;
		}

		std::vector<std::string> getCommentLocations() {
			return comment_locations;
		}
};


// ============================== ASTConsumer ======================================

class SCIG_Consumer : public clang::ASTConsumer {
	private: 
		clang::ast_matchers::MatchFinder matcher;
		FunctionCallback f_callback;
		VariableCallback v_callback;

	public:
		SCIG_Consumer(bool showfunctionsinfo, bool showvariablesinfo) {
			if (showfunctionsinfo)
				matcher.addMatcher(clang::ast_matchers::functionDecl(clang::ast_matchers::isDefinition()).bind("function"), &f_callback);
			
			if (showvariablesinfo)
				matcher.addMatcher(clang::ast_matchers::varDecl().bind("variable"), &v_callback);
		}

		void HandleTranslationUnit(clang::ASTContext &context) override {
			matcher.matchAST(context);
		}
};


// ============================== ASTFrontendAction ======================================

class SCIG_FrontendAction : public clang::ASTFrontendAction {
	private:
		CommentHandler commenthandler_obj;
		bool showcommentsinfo = false;

	public:
		void ExecuteAction() override {
			showcommentsinfo = ShowCommentsInfo || (!ShowCommentsInfo && !ShowFunctionsInfo && !ShowVariablesInfo);

			if (!showcommentsinfo) {
				clang::ASTFrontendAction::ExecuteAction();
				return;
			}

			clang::CompilerInstance &CI = getCompilerInstance();
			commenthandler_obj.setSourceManager(&CI.getSourceManager());
			CI.getPreprocessor().addCommentHandler(&commenthandler_obj);
			
			clang::ASTFrontendAction::ExecuteAction();	
		}

		void EndSourceFileAction() override {
			if (!showcommentsinfo)
				return;
			llvm::outs() << "\nTotal line of comments: " << commenthandler_obj.getTotalComments() << "\n";
			llvm::outs() << "Comments found at following locations: \n";	
			for (auto v : commenthandler_obj.getCommentLocations())
				llvm::outs() << v << "\n";
		}

		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override {

			bool showfunctionsinfo = ShowFunctionsInfo || (!ShowFunctionsInfo && !ShowVariablesInfo && !ShowCommentsInfo);
			bool showvariablesinfo = ShowVariablesInfo || (!ShowVariablesInfo && !ShowFunctionsInfo && !ShowCommentsInfo);

			return std::make_unique<SCIG_Consumer>(showfunctionsinfo, showvariablesinfo); 
		}	
};


// ============================== int main() ======================================

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
