// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#define INDEX_NONE (-1)
#define INDEX_NONE_ZU ((size_t)-1)

#define DO_NOT_USE_DIRECTLY

#ifndef FORWARD_DECLARE_OBJC_CLASS
#ifdef __OBJC__
#define FORWARD_DECLARE_OBJC_CLASS(classname) @class classname
#else
#define FORWARD_DECLARE_OBJC_CLASS(classname) class classname
#endif
#endif
