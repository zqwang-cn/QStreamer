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
    std::any& operator[](const std::string& name);
    void erase(const std::string& name);

private:
    std::map<std::string, std::any> _data;
};