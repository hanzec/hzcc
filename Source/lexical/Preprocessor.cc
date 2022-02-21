#include "lexical.h"
#include <string>
namespace Mycc::Lexical {
Status PreprocessSource(std::istream& source, std::ostream& output,
                        const std::string& source_name) {
  return Status::OkStatus();

}
}  // namespace Mycc::Lexical