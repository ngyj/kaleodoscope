#ifndef PRELUDE_H
#define PRELUDE_H

#pragma once
#include <stdint.h>
#include <memory>

// using isize = intmax_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using usize = size_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;

using std::optional;

template<typename T>
using Ref = std::shared_ptr<T>;


template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// required in sepples17
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif
