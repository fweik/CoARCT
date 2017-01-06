// global_matchers.h
// T. M. Kelley
// Jan 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved


#ifndef GLOBAL_MATCHERS_H
#define GLOBAL_MATCHERS_H

#include "dump_things.h"
#include "types.h"
#include "utilities.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Core/Replacement.h"
#include <iostream>

namespace corct{
// clang-format off
/**\brief Matches references to global variable with g_var_name within any
function. If no name given, matches each use of global variables in functions.

  Bindings: globalReference to the reference
            varName to the VarDecl
            function: to the FunctionDecl
  */
auto mk_global_var_matcher(std::string const & g_var_name = ""){
  using namespace clang::ast_matchers;
  if(g_var_name != ""){
    return
    declRefExpr(
      to(
        varDecl(
          hasGlobalStorage()
         ,hasName(g_var_name)
        ).bind("varName")
      )
     ,hasAncestor(
        functionDecl().bind("function")
      )
    ).bind("globalReference")
    ;
  }
  return
  declRefExpr(
    to(
      varDecl(
        hasGlobalStorage()
      ).bind("varName")
    ) // to
   ,hasAncestor(
      functionDecl().bind("function")
    )
  ).bind("globalReference")
  ;

} // mk_decl_matcherExpression E

//declRefExpr(to(varDecl(hasGlobalStorage(),hasName("global_i"))),hasAncestor(functionDecl()))

/**\brief Matches references to any global variable within any function.

  Bindings: globalReference to the reference
            varName to the VarDecl
            function: to the FunctionDecl
  */
auto all_global_var_matcher(){
  using namespace clang::ast_matchers;
  return
  declRefExpr(
    to(
      varDecl(
        hasGlobalStorage()
      ).bind("varName")
    ) // to
   ,hasAncestor(
      functionDecl().bind("function")
    )
  ).bind("globalReference")
  ;
} // mk_decl_matcher

/**\brief Matches functions that uses specified global variable. If no name
 given, matches each function that uses any global variables.

  Bindings: globalReference to the reference
            varName to the VarDecl
            function: to the FunctionDecl
  */
auto mk_global_fn_matcher(std::string const & g_var_name = ""){
  using namespace clang::ast_matchers;
  if(g_var_name!=""){
    return
    functionDecl(
      hasDescendant(
        declRefExpr(
          to(
            varDecl(
              hasGlobalStorage(),
              hasName(g_var_name)
            ).bind("varName") // varDecl
          ) // to
        ).bind("globalReference") // declRefExpr
      ) // hasDescendant
    ).bind("function")
    ;
  }
  return
  functionDecl(
    hasDescendant(
      declRefExpr(
        to(
          varDecl(
              hasGlobalStorage()
          ).bind("varName")
        )
      ).bind("globalReference")
    )
  ).bind("function")
  ;
} // mk_decl_matcher

/**\brief Matches functions that use any globabl variable.

  Bindings: globalReference to the reference
            varName to the VarDecl
            function: to the FunctionDecl
  */
auto all_global_fn_matcher(){
  using namespace clang::ast_matchers;
  return
  functionDecl(
    hasDescendant(
      declRefExpr(
        to(
          varDecl(
              hasGlobalStorage()
          ).bind("varName")
        )
      ).bind("globalReference")
    )
  ).bind("function")
  ;
} // mk_decl_matcher

// clang-format on

class Global_Printer : public callback_t {
public:
  virtual void
  run(result_t const & result) override
  {
    using namespace clang;
    FunctionDecl const * func_decl =
        result.Nodes.getNodeAs<FunctionDecl>("function");
    Expr const * g_var = result.Nodes.getNodeAs<Expr>("globalReference");
    VarDecl const * var = result.Nodes.getNodeAs<VarDecl>("varName");
    clang::SourceManager & src_manager(
        const_cast<clang::SourceManager &>(result.Context->getSourceManager()));
    if(func_decl && g_var && var) {
      std::cout << "In function '" << func_decl->getNameAsString() << "' ";
      std::cout << "'" << var->getNameAsString() << "' referred to at ";
      corct::dumpSourceRange(g_var->getSourceRange(), &src_manager);
      std::cout << "\n";
    }
    else {
      check_ptr(func_decl,"func_decl");
      check_ptr(g_var,"g_var");
      check_ptr(var,"var");
    }
    return;
  }  // run

};  // class Global_Printer

} // corct::

#endif // include guard


// End of file
