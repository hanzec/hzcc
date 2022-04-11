//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_CODEGEN_GENERATOR_H_
#define MYCC_SOURCE_CODEGEN_GENERATOR_H_
#include <fstream>
#include <string>
namespace Mycc::Codegen {
class Generator {
  public:
    explicit Generator(std::string output_file, std::string input_file);

  protected:
    std::ostream& GetOstream();

    virtual bool FinalizedSource() = 0;

    void RedirectOutputStream(std::ostream& out);

    const std::string& GetInputFile() const;

  private:
    std::fstream _file_handler;
    const std::string _input_file_name;
    const std::string _output_file_name;
};
}  // namespace Mycc::Codegen

#endif  // MYCC_SOURCE_CODEGEN_GENERATOR_H_
