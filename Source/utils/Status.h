//
// Created by chen_ on 2022/1/27.
//
#include <string>
#include <utility>

#include "logging.h"
#include "macro.h"
#ifndef MYCC_STATUS_H
#define MYCC_STATUS_H
namespace Hzcc {
/** #########################################################################
 *  #### Useful Macros     ##################################################
 *  ######################################################################### */
#define HZCC_ExceptOK_WITH_RETURN(expr)                                  \
    {                                                                    \
        auto macro_expand_ret_obj = std::move((expr));                   \
        static_assert(std::is_base_of_v<Hzcc::Status, decltype((expr))>, \
                      "Macro only supported type of Hzcc::Status");      \
                                                                         \
        if (!macro_expand_ret_obj.Ok()) {                                \
            VLOG(CODE_GEN_LEVEL) << macro_expand_ret_obj.Message();      \
            return std::move(macro_expand_ret_obj);                      \
        }                                                                \
    }

/**
 * @brief Status class which is used to represent the status of a method or
 * parameter combination, it is used to indicate the status of a method since
 * the project will disable exceptions.
 */
class Status {
  public:
    enum PACKED Code {
        /**
         * Status which means no error, no exception, preform exactly same
         * behaviors as method described
         */
        OK = 1,

        /**
         * Status which means current method or parameter combination are not
         * supported or even not implemented or resource not found
         */
        NOT_FOUND = 2,

        /**
         * Status which means current method or parameter combination are exist
         * but does not available at this time
         */
        INTERNAL = 2,

        /**
         * Status which means current method or parameter combination are exist
         * but does not available at this time
         */
        UNAVAILABLE = 3,

        SyntaxError = 4,
        /**
         * Status which means current method argument are incorrect
         *
         */
        INVALID_ARGUMENT = 4,
    };
    /**
     * @brief Construct a new Status object with defaule Message
     * @param code the status code which is described in @ref Code
     */
    explicit Status(Code code) : Status(code, "No Message Provided") {}

    /**
     * @brief Construct a new Status object with Message
     * @param code the status code which is described in @ref Code
     * @param msg the detailed Message which is used to describe the current
     * status object
     */
    Status(Code code, std::string msg)
        : _status_code(code), _status_message(std::move(msg)) {}

    /**
     * @brief Construct a new Status object with Message and line number of the
     * status happened
     * @param code the status code which is described in @ref Code
     * @param msg the detailed Message which is used to describe the current
     * status object
     * @param file the file name of the status happened
     * @param line the line number of the status happened
     */
    Status(Code code, std::string msg, std::string file, int line)
        : _status_code(code),
          _status_message(std::move(msg)),
          _source_code_file(std::move(file)),
          _line_number(line) {}

    /**
     * @brief the status code which is described in @ref Code
     * @return Code the status code which is described in @ref Code
     */
    [[nodiscard]] Code Code() const { return _status_code; }

    /**
     * @brief get the status Message
     * @return std::string the status Message
     */
    [[nodiscard]] std::string Message() const { return this->_status_message; }

    /**
     * @brief get the source code file name
     * @return std::string the source code file name
     */
    [[nodiscard]] std::string SourceFile() const {
        return this->_source_code_file;
    }

    /**
     * @brief get the line number of the status happened in the source code file
     * @return int the line number of the status happened in the source code
     * file
     */
    [[nodiscard]] int LineNumber() const { return this->_line_number; }

    /**
     * @brief Check if the status is OK
     * @return true if the status is OK
     * @return false if the status is not OK
     */
    [[nodiscard]] bool Ok() const { return _status_code == Code::OK; }

    /**
     * @brief Static method which is used to create a OK status
     * @return Status Object which contains no message and code is OK
     */
    static Status OkStatus() { return {Code::OK, "Return Ok without error!"}; }

  private:
    int _line_number = -1;
    enum Code _status_code;
    std::string _status_message = "No Message Provided";
    std::string _source_code_file = "No file provided";
};
}  // namespace Hzcc

#endif  // MYCC_STATUS_H
