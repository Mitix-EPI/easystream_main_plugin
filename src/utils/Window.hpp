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
            namespace stringHelper {
                std::string GetWindowTitle(size_t i);
                std::string GetCurrentWindowTitle();
            }

            namespace enumHelper {
            }

            namespace numberHelper {
                std::pair<int, int> GetCursorPos();
            }

            namespace listHelper {
                std::vector<Window> GetTopLevelWindows();
                std::vector<std::string> GetWindowList();
            }

            namespace dataHelper {
                QStringList GetStates(Window window);
                bool IsMaximized(const std::string &title);
                bool IsFullscreen(const std::string &title);
                bool IsInFocus(const QString &executable);
            }

            namespace searchHelper {
            }

            namespace actionHelper {
            }
        }
    }
}

#endif /* !WINDOW_HPP_ */