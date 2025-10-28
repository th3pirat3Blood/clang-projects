/* 
 * scig is a tool takes in source code as an input and gives the information about the source code like lines of code, name & number of functions,
 * number of variables used and their types etc.
 * Date: 14-Oct-2025
*/

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Preprocessor.h"
#include <clang/AST/AST.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

#include <llvm-20/llvm/ADT/StringRef.h>
#include <llvm-20/llvm/Support/Error.h>
#include <llvm-20/llvm/Support/Path.h>
#include <llvm-20/llvm/Support/raw_ostream.h>
#include <llvm/Support/CommandLine.h>
#include <memory>
#include <string>
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
	private:
		std::vector<std::string> &location_details;
	public:
		FunctionCallback(std::vector<std::string> &ld) : location_details(ld) {}

		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
			clang::SourceManager *SM = result.SourceManager;
			if (const clang::FunctionDecl *FD = result.Nodes.getNodeAs<clang::FunctionDecl>("function")) {
				// If function is declared/defined in the current file and not any  header, then print info on it 
				clang::SourceLocation location = FD->getLocation();
				if (!SM->isWrittenInMainFile(location))
			 		return;

				unsigned line_number = SM->getSpellingLineNumber(location);
				unsigned col_number = SM->getSpellingColumnNumber(location);
				std::string location_string = (FD->getNameAsString() + "() found in " + 
											llvm::sys::path::filename(SM->getFilename(location)) +
											" at line " + std::to_string(line_number) + ":" + std::to_string(col_number)).str();
				location_details.push_back(location_string);
			}
		}

		std::vector<std::string> getLocationDetails() {
			return location_details;
		}
};

class VariableCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
	private:
		std::vector<std::string> &location_details;
	public:
		VariableCallback(std::vector<std::string> &ld) : location_details(ld) {}

		void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override {
			clang::SourceManager *SM = result.SourceManager;
			if (const clang::VarDecl *VD = result.Nodes.getNodeAs<clang::VarDecl>("variable")) {
				// If variable is declared in the file being processed and not in any other included file, then print info on it
				clang::SourceLocation location = VD->getLocation();
				if (!SM->isWrittenInMainFile(location))
					return;
				
				unsigned line_number = SM->getSpellingLineNumber(location);
				unsigned col_number = SM->getSpellingColumnNumber(location);
				std::string location_string = ("Variable \"" + VD->getNameAsString() + "\" declared at " +
											llvm::sys::path::filename(SM->getFilename(location)) + 
											"at line" + std::to_string(line_number) + ":" + std::to_string(col_number)).str();
				location_details.push_back(location_string);
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

			llvm::StringRef filename = llvm::sys::path::filename(SM->getFilename(start));
			std::string file_position = (filename + ":" + std::to_string(start_line) + "-" + std::to_string(end_line)).str();
			comment_locations.push_back(file_position);
			

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
		SCIG_Consumer(bool showfunctionsinfo, bool showvariablesinfo, 
						std::vector<std::string> &func_location_details,
					   	std::vector<std::string> &var_location_details
					 ) : f_callback(func_location_details),v_callback(var_location_details) {
			if (showfunctionsinfo) {
				matcher.addMatcher(clang::ast_matchers::traverse(clang::TK_IgnoreUnlessSpelledInSource,
									clang::ast_matchers::functionDecl(
										clang::ast_matchers::isDefinition()
								  		)
									).bind("function"), &f_callback);
			}
			if (showvariablesinfo) {
				matcher.addMatcher(clang::ast_matchers::traverse(
										clang::TK_IgnoreUnlessSpelledInSource, 
											clang::ast_matchers::varDecl()
									).bind("variable"), &v_callback);
//				matcher.addMatcher(clang::ast_matchers::varDecl().bind("variable"), &v_callback);
		
			}
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
		bool showfunctionsinfo = false;
		bool showvariablesinfo = false;
		std::vector<std::string> function_location_details;
		std::vector<std::string> variable_location_details;

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
			if (showfunctionsinfo) {
				llvm::outs() << "============\n";
				llvm::outs() << "Functions\n";
				llvm::outs() << "============\n";
				for (auto v: function_location_details)
					llvm::outs() << v << "\n";
			}

			if (showvariablesinfo) {
				llvm::outs() << "============\n";
				llvm::outs() << "Variables\n";
				llvm::outs() << "============\n";
				for (auto v: variable_location_details)
					llvm::outs() << v << "\n";
			}

			if (showcommentsinfo) {
				llvm::outs() << "=============\n";
				llvm::outs() << "Comments\n";
				llvm::outs() << "=============\n";
				llvm::outs() << "Total line of comments: " << commenthandler_obj.getTotalComments() << "\n";
				llvm::outs() << "Comments found at following locations: \n";	
				for (auto v : commenthandler_obj.getCommentLocations())
					llvm::outs() << v << "\n";
			}
		}

		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override {

			showfunctionsinfo = ShowFunctionsInfo || (!ShowFunctionsInfo && !ShowVariablesInfo && !ShowCommentsInfo);
			showvariablesinfo = ShowVariablesInfo || (!ShowVariablesInfo && !ShowFunctionsInfo && !ShowCommentsInfo);

			return std::make_unique<SCIG_Consumer>(showfunctionsinfo, showvariablesinfo, function_location_details, variable_location_details); 
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
