/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

package com.spacelion.atlas_builder.cmake

class CMakeParser {
    data class ArgumentFragment(val keywords: String, val startIndex: Int, val hasMultiValue: Boolean)

    fun parseCMakeArgs(statement: String, options: MutableSet<String>, argToValue: MutableMap<String, String>,
                       argToMultiValues: MutableMap<String, List<String>>) {
        for (opt in CMakeTypes.OPTIONS) {
            if (statement.indexOf(opt) != -1) {
                options.add(opt)
            }
        }

        val argumentFragments: MutableList<ArgumentFragment> = mutableListOf()

        for (arg in CMakeTypes.ONE_VALUE_ARGS) {
            val idx = statement.indexOf(arg)
            if (idx != -1) {
                argumentFragments.add(ArgumentFragment(arg, idx, false))
            }
        }

        for (arg in CMakeTypes.MULTI_VALUE_ARGS) {
            val idx = statement.indexOf(arg)
            if (idx != -1) {
                argumentFragments.add(ArgumentFragment(arg, idx, true))
            }
        }

        argumentFragments.sortBy { it.startIndex }

        var lastStartIndex = statement.length
        var fragmentIndex = argumentFragments.size - 1
        while (fragmentIndex >= 0) {
            val fragment = argumentFragments[fragmentIndex]
            --fragmentIndex

            val subStringStart = fragment.startIndex + fragment.keywords.length
            val argLine = normalizeCMakeFunctionArgument(
                statement.substring(subStringStart, lastStartIndex))
            lastStartIndex = fragment.startIndex

            val args = argLine.split(' ')

            if (!fragment.hasMultiValue && args.size == 1) {
                argToValue[fragment.keywords] = args.first()
            } else if (fragment.hasMultiValue && args.isNotEmpty()) {
                argToMultiValues[fragment.keywords] = args
            }
        }
    }

    private fun normalizeCMakeFunctionArgument(argument: String) = argument.trim().removeSuffix("\n")
}