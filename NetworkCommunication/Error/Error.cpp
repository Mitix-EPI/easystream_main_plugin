/*
** EPITECH PROJECT, 2021
** easystream_main_plugin
** File description:
** Error
*/

#include "Error.hpp"

es::Error::Error(const std::string &msg, const std::string &whr) noexcept: _msg(msg), _where(whr)
{
}

es::Error::~Error()
{
}

const char *es::Error::what() const noexcept
{
    return (_msg.c_str());
}

const std::string &es::Error::where() const
{
    return (_where);
}