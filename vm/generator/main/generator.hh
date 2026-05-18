// Copyright © 2012, Université catholique de Louvain
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <cstdlib>
#include <iostream>
#include <functional>
#include <string>

#include <clang/Frontend/ASTUnit.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/Frontend/CompilerInstance.h>

typedef clang::ClassTemplateSpecializationDecl SpecDecl;
typedef clang::CXXRecordDecl ClassDecl;

typedef llvm::raw_fd_ostream ostream;

/**
 * @brief If the provided argument contains an error, prints it on LLVM's error stream
 * 
 * @param err A reference to the potential error
 */
inline
void checkErrString(const std::error_code& err) {
  if (err) {
    llvm::errs() << err.message() << "\n";
    exit(1);
  }
}

/**
 * @brief Opens an output stream on the file
 * 
 * @param fileName File to write
 * @return An LLVM output stream
 */
inline
std::unique_ptr<ostream> openFileOutputStream(const std::string& fileName) {
  std::error_code err;
  auto result = std::unique_ptr<ostream>(new ostream(fileName.c_str(), err,
                                                     llvm::sys::fs::OF_None));
  checkErrString(err);

  return result;
}

/**
 * @brief Executes a lambda function with an output stream on the specified file
 * 
 * @param fileName File to write
 * @param body Lambda function
 */
inline
void withFileOutputStream(const std::string& fileName,
                          std::function<void (ostream&)> body) {
  std::error_code err;
  ostream stream(fileName.c_str(), err, llvm::sys::fs::OF_None);
  checkErrString(err);

  body(stream);
}

/**
 * @brief Converts a qualified type to its string representation
 * 
 * @param type
 * @return A string representing type
 */
std::string typeToString(clang::QualType type);

/**
 * @brief Tells if the name corresponds to the provided class declaration
 * 
 * @param cls The declared class
 * @param fullClassName The potential class name
 * @return If fullClassName is the name of cls
 */
bool isTheClass(const ClassDecl* cls,
                const std::string& fullClassName);

/**
 * @brief Tells if the name corresponds to the template name of the declared class
 * 
 * @param cls The declared class
 * @param fullClassTemplateName The template name
 * @return If the class declaration is an instance of the specified template
 */
bool isAnInstantiationOfTheTemplate(const ClassDecl* cls,
                                    const std::string& fullClassTemplateName);

/**
 * @brief Tells if the provided declared class is complete and has one of its
 * sub-classes passing the provided predicate
 * 
 * @param cls The class declaration to test
 * @param testBase The lambda function serving as predicated
 * @return If the predicate matched one of the sub-classes of the class declaration
 */
bool existsBaseClassSuchThat(
  const ClassDecl* cls,
  const std::function<bool(const ClassDecl*)>& testBase);

/**
 * @brief Gets a string representation of the first template paramater
 * in other words the template type name
 * 
 * @param specDecl The template declaration
 * @param basicName Format parameter
 * @return A string representing the template type
 */
std::string getTypeParamAsString(const SpecDecl* specDecl,
                                 bool basicName = true);

/**
 * @brief Gets a string representation of the first template paramater
 * in other words the template type name
 * 
 * @param unaryTemplate The template type
 * @param basicName Format parameter
 * @return A string representing the template type
 */
std::string getTypeParamAsString(const clang::TemplateSpecializationType* unaryTemplate,
                                 bool basicName = true);

/**
 * @brief Generates a template definition according to the provided parameters list
 * and potentially to the optional default arguments list
 * 
 * @param Out The out-stream where to output the generated template
 * @param Params The parameters list
 * @param Args The optional arguments list
 */
void printTemplateParameters(llvm::raw_fd_ostream& Out,
  const clang::TemplateParameterList *Params,
  const clang::TemplateArgumentList *Args = 0);

/**
 * @brief Generates a concrete template according to the provided parameters list
 * and potentially to the optional default arguments list
 * 
 * @param Out The out-stream where to output the generated template
 * @param Params The parameters list
 * @param Args The optional arguments list
 */
void printActualTemplateParameters(llvm::raw_fd_ostream& Out,
  const clang::TemplateParameterList *Params,
  const clang::TemplateArgumentList *Args = 0);

/**
 * @brief Extracts different informations from the provided function declaration
 * 
 * @param function The function declaration
 * @param name The extracted name
 * @param resultType The extracted return type
 * @param formalParams The extracted formal parameters
 * @param actualParams The extracted actual parameters' values
 * @param reflectActualParams The extracted reflect actual parameters (metadata about them)
 * @param hasSelfParam Tells if self parameter has to be included
 * 
 * @note TO-COMPLETE explain self parameter
 */
void parseFunction(const clang::FunctionDecl* function,
                   std::string& name, std::string& resultType,
                   std::string& formalParams, std::string& actualParams,
                   std::string& reflectActualParams,
                   bool hasSelfParam);

namespace internal {
  template <class T>
  struct Dereferencer {
    static T deref(T&& value) {
      return std::forward<T>(value);
    }
  };

  template <class T>
  struct Dereferencer<T*&&> {
    static T& deref(T* value) {
      return *value;
    }
  };

  template <class T>
  inline
  auto dereference(T&& value) ->
      decltype(Dereferencer<decltype(std::forward<T>(value))>::deref(std::forward<T>(value))) {
    return Dereferencer<decltype(std::forward<T>(value))>::deref(std::forward<T>(value));
  }
}

/**
 * @brief Extracts an integral template argument from the provided template
 * class and dereferences it
 * 
 * @tparam T 
 * @param specDecl The template class declaration
 * @return The extracted dereferenced integral
 * 
 * @note There must be only one template argument
 */
template <class T>
T getValueParamAsIntegral(const SpecDecl* specDecl) {
  const clang::TemplateArgumentList& templateArgs = specDecl->getTemplateArgs();

  assert(templateArgs.size() == 1);
  assert(templateArgs[0].getKind() == clang::TemplateArgument::Integral);

  llvm::APSInt integralValue = templateArgs[0].getAsIntegral();
  auto&& integralArg = internal::dereference(integralValue);
  auto result = integralArg.getLimitedValue(std::numeric_limits<T>::max());

  return (T) result;
}

/**
 * @brief Extracts an integral template argument from the provided template
 * class and dereferences it
 * 
 * @tparam T 
 * @param arg The template class as a record declaration
 * @return The extracted dereferenced integral
 * 
 * @note There must be only one template argument
 */
template <class T>
T getValueParamAsIntegral(clang::CXXRecordDecl* arg) {
  return getValueParamAsIntegral<T>(clang::dyn_cast<SpecDecl>(arg));
}

/**
 * @brief Converts a boolean to string
 * 
 * @param value Boolean
 * @return "true" or "false"
 */
inline
std::string b2s(bool value) {
  return value ? "true" : "false";
}

/**
 * @brief Tells if one of the base classes (like parents) of cls are implementing
 * the base class 'mozart::DataType'
 * 
 * @param cls The class to analyze
 * @return If one of the base class is an instance of 'mozart::Datatype' 
 */
bool isImplementationClass(const ClassDecl* cls);

/**
 * @brief According to each base class of the provided one, sets the implementation 
 * options then collects methods and computes properties then finally writes the
 * implementation files into the output directory
 * 
 * @param outputDir The directory containing generated files
 * @param ND The class declaration
 */
void handleImplementation(const std::string& outputDir, const ClassDecl* ND);

/**
 * @brief According to each base class of the provided one, sets the implementation
 * options then generates interfaces then finally writes the implementation files
 * into the output directory
 * 
 * @param outputDir The directory containing generated files
 * @param ND The class declaration
 */
void handleInterface(const std::string& outputDir, const SpecDecl* ND);

/**
 * @brief Tells if one of the base classes of the provided one is a module class
 * 
 * @param cls The class declaration
 * @return If the statement is true
 */
bool isModuleClass(const ClassDecl* cls);

/**
 * @brief Generates header and code for the built-in module and additionally
 * the required inlined instructions
 * 
 * @param outputDir The directory containing generated files
 * @param CD The class declaration
 * @param builtinHeaderFile The output stream where to write the header
 * @param builtinCodeFile The output stream where to write the code
 * @param emulateInlinesTo The output stream where to write the inlined instructions
 */
void handleBuiltinModule(const std::string& outputDir, const ClassDecl* CD,
                         llvm::raw_fd_ostream& builtinHeaderFile,
                         llvm::raw_fd_ostream& builtinCodeFile,
                         llvm::raw_fd_ostream* emulateInlinesTo);

extern clang::ASTContext* context;
