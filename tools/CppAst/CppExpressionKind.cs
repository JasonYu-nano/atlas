﻿// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license.
// See license.txt file in the project root for full license information.

namespace CppAst
{
    public enum CppExpressionKind
    {
        Unexposed,
        DeclRef,
        MemberRef,
        Call,
        ObjCMessage,
        Block,
        IntegerLiteral,
        FloatingLiteral,
        ImaginaryLiteral,
        StringLiteral,
        CharacterLiteral,
        Paren,
        UnaryOperator,
        ArraySubscript,
        BinaryOperator,
        CompoundAssignOperator,
        ConditionalOperator,
        CStyleCast,
        CompoundLiteral,
        InitList,
        AddrLabel,
        Stmt,
        GenericSelection,
        GNUNull,
        CXXStaticCast,
        CXXDynamicCast,
        CXXReinterpretCast,
        CXXConstCast,
        CXXFunctionalCast,
        CXXTypeid,
        CXXBoolLiteral,
        CXXNullPtrLiteral,
        CXXThis,
        CXXThrow,
        CXXNew,
        CXXDelete,
        Unary,
        ObjCStringLiteral,
        ObjCEncode,
        ObjCSelector,
        ObjCProtocol,
        ObjCBridgedCast,
        PackExpansion,
        SizeOfPack,
        Lambda,
        ObjCBoolLiteral,
        ObjCSelf,
        OMPArraySection,
        ObjCAvailabilityCheck,
        FixedPointLiteral,
    }
}