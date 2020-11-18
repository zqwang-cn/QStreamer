#pragma once
#include <any>
#include <map>
#include <memory>
#include <string>

class QBuffer
{
public:
    QBuffer();
    QBuffer(QBuffer& buffer) = delete;
    QBuffer(QBuffer&& buffer);
    QBuffer& operator=(QBuffer& buffer) = delete;
    QBuffer& operator=(QBuffer&& buffer);
    std::any get_buffer(std::string name);
    void set_buffer(std::string name, std::any buffer);
    void remove_buffer(std::string name);

private:
    std::map<std::string, std::any> _data;
};