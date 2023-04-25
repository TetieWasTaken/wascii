#include <stdexcept>

#ifndef INVALIDATION_ERROR_H
#define INVALIDATION_ERROR_H

using namespace std;

enum ErrorCode
{
    InvalidArgument
};

class InvalidationError : public runtime_error
{
public:
    InvalidationError(ErrorCode code, const string &message, const string &value)
        : runtime_error(message), m_code(code), m_value(value) {}

    ErrorCode code() const { return m_code; }
    string value() const { return m_value; }

private:
    ErrorCode m_code;
    string m_value;
};

#endif // INVALIDATION_ERROR_H
