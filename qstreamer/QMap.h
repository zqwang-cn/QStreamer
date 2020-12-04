#pragma once
#include <map>
#include <functional>

template <typename T>
class QMap
{
public:
    const T& operator[](const std::string& name) const
    {
        return _data.at(name);
    }
    void set(const std::string& name, T value)
    {
        _data[name] = value;
    }
    size_t size() const noexcept
    {
        return _data.size();
    }
    void for_each(std::function<void(T)> func)
    {
        for (auto item : _data)
            func(item.second);
    }

private:
    std::map<std::string, T> _data;
};