/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** obs
*/

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "Json.hpp"
#include "../plugin-main.hpp"

namespace es {
    namespace utils {
        namespace window {
            std::string GetCurrentWindowTitle();

            std::pair<int, int> GetCursorPos();

            std::vector<std::string> GetWindowList();

            bool IsMaximized(const std::string &title);
            bool IsFullscreen(const std::string &title);
            bool IsInFocus(const QString &executable);
        }
    }
}

#endif /* !WINDOW_HPP_ */