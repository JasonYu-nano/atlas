/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

package com.spacelion.atlas_builder.build_target

import com.spacelion.atlas_builder.cmake.CMakeParser
import com.spacelion.atlas_builder.cmake.CMakeTypes
import java.io.File

class BuildTargetException(message: String? = null) : Exception(message) {

}

class BuildTargetAssembly (dir: File) {
    private var nameToBuildTarget : MutableMap<String, BuildTargetBase> = mutableMapOf()

    init {
        initializeBuildTargetInDirectory(dir)
        buildDependency()
    }

    fun buildTargets() = nameToBuildTarget.toMap()

    private fun initializeBuildTargetInDirectory(dir : File) {
        require(dir.exists() && dir.isDirectory)

        val cmakeLists = dir.toPath().resolve("CMakeLists.txt")
        if (!cmakeLists.toFile().exists()) {
            return
        }

        try {
            val buildTarget = createBuildTarget(cmakeLists.toFile(), dir)
            if (buildTarget != null) {
                nameToBuildTarget[buildTarget.name] = buildTarget
                return
            }
        } catch (e: Exception) {
            throw BuildTargetException("Create build target failed. Reason: $e")
        }

        for (subDir in  dir.listFiles { file -> file.isDirectory }!!) {
            initializeBuildTargetInDirectory(subDir)
        }
    }

    private fun buildDependency() {
        for (buildTarget in nameToBuildTarget.values) {
            if (CMakeTypes.PUBLIC_LINK_LIB in buildTarget.multiValueArgs) {
                val linkNames = buildTarget.multiValueArgs[CMakeTypes.PUBLIC_LINK_LIB]!!
                for (name in linkNames) {
                    if (name in nameToBuildTarget) {
                        buildTarget.addPublicLinkTarget(nameToBuildTarget[name]!!)
                    }
                }
            }

            if (CMakeTypes.PRIVATE_LINK_LIB in buildTarget.multiValueArgs) {
                val linkNames = buildTarget.multiValueArgs[CMakeTypes.PRIVATE_LINK_LIB]!!
                for (name in linkNames) {
                    if (name in nameToBuildTarget) {
                        buildTarget.addPrivateLinkTarget(nameToBuildTarget[name]!!)
                    }
                }
            }
        }
    }

    private fun createBuildTarget(cmakeListsFile : File, rootDirectory: File) : BuildTargetBase? {
        require(cmakeListsFile.exists() && cmakeListsFile.canRead()) { "CMakeLists $cmakeListsFile can't open for read." }

        val cmakeLists = cmakeListsFile.readText()
        val statementAndExecutable = getBuildTargetArgumentsFromCMakeList(cmakeLists)
        if (statementAndExecutable.first.isEmpty()) {
            return null
        }

        val options: MutableSet<String> = mutableSetOf()
        val argToValue: MutableMap<String, String> = mutableMapOf()
        val argToMultiValues: MutableMap<String, List<String>> = mutableMapOf()

        CMakeParser().parseCMakeArgs(statementAndExecutable.first, options, argToValue, argToMultiValues)

        val targetName = argToValue[CMakeTypes.TARGET]
        check(targetName != null) { "Can't find TARGET in build target arguments. file path: $cmakeListsFile" } //TODO

        return if (statementAndExecutable.second) {
            ExecutableBuildTarget(targetName, rootDirectory, options, argToValue, argToMultiValues)
        } else {
            LibraryBuildTarget(targetName, rootDirectory, options, argToValue, argToMultiValues)
        }
    }

    private fun getBuildTargetArgumentsFromCMakeList(content: String): Pair<String, Boolean> {
        var isExecutable = false

        var funStartIndex = content.indexOf(CMakeTypes.ADD_EXECUTABLE_FUNCTION)
        if (funStartIndex != -1) {
            isExecutable = true
        } else {
            funStartIndex = content.indexOf(CMakeTypes.ADD_LIBRARY_FUNCTION)
            if (funStartIndex == -1) {
                funStartIndex = content.indexOf(CMakeTypes.ADD_PROJECT_FUNCTION)
            }
        }

        var statement = ""
        if (funStartIndex != -1) {
            val leftParenthesisIndex = content.indexOf('(', funStartIndex)
            val rightParenthesisIndex = content.indexOf(')', funStartIndex)
            if (leftParenthesisIndex != -1 && rightParenthesisIndex != -1 && rightParenthesisIndex - leftParenthesisIndex > 1) {
                statement = content.substring(leftParenthesisIndex + 1, rightParenthesisIndex)
            }
        }

        return Pair(statement, isExecutable)
    }
}