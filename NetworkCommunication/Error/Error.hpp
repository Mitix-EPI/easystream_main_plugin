/*
** EPITECH PROJECT, 2021
** easystream_main_plugin
** File description:
** Error
*/

#ifndef ERROR_HPP_
#define ERROR_HPP_
#include <iostream>
#include <exception>

namespace es {
    class Error : public std::exception {
        public:
            Error(const std::string &msg, const std::string &whr) noexcept;
            ~Error();
    
            const char *what() const noexcept override;
            const std::string &where() const;
    
        private:
            std::string _msg;
            std::string _where;
    };
}

#endif /* !ERROR_HPP_ */
