#ifndef SCRIPTLIB_BIND_HEADER
#define SCRIPTLIB_BIND_HEADER

#include <xtl/any.h>
#include <xtl/common_exceptions.h>
#include <xtl/functional_traits.h>
#include <xtl/stat_counter.h>
#include <xtl/trackable_ptr.h>
#include <xtl/type.h>
#include <xtl/type_list.h>

#include <script/invoker.h>
#include <script/interpreter.h>

namespace script
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шлюза для произвольного функтора с известной сигнатурой
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature, class Fn> Invoker make_invoker (Fn fn);
template <class Fn>                  Invoker make_invoker (Fn fn);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание функции, возвращающей константу
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> Invoker make_const (const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шлюза для функции обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature> Invoker make_callback_invoker ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шлюза для функции обратного вызова функций шлюза
///////////////////////////////////////////////////////////////////////////////////////////////////
Invoker make_shell_callback_invoker ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов шлюза
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret>
Ret invoke (IInterpreter&, const char* function_name);

template <class Ret, class T1>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1);

template <class Ret, class T1, class T2>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2);

template <class Ret, class T1, class T2, class T3>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2, const T3& arg3);

template <class Ret, class T1, class T2, class T3, class T4>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4);

template <class Ret, class T1, class T2, class T3, class T4, class T5>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
            const T5& arg5);

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
            const T5& arg5, const T6& arg6);

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
            const T5& arg5, const T6& arg6, const T7& arg7);

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
            const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8);

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
            const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8, const T9& arg9);

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
Ret invoke (IInterpreter&, const char* function_name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
            const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8, const T9& arg9, const T10& arg10);

#include <script/detail/bind.inl>

}

#endif
