/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** obs
*/

#ifndef OBS_HPP_
#define OBS_HPP_

#include <string>
#include <obs.hpp>

template <typename T> T* GetCalldataPointer(const calldata_t *data, const char* name) {
	void *ptr = nullptr;
	calldata_get_ptr(data, name, &ptr);
	return reinterpret_cast<T*>(ptr);
}

#endif /* !OBS_HPP_ */